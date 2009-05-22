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


#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <math.h>
#include "VLMAnalysisDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/CVector.h"
#include "Miarex.h"

void *VLMAnalysisDlg::s_pMiarex;
void *VLMAnalysisDlg::s_pMainFrame;

VLMAnalysisDlg::VLMAnalysisDlg()
{
	SetupLayout();
	pi = 3.141592654;

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
		QMessageBox::warning(this, "QFLR5","The number of points to be calculated will be limited to 100");
//		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
//	m_bTrace = true;
	str = "   Solving the problem... \r\n\r\n";
	AddString(str);

	if(!VLMCreateRHS(0.0))
	{
		AddString("\r\n\r\nFailed to create RHS....\r\n");
		m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if (!VLMCreateMatrix())
	{
		AddString("\r\nFailed to create the matrix....\r\n");
		m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!VLMSolveDouble())
	{
		AddString("\r\n\r\nSingular matrix - aborting....\r\n");
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

bool VLMAnalysisDlg::ControlLoop()
{
	int i, j, nrhs, iter, nCtrl;
	QString str;
	double eps = 1.e-7;
	double t, Cm, angle;
	double a, a0, a1, Cm0, Cm1, tmp;
	Quaternion Quat;
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
		QMessageBox::warning(this,"QFLR5","The number of points to be calculated will be limited to 100");
		nrhs = 100;
	}

//	Loop for each control value
//     Update the geometry, design variables
//     Get solutions for alpha = -pi/4 and alpha = +pi/4
//     Interpolate to find alpha_0 such that Cm=0
//	   Compute plane for alpha_0
//	end loop
//

	m_bTrace = true;
	m_AlphaMin  = 0.0;

	str = QString("   Solving the problem... \r\n");
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
		if(m_pWPolar->m_bActiveControl[0])
		{
			//XCmRef
			m_pWPolar->m_XCmRef = m_pWPolar->m_MinControl[0] + t * (m_pWPolar->m_MaxControl[0] - m_pWPolar->m_MinControl[0]);
		}
		else m_pWPolar->m_XCmRef = m_pWPolar->m_MinControl[0];

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
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			if(m_pWPolar->m_bActiveControl[nCtrl])
			{
				angle = m_pWPolar->m_MinControl[nCtrl] + t * (m_pWPolar->m_MaxControl[nCtrl] - m_pWPolar->m_MinControl[nCtrl]);

				if(!pWing->m_Surface[j].RotateFlap(angle))  return false;
			}
			nCtrl++;
		}

		//elevator next and last
		if(pStab)
		{
			for (j=0; j<pStab->m_NSurfaces; j++)
			{
				if(m_pWPolar->m_bActiveControl[nCtrl])
				{
					angle = m_pWPolar->m_MinControl[nCtrl] + t * (m_pWPolar->m_MaxControl[nCtrl] - m_pWPolar->m_MinControl[nCtrl]);

					if(!pStab->m_Surface[j].RotateFlap(angle)) return false;
				}
				nCtrl++;
			}
		}

		str = QString("      ...Control = %1\r\n").arg(m_ControlMin+i*m_ControlDelta,8,'f',2);
		AddString(str);

		if (m_bCancel) break;

		m_OpAlpha = 0.0;

		if(!VLMCreateRHS(0.0))
		{
			AddString("\r\n\r\nFailed to create RHS....\r\n");
			m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!VLMCreateMatrix())
		{
			AddString("\r\nFailed to create the matrix....\r\n");
			m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!VLMSolveDouble())	//solve for sine and cos
		{
			AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			m_bWarning = true;
			return true;
		}

		//start loop to find zero-pitching-moment alpha

		iter = 0;
		a0 = -pi/4.0;
		a1 =  pi/4.0;
		a = 0.0;
		Cm0 = VLMComputeCm(a0*180.0/pi);
		Cm1 = VLMComputeCm(a1*180.0/pi);

		Cm = 1.0;

		//are there two intial values of opposite signs ?
		while(Cm0*Cm1>0.0 && iter <100)
		{
			a0 *=0.9;
			a1 *=0.9;
			Cm0 = VLMComputeCm(a0*180.0/pi);
			Cm1 = VLMComputeCm(a1*180.0/pi);
			iter++;
		}

		if(iter>=100)
		{
			//no zero moment alpha
			str = QString("      Interpolation unsuccessful for Control=%1 - skipping.\r\n\r\n\r\n").arg(t,10,'f',3);
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
				a0  =  pi/4.0;
				a1  = -pi/4.0;
			}

			while (fabs(Cm)>eps && iter <100)
			{
				a = (a0+a1)/2.0;
				Cm = VLMComputeCm(a*180.0/pi);
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


			Cm = VLMComputeCm(a*180.0/pi);

			if(fabs(Cm)<eps)
			{
				VLMSolveMultiple(a*180.0/pi, 0.0, 1);
				VLMComputePlane(a*180.0/pi, m_ControlDelta, 1);
			}
			else
			{
				//no zero moment alpha
				str = QString("      Interpolation unsuccessful for Control=%1 - skipping.\r\n\r\n\r\n").arg(t,10,'f',3);
				AddString(str);
				m_bWarning = true;
			}
		}
	}

	memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
	memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
	return true;
}



