/****************************************************************************

	PanelAnalysis Class
	Copyright (C) 2006-2014 Andre Deperrois adeperrois@xflr5.com

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



#ifndef PANELANALYSIS_H
#define PANELANALYSIS_H

#include "../../objects/CVector.h"
#include "../../objects/Panel.h"
#include "../../objects/WPolar.h"
#include "../../objects/Plane.h"
#include "../../objects/Body.h"

#define VLMMAXRHS 100


class PanelAnalysis
{
	friend class PanelAnalysisDlg;
	friend class Wing;
	friend class Objects3D;
	friend class QMiarex;

public:
	PanelAnalysis();
	~PanelAnalysis();

	bool initializeAnalysis();



private:
	bool SolveUnitRHS();

	bool loop();
	bool alphaLoop();
	bool QInfLoop();
	bool unitLoop();
	bool controlLoop();

	bool GetZeroMomentAngle();

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
	void GetDoubletInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi, bool bWake=false, bool bAll=true);
	void GetSourceInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi);
	void ScaleResultstoSpeed(int nval);
	void SumPanelForces(double *Cp, double Alpha, double &Lift, double &Drag);
	void VLMGetVortexInfluence(Panel *pPanel, CVector const &C, CVector &V, bool bAll);
	void VLMQmn(CVector &LA, CVector &LB, CVector &TA, CVector &TB, CVector const &C, CVector &V);
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
	void Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bViscous);
	double ComputeCm(double Alpha);

	bool AllocateMatrix(int matSize, int &memsize);
	bool AllocateRHS(int matSize, int &memsize);
	void ReleaseArrays();

	void traceLog(QString str);


	void restorePanels();
	void setArrayPointers(void *pPanel, void *pMemPanel, void *pWakePanel, void *pRefWakePanel, void *pNode, void *pMemNode, void *pWakeNode, void *pRefWakeNode);
	void setArraySize(int MatSize, int WakeSize, int nNodes, int nWakeNodes, int NWakeColumn);
	void setRange(double vMin, double VMax, double vDelta, bool bSequence);
	void setObjectPointers(void *pPlane, void *pSurfaceList);
	void setParams(int nMaxWakeIter);
	void setWPolar(WPolar*pWPolar){m_pWPolar = pWPolar;}
	PlaneOpp* createPlaneOpp(double *Cp, double *Gamma, double *Sigma);


public:
	void GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll=true);

private:

	static bool s_bTrefftz;     /**< /true if the forces should be evaluated in the far-field plane rather than by on-body summation of panel forces */
	static bool s_bKeepOutOpp;  /**< true if points with viscous interpolation issues should be stored nonetheless */
	static bool s_bCancel;      /**< true if the user has cancelled the analysis */
	static int s_TotalTime;     /**< the esimated total time of the analysis, used to set the progress bar. No specific unit. */
	static double s_Progress;   /**< A measure of the progress of the analysis, used to provide feedback to the user */
	static bool s_bWarning;     /**< true if one the OpPoints could not be properly interpolated */

	static int s_MaxRHSSize;    /**< the max number of RHS points, used for memeory allocation >*/
	static int s_MaxMatSize;    /**< the size currently allocated for the influence matrix >*/

	bool m_bPointOut;           /**< true if an interpolation was outside the min or max Cl */
	bool m_bSequence;           /**< true if the calculation is should be performed for a range of aoa */
	bool m_bWakeRollUp;         /**< true if wake roll-up is enabled. This option is disabled */

	int m_nRHS;                 /**< the number of RHS to calculate; cannot be greater than VLMMAXRHS */
	int m_nNodes;               /**< the number of nodes  */
	int m_MatSize;              /**< the number of panels. Is also the size of the linear problem */
	int m_nWakeNodes;           /**< the number of wake nodes */
	int m_WakeSize;	            /**< the number of wake elements */
	int m_NWakeColumn;          /**< the number of wake columns, which is also the number of panels in the spanwise direction */
	int m_nMaxWakeIter;         /**< the max number of wake roll-up iterations. Disabled. */


	double m_vMin;              /**< The minimum value of the analysis parameter*/
	double m_vMax;              /**< The max value of the analysis parameter*/
	double m_vDelta;            /**< The increment value of the analysis parameter*/

	double m_Alpha;             /**< The angle of attack of the current calculation in degree >*/
	double m_OpAlpha;           /**< The angle of attack of the current calculation used in Tilted analysis, in degree >*/
	double m_QInf;              /**< The freestream velocity of the current calculation in m/s >*/
	double m_OpBeta;            /**< The sideslip angle of the current calculation, in degrees >*/
	
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

	CVector m_CP;               /**< The position of the center of pressure */

	double XNP;                 /**< Neutral point x-position resulting from stability analysis */
	double CXu, CZu, Cmu, CXq, CZq, Cmq, CXa, CZa, Cma; // Non dimensional stability derivatives
	double CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	double CXe, CYe, CZe, Cle, Cme, Cne;


	//analysis arrays
	double *m_RHS;           /**< RHS vector. Is declared as a common member variable to save memory allocation times */
	double *m_RHSRef;        /**< RHS vector. Is declared as a common member variable to save memory allocation times */
	double *m_SigmaRef;
	double *m_Sigma;         /**< The array of resulting source strengths of the analysis */
	double *m_Mu;            /**< The array of resulting doublet strengths, or vortex circulations if the panel is located on a thin surface */
	double *m_Cp;            /**< The array of lift coef per panel */
	double *m_3DQInf;        /**< a pointer to the calculated balance speeds for each aoa in Type 2 and Type 7 analysis */


	double *m_aij;           /**< coefficient matrix for the panel analysis. Is declared as a common member variable to save memory allocation times*/
	double *m_aijWake;       /**< coefficient matrix. Is declared as a common member variable to save memory allocation times*/
	double *m_uRHS, *m_vRHS, *m_wRHS;
	double *m_pRHS, *m_qRHS, *m_rRHS;
	double *m_cRHS;
	double *m_uWake, *m_wWake;
	CVector *m_uVl, *m_wVl;
	int *m_Index;               /**< a pointer to the array of indexes used in matrix LU decomposition */
	CVector *m_Speed;           /**< a pointer to the calculated surface speeds in a panel analysis */


	// pointers to the geometry input data
	// these arrays are defined in the QMiarex handling class,
	Panel *m_pPanel;           /**< the current working array of array of panels */
	Panel *m_pWakePanel;       /**< the current working array of wake panel array */
	Panel *m_pRefWakePanel;    /**< a copy of the reference wake node array if wake needs to be reset */
	Panel *m_pMemPanel;        /**< a copy of the reference panel array if the panels need to be restored, for instance after control surfaces have been rotated*/

	CVector *m_pNode;	       /**< the working array of nodes  */
	CVector *m_pMemNode;	   /**< a copy of the reference node array, if the nodes need to be restored */
	CVector *m_pWakeNode;	   /**< the current working wake node array */
	CVector *m_pRefWakeNode;   /**< a copy of the reference wake node array if the flat wake geometry needs to be restored */

	// pointers to the object input data
	Plane *m_pPlane;           /**< a pointer to the plane object, or NULL if the calculation is performed on a wing */
	WPolar *m_pWPolar;         /**< a pointer to the current WPolar object */

	//temp data
	int m_NSpanStations;
	CVector VG, CG;
	double phiG;
	CVector h, r0, r1, r2, Psi, t, Far;
	double r1v,r2v,ftmp, Omega;
	CVector *m_pR[5];

public:
	double *m_Ai;     /**< The array of calculated induced angles */
	double *m_Cl;     /**< The array of calculated lift coefficients */
	double *m_ICd;    /**< The array of calculated induced angles */
	CVector *m_F;     /**< The array of calculated forces on each chordwise strip */
	CVector *m_Vd;    /**< The array of calculated downwash velocity at the T.E. of each chordwise strip */
	CVector m_WingForce[MAXWINGS*VLMMAXRHS];               /**< The array of calculated resulting forces acting on the Wing objects */
	double m_WingIDrag[MAXWINGS*VLMMAXRHS];                /**< The array of calculated resulting induced drag acting on the Wing objects */
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

	QList<Surface*> *m_ppSurface;        /**< A pointer to the array of Surface objects */

	QList<PlaneOpp*> m_PlaneOppList;


	bool m_bTrace;
	QString m_OutMessage;
};

#endif // PANELANALYSIS_H
