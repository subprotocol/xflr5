/****************************************************************************

	GLWidget Class
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
#include <QtOpenGL>
#include "MainFrame.h"
#include <math.h>
#include "Graph/Curve.h"
#include "Miarex/Miarex.h"

#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
	m_wndTextColor = QColor(200,200,200);
	m_iView = XFOILANALYSIS;
	m_pMiarex = NULL;
}

GLWidget::~GLWidget()
{

}


void GLWidget::initializeGL()
{
	glClearColor(0.0, 0.0784, 0.1569, 1.0);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_iView == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mousePressEvent(event);
	}
}
void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseReleaseEvent(event);
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_iView == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseMoveEvent(event);
	}
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	if(m_iView == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->wheelEvent(event);
	}
}

void GLWidget::paintGL()
{
    UpdateView();
}


void GLWidget::resizeGL(int width, int height)
{
	m_rCltRect = geometry();
	int side = qMax(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	double w =(double)width;
	double h = (double)height;

	if(w>h) m_ScaletoGL = 1.0/w;
	else    m_ScaletoGL = 1.0/h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
//	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(w>h)	m_pGLViewRect->SetRect(-s, s*h/w, s, -s*h/w);
	else    m_pGLViewRect->SetRect(-s*w/h, s, s*w/h, -s*h/w);
//qDebug("l=%f   t=%f   r=%f   b=%f",m_pGLViewRect->left, m_pGLViewRect->top, m_pGLViewRect->right,m_pGLViewRect->bottom);

}


void GLWidget::UpdateView()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;

		if(m_iView == MIAREX && pMiarex)
		{
	//		pMiarex->PaintView();
		}
	}
	glPopMatrix();
}




