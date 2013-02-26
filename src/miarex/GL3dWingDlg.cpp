/****************************************************************************

	GL3dWingDlg Class
	Copyright (C) 2009-2010 Andre Deperrois adeperrois@xflr5.com

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
#include "../globals.h" 
#include "../miarex/GLCreateLists.h" 
#include "../threedwidget.h"
#include "../misc/GLLightDlg.h"
#include "Miarex.h"
#include "GL3dWingDlg.h"
#include "WingScaleDlg.h"
#include "InertiaDlg.h"
#include <QDesktopWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QColorDialog>
#include <math.h>


#define WINGGEOMETRY        1400
#define SECTIONHIGHLIGHT    1402



void* GL3dWingDlg::s_pMainFrame;		//pointer to the Frame window
void* GL3dWingDlg::s_pMiarex;	//pointer to the Miarex Application window
void *GL3dWingDlg::s_pGLLightDlg;

QList <void*> *GL3dWingDlg::s_poaWing;
QList <void*> *GL3dWingDlg::s_poaFoil;
	

QPoint GL3dWingDlg::s_WindowPos=QPoint(20,20);
QSize  GL3dWingDlg::s_WindowSize=QSize(900, 700);
bool GL3dWingDlg::s_bWindowMaximized=false;


GL3dWingDlg::GL3dWingDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Wing Edition"));
	setWindowFlags(Qt::Window);
//	setSizeGripEnabled(true);

	m_pWing = NULL;


	m_iSection   = -1;
//	m_yMAC       = 0.0;
	m_GLList     = 0;

	m_UFOOffset.Set( 0.0, 0.0, 0.0);

	m_ClipPlanePos = 5.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;


	m_bResetglSectionHighlight = true;
	m_bResetglWing             = true;
	m_bFoilNames         = false;
	m_bShowMasses        = false;
	m_bChanged           = false;
	m_bEnableName        = true;
	m_bAcceptName        = true;
	m_bTrans             = false;
	m_bDragPoint         = false;
	m_bArcball           = false;
	m_bCrossPoint        = false;
	m_bSurfaces          = true;
	m_bOutline           = true;
	m_bVLMPanels         = true;
	m_bAxes              = true;
	m_bPickCenter        = false;
	m_bShowLight         = false;
	m_bRightSide  = true;

	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_PointDown.setX(0);
	m_PointDown.setY(0);


	memset(MatIn,  0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx    = &m_UFOOffset.x;
	m_ArcBall.m_pOffy    = &m_UFOOffset.y;
	m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy  = &m_glViewportTrans.y;
	m_ArcBall.m_pRect    = &m_pGLWidget->m_rCltRect;

	m_pResetScales   = new QAction(tr("Reset Scales"), this);
	m_pInsertBefore  = new QAction(tr("Insert Before"), this);
	m_pInsertAfter   = new QAction(tr("Insert after"), this);
	m_pDeleteSection = new QAction(tr("Delete section"), this);
	m_pResetSection  = new QAction(tr("Reset section"), this);

	m_pContextMenu = new QMenu(tr("Section"),this);
	m_pContextMenu->addAction(m_pInsertBefore);
	m_pContextMenu->addAction(m_pInsertAfter);
	m_pContextMenu->addAction(m_pDeleteSection);
	m_pContextMenu->addAction(m_pResetSection);

	SetupLayout();
	Connect();

	setMouseTracking(true);
}


bool GL3dWingDlg::CheckWing()
{
	if(!m_pWing->m_WingName.length())
	{
		QMessageBox::warning(this, tr("Warning"), tr("Please enter a name for the wing"));
		m_pctrlWingName->setFocus();
		return false;
	}

	for (int k=1; k<m_pWing->NWingSection(); k++)
	{
		if(m_pWing->YPosition(k)*1.00001 < m_pWing->YPosition(k-1))
		{
			QMessageBox::warning(this, tr("Warning"), tr("Warning : Panel sequence is inconsistent"));
			return false;
		}
	}

	int NYPanels = 0;
	for(int j=0; j<m_pWing->NWingSection()-1; j++)
	{
		NYPanels += m_pWing->NYPanels(j);
	}
	if(NYPanels*2>=MAXSPANSTATIONS)
	{
		QString strange = QString(" %1").arg(MAXSPANSTATIONS/2);
		strange = tr("Too many spanwise panels.\nThe maximum number is")+strange;

		QMessageBox::warning(this, tr("Warning"), strange);
		return false;
	}

	if(VLMGetPanelTotal()>VLMMAXMATSIZE/2)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Too many panels\nReduce the mesh size"));
		return false;
	}

	if(m_pWing->m_nFlaps>=20)
	{
		QString strong = tr("Only 10 flaps x 2 will be handled");
		if (QMessageBox::Ok != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Ok|QMessageBox::Cancel))
		  return false;
	}
	return true;
}



void GL3dWingDlg::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	m_pWing->ComputeGeometry();
	m_pWing->CreateSurfaces(CVector(0.0,0.0,0.0), 0.0, 0.0);

	for (int i=0; i<m_pWing->m_NSurfaces; i++) m_pWing->m_Surface[i].SetSidePoints(NULL, 0.0, 0.0);
}



void GL3dWingDlg::contextMenuEvent(QContextMenuEvent *event)
{
	// Display the context menu
	if(m_pctrlWingTable->geometry().contains(event->pos())) m_pContextMenu->exec(event->globalPos());
}


void GL3dWingDlg::Connect()
{
	connect(m_pInsertBefore, SIGNAL(triggered()), this, SLOT(OnInsertBefore()));
	connect(m_pInsertAfter, SIGNAL(triggered()), this, SLOT(OnInsertAfter()));
	connect(m_pDeleteSection, SIGNAL(triggered()), this, SLOT(OnDeleteSection()));
	connect(m_pResetSection, SIGNAL(triggered()), this, SLOT(OnResetSection()));

	connect(m_pResetScales, SIGNAL(triggered()), this, SLOT(On3DReset()));
//	connect(m_pctrlSetupLight, SIGNAL(clicked()), SLOT(OnSetupLight()));

	connect(m_pctrlIso, SIGNAL(clicked()),this, SLOT(On3DIso()));
	connect(m_pctrlX, SIGNAL(clicked()),this, SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()),this, SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()),this, SLOT(On3DTop()));
	connect(m_pctrlReset, SIGNAL(clicked()),this, SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()),this, SLOT(On3DPickCenter()));
	connect(m_pctrlFoilNames, SIGNAL(clicked()),this, SLOT(OnFoilNames()));
	connect(m_pctrlShowMasses, SIGNAL(clicked()),this, SLOT(OnShowMasses()));

	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));

	connect(m_pctrlAxes,       SIGNAL(clicked()), this, SLOT(OnAxes()));
	connect(m_pctrlPanels,     SIGNAL(clicked()), this, SLOT(OnPanels()));
	connect(m_pctrlLight,      SIGNAL(clicked()), this, SLOT(OnLight()));
	connect(m_pctrlSurfaces,   SIGNAL(clicked()), this, SLOT(OnSurfaces()));
	connect(m_pctrlOutline,    SIGNAL(clicked()), this, SLOT(OnOutline()));

	connect(m_pctrlInsertBefore, SIGNAL(clicked()),this, SLOT(OnInsertBefore()));
	connect(m_pctrlInsertAfter, SIGNAL(clicked()),this, SLOT(OnInsertAfter()));
	connect(m_pctrlDeleteSection, SIGNAL(clicked()),this, SLOT(OnDeleteSection()));
	connect(m_pctrlResetSection, SIGNAL(clicked()),this, SLOT(OnResetSection()));
	connect(m_pctrlResetMesh, SIGNAL(clicked()),this, SLOT(OnResetMesh()));
	connect(m_pctrlScaleWing, SIGNAL(clicked()),this, SLOT(OnScaleWing()));
	connect(m_pctrlWingColor, SIGNAL(clicked()),this, SLOT(OnWingColor()));
	connect(m_pctrlSymetric, SIGNAL(clicked()),this, SLOT(OnSymetric()));
	connect(m_pctrlRightSide, SIGNAL(clicked()),this, SLOT(OnSide()));
	connect(m_pctrlLeftSide, SIGNAL(clicked()),this, SLOT(OnSide()));
	connect(m_pctrlInertiaButton, SIGNAL(clicked()),this, SLOT(OnInertia()));
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
			xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}
	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(XDist==1)
			xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(XDist==1)
			xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}
	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(XDist==1)
			xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
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
	m_pWingModel->setRowCount(m_pWing->NWingSection());

	for(i=0; i<m_pWing->NWingSection(); i++)
	{
		FillTableRow(i);
	}
}



void GL3dWingDlg::FillTableRow(int row)
{
	QString strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ind = m_pWingModel->index(row, 0, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->YPosition(row) * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 1, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->Chord(row) * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 2, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->Offset(row) * pMainFrame->m_mtoUnit);

	ind = m_pWingModel->index(row, 3, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->Dihedral(row));

	ind = m_pWingModel->index(row, 4, QModelIndex());
	m_pWingModel->setData(ind, m_pWing->Twist(row));

	ind = m_pWingModel->index(row, 5, QModelIndex());
	if(m_bRightSide) m_pWingModel->setData(ind, m_pWing->RightFoil(row));
	else             m_pWingModel->setData(ind, m_pWing->LeftFoil(row));

	if(row<m_pWing->NWingSection())
	{
		ind = m_pWingModel->index(row, 6, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->NXPanels(row));

		if(m_pWing->XPanelDist(row)==0)      strong = tr("Uniform");
		else if(m_pWing->XPanelDist(row)==1) strong = tr("Cosine");
		ind = m_pWingModel->index(row, 7, QModelIndex());
		m_pWingModel->setData(ind, strong);

		ind = m_pWingModel->index(row, 8, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->NYPanels(row));

		if(m_pWing->YPanelDist(row)==0)       strong = tr("Uniform");
		else if(m_pWing->YPanelDist(row)==1)  strong = tr("Cosine");
		else if(m_pWing->YPanelDist(row)==2)  strong = tr("Sine");
		else if(m_pWing->YPanelDist(row)==-2) strong = tr("-Sine");
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



void GL3dWingDlg::GLCreateMesh()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	QColor color;
	int width, j,l,k;
	CVector A, B, C, D, N, LATB, TALB;

	glNewList(MESHPANELS,GL_COMPILE);
	{
		m_GLList++;
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		color = pMiarex->m_VLMColor;
//		style = pMiarex->m_VLMStyle;
		width = pMiarex->m_VLMWidth;

		glLineWidth(width);

		glColor3d(color.redF(),color.greenF(),color.blueF());

		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			for(k=0; k<m_pWing->m_Surface[j].m_NYPanels; k++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(k,l,TOPSURFACE);

						LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
						TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(k,l,BOTSURFACE);

						LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
						TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(m_pWing->m_Surface[j].LA.x,
								   m_pWing->m_Surface[j].LA.y,
								   m_pWing->m_Surface[j].LA.z);
						glVertex3d(m_pWing->m_Surface[j].LB.x,
								   m_pWing->m_Surface[j].LB.y,
								   m_pWing->m_Surface[j].LB.z);
					}
					glVertex3d(m_pWing->m_Surface[j].TA.x,
							   m_pWing->m_Surface[j].TA.y,
							   m_pWing->m_Surface[j].TA.z);
					glVertex3d(m_pWing->m_Surface[j].TB.x,
							   m_pWing->m_Surface[j].TB.y,
							   m_pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
		}
		//tip patches
		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			if(m_pWing->m_Surface[j].m_bIsTipLeft)
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					glBegin(GL_QUADS);
					{
						m_pWing->m_Surface[j].GetPanel(0,l,TOPSURFACE);
						A = m_pWing->m_Surface[j].TA;
						B = m_pWing->m_Surface[j].LA;
						m_pWing->m_Surface[j].GetPanel(0,l,BOTSURFACE);
						C = m_pWing->m_Surface[j].LA;
						D = m_pWing->m_Surface[j].TA;

						LATB = A-C;
						TALB = D-B;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(A.x,A.y,A.z);
						glVertex3d(B.x,B.y,B.z);
						glVertex3d(C.x,C.y,C.z);
						glVertex3d(D.x,D.y,D.z);
					}
					glEnd();
				}
			}
			if(m_pWing->m_Surface[j].m_bIsTipRight)
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					glBegin(GL_QUADS);
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l,TOPSURFACE);
						A = m_pWing->m_Surface[j].TB;
						B = m_pWing->m_Surface[j].LB;
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l,BOTSURFACE);
						C = m_pWing->m_Surface[j].LB;
						D = m_pWing->m_Surface[j].TB;

						LATB = A-C;
						TALB = D-B;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(A.x,A.y,A.z);
						glVertex3d(B.x,B.y,B.z);
						glVertex3d(C.x,C.y,C.z);
						glVertex3d(D.x,D.y,D.z);
					}
					glEnd();
				}
			}
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
//		style = pMiarex->m_VLMStyle;
		width = pMiarex->m_VLMWidth;

		glColor3d(color.redF(),color.greenF(),color.blueF());

		glLineWidth(1.0);
		glDisable (GL_LINE_STIPPLE);


		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			for(k=0; k<m_pWing->m_Surface[j].m_NYPanels; k++)
			{
				glBegin(GL_QUAD_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(k,l,TOPSURFACE);

						LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
						TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(k,l,BOTSURFACE);

						LATB = m_pWing->m_Surface[j].TB - m_pWing->m_Surface[j].LA;
						TALB = m_pWing->m_Surface[j].LB - m_pWing->m_Surface[j].TA;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(m_pWing->m_Surface[j].LA.x,
								   m_pWing->m_Surface[j].LA.y,
								   m_pWing->m_Surface[j].LA.z);
						glVertex3d(m_pWing->m_Surface[j].LB.x,
								   m_pWing->m_Surface[j].LB.y,
								   m_pWing->m_Surface[j].LB.z);
					}
					glVertex3d(m_pWing->m_Surface[j].TA.x,
							   m_pWing->m_Surface[j].TA.y,
							   m_pWing->m_Surface[j].TA.z);
					glVertex3d(m_pWing->m_Surface[j].TB.x,
							   m_pWing->m_Surface[j].TB.y,
							   m_pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
		}

		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			if(m_pWing->m_Surface[j].m_bIsTipLeft)
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					glBegin(GL_QUADS);
					{
						m_pWing->m_Surface[j].GetPanel(0,l,TOPSURFACE);
						A = m_pWing->m_Surface[j].TA;
						B = m_pWing->m_Surface[j].LA;
						m_pWing->m_Surface[j].GetPanel(0,l,BOTSURFACE);
						C = m_pWing->m_Surface[j].LA;
						D = m_pWing->m_Surface[j].TA;

						LATB = A-C;
						TALB = D-B;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(A.x,A.y,A.z);
						glVertex3d(B.x,B.y,B.z);
						glVertex3d(C.x,C.y,C.z);
						glVertex3d(D.x,D.y,D.z);
					}
					glEnd();
				}
			}
			if(m_pWing->m_Surface[j].m_bIsTipRight)
			{
				for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
				{
					glBegin(GL_QUADS);
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l,TOPSURFACE);
						A = m_pWing->m_Surface[j].TB;
						B = m_pWing->m_Surface[j].LB;
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1,l,BOTSURFACE);
						C = m_pWing->m_Surface[j].LB;
						D = m_pWing->m_Surface[j].TB;

						LATB = A-C;
						TALB = D-B;

						N = LATB *TALB;
						N. Normalize();

						glNormal3d(N.x, N.y, N.z);
						glVertex3d(A.x,A.y,A.z);
						glVertex3d(B.x,B.y,B.z);
						glVertex3d(C.x,C.y,C.z);
						glVertex3d(D.x,D.y,D.z);
					}
					glEnd();
				}
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();
}




void GL3dWingDlg::GLCreateSectionHighlight()
{
//	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int j,l;

	int section = 0;
	for(j=0; j<m_pWing->NWingSection(); j++)
	{
		if(j==m_iSection) break;
		if(fabs(m_pWing->YPosition(j+1)-m_pWing->YPosition(j)) > QMiarex::s_MinPanelSize)
			section++;
	}

	glNewList(SECTIONHIGHLIGHT,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glDisable (GL_LINE_STIPPLE);
		glColor3d(1.0, 0.0, 0.0);
		glLineWidth(3);

		if((m_pWing->m_bSymetric || m_bRightSide) && !m_pWing->m_bIsFin)
		{
			if(m_iSection<m_pWing->NWingSection())
			{
				j = m_pWing->m_NSurfaces/2 + section;
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(0, l, TOPSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
					}

					glVertex3d(m_pWing->m_Surface[j].LA.x,
							   m_pWing->m_Surface[j].LA.y,
							   m_pWing->m_Surface[j].LA.z);

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(0, l, BOTSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
					}
				}
				glEnd();
			}
			else
			{
				j = m_pWing->m_NSurfaces/2 + section -1;
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, TOPSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}

					glVertex3d(m_pWing->m_Surface[j].LB.x,
							   m_pWing->m_Surface[j].LB.y,
							   m_pWing->m_Surface[j].LB.z);

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, BOTSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}
				}
				glEnd();
			}
		}
		if(m_pWing->m_bSymetric || !m_bRightSide)
		{
			if(m_iSection>0 )
			{
				if(!m_pWing->m_bIsFin) j = m_pWing->m_NSurfaces/2 - section;
				else                   j = m_pWing->m_NSurfaces - section;
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(0, l, TOPSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
					}

					glVertex3d(m_pWing->m_Surface[j].LA.x,
							   m_pWing->m_Surface[j].LA.y,
							   m_pWing->m_Surface[j].LA.z);

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(0, l, BOTSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TA.x,
								   m_pWing->m_Surface[j].TA.y,
								   m_pWing->m_Surface[j].TA.z);
					}
				}
				glEnd();
			}
			else
			{
				if(!m_pWing->m_bIsFin) j = m_pWing->m_NSurfaces/2 - 1;
				else                   j = m_pWing->m_NSurfaces - 1;
				glBegin(GL_LINE_STRIP);
				{
					for (l=0; l<m_pWing->m_Surface[j].m_NXPanels; l++)
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, TOPSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}

					glVertex3d(m_pWing->m_Surface[j].LB.x,
							   m_pWing->m_Surface[j].LB.y,
							   m_pWing->m_Surface[j].LB.z);

					for (l=m_pWing->m_Surface[j].m_NXPanels-1; l>=0; l--)
					{
						m_pWing->m_Surface[j].GetPanel(m_pWing->m_Surface[j].m_NYPanels-1, l, BOTSURFACE);
						glVertex3d(m_pWing->m_Surface[j].TB.x,
								   m_pWing->m_Surface[j].TB.y,
								   m_pWing->m_Surface[j].TB.z);
					}
				}
				glEnd();
			}
		}
	}
	glEndList();
}

void GL3dWingDlg::GLDraw3D()
{
//	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;

	glClearColor(pMainFrame->m_BackgroundColor.redF(), pMainFrame->m_BackgroundColor.greenF(), pMainFrame->m_BackgroundColor.blueF(),0.0);

	if(m_bResetglWing)
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
		m_pGLWidget->CreateArcballList(m_ArcBall, 1.0);
		m_GLList+=2;

	}

	if(m_bResetglSectionHighlight || m_bResetglWing)
	{
		if(glIsList(SECTIONHIGHLIGHT))
		{
			glDeleteLists(SECTIONHIGHLIGHT,1);
			m_GLList-=1;
		}
		if(m_iSection>=0)
		{
			GLCreateSectionHighlight();
			m_bResetglSectionHighlight = false;
		}
	}

	if(m_bResetglWing)
	{
		if(glIsList(WINGGEOMETRY))
		{
			glDeleteLists(WINGGEOMETRY,2);
			m_GLList-=2;
		}
		QMiarex *pMiarex = (QMiarex*)s_pMiarex;
		GLCreateGeom(pMiarex, m_pWing, WINGGEOMETRY);

		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh();

		m_bResetglWing = false;
	}
}



void GL3dWingDlg::GLDrawFoils()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int j;
	CFoil *pFoil;

	glColor3d(pMainFrame->m_TextColor.redF(), pMainFrame->m_TextColor.greenF(), pMainFrame->m_TextColor.blueF());

	for(j=0; j<m_pWing->m_NSurfaces; j++)
	{
		pFoil = m_pWing->m_Surface[j].m_pFoilA;

		if(pFoil) m_pGLWidget->renderText(m_pWing->m_Surface[j].m_TA.x, m_pWing->m_Surface[j].m_TA.y, m_pWing->m_Surface[j].m_TA.z,
								    pFoil->m_FoilName);

	}

	j = m_pWing->m_NSurfaces-1;
	pFoil = m_pWing->m_Surface[j].m_pFoilB;
	if(pFoil) m_pGLWidget->renderText(m_pWing->m_Surface[j].m_TB.x, m_pWing->m_Surface[j].m_TB.y, m_pWing->m_Surface[j].m_TB.z,
							    pFoil->m_FoilName);

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




void GL3dWingDlg::GLRenderView()
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

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
		m_pGLWidget->GLSetupLight((GLLightDlg*)s_pGLLightDlg, m_UFOOffset.y, 1.0);
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

		if(m_bAxes)  m_pGLWidget->GLDrawAxes(1, pMiarex->m_3DAxisColor, pMiarex->m_3DAxisStyle, pMiarex->m_3DAxisWidth);

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_bOutline)
		{
			glCallList(WINGGEOMETRY+4);
		}

		if(m_iSection>=0)
		{
			glCallList(SECTIONHIGHLIGHT);
		}

		if(GLLightDlg::IsLightOn())
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

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_bVLMPanels)
		{
			if(!m_bSurfaces) glCallList(MESHBACK);
			glCallList(MESHPANELS);
		}
	}
	if(m_bFoilNames)
	{
		GLDrawFoils();
	}
	if(m_bShowMasses)
	{
		glColor3d(pMiarex->m_MassColor.redF(),pMiarex->m_MassColor.greenF(),pMiarex->m_MassColor.blueF());
		for(int im=0; im<m_pWing->m_MassValue.size(); im++)
		{
			glPushMatrix();
			{
				glTranslated(m_pWing->m_MassPosition[im].x,m_pWing->m_MassPosition[im].y,m_pWing->m_MassPosition[im].z);
				double radius = .02;//2cm
				m_pGLWidget->GLRenderSphere(pMiarex->m_MassColor,radius,18,18);
				m_pGLWidget->renderText(0.0, 0.0, 0.02, m_pWing->m_MassTag[im]);

			}
			glPopMatrix();
		}
	 }

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glPopMatrix();

	glDisable(GL_CLIP_PLANE1);
	glFinish();
}




bool GL3dWingDlg::InitDialog(CWing *pWing)
{
	QString str;
	m_iSection = 0;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	m_bSurfaces = pMiarex->m_bSurfaces;
	m_bOutline = pMiarex->m_bOutline;
	m_bVLMPanels = pMiarex->m_bVLMPanels;

	GetAreaUnit(str, pMainFrame->m_AreaUnit);
	m_pctrlAreaUnit1->setText(str);
	m_pctrlAreaUnit2->setText(str);

	GetLengthUnit(str, pMainFrame->m_LengthUnit);

	m_pctrlLength1->setText(str);
	m_pctrlLength2->setText(str);
	m_pctrlLength3->setText(str);
	m_pctrlLength4->setText(str);
	m_pctrlLength5->setText(str);
	m_pWing = pWing;
	if(!m_pWing) return false;
	ComputeGeometry();

	m_pctrlWingName->setText(m_pWing->m_WingName);
	if(m_pWing->m_WingDescription.length())
	{
		m_pctrlWingDescription->setPlainText(m_pWing->m_WingDescription);
	}
	else
	{
		m_pctrlWingDescription->setPlainText("");
	}

	if(!m_bAcceptName) m_pctrlWingName->setEnabled(false);
	m_pctrlSymetric->setChecked(m_pWing->m_bSymetric);
	m_pctrlRightSide->setChecked(m_pWing->m_bSymetric);
	m_pctrlLeftSide->setEnabled(!m_pWing->m_bSymetric);
	m_pctrlRightSide->setChecked(m_bRightSide);
	m_pctrlLeftSide->setChecked(!m_bRightSide);


	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlLight->setChecked(GLLightDlg::IsLightOn());
	m_pctrlFoilNames->setChecked(m_bFoilNames);
	m_pctrlShowMasses->setChecked(m_bShowMasses);

	m_pctrlWingColor->SetColor(m_pWing->m_WingColor);

	m_pWingModel = new QStandardItemModel;
	m_pWingModel->setRowCount(30);//temporary
	m_pWingModel->setColumnCount(10);

	m_pWingModel->setHeaderData(0, Qt::Horizontal, tr("y (")+str+")");
	m_pWingModel->setHeaderData(1, Qt::Horizontal, tr("chord (")+str+")");
	m_pWingModel->setHeaderData(2, Qt::Horizontal, tr("offset (")+str+")");
	m_pWingModel->setHeaderData(3, Qt::Horizontal, QObject::tr("dihedral"));
	m_pWingModel->setHeaderData(4, Qt::Horizontal, QObject::tr("twist")+QString::fromUtf8("(°)"));
	m_pWingModel->setHeaderData(5, Qt::Horizontal, QObject::tr("foil"));
	m_pWingModel->setHeaderData(6, Qt::Horizontal, QObject::tr("X-panels"));
	m_pWingModel->setHeaderData(7, Qt::Horizontal, QObject::tr("X-dist"));
	m_pWingModel->setHeaderData(8, Qt::Horizontal, QObject::tr("Y-panels"));
	m_pWingModel->setHeaderData(9, Qt::Horizontal, QObject::tr("Y-dist"));

	m_pctrlWingTable->setModel(m_pWingModel);

	QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pWingModel);
	m_pctrlWingTable->setSelectionModel(selectionModel);
	connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));


	m_pWingDelegate = new WingDelegate(this);
	m_pctrlWingTable->setItemDelegate(m_pWingDelegate);
	connect(m_pWingDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	int  *precision = new int[10];
	precision[0] = 3;
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 1;
	precision[4] = 2;
	precision[5] = 1;
	precision[6] = 0;
	precision[7] = 0;
	precision[8] = 0;
	precision[9] = 0;
	m_pWingDelegate->SetPrecision(precision);
	m_pWingDelegate->m_pWingSection = &m_pWing->m_WingSection;
	m_pWingDelegate->m_poaFoil = s_poaFoil;
	FillDataTable();
	SetWingData();
	m_pctrlWingTable->selectRow(m_iSection);
	SetCurrentSection(m_iSection);
	return true;
}


void GL3dWingDlg::keyPressEvent(QKeyEvent *event)
{
//	bool bShift = false;
//	bool bCtrl  = false;
//	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
//	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                      accept();

			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		case Qt::Key_Delete:
		{
			OnDeleteSection();
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

		default:
			event->ignore();
	}
}


bool GL3dWingDlg::LoadSettings(QSettings *pSettings)
{
	return true;
}


void GL3dWingDlg::MouseDoubleClickEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());

	Set3DRotationCenter(point);
	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
	UpdateView();
}

void GL3dWingDlg::MouseMoveEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x(), event->pos().y());
	QPoint glPoint(event->pos().x() + m_pGLWidget->geometry().x(), event->pos().y()+m_pGLWidget->geometry().y());
	m_MousePos = event->pos();
	CVector Real;

	QPoint Delta(point.x() - m_LastPoint.x(), point.y() - m_LastPoint.y());
	m_pGLWidget->ClientToGL(point, Real);

//	if(!m_pGLWidget->hasFocus()) m_pGLWidget->setFocus();

	bool bCtrl = false;

	if (event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if (event->buttons()   & Qt::LeftButton)
	{
		if(bCtrl&& m_pGLWidget->geometry().contains(glPoint))
		{
			//rotate
			m_ArcBall.Move(point.x(), m_pGLWidget->m_rCltRect.height()-point.y());
			UpdateView();
		}
		else if(m_bTrans)
		{
			//translate
			if(m_pGLWidget->geometry().contains(glPoint))
			{
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0/m_glScaled/m_pGLWidget->m_rCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0/m_glScaled/m_pGLWidget->m_rCltRect.width());

				m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
		}
	}

	else if (event->buttons() & Qt::MidButton)
	{
		if(m_pWing)
		{		
			m_ArcBall.Move(point.x(), m_pGLWidget->m_rCltRect.height()-point.y());
			UpdateView();
		}
	}

	m_LastPoint = point;
}


void GL3dWingDlg::MousePressEvent(QMouseEvent *event)
{
	// the event has been sent by GLWidget, so event is in GL Widget coordinates
	// but m_3DWingRect is in window client coordinates
	// the difference is m_pGLWidget->geometry() !

	QPoint point(event->pos().x(), event->pos().y());
	QPoint glPoint(event->pos().x() + m_pGLWidget->geometry().x(), event->pos().y()+m_pGLWidget->geometry().y());

	CVector Real;
	bool bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	m_pGLWidget->ClientToGL(point, Real);

	if(m_pGLWidget->geometry().contains(glPoint)) m_pGLWidget->setFocus();
	
	if (event->buttons() & Qt::MidButton)
	{
		m_bArcball = true;
		m_ArcBall.Start(event->pos().x(), m_pGLWidget->m_rCltRect.height()-event->pos().y());
		m_bCrossPoint = true;

		Set3DRotationCenter();
		UpdateView();
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		if(m_bPickCenter)
		{
			Set3DRotationCenter(point);
			m_bPickCenter = false;
			m_pctrlPickCenter->setChecked(false);
		}
		else
		{
			m_bTrans=true;
	
			if(m_pWing && m_pGLWidget->geometry().contains(glPoint))
			{
				m_ArcBall.Start(point.x(), m_pGLWidget->m_rCltRect.height()-point.y());
				m_bCrossPoint = true;
				Set3DRotationCenter();
				if (!bCtrl)
				{
					m_bTrans = true;
					m_pGLWidget->setCursor(Qt::ClosedHandCursor);
				}
				else
				{
					m_bArcball = true;
				}
				UpdateView();
			}
		}
	}

	m_bPickCenter = false;
	m_pctrlPickCenter->setChecked(false);
	m_PointDown = point;
	m_LastPoint = point;
}



void GL3dWingDlg::MouseReleaseEvent(QMouseEvent *event)
{
	m_pGLWidget->setCursor(Qt::CrossCursor);
	
	m_bTrans = false;
	m_bDragPoint  = false;

	m_bArcball = false;
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



void GL3dWingDlg::OnCellChanged(QWidget *pWidget)
{
	m_bChanged = true;
	m_bResetglWing = true;
	ReadParams();
	SetWingData();
	UpdateView();
}



void GL3dWingDlg::OnDeleteSection()
{
	if(m_iSection <0 || m_iSection>m_pWing->NWingSection()) return;

	if(m_iSection==0)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The first section cannot be deleted"));
		return;
	}
	m_pctrlWingTable->closePersistentEditor(m_pctrlWingTable->currentIndex());

	int ny, size;

	size = m_pWingModel->rowCount();
	if(size<=2) return;

	ny = m_pWing->NYPanels(m_iSection-1) + m_pWing->NYPanels(m_iSection);

/*	for (k=m_iSection; k<size-1; k++)
	{
		m_pWing->TPos(k)      = m_pWing->TPos(k+1);
		m_pWing->TChord(k)    = m_pWing->TChord(k+1);
		m_pWing->TOffset(k)   = m_pWing->TOffset(k+1);
		m_pWing->TTwist(k)     = m_pWing->TTwist(k+1);
		m_pWing->TDihedral(k)  = m_pWing->TDihedral(k+1);
		m_pWing->NXPanels(k)   = m_pWing->NXPanels(k+1);
		m_pWing->NYPanels(k)   = m_pWing->NYPanels(k+1);
		m_pWing->XPanelDist(k) = m_pWing->XPanelDist(k+1);
		m_pWing->YPanelDist(k) = m_pWing->YPanelDist(k+1);
	}

	m_pWing->m_RightFoil.removeAt(m_iSection);
	m_pWing->m_LeftFoil.removeAt(m_iSection);*/

	m_pWing->RemoveWingSection(m_iSection);


	m_pWing->NYPanels(m_iSection-1) = ny;

	FillDataTable();
	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
	m_bResetglWing = true;
	UpdateView();
}



