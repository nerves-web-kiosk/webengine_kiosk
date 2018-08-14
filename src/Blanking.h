#ifndef BLANKING_H
#define BLANKING_H

#include <QLabel>

class Blanking : public QLabel
{
public:
    explicit Blanking(QWidget *parent);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // BLANKING_H
