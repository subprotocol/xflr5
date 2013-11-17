/****************************************************************************

	PanelAnalysisDlg Class
	Copyright (C) 2009-2013 Andre Deperrois adeperrois@xflr5.com

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
 

/**
 *@file
 *
 * This file contains the PanelAnalysisDlg class, which is used to perform VLM and 3D-Panel analysis
 *
 */

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

#define VLMMAXRHS 100

/**
 *@class PanelAnalysisDlg
 *@brief The class is used to perform VLM and 3D-ponel analysis
 
 For convenience, the class is derived from the QDialog class. 
 The dialog interface allows the user to visualize the progress of the analysis, and to interrupt it as necessary.
 Ideally for oop good practices, the GUI should be implemented in a class separate from the class which performs the analysis.
 
 All calculations are performed, and all results are stored, in International Standard Units, i.e. m, kg, s, Newtons
 
 * Note: this class has grown quite complex with time, and would need to be revisited and simplified.
 * Improvement : remove the option for symetry, benefit is not worth the increased complexity of the implementation
 */
class PanelAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;
	friend class Wing;

public:
    PanelAnalysisDlg(QWidget *pParent);
	~PanelAnalysisDlg();

	bool InitDialog();

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
	bool QInfLoop();
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
	void CreateRHS(double *RHS, CVector VInf, double *VField = NULL);
	void CreateUnitRHS();
	void CreateWakeContribution();
	void CreateWakeContribution(double *pWakeContrib, CVector WindDirection);
	void DoubletNASA4023(CVector const &C, Panel *pPanel, CVector &V, double &phi, bool bWake=false);
	void GetDoubletInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi, bool bWake=false, bool bAll=true);
	void GetSourceInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi);
	void ScaleResultstoSpeed(int nval);
	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);
	void SetFileHeader();
	void SourceNASA4023(CVector const &C, Panel *pPanel, CVector &V, double &phi);
	void SetupLayout();
	void StartAnalysis();
	void SumPanelForces(double *Cp, double Alpha, double &Lift, double &Drag);
	void UpdateView();
	void WriteString(QString strong);
	void VLMGetVortexInfluence(Panel *pPanel, CVector const &C, CVector &V, bool bAll);
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
	void Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bTilted);
	double ComputeCm(double Alpha);

	bool AllocateMatrix(int &memsize);
	bool AllocateRHS(int &memsize);
	void Release();

public:
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll=true);
	void GetSpeedVector(CVector const &C, float *Mu,  float *Sigma,  CVector &VT, bool bAll=true);
	
private:
	static void *s_pMiarex;          /**< a void pointer to the instance of the QMiarex class. */
	static void *s_pMainFrame;       /**< a void pointer to the instance of the MainFrame class. */

	static int s_MaxRHSSize;         /**< the max number of RHS points, used for memeory allocation */

	QFile *m_pXFile;                /**< A pointer to the instance of the output log file */

	bool m_bIsFinished;         /**< true if the analysis is completed */
//	bool m_bType4;
//	bool m_bXFile;
	bool m_b3DSymetric;         /** true if the symetry properties should be used to reduce the size of the problem */
	bool m_bPointOut;           /** true if an interpolation was outside the min or max Cl */
	bool m_bInverted;           /** true if the resolution of the linear system has been successful */