void GL3dWingDlg::OnFoilNames()
{
	m_bFoilNames = m_pctrlFoilNames->isChecked();
	UpdateView();
}



void GL3dWingDlg::OnShowMasses()
{
	m_bShowMasses = m_pctrlShowMasses->isChecked();
	UpdateView();
}


void GL3dWingDlg::OnInertia()
{
    InertiaDlg dlg(this);
	dlg.s_pMainFrame = s_pMainFrame;
	dlg.m_pWing = m_pWing;

	//save inertia properties
	QList<double>MassValue;
	QList<CVector> MassPosition;
	QStringList MassTag;

	for(int i=0; i< m_pWing->m_MassValue.size(); i++)
	{
		MassValue.append(m_pWing->m_MassValue[i]);
		MassPosition.append(m_pWing->m_MassPosition[i]);
		MassTag.append(m_pWing->m_MassTag[i]);
	}

	dlg.InitDialog();
	if(dlg.exec() == QDialog::Accepted)
	{
		if(dlg.m_bChanged) m_bChanged = true;
	}
	else
	{
		// restore saved inertia
		m_pWing->m_MassValue.clear();
		m_pWing->m_MassPosition.clear();
		m_pWing->m_MassTag.clear();

		for(int i=0; i< MassValue.size(); i++)
		{
			m_pWing->m_MassValue.append(MassValue[i]);
			m_pWing->m_MassPosition.append(MassPosition[i]);
			m_pWing->m_MassTag.append(MassTag[i]);
		}
	}
}



