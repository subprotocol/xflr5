/****************************************************************************

    Wing Class
    Copyright (C) 2005-2008 André Deperrois xflr5@yahoo.com

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
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "../misc/ClrClasses.h"
#include "PanelListCtrl.h"


class CWingDlg : public CDialog
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
	friend class CPlaneDlg;
	friend class CNameDlg;
	friend class CManageDlg;
	friend class CPOperDlgBar;
	friend class C3DPanelDlg;
	friend class CPanelListCtrl;
	friend class CUFOListDlg;
	friend class CControlAnalysis;

public:

	CWingDlg(CWnd* pParent = NULL);   
	CWingDlg(CWing *pWing);   
	virtual ~CWingDlg();

	enum { IDD = IDD_WINGDLG };
	CStatic	m_ctrlSweep;
	CStatic	m_ctrlLength5;
	CStatic	m_ctrlYMac;
	CStatic	m_ctrlArea;
	CStatic	m_ctrlVolumeUnit;
	CStatic	m_ctrlVolume;
	CStatic	m_ctrlLength3;
	CStatic	m_ctrlLength2;
	CStatic	m_ctrlLength1;
	CStatic m_ctrlTotal3DPanels;
	CStatic	m_ctrlAeroChord;
	CStatic	m_ctrlMeanChord;
	CStatic	m_ctrlTaperRatio;
	CStatic	m_ctrlSurface;
	CStatic	m_ctrlAspectRatio;
	CStatic	m_ctrlSpan;
	CClrBtn m_ctrlWingColor;
	CButton	m_ctrlVLMMesh;
	CButton	m_ctrlLeftWing;
	CButton	m_ctrlSymetric;
	CButton	m_ctrlCancel;
	CButton	m_ctrlOK;
	CEdit	m_ctrlWingName;
	CNumEdit m_ctrlVLMPanels;
	CPanelListCtrl m_ctrlPanelList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	//define all we can as static, to save space
	//static variables will be common to all wings...  
	//... but are generated at each call to CWing::DoModal();
	static double m_DTChord[MAXPANELS+1];// Same for display, with converted units
	static double m_DTPos[MAXPANELS+1];
	static double m_DTOffset[MAXPANELS+1];// b-position of each panel end

	bool VLMSetAutoMesh(int total = 0);
	int  VLMGetPanelTotal(void);

	CString GetFormat(double f, int precision);


	double IntegralC2(double y1, double y2, double c1, double c2);
	double IntegralCy(double y1, double y2, double c1, double c2);

	void CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB,
		               double *xPointA, double *xPointB, int &NXLead, int &NXFlap);
	void Convert(bool bSet);
	void DrawWing(CDC* pDC, CPoint O);
	void DrawVLMMesh(CDC* pDC, CPoint O);
	void DrawFoils(CDC* pDC, CPoint O);
	void DrawDihedral(CDC* pDC, CPoint O);
	void ComputeGeometry();
	void FillPanelList();
	void InsertSection(double TPos);
	void InsertSection(double TPos, double TChord, double TOffset, double TZPos, double Twist, CString Foil,int NChord, int NSpan, int SSpan);
	void SetResults();
	void SetParams();
	void SetSectionData();
	void SetScale();
	void SplitPanel(int i, double tau);
	void ReadParams();
	void ReadSectionData(int sel);
	bool CheckWing();
	int IsFoil(CPoint point);
	double GetAverageSweep();
	double GetChord(double yob);
	double GetOffset(double yob);
	double GetTwist(double y);
	double GetZPos(double y);

	void CopyDataTo(CWing *pWing);
	void CopyDataFrom(CWing *pWing);

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
	afx_msg void OnContextMenu(CWnd* pWnd , CPoint point);
	afx_msg void OnAppend();
	afx_msg void OnNMClickPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndLabelEditPanelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateMeshFromTotal();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

//__________________________Variables_______________________

	static CWnd* s_pFrame;		//pointer to the Frame window
	static CWnd* s_pMiarex;	//pointer to the Miarex Application window

	CWing *m_pWing;

	CString m_WingName;	//the wing's name

	CStdioFile * m_pXFile;	// a pointer to the output .log file
	CFont m_FixedFont;	
	
	bool m_bVLMAutoMesh; 	// true if the mesh should be set automatically
	bool m_bTrace;		// true if the messages need to be traces
	bool m_bSymetric;	// true if the wing's geometry is symmetric
	bool m_bRightSide;	// true if the wing's right side is selected for display
	bool m_bCheckName;	// true if the wing's name should be checked 
	bool m_bTrans;		// true if the wing is being dragged 
	bool m_bChanged;	// true if the user has made changes to the wing's definition
	bool m_bIsFin, m_bSymFin, m_bDoubleFin;

	int m_NPanel;		// number of span panels in wing definition
	int m_NStation;		// number of stations for wing calculation
	int m_iSection;		// the currently selected section

	int m_nFlaps;		// the number of T.E. flaps, numbered from left wing to right wing

	double pi;		//
	double m_Span;		// wing span
	double m_GChord;	// mean geometric chord
	double m_MAChord;	// mean aerodynamic chord
	double m_yMac;		// mean aerodynamic chord span position
	double m_Area;		// wing surface
	double m_Volume;	// for tentative wieght calculations
	double m_AR;		// Aspect ratio
	double m_TR;		// Taper ratio
	double m_fWingScale;	// scale to display the wing in the dialog box

	int m_NXPanels[MAXPANELS+1]; 		// VLM Panels along chord, for each Wing Panel
	int m_NYPanels[MAXPANELS+1]; 		// VLM Panels along span, for each Wing Panel
	int m_XPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel
	int m_YPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel
	double m_TChord[MAXPANELS+1];		// Chord length at each panel side
	double m_TLength[MAXPANELS+1];		// the length of each panel
	double m_TPos[MAXPANELS+1];		// b-position of each panel end on developed surface
	double m_TYProj[MAXPANELS+1];		// b-position of each panel end projected on horizontal surface
	double m_TOffset[MAXPANELS+1];		// b-position of each panel end
	double m_TDihedral[MAXPANELS+1];	// b-position of each panel end
	double m_TZPos[MAXPANELS+1];		// vertical offset - calculation result only
	double m_TTwist[MAXPANELS+1];		//Twist value of each foil (measured to the wing root)

	// Span Coefficients  resulting from VLM or LLT calculation
	double m_Chord[MAXSTATIONS+1];		//chord at stations
	double m_Offset[MAXSTATIONS+1];		//offset at LLT stations
	double m_Twist[MAXSTATIONS+1];		//twist at LLT stations
	double m_SpanPos[MAXSTATIONS+1];	//span positions of LLT stations
	int m_NHinges;				//number of different flap hinge positions across the wing

	CStringArray m_RFoil;			// name of the right foils
	CStringArray m_LFoil;			// name of the left foils

	CRect m_DrawRect;			// the drawing rectangle in the dialog box

	CPoint m_PointDown;			// the last client point wherer the user has left-clicked
	CPoint m_ptOffset;			// the offset point to display the wing

	COLORREF m_WingColor;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedPanellist(NMHDR *pNMHDR, LRESULT *pResult);
};
