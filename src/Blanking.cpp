#include "Blanking.h"
#include <QMouseEvent>

Blanking::Blanking(QWidget *parent) :
    QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
}

void Blanking::mouseMoveEvent(QMouseEvent *event)
{
    qDebug("Blanking mouse move event: %d, %d", event->x(), event->y());
}

void Blanking::mousePressEvent(QMouseEvent *event)
{
    qDebug("Blanking mouse press event: %d, %d", event->x(), event->y());
}
