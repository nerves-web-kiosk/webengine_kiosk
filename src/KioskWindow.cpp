#include "KioskWindow.h"
#include "Kiosk.h"
#include "KioskProgress.h"
#include "KioskView.h"
#include "Blanking.h"

#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>
#include <QApplication>

KioskWindow::KioskWindow(Kiosk *kiosk, const KioskSettings *settings) :
    QWidget(),
    kiosk_(kiosk),
    settings_(settings),
    view_(nullptr),
    showingBrowser_(false)
{
    setMinimumWidth(320);
    setMinimumHeight(200);

    blank_ = new Blanking(this);
    blank_->setStyleSheet(QString("background: %1").arg(settings->backgroundColor.name()));
    if (!settings->blankImage.isEmpty())
        blank_->setPixmap(settings->blankImage);
    connect(blank_, SIGNAL(mousePressed()), SIGNAL(wakeup()));

    progress_ = new KioskProgress(this);
    progress_->hide();

    QAction* tempAction = new QAction(this);
    tempAction->setShortcut(QKeySequence::Quit);
    tempAction->setShortcutContext(Qt::ApplicationShortcut);
    connect(tempAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    addAction(tempAction);

#if 0
    if (settings->debugMenuEnabled) {
        QMenu *debugMenu = menuBar()->addMenu(tr("&Debug"));
        QAction *action = new QAction(tr("Run &Javascript"), this);
        connect(action, &QAction::triggered, this, &KioskWindow::doRunJavascriptDialog);
        debugMenu->addAction(action);
        action = new QAction(tr("Go to &URL"), this);
        connect(action, &QAction::triggered, this, &KioskWindow::doGotoURLDialog);
        debugMenu->addAction(action);
    }
#endif
}

KioskWindow::~KioskWindow()
{
}

void KioskWindow::setView(KioskView *view)
{
    view_ = view;
    view_->setVisible(showingBrowser_);
    //view_->setParent(this);
    view_->stackUnder(progress_);
    QSize sz = size();
    view_->setGeometry(0, 0, sz.width(), sz.height());
    if (showingBrowser_) {
        view_->setEnabled(true);
        view_->setFocus();
    }
}

void KioskWindow::setBrowserVisible(bool enabled)
{
    showingBrowser_ = enabled;
    if (view_) {
        view_->setVisible(enabled);
        view_->setEnabled(enabled);
        blank_->setEnabled(!enabled);
        if (enabled) {
            view_->setFocus();
        }
    }
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
    QSize sz = event->size();
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
                                         tr("Enter a URL:"), QLineEdit::Normal, "https://elixir-lang.org/", &ok);
    if (ok && !uri.isEmpty())
        kiosk_->goToUrl(uri);
}

