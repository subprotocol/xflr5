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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QWorkspace;
class MdiChild;
class DesignChild;
class QSignalMapper;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newProject();
    void openProject();
	void insertProject();
    void save();
    void saveAs();
	void saveOptions();
	void openFile();    
	void cut();
    void copy();
    void paste();
		void print();
    void about();
    void updateMenus();
    void updateWindowMenu();
    MdiChild *createMdiChild();
	DesignChild *createDesignChild();
    void switchLayoutDirection();

	//Application slots
	void directDesign();
	void inverseDesignFull();
	void inverseDesignMixed();
	void directAnalysis();
	void wingDesign();

	// View slots
	void preferences();
	void units();
	void toolbars();
	void statusbar(bool toggle);
private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    DesignChild *activeMdiChild();
    MdiChild *findMdiChild(const QString &fileName);

    QWorkspace *workspace;
    QSignalMapper *windowMapper;

    QMenu *fileMenu;
    QMenu *appMenu;
	QMenu *appInverseDesignMenu;
	QMenu *viewMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
	QAction *openProjectAct;
	QAction *insertProjectAct;
    QAction *openFileAct;
    QAction *saveAct;
    QAction *saveAsAct;
	QAction *saveOptionsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
		QAction *printAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *arrangeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
	
	// Application actions
	QAction *directDesignAct;
	QAction *inverseDesignFullAct;
	QAction *inverseDesignMixedAct;
	QAction *directAnalysisAct;
	QAction *wingDesignAct;

	// View actions
	QAction *preferencesAct;
	QAction *unitsAct;
	QAction *toolbarsAct;
	QAction *statusbarAct;
};

#endif
