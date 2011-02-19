/****************************************************************************

	GL3dBodyDlg Class
	Copyright (C) 2009-10 Andre Deperrois xflr5@yahoo.com

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
  
#include "../MainFrame.h"
#include "../GLWidget.h"
#include "../Globals.h"
#include "../Misc/LinePickerDlg.h"
#include "BodyTransDlg.h"
#include "Miarex.h"
#include "BodyScaleDlg.h"
#include "GL3dBodyDlg.h"
#include "GLLightDlg.h"
#include "InertiaDlg.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QHeaderView>
#include <QtDebug>
#include <math.h>

//2D
#define BODYAXIALLINES      1304
#define BODYFRAME		    1305
#define BODYFRAME3D         1306
#define BODYFRAMEGRID		1307
#define BODYLINEGRID		1308

#define BODYPOINTS			1309
#define FRAMEPOINTS			1310
#define BODYOVERLAY			1311

#define BODYFRAMESCALES		1316
#define BODYLINESCALES		1317



void* GL3dBodyDlg::s_pMainFrame;		//pointer to the Frame window
void* GL3dBodyDlg::s_pMiarex;	//pointer to the Miarex Application window
void *GL3dBodyDlg::s_pGLLightDlg;
QPoint GL3dBodyDlg::s_WindowPos=QPoint(0,0);
QSize  GL3dBodyDlg::s_WindowSize=QSize(700, 500);
bool GL3dBodyDlg::s_bWindowMaximized=false;


QList <void*> *GL3dBodyDlg::s_poaBody;

GL3dBodyDlg::GL3dBodyDlg(void *pParent)
{
	setWindowTitle(tr("Body Edition"));
	setWindowFlags(Qt::Window);
//	setSizeGripEnabled(true);
//	setAttribute(Qt::WA_QuitOnClose );

	m_pBody = NULL;

	m_BodyOffset.Set( 0.20, -0.12, 0.0);
	m_FrameOffset.Set(0.80, -0.50, 0.0);
	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_GLList = 0;

	m_ClipPlanePos = 5.0;

	m_NXPoints    = 30;
	m_NHoopPoints = 20;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;

	m_StackSize = 0; //the current stacksize
	m_StackPos  = 0; //the current position of the stack
	m_bResetFrame = true;


	m_BodyScale     = 1.0;
	m_BodyRefScale  = 1.0;
	m_FrameScale    = 1.0;
	m_FrameRefScale = 1.0;

	m_bChanged    = false;
	m_bEnableName = true;

	m_bBodyOverlay        = false;
	m_bResetglBody        = true;//otherwise endless repaint if no body present
	m_bResetglBody2D      = true;//
	m_bResetglBodyPoints  = true;
	m_bResetglBody2D      = true;
	m_bResetglBodyOverlay = true;
	m_bResetglBodyMesh    = true;

	m_bTrans             = false;
	m_bDragPoint         = false;
	m_bArcball           = false;
	m_bCrossPoint        = false;
	m_bSurfaces          = true;
	m_bOutline           = true;
	m_bVLMPanels         = false;
	m_bAxes              = true;
	m_bglLight           = true;
	m_bPickCenter        = false;
	m_bShowLight         = false;
	m_bIs3DScaleSet      = false;

	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_PointDown.setX(0);
	m_PointDown.setY(0);

	memset(MatIn, 0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx    = &m_UFOOffset.x;
	m_ArcBall.m_pOffy    = &m_UFOOffset.y;
	m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy  = &m_glViewportTrans.y;
	m_ArcBall.m_pRect    = &m_rCltRect;


	m_pInsertPoint      = new QAction(tr("Insert"), this);
	m_pRemovePoint      = new QAction(tr("Remove"), this);
	m_pScaleBody        = new QAction(tr("Scale"), this);
	m_pGrid             = new QAction(tr("Grid Setup"), this);
	m_pResetScales      = new QAction(tr("Reset Scales"), this);
	m_pShowCurFrameOnly = new QAction(tr("Show Current Frame Only"), this);
	m_pShowCurFrameOnly->setCheckable(true);
	m_pInertia          = new QAction(tr("Inertia..."), this);

	m_pUndo= new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
	m_pUndo->setStatusTip(tr("Cancels the last modifiction made to the body"));
	connect(m_pUndo, SIGNAL(triggered()), this, SLOT(OnUndo()));

	m_pRedo = new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
	m_pRedo->setStatusTip(tr("Restores the last cancelled modification made to the body"));
	connect(m_pRedo, SIGNAL(triggered()), this, SLOT(OnRedo()));

	m_pExportBodyGeom = new QAction(tr("Export Body Geometry to File"), this);
	connect(m_pExportBodyGeom, SIGNAL(triggered()), this, SLOT(OnExportBodyGeom()));

	m_pExportBodyDef = new QAction(tr("Export Body Definition to File"), this);
	connect(m_pExportBodyDef, SIGNAL(triggered()), this, SLOT(OnExportBodyDef()));

	m_pImportBodyDef = new QAction(tr("Import Body Definition from File"), this);
	connect(m_pImportBodyDef, SIGNAL(triggered()), this, SLOT(OnImportBodyDef()));

	m_pTranslateBody = new QAction(tr("Translate"), this);
	connect(m_pTranslateBody, SIGNAL(triggered()), this, SLOT(OnTranslateBody()));
	SetupLayout();

	connect(m_pInsertPoint,      SIGNAL(triggered()), this, SLOT(OnInsert()));
	connect(m_pRemovePoint,      SIGNAL(triggered()), this, SLOT(OnRemove()));
	connect(m_pScaleBody,        SIGNAL(triggered()), this, SLOT(OnScaleBody()));
	connect(m_pShowCurFrameOnly, SIGNAL(triggered()), this, SLOT(OnShowCurFrameOnly()));
	connect(m_pResetScales,      SIGNAL(triggered()), this, SLOT(OnResetScales()));
	connect(m_pGrid,             SIGNAL(triggered()), this, SLOT(OnGrid()));
	connect(m_pInertia,          SIGNAL(triggered()), this, SLOT(OnInertia()));

	connect(m_pctrlIso, SIGNAL(clicked()),this, SLOT(On3DIso()));
	connect(m_pctrlX, SIGNAL(clicked()),this, SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()),this, SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()),this, SLOT(On3DTop()));
	connect(m_pctrlReset, SIGNAL(clicked()),this, SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()),this, SLOT(On3DPickCenter()));

	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));

	connect(m_pctrlAxes,       SIGNAL(clicked()), this, SLOT(OnAxes()));
	connect(m_pctrlPanels,     SIGNAL(clicked()), this, SLOT(OnPanels()));
	connect(m_pctrlLight,      SIGNAL(clicked()), this, SLOT(OnLight()));
	connect(m_pctrlShowMasses, SIGNAL(clicked()), this, SLOT(OnShowMasses()));
	connect(m_pctrlSurfaces,   SIGNAL(clicked()), this, SLOT(OnSurfaces()));
	connect(m_pctrlOutline,    SIGNAL(clicked()), this, SLOT(OnOutline()));
	connect(m_pctrlFlatPanels, SIGNAL(clicked()), this, SLOT(OnLineType()));
	connect(m_pctrlBSplines,   SIGNAL(clicked()), this, SLOT(OnLineType()));
	connect(m_pctrlBodyStyle,  SIGNAL(clicked()), this, SLOT(OnBodyStyle()));

	connect(m_pctrlBodyName,   SIGNAL(editingFinished()), this, SLOT(OnBodyName()));
	connect(m_pctrlNHoopPanels,SIGNAL(editingFinished()), this, SLOT(OnNURBSPanels()));
	connect(m_pctrlNXPanels,   SIGNAL(editingFinished()), this, SLOT(OnNURBSPanels()));
	connect(m_pctrlXDegree,    SIGNAL(activated(int)), this, SLOT(OnSelChangeXDegree(int)));
	connect(m_pctrlHoopDegree, SIGNAL(activated(int)), this, SLOT(OnSelChangeHoopDegree(int)));

	connect(m_pctrlUndo, SIGNAL(clicked()),this, SLOT(OnUndo()));
	connect(m_pctrlRedo, SIGNAL(clicked()),this, SLOT(OnRedo()));
	connect(m_pFrameDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnFrameCellChanged(QWidget *)));
	connect(m_pPointDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnPointCellChanged(QWidget *)));
	connect(m_pctrlOK, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(m_pctrlCancel, SIGNAL(clicked()),this, SLOT(reject()));


	setMouseTracking(true);
}




void GL3dBodyDlg::ClientToGL(QPoint const &point, CVector &real)
{
	if(!m_pglWidget) return;
	double h2 = (double)m_pglWidget->m_rCltRect.height() /2.0;
	double w2 = (double)m_pglWidget->m_rCltRect.width()  /2.0;

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


void GL3dBodyDlg::FillFrameCell(int iItem, int iSubItem)
{
	MainFrame *pMainFrame = (MainFrame*)(s_pMainFrame);

	QModelIndex ind;

	switch (iSubItem)
	{
		case 0:
		{
			ind = m_pFrameModel->index(iItem, 0, QModelIndex());
			m_pFrameModel->setData(ind, m_pBody->m_FramePosition[iItem].x * pMainFrame->m_mtoUnit);
			break;
		}
		case 1:
		{
			ind = m_pFrameModel->index(iItem, 1, QModelIndex());
			m_pFrameModel->setData(ind, m_pBody->m_FramePosition[iItem].z * pMainFrame->m_mtoUnit);
			break;
		}
		case 2:
		{
			ind = m_pFrameModel->index(iItem, 2, QModelIndex());
			m_pFrameModel->setData(ind, m_pBody->m_xPanels[iItem]);
			break;
		}
		default:
		{
			break;
		}
	}
}


void GL3dBodyDlg::FillFrameDataTable()
{
	if(!m_pBody) return;
	int i;

	m_pFrameModel->setRowCount(m_pBody->m_NStations);

	for(i=0; i<m_pBody->m_NStations; i++)
	{
		FillFrameTableRow(i);
	}
}


void GL3dBodyDlg::FillFrameTableRow(int row)
{
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ind = m_pFrameModel->index(row, 0, QModelIndex());
	m_pFrameModel->setData(ind, m_pBody->m_FramePosition[row].x * pMainFrame->m_mtoUnit);

	ind = m_pFrameModel->index(row, 1, QModelIndex());
	m_pFrameModel->setData(ind, m_pBody->m_FramePosition[row].z * pMainFrame->m_mtoUnit);

	ind = m_pFrameModel->index(row, 2, QModelIndex());
	m_pFrameModel->setData(ind, m_pBody->m_xPanels[row]);
}


void GL3dBodyDlg::FillPointCell(int iItem, int iSubItem)
{
	MainFrame *pMainFrame = (MainFrame*)(s_pMainFrame);
	QString strong;
	QModelIndex ind;

	if(!m_pBody) return;
	int l = m_pBody->m_iActiveFrame;

	switch (iSubItem)
	{
		case 0:
		{
			ind = m_pPointModel->index(iItem, 0, QModelIndex());
			m_pPointModel->setData(ind, m_pBody->m_Frame[l].m_Point[iItem].y * pMainFrame->m_mtoUnit);
			break;
		}
		case 1:
		{
			ind = m_pPointModel->index(iItem, 1, QModelIndex());
			m_pPointModel->setData(ind, m_pBody->m_Frame[l].m_Point[iItem].z*pMainFrame->m_mtoUnit);

			break;
		}
		case 2:
		{
			ind = m_pPointModel->index(iItem, 2, QModelIndex());
			m_pPointModel->setData(ind,m_pBody->m_hPanels[iItem]);
			break;
		}

		default:
		{
			break;
		}
	}
}



void GL3dBodyDlg::FillPointDataTable()
{
	if(!m_pBody) return;
	int i;

	m_pPointModel->setRowCount(m_pBody->m_NSideLines);

	for(i=0; i<m_pBody->m_NSideLines; i++)
	{
		FillPointTableRow(i);
	}
}


void GL3dBodyDlg::FillPointTableRow(int row)
{
	if(!m_pFrame) return;
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ind = m_pPointModel->index(row, 0, QModelIndex());
	m_pPointModel->setData(ind, m_pFrame->m_Point[row].y * pMainFrame->m_mtoUnit);

	ind = m_pPointModel->index(row, 1, QModelIndex());
	m_pPointModel->setData(ind, m_pFrame->m_Point[row].z * pMainFrame->m_mtoUnit);

	ind = m_pPointModel->index(row, 2, QModelIndex());
	m_pPointModel->setData(ind, m_pBody->m_hPanels[row]);
}


void GL3dBodyDlg::GLCreateBodyBezier(CBody *pBody)
{
	int i,j,l;
	l=0;

	QColor color;
	int style, width;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	CFrame *pFrame;
	GLfloat rightpts[MAXBODYFRAMES*30*3];
	GLfloat leftpts[MAXBODYFRAMES*30*3];

	if(!pBody)
		return;

	for(i=0;i<pBody->m_NStations; i++)
	{
		pFrame = pBody->m_Frame+i;
		for(j=0;j<pFrame->m_NPoints; j++)
		{

			rightpts[l+0]= (GLfloat)(pBody->m_FramePosition[i].x);
			rightpts[l+1]= (GLfloat)pFrame->m_Point[j].y;
			rightpts[l+2]= (GLfloat)pFrame->m_Point[j].z;

			l += 3;
		}
	}

	l=0;
	for(i=0;i<pBody->m_NStations; i++)
	{
		pFrame = pBody->m_Frame+i;
		for(j=pFrame->m_NPoints-1;j>=0; j--)
		{

			leftpts[l+0]=  (GLfloat)(pBody->m_FramePosition[i].x);
			leftpts[l+1]= -(GLfloat)pFrame->m_Point[j].y;
			leftpts[l+2]=  (GLfloat)pFrame->m_Point[j].z;

			l += 3;
		}
	}

	glNewList(BODYSURFACES,GL_COMPILE);
	{
		m_GLList++;
//		glShadeModel(GL_FLAT);
		color = pBody->m_BodyColor;
		glColor3d(color.redF(),color.greenF(),color.blueF());

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, pBody->m_NSideLines*3, pBody->m_NStations,
								  0.0, 1.0, 3,                          pBody->m_NSideLines,
				rightpts);

		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

//		glPushMatrix ();
			glEvalMesh2(GL_FILL, 0, 8, 0, 8);
	/*		for (j = 0; j <= 8; j++) {
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)i/15.0, (GLfloat)j/8.0);
				glEnd();
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/15.0);
				glEnd();
			}*/
//		glPopMatrix();

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, pBody->m_NSideLines*3, pBody->m_NStations,
								  0.0, 1.0, 3,                          pBody->m_NSideLines,
				leftpts);

//		glEnable(GL_MAP2_VERTEX_3);
//		glEnable(GL_AUTO_NORMAL);
//		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

//		glPushMatrix ();
			glEvalMesh2(GL_FILL, 0, 8, 0, 8);
	/*		for (j = 0; j <= 8; j++) {
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)i/15.0, (GLfloat)j/8.0);
				glEnd();
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/15.0);
				glEnd();
			}*/
//		glPopMatrix();

//		glFlush();
	}
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
	{
		m_GLList++;

		glLineWidth(pMiarex->m_OutlineWidth);

		color = pMiarex->m_OutlineColor;
		style = pMiarex->m_OutlineStyle;
		width = pMiarex->m_OutlineWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, pBody->m_NSideLines*3, pBody->m_NStations,
								  0.0, 1.0, 3,                       pBody->m_NSideLines,
				rightpts);

		glPolygonMode(GL_FRONT,GL_LINE);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

		glEvalMesh2(GL_LINE, 0, 8, 0, 8);


		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, pBody->m_NSideLines*3, pBody->m_NStations,
								  0.0, 1.0, 3,                          pBody->m_NSideLines,
				leftpts);

		glEvalMesh2(GL_LINE, 0, 8, 0, 8);
	}
	glEndList();
}


void GL3dBodyDlg::GLCreateBody3DSplines(CBody *pBody)
{
	int i,j,k,l;
	int p, style, width, nx, nh;
	double v;

	CVector Point;
	double xinc, hinc, u;
	CVector N, LATB, TALB;
	CVector LA, LB, TA, TB;
	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	nx = qMin(500, m_NXPoints);
	nh = qMax(m_NHoopPoints, 3);

	if(nx*nh>=10000)
	{
		QString strong;
		strong = QString(tr("Resolution cannot exceed %1")).arg(10000);
		QMessageBox::warning(this, tr("Warning"), strong);
		return;
	}

	p = 0;
	for (k=0; k<=nx; k++)
	{
		u = (double)k / (double)nx;
		for (l=0; l<=nh; l++)
		{
			v = (double)l / (double)nh;
			pBody->GetPoint(u,  v, true, m_T[p]);
			p++;
		}
	}

	glNewList(BODYSURFACES,GL_COMPILE);
	{
		m_GLList++;

		color = pBody->m_BodyColor;
		glColor3d(color.redF(),color.greenF(),color.blueF());
		glLineWidth(1.0);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

		//right side first;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];     //	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2];      //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, N.y, N.z);

				glVertex3d(m_T[p].x, m_T[p].y, m_T[p].z);
				glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(m_T[p].x,      m_T[p].y,      m_T[p].z);
					glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}
		//left side next;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];//	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2]; //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, -N.y, N.z);

				glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
				glVertex3d(m_T[p].x,      -m_T[p].y, m_T[p].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, -N.y, N.z);
					glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
					glVertex3d(m_T[p].x,      -m_T[p].y,      m_T[p].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}
	}
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
	{
		m_GLList++;

		glLineWidth(pMiarex->m_OutlineWidth);

		color = pMiarex->m_OutlineColor;
		style = pMiarex->m_OutlineStyle;
		width = pMiarex->m_OutlineWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		xinc = 0.1; hinc=1./(double)(nh-1);
		u=0.0; v = 0.0;

		// sides
		for (i=1; i<pBody->m_NStations-1; i++)
		{
			u = pBody->Getu(pBody->m_FramePosition[i].x);

			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc;
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					pBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.x,Point.y, Point.z);
					v += hinc;
				}
			}
			glEnd();
		}

		//top line
		u=0.0;
		glBegin(GL_LINE_STRIP);
		{
			v = 0.0;
			for (i=0; i<=nh; i++)
			{
				pBody->GetPoint(u,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
				u += xinc;
			}
		}
		glEnd();

		//bot line
		u=0.0;
		glBegin(GL_LINE_STRIP);
		{
			v = 1.0;
			for (i=0; i<=nh; i++)
			{
				pBody->GetPoint(u,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
				u += xinc;
			}
		}
		glEnd();
	}
	glEndList();
}



