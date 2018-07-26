#ifndef KIOSK_WINDOW_H
#define KIOSK_WINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QtWebEngineCore>

#include "KioskView.h"
#include "KioskSettings.h"

class Kiosk;

class KioskWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit KioskWindow(Kiosk *kiosk, const KioskSettings *settings);
    ~KioskWindow();

    void init();

    void showProgress(int p);
    void hideProgress();

    void showFullScreen();
    qreal getPixelRatio();

private slots:
    void doRunJavascriptDialog();
    void doGotoURLDialog();

    void desktopResized(int p);

private:
    int computedScreen();

private:
    Kiosk *kiosk_;
    const KioskSettings *settings_;
    QProgressBar *loadProgress_;
};

#endif // KIOSK_WINDOW_H
