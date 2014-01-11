/****************************************************************************

	CEditFoilDlg class
	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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


// EditFoilDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "EditFoilDlg.h"
#include "../main/ChildView.h"

/////////////////////////////////////////////////////////////////////////////
// CEditFoilDlg dialog


CEditFoilDlg::CEditFoilDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditFoilDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditFoilDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bApplied  = true;
	m_bModified = false;
	m_pChildView  = NULL;
	m_pMemFoil    = NULL;
	m_pBufferFoil = NULL;
	m_pPF         = NULL;
	m_pSF         = NULL;
	m_pmemSF      = NULL;
	m_pmemPF      = NULL;

	m_iInt = -1;
	m_iExt = -1;
}


void CEditFoilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditFoilDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_INSERTPT, m_ctrlInsert);
	DDX_Control(pDX, IDC_DELETEPT, m_ctrlDelete);
	DDX_Control(pDX, IDC_COORDLIST, m_ctrlCoordList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditFoilDlg, CDialog)
	//{{AFX_MSG_MAP(CEditFoilDlg)
	ON_BN_CLICKED(IDC_DELETEPT, OnDeletePt)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_INSERTPT, OnInsertPt)
	ON_COMMAND(IDM_INSERTBEFORE, OnInsertPt)
	ON_COMMAND(IDM_APPEND, OnAppend)
	ON_COMMAND(IDM_DELETEINPUT, OnDeletePt)
	ON_NOTIFY(NM_CLICK, IDC_COORDLIST, OnNMClickCoordList)
	ON_NOTIFY(NM_RCLICK, IDC_COORDLIST, OnNMRClickCoordList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_COORDLIST, OnLvnEndLabelEditCoordList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditFoilDlg message handlers

BOOL CEditFoilDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pBufferFoil) m_pBufferFoil->m_iHighLight = 0;
	if(m_pSF || m_pPF)
	{
		m_ctrlDelete.EnableWindow(false);
		m_ctrlInsert.EnableWindow(false);
	}

	if(m_pSF)
	{
		m_iExt = m_pSF->m_Extrados.m_iCtrlPoints;
		m_iInt = m_pSF->m_Intrados.m_iCtrlPoints;
		SetWindowText("Spline Ctrl Points Edition");
	}
	else if(m_pPF)
	{
		m_iExt = m_pPF->m_Extrados.m_iPoints;
		m_iInt = m_pPF->m_Intrados.m_iPoints;
		SetWindowText("Spline Ctrl Points Edition");
	}

	m_ctrlCoordList.InsertColumn(0," ",LVCFMT_LEFT, 20);
	m_ctrlCoordList.InsertColumn(1,"X",LVCFMT_RIGHT, 90);
	m_ctrlCoordList.InsertColumn(2,"Y",LVCFMT_RIGHT, 90);
	m_ctrlCoordList.m_nColumns = 3;

	m_ctrlCoordList.m_iPrecision[0] = 0;
	m_ctrlCoordList.m_iPrecision[1] = 5;
	m_ctrlCoordList.m_iPrecision[2] = 5;
	
	m_ctrlCoordList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlCoordList.SetFocus();

	SetList();

	if(m_ctrlCoordList.GetItemCount())
	{
		m_ctrlCoordList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}

	m_pChildView->Invalidate();
	return FALSE;
}

void CEditFoilDlg::SetSelection(int sel)
{
	if(sel>=0)
	{
		m_ctrlCoordList.EnsureVisible(sel,FALSE);
		m_ctrlCoordList.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlCoordList.m_iItem = sel;
	}
}

void CEditFoilDlg::SetList()
{
	CString strong;
	int i,j,off;
	m_ctrlCoordList.DeleteAllItems();
	if(m_pSF)
	{
		for (i=0; i<=m_pSF->m_Extrados.m_iCtrlPoints; i++)
		{
			strong.Format("u%d", i);
			m_ctrlCoordList.InsertItem(i, strong);
			strong.Format("%8.5f", m_pSF->m_Extrados.m_Input[i].x);
			m_ctrlCoordList.SetItemText(i, 1, strong);
			strong.Format("%8.5f", m_pSF->m_Extrados.m_Input[i].y);
			m_ctrlCoordList.SetItemText(i, 2, strong);
		}
		off = m_pSF->m_Extrados.m_iCtrlPoints+1;
		for (j=0; j<=m_pSF->m_Intrados.m_iCtrlPoints; j++)
		{
			strong.Format("l%d", j);
			m_ctrlCoordList.InsertItem(j+off, strong);
			strong.Format("%8.5f", m_pSF->m_Intrados.m_Input[j].x);
			m_ctrlCoordList.SetItemText(j+off, 1, strong);
			strong.Format("%8.5f", m_pSF->m_Intrados.m_Input[j].y);
			m_ctrlCoordList.SetItemText(j+off, 2, strong);
		}
	}
	else if(m_pPF)
	{
		for (i=0; i<m_pPF->m_Extrados.m_iPoints; i++)
		{
			strong.Format("u%d", i);
			m_ctrlCoordList.InsertItem(i, strong);
			strong.Format("%8.5f", m_pPF->m_Extrados.m_ctrlPoint[i].x);
			m_ctrlCoordList.SetItemText(i, 1, strong);
			strong.Format("%8.5f", m_pPF->m_Extrados.m_ctrlPoint[i].y);
			m_ctrlCoordList.SetItemText(i, 2, strong);
		}
		off = m_pPF->m_Extrados.m_iPoints;
		for (j=0; j<m_pPF->m_Intrados.m_iPoints; j++)
		{
			strong.Format("l%d", j);
			m_ctrlCoordList.InsertItem(j+off, strong);
			strong.Format("%8.5f", m_pPF->m_Intrados.m_ctrlPoint[j].x);
			m_ctrlCoordList.SetItemText(j+off, 1, strong);
			strong.Format("%8.5f", m_pPF->m_Intrados.m_ctrlPoint[j].y);
			m_ctrlCoordList.SetItemText(j+off, 2, strong);
		}
	}
	else if(m_pBufferFoil)
	{
		for (i=0; i<m_pBufferFoil->n; i++)
		{
			strong.Format("%d", i+1);
			m_ctrlCoordList.InsertItem(i, strong);
			strong.Format("%8.5f", m_pBufferFoil->x[i]);
			m_ctrlCoordList.SetItemText(i, 1, strong);
			strong.Format("%8.5f", m_pBufferFoil->y[i]);
			m_ctrlCoordList.SetItemText(i, 2, strong);
		}
	}
}


void CEditFoilDlg::OnLvnEndLabelEditCoordList(NMHDR *pNMHDR, LRESULT *pResult)
{
	double d,X,Y;
	int res, position;
	CString strong;

	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}
//	m_iSelection = pDispInfo->item.iItem;
	POSITION pos = m_ctrlCoordList.GetFirstSelectedItemPosition();
	int sel  = m_ctrlCoordList.GetNextSelectedItem(pos);
	strong = m_ctrlCoordList.GetItemText(sel,1);
	res = sscanf(strong, "%lf",&d);
	if(res==1) X =d;

	strong = m_ctrlCoordList.GetItemText(sel,2);
	res = sscanf(strong, "%lf",&d);
	if(res==1) Y=d;

	if(m_pSF)
	{
		strong = m_ctrlCoordList.GetItemText(sel,0);
		position = strong.Find("u",0);
		if(position>=0)
		{
			//extrados
			m_pSF->m_Extrados.m_Input[sel].x = X;
			m_pSF->m_Extrados.m_Input[sel].y = Y;
			m_pSF->Update(true);
		}
		else if(sel-m_iExt>=0)
		{
			m_pSF->m_Intrados.m_Input[sel-m_iExt-1].x = X;
			m_pSF->m_Intrados.m_Input[sel-m_iExt-1].y = Y;
			m_pSF->Update(false);
		}
	}
	else if(m_pPF)
	{
		strong = m_ctrlCoordList.GetItemText(sel,0);
		position = strong.Find("u",0);
		if(position>=0)
		{
			//extrados
			m_pPF->m_Extrados.m_ctrlPoint[sel].x = X;
			m_pPF->m_Extrados.m_ctrlPoint[sel].y = Y;
			m_pPF->Update(true);
		}
		else if(sel-m_iExt>=0)
		{
			m_pPF->m_Intrados.m_ctrlPoint[sel-m_iExt-1].x = X;
			m_pPF->m_Intrados.m_ctrlPoint[sel-m_iExt-1].y = Y;
			m_pPF->Update(false);
		}
	}
	else{

		//modify buffer foil coordinates
		m_pBufferFoil->x[sel]  = X;
		m_pBufferFoil->y[sel]  = Y;
		m_pBufferFoil->xb[sel] = X;
		m_pBufferFoil->yb[sel] = Y;
	}
	m_bApplied = false;
//	SetList();
//	SetSelection(sel);
	OnApply();
}

void CEditFoilDlg::OnNMClickCoordList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		if(m_pSF)
		{
		}
		else if(m_pPF)
		{
		}
		else if(m_pBufferFoil)	m_pBufferFoil->m_iHighLight = -1;
		*pResult =0;
		return ;
	}
	if(m_pSF)
	{
		if(pNMListView->iItem <= m_iExt)	
		{
			m_pSF->m_Extrados.m_iHighlight = pNMListView->iItem;
			m_pSF->m_Intrados.m_iHighlight = -1;
		}
		else
		{
			m_pSF->m_Extrados.m_iHighlight = -1;
			m_pSF->m_Intrados.m_iHighlight = pNMListView->iItem-m_iExt-1;
		}
	}
	else if(m_pPF)
	{
		if(pNMListView->iItem <= m_iExt)	
		{
			m_pPF->m_Extrados.m_iHighlight = pNMListView->iItem;
			m_pPF->m_Intrados.m_iHighlight = -1;
		}
		else
		{
			m_pPF->m_Extrados.m_iHighlight = -1;
			m_pPF->m_Intrados.m_iHighlight = pNMListView->iItem-m_iExt-1;
		}
	}
	else if(m_pBufferFoil)	m_pBufferFoil->m_iHighLight = pNMListView->iItem;

	m_pChildView->Invalidate();
	*pResult = 0;
}



void CEditFoilDlg::OnNMRClickCoordList(NMHDR *pNMHDR, LRESULT *pResult)
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



void CEditFoilDlg::ReadSectionData(int sel, double &X, double &Y)
{
	double d;
	int res;
	CString strong;

	strong = m_ctrlCoordList.GetItemText(sel,1);
	res = sscanf(strong, "%lf",&d);
	if(res==1) X =d;

	strong = m_ctrlCoordList.GetItemText(sel,2);
	res = sscanf(strong, "%lf",&d);
	if(res==1) Y=d;
}


void CEditFoilDlg::OnRestore() 
{
	int i;
	if(m_pSF)
	{
		m_pSF->Copy(m_pmemSF);
	}
	else if(m_pPF)
	{
		m_pPF->Copy(m_pmemPF);
	}
	else
	{
		for (i=0;i<m_pMemFoil->nb; i++)
		{
			m_pBufferFoil->xb[i] = m_pMemFoil->xb[i];
			m_pBufferFoil->yb[i] = m_pMemFoil->yb[i];
		}
		m_pBufferFoil->nb = m_pMemFoil->n;
		for (i=0;i<m_pMemFoil->n; i++)
		{
			m_pBufferFoil->x[i]  = m_pMemFoil->x[i];
			m_pBufferFoil->y[i]  = m_pMemFoil->y[i];
		}
		m_pBufferFoil->n = m_pMemFoil->n;
	}

	SetList();
	m_bApplied = true;
	m_bModified = false;

	SetSelection(0);
	m_pChildView->Invalidate();
}

BOOL CEditFoilDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if(GetDlgItem(IDOK) != GetFocus())
			{

				GetDlgItem(IDOK)->SetFocus();
			}
			else OnOK();
			return TRUE ;
		}

/*		if (pMsg->wParam == VK_DELETE){
			OnDeletePt();
			return true;

		}
		if ( pMsg->wParam == VK_INSERT){
			OnInsertPt();
			return true;
		}*/
	}

	return CDialog::PreTranslateMessage(pMsg);
}



