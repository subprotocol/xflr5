/****************************************************************************

	GL3dWingDlg Class
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
  
#include "../MainFrame.h" 
#include "../Globals.h" 
#include "Miarex.h"
#include "GL3dWingDlg.h"
#include "GLWidget.h" 
#include "GLLightDlg.h"
#include <QDesktopWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QtDebug>
#include <math.h>

#define WINGGEOMETRY        1400

#define MESHPANELS			1410
#define MESHBACK			1420


#define ARCBALL             1430
#define ARCPOINT            1431

void* GL3dWingDlg::s_pMainFrame;		//pointer to the Frame window
void* GL3dWingDlg::s_pMiarex;	//pointer to the Miarex Application window
void *GL3dWingDlg::s_pGLLightDlg;
QList <void*> *GL3dWingDlg::s_poaWing;
QList <void*> *GL3dWingDlg::s_poaFoil;
	


GL3dWingDlg::GL3dWingDlg(void *pParent)
{
	setWindowTitle("Wing Edition");

	m_pWing = NULL;

	m_hcArrow.setShape(Qt::ArrowCursor);
	m_hcCross.setShape(Qt::CrossCursor);
	m_hcMove.setShape(Qt::ClosedHandCursor);

	pi = 3.141592654;

	m_iView      = 3;
	m_iSection   = 0;
	m_yMAC       = 0.0;
	m_GLList     = 0;

	m_UFOOffset.Set( 0.0, 0.0, 0.0);

	m_ClipPlanePos = 5.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;

	m_StackSize = 0; //the current stacksize
	m_StackPos  = 0; //the current position of the stack

	m_bChanged           = false;
	m_bEnableName        = true;
	m_bAcceptName        = true;
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
	m_bRightSide  = true;

	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_PointDown.setX(0);
	m_PointDown.setY(0);


	m_Precision[0] = 3;
	m_Precision[1] = 3;
	m_Precision[2] = 0;

	memset(MatIn,  0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx    = &m_UFOOffset.x;
	m_ArcBall.m_pOffy    = &m_UFOOffset.y;
	m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy  = &m_glViewportTrans.y;


	m_pSetupLight    = new QAction("Light Setup", this);
	m_pResetScales   = new QAction("Reset Scales", this);
	m_pInsertBefore  = new QAction("Insert Before", this);
	m_pInsertAfter   = new QAction("Insert after", this);
	m_pDeleteSection = new QAction("Delete section", this);
	m_pUndo          = new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
	m_pUndo->setStatusTip(tr("Cancels the last modifiction made to the wing"));
	m_pRedo          = new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
	m_pRedo->setStatusTip(tr("Restores the last cancelled modification made to the wing"));

	m_pContextMenu = new QMenu("Section",this);
	m_pContextMenu->addAction(m_pInsertBefore);
	m_pContextMenu->addAction(m_pInsertAfter);
	m_pContextMenu->addAction(m_pDeleteSection);

	SetupLayout();
	Connect();

	setMouseTracking(true);
}

bool GL3dWingDlg::CheckWing()
{
	if(!m_pWing->m_WingName.length())
	{
		QMessageBox::warning(this, "QFLR5", "Please enter a name for the wing");
		m_pctrlWingName->setFocus();
		return false;
	}
	for (int k=1; k<=m_pWing->m_NPanel; k++)
	{
		if(m_pWing->m_TPos[k]*1.00001 < m_pWing->m_TPos[k-1])
		{
			QMessageBox::warning(this, "QFLR5", "Warning : Panel sequence is inconsistent");
			return false;
		}
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2)
	{
		QMessageBox::warning(this, "QFLR5", "Too many panels\nReduce the mesh size");
		return false;
	}

	if(m_pWing->m_nFlaps>=20)
	{
		QString strong = "Only 10 flaps x 2 will be handled";
		if (QMessageBox::Ok != QMessageBox::question(window(), "QFLR5", strong, QMessageBox::Ok|QMessageBox::Cancel))
		  return false;
	}
	return true;
}



void GL3dWingDlg::ClientToGL(QPoint const &point, CVector &real)
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


void GL3dWingDlg::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	MainFrame *pMainFrame  = (MainFrame*)s_pMainFrame;
	QMiarex    *pMiarex = (QMiarex*)s_pMiarex;
	m_pWing->ComputeGeometry();
	m_pWing->CreateSurfaces(CVector(0.0,0.0,0.0), 0.0, 0.0);
	for (int i=0; i<m_pWing->m_NSurfaces; i++) m_pWing->m_Surface[i].SetSidePoints(NULL, 0.0, 0.0);

}



void GL3dWingDlg::contextMenuEvent(QContextMenuEvent *event)
{
	// Display the context menu
	QPoint ScreenPt = event->pos();
	ScreenPt.rx() += geometry().x();
	ScreenPt.ry() += geometry().y();

	QRect TableRect = m_pctrlWingTable->geometry();

	m_pContextMenu->exec(ScreenPt);
}






void GL3dWingDlg::Connect()
{
	m_pWingModel = new QStandardItemModel;
	m_pWingModel->setRowCount(10);//temporary
	m_pWingModel->setColumnCount(3);

	m_pctrlWingTable->setModel(m_pWingModel);
	m_pWingDelegate = new WingDelegate;
	m_pctrlWingTable->setItemDelegate(m_pWingDelegate);
	connect(m_pWingDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));
	connect(m_pctrlWingTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));
	connect(m_pctrlWingTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));
	connect(m_pInsertBefore, SIGNAL(triggered()), this, SLOT(OnInsertBefore()));
	connect(m_pInsertAfter, SIGNAL(triggered()), this, SLOT(OnInsertAfter()));
	connect(m_pDeleteSection, SIGNAL(triggered()), this, SLOT(OnDeleteSection()));

	connect(m_pRedo, SIGNAL(triggered()), this, SLOT(OnRedo()));
	connect(m_pUndo, SIGNAL(triggered()), this, SLOT(OnUndo()));
	connect(m_pResetScales, SIGNAL(triggered()), this, SLOT(OnResetScales()));
	connect(m_pSetupLight, SIGNAL(triggered()), SLOT(OnSetupLight()));

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
	connect(m_pctrlSurfaces,   SIGNAL(clicked()), this, SLOT(OnSurfaces()));
	connect(m_pctrlOutline,    SIGNAL(clicked()), this, SLOT(OnOutline()));

	connect(m_pctrlResetMesh, SIGNAL(clicked()),this, SLOT(OnResetMesh()));
	connect(m_pctrlWingColor, SIGNAL(clicked()),this, SLOT(OnWingColor()));
	connect(m_pctrlSymetric, SIGNAL(clicked()),this, SLOT(OnSymetric()));
	connect(m_pctrlRightSide, SIGNAL(clicked()),this, SLOT(OnSide()));
	connect(m_pctrlLeftSide, SIGNAL(clicked()),this, SLOT(OnSide()));
//	connect(m_pctrlWingTable, SIGNAL(activated(const QModelIndex &)), this, SLOT(OnItemActivated(const QModelIndex&)));
	connect(m_pctrlWingTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));
	connect(m_pctrlWingTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));
}


void GL3dWingDlg::CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap)
{
	// the chordwise panel distribution is set i.a.w. with the flap hinges;

	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	if(pFoilA && pFoilA->m_bTEFlap) xHingeA=pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(pFoilB && pFoilB->m_bTEFlap) xHingeB=pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * NXPanels);
	NXFlapB = (int)((1.0-xHingeB) * NXPanels);
	if(pFoilA && pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(pFoilB && pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;
	NXLeadA = NXPanels - NXFlapA;
	NXLeadB = NXPanels - NXFlapB;

	NXFlap  = qMax(NXFlapA, NXFlapB);
	if(NXFlap>NXPanels/2) NXFlap=(int)NXPanels/2;
	NXLead  = NXPanels - NXFlap;

	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(XDist==1)
			xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}
	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(XDist==1)
			xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(XDist==1)
			xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}
	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(XDist==1)
			xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}

	xPointA[NXPanels] = 0.0;
	xPointB[NXPanels] = 0.0;
}


void GL3dWingDlg::FillDataTable()
{
	if(!m_pWing) return;
	int i;
//	if(m_pWing->m_bVLMAutoMesh) VLMSetAutoMesh();
	m_pWingModel->setRowCount(m_pWing->m_NPanel+1);

	for(i=0; i<=m_pWing->m_NPanel; i++)
	{
		FillTableRow(i);
	}
}



void GL3dWingDlg::FillTableRow(int row)
{
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ind = m_pWingModel->index(row, 0, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->m_TPos[row] * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 1, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->m_TChord[row] * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 2, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->m_TOffset[row] * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 3, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->m_TDihedral[row]);

	ind = m_pWingModel->index(row, 4, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->m_TTwist[row]);

	ind = m_pWingModel->index(row, 5, QModelIndex());
	if(m_bRightSide) m_pWingModel->setData(ind, m_pWing->m_RFoil[row]);
	else             m_pWingModel->setData(ind, m_pWing->m_LFoil[row]);

	if(row<=m_pWing->m_NPanel)
	{
		ind = m_pWingModel->index(row, 6, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->m_NXPanels[row]);

		if(m_pWing->m_XPanelDist[row]==0)      strong = "Uniform";
		else if(m_pWing->m_XPanelDist[row]==1) strong = "Cosine";
		ind = m_pWingModel->index(row, 7, QModelIndex());
		m_pWingModel->setData(ind, strong);

		ind = m_pWingModel->index(row, 8, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->m_NYPanels[row]);

		if(m_pWing->m_YPanelDist[row]==0)       strong = "Uniform";
		else if(m_pWing->m_YPanelDist[row]==1)  strong = "Cosine";
		else if(m_pWing->m_YPanelDist[row]==2)  strong = "Sine";
		else if(m_pWing->m_YPanelDist[row]==-2) strong = "-Sine";
		ind = m_pWingModel->index(row, 9, QModelIndex());
		m_pWingModel->setData(ind, strong);
	}
	else
	{
		strong = " ";
		ind = m_pWingModel->index(row, 6, QModelIndex());
		m_pWingModel->setData(ind, 0);
		ind = m_pWingModel->index(row, 7, QModelIndex());
		m_pWingModel->setData(ind, " ");
		ind = m_pWingModel->index(row, 8, QModelIndex());
		m_pWingModel->setData(ind, 0);
		ind = m_pWingModel->index(row, 9, QModelIndex());
		m_pWingModel->setData(ind, " ");
	}
}



void GL3dWingDlg::GLCreateGeometry()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int j,l;
	int style, width;
	CVector Pt, A, B, C, D, N, BD, AC, LATB, TALB;
	QColor color;
	CFoil * pFoilA, *pFoilB;

	N.Set(0.0, 0.0, 0.0);
	glNewList(WINGGEOMETRY,GL_COMPILE);
	{
		m_GLList++;
		glLineWidth(1.0);

		color = m_pWing->m_WingColor;
		style = 0;
		width = 0;

		glColor3d(color.redF(),color.greenF(),color.blueF());

		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_DEPTH_TEST);

		//top surface
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					m_pWing->m_Surface[j].GetPanel(0,l,1);

					LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
					TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

					N = LATB *TALB;
					N. Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(m_pWing->m_Surface[j].LA.x,
							   m_pWing->m_Surface[j].LA.y,
							   m_pWing->m_Surface[j].LA.z);
					glVertex3d(m_pWing->m_Surface[j].TA.x,
							   m_pWing->m_Surface[j].TA.y,
							   m_pWing->m_Surface[j].TA.z);

					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l, 1);
					glVertex3d(m_pWing->m_Surface[j].TB.x,
							   m_pWing->m_Surface[j].TB.y,
							   m_pWing->m_Surface[j].TB.z);
					glVertex3d(m_pWing->m_Surface[j].LB.x,
							   m_pWing->m_Surface[j].LB.y,
							   m_pWing->m_Surface[j].LB.z);

				}
				glEnd();
			}
		}

		//bottom surface
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					m_pWing->m_Surface[j].GetPanel(0,l, -1);

					LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
					TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

					N = TALB * LATB;
					N. Normalize();

					glNormal3d( N.x,  N.y,  N.z);
					glVertex3d( m_pWing->m_Surface[j].TA.x,
								m_pWing->m_Surface[j].TA.y,
								m_pWing->m_Surface[j].TA.z);
					glVertex3d( m_pWing->m_Surface[j].LA.x,
								m_pWing->m_Surface[j].LA.y,
								m_pWing->m_Surface[j].LA.z);

					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l, -1);
					glVertex3d( m_pWing->m_Surface[j].LB.x,
								m_pWing->m_Surface[j].LB.y,
								m_pWing->m_Surface[j].LB.z);
					glVertex3d( m_pWing->m_Surface[j].TB.x,
								m_pWing->m_Surface[j].TB.y,
								m_pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
		}

		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			//All surfaces
			//Left surface
			if(m_pWing->m_Surface[j].m_bIsTipLeft)
			{
				glBegin(GL_QUAD_STRIP);
				{
					m_pWing->m_Surface[j].GetPanel(0, 0, -1);
					C. Copy(m_pWing->m_Surface[0].LA);
					D. Copy(m_pWing->m_Surface[0].TA);
					m_pWing->m_Surface[j].GetPanel(0, 0, 1);
					A. Copy(m_pWing->m_Surface[0].TA);
					B. Copy(m_pWing->m_Surface[0].LA);

					BD = D-B;
					AC = C-A;
					N  = AC*BD;
					N.Normalize();
					glNormal3d( N.x, N.y, N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);

					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(0,l,-1);
						glVertex3d( m_pWing->m_Surface[0].LA.x,
									m_pWing->m_Surface[0].LA.y,
									m_pWing->m_Surface[0].LA.z);
						m_pWing->m_Surface[j].GetPanel(0,l,1);
						glVertex3d( m_pWing->m_Surface[0].LA.x,
									m_pWing->m_Surface[0].LA.y,
									m_pWing->m_Surface[0].LA.z);
					}
				}
				glEnd();
			}
			if(m_pWing->m_Surface[j].m_bIsTipRight)
			{
				//right surface
				glBegin(GL_QUAD_STRIP);
				{
					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,0, 1);
					A. Copy(m_pWing->m_Surface[0].TB);
					B. Copy(m_pWing->m_Surface[0].LB);
					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,0, -1);
					C. Copy(m_pWing->m_Surface[0].LB);
					D. Copy(m_pWing->m_Surface[0].TB);

					BD = D-B;
					AC = C-A;
					N  = BD * AC;
					N.Normalize();
					glNormal3d( N.x,  N.y,  N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);

					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, 1);
						glVertex3d(m_pWing->m_Surface[j].LB.x,
								   m_pWing->m_Surface[j].LB.y,
								   m_pWing->m_Surface[j].LB.z);
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, -1);
						glVertex3d(m_pWing->m_Surface[j].LB.x,
								   m_pWing->m_Surface[j].LB.y,
								   m_pWing->m_Surface[j].LB.z);
					}
				}
				glEnd();
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//OUTLINE
	glNewList(WINGGEOMETRY+1,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glEnable (GL_LINE_STIPPLE);

		color = pMiarex->m_OutlineColor;
		style = pMiarex->m_OutlineStyle;
		width = pMiarex->m_OutlineWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(),color.greenF(),color.blueF());
		glLineWidth((GLfloat)width);

		//TOP outline
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					m_pWing->m_Surface[j].GetPanel(0, l, 1);
					glVertex3d(m_pWing->m_Surface[j].TA.x,
							   m_pWing->m_Surface[j].TA.y,
							   m_pWing->m_Surface[j].TA.z);
				}

				glVertex3d(m_pWing->m_Surface[j].LA.x,
						   m_pWing->m_Surface[j].LA.y,
						   m_pWing->m_Surface[j].LA.z);
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, 1);
					glVertex3d(m_pWing->m_Surface[j].TB.x,
							   m_pWing->m_Surface[j].TB.y,
							   m_pWing->m_Surface[j].TB.z);
				}

				glVertex3d(m_pWing->m_Surface[j].LB.x,
						   m_pWing->m_Surface[j].LB.y,
						   m_pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}

		//BOTTOM outline
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					m_pWing->m_Surface[j].GetPanel(0, l, -1);
					glVertex3d(m_pWing->m_Surface[j].TA.x,
							   m_pWing->m_Surface[j].TA.y,
							   m_pWing->m_Surface[j].TA.z);
				}
				glVertex3d(m_pWing->m_Surface[j].LA.x,
						   m_pWing->m_Surface[j].LA.y,
						   m_pWing->m_Surface[j].LA.z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, -1);
					glVertex3d(m_pWing->m_Surface[j].TB.x,
							   m_pWing->m_Surface[j].TB.y,
							   m_pWing->m_Surface[j].TB.z);
				}

				glVertex3d(m_pWing->m_Surface[j].LB.x,
						   m_pWing->m_Surface[j].LB.y,
						   m_pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//WingContour
		//Leading edge outline
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				m_pWing->m_Surface[j].GetPanel(0,m_pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(m_pWing->m_Surface[j].LA.x,
						   m_pWing->m_Surface[j].LA.y,
						   m_pWing->m_Surface[j].LA.z);
				m_pWing->m_Surface[j].GetPanel( m_pWing->m_Surface[j].m_NYPanels-1,m_pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(m_pWing->m_Surface[j].LB.x,
						   m_pWing->m_Surface[j].LB.y,
						   m_pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//Trailing edge outline
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				m_pWing->m_Surface[j].GetPanel(0,0, 0);
				glVertex3d(m_pWing->m_Surface[j].TA.x,
						   m_pWing->m_Surface[j].TA.y,
						   m_pWing->m_Surface[j].TA.z);
				m_pWing->m_Surface[j].GetPanel( m_pWing->m_Surface[j].m_NYPanels-1, 0, 0);
				glVertex3d(m_pWing->m_Surface[j].TB.x,
						   m_pWing->m_Surface[j].TB.y,
						   m_pWing->m_Surface[j].TB.z);
			}
			glEnd();
		}
		//flap outline....
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			pFoilA = m_pWing->m_Surface[j].m_pFoilA;
			pFoilB = m_pWing->m_Surface[j].m_pFoilB;
			if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap)
			{
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bTEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 0.0, Pt, 1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bTEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 1.0, Pt, 1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bTEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 0.0, Pt, -1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bTEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 1.0, Pt, -1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			pFoilA = m_pWing->m_Surface[j].m_pFoilA;
			pFoilB = m_pWing->m_Surface[j].m_pFoilB;
			if(pFoilA && pFoilB && pFoilA->m_bLEFlap && pFoilB->m_bLEFlap)
			{
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bLEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 0.0, Pt, 1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bLEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 1.0, Pt, 1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bLEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 0.0, Pt, -1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bLEFlap)
						m_pWing->m_Surface[j].GetPoint(m_pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 m_pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 1.0, Pt, -1);
					else 	m_pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

}

void GL3dWingDlg::GLCreateMesh()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	QColor color;
	int iLA, iLB, iTA, iTB;
	int style, width, p;
	CVector A, B, N;
	N.Set(0.0, 0.0, 0.0);

	glNewList(MESHPANELS,GL_COMPILE);
	{
		m_GLList++;
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(1.0, 1.0);

		color = pMiarex->m_VLMColor;
		style = pMiarex->m_VLMStyle;
		width = pMiarex->m_VLMWidth;

		glLineWidth(1.0);

		glColor3d(color.redF(),color.greenF(),color.blueF());

		for (p=0; p<pMiarex->m_MatSize; p++)
		{
				glBegin(GL_QUADS);
				{
					iLA = m_pPanel[p].m_iLA;
					iLB = m_pPanel[p].m_iLB;
					iTA = m_pPanel[p].m_iTA;
					iTB = m_pPanel[p].m_iTB;

					glNormal3d(m_pPanel[p].Normal.x, m_pPanel[p].Normal.y, m_pPanel[p].Normal.z);
					glVertex3d(m_pNode[iLA].x, m_pNode[iLA].y, m_pNode[iLA].z);
					glVertex3d(m_pNode[iTA].x, m_pNode[iTA].y, m_pNode[iTA].z);
					glVertex3d(m_pNode[iTB].x, m_pNode[iTB].y, m_pNode[iTB].z);
					glVertex3d(m_pNode[iLB].x, m_pNode[iLB].y, m_pNode[iLB].z);
				}
				glEnd();
		}
	}
	glEndList();

	glNewList(MESHBACK,GL_COMPILE);
	{
		m_GLList++;
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		color = pMainFrame->m_BackgroundColor;
		style = pMiarex->m_VLMStyle;
		width = pMiarex->m_VLMWidth;

		glColor3d(color.redF(),color.greenF(),color.blueF());

		glLineWidth(1.0);
		glDisable (GL_LINE_STIPPLE);

		for (p=0; p<pMiarex->m_MatSize; p++)
		{
				glBegin(GL_QUADS);
				{
					iLA = m_pPanel[p].m_iLA;
					iLB = m_pPanel[p].m_iLB;
					iTA = m_pPanel[p].m_iTA;
					iTB = m_pPanel[p].m_iTB;

					glVertex3d(m_pNode[iLA].x, m_pNode[iLA].y, m_pNode[iLA].z);
					glVertex3d(m_pNode[iTA].x, m_pNode[iTA].y, m_pNode[iTA].z);
					glVertex3d(m_pNode[iTB].x, m_pNode[iTB].y, m_pNode[iTB].z);
					glVertex3d(m_pNode[iLB].x, m_pNode[iLB].y, m_pNode[iLB].z);
					glNormal3d(m_pPanel[p].Normal.x, m_pPanel[p].Normal.y, m_pPanel[p].Normal.z);
				}
				glEnd();
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();
}






void GL3dWingDlg::GLDrawAxes()
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

//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.1*(double)i*l, 0.0, 0.0);
//		}
//	glEnd();
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
	glBegin(GL_LINES);
		glVertex3d(1.0*l, -0.050*l, -0.020*l);
		glVertex3d(1.0*l, -0.020*l, -0.050*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d(1.0*l, -0.020*l, -0.020*l);
		glVertex3d(1.0*l, -0.050*l, -0.050*l);
	glEnd();

// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -.8, 0.0);
		glVertex3d(0.0,  .8, 0.0);
	glEnd();
//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.0, 0.1*(double)i*l, 0.0);
//		}
//	glEnd();

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
	glBegin(GL_LINES);
		glVertex3d(-0.020*l, 1.0*l, -0.020*l);
		glVertex3d(-0.050*l, 1.0*l, -0.050*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d(-0.050*l, 1.0*l,-0.020*l);
		glVertex3d(-0.035*l, 1.0*l,-0.035*l);
	glEnd();

// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -.8);
		glVertex3d(0.0, 0.0,  .8);
	glEnd();

//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.0, 0.0, 0.1*(double)i*l);
//		}
//	glEnd();

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
	glBegin(GL_LINE_STRIP);
		glVertex3d(-0.050*l, -0.050*l,1.0*l);
		glVertex3d(-0.050*l, -0.020*l,1.0*l);

		glVertex3d(-0.020*l, -0.050*l,1.0*l);
		glVertex3d(-0.020*l, -0.020*l,1.0*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
}


void GL3dWingDlg::GLDraw3D()
{
//	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;

	glClearColor(pMainFrame->m_BackgroundColor.redF(), pMainFrame->m_BackgroundColor.greenF(), pMainFrame->m_BackgroundColor.blueF(),0.0);


	if(m_bResetglWing)
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

			glColor3d(0.3,0.3,.5);
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
					phi = (col * lon_incr) * pi/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = (row * lat_incr) * pi/180.0;
						glVertex3d(R*cos(phi)*cos(theta), R*sin(theta), R*sin(phi)*cos(theta));
					}
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr ) * pi/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = -(row * lat_incr) * pi/180.0;
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
					phi = (0.0 + (double)col*360.0/72.0) * pi/180.0;
					glVertex3d(R * cos(phi) * cos(theta), R * sin(theta), R * sin(phi) * cos(theta));
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * pi/180.0;
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
					theta = (row * lat_incr) * pi/180.0;
					glVertex3d(R*cos(phi)*cos(theta), R*sin(theta), R*sin(phi)*cos(theta));
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=-NumAngles; col<NumAngles; col++)
				{
					phi = (0.0 + (double)col*30.0/NumAngles) * pi/180.0;
					glVertex3d(R * cos(phi) * cos(theta), R * sin(theta), R * sin(phi) * cos(theta));
				}
			}
			glEnd();
		}
		glEndList();
	}

	if(m_bResetglWing)
	{
		if(glIsList(WINGGEOMETRY))
		{
			glDeleteLists(WINGGEOMETRY,2);
			m_GLList-=2;
		}
		GLCreateGeometry();

/*		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh();*/

		m_bResetglWing = false;
	}
}


