#ifndef KIOSKMESSAGE_H
#define KIOSKMESSAGE_H

#include <QByteArray>

class KioskMessage
{
public:
    enum Type {
        GoToURL = 1,
        RunJavascript = 2,
        LoadingPage = 3,
        Progress = 4,
        FinishedLoadingPage = 5
    };

    explicit KioskMessage(const QByteArray &rawMessage);
    KioskMessage(Type type, QByteArray payload);
    KioskMessage(const char *buffer, int length);

    enum Type type() const;
    QByteArray payload() const;

    int length() const { return rawMessage_.length(); }
    const char *constData() const { return rawMessage_.constData(); }

    static KioskMessage progressMessage(int progress);
    static KioskMessage loadingPageMessage();
    static KioskMessage finishedLoadingPageMessage();

private:
    const QByteArray rawMessage_;
};

#endif // KIOSKMESSAGE_H
