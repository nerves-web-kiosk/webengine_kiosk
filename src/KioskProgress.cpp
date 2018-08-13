#include "KioskProgress.h"

#include <QProgressBar>
#include <QResizeEvent>

KioskProgress::KioskProgress(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(100);
    setMinimumHeight(16);
    setFixedHeight(16);

    loadProgress_ = new QProgressBar(this);
    loadProgress_->setContentsMargins(2, 2, 2, 2);
    loadProgress_->setMinimumWidth(100);
    loadProgress_->setMinimumHeight(16);
    loadProgress_->setFixedHeight(16);
    loadProgress_->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(255,255,255,63));
    loadProgress_->setPalette(palette);
}

void KioskProgress::setProgress(int p)
{
    loadProgress_->setValue(p);
}
