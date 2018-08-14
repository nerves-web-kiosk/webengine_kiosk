#include <QApplication>

#include <err.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "Kiosk.h"
#include "KioskSettings.h"

static uid_t stringToUid(const char *s)
{
    if (*s == '\0')
        return 0;

    char *endptr;
    uid_t uid = (uid_t) strtoul(s, &endptr, 0);
    if (*endptr != '\0') {
        struct passwd *passwd = getpwnam(s);
        if (!passwd)
            errx(EXIT_FAILURE, "Unknown user '%s'", s);
        uid = passwd->pw_uid;
    }
    if (uid == 0)
        errx(EXIT_FAILURE, "Setting the user to root or uid 0 is not allowed");
    return uid;
}

static gid_t stringToGid(const char *s)
{
    if (*s == '\0')
        return 0;

    char *endptr;
    gid_t gid = (gid_t) strtoul(s, &endptr, 0);
    if (*endptr != '\0') {
        struct group *group = getgrnam(s);
        if (!group)
            errx(EXIT_FAILURE, "Unknown group '%s'", s);
        gid = group->gr_gid;
    }
    if (gid == 0)
        errx(EXIT_FAILURE, "Setting the group to root or gid 0 is not allowed");
    return gid;
}

static void setOpenGLMode(const char *mode)
{
    if (strcmp(mode, "gl") == 0) {
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        warnx("OpenGL: Qt::AA_UseDesktopOpenGL");
    } else if (strcmp(mode, "gles") == 0) {
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
        warnx("OpenGL: Qt::AA_UseOpenGLES");
    } else if (strcmp(mode, "software") == 0) {
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
        warnx("OpenGL: Qt::AA_UseSoftwareOpenGL");
    } else {
        warnx("OpenGL: Default");
    }
}

int main(int argc, char *argv[])
{
    // Some settings need to be handled before Qt does anything.
    // Scan for them here. If there are issues, then KioskSettings
    // will report them.
    gid_t gid = 0;
    uid_t uid = 0;
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--gid") == 0) {
            gid = stringToGid(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--uid") == 0) {
            uid = stringToUid(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--opengl") == 0) {
            setOpenGLMode(argv[i + 1]);
            i++;
        }
    }

    // Drop/change privilege if requested
    // See https://wiki.sei.cmu.edu/confluence/display/c/POS36-C.+Observe+correct+revocation+order+while+relinquishing+privileges
    if (gid > 0 && setgid(gid) < 0)
        err(EXIT_FAILURE, "setgid(%d)", gid);

    if (uid > 0 && setuid(uid) < 0)
        err(EXIT_FAILURE, "setuid(%d)", uid);

    QApplication app(argc, argv);
    KioskSettings settings(app);

    // Copy in the uid/gid settings for posterity.
    settings.uid = uid;
    settings.gid = gid;

    Kiosk kiosk(&settings);
    kiosk.init();

    return app.exec();
}
