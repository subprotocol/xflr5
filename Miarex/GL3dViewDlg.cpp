/****************************************************************************

	GL3dViewDlg Class
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
#include "GLWidget.h" 
#include "GLLightDlg.h"
#include "CpScaleDlg.h"
#include <QDesktopWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QtDebug>
#include <math.h>
 

#define PANELCP				1227

#define VLMWINGLIFT			1230
#define LIFTFORCE			1231
#define VLMMOMENTS			1232

#define VLMWING2LIFT		1233
#define VLMSTABLIFT			1234
#define VLMFINLIFT			1235

#define VLMWINGDRAG			1236
#define VLMWING2DRAG		1237
#define VLMSTABDRAG			1238
#define VLMFINDRAG			1239

#define VLMWINGWASH			1241
#define VLMWING2WASH		1242
#define VLMSTABWASH			1243
#define VLMFINWASH			1244

#define VLMCTRLPTS			1250
#define VLMVORTICES			1251

#define VLMSTREAMLINES		1255
#define SURFACESPEEDS		1256

#define WINGSURFACES		1258
#define WINGOUTLINE			1259

#define WING2SURFACES		1260
#define WING2OUTLINE		1261

#define STABSURFACES		1265
#define STABOUTLINE			1266

#define FINSURFACES 		1270
#define FINOUTLINE			1271

#define MESHPANELS			1272
#define MESHBACK			1273

#define WINGLEGEND			1276
#define WOPPLEGEND			1277
#define WOPPCPLEGENDCLR		1278
#define WOPPCPLEGENDTXT		1279

#define WINGWAKEPANELS		1283

#define VLMWINGTOPTRANS     1293
#define VLMWINGBOTTRANS     1294
#define VLMWING2TOPTRANS    1295
#define VLMWING2BOTTRANS    1296
#define VLMSTABTOPTRANS     1297
#define VLMSTABBOTTRANS     1298
#define VLMFINTOPTRANS      1299
#define VLMFINBOTTRANS      1300

//3D
#define BODYGEOM            1302
#define BODYSURFACES        1303
//2D
#define BODYAXIALLINES      1304
#define BODYFRAME		    1305
#define BODYFRAME3D         1306
#define BODYFRAMEGRID		1307
#define BODYLINEGRID		1308
#define BODYPOINTS			1309
#define FRAMEPOINTS			1310
#define BODYOVERLAY			1311

#define BODYMESHPANELS		1312
#define BODYMESHBACK		1313

#define ARCBALL             1314
#define ARCPOINT            1315

void* GL3dViewDlg::s_pMainFrame;		//pointer to the Frame window
void* GL3dViewDlg::s_pMiarex;	//pointer to the Miarex Application window
void *GL3dViewDlg::s_pGLLightDlg;


GL3dViewDlg::GL3dViewDlg(void *pParent)
{
	setWindowTitle("3D View");
	setWindowFlags(Qt::WindowStaysOnTopHint);
//	setAttribute(Qt::WA_QuitOnClose );

	m_hcArrow.setShape(Qt::ArrowCursor);
	m_hcCross.setShape(Qt::CrossCursor);
	m_hcMove.setShape(Qt::ClosedHandCursor);

	pi = 3.141592654;

	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_iView = 3;

	m_GLList = 0;

	m_ClipPlanePos = 5.0;

	m_LegendMax  =  1.0;
	m_LegendMin  = -1.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;
	m_GLScale       = 1.0;

	m_LiftScale = m_DragScale = m_VelocityScale = 0.7;

	m_bAutoScales         = false;

	m_bBodyOverlay        = false;
	m_bResetglBody        = false;//otherwise endless repaint if no body present
	m_bResetglBody2D      = false;//
	m_bResetglBodyPoints  = false;
	m_bResetglBody2D      = false;
	m_bResetglBodyOverlay = true;
	m_bResetglBodyMesh    = true;

	m_bResetglGeom       = true;
	m_bResetglOpp        = true;
	m_bResetglLift       = true;
	m_bResetglDrag       = true;
	m_bResetglDownwash   = true;
	m_bResetglStream     = true;
	m_bResetglMesh       = true;
	m_bResetglWake       = true;
	m_bResetglLegend     = true;
	m_bResetglFlow       = true;

	m_bTrans             = false;
	m_bDragPoint         = false;
	m_bArcball           = false;
	m_bStream            = false;
	m_bVortices          = false;
	m_bSpeeds            = false;
	m_bWakePanels        = false;
	m_bCrossPoint        = false;
	m_bSurfaces          = true;
	m_bOutline           = true;
	m_bVLMPanels         = false;
	m_bAxes              = true;
	m_bglLight           = true;
	m_b3DCp              = false;
	m_bDownwash          = true;
	m_bMoments           = true;
	m_bPickCenter        = false;
	m_bAutoCpScale	     = false;
	m_bShowCpScale       = true;
	m_bShowLight         = false;
	m_bIs3DScaleSet      = false;
	m_bXTop              = false;
	m_bXBot              = false;
	m_bXCP               = false;

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

	SetupLayout();


	double pos;
	pos = -m_LiftScale*m_LiftScale + m_LiftScale*sqrt(m_LiftScale*m_LiftScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlLiftScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -m_DragScale*m_DragScale + m_DragScale*sqrt(m_DragScale*m_DragScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlDragScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -m_VelocityScale*m_VelocityScale + m_VelocityScale*sqrt(m_VelocityScale*m_VelocityScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlVelocityScaleSlider->setSliderPosition((int)(pos*100.0));

	connect(m_pctrlIso, SIGNAL(clicked()),this, SLOT(On3DIso()));
	connect(m_pctrlX, SIGNAL(clicked()),this, SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()),this, SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()),this, SLOT(On3DTop()));
	connect(m_pctrlReset, SIGNAL(clicked()),this, SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()),this, SLOT(On3DPickCenter()));
	connect(m_pctrlGLLight, SIGNAL(clicked()),this, SLOT(OnSetupLight()));
	connect(m_pctrlCpScales, SIGNAL(clicked()),this, SLOT(OnCpScale()));

	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));

	connect(m_pctrlAxes, SIGNAL(stateChanged(int)), this, SLOT(OnAxes(int)));
	connect(m_pctrlPanels, SIGNAL(stateChanged(int)), this, SLOT(OnPanels(int)));
	connect(m_pctrlVortices, SIGNAL(stateChanged(int)), this, SLOT(OnVortices(int)));
	connect(m_pctrlLight, SIGNAL(stateChanged(int)), this, SLOT(OnLight(int)));
	connect(m_pctrlSurfaces, SIGNAL(stateChanged(int)), this, SLOT(OnSurfaces(int)));
	connect(m_pctrlOutline, SIGNAL(stateChanged(int)), this, SLOT(OnOutline(int)));

	connect(m_pctrlCp, SIGNAL(clicked(bool)),this, SLOT(On3DCp(bool)));
	connect(m_pctrlLift, SIGNAL(stateChanged(int)), this, SLOT(OnLift(int)));
	connect(m_pctrlIDrag, SIGNAL(stateChanged(int)), this, SLOT(OnIDrag(int)));
	connect(m_pctrlVDrag, SIGNAL(stateChanged(int)), this, SLOT(OnVDrag(int)));
	connect(m_pctrlMoment, SIGNAL(stateChanged(int)), this, SLOT(OnMoment(int)));
	connect(m_pctrlDownwash, SIGNAL(stateChanged(int)), this, SLOT(OnDownwash(int)));

	connect(m_pctrlLiftScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnLiftScale(int)));
	connect(m_pctrlDragScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnDragScale(int)));
	connect(m_pctrlVelocityScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnVelocityScale(int)));
}

GL3dViewDlg::~GL3dViewDlg()
{
}

void GL3dViewDlg::ClientToGL(QPoint const &point, CVector &real)
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


void GL3dViewDlg::GLToClient(CVector const &real, QPoint &point)
{
	if(!m_pglWidget) return;
	double h2 = m_pglWidget->m_GLViewRect.height() /2.0;
	double w2 = m_pglWidget->m_GLViewRect.width()  /2.0;

	double dx, dy;
	dx =  real.x + w2;
	dy = -real.y + h2;

	point.setX((int)(dx * m_pglWidget->m_rCltRect.width()));
	point.setY((int)(dy * m_pglWidget->m_rCltRect.height()));
}



void GL3dViewDlg::GLCallViewLists()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (m_pWPolar && (m_pWPolar)->m_AnalysisType ==3 && (m_pWPolar)->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);


	if(m_bMoments && m_pWOpp)			glCallList(VLMMOMENTS);

	if (m_pWOpp
		&& m_bStream
		&& (m_pWOpp)->m_AnalysisType>=2
		&& !m_bResetglStream)
		glCallList(VLMSTREAMLINES);//streamlines are not rotated

	if(m_pWOpp && m_bSpeeds && (m_pWOpp)->m_AnalysisType>=2 && !m_bResetglFlow)
		glCallList(SURFACESPEEDS);


	if (m_pWOpp)                                      glRotated((m_pWOpp)->m_Alpha, 0.0, 1.0, 0.0);

/*	glColor3d(1.0, 0.0, 0.5);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glVertex3d(RA.x, RA.y, RA.z);
		glVertex3d(RB.x, RB.y, RB.z);
	}*/


	if (m_pWPolar && (m_pWPolar)->m_AnalysisType ==3 && !(m_pWPolar)->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);

	if(m_bVLMPanels && m_pWing)
	{
		if(!(m_b3DCp&&m_pWOpp) && !m_bSurfaces) glCallList(MESHBACK);
		glCallList(MESHPANELS);
	}
	if(m_b3DCp && m_pWOpp && m_pWOpp->m_AnalysisType>=2)
	{
		glCallList(PANELCP);
	}

	if (m_pWPolar && abs(m_pWPolar->m_Beta)>0.001) glRotated(-m_pWPolar->m_Beta, 0.0, 0.0, 1.0);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bOutline)
	{
		if(m_pWing)   glCallList(WINGOUTLINE);
		if(m_pWing2)  glCallList(WING2OUTLINE);
		if(m_pStab)   glCallList(STABOUTLINE);
		if(m_pFin)    glCallList(FINOUTLINE);
		if(m_pPlane)  glTranslated((m_pPlane)->m_BodyPos.x, 0.0, (m_pPlane)->m_BodyPos.z);
		if(m_pBody)	  glCallList(BODYGEOM);
		if(m_pPlane)  glTranslated(-(m_pPlane)->m_BodyPos.x, 0.0, -(m_pPlane)->m_BodyPos.z);
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
		if(m_pWing)  glCallList(WINGSURFACES);
		if(m_pWing2) glCallList(WING2SURFACES);
		if(m_pStab)  glCallList(STABSURFACES);
		if(m_pFin)   glCallList(FINSURFACES);
		if(m_pBody)
		{
			if(m_pPlane)	glTranslated((m_pPlane)->m_BodyPos.x, 0.0, (m_pPlane)->m_BodyPos.z);
			glCallList(BODYSURFACES);
			if(m_pPlane)	glTranslated(-(m_pPlane)->m_BodyPos.x, 0.0, -(m_pPlane)->m_BodyPos.z);
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bVortices && m_pWing)
	{
		glCallList(VLMCTRLPTS);
		glCallList(VLMVORTICES);
	}

	if(m_bXCP && m_pWOpp)
	{
		if(m_pWing) glCallList(VLMWINGLIFT);
		if(m_pPOpp)
		{
			if(m_pWing2) glCallList(VLMWING2LIFT);
			if(m_pStab) 	glCallList(VLMSTABLIFT);
			if(m_pFin) 	glCallList(VLMFINLIFT);
		}
		glCallList(LIFTFORCE);
	}

	if((m_bICd || m_bVCd) && m_pWOpp )
	{
		if(m_pWing) glCallList(VLMWINGDRAG);
		if(m_pPOpp)
		{
			if(m_pWing2) glCallList(VLMWING2DRAG);
			if(m_pStab)  glCallList(VLMSTABDRAG);
			if(m_pFin)   glCallList(VLMFINDRAG);
		}
	}

	if(m_bDownwash && m_pWOpp)
	{
		if(m_pWing) glCallList(VLMWINGWASH);
		if(m_pPOpp)
		{
			if(m_pWing2) glCallList(VLMWING2WASH);
			if(m_pStab) 	glCallList(VLMSTABWASH);
			if(m_pFin) 	glCallList(VLMFINWASH);
		}
	}

	if(m_bXTop && m_pWOpp)
	{
		if(m_pWing) glCallList(VLMWINGTOPTRANS);
		if(m_pPOpp){
			if(m_pWing2) glCallList(VLMWING2TOPTRANS);
			if(m_pStab) 	glCallList(VLMSTABTOPTRANS);
			if(m_pFin) 	glCallList(VLMFINTOPTRANS);
		}
	}
	if(m_bXBot && m_pWOpp)
	{
		if(m_pWing) glCallList(VLMWINGBOTTRANS);
		if(m_pPOpp)
		{
			if(m_pWing2) 	glCallList(VLMWING2BOTTRANS);
			if(m_pStab) 		glCallList(VLMSTABBOTTRANS);
			if(m_pFin) 		glCallList(VLMFINBOTTRANS);
		}
	}
}


