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


#include <QtDebug>
#include <QtGui>
#include <QtOpenGL>
#include "MainFrame.h"
#include "Miarex/Miarex.h" 
#include <math.h>
#include "Graph/Curve.h"
#include "Miarex/GL3dBodyDlg.h"
#include "Miarex/GL3dWingDlg.h"
#include "GLWidget.h"

void *GLWidget::s_pMiarex;
void *GLWidget::s_pMainFrame;

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
	m_pParent = parent;
	m_wndTextColor = QColor(200,200,200);
	m_iView = 3;

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}

void GLWidget::contextMenuEvent (QContextMenuEvent * event)
{
	if(m_iView == 3)
	{
		QMiarex *pMiarex = (QMiarex *)s_pMiarex;
		pMiarex->contextMenuEvent(event);
	}
	else  if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->ShowContextMenu(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->ShowContextMenu(event);
	}
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mousePressEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mousePressEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MousePressEvent(event);
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mouseReleaseEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseReleaseEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MouseReleaseEvent(event);
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mouseMoveEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseMoveEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MouseMoveEvent(event);
	}
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->wheelEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->wheelEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->WheelEvent(event);
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
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->keyPressEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyPressEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->keyPressEvent(event);
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->keyReleaseEvent(event);
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyReleaseEvent(event);
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->keyReleaseEvent(event);
	}
}


void GLWidget::paintGL()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
	if(m_iView == 3)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->GLDraw3D();
		pMiarex->GLRenderView();
	}
	else if(m_iView == 5)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pDlg->GLDraw3D();
		pDlg->GLRenderBody();
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pDlg->GLDraw3D();
		pDlg->GLRenderView();
	}
}


void GLWidget::resizeGL(int width, int height)
{
	double w, h;
	m_rCltRect = geometry();
	int side = qMax(width, height);
	w = (double)width;
	h = (double)height;

	glViewport((width - side) / 2, (height - side) / 2, side, side);
//	d = qMax(w,h);
//	glViewport(0,0, d, d);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
//	glFrustum(-1.0, +1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(w>h)	m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
	else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);

	if(m_iView == 3)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->m_ArcBall.GetMatrix();
		pMiarex->SetScale(m_rCltRect);
		pMiarex->Set3DScale();
	}
	else if(m_iView == 7)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->m_3DWingRect = m_rCltRect;
	}
}

void GLWidget::RenderText(double const &x, double const &y, QString const &strong, QFont const &font, QColor const &color)
{
	glColor3d(color.redF(), color.greenF(), color.blueF());
	renderText(x, y, strong, font);
}




