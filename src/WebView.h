#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>

#include <qplayer.h>

struct KioskSettings;

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebView(const KioskSettings *settings, QWidget *parent = nullptr);

    void loadCustomPage(const QUrl &url);
    void loadHomepage();

    void setPage(QWebEnginePage* page);

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

    void playSound(const QUrl &sound);
    void updateZoom();

public slots:
    void handleUrlChanged(const QUrl &);

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void handleWindowCloseRequested();

private:
    const KioskSettings *settings_;
    QPlayer *player_;
    QWebEngineView *loader_;
};

#endif // WEBVIEW_H
