#include "StderrPipe.h"
#include <QSocketNotifier>

#include <errno.h>
#include <unistd.h>

StderrPipe::StderrPipe(QObject *parent) :
    QObject(parent),
    index_(0)
{
    if (pipe(pipefd_) < 0)
        qFatal("pipe");
    if (dup2(pipefd_[1], STDERR_FILENO) < 0)
        qFatal("dup2(stderr)");

    notifier_ = new QSocketNotifier(pipefd_[0], QSocketNotifier::Read, this);
    connect(notifier_, SIGNAL(activated(int)), SLOT(process()));
}

/**
 * @brief Dispatch commands in the buffer
 * @return the number of bytes processed
 */
ssize_t StderrPipe::tryDispatch()
{
    const char *newline = (const char *) memchr(buffer_, '\n', index_);
    if (!newline) {
        if (index_ == sizeof(buffer_)) {
            emit inputReceived(QByteArray(buffer_, sizeof(buffer_)));
            return sizeof(buffer_);
        } else
            return 0;
    }

    int newline_ix = (newline - buffer_);
    buffer_[newline_ix] = 0;

    emit inputReceived(QByteArray(buffer_, newline_ix));
    return newline_ix + 1;
}

/**
 * @brief call to process any new requests from Erlang
 */
void StderrPipe::process()
{
    ssize_t amountRead = read(pipefd_[0], buffer_, sizeof(buffer_) - index_);
    if (amountRead < 0) {
        /* EINTR is ok to get, since we were interrupted by a signal. */
        if (errno == EINTR)
            return;

        /* Everything else is unexpected. */
        qFatal("read failed");
    }

    index_ += amountRead;
    for (;;) {
        ssize_t bytesProcessed = tryDispatch();

        if (bytesProcessed == 0) {
            break;
        } else if (index_ > (size_t) bytesProcessed) {
            /* Processed the line and there's more. */
            memmove(buffer_, &buffer_[bytesProcessed], index_ - bytesProcessed);
            index_ -= bytesProcessed;
        } else {
            /* Processed the whole buffer. */
            index_ = 0;
            break;
        }
    }
}
