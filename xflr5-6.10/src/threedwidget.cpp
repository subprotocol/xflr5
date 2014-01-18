/****************************************************************************

	ThreeDWidget Class
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


#include <QtOpenGL>
#include "mainframe.h"
#include "misc/Settings.h"
#include "miarex/Miarex.h"
#include <math.h>

#include "graph/Curve.h"
#include "miarex/design/GL3dBodyDlg.h"
#include "miarex/design/GL3dWingDlg.h"
#include "misc/W3dPrefsDlg.h"
#include "threedwidget.h"

void *ThreeDWidget::s_pMiarex;
void *ThreeDWidget::s_pMainFrame;

/**
*The public constructor
*/
ThreeDWidget::ThreeDWidget(QWidget *parent)
	: QGLWidget(parent)
{
	m_pParent = parent;
	m_iView = GLMIAREXVIEW;


	setAutoFillBackground(false);

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}


/**
*Overrides the contextMenuEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint ScreenPt = event->globalPos();
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex *pMiarex = (QMiarex *)s_pMiarex;
		if (pMiarex->m_iView==W3DVIEW)
		{
			if(pMiarex->m_pCurWPolar->polarType()==STABILITYPOLAR)
				pMainFrame->W3DStabCtxMenu->exec(ScreenPt);
			else pMainFrame->W3DCtxMenu->exec(ScreenPt);
		}
	}
	else  if(m_iView ==GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->ShowContextMenu(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
//		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
//		pDlg->ShowContextMenu(event);
	}
}


/**
*Overrides the mousePressEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mousePressEvent(event);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mousePressEvent(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MousePressEvent(event);
	}
}


/**
*Overrides the mouseReleaseEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mouseReleaseEvent(event);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseReleaseEvent(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MouseReleaseEvent(event);
	}
}


/**
*Overrides the mouseMoveEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->mouseMoveEvent(event);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->mouseMoveEvent(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->MouseMoveEvent(event);
	}
}


/**
*Overrides the mouseDoubleClickEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->doubleClickEvent(event->pos());
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->doubleClickEvent(event->pos());
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->doubleClickEvent(event->pos());
	}
}


/**
*Overrides the wheelEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::wheelEvent(QWheelEvent *event)
{
	double zoomFactor=1.0;
	if(event->delta()>0)
	{
		if(!Settings::s_bReverseZoom) zoomFactor = 1./1.06;
		else                           zoomFactor = 1.06;
	}
	else
	{
		if(!Settings::s_bReverseZoom) zoomFactor = 1.06;
		else                           zoomFactor = 1./1.06;
	}

	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->zoomEvent(event->pos(), zoomFactor);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->zoomEvent(event->pos(), zoomFactor);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->zoomEvent(event->pos(), zoomFactor);
	}
}


/*void ThreeDWidget::initializeGL()
{
	glClearColor(.1, 0.0784, 0.1569, 1.0);
}*/

/**
*Overrides the keyPressEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::keyPressEvent(QKeyEvent *event)
{
	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->keyPressEvent(event);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyPressEvent(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->keyPressEvent(event);
	}
}


/**
*Overrides the keyReleaseEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(m_iView ==GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->keyReleaseEvent(event);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->keyReleaseEvent(event);
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->keyReleaseEvent(event);
	}
}


/**
*Overrides the paintGL method of the base class.
*Dispatches the handling to the active child application.
*/
void ThreeDWidget::paintGL()
{
	qreal pixelRatio = 1;
#if QT_VERSION >= 0x050000
	pixelRatio = devicePixelRatio();
#endif
	setupViewPort(width() * pixelRatio, height() * pixelRatio);

	if(m_iView==GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->GLDraw3D();
		if(pMiarex->m_iView==W3DVIEW) pMiarex->GLRenderView();
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->GLDraw3D();
		pDlg->GLRenderBody();
	}
	else if(m_iView == GLWINGVIEW)
	{
		GL3dWingDlg *pDlg = (GL3dWingDlg*)m_pParent;
		pDlg->GLDraw3D();
		pDlg->GLRenderView();
	}
}



/**
* Overrides the virtual paintEvent method of the base class.
* Calls the paintGL() method, then overlays the text using the widget's QPainter
* @param event
*/
void ThreeDWidget::paintEvent(QPaintEvent *event)
{
	paintGL();
	QPainter painter(this);
//	painter.setFont(Settings::m_TextFont);
//	painter.setRenderHint(QPainter::Antialiasing);

	if(m_iView==GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->PaintPlaneLegend(painter, rect());
		pMiarex->PaintPlaneOppLegend(painter, rect());
		pMiarex->PaintCpLegendText(painter);
		pMiarex->PaintPanelForceLegendText(painter);
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->PaintBodyLegend(painter);
	}
	else if(m_iView == GLWINGVIEW)
	{
	}
	event->accept();
}



