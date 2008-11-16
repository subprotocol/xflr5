/****************************************************************************

    CInPlaceList Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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


// InPlaceList.cpp : implementation file
//

#include "stdafx.h"
#include "EditListCtrl.h"
#include "../Miarex/PanelListCtrl.h"
#include ".\inplacelist.h"


/////////////////////////////////////////////////////////////////////////////
// CInPlaceList

CInPlaceList::CInPlaceList()
{
	m_nSel = 0;
	m_bWingCtrl = false;
	m_bESC = FALSE;
}


CInPlaceList::CInPlaceList(CListCtrl *pCtrl, int iItem, int iSubItem)
{
	m_pListCtrl = pCtrl;

	m_iItem = iItem;
	m_iSubItem = iSubItem;

	m_nSel = 0;
	m_bWingCtrl = false;
	m_bESC = FALSE;
}

CInPlaceList::~CInPlaceList()
{
} 


BEGIN_MESSAGE_MAP(CInPlaceList, CComboBox)
	//{{AFX_MSG_MAP(CInPlaceList)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnCbnSelchange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceList message handlers

int CInPlaceList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

//	for(int i=0; i<m_strArray.GetSize();i++)		AddString(m_strArray[i] );
//	SetCurSel( m_nSel );

	SetFocus();
	return 0;
}

BOOL CInPlaceList::PreTranslateMessage(MSG* pMsg)
{
	CPanelListCtrl *pListCtrl = (CPanelListCtrl*)m_pListCtrl;
	
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			if( pMsg->wParam == VK_ESCAPE)m_bESC = TRUE;
//			GetParent()->SetFocus();
			PostMessage( WM_CLOSE );
			return TRUE;				// DO NOT process further
		}
/*		if(pMsg->wParam == VK_LEFT)
		{
			if (m_iSubItem > 0)
				pListCtrl->EditSubLabel (m_iItem, m_iSubItem-1);
			PostMessage( WM_CLOSE );
			return TRUE;				// DO NOT process further
			
		}
		if(pMsg->wParam == VK_RIGHT)
		{
			if (m_iSubItem == pListCtrl->m_nColumns-1)
				pListCtrl->EditSubLabel (m_iItem , 1);
			else 
				pListCtrl->EditSubLabel (m_iItem , m_iSubItem+1);
			PostMessage( WM_CLOSE );
			return TRUE;				// DO NOT process further
		}

		if(pMsg->wParam == VK_TAB)
		{

			SHORT sh   = GetKeyState(VK_SHIFT);
			if ((sh & 0x8000))	{
				pListCtrl->EditSubLabel(m_iItem, m_iSubItem - 1);
			}
			else{
				if(m_iSubItem == pListCtrl->m_nColumns - 1) pListCtrl->EditSubLabel(m_iItem, 1);
				else                                        pListCtrl->EditSubLabel(m_iItem, m_iSubItem+1);
			}
			return TRUE;
		}*/
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CInPlaceList::OnCbnSelchange()
{
/*	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );*/
//	PostMessage(WM_CLOSE);
}



void CInPlaceList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );
//	GetParent->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );

	PostMessage( WM_CLOSE );
}

void CInPlaceList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CPanelListCtrl *pListCtrl = (CPanelListCtrl*)m_pListCtrl;
	if(nChar == VK_ESCAPE){
		m_bESC = TRUE;
//		GetParent()->SetFocus();
		PostMessage( WM_CLOSE );
		return;
	}
	if(nChar == VK_TAB)
	{
		SHORT sh   = GetKeyState(VK_SHIFT);
		if ((sh & 0x8000))	{
			pListCtrl->EditSubLabel(m_iItem, pListCtrl->m_nColumns - 1);
			
		}
		else{
			if(m_iSubItem == pListCtrl->m_nColumns - 1) pListCtrl->EditSubLabel(m_iItem, 1);
			else                                        pListCtrl->EditSubLabel(m_iItem, m_iSubItem+1);
		}
		return;
	}	

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}


void CInPlaceList::OnCloseup()
{
//	GetParent()->SetFocus();
}

void CInPlaceList::Set(int iItem, int iSubItem)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
}


