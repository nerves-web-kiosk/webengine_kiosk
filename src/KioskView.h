#ifndef KIOSK_VIEW_H
#define KIOSK_VIEW_H

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>
#include "qplayer.h"

struct KioskSettings;

class KioskView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit KioskView(const KioskSettings *settings, QWidget *parent = nullptr);

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

    void playSound(const QUrl &sound);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void handleWindowCloseRequested();

private:
    const KioskSettings *settings_;
    QPlayer *player_;
    QWebEngineView *loader_;
};

#endif // KIOSK_VIEW_H
