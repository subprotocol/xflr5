/****************************************************************************

    CVLMThread Class
    Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// VLMThread.cpp : implementation file
// This class will run the VLM thread and call the appropriate
// methods implemented in the CWing class
//
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "VLMThread.h"
#include "VLMDlg.h"
#include "Miarex.h"
#include "Quaternion.h"
#include <math.h>



/////////////////////////////////////////////////////////////////////////////
// CVLMThread

IMPLEMENT_DYNCREATE(CVLMThread, CWinThread)

CVLMThread::CVLMThread()
{
	pi = 3.141592654;
	m_bCancel    = false;
	m_bSequence  = false;
	m_bFinished  = false;
	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;
	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_DeltaQInf  = 0.0;
	m_MaxWakeIter = 5;

	m_pWPolar = NULL;
}

CVLMThread::~CVLMThread()
{
}

BOOL CVLMThread::InitInstance()
{
	CString strong;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	pVLMDlg->AddString("Launching VLM Analysis....\r\n\r\n");

	strong.Format("Type %d Analysis\r\n\r\n", m_pWPolar->m_Type);
	pVLMDlg->AddString(strong);
	pVLMDlg->m_bCancel = false;

	if(!m_pWPolar) return FALSE;
	if(m_pWPolar->m_Type<3) 
	{
		if(m_pWPolar->m_bTiltedGeom) UnitLoop();
		else                                  AlphaLoop();
	}
	else 	if(m_pWPolar->m_Type==4) ReLoop();
	else 	if(m_pWPolar->m_Type==5 || m_pWPolar->m_Type==6) ControlLoop();

	if (!m_bCancel && !pVLMDlg->m_bWarning) pVLMDlg->AddString("\r\nVLM Analysis completed successfully\r\n");
	else if (pVLMDlg->m_bWarning)           pVLMDlg->AddString("\r\nVLM Analysis completed ... Errors encountered\r\n");

	Sleep(100);
	return FALSE;
}

int CVLMThread::ExitInstance()
{
	m_bFinished = true;
//	m_pParent->PostMessage(WM_CLOSE);
//	m_pParent->PostMessage(V_ENDTHREAD);
//	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
//	pVLMDlg->EndDialog(0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CVLMThread, CWinThread)
	//{{AFX_MSG_MAP(CVLMThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVLMThread message handlers

bool CVLMThread::AlphaLoop()
{
	CString str;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	int nrhs;

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);
	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;
	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n\r\n");
	pVLMDlg->AddString(str);

	if(!pVLMDlg->VLMCreateRHS(0.0)) 
	{
		pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if (!pVLMDlg->VLMCreateMatrix()) 
	{
		pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!pVLMDlg->VLMSolveDouble())	
	{
		pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	pVLMDlg->VLMSolveMultiple(m_Alpha, m_DeltaAlpha, nrhs);	

	if (m_bCancel) return true;
		
	pVLMDlg->VLMComputePlane(m_Alpha, m_DeltaAlpha, nrhs);
	
	if (m_bCancel) return true;

	return true;
}



bool CVLMThread::UnitLoop()
{
	int i, nrhs;
	CString str;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	CVector O(0.0,0.0,0.0);

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);

	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;


	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}

	pVLMDlg->m_bTrace = true;
	pVLMDlg->m_Alpha = 0.0;

	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	for (i=0; i<nrhs; i++)
	{
		memcpy(pVLMDlg->m_pPanel, pVLMDlg->m_pMemPanel, pVLMDlg->m_MatSize * sizeof(CPanel));
		memcpy(pVLMDlg->m_pNode,  pVLMDlg->m_pMemNode,  pVLMDlg->m_nNodes  * sizeof(CVector));

		pMiarex->RotateGeomY(m_Alpha+i*m_DeltaAlpha, O);

		str.Format("      ...Alpha = %8.2f\r\n",m_Alpha+i*m_DeltaAlpha);
		pVLMDlg->AddString(str);

		if (m_bCancel) break;

		pVLMDlg->m_OpAlpha = m_Alpha+i*m_DeltaAlpha;

		if(!pVLMDlg->VLMCreateRHS(0.0)) 
		{
			pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!pVLMDlg->VLMCreateMatrix()) 
		{
			pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!pVLMDlg->VLMSolveDouble())	
		{
			pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
		pVLMDlg->VLMSolveMultiple(0.0, m_DeltaAlpha, 1);
	
		if (m_bCancel) break;
			
		pVLMDlg->VLMComputePlane(m_Alpha+i*m_DeltaAlpha, m_DeltaAlpha, 1);	
	}
	return true;
}




bool CVLMThread::ReLoop()
{
	CWaitCursor Wait;
	CString str;
	int nrhs;
	double QInf  = 0.0;
	double Alpha = 0.0;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	if(m_QInfMax<m_QInf) m_DeltaQInf = -abs(m_DeltaQInf);
	nrhs  = (int)abs((m_QInfMax-m_QInf)*1.0001/m_DeltaQInf) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;

	if(m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(pVLMDlg->m_pPanel,     pVLMDlg->m_pMemPanel,     pVLMDlg->m_MatSize * sizeof(CPanel));
		memcpy(pVLMDlg->m_pNode,      pVLMDlg->m_pMemNode,      pVLMDlg->m_nNodes  * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		pMiarex->RotateGeomY(m_pWPolar->m_ASpec, O);
		pVLMDlg->m_OpAlpha = m_pWPolar->m_ASpec;
		Alpha = 0.0;
	}
	else Alpha = m_Alpha;


	if(!pVLMDlg->VLMCreateRHS(Alpha)) 
	{
		pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	if(!pVLMDlg->VLMCreateMatrix())
	{
		pVLMDlg->AddString("\r\n\r\nFailed to create matrix....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}

//first solve for unit speed... calculation is linear
	if (!pVLMDlg->VLMSolveDouble())	
	{
		pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	pVLMDlg->VLMSolveMultiple(m_QInf, m_DeltaQInf, nrhs);

//then compute all points
	pVLMDlg->VLMComputePlane(m_QInf, m_DeltaQInf, nrhs);

	return true;
}


bool CVLMThread::ControlLoop()
{
	int i, j, nrhs, iter, nCtrl;
	CString str;
	double t, Cm, angle;
	double a, a0, a1, Cm0, Cm1, tmp;
	Quaternion Quat;
	CWing *pWing, *pStab;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	CVector H(0.0, 1.0, 0.0);
	CVector O(0.0, 0.0, 0.0);

	pWing = pVLMDlg->m_pWing;
	pStab = pVLMDlg->m_pStab;

	if(m_ControlMax<m_Control) m_DeltaControl = -abs(m_DeltaControl);

	nrhs  = (int)abs((m_ControlMax-m_Control)*1.0001/m_DeltaControl) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}

//	Loop for each control value
//     Update the geometry, design variables
//     Get solutions for alpha = -pi/4 and alpha = +pi/4
//     Interpolate to find alpha_0 such that Cm=0
//	   Compute plane for alpha_0
//	end loop
//

	pVLMDlg->m_bTrace = true;
	pVLMDlg->m_Alpha  = 0.0;

	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	pVLMDlg->m_VLMQInf[0] = 1.0;

	for (i=0; i<nrhs; i++)
	{
		memcpy(pVLMDlg->m_pPanel, pVLMDlg->m_pMemPanel, pVLMDlg->m_MatSize * sizeof(CPanel));
		memcpy(pVLMDlg->m_pNode,  pVLMDlg->m_pMemNode,  pVLMDlg->m_nNodes  * sizeof(CVector));

		t = m_Control +(double)i *m_DeltaControl;

		pVLMDlg->m_Ctrl = t;

		//update the variables & geometry
		// XcmRef, 
		// if plane : WingTilt, elevator Tilt
		// if flaps : wing flaps, elevator flaps
		if(m_pWPolar->m_bActiveControl[0])
		{
			//XCmRef
			m_pWPolar->m_XCmRef = m_pWPolar->m_MinControl[0] + t * (m_pWPolar->m_MaxControl[0] - m_pWPolar->m_MinControl[0]);
		}
		else m_pWPolar->m_XCmRef = m_pWPolar->m_MinControl[0];

		if(pVLMDlg->m_pPlane)
		{
			//wing incidence
			if(m_pWPolar->m_bActiveControl[1])
			{
				//wing tilt 
				angle = m_pWPolar->m_MinControl[1] + t * (m_pWPolar->m_MaxControl[1] - m_pWPolar->m_MinControl[1]);
				angle -= pVLMDlg->m_pPlane->m_WingTilt;
				Quat.Set(angle, H);

				for(j=0; j<pVLMDlg->m_pWing->m_MatSize; j++)
				{
					(pVLMDlg->m_pWing->m_pPanel+j)->Rotate(pVLMDlg->m_pPlane->m_LEWing, Quat, angle);
				}
			}

			//elevator incidence
			if(m_pWPolar->m_bActiveControl[2] && pVLMDlg->m_pStab)
			{
				//Elevator tilt 
				angle = m_pWPolar->m_MinControl[2] + t * (m_pWPolar->m_MaxControl[2] - m_pWPolar->m_MinControl[2]);
				angle -= pVLMDlg->m_pPlane->m_StabTilt;

				Quat.Set(angle, H);

				for(j=0; j<pVLMDlg->m_pStab->m_MatSize; j++)
				{
					(pVLMDlg->m_pStab->m_pPanel+j)->Rotate(pVLMDlg->m_pPlane->m_LEStab, Quat, angle);
				}
			}
		}

		// flap controls
		nCtrl = 3;
		//wing first
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			if(m_pWPolar->m_bActiveControl[nCtrl])
			{
				angle = m_pWPolar->m_MinControl[nCtrl] + t * (m_pWPolar->m_MaxControl[nCtrl] - m_pWPolar->m_MinControl[nCtrl]);

				pWing->m_Surface[j].RotateFlap(angle);
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

					pStab->m_Surface[j].RotateFlap(angle);
				}
				nCtrl++;
			}
		}

		str.Format("      ...Control = %8.2f\r\n",m_Control+i*m_DeltaControl);
		pVLMDlg->AddString(str);

		if (m_bCancel) break;

		pVLMDlg->m_OpAlpha = 0.0;

		if(!pVLMDlg->VLMCreateRHS(0.0)) 
		{
			pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!pVLMDlg->VLMCreateMatrix()) 
		{
			pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!pVLMDlg->VLMSolveDouble())	//solve for sine and cos
		{
			pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}

		//start loop to find zero-pitching-moment alpha
		
		iter = 0;
		a0 = -pi/4.0;
		a1 =  pi/4.0;
		a = 0.0;
		Cm0 = pVLMDlg->VLMComputeCm(a0*180.0/pi);
		Cm1 = pVLMDlg->VLMComputeCm(a1*180.0/pi);

		Cm = 1.0;

		//are there two intial values of opposite signs ?
		while(Cm0*Cm1>0.0 && iter <100) 
		{
			a0 *=0.9;
			a1 *=0.9;
			Cm0 = pVLMDlg->VLMComputeCm(a0*180.0/pi);
			Cm1 = pVLMDlg->VLMComputeCm(a1*180.0/pi);
			iter++;
		}

		if(iter>=100)
		{
			//no zero moment alpha
			str.Format("\r\n\r\n Interpolation unsuccessful for t=%10.3f - skipping.\r\n", t);
			pVLMDlg->AddString(str);
			pVLMDlg->m_bWarning = true;
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

			while (abs(Cm)>.0000001 && iter <100)
			{
				a = (a0+a1)/2.0;
				Cm = pVLMDlg->VLMComputeCm(a*180.0/pi);
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


			Cm = pVLMDlg->VLMComputeCm(a*180.0/pi, false);

			if(abs(Cm)<1.e-7)
			{
				pVLMDlg->VLMSolveMultiple(a*180.0/pi, 0.0, 1);
				pVLMDlg->VLMComputePlane(a*180.0/pi, m_DeltaControl, 1);
			}
			else
			{
				//no zero moment alpha
				str.Format("\r\n\r\n Interpolation unsuccessful for t=%10.3f - skipping.\r\n", t);
				pVLMDlg->AddString(str);
				pVLMDlg->m_bWarning = true;
			}
		}
	}

	memcpy(pVLMDlg->m_pPanel, pVLMDlg->m_pMemPanel, pVLMDlg->m_MatSize * sizeof(CPanel));
	memcpy(pVLMDlg->m_pNode,  pVLMDlg->m_pMemNode,  pVLMDlg->m_nNodes  * sizeof(CVector));
	return true;
}

