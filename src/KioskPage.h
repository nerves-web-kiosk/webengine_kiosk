#ifndef KIOSKPAGE_H
#define KIOSKPAGE_H

#include <QObject>
#include <QWebEnginePage>

class KioskPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit KioskPage(QObject *parent = nullptr);

protected:
    QWebEnginePage *createWindow(WebWindowType type);
    QStringList chooseFiles(FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes);
    void javaScriptAlert(const QUrl &securityOrigin, const QString& msg);
    bool javaScriptConfirm(const QUrl &securityOrigin, const QString& msg);
    bool javaScriptPrompt(const QUrl &securityOrigin, const QString& msg, const QString& defaultValue, QString* result);
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString& message, int lineNumber, const QString& sourceID);
    bool certificateError(const QWebEngineCertificateError &certificateError);
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
};

#endif // KIOSKPAGE_H
