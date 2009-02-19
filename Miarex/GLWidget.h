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


#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "../Params.h"
#include "Objects/CRectangle.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

	friend class QMiarex;
	friend class GL3dViewDlg;
	friend class GL3dBodyDlg;
	friend class MainFrame;

public:
	GLWidget(QWidget *parent = 0);

	void UpdateView();

private:
	void *m_pParent;


private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();//virtual override

private slots:

private:

	QColor m_wndTextColor;
	QRect m_rCltRect;          // The client window rectangle
	CRectangle m_GLViewRect; // The OpenGl Viewport
	double m_ScaletoGL;        // conversion factor from window client coordinates to GL Viewport coordinates
	void * m_pMiarex;
	bool m_bPlane;

	int m_iView;//Wing=3  Body=5
};

#endif
