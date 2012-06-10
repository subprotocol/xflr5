/****************************************************************************

	BodyDlg Class
	Copyright (C) 2009-12 Andre Deperrois adeperrois@xflr5.com

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


#include "../mainframe.h"
#include "../threedwidget.h"
#include "../globals.h"
#include "../misc/LinePickerDlg.h"
#include "../misc/GLLightDlg.h"
#include "../misc/W3dPrefsDlg.h"
#include "./BodyTransDlg.h"
#include "./BodyScaleDlg.h"
#include "./GL3dBodyDlg.h"
#include "./GLCreateBodyLists.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QHeaderView>
#include <math.h>
#include <QtDebug>

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



void* GL3dBodyDlg::s_pMainFrame;
void *GL3dBodyDlg::s_pGLLightDlg;

bool GL3dBodyDlg::s_bSurfaces = true;
bool GL3dBodyDlg::s_bOutline = true;
bool GL3dBodyDlg::s_bAxes = true;
bool GL3dBodyDlg::s_bVLMPanels = false;


int GL3dBodyDlg::s_NHoopPoints = 37;
int GL3dBodyDlg::s_NXPoints = 47;


QPoint GL3dBodyDlg::s_WindowPos=QPoint(20,20);
QSize  GL3dBodyDlg::s_WindowSize=QSize(900, 700);
#ifdef Q_WS_MAC
bool BodyDlg::s_bWindowMaximized=true;
#else
bool GL3dBodyDlg::s_bWindowMaximized=false;
#endif


QList <void*> *GL3dBodyDlg::s_poaBody;

GL3dBodyDlg::GL3dBodyDlg(void *pParent)
{
	setWindowTitle(tr("Body Edition"));
	setWindowFlags(Qt::Window);
//	setSizeGripEnabled(true);

/*	qDebug()<<"_____N=10:";
	int N=10;
//	qDebug()<<(int)7.1 <<(int)7.5<< (int)7.5001<< (int)7.9<< (int)(N/2) << (int)((double)N/2.0);
	for(int i=0; i<N; i++)
	{
		m_TmpPic.m_SplineSurface.Weight(i, N);
	}
	qDebug()<<"_____N=11:";
	N=11;
//	qDebug()<<(int)7.1 <<(int)7.5<< (int)7.5001<< (int)7.9<< (int)(N/2) << (int)((double)N/2.0);
	for(int i=0; i<N; i++)
	{
		m_TmpPic.m_SplineSurface.Weight(i, N);
	}*/

	m_pBody = NULL;

	m_BodyOffset.Set( 0.20, -0.12, 0.0);
	m_FrameOffset.Set(0.80, -0.50, 0.0);
	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_GLList = 0;

	m_ClipPlanePos = 5.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;

	m_StackSize = 0; //the current stacksize
	m_StackPos  = 0; //the current position on the stack
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

	m_pInsertPoint      = new QAction(tr("Insert Point") + QString("\tShift+Click"), this);
	m_pRemovePoint      = new QAction(tr("Remove Point") + QString("\tCtrl+Click"), this);
	m_pScaleBody        = new QAction(tr("Scale"), this);
	m_pGrid             = new QAction(tr("Grid Setup"), this);
	m_pResetScales      = new QAction(tr("Reset Scales")+("\t(R)"), this);
	m_pShowCurFrameOnly = new QAction(tr("Show Current Frame Only"), this);
	m_pShowCurFrameOnly->setCheckable(true);

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

	connect(m_pctrlIso, SIGNAL(clicked()),this, SLOT(On3DIso()));
	connect(m_pctrlX, SIGNAL(clicked()),this, SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()),this, SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()),this, SLOT(On3DTop()));
	connect(m_pctrlReset, SIGNAL(clicked()),this, SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()),this, SLOT(On3DPickCenter()));

	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane()));
	connect(m_pctrlClipPlanePos, SIGNAL(sliderReleased()), this, SLOT(OnClipPlane()));
	connect(m_pctrlEdgeWeight, SIGNAL(sliderReleased()), this, SLOT(OnEdgeWeight()));
	connect(m_pctrlPanelBunch, SIGNAL(sliderMoved(int)), this, SLOT(OnNURBSPanels()));

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



void GL3dBodyDlg::FillFrameCell(int iItem, int iSubItem)
{
	MainFrame *pMainFrame = (MainFrame*)(s_pMainFrame);

	QModelIndex ind;

	switch (iSubItem)
	{
		case 0:
		{
			ind = m_pFrameModel->index(iItem, 0, QModelIndex());
			m_pFrameModel->setData(ind, m_pBody->Frame(iItem)->m_Position.x * pMainFrame->m_mtoUnit);
			break;
		}
		case 1:
		{
			ind = m_pFrameModel->index(iItem, 1, QModelIndex());
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

	m_pFrameModel->setRowCount(m_pBody->FrameSize());

	for(i=0; i<m_pBody->FrameSize(); i++)
	{
		FillFrameTableRow(i);
	}
}


void GL3dBodyDlg::FillFrameTableRow(int row)
{
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ind = m_pFrameModel->index(row, 0, QModelIndex());
	m_pFrameModel->setData(ind, m_pBody->Frame(row)->m_Position.x * pMainFrame->m_mtoUnit);

	ind = m_pFrameModel->index(row, 1, QModelIndex());
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
			m_pPointModel->setData(ind, m_pBody->Frame(l)->m_CtrlPoint[iItem].y * pMainFrame->m_mtoUnit);
			break;
		}
		case 1:
		{
			ind = m_pPointModel->index(iItem, 1, QModelIndex());
			m_pPointModel->setData(ind, m_pBody->Frame(l)->m_CtrlPoint[iItem].z*pMainFrame->m_mtoUnit);

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
	m_pPointModel->setData(ind, m_pFrame->m_CtrlPoint[row].y * pMainFrame->m_mtoUnit);

	ind = m_pPointModel->index(row, 1, QModelIndex());
	m_pPointModel->setData(ind, m_pFrame->m_CtrlPoint[row].z * pMainFrame->m_mtoUnit);

	ind = m_pPointModel->index(row, 2, QModelIndex());
	m_pPointModel->setData(ind, m_pBody->m_hPanels[row]);
}




void GL3dBodyDlg::GLCreateBodyOverlay()
{
	int style, width;

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

		style = DASHLINE;
		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		//Middle Line
		glBegin(GL_LINE_STRIP);
		{
/*			double BodyLength = m_pBody->Frame(m_pBody->FrameSize()-1)->m_Position.x -m_pBody->Frame(0)->m_Position.x;
			for (k=0; k<m_pBody->m_np;k++)
				glVertex3d(m_pBody->m_x[k] * BodyLength + m_pBody->Frame(0)->m_Position.x,
						   m_pBody->m_y[k] * BodyLength, 0.0);*/
		}
		glEnd();

		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}



void GL3dBodyDlg::GLCreateBody2DBodySection()
{
	int i,k, width;
	double zpos;
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
		width = 1;
		glLineWidth(width);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		//Middle Line
//		style = DASHLINE;
		glLineStipple (1, 0xCFCF);
		glBegin(GL_LINE_STRIP);
		{
			for (k=0; k<m_pBody->FrameSize();k++)
			{
				zpos = (m_pBody->Frame(k)->m_CtrlPoint.first().z +m_pBody->Frame(k)->m_CtrlPoint.last().z )/2.0;
				glVertex3d(m_pBody->Frame(k)->m_Position.x,
						   zpos	,
						   0.0);
			}
		}
		glEnd();
		glDisable(GL_LINE_STIPPLE);


		if(m_pBody->m_LineType==BODYPANELTYPE)
		{
			//Top Line
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pBody->FrameSize();k++)
					glVertex3d(m_pBody->Frame(k)->m_Position.x,
							   m_pBody->Frame(k)->m_CtrlPoint[0].z,
							   0.0);
			}
			glEnd();

			//Bottom Line
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pBody->FrameSize();k++)
					glVertex3d(m_pBody->Frame(k)->m_Position.x,
							   m_pBody->Frame(k)->m_CtrlPoint[ m_pBody->Frame(k)->m_CtrlPoint.size()-1].z,
							   0.0);
			}
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
	int k,width;
	double zpos;
	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

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
//		style = 0;
		width = 1;
		glLineWidth(width);

		glDisable(GL_LINE_STIPPLE);
		glColor3d(color.redF(), color.greenF(), color.blueF());


		for (k=0; k<m_pBody->FrameSize();k++)
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
			zpos = (m_pBody->Frame(k)->m_CtrlPoint.first().z + m_pBody->Frame(k)->m_CtrlPoint.last().z ) /2.0;
			glRectd(m_pBody->Frame(k)->m_Position.x -0.006/m_BodyScale,
					zpos                            -0.006/m_BodyScale,
					m_pBody->Frame(k)->m_Position.x +0.006/m_BodyScale,
					zpos                            +0.006/m_BodyScale);
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
//		style = 0;
		width = 1;

		glLineWidth(width);

		glDisable(GL_LINE_STIPPLE);

		for (k=0; k<m_pFrame->m_CtrlPoint.size();k++)
		{
			if(m_pFrame->m_iSelect==k)
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
			glRectd(m_pFrame->m_CtrlPoint[k].y-0.006/m_FrameScale,
					m_pFrame->m_CtrlPoint[k].z-0.006/m_FrameScale,
					m_pFrame->m_CtrlPoint[k].y+0.006/m_FrameScale,
					m_pFrame->m_CtrlPoint[k].z+0.006/m_FrameScale);
		}
	}
	glEndList();

}


