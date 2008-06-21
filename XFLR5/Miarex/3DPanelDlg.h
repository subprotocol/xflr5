/****************************************************************************

    C3DPanelDlg Class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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
#include "Body.h"
#include "Wing.h"
#include "WPolar.h"
#include "3DPanelThread.h"
  
#include "afxwin.h"
 
// Boîte de dialogue C3DPanelDlg

class C3DPanelDlg : public CDialog
{
	friend class CMiarex;
	friend class CWing;
	friend class CWingDlg;
	friend class C3DPanelThread;

public:
	C3DPanelDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~C3DPanelDlg();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    

	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);

// Données de boîte de dialogue
	enum { IDD = IDD_3DPANELDLG };
 
protected:

	CEdit m_ctrlOutput;

	bool ComputeAeroCoefs(double V0, double VDelta, int nrhs);
	bool ComputeOnBody(int q, double Alpha);
	bool ComputePlane(double Alpha, int qrhs);
	bool ComputeSurfSpeeds(double *Mu, double *Sigma);
	bool CreateDoubletStrength(double V0, double VDelta, int nval);
	bool CreateMatrix();
	bool CreateRHS(double V0, double VDelta, int nval);
	bool CreateWakeContribution();
	bool Gauss(double *A, int n, double *B, int m, int TaskSize);
	bool StartPanelThread();
	bool SolveMultiple(double V0, double VDelta, int nval);

	void AddString(CString strong);
	void CheckSolution();
	void DoubletNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void EndSequence();
	void GetDoubletInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void GetSourceInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi);
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT);
	void RelaxWake();
	void SetProgress(int TaskSize,double TaskProgress);
	void SetFileHeader();
	void SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void SetDownwash(double *Mu, double *Sigma);
	void SetAi(int qrhs);
	void SumPanelForces(double *Cp, double Alpha, double Qinf, double &Lift, double &Drag);
	void VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V);


	double *m_aij, *m_aijRef;
	double *m_RHS;
	double *m_RHSRef;

	double m_Sigma[VLMMATSIZE*100];			// Source strengths
	double m_Mu[VLMMATSIZE*100];			// Doublet strengths
	double m_Cp[VLMMATSIZE*100];			// lift coef per panel
	double m_3DQInf[100];

	CVector m_Speed[VLMMATSIZE];

	CStdioFile m_XFile;

	CProgressCtrl m_ctrlProgress;

	bool m_bCancel;
	bool m_bSequence;
	bool m_bWarning;
	bool m_bType4;
	bool m_bXFile;
	bool m_b3DSymetric;
	bool m_bPointOut;
	bool m_bConverged;
	bool m_bDirichlet;// true if Dirichlet boundary conditions, false if Neumann
	bool m_bTrefftz;

	double pi;
	double m_Alpha;//Angle of Attack in °
	double m_OpAlpha;
	double m_AlphaMax;
	double m_DeltaAlpha;
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_XCP, m_YCP;
	double m_VCm,m_VYm; //Viscous moments
	double m_IYm;		// Induced Yawing Moment
	double m_GCm, m_GRm, m_GYm;		// Geometric Moments
	double m_QInf, m_QInfMax, m_DeltaQInf;

	double eps;
	double phiw, rz;
	double RFF;

	int m_nNodes;
	int m_MatSize;
	int m_NSurfaces;
	int m_Progress;

	int m_nWakeNodes;
	int m_WakeSize;	
	int m_NWakeColumn;
	int m_WakeInterNodes;
	int m_MaxWakeIter;

	double *m_pCoreSize;

	double side, sign, dist, S, GL;
	double RNUM, DNOM, PN, A, B, PA, PB, SM, SL, AM, AL, Al, pjk, CJKi;
	double ftmp, Omega, r1v, r2v;
	CVector R[5], r0, r1, r2, Psi, t;
	CVector PJK, a, b, s, T1, T2, T, h;

	CString m_strOut;
	CString m_VersionName;

	double m_row[VLMMATSIZE];
	double m_cosRHS[VLMMATSIZE], m_sinRHS[VLMMATSIZE];

	CPanel **m_ppPanel;//the sorted array of panel pointers
	CPanel *m_pPanel; //the original array of panels
	CPanel *m_pWakePanel;// the current working wake panel array
	CPanel *m_pRefWakePanel;// a copy of the reference wake node array if wake needs to be reset
	CPanel *m_pMemPanel;// a copy of the reference panel array for tilted calc

	CVector *m_pNode;	// the working array of Nodes 
	CVector *m_pMemNode;	// a copy of the reference node array for tilted calc
	CVector *m_pWakeNode;	// the current working wake node array
	CVector *m_pRefWakeNode; // a copy of the reference wake node array if wake needs to be reset
	CVector *m_pTempWakeNode;// the temporary wake node array during relaxation calc

	
	CWPolar *m_pWPolar;
	CWing *m_pWing; //pointer to the geometry class of the wing 
	CWing *m_pWing2;//pointer to the geometry class of a biplane's second wing 
	CWing *m_pStab;
	CWing *m_pFin;

	CBody **m_ppBody;

	CWnd *m_pFrame;
	CWnd *m_pMiarex;

	C3DPanelThread *m_pPanelThread;

	CPlane *m_pPlane;

	//temp data
	CVector VG, CG;
	double phiG;
	CPanel m_SymPanel;

	DECLARE_MESSAGE_MAP()
public:
};
