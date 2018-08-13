#ifndef KIOSKPROGRESS_H
#define KIOSKPROGRESS_H

#include <QObject>
#include <QWidget>

class QProgressBar;

class KioskProgress : public QWidget
{
    Q_OBJECT
public:
    explicit KioskProgress(QWidget *parent = nullptr);

    void setProgress(int p);

private:
    QProgressBar *loadProgress_;
};

#endif // KIOSKPROGRESS_H