void GL3dWingDlg::GLInverseMatrix()
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



void GL3dWingDlg::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glBegin(GL_TRIANGLES);
	glColor3ub(cr.redF(),cr.greenF(),cr.blueF());

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
		phi1 = (start_lon + col * lon_incr) * pi/180.0;
		phi2 = (start_lon + (col + 1) * lon_incr) * pi/180.0;

		for (row = 0; row < NumLatitudes; row++)
		{
			theta1 = (start_lat + row * lat_incr) * pi/180.0;
			theta2 = (start_lat + (row + 1) * lat_incr) * pi/180.0;

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



void GL3dWingDlg::GLRenderView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	GLdouble pts[4];
	pts[0]= 0.0; pts[1]=0.0; pts[2]=-1.0; pts[3]= m_ClipPlanePos;  //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE1, pts);
	if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE1);
	else						glEnable(GL_CLIP_PLANE1);

	// Clear the viewport
	glFlush();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	{
		GLSetupLight();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
/*		if(m_bShowLight)
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glTranslated(( m_GLLightDlg.m_XLight+ m_UFOOffset.x)*m_GLScale,
							 ( m_GLLightDlg.m_YLight+ m_UFOOffset.y)*m_GLScale,
							   m_GLLightDlg.m_ZLight*m_GLScale);
				double radius = (m_GLLightDlg.m_ZLight+2.0)/40.0*m_GLScale;
				QColor color;
				color = QColor((int)(m_GLLightDlg.m_Red  *255),
							(int)(m_GLLightDlg.m_Green*255),
							(int)(m_GLLightDlg.m_Blue *255));
				GLRenderSphere(color,radius,18,18);
			}
			glPopMatrix();
		}*/

		glLoadIdentity();
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

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_bOutline)
		{
			glCallList(WINGGEOMETRY+1);
		}

		if(m_bglLight)
		{
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
		}
		else
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}

		if(m_bSurfaces)
		{
			glCallList(WINGGEOMETRY);
		}

	}

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glPopMatrix();

	glDisable(GL_CLIP_PLANE1);
	glFinish();
}




