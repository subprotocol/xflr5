/****************************************************************************

	TwoDWidget Class

    Copyright (C) 2008-2009 Andre Deperrois XFLR5@yahoo.com

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

#ifndef TwoDWidget_H
#define TwoDWidget_H

#include <QWidget>
#include "Params.h"


class TwoDWidget : public QWidget
{
    Q_OBJECT

	friend class MainFrame;
	friend class QXDirect;
public:
	TwoDWidget(QWidget *parent = 0);
	~TwoDWidget();
	void UpdateView();


protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );
	void resizeEvent ( QResizeEvent * event );
	void paintEvent(QPaintEvent *event);
	void showEvent ( QShowEvent * event );

private:

	QRect m_rCltRect;          // The client window rectangle

	void * m_pXDirect, *m_pMiarex, *m_pAFoil, *m_pXInverse;
	void *m_pMainFrame;

	QCursor m_hcArrow;
	QCursor m_hcCross;
	QCursor m_hcMove;
};

#endif
