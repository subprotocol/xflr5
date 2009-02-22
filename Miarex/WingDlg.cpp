/****************************************************************************

	WingDlg Class 
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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
#include "./WingDlg.h"
#include "GL3dViewDlg.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <math.h>
 

void* WingDlg::s_pMainFrame;		//pointer to the Frame window
void* WingDlg::s_pMiarex;	//pointer to the Miarex Application window
QList <void *> *WingDlg::s_poaFoil;

WingDlg::WingDlg()
{
	m_pWing = NULL;

	setWindowTitle("Wing Edition");
	SetupLayout();
	QDesktopWidget desktop;
	QRect r = desktop.rect();
	setGeometry(100,50,r.width()/2, r.height()*.75);

	m_pctrlSymetric->setChecked(true);
	m_pctrlRightSide->setChecked(true);
	m_pctrlLeftSide->setEnabled(false);

	m_bAcceptName = true;
	m_bRightSide  = true;
	m_bChanged    = true;
	m_bTrans      = false;
	m_iSection = 0;
	m_yMAC = 0.;
	m_fWingScale = 1.0;

	m_PointDown = QPoint(0,0);
	m_DrawRect.setRect(0,0,0,0);

	pi = 3.141592654;

	m_pInsertBefore = new QAction("Insert Before", this);
	m_pInsertAfter = new QAction("Insert after", this);
	m_pDeleteSection = new QAction("Delete section", this);
	connect(m_pInsertBefore, SIGNAL(triggered()), this, SLOT(OnInsertBefore()));
	connect(m_pInsertAfter, SIGNAL(triggered()), this, SLOT(OnInsertAfter()));
	connect(m_pDeleteSection, SIGNAL(triggered()), this, SLOT(OnDeleteSection()));

	m_pContextMenu = new QMenu("Section",this);
	m_pContextMenu->addAction(m_pInsertBefore);
	m_pContextMenu->addAction(m_pInsertAfter);
	m_pContextMenu->addAction(m_pDeleteSection);
	m_pctrlWingTable->setSelectionMode(QAbstractItemView::NoSelection);
	m_pctrlWingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlWingTable->setEditTriggers(QAbstractItemView::CurrentChanged);

	connect(m_pctrlWingColor, SIGNAL(clicked()),this, SLOT(OnWingColor()));
	connect(m_pctrlSymetric, SIGNAL(clicked()),this, SLOT(OnSymetric()));
	connect(m_pctrlRightSide, SIGNAL(clicked()),this, SLOT(OnSide()));
	connect(m_pctrlLeftSide, SIGNAL(clicked()),this, SLOT(OnSide()));
//	connect(m_pctrlWingTable, SIGNAL(activated(const QModelIndex &)), this, SLOT(OnItemActivated(const QModelIndex&)));
	connect(m_pctrlWingTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));
	connect(m_pctrlWingTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnItemClicked(const QModelIndex&)));


}

bool WingDlg::CheckWing()
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

void WingDlg::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	MainFrame *pMainFrame  = (MainFrame*)s_pMainFrame;
	QMiarex    *pMiarex = (QMiarex*)s_pMiarex;
	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	int i, k;

	double surface = 0.0;
	m_pWing->m_TLength[0] = 0.0;
	m_pWing->m_TYProj[0]  = m_pWing->m_TPos[0];
	for (i=1; i<=m_pWing->m_NPanel; i++)
	{
		m_pWing->m_TLength[i] = m_pWing->m_TPos[i] - m_pWing->m_TPos[i-1];
	}
	for (i=1; i<=m_pWing->m_NPanel; i++)
	{
		m_pWing->m_TYProj[i] = m_pWing->m_TYProj[i-1] + m_pWing->m_TLength[i] * cos(m_pWing->m_TDihedral[i-1]*pi/180.0);
	}

	m_pWing->m_Span    = 2.0 * m_pWing->m_TPos[m_pWing->m_NPanel];
	m_pWing->m_MAChord = 0.0;
	m_yMAC    = 0.0;
	m_pWing->m_Volume  = 0.0;
	for (k=0; k<m_pWing->m_NPanel; k++)
	{
		pFoilA = pMainFrame->GetFoil(m_pWing->m_RFoil[k]);
		pFoilB = pMainFrame->GetFoil(m_pWing->m_RFoil[k+1]);
		surface   += m_pWing->m_TLength[k+1]*(m_pWing->m_TChord[k]+m_pWing->m_TChord[k+1])/2.0;//m2
		if(pFoilA && pFoilB)
			m_pWing->m_Volume  += m_pWing->m_TLength[k+1]*(pFoilA->GetArea()*m_pWing->m_TChord[k] + pFoilB->GetArea()*m_pWing->m_TChord[k+1])/2.0;//m3
		m_pWing->m_MAChord += IntegralC2(m_pWing->m_TPos[k], m_pWing->m_TPos[k+1], m_pWing->m_TChord[k], m_pWing->m_TChord[k+1]);
		m_yMAC    += IntegralCy(m_pWing->m_TPos[k], m_pWing->m_TPos[k+1], m_pWing->m_TChord[k], m_pWing->m_TChord[k+1]);
	}
	if(!m_pWing->m_bIsFin || m_pWing->m_bSymFin || m_pWing->m_bDoubleFin)
	{
		m_pWing->m_Area    = 2.0 * surface;
		m_pWing->m_Volume *= 2.0;
		m_pWing->m_MAChord = m_pWing->m_MAChord * 2.0 / m_pWing->m_Area;
		m_yMAC             = m_yMAC             * 2.0 / m_pWing->m_Area;

		m_pWing->m_GChord  = m_pWing->m_Area/m_pWing->m_Span;
		m_pWing->m_AR      = m_pWing->m_Span*m_pWing->m_Span/m_pWing->m_Area;
	}
	else
	{
		m_pWing->m_Area = surface;
		m_pWing->m_MAChord = m_pWing->m_MAChord / m_pWing->m_Area;
		m_yMAC    = m_yMAC    / m_pWing->m_Area;

		m_pWing->m_GChord  = m_pWing->m_Area/m_pWing->m_Span*2.0;
		m_pWing->m_AR      = m_pWing->m_Span*m_pWing->m_Span/m_pWing->m_Area/2.0;
	}

	if(m_pWing->m_TChord[m_pWing->m_NPanel]>0.0)	m_pWing->m_TR = m_pWing->m_TChord[0]/m_pWing->m_TChord[m_pWing->m_NPanel];
	else						                    m_pWing->m_TR = 99999.0;

	//calculate the number of flaps
	m_pWing->m_nFlaps = 0;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span;

	for (i=1; i<=m_pWing->m_NPanel; i++)
	{
		pFoilA = pMainFrame->GetFoil(m_pWing->m_RFoil[i-1]);
		pFoilB = pMainFrame->GetFoil(m_pWing->m_RFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize)
			{
				m_pWing->m_nFlaps++;
			}
		}
		pFoilA = pMainFrame->GetFoil(m_pWing->m_LFoil[i-1]);
		pFoilB = pMainFrame->GetFoil(m_pWing->m_LFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize)
			{
				m_pWing->m_nFlaps++;
			}
		}
	}
}



void WingDlg::contextMenuEvent(QContextMenuEvent *event)
{
	// Display the context menu

	QPoint ScreenPt = event->pos();
	ScreenPt.rx() += geometry().x();
	ScreenPt.ry() += geometry().y();

	QRect TableRect = m_pctrlWingTable->geometry();

	if(m_DrawRect.contains(event->pos()))
	{
		int res = IsFoil(event->pos());

		if(res>=0)
		{
			m_iSection = res;
		}
		if(m_iSection>=0)
		{
			m_pctrlWingTable->selectRow(res);
//			repaint();
			m_pContextMenu->exec(ScreenPt);
		}
	}
	else if(TableRect.contains(event->pos()))
	{
		m_pContextMenu->exec(ScreenPt);
	}
}





void WingDlg::CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap)
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


void WingDlg::DrawDihedral(QPainter &painter, QPoint O)
{
	painter.save();

	double x,z;
	QPoint From, To;
	int yOff = -30;

	QPen DihPen(Qt::black);
	painter.setPen(DihPen);


	if(m_bRightSide)
	{
		From = QPoint(O.x() , O.y() + yOff);
		for (int i=1; i<=m_pWing->m_NPanel;i++)
		{
			x = m_pWing->m_TYProj[i];
			z = GetZPos(m_pWing->m_TPos[i]);
			To.rx() = O.x() +      (int)(x*m_fWingScale);
			To.ry() = O.y() + yOff-(int)(z*m_fWingScale);
			painter.drawLine(From, To);
			From = To;
		}
	}
	else
	{
		From = QPoint(O.x() , O.y() + yOff);
		for (int i=1; i<=m_pWing->m_NPanel;i++)
		{
			x = m_pWing->m_TYProj[i];
			z = GetZPos(m_pWing->m_TPos[i]);
			To.rx() = O.x() -      (int)(x*m_fWingScale);
			To.ry() = O.y() + yOff-(int)(z*m_fWingScale);
			painter.drawLine(From, To);
			From = To;
		}
	}

	painter.restore();
}




void WingDlg::DrawFoils(QPainter &painter, QPoint O)
{
	painter.save();
	double x, y, Chord;
	int i, nFlap;
	QString strong;
	MainFrame *pMainFrame  = (MainFrame*)s_pMainFrame;
	QMiarex   *pMiarex = (QMiarex*)s_pMiarex;

	QPen FoilPen(Qt::black);
	FoilPen.setStyle(Qt::DashLine);
	QPen HighPen(Qt::red);
	HighPen.setStyle(Qt::DashLine);


	painter.setPen(FoilPen);

	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span;

	if(m_bRightSide)
	{
		for (i=0; i<=m_pWing->m_NPanel;i++)
		{
			x = m_pWing->m_TPos[i];
			y = GetOffset(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			Chord = GetChord(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			if(Chord<0.0) break;

			if (i==m_iSection)
			{
				painter.setPen(HighPen);
			}
			else
			{
				painter.setPen(FoilPen);
			}
			painter.drawLine(O.x() + (int)(x*m_fWingScale),	O.y() + (int)(y*m_fWingScale),
							 O.x() + (int)(x*m_fWingScale), O.y() + (int)((y+Chord)*m_fWingScale));

			strong = m_pWing->m_RFoil[i].left(30);
			painter.drawText(O.x() + (int)(x*m_fWingScale), O.y() + 10+(int)((y+Chord)*m_fWingScale), strong);
		}
		nFlap = 0;
		//	count left wing flaps
		for (i=1; i<=m_pWing->m_NPanel; i++)
		{
			pFoilA = pMainFrame->GetFoil(m_pWing->m_LFoil[i-1]);
			pFoilB = pMainFrame->GetFoil(m_pWing->m_LFoil[i]);
			if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize)
			{
				//we have a flap, so count it
				nFlap++;
			}
		}
		for (i=1; i<=m_pWing->m_NPanel; i++)
		{
			pFoilA = pMainFrame->GetFoil(m_pWing->m_RFoil[i-1]);
			pFoilB = pMainFrame->GetFoil(m_pWing->m_RFoil[i]);
			if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize)
			{
				//we have a flap, so write it down
				x = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i-1])/2.0;
				y = GetOffset(x/m_pWing->m_Span*2.0);
				strong = QString("Flap %1").arg(nFlap+1);
				painter.drawText(O.x() + (int)(x*m_fWingScale), O.y() + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	else
	{
		for (i=0; i<=m_pWing->m_NPanel;i++)
		{
			x = m_pWing->m_TPos[i];
			y = GetOffset(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			Chord = GetChord(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			if(Chord<0.0 || y<0.0) break;

			if (i==m_iSection)
			{
				painter.setPen(HighPen);
			}
			else
			{
				painter.setPen(FoilPen);
			}
			painter.drawLine(O.x() - (int)(x*m_fWingScale), O.y() + (int)(y*m_fWingScale),
							 O.x() - (int)(x*m_fWingScale), O.y() + (int)((y+Chord)*m_fWingScale));

			strong = m_pWing->m_LFoil[i].left(30);
			painter.drawText(O.x() - (int)(x*m_fWingScale), O.y() + 10+(int)((y+Chord)*m_fWingScale),strong);
		}
		nFlap = 0;

//		for (i=1; i<=m_pWing->m_NPanel; i++){
		for (i=m_pWing->m_NPanel; i>=1; i--)
		{
			pFoilA = pMainFrame->GetFoil(m_pWing->m_LFoil[i-1]);
			pFoilB = pMainFrame->GetFoil(m_pWing->m_LFoil[i]);
			if(pFoilA && pFoilB && pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize)
			{
				//we have a flap, so write it down
				x = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i-1])/2.0;
				y = GetOffset(x/m_pWing->m_Span*2.0);
				strong = QString("Flap %1").arg(nFlap+1);
				painter.drawText(O.x() - (int)(x*m_fWingScale), O.y() + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	painter.restore();
}



void WingDlg::DrawWing(QPainter &painter, QPoint O)
{
	painter.save();
	QPoint P1, P2;
	double x, x1, x2;
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CFoil *pFoil1, *pFoil2;
	QPen BlackPen(Qt::black);
	BlackPen.setWidth(2);
	painter.setPen(BlackPen);

	if(m_bRightSide)
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			painter.drawLine(O.x() +(int)(x1*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i]*m_fWingScale),
							 O.x() +(int)(x2*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale));
			painter.drawLine(O.x() +(int)(x2*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale),
							 O.x() +(int)(x2*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
			painter.drawLine(O.x() +(int)(x2*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale),
							 O.x() +(int)(x1*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
			painter.drawLine(O.x() +(int)(x1*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale),
							 O.x() +(int)(x1*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
		}
	}
	else
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			painter.drawLine(O.x() -(int)(x1*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i]*m_fWingScale),
							 O.x() -(int)(x2*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale));
			painter.drawLine(O.x() -(int)(x2*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale),
							 O.x() -(int)(x2*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
			painter.drawLine(O.x() -(int)(x2*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale),
							 O.x() -(int)(x1*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
			painter.drawLine(O.x() -(int)(x1*m_fWingScale), O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale),
							 O.x() -(int)(x1*m_fWingScale), O.y() +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
		}
	}
	//Draw Flaps
	QPen DotPen(Qt::black);
	painter.setPen(DotPen);

	if(m_bRightSide)
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			pFoil1 = pMainFrame->GetFoil(m_pWing->m_RFoil[i]);
			pFoil2 = pMainFrame->GetFoil(m_pWing->m_RFoil[i+1]);
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			if(pFoil1 && pFoil2 && pFoil1->m_bTEFlap && pFoil2->m_bTEFlap)
			{
				if(pFoil1->m_bTEFlap)
				{
					P1.rx() = O.x() +(int)(x1*m_fWingScale);
					P1.ry() = O.y() +(int)((m_pWing->m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_pWing->m_TChord[i])*m_fWingScale);
				}
				else
				{
					P1.rx() = O.x() +(int)(x1*m_fWingScale);
					P1.ry() = O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale);
				}
				if(pFoil2->m_bTEFlap)
				{
					P2.rx() = O.x() +(int)(x2*m_fWingScale);
					P2.ry() = O.y() +(int)((m_pWing->m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_pWing->m_TChord[i+1])*m_fWingScale);
				}
				else
				{
					P2.rx() = O.x() +(int)(x2*m_fWingScale);
					P2.ry() = O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale);
				}
			}
		}
	}
	else
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			pFoil1 = pMainFrame->GetFoil(m_pWing->m_LFoil[i]);
			pFoil2 = pMainFrame->GetFoil(m_pWing->m_LFoil[i+1]);
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			if(pFoil1 && pFoil2 && pFoil1->m_bTEFlap && pFoil2->m_bTEFlap)
			{
				if(pFoil1->m_bTEFlap)
				{
					P1.rx() = O.x() -(int)(x1*m_fWingScale);
					P1.ry() = O.y() +(int)((m_pWing->m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_pWing->m_TChord[i])*m_fWingScale);

				}
				else
				{
					P1.rx() = O.x() -(int)(x1*m_fWingScale);
					P1.ry() = O.y() +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale);
				}
				if(pFoil2->m_bTEFlap)
				{
					P2.rx() = O.x() -(int)(x2*m_fWingScale);
					P2.ry() = O.y() +(int)((m_pWing->m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_pWing->m_TChord[i+1])*m_fWingScale);
				}
				else
				{
					P2.rx() = O.x() -(int)(x2*m_fWingScale);
					P2.ry() = O.y() +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale);
				}
				painter.drawLine(P1, P2);
			}
		}

	}
	//find mean aero chord, and draw it
	if (m_yMAC>0.0)
	{
		QPen GreenPen(QColor(0,170,0));
		painter.setPen(GreenPen);

		if(m_bRightSide)
		{
			x = GetOffset(2.0*m_yMAC/m_pWing->m_Span);
			painter.drawLine(O.x() + (int)(m_yMAC*m_fWingScale),      O.y() + (int)(x*m_fWingScale),
							 O.x() + (int)(m_yMAC*m_fWingScale),      O.y() + (int)((x+m_pWing->m_MAChord)*m_fWingScale));
			painter.drawText(O.x() - 10 + (int)(m_yMAC*m_fWingScale), O.y() - 20 +(int)(x*m_fWingScale),"m.a.c.");
		}
		else
		{
			x = GetOffset(2.0*m_yMAC/m_pWing->m_Span);
			painter.drawLine(O.x() - (int)(m_yMAC*m_fWingScale),    O.y() + (int)(x*m_fWingScale),
							 O.x() - (int)(m_yMAC*m_fWingScale),    O.y() + (int)((x+m_pWing->m_MAChord)*m_fWingScale));
			painter.drawText(O.x() -10- (int)(m_yMAC*m_fWingScale), O.y() -20+(int)(x*m_fWingScale), "m.a.c.");
		}
	}
	painter.restore();
}


void WingDlg::DrawVLMMesh(QPainter &painter, QPoint O)
{
	painter.save();
	double chord, offset, yl;
	double offset1, offset2, ch1, ch2, x1, x2, y;
	double dT,dk,dNY;
	double xPoint1[MAXCHORDPANELS+1],xPoint2[MAXCHORDPANELS+1];
	int i,k,l;
	int NXLead, NXFlap;

	CFoil *pFoil1, *pFoil2;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QPen MeshPen(QColor(80, 80, 255));
	painter.setPen(MeshPen);


	if(m_bRightSide)
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			//first draw lines parallel to chord,
			dT  = m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i];
			dNY = (double)m_pWing->m_NYPanels[i];
			for (k=1;k<m_pWing->m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_pWing->m_YPanelDist[i] == 1)
				{
					//cosine distribution
					y = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i+1])/2.0;
					yl = y +cos(k*pi/m_pWing->m_NYPanels[i]) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/2.0;
				}
				else if(m_pWing->m_YPanelDist[i] ==  2)
				{
					//sine distribution
					yl = m_pWing->m_TPos[i+1] -(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else if(m_pWing->m_YPanelDist[i] == -2)
				{
					//-sine distribution
					yl = m_pWing->m_TPos[i] +(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else
				{
					//even spacing by default
					yl = m_pWing->m_TPos[i] +(k*(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_NYPanels[i]);
				}
				offset = GetOffset(yl*2.0/m_pWing->m_Span);
				chord  = GetChord( yl*2.0/m_pWing->m_Span);
				painter.drawLine(O.x() +(int)(yl*m_fWingScale), O.y() +(int)(offset*m_fWingScale),
								 O.x() +(int)(yl*m_fWingScale), O.y() +(int)((offset+chord)*m_fWingScale));
			}
			//next draw lines parallel to span,

			offset1 = m_pWing->m_TOffset[i];
			offset2 = m_pWing->m_TOffset[i+1];
			ch1     = m_pWing->m_TChord[i];
			ch2     = m_pWing->m_TChord[i+1];
			pFoil1  = pMainFrame->GetFoil(m_pWing->m_RFoil[i]);
			pFoil2  = pMainFrame->GetFoil(m_pWing->m_RFoil[i+1]);

			CreateXPoints(m_pWing->m_NXPanels[i], m_pWing->m_XPanelDist[i], pFoil1, pFoil2, xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_pWing->m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				painter.drawLine(O.x() +(int)(m_pWing->m_TPos[i]*m_fWingScale),   O.y() +(int)(x1*m_fWingScale),
								 O.x() +(int)(m_pWing->m_TPos[i+1]*m_fWingScale), O.y() +(int)(x2*m_fWingScale));
			}
		}
	}
	else
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			//first draw lines parallel to chord,
			dT  = m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i];
			dNY = (double)m_pWing->m_NYPanels[i];
			for (k=1;k<m_pWing->m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_pWing->m_YPanelDist[i] == 1)
				{
					//cosine distribution
					y = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i+1])/2.0;
					yl = y +cos(k*pi/m_pWing->m_NYPanels[i]) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/2.0;
				}
				else if(m_pWing->m_YPanelDist[i] == -2)
				{
					//-sine distribution
					yl = m_pWing->m_TPos[i]   +(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else if(m_pWing->m_YPanelDist[i] == 2)
				{
					// sine distribution
					yl = m_pWing->m_TPos[i+1] -(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else
				{
					//even spacing by default
					yl = m_pWing->m_TPos[i] +(k*(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_NYPanels[i]);
				}
				offset = GetOffset( yl*2.0/m_pWing->m_Span);
				chord  = GetChord(  yl*2.0/m_pWing->m_Span);
				painter.drawLine(O.x() -(int)(yl*m_fWingScale), O.y() +(int)(offset*m_fWingScale),
								 O.x() -(int)(yl*m_fWingScale), O.y() +(int)((offset+chord)*m_fWingScale));
			}

			//next draw lines parallel to span,
			offset1 = m_pWing->m_TOffset[i];
			offset2 = m_pWing->m_TOffset[i+1];
			ch1     = m_pWing->m_TChord[i];
			ch2     = m_pWing->m_TChord[i+1];
			pFoil1  = pMainFrame->GetFoil(m_pWing->m_LFoil[i]);
			pFoil2  = pMainFrame->GetFoil(m_pWing->m_LFoil[i+1]);

			CreateXPoints(m_pWing->m_NXPanels[i], m_pWing->m_XPanelDist[i], pFoil1, pFoil2, xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_pWing->m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				painter.drawLine(O.x() -(int)(m_pWing->m_TPos[i]*m_fWingScale),   O.y() +(int)(x1*m_fWingScale),
								 O.x() -(int)(m_pWing->m_TPos[i+1]*m_fWingScale), O.y() +(int)(x2*m_fWingScale));
			}
		}
	}
	painter.restore();
}



void WingDlg::FillDataTable()
{
	if(!m_pWing) return;
	int i;
//	if(m_pWing->m_bVLMAutoMesh) VLMSetAutoMesh();
	m_pWingModel->setRowCount(m_pWing->m_NPanel+2);

	for(i=0; i<=m_pWing->m_NPanel; i++)
	{
		FillTableRow(i);
	}
}

void WingDlg::FillTableRow(int row)
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

	if(row<m_pWing->m_NPanel)
	{
		ind = m_pWingModel->index(row, 6, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->m_NXPanels[row]);

		if(m_pWing->m_XPanelDist[row]==0)		 strong = "Uniform";
		else if(m_pWing->m_XPanelDist[row]==1) strong = "Cosine";
		ind = m_pWingModel->index(row, 7, QModelIndex());
		m_pWingModel->setData(ind, strong);

		ind = m_pWingModel->index(row, 8, QModelIndex());
		m_pWingModel->setData(ind, m_pWing->m_NYPanels[row]);

		if(m_pWing->m_YPanelDist[row]==0)			strong = "Uniform";
		else if(m_pWing->m_YPanelDist[row]==1)	strong = "Cosine";
		else if(m_pWing->m_YPanelDist[row]==2)	strong = "Sine";
		else if(m_pWing->m_YPanelDist[row]==-2)	strong = "-Sine";
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

double WingDlg::GetChord(double yob)
{
	double Chord = 0.0;
	double tau;
	double y;

	y= fabs(yob*m_pWing->m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_pWing->m_NPanel; i++)
	{
		if(m_pWing->m_TPos[i]<=y && y <=m_pWing->m_TPos[i+1])
		{
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			Chord = m_pWing->m_TChord[i] + tau * (m_pWing->m_TChord[i+1] - m_pWing->m_TChord[i]);
			return Chord;
		}
	}
	return -1.0;
}




double WingDlg::GetOffset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= fabs(yob*m_pWing->m_Span/2.0);
	for (int i=0; i<m_pWing->m_NPanel; i++)
	{
		if(m_pWing->m_TPos[i]<= y && y <=m_pWing->m_TPos[i+1])
		{
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			Offset = m_pWing->m_TOffset[i] + tau * (m_pWing->m_TOffset[i+1] - m_pWing->m_TOffset[i]);
			return Offset;
		}
	}

	return -1.0;
}


double WingDlg::GetZPos(double y)
{
	double tau;
	double ZPos =0.0;

	y= fabs(y);
	if(y<=0.0) return 0.0;
	for (int i=0; i<m_pWing->m_NPanel; i++)
	{
		if(m_pWing->m_TPos[i]< y && y<=m_pWing->m_TPos[i+1])
		{
			for (int k=0; k<i; k++)
			{
				ZPos+=m_pWing->m_TLength[k+1] * sin(m_pWing->m_TDihedral[k]*pi/180.0);
			}
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			ZPos += tau * m_pWing->m_TLength[i+1] * sin(m_pWing->m_TDihedral[i]*pi/180.0);
			return ZPos;
		}
	}
	return 0.0;
}




void WingDlg::InitDialog()
{
	if(!m_pWing) return;

	m_pctrlWingName->setText(m_pWing->m_WingName);
	if(!m_bAcceptName) m_pctrlWingName->setEnabled(false);
	m_pctrlSymetric->setChecked(m_pWing->m_bSymetric);
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

	m_pWingDelegate = new WingDelegate;
	m_pctrlWingTable->setItemDelegate(m_pWingDelegate);
	connect(m_pWingDelegate,  SIGNAL(closeEditor(QWidget *)),         this, SLOT(OnCellChanged(QWidget *)));

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



int WingDlg::IsFoil(QPoint point)
{
	// Returns the sections index where the
	if(m_bRightSide)
	{
		double x = (point.x()-m_ptOffset.x())/m_fWingScale;
		double y = (point.y()-m_ptOffset.y())/m_fWingScale;
		for (int i=0; i<=m_pWing->m_NPanel; i++)
		{
			if((fabs(x-m_pWing->m_TPos[i])/m_pWing->m_Span<0.01) &&
				(y>=m_pWing->m_TOffset[i] && y<=m_pWing->m_TOffset[i]+m_pWing->m_TChord[i]))
				return i;
//				TRACE("returning %d\n",i);
		}
	}
	else
	{
		int Ox = m_ptOffset.x() + (int)(m_pWing->m_TPos[m_pWing->m_NPanel]*m_fWingScale);
		double x = (point.x()-Ox)/m_fWingScale;
		double y = (point.y()-m_ptOffset.y())/m_fWingScale;
		for (int i=0; i<=m_pWing->m_NPanel; i++){
			if((fabs(-x-m_pWing->m_TPos[i])/m_pWing->m_Span<0.01) &&
				(y>=m_pWing->m_TOffset[i] && y<=m_pWing->m_TOffset[i]+m_pWing->m_TChord[i]))
				return i;
		}
	}
	return -1;
}


void WingDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
				return;
			}
			else
			{
				OnOK();
				return;
			}
			break;
		}
		default:
			QDialog::keyPressEvent(event);
	}

	QDialog::keyPressEvent(event);
}


void WingDlg::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	if(m_DrawRect.contains(point))
	{
		int res = IsFoil(point);

		if(res>=0)
		{
			m_iSection = res;
			m_pctrlWingTable->selectRow(res);
			m_pctrlWingTable->setFocus();
			repaint();
			return;
		}
		else setFocus();
		m_PointDown = point;
		m_bTrans = true;
	}
	else
	{
		m_bTrans = false;
		event->ignore();
	}
}


void WingDlg::mouseMoveEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	if(m_bTrans)
	{
		// we translate the wing
		m_ptOffset.rx() += point.x() - m_PointDown.x();
		m_ptOffset.ry() += point.y() - m_PointDown.y();

		m_PointDown = point;
		repaint();
	}
	else if (event->buttons() & Qt::MidButton)
	{
		if(point.y()-m_PointDown.y()>0) m_fWingScale *= 1.06;
		else                            m_fWingScale /= 1.06;
		repaint();

	}
	m_PointDown = point;
}


void WingDlg::mouseReleaseEvent(QMouseEvent *event)
{
	m_bTrans = false;
}


void WingDlg::OnCellChanged(QWidget *pWidget)
{
/*	if(m_iSection>m_pWing->m_NPanel)
	{
		//the user has filled a cell in the last line
		//so add an item before reading
		m_pWing->m_NPanel++;
		m_pWingModel->setRowCount(m_pWing->m_NPanel+2);
		FillTableRow(m_pWing->m_NPanel);
	}*/
	ReadSectionData(m_iSection);
	
	repaint();
}


