#include "Kiosk.h"
#include "KioskWindow.h"
#include "KioskView.h"
#include "KioskProgress.h"
#include "ElixirComs.h"

#include <QNetworkProxy>
#include <QWebEngineSettings>
#include <QApplication>
#include <QLabel>
#include <QMetaObject>

Kiosk::Kiosk(const KioskSettings *settings, QObject *parent) :
    QObject(parent),
    settings_(settings),
    coms_(nullptr),
    view_(nullptr),
    loadingPage_(false),
    showPageWhenDone_(true)
{
    // Set up the UI
    window_ = new KioskWindow(this, settings);
}

void Kiosk::init()
{
    if (settings_->proxyEnabled) {
        if (settings_->proxySystem) {
            QNetworkProxyFactory::setUseSystemConfiguration(true);
        } else {
            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(settings_->proxyHostname);
            proxy.setPort(settings_->proxyPort);
            if (!settings_->proxyUsername.isEmpty()) {
                proxy.setUser(settings_->proxyUsername);
                proxy.setPassword(settings_->proxyPassword);
            }
            QNetworkProxy::setApplicationProxy(proxy);
        }
    }

    if (settings_->hideCursor)
        QApplication::setOverrideCursor(Qt::BlankCursor);

    moveToMonitor();
    if (settings_->fullscreen) {
        window_->showFullScreen();
    } else {
        window_->show();
        window_->resize(settings_->width, settings_->height);
    }

    // Do the heavy lifting of starting up the webbrowser on the next
    // pass through the event loop.
    QTimer::singleShot(100, this, SLOT(completeInit()));
}

void Kiosk::completeInit()
{
    // Set up communication with Elixir
    coms_ = new ElixirComs(this);
    connect(coms_, SIGNAL(messageReceived(KioskMessage)), SLOT(handleRequest(KioskMessage)));

    // Start the browser up
    view_ = new KioskView(settings_);
    view_->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, settings_->javascriptEnabled);
    view_->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, settings_->javascriptCanOpenWindows);

    connect(view_, SIGNAL(loadStarted()), SLOT(startLoading()));
    connect(view_, SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
    connect(view_, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view_, SIGNAL(loadFinished(bool)), SLOT(finishLoading()));
    window_->setView(view_);
    view_->load(settings_->homepage);
}

void Kiosk::goToUrl(const QUrl &url)
{
    view_->load(url);
}

void Kiosk::runJavascript(const QString &program)
{
    view_->page()->runJavaScript(program);
}

void Kiosk::handleRequest(const KioskMessage &message)
{
    switch (message.type()) {
    case KioskMessage::GoToURL:
        goToUrl(QUrl(QString::fromUtf8(message.payload())));
        break;

    case KioskMessage::RunJavascript:
        runJavascript(QString::fromUtf8(message.payload()));
        break;

    case KioskMessage::Blank:
        window_->setBrowserVisible(message.payload().at(0) == 0);
        break;

    default:
        qFatal("Unknown message from Elixir: %d", message.type());
    }
}

void Kiosk::startLoading()
{
    if (settings_->progress)
        window_->showProgress(0);

    coms_->send(KioskMessage::loadingPageMessage());
    loadingPage_ = true;
}

void Kiosk::setProgress(int p)
{
    if (settings_->progress)
        window_->showProgress(p);

    coms_->send(KioskMessage::progressMessage(p));

    if (loadingPage_ && p >= 100)
        finishLoading();
}

void Kiosk::finishLoading()
{
    if (settings_->progress)
        window_->hideProgress();

    if (loadingPage_) {
        coms_->send(KioskMessage::finishedLoadingPageMessage());
        loadingPage_ = false;

        if (showPageWhenDone_) {
            // Let the event loop settle before showing the browser
            QTimer::singleShot(100, window_, SLOT(showBrowser()));
        }
    }

    // 3. Focus window and click into it to stimulate event loop after signal handling
    QApplication::setActiveWindow(window_);
    window_->focusWidget();
}

void Kiosk::urlChanged(const QUrl &url)
{
    coms_->send(KioskMessage::urlChanged(url));

    // This is real link clicked
    view_->playSound(settings_->linkClickedSound);
}

void Kiosk::moveToMonitor()
{
    QList<QScreen*> screens = QApplication::screens();
    if (settings_->monitor >= 0 && settings_->monitor < screens.length()) {
        QRect screenRect = screens.at(settings_->monitor)->geometry();
        if (!screenRect.contains(window_->geometry()))
            window_->move(screenRect.x(), screenRect.y());
    }
}
