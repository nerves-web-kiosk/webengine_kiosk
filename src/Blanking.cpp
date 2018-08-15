#include "Blanking.h"
#include <QMouseEvent>

Blanking::Blanking(QWidget *parent) :
    QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
}

void Blanking::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit mousePressed();
}
