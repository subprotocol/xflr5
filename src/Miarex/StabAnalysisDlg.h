/****************************************************************************

	StabAnalysisDlg Class
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


#ifndef STABANALYSISDLG_H
#define STABANALYSISDLG_H

#include <QDialog>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include "../Objects/WPolar.h"
#include "../Objects/Plane.h"
#include "../Objects/CVector.h"
#include <math.h>
#include <complex>

using namespace std;


class StabAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;
	friend class CWing;

public:
	StabAnalysisDlg();

	void InitDialog();

private slots:
	void OnCancelAnalysis();


private:
	void keyPressEvent(QKeyEvent *event);

	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);
	void SetFileHeader();
	void SetupLayout();
	void StartAnalysis();
	void UpdateView();
	void AddString(QString strong);
	void WriteString(QString strong);
	void BuildRHS();
	void BuildControlRHS(double const & DeltaAngle);
	void ComputeBodyAxisInertia();
	void ComputeResults();
	void Forces(double *Gamma,double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace = false);
	void VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll);
	void VLMSetDownwash(double *Gamma);
	void VLMSetAi(double *Gamma);
	bool ControlLoop();
	bool SolveUnitSpeeds();
	bool BuildInfluenceMatrix();
	bool ComputeTrimmedConditions();
	double VLMComputeCm(double alpha) ;
	void GetNDStabDerivatives(CWOpp *pNewPoint);
	
	CVector GetSpeedVector(CVector C, double *Gamma);

	void BuildRotationMatrix();
	void ComputeStabilityInertia();
	void ComputeControlDerivatives();
	void ComputeStabilityDerivatives();
	void BuildStateMatrices();
	void SetControlPositions(double t);
	bool SolveEigenvalues();

	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;

	bool m_bSequence;
	bool m_bWarning;
	bool m_bCancel;

	int m_State;

	static void *s_pMiarex;
	static void *s_pMainFrame;

	QFile *m_pXFile;

	int m_Index[VLMMATSIZE];
	double *m_RHS;
	double *m_aij, *m_aijRef;
	double *m_Gamma;
	double m_uRHS[VLMMATSIZE], m_vRHS[VLMMATSIZE], m_wRHS[VLMMATSIZE];
	double m_pRHS[VLMMATSIZE], m_qRHS[VLMMATSIZE], m_rRHS[VLMMATSIZE];
	double m_cRHS[VLMMATSIZE*MAXCONTROLS];
	double m_Cp[VLMMATSIZE];//lift coef per panel

	double m_AlphaEq, m_Ctrl;

	double m_ControlMin, m_ControlMax, m_ControlDelta;
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_CX, m_CY; //Force summation
	double m_XCP, m_YCP;
	double *m_pCoreSize;

	double m_VCm, m_VYm;            //Viscous moments
	double m_ICm, m_IYm;            // Induced Moments, by pressure forces, or in Trefftz plane
	double m_GCm, m_GRm, m_GYm;     // Total Moments

	QString m_strOut;
	QString m_VersionName;


	CPanel *m_pMemPanel;    //pointer to the reference panels array created in Miarex

	CPanel  *m_pPanel;      // the original array of panels
	CVector *m_pNode;       // the working array of Nodes
	CVector *m_pMemNode;    // a copy of the reference node array for tilted calcs

	CPlane *m_pPlane;
	CWing  *m_pWing;
	CWing  *m_pWing2;//pointer to the second wing if Biplane
	CWing  *m_pStab;
	CWing  *m_pFin;
	CWPolar *m_pWPolar;
	CSurface *m_pSurface[2*MAXPANELS];	// An array with the pointers to the wings surfaces

	double m_radius, m_W, m_p, m_q, m_r; //trimmed flight data

	bool m_bIsFinished;
	bool m_bXFile;
	bool m_bTrace;
	bool m_bConverged;
	bool m_bPointOut;
	bool m_bTrefftz;

	int m_MatSize;
	int m_nNodes;
	int m_NSurfaces;
	int m_NCtrls;

	CSurface **m_ppSurface;

	//Equilibrium Forces and moments
	CVector Force0, Moment0; 

	// longitudinal stability derivatives
	double Xu, Xw, Zu, Zw, Zq, Mu, Mw, Mq;//first order
	double Zwp, Mwp;                  //second order

	// latal stability derivatives
	double Yv, Yp, Yr, Lv, Lp, Lr, Nv, Np, Nr;//first order

	//stability control derivatives
	double Xde[MAXCONTROLS], Yde[MAXCONTROLS], Zde[MAXCONTROLS];
	double Lde[MAXCONTROLS], Mde[MAXCONTROLS], Nde[MAXCONTROLS];
	
	double u0;     //steady state x-velocity
	double Theta0; //steady state pitch angle
//	double m_BankAngle;
	double m_ALong[4][4];	//Longitudinal State Matrix
	double m_ALat[4][4];	//Lateral State Matrix
	double m_BLong[MAXCONTROLS][4];
	double m_BLat[MAXCONTROLS][4];
	double m_R[3][3];	//Rotation matrix

	//COG position in Body axis
	CVector m_CoG;
	double m_Mass;

	double m_Ib[3][3];	//Inertia tensor in body axis
	double m_Is[3][3];	//Inertia tensor in stability axis

	complex<double> m_rLong[4],  m_rLat[4]; // complex eigenvalues
	complex<double> m_vLong[16], m_vLat[16];// complex eigenvecctors


};

#endif // STABANALYSISDLG_H