void GL3dWingDlg::GLSetupLight()
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


void GL3dWingDlg::GLToClient(CVector const &real, QPoint &point)
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


void GL3dWingDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString length;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	if(!m_pWing) return;

	m_pctrlWingName->setText(m_pWing->m_WingName);
	if(!m_bAcceptName) m_pctrlWingName->setEnabled(false);
	m_pctrlSymetric->setChecked(m_pWing->m_bSymetric);
	m_pctrlRightSide->setChecked(m_pWing->m_bSymetric);
	m_pctrlLeftSide->setEnabled(!m_pWing->m_bSymetric);

	m_pctrlWingColor->SetColor(m_pWing->m_WingColor);

	m_pWingModel = new QStandardItemModel;
	m_pWingModel->setRowCount(30);//temporary
	m_pWingModel->setColumnCount(10);

	m_pWingModel->setHeaderData(0, Qt::Horizontal, QObject::tr("y-pos"));
	m_pWingModel->setHeaderData(1, Qt::Horizontal, QObject::tr("chord"));
	m_pWingModel->setHeaderData(2, Qt::Horizontal, QObject::tr("offset"));
	m_pWingModel->setHeaderData(3, Qt::Horizontal, QObject::tr("dihedral"));
	m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("twist"));
	m_pWingModel->setHeaderData(5, Qt::Horizontal, QObject::tr("foil"));
	m_pWingModel->setHeaderData(6, Qt::Horizontal, QObject::tr("X-panels"));
	m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("X-dist"));
	m_pWingModel->setHeaderData(8, Qt::Horizontal, QObject::tr("Y-panels"));
	m_pWingModel->setHeaderData(9, Qt::Horizontal, QObject::tr("Y-dist"));

	m_pctrlWingTable->setModel(m_pWingModel);
	m_pctrlWingTable->setWindowTitle(QObject::tr("Wing definition"));

	m_pWingDelegate = new WingDelegate(this);
	m_pctrlWingTable->setItemDelegate(m_pWingDelegate);
	connect(m_pWingDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	int  *precision = new int[10];
	precision[0] = 2;//five digits for x and y coordinates
	precision[1] = 2;
	precision[2] = 2;
	precision[3] = 1;
	precision[4] = 2;
	precision[5] = 1;
	precision[6] = 0;
	precision[7] = 0;
	precision[8] = 0;
	precision[9] = 0;
	m_pWingDelegate->SetPointers(precision,&m_pWing->m_NPanel);
	m_pWingDelegate->m_poaFoil = s_poaFoil;


//	Convert(true);
	FillDataTable();

	m_pctrlWingTable->setColumnWidth(0,70);
	m_pctrlWingTable->setColumnWidth(1,70);
	m_pctrlWingTable->setColumnWidth(2,70);
	m_pctrlWingTable->setColumnWidth(3,70);
	m_pctrlWingTable->setColumnWidth(4,70);
	m_pctrlWingTable->setColumnWidth(5,130);
	m_pctrlWingTable->setColumnWidth(6,50);
	m_pctrlWingTable->setColumnWidth(7,80);
	m_pctrlWingTable->setColumnWidth(8,50);
	m_pctrlWingTable->setColumnWidth(9,80);
//	m_pctrlWingTable->resizeColumnsToContents();

	SetWingData();

}