void GL3dBodyDlg::GLCreateBodyFrames()
{
	int j,k;
	CVector Point;
	double hinc, u, v;
	int nh,style, width;
	QColor color;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pBody || ! m_pFrame || (m_pBody && m_pBody->m_iActiveFrame<0))
	{
		glNewList(BODYFRAME,GL_COMPILE);
		m_GLList++;
		glEndList();
		return;
	}

	nh = 23;
//	xinc = 0.1;
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

		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);


		glColor3d(color.redF(), color.greenF(), color.blueF());

		if(m_pBody->m_LineType ==BODYSPLINETYPE)
		{
			if(m_pBody->ActiveFrame())
			{
				u = m_pBody->Getu(m_pBody->ActiveFrame()->m_Position.x);

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
		}
		else
		{
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pFrame->m_CtrlPoint.size();k++)
					glVertex3d(m_pFrame->m_CtrlPoint[k].y,
							   m_pFrame->m_CtrlPoint[k].z,
							   0.0);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pFrame->m_CtrlPoint.size();k++)
					glVertex3d(-m_pFrame->m_CtrlPoint[k].y,
								m_pFrame->m_CtrlPoint[k].z,
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
			for(j=0; j<m_pBody->FrameSize(); j++)
			{
				if(j!=m_pBody->m_iActiveFrame)
				{
					if(m_pBody->m_LineType ==BODYSPLINETYPE)
					{
						u = m_pBody->Getu(m_pBody->Frame(j)->m_Position.x);

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
								glVertex3d(m_pBody->Frame(j)->m_CtrlPoint[k].y,
										   m_pBody->Frame(j)->m_CtrlPoint[k].z,
										   0.0);
						}
						glEnd();
						glBegin(GL_LINE_STRIP);
						{
							for (k=0; k<m_pBody->m_NSideLines;k++)
								glVertex3d(m_pBody->Frame(j)->m_CtrlPoint[k].y,
										   m_pBody->Frame(j)->m_CtrlPoint[k].z,
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


		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);


		glColor3d(color.redF(), color.greenF(), color.blueF());


		if(m_pBody->m_LineType ==BODYSPLINETYPE)
		{
			if(m_pBody->ActiveFrame())
			{
				u = m_pBody->Getu(m_pBody->ActiveFrame()->m_Position.x);

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
		}
		else
		{
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pFrame->m_CtrlPoint.size();k++)
					glVertex3d( m_pBody->ActiveFrame()->m_Position.x,
								m_pFrame->m_CtrlPoint[k].y, m_pFrame->m_CtrlPoint[k].z);
			glEnd();
			glBegin(GL_LINE_STRIP);
			for (k=0; k<m_pFrame->m_CtrlPoint.size();k++)
					glVertex3d( m_pBody->ActiveFrame()->m_Position.x,
							   -m_pFrame->m_CtrlPoint[k].y, m_pFrame->m_CtrlPoint[k].z);
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

	double unit = m_BodyGridDlg.m_Unit2 * pMainFrame->m_mtoUnit;

	QFontMetrics fm(pMainFrame->m_TextFont);
	double LabelWidth;
	double h2 = (double)m_3dWidget.geometry().height() /2.0;
	double w2 = (double)m_3dWidget.geometry().width()  /2.0;
	if(w2>h2) LabelWidth = (double)fm.width("-12.34") / w2;
	else      LabelWidth = (double)fm.width("-12.34") /h2;

	glEnable(GL_DEPTH_TEST);
	glDisable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	glColor3d(W3dPrefsDlg::s_3DAxisColor.redF(), W3dPrefsDlg::s_3DAxisColor.greenF(), W3dPrefsDlg::s_3DAxisColor.blueF());
	glLineWidth(W3dPrefsDlg::s_3DAxisWidth);

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
			m_3dWidget.renderText(m_FrameScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale-LabelWidth/2,
									m_FrameScaledOffset.y-0.04,
									0.0,
									strong,
									pMainFrame->m_TextFont);
		}
		for(i=1; i<fabs(m_FrameScaledOffset.x-(m_VerticalSplit))/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_3dWidget.renderText(m_FrameScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale-LabelWidth/2,
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
			m_3dWidget.renderText(m_FrameScaledOffset.x- 0.02 -LabelWidth,
									m_FrameScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit2*m_FrameScale,
									0.0,
									strong);
		}
		for(i=1; i<fabs(-1.0-m_FrameScaledOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_3dWidget.renderText(m_FrameScaledOffset.x- 0.02 -LabelWidth,
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

	double unit = m_BodyGridDlg.m_Unit * pMainFrame->m_mtoUnit;

	QFontMetrics fm(pMainFrame->m_TextFont);
	double LabelWidth;
	double h2 = (double)m_3dWidget.geometry().height() /2.0;
	double w2 = (double)m_3dWidget.geometry().width()  /2.0;
	if(w2>h2) LabelWidth = (double)fm.width("-12.34") / w2;
	else      LabelWidth = (double)fm.width("-12.34") /h2;

	glEnable(GL_DEPTH_TEST);
	glDisable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	glColor3d(W3dPrefsDlg::s_3DAxisColor.redF(), W3dPrefsDlg::s_3DAxisColor.greenF(), W3dPrefsDlg::s_3DAxisColor.blueF());
	glLineWidth(W3dPrefsDlg::s_3DAxisWidth);
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
			m_3dWidget.renderText(m_BodyScaledOffset.x+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale - LabelWidth/2.0,
							    m_BodyScaledOffset.y-0.04,
							    0.0,
							   strong);
		}
		for(i=1; i<fabs(m_BodyScaledOffset.x-(-1.0))/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_3dWidget.renderText(m_BodyScaledOffset.x-(double)i*m_BodyGridDlg.m_Unit*m_BodyScale - LabelWidth/2.0,
							    m_BodyScaledOffset.y-0.04,
							    0.0,
							    strong);
		}
	}

	// Vertical scale____________
	if(fabs(m_glTop-m_HorizontalSplit)/m_BodyScale/m_BodyGridDlg.m_Unit<50)
	{
		glColor3d(W3dPrefsDlg::s_3DAxisColor.redF(), W3dPrefsDlg::s_3DAxisColor.greenF(), W3dPrefsDlg::s_3DAxisColor.blueF());
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
			m_3dWidget.renderText(m_BodyScaledOffset.x- 0.02 - LabelWidth,
									m_BodyScaledOffset.y+(double)i*m_BodyGridDlg.m_Unit*m_BodyScale,
									0.0,
									strong);
		}
		for(i=1; i<fabs(m_HorizontalSplit-m_BodyScaledOffset.y)/m_BodyScale/m_BodyGridDlg.m_Unit; i++)
		{
			strong = QString("%1").arg(-(double)i*unit, 6,'f', 2);
			m_3dWidget.renderText(m_BodyScaledOffset.x- 0.02 - LabelWidth,
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

	int style = W3dPrefsDlg::s_3DAxisStyle;
	int MaxLines = 150;
	int start = 0;
	if(s_bAxes) start = 1;

	glNewList(BODYFRAMEGRID,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		if(s_bAxes)
		{
			// Frame axis____________
			glColor3d(W3dPrefsDlg::s_3DAxisColor.redF(), W3dPrefsDlg::s_3DAxisColor.greenF(), W3dPrefsDlg::s_3DAxisColor.blueF());
			glLineWidth(W3dPrefsDlg::s_3DAxisWidth);

			if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
			else if(style == DOTLINE)        glLineStipple (1, 0x6666);
			else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
			else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
			else                             glLineStipple (1, 0xFFFF);

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

		style = m_BodyGridDlg.m_Style2;
		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

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

		style = m_BodyGridDlg.m_MinStyle2 ;
		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

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

		if(s_bAxes)
		{
			// Frame axis____________
			glColor3d(W3dPrefsDlg::s_3DAxisColor.redF(), W3dPrefsDlg::s_3DAxisColor.greenF(), W3dPrefsDlg::s_3DAxisColor.blueF());
			glLineWidth(W3dPrefsDlg::s_3DAxisWidth);

			style = W3dPrefsDlg::s_3DAxisStyle;
			if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
			else if(style == DOTLINE)        glLineStipple (1, 0x6666);
			else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
			else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
			else                             glLineStipple (1, 0xFFFF);

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

		style = m_BodyGridDlg.m_Style;
		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

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

		style = m_BodyGridDlg.m_MinStyle;
		if     (style == DASHLINE)       glLineStipple (1, 0xCFCF);
		else if(style == DOTLINE)        glLineStipple (1, 0x6666);
		else if(style == DASHDOTLINE)    glLineStipple (1, 0xFF18);
		else if(style == DASHDOTDOTLINE) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

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




void GL3dBodyDlg::GLDraw3D()
{
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
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.3,eye,up);

		if(glIsList(ARCBALL))
		{
			glDeleteLists(ARCBALL,2);
			m_GLList-=2;
		}
		m_3dWidget.CreateArcballList(m_ArcBall, 1.0);
		m_GLList+=2;
	}

	if(m_bResetglBody && m_pBody)
	{
		if(glIsList(BODYGEOMBASE))
		{
			glDeleteLists(BODYGEOMBASE,1);
			glDeleteLists(BODYGEOMBASE+MAXBODIES,1);
			m_GLList -=2;
		}
		if(m_pBody->m_LineType==BODYPANELTYPE)	     GLCreateBody3DFlatPanels(s_pMainFrame, BODYGEOMBASE, m_pBody);
		else if(m_pBody->m_LineType==BODYSPLINETYPE) GLCreateBody3DSplines(s_pMainFrame, BODYGEOMBASE, m_pBody, s_NXPoints, s_NHoopPoints);

		m_bResetglBody = false;
		if(glIsList(BODYMESHBASE))
		{
			glDeleteLists(BODYMESHBASE,1);
			glDeleteLists(BODYMESHBASE+MAXBODIES,1);
			m_GLList -=2;
		}
		GLCreateBodyMesh(s_pMainFrame, BODYMESHBASE, m_pBody);
		m_bResetglBodyMesh = false;
	}
}


void GL3dBodyDlg::GLDrawBodyLegend()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strLengthUnit;

	int width;
	QColor color;

	GetLengthUnit(strLengthUnit, pMainFrame->m_LengthUnit);

//	glNewList(BODYLEGEND,GL_COMPILE);//create 2D Splines or Lines

	//draw view rectangles

	glEnable(GL_DEPTH_TEST);
	glEnable (GL_LINE_STIPPLE);
	glPolygonMode(GL_FRONT,GL_LINE);

	color = pMainFrame->m_TextColor;
//	style = 0;
	width = 3;

	glLineWidth((float)width);
	glLineStipple (1, 0xFFFF);// Solid
	glColor3d(color.redF(),color.greenF(),color.blueF());

	glBegin(GL_LINES);
		glVertex2d(           -1.0, m_HorizontalSplit);
		glVertex2d(m_VerticalSplit, m_HorizontalSplit);
	glEnd();

	glBegin(GL_LINES);
		glVertex2d(m_VerticalSplit, m_3dWidget.m_GLViewRect.bottom);
		glVertex2d(m_VerticalSplit, m_3dWidget.m_GLViewRect.top );
	glEnd();


	if(m_pBody)
	{
		//draw the legend
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		// Draw the labels
		CVector real;
		m_3dWidget.ClientToGL(m_MousePos, real);
		QFontMetrics fm(pMainFrame->m_TextFont);
		int dD = fm.height();

		strong = QString(tr("Frame %1")).arg(m_pBody->m_iActiveFrame+1,2);
		m_3dWidget.renderText(m_FrameRect.left() +dD ,dD,strong, pMainFrame->m_TextFont);

		strong = QString(tr("Scale = %1")).arg(m_FrameScale/m_BodyRefScale,4,'f',2);
		m_3dWidget.renderText(m_FrameRect.left() +dD ,2*dD,strong, pMainFrame->m_TextFont);

		strong = QString(tr("Scale = %1")).arg(m_BodyScale/m_BodyRefScale,4,'f',2);
		m_3dWidget.renderText(m_BodyLineRect.left() +dD ,dD,strong, pMainFrame->m_TextFont);

		if(m_FrameRect.contains(m_MousePos))
		{
			real.x =  (real.x - m_FrameScaledOffset.x)/m_FrameScale;
			real.y =  (real.y - m_FrameScaledOffset.y)/m_FrameScale;
			real.z = 0.0;

			strong = QString("y = %1 ").arg(real.x * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_3dWidget.renderText(m_FrameRect.left() +dD ,3*dD,strong, pMainFrame->m_TextFont);

			strong = QString("z = %1 ").arg(real.y * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_3dWidget.renderText(m_FrameRect.left() +dD ,4*dD,strong, pMainFrame->m_TextFont);
		}
		else if(m_BodyLineRect.contains(m_MousePos))
		{
			real.x =  (real.x - m_BodyScaledOffset.x)/m_BodyScale;
			real.y =  (real.y - m_BodyScaledOffset.y)/m_BodyScale;
			real.z = 0.0;

			strong = QString("x = %1 ").arg(real.x * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_3dWidget.renderText(m_BodyLineRect.left() +dD ,2*dD,strong, pMainFrame->m_TextFont);

			strong = QString("z = %1 ").arg(real.y * pMainFrame->m_mtoUnit,9,'f',3);
			strong += strLengthUnit;
			m_3dWidget.renderText(m_BodyLineRect.left() +dD ,3*dD,strong, pMainFrame->m_TextFont);
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
//	int width;

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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

//	width = m_rCltRect.width();

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
			m_3dWidget.GLSetupLight((GLLightDlg*)s_pGLLightDlg, m_BodyOffset.y, 1.0);

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

			if(s_bAxes)  m_3dWidget.GLDrawAxes(1.0, W3dPrefsDlg::s_3DAxisColor, W3dPrefsDlg::s_3DAxisStyle, W3dPrefsDlg::s_3DAxisWidth);


			if(m_bglLight)
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
			}

			if(s_bSurfaces && m_pBody)	glCallList(BODYGEOMBASE);

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

			if(m_pBody && m_pFrame && (s_bOutline||s_bSurfaces)) glCallList(BODYFRAME3D);
			if(s_bOutline && m_pBody)	glCallList(BODYGEOMBASE+MAXBODIES);
			if(s_bVLMPanels && m_pBody)
			{
				glCallList(BODYMESHBASE);
//				glCallList(BODYMESHBASE+MAXBODIES);
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
		case Qt::Key_R:
		{
			if(m_FrameRect.contains(m_LastPoint))          SetFrameScale();
			else if(m_BodyLineRect.contains(m_LastPoint))  SetBodyLineScale();
			else                                           SetBodyScale();
			UpdateView();
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

		s_WindowPos =  pSettings->value("BodyWindowPos", QPoint(20,20)).toPoint();
		s_WindowSize = pSettings->value("BodyWindowSize", QSize(900,700)).toSize();
	}
	pSettings->endGroup();
	return true;
}


void GL3dBodyDlg::mouseDoubleClickEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	CVector Real;
//	bool bCtrl = false;
//	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	m_3dWidget.ClientToGL(point, Real);

	if(m_3dWidget.geometry().contains(point)) m_3dWidget.setFocus();

	Set3DRotationCenter(point);
	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
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
	m_3dWidget.ClientToGL(point, Real);

	if(!m_3dWidget.hasFocus()) m_3dWidget.setFocus();

	bool bCtrl = false;

	if (event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if (event->buttons()   & Qt::LeftButton)
	{
		if(bCtrl&& m_BodyRect.contains(point))
		{
			//rotate
			m_ArcBall.Move(point.x(), m_3dWidget.geometry().height()-point.y());
			UpdateView();
		}
		else if(m_bTrans)
		{
			//translate
			if(m_BodyRect.contains(point))
			{
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0/m_glScaled/m_3dWidget.geometry().width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0/m_glScaled/m_3dWidget.geometry().width());

				m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
			else if (m_BodyLineRect.contains(point) && m_pBody)
			{
				m_BodyOffset.x  +=  (double)Delta.x()*2.0/(double)m_3dWidget.geometry().width()/m_BodyScale;
				m_BodyOffset.y  += -(double)Delta.y()*2.0/(double)m_3dWidget.geometry().width()/m_BodyScale;
				m_BodyScaledOffset.Set((1.0-m_BodyScale)*m_BodyScalingCenter.x + m_BodyScale * m_BodyOffset.x,
									   (1.0-m_BodyScale)*m_BodyScalingCenter.y + m_BodyScale * m_BodyOffset.y,
									   0.0);
				UpdateView();
			}
			else if(m_FrameRect.contains(point) && m_pFrame)
			{
				m_FrameOffset.x +=  (double)Delta.x()*2.0/(double)m_3dWidget.geometry().width()/m_FrameScale;
				m_FrameOffset.y += -(double)Delta.y()*2.0/(double)m_3dWidget.geometry().width()/m_FrameScale;
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
			if (n>=0 && n<=m_pBody->FrameSize() && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				m_pFrame = m_pBody->ActiveFrame();
				m_pBody->Frame(n)->m_Position.x = Real.x;

				double zpos = (m_pBody->Frame(n)->m_CtrlPoint.first().z + m_pBody->Frame(n)->m_CtrlPoint.last().z)/2.0;
				for(int ic=0; ic<m_pBody->Frame(n)->m_CtrlPoint.size(); ic++)
				{
					m_pBody->Frame(n)->m_CtrlPoint[ic].x  = Real.x;
					m_pBody->Frame(n)->m_CtrlPoint[ic].z += Real.y-zpos;
				}
				m_bTrans = false;
				m_bResetglBody2D = true;
			}

			UpdateView();
		}
		else if(m_FrameRect.contains(point) && m_pFrame)
		{
			Real.x =  (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.z =   m_pFrame->m_Position.x;

			if(m_pFrame)	n = m_pFrame->m_iSelect;
			else			n = -10;
			if (n>0 && n<m_pFrame->m_CtrlPoint.size()-1 && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				if(Real.x<0.0) 	m_pFrame->m_CtrlPoint[n].y = 0.0;
				else            m_pFrame->m_CtrlPoint[n].y = Real.x;
				m_pFrame->m_CtrlPoint[n].z = Real.y;
//				m_pBody->ComputeCenterLine();
				m_bTrans = false;
				m_bResetglBody2D = true;
			}
			else if ((n==0 || n==m_pFrame->m_CtrlPoint.size()-1)  && !m_bTrans && m_bDragPoint)
			{
				//dragging a point
				m_pFrame->m_CtrlPoint[n].y = 0.0;
				m_pFrame->m_CtrlPoint[n].y = Real.x;//TODO : remove
				m_pFrame->m_CtrlPoint[n].z = Real.y;
//				m_pBody->ComputeCenterLine();
				m_bTrans = false;
				m_bResetglBody2D = true;
			}

			if(m_pFrame->m_CtrlPoint[n].y<0.0) m_pFrame->m_CtrlPoint[n].y=0.0;

			UpdateView();
		}
	}
	else if (event->buttons() & Qt::MidButton)
	{
		if(m_BodyRect.contains(point))
		{
			//rotate
			m_ArcBall.Move(point.x(), m_3dWidget.geometry().height()-point.y());
			UpdateView();
		}
	}
	else 
	{
		//Highlight points
		if (m_BodyLineRect.contains(point) && m_pBody)
		{
			Real.x = (Real.x - m_BodyScaledOffset.x)/m_BodyScale;
			Real.y = (Real.y - m_BodyScaledOffset.y)/m_BodyScale;
			Real.z = 0.0;
			int n = m_pBody->IsFramePos(Real, m_BodyScale/m_BodyRefScale);
			m_pBody->m_iHighlight = -10;
			if (n>=0 && n<=m_pBody->FrameSize())
			{
				m_pBody->m_iHighlight = n;
			}
			m_bResetglBodyPoints = true;
			UpdateView();
		}
		else if (m_pFrame && m_FrameRect.contains(point))
		{
			Real.z = (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.y = (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.x = m_pFrame->m_Position.x;

			int n = m_pFrame->IsPoint(Real, m_FrameScale/m_FrameRefScale);
			m_pFrame->m_iHighlight = -10;
			if (n>=0 && n<=m_pFrame->m_CtrlPoint.size())
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
	bool bShift = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;
	if(event->modifiers() & Qt::ShiftModifier) bShift =true;

	m_3dWidget.ClientToGL(point, Real);

	if(m_3dWidget.geometry().contains(point)) m_3dWidget.setFocus();


	if(m_bPickCenter)
	{
		Set3DRotationCenter(point);
		m_bPickCenter = false;
		m_pctrlPickCenter->setChecked(false);
	}
	if (event->buttons() & Qt::MidButton)
	{
		if(m_BodyRect.contains(point))
		{
			m_bArcball = true;
			m_ArcBall.Start(event->pos().x(), m_3dWidget.geometry().height()-event->pos().y());
			m_bCrossPoint = true;

			Set3DRotationCenter();
			UpdateView();
		}
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		m_ptPopUp = event->pos();
		m_bTrans=true;
		if(m_pBody && m_BodyRect.contains(point))
		{
			m_ArcBall.Start(point.x(), m_3dWidget.geometry().height()-point.y());
			m_bCrossPoint = true;
			Set3DRotationCenter();
			if (!bCtrl)
			{
				m_bTrans = true;
				m_3dWidget.setCursor(Qt::ClosedHandCursor);
			}
			else
			{
				m_bArcball = true;
			}
			UpdateView();
		}
		else if(bShift)  Insert(Real);
		else if(bCtrl)   Remove(Real);
		else if(m_pBody && m_BodyLineRect.contains(point))
		{
			Real.x =  (Real.x - m_BodyScaledOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyScaledOffset.y)/m_BodyScale;
			Real.z = 0.0;
			iF = m_pBody->IsFramePos(Real, m_BodyScale/m_BodyRefScale);
			if(iF >=0)
			{
				TakePicture();
				StorePicture();
				m_pBody->m_iActiveFrame = iF;
				m_pFrame = m_pBody->ActiveFrame();

				SetFrame(m_pBody->m_iActiveFrame);
				m_pctrlFrameTable->selectRow(iF);
				if(m_pFrame && m_pFrame->m_iSelect>=0) m_pctrlPointTable->selectRow(m_pFrame->m_iSelect);

				m_bTrans = false;
				m_bDragPoint  = true;
				UpdateView();
			}
		}
		else if(m_pFrame && m_FrameRect.contains(point))
		{
			Real.z =  (Real.y - m_FrameScaledOffset.y)/m_FrameScale;
			Real.y =  (Real.x - m_FrameScaledOffset.x)/m_FrameScale;
			Real.x = m_pFrame->m_Position.x;

			m_pFrame->m_iSelect = m_pFrame->IsPoint(Real, m_FrameScale/m_FrameRefScale);
			if(m_pFrame->m_iSelect >=0)
			{
				TakePicture();
				StorePicture();
				m_bTrans = false;
				m_bDragPoint  = true;
				m_pctrlPointTable->selectRow(m_pFrame->m_iSelect);
			}
		}
		if(m_bTrans && !bCtrl)	m_3dWidget.setCursor(Qt::ClosedHandCursor);
	}
	

	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
	m_PointDown = point;
	m_LastPoint = point;
}



void GL3dBodyDlg::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	m_3dWidget.setCursor(Qt::CrossCursor);

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
			if(m_pFrame)	n2 = m_pFrame->m_iSelect;
			else			n2 = -10;

			if (m_BodyLineRect.contains(point) && n1>=0 && n1<m_pBody->FrameSize())
			{	//the user has been dragging a point
				FillFrameCell(n1,0);
				FillFrameCell(n1,1);
				SetFrame(n1);
				m_bResetglBody     = true;
				m_bResetglBodyMesh = true;
				m_bResetglBody2D   = true;
			}
			else if (m_FrameRect.contains(point) && n2>=0 && n2<m_pFrame->m_CtrlPoint.size())
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



void GL3dBodyDlg::On3DIso()
{
	SetViewControls();
	m_pctrlIso->setChecked(true);

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
	SetViewControls();
	m_pctrlZ->setChecked(true);
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::On3DLeft()
{
	SetViewControls();
	m_pctrlY->setChecked(true);
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::On3DFront()
{
	SetViewControls();
	m_pctrlX->setChecked(true);
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dBodyDlg::SetViewControls()
{
	m_pctrlX->setChecked(false);
	m_pctrlY->setChecked(false);
	m_pctrlZ->setChecked(false);
	m_pctrlIso->setChecked(false);
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
	s_bAxes = m_pctrlAxes->isChecked();
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


void GL3dBodyDlg::OnClipPlane()
{
	double planepos =  (double)m_pctrlClipPlanePos->sliderPosition()/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}




void GL3dBodyDlg::OnExportBodyDef()
{
	if(!m_pBody) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName;

	FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(pMainFrame, QObject::tr("Export Body Definition"),
											pMainFrame->m_LastDirName,
											QObject::tr("Text Format (*.txt)"));
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return;

	QTextStream out(&XFile);

	m_pBody->ExportDefinition(out, pMainFrame->m_mtoUnit);
	XFile.close();

}


void GL3dBodyDlg::OnExportBodyGeom()
{
	if(!m_pBody) return;
	QString LengthUnit, FileName;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	GetLengthUnit(LengthUnit, pMainFrame->m_LengthUnit);

	FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");

	int type = 1;

	QString filter =".csv";

	FileName = QFileDialog::getSaveFileName(pMainFrame, QObject::tr("Export Body Geometry"),
											pMainFrame->m_LastDirName ,
											QObject::tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) type = 2;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	m_pBody->ExportGeometry(out, pMainFrame->m_mtoUnit, type, s_NXPoints, s_NHoopPoints);
}


void GL3dBodyDlg::OnImportBodyDef()
{
	CBody *pNewBody = new CBody();
	if(!pNewBody) return;


	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;


	double mtoUnit;

	UnitsDlg Dlg;

	Dlg.m_bLengthOnly = true;
	Dlg.m_Length    = pMainFrame->m_LengthUnit;
	Dlg.m_Area      = pMainFrame->m_AreaUnit;
	Dlg.m_Speed     = pMainFrame->m_SpeedUnit;
	Dlg.m_Weight    = pMainFrame->m_WeightUnit;
	Dlg.m_Force     = pMainFrame->m_ForceUnit;
	Dlg.m_Moment    = pMainFrame->m_MomentUnit;
	Dlg.m_Question = QObject::tr("Choose the length unit to read this file :");
	Dlg.InitDialog();

	if(Dlg.exec() == QDialog::Accepted)
	{
		switch(Dlg.m_Length)
		{
			case 0:{//mdm
				mtoUnit  = 1000.0;
				break;
			}
			case 1:{//cm
				mtoUnit  = 100.0;
				break;
			}
			case 2:{//dm
				mtoUnit  = 10.0;
				break;
			}
			case 3:{//m
				mtoUnit  = 1.0;
				break;
			}
			case 4:{//in
				mtoUnit  = 1000.0/25.4;
				break;
			}
			case 5:{///ft
				mtoUnit  = 1000.0/25.4/12.0;
				break;
			}
			default:{//m
				mtoUnit  = 1.0;
				break;
			}
		}
	}
	else return;

	QString PathName;

	PathName = QFileDialog::getOpenFileName(pMainFrame, QObject::tr("Open File"),
											pMainFrame->m_LastDirName,
											QObject::tr("All files (*.*)"));
	if(!PathName.length()) return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = PathName.left(pos);

	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = QObject::tr("Could not read the file\n")+PathName;
		QMessageBox::warning(pMainFrame, QObject::tr("Warning"), strange);
		return;
	}

	QTextStream in(&XFile);

	if(!pNewBody->ImportDefinition(in, pMainFrame->m_mtoUnit))
	{
		delete pNewBody;
		return;
	}

	XFile.close();

	SetBody(pNewBody);
}


void GL3dBodyDlg::OnFrameCellChanged(QWidget *pWidget)
{
	TakePicture();
	StorePicture();
	m_bChanged = true;
//	int n = m_pBody->m_iActiveFrame;
	ReadFrameSectionData(m_pBody->m_iActiveFrame);
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



void GL3dBodyDlg::OnInsert()
{
	Insert(m_RealPopUp);
}

void GL3dBodyDlg::Insert(CVector Pt)
{
	CVector Real;
	m_bChanged = true;
	int FrameSel = 0;
	if(m_BodyLineRect.contains(m_ptPopUp))
	{
		TakePicture();
		StorePicture();

		Real.x = (Pt.x - m_BodyScaledOffset.x)/m_BodyScale;
		Real.z = (Pt.y - m_BodyScaledOffset.y)/m_BodyScale;
		Real.y = 0.0;

		if(m_pFrame)
		{
			FrameSel = m_pBody->InsertFrame(Real);
			if(FrameSel>0) m_pFrame = m_pBody->Frame(FrameSel);
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

		Real.x = (Pt.x - m_FrameScaledOffset.x)/m_FrameScale;
		Real.y = (Pt.y - m_FrameScaledOffset.y)/m_FrameScale;
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
		m_pBody->m_LineType = BODYPANELTYPE;
		m_pctrlNXPanels->setEnabled(false);
		m_pctrlNHoopPanels->setEnabled(false);
		m_pctrlXDegree->setEnabled(false);
		m_pctrlHoopDegree->setEnabled(false);
	}
	else
	{
		m_pBody->m_LineType = BODYSPLINETYPE;
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
	s_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


void GL3dBodyDlg::OnPanels()
{
	s_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}



void GL3dBodyDlg::OnPointCellChanged(QWidget *pWidget)
{
	if(!m_pFrame) return;
	TakePicture();
	StorePicture();
	m_bChanged = true;
	ReadPointSectionData(m_pFrame->m_iSelect);
	m_bResetglBodyPoints = true;
	m_bResetglBody       = true;
	m_bResetglBody2D     = true;
	m_bResetglBodyMesh   = true;
	UpdateView();
}


void GL3dBodyDlg::OnPointItemClicked(const QModelIndex &index)
{
	if(!m_pFrame) return;
	m_pFrame->m_iSelect = index.row();
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
	Remove(m_RealPopUp);
}


void GL3dBodyDlg::Remove(CVector Pt)
{
	int i,n;
	CVector Real;

	CFrame *pBodyFrame;

	if(m_BodyLineRect.contains(m_ptPopUp))
	{
		TakePicture();
		StorePicture();

		Real.x =  (Pt.x - m_BodyScaledOffset.x)/m_BodyScale;
		Real.y =  (Pt.y - m_BodyScaledOffset.y)/m_BodyScale;
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

		Real.z =  (Pt.y - m_FrameScaledOffset.y)/m_FrameScale;
		Real.y =  (Pt.x - m_FrameScaledOffset.x)/m_FrameScale;
		Real.x = 0.0;

		n =   m_pFrame->IsPoint(Real, m_FrameScale/m_FrameRefScale);
		if (n>=0)
		{
			for (i=0; i<m_pBody->FrameSize();i++)
			{
				pBodyFrame = m_pBody->Frame(i);
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
	if(m_FrameRect.contains(m_ptPopUp))         SetFrameScale();
	else if(m_BodyLineRect.contains(m_ptPopUp)) SetBodyLineScale();
	else                                        SetBodyScale();

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
	m_bChanged = true;

	m_pBody->m_SplineSurface.m_iuDegree = sel+1;
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

	m_pBody->m_SplineSurface.m_ivDegree = sel+1;
	m_pBody->SetKnots();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	UpdateView();
}


void GL3dBodyDlg::OnEdgeWeight()
{
	if(!m_pBody) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);

	m_bChanged = true;
	TakePicture();
	StorePicture();

	double w= (double)m_pctrlEdgeWeight->value()/100.0 + 1.0;
	m_pBody->SetEdgeWeight(w, w);

	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	m_bResetglBodyMesh = true;
	UpdateView();
}



void GL3dBodyDlg::OnNURBSPanels()
{
	if(!m_pBody) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);

	m_bChanged = true;
	TakePicture();
	StorePicture();

	m_pBody->m_Bunch = m_pctrlPanelBunch->sliderPosition()/100.0;

	m_pBody->m_nhPanels = m_pctrlNHoopPanels->Value();
	m_pBody->m_nxPanels = m_pctrlNXPanels->Value();
	m_pBody->SetPanelPos();

	m_bResetglBody   = true;
	m_bResetglBody2D = true;
	m_bResetglBodyMesh = true;
	UpdateView();
}



void GL3dBodyDlg::OnShowCurFrameOnly()
{
	m_bCurFrameOnly = !m_bCurFrameOnly;
	m_bResetglBody2D = true;
	m_pShowCurFrameOnly->setChecked(m_bCurFrameOnly);
	UpdateView();
}


void GL3dBodyDlg::OnSurfaces()
{
	s_bSurfaces = m_pctrlSurfaces->isChecked();
	UpdateView();
}


void GL3dBodyDlg::OnTranslateBody()
{
	if(!m_pBody) return;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	BodyTransDlg dlg;
	BodyTransDlg::s_pMainFrame = s_pMainFrame;
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
	d = strong.toDouble(&bOK);
	if(bOK) m_pBody->Frame(sel)->m_Position.x = d / pMainFrame->m_mtoUnit;

	pItem = m_pFrameModel->item(sel,1);
	strong = pItem->text();
	strong.replace(" ","");
	k = strong.toInt(&bOK);
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
	if(bOK) m_pFrame->m_CtrlPoint[sel].y =d / pMainFrame->m_mtoUnit;

	pItem = m_pPointModel->item(sel,1);
	strong = pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pFrame->m_CtrlPoint[sel].z =d / pMainFrame->m_mtoUnit;

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
//	SetBodyScale();
//	SetRectangles();

	ResizeTables();
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

		pSettings->setValue("BodyWindowPos",s_WindowPos);
		pSettings->setValue("BodyWindowSize",s_WindowSize);

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

	int  i, j;
	CVector  A, B, AA, BB, PP, U;

	i=-1;

	m_3dWidget.ClientToGL(point, B);

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

	bIntersect = m_pBody->Intersect(AA,BB, PP, true);
	if(!bIntersect) bIntersect = m_pBody->Intersect(AA,BB, PP, false);

	if(bIntersect)
	{
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




void GL3dBodyDlg::SetControls()
{
	m_pctrlBodyName->setEnabled(m_bEnableName);

	m_pctrlOutline->setChecked(s_bOutline);
	m_pctrlPanels->setChecked(s_bVLMPanels);
	m_pctrlAxes->setChecked(s_bAxes);
	m_pctrlLight->setChecked(m_bglLight);
	m_pctrlShowMasses->setChecked(m_bShowMasses);
	m_pctrlSurfaces->setChecked(s_bSurfaces);
	m_pctrlOutline->setChecked(s_bOutline);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));
	m_pctrlSurfaces->setChecked(s_bSurfaces);

	m_pctrlPanelBunch->setSliderPosition((int)(m_pBody->m_Bunch*100.0));

	m_pctrlUndo->setEnabled(m_StackPos>0);
	m_pctrlRedo->setEnabled(m_StackPos<m_StackSize);

	m_pctrlEdgeWeight->setSliderPosition((int)((m_pBody->m_SplineSurface.m_EdgeWeightu-1.0)*100.0));

	if(m_pBody && m_pBody->m_LineType==BODYPANELTYPE)
	{
		m_pctrlNXPanels->setEnabled(false);
		m_pctrlNHoopPanels->setEnabled(false);
		m_pctrlXDegree->setEnabled(false);
		m_pctrlHoopDegree->setEnabled(false);
	}
	else if(m_pBody && m_pBody->m_LineType==BODYSPLINETYPE)
	{
		m_pctrlNXPanels->setEnabled(true);
		m_pctrlNHoopPanels->setEnabled(true);
		m_pctrlXDegree->setEnabled(true);
		m_pctrlHoopDegree->setEnabled(true);
	}

	m_pctrlBodyStyle->SetStyle(m_pBody->m_BodyStyle);
	m_pctrlBodyStyle->SetWidth(m_pBody->m_BodyWidth);
	m_pctrlBodyStyle->SetColor(m_pBody->m_BodyColor);

	m_pctrlNXPanels->SetValue(m_pBody->m_nxPanels);
	m_pctrlNHoopPanels->SetValue(m_pBody->m_nhPanels);

	m_pctrlXDegree->setCurrentIndex(m_pBody->m_SplineSurface.m_iuDegree-1);
	m_pctrlHoopDegree->setCurrentIndex(m_pBody->m_SplineSurface.m_ivDegree-1);
}


bool GL3dBodyDlg::InitDialog(CBody *pBody)
{
	if(!pBody) return false;
	return SetBody(pBody);
}


bool GL3dBodyDlg::SetBody(CBody *pBody)
{
	m_pBody = pBody;
	if(!m_pBody) return false;


	if(m_pBody->m_LineType==BODYPANELTYPE)       m_pctrlFlatPanels->setChecked(true);
	else if(m_pBody->m_LineType==BODYSPLINETYPE) m_pctrlBSplines->setChecked(true);


	m_pFrame = m_pBody->ActiveFrame();
	m_bResetglBody2D = true;

	SetControls();
	FillFrameDataTable();
	FillPointDataTable();


	m_pctrlBodyName->setText(pBody->m_BodyName);
	return true;
}



void GL3dBodyDlg::SetBodyLineScale()
{
	if(m_pBody) m_BodyScale = (m_VerticalSplit-m_3dWidget.m_GLViewRect.left)*.7/m_pBody->Length();
	else        m_BodyScale = 1.0;

	m_BodyOffset.Set((m_3dWidget.m_GLViewRect.left+m_VerticalSplit)/2.0-m_pBody->Length()/2.0-m_pBody->LeadingPoint().x,
					 (m_3dWidget.m_GLViewRect.top+m_HorizontalSplit)/2.0,
					  0.0);

	m_BodyScaledOffset.Set((1.0 - m_BodyScale)*m_BodyScalingCenter.x + m_BodyScale * m_BodyOffset.x,
						   (1.0 - m_BodyScale)*m_BodyScalingCenter.y + m_BodyScale * m_BodyOffset.y,
							0.0);
	m_BodyRefScale  = m_BodyScale;
	m_BodyScalingCenter.x  = (m_VerticalSplit                + m_3dWidget.m_GLViewRect.left)   /2.0;
	m_BodyScalingCenter.y  = (m_3dWidget.m_GLViewRect.top  + m_HorizontalSplit)                /2.0;
}


void GL3dBodyDlg::SetFrameScale()
{
	int k;
	double height;
	if(m_pBody)
	{
		height = 0.0;
		for(k=0; k<m_pBody->FrameSize(); k++)
		{
			height = qMax(height,m_pBody->Frame(k)->Height());
		}
		m_FrameScale = (1.0-0.5)/height;
	}
	else m_FrameScale = 1.0;

	m_FrameOffset.Set((m_VerticalSplit + m_3dWidget.m_GLViewRect.right)/2.0,
				   (m_3dWidget.m_GLViewRect.top +m_3dWidget.m_GLViewRect.bottom)/2.0,
				   0.0);


	m_FrameScaledOffset.Set((1.0 - m_FrameScale)*m_FrameScalingCenter.x + m_FrameScale * m_FrameOffset.x,
							(1.0 - m_FrameScale)*m_FrameScalingCenter.y + m_FrameScale * m_FrameOffset.y,
						0.0);

	m_FrameRefScale = m_FrameScale;
	m_FrameScalingCenter.x = (m_VerticalSplit                + m_3dWidget.m_GLViewRect.right)  /2.0;
	m_FrameScalingCenter.y = (m_3dWidget.m_GLViewRect.top  + m_3dWidget.m_GLViewRect.bottom) /2.0;


}

void GL3dBodyDlg::SetRectangles()
{
	CVector V1, V2;
	QPoint P1, P2;

	m_glTop = m_3dWidget.m_GLViewRect.top;

	m_VerticalSplit     = m_3dWidget.m_GLViewRect.width()  /6.0;
	m_HorizontalSplit   = m_3dWidget.m_GLViewRect.height() /6.0;

	V1.Set(m_3dWidget.m_GLViewRect.left, m_3dWidget.m_GLViewRect.top, 0.0);
	V2.Set(m_VerticalSplit,                m_HorizontalSplit,             0.0);
	m_3dWidget.GLToClient(V1, P1);
	m_3dWidget.GLToClient(V2, P2);
	m_BodyLineRect.setTopLeft(P1);
	m_BodyLineRect.setBottomRight(P2);

	V1.Set(m_VerticalSplit,                 m_3dWidget.m_GLViewRect.top,    0.0);
	V2.Set(m_3dWidget.m_GLViewRect.right, m_3dWidget.m_GLViewRect.bottom, 0.0);
	m_3dWidget.GLToClient(V1, P1);
	m_3dWidget.GLToClient(V2, P2);
	m_FrameRect.setTopLeft(P1);
	m_FrameRect.setBottomRight(P2);

	V1.Set(m_3dWidget.m_GLViewRect.left, m_HorizontalSplit,                0.0);
	V2.Set(m_VerticalSplit,                m_3dWidget.m_GLViewRect.bottom, 0.0);
	m_3dWidget.GLToClient(V1, P1);
	m_3dWidget.GLToClient(V2, P2);
	m_BodyRect.setTopLeft(P1);
	m_BodyRect.setBottomRight(P2);
}


void GL3dBodyDlg::SetBodyScale()
{
//	if(m_bIs3DScaleSet /*&& !m_bAutoScales*/) return;

	m_glViewportTrans.x = 0.0;
	m_glViewportTrans.y = 0.0;
	m_glViewportTrans.z = 0.0;
	m_bIs3DScaleSet = true;


	m_UFOOffset.x = (GLfloat)(m_3dWidget.m_GLViewRect.left + m_VerticalSplit  )/2.0;
	m_UFOOffset.y = (GLfloat)(m_3dWidget.m_GLViewRect.bottom + m_HorizontalSplit)/2.0;
  
	m_glScaled = (GLfloat)(3./4.* (m_VerticalSplit+1.0) / m_pBody->Length());

	m_ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	m_ArcBall.SetZoom(0.3,eye,up);

	Set3DRotationCenter();
}


void GL3dBodyDlg::SetScales()
{
	SetBodyScale();
	SetFrameScale();
	SetBodyLineScale();
}



void GL3dBodyDlg::SetFrame(int iFrame)
{
	if(!m_pBody) return;
	if(iFrame<0 || iFrame>=m_pBody->FrameSize()) m_pFrame = NULL;
	else                                         m_pFrame = m_pBody->Frame(iFrame);
	m_pBody->m_iActiveFrame = iFrame;

	FillPointDataTable();;

	m_bResetglBody2D = true;
}

void GL3dBodyDlg::SetFrame(CFrame *pFrame)
{
	if(!m_pBody || !pFrame) return;

	m_pBody->SetActiveFrame(pFrame);

	FillPointDataTable();;

	m_bResetglBody2D = true;
}


void GL3dBodyDlg::SetPicture()
{
	m_pBody->Duplicate(m_UndoPic+m_StackPos);
	m_pBody->SetKnots();
	FillFrameDataTable();
	m_pFrame = m_pBody->ActiveFrame();
	FillPointDataTable();
	m_bResetglBody   = true;
	m_bResetglBody2D = true;

	UpdateView();
}


void GL3dBodyDlg::SetupLayout()
{
	int i;
	QString str;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString length;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	m_3dWidget.m_pParent = this;
	m_3dWidget.m_iView = GLBODYVIEW;
	m_ArcBall.m_p3dWidget = &m_3dWidget;


	QVBoxLayout *ControlsLayout = new QVBoxLayout;
	{
		QGridLayout *ThreeDParams = new QGridLayout;
		{
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

		}

		QHBoxLayout*AxisView = new QHBoxLayout;
		{
			m_pctrlX          = new QToolButton;
			m_pctrlY          = new QToolButton;
			m_pctrlZ          = new QToolButton;
			m_pctrlIso        = new QToolButton;
			if(m_pctrlX->iconSize().height()<=48)
			{
				m_pctrlX->setIconSize(QSize(32,32));
				m_pctrlY->setIconSize(QSize(32,32));
				m_pctrlZ->setIconSize(QSize(32,32));
				m_pctrlIso->setIconSize(QSize(32,32));
			}
			m_pXView = new QAction(QIcon(":/images/OnXView.png"), tr("X View"), this);
			m_pYView = new QAction(QIcon(":/images/OnYView.png"), tr("Y View"), this);
			m_pZView = new QAction(QIcon(":/images/OnZView.png"), tr("Z View"), this);
			m_pIsoView = new QAction(QIcon(":/images/OnIsoView.png"), tr("Iso View"), this);
			m_pXView->setCheckable(true);
			m_pYView->setCheckable(true);
			m_pZView->setCheckable(true);
			m_pIsoView->setCheckable(true);

			m_pctrlX->setDefaultAction(m_pXView);
			m_pctrlY->setDefaultAction(m_pYView);
			m_pctrlZ->setDefaultAction(m_pZView);
			m_pctrlIso->setDefaultAction(m_pIsoView);

			AxisView->addWidget(m_pctrlX);
			AxisView->addWidget(m_pctrlY);
			AxisView->addWidget(m_pctrlZ);
			AxisView->addWidget(m_pctrlIso);
		}

		QHBoxLayout* ThreeDView = new QHBoxLayout;
		{
			m_pctrlPickCenter = new QPushButton(tr("Pick Center"));
			m_pctrlPickCenter->setSizePolicy(szPolicyMinimum);
			m_pctrlPickCenter->setCheckable(true);
			m_pctrlReset      = new QPushButton(tr("Reset Scales"));
			m_pctrlReset->setSizePolicy(szPolicyMinimum);

			ThreeDView->addWidget(m_pctrlReset);
			ThreeDView->addWidget(m_pctrlPickCenter);
		}

		QHBoxLayout *ThreeDViewControls = new QHBoxLayout;
		{
			QLabel *ClipLabel = new QLabel(tr("Clip Plane"));
			ClipLabel->setSizePolicy(szPolicyMaximum);
			m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
			m_pctrlClipPlanePos->setMinimum(-300);
			m_pctrlClipPlanePos->setMaximum(300);
			m_pctrlClipPlanePos->setSliderPosition(0);
			m_pctrlClipPlanePos->setTickInterval(30);
			m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
			m_pctrlClipPlanePos->setSizePolicy(szPolicyMinimum);
			ThreeDViewControls->addWidget(ClipLabel);
			ThreeDViewControls->addWidget(m_pctrlClipPlanePos);
		}

		QHBoxLayout *ActionButtons = new QHBoxLayout;
		{
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
			m_pctrlMenuButton->setMenu(BodyMenu);

			ActionButtons->addWidget(m_pctrlUndo);
			ActionButtons->addWidget(m_pctrlRedo);
			ActionButtons->addWidget(m_pctrlMenuButton);
		}

		QHBoxLayout *CommandButtons = new QHBoxLayout;
		{
			m_pctrlOK = new QPushButton(tr("Save and Close"));
			m_pctrlOK->setAutoDefault(true);
			m_pctrlCancel = new QPushButton(tr("Cancel"));
			m_pctrlCancel->setAutoDefault(false);
			CommandButtons->addWidget(m_pctrlOK);
			CommandButtons->addWidget(m_pctrlCancel);
		}

		ControlsLayout->addLayout(AxisView);
		ControlsLayout->addLayout(ThreeDViewControls);
		ControlsLayout->addLayout(ThreeDParams);
		ControlsLayout->addLayout(ThreeDView);
		ControlsLayout->addStretch(1);
		ControlsLayout->addLayout(ActionButtons);
		ControlsLayout->addStretch(1);
		ControlsLayout->addLayout(CommandButtons);
	}



	QVBoxLayout *BodyParams = new QVBoxLayout;
	{
		QHBoxLayout *BodyDef = new QHBoxLayout;
		{
			m_pctrlBodyName = new QLineEdit(tr("BodyName"));
			m_pctrlBodyStyle = new LineButton;
			m_pctrlBodyStyle->setSizePolicy(szPolicyMinimum);
			BodyDef->addWidget(m_pctrlBodyName);
			BodyDef->addWidget(m_pctrlBodyStyle);
		}

		QLabel *BodyDes = new QLabel(tr("Description:"));
		BodyParams->setStretchFactor(m_pctrlBodyDescription,1);

		m_pctrlBodyDescription = new QTextEdit();
		m_pctrlBodyDescription->setToolTip(tr("Enter here a short description for the body"));

		QHBoxLayout *BodyType = new QHBoxLayout;
		{
			m_pctrlFlatPanels = new QRadioButton(tr("Flat Panels"));
			m_pctrlBSplines = new QRadioButton(tr("BSplines"));
			m_pctrlFlatPanels->setSizePolicy(szPolicyMinimum);
			m_pctrlBSplines->setSizePolicy(szPolicyMinimum);
			BodyType->addWidget(m_pctrlFlatPanels);
			BodyType->addWidget(m_pctrlBSplines);
		}

		QGridLayout *SplineParams = new QGridLayout;
		{
			QLabel *lab1 = new QLabel(tr("x"));
			QLabel *lab2 = new QLabel(tr("Hoop"));
			QLabel *lab3 = new QLabel(tr("Degree"));
			QLabel *lab4 = new QLabel(tr("Panels"));
			QLabel *labWeight = new QLabel(tr("Edge Weight"));
			QLabel *labBunch = new QLabel(tr("Panel bunch"));

			m_pctrlXDegree = new QComboBox;
			m_pctrlHoopDegree = new QComboBox;
			m_pctrlNXPanels = new FloatEdit;
			m_pctrlNHoopPanels = new FloatEdit;
			m_pctrlEdgeWeight = new QSlider(Qt::Horizontal);
			m_pctrlEdgeWeight->setMinimum(0);
			m_pctrlEdgeWeight->setMaximum(100);
			m_pctrlEdgeWeight->setSliderPosition(1);
			m_pctrlEdgeWeight->setTickInterval(10);
			m_pctrlEdgeWeight->setTickPosition(QSlider::TicksBelow);
			m_pctrlEdgeWeight->setSizePolicy(szPolicyMinimum);

			m_pctrlPanelBunch= new QSlider(Qt::Horizontal);
			m_pctrlPanelBunch->setMinimum(0	);
			m_pctrlPanelBunch->setMaximum(100.0);
			m_pctrlPanelBunch->setSliderPosition(0);
			m_pctrlPanelBunch->setTickInterval(10);
			m_pctrlPanelBunch->setTickPosition(QSlider::TicksBelow);
			m_pctrlPanelBunch->setSizePolicy(szPolicyMinimum);


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
			SplineParams->addWidget(lab1,1,2, Qt::AlignCenter);
			SplineParams->addWidget(lab2,1,3, Qt::AlignCenter);
			SplineParams->addWidget(lab3,2,1, Qt::AlignRight);
			SplineParams->addWidget(lab4,3,1, Qt::AlignRight);
			SplineParams->addWidget(m_pctrlXDegree,2,2);
			SplineParams->addWidget(m_pctrlHoopDegree,2,3);
			SplineParams->addWidget(m_pctrlNXPanels,3,2);
			SplineParams->addWidget(m_pctrlNHoopPanels,3,3);
			SplineParams->addWidget(labWeight,4,1);
			SplineParams->addWidget(m_pctrlEdgeWeight,4,2,1,2);
			SplineParams->addWidget(labBunch,5,1);
			SplineParams->addWidget(m_pctrlPanelBunch,5,2,1,2);
		}

		BodyParams->addLayout(BodyDef);
		BodyParams->addWidget(BodyDes);
		BodyParams->addWidget(m_pctrlBodyDescription);
		BodyParams->addStretch(1);
		BodyParams->addLayout(BodyType);
		BodyParams->addLayout(SplineParams);
	}


	QVBoxLayout * FramePosLayout = new QVBoxLayout;
	{
		m_pctrlFrameTable = new QTableView;
	//	m_pctrlFrameTable->setSizePolicy(szPolicyMinimum);
		m_pctrlFrameTable->setWindowTitle(tr("Frames"));
		QLabel *LabelFrame = new QLabel(tr("Frame Positions"));
		LabelFrame->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		FramePosLayout->addWidget(LabelFrame);
	//	FramePosLayout->addStretch(1);
		m_pctrlFrameTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pctrlFrameTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pctrlFrameTable->setEditTriggers(QAbstractItemView::CurrentChanged |
										   QAbstractItemView::DoubleClicked |
										   QAbstractItemView::SelectedClicked |
										   QAbstractItemView::EditKeyPressed |
										   QAbstractItemView::AnyKeyPressed);
		FramePosLayout->addWidget(m_pctrlFrameTable);
	}


	QVBoxLayout * FramePointLayout = new QVBoxLayout;
	{
		m_pctrlPointTable = new QTableView;
	//	m_pctrlPointTable->setSizePolicy(szPolicyMinimum);
		m_pctrlPointTable->setWindowTitle(tr("Points"));
		QLabel *LabelPoints = new QLabel(tr("Current Frame Definition"));
		LabelPoints->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		FramePointLayout->addWidget(LabelPoints);
	//	FramePointLayout->addStretch(1);
		m_pctrlPointTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pctrlPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pctrlPointTable->setEditTriggers(QAbstractItemView::CurrentChanged |
										   QAbstractItemView::DoubleClicked |
										   QAbstractItemView::SelectedClicked |
										   QAbstractItemView::EditKeyPressed |
										   QAbstractItemView::AnyKeyPressed);
		FramePointLayout->addWidget(m_pctrlPointTable);
	}

	m_pctrlControlsWidget = new QWidget;
	{
		QHBoxLayout *AllControls = new QHBoxLayout;
		{
			AllControls->addLayout(BodyParams);
			AllControls->addStretch(1);
			AllControls->addLayout(FramePosLayout);
			AllControls->addStretch(1);
			AllControls->addLayout(FramePointLayout);
			AllControls->addStretch(1);
			AllControls->addLayout(ControlsLayout);
		}
		m_pctrlControlsWidget->setLayout(AllControls);
	}


	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(&m_3dWidget,2);
	MainLayout->addWidget(m_pctrlControlsWidget,1);

	setLayout(MainLayout);

	for (i=1; i<6; i++)
	{
		str = QString("%1").arg(i);
		m_pctrlXDegree->addItem(str);
		m_pctrlHoopDegree->addItem(str);
	}

	//Setup Frame table
	m_pctrlFrameTable->horizontalHeader()->setStretchLastSection(true);

	m_pFrameModel = new QStandardItemModel;
	m_pFrameModel->setRowCount(10);//temporary
	m_pFrameModel->setColumnCount(2);
	m_pFrameModel->setHeaderData(0, Qt::Horizontal, "x ("+length+")");
	m_pFrameModel->setHeaderData(1, Qt::Horizontal, tr("NPanels"));
	m_pctrlFrameTable->setModel(m_pFrameModel);

	QItemSelectionModel *selectionModelFrame = new QItemSelectionModel(m_pFrameModel);
	m_pctrlFrameTable->setSelectionModel(selectionModelFrame);
	connect(selectionModelFrame, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnFrameItemClicked(QModelIndex)));

	m_pFrameDelegate = new BodyTableDelegate;
	m_pctrlFrameTable->setItemDelegate(m_pFrameDelegate);
	int *pFramePrecision = new int[2];
	pFramePrecision[0] = 3;//five digits for x and y coordinates
	pFramePrecision[1] = 0;
	m_pFrameDelegate->SetPointer(pFramePrecision);

	//Setup Point Table
	m_pctrlPointTable->horizontalHeader()->setStretchLastSection(true);

	m_pPointModel = new QStandardItemModel;
	m_pPointModel->setRowCount(10);//temporary
	m_pPointModel->setColumnCount(3);
	m_pPointModel->setHeaderData(0, Qt::Horizontal, "y ("+length+")");
	m_pPointModel->setHeaderData(1, Qt::Horizontal, "z ("+length+")");
	m_pPointModel->setHeaderData(2, Qt::Horizontal, tr("NPanels"));
	m_pctrlPointTable->setModel(m_pPointModel);
	QItemSelectionModel *selectionModelPoint = new QItemSelectionModel(m_pPointModel);
	m_pctrlPointTable->setSelectionModel(selectionModelPoint);
	connect(selectionModelPoint, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnPointItemClicked(QModelIndex)));

	m_pPointDelegate = new BodyTableDelegate;
	m_pctrlPointTable->setItemDelegate(m_pPointDelegate);
	int *pPointPrecision = new int[3];
	pPointPrecision[0] = 3;//five digits for x and y coordinates
	pPointPrecision[1] = 3;
	pPointPrecision[2] = 0;
	m_pPointDelegate->SetPointer(pPointPrecision);
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
	m_ptPopUp.rx() = CltPt.x();
	m_ptPopUp.ry() = CltPt.y();
	m_3dWidget.ClientToGL(m_ptPopUp, m_RealPopUp);

	CtxMenu->exec(event->globalPos());

	m_3dWidget.setCursor(Qt::CrossCursor);
}



void GL3dBodyDlg::showEvent(QShowEvent *event)
{
	m_bChanged    = false;
	m_bResetglBody = true;
	m_bIs3DScaleSet = false;
	SetScales();

	ResizeTables();

	UpdateView();
}


void GL3dBodyDlg::ResizeTables()
{
	int ColumnWidth = (int)((double)(m_pctrlFrameTable->width())/2.5);
	m_pctrlFrameTable->setColumnWidth(0,ColumnWidth);
	m_pctrlFrameTable->setColumnWidth(1,ColumnWidth);
//	m_pctrlFrameTable->setColumnWidth(2,ColumnWidth);
	ColumnWidth = (int)((double)(m_pctrlPointTable->width())/4);
	m_pctrlPointTable->setColumnWidth(0,ColumnWidth);
	m_pctrlPointTable->setColumnWidth(1,ColumnWidth);
	m_pctrlPointTable->setColumnWidth(2,ColumnWidth);
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
	if(isVisible()) m_3dWidget.updateGL();
}


void  GL3dBodyDlg::wheelEvent(QWheelEvent *event)
{
	QPoint point(event->pos().x() - m_3dWidget.geometry().x(), event->pos().y() - m_3dWidget.geometry().y());
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