bool VLMAnalysisDlg::Gauss(double *A, int n, double *B, int m)
{
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n)
	{
		qApp->processEvents();
		if(m_bCancel) return false;
		//  find the pivot row
		A_pivot_row = pa;
		max = fabs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
		{
			if ((dum = fabs(*(pA+row))) > max)
			{
				max = dum;
				A_pivot_row = pA;
				pivot_row = i;
			}
		}
		if (max <= 0.0)
			return false;                // the matrix A is singular

			// and if it differs from the current row, interchange the two rows.

		if (pivot_row != row)
		{
			for (i = row; i < n; i++)
			{
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<=m; k++)
			{
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}

		// Perform forward substitution
		for (i = row+1; i<n; i++)
		{
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++)
				B[i+k*n] += dum * B[row+k*n];
		}
	}

	// Perform backward substitution

	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--)
	{
		qApp->processEvents();
		if(m_bCancel) return false;

		if ( *(pa + row) == 0.0 )
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum;
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++)
		{
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++)
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}


CVector VLMAnalysisDlg::GetSpeedVector(CVector C, double *Gamma)
{
	int pp;
	CVector VTot;
	VTot.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
		VTot += V * Gamma[pp];
	}

	return VTot;
}



void VLMAnalysisDlg::InitDialog()
{
	QString FileName = QDir::tempPath() + "/QFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_Type==5|| m_pWPolar->m_Type==6))
	{
		//back-up the current geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
	}

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
		str += "     Sideslip is asymmetric\r\n";
		m_bVLMSymetric = false;
	}

	if(!m_pWing->m_bSymetric) str += "     Main wing is asymmetric\r\n";

	if(m_pWing2)
	{
		if(!m_pWing2->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += "     2nd wing is asymmetric\r\n";
		}
	}
	if(m_pStab)
	{
		if(!m_pStab->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += "     Elevator is asymmetric\r\n";
		}
	}

	if(m_pFin)
	{
		m_bVLMSymetric = false;
		str += "     A fin is considered asymmetric\r\n";
	}

	if (m_bVLMSymetric) AddString("Perfoming symmetric calculation\r\n");
	else
	{
		str = "Performing asymmetric calculation : \r\n" + str;
		AddString(str);
	}
	AddString("\r\n");

	m_pWing->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pWing2)  m_pWing2->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pStab)   m_pStab->m_bVLMSymetric  = m_bVLMSymetric;
	if(m_pFin)    m_pFin->m_bVLMSymetric   = m_bVLMSymetric;
}


