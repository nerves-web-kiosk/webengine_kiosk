#ifndef KIOSK_H
#define KIOSK_H

#include "KioskSettings.h"
#include <QWebEnginePage>

class ElixirComs;
class KioskMessage;
class KioskWindow;
class KioskView;
class KioskProgress;
class KioskSounds;
class QWindow;
class StderrPipe;

class Kiosk : public QObject
{
    Q_OBJECT
public:
    explicit Kiosk(const KioskSettings *settings, QObject *parent = nullptr);

    void init();

public slots:
    void goToUrl(const QUrl &url);
    void runJavascript(const QString &program);
    void reload();
    void goBack();
    void goForward();
    void stopLoading();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void handleRequest(const KioskMessage &message);

    void urlChanged(const QUrl &);
    void startLoading();
    void setProgress(int p);
    void finishLoading();

    void handleWakeup();
    void handleRenderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus status, int exitCode);
    void handleStderr(const QByteArray &line);

private:
    QRect calculateWindowRect() const;

private:
    const KioskSettings *settings_;
    ElixirComs *coms_;
    StderrPipe *stderrPipe_;

    KioskWindow *window_;
    KioskView *view_;

    bool loadingPage_;
    bool showPageWhenDone_;

    KioskSounds *player_;
    QWindow *theGoodWindow_;
};

#endif // KIOSK_H