void WingDlg::OnDeleteSection()
{
	if(m_iSection<0) return;

	if(m_iSection==0)
	{
		QMessageBox::warning(this, "QFLR5","The first section cannot be deleted");
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
	repaint();
}



void WingDlg::OnInsertBefore()
{
	if (m_pWing->m_NPanel==MAXPANELS)
	{
		QMessageBox::warning(this, "QFLR5", "The maximum number of panels has been reached");
		return;
	}
	if(m_iSection<=0)
	{
		QMessageBox::warning(this, "QFLR5", "No insertion possible before the first section");
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
	repaint();
}




void WingDlg::OnInsertAfter()
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

		m_pWing->m_TPos[k]      = m_pWing->m_TPos[k-1];
		m_pWing->m_TChord[k]    = m_pWing->m_TChord[k-1];
		m_pWing->m_TOffset[k]   = m_pWing->m_TOffset[k-1];
		m_pWing->m_TTwist[k]     = m_pWing->m_TTwist[k-1];
		m_pWing->m_TDihedral[k]  = m_pWing->m_TDihedral[k-1];
		m_pWing->m_NXPanels[k]   = m_pWing->m_NXPanels[k-1];
		m_pWing->m_NYPanels[k]   = m_pWing->m_NYPanels[k-1];
		m_pWing->m_XPanelDist[k] = m_pWing->m_XPanelDist[k-1];
		m_pWing->m_YPanelDist[k] = m_pWing->m_YPanelDist[k-1];
		m_pWing->m_RFoil[k]      = m_pWing->m_RFoil[k-1];
	}

	if(n+1<m_pWing->m_NPanel)
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
	repaint();
}



