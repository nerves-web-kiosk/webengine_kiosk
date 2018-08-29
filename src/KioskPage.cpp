#include "KioskPage.h"

KioskPage::KioskPage(QObject *parent) : QWebEnginePage(parent)
{

}

QWebEnginePage *KioskPage::createWindow(QWebEnginePage::WebWindowType type)
{
    qDebug("KioskPage::createWindow");
    return QWebEnginePage::createWindow(type);
}

QStringList KioskPage::chooseFiles(QWebEnginePage::FileSelectionMode mode, const QStringList &oldFiles, const QStringList &acceptedMimeTypes)
{
    qDebug("KioskPage::chooseFiles");
    return QWebEnginePage::chooseFiles(mode, oldFiles, acceptedMimeTypes);
}

void KioskPage::javaScriptAlert(const QUrl &securityOrigin, const QString &msg)
{
    qDebug("KioskPage::javaScriptAlert");
    return QWebEnginePage::javaScriptAlert(securityOrigin, msg);
}

bool KioskPage::javaScriptConfirm(const QUrl &securityOrigin, const QString &msg)
{
    qDebug("KioskPage::javaScriptConfirm");
    return QWebEnginePage::javaScriptConfirm(securityOrigin, msg);
}

bool KioskPage::javaScriptPrompt(const QUrl &securityOrigin, const QString &msg, const QString &defaultValue, QString *result)
{
    qDebug("KioskPage::javaScriptPrompt %s", qPrintable(msg));
    return QWebEnginePage::javaScriptPrompt(securityOrigin, msg, defaultValue, result);
}

void KioskPage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    qDebug("KioskPage::javaScriptConsoleMessage %s", qPrintable(message));
    return QWebEnginePage::javaScriptConsoleMessage(level, message, lineNumber, sourceID);
}

bool KioskPage::certificateError(const QWebEngineCertificateError &certificateError)
{
    qDebug("KioskPage::certificateError");
    return QWebEnginePage::certificateError(certificateError);
}

bool KioskPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    qDebug("KioskPage::acceptNavigationRequest %s, %d, %d", qPrintable(url.toString()), type, isMainFrame);
    bool rc;
    if (!isMainFrame)
        rc = false;
    else
        rc = QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);

    qDebug("    rc = %d", rc);
    return rc;
}
