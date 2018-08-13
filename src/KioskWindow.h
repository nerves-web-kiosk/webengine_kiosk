#ifndef KIOSK_WINDOW_H
#define KIOSK_WINDOW_H

#include <QMainWindow>
#include <QtWebEngineCore>

#include "KioskSettings.h"

class Kiosk;
class KioskProgress;
class KioskView;
class QLabel;

class KioskWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit KioskWindow(Kiosk *kiosk, const KioskSettings *settings);
    ~KioskWindow();

    void setView(KioskView *view);
    void setBrowserVisible(bool enabled);

    void showProgress(int percent);
    void hideProgress();

public slots:
    void showBrowser();
    void hideBrowser();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void doRunJavascriptDialog();
    void doGotoURLDialog();

private:
    int computedScreen();

private:
    Kiosk *kiosk_;
    const KioskSettings *settings_;

    KioskProgress *progress_;
    QLabel *blank_;
    KioskView *view_;

    bool showingBrowser_;
};

#endif // KIOSK_WINDOW_H
