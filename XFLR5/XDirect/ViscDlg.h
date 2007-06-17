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
	//}}AFX_VIRTUAL

// Implementation

private:

	double m_fAlphaMin, m_fAlphaMax, m_fDAlpha;
	CStdioFile m_XFile;
	bool m_bAlpha;
	bool m_bType4;

	CPoint m_LegendPlace;
	CRect m_ViscRect;
	CString m_FoilName;
	CWnd * m_pXDirect;
	CWnd *m_pFrame;
	Graph m_RmsGraph;
	CViscThread *m_pIterThread;
	int m_Iterations, m_IterLim;

	XFoil* m_pXFoil;
	
	void AddOpPoint();
//	void EndSequence();
	void SetAlphaMin(double alpha);
	void SetAlphaMax(double alpha);
	void SetDAlpha(double alpha);
	void SetFileHeader();
	void ResetCurves();
	void UpdateView();

protected:


	// Generated message map functions
	//{{AFX_MSG(CViscDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
//	afx_msg void OnDestroy();
//	afx_msg void OnTimer();
	afx_msg void OnSkip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnTimer(UINT nIDEvent);
};

