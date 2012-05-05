/****************************************************************************

	PanelAnalysisDlg Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

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
#include <QCheckBox>
#include "../objects/WPolar.h"
#include "../objects/Plane.h"
#include "../objects/CVector.h"



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
	void OnProgress();


private:

	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;
	QProgressBar *m_pctrlProgress;

	void keyPressEvent(QKeyEvent *event);

	bool StartPanelThread();
	bool SolveUnitRHS();

	bool AlphaLoop();
	bool ReLoop();
	bool UnitLoop();
	bool ControlLoop();

	bool GetZeroMomentAngle();

	void AddString(QString strong);
	void BuildInfluenceMatrix();

	void ComputeAeroCoefs(double V0, double VDelta, int nrhs);
	void ComputeOnBodyCp(double V0, double VDelta, int nval);
	void ComputePlane(double Alpha, double QInf, int qrhs);
	void ComputeFarField(double QInf, double Alpha0, double AlphaDelta, int nval);
	void ComputeBalanceSpeeds(double Alpha, int q);
	void CreateDoubletStrength(double Alpha0, double AlphaDelta, int nval);
	void CreateSourceStrength(double Alpha0, double AlphaDelta, int nval);
	void CreateRHS(double *RHS, CVector VInf, double *VField = NULL, bool bTrace=false);
	void CreateUnitRHS();
	void CreateWakeContribution();
	void CreateWakeContribution(double *pWakeContrib, CVector WindDirection);
	void DoubletNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake=false);
	void GetDoubletInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake=false, bool bAll=true);
	void GetSourceInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void RelaxWake();
	void ScaleResultstoSpeed(int nval);
	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);
	void SetFileHeader();
	void SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi);
	void SetupLayout();
	void StartAnalysis();
	void SumPanelForces(double *Cp, double Alpha, double &Lift, double &Drag);
	void UpdateView();
	void WriteString(QString strong);
	void VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll);
	void VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V);
	void VLMCmn(CVector const &A, CVector const &B, CVector const &C, CVector &V, bool const &bAll=true);

	void GetDoubletDerivative(const int &p, double *Mu, double &Cp, CVector &VTotl, double const &QInf, double Vx, double Vy, double Vz);
	void GetVortexCp(const int &p, double *Gamma, double *Cp, CVector &VInf);

	void ComputeStabilityDerivatives();
	void ComputeStabilityInertia();
	bool ComputeTrimmedConditions();
	bool SolveEigenvalues();
	void BuildRotationMatrix();
	void BuildStateMatrices();
	void ComputeControlDerivatives();
	void ComputeResults();
	void ComputeNDStabDerivatives();
	void Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace=false);
	double ComputeCm(double Alpha, bool bTrace=false);


	static void *s_pMiarex;
	static void *s_pMainFrame;

	QFile *m_pXFile;

	bool m_bIsFinished;
	bool m_bType4;
	bool m_bXFile;
	bool m_b3DSymetric;
	bool m_bPointOut;
	bool m_bConverged;
//	bool m_bDirichlet;// true if Dirichlet boundary conditions, false if Neumann
	bool m_bTrefftz;
	bool m_bSequence;
	bool m_bSkip, m_bExit, m_bCancel, m_bWarning;
	bool m_bWakeRollUp;

	int m_State;
	int m_nNodes;
	int m_MatSize;
	int m_SymSize; // the size of the matrix, excluding symetric elements
	int m_NSurfaces;
	int m_nWakeNodes;
	int m_WakeSize;	
	int m_NWakeColumn;
	int m_WakeInterNodes;
	int m_MaxWakeIter;

	double m_Progress;

	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_Alpha;//Angle of Attack in degree
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

	double XNP;//Neutral point x-position resulting from stability analysis
	double CXu, CZu, Cmu, CXq, CZq, Cmq, CXa, CZa, Cma; // Non dimensional stability derivatives
	double CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	double CXe, CYe, CZe, Cle, Cme, Cne;

	double *m_aij, *m_aijWake;
	double *m_RHS, *m_RHSRef;

	double m_Sigma[VLMMAXMATSIZE*VLMMAXRHS];		// Source strengths
	double m_Mu[VLMMAXMATSIZE*VLMMAXRHS];			// Doublet strengths, or vortex circulations if panel is located on a thin surface
	double m_Cp[VLMMAXMATSIZE*VLMMAXRHS];			// lift coef per panel
	double m_3DQInf[VLMMAXRHS];

	CVector m_uVl[VLMMAXMATSIZE],m_wVl[VLMMAXMATSIZE];
	double m_uRHS[VLMMAXMATSIZE], m_vRHS[VLMMAXMATSIZE], m_wRHS[VLMMAXMATSIZE];
	double m_pRHS[VLMMAXMATSIZE], m_qRHS[VLMMAXMATSIZE], m_rRHS[VLMMAXMATSIZE];
	double m_cRHS[VLMMAXMATSIZE];
	double m_uWake[VLMMAXMATSIZE], m_wWake[VLMMAXMATSIZE];

	int m_Index[VLMMAXMATSIZE];

	CVector m_Speed[VLMMAXMATSIZE];


	QString m_strOut;
	QString m_VersionName;
	
//	CPanel **m_ppPanel;//the sorted array of panel pointers
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
	CBody *m_pBody;
	CPlane *m_pPlane;

	//temp data
	CVector VG, CG;
	double phiG;
	CPanel m_SymPanel;
	CVector R[5];
	CVector r0, r1, r2, Psi, t, Far;
	double r1v,r2v,ftmp, Omega;
	CVector *m_pR[5];
	CVector PJK, a, b, s, T1, T2, T, h;

public:
	double m_Ai[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS]; //temporary results from far field calculation
	double m_Cl[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS]; //temporary results from far field calculation
	double m_ICd[MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];
	CVector m_F[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];
	CVector m_Vd[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];
	CVector m_WingForce[MAXWINGS*VLMMAXRHS];
	double m_WingIDrag[MAXWINGS*VLMMAXRHS];
	CWing *m_pWing; //pointer to the geometry class of the wing
	CWing * m_pWingList[MAXWINGS]; //pointer to the plane's four wings
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll=true);
	void GetSpeedVector(CVector const &C, float *Mu,  float *Sigma,  CVector &VT, bool bAll=true);

public://stability analysis method and variables

	int m_NCtrls;
	//Equilibrium Forces and moments
	CVector Force0, Moment0;

	// longitudinal stability derivatives
	double Xu, Xw, Zu, Zw, Xq, Zq, Mu, Mw, Mq;//first order
	double Zwp, Mwp;                  //second order

	// latal stability derivatives
	double Yv, Yp, Yr, Lv, Lp, Lr, Nv, Np, Nr;//first order

	//stability control derivatives
	double Xde, Yde, Zde, Lde, Mde, Nde;

	double u0;     //steady state x-velocity
	double Theta0; //steady state pitch angle
//	double m_BankAngle;
	double m_ALong[4][4];	//Longitudinal State Matrix
	double m_ALat[4][4];	//Lateral State Matrix
	double m_BLong[4];
	double m_BLat[4];
	double m_R[3][3];	//Rotation matrix


	double m_Ib[3][3];	//Inertia tensor in body axis
	double m_Is[3][3];	//Inertia tensor in stability axis

	complex<double> m_rLong[4],  m_rLat[4]; // complex eigenvalues
	complex<double> m_vLong[16], m_vLat[16];// complex eigenvecctors

	double m_AlphaEq, m_Ctrl;
	double m_ControlMin, m_ControlMax, m_ControlDelta;

	CSurface **m_ppSurface;
	double m_radius, m_W, m_p, m_q, m_r; //trimmed flight data

	bool m_bTrace;
};

#endif // PANELANALYSISDLG_H














