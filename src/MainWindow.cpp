#include <QtGui>
#include <QtNetwork>
#include <QtWebEngineCore>
#include <QDebug>
#include "MainWindow.h"

#include <QStandardPaths>
#include <QApplication>
#include <QDesktopWidget>
#include <QtWebEngineWidgets>
#include <QMenu>
#include <QInputDialog>

class WebView;

MainWindow::MainWindow(const KioskSettings *settings) : QMainWindow(),
    settings_(settings)
{
    if (settings->debugMenuEnabled) {
        QMenu *debugMenu = menuBar()->addMenu(tr("&Debug"));
        QAction *action = new QAction(tr("Run &Javascript"), this);
        connect(action, &QAction::triggered, this, &MainWindow::doRunJavascriptDialog);
        debugMenu->addAction(action);
        action = new QAction(tr("Go to &URL"), this);
        connect(action, &QAction::triggered, this, &MainWindow::doGotoURLDialog);
        debugMenu->addAction(action);
    }
}

void MainWindow::doRunJavascriptDialog()
{
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, tr("Kiosk"),
                                         tr("Enter some Javascript:"), "", &ok);
    if (ok && !text.isEmpty()) {
        qDebug("Running Javascript: %s", qPrintable(text));
        view_->page()->runJavaScript(text);
    }
}

void MainWindow::doGotoURLDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Kiosk"),
                                         tr("Enter a URL:"), QLineEdit::Normal, "https://youtube.com/", &ok);
    if (ok && !text.isEmpty()) {
        qDebug("Going to URL: %s", qPrintable(text));
        view_->loadCustomPage(text);
    }
}

void MainWindow::init()
{
    qDebug("MainWindow::init()");
    setMinimumWidth(320);
    setMinimumHeight(200);

    // --- Network --- //

    if (settings_->proxyEnabled) {
        if (settings_->proxySystem) {
            QNetworkProxyFactory::setUseSystemConfiguration(true);
        } else {
            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(settings_->proxyHostname);
            proxy.setPort(settings_->proxyPort);
            if (!settings_->proxyUsername.isEmpty()) {
                proxy.setUser(settings_->proxyUsername);
                proxy.setPassword(settings_->proxyPassword);
            }
            QNetworkProxy::setApplicationProxy(proxy);
        }
    }

    // --- Web View --- //
    view_ = new WebView(settings_, this);

    if (settings_->progress) {
        // --- Progress Bar --- //
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
    }

    if (settings_->hideCursor)
        QApplication::setOverrideCursor(Qt::BlankCursor);

    setCentralWidget(view_);

    view_->setPage(new QWebEnginePage(view_));
    view_->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, settings_->javascriptEnabled);
    view_->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, settings_->javascriptCanOpenWindows);

    connect(view_, SIGNAL(loadStarted()), SLOT(startLoading()));
    connect(view_, SIGNAL(urlChanged(const QUrl &)), SLOT(urlChanged(const QUrl &)));
    connect(view_, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view_, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    QDesktopWidget *desktop = QApplication::desktop();
    connect(desktop, SIGNAL(resized(int)), SLOT(desktopResized(int)));

    shortcutKeys_ = {
        //{Qt::Key_Up, [](){view->scrollUp();},
        //{Qt::Key_Down, [](){view->scrollDown();},
        //{Qt::Key_PageUp, [](){view->scrollPageUp();},
        //{Qt::Key_PageDown, [](){view->scrollPageDown();},
        //{Qt::Key_End, [](){view->scrollEnd();},
        {
            Qt::Key_HomePage,
            [this](){view_->loadHomepage();}
        },
        //{Qt::Key_Home, [](){ view->scrollHome(); }},
        {
            Qt::Key_Home + Qt::CTRL,
            [this](){view_->loadHomepage();}
        },
        {
            QKeySequence::Back,
            [this](){view_->page()->triggerAction(QWebEnginePage::Back);}
        },
        {
            QKeySequence::Forward,
            [this](){view_->page()->triggerAction(QWebEnginePage::Forward);}
        },
        {
            QKeySequence::Quit,
            [this]() {
                clearCacheOnExit();
                QApplication::exit(0);
            }
        },
        {
            QKeySequence(Qt::Key_F5 + Qt::CTRL, Qt::Key_R+Qt::CTRL+Qt::SHIFT),
            [this]() {
                clearCache();
                view_->page()->triggerAction(QWebEnginePage::ReloadAndBypassCache);
            }
        },
        {
            QKeySequence::Refresh,
            [this]() {view_->reload();}
        },
        {
            QKeySequence::FullScreen,
            [this]() {
                if (isFullScreen()) {
                    showNormal();
                } else {
                    showFullScreen();
                }
            }
        }
    };

    auto i = shortcutKeys_.constBegin();
    while (i != shortcutKeys_.constEnd()) {
        QAction* tempAction = new QAction(this);
        tempAction->setShortcut(i.key());
        tempAction->setShortcutContext(Qt::ApplicationShortcut);
        connect(tempAction, &QAction::triggered,
                i.value());
        this->addAction(tempAction);
        ++i;
    }

    showFullScreen();
    view_->page()->view()->setFocusPolicy(Qt::StrongFocus);
    view_->setFocusPolicy(Qt::StrongFocus);
#if 0
    if (mainSettings_->value("view/stay_on_top").toBool()) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    }
