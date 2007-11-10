/****************************************************************************

    Wing Class
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

// Wing.h : header file
//
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "../misc/ClrClasses.h"
#include "Surface.h"
#include "Panel.h"
#include "PanelListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CWing dialog


class CWing : public CDialog
{ 
	friend class CImportWingDlg;
	friend class CMainFrame;
	friend class CWngAnalysis;
	friend class CCtrlAnalysis;
	friend class CWOper;
	friend class CWOperDlgBar;
	friend class CLLTThread;
	friend class CVLMThread;
	friend class CVLMDlg;
	friend class CMiarex;
	friend class CListWing;
	friend class CLLTDlg;
	friend class CGeomPage;
	friend class CFoilsPage;
	friend class CVLMMeshDlg;
	friend class CPlane;
	friend class CNameDlg;
	friend class CManageDlg;
	friend class CPOperDlgBar;
	friend class C3DPanelDlg;
	friend class CPanelListCtrl;
	friend class CUFOListDlg;
	// Construction
public:

	CWing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWing)
	enum { IDD = IDD_WING };
	CStatic	m_ctrlSweep;
	CButton	m_ctrlVLMMesh;
	CStatic	m_ctrlLength5;
	CStatic	m_ctrlYMac;
	CButton	m_ctrlLeftWing;
	CButton	m_ctrlSymetric;
	CClrBtn m_ctrlWingColor;
	CStatic	m_ctrlArea;
	CStatic	m_ctrlVolumeUnit;
	CStatic	m_ctrlVolume;
	CStatic	m_ctrlLength3;
	CStatic	m_ctrlLength2;
	CStatic	m_ctrlLength1;
	CStatic	m_ctrlAeroChord;
	CEdit	m_ctrlWingName;
	CStatic	m_ctrlSpan;
	CButton	m_ctrlCancel;
	CButton	m_ctrlOK;
	CNumEdit m_ctrlVLMPanels;
	CNumEdit m_ctrlnFlaps;
	CStatic	m_ctrlMeanChord;
	CStatic	m_ctrlTaperRatio;
	CStatic	m_ctrlSurface;
	CStatic	m_ctrlAspectRatio;
	CPanelListCtrl m_ctrlPanelList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWing)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//define all we can as static, to save space
	//static variables will be common to all wings...  
	//... but are generated at each call to CWing::DoModal();
	static double m_DTChord[MAXPANELS+1];// Same for display, with converted units
	static double m_DTPos[MAXPANELS+1];
	static double m_DTOffset[MAXPANELS+1];// b-position of each panel end
	CSurface m_Surface[MAXVLMSURFACES];//we can have three wings at a time, wing, elev, fin
	double m_VLMQInf[100];

	bool CreateSurfaces(CVector T, double XTilt, double YTilt);//generic surface, LLT, VLM or Panel

	bool VLMSetAutoMesh(int total = 0);
	int  VLMGetPanelTotal(void);
	void VLMSetBending();
	void VLMTrefftz(double *Gamma, double & Lift, double & Drag, bool bWakeRollUp);
	void VLMComputeWing(double *Gamma, double *Ai, double *Cp, double &Lift, double &IDrag,  double &VDrag, double &XCP, double &YCP,
						double &Pm, double &Rm, double &IYm, double &GYm, bool bViscous);
	bool LLTInitialize();
	bool LLTSetLinearSolution();
	void LLTInitCl();
	void LLTComputeWing();
	int  LLTIterate();

//	void PanelComputeWing(double *Ai, double *Cp, double &Lift, double &IDrag,  double &VDrag, double &XCP, double &YCP,
//						double &Pm, double &Rm, double &IYm, double &GYm, bool bViscous);

	CString GetFormat(double f, int precision);

	void Convert(bool bSet);
	void GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t);
	void Getjk(double y, int &j, int &k);
	void DrawWing(CDC* pDC, CPoint O);
	void DrawVLMMesh(CDC* pDC, CPoint O);
	void DrawFoils(CDC* pDC, CPoint O);
	void DrawDihedral(CDC* pDC, CPoint O);
	void Duplicate(CWing *pWing);
	void ComputeChords(int NStation=0);
	void ComputeDihedrals();
	void ComputeGeometry();
	void FillPanelList();
	void GetxDist(int l, int NXPanels, int XDistType, double &xA1, double &xA2, double &xB1, double &xB2, CFoil* pFoilA,  CFoil* pFoilB);
	void GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos);
	void InsertSection(double TPos);
	void InsertSection(double TPos, double TChord, double TOffset, double TZPos, double Twist, CString Foil,int NChord, int NSpan, int SSpan);
	void PanelComputeWing(double *Ai, double *Cp, double &Lift,  double &IDrag, double &VDrag, double &XCP, double &YCP,
							 double &Pm, double &Rm, double &IYm, double &GYm, bool bViscous);
	void SetResults();
	void SetParams();
	void SetSectionData();
	void SetScale();
	void SplitPanel(int i, double tau);
	void SetSweep(double Sweep);
	void SetTwist(double Twist);
	void ScaleSpan(double NewSpan);
	void ScaleChord(double NewChord);
	void ReadParams();
	void ReadSectionData(int sel);
	bool Gauss(double *A, int n, double *B, int m);
	bool CheckWing();
	bool SerializeWing(CArchive& ar);
	bool ExportAVLWing(CStdioFile *pXFile, double x, double z, double Thetax, double Thetay, double bFin);
	int IsFoil(CPoint point);
	double Beta(int m, int k);
	double Eta(int m);
	double Sigma(int m);
	double AlphaInduced(int k);
	double GetAverageSweep();
	double GetChord(double yob);
	double GetC4(double yob, double xRef);
	double GetLE(double yob);
	double GetDihedral(double yob);
	double GetOffset(double yob);
	double GetTwist(double y);
	double GetZPos(double y);
	double Getyrel(double SpanPos);
	double GetViewZPos(double xrel, double y, int pos =0);
	double GetTopz(double x, double y);
	double GetBotz(double x, double y);
	double IntegralC2(double y1, double y2, double c1, double c2);
	double IntegralCy(double y1, double y2, double c1, double c2);


	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnInsertBefore();
	afx_msg void OnDeleteInput();
	afx_msg void OnSymetric();
	afx_msg void OnSide();
	afx_msg void OnVLMMesh();
	afx_msg void OnResetscale();
	afx_msg void OnWingColor();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnContextMenu(CWnd* pWnd , CPoint point);
	afx_msg void OnAppend();
	afx_msg void OnNMClickPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndLabelEditPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateMeshFromTotal();

//__________________________Variables_______________________

	CWnd* m_pFrame;		//pointer to the Frame window
	CWnd* m_pMiarex;	//pointer to the Miarex Application window
	CWnd* m_pVLMDlg;	//pointer to the VLM analysis dialog class
	CWnd* m_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class

	CString m_WingName;	//the wing's name

	CStdioFile * m_pXFile;	// a pointer to the output .log file
	CFont m_FixedFont;	
	
	bool m_bInitCalc;	//
	bool m_bVLMAutoMesh; 	// true if the mesh should be set automatically
	bool m_bWingOut;	// true if the wing OpPoint is outside the flight envelope of the available Type1 polars
	bool m_bConverged;	// true if LLT || VLM calculation has converged
	bool m_bTrace;		// true if the messages need to be traces
	bool m_bSymetric;	// true if the wing's geometry is symmetric
	bool m_bVLMSymetric;	// true if the vlm calculation is symmetric
	bool m_bRightSide;	// true if the wing's right side is selected for display
	bool m_bCheckName;	// true if the wing's name should be checked 
	bool m_bLLT;		// true if performing an LLT calculation, false if VLM
	bool m_bTrans;		// true if the wing is being dragged 
	bool m_bIsFin, m_bDoubleFin, m_bSymFin, m_bDoubleSymFin; //fin flags
	bool m_bVLM1;		// true if classic, false if quad ring (K&P) method
	bool m_bCancel;		// true if the calculation is to be aborted
	bool m_bChanged;	// true if the user has made changes to the wing's definition

	int m_Type;		// the LLT analysis type
	int m_NPanel;		// number of span panels in wing definition
	int m_NStation;		// number of stations for wing calculation
	int m_iSection;		// the currently selected section
	int m_nNodes;		// the number of VLM panel nodes
	int m_NSurfaces; 	// The number of VLM Surfaces (=2 x Wing Panels)
	int m_MatSize;	// Max Size for the VLMMatrix
	int m_AVLIndex;		// need to export to AVL

	int m_nFlaps;		// the number of T.E. flaps, numbered from left wing to right wing
	double m_FlapMoment[20];//max 10 flaps x 2

	double pi;		//
	double m_Span;		// wing span
	double m_GChord;	// mean geometric chord
	double m_MAChord;	// mean aerodynamic chord
	double m_yMac;		// mean aerodynamic chord span position
	double m_Density, m_Viscosity; //fluid properties
	double m_Area;		// wing surface
	double m_Volume;	// for tentative wieght calculations
	double m_AR;		// Aspect ratio
	double m_TR;		// Taper ratio
	double m_CL;		//Lift
	double m_InducedDrag, m_ViscousDrag;
	double m_XCP, m_YCP;	// Centre of pressure's position
	double m_PitchingMoment, m_VCm, m_GCm;// Total, Viscous, Geometric
	double m_RollingMoment;
	double m_IYm;		// Induced Yawing Moment
	double m_GYm;		// Geometric Yawing Moment
	double m_QInf;		// Freestream speed
	double m_QInf0;		// Freestream speed for initial Type 2 analysis
	double m_Weight;	// For type 2 analysis
	double m_Alpha;		// angle of attack
	double m_Maxa; 		// Used in LLT
	double m_CvPrec;	// Precision required for LLT convergence
	double m_RelaxMax;	// relaxation factor for LLT convergence
	double m_fWingScale;	// scale to display the wing in the dialog box
	double m_XCmRef;	// the reference point position for moment calculations

	int m_NXPanels[MAXPANELS]; 		// VLM Panels along chord, for each Wing Panel
	int m_NYPanels[MAXPANELS]; 		// VLM Panels along span, for each Wing Panel
	int m_XPanelDist[MAXPANELS];		// VLM Panel distribution type, for each Wing Panel
	int m_YPanelDist[MAXPANELS];		// VLM Panel distribution type, for each Wing Panel
	double m_TChord[MAXPANELS+1];		// Chord length at each panel side
	double m_TLength[MAXPANELS+1];		// the length of each panel
	double m_TPos[MAXPANELS+1];		// b-position of each panel end on developed surface
	double m_TYProj[MAXPANELS+1];		// b-position of each panel end projected on horizontal surface
	double m_TOffset[MAXPANELS+1];		// b-position of each panel end
	double m_TDihedral[MAXPANELS+1];	// b-position of each panel end
	double m_TZPos[MAXPANELS+1];		// vertical offset - calculation result only
	double m_TTwist[MAXPANELS+1];		//Twist value of each foil (measured to the wing root)

	// Span Coefficients  resulting from VLM or LLT calculation
	double m_Ai[MAXSTATIONS+1];		//Induced angles, in degrees
	double m_Cl[MAXSTATIONS+1];		//Lift coefficient at stations
	double m_PCd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_ICd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_Cm[MAXSTATIONS+1];		//Pitching moment coefficient at stations
	double m_CmAirf[MAXSTATIONS+1];		//Airfoil part of Pitching moment coefficient at stations
	double m_CmGeom[MAXSTATIONS+1];		//Geometric part of Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_XCPSpanAbs[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_Re[MAXSTATIONS+1];		//Reynolds number at stations
	double m_Chord[MAXSTATIONS+1];		//chord at stations
	double m_Offset[MAXSTATIONS+1];		//offset at LLT stations
	double m_XTrTop[MAXSTATIONS+1];		//Upper transition location at stations
	double m_XTrBot[MAXSTATIONS+1];		//Lower transition location at stations
	double m_Twist[MAXSTATIONS+1];		//twist at LLT stations
	double m_BendingMoment[MAXSTATIONS+1];	//bending moment at stations
	double m_SpanPos[MAXSTATIONS+1];	//span positions of LLT stations
//	double m_dy[MAXSTATIONS];	

	CVector m_GeomMoment[MAXSTATIONS+1];	//geometric pitching moment at stations
	
	CVector *m_pNode;			//pointer to the VLM node array
	CPanel *m_pPanel;			//pointer to the VLM Panel array

	CVector *m_pWakeNode;			//pointer to the VLM wake node array
	CPanel *m_pWakePanel;			//pointer to the VLM Wake Panel array

	CStringArray m_RFoil;			// name of the right foils
	CStringArray m_LFoil;			// name of the left foils

	CVector m_Vd[MAXSTATIONS];		// downwash vector at span stations
	CVector m_F[MAXSTATIONS];		// lift vector at span stations

	CRect m_DrawRect;			// the drawing rectangle in the dialog box

	CPoint m_PointDown;			// the last client point wherer the user has left-clicked
	CPoint m_ptOffset;			// the offset point to display the wing

	COLORREF m_WingColor;


	DECLARE_MESSAGE_MAP()

public:
};
