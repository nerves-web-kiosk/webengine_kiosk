/****************************************************************************
**
** Copyright (C) 2011 Sergey Dryabzhinsky
** All rights reserved.
** Contact: Sergey Dryabzhinsky (sergey.dryabzhinsky@gmail.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>

#include <sys/types.h>
#include <unistd.h>
#include <err.h>

#include "mainwindow.h"
#include "KioskSettings.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    KioskSettings settings(app);

    // Drop/change privilege if requested
    // See https://wiki.sei.cmu.edu/confluence/display/c/POS36-C.+Observe+correct+revocation+order+while+relinquishing+privileges
    if (settings.gid > 0 && setgid(settings.gid) < 0)
        err(EXIT_FAILURE, "setgid(%d)", settings.gid);

    if (settings.uid > 0 && setuid(settings.uid) < 0)
        err(EXIT_FAILURE, "setuid(%d)", settings.uid);

    if (settings.opengl == "gl") {
        QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        qDebug() << "attempting to use native OpenGL";
    } else if (settings.opengl == "gles") {
        QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
        qDebug() << "attempting to use OpenGL ES (or ANGLE, on Windows)";
    } else if (settings.opengl == "software") {
        qDebug() << "using software rendering";
        QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    } else {
        qDebug() << "using automatic gl";
    }

    MainWindow browser(&settings);
    browser.init();

    // executes browser.cleanupSlot() when the Qt framework emits aboutToQuit() signal.
    QObject::connect(&app, SIGNAL(aboutToQuit()),
                     &browser, SLOT(cleanupSlot()));

    return app.exec();
}
