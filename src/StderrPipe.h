#ifndef STDERRPIPE_H
#define STDERRPIPE_H

#include <QObject>
#include "KioskMessage.h"

class QSocketNotifier;

class StderrPipe : public QObject
{
    Q_OBJECT
public:
    explicit StderrPipe(QObject *parent = nullptr);

signals:
    void inputReceived(const QByteArray &line);

private slots:
    void process();

private:
    ssize_t tryDispatch();

private:
    QSocketNotifier *notifier_;
    int pipefd_[2];
    char buffer_[1024];
    size_t index_;
};


#endif // STDERRPIPE_H
