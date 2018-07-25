#include <functional>
#include <QProgressBar>
#include <QMainWindow>
#include <QtNetwork>
#include <QtWebEngineCore>

#include "WebView.h"
#include "KioskSettings.h"

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define qsl(...) QStringList({__VA_ARGS__})

struct keyFunction {
    Qt::Key key;
    std::function<void()> f;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const KioskSettings *settings);

    void init();

    void clearCache();
    void clearCacheOnExit();

    void showFullScreen();
    qreal getPixelRatio();

protected:
    //void moveEvent(QMoveEvent *);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void doRunJavascriptDialog();
    void doGotoURLDialog();

    void cleanupSlot();

    void setProgress(int p);
    void startLoading();
    void urlChanged(const QUrl &);
    void finishLoading(bool);

    void desktopResized(int p);

private:
    void updatePixelRatio();
    int computedScreen();

private:
    const KioskSettings *settings_;

    WebView* view_;                      // Webkit Page View
    QProgressBar *loadProgress_;         // progress bar to display page loading

    QMap<QKeySequence, std::function<void()>> shortcutKeys_;
    QKeyEvent * eventExit_;

    qreal pixelRatio_;
};

#endif
