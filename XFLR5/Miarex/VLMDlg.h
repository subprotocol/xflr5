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
protected:
	CStdioFile m_XFile;

	bool m_bSequence;
	bool m_bWarning;
	bool m_bXFile;
	bool m_bVLMSymetric;
	bool m_bWakeRollUp;
	bool m_bTrace;
	bool m_bConverged;
	bool m_bCancel;
	bool m_bPointOut;

	double pi;
	double m_row[VLMMATSIZE*100];
	double m_aij[VLMMATSIZE*VLMMATSIZE];// coefficient matrix
	double m_VLMQInf[100];
	double m_Gamma[VLMMATSIZE*100];//Vortex circulation for VLM method
	double m_Cp[VLMMATSIZE];//lift coef per panel

	double m_AlphaCalc;

	double m_QInf, m_QInfMax, m_DeltaQInf;
	double m_Alpha, m_AlphaMax, m_DeltaAlpha;
	CString m_strOut;
	CString m_VersionName;

	CPanel *m_pRefPanels;
	CVector *m_pRefNodes;
	CSurface *m_pRefSurfaces;

	CPanel m_pPanel[VLMMATSIZE];
	CVector m_pNode[2*VLMMATSIZE];
	CSurface m_pSurface[MAXVLMSURFACES];

	CVLMThread *m_pVLMThread;

	int m_VLMMatSize;
	int m_nNodes;
	int m_NSurfaces;
	int m_MaxWakeIter;
	int m_WakeInterNodes;
	double m_CoreSize;

	CWPolar *m_pWPolar;
	CWing *m_pWing;//pointer to the geometry class of the wing 
	CWnd* m_pMiarex;

	CPlane *m_pPlane;
	CWing *m_pStab;
	CWing *m_pFin;

	CWnd *m_pFrame;
	// Generated message map functions
	//{{AFX_MSG(CVLMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	double m_Ai[MAXSTATIONS+1];//Induced angles, in degrees
	double m_ICd[MAXSTATIONS];
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_XCP, m_YCP;
	double m_TCm, m_GCm, m_VCm;
	double m_Rm;
	double m_GYm, m_IYm;

	CVector AA, BB, AA1, BB1, AAG, BBG, V, VT, VS, VG;

	int m_nWakeNodes;
	int m_NXWakePanels;
	int m_VLMWakeSize;// Max Size for the VLMMatrix
	CPanel  * m_pWakePanel;
	CVector *m_pWakeNode;
	CVector m_WakeNode[2*VLMMATSIZE];
	CVector GetSpeedVector(CVector C, double *Gamma);
	void AddString(CString strong);
	void SetFileHeader();
	void EndSequence();
	bool VLMCreateMatrix();
	bool VLMSolveMultiple(double V0, double VDelta, int nval);
	void VLMGetVortexInfluence(int p, CVector C, CVector &V, bool bAll, bool bGround = false, double Height = 0.0);
	void VLMSetAi(double *Gamma);
	void VLMComputePlane(double V0, double VDelta, int nrhs);
	void VLMComputeTBCp(double *Gamma);
	void VLMSetDownwash(double *Gamma);
	void VLMResetWakeNodes();
	void VLMMoveWakeNodes();
	void VLMRotateGeomY(double Angle, CVector P);
	void VLMCmn(CVector A, CVector B, CVector C, CVector &V, bool bAll=true);
	void VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V);
	bool Gauss(double *A, int n, double *B, int m);
//	int  VLMIsWakeNode(CVector Pt);

};