//	bool m_bDirichlet;// true if Dirichlet boundary conditions, false if Neumann
	bool m_bTrefftz;            /** true if the forces should be evaluated in the far-field plane rather than by on-body summation of panel forces */
	bool m_bSequence;           /** true if the calculation is should be performed for a range of aoa */
	bool m_bCancel;             /**< true if the user has cancelled the analysis */
	bool m_bExit;               /**< true if the analysis should be interrupted - either skipped or cancelled */
	bool m_bSkip;               /**< true if the user has requested to skip the on-going OpPoint calculation. The analysis will move on to the calculation of the next OpPoint in sequence */
	bool m_bWarning;            /**< true if one the OpPoints could not be properly interpolated */
	bool m_bWakeRollUp;         /**< true if wake roll-up is enabled. This option is disabled */

    int m_nRHS;                 /**< the number of RHS to calculate; cannot be greater than VLMMAXRHS */
	int m_nNodes;               /**< the number of nodes  */
	int m_MatSize;              /**< the number of panels. Is also the size of the linear problem */
	int m_SymSize;              /**< the size of the matrix, excluding symetric elements */
	int m_NSurfaces;            /**< the number of Surface objects used to define the plane */
	int m_nWakeNodes;           /**< the number of wake nodes */
	int m_WakeSize;	            /**< the number of wake elements */
	int m_NWakeColumn;          /**< the number of wake columns, which is also the number of panels in the spanwise direction */
	int m_WakeInterNodes;
	int m_MaxWakeIter;          /** the max number of wake roll-up iterations. Disabled. */

	double m_Progress;          /**< A measure of the progress of the analysis, used to provide feedback to the user */

	double m_AlphaMax;          /**< The ending aoa for the analysis of type 1 & 2 polars */
	double m_AlphaDelta;        /**< The aoa increment for the analysis of type 1 & 2 polars */
	double m_Alpha;             /**< The angle of attack of the current calculation, in degree */
	double m_OpAlpha;           /**< The angle of attack of the current calculation used in Tilted analysis, in degree */

	double m_CL;                /**< The lift coefficient */
	double m_CX;                /**< The drag coefficient */
	double m_CY;                /**< The side force coefficient */
	
 	double m_InducedDrag;       /**< The UFO's induced drag coefficient */
	double m_ViscousDrag;       /**< The UFO's viscous drag coefficient */
	double m_VCm;               /**< The UFO's viscous pitching moment coefficient */
	double m_VYm;               /**< The UFO's viscous yawing moment coefficient */
	double m_ICm;               /**< The UFO's induced pitching moment coefficient */
	double m_IYm;               /**< The UFO's induced yawing moment coefficient */
	double m_GCm;               /**< The UFO's total pitching moment coefficient */
	double m_GRm;               /**< The UFO's total rolling moment coefficient */
	double m_GYm;               /**< The UFO's total yawing moment coefficient */
	double m_QInf;              /**< The value of the velocity used in the current calculation */
	double m_QInfMax;           /**< The max value of the velocity for the analysis of type 4 polars */
	double m_QInfDelta;         /**< The increment value of the velocity for the analysis of type 4 polars */

	CVector m_CP;               /**< The position of the center of pressure */

	double eps;
	double phiw, rz;
	double RFF;                 /**< factor used to evaluate the distance of a point to a panel */
	double *m_pCoreSize;        /**< A pointer to the value radius of the vortex core */
	double side, sign, dist, S, GL;
	double RNUM, DNOM, PN, A, B, PA, PB, SM, SL, AM, AL, Al, pjk, CJKi;

	double XNP;                 /**< Neutral point x-position resulting from stability analysis */
	double CXu, CZu, Cmu, CXq, CZq, Cmq, CXa, CZa, Cma; // Non dimensional stability derivatives
	double CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	double CXe, CYe, CZe, Cle, Cme, Cne;

	double *m_aij;           /**< coefficient matrix for the panel analysis. Is declared as a common member variable to save memory allocation times*/
	double *m_aijWake;       /**< coefficient matrix. Is declared as a common member variable to save memory allocation times*/
	double *m_RHS;           /**< RHS vector. Is declared as a common member variable to save memory allocation times */
	double *m_RHSRef;        /**< RHS vector. Is declared as a common member variable to save memory allocation times */
	double *m_SigmaRef;
	double *m_Sigma;         /**< The array of resulting source strengths of the analysis */
	double *m_Mu;            /**< The array of resulting doublet strengths, or vortex circulations if the panel is located on a thin surface */
	double *m_Cp;            /**< The array of lift coef per panel */
	double *m_3DQInf;        /**< a pointer to the calculated balance speeds for each aoa in Type 2 and Type 7 analysis */

	CVector *m_uVl, *m_wVl;
	double *m_uRHS, *m_vRHS, *m_wRHS;
	double *m_pRHS, *m_qRHS, *m_rRHS;
	double *m_cRHS;
	double *m_uWake, *m_wWake;

	int *m_Index;               /**< a point to the array of indexes used in matrix LU decomposition */

	CVector *m_Speed;           /**< a pointer to the calculated surface speeds in a panel analysis */

	QString m_strOut;
	
	Panel *m_pPanel;           /**< the current working array of array of panels */
	Panel *m_pWakePanel;       /**< the current working array of wake panel array */
	Panel *m_pRefWakePanel;    /**< a copy of the reference wake node array if wake needs to be reset */
	Panel *m_pMemPanel;        /**< a copy of the reference panel array if the panels need to be restored, for instance after control surfaces have been rotated*/

	CVector *m_pNode;	       /**< the working array of nodes  */
	CVector *m_pMemNode;	   /**< a copy of the reference node array, if the nodes need to be restored */
	CVector *m_pWakeNode;	   /**< the current working wake node array */
	CVector *m_pRefWakeNode;   /**< a copy of the reference wake node array if the flat wake geometry needs to be restored */

	
	WPolar *m_pWPolar;         /**< a pointer to the current WPolar object */
	Body *m_pBody;             /**< a pointer to the plane's body, or NULL if none */
	Plane *m_pPlane;           /**< a pointer to the plane object, or NULL if the calculation is performed on a wing */

	//temp data
	CVector VG, CG;
	double phiG;
