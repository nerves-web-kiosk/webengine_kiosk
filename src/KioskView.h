#ifndef KIOSK_VIEW_H
#define KIOSK_VIEW_H

#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>

struct KioskSettings;

class KioskView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit KioskView(const KioskSettings *settings, QWidget *parent = nullptr);

    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type);

private:
    const KioskSettings *settings_;
};

#endif // KIOSK_VIEW_H
