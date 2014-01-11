/****************************************************************************

    CInPlaceNumEdit Class
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


#include "stdafx.h"
#include ".\inplacenumedit.h"
#include "../Miarex/PanelListCtrl.h"


CInPlaceNumEdit::CInPlaceNumEdit()
{
	m_nColumns = 0;
	m_nRows = 0;
	m_bESC = FALSE;
	//until we know better
	m_iMin = -10000000;
	m_iMax =  10000000;
}


CInPlaceNumEdit::CInPlaceNumEdit(CListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_nColumns = 0;
	m_nRows = 0;
	m_bESC = FALSE;
	m_pListCtrl = pCtrl;

	m_iniStr = sInitText;
	//until we know better
	m_iMin = -10000000;
	m_iMax =  10000000;
}

CInPlaceNumEdit::~CInPlaceNumEdit(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// CInPlaceNumEdit



BEGIN_MESSAGE_MAP(CInPlaceNumEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceNumEdit)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceNumEdit message handlers

int CInPlaceNumEdit::GetValue()
{
	int i;
	if (IsValid())	{
		CString str;
		GetWindowText(str);
		sscanf(str, "%d", &i);
		return i;
	}
	else return 0;//perfectible...
}

bool CInPlaceNumEdit::IsValid()
{
	CString str;
	GetWindowText(str);
	int i;
	char lp[10];
	if (sscanf(str, "%d%s", &i, lp) > 1){
		str.Empty();
		return false;
	}
	
	return true;
}

void CInPlaceNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Make sure to check "Number" box in dialog control...
	// and let MFC do the control work
	if (nChar == ' ') return;
	CString oldstr;
	GetWindowText(oldstr);
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if (!IsValid())	SetWindowText(oldstr);
}

void CInPlaceNumEdit::SetValue(int i)
{
 	CString str;
 	str.Format("%d", i);
 	SetWindowText(str);
}


BOOL CInPlaceNumEdit::PreTranslateMessage(MSG* pMsg) 
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


void CInPlaceNumEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	if(CheckBeforeExit()) 
		SetWindowText(m_iniStr);//user blundered
	else {
		int Value = GetValue();
		SetValue(Value);
	}

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

//	DestroyWindow();
}


bool CInPlaceNumEdit::CheckBeforeExit()
{
	int iValue = GetValue();
	if (iValue<m_iMin || iValue> m_iMax){
		SetWindowText(m_iniStr);
		return true;//no need for modification
	}
	return false;//no need for modification
}


void CInPlaceNumEdit::OnSetFocus()
{
	//save initial text just in case
	GetWindowText(m_iniStr);
	SetSel(0,-1);
}

void CInPlaceNumEdit::SetMin(int k)
{
	m_iMin = k;
}

void CInPlaceNumEdit::SetMax(int k)
{
	m_iMax = k;
}

void CInPlaceNumEdit::Set(int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_iniStr = sInitText;
}

void CInPlaceNumEdit::EditSubLabel(int iItem, int iSubItem)
{

    CPanelListCtrl *pPanelListCtrl = (CPanelListCtrl*)m_pListCtrl;
	pPanelListCtrl->EditSubLabel(iItem, iSubItem);
}

UINT CInPlaceNumEdit::OnGetDlgCode()
{
    return CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}

void CInPlaceNumEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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



int CInPlaceNumEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