void GL3dViewDlg::GLCreateCp()
{
	if(!m_pWOpp)
	{
		glNewList(PANELCP,GL_COMPILE);
		glEndList();
		return;
	}
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int p, pp, n, averageInf, averageSup, average100;
	int nPanels;

	CVector LA,LB,TA,TB, Pt;
	CVector C,N;
	double color;
	double lmin, lmax, range;
	double *tab;
	double CpInf[2*VLMMATSIZE], CpSup[2*VLMMATSIZE], Cp100[2*VLMMATSIZE];


	glNewList(PANELCP,GL_COMPILE);
	{
		m_GLList++;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

		if(m_pPOpp)
		{
			tab = m_pPOpp->m_Cp;
			nPanels = m_pPOpp->m_NPanels;
		}
		else
		{
			tab = m_pWOpp->m_Cp;
			nPanels = m_pWOpp->m_NVLMPanels;
		}

		lmin = 10000.0;
		lmax = -10000.0;
		// find min and max Cp for scale set
		for (n=0; n<pMiarex->m_nNodes; n++)
		{
			averageInf = 0; averageSup = 0; average100 = 0;
			CpInf[n] = 0.0; CpSup[n] = 0.0; Cp100[n] = 0.0;
			for (pp=0; pp< nPanels; pp++)
			{
				if (m_pNode[m_pPanel[pp].m_iLA].IsSame(m_pNode[n]) || m_pNode[m_pPanel[pp].m_iTA].IsSame(m_pNode[n]) ||
					m_pNode[m_pPanel[pp].m_iTB].IsSame(m_pNode[n]) || m_pNode[m_pPanel[pp].m_iLB].IsSame(m_pNode[n]))
				{
					if(m_pPanel[pp].m_iPos==1)
					{
						CpSup[n] +=tab[pp];
						averageSup++;
					}
					else if(m_pPanel[pp].m_iPos<=0)
					{
						CpInf[n] +=tab[pp];
						averageInf++;
					}
					else if(m_pPanel[pp].m_iPos==100)
					{
						Cp100[n] +=tab[pp];
						average100++;
					}
				}
			}
			if(averageSup>0)
			{
				CpSup[n] /= averageSup;
				if(CpSup[n]<lmin) lmin = CpSup[n];
				if(lmax<CpSup[n]) lmax = CpSup[n];
			}
			if(averageInf>0)
			{
				CpInf[n] /= averageInf;
				if(CpInf[n]<lmin) lmin = CpInf[n];
				if(lmax<CpInf[n]) lmax = CpInf[n];
			}
			if(average100>0)
			{
				Cp100[n] /= average100;
				if(Cp100[n]<lmin) lmin = Cp100[n];
				if(lmax<Cp100[n]) lmax = Cp100[n];
			}
		}

		if(m_bAutoCpScale)
		{
			m_LegendMin = lmin;
			m_LegendMax = lmax;
		}
		else
		{
			lmin = m_LegendMin;
			lmax = m_LegendMax;
		}

		range = lmax - lmin;

		glLineWidth(1.0);
		for (p=0; p<pMiarex->m_MatSize; p++)
		{
			glBegin(GL_QUADS);
			{
				TA.Copy(m_pNode[m_pPanel[p].m_iTA]);
				TB.Copy(m_pNode[m_pPanel[p].m_iTB]);
				LA.Copy(m_pNode[m_pPanel[p].m_iLA]);
				LB.Copy(m_pNode[m_pPanel[p].m_iLB]);

				if(m_pPanel[p].m_iPos==1) color = (CpSup[m_pPanel[p].m_iLA]-lmin)/range;
				else if(m_pPanel[p].m_iPos<=0) color = (CpInf[m_pPanel[p].m_iLA]-lmin)/range;
				else                           color = (Cp100[m_pPanel[p].m_iLA]-lmin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(LA.x, LA.y, LA.z);

				if(m_pPanel[p].m_iPos==1) color = (CpSup[m_pPanel[p].m_iTA]-lmin)/range;
				else if(m_pPanel[p].m_iPos<=0) color = (CpInf[m_pPanel[p].m_iTA]-lmin)/range;
				else                           color = (Cp100[m_pPanel[p].m_iTA]-lmin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(TA.x, TA.y, TA.z);

				if(m_pPanel[p].m_iPos==1) color = (CpSup[m_pPanel[p].m_iTB]-lmin)/range;
				else if(m_pPanel[p].m_iPos<=0) color = (CpInf[m_pPanel[p].m_iTB]-lmin)/range;
				else                           color = (Cp100[m_pPanel[p].m_iTB]-lmin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(TB.x, TB.y, TB.z);

				if(m_pPanel[p].m_iPos==1) color = (CpSup[m_pPanel[p].m_iLB]-lmin)/range;
				else if(m_pPanel[p].m_iPos<=0) color = (CpInf[m_pPanel[p].m_iLB]-lmin)/range;
				else                           color = (Cp100[m_pPanel[p].m_iLB]-lmin)/range;
				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(LB.x, LB.y, LB.z);

			}
			glEnd();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();

}


void GL3dViewDlg::GLCreateCpLegend()
{
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;

	double labellength, ClientToGL;
	int maxlength;
	double f, fi,dD, ZPos,dz,Right1, Right2, RightLetter;
	double color = 0.0;
	double range, delta;

	QFont font("Courier", 10);
	QFontMetrics fm(font);

	double w = (double)m_pglWidget->m_rCltRect.width();
	double h = (double)m_pglWidget->m_rCltRect.height();
	double XPos;

	if(w>h)
	{
		XPos  = 1.0;
		ZPos  = h/w * (-1.0 + 1.0/2.0);
		dz    = h/w*1.0/20.0;
		ClientToGL = 2.0/w;
	}
	else
	{
		XPos = w/h;
		ZPos  = (-1.0 + 1.0/2.0);
		dz    = 1.0/20.0;
		ClientToGL = 2.0/h;
	}

	dD      = 12.0/w*2.0;

	Right1  = .94*XPos;
	Right2  = .98*XPos;

	range = (m_LegendMax - m_LegendMin);
	delta = range / 20;

	glNewList(WOPPCPLEGENDTXT,GL_COMPILE);
	{
		m_GLList++;
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPolygonMode(GL_FRONT,GL_LINE);

		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());

		// Draw the labels
		for (i=0; i<=20; i ++)
		{
			f = m_LegendMin + (double)i * delta;
			fi = (double)i*dz;
			strong = QString("%1").arg(f,5,'f',2);
			labellength = fm.width(strong) * ClientToGL;
			m_pglWidget->renderText(Right1-labellength, ZPos+fi, 0.0, strong);
		}
		m_pglWidget->renderText(Right1, ZPos+21.0*dz, "Cp");
	}
	glEndList();

	glNewList(WOPPCPLEGENDCLR,GL_COMPILE);
	{	
		m_GLList++;
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPolygonMode(GL_FRONT,GL_FILL);

		glBegin(GL_QUAD_STRIP);
		{
			for (i=0; i<=20; i++)
			{
				fi = (double)i*dz;
				color += 0.05;

				glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(Right1, ZPos+fi, 0.0);
				glVertex3d(Right2, ZPos+fi, 0.0);
			}
		}
		glEnd();
	}
	glEndList();
}




void GL3dViewDlg::GLCreateGeom(CWing *pWing, int List, QColor wingcolor)
{
	if(!pWing) return;

	int j,l;
	int style, width;

	CVector Pt, A, B, C, D, N, BD, AC, LATB, TALB;
	QColor color;
	CFoil * pFoilA, *pFoilB;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
//	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	N.Set(0.0, 0.0, 0.0);
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glLineWidth(1.0);

		color = wingcolor;
		style = 0;
		width = 0;

		glColor3d(color.redF(),color.greenF(),color.blueF());

		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_DEPTH_TEST);

		//top surface
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					pWing->m_Surface[j].GetPanel(0,l,1);

					LATB = pWing->m_Surface[j].TB - pWing->m_Surface[j].LA;
					TALB = pWing->m_Surface[j].LB - pWing->m_Surface[j].TA;

					N = LATB *TALB;
					N. Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(pWing->m_Surface[j].LA.x,
							   pWing->m_Surface[j].LA.y,
							   pWing->m_Surface[j].LA.z);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, 1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
				}
				glEnd();
			}
		}

		//bottom surface
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					pWing->m_Surface[j].GetPanel(0,l, -1);

					LATB = pWing->m_Surface[j].TB - pWing->m_Surface[j].LA;
					TALB = pWing->m_Surface[j].LB - pWing->m_Surface[j].TA;

					N = TALB * LATB;
					N. Normalize();

					glNormal3d( N.x,  N.y,  N.z);
					glVertex3d( pWing->m_Surface[j].TA.x,
								pWing->m_Surface[j].TA.y,
								pWing->m_Surface[j].TA.z);
					glVertex3d( pWing->m_Surface[j].LA.x,
								pWing->m_Surface[j].LA.y,
								pWing->m_Surface[j].LA.z);

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, -1);
					glVertex3d( pWing->m_Surface[j].LB.x,
								pWing->m_Surface[j].LB.y,
								pWing->m_Surface[j].LB.z);
					glVertex3d( pWing->m_Surface[j].TB.x,
								pWing->m_Surface[j].TB.y,
								pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
		}

		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			//All surfaces
			//Left surface
			if(pWing->m_Surface[j].m_bIsTipLeft)
			{
				glBegin(GL_QUAD_STRIP);
				{
					pWing->m_Surface[j].GetPanel(0, 0, -1);
					C. Copy(pWing->m_Surface[0].LA);
					D. Copy(pWing->m_Surface[0].TA);
					pWing->m_Surface[j].GetPanel(0, 0, 1);
					A. Copy(pWing->m_Surface[0].TA);
					B. Copy(pWing->m_Surface[0].LA);

					BD = D-B;
					AC = C-A;
					N  = AC*BD;
					N.Normalize();
					glNormal3d( N.x, N.y, N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);

					for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
					{
						pWing->m_Surface[j].GetPanel(0,l,-1);
						glVertex3d( pWing->m_Surface[0].LA.x,
									pWing->m_Surface[0].LA.y,
									pWing->m_Surface[0].LA.z);
						pWing->m_Surface[j].GetPanel(0,l,1);
						glVertex3d( pWing->m_Surface[0].LA.x,
									pWing->m_Surface[0].LA.y,
									pWing->m_Surface[0].LA.z);
					}
				}
				glEnd();
			}
			if(pWing->m_Surface[j].m_bIsTipRight)
			{
				//right surface
				glBegin(GL_QUAD_STRIP);
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, 1);
					A. Copy(pWing->m_Surface[0].TB);
					B. Copy(pWing->m_Surface[0].LB);
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, -1);
					C. Copy(pWing->m_Surface[0].LB);
					D. Copy(pWing->m_Surface[0].TB);

					BD = D-B;
					AC = C-A;
					N  = BD * AC;
					N.Normalize();
					glNormal3d( N.x,  N.y,  N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);

					for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
					{
						pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, 1);
						glVertex3d(pWing->m_Surface[j].LB.x,
								   pWing->m_Surface[j].LB.y,
								   pWing->m_Surface[j].LB.z);
						pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, -1);
						glVertex3d(pWing->m_Surface[j].LB.x,
								   pWing->m_Surface[j].LB.y,
								   pWing->m_Surface[j].LB.z);
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
	glNewList(List+1,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)pMiarex->m_OutlineWidth);

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
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(0, l, 1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}

				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, 1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}

		//BOTTOM outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(0, l, -1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, -1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//WingContour
		//Leading edge outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				pWing->m_Surface[j].GetPanel(0,pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//Trailing edge outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				pWing->m_Surface[j].GetPanel(0,0, 0);
				glVertex3d(pWing->m_Surface[j].TA.x,
						   pWing->m_Surface[j].TA.y,
						   pWing->m_Surface[j].TA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1, 0, 0);
				glVertex3d(pWing->m_Surface[j].TB.x,
						   pWing->m_Surface[j].TB.y,
						   pWing->m_Surface[j].TB.z);
			}
			glEnd();
		}
		//flap outline....
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			pFoilA = pWing->m_Surface[j].m_pFoilA;
			pFoilB = pWing->m_Surface[j].m_pFoilB;
			if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap)
			{
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 0.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 1.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0,
													 0.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0,
													 1.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			pFoilA = pWing->m_Surface[j].m_pFoilA;
			pFoilB = pWing->m_Surface[j].m_pFoilB;
			if(pFoilA && pFoilB && pFoilA->m_bLEFlap && pFoilB->m_bLEFlap)
			{
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 0.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 1.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pFoilA->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0,
													 0.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pFoilB->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0,
													 1.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

}



void GL3dViewDlg::GLCreateDownwash(CWing *pWing, CWOpp *pWOpp, int List, int surf0)
{
	// pWing is either the Wing, the stab, or the fin
	// pWOpp is related to the pWing

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	QColor color;
	int style, width;
	int i,j,k,p;
	double dih, xt, yt, zt, yob;
	double y1, y2, z1, z2, xs, ys, zs;
	CVector C;
	double factor, amp;

	double sina = -sin(pWOpp->m_Alpha*pi/180.0);
	double cosa =  cos(pWOpp->m_Alpha*pi/180.0);
	double sign;

	//DOWNWASH
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pMiarex->m_DownwashColor;
		style = pMiarex->m_DownwashStyle;
		width = pMiarex->m_DownwashWidth;

		glColor3d(color.redF(), color.greenF(), color.blueF());

		glLineWidth((GLfloat)width);

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		if(pWOpp)
		{
			if(pWOpp->m_AnalysisType==1)
			{

				for (i=1; i<pWOpp->m_NStation; i++)
				{
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWing->GetChord(yob);
					pWing->GetViewYZPos(1., pWOpp->m_SpanPos[i], yt,zt,0);

					dih = -pWing->GetDihedral(yob)*pi/180.0;
					amp = pWOpp->m_QInf*sin(pWOpp->m_Ai[i]*pi/180.0);
					amp *= m_VelocityScale/2.0;
					glBegin(GL_LINES);
					{
						glVertex3d(xt, yt, zt);
						glVertex3d(xt + amp * cos(dih)* sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)* cosa);
					}
					glEnd();
				}
				glBegin(GL_LINE_STRIP);
				{
					for (i=1; i<pWOpp->m_NStation; i++)
					{
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWing->GetChord(yob);
						pWing->GetViewYZPos(1., pWOpp->m_SpanPos[i], yt,zt,0);

						dih = -pWing->GetDihedral(yob)*pi/180.0;
						amp = pWOpp->m_QInf*sin(pWOpp->m_Ai[i]*pi/180.0);
						amp *= m_VelocityScale/2.0;

						glVertex3d(xt + amp * cos(dih)* sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)* cosa);
					}
				}
				glEnd();
			}
			else
			{
				factor = m_VelocityScale/2.0;

				p = 0;
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
					{
//						m_pSurface[j+surf0]->GetTrailingPt(k, C);
						pWing->m_Surface[j].GetTrailingPt(k, C);
						if (pWOpp->m_Vd[i].z>0) sign = 1.0; else sign = -1.0;
						glBegin(GL_LINES);
						{
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x+factor*pWOpp->m_Vd[i].z * sina,
									   C.y+factor*pWOpp->m_Vd[i].y,
									   C.z+factor*pWOpp->m_Vd[i].z * cosa);
						}
						glEnd();
						xs = C.x+factor*pWOpp->m_Vd[i].z*sina;
						ys = C.y+factor*pWOpp->m_Vd[i].y;
						zs = C.z+factor*pWOpp->m_Vd[i].z*cosa;
						y1 = ys - 0.085*factor*pWOpp->m_Vd[i].y      + 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z1 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa - 0.05*factor*pWOpp->m_Vd[i].y;
						y2 = ys - 0.085*factor*pWOpp->m_Vd[i].y      - 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z2 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa + 0.05*factor*pWOpp->m_Vd[i].y;

						glBegin(GL_LINES);
						{
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y1, z1);
						}
						glEnd();
						glBegin(GL_LINES);
						{
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y2, z2);
						}
						glEnd();

						i++;
					}
					p++;
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}



