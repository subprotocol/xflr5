/****************************************************************************

    CInitDialogBar Class
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

//////////////////////////////////////////////////////////////////////
//
// InitDialogBar.h: interface for the CInitDialogBar class.
//
//////////////////////////////////////////////////////////////////////



#pragma once



////////////////////////////////////////////////////////////////////////////
//
// CInitDialogBar window
//
////////////////////////////////////////////////////////////////////////////

class CInitDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CInitDialogBar)

	// Construction / Destruction
	public:
	CInitDialogBar();
	virtual ~CInitDialogBar();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitDialogBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	// Implementation
	public:
	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT
	nID);
	BOOL Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName, UINT
	nStyle, UINT nID);

	protected:
	virtual BOOL OnInitDialogBar();

	// Generated message map functions
	protected:
	//{{AFX_MSG(CInitDialogBar)
	// NOTE - the ClassWizard will add and remove member functionshere.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