void CEditFoilDlg::OnApply() 
{
	if(m_bApplied) return;
	m_bApplied = true;
	m_bModified = true;
	m_pChildView->Invalidate();
}

void CEditFoilDlg::OnDeletePt() 
{
	POSITION pos = m_ctrlCoordList.GetFirstSelectedItemPosition();
	if(!pos) return;
	int i, sel;
	sel = m_ctrlCoordList.GetNextSelectedItem(pos);
	if(sel<0) return;

	if(m_pSF)
	{
	}
	else if(m_pPF)
	{
	}
	else
	{
		for (i=sel;i<m_pBufferFoil->nb-1; i++)
		{
			m_pBufferFoil->xb[i] = m_pBufferFoil->xb[i+1];
			m_pBufferFoil->yb[i] = m_pBufferFoil->yb[i+1];
		}
		for (i=sel;i<m_pBufferFoil->n-1; i++)
		{
			m_pBufferFoil->x[i] = m_pBufferFoil->x[i+1];
			m_pBufferFoil->y[i] = m_pBufferFoil->y[i+1];
		}
		m_pBufferFoil->nb--;
		m_pBufferFoil->n--;
	}

	SetList();
	SetSelection(sel);
	m_bModified = true;
	m_pChildView->Invalidate();
}

void CEditFoilDlg::OnInsertPt() 
{
	int i;
	POSITION pos = m_ctrlCoordList.GetFirstSelectedItemPosition();
	if(!pos) return;
	int sel = m_ctrlCoordList.GetNextSelectedItem(pos);
	if(sel<=0) return;

	if(m_pSF)
	{
	}
	else if(m_pPF)
	{
	}
	else
	{
		for (i=m_pBufferFoil->nb; i>sel; i--)
		{
			m_pBufferFoil->xb[i] = m_pBufferFoil->xb[i-1];
			m_pBufferFoil->yb[i] = m_pBufferFoil->yb[i-1];
		}
		m_pBufferFoil->xb[sel] = (m_pBufferFoil->xb[sel-1] + m_pBufferFoil->xb[sel+1])/2.0;
		m_pBufferFoil->yb[sel] = (m_pBufferFoil->yb[sel-1] + m_pBufferFoil->yb[sel+1])/2.0 ;

		for (i=m_pBufferFoil->n; i>sel; i--)
		{
			m_pBufferFoil->x[i] = m_pBufferFoil->x[i-1];
			m_pBufferFoil->y[i] = m_pBufferFoil->y[i-1];
		}
		m_pBufferFoil->x[sel] = (m_pBufferFoil->x[sel-1] + m_pBufferFoil->x[sel+1])/2.;
		m_pBufferFoil->y[sel] = (m_pBufferFoil->y[sel-1] + m_pBufferFoil->y[sel+1])/2.;


		m_pBufferFoil->nb++;
		m_pBufferFoil->n++;
	}
	SetList();
	SetSelection(sel);

	m_bModified = true;
	m_pChildView->Invalidate();
}