void GL3dWingDlg::keyPressEvent(QKeyEvent *event)
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
		case Qt::Key_Escape:
		{
			reject();
			break;
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


void GL3dWingDlg::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
		{
			m_bArcball = false;
			UpdateView();
			break;
		}
		case Qt::Key_Escape:
		{
			hide();
			break;
		}
		default:
			event->ignore();
	}
}


bool GL3dWingDlg::LoadSettings(QDataStream &ar)
{
	return true;
}


void GL3dWingDlg::mouseMoveEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());
//	QPoint point(event->pos().x() - geometry().x(), event->pos().y()-geometry().y());
	m_MousePos = event->pos();
	int n;
	CVector Real;

	QPoint Delta(point.x() - m_LastPoint.x(), point.y() - m_LastPoint.y());
	ClientToGL(point, Real);

	if(!m_pglWidget->hasFocus()) m_pglWidget->setFocus();

	bool bCtrl = false;

	if (event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if (event->buttons()   & Qt::LeftButton)
	{
		if(bCtrl&& m_3DWingRect.contains(point))
		{
			//rotate
			m_ArcBall.Move(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
			UpdateView();
		}
		else if(m_bTrans)
		{
			//translate
			if(m_3DWingRect.contains(point))
			{
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0/m_glScaled/m_pglWidget->m_rCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0/m_glScaled/m_pglWidget->m_rCltRect.width());

				m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
		}

	}
	else if ((event->buttons() & Qt::MidButton) && !bCtrl)
	{
		// we scale the wing

		if(m_pWing && m_3DWingRect.contains(point))
		{	//zoom 3D Wing
			if(point.y()-m_LastPoint.y()>0) m_glScaled *= (GLfloat)1.04;
			else                            m_glScaled /= (GLfloat)1.04;
			UpdateView();
		}
	}
	else 
	{
		//Highlight points

	}
	m_LastPoint = point;
}


void GL3dWingDlg::mousePressEvent(QMouseEvent *event)
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
	else
	{
		m_bTrans=true;
		if(m_pWing && m_3DWingRect.contains(point))
		{
			m_ArcBall.Start(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
			m_bCrossPoint = true;
			Set3DRotationCenter();
			if (!bCtrl)
			{
				m_bTrans = true;
				setCursor(m_hcMove);
			}
				UpdateView();
		}
	
	}

	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
	m_PointDown = point;
	m_LastPoint = point;
}



void GL3dWingDlg::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	setCursor(m_hcCross);
	
	m_bTrans = false;
	m_bDragPoint  = false;

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


void GL3dWingDlg::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
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



void GL3dWingDlg::On3DIso()
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



void GL3dWingDlg::On3DTop()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dWingDlg::On3DLeft()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90� around x
	Set3DRotationCenter();
	UpdateView();
}


void GL3dWingDlg::On3DFront()
{
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90� around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90� around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dWingDlg::On3DReset()
{
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_pctrlPickCenter->setChecked(false);
	m_bIs3DScaleSet = false;

	SetWingScale();
	UpdateView();
}


void GL3dWingDlg::On3DPickCenter()
{
	m_bPickCenter = true;
	m_pctrlPickCenter->setChecked(true);
}



void GL3dWingDlg::OnAxes()
{
	m_bAxes = m_pctrlAxes->isChecked();
	UpdateView();
}




void GL3dWingDlg::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}