void WingDlg::OnItemActivated(const QModelIndex &index)
{
	m_iSection = index.row();
	repaint();
}

void WingDlg::OnItemClicked(const QModelIndex &index)
{
	if(index.row()>m_pWing->m_NPanel)
	{
		//the user has filled a cell in the last line
		//so add an item before reading
		m_pWing->m_NPanel++;
		m_pWingModel->setRowCount(m_pWing->m_NPanel+2);
		FillTableRow(m_pWing->m_NPanel);
	}
	m_iSection = index.row();
	repaint();
}



void WingDlg::OnDelete()
{
	if(m_iSection<=0)
	{
		QMessageBox::warning(this, "QFLR5","The first section cannot be deleted");
		return;
	}

	int ny, k, size, total;

	size = m_pWingModel->rowCount();
	if(size<=2)
	{
		QMessageBox::warning(this, "QFLR5","Two panel sections at least are required... cannot delete");
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




void WingDlg::OnOK()
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

void WingDlg::OnSide()
{
	m_bRightSide = m_pctrlRightSide->isChecked();
	FillDataTable();;
	m_bChanged = true;
	repaint();
}


void WingDlg::OnSymetric()
{
	if(m_pctrlSymetric->isChecked())
	{
		m_pWing->m_bSymetric    = true;
		m_bRightSide   = true;
		m_pctrlLeftSide->setEnabled(false);
		m_pctrlRightSide->setChecked(true);
	}
	else
	{
		m_pWing->m_bSymetric    = false;
		m_pctrlLeftSide->setEnabled(true);
	}

	repaint();
	m_bChanged = true;
}

void WingDlg::OnWingColor()
{
	if(!m_pWing) return;
	QColor WingColor = m_pWing->m_WingColor;

	bool bOK;
	QRgb rgb = WingColor.rgba();
	rgb = QColorDialog::getRgba(rgb, &bOK);
	m_pWing->m_WingColor = QColor::fromRgba(rgb);
	m_pctrlWingColor->SetColor(m_pWing->m_WingColor);
}




void WingDlg::paintEvent(QPaintEvent *event)
{
	if(!m_pWing) return;
	QPainter painter(this);

	painter.save();
	QFont Arial8("Arial");
	Arial8.setPointSize(8);
	painter.setFont(Arial8);
	QPoint O = m_ptOffset;

	if(!m_bRightSide) O.rx() += (int)(m_pWing->m_TPos[m_pWing->m_NPanel]*m_fWingScale);

	DrawVLMMesh(painter, O);
	DrawWing(painter, O);
	DrawFoils(painter, O);
	DrawDihedral(painter, O);

	QPen SymPen(Qt::black);
	SymPen.setStyle(Qt::DashDotLine);
	painter.setBackgroundMode(Qt::TransparentMode);
	painter.setPen(SymPen);
	painter.drawLine(O.x(), O.y()-70, O.x(), O.y()+200);

	painter.restore();
}

void WingDlg::ReadParams()
{
	m_pWing->m_WingName = m_pctrlWingName->text();
	ReadSectionData(m_iSection);

	//Update Geometry
//	Convert(false);// retrieve the data
	//
}

void WingDlg::ReadSectionData(int sel)
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
	if(m_bRightSide)	m_pWing->m_RFoil[sel] = strong;
	else                m_pWing->m_LFoil[sel] = strong;

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

	//Update Geometry
//	Convert(false);// retrieve the data
	//
}



void WingDlg::SetScale()
{
	// Sets the wing scale for diplay in the wing edition dialog box

	double sc1 = 2.0*(m_DrawRect.width()-130)/m_pWing->m_Span;
	double sc2 = (m_DrawRect.height()-90)/m_pWing->m_TChord[0];
	double sc3 = (m_DrawRect.height()-90)/(m_pWing->m_TChord[m_pWing->m_NPanel]+m_pWing->m_TOffset[m_pWing->m_NPanel]);
	m_fWingScale = qMin(sc1, sc2);
	m_fWingScale = qMin(m_fWingScale, sc3);
}


void WingDlg::SetupLayout()
{
	QVBoxLayout *DefLayout = new QVBoxLayout;
	m_pctrlWingName     = new QLineEdit("WingName");
	QHBoxLayout *SymLayout = new QHBoxLayout;
	m_pctrlSymetric     = new QCheckBox(tr("Symetric"));
	m_pctrlRightSide    = new QRadioButton(tr("Right Side"));
	m_pctrlLeftSide     = new QRadioButton(tr("Left Side"));
	SymLayout->addWidget(m_pctrlSymetric);
	SymLayout->addWidget(m_pctrlRightSide);
	SymLayout->addWidget(m_pctrlLeftSide);
	QHBoxLayout *ButtonLayout = new QHBoxLayout;
	m_pctrlWingColor    = new ColorButton;
	m_pctrlResetMesh    = new QPushButton(tr("Reset Mesh"));
	ButtonLayout->addStretch(1);
	ButtonLayout->addWidget(m_pctrlWingColor);
	ButtonLayout->addStretch(1);
	ButtonLayout->addWidget(m_pctrlResetMesh);
	ButtonLayout->addStretch(1);

	DefLayout->addWidget(m_pctrlWingName);
	DefLayout->addStretch(1);
	DefLayout->addLayout(SymLayout);
	DefLayout->addStretch(1);
	DefLayout->addLayout(ButtonLayout);
	DefLayout->addStretch(1);
//	DefLayout->addWidget(m_pctrlSymetric);
//	DefLayout->addWidget(m_pctrlRightSide);
//	DefLayout->addWidget(m_pctrlLeftSide);
//	DefLayout->addWidget(m_pctrlWingColor);
//	DefLayout->addWidget(m_pctrlResetMesh);
//	DefLayout->addWidget(ctrl3D);


	m_pctrlLength1 = new QLabel("mm");
	m_pctrlLength2 = new QLabel("mm");
	m_pctrlLength3 = new QLabel("mm");
	m_pctrlLength4 = new QLabel("mm");
	m_pctrlAreaUnit = new QLabel("mm2");
	m_pctrlVolumeUnit = new QLabel("mm3");
	m_pctrlLength1->setAlignment(Qt::AlignLeft);
	m_pctrlLength2->setAlignment(Qt::AlignLeft);
	m_pctrlLength3->setAlignment(Qt::AlignLeft);
	m_pctrlLength4->setAlignment(Qt::AlignLeft);
	m_pctrlAreaUnit->setAlignment(Qt::AlignLeft);
	m_pctrlVolumeUnit->setAlignment(Qt::AlignLeft);

	QGridLayout *DataLayout = new QGridLayout;

	QLabel *lab1 = new QLabel("Wing Span");
	QLabel *lab2 = new QLabel("Area");
	QLabel *lab3 = new QLabel("Volume");
	QLabel *lab4 = new QLabel("Mean Geom. Chord");
	QLabel *lab5 = new QLabel("Mean Aero Chord");
	QLabel *lab6 = new QLabel("Total VLM Panels");
	lab1->setAlignment(Qt::AlignRight);
	lab2->setAlignment(Qt::AlignRight);
	lab3->setAlignment(Qt::AlignRight);
	lab4->setAlignment(Qt::AlignRight);
	lab5->setAlignment(Qt::AlignRight);
	lab6->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(lab1,1,1);
	DataLayout->addWidget(lab2,2,1);
	DataLayout->addWidget(lab3,3,1);
	DataLayout->addWidget(lab4,4,1);
	DataLayout->addWidget(lab5,5,1);
	DataLayout->addWidget(lab6,6,1);
	m_pctrlWingSpan   = new QLabel("2000.00");
	m_pctrlWingArea   = new QLabel("30.0");
	m_pctrlWingVolume = new QLabel("000.0e+03");
	m_pctrlMAC        = new QLabel("150.0");
	m_pctrlGeomChord  = new QLabel("170.0");
	m_pctrlVLMPanels  = new QLabel("500");
	m_pctrlWingSpan->setAlignment(Qt::AlignRight);
	m_pctrlWingArea->setAlignment(Qt::AlignRight);
	m_pctrlWingVolume->setAlignment(Qt::AlignRight);
	m_pctrlMAC->setAlignment(Qt::AlignRight);
	m_pctrlGeomChord->setAlignment(Qt::AlignRight);
	m_pctrlVLMPanels->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(m_pctrlWingSpan,   1,2);
	DataLayout->addWidget(m_pctrlWingArea,   2,2);
	DataLayout->addWidget(m_pctrlWingVolume, 3,2);
	DataLayout->addWidget(m_pctrlMAC,        4,2);
	DataLayout->addWidget(m_pctrlGeomChord,  5,2);
	DataLayout->addWidget(m_pctrlVLMPanels,  6,2);

	DataLayout->addWidget(m_pctrlLength1,1,3);
	DataLayout->addWidget(m_pctrlAreaUnit,2,3);
	DataLayout->addWidget(m_pctrlVolumeUnit,3,3);
	DataLayout->addWidget(m_pctrlLength2,4,3);
	DataLayout->addWidget(m_pctrlLength3,5,3);
	QLabel *lab13 = new QLabel("Max is 1000");
	lab13->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab13 ,6,3);

	QLabel *lab7 = new QLabel("MAC Span Pos");
	QLabel *lab8 = new QLabel("Aspect ratio");
	QLabel *lab9 = new QLabel("Taper Ratio");
	QLabel *lab10 = new QLabel("Root to Tip Sweep");
	QLabel *lab11 = new QLabel("Number of Flaps");
	QLabel *lab12 = new QLabel("Number of 3D Panels");
	lab7->setAlignment(Qt::AlignRight);
	lab8->setAlignment(Qt::AlignRight);
	lab9->setAlignment(Qt::AlignRight);
	lab10->setAlignment(Qt::AlignRight);
	lab11->setAlignment(Qt::AlignRight);
	lab12->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(lab7,1,5);
	DataLayout->addWidget(lab8,2,5);
	DataLayout->addWidget(lab9,3,5);
	DataLayout->addWidget(lab10,4,5);
	DataLayout->addWidget(lab11,5,5);
	DataLayout->addWidget(lab12,6,5);
	m_pctrlMACSpanPos   = new QLabel("466.00");
	m_pctrlAspectRatio   = new QLabel("13.33");
	m_pctrlTaperRatio = new QLabel("1.50");
	m_pctrlSweep        = new QLabel("2.58");
	m_pctrlNFlaps  = new QLabel("0");
	m_pctrl3DPanels  = new QLabel("1000");
	m_pctrlMACSpanPos->setAlignment(Qt::AlignRight);
	m_pctrlAspectRatio->setAlignment(Qt::AlignRight);
	m_pctrlTaperRatio->setAlignment(Qt::AlignRight);
	m_pctrlSweep->setAlignment(Qt::AlignRight);
	m_pctrlNFlaps->setAlignment(Qt::AlignRight);
	m_pctrl3DPanels->setAlignment(Qt::AlignRight);
	DataLayout->addWidget(m_pctrlMACSpanPos,  1,6);
	DataLayout->addWidget(m_pctrlAspectRatio, 2,6);
	DataLayout->addWidget(m_pctrlTaperRatio,  3,6);
	DataLayout->addWidget(m_pctrlSweep,       4,6);
	DataLayout->addWidget(m_pctrlNFlaps,      5,6);
	DataLayout->addWidget(m_pctrl3DPanels,    6,6);
	DataLayout->addWidget(m_pctrlLength4, 1, 7);
	QLabel *lab14 = new QLabel("deg");
	lab14->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab14, 4, 7);
	QLabel *lab15 = new QLabel("(Max is 2000)");
	lab15->setAlignment(Qt::AlignLeft);
	DataLayout->addWidget(lab15, 6, 7);

	m_pctrlWingTable = new QTableView(this);
	m_pctrlWingTable->setMinimumWidth(800);
	m_pctrlWingTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlWingTable->setSelectionBehavior(QAbstractItemView::SelectRows);//we only need to select panel sections
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
//	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlWingTable->setSizePolicy(szPolicyExpanding);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *TopLayout = new QHBoxLayout;
	TopLayout->addLayout(DefLayout);
