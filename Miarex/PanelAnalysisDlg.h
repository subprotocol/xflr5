/****************************************************************************

	PanelAnalysisDlg Class
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
 

#ifndef PANELANALYSISDLG_H
#define PANELANALYSISDLG_H

#include <QDialog>
#include <QProgressBar>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include "../Objects/WPolar.h"
#include "../Objects/Plane.h"
#include "../Objects/CVector.h"



class PanelAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;
	friend class CWing;

public:
	PanelAnalysisDlg();

	void InitDialog();

private slots:
	void OnCancelAnalysis();


private:

	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;
	QProgressBar *m_pctrlProgress;

	void keyPressEvent(QKeyEvent *event);

	bool ComputeAeroCoefs(double V0, double VDelta, int nrhs);
	bool ComputeOnBody(int q, double Alpha);
	bool ComputePlane(double Alpha, int qrhs);
	bool ComputeSurfSpeeds(double *Mu, double *Sigma);
	bool CreateDoubletStrength(double V0, double VDelta, int nval);
	bool CreateMatrix();
	bool CreateRHS(double V0, double VDelta, int nval);
	bool CreateWakeContribution();
	bool StartPanelThread();
	bool SolveMultiple(double V0, double VDelta, int nval);

	bool AlphaLoop();
	bool ReLoop();
	bool UnitLoop();
	bool ControlLoop();
	bool Gauss(double *A, int n, double *B, int m, int TaskSize);

	void AddString(QString strong);
	void CheckSolution();
	void DoubletNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void EndSequence();
	void GetDoubletInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void GetSourceInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi);
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT);
	void RelaxWake();
	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);
	void SetProgress(int TaskSize,double TaskProgress);
	void SetFileHeader();
	void SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void SetDownwash(double *Mu, double *Sigma);
	void SetAi(int qrhs);
	void SetupLayout();
	void StartAnalysis();
	void SumPanelForces(double *Cp, double Alpha, double QInf, double &Lift, double &Drag);
	void UpdateView();
	void VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V);
	void WriteString(QString strong);

	CVector GetSpeedVector(CVector C, double *Gamma);


	static void *s_pMiarex;
	static void *s_pMainFrame;

	QFile *m_pXFile;

	bool m_bIsFinished;
	bool m_bType4;
	bool m_bXFile;
	bool m_b3DSymetric;
	bool m_bPointOut;
	bool m_bConverged;
	bool m_bDirichlet;// true if Dirichlet boundary conditions, false if Neumann
	bool m_bTrefftz;
	bool m_bSequence;
	bool m_bSkip, m_bExit, m_bCancel, m_bWarning;
	bool m_bWakeRollUp;

	int m_State;
	int m_Progress;
	int m_nNodes;
	int m_MatSize;
	int m_NSurfaces;
	int m_nWakeNodes;
	int m_WakeSize;	
	int m_NWakeColumn;
	int m_WakeInterNodes;
	int m_MaxWakeIter;

	double pi;
	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_Alpha;//Angle of Attack in �
	double m_OpAlpha;
	double m_ReMin, m_ReMax, m_ReDelta;

	double m_CL, m_CX, m_CY, m_ViscousDrag, m_InducedDrag;
	double m_XCP, m_YCP;
	double m_VCm, m_VYm; //Viscous moments
	double m_ICm, m_IYm;		//Induced moments
	double m_GCm, m_GRm, m_GYm;		// Geometric Moments
	double m_QInf, m_QInfMax, m_QInfDelta;
	double eps;
	double phiw, rz;
	double RFF;
	double *m_pCoreSize;
	double side, sign, dist, S, GL;
	double RNUM, DNOM, PN, A, B, PA, PB, SM, SL, AM, AL, Al, pjk, CJKi;
	double ftmp, Omega, r1v, r2v;

	double *m_aij, *m_aijRef;
	double *m_RHS, *m_RHSRef;

	double m_row[VLMMATSIZE];
	double m_cosRHS[VLMMATSIZE], m_sinRHS[VLMMATSIZE];
	double m_Sigma[VLMMATSIZE*100];			// Source strengths
	double m_Mu[VLMMATSIZE*100];			// Doublet strengths
	double m_Cp[VLMMATSIZE*100];			// lift coef per panel
	double m_3DQInf[100];

	CVector m_Speed[VLMMATSIZE];

	CVector R[5], r0, r1, r2, Psi, t;
	CVector PJK, a, b, s, T1, T2, T, h;

	QString m_strOut;
	QString m_VersionName;


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


	CPlane *m_pPlane;

	//temp data
	CVector VG, CG;
	double phiG;
	CPanel m_SymPanel;
};

#endif // PANELANALYSISDLG_H