void GL3dWingDlg::OnCancel()
{
	if(m_bChanged)
	{
		QString strong = tr("Discard the changes ?");
		if (QMessageBox::Yes != QMessageBox::question(this, "Question", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
			return;
	}
//	reject();
	done(QDialog::Rejected);
}



void GL3dWingDlg::OnCellChanged(QWidget *pWidget)
{
	m_bChanged = true;
	m_bResetglWing = true;
	ReadParams();
	UpdateView();
}


void GL3dWingDlg::OnDelete()
{
	if(m_iSection<=0)
	{
		QMessageBox::warning(this, "Warning", "The first section cannot be deleted");
		return;
	}

	int ny, k, size, total;

	size = m_pWingModel->rowCount();
	if(size<=2)
	{
		QMessageBox::warning(this, "Warning","Two panel sections at least are required... cannot delete");
		return;
	}

	ny = m_pWing->m_NYPanels[m_iSection-1] + m_pWing->m_NYPanels[m_iSection];

	total = VLMGetPanelTotal();
	for (k=m_iSection; k<size; k++)
	{
		m_pWing->m_TPos[k]      = m_pWing->m_TPos[k+1];
		m_pWing->m_TChord[k]    = m_pWing->m_TChord[k+1];
		m_pWing->m_TOffset[k]   = m_pWing->m_TOffset[k+1];
		m_pWing->m_TTwist[k]     = m_pWing->m_TTwist[k+1];
		m_pWing->m_TDihedral[k]  = m_pWing->m_TDihedral[k+1];
		m_pWing->m_NXPanels[k]   = m_pWing->m_NXPanels[k+1];
		m_pWing->m_NYPanels[k]   = m_pWing->m_NYPanels[k+1];
		m_pWing->m_XPanelDist[k] = m_pWing->m_XPanelDist[k+1];
		m_pWing->m_YPanelDist[k] = m_pWing->m_YPanelDist[k+1];
		m_pWing->m_RFoil[k]      = m_pWing->m_RFoil[k+1];
		m_pWing->m_LFoil[k]      = m_pWing->m_LFoil[k+1];
	}
	m_pWing->m_NPanel--;

	m_pWing->m_NYPanels[m_iSection-1] = ny;

//	m_pWing->m_bVLMAutoMesh = true;
//	Convert(false);
//	VLMSetAutoMesh(total);
	FillDataTable();
	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
}




void GL3dWingDlg::OnDeleteSection()
{
	if(m_iSection<0) return;

	if(m_iSection==0)
	{
		QMessageBox::warning(this, "Warning","The first section cannot be deleted");
		return;
	}

	int ny, k, size, total;

	size = m_pWingModel->rowCount();
	if(size<=2) return;

	ny = m_pWing->m_NYPanels[m_iSection-1] + m_pWing->m_NYPanels[m_iSection];

	total = VLMGetPanelTotal();
	for (k=m_iSection; k<size; k++)
	{
		m_pWing->m_TPos[k]      = m_pWing->m_TPos[k+1];
		m_pWing->m_TChord[k]    = m_pWing->m_TChord[k+1];
		m_pWing->m_TOffset[k]   = m_pWing->m_TOffset[k+1];
		m_pWing->m_TTwist[k]     = m_pWing->m_TTwist[k+1];
		m_pWing->m_TDihedral[k]  = m_pWing->m_TDihedral[k+1];
		m_pWing->m_NXPanels[k]   = m_pWing->m_NXPanels[k+1];
		m_pWing->m_NYPanels[k]   = m_pWing->m_NYPanels[k+1];
		m_pWing->m_XPanelDist[k] = m_pWing->m_XPanelDist[k+1];
		m_pWing->m_YPanelDist[k] = m_pWing->m_YPanelDist[k+1];
		m_pWing->m_RFoil[k]      = m_pWing->m_RFoil[k+1];
		m_pWing->m_LFoil[k]      = m_pWing->m_LFoil[k+1];
	}
	m_pWing->m_NPanel--;

	m_pWing->m_NYPanels[m_iSection-1] = ny;

//	m_pWing->m_bVLMAutoMesh = true;
//	Convert(false);
//	VLMSetAutoMesh(total);
	FillDataTable();
	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
	UpdateView();
}


void GL3dWingDlg::OnInsert()
{

}



void GL3dWingDlg::OnLight()
{
	m_bglLight = m_pctrlLight->isChecked();
	UpdateView();
}



void GL3dWingDlg::OnOutline()
{
	m_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


void GL3dWingDlg::OnPanels()
{
	m_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}




void GL3dWingDlg::OnRedo()
{
	if(m_StackPos<m_StackSize-1)
	{
		m_StackPos++;
		SetPicture();
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		pTB->EnableButton(IDT_UNDO, true);
//		if(m_StackPos==m_StackSize-1) 	pTB->EnableButton(IDT_REDO, false);
	}
}

void GL3dWingDlg::OnUndo()
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
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		if(m_StackPos==0)	pTB->EnableButton(IDT_UNDO, false);
//		pTB->EnableButton(IDT_REDO, true);
	}
	else m_StackPos = 0;
}



void GL3dWingDlg::OnResetScales()
{
	m_bIs3DScaleSet  = false;
	SetWingScale();
	UpdateView();
}



void GL3dWingDlg::OnSetupLight()
{
	GLLightDlg *pGLLightDlg = (GLLightDlg *)s_pGLLightDlg;
	m_bShowLight = true;
	UpdateView();
	pGLLightDlg->m_pGL3dWingDlg = this;
	pGLLightDlg->exec();

	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}

void GL3dWingDlg::OnSurfaces()
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
	UpdateView();
}


void GL3dWingDlg::OnOK()
{
	ReadParams();
	if(!CheckWing()) return;

	if(m_pWing->m_bSymetric)
	{
		for (int i=0; i<=m_pWing->m_NPanel; i++)
		{
			m_pWing->m_LFoil[i]   = m_pWing->m_RFoil[i];
		}
	}

	m_pWing->m_bVLMAutoMesh = false;

	m_pWing->ComputeGeometry();
	accept();
}



void GL3dWingDlg::OnInsertBefore()
{
	if (m_pWing->m_NPanel==MAXPANELS)
	{
		QMessageBox::warning(this, "Warning", "The maximum number of panels has been reached");
		return;
	}
	if(m_iSection<=0)
	{
		QMessageBox::warning(this, "Warning", "No insertion possible before the first section");
		return;
	}
	int k,n,total, ny;
	total = VLMGetPanelTotal();
	n  = m_iSection;
	for (k=m_pWing->m_NPanel; k>=n; k--)
	{
		m_pWing->m_TPos[k+1]      = m_pWing->m_TPos[k];
		m_pWing->m_TChord[k+1]    = m_pWing->m_TChord[k];
		m_pWing->m_TOffset[k+1]   = m_pWing->m_TOffset[k];
		m_pWing->m_TTwist[k+1]     = m_pWing->m_TTwist[k];
		m_pWing->m_TDihedral[k+1]  = m_pWing->m_TDihedral[k];
		m_pWing->m_NXPanels[k+1]   = m_pWing->m_NXPanels[k];
		m_pWing->m_NYPanels[k+1]   = m_pWing->m_NYPanels[k];
		m_pWing->m_XPanelDist[k+1] = m_pWing->m_XPanelDist[k];
		m_pWing->m_YPanelDist[k+1] = m_pWing->m_YPanelDist[k];
		m_pWing->m_RFoil[k+1]      = m_pWing->m_RFoil[k];
		m_pWing->m_LFoil[k+1]      = m_pWing->m_LFoil[k];
	}

	ny = m_pWing->m_NYPanels[n-1];
	m_pWing->m_TPos[n]    = (m_pWing->m_TPos[n+1]    + m_pWing->m_TPos[n-1])   /2.0;
	m_pWing->m_TChord[n]  = (m_pWing->m_TChord[n+1]  + m_pWing->m_TChord[n-1]) /2.0;
	m_pWing->m_TOffset[n] = (m_pWing->m_TOffset[n+1] + m_pWing->m_TOffset[n-1])/2.0;

	m_pWing->m_NXPanels[n]   = m_pWing->m_NXPanels[n-1];
	m_pWing->m_NYPanels[n]   = (int)(ny/2);
	m_pWing->m_NYPanels[n-1] = ny-m_pWing->m_NYPanels[n];
	if(m_pWing->m_NYPanels[n]==0)   m_pWing->m_NYPanels[n]++;
	if(m_pWing->m_NYPanels[n-1]==0) m_pWing->m_NYPanels[n-1]++;

	m_pWing->m_NPanel++;

//	m_pWing->m_bVLMAutoMesh = true;

//	Convert(false);
//	VLMSetAutoMesh(total);
	FillDataTable();
	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
	UpdateView();
}




void GL3dWingDlg::OnInsertAfter()
{
	if (m_pWing->m_NPanel==MAXPANELS)
	{
		QMessageBox::warning(this, "QFLR5", "The maximum number of panels has been reached");
		return;
	}

	int k,n,ny,total;

	n  = m_iSection;

	if(n<0) n=m_pWing->m_NPanel;
	ny = m_pWing->m_NYPanels[n];
	total = VLMGetPanelTotal();

	for (k=m_pWing->m_NPanel+1; k>n; k--)
	{
		m_pWing->m_TPos[k]       = m_pWing->m_TPos[k-1];
		m_pWing->m_TChord[k]     = m_pWing->m_TChord[k-1];
		m_pWing->m_TOffset[k]    = m_pWing->m_TOffset[k-1];
		m_pWing->m_TTwist[k]     = m_pWing->m_TTwist[k-1];
		m_pWing->m_TDihedral[k]  = m_pWing->m_TDihedral[k-1];
		m_pWing->m_NXPanels[k]   = m_pWing->m_NXPanels[k-1];
		m_pWing->m_NYPanels[k]   = m_pWing->m_NYPanels[k-1];
		m_pWing->m_XPanelDist[k] = m_pWing->m_XPanelDist[k-1];
		m_pWing->m_YPanelDist[k] = m_pWing->m_YPanelDist[k-1];
		m_pWing->m_RFoil[k]      = m_pWing->m_RFoil[k-1];
	}

	if(n<m_pWing->m_NPanel)
	{
		m_pWing->m_TPos[n+1]    = (m_pWing->m_TPos[n]    + m_pWing->m_TPos[n+2])   /2.0;
		m_pWing->m_TChord[n+1]  = (m_pWing->m_TChord[n]  + m_pWing->m_TChord[n+2]) /2.0;
		m_pWing->m_TOffset[n+1] = (m_pWing->m_TOffset[n] + m_pWing->m_TOffset[n+2])/2.0;
	}
	else
	{
		m_pWing->m_TPos[n+1]     = m_pWing->m_TPos[n+1]*1.1;
		m_pWing->m_TChord[n+1]   = m_pWing->m_TChord[n+1]/1.1;
		m_pWing->m_TOffset[n+1]  = m_pWing->m_TOffset[n+1] + m_pWing->m_TChord[n] - m_pWing->m_TChord[n+1] ;
	}
	m_pWing->m_TTwist[n+1]     = m_pWing->m_TTwist[n];
	m_pWing->m_TDihedral[n+1]  = m_pWing->m_TDihedral[n];
	m_pWing->m_NXPanels[n+1]   = m_pWing->m_NXPanels[n];
	m_pWing->m_NYPanels[n+1]   = m_pWing->m_NYPanels[n];
	m_pWing->m_XPanelDist[n+1] = m_pWing->m_XPanelDist[n];
	m_pWing->m_YPanelDist[n+1] = m_pWing->m_YPanelDist[n];
	m_pWing->m_RFoil[n+1]      = m_pWing->m_RFoil[n];

	m_pWing->m_NYPanels[n+1] = qMax(1,(int)(ny/2));
	m_pWing->m_NYPanels[n]   = qMax(1,ny-m_pWing->m_NYPanels[n+1]);

	m_pWing->m_NPanel++;

//	m_pWing->m_bVLMAutoMesh = true;
//	VLMSetAutoMesh(total);

	FillDataTable();
	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
	UpdateView();
}



