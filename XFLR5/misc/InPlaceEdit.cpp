/****************************************************************************

    CInPlaceEdit Class
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


// InPlaceEdit.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "./X-FLR6.h"
#include "InPlaceEdit.h"



// CInPlaceEdit

IMPLEMENT_DYNAMIC(CInPlaceEdit, CEdit)

CInPlaceEdit::CInPlaceEdit(CListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_nColumns = 0;
	m_nRows = 0;
	m_bESC = FALSE;
	m_pListCtrl = pCtrl;

	m_iniStr = sInitText;

}

CInPlaceEdit::~CInPlaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	ON_WM_GETDLGCODE()
	ON_WM_NCDESTROY()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// Gestionnaires de messages CInPlaceEdit





BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN)
		{
			PostMessage( WM_CLOSE );
			return true;
		}
		if(pMsg->wParam == VK_ESCAPE)
		{
			m_bESC = true;
			PostMessage( WM_CLOSE );
			return true;
		}
		if(pMsg->wParam == VK_DELETE)
		{
			Clear();
			return true;
		}
	}
	else if(pMsg->message == WM_VSCROLL){
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

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

	CWnd* pParent = GetParent();
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
//	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	DestroyWindow();
}



void CInPlaceEdit::OnSetFocus()
{
	//save initial text just in case
	GetWindowText(m_iniStr);
	SetSel(0,-1);
}


void CInPlaceEdit::EditSubLabel(int iItem, int iSubItem)
{
//    CControlListCtrl *pControlListCtrl = (CControlListCtrl*)m_pListCtrl;
//	pControlListCtrl->EditSubLabel(iItem, iSubItem);
}

UINT CInPlaceEdit::OnGetDlgCode()
{
    return CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}

void CInPlaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Up and down are in the OnKeyDown so that the user can hold down the arrow
    // keys to scroll through the entries.
	SHORT sh1 = GetKeyState(VK_LCONTROL);
	SHORT sh2 = GetKeyState(VK_RCONTROL);
	switch (nChar)
    {
		case VK_UP :
		{
			EditSubLabel (m_iItem - 1, m_iSubItem);
			return;
		}
		case VK_DOWN :
		{
			EditSubLabel (m_iItem + 1, m_iSubItem);
			return;
		}
		case VK_LEFT :
		{
			if (m_iSubItem == 1)
				EditSubLabel (m_iItem, m_nColumns-1);
			else if (m_iSubItem > 1)
				EditSubLabel (m_iItem, m_iSubItem-1);
			return;
		}
		case VK_RIGHT :
		{
			if (m_iSubItem < m_nColumns-1)	EditSubLabel (m_iItem , m_iSubItem+ 1);
			else							EditSubLabel (m_iItem , 1);
			return;
		}
		case VK_HOME :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				EditSubLabel (0, m_iSubItem);
				return;
			}
		}
		case VK_END :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				int Count = m_nRows - 1;
				EditSubLabel (Count, m_iSubItem);
				return;
			}
		}
    }
  
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CInPlaceEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();
	delete this;
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_iniStr );
	SetFocus();
	SetSel( 0, -1 );
	return 0;
}



