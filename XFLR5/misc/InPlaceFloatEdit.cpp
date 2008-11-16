/****************************************************************************

    CInPlaceFloatEdit Class
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
#include "InPlaceFloatEdit.h"
#include "../X-FLR5.h"
#include "../Miarex/PanelListCtrl.h"
#include "EditListCtrl.h"
#include <math.h>


CInPlaceFloatEdit::CInPlaceFloatEdit()
{
	m_fMin = -1.e10;
	m_fMax =  1.e10;
	m_iPrecision = 2;
	m_iItem = 0;
	m_iSubItem = 0;
	m_nColumns = 0;
	m_nRows = 0;
	m_bESC = FALSE;
	m_ParentList = -1;
}

CInPlaceFloatEdit::CInPlaceFloatEdit(CListCtrl* pCtrl, int iItem, int iSubItem, CString sInitText)
{
	m_fMin = -1.e10;
	m_fMax =  1.e10;
	m_iPrecision = 2;
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_nColumns = 0;
	m_nRows = 0;
	m_bESC = FALSE;
	m_pListCtrl = pCtrl;
	m_ParentList = -1;
	m_iniStr = sInitText;
}

CInPlaceFloatEdit::~CInPlaceFloatEdit(void)
{
}

BEGIN_MESSAGE_MAP(CInPlaceFloatEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceFloatEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFloatEdit message handlers

BOOL CInPlaceFloatEdit::PreTranslateMessage(MSG* pMsg)
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


void CInPlaceFloatEdit::Set(int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_iniStr = sInitText;
}

void CInPlaceFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_ESCAPE) {
		m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	switch(nChar){
		case 8:{ //BackSpace
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
			}

/*		case VK_TAB :
		{
			SHORT sh   = GetKeyState(VK_SHIFT);
			if ((sh & 0x8000))	{
				if (m_iSubItem > 1) EditParentSubLabel(m_iItem, m_iSubItem - 1);
				else				EditParentSubLabel(m_iItem, m_nColumns - 1);
				
			}
			else{
				if (m_iSubItem < m_nColumns-1)	EditParentSubLabel(m_iItem, m_iSubItem + 1);
				else							EditParentSubLabel(m_iItem , 1);
			}
			return;
		}*/
		case ' ': return;
		default: {
			if (nChar != 45 &&
				nChar != 46 &&
				nChar !=101 &&
				!(nChar>=48 && nChar<=57)) return;
			CString oldstr;
			GetWindowText(oldstr);
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			if (!IsValid())	SetWindowText(oldstr);
		 }
	}
}


void CInPlaceFloatEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Up and down are in the OnKeyDown so that the user can hold down the arrow
    // keys to scroll through the entries.
/*	SHORT sh1 = GetKeyState(VK_LCONTROL);
	SHORT sh2 = GetKeyState(VK_RCONTROL);
	switch (nChar)
    {
		case VK_UP :
		{
			EditParentSubLabel (m_iItem - 1, m_iSubItem);
			return;
		}
		case VK_DOWN :
		{
			EditParentSubLabel (m_iItem + 1, m_iSubItem);
			return;
		}
		case VK_LEFT :
		{
			if (m_iSubItem == 1)
				EditParentSubLabel (m_iItem, m_nColumns-1);
			else if (m_iSubItem > 1)
				EditParentSubLabel (m_iItem, m_iSubItem-1);
			return;
		}
		case VK_RIGHT :
		{
			if (m_iSubItem < m_nColumns-1)	EditParentSubLabel (m_iItem , m_iSubItem+ 1);
			else							EditParentSubLabel (m_iItem , 1);
			return;
		}
		case VK_HOME :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				EditParentSubLabel (0, m_iSubItem);
				return;
			}
		}
		case VK_END :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				int Count = m_nRows - 1;
				EditParentSubLabel (Count, m_iSubItem);
				return;
			}
		}
		
    }*/
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CInPlaceFloatEdit::Clear()
{
 	SetWindowText("");
}

bool CInPlaceFloatEdit::CheckBeforeExit()
{
	double fValue;
	bool bValid = GetValue(fValue);
	if (!bValid || fValue<m_fMin || fValue> m_fMax){
		return false;
	}
	return true;
}

void CInPlaceFloatEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	if(!CheckBeforeExit()) 
		SetWindowText(m_iniStr);//user blundered
	else 
	{
		double fValue = GetValue();
		SetValue(fValue);
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
	PostMessage( WM_CLOSE );

} 

int CInPlaceFloatEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


bool CInPlaceFloatEdit::IsValid()
{
	CString str;
	GetWindowText(str);
	str.Replace(" ","");
	double f;
	char lp[10];
	if (str=="." || str=="-" || str=="-." || sscanf(str, "%lf%s", &f, lp) == 1){
		return true;
	}
	str.Empty();
	return false;
}

double CInPlaceFloatEdit::GetValue()
{
	double f;
	if (IsValid())	{
		CString str;
		GetWindowText(str);
		str.Replace(" ","");
		sscanf(str, "%lf", &f);
		return f;
	}
	else return 0.f;//perfectible...
}

bool CInPlaceFloatEdit::GetValue(double &f)
{
	if (IsValid())	{
		CString str;

		GetWindowText(str);
		str.Replace(" ","");
		sscanf(str, "%lf", &f);
		return true;
	}
	else return false;
}

void CInPlaceFloatEdit::SetValue(double f)
{
 	CString str, str1;
	if ((f==0.0 || abs(f)>=pow(10.0, -m_iPrecision)) && f <1000.0){
		switch (m_iPrecision){
			//there probably is a more elegant way to do this,
			case 0:
				str.Format("%.0f", f);
				break;
			case 1:
				str.Format("%.1f", f);
				break;
			case 2:
				str.Format("%.2f", f);
				break;
			case 3:
				str.Format("%.3f", f);
				break;
			case 4:
				str.Format("%.4f", f);
				break;
			case 5:
				str.Format("%.5f", f);
				break;
			case 6:
				str.Format("%.6f", f);
				break;
			case 7:
				str.Format("%.7f", f);
				break;
			case 8:
				str.Format("%.8f", f);
				break;
			case 9:
				str.Format("%.9f", f);
				break;
			default:
				str.Format("%.2f", f);
		}
	}
	else if(f>=1000.0){
		int q, r, exp;
		exp = (int)log10(f);
		r = exp%3;
		q = (exp-r)/3;

		CString strong;
		strong.Format("%.0f", f);

		switch (m_iPrecision){
			//there probably is a more elegant way to do this,
			case 0:
				strong.Format("%.0f", f);
				break;
			case 1:
				strong.Format("%.1f", f);
				break;
			case 2:
				strong.Format("%.2f", f);
				break;
			case 3:
				strong.Format("%.3f", f);
				break;
			case 4:
				strong.Format("%.4f", f);
				break;
			case 5:
				strong.Format("%.5f", f);
				break;
			case 6:
				strong.Format("%.6f", f);
				break;
			case 7:
				strong.Format("%.7f", f);
				break;
			case 8:
				strong.Format("%.8f", f);
				break;
			case 9:
				strong.Format("%.9f", f);
				break;
			default:
				strong.Format("%.2f", f);
		}

		int l = strong.GetLength();
		int pos = strong.Find(".");
		if(pos<0) pos = l;
		pos = l-pos;

		for (int i=0; i<q; i++){
			strong.Insert(l-3*(i+1)-i-pos," ");
			l++;
		}

		str = strong;
	}
	else
	{
		int exp  = (int)log10(f)-1;
		str1.Format("e%d", exp);

		double main = f/pow(10.0, exp)*1.000001;
		switch (m_iPrecision){
			case 0:
				str.Format("%.0f", main);
				break;
			case 1:
				str.Format("%.1f", main);
				break;
			case 2:
				str.Format("%.2f", main);
				break;
			case 3:
				str.Format("%.3f", main);
				break;
			case 4:
				str.Format("%.4f", main);
				break;
			case 5:
				str.Format("%.5f", main);
				break;
			default:
				str.Format("%.2f", main);
		}
		str += str1;
	}
 	SetWindowText(str);
}



void CInPlaceFloatEdit::OnSetFocus() 
{
	//save initial text just in case
	GetWindowText(m_iniStr);
	m_iniStr.Replace(" ","");
	SetWindowText(m_iniStr);
	SetSel(0,-1);
}

UINT CInPlaceFloatEdit::OnGetDlgCode()
{
    return CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTTAB;
}
