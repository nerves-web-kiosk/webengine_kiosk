#ifndef BLANKING_H
#define BLANKING_H

#include <QLabel>

class Blanking : public QLabel
{
    Q_OBJECT
public:
    explicit Blanking(QWidget *parent = nullptr);

signals:
    void mousePressed();

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // BLANKING_H
