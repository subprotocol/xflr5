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
#include "Wing.h"
#include "WPolar.h"
#include "3DPanelThread.h"

#include "afxwin.h"

// Boîte de dialogue C3DPanelDlg

class C3DPanelDlg : public CDialog
{
	friend class CMiarex;
	friend class C3DPanelThread;
public:
	C3DPanelDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~C3DPanelDlg();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Données de boîte de dialogue
	enum { IDD = IDD_3DPANELDLG };
 
protected:
	CVector P1, P2, P3, P4;

	CEdit m_ctrlOutput;
	virtual void DoDataExchange(CDataExchange* pDX);    

	bool AddWakeContribution();
	bool Gauss(double *A, int n, double *B, int m);
	bool CreateMatrix();
	bool CreateRHS(double V0, double VDelta, int nval);
	bool StartPanelThread();
	bool SolveMultiple(double V0, double VDelta, int nval);
	bool ComputeOnBody(double *Mu, double *Sigma, double frac);
	bool ComputeSurfSpeeds(double *Mu, double *Sigma, double frac);
	void AddString(CString strong);
	void ComputePlane(double V0, double VDelta, int nrhs);
	void EndSequence();
	void SetProgress(int progress,double fraction);
	void SetFileHeader();
	void SourceNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi);
	void DoubletNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void SetDownwash(double *Mu, double *Sigma);
	void CheckSolution();

	CVector GetSpeedVector(CVector C, double *Mu, double *Sigma);

	double m_aij[VLMMATSIZE*VLMMATSIZE];    // coefficient matrix
	double m_RHS[VLMMATSIZE*100];			// RHS vector
	double m_Sigma[VLMMATSIZE];				// Source strengths
	double m_Mu[VLMMATSIZE];				// Doublet strengths
	double m_Cp[VLMMATSIZE];				//lift coef per panel

	double m_3DQInf[100];

	CVector m_Speed[VLMMATSIZE];

	CStdioFile m_XFile;

	CProgressCtrl m_ctrlProgress;

	bool m_bCancel;
	bool m_bSequence;
	bool m_bWarning;
	bool m_bType4;
	bool m_bXFile;
	bool m_bPlaneAnalysis;
	bool m_b3DSymetric;
	bool m_bPointOut;
	bool m_bConverged;
	bool m_bNeumann;// true if Neumann boundary conditions, false if Dirichlet

	double pi;
	double m_Alpha;//Angle of Attack in °
	double m_AlphaCalc;
	double m_AlphaMax;
	double m_DeltaAlpha;
	double m_Ai[MAXSTATIONS+1];//Induced angles, in degrees
	double m_ICd[MAXSTATIONS];
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_XCP, m_YCP;
	double m_TCm, m_GCm, m_VCm;
	double m_Rm;
	double m_GYm, m_IYm;
	double m_QInf, m_QInfMax, m_DeltaQInf;

	double f,g;
	double x,y,z;//local coordinates
	double x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4;
	double r1, r2, r3, r4, e1, e2, e3, e4, h1, h2, h3, h4;
	double d12, d23, d34, d41;
	double d1, d2, d3, d4;
	double phi1, phi2;
	double us, vs, ws;
	double eps;
	double phiw, rz, dist;
	CVector R;

	int m_nNodes;
	int m_MatSize;
	int m_NSurfaces;
	int m_Progress;
	int RFF;
	int m_NWakeColumn;
	int m_NXWakePanels;

	CString m_strOut;
	CString m_VersionName;

	double m_row[VLMMATSIZE];
//	CPanel *m_pRefPanels;
//	CVector *m_pRefNodes;
//	CSurface *m_pRefSurfaces;

	CPanel *m_pPanel;
	CVector *m_pNode;
	CPanel *m_pWakePanel;
	CVector *m_pWakeNode;

	CSurface *m_pSurface;
	
	CWPolar *m_pWPolar;
	CWing *m_pWing; //pointer to the geometry class of the wing 
	CWing *m_pWing2;//pointer to the geometry class of a biplane's second wing 
	CWing *m_pStab;
	CWing *m_pFin;

	CWnd *m_pFrame;
	CWnd *m_pMiarex;

	C3DPanelThread *m_pPanelThread;

	CPlane *m_pPlane;

	DECLARE_MESSAGE_MAP()
public:
//	LRESULT OnEndViscDialog(WPARAM wParam, LPARAM lParam);            

	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
};
