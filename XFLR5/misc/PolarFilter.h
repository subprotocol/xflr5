/****************************************************************************

    CPolarFilter Class
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


// PolarFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolarFilter dialog

class CPolarFilter : public CDialog
{
	friend class CMiarex;
	friend class CXDirect;


// Construction
public:
	CPolarFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPolarFilter)
	enum { IDD = IDD_POLARFILTER };
	CButton	m_ctrlType1;
	CButton	m_ctrlType3;
	CButton	m_ctrlType5;
	CButton	m_ctrlType6;
	BOOL	m_bType1;
	BOOL	m_bType2;
	BOOL	m_bType3;
	BOOL	m_bType4;
	BOOL	m_bType5;
	BOOL	m_bType6;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolarFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bWing;
	bool m_bFoil;
	// Generated message map functions
	//{{AFX_MSG(CPolarFilter)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

