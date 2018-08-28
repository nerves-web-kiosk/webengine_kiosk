#include <QtDebug>
#include <QtGui>
#include <QtWebEngineCore>
#include <QApplication>
#include "KioskView.h"

#include "KioskWindow.h"
#include "KioskSettings.h"

KioskView::KioskView(const KioskSettings *settings, QWidget* parent): QWebEngineView(parent),
    settings_(settings)
{
    page()->setZoomFactor(settings_->zoomFactor);
    page()->setBackgroundColor(settings_->backgroundColor);

    setFocusPolicy(Qt::StrongFocus);
    setContextMenuPolicy(Qt::PreventContextMenu);
}

QWebEngineView *KioskView::createWindow(QWebEnginePage::WebWindowType type)
{
    qDebug("KioskView::createWindow %d", type);

    // Don't allow windows
    return nullptr;
}
