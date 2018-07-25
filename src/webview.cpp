#include <QtDebug>
#include <QtGui>
#include <QtWebEngineCore>
#include "webview.h"

#include "mainwindow.h"
#include "KioskSettings.h"

WebView::WebView(const KioskSettings *settings, QWidget* parent): QWebEngineView(parent),
    settings_(settings)
{
    player_ = settings_->eventSoundsEnabled ? new QPlayer(this) : nullptr;
    loader_ = nullptr;
}

void WebView::setPage(QWebEnginePage *page)
{
    QWebEngineView::setPage(page);
    connect(page,
            SIGNAL(windowCloseRequested()),
            SLOT(handleWindowCloseRequested()));
}

void WebView::updateZoom() {
    qreal settingsZoom = 1;
    qreal pixelRatio = 1;
    if (settings_->scaleWithDPI)
        pixelRatio = static_cast<MainWindow*>(parent())->getPixelRatio();

    if (settings_->pageScale > 0) {
        settingsZoom = settings_->pageScale;
    }
    page()->setZoomFactor(pixelRatio * settingsZoom);
}

void WebView::loadHomepage()
{
    loadCustomPage(settings_->homepage);
}

void WebView::loadCustomPage(const QUrl &url)
{
    qDebug("Going to %s", qPrintable(url.toString()));
    this->stop();
    this->load(url);
}

void WebView::handleWindowCloseRequested()
{
    qDebug() << "Handle windowCloseRequested:";
#if 0
    if (mainSettings->value("browser/show_homepage_on_window_close").toBool()) {
        qDebug() << "-- load homepage";
        loadHomepage();
    } else {
        qDebug() << "-- exit application";
        QCoreApplication::exit(0);
    }
#endif
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Window Clicked!";
        playSound(settings_->windowClickedSound);
    }
    QWebEngineView::mousePressEvent(event);
}

void WebView::handleUrlChanged(const QUrl &url)
{
    // WTF???
    if (this->url().toString() != url.toString()) {
        qDebug() << "url Changed!" << url.toString();

        this->stop();
        this->load(url);
        qDebug() << "-- load url";
    }

    if (loader_) {
        loader_->close();
        qDebug() << "-- close loader";
        loader_ = nullptr;
    }
}

void WebView::playSound(const QUrl &sound)
{
    if (player_)
        player_->play(sound);
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    // This is called when the browser wants to open a new window
    if (type != QWebEnginePage::WebBrowserWindow) {
        return nullptr;
    }

    if (loader_ == nullptr) {
        qDebug() << "New fake webview loader";
        loader_ = new QWebEngineView(this);
        QWebEnginePage *newWeb = new QWebEnginePage(this);
        loader_->setAttribute(Qt::WA_DeleteOnClose, true);
        loader_->setPage(newWeb);

        connect(loader_, SIGNAL(urlChanged(const QUrl&)), SLOT(handleUrlChanged(const QUrl&)));
    }

    return loader_;
}
