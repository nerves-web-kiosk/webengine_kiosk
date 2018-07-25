#ifndef KIOSKSETTINGS_H
#define KIOSKSETTINGS_H

#include <QUrl>

class QCoreApplication;

struct KioskSettings
{
    explicit KioskSettings(const QCoreApplication &app);

    bool clearCache;
    QUrl homepage;
    int monitor;
    QString opengl;
    bool proxyEnabled;
    bool proxySystem;
    QString proxyHostname;
    quint16 proxyPort;
    QString proxyUsername;
    QString proxyPassword;
    bool stayOnTop;
    bool progress;
    bool scaleWithDPI;
    double pageScale;

    bool eventSoundsEnabled;
    QUrl windowClickedSound;
    QUrl linkClickedSound;

    bool hideCursor;
    bool javascriptEnabled;
    bool javascriptCanOpenWindows;
    bool debugMenuEnabled;

    uid_t uid;
    gid_t gid;
#if 0
{"view/scale_with_dpi", true},
{"view/page_scale", 1.0},
{"browser/javascript", true},
{"browser/javascript_can_open_windows", false},
{"browser/javascript_can_close_windows", false},
{"browser/webgl", false},
{"browser/ignore_ssl_errors", true},     // Don't break on SSL errors
{"browser/show_homepage_on_window_close", true},        // Show default homepage if window closed by javascript
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
#endif
};

#endif // KIOSKSETTINGS_H
