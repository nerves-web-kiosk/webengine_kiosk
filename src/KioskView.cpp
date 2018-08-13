#include <QtDebug>
#include <QtGui>
#include <QtWebEngineCore>
#include "KioskView.h"

#include "KioskWindow.h"
#include "KioskSettings.h"

KioskView::KioskView(const KioskSettings *settings, QWidget* parent): QWebEngineView(parent),
    settings_(settings),
    loader_(nullptr)
{
    player_ = settings_->soundsEnabled ? new QPlayer(this) : nullptr;
    page()->setZoomFactor(settings_->zoomFactor);
    page()->setBackgroundColor(settings_->backgroundColor);
    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::PreventContextMenu);
}

void KioskView::handleWindowCloseRequested()
{
    // TODO: Do we handle windows opening and if so, what happens
    // when they close.
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

void KioskView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        playSound(settings_->windowClickedSound);

    QWebEngineView::mousePressEvent(event);
}

void KioskView::playSound(const QUrl &sound)
{
    if (player_)
        player_->play(sound);
}

QWebEngineView *KioskView::createWindow(QWebEnginePage::WebWindowType /*type*/)
{
    // Don't allow windows
    return nullptr;

#if 0
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
#endif
}
