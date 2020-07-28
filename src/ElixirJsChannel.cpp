#include "ElixirJsChannel.h"

void ElixirJsChannel::send(const QString &messageStr)
{
    emit received(messageStr);
}
