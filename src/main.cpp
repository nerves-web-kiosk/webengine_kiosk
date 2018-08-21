#include <QApplication>
#include <QStandardPaths>
#include <QDir>

#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>

#include "Kiosk.h"
#include "KioskSettings.h"

static uid_t stringToUid(const char *s)
{
    if (*s == '\0')
        return 0;

    char *endptr;
    uid_t uid = static_cast<uid_t>(strtoul(s, &endptr, 0));
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
    gid_t gid = static_cast<gid_t>(strtoul(s, &endptr, 0));
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

static void checkPermissions()
{
    // Check permissions on directories since the error messages from
    // Chromium and QtWebEngine are pretty hard to debug unless you
    // run strace. Maybe this will help someone.
    struct stat st;
    if (stat("/dev/shm", &st) < 0 || (st.st_mode & 01777) != 01777)
        errx(EXIT_FAILURE, "Check that \"/dev/shm\" exists and has mode 1777 (got %o)", st.st_mode & 01777);

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir().mkpath(path))
        errx(EXIT_FAILURE, "Check permissions on directories leading up to '%s' or specify --data_dir", qPrintable(path));
}

int main(int argc, char *argv[])
{
    // Some settings need to be handled before Qt does anything.
    // Scan for them here. If there are issues, then KioskSettings
    // will report them.
    gid_t desired_gid = 0;
    uid_t desired_uid = 0;
    const char *desired_user = nullptr;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--gid") == 0) {
            desired_gid = stringToGid(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--uid") == 0) {
            desired_user = argv[i + 1];
            desired_uid = stringToUid(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--opengl") == 0) {
            setOpenGLMode(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--data_dir") == 0) {
            // Qt derives the data directories for QtWebEngine
            // based on $HOME, so change it if the user specifies
            // --data_dir
            setenv("HOME", argv[i + 1], 1);
            i++;
        }
    }

    gid_t current_gid = getgid();
    uid_t current_uid = getuid();
    if (current_gid == 0 || current_uid == 0) {
        // Running with elevated privileges. This isn't a good idea, so
        // see if the user specified a gid and uid or try to specify
        // one for them.
        if (desired_gid == 0) {
            warnx("Running a web browser with gid == 0 is not allowed. Looking for a kiosk group.");
            desired_gid = stringToGid("kiosk");
        }
        if (desired_uid == 0) {
            warnx("Running a web browser with uid == 0 is not allowed. Looking for a kiosk user.");
            desired_user = "kiosk";
            desired_uid = stringToUid(desired_user);
        }

        if (desired_gid == 0 || desired_uid == 0)
            errx(EXIT_FAILURE, "Refusing to run with uid == %d and gid == %d", desired_uid, desired_gid);
    }

    // Drop/change privilege if requested
    // See https://wiki.sei.cmu.edu/confluence/display/c/POS36-C.+Observe+correct+revocation+order+while+relinquishing+privileges
    if (desired_gid > 0) {
        if (desired_user && initgroups(desired_user, desired_gid) < 0)
            err(EXIT_FAILURE, "initgroups(%s, %d) failed", desired_user, desired_gid);

        if (setgid(desired_gid) < 0)
            err(EXIT_FAILURE, "setgid(%d) failed", desired_gid);
    }

    if (desired_uid > 0 && setuid(desired_uid) < 0)
        err(EXIT_FAILURE, "setuid(%d) failed", desired_uid);

    QApplication app(argc, argv);
    KioskSettings settings(app);

    // Copy in the uid/gid settings for posterity.
    settings.uid = desired_uid;
    settings.gid = desired_gid;

    if (desired_gid || desired_uid)
        checkPermissions();

    Kiosk kiosk(&settings);
    kiosk.init();

    return app.exec();
}