void GL3dViewDlg::GLCreateDrag(CWing *pWing, CWOpp *pWOpp, int List)
{
	if(!pWing || !pWOpp) return;
	CVector C;
	int i,j,k;
	int Istyle, Iwidth, Vstyle, Vwidth;
	QColor Icolor, Vcolor;

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	GLushort IDash, VDash;

	double Ir,Ig,Ib, Vr, Vg, Vb;
	double amp, amp1, amp2;
	double yob, xt, yt, zt, dih;
	double cosa =  cos(pWOpp->m_Alpha * pi/180.0);
	double sina = -sin(pWOpp->m_Alpha * pi/180.0);

	Icolor = pMiarex->m_IDragColor;
	Istyle = pMiarex->m_IDragStyle;
	Iwidth = pMiarex->m_IDragWidth;

	Vcolor = pMiarex->m_VDragColor;
	Vstyle = pMiarex->m_VDragStyle;
	Vwidth = pMiarex->m_VDragWidth;

	if(Istyle == 1)			IDash = 0x1111;
	else if(Istyle == 2)	IDash = 0x0F0F;
	else if(Istyle == 3) 	IDash = 0x1C47;
	else					IDash = 0xFFFF;// Solid

	if(Vstyle == 1)			VDash = 0x1111;
	else if(Vstyle == 2)	VDash = 0x0F0F;
	else if(Vstyle == 3)	VDash = 0x1C47;
	else					VDash = 0xFFFF;// Solid

	Ir = Icolor.redF();
	Ig = Icolor.greenF();
	Ib = Icolor.blueF();
	Vr = Vcolor.redF();
	Vg = Vcolor.greenF();
	Vb = Vcolor.blueF();

	glLineStipple (1, IDash);// Solid
	glLineWidth((GLfloat)(Iwidth));

	//DRAGLINE
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		double q0 = 0.5*(m_pWPolar)->m_Density * (m_pWing)->m_Area*pWOpp->m_QInf*pWOpp->m_QInf;

		if(pWOpp)
		{
			if(pWOpp->m_AnalysisType==1)
			{
				for (i=1; i<pWOpp->m_NStation; i++)
				{
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
					pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
					dih = pWing->GetDihedral(yob)*pi/180.0;
					amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*m_DragScale/1.0;
					amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*m_DragScale/1.0;
					if(m_bICd)
					{
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)(Iwidth));
						glBegin(GL_LINES);
						{
							glVertex3d(xt, yt, zt);
							glVertex3d(	xt + amp1 * cos(dih)*cosa,
										yt,
										zt - amp1 * cos(dih)*sina);
						}
						glEnd();
					}
					if(m_bVCd)
					{
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)(Vwidth));
						glBegin(GL_LINES);
						{
							if(!m_bICd)
							{
								glVertex3d(xt, yt,zt);
								glVertex3d(xt + amp2 * cos(dih)*cosa,
										   yt,
										   zt - amp2 * cos(dih)*sina);
							}
							else {
								glVertex3d(xt + amp1 * cos(dih)*cosa,
										   yt,
										   zt - amp1 * cos(dih)*sina);
								glVertex3d(xt + (amp1+amp2) * cos(dih)*cosa,
										   yt,
										   zt - (amp1+amp2) * cos(dih)*sina);
							}
						}
						glEnd();
					}
				}
				if(m_bICd)
				{
					glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
					glLineStipple (1, IDash);// Solid
					glLineWidth((GLfloat)(Iwidth));
					glBegin(GL_LINE_STRIP);
					{
						for (i=1; i<pWOpp->m_NStation; i++)
						{
							yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
							xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
							pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);

							dih = pWing->GetDihedral(yob)*pi/180.0;

							amp  = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= m_DragScale/1.0;

							glVertex3d(xt + amp * cos(dih)*cosa,
									   yt,
									   zt - amp * cos(dih)*sina);
						}
					}
					glEnd();
				}
				if(m_bVCd)
				{
					glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
					glLineStipple (1, VDash);// Solid
					glLineWidth((GLfloat)(Vwidth));
					glBegin(GL_LINE_STRIP);
					{
						for (i=1; i<pWOpp->m_NStation; i++)
						{
							yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
							xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
							pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);

							dih = pWing->GetDihedral(yob)*pi/180.0;
							amp=0.0;
							if(m_bICd) amp+=pWOpp->m_ICd[i];
							amp +=pWOpp->m_PCd[i];
							amp *= q0*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= m_DragScale/1.0;

							glVertex3d( xt + amp * cos(dih)*cosa,
										yt ,
										zt - amp * cos(dih)*sina);
						}
					}
					glEnd();
				}
			}
			else
			{
				//VLM type drag
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					//All surfaces
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
					{
						pWing->m_Surface[j].GetTrailingPt(k, C);
						amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord*m_DragScale/30.0;
						amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord*m_DragScale/30.0;
						if(m_bICd)
						{
							glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
							glLineStipple (1, IDash);// Solid
							glLineWidth((GLfloat)(Iwidth));
							glBegin(GL_LINES);
							{
								glVertex3d(C.x, C.y, C.z);
								glVertex3d(C.x + amp1*cosa,
										   C.y ,
										   C.z - amp1*sina);
							}
							glEnd();
						}
						if(m_bVCd)
						{
							glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
							glLineStipple (1, VDash);// Solid
							glLineWidth((GLfloat)(Vwidth));
							glBegin(GL_LINES);
							{
								if(!m_bICd)
								{
									glVertex3d(C.x, C.y, C.z);
									glVertex3d(C.x + amp2*cosa,
											   C.y ,
											   C.z - amp2*sina);
								}
								else
								{
									glVertex3d(C.x + amp1*cosa,
											   C.y ,
											   C.z - amp1*sina);
									glVertex3d(C.x + (amp1+amp2)*cosa,
											   C.y ,
											   C.z - (amp1+amp2)*sina);
								}
							}
							glEnd();
						}

						i++;
					}
				}
				if(!pWing->m_bIsFin)
				{
					if(m_bICd)
					{
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)(Iwidth));
						glBegin(GL_LINE_STRIP);
						{
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++)
							{
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
								{
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord;
									amp *= m_DragScale/30.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
						}
						glEnd();
					}
					if(m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)(Vwidth));
						glBegin(GL_LINE_STRIP);
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp=0.0;
									if(m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord;
									amp *= m_DragScale/30.0;

									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
						glEnd();
					}
				}
				else {
					if(m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)Iwidth);
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord;
									amp *= m_DragScale/30.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							glEnd();
						}
					}
					if(m_bVCd)
					{
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)Vwidth);
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
							{
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp=0.0;
									if(m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/(m_pWing)->m_MAChord;
									amp *= m_DragScale/30.0;

									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
							glEnd();
						}
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void GL3dViewDlg::GLCreateMesh()
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

void GL3dViewDlg::GLCreateCtrlPts()
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



void GL3dViewDlg::GLCreateLiftForce()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	int style, width;
	QColor color;
	double forcez,forcex,glx, gly;

	double sign;


	glNewList(LIFTFORCE, GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pMiarex->m_XCPColor;
		style = pMiarex->m_XCPStyle;
		width = pMiarex->m_XCPWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(),color.greenF(),color.blueF());

		glLineWidth((GLfloat)pMiarex->m_XCPWidth);

		//Resulting force vector
		glLineWidth((GLfloat)(width * 2.0));

		double force = 0.5*m_pWPolar->m_Density * m_pWing->m_Area
						  *m_pWOpp->m_QInf*m_pWOpp->m_QInf
						  *m_pWOpp->m_CL;

		force *= m_LiftScale/500.0;

		forcez =  force * cos(m_pWOpp->m_Alpha * pi/180.0);
		forcex = -force * sin(m_pWOpp->m_Alpha * pi/180.0);

		if (force>0.0) sign = 1.0; else sign = -1.0;
		glLineStipple (1, 0xFFFF);// Solid
		glLineWidth(3.0);

		glx = (GLfloat)m_pWOpp->m_XCP;
		gly = (GLfloat)m_pWOpp->m_YCP;

		glBegin(GL_LINES);
			glVertex3d(glx,gly,0.0);
			glVertex3d(glx+forcex,gly,forcez);

		glEnd();

		glBegin(GL_LINES);
			glVertex3d(glx+forcex, gly, forcez);
			glVertex3d(glx+forcex+0.008, gly+0.008, forcez-0.012*sign/m_glScaled);
		glEnd();

		glBegin(GL_LINES);
			glVertex3d(glx+forcex, gly, forcez);
			glVertex3d(glx+forcex-0.008, gly-0.008, forcez-0.012*sign/m_glScaled);
		glEnd();

		glDisable (GL_LINE_STIPPLE);

	}
	glEndList();
}

