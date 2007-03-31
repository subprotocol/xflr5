/****************************************************************************

    NumEdit Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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


// NumEdit.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "NumEdit.h"


/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit(){
	//until we know better
	m_iMin = -10000000;
	m_iMax =  10000000;
}

CNumEdit::~CNumEdit(){
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

int CNumEdit::GetValue(){
	int i;
	if (IsValid())	{
		CString str;
		GetWindowText(str);
		sscanf(str, "%d", &i);
		return i;
	}
	else return 0;//perfectible...
}

bool CNumEdit::IsValid(){
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

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// TODO: Add your message handler code here and/or call default
	// Make sure to check "Number" box in dialog control...
	// and let MFC do the control work
	if (nChar == ' ') return;
	CString oldstr;
	GetWindowText(oldstr);
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if (!IsValid())	SetWindowText(oldstr);
}

void CNumEdit::SetValue(int i){
 	CString str;
 	str.Format("%d", i);
 	SetWindowText(str);
}


BOOL CNumEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && GetFocus() == this){ 
//		SetWindowText(m_iniStr);
//		GetParent()->SetFocus();
		GetParent()->PostMessage(WM_KEYDOWN, VK_ESCAPE);
		return true; // no need to process further
	} 
	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetFocus() == this){ 
		CheckBeforeExit();
//		GetParent()->SetFocus();
		GetParent()->PostMessage(WM_KEYDOWN, VK_RETURN);
		return true; //no need to process further
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CNumEdit::OnKillFocus(CWnd* pNewWnd) {
	CEdit::OnKillFocus(pNewWnd);

	if(CheckBeforeExit()) 
		SetWindowText(m_iniStr);//user blundered
}


bool CNumEdit::CheckBeforeExit(){
	int iValue = GetValue();
	if (iValue<m_iMin || iValue> m_iMax){
		SetWindowText(m_iniStr);
		return true;//no need for modification
	}
	return false;//no need for modification
}


void CNumEdit::OnSetFocus(CWnd* pOldWnd) {
	CEdit::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
	//save initial text just in case
	GetWindowText(m_iniStr);
	SetSel(0,-1);
}

void CNumEdit::SetMin(int k)
{
	m_iMin = k;
}

void CNumEdit::SetMax(int k)
{
	m_iMax = k;
}