void GL3dWingDlg::OnLight()
{
	GLLightDlg::SetLightOn(m_pctrlLight->isChecked());
	UpdateView();
}



void GL3dWingDlg::OnInsertBefore()
{
	if(m_iSection <0 || m_iSection>m_pWing->NWingSection()) return;

	if (m_pWing->NWingSection()>=MAXSPANSECTIONS)
	{
		QMessageBox::warning(this, tr("Warning"), tr("The maximum number of panels has been reached"));
		return;
	}
	if(m_iSection<=0)
	{
		QMessageBox::warning(this, tr("Warning"), tr("No insertion possible before the first section"));
		return;
	}

	int n = m_iSection;

	m_pWing->InsertSection(m_iSection);

	m_pWing->YPosition(n)       = (m_pWing->YPosition(n+1)      + m_pWing->YPosition(n-1))     /2.0;
	m_pWing->Chord(n)     = (m_pWing->Chord(n+1)    + m_pWing->Chord(n-1))   /2.0;
	m_pWing->Offset(n)    = (m_pWing->Offset(n+1)   + m_pWing->Offset(n-1))  /2.0;
	m_pWing->Twist(n)     = (m_pWing->Twist(n+1)    + m_pWing->Twist(n-1))   /2.0;
	m_pWing->Dihedral(n)  = (m_pWing->Dihedral(n+1) + m_pWing->Dihedral(n-1))/2.0;

	m_pWing->XPanelDist(n) = m_pWing->XPanelDist(n-1);
	m_pWing->YPanelDist(n) = m_pWing->YPanelDist(n-1);

	m_pWing->RightFoil(n) = m_pWing->RightFoil(n-1);
	m_pWing->LeftFoil(n)  = m_pWing->LeftFoil(n-1);

	m_pWing->NXPanels(n)   = m_pWing->NXPanels(n-1);


	int ny = m_pWing->NYPanels(n-1);
	m_pWing->NYPanels(n)   = (int)(ny/2);
	m_pWing->NYPanels(n-1) = ny-m_pWing->NYPanels(n);
	if(m_pWing->NYPanels(n)==0)   m_pWing->NYPanels(n)++;
	if(m_pWing->NYPanels(n-1)==0) m_pWing->NYPanels(n-1)++;


	FillDataTable();
	m_pctrlWingTable->closePersistentEditor(m_pctrlWingTable->currentIndex());
	ComputeGeometry();
	SetWingData();

	m_bChanged = true;
	m_bResetglSectionHighlight = true;
	m_bResetglWing = true;
	UpdateView();
}




