#include "KioskSettings.h"
#include <QCommandLineParser>
#include <QVariant>
#include <pwd.h>
#include <grp.h>
#include <err.h>

static bool toBool(const QString &v)
{
    return !v.isNull() && v == QLatin1String("true");
}

static uid_t stringToUid(const QString &s)
{
    if (s.isEmpty())
        return 0;

    bool ok;
    uid_t uid = (uid_t) s.toUInt(&ok);
    if (!ok) {
        struct passwd *passwd = getpwnam(qPrintable(s));
        if (!passwd)
            errx(EXIT_FAILURE, "Unknown user '%s'", qPrintable(s));
        uid = passwd->pw_uid;
    }
    if (uid == 0)
        errx(EXIT_FAILURE, "Setting the user to root or uid 0 is not allowed");
    return uid;
}

static gid_t stringToGid(const QString &s)
{
    if (s.isEmpty())
        return 0;

    bool ok;
    gid_t gid = (gid_t) s.toUInt(&ok);
    if (!ok) {
        struct group *group = getgrnam(qPrintable(s));
        if (!group)
            errx(EXIT_FAILURE, "Unknown group '%s'", qPrintable(s));
        gid = group->gr_gid;
    }
    if (gid == 0)
        errx(EXIT_FAILURE, "Setting the group to root or gid 0 is not allowed");
    return gid;
}

KioskSettings::KioskSettings(const QCoreApplication &app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser for Elixir");
    parser.addHelpOption();
    parser.addVersionOption();

    QList<QCommandLineOption> options = QList<QCommandLineOption>({
            {"clear_cache", "Clear cached request data."},
            {"homepage", "Set starting url", "url", "qrc:///ui/default.html"},
            {"monitor", "Display window on the <n>th monitor.", "n", "0"},
            {"fullscreen", "Run kiosk fullscreen", "true"},
            {"width", "When not in fullscreen mode, this is the window width", "pixels", "1024"},
            {"height", "When not in fullscreen mode, this is the window height", "pixels", "768"},
            {"opengl", "Specify OpenGL preference.", "auto|software|gles|gl", "auto"},
            {"proxy_enable", "Enable a proxy.", "bool", "false" },
            {"proxy_system", "Use the system proxy.", "bool", "false" },
            {"proxy_host", "Specify the proxy hostname.", "hostname" },
            {"proxy_port", "Specify a proxy port number.", "port", "3128"},
            {"proxy_username", "The username for the proxy.", "username"},
            {"proxy_password", "The password for the proxy.", "password"},
            {"stay_on_top", "Use to make the window stay on top", "bool", "true"},
            {"progress", "Show the load progress.", "bool", "true"},
            {"sounds", "Use to enable UI sounds.", "bool", "true"},
            {"window_clicked_sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/window-clicked.ogg"},
            {"link_clicked_sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/link-clicked.ogg"},
            {"hide_cursor", "Specify the hide the mouse cursor.", "bool", "false"},
            {"javascript", "Enable Javascript.", "bool", "true"},
            {"javascript_can_open_windows", "Allow Javascript to open windows.", "bool", "false"},
            {"debug_menu", "Enable a debug menu", "bool", "false"},
            {"uid", "Drop priviledge and run as this uid.", "uid/user"},
            {"gid", "Drop priviledge and run as this gid.", "gid/group"},
            {"zoom_factor", "The zoom factor for the page (0.25 to 5.0).", "factor", "1.0"}
        });
    parser.addOptions(options);
    parser.process(app);

    clearCache = toBool(parser.value("clear_cache"));
    homepage = QUrl(parser.value("homepage"));
    monitor = parser.value("monitor").toInt();
    fullscreen = toBool(parser.value("fullscreen"));
    width = parser.value("width").toInt();
    height = parser.value("height").toInt();
    opengl = parser.value("opengl");
    proxyEnabled = toBool(parser.value("proxy_enable"));
    proxySystem = toBool(parser.value("proxy_system"));
    proxyHostname = parser.value("proxy_host");
    proxyPort = (quint16) parser.value("proxy_host").toUInt();
    proxyUsername = parser.value("proxy_username");
    proxyPassword = parser.value("proxy_password");
    stayOnTop = toBool(parser.value("stay_on_top"));
    progress = toBool(parser.value("progress"));
    soundsEnabled = toBool(parser.value("sounds"));
    windowClickedSound = QUrl(parser.value("window_clicked_sound"));
    linkClickedSound = QUrl(parser.value("link_clicked_sound"));
    hideCursor = toBool(parser.value("hide_cursor"));
    javascriptEnabled = toBool(parser.value("javascript"));
    javascriptCanOpenWindows = toBool(parser.value("javascript_can_open_windows"));
    debugMenuEnabled = toBool(parser.value("debug_menu"));
    uid = stringToUid(parser.value("uid"));
    gid = stringToGid(parser.value("gid"));
    zoomFactor = parser.value("zoom_factor").toDouble();
    if (zoomFactor < 0.25)
        zoomFactor = 0.25;
    else if (zoomFactor > 5.0)
        zoomFactor = 5.0;
}
