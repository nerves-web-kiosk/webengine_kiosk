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
}

QWebEngineView *KioskView::createWindow(QWebEnginePage::WebWindowType type)
{
    qDebug("KioskView::createWindow %d", type);

    // Don't allow windows
    return nullptr;
}
