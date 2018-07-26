#ifndef ELIXIRCOMS_H
#define ELIXIRCOMS_H

#include <QObject>
#include "KioskMessage.h"

class QSocketNotifier;

class ElixirComs : public QObject
{
    Q_OBJECT
public:
    explicit ElixirComs(QObject *parent = nullptr);

    void send(const KioskMessage &message);

signals:
    void messageReceived(KioskMessage message);

private slots:
    void process();
    ssize_t tryDispatch();

private:
    QSocketNotifier *stdinNotifier_;
    char buffer_[16384];
    size_t index_;
};

#endif // ELIXIRCOMS_H
