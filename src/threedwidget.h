/****************************************************************************

	GLWidget Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

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


/**
*@file threedwidget.h
*@brief This file implements the declaration of the class ThreeDWidget,
*Used for 3d openGL-based drawing in the central area of the application's MainFrame.
*/

#ifndef THREEDWIDGET_H
#define THREEDWIDGET_H

#include <QGLWidget>
#include "params.h"
#include "objects/CRectangle.h"
#include "objects/ArcBall.h"
#include "misc/GLLightDlg.h"

/** @enum This enumeration lists the different 3D views used in the program, i.e. the view in Miarex, in Body edition and in Wing edition.*/
typedef enum {GLMIAREXVIEW,GLBODYVIEW, GLWINGVIEW} enumGLView;

#define GLLISTSPHERE 531


/**
*@class ThreeDWidget 
* This class is used for 3d OpenGl-based drawing in the central area of the application's MainFrame.
* There are three instances of this class, one attached to the QStackedWidget of the MainFrame, one for Wing edition, and one for Body edition.
* Depending on the active application, this class calls the drawings methods in QMiarex, in the GLBodyDlg or in GLWingDlg.
* All Qt events received by this widget are sent to the child windows for handling.
*/
class ThreeDWidget : public QGLWidget
{
    Q_OBJECT

	friend class QMiarex;
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;
	friend class MainFrame;
	friend class ArcBall;

public:
	ThreeDWidget(QWidget *parent = 0);
	void CreateArcballList(ArcBall &ArcBall, double GLScale);
	void ClientToGL(QPoint const &point, CVector &real);
	void GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth);
	void GLCreateUnitSphere();
	void GLRenderSphere(double radius);
	void GLSetupLight(double Offset_y, double LightFactor);
	void GLToClient(CVector const &real, QPoint &point);
    void GLToClient(double const &x, double const &y, QPoint &point);
    void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);

private:
	void contextMenuEvent (QContextMenuEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void paintGL();//virtual override
	void resizeGL(int width, int height);
	void wheelEvent (QWheelEvent *event );
	void paintEvent(QPaintEvent *event);

	void setupViewPort(int width, int height);

private:
	static void *s_pMiarex;     /**< A void pointer to the instance of the QMiarex widget.*/
	static void *s_pMainFrame;  /**< A void pointer to the instance of the MainFrame widget.*/
	void *m_pParent;            /**< A void pointer to the parent widget. */

	QRect m_rCltRect;           /**< The client window rectangle  */
	CRectangle m_GLViewRect;    /**< The OpenGl viewport.*/

	enumGLView m_iView;         /**< The identification of the type of the calling parent widget*/
};

#endif
