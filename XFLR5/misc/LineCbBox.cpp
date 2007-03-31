/****************************************************************************

    LineComboBox Class
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


// LineCbBox.cpp : implementation file
//
#include "stdafx.h"
#include "LineCbBox.h"


/////////////////////////////////////////////////////////////////////////////
// CLineCbBox

CLineCbBox::CLineCbBox(){
	m_bWidth = false;
}

CLineCbBox::~CLineCbBox(){
}


BEGIN_MESSAGE_MAP(CLineCbBox, CComboBox)
	//{{AFX_MSG_MAP(CLineCbBox)
	ON_WM_DRAWITEM()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineCbBox message handlers

int CLineCbBox::AddLine()
{
	
	CComboBox::AddString("");
	int i = GetCount();

	if(SetItemDataPtr(i-1, m_Lines+i-1)==CB_ERR){
		ASSERT(false);
	}
	return i;
}

int CLineCbBox::AddString(LPCTSTR lpszString)
{
	ASSERT(FALSE); // cannot add strings 
	return CB_ERR;
}

int CLineCbBox::InsertString(int nIindex, LPCTSTR lpszString)
{
	ASSERT(FALSE); // cannot insert strings
	return CB_ERR;

}

int CLineCbBox::DeleteString(int nIndex)
{
	ASSERT(FALSE); // cannot delete strings
	return CB_ERR;
}

void CLineCbBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = 20;
}


void CLineCbBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	if(!pDC) return;

	if (!IsWindowEnabled())	{
		CBrush brDisabled(RGB(192,192,192)); // light gray
		CBrush* pOldBrush = pDC->SelectObject(&brDisabled);
		CPen penDisabled(PS_SOLID, 1, RGB(192,192,192));
		CPen* pOldPen = pDC->SelectObject(&penDisabled);
		DrawLine(lpDIS);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		return;
	}
	
	// Selected
	if ((lpDIS->itemState & ODS_SELECTED) 
		&& (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brHighlight(GetSysColor(COLOR_HIGHLIGHT)); 
		CBrush* pOldBrush = pDC->SelectObject(&brHighlight);
		CPen penHighlight(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDIS->rcItem);
		pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		DrawLine(lpDIS);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
	else{
	// De-Selected
//	if (!(lpDIS->itemState & ODS_SELECTED) 
//		&& (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brWindow(GetSysColor(COLOR_WINDOW)); 
		CBrush* pOldBrush = pDC->SelectObject(&brWindow);
		CPen penHighlight(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDIS->rcItem);
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		DrawLine(lpDIS);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

    // Focus
	if (lpDIS->itemAction & ODA_FOCUS) 	{ 
		pDC->DrawFocusRect(&lpDIS->rcItem);
	}
}


void CLineCbBox::DrawLine(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	LineData* pLineData = (LineData*) GetItemDataPtr(lpDIS->itemID);
	ASSERT(pLineData != NULL);

//	CPen CurPen(pLineData->nPenStyle, pLineData->nWidth, pLineData->crColor);
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = pLineData->crColor;
	int w = pLineData->nWidth;
	if (w==0) w=1;
	CPen CurPen(PS_GEOMETRIC | pLineData->nPenStyle, w, &lb);
	CPen *pOldPen = pDC->SelectObject(&CurPen);

	if(m_bWidth){
		pDC->MoveTo(lpDIS->rcItem.left+35, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
		pDC->LineTo(lpDIS->rcItem.right-5, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
		CString str;
		if(pLineData->nWidth == 0) str = "0.5 pt";
		else str.Format("%d pt", pLineData->nWidth);
		pDC->TextOut(2,lpDIS->rcItem.top+2,str);
	}
	else {
		pDC->MoveTo(lpDIS->rcItem.left+5, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
		pDC->LineTo(lpDIS->rcItem.right-5, (lpDIS->rcItem.bottom + lpDIS->rcItem.top) / 2);
	}
	pDC->SelectObject(pOldPen);
}



BOOL CLineCbBox::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN){ 
		if(GetDroppedState()){
			int pos = GetCurSel();
			if (pos<GetCount()) {
				pos++;
				SetCurSel(pos);
				Invalidate();
			}
			return true; // no need to process further
		}
		else {
			ShowDropDown(true);
			return true; // no need to process further
		}
	} 

	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP){ 
		int pos = GetCurSel();
		if (pos>0){
			pos--;
			SetCurSel(pos);
			Invalidate();
		}
		return true; // no need to process further
	} 
	return CComboBox::PreTranslateMessage(pMsg);
}
