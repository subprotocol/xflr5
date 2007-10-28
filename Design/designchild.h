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

#ifndef DESIGNCHILD_H
#define DESIGNCHILD_H

#include <QWidget>
#include "foil.h"
#include "naca4digitfoil.h"

#include "Foil.h"
#include "FoilPath.h"
#include "FoilPointPath.h"
#include "FoilPointPolarPath.h"
#include "FoilInterpolPath.h"
#include "FoilMidLinePath.h"

class DesignChild : public QWidget
{
    Q_OBJECT

public:
    DesignChild();
	virtual ~DesignChild();

    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
		void print(QPrinter *printer);
protected:
    void closeEvent(QCloseEvent *event);
	virtual void paintEvent ( QPaintEvent * event );

private slots:
    void documentWasModified();

private:
		bool foilModified(){return false;}
		bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

 	Foil foil;
	Naca4DigitFoil	naca4Foil;
	Foil *foilPtr;
	FoilPath foilPath;
	FoilPointPath foilPointPath;
	FoilPointPolarPath foilPointPolarPath;
	FoilInterpolPath foilInterpolPath;

    QString curFile;
    bool isUntitled;
	// properties for foil display
	double scale;
	QPoint offset;
};

#endif
