#include "KioskSettings.h"
#include <QCommandLineParser>
#include <QVariant>
#include <QFileInfo>

static bool toBool(const QString &v)
{
    return !v.isNull() && v == QLatin1String("true");
}

KioskSettings::KioskSettings(const QCoreApplication &app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("Kiosk browser for Elixir");
    parser.addHelpOption();
    parser.addVersionOption();

    QList<QCommandLineOption> options = QList<QCommandLineOption>({
            {"data_dir", "Data directory (defaults to subdirectories of $HOME)", "path", ""},
            {"clear_cache", "Clear cached request data.", "bool", "true"},
            {"homepage", "Set starting url", "url", ""},
            {"monitor", "Display window on the <n>th monitor.", "n", "0"},
            {"fullscreen", "Run kiosk fullscreen", "bool", "true"},
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
            {"window_clicked_sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/window-clicked.wav"},
            {"link_clicked_sound", "Path to a sound to play when then window is clicked.", "url", "qrc:///ui/link-clicked.wav"},
            {"hide_cursor", "Specify the hide the mouse cursor.", "bool", "false"},
            {"javascript", "Enable Javascript.", "bool", "true"},
            {"javascript_can_open_windows", "Allow Javascript to open windows.", "bool", "false"},
            {"debug_keys", "Enable a debug key shortcuts", "bool", "false"},
            {"uid", "Drop priviledge and run as this uid.", "uid/user", ""},
            {"gid", "Drop priviledge and run as this gid.", "gid/group", ""},
            {"zoom_factor", "The zoom factor for the page (0.25 to 5.0).", "factor", "1.0"},
            {"blank_image", "An image to use when the screen should be blank", "path", ""},
            {"background_color", "The background color of the browser and blank screen (unless there's a blank_image)", "#RRGGBB or name", "white"}
        });
    parser.addOptions(options);
    parser.process(app);

    dataDir = parser.value("data_dir");
    clearCache = toBool(parser.value("clear_cache"));
    QString homePageAsString = parser.value("homepage");
    if (!homePageAsString.isEmpty()) {
        homepage = QUrl(homePageAsString);
    } else {
        // The relative path here works both from being called by
        // Elixir and in QtCreator.
        QFileInfo fi("../priv/www/index.html");
        QString homepageAsUri = QString("file://%1").arg(fi.canonicalFilePath());
        homepage = QUrl(homepageAsUri);
    }
    monitor = parser.value("monitor").toInt();
    fullscreen = toBool(parser.value("fullscreen"));
    width = parser.value("width").toInt();
    height = parser.value("height").toInt();
    opengl = parser.value("opengl");
    proxyEnabled = toBool(parser.value("proxy_enable"));
    proxySystem = toBool(parser.value("proxy_system"));
    proxyHostname = parser.value("proxy_host");
    proxyPort = static_cast<quint16>(parser.value("proxy_host").toUInt());
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
    debugKeysEnabled = toBool(parser.value("debug_keys"));
    uid = 0; // Set in main.c
    gid = 0; // Set in main.c
    zoomFactor = parser.value("zoom_factor").toDouble();
    if (zoomFactor < 0.25)
        zoomFactor = 0.25;
    else if (zoomFactor > 5.0)
        zoomFactor = 5.0;
    blankImage = parser.value("blank_image");
    backgroundColor = QColor(parser.value("background_color"));
}