void GL3dBodyDlg::GLCreateBodyMesh(CBody *pBody)
{
	if(!pBody)
	{
		glNewList(BODYMESHPANELS,GL_COMPILE);
		{
			m_GLList++;
			glEndList();
		}
		return;
	}
	int i,j,k,l;
	int p, style, width, nx, nh;
	double uk, v, dj, dj1, dl1;
	CVector N, LATB, TALB, LA, LB, TA, TB;
	CVector PLA, PLB, PTA, PTB;
	QColor color;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	nx = pBody->m_nxPanels;
	nh = pBody->m_nhPanels;

	if(pBody->m_LineType==1) //LINES
	{
		glNewList(BODYMESHPANELS,GL_COMPILE);
		{
			m_GLList++;

			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable (GL_LINE_STIPPLE);

			color = pMiarex->m_VLMColor;
			style = pMiarex->m_VLMStyle;
			width = pMiarex->m_VLMWidth;

			glColor3d(color.redF(),color.greenF(),color.blueF());

			glLineWidth(1.0);

			for (i=0; i<pBody->m_NStations-1; i++)
			{
				for (j=0; j<pBody->m_xPanels[i]; j++)
				{
					dj  = (double) j   /(double)(pBody->m_xPanels[i]);
					dj1 = (double)(j+1)/(double)(pBody->m_xPanels[i]);

					//body left side
					for (k=0; k<pBody->m_NSideLines-1; k++)
					{
						//build the four corner points of the strips
						PLB.x =  (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLB.y = -(1.0- dj) * pBody->m_Frame[i].m_Point[k].y   -  dj * pBody->m_Frame[i+1].m_Point[k].y;
						PLB.z =  (1.0- dj) * pBody->m_Frame[i].m_Point[k].z   +  dj * pBody->m_Frame[i+1].m_Point[k].z;

						PTB.x =  (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTB.y = -(1.0-dj1) * pBody->m_Frame[i].m_Point[k].y   - dj1 * pBody->m_Frame[i+1].m_Point[k].y;
						PTB.z =  (1.0-dj1) * pBody->m_Frame[i].m_Point[k].z   + dj1 * pBody->m_Frame[i+1].m_Point[k].z;

						PLA.x =  (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLA.y = -(1.0- dj) * pBody->m_Frame[i].m_Point[k+1].y -  dj * pBody->m_Frame[i+1].m_Point[k+1].y;
						PLA.z =  (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].z +  dj * pBody->m_Frame[i+1].m_Point[k+1].z;

						PTA.x =  (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTA.y = -(1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].y - dj1 * pBody->m_Frame[i+1].m_Point[k+1].y;
						PTA.z =  (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].z + dj1 * pBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB;
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<pBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;

								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
					//body right side
					for (k=pBody->m_NSideLines-2; k>=0; k--)
					{
						//build the four corner points of the strips
						PLA.x = (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLA.y = (1.0- dj) * pBody->m_Frame[i].m_Point[k].y   +  dj * pBody->m_Frame[i+1].m_Point[k].y;
						PLA.z = (1.0- dj) * pBody->m_Frame[i].m_Point[k].z   +  dj * pBody->m_Frame[i+1].m_Point[k].z;

						PTA.x = (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTA.y = (1.0-dj1) * pBody->m_Frame[i].m_Point[k].y   + dj1 * pBody->m_Frame[i+1].m_Point[k].y;
						PTA.z = (1.0-dj1) * pBody->m_Frame[i].m_Point[k].z   + dj1 * pBody->m_Frame[i+1].m_Point[k].z;

						PLB.x = (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLB.y = (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].y +  dj * pBody->m_Frame[i+1].m_Point[k+1].y;
						PLB.z = (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].z +  dj * pBody->m_Frame[i+1].m_Point[k+1].z;

						PTB.x = (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTB.y = (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].y + dj1 * pBody->m_Frame[i+1].m_Point[k+1].y;
						PTB.z = (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].z + dj1 * pBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB;
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<pBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;

								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
				}
			}
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
		glNewList(BODYMESHBACK,GL_COMPILE);
		{
			m_GLList++;

			glDisable (GL_LINE_STIPPLE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);

			color = pMainFrame->m_BackgroundColor;
			style = pMiarex->m_VLMStyle;
			width = pMiarex->m_VLMWidth;

			glColor3d(color.redF(), color.greenF(), color.blueF());

			glLineWidth(1.0);

			for (i=0; i<pBody->m_NStations-1; i++)
			{
				for (j=0; j<pBody->m_xPanels[i]; j++)
				{
					dj  = (double) j   /(double)(pBody->m_xPanels[i]);
					dj1 = (double)(j+1)/(double)(pBody->m_xPanels[i]);

					//body left side
					for (k=0; k<pBody->m_NSideLines-1; k++)
					{
						//build the four corner points of the strips
						PLB.x =  (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLB.y = -(1.0- dj) * pBody->m_Frame[i].m_Point[k].y   -  dj * pBody->m_Frame[i+1].m_Point[k].y;
						PLB.z =  (1.0- dj) * pBody->m_Frame[i].m_Point[k].z   +  dj * pBody->m_Frame[i+1].m_Point[k].z;

						PTB.x =  (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTB.y = -(1.0-dj1) * pBody->m_Frame[i].m_Point[k].y   - dj1 * pBody->m_Frame[i+1].m_Point[k].y;
						PTB.z =  (1.0-dj1) * pBody->m_Frame[i].m_Point[k].z   + dj1 * pBody->m_Frame[i+1].m_Point[k].z;

						PLA.x =  (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLA.y = -(1.0- dj) * pBody->m_Frame[i].m_Point[k+1].y -  dj * pBody->m_Frame[i+1].m_Point[k+1].y;
						PLA.z =  (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].z +  dj * pBody->m_Frame[i+1].m_Point[k+1].z;

						PTA.x =  (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTA.y = -(1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].y - dj1 * pBody->m_Frame[i+1].m_Point[k+1].y;
						PTA.z =  (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].z + dj1 * pBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB;
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<pBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;

								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
					//body right side
					for (k=pBody->m_NSideLines-1; k>=0; k--)
					{
						//build the four corner points of the strips
						PLA.x = (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLA.y = (1.0- dj) * pBody->m_Frame[i].m_Point[k].y   +  dj * pBody->m_Frame[i+1].m_Point[k].y;
						PLA.z = (1.0- dj) * pBody->m_Frame[i].m_Point[k].z   +  dj * pBody->m_Frame[i+1].m_Point[k].z;

						PTA.x = (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTA.y = (1.0-dj1) * pBody->m_Frame[i].m_Point[k].y   + dj1 * pBody->m_Frame[i+1].m_Point[k].y;
						PTA.z = (1.0-dj1) * pBody->m_Frame[i].m_Point[k].z   + dj1 * pBody->m_Frame[i+1].m_Point[k].z;

						PLB.x = (1.0- dj) * pBody->m_FramePosition[i].x      +  dj * pBody->m_FramePosition[i+1].x;
						PLB.y = (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].y +  dj * pBody->m_Frame[i+1].m_Point[k+1].y;
						PLB.z = (1.0- dj) * pBody->m_Frame[i].m_Point[k+1].z +  dj * pBody->m_Frame[i+1].m_Point[k+1].z;

						PTB.x = (1.0-dj1) * pBody->m_FramePosition[i].x      + dj1 * pBody->m_FramePosition[i+1].x;
						PTB.y = (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].y + dj1 * pBody->m_Frame[i+1].m_Point[k+1].y;
						PTB.z = (1.0-dj1) * pBody->m_Frame[i].m_Point[k+1].z + dj1 * pBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB;
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<pBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;

								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
				}
			}
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
	}
	else if(pBody->m_LineType==2) //NURBS
	{
		pBody->SetPanelPos();
		p = 0;
		for (k=0; k<=nx; k++)
		{
			uk  = pBody->s_XPanelPos[k];
			for (l=0; l<=nh; l++)
			{
				v = (double)l / (double)(nh);
				pBody->GetPoint(uk,  v, true, m_L[p]);
				p++;
			}
		}
		glNewList(BODYMESHPANELS,GL_COMPILE);
		{
			m_GLList++;

			glDisable (GL_LINE_STIPPLE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			color = pMiarex->m_VLMColor;
			style = pMiarex->m_VLMStyle;
			width = pMiarex->m_VLMWidth;

			glColor3d(color.redF(), color.greenF(), color.blueF());

			glLineWidth(1.0);

			//left side first;
			p=0;

			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x, m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
						glVertex3d(m_L[p].x,      m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}

			//right side next;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x,      -m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, -N.y, N.z);*/
						glVertex3d(m_L[p].x,      -m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
		}
		glEndList();

		glNewList(BODYMESHBACK,GL_COMPILE);
		{
			m_GLList++;

			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glDisable (GL_LINE_STIPPLE);

			color = pMainFrame->m_BackgroundColor;
			style = pMiarex->m_VLMStyle;
			width = pMiarex->m_VLMWidth;

			glColor3d(color.redF(), color.greenF(), color.blueF());

			glLineWidth(1.0);


			//left side first;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x, m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, N.y, N.z);*/
						glVertex3d(m_L[p].x,      m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
			//right side next;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x,      -m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, -N.y, N.z);*/
						glVertex3d(m_L[p].x,      -m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
		}
		glEndList();
	}
}

void GL3dBodyDlg::GLCreateBody3DFlatPanels(CBody *pBody)
{
	int j,k;
	QColor color;
	int style, width;
	CVector P1, P2, P3, P4, N, P1P3, P2P4, Tj, Tjp1;

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	glNewList(BODYSURFACES,GL_COMPILE);
	{
		m_GLList++;

		glDisable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0,1.0);
		glLineWidth(1.0);

		color = pBody->m_BodyColor;

		glColor3d(color.redF(), color.greenF(), color.blueF());

		for (k=0; k<pBody->m_NSideLines-1;k++)
		{
			for (j=0; j<pBody->m_NStations-1;j++)
			{
				Tj.Set(pBody->m_FramePosition[j].x,     0.0, 0.0);
				Tjp1.Set(pBody->m_FramePosition[j+1].x, 0.0, 0.0);

				glBegin(GL_QUADS);
				{
//					P1 = pBody->m_Frame[j].m_Point[k];			P1.Translate(Tj);
//					P2 = pBody->m_Frame[j+1].m_Point[k];		P2.Translate(Tjp1);
//					P3 = pBody->m_Frame[j+1].m_Point[k+1];		P3.Translate(Tjp1);
//					P4 = pBody->m_Frame[j].m_Point[k+1];		P4.Translate(Tj);

					P1 = pBody->m_Frame[j].m_Point[k];			P1.x = pBody->m_FramePosition[j].x;
					P2 = pBody->m_Frame[j+1].m_Point[k];		P2.x = pBody->m_FramePosition[j+1].x;
					P3 = pBody->m_Frame[j+1].m_Point[k+1];		P3.x = pBody->m_FramePosition[j+1].x;
					P4 = pBody->m_Frame[j].m_Point[k+1];		P4.x = pBody->m_FramePosition[j].x;

					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);
				}
				glEnd();
				glBegin(GL_QUADS);
				{
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P2P4 * P1P3;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);
				}
				glEnd();
			}
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pMiarex->m_OutlineColor;
		style = pMiarex->m_OutlineStyle;
		width = pMiarex->m_OutlineWidth;

		glLineWidth(width);

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		for (k=0; k<pBody->m_NSideLines-1;k++)
		{
			for (j=0; j<pBody->m_NStations-1;j++)
			{
				Tj.Set(pBody->m_FramePosition[j].x,     0.0, 0.0);
				Tjp1.Set(pBody->m_FramePosition[j+1].x, 0.0, 0.0);

				glBegin(GL_QUADS);
				{
//					P1 = pBody->m_Frame[j].m_Point[k];			P1.Translate(Tj);
//					P2 = pBody->m_Frame[j+1].m_Point[k];		P2.Translate(Tjp1);
//					P3 = pBody->m_Frame[j+1].m_Point[k+1];		P3.Translate(Tjp1);
//					P4 = pBody->m_Frame[j].m_Point[k+1];		P4.Translate(Tj);

					P1 = pBody->m_Frame[j].m_Point[k];			P1.x = pBody->m_FramePosition[j].x;
					P2 = pBody->m_Frame[j+1].m_Point[k];		P2.x = pBody->m_FramePosition[j+1].x;
					P3 = pBody->m_Frame[j+1].m_Point[k+1];		P3.x = pBody->m_FramePosition[j+1].x;
					P4 = pBody->m_Frame[j].m_Point[k+1];		P4.x = pBody->m_FramePosition[j].x;

					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);
				}
				glEnd();
				glBegin(GL_QUADS);
				{
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void GL3dBodyDlg::GLCreateBodyOverlay()
{
	int k,style, width;

	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QMiarex* pMiarex= (QMiarex*)s_pMiarex;

	if(!m_pBody)
	{
		glNewList(BODYOVERLAY,GL_COMPILE);
		m_GLList++;
		glEndList();
		return;
	}

	glNewList(BODYOVERLAY,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

//		color = m_pBody->m_BodyColor;
		color = QColor(0,255,0);
		style = 0;
		width = 1;
		glLineWidth(width);

		style = 1;
		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		//Middle Line
		glBegin(GL_LINE_STRIP);
		{
			double BodyLength = m_pBody->m_FramePosition[m_pBody->m_NStations-1].x -m_pBody->m_FramePosition[0].x;
			for (k=0; k<m_pBody->m_np;k++)
				glVertex3d(m_pBody->m_x[k] * BodyLength + m_pBody->m_FramePosition[0].x,
						   m_pBody->m_y[k] * BodyLength, 0.0);
		}
		glEnd();

		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}



void GL3dBodyDlg::GLCreateBody2DBodySection()
{
	int i,k,style, width;

	QColor color;

	if(!m_pBody)
	{
		glNewList(BODYAXIALLINES,GL_COMPILE);
		m_GLList++;
		glEndList();
		return;
	}

	glNewList(BODYAXIALLINES,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pBody->m_BodyColor;
		style = 0;
		width = 1;
		glLineWidth(width);

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());


		//Middle Line
		glBegin(GL_LINE_STRIP);
		{
			for (k=0; k<m_pBody->m_NStations;k++)
			{
				glVertex3d(m_pBody->m_FramePosition[k].x,
						   m_pBody->m_FramePosition[k].z,
						   m_pBody->m_FramePosition[k].y);
			}
		}
		glEnd();

		if(m_pBody->m_LineType==1)
		{
			//Top Line
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pBody->m_NStations;k++)
					glVertex3d(m_pBody->m_FramePosition[k].x,
							   m_pBody->m_Frame[k].m_Point[0].z,
							   m_pBody->m_FramePosition[k].y);
			glEnd();

			//Bottom Line
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pBody->m_NStations;k++)
					glVertex3d(m_pBody->m_FramePosition[k].x,
							   m_pBody->m_Frame[k].m_Point[ m_pBody->m_Frame[k].m_NPoints-1].z,
							   m_pBody->m_FramePosition[k].y);
			glEnd();
		}
		else
		{
			CVector Point;
			double xinc, u, v;

			int nh = 50;
			xinc = 1./(double)(nh-1);
			u=0.0; v = 0.0;

			//top line
			u=0.0;
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (i=0; i<=nh; i++)
				{
					m_pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.z, Point.y);
					u += xinc;
				}
			}
			glEnd();

			//bot line
			u=0.0;
			glBegin(GL_LINE_STRIP);
			{
				v = 1.0;
				for (i=0; i<=nh; i++)
				{
					m_pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.z, Point.y);
					u += xinc;
				}
			}
			glEnd();
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void GL3dBodyDlg::GLCreateBodyPoints()
{
	int k,style, width;
	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	if(!m_pBody)
	{
		glNewList(BODYPOINTS,GL_COMPILE); glEndList();
		glNewList(FRAMEPOINTS,GL_COMPILE); glEndList();
		m_GLList+=2;
		return;
	}

	glNewList(BODYPOINTS,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pBody->m_BodyColor;
		style = 0;
		width = 1;
		glLineWidth(width);

		glDisable(GL_LINE_STIPPLE);
		glColor3d(color.redF(), color.greenF(), color.blueF());


		for (k=0; k<m_pBody->m_NStations;k++)
		{
			if(m_pBody->m_iActiveFrame==k)
			{
				glLineWidth(2.0);
				glColor3d(0.,0.,1.0);
			}
			else if(m_pBody->m_iHighlight==k)
			{
				glLineWidth(2.0);
				glColor3d(1.,0.,0.0);
			}
			else
			{
				glLineWidth(1.0);
				glColor3d(color.redF(), color.greenF(), color.blueF());
			}
			glRectd(m_pBody->m_FramePosition[k].x-0.006/m_BodyScale,
			        m_pBody->m_FramePosition[k].z-0.006/m_BodyScale,
			        m_pBody->m_FramePosition[k].x+0.006/m_BodyScale,
			        m_pBody->m_FramePosition[k].z+0.006/m_BodyScale);
		}
	}
	glEndList();

	if(!m_pFrame) return;

	glNewList(FRAMEPOINTS,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pBody->m_BodyColor;
		style = 0;
		width = 1;

		glLineWidth(width);

		glDisable(GL_LINE_STIPPLE);


		for (k=0; k<m_pFrame->m_NPoints;k++)
		{
			if(CFrame::s_iSelect==k)
			{
				glLineWidth(2.0);
				glColor3d(0.0,0.0,1.0);
			}
			else if(m_pFrame->m_iHighlight==k)
			{
				glLineWidth(2.0);
				glColor3d(1.,0.,0.0);
			}
			else
			{
				glLineWidth(1.0);
				glColor3d(color.redF(), color.greenF(), color.blueF());
			}
			glRectd(m_pFrame->m_Point[k].y-0.006/m_FrameScale,
			        m_pFrame->m_Point[k].z-0.006/m_FrameScale,
			        m_pFrame->m_Point[k].y+0.006/m_FrameScale,
			        m_pFrame->m_Point[k].z+0.006/m_FrameScale);
		}
	}
	glEndList();

}


void GL3dBodyDlg::GLCreateBodyFrames()
{
	int j,k;
	CVector Point;
	double xinc, hinc, u, v;
	int nh,style, width;
	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	if(!m_pBody || ! m_pFrame || (m_pBody && m_pBody->m_iActiveFrame<0))
	{
		glNewList(BODYFRAME,GL_COMPILE);
		m_GLList++;
		glEndList();
		return;
	}

	nh = 20;
	xinc = 0.1;
	hinc = 1.0/(double)(nh-1);

	glNewList(BODYFRAME,GL_COMPILE);//create 2D Splines or Lines
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pBody->m_BodyColor;
		style = 0;
		width = 1;

		glLineWidth(width);

		if     (style == 1) glLineStipple (1, 0x1111);
		else if(style == 2) glLineStipple (1, 0x0F0F);
		else if(style == 3) glLineStipple (1, 0x1C47);
		else                glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		if(m_pBody->m_LineType ==2)
		{
			u = m_pBody->Getu(m_pBody->m_FramePosition[m_pBody->m_iActiveFrame].x);

			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.y, Point.z, 0.0);
					v += hinc;
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.y, Point.z, 0.0);
					v += hinc;
				}
			}
			glEnd();
		}
		else
		{
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pFrame->m_NPoints;k++)
					glVertex3d(m_pFrame->m_Point[k].y,
							   m_pFrame->m_Point[k].z,
							   0.0);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pFrame->m_NPoints;k++)
					glVertex3d(-m_pFrame->m_Point[k].y,
								m_pFrame->m_Point[k].z,
								0.0);
			}
			glEnd();
		}

		glDisable(GL_LINE_STIPPLE);

		if(!m_bCurFrameOnly)
		{
			glEnable (GL_LINE_STIPPLE);
			glLineStipple (1, 0x0F0F);
			glLineWidth(1.0);
			for(j=0; j<m_pBody->m_NStations; j++)
			{
				if(j!=m_pBody->m_iActiveFrame)
				{
					if(m_pBody->m_LineType ==2)
					{
						u = m_pBody->Getu(m_pBody->m_FramePosition[j].x);

						glBegin(GL_LINE_STRIP);
						{
							v = 0.0;
							for (k=0; k<nh; k++)
							{
								m_pBody->GetPoint(u,v,true, Point);
								glVertex3d(Point.y, Point.z, 0.0);
								v += hinc;
							}
						}
						glEnd();
						glBegin(GL_LINE_STRIP);
						{
							v = 0.0;
							for (k=0; k<nh; k++)
							{
								m_pBody->GetPoint(u,v,false, Point);
								glVertex3d(Point.y, Point.z, 0.0);
								v += hinc;
							}
						}
						glEnd();
					}
					else
					{
						glBegin(GL_LINE_STRIP);
						{
							for (k=0; k<m_pBody->m_NSideLines;k++)
								glVertex3d(m_pBody->m_Frame[j].m_Point[k].y,
										   m_pBody->m_Frame[j].m_Point[k].z,
										   0.0);
						}
						glEnd();
						glBegin(GL_LINE_STRIP);
						{
							for (k=0; k<m_pBody->m_NSideLines;k++)
								glVertex3d(m_pBody->m_Frame[j].m_Point[k].y,
										   m_pBody->m_Frame[j].m_Point[k].z,
										   0.0);
						}
						glEnd();
					}
				}
			}
			glDisable(GL_LINE_STIPPLE);
		}
	}
	glEndList();


	//create 3D Splines or Lines to overlay on the body
	glNewList(BODYFRAME3D,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = QColor(0,0,255);
		style = 0;
		width = 3;
		glLineWidth(width);

		if     (style == 1) glLineStipple (1, 0x1111);
		else if(style == 2) glLineStipple (1, 0x0F0F);
		else if(style == 3) glLineStipple (1, 0x1C47);
		else                glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());


		if(m_pBody->m_LineType ==2)
		{
			u = m_pBody->Getu(m_pBody->m_FramePosition[m_pBody->m_iActiveFrame].x);

			glBegin(GL_LINE_STRIP);
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc;
				}
			glEnd();
			glBegin(GL_LINE_STRIP);
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc;
				}
			glEnd();
		}
		else
		{
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pFrame->m_NPoints;k++)
					glVertex3d( m_pBody->m_FramePosition[m_pBody->m_iActiveFrame].x,
					            m_pFrame->m_Point[k].y, m_pFrame->m_Point[k].z);
			glEnd();
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pFrame->m_NPoints;k++)
					glVertex3d( m_pBody->m_FramePosition[m_pBody->m_iActiveFrame].x,
					           -m_pFrame->m_Point[k].y, m_pFrame->m_Point[k].z);
			glEnd();
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void GL3dBodyDlg::GLDrawBodyFrameScale()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	QString strong;

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	double unit = m_BodyGridDlg.m_Unit2 * pMainFrame->m_mtoUnit;

	QFontMetrics fm(pMainFrame->m_TextFont);
	double LabelWidth;
	double h2 = (double)m_pglWidget->m_rCltRect.height() /2.0;
	double w2 = (double)m_pglWidget->m_rCltRect.width()  /2.0;
	if(w2>h2) LabelWidth = (double)fm.width("-12.34") / w2;
	else      LabelWidth = (double)fm.width("-12.34") /h2;

	glEnable(GL_DEPTH_TEST);
	glDisable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	glColor3d(pMiarex->m_3DAxisColor.redF(), pMiarex->m_3DAxisColor.greenF(), pMiarex->m_3DAxisColor.blueF());
	glLineWidth(pMiarex->m_3DAxisWidth);

	// Horizontal scale____________
	if(fabs(1.0-m_VerticalSplit)/m_BodyGridDlg.m_Unit2/m_FrameScale<20)
	{
		glBegin(GL_LINES);
		{
			for(i=0; i<fabs(1.0-m_FrameScaledOffset.x)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
			{
				glVertex2d(m_FrameScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale, m_FrameScaledOffset.y+0.00);
				glVertex2d(m_FrameScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale, m_FrameScaledOffset.y-0.02);
			}
			for(i=1; i<fabs(m_FrameScaledOffset.x-m_VerticalSplit)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
			{
				glVertex2d(m_FrameScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale, m_FrameScaledOffset.y+0.00);
				glVertex2d(m_FrameScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale, m_FrameScaledOffset.y-0.02);
			}
		}
		glEnd();

		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
		for(i=0; i<fabs(1.0-m_FrameScaledOffset.x)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg((double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_FrameScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale-LabelWidth/2,
									m_FrameScaledOffset.y-0.04,
									0.0,
									strong,
									pMainFrame->m_TextFont);
		}
		for(i=1; i<fabs(m_FrameScaledOffset.x-(m_VerticalSplit))/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_FrameScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale-LabelWidth/2,
									m_FrameScaledOffset.y-0.04,
									0.0,
									strong,
									pMainFrame->m_TextFont);
		}
	}

	// Vertical scale____________
	if(fabs(m_glTop+1.0)/m_BodyGridDlg.m_Unit2/m_FrameScale<100)
	{
		glBegin(GL_LINES);
		{
			for(i=0; i<fabs(m_glTop-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
			{
				glVertex2d(m_FrameScaledOffset.x+ 0.00, m_FrameScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale);
				glVertex2d(m_FrameScaledOffset.x- 0.02, m_FrameScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale);
			}
			for(i=1; i<fabs(-1.0-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
			{
				glVertex2d(m_FrameScaledOffset.x+ 0.00, m_FrameScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale);
				glVertex2d(m_FrameScaledOffset.x- 0.02, m_FrameScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale);
			}
		}
		glEnd();

		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
		for(i=0; i<fabs(m_glTop-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg((double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_FrameScaledOffset.x- 0.02 -LabelWidth,
									m_FrameScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale,
									0.0,
									strong);
		}
		for(i=1; i<fabs(-1.0-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_FrameScaledOffset.x- 0.02 -LabelWidth,
									m_FrameScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale,
									0.0,
									strong);
		}
	}
}


void GL3dBodyDlg::GLDrawBodyLineScale()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	QString strong;

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	double unit = m_BodyGridDlg.m_Unit * pMainFrame->m_mtoUnit;

	QFontMetrics fm(pMainFrame->m_TextFont);
	double LabelWidth;
	double h2 = (double)m_pglWidget->m_rCltRect.height() /2.0;
	double w2 = (double)m_pglWidget->m_rCltRect.width()  /2.0;
	if(w2>h2) LabelWidth = (double)fm.width("-12.34") / w2;
	else      LabelWidth = (double)fm.width("-12.34") /h2;

	glEnable(GL_DEPTH_TEST);
	glDisable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	glColor3d(pMiarex->m_3DAxisColor.redF(), pMiarex->m_3DAxisColor.greenF(), pMiarex->m_3DAxisColor.blueF());
	glLineWidth(pMiarex->m_3DAxisWidth);
	// Horizontal scale____________

	if(fabs(m_VerticalSplit+1.0)/m_BodyGridDlg.m_Unit/m_BodyScale < 50)
	{
		glBegin(GL_LINES);
		{
			for(i=0;  i<fabs(m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
			{
				glVertex2d(m_BodyScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale, m_BodyScaledOffset.y+0.00);
				glVertex2d(m_BodyScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale, m_BodyScaledOffset.y-0.02);
			}
			for(i=1; i<fabs(m_BodyScaledOffset.x-(-1.0))/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
			{
				glVertex2d(m_BodyScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale,m_BodyScaledOffset.y+0.00);
				glVertex2d(m_BodyScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale,m_BodyScaledOffset.y-0.02);
			}
		}
		glEnd();

		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
		for(i=0; i<fabs(m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
		{
			strong = QString("%1").arg((double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_BodyScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale - LabelWidth/2.0,
									m_BodyScaledOffset.y-0.04,
									0.0,
									strong);
		}
		for(i=1; i<fabs(m_BodyScaledOffset.x-(-1.0))/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_BodyScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale - LabelWidth/2.0,
									m_BodyScaledOffset.y-0.04,
									0.0,
									strong);
		}
	}

	// Vertical scale____________
	if(fabs(m_glTop-m_HorizontalSplit)/m_BodyScale/m_BodyGridDlg.m_Unit<50)
	{
		glColor3d(pMiarex->m_3DAxisColor.redF(), pMiarex->m_3DAxisColor.greenF(), pMiarex->m_3DAxisColor.blueF());
		glBegin(GL_LINES);
		{
			for(i=0; i<fabs(m_glTop-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
			{
				glVertex2d(m_BodyScaledOffset.x+ 0.00, m_BodyScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale);
				glVertex2d(m_BodyScaledOffset.x- 0.02, m_BodyScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale);
			}
			for(i=1; i<fabs(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
			{
				glVertex2d(m_BodyScaledOffset.x+ 0.00, m_BodyScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale);
				glVertex2d(m_BodyScaledOffset.x- 0.02, m_BodyScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale);
			}
		}
		glEnd();
		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
		for(i=0; i<fabs(m_glTop-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
		{
			strong = QString("%1").arg((double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_BodyScaledOffset.x- 0.02 - LabelWidth,
									m_BodyScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale,
									0.0,
									strong);
		}
		for(i=1; i<fabs(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_pglWidget->renderText(m_BodyScaledOffset.x- 0.02 - LabelWidth,
									m_BodyScaledOffset.y-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale,
									0.0,
									strong);
		}
	}
}


void GL3dBodyDlg::GLCreateBodyGrid()
{
	int i;
	double nLines;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	int MaxLines = 150;
	int start = 0;
	if(m_bAxes) start = 1;

	glNewList(BODYFRAMEGRID,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		if(m_bAxes)
		{
			// Frame axis____________
			glColor3d(pMiarex->m_3DAxisColor.redF(), pMiarex->m_3DAxisColor.greenF(), pMiarex->m_3DAxisColor.blueF());
			glLineWidth(pMiarex->m_3DAxisWidth);

			if     (pMiarex->m_3DAxisStyle == 1) glLineStipple (1, 0x1111);
			else if(pMiarex->m_3DAxisStyle == 2) glLineStipple (1, 0x0F0F);
			else if(pMiarex->m_3DAxisStyle == 3) glLineStipple (1, 0x1C47);
			else                                 glLineStipple (1, 0xFFFF);

			glBegin(GL_LINES);
			{
				//vertical line
				glVertex2d(0.0, (-1.0-m_FrameScaledOffset.y)/m_FrameScale);
				glVertex2d(0.0,	(m_glTop-m_FrameScaledOffset.y)   /m_FrameScale);
				//horizontal Line
				glVertex2d((m_VerticalSplit-m_FrameScaledOffset.x)/m_FrameScale, 0.0);
				glVertex2d(( 1.0 -m_FrameScaledOffset.x)          /m_FrameScale, 0.0);
			}
			glEnd();
		}

		// Frame grid____________

		//Main Grid

		glColor3d(m_BodyGridDlg.m_Color2.redF(), m_BodyGridDlg.m_Color2.greenF(), m_BodyGridDlg.m_Color2.blueF());
		glLineWidth(m_BodyGridDlg.m_Width2);

		if     (m_BodyGridDlg.m_Style2 == 1) glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_Style2 == 2) glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_Style2 == 3) glLineStipple (1, 0x1C47);
		else								 glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bGrid2)
			{
				//Main X Grid
				nLines = (1.0 - m_VerticalSplit)/m_BodyGridDlg.m_Unit2/m_FrameScale;
				if(nLines < MaxLines)
				{
					for(i=start; i<fabs(1.0-m_FrameScaledOffset.x)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
					{
						glVertex2d(+(double)i*m_BodyGridDlg.m_Unit2,
								   (-1.0-m_FrameScaledOffset.y)    /m_FrameScale);
						glVertex2d(+(double)i*m_BodyGridDlg.m_Unit2,
								   (m_glTop-m_FrameScaledOffset.y) /m_FrameScale);
					}
					for(i=start; i<fabs(m_FrameScaledOffset.x-m_VerticalSplit)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
					{
						glVertex2d((-(double)i*m_BodyGridDlg.m_Unit2),
								   (-1.0-m_FrameScaledOffset.y)    /m_FrameScale);
						glVertex2d((-(double)i*m_BodyGridDlg.m_Unit2),
								   (m_glTop-m_FrameScaledOffset.y) /m_FrameScale);
					}
				}
				//Main Y Grid
				nLines = (m_glTop+1.0)/m_BodyGridDlg.m_Unit2/m_FrameScale;
				if(nLines < MaxLines)
				{
					for(i=start; i<fabs(m_glTop-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
					{
						glVertex2d((m_VerticalSplit-m_FrameScaledOffset.x)/m_FrameScale,
								   +(double)i*m_BodyGridDlg.m_Unit2);
						glVertex2d(( 1.0 -m_FrameScaledOffset.x)          /m_FrameScale,
								   +(double)i*m_BodyGridDlg.m_Unit2);
					}

					for(i=start; i<fabs(-1.0-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
					{
						glVertex2d((m_VerticalSplit-m_FrameScaledOffset.x)/m_FrameScale,
								   -(double)i*m_BodyGridDlg.m_Unit2);
						glVertex2d(( 1.0 -m_FrameScaledOffset.x)          /m_FrameScale,
								   -(double)i*m_BodyGridDlg.m_Unit2);
					}
				}
			}
		}
		glEnd();

		//Minor Grid
		glColor3d(m_BodyGridDlg.m_MinColor2.redF(), m_BodyGridDlg.m_MinColor2.greenF(), m_BodyGridDlg.m_MinColor2.blueF());
		glLineWidth(m_BodyGridDlg.m_MinWidth2);

		if     (m_BodyGridDlg.m_MinStyle2 == 1) glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_MinStyle2 == 2) glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_MinStyle2 == 2) glLineStipple (1, 0x1C47);
		else									glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bMinGrid2)
			{
				//Minor X Grid
				nLines = (1.0 - m_VerticalSplit)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale;
				if(nLines < MaxLines)
				{
					for(i=start; i<fabs(1.0-m_FrameScaledOffset.x)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
					{
						glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit2,
								   (-1.0-m_FrameScaledOffset.y)/m_FrameScale);
						glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit2,
								   (m_glTop-m_FrameScaledOffset.y)     /m_FrameScale);
					}
					for(i=start; i<fabs(m_FrameScaledOffset.x-(m_VerticalSplit))/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
					{
						glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit2),
								   (-1.0-m_FrameScaledOffset.y)  /m_FrameScale);
						glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit2),
								   (m_glTop-m_FrameScaledOffset.y) /m_FrameScale);
					}
				}
				//Minor Y Grid
				nLines = (m_glTop+1.0)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale;
				if(nLines < MaxLines)
				{
					for(i=start; i<fabs(m_glTop-m_FrameScaledOffset.y)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
					{
						glVertex2d((m_VerticalSplit-m_FrameScaledOffset.x)/m_FrameScale,
								   +(double)i*m_BodyGridDlg.m_MinorUnit2);
						glVertex2d(( 1.0 -m_FrameScaledOffset.x)          /m_FrameScale,
								   +(double)i*m_BodyGridDlg.m_MinorUnit2);
					}

					for(i=start; i<fabs(-1.0-m_FrameScaledOffset.y)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
					{
						glVertex2d((m_VerticalSplit-m_FrameScaledOffset.x)/m_FrameScale,
								   -(double)i*m_BodyGridDlg.m_MinorUnit2);
						glVertex2d(( 1.0 -m_FrameScaledOffset.x)          /m_FrameScale,
								   -(double)i*m_BodyGridDlg.m_MinorUnit2);
					}
				}
			}
		}
		glEnd();

		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();

	glNewList(BODYLINEGRID,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		if(m_bAxes)
		{
			// Frame axis____________
			glColor3d(pMiarex->m_3DAxisColor.redF(), pMiarex->m_3DAxisColor.greenF(), pMiarex->m_3DAxisColor.blueF());
			glLineWidth(pMiarex->m_3DAxisWidth);

			if     (pMiarex->m_3DAxisStyle == 1) glLineStipple (1, 0x1111);
			else if(pMiarex->m_3DAxisStyle == 2) glLineStipple (1, 0x0F0F);
			else if(pMiarex->m_3DAxisStyle == 3) glLineStipple (1, 0x1C47);
			else								 glLineStipple (1, 0xFFFF);

			// BodyLine axis____________

			glBegin(GL_LINES);
			{
				//horizontal Line
				glVertex2d((m_VerticalSplit- m_BodyScaledOffset.x)/m_BodyScale, 0.0);
				glVertex2d((-1.0           - m_BodyScaledOffset.x)/m_BodyScale, 0.0);
				//vertical Line
				glVertex2d(0.0,	(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale);
				glVertex2d(0.0,	(m_glTop          -m_BodyScaledOffset.y)/m_BodyScale);
			}
			glEnd();
		}

		// BodyLine grid____________

		//Main Grid
		glColor3d(m_BodyGridDlg.m_Color.redF(), m_BodyGridDlg.m_Color.greenF(), m_BodyGridDlg.m_Color.blueF());
		glLineWidth(m_BodyGridDlg.m_Width);

		if     (m_BodyGridDlg.m_Style == 1) glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_Style == 2) glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_Style == 3) glLineStipple (1, 0x1C47);
		else								glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bGrid)
			{
				//Main X Grid
				nLines = (m_VerticalSplit+1.0)/m_BodyScale/m_BodyGridDlg.m_Unit;
				if(nLines<MaxLines)
				{
					for(i=start; i<fabs(m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
					{
						glVertex2d(+(double)i*m_BodyGridDlg.m_Unit, (m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale);
						glVertex2d(+(double)i*m_BodyGridDlg.m_Unit, (m_glTop-m_BodyScaledOffset.y)          /m_BodyScale);
					}
					for(i=start; i<fabs(m_BodyScaledOffset.x-(-1.0))/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
					{
						glVertex2d((-(double)i*m_BodyGridDlg.m_Unit),(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale);
						glVertex2d((-(double)i*m_BodyGridDlg.m_Unit),(m_glTop-m_BodyScaledOffset.y)          /m_BodyScale);
					}
				}
				//Main Y Grid

				nLines = (m_glTop-m_HorizontalSplit)/m_BodyScale/m_BodyGridDlg.m_Unit;
				if(nLines<MaxLines)
				{
					for(i=start; i<fabs(m_glTop-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
					{
						glVertex2d((m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyScale, +(double)i*m_BodyGridDlg.m_Unit);
						glVertex2d((-1.0 -m_BodyScaledOffset.x)          /m_BodyScale, +(double)i*m_BodyGridDlg.m_Unit);
					}

					for(i=start; i<fabs(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
					{
						glVertex2d((m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyScale, -(double)i*m_BodyGridDlg.m_Unit);
						glVertex2d((-1.0 -m_BodyScaledOffset.x)          /m_BodyScale, -(double)i*m_BodyGridDlg.m_Unit);
					}
				}
			}
		}
		glEnd();

		//Minor Grid
		glColor3d(m_BodyGridDlg.m_MinColor.redF(), m_BodyGridDlg.m_MinColor.greenF(), m_BodyGridDlg.m_MinColor.blueF());

		glLineWidth(m_BodyGridDlg.m_MinWidth);

		if     (m_BodyGridDlg.m_MinStyle == 1) glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_MinStyle == 2) glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_MinStyle == 3) glLineStipple (1, 0x1C47);
		else								   glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bMinGrid)
			{
				//Minor X Grid
				nLines = (m_VerticalSplit+1.0)/m_BodyScale/m_BodyGridDlg.m_MinorUnit;
				if(nLines<MaxLines)
				{
					for(i=start; i<fabs(m_VerticalSplit-m_BodyScaledOffset.x)/m_BodyScale/m_BodyGridDlg.m_MinorUnit; i++)
					{
						glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit,(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale);
						glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit,(m_glTop          -m_BodyScaledOffset.y)/m_BodyScale);
					}
					for(i=start; i<fabs(m_BodyScaledOffset.x-(-1.0))/m_BodyScale/m_BodyGridDlg.m_MinorUnit; i++)
					{
						glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit),(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale);
						glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit),(m_glTop          -m_BodyScaledOffset.y)/m_BodyScale);
					}
				}
				//Minor Y Grid

				nLines = (m_glTop-m_HorizontalSplit)/m_BodyScale/m_BodyGridDlg.m_MinorUnit;
				if(nLines<MaxLines)
				{
					for(i=start; i<fabs(m_glTop-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_MinorUnit; i++)
					{
						glVertex2d((m_VerticalSplit -m_BodyScaledOffset.x)/m_BodyScale, +(double)i*m_BodyGridDlg.m_MinorUnit);
						glVertex2d((-1.0            -m_BodyScaledOffset.x)/m_BodyScale, +(double)i*m_BodyGridDlg.m_MinorUnit);
					}

					for(i=start; i<fabs(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_MinorUnit; i++)
					{
						glVertex2d((m_VerticalSplit -m_BodyScaledOffset.x)/m_BodyScale,  -(double)i*m_BodyGridDlg.m_MinorUnit);
						glVertex2d((-1.0            -m_BodyScaledOffset.x)/m_BodyScale, -(double)i*m_BodyGridDlg.m_MinorUnit);
					}
				}
			}
		}
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();
}




void GL3dBodyDlg::GLCreateCtrlPts()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	glNewList(VLMCTRLPTS,GL_COMPILE);
	{
		m_GLList++;
//		glPolygonMode(GL_FRONT,GL_FILL);
		glLineWidth(1.0);
		glColor3d(0.0,1.0,0.0);
		for (int p=0; p<pMiarex->m_MatSize; p++)
		{
			//All panels
			glBegin(GL_LINES);
			{
				glVertex3d(m_pPanel[p].CtrlPt.x, m_pPanel[p].CtrlPt.y, m_pPanel[p].CtrlPt.z);
				glVertex3d((m_pPanel[p].CtrlPt.x + m_pPanel[p].Normal.x * 0.04),
						   (m_pPanel[p].CtrlPt.y + m_pPanel[p].Normal.y * 0.04),
						   (m_pPanel[p].CtrlPt.z + m_pPanel[p].Normal.z * 0.04));
/*				glVertex3d(m_pPanel[p].CollPt.x, m_pPanel[p].CollPt.y, m_pPanel[p].CollPt.z);
				glVertex3d(m_pPanel[p].CollPt.x+m_pPanel[p].Normal.x *0.01,
						   m_pPanel[p].CollPt.y+m_pPanel[p].Normal.y *0.01,
						   m_pPanel[p].CollPt.z+m_pPanel[p].Normal.z *0.01);
				glVertex3d(m_pPanel[p].CollPt.x, m_pPanel[p].CollPt.y, m_pPanel[p].CollPt.z);
				glVertex3d(m_pPanel[p].CollPt.x+m_pPanel[p].l.x *0.01,
						   m_pPanel[p].CollPt.y+m_pPanel[p].l.y *0.01,
						   m_pPanel[p].CollPt.z+m_pPanel[p].l.z *0.01);*/
			}
			glEnd();
		}
	}
	glEndList();
}





void GL3dBodyDlg::GLDrawAxes()
{
	double l = .8;
//	if(m_pWing) l=1.1*m_pWing->m_Span/2.0;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth((GLfloat)(pMiarex->m_3DAxisWidth));

	glColor3d(pMiarex->m_3DAxisColor.redF(),pMiarex->m_3DAxisColor.greenF(),pMiarex->m_3DAxisColor.blueF());

// X axis____________
	glEnable (GL_LINE_STIPPLE);
	if(pMiarex->m_3DAxisStyle == 1)     glLineStipple (1, 0x1111);
	else if(pMiarex->m_3DAxisStyle== 2) glLineStipple (1, 0x0F0F);
	else if(pMiarex->m_3DAxisStyle== 3) glLineStipple (1, 0x1C47);
	else                                glLineStipple (1, 0xFFFF);// Solid


	glBegin(GL_LINES);
		glVertex3d(-.8, 0.0, 0.0);
		glVertex3d( .8, 0.0, 0.0);
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
	m_pglWidget->renderText( .8, 0.0, 0.0, "X");


// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -.8, 0.0);
		glVertex3d(0.0,  .8, 0.0);
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
	m_pglWidget->renderText( 0.0, .8, 0.0, "Y");

// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -.8);
		glVertex3d(0.0, 0.0,  .8);
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
	m_pglWidget->renderText( 0.0, 0.0, .8, "Z");

	glDisable (GL_LINE_STIPPLE);
}


void GL3dBodyDlg::GLDraw3D()
{
//	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;

	glClearColor(pMainFrame->m_BackgroundColor.redF(), pMainFrame->m_BackgroundColor.greenF(), pMainFrame->m_BackgroundColor.blueF(),0.0);

	if((m_bResetglBody2D || m_bResetglBodyPoints) && m_pBody)
	{
		if(glIsList(BODYPOINTS))
		{
			glDeleteLists(BODYPOINTS,2);
			m_GLList -=2;
		}
		GLCreateBodyPoints();
		m_bResetglBodyPoints = false;
	}

	if(m_bResetglBodyOverlay && m_pBody)
	{
		if(glIsList(BODYOVERLAY))
		{
			glDeleteLists(BODYOVERLAY,1);
			m_GLList -=1;
		}

		GLCreateBodyOverlay();
		m_bResetglBodyOverlay = false;
	}

	if(m_bResetglBody2D && m_pBody)
	{
		if(glIsList(BODYAXIALLINES))
		{
			glDeleteLists(BODYAXIALLINES,5);
			m_GLList -=5;
		}

		GLCreateBody2DBodySection();
		GLCreateBodyFrames();
		GLCreateBodyGrid();
		m_bResetglBody2D = false;
	}
	if(m_bResetglBody )
	{
		int row, col, NumAngles, NumCircles;
		double R, lat_incr, lon_incr, phi, theta;
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.3,eye,up);

		if(glIsList(ARCBALL))
		{
			glDeleteLists(ARCBALL,2);
			m_GLList-=2;
		}
		glNewList(ARCBALL,GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

			glColor3d(0.3, 0.3, 0.5);
			glLineWidth(1.0);

			R = m_ArcBall.ab_sphere;

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
						glVertex3d(R*cos(phi)*cos(theta), R*sin(theta), R*sin(phi)*cos(theta));
					}
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr ) * PI/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = -(row * lat_incr) * PI/180.0;
						glVertex3d(R*cos(phi)*cos(theta), R*sin(theta), R*sin(phi)*cos(theta));
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
					glVertex3d(R * cos(phi) * cos(theta), R * sin(theta), R * sin(phi) * cos(theta));
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
					glVertex3d(R * cos(-phi) * cos(theta), R * sin(theta), R * sin(-phi) * cos(theta));
				}
			}
			glEnd();
		}
		glEndList();

		glNewList(ARCPOINT,GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT,GL_LINE);

			glColor3d(0.3, 0.1, 0.2);
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
					glVertex3d(R*cos(phi)*cos(theta), R*sin(theta), R*sin(phi)*cos(theta));
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=-NumAngles; col<NumAngles; col++)
				{
					phi = (0.0 + (double)col*30.0/NumAngles) * PI/180.0;
					glVertex3d(R * cos(phi) * cos(theta), R * sin(theta), R * sin(phi) * cos(theta));
				}
			}
			glEnd();
		}
		glEndList();
	}

	if(m_bResetglBody && m_pBody)
	{
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.3,eye,up);

		if(glIsList(BODYGEOM))
		{
			glDeleteLists(BODYGEOM,2);
			m_GLList -=2;
		}
		if(m_pBody->m_LineType==1)	    GLCreateBody3DFlatPanels(m_pBody);
		else if(m_pBody->m_LineType==2)	GLCreateBody3DSplines(m_pBody);

		m_bResetglBody = false;
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
		GLCreateBodyMesh(m_pBody);
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBodyMesh && m_pBody)
	{
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
		GLCreateBodyMesh(m_pBody);
		m_bResetglBodyMesh = false;
	}
}


void GL3dBodyDlg::GLDrawBodyLegend()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strLengthUnit;

	int style, width;
	QColor color;

	GetLengthUnit(strLengthUnit, pMainFrame->m_LengthUnit);

//	glNewList(BODYLEGEND,GL_COMPILE);//create 2D Splines or Lines

	//draw view rectangles

	glEnable(GL_DEPTH_TEST);
	glEnable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	color = pMainFrame->m_TextColor;
	style = 0;
	width = 3;

	glLineWidth((float)width);
	glLineStipple (1, 0xFFFF);// Solid
	glColor3d(color.redF(),color.greenF(),color.blueF());

	glBegin(GL_LINES);
		glVertex2d(           -1.0, m_HorizontalSplit);
		glVertex2d(m_VerticalSplit, m_HorizontalSplit);
	glEnd();

	glBegin(GL_LINES);
		glVertex2d(m_VerticalSplit, m_pglWidget->m_GLViewRect.bottom);
		glVertex2d(m_VerticalSplit, m_pglWidget->m_GLViewRect.top );
	glEnd();


	if(m_pBody)
	{
		//draw the legend
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		// Draw the labels
		CVector real;
		ClientToGL(m_MousePos, real);
		QFontMetrics fm(pMainFrame->m_TextFont);
		int dD = fm.height();

		strong = QString(tr("Frame %1")).arg(m_pBody->m_iActiveFrame+1,2);
		m_pglWidget->renderText(m_FrameRect.left() +dD ,dD,strong, pMainFrame->m_TextFont);

		strong = QString(tr("Scale = %1")).arg(m_FrameScale/m_BodyRefScale,4,'f',2);
		m_pglWidget->renderText(m_FrameRect.left() +dD ,2*dD,strong, pMainFrame->m_TextFont);

		strong = QString(tr("Scale = %1")).arg(m_BodyScale/m_BodyRefScale,4,'f',2);
		m_pglWidget->renderText(m_BodyLineRect.left() +dD ,dD,strong, pMainFrame->m_TextFont);

		if(m_FrameRect.contains(m_MousePos))
		{
			real.x =  (real.x - m_FrameScaledOffset.x)/m_FrameScale;
			real.y =  (real.y - m_FrameScaledOffset.y)/m_FrameScale;
			real.z = 0.0;

			strong = QString("y = %1 ").arg(real.x * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_pglWidget->renderText(m_FrameRect.left() +dD ,3*dD,strong, pMainFrame->m_TextFont);

			strong = QString("z = %1 ").arg(real.y * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_pglWidget->renderText(m_FrameRect.left() +dD ,4*dD,strong, pMainFrame->m_TextFont);
		}
		else if(m_BodyLineRect.contains(m_MousePos))
		{
			real.x =  (real.x - m_BodyScaledOffset.x)/m_BodyScale;
			real.y =  (real.y - m_BodyScaledOffset.y)/m_BodyScale;
			real.z = 0.0;

			strong = QString("x = %1 ").arg(real.x * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_pglWidget->renderText(m_BodyLineRect.left() +dD ,2*dD,strong, pMainFrame->m_TextFont);

			strong = QString("z = %1 ").arg(real.y * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_pglWidget->renderText(m_BodyLineRect.left() +dD ,3*dD,strong, pMainFrame->m_TextFont);
		}
	}
}



void GL3dBodyDlg::GLInverseMatrix()
{
	//Step 1. Transpose the 3x3 rotation portion of the 4x4 matrix to get the inverse rotation
	int i,j;

	for(i=0 ; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			MatOut[j][i] = MatIn[i][j];
		}
	}
}

void GL3dBodyDlg::GLRenderBody()
{
	int width;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	GLdouble pts[4];

	pts[0]= 1.0; pts[1]=0.0; pts[2]=0.0; pts[3]=-m_VerticalSplit;		//x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE1, pts);

	pts[0]=0.0; pts[1]= 1.0; pts[2]=0.0; pts[3]=-m_HorizontalSplit;	//y=m_HorizontalSplit
	glClipPlane(GL_CLIP_PLANE2, pts);

	pts[0]=-1.0; pts[1]=0.0; pts[2]=0.0; pts[3]=m_VerticalSplit;		//x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE3, pts);

	pts[0]=0.0; pts[1]=-1.0; pts[2]=0.0; pts[3]=m_HorizontalSplit;	//y=m_HorizontalSplit
	glClipPlane(GL_CLIP_PLANE4, pts);

	pts[0]= 0.0; pts[1]=0.0; pts[2]=-1.0; pts[3]= m_ClipPlanePos;
	glClipPlane(GL_CLIP_PLANE5, pts);

	width = m_rCltRect.width();

	glPushMatrix();
	{
		glFlush();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//Display 2D View
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);
		glDisable(GL_CLIP_PLANE5);
		GLDrawBodyLegend();

		if(m_BodyGridDlg.m_bScale) GLDrawBodyLineScale();

		glEnable(GL_CLIP_PLANE3);
		glEnable(GL_CLIP_PLANE2);


		glPushMatrix();
		{
			glTranslated(m_BodyOffset.x, m_BodyOffset.y, 0.0);

			glTranslated(-m_BodyOffset.x + m_BodyScalingCenter.x, -m_BodyOffset.y + m_BodyScalingCenter.y, 0.0);
			glScaled(m_BodyScale, m_BodyScale,0.0);
			glTranslated(+m_BodyOffset.x + -m_BodyScalingCenter.x, +m_BodyOffset.y + -m_BodyScalingCenter.y, 0.0);

			glCallList(BODYLINEGRID);
			glCallList(BODYPOINTS);
			glCallList(BODYAXIALLINES);
			if(m_bBodyOverlay) glCallList(BODYOVERLAY);
		}
		glPopMatrix();

		
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);

		if(m_pFrame)
		{
			if(m_BodyGridDlg.m_bScale) GLDrawBodyFrameScale();
			glEnable(GL_CLIP_PLANE1);
			glPushMatrix();
			{
				glTranslated(m_FrameOffset.x, m_FrameOffset.y, 0.0);

				glTranslated(-m_FrameOffset.x +  m_FrameScalingCenter.x, -m_FrameOffset.y + m_FrameScalingCenter.y, 0.0);
				glScaled(m_FrameScale, m_FrameScale,0.0);
				glTranslated(+m_FrameOffset.x + -m_FrameScalingCenter.x, +m_FrameOffset.y + -m_FrameScalingCenter.y, 0.0);

				glCallList(FRAMEPOINTS);
				glCallList(BODYFRAME);
				glCallList(BODYFRAMEGRID);
			}
			glPopMatrix();
		}
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);

		glEnable(GL_CLIP_PLANE3);
		glEnable(GL_CLIP_PLANE4);
		if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE5);
		else						glEnable(GL_CLIP_PLANE5);

		glPushMatrix();
		{
			GLSetupLight();

			glLoadIdentity();

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

			if(m_bCrossPoint && m_bArcball)
			{
				glPushMatrix();
				{
					glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);
					m_ArcBall.RotateCrossPoint();
					glRotated(m_ArcBall.angle, m_ArcBall.p.x, m_ArcBall.p.y, m_ArcBall.p.z);
					glCallList(ARCPOINT);
				}
				glPopMatrix();
			}
			if(m_bArcball)
			{
				glPushMatrix();
				{
					glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);
					m_ArcBall.Rotate();
					glCallList(ARCBALL);
				}
				glPopMatrix();
			}

			glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);

			m_ArcBall.Rotate();

			glScaled(m_glScaled, m_glScaled, m_glScaled);
			glTranslated(m_glRotCenter.x, m_glRotCenter.y, m_glRotCenter.z);

			if(m_bAxes)  GLDrawAxes();


			if(m_bglLight)
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
			}

			if(m_bSurfaces && m_pBody)	glCallList(BODYSURFACES);

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

			if(m_pBody && m_pFrame && (m_bOutline||m_bSurfaces)) glCallList(BODYFRAME3D);
			if(m_bOutline && m_pBody)	glCallList(BODYGEOM);
			if(m_bVLMPanels && m_pBody)
			{
				if(!m_bSurfaces)//else panels will be filled by Cp color
					glCallList(BODYMESHBACK);
				glCallList(BODYMESHPANELS);
			}

			if(m_bShowMasses)
			{
				QString MassUnit;
				GetWeightUnit(MassUnit, pMainFrame->m_WeightUnit);

				glColor3d(pMiarex->m_MassColor.redF(),pMiarex->m_MassColor.greenF(),pMiarex->m_MassColor.blueF());
				double radius = .01;
				glPushMatrix();
				{
					glTranslated(m_pBody->m_FramePosition[0].x,m_pBody->m_FramePosition[0].y,m_pBody->m_FramePosition[0].z);

					m_pglWidget->renderText(0.0, 0.0, m_pBody->GetLength()/10.0,
									  m_pBody->m_BodyName+QString(" %1").arg(m_pBody->m_VolumeMass*pMainFrame->m_kgtoUnit, 7,'g',3)+MassUnit);
				}
				glPopMatrix();

				for(int im=0; im<m_pBody->m_NMass; im++)
				{
					glPushMatrix();
					{
						glTranslated(m_pBody->m_MassPosition[im].x,m_pBody->m_MassPosition[im].y,m_pBody->m_MassPosition[im].z);
						GLRenderSphere(pMiarex->m_MassColor,radius,18,18);

						m_pglWidget->renderText(0.0, 0.0, 0.02, m_pBody->m_MassTag[im]);
					}
					glPopMatrix();
				}
			}
		}
		glPopMatrix();

		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);
	}
	glPopMatrix();
	glFinish();
}


void GL3dBodyDlg::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glBegin(GL_TRIANGLES);
	glColor3d(cr.redF(),cr.greenF(),cr.blueF());

	double start_lat = -90;
	double start_lon = 0.0;
	double R = radius;

	double lat_incr = 180.0 / NumLatitudes;
	double lon_incr = 360.0 / NumLongitudes;

	double phi1, phi2, theta1, theta2;
	GLdouble u[3], v[3], w[3], n[3];

	int row, col;

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
}


void GL3dBodyDlg::GLSetupLight()
{
	GLLightDlg *pGLLightDlg = (GLLightDlg *)s_pGLLightDlg;
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

	float LightFactor = 1.0f;
//	if(LightFactor>1.0) LightFactor = 1.0f;

	// the ambient light conditions.
	fLightAmbient0[0] = LightFactor*pGLLightDlg->m_Ambient * pGLLightDlg->m_Red; // red component
	fLightAmbient0[1] = LightFactor*pGLLightDlg->m_Ambient * pGLLightDlg->m_Green; // green component
	fLightAmbient0[2] = LightFactor*pGLLightDlg->m_Ambient * pGLLightDlg->m_Blue; // blue component
	fLightAmbient0[3] = 1.0; // alpha

	fLightDiffuse0[0] = LightFactor*pGLLightDlg->m_Diffuse * pGLLightDlg->m_Red; // red component
	fLightDiffuse0[1] = LightFactor*pGLLightDlg->m_Diffuse * pGLLightDlg->m_Green; // green component
	fLightDiffuse0[2] = LightFactor*pGLLightDlg->m_Diffuse * pGLLightDlg->m_Blue; // blue component
	fLightDiffuse0[3] = 1.0; // alpha

	fLightSpecular0[0] = LightFactor*pGLLightDlg->m_Specular * pGLLightDlg->m_Red; // red component
	fLightSpecular0[1] = LightFactor*pGLLightDlg->m_Specular * pGLLightDlg->m_Green; // green component
	fLightSpecular0[2] = LightFactor*pGLLightDlg->m_Specular * pGLLightDlg->m_Blue; // blue component
	fLightSpecular0[3] = 1.0; // alpha

	// And finally, its position

	fLightPosition0[0] = (GLfloat)((pGLLightDlg->m_XLight));
	fLightPosition0[1] = (GLfloat)((pGLLightDlg->m_YLight + m_UFOOffset.y));
	fLightPosition0[2] = (GLfloat)((pGLLightDlg->m_ZLight));
	fLightPosition0[3] = 1.0; // W (positional light)



	// Enable the basic light
	glLightfv(GL_LIGHT0, GL_AMBIENT,  fLightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  fLightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition0);


	float fMatAmbient[4]   = {pGLLightDlg->m_MatAmbient,  pGLLightDlg->m_MatAmbient,   pGLLightDlg->m_MatAmbient,1.0f};
	float fMatSpecular[4]  = {pGLLightDlg->m_MatSpecular, pGLLightDlg->m_MatSpecular,  pGLLightDlg->m_MatSpecular,1.0f};
	float fMatDiffuse[4]   = {pGLLightDlg->m_MatDiffuse,  pGLLightDlg->m_MatDiffuse,   pGLLightDlg->m_MatDiffuse,1.0f};
	float fMatEmission[4]  = {pGLLightDlg->m_MatEmission, pGLLightDlg->m_MatEmission,  pGLLightDlg->m_MatEmission,1.0f};

	if(pGLLightDlg->m_bColorMaterial)
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
	glMateriali(GL_FRONT, GL_SHININESS,  pGLLightDlg->m_iMatShininess);

	if(pGLLightDlg->m_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(pGLLightDlg->m_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(pGLLightDlg->m_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(pGLLightDlg->m_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(pGLLightDlg->m_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                          glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}


void GL3dBodyDlg::GLToClient(CVector const &real, QPoint &point)
{
	if(!m_pglWidget) return;
	double h = (double)m_pglWidget->geometry().height();
	double w = (double)m_pglWidget->geometry().width();
	double scale;

	if(w>=h) scale = (double)m_pglWidget->geometry().width()  / 2.0;
	else     scale = (double)m_pglWidget->geometry().height() / 2.0;

//	point.rx() =  (int)(scale *(1.0 + real.x));
	point.rx() = (int)(w/2.0 + real.x*scale);
	point.ry() = (int)(h/2.0 - real.y*scale);
}



void GL3dBodyDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString length;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	m_pFrameModel->setHeaderData(0, Qt::Horizontal, "x ("+length+")");
	m_pFrameModel->setHeaderData(1, Qt::Horizontal, "z ("+length+")");
	m_pFrameModel->setHeaderData(2, Qt::Horizontal, tr("NPanels"));
	m_pPointModel->setHeaderData(0, Qt::Horizontal, "y ("+length+")");
	m_pPointModel->setHeaderData(1, Qt::Horizontal, "z ("+length+")");
	m_pPointModel->setHeaderData(2, Qt::Horizontal, tr("NPanels"));
	
/*	int w3 = (int)(m_pctrlFrameTable->width()/3);
	m_pctrlFrameTable->setColumnWidth(0,w3);
	m_pctrlFrameTable->setColumnWidth(1,w3);
	m_pctrlFrameTable->setColumnWidth(2,w3);
	w3 = (int)(m_pctrlPointTable->width()/3);
	m_pctrlPointTable->setColumnWidth(0,w3);
	m_pctrlPointTable->setColumnWidth(1,w3);
	m_pctrlPointTable->setColumnWidth(2,w3);*/
	
	QHeaderView *HorizontalHeader = m_pctrlFrameTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);
	HorizontalHeader = m_pctrlPointTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);
	
	m_pctrlBodyName->setText(m_pBody->m_BodyName);
	m_pctrlBodyName->setEnabled(m_bEnableName);
	m_pctrlBodyDescription->setPlainText(m_pBody->m_BodyDescription);

	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlLight->setChecked(m_bglLight);
	m_pctrlShowMasses->setChecked(m_bShowMasses);
	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));
	m_pctrlSurfaces->setChecked(m_bSurfaces);

	m_pctrlUndo->setEnabled(false);
	m_pctrlRedo->setEnabled(false);
}


void GL3dBodyDlg::keyPressEvent(QKeyEvent *event)
{
	bool bShift = false;
	bool bCtrl  = false;
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	switch (event->key())
	{
		case Qt::Key_Z:
		{
			if(bCtrl)
			{
				if(bShift)
				{
					OnRedo();
				}
				else OnUndo();
				event->accept();
			}
			else event->ignore();
			break;
		}
		case Qt::Key_Y:
		{
			if(bCtrl)
			{
				OnRedo();
				event->accept();
			}
			else event->ignore();
			break;
		}
		case Qt::Key_Return:
		{
			if(!m_pctrlOK->hasFocus() && !m_pctrlCancel->hasFocus()) m_pctrlOK->setFocus();
			else if(m_pctrlOK->hasFocus()) OnOK();
			else if(m_pctrlCancel->hasFocus()) reject();
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		case Qt::Key_Control:
		{
			m_bArcball = true;
			UpdateView();
			break;
		}
		default:
			event->ignore();
	}
}


void GL3dBodyDlg::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
		{
			m_bArcball = false;
			UpdateView();
			break;
		}
		default:
			event->ignore();
	}
}


bool GL3dBodyDlg::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GL3dBody");
	{
		m_BodyGridDlg.m_bGrid      = pSettings->value("Grid").toBool();
		m_BodyGridDlg.m_bMinGrid   = pSettings->value("MinGrid").toBool();
		m_BodyGridDlg.m_bGrid2     = pSettings->value("Grid2").toBool();
		m_BodyGridDlg.m_bMinGrid2  = pSettings->value("MinGrid2").toBool();
		m_BodyGridDlg.m_Style      = pSettings->value("Style").toInt();
		m_BodyGridDlg.m_MinStyle   = pSettings->value("MinStyle").toInt();
		m_BodyGridDlg.m_Style2     = pSettings->value("Style2").toInt();
		m_BodyGridDlg.m_MinStyle2  = pSettings->value("MinStyle2").toInt();
		m_BodyGridDlg.m_Width      = pSettings->value("Width").toInt();
		m_BodyGridDlg.m_MinWidth   = pSettings->value("MinWidth").toInt();
		m_BodyGridDlg.m_Width2     = pSettings->value("Width2").toInt();
		m_BodyGridDlg.m_MinWidth2  = pSettings->value("MinWidth2").toInt();
		m_BodyGridDlg.m_Color      = pSettings->value("Color").value<QColor>();
		m_BodyGridDlg.m_MinColor   = pSettings->value("MinColor").value<QColor>();
		m_BodyGridDlg.m_Color2     = pSettings->value("Color2").value<QColor>();
		m_BodyGridDlg.m_MinColor2  = pSettings->value("MinColor2").value<QColor>();
		m_BodyGridDlg.m_Unit       = pSettings->value("Unit").toDouble();
		m_BodyGridDlg.m_MinorUnit  = pSettings->value("MinorUnit").toDouble();
		m_BodyGridDlg.m_Unit2      = pSettings->value("Unit2").toDouble();
		m_BodyGridDlg.m_MinorUnit2 = pSettings->value("MinorUnit2").toDouble();
		m_BodyGridDlg.m_bScale     = pSettings->value("bScale").toBool();

	}
	pSettings->endGroup();
	return true;
}



void GL3dBodyDlg::mouseMoveEvent(QMouseEvent *event)
{
	static int n;
	static CVector Real;
	static QPoint Delta, point;

	point = event->pos();

	m_MousePos = event->pos();

	Delta.setX(point.x() - m_LastPoint.x());
	Delta.setY(point.y() - m_LastPoint.y());
	ClientToGL(point, Real);

	if(!m_pglWidget->hasFocus()) m_pglWidget->setFocus();

	bool bCtrl = false;

	if (event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if (event->buttons()   & Qt::LeftButton)
	{
		if(bCtrl&& m_BodyRect.contains(point))
		{
			//rotate
			m_ArcBall.Move(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
			UpdateView();
		}
		else if(m_bTrans)
		{
			//translate
			if(m_BodyRect.contains(point))
			{
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0/m_glScaled/m_pglWidget->m_rCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0/m_glScaled/m_pglWidget->m_rCltRect.width());

				m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
			else if (m_BodyLineRect.contains(point) && m_pBody)
			{
				m_BodyOffset.x  +=  (double)Delta.x()*2.0/(double)m_pglWidget->m_rCltRect.width()/m_BodyScale;
				m_BodyOffset.y  += -(double)Delta.y()*2.0/(double)m_pglWidget->m_rCltRect.width()/m_BodyScale;
				m_BodyScaledOffset.Set((1.0-m_BodyScale)*m_BodyScalingCenter.x + m_BodyScale * m_BodyOffset.x,
									   (1.0-m_BodyScale)*m_BodyScalingCenter.y + m_BodyScale * m_BodyOffset.y,
										0.0);
				UpdateView();
			}
			else if(m_FrameRect.contains(point) && m_pFrame)
			{
				m_FrameOffset.x +=  (double)Delta.x()*2.0/(double)m_pglWidget->m_rCltRect.width()/m_FrameScale;
				m_FrameOffset.y += -(double)Delta.y()*2.0/(double)m_pglWidget->m_rCltRect.width()/m_FrameScale;
				m_FrameScaledOffset.Set((1.0-m_FrameScale)*m_FrameScalingCenter.x + m_FrameScale * m_FrameOffset.x,
										(1.0-m_FrameScale)*m_FrameScalingCenter.y + m_FrameScale * m_FrameOffset.y,
										 0.0);
				UpdateView();
			}
		}
		else if (m_BodyLineRect.contains(point) && m_pBody)
		{

			Real.x =  (Real.x - m_BodyScaledOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyScaledOffset.y)/m_BodyScale;
			Real.z = 0.0;

			int n = m_pBody->m_iActiveFrame;
			if (n>=0 && n<=m_pBody->m_NStations && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				m_pFrame = m_pBody->m_Frame + m_pBody->m_iActiveFrame;
				m_pBody->m_FramePosition[n].x = Real.x;
				m_pBody->m_FramePosition[n].z = Real.y;
				m_pBody->UpdateFramePos(n);
				m_bTrans = false;
				m_bResetglBody2D = true;
			}

			UpdateView();
		}
		else if(m_FrameRect.contains(point) && m_pFrame)
		{
			Real.x =  (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.z =   0.0;

			if(m_pFrame)	n = CFrame::s_iSelect;
			else			n = -10;
			if (n>0 && n<m_pFrame->m_NPoints-1 && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				if(Real.x<0.0) 	m_pFrame->m_Point[n].y = 0.0;
				else            m_pFrame->m_Point[n].y = Real.x;
				m_pFrame->m_Point[n].z = Real.y;
				m_pBody->ComputeCenterLine();
				m_bTrans = false;
				m_bResetglBody2D = true;
			}
			else if ((n==0 || n==m_pFrame->m_NPoints-1)  && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				m_pFrame->m_Point[n].y = 0.0;
				m_pFrame->m_Point[n].z = Real.y;
				m_pBody->ComputeCenterLine();
				m_bTrans = false;
				m_bResetglBody2D = true;
			}

			UpdateView();
		}
	}
	else if (event->buttons() & Qt::MidButton)
	{
		//rotate
		m_ArcBall.Move(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
		UpdateView();
	}
	else 
	{
		//Highlight points
		if (m_BodyLineRect.contains(point) && m_pBody)
		{
			Real.x =  (Real.x - m_BodyScaledOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyScaledOffset.y)/m_BodyScale;
			Real.z = 0.0;
			int n = m_pBody->IsFramePos(Real, m_BodyScale);
			m_pBody->m_iHighlight = -10;
			if (n>=0 && n<=m_pBody->m_NStations)
			{
				m_pBody->m_iHighlight = n;
			}
			m_bResetglBodyPoints = true;
			UpdateView();
		}
		else if (m_pFrame && m_FrameRect.contains(point))
		{
			Real.x =  (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.z = 0.0;
			int n = m_pFrame->IsPoint(Real, m_FrameScale);
			m_pFrame->m_iHighlight = -10;
			if (n>=0 && n<=m_pFrame->m_NPoints)
			{
				m_pFrame->m_iHighlight = n;
			}
			m_bResetglBodyPoints = true;
			UpdateView();
		}
	}
	m_LastPoint = point;
}


void GL3dBodyDlg::mousePressEvent(QMouseEvent *event)
{
	int iF;
	QPoint point(event->pos().x(), event->pos().y());

	CVector Real;
	bool bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	ClientToGL(point, Real);

	if(m_pglWidget->m_rCltRect.contains(point)) m_pglWidget->setFocus();


	if(m_bPickCenter)
	{
		Set3DRotationCenter(point);
		m_bPickCenter = false;
		m_pctrlPickCenter->setChecked(false);
	}
	if (event->buttons() & Qt::MidButton)
	{
		m_bArcball = true;
		m_ArcBall.Start(event->pos().x(), m_pglWidget->m_rCltRect.height()-event->pos().y());
		m_bCrossPoint = true;

		Set3DRotationCenter();
		UpdateView();
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		m_bTrans=true;
		if(m_pBody && m_BodyRect.contains(point))
		{
			m_ArcBall.Start(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
			m_bCrossPoint = true;
			Set3DRotationCenter();
			if (!bCtrl)
			{
				m_bTrans = true;
				m_pglWidget->setCursor(Qt::ClosedHandCursor);
			}
			UpdateView();
		}
		else if(m_pBody && m_BodyLineRect.contains(point))
		{
			Real.x =  (Real.x - m_BodyScaledOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyScaledOffset.y)/m_BodyScale;
			Real.z = 0.0;
			iF = m_pBody->IsFramePos(Real, m_BodyScale);

			if(iF >=0)
			{
				TakePicture();
				StorePicture();
				m_pBody->m_iActiveFrame = iF;
				m_pFrame = m_pBody->GetFrame(m_pBody->m_iActiveFrame);

				SetFrame(m_pBody->m_iActiveFrame);
				m_pctrlFrameTable->selectRow(iF);
				if(CFrame::s_iSelect>=0) m_pctrlPointTable->selectRow(CFrame::s_iSelect);

				m_bTrans = false;
				m_bDragPoint  = true;
				UpdateView();
			}
		}
		else if(m_pFrame && m_FrameRect.contains(point))
		{
			Real.x =  (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.z = 0.0;

			CFrame::s_iSelect = m_pFrame->IsPoint(Real, m_FrameScale);
			if(CFrame::s_iSelect >=0)
			{
				TakePicture();
				StorePicture();
				m_bTrans = false;
				m_bDragPoint  = true;
				m_pctrlPointTable->selectRow(CFrame::s_iSelect);
			}
		}
		if(m_bTrans && !bCtrl)	m_pglWidget->setCursor(Qt::ClosedHandCursor);
	}
	

	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
	m_PointDown = point;
	m_LastPoint = point;
}



void GL3dBodyDlg::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	m_pglWidget->setCursor(Qt::CrossCursor);

	if(!m_bTrans)
	{
		if(point==m_PointDown)
		{
			m_bResetglBodyPoints = true;
		}
		else
		{
			int n1, n2;
			n1 = m_pBody->m_iActiveFrame;
			if(m_pFrame)	n2 = CFrame::s_iSelect;
			else			n2 = -10;

			if (m_BodyLineRect.contains(point) && n1>=0 && n1<m_pBody->m_NStations)
			{	//the user has been dragging a point
				FillFrameCell(n1,0);
				FillFrameCell(n1,1);
				SetFrame(n1);
				m_bResetglBody     = true;
				m_bResetglBodyMesh = true;
				m_bResetglBody2D   = true;
			}
			else if (m_FrameRect.contains(point) && n2>=0 && n2<m_pFrame->m_NPoints)
			{	//the user has been dragging a point
				FillFrameCell(n1,0);
				FillFrameCell(n1,1);
				FillPointCell(n2,0);
				FillPointCell(n2,1);
				m_bResetglBody     = true;
				m_bResetglBodyMesh = true;
				m_bResetglBody2D   = true;
			}
		}
	}
	else m_bResetglBody2D   = true;

	
	m_bTrans = false;
	m_bDragPoint  = false;

	m_bArcball    = false;
	m_bCrossPoint = false;
	UpdateView();

//	We need to re-calculate the translation vector
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

	GLInverseMatrix();
	m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
	m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
	m_glViewportTrans.z =  (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

}


void GL3dBodyDlg::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{
	GLdouble v1[3], v2[3], d;
	// calculate two vectors, using the middle point
	// as the common origin
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



void GL3dBodyDlg::On3DIso()
{
	m_ArcBall.ab_quat[0]	= -0.65987748f;
	m_ArcBall.ab_quat[1]	=  0.38526487f;
	m_ArcBall.ab_quat[2]	= -0.64508355f;
	m_ArcBall.ab_quat[3]	=  0.0f;
	m_ArcBall.ab_quat[4]	= -0.75137258f;
	m_ArcBall.ab_quat[5]	= -0.33720365f;
	m_ArcBall.ab_quat[6]	=  0.56721509f;
	m_ArcBall.ab_quat[7]	=  0.0f;
	m_ArcBall.ab_quat[8]	=  0.000f;
	m_ArcBall.ab_quat[9]	=  0.85899049f;
	m_ArcBall.ab_quat[10]	=  0.51199043f;
	m_ArcBall.ab_quat[11]	=  0.0f;
	m_ArcBall.ab_quat[12]	=  0.0f;
	m_ArcBall.ab_quat[13]	=  0.0f;
	m_ArcBall.ab_quat[14]	=  0.0f;
	m_ArcBall.ab_quat[15]	=  1.0f;

	Set3DRotationCenter();
	UpdateView();
}



void GL3dBodyDlg::On3DTop()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::On3DLeft()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::On3DFront()
{
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::On3DReset()
{
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_pctrlPickCenter->setChecked(false);
	m_bIs3DScaleSet = false;
	m_bResetglBody2D = true;

	SetBodyScale();
	UpdateView();
}


void GL3dBodyDlg::On3DPickCenter()
{
	m_bPickCenter = true;
	m_pctrlPickCenter->setChecked(true);
}



void GL3dBodyDlg::OnAxes()
{
	m_bAxes = m_pctrlAxes->isChecked();
//	m_bResetglBody2D = true;
	UpdateView();
}

void GL3dBodyDlg::OnBodyName()
{
	if(m_pBody)
	{
		m_pBody->m_BodyName = m_pctrlBodyName->text();
		m_pBody->m_BodyDescription = m_pctrlBodyDescription->toPlainText();
	}
}


void GL3dBodyDlg::OnBodyStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	s = m_pBody->m_BodyStyle;
	w = m_pBody->m_BodyWidth;
	color = m_pBody->m_BodyColor;
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_bChanged = true;
		m_pBody->m_BodyStyle = dlg.GetStyle();
		m_pBody->m_BodyWidth = dlg.GetWidth();
		m_pBody->m_BodyColor = dlg.GetColor();

		m_pctrlBodyStyle->SetStyle(dlg.GetStyle());
		m_pctrlBodyStyle->SetWidth(dlg.GetWidth());
		m_pctrlBodyStyle->SetColor(dlg.GetColor());
		m_bResetglBody2D = true;
		m_bResetglBody = true;
		UpdateView();
	}
}


void GL3dBodyDlg::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}




void GL3dBodyDlg::OnExportBodyDef()
{
	if(!m_pBody) return;
	m_pBody->ExportDefinition();
}


void GL3dBodyDlg::OnExportBodyGeom()
{
	if(!m_pBody) return;
	m_pBody->ExportGeometry(m_NXPoints, m_NHoopPoints);
}


void GL3dBodyDlg::OnImportBodyDef()
{
//	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	CBody *pNewBody = new CBody();
	if(!pNewBody) return;
	if(!pNewBody->ImportDefinition())
	{
		delete pNewBody;
		return;
	}

	if(!pMiarex->SetModBody(pNewBody))
	{
		delete pNewBody;
		UpdateView();
	}
	else
	{
		m_pBody = pMiarex->AddBody(pNewBody);
		SetBody(m_pBody);
		SetBodyScale();
	}
}


void GL3dBodyDlg::OnFrameCellChanged(QWidget *pWidget)
{
	TakePicture();
	StorePicture();
	m_bChanged = true;
	int n = m_pBody->m_iActiveFrame;
	ReadFrameSectionData(m_pBody->m_iActiveFrame);
	m_pBody->UpdateFramePos(n);
	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	m_bResetglBodyMesh = true;
	UpdateView();
}




void GL3dBodyDlg::OnFrameItemClicked(const QModelIndex &index)
{
	m_pBody->m_iActiveFrame = index.row();
	SetFrame(m_pBody->m_iActiveFrame);
	UpdateView();
}



void GL3dBodyDlg::OnGrid()
{
	m_BodyGridDlg.InitDialog();
	m_BodyGridDlg.exec();
	m_bResetglBody2D = true;
	UpdateView();
}


void GL3dBodyDlg::OnInertia()
{
	InertiaDlg dlg;
	dlg.m_pBody = m_pBody;

	//save inertia properties
	int NMass;
	double MassValue[MAXMASSES];
	CVector MassPosition[MAXMASSES];
	QString MassTag[MAXMASSES];

	NMass = m_pBody->m_NMass;

	for(int i=0; i< MAXMASSES; i++)
	{
		MassValue[i]    = m_pBody->m_MassValue[i];
		MassPosition[i] = m_pBody->m_MassPosition[i];
		MassTag[i]      = m_pBody->m_MassTag[i];
	}

	dlg.InitDialog();
	if(dlg.exec() == QDialog::Accepted)
	{
		if(dlg.m_bChanged) m_bChanged = true;
	}
	else
	{
		// restore saved inertia
		m_pBody->m_NMass = NMass;

		for(int i=0; i< MAXMASSES; i++)
		{
			MassValue[i]    = m_pBody->m_MassValue[i];
			MassPosition[i] = m_pBody->m_MassPosition[i];
			MassTag[i]      = m_pBody->m_MassTag[i];
		}
	}
}


void GL3dBodyDlg::OnInsert()
{
	CVector Real;
	m_bChanged = true;
	int FrameSel = 0;

	if(m_BodyLineRect.contains(m_ptPopUp))
	{
		TakePicture();
		StorePicture();

		Real.x = (m_RealPopUp.x - m_BodyScaledOffset.x)/m_BodyScale;
		Real.z = (m_RealPopUp.y - m_BodyScaledOffset.y)/m_BodyScale;
		Real.y = 0.0;

		if(m_pFrame)
		{
			FrameSel = m_pBody->InsertFrame(Real);
			if(FrameSel>0) m_pFrame = m_pBody->m_Frame+FrameSel;
			else           m_pFrame = NULL;
		}

		m_bResetglBody   = true;
		m_bResetglBody2D = true;
		FillFrameDataTable();
		SetFrame(FrameSel);
		UpdateView();
	}
	else if(m_FrameRect.contains(m_ptPopUp) && m_pFrame)
	{
		TakePicture();
		StorePicture();

		Real.x = (m_RealPopUp.x - m_FrameScaledOffset.x)/m_FrameScale;
		Real.y = (m_RealPopUp.y - m_FrameScaledOffset.y)/m_FrameScale;
		Real.z = 0.0;

		m_pBody->InsertPoint(Real);
		m_bResetglBody   = true;
		m_bResetglBody2D = true;
		FillPointDataTable();
//		SetPointSel(point);
		UpdateView();
	}
}



void GL3dBodyDlg::OnLight()
{
	m_bglLight = m_pctrlLight->isChecked();
	UpdateView();
}



void GL3dBodyDlg::OnShowMasses()
{
	m_bShowMasses = m_pctrlShowMasses->isChecked();
	UpdateView();
}


void GL3dBodyDlg::OnLineType()
{
	m_bChanged = true;
	if(m_pctrlFlatPanels->isChecked())
	{
		m_pBody->m_LineType = 1;
		m_pctrlNXPanels->setEnabled(false);
		m_pctrlNHoopPanels->setEnabled(false);
		m_pctrlXDegree->setEnabled(false);
		m_pctrlHoopDegree->setEnabled(false);
	}
	else
	{
		m_pBody->m_LineType = 2;
		m_pctrlNXPanels->setEnabled(true);
		m_pctrlNHoopPanels->setEnabled(true);
		m_pctrlXDegree->setEnabled(true);
		m_pctrlHoopDegree->setEnabled(true);
	}
	m_bResetglBody2D = true;
	m_bResetglBody = true;
	UpdateView();
}


void GL3dBodyDlg::OnOK()
{
	if(m_pBody)
	{
		m_pBody->m_BodyDescription = m_pctrlBodyDescription->toPlainText();
		m_pBody->m_BodyColor = m_pctrlBodyStyle->GetColor();
		m_pBody->m_BodyStyle = m_pctrlBodyStyle->GetStyle();
		m_pBody->m_BodyWidth = m_pctrlBodyStyle->GetWidth();
	}
	s_bWindowMaximized= isMaximized();
	s_WindowPos = pos();
	s_WindowSize = size();
	accept();
}


void GL3dBodyDlg::OnOutline()
{
	m_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


void GL3dBodyDlg::OnPanels()
{
	m_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}



void GL3dBodyDlg::OnPointCellChanged(QWidget *pWidget)
{
	TakePicture();
	StorePicture();
	m_bChanged = true;
	ReadPointSectionData(CFrame::s_iSelect);
	m_bResetglBodyPoints = true;
	m_bResetglBody       = true;
	m_bResetglBody2D     = true;
	m_bResetglBodyMesh   = true;
	UpdateView();
}


void GL3dBodyDlg::OnPointItemClicked(const QModelIndex &index)
{
	if(!m_pFrame) return;
	CFrame::s_iSelect = index.row();
	m_pFrame->m_iHighlight = index.row();
	m_bResetglBodyPoints = true;
	UpdateView();
}


void GL3dBodyDlg::OnRedo()
{
	if(m_StackPos<m_StackSize-1)
	{
		m_StackPos++;
		SetPicture();
		m_pctrlUndo->setEnabled(true);
		if(m_StackPos==m_StackSize-1) 	m_pctrlRedo->setEnabled(false);
	}
}

void GL3dBodyDlg::OnUndo()
{
	if(m_StackPos>0)
	{
		if(m_StackPos == m_StackSize)
		{
			//if we're at the first undo command, save current state
			TakePicture();
			StorePicture();//in case we redo
			m_StackPos--;
		}
		m_StackPos--;
		SetPicture();
		if(m_StackPos==0)	m_pctrlUndo->setEnabled(false);
		m_pctrlRedo->setEnabled(true);
	}
	else
	{
		m_StackPos = 0;
		m_pctrlUndo->setEnabled(false);
	}
}



void GL3dBodyDlg::OnRemove()
{
	int i,n;
	CVector Real;

	CFrame *pBodyFrame;

	if(m_BodyLineRect.contains(m_ptPopUp))
	{
		TakePicture();
		StorePicture();

		Real.x =  (m_RealPopUp.x - m_BodyScaledOffset.x)/m_BodyScale;
		Real.y =  (m_RealPopUp.y - m_BodyScaledOffset.y)/m_BodyScale;
		Real.z = 0.0;
		n =  m_pBody->IsFramePos(Real, m_BodyScale/m_BodyRefScale);
		if (n>=0)
		{
			n = m_pBody->RemoveFrame(n);
			FillFrameDataTable();
			SetFrame(n);
			m_bResetglBody   = true;
			m_bResetglBody2D = true;
		}

		UpdateView();
	}
	else if(m_FrameRect.contains(m_ptPopUp) && m_pFrame)
	{
		TakePicture();
		StorePicture();

		Real.x =  (m_RealPopUp.x - m_FrameScaledOffset.x)/m_FrameScale;
		Real.y =  (m_RealPopUp.y - m_FrameScaledOffset.y)/m_FrameScale;
		Real.z = 0.0;

		n =   m_pFrame->IsPoint(Real, m_BodyScale/m_BodyRefScale);
		if (n>=0)
		{
			for (i=0; i<m_pBody->m_NStations;i++)
			{
				pBodyFrame = m_pBody->m_Frame+i;
				pBodyFrame->RemovePoint(n);
			}
			m_pBody->m_NSideLines--;
			m_pBody->SetKnots();
			SetFrame(m_pBody->m_iActiveFrame);
			m_bResetglBody   = true;
			m_bResetglBody2D = true;
		}

		UpdateView();
	}
}


void GL3dBodyDlg::OnResetScales()
{
	m_bIs3DScaleSet  = false;
	m_bResetglBody2D = true;
	SetBodyScale();
	UpdateView();
}


void GL3dBodyDlg::OnScaleBody()
{
	if(!m_pBody) return;

	BodyScaleDlg dlg(this);
	if(m_FrameRect.contains(m_ptPopUp))
	{
		dlg.m_bFrameOnly = true;
	}
	dlg.m_FrameID = m_pBody->m_iActiveFrame;
	dlg.InitDialog();

	if(dlg.exec()==QDialog::Accepted)
	{
		TakePicture();
		StorePicture();
		m_pBody->Scale(dlg.m_XFactor, dlg.m_YFactor, dlg.m_ZFactor, dlg.m_bFrameOnly, dlg.m_FrameID);
		m_bResetglBody     = true;
		m_bResetglBodyMesh = true;
		m_bResetglBody2D   = true;

		UpdateView();
	}
}

void GL3dBodyDlg::OnSelChangeXDegree(int sel)
{
	if(!m_pBody) return;
	if (sel <0) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);

	TakePicture();
	StorePicture();

	m_pBody->m_nxDegree = sel+1;
	m_pBody->SetKnots();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	UpdateView();
}


void GL3dBodyDlg::OnSelChangeHoopDegree(int sel)
{
	if(!m_pBody) return;
	if (sel <0) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);

	TakePicture();
	StorePicture();

	m_pBody->m_nhDegree = sel+1;
	m_pBody->SetKnots();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	UpdateView();
}

void GL3dBodyDlg::OnNURBSPanels()
{
	if(!m_pBody) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);

	TakePicture();
	StorePicture();

	m_pBody->m_nhPanels = m_pctrlNHoopPanels->GetValue();
	m_pBody->m_nxPanels = m_pctrlNXPanels->GetValue();
//	m_pBody->SetKnots();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	UpdateView();
}
/*
void GL3dBodyDlg::OnSetupLight()
{
	GLLightDlg *pGLLightDlg = (GLLightDlg *)s_pGLLightDlg;
	m_bShowLight = true;
	UpdateView();
	pGLLightDlg->m_pGL3dBodyDlg = this;
	pGLLightDlg->exec();

	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}*/

void GL3dBodyDlg::OnShowCurFrameOnly()
{
	m_bCurFrameOnly = !m_bCurFrameOnly;
	m_bResetglBody2D = true;
	m_pShowCurFrameOnly->setChecked(m_bCurFrameOnly);
	UpdateView();
}


void GL3dBodyDlg::OnSurfaces()
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
	UpdateView();
}


void GL3dBodyDlg::OnTranslateBody()
{
	if(!m_pBody) return;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	BodyTransDlg dlg;
	dlg.m_pMainFrame = s_pMainFrame;
	dlg.m_pMiarex    = s_pMiarex;
	dlg.m_FrameID    = m_pBody->m_iActiveFrame;
	dlg.InitDialog();

	if(dlg.exec()==QDialog::Accepted)
	{
		TakePicture();
		StorePicture();
		m_pBody->Translate(dlg.m_XTrans, dlg.m_YTrans, dlg.m_ZTrans, dlg.m_bFrameOnly, dlg.m_FrameID);
		FillFrameDataTable();
		FillPointDataTable();

		m_bResetglBody     = true;
		m_bResetglBodyMesh = true;
		m_bResetglBody2D   = true;
		UpdateView();
	}
}


void GL3dBodyDlg::ReadFrameSectionData(int sel)
{
	if(sel>=m_pFrameModel->rowCount()) return;
	double d;
	int k;

	bool bOK;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	QStandardItem *pItem;

	pItem = m_pFrameModel->item(sel,0);

	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pBody->m_FramePosition[sel].x =d / pMainFrame->m_mtoUnit;

	pItem = m_pFrameModel->item(sel,1);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pBody->m_FramePosition[sel].z =d / pMainFrame->m_mtoUnit;

	pItem = m_pFrameModel->item(sel,2);
	strong = pItem->text();
	strong.replace(" ","");
	k =strong.toInt(&bOK);
	if(bOK) m_pBody->m_xPanels[sel] = k;
}





void GL3dBodyDlg::ReadPointSectionData(int sel)
{
	if(sel>=m_pPointModel->rowCount()) return;
	if(!m_pFrame) return;

	double d;
	int k;

	bool bOK;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	QStandardItem *pItem;

	pItem = m_pPointModel->item(sel,0);

	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pFrame->m_Point[sel].y =d / pMainFrame->m_mtoUnit;

	pItem = m_pPointModel->item(sel,1);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pFrame->m_Point[sel].z =d / pMainFrame->m_mtoUnit;

	pItem = m_pPointModel->item(sel,2);
	strong = pItem->text();
	strong.replace(" ","");
	k =strong.toInt(&bOK);
	if(bOK) m_pBody->m_hPanels[sel] = k;
}


void GL3dBodyDlg::reject()
{
	if(m_bChanged)
	{
		m_pBody->m_BodyName = m_pctrlBodyName->text();

		int res = QMessageBox::question(this, tr("Body Dlg Exit"), tr("Save the Body ?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if (QMessageBox::No == res)
		{
			m_pBody = NULL;
			QDialog::reject();
		}
		else if (QMessageBox::Cancel == res) return;
		else
		{
			m_pBody = NULL;
			done(QDialog::Accepted);
			return;
		}
	}
	else m_pBody = NULL;
	s_bWindowMaximized= isMaximized();
	s_WindowPos = pos();
	s_WindowSize = size();
	done(QDialog::Rejected);
}


void GL3dBodyDlg::resizeEvent(QResizeEvent *event)
{
	m_bResetglBody2D = true;

	m_bIs3DScaleSet = false;
	m_bResetglBody2D = true;
	SetBodyScale();

	int w4 = (int)(m_pctrlFrameTable->width()/4);
	m_pctrlFrameTable->setColumnWidth(0,w4);
	m_pctrlFrameTable->setColumnWidth(1,w4);
	m_pctrlFrameTable->setColumnWidth(2,w4);
	w4 = (int)(m_pctrlPointTable->width()/4);
	m_pctrlPointTable->setColumnWidth(0,w4);
	m_pctrlPointTable->setColumnWidth(1,w4);
	m_pctrlPointTable->setColumnWidth(2,w4);
}


bool GL3dBodyDlg::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GL3dBody");
	{
		pSettings->setValue("Grid", m_BodyGridDlg.m_bGrid);
		pSettings->setValue("MinGrid", m_BodyGridDlg.m_bMinGrid);
		pSettings->setValue("Grid2", m_BodyGridDlg.m_bGrid2);
		pSettings->setValue("MinGrid2", m_BodyGridDlg.m_bMinGrid2);
		pSettings->setValue("Style", m_BodyGridDlg.m_Style);
		pSettings->setValue("MinStyle", m_BodyGridDlg.m_MinStyle);
		pSettings->setValue("Style2", m_BodyGridDlg.m_Style2);
		pSettings->setValue("MinStyle2", m_BodyGridDlg.m_MinStyle2);
		pSettings->setValue("Width", m_BodyGridDlg.m_Width);
		pSettings->setValue("MinWidth", m_BodyGridDlg.m_MinWidth);
		pSettings->setValue("Width2", m_BodyGridDlg.m_Width2);
		pSettings->setValue("MinWidth2", m_BodyGridDlg.m_MinWidth2);
		pSettings->setValue("Color", m_BodyGridDlg.m_Color);
		pSettings->setValue("MinColor", m_BodyGridDlg.m_MinColor);
		pSettings->setValue("Color2", m_BodyGridDlg.m_Color2);
		pSettings->setValue("MinColor2", m_BodyGridDlg.m_MinColor2);
		pSettings->setValue("Unit", m_BodyGridDlg.m_Unit);
		pSettings->setValue("MinorUnit", m_BodyGridDlg.m_MinorUnit);
		pSettings->setValue("Unit2", m_BodyGridDlg.m_Unit2);
		pSettings->setValue("MinorUnit2", m_BodyGridDlg.m_MinorUnit2);
		pSettings->setValue("bScale", m_BodyGridDlg.m_bScale);

	}
	pSettings->endGroup();
	return true;
}


void GL3dBodyDlg::Set3DRotationCenter()
{
	//adjust the new rotation center after a translation or a rotation

	int i,j;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatOut[i][j] =  m_ArcBall.ab_quat[i*4+j];

	m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
	m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
	m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;
}


void GL3dBodyDlg::Set3DRotationCenter(QPoint point)
{
	//adjusts the new rotation center after the user has picked a point on the screen
	//finds the closest panel under the point,
	//and changes the rotation vector and viewport translation
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int  i, j;
	int nx, nh, inx, inh, p;
	CVector N, LATB, TALB, LA, LB, TA, TB;
	CVector I, A, B, AA, BB, PP, U;
	double dmin, dist;

	i=-1;
	dmin = 100000.0;

	ClientToGL(point, B);

	B.x += -m_UFOOffset.x - m_glViewportTrans.x*m_glScaled;
	B.y += -m_UFOOffset.y + m_glViewportTrans.y*m_glScaled;

	B *= 1.0/m_glScaled;

	A.Set(B.x, B.y, +1.0);
	B.z = -1.0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

	//convert screen to model coordinates
	AA.x = MatIn[0][0]*A.x + MatIn[0][1]*A.y + MatIn[0][2]*A.z;
	AA.y = MatIn[1][0]*A.x + MatIn[1][1]*A.y + MatIn[1][2]*A.z;
	AA.z = MatIn[2][0]*A.x + MatIn[2][1]*A.y + MatIn[2][2]*A.z;

	BB.x = MatIn[0][0]*B.x + MatIn[0][1]*B.y + MatIn[0][2]*B.z;
	BB.y = MatIn[1][0]*B.x + MatIn[1][1]*B.y + MatIn[1][2]*B.z;
	BB.z = MatIn[2][0]*B.x + MatIn[2][1]*B.y + MatIn[2][2]*B.z;


	U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
	U.Normalize();

	bool bIntersect = false;



	if(m_pBody->m_LineType==2)
	{
		nx = qMin(500, m_NXPoints);
		nh = qMax(m_NHoopPoints, 3);

		//right side
		p = 0;
		for (inx=0; inx<nx; inx++)
		{
			p++;
			for (inh=1; inh<=nh; inh++)
				{
				LA.Set(m_T[p+1]);   LB.Set(m_T[p]);   TA.Set(m_T[p+nh+2]);   TB.Set(m_T[p+nh+1]);
				LATB = TB - LA;
				TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				if(pMiarex->Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
				{
					if(dist < dmin)
					{
						dmin = dist;
							PP.Set(I);
						bIntersect = true;
					}
				}

				N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;

				if(pMiarex->Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
				{
					if(dist < dmin)
					{
						dmin = dist;
						PP.Set(I);
						bIntersect = true;
					}
				}

				p++;
			}
		}
	}
	else if(m_pBody->m_LineType==1)
	{
		int inh, inx;
		CVector Tj, Tjp1, LA, LB, TA, TB, TALB, LATB, N;
		for (inh=0; inh<m_pBody->m_NSideLines-1;inh++)
		{
			for (inx=0; inx<m_pBody->m_NStations-1;inx++)
			{
				Tj.Set(m_pBody->m_FramePosition[inx].x,     0.0, 0.0);
				Tjp1.Set(m_pBody->m_FramePosition[inx+1].x, 0.0, 0.0);

				LB = m_pBody->m_Frame[inx].m_Point[inh];      LB.x += Tj.x;
				TB = m_pBody->m_Frame[inx+1].m_Point[inh];    TB.x += Tjp1.x;
				TA = m_pBody->m_Frame[inx+1].m_Point[inh+1];  TA.x += Tjp1.x;
				LA = m_pBody->m_Frame[inx].m_Point[inh+1];    LA.x += Tj.x;

				TALB = LB - TA;
				LATB = TB - LA;
				N = TALB * LATB;
				N.Normalize();

				if(pMiarex->Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
				{
					if(dist < dmin)
					{
						dmin = dist;
						PP.Set(I);
						bIntersect = true;
					}
				}

				N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;

				if(pMiarex->Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
				{
					if(dist < dmin)
					{
						dmin = dist;
						PP.Set(I);
						bIntersect = true;
					}
				}
			}
		}
	}


	if(bIntersect)
	{

//		instantaneous visual transition
//		m_glRotCenter -= PP * m_glScaled;

//		smooth visual transition
		GLInverseMatrix();

		U.x = (-PP.x -m_glRotCenter.x)/30.0;
		U.y = (-PP.y -m_glRotCenter.y)/30.0;
		U.z = (-PP.z -m_glRotCenter.z)/30.0;

		for(i=0; i<30; i++)
		{
			m_glRotCenter +=U;
			m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
			m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
			m_glViewportTrans.z=   (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

			UpdateView();
		}
	}
}


void GL3dBodyDlg::SetBody(CBody *pBody)
{
	m_pBody = pBody;
	if(!m_pBody) return;

	m_pFrameDelegate->SetPointers(m_Precision,&m_pBody->m_NStations);
	m_pPointDelegate->SetPointers(m_Precision,&m_pBody->m_NSideLines);

	if(m_pBody->m_LineType==1) m_pctrlFlatPanels->setChecked(true);
	else                       m_pctrlBSplines->setChecked(true);

	m_pctrlBodyStyle->SetStyle(m_pBody->m_BodyStyle);
	m_pctrlBodyStyle->SetWidth(m_pBody->m_BodyWidth);
	m_pctrlBodyStyle->SetColor(m_pBody->m_BodyColor);

	m_pctrlNXPanels->SetValue(m_pBody->m_nxPanels);
	m_pctrlNHoopPanels->SetValue(m_pBody->m_nhPanels);

	m_pctrlXDegree->setCurrentIndex(m_pBody->m_nxDegree-1);
	m_pctrlHoopDegree->setCurrentIndex(m_pBody->m_nhDegree-1);

	m_pFrame = m_pBody->m_Frame+ m_pBody->m_iActiveFrame;
	m_bResetglBody2D = true;

	if(m_pBody->m_LineType==1)
	{
		m_pctrlNXPanels->setEnabled(false);
		m_pctrlNHoopPanels->setEnabled(false);
		m_pctrlXDegree->setEnabled(false);
		m_pctrlHoopDegree->setEnabled(false);
	}
	else
	{
		m_pctrlNXPanels->setEnabled(true);
		m_pctrlNHoopPanels->setEnabled(true);
		m_pctrlXDegree->setEnabled(true);
		m_pctrlHoopDegree->setEnabled(true);
	}
	FillFrameDataTable();
	FillPointDataTable();
}



void GL3dBodyDlg::SetBodyScale()
{
	int k;
	double length, height;
	QPoint P1, P2;
	CVector V1, V2;

	QRect CltRect = m_pglWidget->geometry();

	if(m_bIs3DScaleSet /*&& !m_bAutoScales*/) return;

	m_glViewportTrans.x = 0.0;
	m_glViewportTrans.y = 0.0;
	m_glViewportTrans.z = 0.0;
	m_bIs3DScaleSet = true;

	m_glTop = m_pglWidget->m_GLViewRect.top;

	m_VerticalSplit     = m_pglWidget->m_GLViewRect.width()  /6.0;
	m_HorizontalSplit   = m_pglWidget->m_GLViewRect.height() /6.0;

	m_UFOOffset.x = (GLfloat)(m_pglWidget->m_GLViewRect.left + m_VerticalSplit  )/2.0;
	m_UFOOffset.y = (GLfloat)(m_pglWidget->m_GLViewRect.bottom + m_HorizontalSplit)/2.0;
  
	m_glScaled = (GLfloat)(3./4.* (m_VerticalSplit+1.0) / m_pBody->GetLength());

	m_ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	m_ArcBall.SetZoom(0.3,eye,up);

	Set3DRotationCenter();


	m_BodyScalingCenter.x  = (m_VerticalSplit                + m_pglWidget->m_GLViewRect.left)    /2.0;
	m_BodyScalingCenter.y  = (m_pglWidget->m_GLViewRect.top  + m_HorizontalSplit)                 /2.0;
	m_FrameScalingCenter.x = (m_VerticalSplit                + m_pglWidget->m_GLViewRect.right)   /2.0;
	m_FrameScalingCenter.y = (m_pglWidget->m_GLViewRect.top  + m_pglWidget->m_GLViewRect.bottom) /2.0;


	V1.Set(m_pglWidget->m_GLViewRect.left, m_pglWidget->m_GLViewRect.top, 0.0);
	V2.Set(m_VerticalSplit,                m_HorizontalSplit,             0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_BodyLineRect.setTopLeft(P1);
	m_BodyLineRect.setBottomRight(P2);

	V1.Set(m_VerticalSplit,                 m_pglWidget->m_GLViewRect.top,    0.0);
	V2.Set(m_pglWidget->m_GLViewRect.right, m_pglWidget->m_GLViewRect.bottom, 0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_FrameRect.setTopLeft(P1);
	m_FrameRect.setBottomRight(P2);

	V1.Set(m_pglWidget->m_GLViewRect.left, m_HorizontalSplit,                0.0);
	V2.Set(m_VerticalSplit,                m_pglWidget->m_GLViewRect.bottom, 0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_BodyRect.setTopLeft(P1);
	m_BodyRect.setBottomRight(P2);

	if(m_pBody)
	{
		length =   m_pBody->m_FramePosition[m_pBody->m_NStations-1].x  - m_pBody->m_FramePosition[0].x;
		m_BodyScale = (m_VerticalSplit-(-1.0) - 0.3)/length;

		height = 0.0;
		for(k=0; k<m_pBody->m_NStations; k++)
		{
			height = qMax(height,fabs( m_pBody->m_Frame[k].m_Point[0].z - m_pBody->m_Frame[k].m_Point[m_pBody->m_Frame[k].m_NPoints-1].z));
		}
		m_FrameScale = (1.0-0.5)/height;
	}
	else
	{
		m_BodyScale  = 1.0;
		m_FrameScale = 1.0;
	}

	m_FrameOffset.Set((m_VerticalSplit + m_pglWidget->m_GLViewRect.right)/2.0,
					  (m_pglWidget->m_GLViewRect.top +m_pglWidget->m_GLViewRect.bottom )/2.0,
					  0.0);
	m_BodyOffset.Set((2.0*m_pglWidget->m_GLViewRect.left+m_VerticalSplit)/3.0,
					 (m_pglWidget->m_GLViewRect.top+m_HorizontalSplit)/2.0 ,
					 0.0);

	m_FrameScaledOffset.Set((1.0 - m_FrameScale)*m_FrameScalingCenter.x + m_FrameScale * m_FrameOffset.x,
	                        (1.0 - m_FrameScale)*m_FrameScalingCenter.y + m_FrameScale * m_FrameOffset.y,
	                         0.0);

	m_BodyScaledOffset.Set((1.0 - m_BodyScale)*m_BodyScalingCenter.x + m_BodyScale * m_BodyOffset.x,
	                       (1.0 - m_BodyScale)*m_BodyScalingCenter.y + m_BodyScale * m_BodyOffset.y,
	                        0.0);

	CFrame::s_rViewRect = m_FrameRect;
	CBody::s_rViewRect  = m_BodyLineRect;
	m_BodyRefScale  = m_BodyScale;
	m_FrameRefScale = m_FrameScale;
}


void GL3dBodyDlg::SetFrame(int iFrame)
{
	if(!m_pBody) return;
	if(iFrame<0 || iFrame>=m_pBody->m_NStations) m_pFrame = NULL;
	else                                         m_pFrame = m_pBody->m_Frame + iFrame;
	m_pBody->m_iActiveFrame = iFrame;

//	SetFrameSel(iFrame);
	FillPointDataTable();;
//	SetPointSel(pMiarex->m_pFrame->m_iSelect);

	m_bResetglBody2D = true;
}




void GL3dBodyDlg::SetPicture()
{
	m_pBody->Duplicate(m_UndoPic+m_StackPos);
	FillFrameDataTable();
	m_pFrame = m_pBody->m_Frame + m_pBody->m_iActiveFrame;
	FillPointDataTable();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;

	UpdateView();
}


void GL3dBodyDlg::SetupLayout()
{
	int i;
	QString str;

//	QDesktopWidget desktop;
//	QRect r = desktop.screenGeometry();
//	setMaximumHeight(r.height());
//	int r1 = geometry().width();

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	m_pglWidget = new GLWidget(this);
	m_pglWidget->m_iView = 5;
	m_ArcBall.m_pGLWidget = m_pglWidget;

	QGridLayout *ThreeDParams = new QGridLayout;
	m_pctrlAxes       = new QCheckBox(tr("Axes"));
	m_pctrlLight      = new QCheckBox(tr("Light"));
	m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"));
	m_pctrlOutline    = new QCheckBox(tr("Outline"));
	m_pctrlPanels     = new QCheckBox(tr("Panels"));
	m_pctrlShowMasses = new QCheckBox(tr("Masses"));
	m_pctrlAxes->setSizePolicy(szPolicyMinimum);
	m_pctrlLight->setSizePolicy(szPolicyMinimum);
	m_pctrlSurfaces->setSizePolicy(szPolicyMinimum);
	m_pctrlOutline->setSizePolicy(szPolicyMinimum);
	m_pctrlPanels->setSizePolicy(szPolicyMinimum);
	ThreeDParams->addWidget(m_pctrlAxes, 1,1);
	ThreeDParams->addWidget(m_pctrlPanels, 1,2);
	ThreeDParams->addWidget(m_pctrlLight, 1,3);
	ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
	ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	ThreeDParams->addWidget(m_pctrlShowMasses, 2,3);

	m_pctrlX          = new QPushButton(tr("X"));
	m_pctrlY          = new QPushButton(tr("Y"));
	m_pctrlZ          = new QPushButton(tr("Z"));
	m_pctrlIso        = new QPushButton(tr("Iso"));
	m_pctrlPickCenter = new QPushButton(tr("Pick Center"));
	m_pctrlReset      = new QPushButton(tr("Reset Scales"));
	m_pctrlPickCenter->setSizePolicy(szPolicyMinimum);
	m_pctrlPickCenter->setCheckable(true);
	m_pctrlReset->setSizePolicy(szPolicyMinimum);
	m_pctrlX->setSizePolicy(szPolicyMinimum);
	m_pctrlY->setSizePolicy(szPolicyMinimum);
	m_pctrlZ->setSizePolicy(szPolicyMinimum);
	m_pctrlIso->setSizePolicy(szPolicyMinimum);

	QHBoxLayout*AxisView = new QHBoxLayout;
	AxisView->addWidget(m_pctrlX);
	AxisView->addWidget(m_pctrlY);
	AxisView->addWidget(m_pctrlZ);
	QHBoxLayout* ThreeDView = new QHBoxLayout;
	ThreeDView->addWidget(m_pctrlIso);
	ThreeDView->addWidget(m_pctrlReset);

	m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
	m_pctrlClipPlanePos->setMinimum(-300);
	m_pctrlClipPlanePos->setMaximum(300);
	m_pctrlClipPlanePos->setSliderPosition(0);
	m_pctrlClipPlanePos->setTickInterval(30);
	m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
	m_pctrlClipPlanePos->setSizePolicy(szPolicyMinimum);

	QHBoxLayout *ThreeDViewControls = new QHBoxLayout;
	QLabel *ClipLabel = new QLabel(tr("Clip Plane"));
	ClipLabel->setSizePolicy(szPolicyMaximum);
	ThreeDViewControls->addWidget(ClipLabel);
	ThreeDViewControls->addWidget(m_pctrlClipPlanePos);

	QHBoxLayout *ActionButtons = new QHBoxLayout;
	m_pctrlUndo = new QPushButton(QIcon(":/images/OnUndo.png"), tr("Undo"));
	m_pctrlRedo = new QPushButton(QIcon(":/images/OnRedo.png"), tr("Redo"));
	m_pctrlMenuButton = new QPushButton(tr("Other"));

	BodyMenu = new QMenu(tr("Actions..."),this);

	BodyMenu->addAction(m_pGrid);
	BodyMenu->addSeparator();
	BodyMenu->addAction(m_pImportBodyDef);
	BodyMenu->addAction(m_pExportBodyDef);
	BodyMenu->addAction(m_pExportBodyGeom);
	BodyMenu->addSeparator();
	BodyMenu->addAction(m_pTranslateBody);
	BodyMenu->addAction(m_pScaleBody);
	BodyMenu->addSeparator();
	BodyMenu->addAction(m_pInertia);
	m_pctrlMenuButton->setMenu(BodyMenu);

	ActionButtons->addWidget(m_pctrlUndo);
	ActionButtons->addWidget(m_pctrlRedo);
	ActionButtons->addWidget(m_pctrlMenuButton);



	QHBoxLayout *CommandButtons = new QHBoxLayout;
	m_pctrlOK = new QPushButton(tr("Save and Close"));
	m_pctrlOK->setAutoDefault(true);
	m_pctrlCancel = new QPushButton(tr("Cancel"));
	m_pctrlCancel->setAutoDefault(false);
	CommandButtons->addWidget(m_pctrlOK);
	CommandButtons->addWidget(m_pctrlCancel);


	QVBoxLayout *ControlsLayout = new QVBoxLayout;
	ControlsLayout->addLayout(ThreeDParams);
	ControlsLayout->addLayout(AxisView);
	ControlsLayout->addLayout(ThreeDView);
	ControlsLayout->addWidget(m_pctrlPickCenter);
	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(ThreeDViewControls);
	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(ActionButtons);
	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(CommandButtons);

	QHBoxLayout *BodyType = new QHBoxLayout;
	m_pctrlFlatPanels = new QRadioButton(tr("Flat Panels"));
	m_pctrlBSplines = new QRadioButton(tr("BSplines"));
	m_pctrlFlatPanels->setSizePolicy(szPolicyMinimum);
	m_pctrlBSplines->setSizePolicy(szPolicyMinimum);
	BodyType->addWidget(m_pctrlFlatPanels);
	BodyType->addWidget(m_pctrlBSplines);

	QGridLayout *Params = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("x"));
	QLabel *lab2 = new QLabel(tr("Hoop"));
	QLabel *lab3 = new QLabel(tr("Degree"));
	QLabel *lab4 = new QLabel(tr("Panels"));
	m_pctrlXDegree = new QComboBox;
	m_pctrlHoopDegree = new QComboBox;
	m_pctrlNXPanels = new FloatEdit;
	m_pctrlNHoopPanels = new FloatEdit;
	lab1->setSizePolicy(szPolicyMinimum);
	lab2->setSizePolicy(szPolicyMinimum);
	lab3->setSizePolicy(szPolicyMinimum);
	lab4->setSizePolicy(szPolicyMinimum);
	m_pctrlXDegree->setSizePolicy(szPolicyMinimum);
	m_pctrlHoopDegree->setSizePolicy(szPolicyMinimum);
	m_pctrlNXPanels->setSizePolicy(szPolicyMinimum);
	m_pctrlNHoopPanels->setSizePolicy(szPolicyMinimum);
	m_pctrlNXPanels->SetPrecision(0);
	m_pctrlNHoopPanels->SetPrecision(0);
	Params->addWidget(lab1,1,2, Qt::AlignCenter);
	Params->addWidget(lab2,1,3, Qt::AlignCenter);
	Params->addWidget(lab3,2,1, Qt::AlignRight);
	Params->addWidget(lab4,3,1, Qt::AlignRight);
	Params->addWidget(m_pctrlXDegree,2,2);
	Params->addWidget(m_pctrlHoopDegree,2,3);
	Params->addWidget(m_pctrlNXPanels,3,2);
	Params->addWidget(m_pctrlNHoopPanels,3,3);


	QVBoxLayout *BodyParams = new QVBoxLayout;

	QHBoxLayout *BodyDef = new QHBoxLayout;
	m_pctrlBodyName = new QLineEdit(tr("BodyName"));
	m_pctrlBodyStyle = new LineButton;
	m_pctrlBodyStyle->setSizePolicy(szPolicyMinimum);
	BodyDef->addWidget(m_pctrlBodyName);
	BodyDef->addWidget(m_pctrlBodyStyle);

	m_pctrlBodyDescription = new QTextEdit();
	m_pctrlBodyDescription->setToolTip(tr("Enter here a short description for the body"));
	QLabel *BodyDes = new QLabel(tr("Description:"));
	BodyParams->setStretchFactor(m_pctrlBodyDescription,1);

	BodyParams->addLayout(BodyDef);
	BodyParams->addWidget(BodyDes);
	BodyParams->addWidget(m_pctrlBodyDescription);
	BodyParams->addStretch(1);
	BodyParams->addLayout(BodyType);
	BodyParams->addLayout(Params);


	QVBoxLayout * FramePosLayout = new QVBoxLayout;
	m_pctrlFrameTable = new QTableView;
//	m_pctrlFrameTable->setSizePolicy(szPolicyMinimum);
	m_pctrlFrameTable->setWindowTitle(tr("Frames"));
	QLabel *LabelFrame = new QLabel(tr("Frame Positions"));
	LabelFrame->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	FramePosLayout->addWidget(LabelFrame);
	FramePosLayout->addWidget(m_pctrlFrameTable);
//	FramePosLayout->addStretch(1);
	m_pctrlFrameTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlFrameTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlFrameTable->setEditTriggers(QAbstractItemView::CurrentChanged |
									   QAbstractItemView::DoubleClicked |
									   QAbstractItemView::SelectedClicked |
									   QAbstractItemView::EditKeyPressed |
									   QAbstractItemView::AnyKeyPressed);


	QVBoxLayout * FramePointLayout = new QVBoxLayout;
	m_pctrlPointTable = new QTableView;
//	m_pctrlPointTable->setSizePolicy(szPolicyMinimum);
	m_pctrlPointTable->setWindowTitle(tr("Points"));
	QLabel *LabelPoints = new QLabel(tr("Current Frame Definition"));
	LabelPoints->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	FramePointLayout->addWidget(LabelPoints);
	FramePointLayout->addWidget(m_pctrlPointTable);
//	FramePointLayout->addStretch(1);
	m_pctrlPointTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlPointTable->setEditTriggers(QAbstractItemView::CurrentChanged |
									   QAbstractItemView::DoubleClicked |
									   QAbstractItemView::SelectedClicked |
									   QAbstractItemView::EditKeyPressed |
									   QAbstractItemView::AnyKeyPressed);


	QHBoxLayout *AllControls = new QHBoxLayout;
//	AllControls->addLayout(CommandButtons);
//	AllControls->addStretch(1);
	AllControls->addLayout(BodyParams);
	AllControls->addStretch(1);
	AllControls->addLayout(FramePosLayout);
	AllControls->addStretch(1);
	AllControls->addLayout(FramePointLayout);
	AllControls->addStretch(1);
	AllControls->addLayout(ControlsLayout);

	m_pctrlControlsWidget = new QWidget;
	m_pctrlControlsWidget->setLayout(AllControls);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pglWidget,2);
	MainLayout->addWidget(m_pctrlControlsWidget,1);
//	MainLayout->setStretchFactor(m_pglWidget, 3);
//	MainLayout->setStretchFactor(m_pctrlControlsWidget, 1);

	setLayout(MainLayout);

	for (i=1; i<6; i++)
	{
		str = QString("%1").arg(i);
		m_pctrlXDegree->addItem(str);
		m_pctrlHoopDegree->addItem(str);
	}


	m_pFrameModel = new QStandardItemModel;
	m_pFrameModel->setRowCount(10);//temporary
	m_pFrameModel->setColumnCount(3);
	m_pctrlFrameTable->setModel(m_pFrameModel);
	QItemSelectionModel *selectionModelFrame = new QItemSelectionModel(m_pFrameModel);
	m_pctrlFrameTable->setSelectionModel(selectionModelFrame);
	m_pFrameDelegate = new BodyTableDelegate;
	m_pctrlFrameTable->setItemDelegate(m_pFrameDelegate);

	m_pPointModel = new QStandardItemModel;
	m_pPointModel->setRowCount(10);//temporary
	m_pPointModel->setColumnCount(3);
	m_pctrlPointTable->setModel(m_pPointModel);
	QItemSelectionModel *selectionModelPoint = new QItemSelectionModel(m_pPointModel);
	m_pctrlPointTable->setSelectionModel(selectionModelPoint);
	m_pPointDelegate = new BodyTableDelegate;
	m_pctrlPointTable->setItemDelegate(m_pPointDelegate);

	connect(selectionModelPoint, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnPointItemClicked(QModelIndex)));
	connect(selectionModelFrame, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnFrameItemClicked(QModelIndex)));

	m_Precision[0] = 3;//five digits for x and y coordinates
	m_Precision[1] = 3;
	m_Precision[2] = 0;
}



void GL3dBodyDlg::StorePicture()
{
	int i;
	if(m_StackPos>=20)
	{
		for (i=1; i<20; i++)
		{
			m_UndoPic[i-1].Duplicate(m_UndoPic+i);
		}
		m_StackPos = 19;
		m_StackSize = 19;
	}
	m_UndoPic[m_StackPos].Duplicate(&m_TmpPic);

	m_bStored = true;
	m_StackPos++;
	m_StackSize = m_StackPos;

}


void GL3dBodyDlg::ShowContextMenu(QContextMenuEvent * event)
{
	m_pShowCurFrameOnly->setChecked(m_bCurFrameOnly);
	QMenu *CtxMenu = new QMenu(tr("Context Menu"),this);
	CtxMenu->addAction(m_pInsertPoint);
	CtxMenu->addAction(m_pRemovePoint);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pTranslateBody);
	CtxMenu->addAction(m_pScaleBody);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pShowCurFrameOnly);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pResetScales);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pGrid);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pUndo);
	CtxMenu->addAction(m_pRedo);
	CtxMenu->addSeparator();
	CtxMenu->addAction(m_pImportBodyDef);
	CtxMenu->addAction(m_pExportBodyDef);
	CtxMenu->addAction(m_pExportBodyGeom);

	QPoint CltPt = event->pos();
	QPoint ScreenPt = event->pos();

	m_ptPopUp.rx() = CltPt.x();
	m_ptPopUp.ry() = CltPt.y();
	ClientToGL(m_ptPopUp, m_RealPopUp);

	ScreenPt.rx() += geometry().x();
	ScreenPt.ry() += geometry().y();
	CtxMenu->exec(ScreenPt);

	m_pglWidget->setCursor(Qt::CrossCursor);
}



void GL3dBodyDlg::showEvent(QShowEvent *event)
{
	m_bChanged    = false;
//	InitDialog();
	m_bResetglBody = true;
	m_bIs3DScaleSet = false;
	SetBodyScale();


	UpdateView();
}


void GL3dBodyDlg::TakePicture()
{
	m_bChanged = true;
	m_bStored = false;
	m_TmpPic.Duplicate(m_pBody);
	m_pctrlUndo->setEnabled(true);
}


void GL3dBodyDlg::UpdateView()
{
	if(isVisible()) m_pglWidget->updateGL();
}


void  GL3dBodyDlg::wheelEvent(QWheelEvent *event)
{
	QPoint point(event->pos().x() - m_pglWidget->geometry().x(), event->pos().y() - m_pglWidget->geometry().y());
	//The mouse button has been wheeled
	//Process the message
//	point is in client coordinates

	if(m_BodyRect.contains(point))
	{
		if(event->delta()<0) m_glScaled *= (GLfloat)1.06;
		else                 m_glScaled /= (GLfloat)1.06;
	}
	else if(m_BodyLineRect.contains(point))
	{
		if(event->delta()<0) m_BodyScale *= 1.06;
		else                 m_BodyScale /= 1.06;
		m_BodyScaledOffset.Set((1.0 - m_BodyScale)*m_BodyScalingCenter.x + m_BodyScale * m_BodyOffset.x,
							   (1.0 - m_BodyScale)*m_BodyScalingCenter.y + m_BodyScale * m_BodyOffset.y,
								0.0);
		m_bResetglBody2D=true;
	}
	else if(m_FrameRect.contains(point))
	{
		if(event->delta()<0) m_FrameScale *= 1.06;
		else                 m_FrameScale /= 1.06;
		m_FrameScaledOffset.Set((1.0 - m_FrameScale)*m_FrameScalingCenter.x + m_FrameScale * m_FrameOffset.x,
								(1.0 - m_FrameScale)*m_FrameScalingCenter.y + m_FrameScale * m_FrameOffset.y,
								 0.0);

		m_bResetglBody2D=true;
	}
	UpdateView();
	
}










