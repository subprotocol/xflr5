/****************************************************************************

    SaveOptionsDlg Class
    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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
// SaveOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "SaveOptionsDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CSaveOptionsDlg dialog


CSaveOptionsDlg::CSaveOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveOptionsDlg)
	m_BOpps = FALSE;
	m_BWOpps = FALSE;
	//}}AFX_DATA_INIT
}


void CSaveOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveOptionsDlg)
	DDX_Check(pDX, IDC_SAVEOPPS, m_BOpps);
	DDX_Check(pDX, IDC_SAVEWOPPS, m_BWOpps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveOptionsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveOptionsDlg message handlers