void VLMAnalysisDlg::OnCancelAnalysis()
{
	m_bSkip = true;
	m_bExit = true;
//	if(m_bIsFinished) done(1);
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
	QString str;
	int nrhs;
	double Alpha = 0.0;

	QMiarex * pMiarex = (QMiarex*)s_pMiarex;

	if(m_QInfMax<m_QInfMin) m_QInfDelta = -fabs(m_QInfDelta);
	nrhs  = (int)fabs((m_QInfMax-m_QInfMin)*1.0001/m_QInfDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, "QFLR5", "The number of points to be calculated will be limited to 100");
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

	if(!VLMCreateRHS(Alpha))
	{
		AddString("\r\n\r\nFailed to create RHS....\r\n");
		m_bWarning = true;
		return true;
	}
	if(!VLMCreateMatrix())
	{
		AddString("\r\n\r\nFailed to create matrix....\r\n");
		m_bWarning = true;
		return true;
	}

//first solve for unit speed... calculation is linear
	if (!VLMSolveDouble())
	{
		AddString("\r\n\r\nSingular matrix - aborting....\r\n");
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
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << "QFLR5 v001";
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
	setMinimumHeight(r.height()/3);
	setMinimumWidth(r.width()/3);
	move(r.width()/3, r.height()/6);

	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pctrlCancel = new QPushButton(tr("Cancel"));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(m_pctrlCancel);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
}


void VLMAnalysisDlg::showEvent(QShowEvent *event)
{
//	StartAnalysis();
}


void VLMAnalysisDlg::StartAnalysis()
{
	if(!m_pWPolar) return;
	QString strong;
	m_bIsFinished = false;
	strong = "Launching VLM Analysis....\r\n\r\n";
	AddString(strong);

	strong = QString("Type %1 Analysis\r\n\r\n").arg(m_pWPolar->m_Type);
	AddString(strong);
	m_bCancel = false;

	if(m_pWPolar->m_Type<3)
	{
		if(m_pWPolar->m_bTiltedGeom) UnitLoop();
		else                         AlphaLoop();
	}
	else 	if(m_pWPolar->m_Type==4) ReLoop();
	else 	if(m_pWPolar->m_Type==5 || m_pWPolar->m_Type==6) ControlLoop();

	if (!m_bCancel && !m_bWarning) strong = "\r\nVLM Analysis completed successfully\r\n";
	else if (m_bWarning)           strong = "\r\nVLM Analysis completed ... Errors encountered\r\n";
	AddString(strong);
	m_bIsFinished = true;
	m_pctrlCancel->setText("Close");
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
		QMessageBox::warning(this, "QFLR5","The number of points to be calculated will be limited to 100");
		nrhs = 100;
	}

	m_bTrace = true;
	m_AlphaMin = 0.0;

	str = QString("   Solving the problem... \r\n");
	AddString(str);

	for (i=0; i<nrhs; i++)
	{
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));

		pMiarex->RotateGeomY(m_AlphaMin+i*m_AlphaDelta, O);

		str = QString("      ...Alpha = %1\r\n").arg(m_AlphaMin+i*m_AlphaDelta,8,'f',2);
		AddString(str);

		if (m_bCancel) break;

		m_OpAlpha = m_AlphaMin+i*m_AlphaDelta;

		if(!VLMCreateRHS(0.0))
		{
			AddString("\r\n\r\nFailed to create RHS....\r\n");
			m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!VLMCreateMatrix())
		{
			AddString("\r\nFailed to create the matrix....\r\n");
			m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!VLMSolveDouble())
		{
			AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			m_bWarning = true;
			return true;
		}
		VLMSolveMultiple(0.0, m_AlphaDelta, 1);

		if (m_bCancel) break;

		VLMComputePlane(m_AlphaMin+i*m_AlphaDelta, m_AlphaDelta, 1);
	}
	return true;
}



bool VLMAnalysisDlg::VLMCreateRHS(double V0)
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

//double row[VLMMATSIZE]; memcpy(row, m_xRHS, Size*sizeof(double));
	return true;
}

bool VLMAnalysisDlg::VLMCreateMatrix()
{
	int p,pp, Size;

	CVector  CC;
	AddString("      Creating the influence matrix...\r\n");

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
//double row[VLMMATSIZE]; memcpy(row, m_aij, Size*sizeof(double));
	return true;
}


