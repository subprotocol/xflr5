/****************************************************************************

	XInverse Class
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



#include <QtGui>

#include "../Globals.h"
#include "../MainFrame.h"
#include "XInverse.h"


QXInverse::QXInverse(QWidget *parent)
	: QWidget(parent)
{
	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	SetupLayout();
}


void QXInverse::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);


//	setLayout(MainLayout);
}



void QXInverse::UpdateView()
{
}


void QXInverse::wheelEvent(QWheelEvent *event)
{
}


void QXInverse::mouseMoveEvent(QMouseEvent *event)
{
}


void QXInverse::mousePressEvent(QMouseEvent *event)
{
}


void QXInverse::mouseReleaseEvent(QMouseEvent *event)
{
}


void QXInverse::keyPressEvent(QKeyEvent *event)
{
}


void QXInverse::keyReleaseEvent(QKeyEvent *event)
{
}