void GL3dWingDlg::OnInsertAfter()
{
	if(m_iSection <0 || m_iSection>=m_pWing->NWingSection()) return;
	if (m_pWing->NWingSection()>=MAXSPANSECTIONS)
	{
		QMessageBox::warning(this, tr("Warning"), tr("The maximum number of panels has been reached"));
		return;
	}
	int n = m_iSection;

	if(n<0) n=m_pWing->NWingSection();

	m_pWing->InsertSection(m_iSection+1);

	if(n<m_pWing->NWingSection()-2)
	{
		m_pWing->YPosition(n+1)      = (m_pWing->YPosition(n)      + m_pWing->YPosition(n+2))     /2.0;
		m_pWing->Chord(n+1)    = (m_pWing->Chord(n)    + m_pWing->Chord(n+2))   /2.0;
		m_pWing->Offset(n+1)   = (m_pWing->Offset(n)   + m_pWing->Offset(n+2))  /2.0;
		m_pWing->Twist(n+1)    = (m_pWing->Twist(n)    + m_pWing->Twist(n+2))   /2.0;
	}
	else
	{
		m_pWing->YPosition(n+1)     = m_pWing->YPosition(n)*1.1;
		m_pWing->Chord(n+1)   = m_pWing->Chord(n)/1.1;
		m_pWing->Offset(n+1)  = m_pWing->Offset(n) + m_pWing->Chord(n) - m_pWing->Chord(n) ;
		m_pWing->Twist(n+1)     = m_pWing->Twist(n);
	}

	m_pWing->Dihedral(n+1)  = m_pWing->Dihedral(n);
	m_pWing->NXPanels(n+1)   = m_pWing->NXPanels(n);
	m_pWing->NYPanels(n+1)   = m_pWing->NYPanels(n);
	m_pWing->XPanelDist(n+1) = m_pWing->XPanelDist(n);
	m_pWing->YPanelDist(n+1) = m_pWing->YPanelDist(n);
	m_pWing->RightFoil(n+1)  = m_pWing->RightFoil(n);
	m_pWing->LeftFoil(n+1)   = m_pWing->LeftFoil(n);

	int ny = m_pWing->NYPanels(n);
	m_pWing->NYPanels(n+1) = qMax(1,(int)(ny/2));
	m_pWing->NYPanels(n)   = qMax(1,ny-m_pWing->NYPanels(n+1));

//	m_pWing->m_bVLMAutoMesh = true;

	FillDataTable();
	m_pctrlWingTable->closePersistentEditor(m_pctrlWingTable->currentIndex());

	ComputeGeometry();
	SetWingData();
	m_bChanged = true;
	m_bResetglWing = true;
	UpdateView();
}