bool VLMAnalysisDlg::VLMSolveDouble()
{
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strange;

	int Size;
	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;


	//______________________________________________________________________________________
	// Solve the system for unit vortex circulation

	AddString("      Solving the linear system...\r\n");

	memcpy(m_RHS,      m_xRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_zRHS, Size * sizeof(double));

	if(!Gauss(m_aij,Size, m_RHS, 2))
	{
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

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

	int p, q, pp, Size;
	int n, o, o1, nel, m;

	double Lift, alpha, cosa, sina;
	double row[VLMMATSIZE];memset(row, 0, sizeof(row));

	CVector N, Force, WindNormal;
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
			cosa = cos(alpha*pi/180.0);
			sina = sin(alpha*pi/180.0);
			for(p=0; p<Size; p++)
			{
				m_RHS[m] =   cosa * m_xRHS[p] + sina * m_zRHS[p];
				m++;
			}
		}
	}
	else
	{
		cosa = cos(m_AlphaMin*pi/180.0);
		sina = sin(m_AlphaMin*pi/180.0);
		for(p=0; p<Size; p++)
		{
			m_RHS[m] =   cosa * m_xRHS[p] + sina * m_zRHS[p];
			m++;
		}
	}
	qApp->processEvents();

	//______________________________________________________________________________________
	// Calculate speeds i.a.w. polar types

	AddString("      Calculating the vortices circulations...\r\n");

	//so far we have a unit Vortex Strength
	if(m_pWPolar->m_Type==2)
	{
		//type 2; find the speeds which will create a lift equal to the weight
		AddString("      Calculating speeds to balance the weight\r\n\r\n");

		for (q=0; q<nval;q++)
		{
			alpha = V0+q*VDelta;
			WindNormal.Set(-sin(alpha*pi/180.0),   0.0, cos(alpha*pi/180.0));
			VInf.Set(cos(alpha*pi/180.0),  0.0, sin(alpha*pi/180.0));
			memcpy(row, m_RHS+q*Size, sizeof(row));
			Lift = 0.0;
			p=0;
			for (p=0; p<Size; p++)
			{
				// for each panel along the chord, add the lift coef
				if(m_pWPolar->m_bVLM1 || m_ppPanel[p]->m_bIsTrailing)
				{
					//changed v4.00 Leading-->Trailing
					Force = VInf * m_ppPanel[p]->Vortex;
					Lift += Force.dot(WindNormal) * row[p];
				}
				else
				{
					Force = VInf * m_ppPanel[p]->Vortex;
					Lift += Force.dot(WindNormal) * row[p];
					Force = VInf * m_ppPanel[p-1]->Vortex;
					Lift -= Force.dot(WindNormal) * row[p];
				}
			}
			if(m_bVLMSymetric) Lift *=2.0;

			if(Lift<=0.0)
			{
				strong = QString("      Found a negative lift for Alpha=%1.... skipping the angle...\r\n").arg(V0+q*VDelta,0,'f',2);
				if(m_bTrace) AddString(strong);
				m_bPointOut = true;
				m_bWarning = true;
				m_VLMQInf[q] = -100.0;
			}
			else
			{
				m_VLMQInf[q] =  sqrt( 9.81 * m_pWPolar->m_Weight /m_pWPolar->m_Density / Lift);
				strong = QString("      Alpha=%1   QInf = %2").arg(V0+q*VDelta,5,'f',2).arg(m_VLMQInf[q]*pMainFrame->m_mstoUnit,5,'f',2);
				GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
				strong+= strange + "\r\n";
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

	else if(m_pWPolar->m_Type==6)
	{
		//type 2; find the speeds which will create a lift equal to the weight
		AddString("      Calculating speeds to balance the weight\r\n\r\n");

		VInf.Set(cos(alpha*pi/180.0), 0.0, sin(alpha*pi/180.0));
		WindNormal.Set(-sin(alpha*pi/180.0), 0.0, cos(alpha*pi/180.0));

		memcpy(row, m_RHS, sizeof(row));
		Lift = 0.0;

		for (p=0; p<Size; p++)
		{
			// for each panel along the chord, add the lift coef
			Force = VInf * m_ppPanel[p]->Vortex;
			Lift += Force.dot(WindNormal) * row[p];
		}
		if(m_bVLMSymetric) Lift *=2.0;

		if(Lift<=0.0)
		{
			strong = QString("      Found a negative lift for Alpha=%1.... skipping the angle...\r\n").arg(V0+q*VDelta,0,'f',2);
			if(m_bTrace) AddString(strong);
			m_bPointOut = true;
			m_bWarning = true;
			m_VLMQInf[0] = -100.0;
		}
		else
		{
			m_VLMQInf[0] =  sqrt( 9.81 * m_pWPolar->m_Weight /m_pWPolar->m_Density / Lift);
			strong = QString("      Alpha=%1   QInf = %2").arg(V0,5,'f',2).arg(m_VLMQInf[0]*pMainFrame->m_mstoUnit,5,'f',2);
			GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
			strong+= strange + "\r\n";
			AddString(strong);
		}
	}

	//______________________________________________________________________________________
	// Scale circulations to speeds

	p=0;
	for (q=0; q<nval;q++)
	{
		for(pp=0; pp<Size; pp++)
		{
			if(m_pWPolar->m_Type!=4)	m_Gamma[q*Size+pp] = m_RHS[q*Size+pp]*m_VLMQInf[q];
			else 						m_Gamma[q*Size+pp] = m_RHS[pp]*m_VLMQInf[q];
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
	return  true;
}

double VLMAnalysisDlg::VLMComputeCm(double alpha)
{
	CVector V(cos(alpha*pi/180.0), 0.0, sin(alpha*pi/180.0));
	CVector Force, PanelLeverArm;
	int p, Size;
	double Cm = 0.0;
	double cosa = cos(alpha*pi/180.0);
	double sina = sin(alpha*pi/180.0);

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	int pUpwards;

	for(p=0; p<Size; p++)
	{
		PanelLeverArm.Set(m_ppPanel[p]->VortexPos.x - m_pWPolar->m_XCmRef, m_ppPanel[p]->VortexPos.y, m_ppPanel[p]->VortexPos.z);
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
	double Lift, IDrag, VDrag ,XCP, YCP, qdyn;
	double WingLift, WingIDrag, Alpha;
	double cosa, sina, beta;
	QString str, strong;
	CVector Force, WindNormal, WindDirection, WindSide;

	beta = 0.0;
	m_pWing->m_bTrace   = false;
	m_pWing->m_bVLM1    = m_pWPolar->m_bVLM1;
	m_pWing->m_bTrace   = true;
	m_pWing->m_pWakeNode  = m_pWakeNode;
	m_pWing->m_pWakePanel = m_pWakePanel;

	if(m_pWing2)
	{
		m_pWing2->m_bTrace   = false;
		m_pWing2->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pWing2->m_bTrace   = true;
	}

	if(m_pStab)
	{
		m_pStab->m_bTrace   = false;
		m_pStab->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pStab->m_bTrace   = true;
	}

	if(m_pFin)
	{
		m_pFin->m_bTrace   = false;
		m_pFin->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pFin->m_bTrace   = true;
	}

	for(q=0; q<nrhs; q++)
	{
		qApp->processEvents();
		if(m_bCancel) break;

		Force.Set(0.0, 0.0, 0.0);

		m_pWing->m_bWingOut = false;
		if(m_pWing2)	m_pWing2->m_bWingOut = false;
		if(m_pStab)		m_pStab->m_bWingOut  = false;
		if(m_pFin)		m_pFin->m_bWingOut   = false;

		if(m_pWPolar->m_bTiltedGeom)
		{
			Alpha = m_OpAlpha;
			//   Define wind axis
			WindNormal.Set(0, 0.0, 1.0);
			WindDirection.Set(1.0, 0.0, 0.0);
		}
		else
		{
			if(m_pWPolar->m_Type!=4)
			{
				Alpha = V0+q*VDelta;
			}
			else if(m_pWPolar->m_Type==4)
			{
				Alpha = m_pWPolar->m_ASpec;
			}
			m_OpAlpha = Alpha;
			//   Define wind axis
			cosa = cos(Alpha*pi/180.0);
			sina = sin(Alpha*pi/180.0);
			WindNormal.Set(-sina, 0.0, cosa);
			WindDirection.Set(cosa, 0.0, sina);
		}

		WindSide = WindNormal * WindDirection;

		m_OpQInf = m_VLMQInf[q];
		qdyn = 0.5 * m_pWPolar->m_Density * m_OpQInf * m_OpQInf;

		if(m_OpQInf >0.0 || m_pWPolar->m_Type==5|| m_pWPolar->m_Type==6)
		{
			if(m_pWPolar->m_Type!=4 && !m_pWPolar->m_bTiltedGeom)
			{
				str = QString("      ...Alpha=%1\r\n").arg(m_OpAlpha,0,'f',2);
				AddString(str);
			}
			else if (m_pWPolar->m_Type==4)
			{
				str = QString("   ...QInf = %1 ").arg(m_OpQInf*pMainFrame->m_mstoUnit,6,'f',2);
				GetSpeedUnit(strong, pMainFrame->m_SpeedUnit);
				str += strong + "\r\n";
				AddString(str);
			}

			AddString("        Calculating aerodynamic coefficients...\r\n");
			m_bPointOut          = false;
			m_pWing->m_Alpha     = Alpha;
			m_pWing->m_QInf      = m_OpQInf;
			m_pWing->m_Viscosity = m_pWPolar->m_Viscosity;
			m_pWing->m_Density   = m_pWPolar->m_Density;

			Lift   = 0.0;
			IDrag  = 0.0;
			VDrag  = 0.0;
			XCP    = 0.0;
			YCP    = 0.0;

			m_GRm                 = 0.0;
			m_GCm = m_VCm         = 0.0;
			m_GYm = m_VYm = m_IYm = 0.0;

			AddString("         Calculating Trefftz plane...\r\n");
			m_pWing->VLMTrefftz(m_Gamma+q*m_MatSize, 0, Force, IDrag, m_pWPolar->m_bTiltedGeom);
			AddString("         Calculating Aero Forces...\r\n");
			m_pWing->VLMComputeWing(m_Gamma+q*m_MatSize, m_Cp,VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom, m_pWPolar->m_RefAreaType);

			AddString("         Calculating Bending...\r\n");
			m_pWing->VLMSetBending();
			if(m_pWing->m_bWingOut)  m_bPointOut = true;
			pos = m_pWing->m_MatSize;

			Station = m_pWing->m_NStation;

			if(m_pWing2)
			{
				AddString("       Calculating 2nd wing...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pWing2->m_Alpha     = Alpha;
				m_pWing2->m_QInf      = m_OpQInf;
				m_pWing2->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pWing2->m_Density   = m_pWPolar->m_Density;
				m_pWing2->VLMTrefftz(m_Gamma+q*m_MatSize, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pWing2->VLMComputeWing(m_Gamma+q*m_MatSize+m_pWing->m_MatSize,
										m_Cp+m_pWing->m_MatSize,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm,
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom, m_pWPolar->m_RefAreaType);
				IDrag += WingIDrag;

				m_pWing2->VLMSetBending();
				if(m_pWing2->m_bWingOut) m_bPointOut = true;

				Station += m_pWing2->m_NStation;
				pos += m_pWing2->m_MatSize;
			}

			if(m_pStab)
			{
				AddString("         Calculating elevator...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pStab->m_Alpha     = Alpha;
				m_pStab->m_QInf      = m_OpQInf;
				m_pStab->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pStab->m_Density   = m_pWPolar->m_Density;

				m_pStab->VLMTrefftz(m_Gamma+q*m_MatSize, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pStab->VLMComputeWing(m_Gamma+q*m_MatSize+pos,
										m_Cp+pos,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm,
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom, m_pWPolar->m_RefAreaType);
				IDrag += WingIDrag;

				m_pStab->VLMSetBending();
				if(m_pStab->m_bWingOut) m_bPointOut = true;

				Station += m_pStab->m_NStation;
				pos += m_pStab->m_MatSize;
			}

			if(m_pFin)
			{
				AddString("         Calculating fin...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pFin->m_Alpha      = Alpha;
				m_pFin->m_QInf       = m_OpQInf;
				m_pFin->m_Viscosity  = m_pWPolar->m_Viscosity;
				m_pFin->m_Density    = m_pWPolar->m_Density;

				m_pFin->VLMTrefftz(m_Gamma+q*m_MatSize, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pFin->VLMComputeWing( m_Gamma+q*m_MatSize+pos,
										m_Cp+pos,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm,
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom, m_pWPolar->m_RefAreaType);
				if(m_pFin->m_bWingOut)  m_bPointOut = true;

				IDrag += WingIDrag;

				m_pFin->VLMSetBending();
			}

//			m_CL          =  2.0*Lift /m_OpQInf/m_OpQInf/m_pWing->m_Area;
			m_CL          =  2.0*Force.dot(WindNormal)    /m_OpQInf/m_OpQInf/m_pWing->m_Area;
			m_CX          =  2.0*Force.dot(WindDirection) /m_OpQInf/m_OpQInf/m_pWing->m_Area;
			m_CY          =  2.0*Force.dot(WindSide)      /m_OpQInf/m_OpQInf/m_pWing->m_Area;

			m_InducedDrag =  1.0*IDrag/m_OpQInf/m_OpQInf/m_pWing->m_Area;
			m_ViscousDrag =  1.0*VDrag              /m_pWing->m_Area;

			m_XCP         = XCP/Force.dot(WindNormal)/m_pWPolar->m_Density;
			m_YCP         = YCP/Force.dot(WindNormal)/m_pWPolar->m_Density;

			m_GCm *=  1.0 / m_pWing->m_Area /m_pWing->m_MAChord /qdyn;
			m_GRm *=  1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;
			m_GYm *=  1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			m_VCm *= 1.0 / m_pWing->m_Area /m_pWing->m_MAChord /qdyn;
			m_VYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			m_IYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			VLMSetDownwash(m_Gamma+q*m_MatSize);

			if(m_bPointOut) m_bWarning = true;
			if(m_bConverged)
			{
				if(m_pPlane)
					pMiarex->AddPOpp(m_bPointOut, m_Cp, m_Gamma+q*m_MatSize);
				else
					pMiarex->AddWOpp(m_bPointOut, m_Gamma+q*m_MatSize);
			}
			AddString("     \r\n");
		}
		else m_bPointOut = true;
	}
}


void VLMAnalysisDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	int lw, pw;
	int p = pPanel->m_iElement;
	V.Set(0.0,0.0,0.0);

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
				V += VT;
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


void VLMAnalysisDlg::VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V)
{
	//Quadrilateral VLM FORMULATION
	// LA, LB, TA, TB are the vortex's four corners
	// LA and LB are at the 3/4 point of panel nx
	// TA and TB are at the 3/4 point of panel nx+1
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    TA__________TB
	//
	//
	//
	// C is the point where the induced speed is calculated
	// V is the resulting speed
	//
	// Vectorial operations are written explicitly to save computing times (4x more efficient)
	//
	double CoreSize = 0.00000;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;

	int i;

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	R[0].x = LB.x;
	R[0].y = LB.y;
	R[0].z = LB.z;
	R[1].x = TB.x;
	R[1].y = TB.y;
	R[1].z = TB.z;
	R[2].x = TA.x;
	R[2].y = TA.y;
	R[2].z = TA.z;
	R[3].x = LA.x;
	R[3].y = LA.y;
	R[3].z = LA.z;
	R[4].x = LB.x;
	R[4].y = LB.y;
	R[4].z = LB.z;

	for (i=0; i<4; i++)
	{
		r0.x = R[i+1].x - R[i].x;
		r0.y = R[i+1].y - R[i].y;
		r0.z = R[i+1].z - R[i].z;
		r1.x = C.x - R[i].x;
		r1.y = C.y - R[i].y;
		r1.z = C.z - R[i].z;
		r2.x = C.x - R[i+1].x;
		r2.y = C.y - R[i+1].y;
		r2.z = C.z - R[i+1].z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		r1v = sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z));
		r2v = sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/r1v - (r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/r2v;
			V.x += Psi.x * Omega/4.0/pi;
			V.y += Psi.y * Omega/4.0/pi;
			V.z += Psi.z * Omega/4.0/pi;
		}
	}
}



void VLMAnalysisDlg::VLMCmn(CVector const &A, CVector const &B, CVector const &C, CVector &V, bool bAll)
{
	// CLASSIC VLM FORMULATION/
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    \/          \/
	//
	// Note : the geometry has been rotated by sideslip, hence, there is no need to align the trailing vortices with sideslip
	//
	// V is the resulting speed at point C
	//
	// Vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....
	double CoreSize = 0.000000;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;

	if(!m_pWing) return;

	V.Set(0.0,0.0,0.0);

	if(bAll)
	{
		r0.x = B.x - A.x;
		r0.y = B.y - A.y;
		r0.z = B.z - A.z;

		r1.x = C.x - A.x;
		r1.y = C.y - A.y;
		r1.z = C.z - A.z;

		r2.x = C.x - B.x;
		r2.y = C.y - B.y;
		r2.z = C.z - B.z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) >CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

			V.x = Psi.x * Omega/4.0/pi;
			V.y = Psi.y * Omega/4.0/pi;
			V.z = Psi.z * Omega/4.0/pi;
		}
	}

	// We create Far points to align the trailing vortices with the reference axis
	// The trailing vortex legs are not aligned with the free-stream, i.a.w. the small angle approximation
	// If this approximation is not valid, then the geometry should be tilted in the polar definition

	// calculate left contribution
	Far.x = A.x + m_pWing->m_Span * 10000.0;
	Far.y = A.y;
	Far.z = A.z;// + (Far.x-A.x) * tan(m_Alpha*pi/180.0);

	r0.x = A.x - Far.x;
	r0.y = A.y - Far.y;
	r0.z = A.z - Far.z;

	r1.x = C.x - A.x;
	r1.y = C.y - A.y;
	r1.z = C.z - A.z;

	r2.x = C.x - Far.x;
	r2.y = C.y - Far.y;
	r2.z = C.z - Far.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	t.x=1.0; t.y=0.0; t.z=0.0;

	h.x =  r1.y*t.z - r1.z*t.y;
	h.y = -r1.x*t.z + r1.z*t.x;
	h.z =  r1.x*t.y - r1.y*t.x;

	//Next add 'left' semi-infinite contribution
	//eq.6-56

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				-(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}

	// calculate right vortex contribution
	Far.x = B.x + m_pWing->m_Span * 10000.0;
	Far.y = B.y ;
	Far.z = B.z;// + (Far.x-B.x) * tan(m_Alpha*pi/180.0);

	r0.x = Far.x - B.x;
	r0.y = Far.y - B.y;
	r0.z = Far.z - B.z;

	r1.x = C.x - Far.x;
	r1.y = C.y - Far.y;
	r1.z = C.z - Far.z;

	r2.x = C.x - B.x;
	r2.y = C.y - B.y;
	r2.z = C.z - B.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	//Last add 'right' semi-infinite contribution
	//eq.6-57
	h.x =  r2.y*t.z - r2.z*t.y;
	h.y = -r2.x*t.z + r2.z*t.x;
	h.z =  r2.x*t.y - r2.y*t.x;

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				-(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}
}



void VLMAnalysisDlg::VLMSetDownwash(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	int p, pp, m, nSurf;
	CVector C;

	m=0;
	nSurf = 0;
	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));

	m=0;
	for (p=0; p< m_pWing->m_MatSize; p++)
	{
		if(m_pWing->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pWing->m_pPanel[p].m_iTA] + m_pNode[m_pWing->m_pPanel[p].m_iTB])/2.0;
			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
				m_pWing->m_Vd[m] += V * Gamma[pp];
			}
			m_pWing->m_Ai[m] = atan2(m_pWing->m_Vd[m].z, m_OpQInf) * 180.0/pi;
			m++;
		}
	}


	if(m_pWing2)
	{
		m=0;
		for (p=0; p< m_pWing2->m_MatSize; p++)
		{
			if(m_pWing2->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pWing2->m_pPanel[p].m_iTA] + m_pNode[m_pWing2->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pWing2->m_Vd[m] += V * Gamma[pp];
				}
				m_pWing2->m_Ai[m] = atan2(m_pWing2->m_Vd[m].z, m_OpQInf) * 180.0/pi;
				m++;
			}
		}
	}

	if(m_pStab)
	{
		m=0;
		for (p=0; p< m_pStab->m_MatSize; p++)
		{
			if(m_pStab->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pStab->m_pPanel[p].m_iTA] + m_pNode[m_pStab->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pStab->m_Vd[m] += V * Gamma[pp];
				}
				m_pStab->m_Ai[m] = atan2(m_pStab->m_Vd[m].z, m_OpQInf) * 180.0/pi;
				m++;
			}
		}
	}

	if(m_pFin)
	{
		m=0;
		for (p=0; p< m_pFin->m_MatSize; p++)
		{
			if(m_pFin->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pFin->m_pPanel[p].m_iTA] + m_pNode[m_pFin->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pFin->m_Vd[m] += V * Gamma[pp];
				}
				m_pFin->m_Ai[m] = atan2(m_pFin->m_Vd[m].z, m_OpQInf) * 180.0/pi;
				m++;
			}
		}
	}
}