void GL3dViewDlg::GLCreateMoments()
{
//	The most common aeronautical convention defines
//	- the roll as acting about the longitudinal axis, positive with the starboard wing down.
//	- The yaw is about the vertical body axis, positive with the nose to starboard.
//	- Pitch is about an axis perpendicular to the longitudinal plane of symmetry, positive nose up.
//	-- Wikipedia flight dynamics --

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int i;
	int style, width;
	QColor color;

	double sign, amp, radius;
	double angle;//radian
	double endx, endy, endz, dx, dy, dz,xae, yae, zae;
	double factor = 10.0;


	glNewList(VLMMOMENTS, GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pMiarex->m_MomentColor;
		style = pMiarex->m_MomentStyle;
		width = pMiarex->m_MomentWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);


		glColor3d(color.redF(),color.greenF(),color.blueF());

		glLineWidth((GLfloat)(width*2.0));

		amp = 0.5*m_pWPolar->m_Density * m_pWing->m_Area
			  *m_pWOpp->m_QInf*m_pWOpp->m_QInf	*m_pWOpp->m_GCm/3.0;

		amp *= m_LiftScale*factor;

		radius= m_pWing->m_Span/4.0;

		if (amp>0.0) sign = -1.0; else sign = 1.0;

		glBegin(GL_LINE_STRIP);
		{
			for (i=0; i<=int(abs(amp)); i++)
			{
				angle = sign*(double)i/500.0*pi;
				glVertex3d(radius*cos(angle),0.0,radius*sin(angle));
			}
		}
		glEnd();

		endx = radius*cos(angle);
		endz = radius*sin(angle);

		dx = 0.03;
		dz = 0.03*sign;

		xae = (radius-dx)*cos(angle) +dz *sin(angle);
		zae = (radius-dx)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, 0.0, endz);
			glVertex3d(xae,  0.0, zae);
		}
		glEnd();

		xae = (radius+dx)*cos(angle) +dz *sin(angle);
		zae = (radius+dx)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, 0.0, endz);
			glVertex3d(xae,  0.0, zae);
		}
		glEnd();

		//Resulting Rolling Moment Arc vector

		amp = 0.5*m_pWPolar->m_Density * m_pWing->m_Area
						*m_pWOpp->m_QInf*m_pWOpp->m_QInf
						*m_pWOpp->m_GRm/3.0;

		amp *= m_LiftScale*factor;

		if (amp>0.0) sign = -1.0; else sign = 1.0;

		glBegin(GL_LINE_STRIP);
			for (i=0; i<=int(abs(amp)); i++)
			{
				angle = sign*(double)i/500.0*pi;
				glVertex3d(0.0,radius*cos(angle),radius*sin(angle));
			}
		glEnd();

		endy = radius*cos(angle);
		endz = radius*sin(angle);

		dy = 0.03;
		dz = 0.03*sign;

		yae = (radius-dy)*cos(angle) +dz *sin(angle);
		zae = (radius-dy)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(0.0, endy, endz);
			glVertex3d(0.0, yae,  zae);
		glEnd();

		yae = (radius+dy)*cos(angle) +dz *sin(angle);
		zae = (radius+dy)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(0.0, endy, endz);
			glVertex3d(0.0, yae,  zae);
		glEnd();

		//Resulting Yawing Moment Arc vector

		amp = 0.5*m_pWPolar->m_Density * m_pWing->m_Area
						*m_pWOpp->m_QInf*m_pWOpp->m_QInf
						*(m_pWOpp->m_GYm)/3.0;

		amp *= m_LiftScale*factor;

		if (amp>0.0) sign = -1.0; else sign = +1.0;

		glBegin(GL_LINE_STRIP);
		{
			for (i=0; i<=int(abs(amp)); i++)
			{
				angle = sign*(double)i/500.0*pi;
				glVertex3d(-radius*cos(angle),-radius*sin(angle),0.0);
			}
		}
		glEnd();

		endx = -radius*cos(angle);
		endy = -radius*sin(angle);

		dx =   0.03;
		dy =  -0.03*sign;

		xae = (-radius+dx)*cos(angle) +dy *sin(angle);
		yae = (-radius+dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		}
		glEnd();

		xae = (-radius-dx)*cos(angle) +dy *sin(angle);
		yae = (-radius-dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		}
		glEnd();

		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}

