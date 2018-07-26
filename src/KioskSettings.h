#ifndef KIOSKSETTINGS_H
#define KIOSKSETTINGS_H

#include <QUrl>

class QCoreApplication;

struct KioskSettings
{
    explicit KioskSettings(const QCoreApplication &app);

    bool clearCache;
    QUrl homepage;
    bool fullscreen;
    int width;
    int height;
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

    qreal zoomFactor;
};

#endif // KIOSKSETTINGS_H