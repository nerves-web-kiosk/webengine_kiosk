#include "KioskWindow.h"
#include "Kiosk.h"
#include "KioskProgress.h"
#include "KioskView.h"
#include "KioskPage.h"
#include "Blanking.h"

#include <QInputDialog>
#include <QApplication>
#include <QAction>


static void dumpWidgets(QWidget *widget, const QString &indent)
{
    qDebug("%s%p (%s::%s) hidden=%d enabled=%d isActiveWindow=%d\r",
           qPrintable(indent), widget,
           qPrintable(widget->metaObject()->className()), qPrintable(widget->objectName()),
           widget->isHidden(), widget->isEnabled(),
           widget->isActiveWindow());
    QString newIndent = indent + "  ";
    foreach (QObject *child, widget->children()) {
        QWidget *c = dynamic_cast<QWidget*>(child);
        if (c)
            dumpWidgets(c, newIndent);
    }
}

static void dumpWidgets()
{
    qDebug("----dumpWidgets begin----\r");
    qDebug("Active window: %p\r", qApp->activeWindow());
    QString indent = "->";
    foreach (QWidget *w, qApp->topLevelWidgets()) {
        dumpWidgets(w, indent);
    }
    qDebug("All Widgets\r");
    foreach (QWidget *w, qApp->allWidgets()) {
        qDebug() << w << "\r";
    }
    qDebug("----dumpWidgets end----\r");
}

KioskWindow::KioskWindow(Kiosk *kiosk, const KioskSettings *settings) :
    QWidget(),
    kiosk_(kiosk),
    settings_(settings),
    showingBrowser_(false)
{
    setMinimumWidth(320);
    setMinimumHeight(200);
    setObjectName("KioskWindow");

#if 0
    blank_ = new Blanking(this);
    blank_->setObjectName("Blanking");
    blank_->setStyleSheet(QString("background: %1").arg(settings->backgroundColor.name()));
    if (!settings->blankImage.isEmpty())
        blank_->setPixmap(settings->blankImage);
    connect(blank_, SIGNAL(mousePressed()), SIGNAL(wakeup()));
#endif

    //view_ = new KioskView(settings_, this);
    view_ = new QWebEngineView(this);
    view_->setObjectName("QWebEngineView");
    view_->setPage(new KioskPage(this));
    qDebug("Setting zoom factor to %f", settings_->zoomFactor);
    view_->page()->setZoomFactor(settings_->zoomFactor);
    view_->page()->setBackgroundColor(settings_->backgroundColor);
    view_->setFocusPolicy(Qt::StrongFocus);
    view_->setContextMenuPolicy(Qt::PreventContextMenu);

    dumpWidgets();
    //view_->hide();

   // progress_ = new KioskProgress(this);
   // progress_->setObjectName("KioskProgress");
   // progress_->hide();

    QAction* action = new QAction(this);
    action->setShortcut(QKeySequence::Quit);
    action->setShortcutContext(Qt::ApplicationShortcut);
    connect(action, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::Key_F5));
    action->setShortcutContext(Qt::WindowShortcut);
    connect(action, SIGNAL(triggered(bool)), kiosk, SLOT(reload()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_BracketLeft));
    action->setShortcutContext(Qt::WindowShortcut);
    connect(action, SIGNAL(triggered(bool)), kiosk, SLOT(goBack()));
    addAction(action);

    action = new QAction(this);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_BracketRight));
    action->setShortcutContext(Qt::WindowShortcut);
    connect(action, SIGNAL(triggered(bool)), kiosk, SLOT(goForward()));
    addAction(action);

    if (settings->debugKeysEnabled) {
        action = new QAction(this);
        action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
        action->setShortcutContext(Qt::WindowShortcut);
        connect(action, SIGNAL(triggered(bool)), SLOT(doRunJavascriptDialog()));
        addAction(action);

        action = new QAction(this);
        action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
        action->setShortcutContext(Qt::WindowShortcut);
        connect(action, SIGNAL(triggered(bool)), SLOT(doGotoURLDialog()));
        addAction(action);
    }
}

KioskWindow::~KioskWindow()
{
}

QWebEngineView *KioskWindow::view() const
{
    return view_;
}

void KioskWindow::setBrowserVisible(bool enabled)
{
    dumpWidgets();
    showingBrowser_ = enabled;
    //view_->setVisible(enabled);
    //view_->setEnabled(enabled);
//    blank_->setEnabled(!enabled);
    if (enabled) {
        view_->children();
    }
    qDebug("KioskWindow::setBrowserVisible %d\r", enabled);
}

void KioskWindow::showProgress(int percent)
{
    dumpWidgets();
    //progress_->setProgress(percent);
    //progress_->show();
}

void KioskWindow::hideProgress()
{
    //progress_->hide();
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
//    blank_->setGeometry(0, 0, sz.width(), sz.height());
    view_->setGeometry(0, 0, sz.width(), sz.height());

    // HACK??
    view_->setZoomFactor(view_->zoomFactor());
#if 0
    int x = (sz.width() - progress_->width()) / 2;
    int y = (sz.height() - progress_->height()) / 2;
    progress_->setGeometry(x, y, progress_->width(), progress_->height());
#endif
}

bool KioskWindow::event(QEvent *event)
{
    // Input events don't come here even when they're working.
    bool rc = QWidget::event(event);
    //qDebug("KioskWindow::event(%d) -> %d\r", event->type(), rc);
    return rc;
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