void GL3dViewDlg::GLCreateLiftStrip(CWing *pWing, CWOpp *pWOpp, int List)
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int i,j,k;
	int style, width;
	CVector C, CL;

	QColor color;

	double amp, yob, xt, yt, zt, dih;
	double cosa =  cos(pWOpp->m_Alpha * pi/180.0);
	double sina = -sin(pWOpp->m_Alpha * pi/180.0);

	//LIFTLINE
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pMiarex->m_XCPColor;
		style = pMiarex->m_XCPStyle;
		width = pMiarex->m_XCPWidth;

		if     (style == 1) 	glLineStipple (1, 0x1111);
		else if(style == 2) 	glLineStipple (1, 0x0F0F);
		else if(style == 3) 	glLineStipple (1, 0x1C47);
		else					glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());
		glLineWidth((GLfloat)width);

		//dynamic pressure x area
		double q0 = 0.5 * m_pWPolar->m_Density * pWOpp->m_QInf * pWOpp->m_QInf;

		if(pWOpp)
		{
			if(pWOpp->m_AnalysisType==1)
			{
				for (i=1; i<pWOpp->m_NStation; i++)
				{
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
					pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i], yt, zt, 0);
					dih = -pWing->GetDihedral(yob)*pi/180.0;
					amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
					amp *= m_LiftScale/1000.0;

					glBegin(GL_LINES);
					{
						glVertex3d(xt, yt, zt);
						glVertex3d((xt + amp * cos(dih)*sina),
									yt + amp * sin(dih),
									zt + amp * cos(dih)*cosa);
					}
					glEnd();
				}
				glBegin(GL_LINE_STRIP);
				{
					for (i=1; i<pWOpp->m_NStation; i++)
					{
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i],yt,zt,0);

						dih = -pWing->GetDihedral(yob)*pi/180.0;
						amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
						amp *= m_LiftScale/1000.0;

						glVertex3d(xt + amp * cos(dih)*sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)*cosa);
					}
				}
				glEnd();
			}
			else
			{
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
					{
						pWing->m_Surface[j].GetLeadingPt(k, C);
						amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pWing->m_MAChord * m_LiftScale/1000.0;
						C.x = pWOpp->m_XCPSpanAbs[i];

						glBegin(GL_LINES);
						{
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x + pWOpp->m_F[i].x*amp,//F is in Body axes
									   C.y + pWOpp->m_F[i].y*amp,
									   C.z + pWOpp->m_F[i].z*amp);
						}
						glEnd();
						i++;
					}
				}

				//Lift strip on each surface
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					if(j>0 && pWing->m_Surface[j-1].m_bJoinRight)
					{
						//then connect strip to previous surface's last point
						glBegin(GL_LINES);
						{
							glVertex3d(CL.x, CL.y, CL.z);

							k=0;
							pWing->m_Surface[j].GetLeadingPt(k, C);
							amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pWing->m_MAChord * m_LiftScale/1000.0;
							C.x = pWOpp->m_XCPSpanAbs[i];
							glVertex3d(C.x + pWOpp->m_F[i].x*amp,
									   C.y + pWOpp->m_F[i].y*amp,
									   C.z + pWOpp->m_F[i].z*amp);

						}
						glEnd();
					}
					glBegin(GL_LINE_STRIP);
					{
						for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
						{
							pWing->m_Surface[j].GetLeadingPt(k, C);
							amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pWing->m_MAChord * m_LiftScale/1000.0;
							C.x = pWOpp->m_XCPSpanAbs[i];
							CL.x = C.x + pWOpp->m_F[i].x*amp;
							CL.y = C.y + pWOpp->m_F[i].y*amp;
							CL.z = C.z + pWOpp->m_F[i].z*amp;
							glVertex3d(CL.x, CL.y, CL.z);
							i++;
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


void GL3dViewDlg::GLCreateVortices()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	int p;
	CVector A, B, C, D, AC, BD;
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0xFFFF);

	glNewList(VLMVORTICES,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(1.0);
		glColor3d(0.7,0.0,0.0);
		for (p=0; p<pMiarex->m_MatSize; p++)
		{
			if(!m_pPanel[p].m_bIsTrailing)
			{
				if(m_pPanel[p].m_iPos<=0)
				{
					A = m_pPanel[p].A;
					B = m_pPanel[p].B;
					C = m_pPanel[p-1].B;
					D = m_pPanel[p-1].A;
				}
				else
				{
					A = m_pPanel[p].A;
					B = m_pPanel[p].B;
					C = m_pPanel[p+1].B;
					D = m_pPanel[p+1].A;
				}
			}
			else
			{
				A = m_pPanel[p].A;
				B = m_pPanel[p].B;
				// we define point AA=A+1 and BB=B+1
				C.x =  m_pNode[m_pPanel[p].m_iTB].x
					+ (m_pNode[m_pPanel[p].m_iTB].x-m_pPanel[p].B.x)/3.0;
				C.y =  m_pNode[m_pPanel[p].m_iTB].y;
				C.z =  m_pNode[m_pPanel[p].m_iTB].z;
				D.x =  m_pNode[m_pPanel[p].m_iTA].x
					+ (m_pNode[m_pPanel[p].m_iTA].x-m_pPanel[p].A.x)/3.0;
				D.y =  m_pNode[m_pPanel[p].m_iTA].y;
				D.z =  m_pNode[m_pPanel[p].m_iTA].z;
			}

			//next we "shrink" the points to avoid confusion with panels sides
			AC = C-A;
			BD = D-B;

			AC *= 0.03;
			A  += AC;
			C  -= AC;
			BD *= 0.03;
			B  += BD;
			D  -= BD;


			if(m_pWPolar && m_pWPolar->m_bVLM1)
			{
				glLineStipple (1, 0xFFFF);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
				}
				glEnd();
				glLineStipple (1, 0x0F0F);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
				}
				glEnd();
			}
			else if(!m_pWPolar || (m_pWPolar && !m_pWPolar->m_bVLM1))
			{
				glBegin(GL_LINE_STRIP);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
					glVertex3d(D.x, D.y, D.z);
					glVertex3d(A.x, A.y, A.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}



void GL3dViewDlg::GLCreateWOppLegend()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	double dD, YPos, XPos;
	QString Result, str, strong, str1;
	int i,l;

	QFont font("Courier", 10);
	QFontMetrics fm(font);
	dD = fm.height()+5;//pixels

	YPos = m_pglWidget->m_rCltRect.bottom()- 11 * dD;
	XPos = m_pglWidget->m_rCltRect.right() - 10 ;

	glNewList(WOPPLEGEND,GL_COMPILE);
	{
		m_GLList++;

		glColor3d(pMainFrame->m_TextColor.redF(),pMainFrame->m_TextColor.greenF(),pMainFrame->m_TextColor.blueF());
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pWOpp)
		{
			YPos += m_pWOpp->m_nFlaps * dD;
			if(m_pWOpp->m_bOut)
			{
				YPos -=dD;
				Result = "Point is out of the flight envelope";
				m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);
			}

			GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
			l = str.length();
			if     (l==2) Result = QString("QInf = %1 ").arg(m_pWOpp->m_QInf*pMainFrame->m_mstoUnit,7,'f',2);
			else if(l==3) Result = QString("QInf = %1 ").arg(m_pWOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',1);
			else if(l==4) Result = QString("QInf = %1 ").arg(m_pWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',1);
			Result += str;
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("Alpha = %1").arg(m_pWOpp->m_Alpha,6,'f',2);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("CL = %1").arg(m_pWOpp->m_CL, 6,'f',2);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("CD = %1").arg(m_pWOpp->m_ViscousDrag+m_pWOpp->m_InducedDrag,9,'f',4);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			double cxielli=m_pWOpp->m_CL*m_pWOpp->m_CL/pi/m_pWing->m_AR;
			Result = QString("Efficiency = %1 ").arg(cxielli/m_pWOpp->m_InducedDrag,9,'f',4);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("Cl/Cd = %1 ").arg(m_pWOpp->m_CL/(m_pWOpp->m_InducedDrag+m_pWOpp->m_ViscousDrag),9,'f',4);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("GCm = %1 ").arg(m_pWOpp->m_GCm,9,'f',4);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("Rolling Moment = %1").arg(m_pWOpp->m_GRm);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("Induced Moment = %1").arg(m_pWOpp->m_IYm);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			Result = QString("Airfoil Yawing Moment = %1").arg(m_pWOpp->m_GYm);
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			GetLengthUnit(str, pMainFrame->m_LengthUnit);
			l = str.length();
			if (l==1)     Result = QString("XCP = %1 ").arg(m_pWOpp->m_XCP*pMainFrame->m_mtoUnit, 8, 'f', 3);
			else if(l==2) Result = QString("XCP = %1 ").arg(m_pWOpp->m_XCP*pMainFrame->m_mtoUnit, 7, 'f', 2);
			else if(l>=3) Result = QString("XCP = %1 ").arg(m_pWOpp->m_XCP*pMainFrame->m_mtoUnit, 7, 'f', 2);
			Result += str;
			YPos += dD;
			m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);

			for(i=0; i<m_pWOpp->m_nFlaps; i++)
			{
				Result = QString("Flap Moment[%1] = %2").arg(i+1).arg(m_pWOpp->m_FlapMoment[i]*pMainFrame->m_NmtoUnit, 12,'f',4);
				GetMomentUnit(str, pMainFrame->m_MomentUnit);
				Result += str;
				YPos += dD;
				m_pglWidget->renderText(XPos-fm.width(Result), YPos, Result);
			}
		}
	}
	glEndList();

	if(m_bglLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}




void GL3dViewDlg::GLDrawAxes()
{
	double l = .8;
//	if(m_pWing) l=1.1*m_pWing->m_Span/2.0;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth((GLfloat)(pMiarex->m_3DAxisWidth));

	glColor3d(pMiarex->m_3DAxisColor.redF(),pMiarex->m_3DAxisColor.greenF(),pMiarex->m_3DAxisColor.blueF());

// X axis____________
	glEnable (GL_LINE_STIPPLE);
	if(pMiarex->m_3DAxisStyle == 1) 		glLineStipple (1, 0x1111);
	else if(pMiarex->m_3DAxisStyle== 2) 	glLineStipple (1, 0x0F0F);
	else if(pMiarex->m_3DAxisStyle== 3) 	glLineStipple (1, 0x1C47);
	else									glLineStipple (1, 0xFFFF);// Solid

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


void GL3dViewDlg::GLDraw3D()
{
	if (!m_pWing)
	{
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}
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

//		GLCreateBodyPoints();
		m_bResetglBodyPoints = false;
	}

	if(m_bResetglBodyOverlay && m_pBody)
	{
		if(glIsList(BODYOVERLAY))
		{
			glDeleteLists(BODYOVERLAY,1);
			m_GLList -=1;
		}

//		GLCreateBodyOverlay();
		m_bResetglBodyOverlay = false;
	}

	if(m_bResetglBody2D && m_pBody)
	{
		if(glIsList(BODYAXIALLINES))
		{
			glDeleteLists(BODYAXIALLINES,5);
			m_GLList -=5;
		}

//		GLCreateBody2DBodySection();
//		GLCreateBodyFrames();
//		GLCreateBodyGrid();
		m_bResetglBody2D = false;
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
//		if((m_pBody)->m_LineType==1)	    GLCreateBody3DFlatPanels();
//		else if((m_pBody)->m_LineType==2)	GLCreateBody3DSplines();

		m_bResetglBody = false;
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
//		GLCreateBodyMesh();
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBodyMesh && m_pBody)
	{
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
//		GLCreateBodyMesh();
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBody || m_bResetglGeom)
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
						glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
					}
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr ) * pi/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = -(row * lat_incr) * pi/180.0;
						glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
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
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * pi/180.0;
					glVertex3d(R * cos(-phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(-phi) * cos(theta)*m_GLScale);
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
					glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=-NumAngles; col<NumAngles; col++)
				{
					phi = (0.0 + (double)col*30.0/NumAngles) * pi/180.0;
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();
		}
		glEndList();
	}

	if(m_bResetglGeom  && m_iView==3)
	{

		if(m_pWing)
		{
			if(glIsList(WINGSURFACES))
			{
				glDeleteLists(WINGSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pWing,WINGSURFACES,(m_pWing)->m_WingColor);
		}

		if(m_pWing2)
		{
			if(glIsList(WING2SURFACES))
			{
				glDeleteLists(WING2SURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pWing2,WING2SURFACES,(m_pWing2)->m_WingColor);
		}

		if(m_pStab)
		{
			if(glIsList(STABSURFACES))
			{
				glDeleteLists(STABSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pStab,STABSURFACES, (m_pStab)->m_WingColor);
		}

		if(m_pFin)
		{
			if(glIsList(FINSURFACES))
			{
				glDeleteLists(FINSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pFin,FINSURFACES, (m_pFin)->m_WingColor);
		}

		m_bResetglGeom = false;
	}

	if(m_bResetglMesh && m_bVLMPanels && m_iView==3)
	{
		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh();
		if(glIsList(VLMCTRLPTS))
		{
			glDeleteLists(VLMCTRLPTS,1);
			m_GLList-=1;
		}
		GLCreateCtrlPts();
		if(glIsList(VLMVORTICES))
		{
			glDeleteLists(VLMVORTICES,1);
			m_GLList-=1;
		}
		GLCreateVortices();
		m_bResetglMesh = false;
	}

	if(m_bResetglWake && m_iView==3)
	{
		if(glIsList(WINGWAKEPANELS))
		{
			glDeleteLists(WINGWAKEPANELS,1);
			m_GLList-=1;
		}
		if (m_pWPolar && (m_pWPolar)->m_AnalysisType==3)
		{
//			GLCreateWakePanels(WINGWAKEPANELS);
		}
		m_bResetglWake = false;
	}

	if((m_bResetglLift || m_bResetglOpp) && m_iView==3)
	{

		if(glIsList(VLMWINGLIFT))
		{
			glDeleteLists(VLMWINGLIFT,3);
			m_GLList -=3;
		}
		if(glIsList(VLMWING2LIFT))
		{
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT))
		{
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT))
		{
			glDeleteLists(VLMSTABLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINLIFT))
		{
			glDeleteLists(VLMFINLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWINGTOPTRANS))
		{
			glDeleteLists(VLMWINGTOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMWING2TOPTRANS))
		{
			glDeleteLists(VLMWING2TOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMSTABTOPTRANS))
		{
			glDeleteLists(VLMSTABTOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMFINTOPTRANS))
		{
			glDeleteLists(VLMFINTOPTRANS,2);
			m_GLList-=2;
		}
                if ((m_pWing && m_pWOpp) || (m_pPOpp &&m_pWOpp) )
		{
			GLCreateLiftStrip(m_pWing, m_pWOpp, VLMWINGLIFT);
//			GLCreateTrans(m_pWing, m_pWOpp, VLMWINGTOPTRANS);
			int surf = (m_pWing)->m_NSurfaces;
			if(m_pPOpp)
			{
				if(m_pWing2)
				{
					GLCreateLiftStrip(m_pWing2, & (m_pPOpp)->m_Wing2WOpp, VLMWING2LIFT);
//					GLCreateTrans(m_pWing2, &(m_pPOpp)->m_Wing2WOpp, VLMWING2TOPTRANS);
					surf += (m_pWing2)->m_NSurfaces;
				}
				if(m_pStab)
				{
					GLCreateLiftStrip(m_pStab, &(m_pPOpp)->m_StabWOpp, VLMSTABLIFT);
//					GLCreateTrans(m_pStab, &(m_pPOpp)->m_StabWOpp, VLMSTABTOPTRANS);
					surf += (m_pStab)->m_NSurfaces;
				}
				if(m_pFin)
				{
					GLCreateLiftStrip(m_pFin, &(m_pPOpp)->m_FinWOpp, VLMFINLIFT);
//					GLCreateTrans(m_pFin, &(m_pPOpp)->m_FinWOpp, VLMFINTOPTRANS);
				}
			}
			GLCreateLiftForce();
			GLCreateMoments();
		}
		m_bResetglLift = false;
	}

	if((m_bResetglDrag || m_bResetglOpp) && m_iView==3)
	{
		if(glIsList(VLMWINGDRAG))
		{
			glDeleteLists(VLMWINGDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWING2DRAG))
		{
			glDeleteLists(VLMWING2DRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABDRAG))
		{
			glDeleteLists(VLMSTABDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINDRAG))
		{
			glDeleteLists(VLMFINDRAG,1);
			m_GLList -=1;
		}

                if ((m_pWing && m_pWOpp) || (m_pPOpp && m_pWOpp))
		{
			GLCreateDrag(m_pWing, m_pWOpp, VLMWINGDRAG);
			int surf = (m_pWing)->m_NSurfaces;
			if(m_pPOpp)
			{
				if(m_pWing2)
				{
					GLCreateDrag(m_pWing2, &(m_pPOpp)->m_Wing2WOpp, VLMWING2DRAG);
					surf += (m_pWing2)->m_NSurfaces;
				}
				if(m_pStab)
				{
					GLCreateDrag(m_pStab, &(m_pPOpp)->m_StabWOpp, VLMSTABDRAG);
					surf += (m_pStab)->m_NSurfaces;
				}
				if(m_pFin)
				{
					GLCreateDrag(m_pFin, &(m_pPOpp)->m_FinWOpp, VLMFINDRAG);
				}
			}
		}
		m_bResetglDrag = false;
	}

	if((m_bResetglDownwash || m_bResetglOpp) && m_iView==3)
	{
		if(glIsList(VLMWINGWASH))
		{
			glDeleteLists(VLMWINGWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMWING2WASH))
		{
			glDeleteLists(VLMWING2WASH,1);
			m_GLList--;
		}
		if(glIsList(VLMSTABWASH))
		{
			glDeleteLists(VLMSTABWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMFINWASH))
		{
			glDeleteLists(VLMFINWASH,1);
			m_GLList--;
		}
		if (m_pWing && m_pWOpp)
		{
			GLCreateDownwash(m_pWing, m_pWOpp, VLMWINGWASH,0);
			int surf = (m_pWing)->m_NSurfaces;
			if(m_pPOpp)
			{
				if(m_pWing2)
				{
					GLCreateDownwash(m_pWing2, &(m_pPOpp)->m_Wing2WOpp, VLMWING2WASH, surf);
					surf += (m_pWing2)->m_NSurfaces;
				}
				if(m_pStab)
				{
					GLCreateDownwash(m_pStab, &(m_pPOpp)->m_StabWOpp, VLMSTABWASH, surf);
					surf += (m_pStab)->m_NSurfaces;
				}
				if(m_pFin)		GLCreateDownwash(m_pFin, &(m_pPOpp)->m_FinWOpp, VLMFINWASH, surf);
			}
		}
		m_bResetglDownwash = false;
	}

	if(m_bResetglOpp && m_iView==3)
	{
		if (m_pWing && m_pWOpp)
		{
			if(glIsList(PANELCP))
			{
				glDeleteLists(PANELCP,1);
				m_GLList--;
			}

			GLCreateCp();
		}

		m_bResetglOpp = false;
	}

	if((m_bResetglLegend || m_bResetglOpp || m_bResetglGeom) && m_iView==3)
	{
		if(glIsList(WINGLEGEND))
		{
			glDeleteLists(WINGLEGEND,1);
			m_GLList -= 1;
		}
//		GLCreateWingLegend();
		if(m_pWOpp)
		{
			glDeleteLists(WOPPLEGEND,3);
			m_GLList -= 3;
//			GLCreateWOppLegend();
			GLCreateCpLegend();
		}
		m_bResetglLegend = false;
	}

	// draw once without streamlines to update the screen
	// then draw with streamlines
	if((m_bResetglStream || m_bResetglOpp) && m_iView==3)
	{
		if(m_bStream)
		{
			//no need to recalculate if not showing
			if(m_pWing && m_pWOpp && (m_pWOpp)->m_AnalysisType>=2)
			{

				if(glIsList(VLMSTREAMLINES))
				{
					glDeleteLists(VLMSTREAMLINES,1);
					m_GLList -=1;
				}
//				GLCreateStreamLines();
				m_bResetglStream = false;
//				UpdateView();
			}
		}
	}

	if((m_bResetglFlow || m_bResetglOpp) && m_iView==3)
	{
		if(m_bSpeeds)
		{
			if(m_pWing && m_pWOpp && (m_pWOpp)->m_AnalysisType>=2)
			{

				if(glIsList(SURFACESPEEDS))
				{
					glDeleteLists(SURFACESPEEDS,1);
					m_GLList -=1;
				}
//				GLCreateSurfSpeeds();
				m_bResetglFlow = false;
//				UpdateView();
			}
		}
	}
}


void GL3dViewDlg::GLInverseMatrix()
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


void GL3dViewDlg::GLRenderView()
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

//	wglMakeCurrent(pDC->m_hDC,pChildView->m_hRC);

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
		if(m_iView==3)
		{
			if (m_b3DCp && m_pWOpp && m_pWOpp->m_AnalysisType>=2 /*&& m_bShowCpScale*/)
			{
				glCallList(WOPPCPLEGENDTXT);
				glCallList(WOPPCPLEGENDCLR);
			}
			if(m_pWing)
				glCallList(WINGLEGEND);
			if(m_pWOpp)
			{
				glCallList(WOPPLEGEND);
				glCallList(WOPPLEGEND);
			}
		}

		GLSetupLight();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
/*		if(m_bShowLight)
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glTranslated(( pGLLightDlg->m_XLight+ m_UFOOffset.x)*m_GLScale,
							 ( pGLLightDlg->m_YLight+ m_UFOOffset.y)*m_GLScale,
							   pGLLightDlg->m_ZLight*m_GLScale);
				double radius = (pGLLightDlg->m_ZLight+2.0)/40.0*m_GLScale;
				QColor color;
				color = QColor((int)(pGLLightDlg->m_Red  *255),
							(int)(pGLLightDlg->m_Green*255),
							(int)(pGLLightDlg->m_Blue *255));
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

/*		GLUquadricObj* quadric;
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		quadric = gluNewQuadric();
		gluQuadricDrawStyle(quadric,GLU_FILL);
		gluQuadricNormals(quadric,GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);
		gluSphere(quadric,.5,40,40);*/

		GLCallViewLists();
	}
	glPopMatrix();

	glDisable(GL_CLIP_PLANE1);
	glFinish();
