/****************************************************************************

	CReListDlg Class
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
// ReListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ReListDlg.h"
#include <math.h>
#include ".\relistdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CReListDlg dialog


CReListDlg::CReListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	memset(m_ReList, 0, sizeof(m_ReList));
	memset(m_MachList, 0, sizeof(m_MachList));
	memset(m_NCritList, 0, sizeof(m_NCritList));
	m_NRe = 0;
	m_iSelection = 0;
}


void CReListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReListDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_RELIST2, m_ctrlReList);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReListDlg, CDialog)
	//{{AFX_MSG_MAP(CReListDlg)
	ON_NOTIFY(NM_CLICK, IDC_RELIST2, OnNMClickReList)
	ON_NOTIFY(NM_RCLICK, IDC_RELIST2, OnNMRClickReList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_RELIST2, OnLvnEndLabelEditReList2)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_INSERTBEFORE, OnInsertBefore)
	ON_COMMAND(IDM_DELETEINPUT, OnDelete)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_INSERT, OnInsertBefore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReListDlg message handlers


BOOL CReListDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Prevent Return Key from closing App
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			if(GetDlgItem(IDOK) != GetFocus())
			{
				GetDlgItem(IDOK)->SetFocus();
			}
			else OnOK();
			return TRUE ;
		}
		if (pMsg->wParam == VK_DELETE)
		{
			OnDelete();
			return TRUE ;
		}
		if (pMsg->wParam == VK_INSERT)
		{
			OnInsertBefore();
			return TRUE ;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}



BOOL CReListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	m_ctrlReList.InsertColumn(0," ",LVCFMT_LEFT, 30);
	m_ctrlReList.InsertColumn(1,"Reynolds",LVCFMT_RIGHT, 80);
	m_ctrlReList.InsertColumn(2,"Mach",LVCFMT_RIGHT,60);
	m_ctrlReList.InsertColumn(3,"NCrit",LVCFMT_RIGHT,60);
	m_ctrlReList.m_nColumns = 4;

	m_ctrlReList.m_iPrecision[0] = 0;
	m_ctrlReList.m_iPrecision[1] = 0;
	m_ctrlReList.m_iPrecision[2] = 2;
	m_ctrlReList.m_iPrecision[3] = 2;
	
	m_ctrlReList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlReList.SetFocus();

	FillReList();

	if(m_ctrlReList.GetItemCount())
	{
		m_ctrlReList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	return FALSE; 
}

void CReListDlg::OnLvnEndLabelEditReList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}
//	m_iSelection = pDispInfo->item.iItem;
	POSITION pos = m_ctrlReList.GetFirstSelectedItemPosition();
	m_iSelection = m_ctrlReList.GetNextSelectedItem(pos);

	//first read the section
	int i, j;
	double Re,Mach, NCrit;
	ReadSectionData(m_iSelection, Re, Mach, NCrit);

	//then remove the section from the list
	for (i=m_iSelection; i<m_NRe; i++)
	{
		m_ReList[i]    = m_ReList[i+1];
		m_MachList[i]  = m_MachList[i+1];
		m_NCritList[i] = m_NCritList[i+1];
	}
	//and re-insert it at the proper place
	bool bInserted = false;
	for (i=0; i<m_NRe; i++)
	{
		if(Re<m_ReList[i])
		{
			for(j=m_NRe-1;j>i;j--)
			{
				m_ReList[j]    = m_ReList[j-1];
				m_MachList[j]  = m_MachList[j-1];
				m_NCritList[j] = m_NCritList[j-1];
			}
			m_ReList[i]    = Re;
			m_MachList[i]  = Mach;
			m_NCritList[i] = NCrit;
			bInserted = true;
			break;
		}
	}
	if(!bInserted)
	{//append at the end
			m_ReList[m_NRe-1]    = Re;
			m_MachList[m_NRe-1]  = Mach;
			m_NCritList[m_NRe-1] = NCrit;
	}
	FillReList();
	SetSelection(m_iSelection);
}


void CReListDlg::FillCell(int iItem, int iSubItem)
{
	CString strong;

	switch (iSubItem)
	{
		case 0:
		{
			strong.Format("%d", iItem);
			m_ctrlReList.SetItemText(iItem, 0, strong);
			break;
		}
		case 1:
		{
			ReynoldsFormat(strong, m_ReList[iItem]);
			m_ctrlReList.SetItemText(iItem, 1, strong);
			break;
		}
		case 2:
		{
			strong.Format(" %7.2f", m_MachList[iItem]);
			m_ctrlReList.SetItemText(iItem,2,strong);
			break;
		}
		case 3:
		{
			strong.Format(" %7.2f", m_NCritList[iItem]);
			m_ctrlReList.SetItemText(iItem,3,strong);
			break;
		}
		default:
		{
			break;
		}
	}
}


void CReListDlg::FillReList()
{

	CString strong;
	m_ctrlReList.DeleteAllItems();
	for (int i=0; i<m_NRe; i++)
	{
		strong.Format("%d", i+1);
		m_ctrlReList.InsertItem(i, strong);

		ReynoldsFormat(strong, m_ReList[i]);
		m_ctrlReList.SetItemText(i, 1, strong);
		strong.Format(" %7.2f", m_MachList[i]);
		m_ctrlReList.SetItemText(i,2,strong);
		strong.Format(" %7.2f", m_NCritList[i]);
		m_ctrlReList.SetItemText(i,3,strong);
	}
}


void CReListDlg::ReadSectionData(int sel, double &Re, double &Mach, double &NCrit)
{
	double d;
	int res;
	CString strong;

	strong = m_ctrlReList.GetItemText(sel,1);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) Re =d;

	strong = m_ctrlReList.GetItemText(sel,2);
	res = sscanf(strong, "%lf",&d);
	if(res==1) Mach=d;

	strong = m_ctrlReList.GetItemText(sel,3);
	res = sscanf(strong, "%lf",&d);
	if(res==1) NCrit =d;
}


void CReListDlg::OnNMClickReList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSelection = pNMListView->iItem;
	*pResult = 0;
}



void CReListDlg::OnNMRClickReList(NMHDR *pNMHDR, LRESULT *pResult)
{
/*	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSelection = pNMListView->iItem;

	*pResult = 0;*/
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTXWNGMENU))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	pt.x, pt.y, this); 
	}
}