/**
*Overrides the resizeGL method of the base class.
* Sets the GL viewport to fit in the client area.
* Sets the scaling factors for the objects to be drawn in the viewport.
*@param width the width in pixels of the client area
*@param height the height in pixels of the client area
*/
void ThreeDWidget::resizeGL(int width, int height)
{
	double w, h, s;
	m_rCltRect = geometry();
	w = (double)width;
	h = (double)height;
	s = 1.0;

	makeCurrent();

	setupViewPort(width, height);

	glLoadIdentity();
	if(w>h)	m_GLViewRect.SetRect(-s, s*h/w, s, -s*h/w);
	else    m_GLViewRect.SetRect(-s*w/h, s, s*w/h, -s*h/w);

	if(m_iView == GLMIAREXVIEW)
	{
		QMiarex* pMiarex = (QMiarex*)s_pMiarex;
		pMiarex->m_ArcBall.GetMatrix();
		pMiarex->m_bIs3DScaleSet = false;
		pMiarex->Set3DScale();
	}
	else if(m_iView == GLWINGVIEW)
	{
	}
	else if(m_iView == GLBODYVIEW)
	{
		GL3dBodyDlg *pDlg = (GL3dBodyDlg*)m_pParent;
		pDlg->m_bIs3DScaleSet = false;
		pDlg->SetRectangles();
		pDlg->SetBodyScale();
		pDlg->SetBodyLineScale();
		pDlg->SetFrameScale();
	}


/*	QGLFormat fmt = format();
	qDebug()<<"accum"<<fmt.accum();
	qDebug()<<"accumbuffersize"<<fmt.accumBufferSize();
	qDebug()<<"alpha"<<fmt.alpha();
	qDebug()<<"depth"<<fmt.depth();
	qDebug()<<"double bufffer"<<fmt.doubleBuffer();
	qDebug()<<"depth buffer size"<<fmt.depthBufferSize();
	qDebug()<<"directRendering"<<fmt.directRendering();
	qDebug()<<"has overlay"<<fmt.hasOverlay();
	qDebug()<<"majorversion"<<fmt.majorVersion();
	qDebug()<<"minorversion"<<fmt.minorVersion();
	qDebug()<<"hasOpenGLOverlays"<<fmt.hasOpenGLOverlays();
	qDebug()<<"overlay"<<fmt.hasOverlay();
	qDebug()<<"plane"<<fmt.plane();
	qDebug()<<"rgba"<<fmt.rgba();
	qDebug()<<"sampleBuffers"<<fmt.sampleBuffers();
	qDebug()<<"stereo"<<fmt.stereo();
	qDebug()<<"swapInterval"<<fmt.swapInterval();
	qDebug()<<"______________________";*/

}




void ThreeDWidget::setupViewPort(int width, int height)
{
	makeCurrent();
	int side = qMax(width, height);
#ifdef Q_WS_MAC
	glViewport(0,0, width, height);
#else
	glViewport((width - side) / 2, (height - side) / 2, side, side);
#endif

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double s = 1.0;
#ifdef Q_WS_MAC
    glOrtho(-s,s,-(height*s)/width,(height*s)/width,-100.0*s,100.0*s);
#else
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);
#endif
	glMatrixMode(GL_MODELVIEW);
}