//	SwapBuffers(pDC->m_hDC);
//	wglMakeCurrent(pDC->m_hDC, NULL);
}


void GL3dViewDlg::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
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


void GL3dViewDlg::GLSetupLight()
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
	if(m_pWing)      LightFactor =  (GLfloat)pow((m_pWing)->m_Span/2.0,0.1);
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

void GL3dViewDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
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

void GL3dViewDlg::keyReleaseEvent(QKeyEvent *event)
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

void GL3dViewDlg::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
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


void GL3dViewDlg::mouseMoveEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x() - m_pglWidget->geometry().x(), event->pos().y() - m_pglWidget->geometry().y());

	CVector Real;
	QPoint Delta(point.x() - m_LastPoint.x(), point.y() - m_LastPoint.y());
	ClientToGL(point, Real);

	if(!m_pglWidget->hasFocus()) m_pglWidget->setFocus();

	int n;
	bool bCtrl = false;
//	SHORT shX  = GetKeyState('X');
//	SHORT shY  = GetKeyState('Y');
//	SHORT shZ  = GetKeyState('Z');
//	SHORT sh1  = GetKeyState(VK_LCONTROL);
//	SHORT sh2  = GetKeyState(VK_RCONTROL);

	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if (event->buttons() & Qt::LeftButton && m_iView==3)
	{
		if(bCtrl)//rotate
		{
			m_ArcBall.Move(point.x(), m_pglWidget->m_rCltRect.height()-point.y());

			UpdateView();

			// Flush the event queue otherwise the display cannot follow the mouse movements
			// Except for ctrl up and left button up messages
/*			MSG msg;
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
//				GetMessage(&msg, NULL, 0, 0);
				if(msg.message == WM_KEYUP && msg.wParam == VK_CONTROL)
				{
					PreTranslateMessage(&msg);
				}
				else if(msg.message == WM_LBUTTONUP)
				{
					OnLButtonUp(nFlags, point);
				}
				else
				{
//					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}*/
		}
		else if(m_bTrans)
		{
			//translate
			m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0*m_GLScale/m_glScaled/m_pglWidget->m_rCltRect.width());
			m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0*m_GLScale/m_glScaled/m_pglWidget->m_rCltRect.width());

			m_glRotCenter.x = MatOut[0][0]*m_glViewportTrans.x + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
			m_glRotCenter.y = MatOut[1][0]*m_glViewportTrans.x + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
			m_glRotCenter.z = MatOut[2][0]*m_glViewportTrans.x + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

			UpdateView();
		}
	}
	else if ((event->buttons() & Qt::MidButton) && !bCtrl)
	//scaling
	{
		// we zoom the graph or the wing

		if(m_pWing )
		{	//zoom 3D wing
			if(point.y()-m_LastPoint.y()>0) m_glScaled *= (GLfloat)1.06;
			else                            m_glScaled /= (GLfloat)1.06;

			UpdateView();
		}

	}
	m_LastPoint = point;

//	CWnd::OnMouseMove(nFlags, point);
}