void GL3dWingDlg::OnItemActivated(const QModelIndex &index)
{
	m_iSection = index.row();

	UpdateView();
}



void GL3dWingDlg::OnItemClicked(const QModelIndex &index)
{
	if(index.row()>m_pWing->m_NPanel)
	{
		//the user has filled a cell in the last line
		if(index.row()<MAXPANELS-1)
		{
			//so add an item before reading
			m_pWing->m_NPanel++;
			m_pWingModel->setRowCount(m_pWing->m_NPanel+2);
			FillTableRow(m_pWing->m_NPanel);
		}
	}
	m_iSection = index.row();

	UpdateView();
}



void GL3dWingDlg::OnResetMesh()
{
	VLMSetAutoMesh();
	FillDataTable();;
	SetWingData();
	m_bChanged = true;
	UpdateView();
}


void GL3dWingDlg::OnSide()
{
	m_bRightSide = m_pctrlRightSide->isChecked();
	FillDataTable();
	m_bChanged = true;
	UpdateView();
}


void GL3dWingDlg::OnSymetric()
{
	if(m_pctrlSymetric->isChecked())
	{
		m_pWing->m_bSymetric  = true;
		m_bRightSide          = true;
		m_pctrlLeftSide->setEnabled(false);
		m_pctrlRightSide->setChecked(true);
	}
	else
	{
		m_pWing->m_bSymetric    = false;
		m_pctrlLeftSide->setEnabled(true);
	}

	UpdateView();
	m_bChanged = true;
}

void GL3dWingDlg::OnWingColor()
{
	if(!m_pWing) return;
	QColor WingColor = m_pWing->m_WingColor;

	bool bOK;
	QRgb rgb = WingColor.rgba();
	rgb = QColorDialog::getRgba(rgb, &bOK);
	m_pWing->m_WingColor = QColor::fromRgba(rgb);
	m_pctrlWingColor->SetColor(m_pWing->m_WingColor);
}



void GL3dWingDlg::ReadParams()
{
	m_pWing->m_WingName = m_pctrlWingName->text();

	for (int i=0; i< m_pWingModel->rowCount();  i++)
	{
		ReadSectionData(i);
	}
	//Update Geometry
	ComputeGeometry();
	UpdateView();
}

void GL3dWingDlg::ReadSectionData(int sel)
{
	if(sel>=m_pWingModel->rowCount()) return;
	double d;

	bool bOK;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	QStandardItem *pItem;

	pItem = m_pWingModel->item(sel,0);

	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_TPos[sel] =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,1);
	strong =pItem->text();
	strong.replace(" ","");
	 d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_TChord[sel] =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,2);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_TOffset[sel] =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,3);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_TDihedral[sel] =d;


	pItem = m_pWingModel->item(sel,4);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_TTwist[sel] =d;

	pItem = m_pWingModel->item(sel,5);
	strong =pItem->text();
	if(m_pWing->m_bSymetric)
	{
		m_pWing->m_RFoil[sel] = strong;
		m_pWing->m_LFoil[sel] = strong;
	}
	else
	{
		if(m_bRightSide)	m_pWing->m_RFoil[sel] = strong;
		else                m_pWing->m_LFoil[sel] = strong;
	}

	pItem = m_pWingModel->item(sel,6);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_NXPanels[sel] =(int)qMax(1.0,d);
	pItem = m_pWingModel->item(sel,7);
	strong =pItem->text();
	strong.replace(" ","");
	if(strong=="Uniform")		m_pWing->m_XPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_pWing->m_XPanelDist[sel] = 1;
	else if(strong=="Sine")		m_pWing->m_XPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_pWing->m_XPanelDist[sel] = -2;

	pItem = m_pWingModel->item(sel,8);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->m_NYPanels[sel] =(int)qMax(1.0,d);

	pItem = m_pWingModel->item(sel,9);
	strong =pItem->text();
	strong.replace(" ","");

	if(strong=="Uniform")		m_pWing->m_YPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_pWing->m_YPanelDist[sel] = 1;
	else if(strong=="Sine")		m_pWing->m_YPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_pWing->m_YPanelDist[sel] = -2;

}


