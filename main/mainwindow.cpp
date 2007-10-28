/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "mdichild.h"
#include "Design/designchild.h"

MainWindow::MainWindow()
{
    workspace = new QWorkspace;
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)),
            this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget *)),
            workspace, SLOT(setActiveWindow(QWidget *)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    readSettings();

    setWindowTitle(tr("XFLR5QT"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    workspace->closeAllWindows();
    if (activeMdiChild()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newProject()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        MdiChild *existing = findMdiChild(fileName);
        if (existing) {
            workspace->setActiveWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("Project loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}


void MainWindow::insertProject()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        MdiChild *existing = findMdiChild(fileName);
        if (existing) {
            workspace->setActiveWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("Project loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}


void MainWindow::openFile()
{
	directDesign();
/*    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        MdiChild *existing = findMdiChild(fileName);
        if (existing) {
            workspace->setActiveWindow(existing);
            return;
        }

        MdiChild *child = createMdiChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }*/
}


void MainWindow::save()
{
    if (activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    if (activeMdiChild()->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}


void MainWindow::saveOptions()
{
   QMessageBox::information(this, tr("Save Options"),
            tr("Save options slot"));
}


void MainWindow::cut()
{
    //activeMdiChild()->cut();
}

void MainWindow::copy()
{
//     activeMdiChild()->copy();
}

void MainWindow::paste()
{
//     activeMdiChild()->paste();
}


void MainWindow::print()
{
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
		activeMdiChild()->print(&printer);
//    document->print(&printer);

    statusBar()->showMessage(tr("Ready"), 2000);
}


void MainWindow::about()
{
   QMessageBox::about(this, tr("About XFLR5QT"),
            tr("<b>XFLR5QT</b> is the Qt4 Version of XFLR5."));
}



void MainWindow::directDesign()
{
	QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
	  	DesignChild *child = createDesignChild();

        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("Foil loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
}


void MainWindow::inverseDesignFull()
{
   QMessageBox::information(this, tr("Full Inverse Design"),
            tr("Full inverse design slot"));
}

void MainWindow::inverseDesignMixed()
{
   QMessageBox::information(this, tr("Mixed Inverse Design"),
            tr("Mixed inverse desgin slot"));
}

void MainWindow::directAnalysis()
{
   QMessageBox::information(this, tr("Direct Analysis"),
            tr("Direct analysis slot"));
}

void MainWindow::wingDesign()
{
   QMessageBox::information(this, tr("Wing Design"),
            tr("Wing design slot"));
}


void MainWindow::preferences()
{
   QMessageBox::information(this, tr("Preferences"),
            tr("Preferences slot"));

}


void MainWindow::units()
{
   QMessageBox::information(this, tr("Units"),
            tr("Units slot"));

}


void MainWindow::toolbars()
{
   QMessageBox::information(this, tr("Toolbars"),
            tr("Toolbars slot"));

}


void MainWindow::statusbar(bool toggle)
{
	statusBar()->setVisible(toggle);
}


void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    arrangeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = false;//(activeMdiChild() && activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addAction(arrangeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QWidget *> windows = workspace->windowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        DesignChild *child = qobject_cast<DesignChild *>(windows.at(i));

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, child);
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    workspace->addWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));

    return child;
}


DesignChild *MainWindow::createDesignChild()
{
    DesignChild *child = new DesignChild;
    workspace->addWindow(child);

    return child;
}


void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New project"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a project"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newProject()));

    openProjectAct = new QAction(QIcon(":/images/open.png"), tr("&Load project..."), this);
    openProjectAct->setShortcut(tr("Ctrl+L"));
    openProjectAct->setStatusTip(tr("Load an existing project"));
    connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

    insertProjectAct = new QAction(tr("&Insert project..."), this);
    insertProjectAct->setShortcut(tr("Ctrl+I"));
    insertProjectAct->setStatusTip(tr("Insert an existing project"));
    connect(insertProjectAct, SIGNAL(triggered()), this, SLOT(insertProject()));


    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save project"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the project to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the project under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    saveOptionsAct = new QAction(tr("Save options..."), this);
    saveOptionsAct->setStatusTip(tr("Save options"));
    connect(saveOptionsAct, SIGNAL(triggered()), this, SLOT(saveOptions()));


    printAct = new QAction(QIcon(":/images/print.png"), tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print the current foil"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));


    openFileAct = new QAction(tr("&Open file..."), this);
    openFileAct->setShortcut(tr("Ctrl+O"));
    openFileAct->setStatusTip(tr("Open an existing file"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setShortcut(tr("Ctrl+F4"));
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(triggered()),
            workspace, SLOT(closeActiveWindow()));

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(triggered()),
            workspace, SLOT(closeAllWindows()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

    arrangeAct = new QAction(tr("Arrange &icons"), this);
    arrangeAct->setStatusTip(tr("Arrange the icons"));
    connect(arrangeAct, SIGNAL(triggered()), workspace, SLOT(arrangeIcons()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            workspace, SLOT(activateNextWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            workspace, SLOT(activatePreviousWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// Application Actions
    directDesignAct = new QAction(tr("&Foil Direct Design"), this);
    directDesignAct->setStatusTip(tr("Foil direct design"));
    connect(directDesignAct, SIGNAL(triggered()), this, SLOT(directDesign()));

    inverseDesignFullAct = new QAction(tr("&Full Inverse"), this);
    inverseDesignFullAct->setShortcut(tr("Ctrl+3"));
    inverseDesignFullAct->setStatusTip(tr("Full inverse design"));
    connect(inverseDesignFullAct, SIGNAL(triggered()), this, SLOT(inverseDesignFull()));

    inverseDesignMixedAct = new QAction(tr("&Mixed Inverse"), this);
    inverseDesignMixedAct->setShortcut(tr("Ctrl+4"));
    inverseDesignMixedAct->setStatusTip(tr("Mixed inverse design"));
    connect(inverseDesignMixedAct, SIGNAL(triggered()), this, SLOT(inverseDesignMixed()));

    directAnalysisAct = new QAction(tr("XFoil &Direct Analysis"), this);
    directAnalysisAct->setShortcut(tr("Ctrl+5"));
    directAnalysisAct->setStatusTip(tr("XFoil direct analysis"));
    connect(directAnalysisAct, SIGNAL(triggered()), this, SLOT(directAnalysis()));

    wingDesignAct = new QAction(tr("&Wing Design"), this);
    wingDesignAct->setShortcut(tr("Ctrl+6"));
    wingDesignAct->setStatusTip(tr("Wing design"));
    connect(wingDesignAct, SIGNAL(triggered()), this, SLOT(wingDesign()));


	// View Actions
	preferencesAct = new QAction(tr("Preferences..."), this);
    preferencesAct->setStatusTip(tr("Preferences"));
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(preferences()));

	unitsAct = new QAction(tr("Units..."), this);
    unitsAct->setStatusTip(tr("Units"));
    connect(unitsAct, SIGNAL(triggered()), this, SLOT(units()));

	toolbarsAct = new QAction(tr("Toolbars..."), this);
    toolbarsAct->setStatusTip(tr("Toolbars"));
    connect(toolbarsAct, SIGNAL(triggered()), this, SLOT(toolbars()));

    statusbarAct = new QAction(tr("&Status Bar"), this);
	statusbarAct->setCheckable(true);
    statusbarAct->setStatusTip(tr("Turn on/off status bar"));
    connect(statusbarAct, SIGNAL(toggled(bool)), this, SLOT(statusbar(bool)));
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openProjectAct);
	fileMenu->addAction(insertProjectAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
	fileMenu->addAction(saveOptionsAct);
    fileMenu->addSeparator();
	fileMenu->addAction(printAct);
	fileMenu->addAction(openFileAct);
	fileMenu->addSeparator();
    QAction *action = fileMenu->addAction(tr("Switch layout direction"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
    fileMenu->addAction(exitAct);

	appMenu = menuBar()->addMenu(tr("&Application"));
	appMenu->addAction(directDesignAct);
	appInverseDesignMenu = appMenu->addMenu(tr("XFoil &Inverse Design"));
	appInverseDesignMenu->addAction(inverseDesignFullAct);
	appInverseDesignMenu->addAction(inverseDesignMixedAct);
	appMenu->addAction(directAnalysisAct);
	appMenu->addAction(wingDesignAct);

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(preferencesAct);
	viewMenu->addAction(unitsAct);
	viewMenu->addAction(toolbarsAct);
	viewMenu->addAction(statusbarAct);


    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openProjectAct);
    fileToolBar->addAction(saveAct);
	fileToolBar->addAction(printAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
	statusbarAct->setChecked(true);
}

void MainWindow::readSettings()
{
    QSettings settings("XFLR5", "XFLR5QT");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("XFLR5", "XFLR5QT");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

DesignChild *MainWindow::activeMdiChild()
{
    return qobject_cast<DesignChild *>(workspace->activeWindow());
}

MdiChild *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *window, workspace->windowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window);
        if (mdiChild->currentFile() == canonicalFilePath)
            return mdiChild;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}