void GL3dWingDlg::OnResetSection()
{
	int n = m_iSection;

	if((0 < n) && (n < (m_pWing->NWingSection()-1)))
	{
	        double ratio;
	        ratio = (m_pWing->YPosition(n) - m_pWing->YPosition(n - 1)) / (m_pWing->YPosition(n + 1) - m_pWing->YPosition(n - 1));
                
	        m_pWing->Chord   (n) = m_pWing->Chord   (n-1) + ratio * (m_pWing->Chord   (n+1) - m_pWing->Chord   (n-1));
	        m_pWing->Offset  (n) = m_pWing->Offset  (n-1) + ratio * (m_pWing->Offset  (n+1) - m_pWing->Offset  (n-1));
	        m_pWing->Twist   (n) = m_pWing->Twist   (n-1) + ratio * (m_pWing->Twist   (n+1) - m_pWing->Twist   (n-1));
                
                // same code here that in OnResetMesh
	        FillDataTable();
	        SetWingData();
	        ComputeGeometry();
	        m_bChanged = true;
	        m_bResetglWing = true;
	        UpdateView();
	}
}


void GL3dWingDlg::OnItemClicked(const QModelIndex &index)
{
	if(index.row()>=m_pWing->NWingSection())
	{
		//the user has filled a cell in the last line
		if(index.row()<MAXSPANSECTIONS-1)
		{
			//so add an item before reading
			m_pWingModel->setRowCount(m_pWing->NWingSection()+1);
			FillTableRow(m_pWing->NWingSection());
		}
	}
	SetCurrentSection(index.row());
	UpdateView();
}



void GL3dWingDlg::OnOK()
{
	ReadParams();

	if(!CheckWing()) return;

	if(m_pWing->m_bSymetric)
	{
		for (int i=0; i<m_pWing->NWingSection(); i++)
		{
			m_pWing->LeftFoil(i)   = m_pWing->RightFoil(i);
		}
	}

	m_pWing->ComputeGeometry();
	m_pWing->ComputeBodyAxisInertia();

	s_bWindowMaximized= isMaximized();
	s_WindowPos = pos();
	s_WindowSize = size();

	accept();
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




void GL3dWingDlg::OnResetMesh()
{
	VLMSetAutoMesh();
	FillDataTable();
	SetWingData();
	ComputeGeometry();
	m_bChanged = true;
	m_bResetglWing = true;
	UpdateView();
}



void GL3dWingDlg::OnScaleWing()
{
    WingScaleDlg dlg(this);
	dlg.InitDialog(m_pWing->m_PlanformSpan, m_pWing->Chord(0), m_pWing->AverageSweep(), m_pWing->Twist(m_pWing->NWingSection()-1));

	if(QDialog::Accepted == dlg.exec())
	{
		if (dlg.m_bSpan || dlg.m_bChord || dlg.m_bSweep || dlg.m_bTwist)
		{
			if(dlg.m_bSpan)  m_pWing->ScaleSpan(dlg.m_NewSpan);
			if(dlg.m_bChord) m_pWing->ScaleChord(dlg.m_NewChord);
			if(dlg.m_bSweep) m_pWing->ScaleSweep(dlg.m_NewSweep);
			if(dlg.m_bTwist) m_pWing->ScaleTwist(dlg.m_NewTwist);
		}

		FillDataTable();
		m_bChanged = true;
		m_bResetglWing = true;
		m_bResetglSectionHighlight = true;
		ComputeGeometry();
		UpdateView();
	}
}


void GL3dWingDlg::OnSide()
{
	m_bRightSide = m_pctrlRightSide->isChecked();
	FillDataTable();

	m_bChanged = true;
	m_bResetglSectionHighlight = true;
	UpdateView();
}


void GL3dWingDlg::OnSurfaces()
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
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
		for(int i=0; i<m_pWing->NWingSection(); i++)
		{
			m_pWing->LeftFoil(i) = m_pWing->RightFoil(i);
		}
	}
	else
	{
		m_pWing->m_bSymetric    = false;
		m_pctrlLeftSide->setEnabled(true);
	}

	m_bChanged = true;
	ComputeGeometry();
	m_bResetglWing             = true;
	m_bResetglSectionHighlight = true;
	UpdateView();
}


void GL3dWingDlg::OnWingColor()
{
	if(!m_pWing) return;

    QColorDialog::ColorDialogOptions dialogOptions = QColorDialog::ShowAlphaChannel;
#ifdef Q_WS_MAC
#if QT_VERSION >= 0x040700
    dialogOptions |= QColorDialog::DontUseNativeDialog;
#endif
#endif
	QColor WingColor = QColorDialog::getColor(m_pWing->m_WingColor,
                                      this, "Select the wing color", dialogOptions);
    if(WingColor.isValid()) m_pWing->m_WingColor = WingColor;

	m_pctrlWingColor->SetColor(m_pWing->m_WingColor);
	m_bResetglWing = true;
	UpdateView();
}



