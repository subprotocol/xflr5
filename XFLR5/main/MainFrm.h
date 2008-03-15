/****************************************************************************

    CMainFrame Class
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

#include "ChildView.h"
#include "../Miarex/Miarex.h"
#include "../Miarex/ScaleOppBar.h"
#include "../Miarex/BodyCtrlBar.h"
#include "../Miarex/SpanPosBar.h"
#include "../Miarex/WOperDlgBar.h"
#include "../Miarex/W3DBar.h"
#include "../Design/AFoil.h"
#include "../Design/AFoilCtrlBar.h"
#include "../XDirect/XDirect.h"
#include "../XDirect/OperDlgBar.h"
#include "../XDirect/XFoil.h"
#include "../Graph/CurveDlgBar.h"


class CMainFrame : public CFrameWnd
{
	friend class CXFLR5App;
	friend class CLLTDlg;
	friend class CBatchDlg;
	friend class CBody;
	friend class CPolar;
	friend class CViscDlg;
	friend class CXDirect;
	friend class CMiarex;
	friend class CAFoil;
	friend class CXInverse;
	friend class CChildView;
	friend class CListPlrDlg;
	friend class CCurveDlgBar;
	friend class CSelectFoilDlg;
	friend class CWPolar;
	friend class CWing;
	friend class CPlane;
	friend class CWOperDlgBar;
	friend class CPOperDlgBar;
	friend class CW3DBar;
	friend class CCtrlAnalysisDlg;
	friend class CWingScaleDlg;
	friend class CScaleOppBar;
	friend class CSpanPosBar;
	friend class CVLMDlg;
	friend class C3DPanelDlg;
	friend class CFlowLinesDlg;
	friend class CWAdvDlg;
	friend class CBatchThread;
	friend class CUFOListDlg;
	friend class CAFoilCtrlBar;
	friend class CBodyCtrlBar;
	friend class CWngAnalysis;
	friend class CCtrlAnalysis;
	friend class CBodyGridDlg;
	friend class CBodyNURBSDlg;


public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CString m_VersionName;
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_XDirectBar;
	CToolBar    m_MiarexBar;
	CToolBar    m_AFoilBar;
	CToolBar    m_XInverseBar;

	COperDlgBar   m_OperDlgBar;
	CCurveDlgBar  m_PolarDlgBar;
	CWOperDlgBar  m_WOperDlgBar;
	CW3DBar       m_W3DBar;
	CAFoilCtrlBar m_AFoilCtrlBar;
	CFInvCtrlBar  m_FInvCtrlBar;
	CMInvCtrlBar  m_MInvCtrlBar;
	CScaleOppBar  m_ScaleOppBar;
	CSpanPosBar   m_SpanPosBar;
	CBodyCtrlBar  m_BodyCtrlBar;
	CChildView  m_wndView;

	CComboBox m_ctrlFoil;
	CComboBox m_ctrlPlr;
	CComboBox m_ctrlOpp;
	CComboBox m_ctrlUFO;
	CComboBox m_ctrlWPlr;
	CComboBox m_ctrlWOpp;

	WINDOWPLACEMENT m_wndpl;

private:
	void UpdateView();

	CFoil* m_pCurFoil; // pointer to the currently selected CFoil, for all DIRECTDESIGN, INVERSEDESIGN and XFOILANALYSIS
	CString m_ProjectName;
	CString m_FileName;

	bool m_bSaved; //true if the project hasn't been modified since the last save
	bool m_bSaveOpps, m_bSaveWOpps; // true if Opps and WOpps should we saved in the .wpa project file
	int m_iApp;	// the currently active application, may be XFOILANALYSIS, DIRECTDESIGN, INVERSEDESIGN, or MIAREX


	//printing parameters
	double m_LeftMargin, m_RightMargin, m_TopMargin, m_BottomMargin;

	//the object arrays
	CObArray m_oaFoil;
	CObArray m_oaPolar;
	CObArray m_oaOpp;
	CObArray m_oaWing;
	CObArray m_oaWPolar;
	CObArray m_oaWOpp;
	CObArray m_oaPlane;
	CObArray m_oaPOpp;
	CObArray m_oaBody;

	COLORREF m_crColors[30];

	CFont m_FFont;

	CMenu m_XDirectMenu;
	CMenu m_MiarexMenu;
	CMenu m_AFoilMenu;
	CMenu m_XInverseMenu;
	CString m_RecentFile[8];
	void AddRecentFile(CString FileName);
	void SetRecentFileMenu();
	void ShortenFileName(CString &PathName);

	//The applications
	CXDirect XDirect;
	CXInverse XInverse;
	CMiarex Miarex;
	CAFoil AFoil;
	XFoil m_XFoil;

	bool DeleteFoil(CFoil *pFoil, bool bAsk = true);
	void DeletePlane(CPlane *pPlane, bool bResultsOnly = false);
	bool LoadProject(CString PathName);
	bool LoadPolarFile(CArchive &ar);
	bool LoadPolarFileV3(CArchive &ar, int ArchiveFormat=0);
	bool ReadProject(CString FileName);
	bool SerializeProject(CArchive &ar);
	bool SerializeUFOProject(CArchive &ar, CString UFOName);
	bool RenameFoil(CFoil* pFoil);
	bool ReadPolarFile(CString FileName);
	bool SaveProject(CString FileName);
	bool SaveProjectAs();
	bool SaveUFOProject();
	bool SaveBodyProject(CBody *pBody);

	int LoadFile(CString FileName, CString PathName);

	CFoil* AddFoil(CString strFoilName, double x[], double y[], int nf);
	void AddFoil(CFoil *pFoil);
	void CreateMiarexBar();
	void CreateAFoilBar();
	void CreateXInverseBar();
	void CreateXDirectBar();
	void DeleteProject();
	void DockMiarexBars();
	void HideBars();
	void LoadOldPolarFile(CArchive &ar, int n);
	void LoadSettings();
	void SavePolars(CString FileName, CFoil *pFoil=NULL);
	void SaveSettings();
	void SetCurrentFoil(CFoil* pFoil);
	void SetSaveState(bool bSave);
	void SetProjectName(CString PathName);
	void ShowOperBar(bool bShow);
	void WritePolars(CArchive &ar, CFoil *pFoil=NULL);
	void ExportWPlr(CWPolar * pWPolar);
	void DeleteWing(CWing *pThisWing, bool bResultsOnly = false);
	void DeleteBody(CBody *pThisBody);

	void UpdateOpps();
	void UpdateFoils();
	void UpdatePlrs();
	void UpdateWOpps();
	void UpdateUFOs();
	void UpdateWPlrs();
	COLORREF GetColor(int type);


	CPolar* AddPolar(CPolar *pPolar);
	CFoil* GetFoil(CString strFoilName);
	CFoil* ReadFoilFile(CString FileName, bool bKeepExistingFoil = false);
	CFoil* SetModFoil(CFoil* pNewFoil, bool bKeepExistingFoil = false);

	void * GetPlrVariable(CPolar *pPolar, int iVar);
	double GetPlrPoint(CFoil *pFoil, double Re, double Alpha, 
						   int PlrVar, bool &bOutRe, bool &bError);
	double GetCl(CFoil *pFoil0, CFoil *pFoil1, 
					 double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
	double GetCm(CFoil *pFoil0, CFoil *pFoil1,
					 double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
	double GetCm0(CFoil *pFoil0, CFoil *pFoil1,
					  double Re, double Tau, bool &bOutRe, bool &bError);
	double VLMGetVar(int nVar, CFoil *pFoil0, CFoil *pFoil1,
						 double Re, double Cl, double Tau, bool &bOutRe, bool &bError);
	double VLMGetPlrPoint(CFoil *pFoil, double Re,
						  double Cl, int PlrVar, bool &bOutRe, bool &bError);

	double pi;
	double m_mtoUnit;
	double m_mstoUnit;
	double m_m2toUnit;
	double m_kgtoUnit;
	double m_NtoUnit;
	double m_NmtoUnit;

	UINT m_LengthUnit;
	UINT m_AreaUnit;
	UINT m_WeightUnit;
	UINT m_SpeedUnit;
	UINT m_ForceUnit;
	UINT m_MomentUnit;

	double m_LiftScale;
	double m_DragScale;
	double m_DownwashScale;

	int m_VLMStyle, m_VLMWidth;
	COLORREF m_VLMColor;

	int m_OutlineStyle, m_OutlineWidth;
	COLORREF m_OutlineColor;

	int m_XCPStyle, m_XCPWidth;
	COLORREF m_XCPColor;

	int m_MomentStyle, m_MomentWidth;
	COLORREF m_MomentColor;

	int m_IDragStyle, m_IDragWidth;
	COLORREF m_IDragColor;

	int m_VDragStyle, m_VDragWidth;
	COLORREF m_VDragColor;

	int m_XTopStyle, m_XTopWidth;
	COLORREF m_XTopColor;

	int m_XBotStyle, m_XBotWidth;
	COLORREF m_XBotColor;

	int m_DownwashStyle, m_DownwashWidth;
	COLORREF m_DownwashColor;

	int m_WakeStyle, m_WakeWidth;
	COLORREF m_WakeColor;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnXDirect();
	afx_msg void OnSelChangeWPlr();
	afx_msg void OnSelChangeWing();
	afx_msg void OnSelChangeWOpp();
	afx_msg void OnSelChangeFoil();
	afx_msg void OnSelChangePlr();
	afx_msg void OnSelChangeOpp();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnIndicatorProject(CCmdUI* pCmdUI);
	afx_msg void OnXDirectSequence(CCmdUI* pCmdUI);
	afx_msg void OnParam12(CCmdUI* pCmdUI);
	afx_msg void OnParam3(CCmdUI* pCmdUI);
	afx_msg void OnXDirectPolar(CCmdUI* pCmdUI);
	afx_msg void OnXDirectDisplay(CCmdUI* pCmdUI);
	afx_msg void OnAnimate(CCmdUI* pCmdUI);
	afx_msg void OnXMPolar(CCmdUI* pCmdUI);
	afx_msg void OnWAMax(CCmdUI* pCmdUI);
	afx_msg void OnInitCalc(CCmdUI* pCmdUI);
	afx_msg void OnSpanPos(CCmdUI* pCmdUI);
	afx_msg void OnDAlpha(CCmdUI* pCmdUI);
	afx_msg void OnMiarex();
	afx_msg void OnFoilColor(CCmdUI* pCmdUI);
	afx_msg void OnHalfWing(CCmdUI* pCmdUI);
	afx_msg void OnStreamLines(CCmdUI* pCmdUI);
	afx_msg void OnMiarexW3DOppVLM(CCmdUI* pCmdUI);
	afx_msg void OnMiarexWPolar(CCmdUI* pCmdUI);
	afx_msg void OnWOpp(CCmdUI* pCmdUI);
	afx_msg void On3D(CCmdUI* pCmdUI);
	afx_msg void OnglLight(CCmdUI* pCmdUI);
	afx_msg void OnMiarexBar(CCmdUI* pCmdUI);
	afx_msg void On3DAxes(CCmdUI* pCmdUI);
	afx_msg void On3DSurfaces(CCmdUI* pCmdUI);
	afx_msg void On3DOutline(CCmdUI* pCmdUI);
	afx_msg void OnToolbars();
	afx_msg void OnMFC(CCmdUI* pCmdUI);
	afx_msg void OnAFoilCtrl(CCmdUI* pCmdUI);
	afx_msg void OnFullInverse();
	afx_msg void OnMixedInverse();
	afx_msg void OnSpecInv(CCmdUI* pCmdUI);
	afx_msg void OnSpecFInv(CCmdUI* pCmdUI);
	afx_msg void OnSpecMInv(CCmdUI* pCmdUI);
	afx_msg void OnBodyCtrl(CCmdUI* pCmdUI);
	afx_msg void OnAppOpen();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUnits();
	afx_msg void OnNewProject();
	afx_msg void OnLoadProject();
	afx_msg void OnInsertProject();
	afx_msg void OnSaveProject();
	afx_msg void OnSaveProjectAs();
	afx_msg void OnCloseProject();
	afx_msg void OnGuidelines();
	afx_msg void OnLogFile();
	afx_msg void OnSaveOptions();
	afx_msg void OnPlanePrefs();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnFoilDirectDesign();
	afx_msg void OnRecentFile1();
	afx_msg void OnRecentFile2();
	afx_msg void OnRecentFile3();
	afx_msg void OnRecentFile4();
	afx_msg void OnRecentFile5();
	afx_msg void OnRecentFile6();
	afx_msg void OnRecentFile7();
	afx_msg void OnRecentFile8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

};
