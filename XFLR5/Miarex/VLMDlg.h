/****************************************************************************

    CVLMDlg Class
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

#pragma once
// VLMDlg.h : header file
//

#include "VLMThread.h"
#include "WPolar.h"
#include "Plane.h"

/////////////////////////////////////////////////////////////////////////////
// CVLMDlg dialog

class CVLMDlg : public CDialog
{
	friend class CMainFrame;
	friend class CMiarex;
	friend class CWOperDlgBar;
	friend class CWing;
	friend class CVLMThread;
	friend class CPOperDlgBar;
// Construction
public:
	CVLMDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVLMDlg)
	enum { IDD = IDD_VLMDLG };
	CEdit	m_ctrlOutput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVLMDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CVLMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CVector GetSpeedVector(CVector C, double *Gamma);
	void AddString(CString strong);
	void SetFileHeader();
	void EndSequence();
	void VLMRotateGeomY(double Angle, CVector P);
	bool VLMCreateRHS(double V0, double VDelta, int nval);
	bool VLMCreateMatrix();
	bool VLMSolveMultiple(double V0, double VDelta, int nval);
	void VLMGetVortexInfluence(CPanel *pPanel, CVector C, CVector &V, bool bAll, bool bGround = false, double Height = 0.0);
	void VLMSetAi(double *Gamma);
	void VLMComputePlane(double V0, double VDelta, int nrhs);
	void VLMSetDownwash(double *Gamma);
	void VLMCmn(CVector A, CVector B, CVector C, CVector &V, bool bAll=true);
	void VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V);
	void ResetWakeNodes();
	void RelaxWake();

protected:
	CWnd* m_pMiarex;
	CWnd *m_pFrame;

	bool m_bSequence;
	bool m_bWarning;
	bool m_bXFile;
	bool m_bVLMSymetric;
	bool m_bWakeRollUp;
	bool m_bTrace;
	bool m_bConverged;
	bool m_bCancel;
	bool m_bPointOut;
 
	int m_MatSize;
	int m_nNodes;
	int m_NSurfaces;
	int m_MaxWakeIter;
	int m_WakeInterNodes;
	int m_nWakeNodes;
	int m_WakeSize;// Max Size for the VLMMatrix
	int m_NWakeColumn;

	double pi;
	double *m_RHS;
	double *m_aij;
	double *m_Gamma;
	double m_Cp[VLMMATSIZE];//lift coef per panel
	double m_VLMQInf[100];
	double m_OpAlpha;
	double m_QInf, m_QInfMax, m_DeltaQInf;
	double m_Alpha, m_AlphaMax, m_DeltaAlpha;
	double m_Ai[MAXSTATIONS+1];//Induced angles, in degrees
	double m_ICd[MAXSTATIONS];
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_XCP, m_YCP;
	double m_TCm, m_GCm, m_VCm;
	double m_Rm;
	double m_GYm, m_IYm;
	double Omega;
	double ftmp;
	double *m_pCoreSize;

	CString m_strOut;
	CString m_VersionName;

	CStdioFile m_XFile;

	CPanel *m_pMemPanel;//pointer to the reference panels array created in Miarex

	CPanel *m_pPanel; 	// the original array of panels
	CPanel **m_ppPanel;	// the re-ordered array of pointers to the panels
	CPanel  *m_pWakePanel;
	CPanel  *m_pRefWakePanel;

	CVector *m_pNode;	// the working array of Nodes 
	CVector *m_pMemNode;	// a copy of the reference node array for tilted calcs
	CVector *m_pWakeNode;	// the current working wake node array
	CVector *m_pRefWakeNode; // the reference wake node array if wake needs to be reset
	CVector *m_pTempWakeNode;// the temporary wake node array during relaxation calc

	CVLMThread *m_pVLMThread;

	CPlane *m_pPlane;
	CWing *m_pWing;//pointer to the main wing 
	CWing *m_pWing2;//pointer to the second wing if Biplane
	CWing *m_pStab;
	CWing *m_pFin;
	CWPolar *m_pWPolar;


//	CVector m_UInf;
	CVector AA, BB, AA1, BB1, AAG, BBG, V, VT, VS, VG;
	CVector Far;
	CVector r0, r1, r2;
	CVector t;
	CVector Psi;

public:
};