//	Panel m_SymPanel;
	CVector R[5];
	CVector r0, r1, r2, Psi, t, Far;
	double r1v,r2v,ftmp, Omega;
	CVector *m_pR[5];
	CVector PJK, a, b, s, T1, T2, T, h;

public:
	double m_Ai[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];    /**< The array of calculated induced angles */
	double m_Cl[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];    /**< The array of calculated lift coefficients */
	double m_ICd[MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];    /**< The array of calculated induced angles */
	CVector m_F[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];    /**< The array of calculated forces on each chordwise strip */
	CVector m_Vd[ MAXWINGS*VLMMAXRHS * MAXSPANSTATIONS];   /**< The array of calculated downwash velocity at the T.E. of each chordwise strip */
	CVector m_WingForce[MAXWINGS*VLMMAXRHS];               /**< The array of calculated resulting forces acting on the Wing objects */
	double m_WingIDrag[MAXWINGS*VLMMAXRHS];                /**< The array of calculated resulting induced drag acting on the Wing objects */
	Wing *m_pWing;                                         /**< A pointer to the instance of the Wing, if the claculation is performed on a single wing, NULL otherwise */
	Wing * m_pWingList[MAXWINGS];                          /**< The array of pointers to the plane's Wing objects */

public: //stability analysis method and variables

	int m_NCtrls;  /**< The total number of control surfaces */
	
	CVector Force0;  /** The calculated equilibrium force */
	CVector Moment0; /** The calculated equilibrium moment */

	// longitudinal stability derivatives
	double Xu, Xw, Zu, Zw, Xq, Zq, Mu, Mw, Mq;//first order
	double Zwp, Mwp;                          //second order derivatives, cannot be calculated by a panel method, set to zero.

	// latal stability derivatives
	double Yv, Yp, Yr, Lv, Lp, Lr, Nv, Np, Nr;//first order

	//stability control derivatives
	double Xde, Yde, Zde, Lde, Mde, Nde;

	double u0;              /**< steady state x-velocity, used in stability analysis */
	double Theta0;          /**< steady state pitch angle, used in stability analysis */
//	double m_BankAngle;
	double m_ALong[4][4];	/**< The longitudinal state matrix */
	double m_ALat[4][4];	/**< The lateral state matrix */
    double m_BLong[4];      /**< The longitudinal control vector */
    double m_BLat[4];       /**< The lateral control vector */
	double m_R[3][3];	    /**< Rotation matrix, used as a temp variable in the calculations */


	double m_Ib[3][3];	          /**< Inertia tensor in body (geometrical) axis */
	double m_Is[3][3];	          /**< Inertia tensor in stability axis */

	complex<double> m_rLong[4];   /**< complex longitudinal eigenvalues resulting from the stability analysis*/
	complex<double> m_rLat[4];    /**< complex lateral eigenvalues resulting from the stability analysis*/
	complex<double> m_vLong[16];  /**< complex longitudinal eigenvectors resulting from the stability analysis*/
	complex<double> m_vLat[16];   /**< complex lateral eigenvectors resulting from the stability analysis*/

	double m_AlphaEq;             /**< the balance aoa, calculated in stability analysis */
	double m_Ctrl;                /**< the control parameter, which defines the position of the control surfaces */
	double m_ControlMin;          /**< The starting control parameter for stability analysis */
	double m_ControlMax;          /**< The ending control parameter for stability analysis */
	double m_ControlDelta;        /**< The control parameter increment for stability analysis */

	Surface **m_ppSurface;        /**< A pointer to the array of Surface objects */
	double m_radius, m_W, m_p, m_q, m_r; //trimmed flight data

	bool m_bTrace;
};

#endif // PANELANALYSISDLG_H














