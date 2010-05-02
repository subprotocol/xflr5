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


#ifndef VLMANALYSISDLG_H
#define VLMANALYSISDLG_H

#include <QDialog>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include "../Objects/WPolar.h"
#include "../Objects/Plane.h"
#include "../Objects/CVector.h"


class VLMAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;
	friend class CWing;

public:
    VLMAnalysisDlg();

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

	bool AlphaLoop();
	bool ReLoop();
	bool UnitLoop();
	bool ControlLoop();
	bool SetControlPositions(double t);
	bool VLMCreateUnitRHS();
	bool VLMSolveDouble();
	bool VLMCreateMatrix();
	bool VLMSolveMultiple(double V0, double VDelta, int nval);
	void VLMComputePlane(double V0, double VDelta, int nrhs);
	void VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll);
	void VLMSetDownwash(double *Gamma);
	void VLMSetAi(double *Gamma);
	double VLMComputeCm(double alpha) ;
	void pgmat(double const &mach, double const &alfa, double const &beta, double pg[3][3]);


	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;

	bool m_bSequence;
	bool m_bCancel, m_bWarning;

	int m_State;

	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_ReMin, m_ReMax, m_ReDelta;

	static void *s_pMiarex;
	static void *s_pMainFrame;

	QFile *m_pXFile;

	double *m_RHS;
	double *m_aij;
	double *m_Gamma;
	double m_xRHS[VLMMATSIZE], m_yRHS[VLMMATSIZE], m_zRHS[VLMMATSIZE];
	double m_Cp[VLMMATSIZE];//lift coef per panel
	double m_VLMQInf[100];
	double m_OpAlpha, m_OpQInf, m_Ctrl;
	double m_QInfMin, m_QInfMax, m_QInfDelta;
	double m_ControlMin, m_ControlMax, m_ControlDelta;
	double m_CL, m_ViscousDrag, m_InducedDrag;
	double m_CX, m_CY; //Force summation
	double m_XCP, m_YCP;
	double Omega;
	double ftmp;
	double *m_pCoreSize;

	double m_VCm, m_VYm; //Viscous moments
	double m_ICm, m_IYm;		// Induced Moments, by pressure forces, or in Trefftz plane
	double m_GCm, m_GRm, m_GYm;		// Total Moments

	QString m_strOut;
	QString m_VersionName;

	CPanel *m_pMemPanel;//pointer to the reference panels array created in Miarex

	CPanel *m_pPanel; 	// the original array of panels
	CPanel **m_ppPanel;	// the re-ordered array of pointers to the panels
	CPanel  *m_pWakePanel;
	CPanel  *m_pRefWakePanel;

	CVector R[5];
	CVector *m_pNode;	// the working array of Nodes
	CVector *m_pMemNode;	// a copy of the reference node array for tilted calcs
	CVector *m_pWakeNode;	// the current working wake node array
	CVector *m_pRefWakeNode; // the reference wake node array if wake needs to be reset
	CVector *m_pTempWakeNode;// the temporary wake node array during relaxation calc


	CPlane *m_pPlane;
	CWing *m_pWing2;//pointer to the second wing if Biplane
	CWing *m_pStab;
	CWing *m_pFin;
	CWPolar *m_pWPolar;

	CVector AA, BB, AA1, BB1, AAG, BBG, V, VT, VS, CG, VG, D;
	CVector Far;
	CVector r0, r1, r2;
	CVector h,t;
	CVector Psi;

	double r1v, r2v;

	bool m_bIsFinished;
	bool m_bXFile;
	bool m_bVLMSymetric;
	bool m_bWakeRollUp;
	bool m_bTrace;
	bool m_bConverged;
	bool m_bPointOut;
	bool m_bTrefftz;

	int m_MatSize;
	int m_nNodes;
	int m_NSurfaces;
	int m_MaxWakeIter;
	int m_WakeInterNodes;
	int m_nWakeNodes;
	int m_WakeSize;// Max Size for the VLMMatrix
	int m_NWakeColumn;
public:
	CWing  *m_pWing;
	void GetSpeedVector(CVector const &C, double *Gamma, CVector &VTot);
};

#endif // VLMANALYSISDLG_H