#endif

    view_->loadHomepage();
}

void MainWindow::clearCache()
{
    // Look at QWebEngineProfile
}

void MainWindow::clearCacheOnExit()
{
    // Look at QWebEngineProfile
}

void MainWindow::cleanupSlot()
{
    qDebug("Cleanup Slot (application exit)");
    clearCacheOnExit();
    //QWebEngineSettings::clearMemoryCaches();
}

void MainWindow::showFullScreen()
{
    qDebug("showFullScreen");
    int screen = computedScreen();
    if (screen >= 0) {
        if (this->windowHandle()) {
            this->windowHandle()->setScreen(qApp->screens()[screen]);
        }
        QRect screenGeometry = qApp->desktop()->availableGeometry(screen);
        setGeometry(screenGeometry);
        view_->updateZoom();
        qDebug() << "setting geometry:" << screenGeometry;
    }

    QMainWindow::showFullScreen();
    updatePixelRatio();
}

#if 0
void MainWindow::moveEvent(QMoveEvent *) {

    // What???!?!?!?!?!?? This makes no sense
    updatePixelRatio();
}
#endif

void MainWindow::updatePixelRatio() {
    qDebug("MainWindow::updatePixelRatio");
    qreal _pixelRatio = (windowHandle()->screen()->logicalDotsPerInch()/96.0);
    if (pixelRatio_ != _pixelRatio) {
        pixelRatio_ = _pixelRatio;
        view_->updateZoom();
        qDebug() << "pixel ratio set to: " << pixelRatio_;
    }
}

qreal MainWindow::getPixelRatio() {
    return pixelRatio_;
}

int MainWindow::computedScreen() {
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "got key: " << event->key();
    //if (mainSettings_->value("browser/disable_hotkeys").toBool()) {
        QMainWindow::keyPressEvent(event);
        return;
//    }
}

void MainWindow::desktopResized(int p)
{
    qDebug() << "Desktop resized event: " << p;
    showFullScreen();
}

void MainWindow::startLoading()
{
    //QWebEngineSettings::clearMemoryCaches();

    if (settings_->progress)
        loadProgress_->show();

    qDebug("Start loading...");
}

void MainWindow::setProgress(int p)
{
    qDebug() << "Loading progress: " << p;

    if (settings_->progress) {
        loadProgress_->setValue(p);
        if (p >= 100)
            loadProgress_->hide();
    }
}

void MainWindow::urlChanged(const QUrl &url)
{
    qDebug() << "URL changes: " << url.toString();

    view_->updateZoom();

    // This is real link clicked
    view_->playSound(settings_->linkClickedSound);
}

void MainWindow::finishLoading(bool)
{
    qDebug("Finish loading...");

    if (settings_->progress)
        loadProgress_->hide();

    // 3. Focus window and click into it to stimulate event loop after signal handling
    QApplication::setActiveWindow(this);
    this->focusWidget();
}

