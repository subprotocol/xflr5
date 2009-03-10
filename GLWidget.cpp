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
#include "Miarex/Miarex.h"
#include <math.h>
#include "Graph/Curve.h"
#include "Miarex/GL3dBodyDlg.h"
#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
	m_pParent = parent;
	m_wndTextColor = QColor(200,200,200);
	m_pMiarex = NULL;
	m_iView = 3;
	setMouseTracking(true);
}

void GLWidget::contextMenuEvent (QContextMenuEvent * event)
{
	if(m_iView == 3)
	{
		QMiarex *pMiarex = (QMiarex *)m_pMiarex;
		pMiarex->contextMenuEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->ShowContextMenu(event);
	}
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mousePressEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mousePressEvent(event);
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseReleaseEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseReleaseEvent(event);
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseMoveEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseMoveEvent(event);
	}
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->wheelEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->wheelEvent(event);
	}
}


void GLWidget::initializeGL()
{
	glClearColor(.1, 0.0784, 0.1569, 1.0);
}


void GLWidget::keyPressEvent(QKeyEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyPressEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyPressEvent(event);
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyReleaseEvent(event);
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyReleaseEvent(event);
	}
}


void GLWidget::paintGL()
{
	if(m_iView == 3)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->GLDraw3D();
		pMiarex->GLRenderView();
	}
	else
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pDlg->GLDraw3D();
		pDlg->GLRenderBody();
	}
}


void GLWidget::resizeGL(int width, int height)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->Set3DScale();
	}
	double w, h;
	m_rCltRect = geometry();
	int side = qMax(width, height);
	w = (double)width;
	h = (double)height;

	glViewport((width - side) / 2, (height - side) / 2, side, side);
//	d = qMax(w,h);
//	glViewport(0,0, d, d);

	if(w>h) m_ScaletoGL = 1.0/w;
	else    m_ScaletoGL = 1.0/h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
//	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(w>h)	m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
	else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);
}





