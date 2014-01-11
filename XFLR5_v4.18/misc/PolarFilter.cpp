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

// PolarFilter.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "PolarFilter.h"



/////////////////////////////////////////////////////////////////////////////
// CPolarFilter dialog


CPolarFilter::CPolarFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CPolarFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolarFilter)
	m_bType1 = FALSE;
	m_bType2 = FALSE;
	m_bType3 = FALSE;
	m_bType4 = FALSE;
	m_bType5 = FALSE;
	m_bType6 = FALSE;
	//}}AFX_DATA_INIT
	m_bWing  = false;
	m_bFoil  = false;
}


void CPolarFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolarFilter)
	DDX_Control(pDX, IDC_TYPE1, m_ctrlType1);
	DDX_Control(pDX, IDC_TYPE3, m_ctrlType3);
	DDX_Control(pDX, IDC_TYPE5, m_ctrlType5);
	DDX_Control(pDX, IDC_TYPE6, m_ctrlType6);
	DDX_Check(pDX, IDC_TYPE1, m_bType1);
	DDX_Check(pDX, IDC_TYPE2, m_bType2);
	DDX_Check(pDX, IDC_TYPE3, m_bType3);
	DDX_Check(pDX, IDC_TYPE4, m_bType4);
	DDX_Check(pDX, IDC_TYPE5, m_bType5);
	DDX_Check(pDX, IDC_TYPE6, m_bType6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolarFilter, CDialog)
	//{{AFX_MSG_MAP(CPolarFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolarFilter message handlers


BOOL CPolarFilter::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ctrlType1.SetFocus();

	if(m_bWing) m_ctrlType3.EnableWindow(false);

	if(m_bFoil)
	{
		m_ctrlType5.EnableWindow(false);
		m_ctrlType6.EnableWindow(false);
	}

	return FALSE;  
}