void CReListDlg::OnDelete() 
{
	if(m_NRe<=0) return;
	int i;

	m_ctrlReList.CloseEdit();

	POSITION pos = m_ctrlReList.GetFirstSelectedItemPosition();
	m_iSelection = m_ctrlReList.GetNextSelectedItem(pos);
	
	for(i=m_iSelection; i<m_NRe; i++)
	{
		m_ReList[i]    = m_ReList[i+1];
		m_MachList[i]  = m_MachList[i+1];
		m_NCritList[i] = m_NCritList[i+1];
	}

	m_NRe--;
	FillReList();
	if(m_iSelection>=m_NRe) m_iSelection = m_NRe-1;

	SetSelection(m_iSelection);
}


void CReListDlg::OnInsertBefore()
{
	if(m_NRe>=30) 
	{
		AfxMessageBox("The maximum number of polars has been reached", MB_OK);
		return;
	}

	m_ctrlReList.CloseEdit();

	POSITION pos = m_ctrlReList.GetFirstSelectedItemPosition();
	m_iSelection = m_ctrlReList.GetNextSelectedItem(pos);

	int n = m_iSelection;

	for (int k=m_NRe; k>=n; k--)
	{
		m_ReList[k+1]      = m_ReList[k];
		m_MachList[k+1]    = m_MachList[k];
		m_NCritList[k+1]   = m_NCritList[k];
	}
	if(m_NRe==0)
	{
		m_ReList[0]    = 100000.0;
		m_MachList[0]  = 0.0;
		m_NCritList[0] = 9.0;	
	}
	else
	{
		if(n==0) 
		{
			m_ReList[n]    = m_ReList[1] /2.0;
			m_MachList[n]  = m_MachList[1];
			m_NCritList[n] = m_NCritList[1];	
		}
		else
		{
			m_ReList[n]    = (m_ReList[n+1] + m_ReList[n-1]) /2.0;
			m_MachList[n]  = m_MachList[n-1];
			m_NCritList[n] = m_NCritList[n-1];
		}
	}
	m_NRe++;
	FillReList();
	SetSelection(m_iSelection);
//	m_ctrlReList.EditSubLabel(m_iSelection, 1);
}


void CReListDlg::SetSelection(int sel)
{
	if(sel>=0)
	{
		m_ctrlReList.EnsureVisible(sel,FALSE);
		m_ctrlReList.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlReList.m_iItem = sel;
	}
}
	
