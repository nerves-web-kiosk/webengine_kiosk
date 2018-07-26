#include "KioskWindow.h"
#include "Kiosk.h"

#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>

#include <QApplication>
#include <QDesktopWidget>

KioskWindow::KioskWindow(Kiosk *kiosk, const KioskSettings *settings) :
    QMainWindow(),
    kiosk_(kiosk),
    settings_(settings)
{
    setMinimumWidth(320);
    setMinimumHeight(200);

    loadProgress_ = new QProgressBar();
    loadProgress_->setWindowFlag(Qt::FramelessWindowHint, true);
    loadProgress_->setContentsMargins(2, 2, 2, 2);
    loadProgress_->setMinimumWidth(100);
    loadProgress_->setMinimumHeight(16);
    loadProgress_->setFixedHeight(16);
    loadProgress_->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(255,255,255,63));
    loadProgress_->setPalette(palette);

    // Do not work... Need Layout...
    loadProgress_->setAlignment(Qt::AlignTop);
    loadProgress_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    loadProgress_->hide();

    QAction* tempAction = new QAction(this);
    tempAction->setShortcut(QKeySequence::Quit);
    tempAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(tempAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    addAction(tempAction);


    if (settings->debugMenuEnabled) {
        QMenu *debugMenu = menuBar()->addMenu(tr("&Debug"));
        QAction *action = new QAction(tr("Run &Javascript"), this);
        connect(action, &QAction::triggered, this, &KioskWindow::doRunJavascriptDialog);
        debugMenu->addAction(action);
        action = new QAction(tr("Go to &URL"), this);
        connect(action, &QAction::triggered, this, &KioskWindow::doGotoURLDialog);
        debugMenu->addAction(action);
    }
}

KioskWindow::~KioskWindow()
{
    delete loadProgress_;
}

void KioskWindow::doRunJavascriptDialog()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, tr("Kiosk"),
                                         tr("Enter some Javascript:"), "", &ok);
    if (ok && !text.isEmpty())
        kiosk_->runJavascript(text);
}

void KioskWindow::doGotoURLDialog()
{
    bool ok;
    QString uri = QInputDialog::getText(this, tr("Kiosk"),
                                         tr("Enter a URL:"), QLineEdit::Normal, "https://youtube.com/", &ok);
    if (ok && !uri.isEmpty())
        kiosk_->goToUrl(uri);
}

void KioskWindow::init()
{
    qDebug("MainWindow::init()");

    if (settings_->hideCursor)
        QApplication::setOverrideCursor(Qt::BlankCursor);

    QDesktopWidget *desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SLOT(desktopResized(int)));


    showFullScreen();
#if 0
    if (mainSettings_->value("view/stay_on_top").toBool()) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
#endif
}

void KioskWindow::showProgress(int p)
{
    loadProgress_->setValue(p);
    loadProgress_->show();
}

void KioskWindow::hideProgress()
{
    loadProgress_->hide();
}

void KioskWindow::showFullScreen()
{
    qDebug("showFullScreen");
    int screen = computedScreen();
    if (screen >= 0) {
        if (this->windowHandle()) {
            this->windowHandle()->setScreen(qApp->screens()[screen]);
        }
        QRect screenGeometry = qApp->desktop()->availableGeometry(screen);
        setGeometry(screenGeometry);
    }

    QMainWindow::showFullScreen();
}

int KioskWindow::computedScreen() {
    qDebug("MainWindow::computedScreen");
    const QList<QScreen*> screens = qApp->screens();
    int numScreens = screens.size();
    if (settings_->monitor >= numScreens) {
        qDebug() << "invalid monitor" << settings_->monitor << ", you only have " << numScreens << "screens.";
        return 0;
    } else {
        qDebug() << "setting screen" << settings_->monitor+1 << "/" << numScreens;
        return settings_->monitor;
    }
}

void KioskWindow::desktopResized(int p)
{
    qDebug() << "Desktop resized event: " << p;
    showFullScreen();
}



