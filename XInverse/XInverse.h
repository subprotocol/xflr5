/****************************************************************************

	QXInverse Class
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/


#ifndef QXINVERSE_H
#define QXINVERSE_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include "../XDirect/XFoil.h"
#include "XInverse.h"



class QXInverse : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class TwoDWidget;

public:
	QXInverse(QWidget *parent = NULL);
	void SetupLayout();
	void InitDialog();
 
private slots:



private:
	void UpdateView();

	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);


private:
	void * m_p2DWidget;
	void * m_pMainFrame;

	QList<void *> *m_poaFoil;
	XFoil *m_pXFoil;

	CFoil *m_pCurFoil;
	QCursor m_hcArrow;
	QCursor m_hcMove;
	QCursor m_hcCross;


};

#endif // QXINVERSE_H