void GL3dViewDlg::mousePressEvent(QMouseEvent *event)
{
//	point is in client coordinates
	int iF;
	QPoint point(event->pos().x() - m_pglWidget->geometry().x(), event->pos().y() - m_pglWidget->geometry().y());
	CVector Real;
	bool bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	ClientToGL(point, Real);

	if(m_pglWidget->m_rCltRect.contains(point)) m_pglWidget->setFocus();

	if(m_iView==3)
	{
		if(m_bPickCenter)
		{
			Set3DRotationCenter(point);
			m_bPickCenter = false;
			m_pctrlPickCenter->setChecked(false);
		}
		else
		{
			m_ArcBall.Start(point.x(), m_pglWidget->m_rCltRect.height()-point.y());
			m_bCrossPoint = true;

			Set3DRotationCenter();

			if (!bCtrl)
			{
				m_bTrans = true;
				setCursor(m_hcMove);
			}
		}

		UpdateView();
	}

	m_bPickCenter = false;
	m_PointDown = point;
	m_LastPoint = point;
}


void GL3dViewDlg::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint point(event->pos().x() - m_pglWidget->geometry().x(), event->pos().y() - m_pglWidget->geometry().y());
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

void GL3dViewDlg::On3DCp(bool bChecked)
{
	m_b3DCp = bChecked;
//	m_b3DCp = m_pctrlCp->isChecked();
	if(m_b3DCp) m_bSurfaces = false;
	UpdateView();
}

void GL3dViewDlg::On3DIso()
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



void GL3dViewDlg::On3DTop()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dViewDlg::On3DLeft()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90° around x
	Set3DRotationCenter();
	UpdateView();
}


void GL3dViewDlg::On3DFront()
{
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90° around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90° around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void GL3dViewDlg::On3DReset()
{
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_bIs3DScaleSet = false;
	Set3DScale();
	UpdateView();
}


void GL3dViewDlg::On3DPickCenter()
{
	m_bPickCenter = true;
	m_pctrlPickCenter->setChecked(true);
}



void GL3dViewDlg::OnAxes(int state)
{
	m_bAxes = m_pctrlAxes->isChecked();
//	m_bResetglBody2D = true;
	UpdateView();
}


void GL3dViewDlg::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}


void GL3dViewDlg::OnCpScale()
{
	CpScaleDlg dlg;
	dlg.m_LegendMax = m_LegendMax;
	dlg.m_LegendMin = m_LegendMin;
	dlg.m_bAutoCpScale = m_bAutoCpScale;
	dlg.InitDialog();
//qDebug() << "kjhj"<< m_LegendMax << m_LegendMin;
	dlg.exec();

	m_LegendMax = dlg.m_LegendMax;
	m_LegendMin = dlg.m_LegendMin;
	m_bAutoCpScale = dlg.m_bAutoCpScale;
	m_bResetglLegend = true;
	UpdateView();
}

void GL3dViewDlg::OnLiftScale(int pos)
{
	m_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglLift = true;
	UpdateView();
}

void GL3dViewDlg::OnDragScale(int pos)
{
	m_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglDrag = true;
	UpdateView();
}

void GL3dViewDlg::OnVelocityScale(int pos)
{
	m_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglDownwash = true;
	UpdateView();
}

void GL3dViewDlg::OnLift(int state)
{
	m_bXCP = m_pctrlLift->isChecked();
	UpdateView();
}


void GL3dViewDlg::OnIDrag(int state)
{
	m_bICd = m_pctrlIDrag->isChecked();
	UpdateView();
}

void GL3dViewDlg::OnVDrag(int state)
{
	m_bVCd = m_pctrlVDrag->isChecked();
	UpdateView();
}

void GL3dViewDlg::OnDownwash(int state)
{
	m_bDownwash = m_pctrlDownwash->isChecked();
	UpdateView();
}


void GL3dViewDlg::OnMoment(int state)
{
	m_bMoments = m_pctrlMoment->isChecked();
	UpdateView();
}


void GL3dViewDlg::OnLight(int state)
{
	m_bglLight = m_pctrlLight->isChecked();
	UpdateView();
}

void GL3dViewDlg::OnOutline(int state)
{
	m_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


void GL3dViewDlg::OnPanels(int state)
{
	m_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}

void GL3dViewDlg::OnSurfaces(int state)
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
	UpdateView();
}

void GL3dViewDlg::OnSetupLight()
{
	GLLightDlg *pGLLightDlg = (GLLightDlg *)s_pGLLightDlg;
	m_bShowLight = true;
	UpdateView();
	pGLLightDlg->m_pGL3dViewDlg = this;
	pGLLightDlg->exec();

	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}

void GL3dViewDlg::OnVortices(int state)
{
	m_bVortices = m_pctrlVortices->isChecked();
	UpdateView();
}

void GL3dViewDlg::resizeEvent(QResizeEvent *event)
{
	m_bResetglLegend = true;
}


void GL3dViewDlg::Set3DRotationCenter()
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


