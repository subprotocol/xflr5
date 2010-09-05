/****************************************************************************

	VLMAnalysisDlg Class
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


#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <math.h>
#include "VLMAnalysisDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/CVector.h"
#include "Miarex.h"
#include <QtDebug>

#define CM_ITER_MAX 50

void *VLMAnalysisDlg::s_pMiarex;
void *VLMAnalysisDlg::s_pMainFrame;

VLMAnalysisDlg::VLMAnalysisDlg()
{
	setWindowTitle(tr("VLM Analysis"));

	SetupLayout();

	m_bSequence      = false;
	m_bWarning       = false;
	m_bXFile         = false;
	m_bVLMSymetric   = true;
	m_bWakeRollUp    = false;
	m_bConverged     = false;
	m_bPointOut      = false;
	m_bCancel        = false;
	m_bTrefftz       = false;
	m_bIsFinished    = false;
	m_bTrace         = true;

	m_MatSize        = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_MaxWakeIter    = 5;
	m_WakeInterNodes = 6;
	m_NWakeColumn    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	m_Ctrl = 0.0;

	m_AlphaMin    = m_AlphaMax   = m_AlphaDelta    = 0.0;
	m_QInfMin     = m_QInfMax    = m_QInfDelta     = 0.0;
	m_ControlMin  = m_ControlMax = m_ControlDelta  = 0.0;

	m_OpQInf   = 0.0;//the current speed;
	m_OpAlpha  = 0.0;//calculation on a tilted geometry

	m_XCP = 0.0;
	m_YCP = 0.0;
	m_VCm = 0.0;
	m_GCm = 0.0;
	m_GCm = 0.0;
	m_CL  = 0.0;
	m_ViscousDrag = 0.0;
	m_InducedDrag = 0.0;
	m_IYm = 0.0;
	m_GYm = 0.0;

	m_strOut = "";

	m_pWingList[0] = m_pWingList[1] = m_pWingList[2] = m_pWingList[3] = NULL;
	m_pWing   = NULL;
	m_pWing2  = NULL;
	m_pStab   = NULL;
	m_pFin    = NULL;
	m_pWPolar = NULL;
	m_pPlane  = NULL;

	m_ppPanel       = NULL;
	m_pPanel        = NULL;
	m_pWakePanel    = NULL;
	m_pRefWakePanel = NULL;
	m_pMemPanel     = NULL;
	m_pNode         = NULL;
	m_pMemNode      = NULL;
	m_pWakeNode     = NULL;
	m_pRefWakeNode  = NULL;
	m_pTempWakeNode = NULL;

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_VLMQInf,0,sizeof(m_VLMQInf));
	memset(m_zRHS, 0, sizeof(m_zRHS));
	memset(m_xRHS, 0, sizeof(m_xRHS));
}




void VLMAnalysisDlg::AddString(QString strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}



bool VLMAnalysisDlg::AlphaLoop()
{
	QString str;
	int nrhs;

	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -fabs(m_AlphaDelta);
	nrhs  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
//		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
//	m_bTrace = true;
	str = tr("   Solving the problem... \n\n");
	AddString(str);

	if(!VLMCreateUnitRHS())
	{
		AddString(tr("\n\nFailed to create RHS....\n"));

		m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if (!VLMCreateMatrix())
	{
		AddString(tr("\nFailed to create the matrix....\n"));
		m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!VLMSolveDouble())
	{
		AddString(tr("\n\nSingular matrix - aborting....\n"));
		m_bWarning = true;
		return true;
	}
	VLMSolveMultiple(m_AlphaMin, m_AlphaDelta, nrhs);

	AddString("\n");

	if (m_bCancel) return true;

	VLMComputePlane(m_AlphaMin, m_AlphaDelta, nrhs);

	if (m_bCancel) return true;
	return true;
}


bool VLMAnalysisDlg::SetControlPositions(double t)
{
	int j;
	int nCtrl = 0;
	double angle;
	QString str;
	CVector H(0.0, 1.0, 0.0);

	Quaternion Quat;

	if(m_pWPolar->m_bActiveControl[0])
	{
		//XCmRef
		m_pWPolar->m_CoG.x = m_pWPolar->m_MinControl[0] + t * (m_pWPolar->m_MaxControl[0] - m_pWPolar->m_MinControl[0]);
	}
	else m_pWPolar->m_CoG.x = m_pWPolar->m_MinControl[0];

	nCtrl = 1;

	if(m_pPlane)
	{
		//wing incidence
		if(m_pWPolar->m_bActiveControl[1])
		{
			//wing tilt
			angle = m_pWPolar->m_MinControl[1] + t * (m_pWPolar->m_MaxControl[1] - m_pWPolar->m_MinControl[1]);
			angle -= m_pPlane->m_WingTilt;
			Quat.Set(angle, H);

			for(j=0; j<m_pWing->m_MatSize; j++)
			{
				(m_pWing->m_pPanel+j)->Rotate(m_pPlane->m_LEWing, Quat, angle);
			}
		}
		nCtrl=2;

		if(m_pStab)
		{
			//elevator incidence
			if(m_pWPolar->m_bActiveControl[2] )
			{
				//Elevator tilt
				angle = m_pWPolar->m_MinControl[2] + t * (m_pWPolar->m_MaxControl[2] - m_pWPolar->m_MinControl[2]);
				angle -= m_pPlane->m_StabTilt;

				Quat.Set(angle, H);

				for(j=0; j<m_pStab->m_MatSize; j++)
				{
					(m_pStab->m_pPanel+j)->Rotate(m_pPlane->m_LEStab, Quat, angle);
				}
			}
			nCtrl = 3;
		}
	}

	// flap controls
	//wing first
	for (j=0; j<m_pWing->m_NSurfaces; j++)
	{
		if(m_pWing->m_Surface[j].m_bTEFlap)
		{
			if(m_pWPolar->m_bActiveControl[nCtrl])
			{
				angle = m_pWPolar->m_MinControl[nCtrl] + t * (m_pWPolar->m_MaxControl[nCtrl] - m_pWPolar->m_MinControl[nCtrl]);
				if(!m_pWing->m_Surface[j].RotateFlap(angle))  return false;
			}
			nCtrl++;
		}
	}
	//elevator next and last
	if(m_pStab)
	{
		for (j=0; j<m_pStab->m_NSurfaces; j++)
		{
			if(m_pStab->m_Surface[j].m_bTEFlap)
			{
				if(m_pWPolar->m_bActiveControl[nCtrl])
				{
					angle = m_pWPolar->m_MinControl[nCtrl] + t * (m_pWPolar->m_MaxControl[nCtrl] - m_pWPolar->m_MinControl[nCtrl]);
					if(!m_pStab->m_Surface[j].RotateFlap(angle)) return false;
				}
				nCtrl++;
			}
		}
	}
	str = QString(tr("      ...Control = %1\n")).arg(t,8,'f',2);
	AddString(str);
	return true;
}



bool VLMAnalysisDlg::ControlLoop()
{
	int i, nrhs, iter;
	QString str;
	double eps = 1.e-7;
	double t;
	double a, a0, a1, Cm, Cm0, Cm1, tmp;
	CWing *pWing, *pStab;

	CVector H(0.0, 1.0, 0.0);
	CVector O(0.0, 0.0, 0.0);

	pWing = m_pWing;
	pStab = m_pStab;

	if(m_ControlMax<m_ControlMin) m_ControlDelta = -fabs(m_ControlDelta);

	nrhs  = (int)fabs((m_ControlMax-m_ControlMin)*1.0001/m_ControlDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this,tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

//	Loop for each control value
//     Update the geometry, design variables
//     Get solutions for alpha = -PI/4 and alpha = +PI/4
//     Interpolate to find alpha_0 such that Cm=0
//	   Compute plane for alpha_0
//	end loop
//

	m_bTrace = true;
	m_AlphaMin  = 0.0;

	str = QString(tr("   Solving the problem... \n"));
	AddString(str);

	m_VLMQInf[0] = 1.0;

	for (i=0; i<nrhs; i++)
	{
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));

		t = m_ControlMin +(double)i *m_ControlDelta;
		m_Ctrl = t;

		//update the variables & geometry
		// if plane : WingTilt, elevator Tilt
		// if flaps : wing flaps, elevator flaps
		SetControlPositions(t);

		if (m_bCancel) break;

		m_OpAlpha = 0.0;

		if(!VLMCreateUnitRHS())
		{
			AddString(tr("\n\nFailed to create RHS....\n"));
			m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!VLMCreateMatrix())
		{
			AddString(tr("\nFailed to create the matrix....\n"));
			m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!VLMSolveDouble())	//solve for sine and cos
		{
			AddString(tr("\n\nSingular matrix - aborting....\n"));
			m_bWarning = true;
			return true;
		}
		//start loop to find zero-pitching-moment alpha

		iter = 0;
		a0 = -PI/4.0;
		a1 =  PI/4.0;
		a = 0.0;
		Cm0 = VLMComputeCm(a0*180.0/PI);
		Cm1 = VLMComputeCm(a1*180.0/PI);

		Cm = 1.0;

		//are there two intial values of opposite signs ?
		while(Cm0*Cm1>0.0 && iter <CM_ITER_MAX)
		{
			a0 *=0.9;
			a1 *=0.9;
			Cm0 = VLMComputeCm(a0*180.0/PI);
			Cm1 = VLMComputeCm(a1*180.0/PI);
			iter++;
			qApp->processEvents();
			if(m_bCancel) break;
		}
		if(m_bCancel || m_bCancel) break;
		if(iter>=CM_ITER_MAX)
		{
			//no zero moment alpha
			str = QString(tr("      Interpolation unsuccessful for Control=%1 - skipping.\n\n\n")).arg(t,10,'f',3);
			AddString(str);
			m_bWarning = true;
		}
		else
		{
			//Cm0 and Cm1 are of opposite sign
			if(Cm0>Cm1)
			{
				tmp = Cm1;
				Cm1 = Cm0;
				Cm0 = tmp;
				tmp = a0;
				a0 = a1; 
				a1 = tmp;
			}
			
			iter = 0;
			while (fabs(Cm)>eps && iter<CM_ITER_MAX)
			{
				a = a0 - (a1-a0) * Cm0/(Cm1-Cm0);
				Cm = VLMComputeCm(a*180.0/PI);
				if(Cm>0.0)
				{
					a1  = a;
					Cm1 = Cm;
				}
				else
				{
					a0  = a;
					Cm0 = Cm;
				}
				iter++;
			}

			if (m_bCancel) break;


			Cm = VLMComputeCm(a*180.0/PI);
			if(fabs(Cm)<eps && iter<100)
			{
				VLMSolveMultiple(a*180.0/PI, 0.0, 1);
				VLMComputePlane(a*180.0/PI, m_ControlDelta, 1);
			}
			else
			{
				//no zero moment alpha
				str = QString(tr("      Interpolation unsuccessful for Control=%1 - skipping.\n\n\n")).arg(t,10,'f',3);
				AddString(str);
				m_bWarning = true;
			}
		}
		if(m_bCancel) break;
	}

	return true;
}



void VLMAnalysisDlg::GetSpeedVector(CVector const &C, double *Gamma, CVector &VTot)
{
	int pp;
	
	VTot.x = VTot.y = VTot.z = 0.0;

	for (pp=0; pp<m_MatSize;pp++)
	{
		VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
		VTot.x += V.x * Gamma[pp];
		VTot.y += V.y * Gamma[pp];
		VTot.z += V.z * Gamma[pp];
	}
}



void VLMAnalysisDlg::InitDialog()
{
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_Type==5|| m_pWPolar->m_Type==6))
	{
		//back-up the current geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
	}

	m_pctrlTextOutput->clear();
	memcpy(m_pRefWakeNode, m_pWakeNode, sizeof(m_pRefWakeNode));
	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;
	m_bWakeRollUp    = false;

	SetFileHeader();

	QString str = "";

	m_bVLMSymetric = m_pWing->m_bSymetric;

	if(fabs(m_pWPolar->m_Beta)>0.001)
	{
		str += tr("     Sideslip is asymmetric\n");
		m_bVLMSymetric = false;
	}

	if(!m_pWing->m_bSymetric) str += tr("     Main wing is asymmetric\n");

	if(m_pWing2)
	{
		if(!m_pWing2->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += tr("     2nd wing is asymmetric\n");
		}
	}
	if(m_pStab)
	{
		if(!m_pStab->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += tr("     Elevator is asymmetric\n");
		}
	}

	if(m_pFin)
	{
		m_bVLMSymetric = false;
		str += tr("     A fin is considered asymmetric\n");
	}

	if (m_bVLMSymetric) AddString(tr("Perfoming symmetric calculation\n"));
	else
	{
		str = tr("Performing asymmetric calculation : \n") + str;
		AddString(str);
	}
	AddString("\n");

	CWing::s_bVLMSymetric = m_bVLMSymetric;
}


void VLMAnalysisDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			OnCancelAnalysis();
			event->accept();
			return;
		}
		default:
			event->ignore();
	}
}


void VLMAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
	m_bCancel = true;
	if(m_bIsFinished) done(1);
	WriteString(tr("Analysis Cancelled\n"));
}


void VLMAnalysisDlg::pgmat(double const &mach, double const &alfa, double const &beta, double pg[3][3])
{
//-------------------------------------------------------
//     Calculates the Prandtl-Glauert transformation matrix.
//
//      xi      [       ] x
//              [       ]
//      eta  =  [   P   ] y
//              [       ]
//      zeta    [       ] z
//
//-------------------------------------------------------
//
	double binv, bi_m, sina, sinb, cosa, cosb;
	binv = 1.0 / sqrt(1.0 - mach*mach);
	bi_m = mach * binv*binv*binv;

	sina = sin(alfa);
	cosa = cos(alfa);

	sinb = sin(beta);
	cosb = cos(beta);

	pg[0][0] =  cosa*cosb*binv;
	pg[0][1] =      -sinb*binv;
	pg[0][2] =  sina*cosb*binv;

	pg[1][0] =  cosa*sinb;
	pg[1][1] =       cosb;
	pg[1][2] =  sina*sinb;

	pg[2][0] = -sina;
	pg[2][1] =   0.0;
	pg[2][2] =  cosa;
}

bool VLMAnalysisDlg::ReLoop()
{
	int nrhs;
	double Alpha = 0.0;

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	if(m_QInfMax<m_QInfMin) m_QInfDelta = -fabs(m_QInfDelta);
	nrhs  = (int)fabs((m_QInfMax-m_QInfMin)*1.0001/m_QInfDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"), tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}
	m_bTrace = true;

	if(m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(m_pPanel,     m_pMemPanel,     m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,      m_pMemNode,      m_nNodes  * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		pMiarex->RotateGeomY(m_pWPolar->m_ASpec, O);
		m_OpAlpha = m_pWPolar->m_ASpec;
		Alpha = 0.0;
	}
	else
	{
		Alpha = m_AlphaMin;
	}

	if(!VLMCreateUnitRHS())
	{
		AddString(tr("\n\nFailed to create RHS....\n"));
		m_bWarning = true;
		return true;
	}
	if(!VLMCreateMatrix())
	{
		AddString(tr("\n\nFailed to create matrix....\n"));
		m_bWarning = true;
		return true;
	}

//first solve for unit speed... calculation is linear
	if (!VLMSolveDouble())
	{
		AddString(tr("\n\nSingular matrix - aborting....\n"));
		m_bWarning = true;
		return true;
	}
	VLMSolveMultiple(m_QInfMin, m_QInfDelta, nrhs);

//then compute all points
	VLMComputePlane(m_QInfMin, m_QInfDelta, nrhs);

	return true;
}


void VLMAnalysisDlg::SetFileHeader()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	out << m_pWing->m_WingName;
	out << "\n";
	if(pMiarex && pMiarex->m_pCurWPolar)
	{
//		out << pMiarex->m_pCurWPolar->m_WPlrName;
//		out << "\n";
	}

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";

}


void VLMAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/2);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit;
	QFont CourierFont = QFont("Courier",10);
	CourierFont.setStyleHint(QFont::Courier);
	m_pctrlTextOutput->setFont(CourierFont);
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pctrlCancel = new QPushButton(tr("Cancel"));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(m_pctrlCancel);
	buttonsLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
}



void VLMAnalysisDlg::StartAnalysis()
{
	if(!m_pWPolar) return;
	QString strong;
	m_bIsFinished = false;
	m_pctrlCancel->setText(tr("Cancel"));
	strong = tr("Launching VLM Analysis....\n\n");
	AddString(strong);

	strong = QString(tr("Type %1 Analysis\n\n")).arg(m_pWPolar->m_Type);
	AddString(strong);
	m_bCancel = false;
	
	m_pWingList[0] = m_pWing;
	m_pWingList[1] = m_pWing2;
	m_pWingList[2] = m_pStab;
	m_pWingList[3] = m_pFin;


	if(m_pWPolar->m_Type<3)
	{
		if(m_pWPolar->m_bTiltedGeom) UnitLoop();
		else                         AlphaLoop();
	}
	else 	if(m_pWPolar->m_Type==4) ReLoop();
	else 	if(m_pWPolar->m_Type==5 || m_pWPolar->m_Type==6) ControlLoop();

	if (!m_bCancel && !m_bWarning) strong = tr("\nVLM Analysis completed successfully\n");
	else if (m_bWarning)           strong = tr("\nVLM Analysis completed ... Errors encountered\n");
	AddString(strong);

	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_Type==5|| m_pWPolar->m_Type==6))
	{
		//restore the panels and nodes;
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
	}
	m_bIsFinished = true;
	m_pctrlCancel->setText(tr("Close"));
}



bool VLMAnalysisDlg::UnitLoop()
{
	int i, nrhs;
	QString str;

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	CVector O(0.0,0.0,0.0);

	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -fabs(m_AlphaDelta);

	nrhs  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

	m_bTrace = true;
//	m_AlphaMin = 0.0;

	str = QString(tr("   Solving the problem... \n"));
	AddString(str);

	for (i=0; i<nrhs; i++)
	{
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
		m_OpAlpha = m_AlphaMin+i*m_AlphaDelta;

		pMiarex->RotateGeomY(m_OpAlpha, O);

		str = QString(tr("        ...Alpha = %1\n")).arg(m_OpAlpha,8,'f',2);
		AddString(str);

		if (m_bCancel) break;

		if(!VLMCreateUnitRHS())
		{
			AddString(tr("\n\nFailed to create RHS....\n"));
			m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!VLMCreateMatrix())
		{
			AddString(tr("\nFailed to create the matrix....\n"));
			m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!VLMSolveDouble())
		{
			AddString(tr("\n\nSingular matrix - aborting....\n"));
			m_bWarning = true;
			return true;
		}
		VLMSolveMultiple(0.0, m_AlphaDelta, 1);

		if (m_bCancel) break;

		VLMComputePlane(m_OpAlpha, m_AlphaDelta, 1);
	}
	return true;
}



bool VLMAnalysisDlg::VLMCreateUnitRHS()
{
	int m, Size;

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;
	//______________________________________________________________________________________
	// Create RHS
	//solve twice for sine and cos components

	for (m=0; m<Size; m++)
	{
		m_xRHS[m] = - m_ppPanel[m]->Normal.x;
		m_yRHS[m] = - m_ppPanel[m]->Normal.y;
		m_zRHS[m] = - m_ppPanel[m]->Normal.z;
	}

	return true;
}


bool VLMAnalysisDlg::VLMCreateMatrix()
{
	int p,pp, Size;

	CVector  CC;
	AddString(tr("      Creating the influence matrix...\n"));

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	for(p=0; p<Size; p++)
	{
		qApp->processEvents();

		if(m_bCancel) break;
		CC = m_ppPanel[p]->CtrlPt;
		CC.y = -CC.y;

		for(pp=0; pp<Size; pp++)
		{
			if(m_bCancel) break;
			VLMGetVortexInfluence(m_ppPanel[pp], m_ppPanel[p]->CtrlPt, V, true);

			if(m_bVLMSymetric)
			{
				if(!m_ppPanel[pp]->m_bIsInSymPlane)
				{
					// add right wing contribution
					VLMGetVortexInfluence(m_ppPanel[pp], CC, VS, true);
					V.x += VS.x;
					V.y -= VS.y;
					V.z += VS.z;
				}
			}
			m_aij[p*Size+pp] = V.dot(m_ppPanel[p]->Normal);
		}
	}
	return true;
}


double VLMAnalysisDlg::VLMComputeCm(double alpha)
{
	static double cosa, sina, Cm;
	static CVector V, Force, PanelLeverArm;
	static int p, Size,pUpwards;

	V.Set(cos(alpha*PI/180.0), 0.0, sin(alpha*PI/180.0));
	Cm = 0.0;
	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	for(p=0; p<Size; p++)
	{
		PanelLeverArm = m_ppPanel[p]->VortexPos - m_pWPolar->m_CoG;
		Force = V * m_ppPanel[p]->Vortex * (m_xRHS[p]*cosa + m_zRHS[p]*sina);         //Newtons/rho

		if(!m_pWPolar->m_bVLM1 && !m_ppPanel[p]->m_bIsLeading)
		{
			pUpwards = m_ppPanel[p]->m_iPU;
			Force  -= V  * m_ppPanel[p]->Vortex * (m_xRHS[pUpwards]*cosa + m_zRHS[pUpwards]*sina);       //Newtons/rho
		}

		Force*=m_pWPolar->m_Density;

		Cm += -PanelLeverArm.x * Force.z + PanelLeverArm.z*Force.x; //N.m/rho;
		qApp->processEvents();
	}
	return Cm;
}



void VLMAnalysisDlg::VLMComputePlane(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int q, pos, Station;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	double Lift, IDrag, VDrag, WingIDrag, WingVDrag, XCP, YCP, qdyn;
	double Alpha, cosa, sina;
	QString str, strong, OutString;
	CVector Force, WindNormal, WindDirection, WindSide;

	CWing::s_bTrace   = true;
	CWing::s_bVLM1    = m_pWPolar->m_bVLM1;
	m_pWing->m_pWakeNode  = m_pWakeNode;
	m_pWing->m_pWakePanel = m_pWakePanel;


	for(q=0; q<nrhs; q++)
	{
		qApp->processEvents();
		if(m_bCancel) break;

		Force.Set(0.0, 0.0, 0.0);

		m_pWing->m_bWingOut = false;
		if(m_pWing2) m_pWing2->m_bWingOut = false;
		if(m_pStab)  m_pStab->m_bWingOut  = false;
		if(m_pFin)   m_pFin->m_bWingOut   = false;

		if(m_pWPolar->m_bTiltedGeom)
		{
			Alpha = m_OpAlpha;
			//   Define wind axis
			WindNormal.Set(0, 0.0, 1.0);
			WindDirection.Set(1.0, 0.0, 0.0);
		}
		else
		{
			if(m_pWPolar->m_Type!=4)      Alpha = V0+q*VDelta;
			else if(m_pWPolar->m_Type==4) Alpha = m_pWPolar->m_ASpec;

			m_OpAlpha = Alpha;

			//   Define wind axis
			cosa = cos(Alpha*PI/180.0);
			sina = sin(Alpha*PI/180.0);
			WindNormal.Set(-sina, 0.0, cosa);
			WindDirection.Set(cosa, 0.0, sina);
		}

		WindSide = WindNormal * WindDirection;

		m_OpQInf = m_VLMQInf[q];
		qdyn = 0.5 * m_pWPolar->m_Density * m_OpQInf * m_OpQInf;

		if(m_OpQInf >0.0/* || m_pWPolar->m_Type==5 || m_pWPolar->m_Type==6*/)
		{
			if(m_pWPolar->m_Type!=4 && !m_pWPolar->m_bTiltedGeom)
			{
				str = QString(tr("      ...Alpha=%1\n")).arg(m_OpAlpha,0,'f',2);
				AddString(str);
			}
			else if (m_pWPolar->m_Type==4)
			{
				str = QString(tr("   ...QInf = %1 ")).arg(m_OpQInf*pMainFrame->m_mstoUnit,6,'f',2);
				GetSpeedUnit(strong, pMainFrame->m_SpeedUnit);
				str += strong + "\n";
				AddString(str);
			}

			AddString(tr("        Calculating aerodynamic coefficients...\n"));
			m_bPointOut          = false;
			CWing::s_Alpha     = Alpha;
			CWing::s_QInf      = m_OpQInf;
			CWing::s_Viscosity = m_pWPolar->m_Viscosity;
			CWing::s_Density   = m_pWPolar->m_Density;

			Lift   = 0.0;
			IDrag  = 0.0;
			VDrag  = 0.0;
			XCP    = 0.0;
			YCP    = 0.0;

			m_GRm                 = 0.0;
			m_GCm = m_VCm = m_ICm = 0.0;
			m_GYm = m_VYm = m_IYm = 0.0;
			
			pos = Station = 0;
			IDrag = 0.0;
			VDrag = 0.0;

			for(int i=0; i<4; i++)
			{
				if(m_pWingList[i])
				{
					AddString(tr("         Calculating wing...")+m_pWingList[i]->m_WingName+"\n");

					m_pWingList[i]->VLMTrefftz(m_Gamma+q*m_MatSize, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
					IDrag += WingIDrag;

					m_pWingList[i]->PanelComputeViscous(m_OpQInf, WingVDrag, OutString);
					VDrag += WingVDrag;
					if(m_pWingList[i]->m_bWingOut)  m_bPointOut = true;
					AddString(OutString);

					m_pWingList[i]->PanelComputeOnBody(m_OpQInf, Alpha, m_Cp+pos, m_Gamma+q*m_MatSize+pos, XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm, m_pWPolar->m_bViscous, true);

					m_pWingList[i]->PanelSetBending(true);

					pos     += m_pWingList[i]->m_MatSize;
					Station += m_pWingList[i]->m_NStation;
				}
			}

			m_CL          =  2.0*Force.dot(WindNormal)    /m_OpQInf/m_OpQInf/m_pWPolar->m_WArea;
			m_CX          =  2.0*Force.dot(WindDirection) /m_OpQInf/m_OpQInf/m_pWPolar->m_WArea;
			m_CY          =  2.0*Force.dot(WindSide)      /m_OpQInf/m_OpQInf/m_pWPolar->m_WArea;
			m_InducedDrag =  1.0*IDrag                    /m_OpQInf/m_OpQInf/m_pWPolar->m_WArea;

			m_ViscousDrag =  VDrag/m_pWPolar->m_WArea;

			m_XCP         = XCP/Force.dot(WindNormal)/m_pWPolar->m_Density*qdyn;
			m_YCP         = YCP/Force.dot(WindNormal)/m_pWPolar->m_Density*qdyn;

			m_GCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
			m_VCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
			m_ICm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;

			m_GRm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

			m_GYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
			m_VYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
			m_IYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

			VLMSetDownwash(m_Gamma+q*m_MatSize);

			if(m_bPointOut) m_bWarning = true;
			if(m_bConverged)
			{
				if(m_pPlane) pMiarex->AddPOpp(m_bPointOut, m_Cp, m_Gamma+q*m_MatSize);
				else         pMiarex->AddWOpp(m_bPointOut, m_Gamma+q*m_MatSize);
			}
			AddString("     \n");
		}
		else m_bPointOut = true;
	}
}


void VLMAnalysisDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	static int lw, pw, p;
	p = pPanel->m_iElement;

	V.x = V.y = V.z = 0.0;

	if(m_pWPolar->m_bVLM1)
	{
		//just get the horseshoe vortex's influence
		VLMCmn(pPanel->A, pPanel->B, C, V, bAll);
		if(m_pWPolar->m_bGround)
		{
			AA = pPanel->A;
			BB = pPanel->B;
			AA.z = -AA.z - 2.0*m_pWPolar->m_Height;//correction in V4.0
			BB.z = -BB.z - 2.0*m_pWPolar->m_Height;
			VLMCmn(AA, BB, C, VG, bAll);
			V.x += VG.x;
			V.y += VG.y;
			V.z -= VG.z;
		}
	}
	else
	{
		// we have quad vortices
		// so we follow Katz and Plotkin's lead
		if(!pPanel->m_bIsTrailing)
		{
			if(bAll)
			{
				VLMQmn(pPanel->A, pPanel->B, m_pPanel[p-1].A, m_pPanel[p-1].B, C, V);
				if(m_pWPolar->m_bGround)
				{
					AA  = pPanel->A;
					BB  = pPanel->B;
					AA1 = m_pPanel[p-1].A;
					BB1 = m_pPanel[p-1].B;
					AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
					BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
					AA1.z = -AA1.z - 2.0*m_pWPolar->m_Height;
					BB1.z = -BB1.z - 2.0*m_pWPolar->m_Height;
					VLMQmn(AA, BB, AA1, BB1, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
			}
		}
		else
		{
			// then panel p is trailing and shedding a wake
			if(!m_bWakeRollUp)
			{
				// since Panel p+1 does not exist...
				// we define the points AA=A+1 and BB=B+1
				AA1.x = m_pNode[pPanel->m_iTA].x + (m_pNode[pPanel->m_iTA].x-pPanel->A.x)/3.0;
				AA1.y = m_pNode[pPanel->m_iTA].y;
				AA1.z = m_pNode[pPanel->m_iTA].z;
				BB1.x = m_pNode[pPanel->m_iTB].x + (m_pNode[pPanel->m_iTB].x-pPanel->B.x)/3.0;
				BB1.y = m_pNode[pPanel->m_iTB].y;
				BB1.z = m_pNode[pPanel->m_iTB].z;
				if(m_pWPolar->m_bGround)
				{
					AA  = pPanel->A;
					BB  = pPanel->B;
					AAG = AA1;
					BBG = BB1;
					AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
					BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
					AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
					BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;
				}
				// first we get the quad vortex's influence
				if (bAll)
				{
					VLMQmn(pPanel->A, pPanel->B, AA1, BB1, C, V);
					if(m_pWPolar->m_bGround)
					{
						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,C,VT,bAll);

				if(m_pWPolar->m_bGround)
				{
					VLMCmn(AAG, BBG, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
				V.x += VT.x;
				V.y += VT.y;
				V.z += VT.z;
			}
			else
			{
				// if there is a wake roll-up required
				pw = pPanel->m_iWake;
				// first close the wing's last vortex ring at T.E.
				if (bAll)
				{
					VLMQmn(pPanel->A, pPanel->B, m_pWakePanel[pw].A, m_pWakePanel[pw].B, C, V);
					if(m_pWPolar->m_bGround)
					{
						AA  = pPanel->A;
						BB  = pPanel->B;
						AAG = m_pWakePanel[pw].A;
						BBG = m_pWakePanel[pw].B;
						AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
						BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
						AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
						BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;

						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//each wake panel has the same vortex strength than the T.E. panel
				//so we just cumulate their unit influences
				if(bAll)
				{
					for (lw=0; lw<m_pWPolar->m_NXWakePanels-1; lw++)
					{
						VLMQmn(m_pWakePanel[pw  ].A, m_pWakePanel[pw  ].B,
							   m_pWakePanel[pw+1].A, m_pWakePanel[pw+1].B, C, VT);
						V += VT;
						if(m_pWPolar->m_bGround)
						{
							AA  = m_pWakePanel[pw].A;
							BB  = m_pWakePanel[pw].B;
							AAG = m_pWakePanel[pw+1].A;
							BBG = m_pWakePanel[pw+1].B;
							AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
							BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
							AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
							BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;

							VLMQmn(AA, BB, AAG, BBG, C, VG);
							V.x += VG.x;
							V.y += VG.y;
							V.z -= VG.z;
						}
						pw++;
					}
				}
//				//for the very last wake panel downstream, just add a horseshoe vortex influence
//				//TODO : check influence on results
//				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//				if(m_pWPolar->m_bGround)
//				{
//					VLMCmn(AAG, BBG, C, VG);
//					V.x += VG.x;
//					V.y += VG.y;
//					V.z -= VG.z;
//				}
				//simple really !
			}
		}
	}
}


void VLMAnalysisDlg::VLMSetDownwash(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	int p, pp, m, nSurf;
	CVector C;

	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));

	nSurf = 0;

	for(int i=0; i<4; i++)
	{
		if(m_pWingList[i])
		{
			memset(m_pWingList[i]->m_Vd, 0, sizeof(m_pWing2->m_Vd));
			m=0;
			for (p=0; p<m_pWingList[i]->m_MatSize; p++)
			{
				if(m_pWingList[i]->m_pPanel[p].m_bIsTrailing)
				{
					C = (m_pNode[m_pWingList[i]->m_pPanel[p].m_iTA] + m_pNode[m_pWingList[i]->m_pPanel[p].m_iTB])/2.0;

					for (pp=0; pp<m_MatSize; pp++)
					{
						VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
						m_pWingList[i]->m_Vd[m] += V * Gamma[pp];
					}
					m_pWingList[i]->m_Ai[m] = atan2(m_pWingList[i]->m_Vd[m].z, m_OpQInf) * 180.0/PI;
					m++;
				}
			}
		}
	}
}



void VLMAnalysisDlg::VLMSetAi(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	CVector C, CG, Vt;
	int p, pp, m;
	double Ai[4*MAXSTATIONS];
	m=0;

//	CVector K(   -sin(m_Alpha*PI/180.0), 0.0, cos(m_Alpha*PI/180.0));
//	CVector QInf( cos(m_Alpha*PI/180.0), 0.0, sin(m_Alpha*PI/180.0));

//	QInf.Set(1.0,0.0,0.0);

	if(m_bTrace) AddString(tr("        Calculating induced angles...\n"));

	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsTrailing)
		{
			C    = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0;
			C.x  = 100.0*m_pWing->m_PlanformSpan;

			CG.x = C.x;
			CG.y = C.y;
			CG.z = C.z-2.0*m_pWPolar->m_Height;

			Vt.x = Vt.y = Vt.z = 0.0;

			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				Vt.x += V.x * Gamma[pp];
				Vt.y += V.y * Gamma[pp];
				Vt.z += V.z * Gamma[pp];
			}
			Ai[m] = atan2(Vt.z, m_OpQInf) * 180.0/PI; // TODO : factor 2 in excess when calculating in the Trefftz plane
			m++;
		}
	}
	int pos = 0;
	for(int i=0; i<4; i++)
	{
		if(m_pWingList[i])
		{
			for (m=0; m<m_pWing->m_NStation; m++)	    m_pWingList[i]->m_Ai[m] = Ai[m];
			pos += m_pWingList[i]->m_NStation;
		}
	}
}




bool VLMAnalysisDlg::VLMSolveDouble()
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	static int index[VLMMAXMATSIZE];

	int Size;
	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;


	//______________________________________________________________________________________
	// Solve the system for unit vortex circulation

	AddString(tr("      Solving the linear system...\n"));

	memcpy(m_RHS,      m_xRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_zRHS, Size * sizeof(double));

//	if(!Gauss(m_aij,Size, m_RHS, 2, &m_bCancel))

	double progress = 0.0;
	double TaskSize = 0.0;
	if(!Crout_LU_Decomposition_with_Pivoting(m_aij, index, Size, &m_bCancel, TaskSize, progress))
	{
		AddString(tr("      Singular Matrix.... Aborting calculation...\n"));
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	Crout_LU_with_Pivoting_Solve(m_aij, m_xRHS, index, m_RHS,      Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_zRHS, index, m_RHS+Size, Size, &m_bCancel);

	memcpy(m_xRHS, m_RHS,      Size * sizeof(double));
	memcpy(m_zRHS, m_RHS+Size, Size * sizeof(double));

	return true;
}



bool VLMAnalysisDlg::VLMSolveMultiple(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method :
	// 	- If the polar is of type 1 or 2, solve the linear system
	//	  for all AlphaDeltas simultaneously, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strange;

	int p, q, pp, Size, position;
	int n, o, o1, nel, m;

	double Lift, alpha, cosa, sina;
//	double row[VLMMAXMATSIZE];memset(row, 0, sizeof(row));
	double WingIDrag, TempCl;

	CVector Force, WindNormal;
	CVector VInf;

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	//reconstruct all results from cosine and sine unit vectors
	m=0;
	if(m_pWPolar->m_Type !=4)
	{
		for (q=0; q<nval;q++)
		{
			alpha = V0 + q * VDelta;
			cosa = cos(alpha*PI/180.0);
			sina = sin(alpha*PI/180.0);
			for(p=0; p<Size; p++)
			{
				m_RHS[m] =   cosa * m_xRHS[p] + sina * m_zRHS[p];
				m++;
			}
		}
	}
	else
	{
		cosa = cos(m_AlphaMin*PI/180.0);
		sina = sin(m_AlphaMin*PI/180.0);
		for(p=0; p<Size; p++)
		{
			m_RHS[m] =   cosa * m_xRHS[p] + sina * m_zRHS[p];
			m++;
		}
	}
	qApp->processEvents();

	//______________________________________________________________________________________
	// Construct circulations from RHS

	p=0;
	for (q=0; q<nval;q++)
	{
		for(pp=0; pp<Size; pp++)
		{
			if(m_pWPolar->m_Type!=4)	m_Gamma[q*Size+pp] = m_RHS[q*Size+pp];
			else 						m_Gamma[q*Size+pp] = m_RHS[pp];
		}
	}

	//______________________________________________________________________________________
	//Reconstruct right side results

	double *GammaRef = m_aij;//use existing reserved memory, do not re-allocate
	if(m_bVLMSymetric)
	{
		memcpy(GammaRef, m_Gamma, nval*Size*sizeof(double));

		for (q=0; q<nval;q++)
		{
			n  =  q    * Size;
			o  =  q    * m_MatSize;
			o1 = (q+1) * m_MatSize;

			for (p=0; p<m_MatSize/2; p++)
			{
				m_Gamma[o+p]    = GammaRef[n+p];
				m_Gamma[o1-1-p] = GammaRef[n+p];
			}
		}
	}

	//______________________________________________________________________________________
	//at this stage, m_Gamma is ordered as m_ppPanel[]... need to sort as m_pPanel

	memcpy(GammaRef, m_Gamma, nval*m_MatSize*sizeof(double));

	for (q=0; q<nval;q++)
	{
		o  =  q * m_MatSize;

		for (p=0; p<m_MatSize; p++)
		{
			nel = m_ppPanel[p]->m_iElement;
			m_Gamma[o+nel]    = GammaRef[o+p];
		}
	}
	//______________________________________________________________________________________
	// Calculate speeds i.a.w. polar types

	AddString(tr("      Calculating the vortices circulations...\n"));

	//so far we have a unit Vortex Strength
	if(m_pWPolar->m_Type==2 || m_pWPolar->m_Type==6)
	{
		//type 2; find the speeds which will create a lift equal to the weight
		AddString(tr("      Calculating speeds to balance the weight\n"));
		CWing::s_QInf      = 1.0;
		CWing::s_Viscosity = m_pWPolar->m_Viscosity;
		CWing::s_Density   = m_pWPolar->m_Density;

		for (q=0; q<nval;q++)
		{
			alpha = V0+q*VDelta;
			WindNormal.Set(-sin(alpha*PI/180.0),   0.0, cos(alpha*PI/180.0));
			VInf.Set(cos(alpha*PI/180.0),  0.0, sin(alpha*PI/180.0));
//			memcpy(row, m_RHS+q*Size, sizeof(row));
			Lift = 0.0;
			p=0;
			position = 0;

			Force.Set(0.0,0.0,0.0);
			CWing::s_Alpha     = alpha;
			m_pWing->VLMTrefftz(m_Gamma+q*m_MatSize, 0, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
			position = m_pWing->m_MatSize;

			if(m_pWing2)
			{
				m_pWing2->VLMTrefftz(m_Gamma+q*m_MatSize, position, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				position += m_pWing2->m_MatSize;
			}
			if(m_pStab)
			{
				m_pStab->VLMTrefftz(m_Gamma+q*m_MatSize, position, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				position += m_pStab->m_MatSize;
			}
			if(m_pFin)
			{
				m_pFin->VLMTrefftz(m_Gamma+q*m_MatSize, position, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				position += m_pFin->m_MatSize;
			}

			Lift = Force.dot(WindNormal);//N/rho
			TempCl = Lift*2.0/m_pWPolar->m_WArea;

			if(Lift<=0.0)
			{
				strong = QString(tr("      Found a negative lift for Alpha=%1.... skipping the angle...\n")).arg(V0+q*VDelta,0,'f',2);
				if(m_pWPolar->m_Type==6 && m_bTrace) AddString("\n");
				if(m_bTrace) AddString(strong);
				m_bPointOut = true;
				m_bWarning = true;
				m_VLMQInf[q] = -100.0;
			}
			else
			{
				m_VLMQInf[q] =  sqrt( 2.0* 9.81 * m_pWPolar->m_Weight /m_pWPolar->m_Density/m_pWPolar->m_WArea / TempCl );

				strong = QString(tr("      Alpha=%1   QInf = %2")).arg(V0+q*VDelta,5,'f',2).arg(m_VLMQInf[q]*pMainFrame->m_mstoUnit,5,'f',2);
				GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
				strong+= strange + "\n";
				if(m_bTrace) AddString(strong);
			}
		}
	}

	else if (m_pWPolar->m_Type==1)
		for (q=0; q<nval;q++) m_VLMQInf[q] = m_pWPolar->m_QInf;

	else if (m_pWPolar->m_Type==4)
		for (q=0; q<nval;q++) m_VLMQInf[q] = V0 + q*VDelta;

	else if (m_pWPolar->m_Type==5)
		m_VLMQInf[0] = m_pWPolar->m_QInf;

	//______________________________________________________________________________________
	// Scale circulations to speeds

	p=0;
	for (q=0; q<nval;q++)
	{
		for(pp=0; pp<m_MatSize; pp++)
		{
			m_Gamma[q*m_MatSize+pp] *= m_VLMQInf[q];
		}
	}

	return  true;
}



void VLMAnalysisDlg::WriteString(QString strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}




void VLMAnalysisDlg::Forces(double *Gamma, CVector &VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace)
{
	// Calculates the forces using a farfield method
	// Calculates the moments by a near field method, i.e. direct summation on the panels
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
	//
	if(!m_pPanel||!m_pWPolar) return;

	static bool bOutRe, bError, bOut, bOutCl;
	static int j, k, l,  p,  m;
	static double alpha, cosa, sina, Re, PCd, Cl, tau, StripArea, ViscousDrag;
	static double QInf, qdyn;
	static CVector  C, PtC4, LeverArm, dF, WindDirection, WindNormal, PanelLeverArm, Wg, Fd, dFM;
	static CVector Velocity;
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;

	bOut = bOutCl = bError = false;

	if(!bTilted) alpha = m_OpAlpha;
	else         alpha = 0.0;

	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	//   Define the wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	p=m=0;

	Fd.Set(0.0, 0.0, 0.0);
	Force.Set( 0.0, 0.0, 0.0);
	Moment.Set(0.0, 0.0, 0.0);
	ViscousDrag = 0.0;

	for(j=0; j<m_NSurfaces; j++)
	{
		for(k=0; k<m_ppSurface[j]->m_NYPanels; k++)
		{
			StripArea = 0.0;
			
			for(l=0; l<m_ppSurface[j]->m_NXPanels; l++)
			{
/*				Velocity.x = *(VInf               +p);
				Velocity.y = *(VInf +   m_MatSize +p);
				Velocity.z = *(VInf + 2*m_MatSize +p);*/
				Velocity = VInf;
				
				StripArea += m_pPanel[p].Area;
				// get the far-field force
				if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
				{
					C = m_pPanel[p].CtrlPt;
					C.x = m_pWing->m_PlanformSpan * 100.0;

					GetSpeedVector(C, Gamma,Wg);
					Wg += Velocity; //total speed vector

					//induced force
					dF  = Wg * m_pPanel[p].Vortex;
					dF.x *= 1.    * Gamma[p];  // N/rho
					dF.y *=         Gamma[p];  // N/rho
					dF.z *=         Gamma[p];  // N/rho

					Force += dF;        // N/rho
					Fd += dF;
				}

				// for the moments, get the direct summation on the panels
				PanelLeverArm = m_pPanel[p].VortexPos - m_pWPolar->m_CoG;
				dFM  = Velocity * m_pPanel[p].Vortex * Gamma[p];      // Newtons/rho
				Moment += dFM * PanelLeverArm;                    // N.m/rho
				p++;
			}

			if(m_pWPolar->m_bViscous)
			{
				//add the viscous drag component to force and moment
				QInf = Velocity.VAbs();
				
				qdyn = 0.5 * m_pWPolar->m_Density * QInf * QInf;
				m_ppSurface[j]->GetC4(k, PtC4, tau);
				Re = m_ppSurface[j]->GetChord(tau) * QInf /m_pWPolar->m_Viscosity;
				Cl = Fd.dot(WindNormal)*m_pWPolar->m_Density/qdyn/StripArea;
				PCd    = GetVar(pMiarex->m_poaPolar, 2, m_ppSurface[j]->m_pFoilA, m_ppSurface[j]->m_pFoilB, Re, Cl, tau, bOutRe, bError);
				PCd   *= StripArea * 1/2*QInf*QInf;              // Newtons/rho
				bOut = bOut || bOutRe || bError;
				ViscousDrag += PCd ;                             // Newtons/rho

				LeverArm   = PtC4 - m_pWPolar->m_CoG;
//				Moment += (WindDirection*LeverArm)*PCd;                                        // N.m/rho
				Moment.x += PCd * (WindDirection.y*LeverArm.z - WindDirection.z*LeverArm.y);   // N.m/rho
				Moment.y += PCd * (WindDirection.z*LeverArm.x - WindDirection.x*LeverArm.z);
				Moment.z += PCd * (WindDirection.x*LeverArm.y - WindDirection.y*LeverArm.x);
			}
			Fd.Set(0.0,0.0,0.0);
			m++;
		}
	}
	Force -= WindDirection*Force.dot(WindDirection)/2.0;

	if(m_pWPolar->m_bViscous) Force += WindDirection * ViscousDrag;

	Force  *= m_pWPolar->m_Density;                          // N
	Moment *= m_pWPolar->m_Density;                          // N.m
}



