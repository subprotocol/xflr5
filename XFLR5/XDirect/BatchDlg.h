/****************************************************************************

	Batch Analysis class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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

// BatchDlg.h : header file
//
#include "BatchThread.h"
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "../Graph/Graph.h"

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog
 
class CBatchDlg : public CDialog
{
	friend class CXDirect;
	friend class CBatchThread;
// Construction
public:
	CBatchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchDlg)
	enum { IDD = IDD_BATCHANALYSISDLG };
	CButton	m_ctrlIterGraph;
	CButton	m_ctrlSkipPolar;
	CButton	m_ctrlSkipPoint;
	CButton	m_ctrlFromZero;
	CButton	m_ctrlList;
	CButton	m_ctrlRange;
	CButton	m_ctrlCreateList;
	CFloatEdit	m_ctrlXBotTr;
	CFloatEdit	m_ctrlXTopTr;
	CFloatEdit	m_ctrlNCrit;
	CFloatEdit	m_ctrlReInc;
	CButton	m_ctrlAcl1;
	CButton	m_ctrlAcl2;
	CButton	m_ctrlInitBL;
	CButton	m_ctrlShowTextOutput;
	CButton	m_ctrlClose;
	CButton	m_ctrlAnalysis;
	CFloatEdit	m_ctrlSpMin;
	CFloatEdit	m_ctrlSpMax;
	CFloatEdit	m_ctrlSpInc;
	CStatic	m_ctrlSpecTyp;
	CStatic	m_ctrlReTyp;
	CButton	m_ctrlType;
	CFloatEdit	m_ctrlReMin;
	CFloatEdit	m_ctrlReMax;
	CEdit	m_ctrlOutput;
	CStatic	m_ctrlMaTyp;
	CFloatEdit	m_ctrlMach;
	double	m_ReInc;
	double	m_ReMax;
	double	m_ReMin;
	double	m_Mach;
	double	m_SpInc;
	double	m_SpMax;
	double	m_SpMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	bool InitXFoil2();
	void Analysis2();
	void Analysis3();
	void CleanUp();
	void UpdateOutput(CString &str);
	void UpdateGraph(double Re, double Alpha);
	void OutputIter(int iter, double Re, double Alpha);
	void ReadParams();
	void SetFileHeader();
	void ResetCurves();

	bool m_bOutput;
	bool m_bAlpha;
	bool m_bFromList;
	bool m_bFromZero;
	bool m_bShowTextOutput;

	int m_Type;
	int m_Iterations;
	int m_IterLim;
	double m_NCrit;
	double m_XTopTr;
	double m_XBotTr;
	double *m_ReList;
	double *m_MachList;
	double *m_NCritList;

	double pi;

	int m_NRe;

	CStdioFile m_XFile;
	CBatchThread* m_pBatchThread;
	CWnd* m_pXDirect;
	XFoil* m_pXFoil;
	CFont m_RFont;
	CFoil *m_pCurFoil;
	
	CFont m_TitleFont;
	
	CString m_FoilName;
	CString m_PlrName;

	Graph m_RmsGraph;
	CRect m_IterRect;
	CCurve* m_pIterCurve;
	CWnd * m_pFrame;

	// Generated message map functions
	//{{AFX_MSG(CBatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnType1();
	afx_msg void OnAnalysis();
	afx_msg void OnAcl();
	afx_msg void OnClose();
	afx_msg void OnInitBL();
	afx_msg void OnShowTextOutput();
	afx_msg void OnRange();
	afx_msg void OnCreateList();
	afx_msg void OnFromZero();
	afx_msg void OnSkipPoint();
	afx_msg void OnSkipPolar();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

