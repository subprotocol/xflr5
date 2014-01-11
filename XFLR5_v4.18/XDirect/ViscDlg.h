/****************************************************************************

    ViscDlg Class
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

// ViscDlg.h : header file
//
#include "XFoil.h"
#include "../Graph/Graph.h"
#include "ViscThread.h"



/////////////////////////////////////////////////////////////////////////////
// CViscDlg dialog

class CViscDlg : public CDialog
{
	friend class CViscThread;
	friend class CXDirect;
// Construction
public:
	CViscDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViscDlg)
	enum { IDD = IDD_VISCDLG };
	CButton	m_ctrlSkip;
	CButton	m_ctrlCancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViscDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSkip();	//}}AFX_VIRTUAL

// Implementation

private:

	bool m_bAlpha;
	bool m_bType4;
	int m_Iterations, m_IterLim;
	double m_AlphaMin, m_AlphaMax, m_DeltaAlpha;
	double m_ClMin, m_ClMax, m_DeltaCl;
	double m_ReMin, m_ReMax, m_DeltaRe;

	CWnd * m_pXDirect;
	CWnd *m_pFrame;
	CStdioFile m_XFile;
	CPoint m_LegendPlace;
	CRect m_ViscRect;
	CString m_FoilName;
	Graph m_RmsGraph;
	CViscThread *m_pIterThread;

	XFoil* m_pXFoil;
	
	void AddOpPoint();
	void SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha);
	void SetCl(double ClMin, double ClMax, double DeltaCl);
	void SetRe(double ReMin, double ReMax, double DeltaRe);
	void SetFileHeader();
	void ResetCurves();
	void UpdateView();


	DECLARE_MESSAGE_MAP()
public:

};