//	TopLayout->addStretch(1);
	TopLayout->addLayout(DataLayout);
	QVBoxLayout *MainLayout = new QVBoxLayout(this);
	MainLayout->addLayout(TopLayout);
//	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlWingTable);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);
}


void WingDlg::SetWingData()
{
	if(!m_pWing) return;
	//Updates the wing's properties after a change of geometry

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

	str = QString("%1").arg(m_pWing->m_Area*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlWingArea->setText(str);

	str = QString("%1").arg(m_pWing->m_Volume*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit,5,'e',2);
	m_pctrlWingVolume->setText(str);

	str = QString("%1").arg(m_pWing->m_Span*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlWingSpan->setText(str);

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


void WingDlg::showEvent(QShowEvent *event)
{
	ComputeGeometry();
	QRect CltRect = geometry();

	m_ptOffset = QPoint(50, CltRect.height() * .70);
	m_DrawRect = QRect(0,0,geometry().width(), geometry().height());
	m_DrawRect.adjust(0, 280, 0, -20);
	SetScale();
	setFocus();
}



int WingDlg::VLMGetPanelTotal()
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


void WingDlg::wheelEvent (QWheelEvent *event )
{
	if(event->delta()<0) m_fWingScale *= 1.06;
	else                 m_fWingScale /= 1.06;

	repaint();
}