void GL3dWingDlg::reject()
{
	if(m_bChanged)
	{
		m_pWing->m_WingName = m_pctrlWingName->text();

		int res = QMessageBox::question(window(), "Wing Dlg Exit", "Save the wing ?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if (QMessageBox::No == res) QDialog::reject();
		else if (QMessageBox::Cancel == res) return;
		else done(QDialog::Accepted);
	}
	else QDialog::reject();

}


void GL3dWingDlg::resizeEvent(QResizeEvent *event)
{
	m_bIs3DScaleSet = false;

	SetWingScale();

	int w4 = m_pctrlWingTable->width()/4;
	m_pctrlWingTable->setColumnWidth(0,w4);
	m_pctrlWingTable->setColumnWidth(1,w4);
	m_pctrlWingTable->setColumnWidth(2,w4);


//	UpdateView();
}



bool GL3dWingDlg::SaveSettings(QDataStream &ar)
{
	return true;
}



void GL3dWingDlg::Set3DRotationCenter()
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


void GL3dWingDlg::Set3DRotationCenter(QPoint point)
{
	//adjusts the new rotation center after the user has picked a point on the screen
	//finds the closest panel under the point,
	//and changes the rotation vector and viewport translation
	int  i, j;

	CVector N, LATB, TALB, LA, LB, TA, TB;
	CVector I, A, B, AA, BB, PP, U;
	double dmin;

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


/*	if(m_pBody->m_LineType==2)
	{
		...
		bIntersect = true;
	}*/


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



void GL3dWingDlg::SetCurrentSection(int section)
{
//	m_iSection = section;
	UpdateView();
}


void GL3dWingDlg::SetWing(CWing *pWing)
{
	m_pWing = pWing;
	if(!m_pWing) return;

	m_pWingDelegate->SetPointers(m_Precision,&m_pWing->m_NPanel);

	//Fill all widget data
	FillDataTable();;
}



void GL3dWingDlg::SetWingScale()
{

//if(m_bIs3DScaleSet) return;

	//wing along X axis will take 3/4 of the screen
	m_glScaled = (GLfloat)(3./4.*2.0*m_pWing->m_Span);
	m_glViewportTrans.x = 0.0;
	m_glViewportTrans.y = 0.0;
	m_glViewportTrans.z = 0.0;
	m_bIs3DScaleSet = true;

	m_UFOOffset.x = 0.0;
	m_UFOOffset.y = 0.0;
}


void GL3dWingDlg::SetWingData()
{
	if(!m_pWing) return;
	//Updates the wing's properties after a change of geometry

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

	str = QString("%1").arg(m_pWing->m_Volume*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit,5,'e',2);
	m_pctrlWingVolume->setText(str);

	str = QString("%1").arg(m_pWing->m_Area*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlWingArea->setText(str);

	str = QString("%1").arg(m_pWing->m_Span*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlWingSpan->setText(str);

	str = QString("%1").arg(m_pWing->m_ProjectedArea*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlProjectedArea->setText(str);

	str = QString("%1").arg(m_pWing->m_ProjectedSpan*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlProjectedSpan->setText(str);

	str = QString("%1").arg(m_pWing->m_GChord*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlGeomChord->setText(str);

	str = QString("%1").arg(m_pWing->m_MAChord*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlMAC->setText(str);

	str = QString("%1").arg(m_yMAC*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlMACSpanPos->setText(str);

	str = QString("%1").arg(m_pWing->m_AR,5,'f',2);
	m_pctrlAspectRatio->setText(str);

	if(m_pWing->m_TChord[m_pWing->m_NPanel]>0.0) str = QString("%1").arg(m_pWing->m_TR,0,'f',2);
	else                                         str = "Undefined";
	m_pctrlTaperRatio->setText(str);

	//TODO
//	str = QString("%5.2f").arg(GetAverageSweep(),5,'f',2));
//	m_pctrlSweep->SetWindowText(str);

	int VLMTotal=100;
	VLMTotal = VLMGetPanelTotal();
	str = QString("%1").arg(VLMTotal);
	m_pctrlVLMPanels->setText(str);

	VLMTotal *= 2;
	VLMTotal += 2*m_pWing->m_NXPanels[m_pWing->m_NPanel-1];
	str = QString("%1").arg(VLMTotal);
	m_pctrl3DPanels->setText(str);
}



void GL3dWingDlg::SetPicture()
{
	UpdateView();
}


void GL3dWingDlg::SetupLayout()
{
	int i;
	QString str;

	QDesktopWidget desktop;
	QRect r = desktop.screenGeometry();
	setMaximumHeight(r.height());
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
	m_pglWidget->m_iView = 7;
	m_pglWidget->setMinimumHeight(400);
	m_pglWidget->setMinimumWidth(500);

/*_____________Start Top Layout Here____________*/
	QVBoxLayout *DefLayout = new QVBoxLayout;
	QHBoxLayout *SymLayout = new QHBoxLayout;
	m_pctrlSymetric     = new QCheckBox(tr("Symetric"));
	m_pctrlRightSide    = new QRadioButton(tr("Right Side"));
	m_pctrlLeftSide     = new QRadioButton(tr("Left Side"));
	m_pctrlResetMesh    = new QPushButton(tr("Reset Mesh"));
	SymLayout->addWidget(m_pctrlSymetric);
	SymLayout->addWidget(m_pctrlRightSide);
	SymLayout->addWidget(m_pctrlLeftSide);
	SymLayout->addStretch(1);
	SymLayout->addWidget(m_pctrlResetMesh);

	QHBoxLayout *NameLayout = new QHBoxLayout;
	m_pctrlWingName     = new QLineEdit("WingName");
	m_pctrlWingColor    = new ColorButton;
	NameLayout->addWidget(m_pctrlWingName);
//	NameLayout->addStretch(1);
	NameLayout->addWidget(m_pctrlWingColor);

	m_pctrlWingTable = new QTableView(this);
	m_pctrlWingTable->setMinimumWidth(800);
//	m_pctrlWingTable->setMaximumHeight((int)(r.height()/4));
	m_pctrlWingTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlWingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlWingTable->setEditTriggers(QAbstractItemView::CurrentChanged);
	m_pctrlWingTable->setSizePolicy(szPolicyExpanding);

	DefLayout->addLayout(NameLayout);
	DefLayout->addLayout(SymLayout);
	DefLayout->addWidget(m_pctrlWingTable);

	m_pctrlControlsWidget = new QWidget;
	m_pctrlControlsWidget->setLayout(DefLayout);

	QVBoxLayout *LeftLayout = new QVBoxLayout;
	LeftLayout->addWidget(m_pctrlControlsWidget);
	LeftLayout->addWidget(m_pglWidget);

	m_pctrlLength1    = new QLabel("mm");
	m_pctrlLength2    = new QLabel("mm");
	m_pctrlLength3    = new QLabel("mm");
	m_pctrlLength4    = new QLabel("mm");
	m_pctrlLength5    = new QLabel("mm");
	m_pctrlAreaUnit   = new QLabel("mm2");
	m_pctrlAreaUnit2  = new QLabel("mm2");
	m_pctrlVolumeUnit = new QLabel("mm3");
	m_pctrlLength1->setAlignment(Qt::AlignLeft);
	m_pctrlLength2->setAlignment(Qt::AlignLeft);
	m_pctrlLength3->setAlignment(Qt::AlignLeft);
	m_pctrlLength4->setAlignment(Qt::AlignLeft);
	m_pctrlLength5->setAlignment(Qt::AlignLeft);
	m_pctrlAreaUnit->setAlignment(Qt::AlignLeft);
	m_pctrlAreaUnit2->setAlignment(Qt::AlignLeft);
	m_pctrlVolumeUnit->setAlignment(Qt::AlignLeft);

	QGridLayout *DataLayout = new QGridLayout;
	QLabel *lab1 = new QLabel("Wing Span");
	QLabel *lab2 = new QLabel("Area");
	QLabel *lab3 = new QLabel("Projected Span");
	QLabel *lab4 = new QLabel("Projected Area");
	QLabel *lab5 = new QLabel("Volume");
	QLabel *lab13 = new QLabel("Total VLM Panels");
	QLabel *lab14 = new QLabel("Number of 3D Panels");
	lab1->setAlignment(Qt::AlignRight);
	lab2->setAlignment(Qt::AlignRight);
	lab3->setAlignment(Qt::AlignRight);
	lab4->setAlignment(Qt::AlignRight);
	lab5->setAlignment(Qt::AlignRight);
	lab13->setAlignment(Qt::AlignRight);
	lab14->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(lab1,1,1);
	DataLayout->addWidget(lab2,2,1);
	DataLayout->addWidget(lab3,3,1);
	DataLayout->addWidget(lab4,4,1);
	DataLayout->addWidget(lab5,5,1);
	DataLayout->addWidget(lab13,13,1);
	DataLayout->addWidget(lab14,14,1);
	m_pctrlWingSpan      = new QLabel("2000.00");
	m_pctrlWingArea      = new QLabel("30.0");
	m_pctrlProjectedArea = new QLabel("25.0");
	m_pctrlProjectedSpan = new QLabel("1900.0");;
	m_pctrlWingVolume    = new QLabel("000.0e+03");
	m_pctrlVLMPanels     = new QLabel("500");
	m_pctrl3DPanels      = new QLabel("1000");
	m_pctrlWingSpan->setAlignment(Qt::AlignRight);
	m_pctrlWingArea->setAlignment(Qt::AlignRight);
	m_pctrlProjectedSpan->setAlignment(Qt::AlignRight);
	m_pctrlProjectedArea->setAlignment(Qt::AlignRight);
	m_pctrlWingVolume->setAlignment(Qt::AlignRight);
	m_pctrlVLMPanels->setAlignment(Qt::AlignRight);
	m_pctrl3DPanels->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(m_pctrlWingSpan,   1,2);
	DataLayout->addWidget(m_pctrlWingArea,   2,2);
	DataLayout->addWidget(m_pctrlProjectedSpan,   3,2);
	DataLayout->addWidget(m_pctrlProjectedArea,   4,2);
	DataLayout->addWidget(m_pctrlWingVolume, 5,2);
	DataLayout->addWidget(m_pctrlVLMPanels, 13,2);
	DataLayout->addWidget(m_pctrl3DPanels,  14,2);

	DataLayout->addWidget(m_pctrlLength1,1,3);
	DataLayout->addWidget(m_pctrlAreaUnit,2,3);
	DataLayout->addWidget(m_pctrlLength2,3,3);
	DataLayout->addWidget(m_pctrlAreaUnit2,4,3);
	DataLayout->addWidget(m_pctrlVolumeUnit,5,3);
	QLabel *lab15 = new QLabel("Max is 1000");
	lab13->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab15 ,13,3);
	QLabel *lab16 = new QLabel("Max is 2000");
	lab15->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab16, 14, 3);


	QLabel *lab20 = new QLabel("Mean Geom. Chord");
	QLabel *lab21 = new QLabel("Mean Aero Chord");
	QLabel *lab22 = new QLabel("MAC Span Pos");
	QLabel *lab23 = new QLabel("Aspect ratio");
	QLabel *lab24 = new QLabel("Taper Ratio");
	QLabel *lab25 = new QLabel("Root to Tip Sweep");
	QLabel *lab26 = new QLabel("Number of Flaps");
	lab20->setAlignment(Qt::AlignRight);
	lab21->setAlignment(Qt::AlignRight);
	lab22->setAlignment(Qt::AlignRight);
	lab23->setAlignment(Qt::AlignRight);
	lab24->setAlignment(Qt::AlignRight);
	lab25->setAlignment(Qt::AlignRight);
	lab26->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(lab20,6,1);
	DataLayout->addWidget(lab21,7,1);
	DataLayout->addWidget(lab22,8,1);
	DataLayout->addWidget(lab23,9,1);
	DataLayout->addWidget(lab24,10,1);
	DataLayout->addWidget(lab25,11,1);
	DataLayout->addWidget(lab26,12,1);

	m_pctrlMAC          = new QLabel("150.0");
	m_pctrlGeomChord    = new QLabel("170.0");
	m_pctrlMACSpanPos   = new QLabel("466.00");
	m_pctrlAspectRatio  = new QLabel("13.33");
	m_pctrlTaperRatio   = new QLabel("1.50");
	m_pctrlSweep        = new QLabel("2.58");
	m_pctrlNFlaps       = new QLabel("0");
	m_pctrlMAC->setAlignment(Qt::AlignRight);
	m_pctrlGeomChord->setAlignment(Qt::AlignRight);
	m_pctrlMACSpanPos->setAlignment(Qt::AlignRight);
	m_pctrlAspectRatio->setAlignment(Qt::AlignRight);
	m_pctrlTaperRatio->setAlignment(Qt::AlignRight);
	m_pctrlSweep->setAlignment(Qt::AlignRight);
	m_pctrlNFlaps->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(m_pctrlMAC,          6,2);
	DataLayout->addWidget(m_pctrlGeomChord,    7,2);
	DataLayout->addWidget(m_pctrlMACSpanPos,   8,2);
	DataLayout->addWidget(m_pctrlAspectRatio,  9,2);
	DataLayout->addWidget(m_pctrlTaperRatio,  10,2);
	DataLayout->addWidget(m_pctrlSweep,       11,2);
	DataLayout->addWidget(m_pctrlNFlaps,      12,2);
	DataLayout->addWidget(m_pctrlLength3, 6, 3);
	DataLayout->addWidget(m_pctrlLength4, 7, 3);
	DataLayout->addWidget(m_pctrlLength5, 8, 3);
	QLabel *lab30 = new QLabel("deg");
	lab30->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab30, 11, 3);


/*_____________End Top Layout Here______________*/

/*_____________Start Right Layout Here_________*/

	QVBoxLayout *RightLayout = new QVBoxLayout;

	QGridLayout *ThreeDParams = new QGridLayout;
	m_pctrlAxes       = new QCheckBox("Axes");
	m_pctrlLight      = new QCheckBox("Light");
	m_pctrlSurfaces   = new QCheckBox("Surfaces");
	m_pctrlOutline    = new QCheckBox("Outline");
	m_pctrlPanels     = new QCheckBox("Panels");
	m_pctrlAxes->setSizePolicy(szPolicyMinimum);
	m_pctrlLight->setSizePolicy(szPolicyMinimum);
	m_pctrlSurfaces->setSizePolicy(szPolicyMinimum);
	m_pctrlOutline->setSizePolicy(szPolicyMinimum);
	m_pctrlPanels->setSizePolicy(szPolicyMinimum);
	ThreeDParams->addWidget(m_pctrlAxes, 1,1);
	ThreeDParams->addWidget(m_pctrlLight, 1,2);
	ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
	ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	ThreeDParams->addWidget(m_pctrlPanels, 3,1);

	m_pctrlX          = new QPushButton("X");
	m_pctrlY          = new QPushButton("Y");
	m_pctrlZ          = new QPushButton("Z");
	m_pctrlIso        = new QPushButton("Iso");
	m_pctrlPickCenter = new QPushButton("Pick Center");
	m_pctrlReset      = new QPushButton("Reset Scales");
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
	QLabel *ClipLabel = new QLabel("Clip Plane");
	ThreeDViewControls->addWidget(ClipLabel);
	ThreeDViewControls->addWidget(m_pctrlClipPlanePos);


	QHBoxLayout *ActionButtons = new QHBoxLayout;
	QPushButton *UndoButton = new QPushButton(QIcon(":/images/OnUndo.png"), tr("Undo"));
	QPushButton *RedoButton = new QPushButton(QIcon(":/images/OnRedo.png"), tr("Redo"));
	QPushButton *MenuButton = new QPushButton(tr("Other"));

	QMenu *WingMenu = new QMenu("Actions...",this);
	WingMenu->addAction(m_pSetupLight);
	WingMenu->addSeparator();
	MenuButton->setMenu(WingMenu);

	ActionButtons->addWidget(UndoButton);
	ActionButtons->addWidget(RedoButton);
	ActionButtons->addWidget(MenuButton);
	connect(UndoButton, SIGNAL(clicked()),this, SLOT(OnUndo()));
	connect(RedoButton, SIGNAL(clicked()),this, SLOT(OnRedo()));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("Save and Close"));
	OKButton->setAutoDefault(true);
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()),this, SLOT(reject()));

	QVBoxLayout *WingActions = new QVBoxLayout;
	WingActions->addStretch(1);
	WingActions->addLayout(ActionButtons);
	WingActions->addStretch(1);
	WingActions->addLayout(CommandButtons);

	QVBoxLayout *All3DControls = new QVBoxLayout;
	All3DControls->addStretch(1);
	All3DControls->addLayout(ThreeDParams);
	All3DControls->addLayout(AxisView);
	All3DControls->addLayout(ThreeDView);
	All3DControls->addWidget(m_pctrlPickCenter);
	All3DControls->addLayout(ThreeDViewControls);
	All3DControls->addStretch(1);
	All3DControls->addLayout(WingActions);

	RightLayout->addLayout(DataLayout);
//	RightLayout->addStretch(1);
	RightLayout->addLayout(All3DControls);


/*	QGridLayout *MainLayout = new QGridLayout;
	MainLayout->addLayout(LeftLayout,1,1);
	MainLayout->addLayout(RightLayout,1,2,2,1);*/

	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addLayout(LeftLayout);
	MainLayout->addLayout(RightLayout);

	setLayout(MainLayout);
}


void GL3dWingDlg::ShowContextMenu(QContextMenuEvent * event)
{
	QMenu *CtxMenu = new QMenu("Context Menu",this);
//	CtxMenu->addAction(m_pInsertPoint);

	QPoint CltPt = event->pos();
	QPoint ScreenPt = event->pos();

	m_ptPopUp.rx() = CltPt.x();
	m_ptPopUp.ry() = CltPt.y();
	ClientToGL(m_ptPopUp, m_RealPopUp);

	ScreenPt.rx() += geometry().x();
	ScreenPt.ry() += geometry().y();
	CtxMenu->exec(ScreenPt);

	setCursor(m_hcCross);
}


void GL3dWingDlg::showEvent(QShowEvent *event)
{
	m_bChanged    = false;
	ComputeGeometry();
	InitDialog();
	m_bResetglWing = true;
	m_bIs3DScaleSet = false;
	SetWingScale();
	m_3DWingRect = m_pglWidget->geometry();
	CVector O(0.0,0.0,0.0);

	UpdateView();
}



void GL3dWingDlg::StorePicture()
{
	int i;
	if(m_StackPos>=20)
	{
		for (i=1; i<20; i++)
		{
//			m_UndoPic[i-1].Duplicate(m_UndoPic+i);
		}
		m_StackPos = 19;
		m_StackSize = 19;
	}
//	m_UndoPic[m_StackPos].Duplicate(&m_TmpPic);

	m_bStored = true;
	m_StackPos++;
	m_StackSize = m_StackPos;

}




void GL3dWingDlg::TakePicture()
{
	m_bChanged = true;
	m_bStored = false;
//	m_TmpPic.Duplicate(m_pWing);
}


void GL3dWingDlg::UpdateView()
{
	if(isVisible()) m_pglWidget->updateGL();
}


int GL3dWingDlg::VLMGetPanelTotal()
{
	QMiarex    *pMiarex = (QMiarex*)s_pMiarex;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span/1000.0;

	int total = 0;
	for (int i=0; i<m_pWing->m_NPanel; i++)
	{
			//do not create a surface if its length is less than the critical size
//			if(fabs(m_pWing->m_TPos[j]-m_pWing->m_TPos[j+1])/m_pWing->m_Span >0.001){
			if (fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i+1]) > MinPanelSize)
				total +=m_pWing->m_NXPanels[i]*m_pWing->m_NYPanels[i];
	}
//	if(!m_bMiddle) total *=2;
	if(!m_pWing->m_bIsFin) return total*2;
	else                   return total;
}




bool GL3dWingDlg::VLMSetAutoMesh(int total)
{
	//set automatic mesh : keep it simple
	m_pWing->m_bVLMAutoMesh = true;
	m_bChanged = true;
	//split (NYTotal) panels on each side proportionnaly to length, and space evenly
	//Set VLMMATSIZE/NYTotal panels along chord
	int NYTotal, size;

	if(!total)
	{
		size = (int)(VLMMATSIZE/4);//why not ? Too much refinement isn't worthwile
		NYTotal = 22;
	}
	else{
		size = total;
		NYTotal = (int)sqrt((float)size);
	}

	NYTotal *= 2;

	double d1, d2; //spanwise panel densities at i and i+1

	for (int i=0; i<m_pWing->m_NPanel;i++)
	{
		d1 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->m_TPos[i],  3) + 0.5;
		d2 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->m_TPos[i+1],3) + 0.5;
		m_pWing->m_NYPanels[i] = (int) (NYTotal * (0.8*d1+0.2*d2)* (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_Span);
		m_pWing->m_NXPanels[i] = (int) (size/NYTotal);
		m_pWing->m_NXPanels[i] = qMin(m_pWing->m_NXPanels[i], MAXCHORDPANELS);

		if(m_pWing->m_NYPanels[i]==0) m_pWing->m_NYPanels[i] = 1;
		if(m_pWing->m_NXPanels[i]==0) m_pWing->m_NXPanels[i] = 1;
//		m_pWing->m_XPanelDist[i] = 1;//cosine distribution
//		m_pWing->m_YPanelDist[i] = 0;//uniformly distributed, except at the root and tip (see next)
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2)
	{
		QMessageBox::warning(this, "Warning", "Too many panels\nReduce the mesh size");
		return false;
	}
	return true;
}



void  GL3dWingDlg::wheelEvent(QWheelEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	if(m_pglWidget->m_rCltRect.contains(point)) m_pglWidget->setFocus();	//The mouse button has been wheeled
	//Process the message
//	point is in client coordinates
	if(m_3DWingRect.contains(point))
	{
		if(event->delta()<0) m_glScaled *= (GLfloat)1.06;
		else                 m_glScaled /= (GLfloat)1.06;
	}

	UpdateView();

}






