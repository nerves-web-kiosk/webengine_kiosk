#include "ElixirComs.h"
#include <QSocketNotifier>

#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>
#include <arpa/inet.h>

ElixirComs::ElixirComs(QObject *parent) :
    QObject(parent),
    index_(0)
{
    stdinNotifier_ = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, this);
    connect(stdinNotifier_, SIGNAL(activated(int)), SLOT(process()));
}

/**
 * @brief Synchronously send a response back to Erlang
 *
 * @param response what to send back
 */
void ElixirComs::send(const KioskMessage &message)
{
    ssize_t len = message.length();
    uint16_t be_len = htons(len);

    iovec chunks[2];
    chunks[0].iov_base = &be_len;
    chunks[0].iov_len = sizeof(uint16_t);
    chunks[1].iov_base = (void *) message.constData();
    chunks[1].iov_len = len;

    len += sizeof(uint16_t);
    ssize_t wrote = 0;
    for (;;) {
        ssize_t amount_written = writev(STDOUT_FILENO, chunks, 2);
        if (amount_written < 0) {
            if (errno == EINTR)
                continue;

            qFatal("write");
        }

        wrote += amount_written;
        if (wrote == len)
            break;

        for (int i = 0; i < 2; i++) {
            if (amount_written > (ssize_t) chunks[i].iov_len) {
                amount_written -= chunks[i].iov_len;
                chunks[i].iov_len = 0;
            } else {
                chunks[i].iov_len -= amount_written;
                break;
            }
        }
    }
}

/**
 * @brief Dispatch commands in the buffer
 * @return the number of bytes processed
 */
ssize_t ElixirComs::tryDispatch()
{
    /* Check for length field */
    if (index_ < sizeof(uint16_t))
        return 0;

    uint16_t be_len;
    memcpy(&be_len, buffer_, sizeof(uint16_t));
    ssize_t msglen = ntohs(be_len);
    if (msglen + sizeof(uint16_t) > sizeof(buffer_))
        qFatal("Message too long");

    /* Check whether we've received the entire message */
    if (msglen + sizeof(uint16_t) > index_)
        return 0;

    emit messageReceived(KioskMessage(&buffer_[2], msglen));

    return msglen + sizeof(uint16_t);
}

/**
 * @brief call to process any new requests from Erlang
 */
void ElixirComs::process()
{
    ssize_t amountRead = read(STDIN_FILENO, buffer_, sizeof(buffer_) - index_);
    if (amountRead < 0) {
        /* EINTR is ok to get, since we were interrupted by a signal. */
        if (errno == EINTR)
            return;

        /* Everything else is unexpected. */
        qFatal("read failed");
    } else if (amountRead == 0) {
        /* EOF. Erlang process was terminated. This happens after a release or if there was an error. */
        exit(EXIT_SUCCESS);
    }

    index_ += amountRead;
    for (;;) {
        ssize_t bytesProcessed = tryDispatch();

        if (bytesProcessed == 0) {
            /* Only have part of the command to process. */
            break;
        } else if (index_ > (size_t) bytesProcessed) {
            /* Processed the command and there's more data. */
            memmove(buffer_, &buffer_[bytesProcessed], index_ - bytesProcessed);
            index_ -= bytesProcessed;
        } else {
            /* Processed the whole buffer. */
            index_ = 0;
            break;
        }
    }
}