void GL3dWingDlg::ReadParams()
{
	m_pWing->m_WingName = m_pctrlWingName->text();
	QString strange = m_pctrlWingDescription->toPlainText();
	if(strange == tr("Wing Description")) strange="";
	m_pWing->m_WingDescription = strange;

	for (int i=0; i< m_pWingModel->rowCount();  i++)
	{
		ReadSectionData(i);
	}

	//Update Geometry
	ComputeGeometry();
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
	if(bOK) m_pWing->YPosition(sel) =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,1);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->Chord(sel) =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,2);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->Offset(sel) =d / pMainFrame->m_mtoUnit;

	pItem = m_pWingModel->item(sel,3);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->Dihedral(sel) =d;

	pItem = m_pWingModel->item(sel,4);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->Twist(sel) =d;

	pItem = m_pWingModel->item(sel,5);
	strong =pItem->text();
	if(m_pWing->m_bSymetric)
	{
		m_pWing->RightFoil(sel) = strong;
		m_pWing->LeftFoil(sel)  = strong;
	}
	else
	{
		if(m_bRightSide)	m_pWing->RightFoil(sel) = strong;
		else                m_pWing->LeftFoil(sel)  = strong;
	}

	pItem = m_pWingModel->item(sel,6);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->NXPanels(sel) =(int)qMax(1.0,d);
	m_pWing->NXPanels(sel) = qMin(m_pWing->NXPanels(sel), MAXCHORDPANELS);

	pItem = m_pWingModel->item(sel,7);
	strong =pItem->text();
	strong.replace(" ","");
	if(strong==tr("Uniform"))		m_pWing->XPanelDist(sel) =  0;
	else if(strong==tr("Cosine"))	m_pWing->XPanelDist(sel) =  1;
	else if(strong==tr("Sine"))		m_pWing->XPanelDist(sel) =  2;
	else if(strong==tr("-Sine"))	m_pWing->XPanelDist(sel) = -2;

	pItem = m_pWingModel->item(sel,8);
	strong =pItem->text();
	strong.replace(" ","");
	d =strong.toDouble(&bOK);
	if(bOK) m_pWing->NYPanels(sel) =(int)qMax(1.0,d);

	pItem = m_pWingModel->item(sel,9);
	strong =pItem->text();
	strong.replace(" ","");

	if(strong==tr("Uniform"))		m_pWing->YPanelDist(sel) =  0;
	else if(strong==tr("Cosine"))	m_pWing->YPanelDist(sel) =  1;
	else if(strong==tr("Sine"))		m_pWing->YPanelDist(sel) =  2;
	else if(strong==tr("-Sine"))	m_pWing->YPanelDist(sel) = -2;

}