void GL3dViewDlg::Set3DRotationCenter(QPoint point)
{
	//adjusts the new rotation center after the user has picked a point on the screen
	//finds the closest panel under the point,
	//and changes the rotation vector and viewport translation
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int  i, j, p;
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

	if(m_pWOpp)
	{
		AA.RotateY(-(m_pWOpp)->m_Alpha);
		BB.RotateY(-(m_pWOpp)->m_Alpha);
	}

	U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
	U.Normalize();

	bool bIntersect = false;

	if(m_iView==3)
	{
		for(p=0; p<pMiarex->m_MatSize; p++)
		{
			if(m_pPanel[p].Intersect(AA, U, I, dist))
//			if(Intersect(m_pNode[m_pPanel[p].m_iLA], m_pNode[m_pPanel[p].m_iLB], m_pNode[m_pPanel[p].m_iTA], m_pNode[m_pPanel[p].m_iTB],
//				         m_pPanel[p].Normal, AA, U, I, dist))
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
/*	else if(m_iView==5 && m_pBody)
	{
		if((m_pBody)->m_LineType==2)
		{
			int nx, nh, inx, inh;
			CVector N, LATB, TALB, LA, LB, TA, TB;
			nx = min(500, pMiarex->m_NXPoints);
			nh = max(pMiarex->m_NHoopPoints, 3);

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

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
					{
						if(dist < dmin)
						{
							dmin = dist;
							PP.Set(I);
							bIntersect = true;
						}
					}

					N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
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
		else if((m_pBody)->m_LineType==1)
		{
			int inh, inx;
			CVector Tj, Tjp1, LA, LB, TA, TB, TALB, LATB, N;
			for (inh=0; inh<(m_pBody)->m_NSideLines-1;inh++)
			{
				for (inx=0; inx<(m_pBody)->m_NStations-1;inx++)
				{
					Tj.Set((m_pBody)->m_FramePosition[inx].x,     0.0, 0.0);
					Tjp1.Set((m_pBody)->m_FramePosition[inx+1].x, 0.0, 0.0);

					LB = (m_pBody)->m_Frame[inx].m_Point[inh];		LB.x += Tj.x;
					TB = (m_pBody)->m_Frame[inx+1].m_Point[inh];		TB.x += Tjp1.x;
					TA = (m_pBody)->m_Frame[inx+1].m_Point[inh+1];	TA.x += Tjp1.x;
					LA = (m_pBody)->m_Frame[inx].m_Point[inh+1];		LA.x += Tj.x;

					TALB = LB - TA;
					LATB = TB - LA;
					N = TALB * LATB;
					N.Normalize();

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
					{
						if(dist < dmin)
						{
							dmin = dist;
							PP.Set(I);
							bIntersect = true;
						}
					}

					N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist))
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
	}*/

	if(bIntersect)
	{
		if(m_pWOpp)
		{
			PP.RotateY((m_pWOpp)->m_Alpha);
		}
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


void GL3dViewDlg::Set3DScale()
{
	if(m_iView==3) m_bResetglLegend = true;
	if(m_bIs3DScaleSet && !m_bAutoScales) return;

	QRect CltRect = m_pglWidget->geometry();

	if(m_pWing)
	{
		//wing along X axis will take 3/4 of the screen
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/(m_pWing)->m_Span);
		m_glViewportTrans.x = 0.0;
		m_glViewportTrans.y = 0.0;
		m_glViewportTrans.z = 0.0;
		m_bIs3DScaleSet = true;
	}
	else if(m_pBody)
	{
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/m_pBody->GetLength());
		m_glViewportTrans.x = 0.0;
		m_glViewportTrans.y = 0.0;
		m_glViewportTrans.z = 0.0;
		m_bIs3DScaleSet = true;
	}


	else //(m_iView==3)
	{
/*		double gh = (double)m_pglWidget->m_rCltRect.height()/(double)m_pglWidget->m_rCltRect.width() ;
		if(gh<1.0)
		{
			m_UFOOffset.x = 0.0;
			m_UFOOffset.y = (GLfloat)(gh - 1.0)*m_GLScale;
		}
		else
		{
			m_UFOOffset.x = (GLfloat)(1.0/gh - 1.0)*m_GLScale;
			m_UFOOffset.y = 0.0;
		}*/
		m_UFOOffset.x = 0.0;
		m_UFOOffset.y = 0.0;
	}

	m_ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	m_ArcBall.SetZoom(0.3,eye,up);

	Set3DRotationCenter();
}

void  GL3dViewDlg::SetWOppPtr(CWOpp* pWOpp)
{
	m_pWOpp    = pWOpp;
	m_bResetglDrag  = true;
	m_bResetglFlow  = true;
	m_bResetglDownwash = true;
	m_bResetglLegend   = true;
	m_bResetglLift     = true;
	m_bResetglOpp      = true;
	m_bResetglStream   = true;
	m_bResetglWake     = true;
}

void  GL3dViewDlg::SetPOppPtr(CPOpp* pPOpp)
{
	m_pPOpp    = pPOpp;
	m_bResetglDrag  = true;
	m_bResetglFlow  = true;
	m_bResetglDownwash = true;
	m_bResetglLegend   = true;
	m_bResetglLift     = true;
	m_bResetglOpp      = true;
	m_bResetglStream   = true;
	m_bResetglWake     = true;
}

void  GL3dViewDlg::SetWPlrPtr(CWPolar* pWPolar)
{
	m_pWPolar    = pWPolar;
	m_bResetglDrag  = true;
	m_bResetglFlow  = true;
	m_bResetglDownwash = true;
	m_bResetglLegend   = true;
	m_bResetglLift     = true;
	m_bResetglOpp      = true;
	m_bResetglStream   = true;
	m_bResetglWake     = true;
}

void GL3dViewDlg::SetPointers(CWing *pWing, CWing *pWing2, CWing *pStab, CWing*pFin, CBody *pBody, CPlane *pPlane,
							  CWPolar *pWPolar, CWOpp* pWOpp, CPOpp *pPOpp)
{
	m_pWing    = pWing;
	m_pWing2   = pWing2;
	m_pStab    = pStab;
	m_pFin     = pFin;
	m_pBody    = pBody;
	m_pPlane   = pPlane;
	m_pWPolar  = pWPolar;
	m_pWOpp    = pWOpp;
	m_pPOpp    = pPOpp;
	m_bResetglBody = true;
	m_bResetglBody2D = true;
	m_bResetglBodyMesh = true;
	m_bResetglBodyOverlay = true;
	m_bResetglBodyPoints = true;
	m_bResetglGeom  = true;
	m_bResetglMesh  = true;
	m_bResetglDrag  = true;
	m_bResetglFlow  = true;
	m_bResetglDownwash = true;
	m_bResetglLegend   = true;
	m_bResetglLift     = true;
	m_bResetglOpp      = true;
	m_bResetglStream   = true;
	m_bResetglWake     = true;
}



void GL3dViewDlg::SetupLayout()
{
//	QDesktopWidget desktop;
//	QRect r = desktop.screenGeometry();
//	setMinimumWidth(r.width()/2);
	int r1 = geometry().width();

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);
//setSizePolicy(szPolicyMinimum);
	m_pglWidget = new GLWidget(this);
//	m_pglWidget->setSizePolicy(szPolicyMinimum);
//	m_pglWidget->setMinimumWidth(r1);
//	m_pglWidget->setSizePolicy(szPolicyExpanding);


	QGridLayout *ThreeDParams = new QGridLayout;
	m_pctrlAxes       = new QCheckBox("Axes");
	m_pctrlLight      = new QCheckBox("Light");
	m_pctrlSurfaces   = new QCheckBox("Surfaces");
	m_pctrlOutline    = new QCheckBox("Outline");
	m_pctrlPanels     = new QCheckBox("Panels");
	m_pctrlVortices   = new QCheckBox("Vortices");
	m_pctrlAxes->setSizePolicy(szPolicyMinimum);
	m_pctrlLight->setSizePolicy(szPolicyMinimum);
	m_pctrlSurfaces->setSizePolicy(szPolicyMinimum);
	m_pctrlOutline->setSizePolicy(szPolicyMinimum);
	m_pctrlPanels->setSizePolicy(szPolicyMinimum);
	m_pctrlVortices->setSizePolicy(szPolicyMinimum);
	ThreeDParams->addWidget(m_pctrlAxes, 1,1);
	ThreeDParams->addWidget(m_pctrlLight, 1,2);
	ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
	ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	ThreeDParams->addWidget(m_pctrlPanels, 3,1);
	ThreeDParams->addWidget(m_pctrlVortices, 3,2);

	QGridLayout *ThreeDView = new QGridLayout;
	m_pctrlX          = new QPushButton("X");
	m_pctrlY          = new QPushButton("Y");
	m_pctrlZ          = new QPushButton("Z");
	m_pctrlIso        = new QPushButton("Iso");
	m_pctrlX->setSizePolicy(szPolicyMinimum);
	m_pctrlY->setSizePolicy(szPolicyMinimum);
	m_pctrlZ->setSizePolicy(szPolicyMinimum);
	m_pctrlIso->setSizePolicy(szPolicyMinimum);

	ThreeDView->addWidget(m_pctrlX,1,1);
	ThreeDView->addWidget(m_pctrlY,1,2);
	ThreeDView->addWidget(m_pctrlZ,2,1);
	ThreeDView->addWidget(m_pctrlIso,2,2);

	QGridLayout *ThreeDCtrls = new QGridLayout;
	m_pctrlPickCenter = new QPushButton("Pick Center");
	m_pctrlReset      = new QPushButton("Reset");
	m_pctrlGLLight    = new QPushButton("Light");
	m_pctrlCpScales   = new QPushButton("Cp Scales");
	m_pctrlPickCenter->setSizePolicy(szPolicyMinimum);
	m_pctrlReset->setSizePolicy(szPolicyMinimum);
	m_pctrlGLLight->setSizePolicy(szPolicyMinimum);
	ThreeDCtrls->addWidget(m_pctrlReset,1,1);
	ThreeDCtrls->addWidget(m_pctrlPickCenter,1,2);
	ThreeDCtrls->addWidget(m_pctrlGLLight,2,1);
	ThreeDCtrls->addWidget(m_pctrlCpScales,2,2);

	m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
	m_pctrlClipPlanePos->setMinimum(-300);
	m_pctrlClipPlanePos->setMaximum(300);
	m_pctrlClipPlanePos->setSliderPosition(0);
	m_pctrlClipPlanePos->setTickInterval(30);
	m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
	m_pctrlClipPlanePos->setSizePolicy(szPolicyMinimum);

	QVBoxLayout *ThreeDViewControls = new QVBoxLayout;
	ThreeDViewControls->addLayout(ThreeDCtrls);
	ThreeDViewControls->addWidget(m_pctrlClipPlanePos);

	QVBoxLayout *WOppGroup = new QVBoxLayout;
	QGridLayout *WOppCheckLayout = new QGridLayout;
	m_pctrlLift          = new QCheckBox("Lift");
	m_pctrlMoment        = new QCheckBox("Moment");
	m_pctrlIDrag         = new QCheckBox("Ind. Drag");
	m_pctrlVDrag         = new QCheckBox("Visc. Drag");
	m_pctrlTrans         = new QCheckBox("Trans.");
	m_pctrlDownwash      = new QCheckBox("Downw.");
	m_pctrlCp            = new QCheckBox("Cp");
	m_pctrlSurfVel       = new QCheckBox("Surf. Vel.");
	m_pctrlAnimate       = new QCheckBox("Anim.");
	m_pctrlStream        = new QCheckBox("Stream");
	m_pctrlLift->setSizePolicy(szPolicyMinimum);
	m_pctrlMoment->setSizePolicy(szPolicyMinimum);
	m_pctrlIDrag->setSizePolicy(szPolicyMinimum);
	m_pctrlVDrag->setSizePolicy(szPolicyMinimum);
	m_pctrlTrans->setSizePolicy(szPolicyMinimum);
	m_pctrlDownwash->setSizePolicy(szPolicyMinimum);
	m_pctrlCp->setSizePolicy(szPolicyMinimum);
	m_pctrlSurfVel->setSizePolicy(szPolicyMinimum);
	m_pctrlAnimate->setSizePolicy(szPolicyMinimum);
	m_pctrlStream->setSizePolicy(szPolicyMinimum);

	WOppCheckLayout->addWidget(m_pctrlLift,     1, 1);
	WOppCheckLayout->addWidget(m_pctrlMoment,   1, 2);
	WOppCheckLayout->addWidget(m_pctrlIDrag,    2, 1);
	WOppCheckLayout->addWidget(m_pctrlVDrag,    2, 2);
	WOppCheckLayout->addWidget(m_pctrlTrans,    3, 1);
	WOppCheckLayout->addWidget(m_pctrlDownwash, 3, 2);
	WOppCheckLayout->addWidget(m_pctrlCp,       4, 1);
	WOppCheckLayout->addWidget(m_pctrlSurfVel,  4, 2);
	WOppCheckLayout->addWidget(m_pctrlAnimate,  5, 1);
	WOppCheckLayout->addWidget(m_pctrlStream,   5, 2);

	m_pctrlAnimateSpeed  = new QSlider(Qt::Horizontal);
	m_pctrlAnimateSpeed->setMinimum(0);
	m_pctrlAnimateSpeed->setMaximum(500);
	m_pctrlAnimateSpeed->setSliderPosition(250);
	m_pctrlAnimateSpeed->setTickInterval(25);
	m_pctrlAnimateSpeed->setTickPosition(QSlider::TicksBelow);
	m_pctrlAnimateSpeed->setSizePolicy(szPolicyMinimum);

	WOppGroup->addLayout(WOppCheckLayout);
	WOppGroup->addWidget(m_pctrlAnimateSpeed);


	QVBoxLayout *ScaleLayout = new QVBoxLayout;
	m_pctrlLiftScaleSlider  = new QSlider(Qt::Horizontal);
	m_pctrlLiftScaleSlider->setMinimum(0);
	m_pctrlLiftScaleSlider->setMaximum(100);
	m_pctrlLiftScaleSlider->setSliderPosition(50);
	m_pctrlLiftScaleSlider->setTickInterval(5);
	m_pctrlLiftScaleSlider->setTickPosition(QSlider::TicksBelow);
	m_pctrlLiftScaleSlider->setSizePolicy(szPolicyMinimum);
	m_pctrlDragScaleSlider = new QSlider(Qt::Horizontal);
	m_pctrlDragScaleSlider->setMinimum(0);
	m_pctrlDragScaleSlider->setMaximum(100);
	m_pctrlDragScaleSlider->setSliderPosition(50);
	m_pctrlDragScaleSlider->setTickInterval(5);
	m_pctrlDragScaleSlider->setTickPosition(QSlider::TicksBelow);
	m_pctrlDragScaleSlider->setSizePolicy(szPolicyMinimum);
	m_pctrlVelocityScaleSlider  = new QSlider(Qt::Horizontal);
	m_pctrlVelocityScaleSlider->setMinimum(0);
	m_pctrlVelocityScaleSlider->setMaximum(100);
	m_pctrlVelocityScaleSlider->setSliderPosition(50);
	m_pctrlVelocityScaleSlider->setTickInterval(5);
	m_pctrlVelocityScaleSlider->setTickPosition(QSlider::TicksBelow);
	m_pctrlVelocityScaleSlider->setSizePolicy(szPolicyMinimum);


	QLabel *lab1 = new QLabel("Lift Scale");
	QLabel *lab2 = new QLabel("Drag Scale");
	QLabel *lab3 = new QLabel("Velocity Scale");

	ScaleLayout->addWidget(lab1);
	ScaleLayout->addWidget(m_pctrlLiftScaleSlider);
	ScaleLayout->addWidget(lab2);
	ScaleLayout->addWidget(m_pctrlDragScaleSlider);
	ScaleLayout->addWidget(lab3);
	ScaleLayout->addWidget(m_pctrlVelocityScaleSlider);

	QVBoxLayout *ControlsLayout = new QVBoxLayout;
//	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(ThreeDParams);
	ControlsLayout->addLayout(ThreeDView);
	ControlsLayout->addLayout(ThreeDViewControls);
	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(WOppGroup);
	ControlsLayout->addStretch(1);
	ControlsLayout->addLayout(ScaleLayout);
	ControlsLayout->addStretch(1);
	QWidget *ControlsWidget = new QWidget;
	ControlsWidget->setLayout(ControlsLayout);
	int maxSize =qMax(200, r1/30);
	ControlsWidget->setMaximumWidth(maxSize);

	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pglWidget);
	MainLayout->addWidget(ControlsWidget);

	setLayout(MainLayout);
}


void GL3dViewDlg::showEvent(QShowEvent *event)
{
	Set3DScale();

	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlCp->setChecked(m_b3DCp);
	m_pctrlDownwash->setChecked(m_bDownwash);
	m_pctrlMoment->setChecked(m_bMoments);
	m_pctrlTrans->setChecked(m_bXTop);
	m_pctrlLift->setChecked(m_bXCP);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlLight->setChecked(m_bglLight);
	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));

	UpdateView();
}


void GL3dViewDlg::UpdateView()
{
	if(isVisible()) m_pglWidget->updateGL();
}


void  GL3dViewDlg::wheelEvent(QWheelEvent *event)
{
	QPoint point(event->pos().x() - m_pglWidget->geometry().x(), event->pos().y() - m_pglWidget->geometry().y());
	//The mouse button has been wheeled
	//Process the message
//	point is in client coordinates
	if(m_iView==3)
	{
		if(m_pWing)
		{
			//zoom wing
			if(event->delta()>0) m_glScaled /= (GLfloat)1.06;
			else                 m_glScaled *= (GLfloat)1.06;
			UpdateView();
		}
	}
}