/**
*Creates the OpenGL List for the ArcBall.
*@param ArcBall the ArcBall object associated to the view 
*@param GLScale the overall scaling factor for the view @deprecated and unused
*/
void ThreeDWidget::CreateArcballList(ArcBall &ArcBall, double GLScale)
{
	int row, col, NumAngles, NumCircles;
	double R, lat_incr, lon_incr, phi, theta;
	ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	ArcBall.SetZoom(0.45,eye,up);

	glNewList(ARCBALL,GL_COMPILE);
	{
		glColor3d(0.3,0.3,.5);
		glLineWidth(1.0);

		R = ArcBall.ab_sphere;

		NumAngles  = 50;
		NumCircles =  6;
		lat_incr =  90.0 / NumAngles;
		lon_incr = 360.0 / NumCircles;

		for (col = 0; col < NumCircles; col++)
		{
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = (row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				phi = (col * lon_incr ) * PI/180.0;

				for (row = 1; row < NumAngles-1; row++)
				{
					theta = -(row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
				}
			}
			glEnd();
		}


		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=1; col<35; col++)
			{
				phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
				glVertex3d(R * cos(-phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(-phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();

	glNewList(ARCPOINT,GL_COMPILE);
	{
		glColor3d(0.3,0.1,.2);
		glLineWidth(2.0);

		NumAngles  = 10;

		lat_incr = 30.0 / NumAngles;
		lon_incr = 30.0 / NumAngles;

		glBegin(GL_LINE_STRIP);
		{
			phi = 0.0;//longitude

			for (row = -NumAngles; row < NumAngles; row++)
			{
				theta = (row * lat_incr) * PI/180.0;
				glVertex3d(R*cos(phi)*cos(theta)*GLScale, R*sin(theta)*GLScale, R*sin(phi)*cos(theta)*GLScale);
			}
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		{
			theta = 0.;
			for(col=-NumAngles; col<NumAngles; col++)
			{
				phi = (0.0 + (double)col*30.0/NumAngles) * PI/180.0;
				glVertex3d(R * cos(phi) * cos(theta)*GLScale, R * sin(theta)*GLScale, R * sin(phi) * cos(theta)*GLScale);
			}
		}
		glEnd();
	}
	glEndList();
}



/**
* Draws the axis in the OpenGL view
*@param length the length of the axis
*@param color the color to use to draw the axis
*@param style the index of the style to use to draw the axis
*@param width the width to use to draw the axis
*/
void ThreeDWidget::GLDrawAxes(double length, QColor AxisColor, int AxisStyle, int AxisWidth)
{
	double l = .6*length;

	glLineWidth((GLfloat)(AxisWidth));

	glColor3d(AxisColor.redF(),AxisColor.greenF(),AxisColor.blueF());

	glEnable (GL_LINE_STIPPLE);

	if     (AxisStyle == 1) glLineStipple (1, 0xCFCF);
	else if(AxisStyle == 2) glLineStipple (1, 0x6666);
	else if(AxisStyle == 3) glLineStipple (1, 0xFF18);
	else if(AxisStyle == 4) glLineStipple (1, 0x7E66);
	else                    glLineStipple (1, 0xFFFF);

	// X axis____________
	glBegin(GL_LINES);
		glVertex3d(-l, 0.0, 0.0);
		glVertex3d( l, 0.0, 0.0);
	glEnd();
	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,   0.0,   0.0);
		glVertex3d( 0.98*l,  0.015*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,  0.0,    0.0);
		glVertex3d( 0.98*l,-0.015*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//XLabel
	renderText( l, 0.0, 0.0, "X", Settings::s_TextFont);


	// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -l, 0.0);
		glVertex3d(0.0,  l, 0.0);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d( 0.015*l, 0.98*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d(-0.015*l, 0.98*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//Y Label
	renderText( 0.0, l, 0.0, "Y", Settings::s_TextFont);


	// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -l);
		glVertex3d(0.0, 0.0,  l);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d(  0.0,   0.0, 1.0*l);
		glVertex3d( 0.015*l,  0.015*l,  0.98*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,    0.0, 1.0*l);
		glVertex3d(-0.015*l, -0.015*l,  0.98*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//ZLabel
	renderText( 0.0, 0.0, l, "Z", Settings::s_TextFont);

	glDisable (GL_LINE_STIPPLE);
}



/**
Creates a list for a sphere with unit radius
*/
void ThreeDWidget::GLCreateUnitSphere()
{
	double start_lat, start_lon,lat_incr, lon_incr, R;
	double phi1, phi2, theta1, theta2;
	GLdouble u[3], v[3], w[3], n[3];
	int row, col;

	int NumLongitudes, NumLatitudes;
	NumLongitudes = NumLatitudes = 19;

	glNewList(GLLISTSPHERE, GL_COMPILE);
	{
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBegin(GL_TRIANGLES);
//		glColor3d(cr.redF(),cr.greenF(),cr.blueF());

		start_lat = -90;
		start_lon = 0.0;
		R = 1.0;

		lat_incr = 180.0 / NumLatitudes;
		lon_incr = 360.0 / NumLongitudes;

		for (col = 0; col < NumLongitudes; col++)
		{
			phi1 = (start_lon + col * lon_incr) * PI/180.0;
			phi2 = (start_lon + (col + 1) * lon_incr) * PI/180.0;

			for (row = 0; row < NumLatitudes; row++)
			{
				theta1 = (start_lat + row * lat_incr) * PI/180.0;
				theta2 = (start_lat + (row + 1) * lat_incr) * PI/180.0;

				u[0] = R * cos(phi1) * cos(theta1);//x
				u[1] = R * sin(theta1);//y
				u[2] = R * sin(phi1) * cos(theta1);//z

				v[0] = R * cos(phi1) * cos(theta2);//x
				v[1] = R * sin(theta2);//y
				v[2] = R * sin(phi1) * cos(theta2);//z

				w[0] = R * cos(phi2) * cos(theta2);//x
				w[1] = R * sin(theta2);//y
				w[2] = R * sin(phi2) * cos(theta2);//z

				NormalVector(u,v,w,n);

				glNormal3dv(n);
				glVertex3dv(u);
				glVertex3dv(v);
				glVertex3dv(w);

				v[0] = R * cos(phi2) * cos(theta1);//x
				v[1] = R * sin(theta1);//y
				v[2] = R * sin(phi2) * cos(theta1);//z

				NormalVector(u,w,v,n);
				glNormal3dv(n);
				glVertex3dv(u);
				glVertex3dv(w);
				glVertex3dv(v);
			}
		}
		glEnd();

		glDisable(GL_DEPTH_TEST);
	}
	glEndList();
}




/**
*Renders a sphere in the viewport. Used to draw the point masses and the light.
*@param radius the sphere's radius, in IS units
*/
void ThreeDWidget::GLRenderSphere(double radius)
{
	if(radius>0)
	{
		glPushMatrix();
		glScaled(radius, radius, radius);
		glCallList(GLLISTSPHERE);
		glScaled(1./radius, 1./radius, 1./radius);
		glPopMatrix();
	}
}


/**
* Calculates two vectors, using the middle point as the common origin
*/
void ThreeDWidget::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{

	GLdouble v1[3], v2[3], d;
	v1[0] = p3[0] - p1[0];
	v1[1] = p3[1] - p1[1];
	v1[2] = p3[2] - p1[2];
	v2[0] = p3[0] - p2[0];
	v2[1] = p3[1] - p2[1];
	v2[2] = p3[2] - p2[2];

	// calculate the cross product of the two vectors
	n[0] = v1[1] * v2[2] - v2[1] * v1[2];
	n[1] = v1[2] * v2[0] - v2[2] * v1[0];
	n[2] = v1[0] * v2[1] - v2[0] * v1[1];

	// normalize the vector
	d = ( n[0] * n[0] + n[1] * n[1] + n[2] * n[2] );
	// try to catch very small vectors
	if (d < (GLdouble)0.00000001)
	{
		d = (GLdouble)100000000.0;
	}
	else
	{
		d = (GLdouble)1.0 / sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
}

/**
*Initializes the light parameters of the GL Viewport
*@param pglLightParams a pointer to the instance of the GLLightDlg which holds the user-defined settings for the light.
*@param Offset_y
*@param LightFactor a global factor for all light intensities.
*/
void ThreeDWidget::GLSetupLight(double Offset_y, double LightFactor)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // the ambient light
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	float fLightAmbient0[4];
	float fLightDiffuse0[4];
	float fLightSpecular0[4];
	float fLightPosition0[4];

	if(LightFactor>1.0) LightFactor = 1.0f;

	// the ambient light conditions.
	fLightAmbient0[0] = LightFactor*GLLightDlg::s_Ambient * GLLightDlg::s_Red; // red component
	fLightAmbient0[1] = LightFactor*GLLightDlg::s_Ambient * GLLightDlg::s_Green; // green component
	fLightAmbient0[2] = LightFactor*GLLightDlg::s_Ambient * GLLightDlg::s_Blue; // blue component
	fLightAmbient0[3] = 1.0; // alpha

	fLightDiffuse0[0] = LightFactor*GLLightDlg::s_Diffuse * GLLightDlg::s_Red; // red component
	fLightDiffuse0[1] = LightFactor*GLLightDlg::s_Diffuse * GLLightDlg::s_Green; // green component
	fLightDiffuse0[2] = LightFactor*GLLightDlg::s_Diffuse * GLLightDlg::s_Blue; // blue component
	fLightDiffuse0[3] = 1.0; // alpha

	fLightSpecular0[0] = LightFactor*GLLightDlg::s_Specular * GLLightDlg::s_Red; // red component
	fLightSpecular0[1] = LightFactor*GLLightDlg::s_Specular * GLLightDlg::s_Green; // green component
	fLightSpecular0[2] = LightFactor*GLLightDlg::s_Specular * GLLightDlg::s_Blue; // blue component
	fLightSpecular0[3] = 1.0; // alpha

	// And finally, its position

	fLightPosition0[0] = (GLfloat)((GLLightDlg::s_XLight));
	fLightPosition0[1] = (GLfloat)((GLLightDlg::s_YLight + Offset_y));
	fLightPosition0[2] = (GLfloat)((GLLightDlg::s_ZLight));
	fLightPosition0[3] = 1.0; // W (positional light)


	// Enable the basic light
	glLightfv(GL_LIGHT0, GL_AMBIENT,  fLightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  fLightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition0);


	float fMatAmbient[4]   = {GLLightDlg::s_MatAmbient,  GLLightDlg::s_MatAmbient,   GLLightDlg::s_MatAmbient,  1.0f};
	float fMatSpecular[4]  = {GLLightDlg::s_MatSpecular, GLLightDlg::s_MatSpecular,  GLLightDlg::s_MatSpecular, 1.0f};
	float fMatDiffuse[4]   = {GLLightDlg::s_MatDiffuse,  GLLightDlg::s_MatDiffuse,   GLLightDlg::s_MatDiffuse,  1.0f};
	float fMatEmission[4]  = {GLLightDlg::s_MatEmission, GLLightDlg::s_MatEmission,  GLLightDlg::s_MatEmission, 1.0f};

	if(GLLightDlg::s_bColorMaterial)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_AMBIENT);
//		glColorMaterial(GL_FRONT, GL_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_SPECULAR);

	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);

	}
	glMaterialfv(GL_FRONT, GL_SPECULAR,  fMatSpecular);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   fMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   fMatDiffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION,  fMatEmission);
	glMateriali( GL_FRONT, GL_SHININESS, GLLightDlg::s_iMatShininess);

	if(GLLightDlg::s_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(GLLightDlg::s_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(GLLightDlg::s_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(GLLightDlg::s_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(GLLightDlg::s_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                           glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}


/**
*Converts Client coordinates to OpenGL Viewport coordinates.
*@param point the client coordinates.
*@param real the viewport coordinates.
*/
void ThreeDWidget::ClientToGL(QPoint const &point, CVector &real)
{
	//
	// In input, takes the 2D point in screen client area coordinates
	// In output, returns the 2D OpenGL point
	//
	static double h2, w2;
	h2 = (double)geometry().height() /2.0;
	w2 = (double)geometry().width()  /2.0;

	if(w2>h2)
	{
		real.x =  ((double)point.x() - w2) / w2;
		real.y = -((double)point.y() - h2) / w2;
	}
	else
	{
		real.x =  ((double)point.x() - w2) / h2;
		real.y = -((double)point.y() - h2) / h2;
	}
}



/**
*Converts OpenGL Viewport coordinates to client coordinates
*@param real the viewport coordinates.
*@param point the client coordinates.
*/
void ThreeDWidget::GLToClient(CVector const &real, QPoint &point)
{
	//
	//converts an opengl 2D vector to screen client coordinates
	//
	static double dx, dy, h2, w2;

	h2 = m_GLViewRect.height() /2.0;
	w2 = m_GLViewRect.width()  /2.0;

	dx = ( real.x + w2)/2.0;
	dy = (-real.y + h2)/2.0;

	if(w2>h2)
	{
		point.setX((int)(dx * (double)geometry().width()));
		point.setY((int)(dy * (double)geometry().width()));
	}
	else
	{
		point.setX((int)(dx * (double)geometry().height()));
		point.setY((int)(dy * (double)geometry().height()));
	}
}




/**
*Converts OpenGL Viewport coordinates to client coordinates
*@param x the viewport x-coordinate.
*@param y the viewport y-coordinate.
*@param point the client coordinates.
*/
void ThreeDWidget::GLToClient(double const &x, double const &y, QPoint &point)
{
    //
    //converts an opengl 2D vector to screen client coordinates
    //
    static double dx, dy, h2, w2;

    h2 = m_GLViewRect.height() /2.0;
    w2 = m_GLViewRect.width()  /2.0;

    dx = ( x + w2)/2.0;
    dy = (-y + h2)/2.0;

    if(w2>h2)
    {
        point.setX((int)(dx * (double)geometry().width()));
        point.setY((int)(dy * (double)geometry().width()));
    }
    else
    {
        point.setX((int)(dx * (double)geometry().height()));
        point.setY((int)(dy * (double)geometry().height()));
    }
}


