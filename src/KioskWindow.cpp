#include "KioskWindow.h"
#include "Kiosk.h"
#include "KioskProgress.h"
#include "KioskView.h"

#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>
#include <QLabel>

KioskWindow::KioskWindow(Kiosk *kiosk, const KioskSettings *settings) :
    QMainWindow(),
    kiosk_(kiosk),
    settings_(settings),
    view_(nullptr),
    showingBrowser_(false)
{
    setMinimumWidth(320);
    setMinimumHeight(200);

    QWidget *centralWidget = new QWidget(this);

    blank_ = new QLabel(centralWidget);
    blank_->setAlignment(Qt::AlignCenter);
    blank_->setStyleSheet(QString("background: %1").arg(settings->backgroundColor.name()));
    if (!settings->blankImage.isEmpty())
        blank_->setPixmap(settings->blankImage);

    progress_ = new KioskProgress(centralWidget);
    progress_->hide();

    setCentralWidget(centralWidget);

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
}

void KioskWindow::setView(KioskView *view)
{
    view_ = view;
    view_->setVisible(showingBrowser_);
    view_->setParent(centralWidget());
    view_->stackUnder(progress_);
    QSize sz = centralWidget()->size();
    view_->setGeometry(0, 0, sz.width(), sz.height());
}

void KioskWindow::setBrowserVisible(bool enabled)
{
    showingBrowser_ = enabled;
    if (view_)
        view_->setVisible(enabled);
}

void KioskWindow::showProgress(int percent)
{
    progress_->setProgress(percent);
    progress_->show();
}

void KioskWindow::hideProgress()
{
    progress_->hide();
}

void KioskWindow::showBrowser()
{
    setBrowserVisible(true);
}

void KioskWindow::hideBrowser()
{
    setBrowserVisible(false);
}

void KioskWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize sz = centralWidget()->size();

    blank_->setGeometry(0, 0, sz.width(), sz.height());
    if (view_)
        view_->setGeometry(0, 0, sz.width(), sz.height());

    int x = (sz.width() - progress_->width()) / 2;
    int y = (sz.height() - progress_->height()) / 2;
    progress_->setGeometry(x, y, progress_->width(), progress_->height());
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