void CEditFoilDlg::OnAppend() 
{
	int i;
	POSITION pos = m_ctrlCoordList.GetFirstSelectedItemPosition();
	if(!pos) return;
	int sel = m_ctrlCoordList.GetNextSelectedItem(pos);
	if(sel<=0) return;

	if(m_pSF)
	{
	}
	else if(m_pPF)
	{
	}
	else
	{
		if (sel>=m_pBufferFoil->nb-1 || sel>=m_pBufferFoil->n-1) return;

		for (i=m_pBufferFoil->nb; i>sel; i--)
		{
			m_pBufferFoil->xb[i] = m_pBufferFoil->xb[i-1];
			m_pBufferFoil->yb[i] = m_pBufferFoil->yb[i-1];
		}
		m_pBufferFoil->xb[sel+1] = (m_pBufferFoil->xb[sel] + m_pBufferFoil->xb[sel+2])/2.0;
		m_pBufferFoil->yb[sel+1] = (m_pBufferFoil->yb[sel] + m_pBufferFoil->yb[sel+2])/2.0 ;

		for (i=m_pBufferFoil->n; i>sel; i--)
		{
			m_pBufferFoil->x[i] = m_pBufferFoil->x[i-1];
			m_pBufferFoil->y[i] = m_pBufferFoil->y[i-1];
		}
		m_pBufferFoil->x[sel+1] = (m_pBufferFoil->x[sel] + m_pBufferFoil->x[sel+2])/2.;
		m_pBufferFoil->y[sel+1] = (m_pBufferFoil->y[sel] + m_pBufferFoil->y[sel+2])/2.;

		m_pBufferFoil->nb++;
		m_pBufferFoil->n++;

		SetList();
		SetSelection(sel);

		m_bModified = true;
		m_pChildView->Invalidate();
	}
}