void GL3dWingDlg::reject()
{
	if(m_bChanged)
	{
		QString strong = tr("Save the changes ?");
		int Ans = QMessageBox::question(this, tr("Question"), strong,
										QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if (QMessageBox::Yes == Ans)
		{
			OnOK();
			return;
		}
		else if(QMessageBox::Cancel == Ans) return;
	}

	s_bWindowMaximized= isMaximized();
	s_WindowPos = pos();
	s_WindowSize = size();
//	reject();
	done(QDialog::Rejected);
}


void GL3dWingDlg::resizeEvent(QResizeEvent *event)
{
}


bool GL3dWingDlg::SaveSettings(QSettings *pSettings)
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
	CVector N, LA, LB, TA, TB;
	CVector I, A, B, AA, BB, PP, U;

	i=-1;

	m_pGLWidget->ClientToGL(point, B);

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
	double dist;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	for(j=0; j<m_pWing->m_NSurfaces; j++)
	{
		N = m_pWing->m_Surface[j].Normal;
		LA = m_pWing->m_Surface[j].m_LA;
		TA = m_pWing->m_Surface[j].m_TA;
		LB = m_pWing->m_Surface[j].m_LB;
		TB = m_pWing->m_Surface[j].m_TB;
		bIntersect = pMiarex->Intersect(m_pWing->m_Surface[j].m_LA,
										m_pWing->m_Surface[j].m_LB,
										m_pWing->m_Surface[j].m_TA,
										m_pWing->m_Surface[j].m_TB,
										m_pWing->m_Surface[j].Normal,
										AA, U, I, dist);
		if(bIntersect)
		{
			PP.Set(I);
			break;
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



void GL3dWingDlg::SetCurrentSection(int section)
{
	m_iSection = section;
	if(m_iSection <0 || m_iSection>m_pWing->NWingSection())
	{
		m_pctrlInsertAfter->setEnabled(false);
		m_pctrlInsertBefore->setEnabled(false);
		m_pctrlDeleteSection->setEnabled(false);
		m_pctrlResetSection->setEnabled(false);
	}
	else
	{
		m_pctrlInsertAfter->setEnabled(true);
		m_pctrlInsertBefore->setEnabled(true);
		m_pctrlDeleteSection->setEnabled(true);
		m_pctrlResetSection->setEnabled(true);

		QString str;
		str = tr("Insert after section") +" %1";
		str = QString(str).arg(m_iSection+1);
		m_pctrlInsertAfter->setText(str);

		str = tr("Insert before section") +" %1";
		str = QString(str).arg(m_iSection+1);
		m_pctrlInsertBefore->setText(str);

		str = tr("Delete section") +" %1";
		str = QString(str).arg(m_iSection+1);
		m_pctrlDeleteSection->setText(str);

		str = tr("Reset section") +" %1";
		str = QString(str).arg(m_iSection+1);
		m_pctrlResetSection->setText(str);
	}
	m_bResetglSectionHighlight = true;
}


void GL3dWingDlg::SetWingScale()
{
//if(m_bIs3DScaleSet) return;

	//wing along X axis will take 3/4 of the screen
	m_glScaled = (GLfloat)(3./4.*2.0/m_pWing->m_PlanformSpan);
	m_glViewportTrans.x = 0.0;
	m_glViewportTrans.y = 0.0;
	m_glViewportTrans.z = 0.0;

	m_UFOOffset.x = 0.0;
	m_UFOOffset.y = 0.0;

	m_ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	m_ArcBall.SetZoom(0.3,eye,up);

	Set3DRotationCenter();
}


void GL3dWingDlg::SetWingData()
{
	if(!m_pWing) return;
	//Updates the wing's properties after a change of geometry

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

	str = QString("%1").arg(m_pWing->m_PlanformArea*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlWingArea->setText(str);

	str = QString("%1").arg(m_pWing->m_PlanformSpan*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlWingSpan->setText(str);

	str = QString("%1").arg(m_pWing->m_ProjectedArea*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlProjectedArea->setText(str);

	str = QString("%1").arg(m_pWing->m_ProjectedSpan*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlProjectedSpan->setText(str);

	str = QString("%1").arg(m_pWing->m_GChord*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlGeomChord->setText(str);

	str = QString("%1").arg(m_pWing->m_MAChord*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlMAC->setText(str);

	str = QString("%1").arg(m_pWing->m_yMac*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlMACSpanPos->setText(str);

	str = QString("%1").arg(m_pWing->m_AR,5,'f',2);
	m_pctrlAspectRatio->setText(str);

	if(m_pWing->TipChord()>0.0) str = QString("%1").arg(m_pWing->m_TR,0,'f',2);
	else                        str = tr("Undefined");
	m_pctrlTaperRatio->setText(str);

	str = QString("%1").arg(m_pWing->AverageSweep(),5,'f',2);
	m_pctrlSweep->setText(str);

	int VLMTotal=100;
	VLMTotal = VLMGetPanelTotal();
	str = QString("%1").arg(VLMTotal);
	m_pctrlVLMPanels->setText(str);

	VLMTotal *= 2;
	VLMTotal += 2*m_pWing->NXPanels(m_pWing->NWingSection()-1);
	str = QString("%1").arg(VLMTotal);
	m_pctrl3DPanels->setText(str);
}



void GL3dWingDlg::SetupLayout()
{
//	QDesktopWidget desktop;
//	QRect r = desktop.screenGeometry();
//	setMaximumHeight(r.height());
	setMinimumHeight(700);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	m_pGLWidget = new ThreeDWidget(this);
	m_pGLWidget->m_iView = GLWINGVIEW;
	m_ArcBall.m_p3dWidget = m_pGLWidget;

/*_____________Start Top Layout Here____________*/
	QVBoxLayout *DefLayout = new QVBoxLayout;
	{
		QHBoxLayout *SymLayout = new QHBoxLayout;
		{
			m_pctrlSymetric     = new QCheckBox(tr("Symetric"));
			m_pctrlRightSide    = new QRadioButton(tr("Right Side"));
			m_pctrlLeftSide     = new QRadioButton(tr("Left Side"));
			m_pctrlInsertBefore   = new QPushButton("Insert Before");
			m_pctrlInsertAfter    = new QPushButton("Insert After");
			m_pctrlDeleteSection  = new QPushButton("Delete Section");
			m_pctrlResetSection   = new QPushButton("Reset Section");

			SymLayout->addWidget(m_pctrlSymetric);
			SymLayout->addWidget(m_pctrlRightSide);
			SymLayout->addWidget(m_pctrlLeftSide);
			SymLayout->addStretch(1);
			SymLayout->addWidget(m_pctrlInsertBefore);
			SymLayout->addWidget(m_pctrlInsertAfter);
			SymLayout->addWidget(m_pctrlDeleteSection);
			SymLayout->addWidget(m_pctrlResetSection);
		}

		QHBoxLayout *NameLayout = new QHBoxLayout;
		{
			m_pctrlWingName     = new QLineEdit(tr("WingName"));
			m_pctrlWingColor    = new ColorButton;
			NameLayout->addWidget(m_pctrlWingName);
			NameLayout->addWidget(m_pctrlWingColor);

			m_pctrlWingTable = new QTableView(this);
			m_pctrlWingTable->setWindowTitle(QObject::tr("Wing definition"));
			m_pctrlWingTable->setSelectionMode(QAbstractItemView::SingleSelection);
			m_pctrlWingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
			m_pctrlWingTable->setEditTriggers(QAbstractItemView::CurrentChanged |
											  QAbstractItemView::DoubleClicked |
											  QAbstractItemView::SelectedClicked |
											  QAbstractItemView::EditKeyPressed |
											  QAbstractItemView::AnyKeyPressed);
			QHeaderView *HorizontalHeader = m_pctrlWingTable->horizontalHeader();
			HorizontalHeader->setStretchLastSection(true);
		}

		DefLayout->addLayout(NameLayout);
		DefLayout->addLayout(SymLayout);
		DefLayout->addWidget(m_pctrlWingTable,1);
	}

	m_pctrlControlsWidget = new QWidget;
	m_pctrlControlsWidget->setLayout(DefLayout);

	QVBoxLayout *LeftLayout = new QVBoxLayout;

	LeftLayout->addWidget(m_pctrlControlsWidget);
	LeftLayout->addWidget(m_pGLWidget,1);


	QGridLayout *DataLayout = new QGridLayout;
	{
		m_pctrlLength1    = new QLabel("mm");
		m_pctrlLength2    = new QLabel("mm");
		m_pctrlLength3    = new QLabel("mm");
		m_pctrlLength4    = new QLabel("mm");
		m_pctrlLength5    = new QLabel("mm");
		m_pctrlAreaUnit1  = new QLabel("mm2");
		m_pctrlAreaUnit2  = new QLabel("mm2");
		m_pctrlLength1->setAlignment(Qt::AlignLeft);
		m_pctrlLength2->setAlignment(Qt::AlignLeft);
		m_pctrlLength3->setAlignment(Qt::AlignLeft);
		m_pctrlLength4->setAlignment(Qt::AlignLeft);
		m_pctrlLength5->setAlignment(Qt::AlignLeft);
		m_pctrlAreaUnit1->setAlignment(Qt::AlignLeft);
		m_pctrlAreaUnit2->setAlignment(Qt::AlignLeft);

		QLabel *lab1 = new QLabel(tr("Wing Span"));
		QLabel *lab2 = new QLabel(tr("Area"));
		QLabel *lab3 = new QLabel(tr("Projected Span"));
		QLabel *lab4 = new QLabel(tr("Projected Area"));
		QLabel *lab13 = new QLabel(tr("Total VLM Panels"));
		QLabel *lab14 = new QLabel(tr("Number of 3D Panels"));
		lab1->setAlignment(Qt::AlignRight);
		lab2->setAlignment(Qt::AlignRight);
		lab3->setAlignment(Qt::AlignRight);
		lab4->setAlignment(Qt::AlignRight);
		lab13->setAlignment(Qt::AlignRight);
		lab14->setAlignment(Qt::AlignRight);
		DataLayout->addWidget(lab1,1,1);
		DataLayout->addWidget(lab2,2,1);
		DataLayout->addWidget(lab3,3,1);
		DataLayout->addWidget(lab4,4,1);
		DataLayout->addWidget(lab13,13,1);
		DataLayout->addWidget(lab14,14,1);
		m_pctrlWingSpan      = new QLabel("2000.00");
		m_pctrlWingArea      = new QLabel("30.0");
		m_pctrlProjectedArea = new QLabel("25.0");
		m_pctrlProjectedSpan = new QLabel("1900.0");;
		m_pctrlVLMPanels     = new QLabel("500");
		m_pctrl3DPanels      = new QLabel("1000");
		m_pctrlWingSpan->setAlignment(Qt::AlignRight);
		m_pctrlWingArea->setAlignment(Qt::AlignRight);
		m_pctrlProjectedSpan->setAlignment(Qt::AlignRight);
		m_pctrlProjectedArea->setAlignment(Qt::AlignRight);
		m_pctrlVLMPanels->setAlignment(Qt::AlignRight);
		m_pctrl3DPanels->setAlignment(Qt::AlignRight);
		DataLayout->addWidget(m_pctrlWingSpan,   1,2);
		DataLayout->addWidget(m_pctrlWingArea,   2,2);
		DataLayout->addWidget(m_pctrlProjectedSpan,   3,2);
		DataLayout->addWidget(m_pctrlProjectedArea,   4,2);
		DataLayout->addWidget(m_pctrlVLMPanels, 13,2);
		DataLayout->addWidget(m_pctrl3DPanels,  14,2);

		DataLayout->addWidget(m_pctrlLength1,1,3);
		DataLayout->addWidget(m_pctrlAreaUnit1,2,3);
		DataLayout->addWidget(m_pctrlLength2,3,3);
		DataLayout->addWidget(m_pctrlAreaUnit2,4,3);
		QString strong = QString("%1").arg(VLMMAXMATSIZE/2);
		QLabel *lab15 = new QLabel(tr("Max is ")+strong);
		lab13->setAlignment(Qt::AlignLeft);
		DataLayout->addWidget(lab15 ,13,3);
		strong = QString("%1").arg(VLMMAXMATSIZE);
		QLabel *lab16 = new QLabel(tr("Max is ")+strong);
		lab15->setAlignment(Qt::AlignLeft);
		DataLayout->addWidget(lab16, 14, 3);


		QLabel *lab20 = new QLabel(tr("Mean Geom. Chord"));
		QLabel *lab21 = new QLabel(tr("Mean Aero Chord"));
		QLabel *lab22 = new QLabel(tr("MAC Span Pos"));
		QLabel *lab23 = new QLabel(tr("Aspect ratio"));
		QLabel *lab24 = new QLabel(tr("Taper Ratio"));
		QLabel *lab25 = new QLabel(tr("Root to Tip Sweep"));
		QLabel *lab26 = new QLabel(tr("Number of Flaps"));
		lab20->setAlignment(Qt::AlignRight);
		lab21->setAlignment(Qt::AlignRight);
		lab22->setAlignment(Qt::AlignRight);
		lab23->setAlignment(Qt::AlignRight);
		lab24->setAlignment(Qt::AlignRight);
		lab25->setAlignment(Qt::AlignRight);
		lab26->setAlignment(Qt::AlignRight);
		DataLayout->addWidget(lab20,6,1);
		DataLayout->addWidget(lab21,7,1);
	//	DataLayout->addWidget(lab22,8,1);
		DataLayout->addWidget(lab23,9,1);
		DataLayout->addWidget(lab24,10,1);
		DataLayout->addWidget(lab25,11,1);
		DataLayout->addWidget(lab26,12,1);

		m_pctrlGeomChord    = new QLabel("170.0");
		m_pctrlMAC          = new QLabel("150.0");
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
		DataLayout->addWidget(m_pctrlGeomChord,    6,2);
		DataLayout->addWidget(m_pctrlMAC,          7,2);
	//	DataLayout->addWidget(m_pctrlMACSpanPos,   8,2);
		DataLayout->addWidget(m_pctrlAspectRatio,  9,2);
		DataLayout->addWidget(m_pctrlTaperRatio,  10,2);
		DataLayout->addWidget(m_pctrlSweep,       11,2);
		DataLayout->addWidget(m_pctrlNFlaps,      12,2);
		DataLayout->addWidget(m_pctrlLength3, 6, 3);
		DataLayout->addWidget(m_pctrlLength4, 7, 3);
	//	DataLayout->addWidget(m_pctrlLength5, 8, 3);
		QLabel *lab30 = new QLabel(QString::fromUtf8("°"));
		lab30->setAlignment(Qt::AlignLeft);
		DataLayout->addWidget(lab30, 11, 3);
	}


/*_____________End Top Right Layout Here______________*/

	m_pctrlWingDescription = new QTextEdit();
	m_pctrlWingDescription->setToolTip(tr("Enter here a short description for the wing"));
	m_pctrlWingDescription->setSizePolicy(szPolicyMaximum);

	QLabel *WingDescription = new QLabel(tr("Description:"));

/*_____________Start Bottom Right Layout Here_________*/

	QVBoxLayout *RightLayout = new QVBoxLayout;

	QGridLayout *ThreeDParams = new QGridLayout;
	{
		m_pctrlAxes       = new QCheckBox(tr("Axes"));
		m_pctrlLight      = new QCheckBox(tr("Light"));
		m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"));
		m_pctrlOutline    = new QCheckBox(tr("Outline"));
		m_pctrlPanels     = new QCheckBox(tr("Panels"));
		m_pctrlFoilNames  = new QCheckBox(tr("Foil Names"));
		m_pctrlShowMasses = new QCheckBox(tr("Masses"));
		m_pctrlAxes->setSizePolicy(szPolicyMinimum);
		m_pctrlLight->setSizePolicy(szPolicyMinimum);
		m_pctrlSurfaces->setSizePolicy(szPolicyMinimum);
		m_pctrlOutline->setSizePolicy(szPolicyMinimum);
		m_pctrlPanels->setSizePolicy(szPolicyMinimum);
		ThreeDParams->addWidget(m_pctrlAxes, 1,1);
		ThreeDParams->addWidget(m_pctrlPanels, 1,2);
		ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
		ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	//	ThreeDParams->addWidget(m_pctrlLight, 3,1);
		ThreeDParams->addWidget(m_pctrlFoilNames, 3,1);
		ThreeDParams->addWidget(m_pctrlShowMasses, 3,2);
	}

	QVBoxLayout *ThreeDView = new QVBoxLayout;
	{
		QHBoxLayout *AxisViewLayout = new QHBoxLayout;
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
			m_pXView   = new QAction(QIcon(":/images/OnXView.png"), tr("X View"), this);
			m_pYView   = new QAction(QIcon(":/images/OnYView.png"), tr("Y View"), this);
			m_pZView   = new QAction(QIcon(":/images/OnZView.png"), tr("Z View"), this);
			m_pIsoView = new QAction(QIcon(":/images/OnIsoView.png"), tr("Iso View"), this);
			m_pXView->setCheckable(true);
			m_pYView->setCheckable(true);
			m_pZView->setCheckable(true);
			m_pIsoView->setCheckable(true);

			m_pctrlX->setDefaultAction(m_pXView);
			m_pctrlY->setDefaultAction(m_pYView);
			m_pctrlZ->setDefaultAction(m_pZView);
			m_pctrlIso->setDefaultAction(m_pIsoView);
			AxisViewLayout->addWidget(m_pctrlX);
			AxisViewLayout->addWidget(m_pctrlY);
			AxisViewLayout->addWidget(m_pctrlZ);
			AxisViewLayout->addWidget(m_pctrlIso);
		}


		QHBoxLayout *ViewResetLayout = new QHBoxLayout;
		{
			m_pctrlPickCenter     = new QPushButton(tr("Pick Center"));
			m_pctrlPickCenter->setToolTip(tr("Activate the button, then click on the object to center it in the viewport; alternatively, double click on the object"));
			m_pctrlReset          = new QPushButton(tr("Reset"));
			m_pctrlPickCenter->setCheckable(true);

			ViewResetLayout->addWidget(m_pctrlReset);
			ViewResetLayout->addWidget(m_pctrlPickCenter);
		}
		ThreeDView->addLayout(AxisViewLayout);
		ThreeDView->addLayout(ViewResetLayout);
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

	QHBoxLayout *WingModCommands = new QHBoxLayout;
	{
		m_pctrlResetMesh    = new QPushButton(tr("Reset Mesh"));
		m_pctrlScaleWing    = new QPushButton(tr("Scale Wing"));
		m_pctrlInertiaButton = new QPushButton(tr("Inertia..."));
	//	m_pctrlSetupLight   = new QPushButton("Setup Light");
		WingModCommands->addWidget(m_pctrlResetMesh);
		WingModCommands->addWidget(m_pctrlScaleWing);
		WingModCommands->addWidget(m_pctrlInertiaButton);
	//	WingModCommands->addWidget(m_pctrlSetupLight);
	}


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("Save and Close"));
		OKButton->setAutoDefault(true);
		CancelButton = new QPushButton(tr("Cancel"));
		CancelButton->setAutoDefault(false);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addWidget(CancelButton);
		connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
		connect(CancelButton, SIGNAL(clicked()),this, SLOT(reject()));
	}


	QVBoxLayout *All3DControls = new QVBoxLayout;
	{
		All3DControls->addStretch(1);
		All3DControls->addLayout(ThreeDParams);
		All3DControls->addLayout(ThreeDView);
		All3DControls->addLayout(ThreeDViewControls);
		All3DControls->addStretch(1);
		All3DControls->addLayout(WingModCommands);
		All3DControls->addStretch(1);
		All3DControls->addSpacing(20);
		All3DControls->addLayout(CommandButtons);
	}

	RightLayout->addWidget(WingDescription);
	RightLayout->addWidget(m_pctrlWingDescription);
	RightLayout->addStretch();
	RightLayout->addLayout(DataLayout);
	RightLayout->addSpacing(20);
	RightLayout->addLayout(All3DControls);

	QHBoxLayout *MainLayout = new QHBoxLayout;
	{
		MainLayout->addLayout(LeftLayout);
		MainLayout->addLayout(RightLayout);
		MainLayout->setStretchFactor(LeftLayout, 4);
		MainLayout->setStretchFactor(RightLayout, 1);
	}

	setLayout(MainLayout);
}


void GL3dWingDlg::showEvent(QShowEvent *event)
{
	move(s_WindowPos);
	resize(s_WindowSize);
	if(s_bWindowMaximized) setWindowState(Qt::WindowMaximized);

//	InitDialog();
//	resizeEvent(NULL);
	m_bChanged = false;
	m_bResetglWing = true;
	SetWingScale();
//	m_3DWingRect = m_pGLWidget->geometry();

	double w = (double)m_pctrlWingTable->width()*.97;
	int w6  = (int)(w/6.);
	int w8  = (int)(w/8.);
	int w12 = (int)(w/12.);

	m_pctrlWingTable->setColumnWidth(0, w12);
	m_pctrlWingTable->setColumnWidth(1, w12);
	m_pctrlWingTable->setColumnWidth(2, w12);
	m_pctrlWingTable->setColumnWidth(3, w12);
	m_pctrlWingTable->setColumnWidth(4, w12);
	m_pctrlWingTable->setColumnWidth(5, w6);
	m_pctrlWingTable->setColumnWidth(6, w12);
	m_pctrlWingTable->setColumnWidth(7, w8);
	m_pctrlWingTable->setColumnWidth(8, w12);
	m_pctrlWingTable->setColumnWidth(9, w8);

	UpdateView();
}




void GL3dWingDlg::UpdateView()
{
	if(isVisible()) m_pGLWidget->updateGL();
}


int GL3dWingDlg::VLMGetPanelTotal()
{
//	QMiarex    *pMiarex = (QMiarex*)s_pMiarex;
	double MinPanelSize;
	if(QMiarex::s_MinPanelSize>0.0) MinPanelSize = QMiarex::s_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_PlanformSpan/1000.0;

	int total = 0;
	for (int i=0; i<m_pWing->NWingSection()-1; i++)
	{
			//do not create a surface if its length is less than the critical size
//			if(fabs(m_pWing->TPos[j]-m_pWing->TPos(j+1))/m_pWing->m_Span >0.001){
			if (fabs(m_pWing->YPosition(i)-m_pWing->YPosition(i+1)) > MinPanelSize)
				total +=m_pWing->NXPanels(i)*m_pWing->NYPanels(i);
	}
//	if(!m_bMiddle) total *=2;
	if(!m_pWing->m_bIsFin) return total*2;
	else                   return total;
}




bool GL3dWingDlg::VLMSetAutoMesh(int total)
{
	m_bChanged = true;
	//split (NYTotal) panels on each side proportionnaly to length, and space evenly
	//Set VLMMATSIZE/NYTotal panels along chord
	int NYTotal, size;

	if(!total)
	{
		size = (int)(2000/4);//why not ? Too much refinement isn't worthwile
		NYTotal = 22;
	}
	else
	{
		size = total;
		NYTotal = (int)sqrt((float)size);
	}

	NYTotal *= 2;

//	double d1, d2; //spanwise panel densities at i and i+1

	for (int i=0; i<m_pWing->NWingSection()-1;i++)
	{
//		d1 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->TPos[i],  3) + 0.5;
//		d2 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->TPos(i+1),3) + 0.5;
//		m_pWing->NYPanels(i) = (int) (NYTotal * (0.8*d1+0.2*d2)* (m_pWing->TPos(i+1)-m_pWing->TPos(i))/m_pWing->m_Span);

		m_pWing->NYPanels(i) = (int)(fabs(m_pWing->YPosition(i+1) - m_pWing->YPosition(i))* (double)NYTotal/m_pWing->m_PlanformSpan);

		m_pWing->NXPanels(i) = (int) (size/NYTotal);
		m_pWing->NXPanels(i) = qMin(m_pWing->NXPanels(i), MAXCHORDPANELS);

		if(m_pWing->NYPanels(i)==0) m_pWing->NYPanels(i) = 1;
		if(m_pWing->NXPanels(i)==0) m_pWing->NXPanels(i) = 1;
	}

	if(VLMGetPanelTotal()>VLMMAXMATSIZE/2)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Too many panels\nReduce the mesh size"));
		return false;
	}
	return true;
}



void  GL3dWingDlg::WheelEvent(QWheelEvent *event)
{
	MainFrame*pMainFrame = (MainFrame*)s_pMainFrame;
	double ZoomFactor;
	QPoint glPoint(event->pos().x() + m_pGLWidget->geometry().x(), event->pos().y()+m_pGLWidget->geometry().y());

	if(m_pGLWidget->geometry().contains(glPoint)) m_pGLWidget->setFocus();	//The mouse button has been wheeled

	if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1./1.06;
	else                            ZoomFactor = 1.06;

	if(m_pGLWidget->geometry().contains(glPoint))
	{
		if(event->delta()>0) m_glScaled *= ZoomFactor;
		else                 m_glScaled /= ZoomFactor;
	}

	UpdateView();
}