void VLMAnalysisDlg::VLMSetAi(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	CVector C, CG;
	CVector Vt;
	int p, pp, m;
	double Ai[4*MAXSTATIONS];
	m=0;

//	CVector K(   -sin(m_Alpha*pi/180.0), 0.0, cos(m_Alpha*pi/180.0));
//	CVector QInf( cos(m_Alpha*pi/180.0), 0.0, sin(m_Alpha*pi/180.0));

//	QInf.Set(1.0,0.0,0.0);

	if(m_bTrace) AddString("        Calculating induced angles...\r\n");

	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0;
//			C += QInf * 20.0*m_pWing->m_Span;
			C.x  = 100.0*m_pWing->m_Span;

			CG.x = C.x;
			CG.y = C.y;
			CG.z = C.z-2.0*m_pWPolar->m_Height;

			Vt.Set(0.0,0.0,0.0);

			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				Vt += V * Gamma[pp];
			}
			Ai[m] = atan2(Vt.z, m_OpQInf) * 180.0/pi; // TODO : factor 2 in excess when calculating in the Trefftz plane
			m++;
		}
	}
	int pos = 0;
	for (m=0; m<m_pWing->m_NStation; m++)	    m_pWing->m_Ai[m] = Ai[m];
	pos += m_pWing->m_NStation;
	if(m_pWing2)
	{
		for (m=0; m<m_pWing2->m_NStation; m++)	m_pWing2->m_Ai[m] = Ai[m+pos];
		pos += m_pWing2->m_NStation;
	}
	if(m_pStab)
	{
		for (m=0; m<m_pStab->m_NStation; m++)	m_pStab->m_Ai[m] = Ai[m+pos];
		pos += m_pStab->m_NStation;
	}
	if(m_pFin)
	{
		for (m=0; m<m_pFin->m_NStation; m++)	m_pFin->m_Ai[m] = Ai[m+pos];
	}
}


void VLMAnalysisDlg::WriteString(QString strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}





