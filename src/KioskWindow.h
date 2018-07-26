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

    void showProgress(int p);
    void hideProgress();

private slots:
    void doRunJavascriptDialog();
    void doGotoURLDialog();

private:
    int computedScreen();

private:
    Kiosk *kiosk_;
    const KioskSettings *settings_;
    QProgressBar *loadProgress_;
};

#endif // KIOSK_WINDOW_H
