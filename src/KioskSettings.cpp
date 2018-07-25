#include "KioskSettings.h"
#include <QCommandLineParser>
#include <QVariant>

static bool toBool(const QString &v)
{
    return !v.isNull() && v == QLatin1String("true");
}


#if 0

defaultSettings = {
    {"application/opengl_mode", "auto"},
    {"proxy/enable", false },
    {"proxy/system", true },
    {"proxy/host", "proxy.example.com" },
    {"proxy/port", 3128},
    {"proxy/auth", false },
    {"proxy/username", "username"},
    {"proxy/password", "password"},
    {"view/fullscreen", true},
    {"view/maximized", false},
    {"view/minimal-width", 320},
    {"view/minimal-height", 200},
    {"view/startup_resize_delayed", true},
    {"view/startup_resize_delay", 200},
    {"view/hide_scrollbars", true},
    {"view/stay_on_top", false},
    {"view/disable_selection", true},
    {"view/show_load_progress", true},
    {"view/scale_with_dpi", true},
    {"view/page_scale", 1.0},
    {"browser/homepage", "qrc:///ui/default.html"},
    {"browser/javascript", true},
    {"browser/javascript_can_open_windows", false},
    {"browser/javascript_can_close_windows", false},
    {"browser/ignore_ssl_errors", true},     // Don't break on SSL errors
    {"browser/show_homepage_on_window_close", true},        // Show default homepage if window closed by javascript
    {"browser/startup_load_delayed", true},
    {"browser/startup_load_delay", 100},
    {"browser/disable_hotkeys", false},
    {"event-sounds/enable", false},
    {"event-sounds/window-clicked", ":ui/window-clicked.ogg"},
    {"event-sounds/link-clicked", ":ui/window-clicked.ogg"},
    {"cache/enable", false},
    {"cache/location", QStandardPaths::writableLocation(QStandardPaths::CacheLocation)},
    {"cache/size", 100*1000*1000},
    {"cache/clear-on-start", false},
    {"cache/clear-on-exit", false},
    {"attach/javascripts", ""},
    {"attach/styles", ""},
    {"view/hide_mouse_cursor", false}
};

#endif

KioskSettings::KioskSettings(const QCoreApplication &app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser for Elixir");
    parser.addHelpOption();
    parser.addVersionOption();

    QList<QCommandLineOption> options = QList<QCommandLineOption>({
            {"clear-cache", "Clear cached request data."},
            {"homepage", "Set starting url", "url", "qrc:///ui/default.html"},
            {"monitor", "Display window on the <n>th monitor.", "n", "0"},
            {"opengl", "Specify OpenGL preference.", "auto|software|gles|gl", "auto"},
            {"proxy-enable", "Enable a proxy.", "bool", "false" },
            {"proxy-system", "Use the system proxy.", "bool", "false" },
            {"proxy-host", "Specify the proxy hostname.", "hostname" },
            {"proxy-port", "Specify a proxy port number.", "port", "3128"},
            {"proxy-username", "The username for the proxy.", "username"},
            {"proxy-password", "The password for the proxy.", "password"},
            {"stay-on-top", "Use to make the window stay on top", "bool", "true"},
            {"progress", "Show the load progress.", "bool", "true"},
            {"event-sounds", "Use to enable click sounds.", "bool", "true"},
            {"window-clicked-sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/window-clicked.ogg"},
            {"link-clicked-sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/link-clicked.ogg"},
            {"hide-cursor", "Specify the hide the mouse cursor", "bool", "false"},
            {"javascript", "Enable Javascript", "bool", "true"},
            {"javascript-can-open-windows", "Allow Javascript to open windows", "bool", "false"},
            {"debug-menu", "Enable a debug menu", "bool", "false"}
        });
    parser.addOptions(options);
    parser.process(app);

    clearCache = toBool(parser.value("clear-cache"));
    homepage = QUrl(parser.value("homepage"));
    monitor = parser.value("monitor").toInt();
    opengl = parser.value("opengl");
    proxyEnabled = toBool(parser.value("proxy-enable"));
    proxySystem = toBool(parser.value("proxy-system"));
    proxyHostname = parser.value("proxy-host");
    proxyPort = (quint16) parser.value("proxy-host").toUInt();
    proxyUsername = parser.value("proxy-username");
    proxyPassword = parser.value("proxy-password");
    stayOnTop = toBool(parser.value("stay-on-top"));
    progress = toBool(parser.value("progress"));
    eventSoundsEnabled = toBool(parser.value("event-sounds"));
    windowClickedSound = QUrl(parser.value("window-clicked-sound"));
    linkClickedSound = QUrl(parser.value("link-clicked-sound"));
    hideCursor = toBool(parser.value("hide-cursor"));
    javascriptEnabled = toBool(parser.value("javascript"));
    javascriptCanOpenWindows = toBool(parser.value("javascript-can-open-windows"));
    debugMenuEnabled = toBool(parser.value("debug-menu"));
}
