#ifndef ELIXIRJSCHANNEL_H
#define ELIXIRJSCHANNEL_H

#include <QObject>

class ElixirJsChannel : public QObject
{
public:
    Q_OBJECT

signals:
    void received(const QString &messageStr);

public slots:
    void send(const QString &messageStr);
};

#endif // ELIXIRJSCHANNEL_H
