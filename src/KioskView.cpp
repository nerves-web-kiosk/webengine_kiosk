#include <QtDebug>
#include <QtGui>
#include <QtWebEngineCore>
#include <QApplication>
#include <QWebEngineProfile>
#include "KioskView.h"

#include "KioskWindow.h"
#include "KioskSettings.h"

KioskView::KioskView(const KioskSettings *settings, QWidget* parent): QWebEngineView(parent),
    settings_(settings),
    loader_(nullptr)
{
    page()->setZoomFactor(settings_->zoomFactor);
    page()->setBackgroundColor(settings_->backgroundColor);
    if (!settings_->httpAcceptLanguage.isEmpty()) {
        page()->profile()->setHttpAcceptLanguage(settings_->httpAcceptLanguage);
    }
    if (!settings_->httpUserAgent.isEmpty()) {
        page()->profile()->setHttpUserAgent(settings_->httpUserAgent);
    }

    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::PreventContextMenu);
}

void KioskView::handleWindowCloseRequested()
{

    qDebug("KioskView::handleWindowCloseRequested");
}

QWebEngineView *KioskView::createWindow(QWebEnginePage::WebWindowType /*type*/)
{
    qDebug("KioskView::createWindow");

    // Don't allow windows
    return nullptr;
}
