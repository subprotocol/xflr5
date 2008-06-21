/****************************************************************************

	
	CEditListCtrl	class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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
//
// Built with the help of C++ Codegurus @ www.codeguru.com
// Special thanks to Zafir Anjum's input
//
// SubClass specially (and only) designed for wing definition
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "InPlaceFloatEdit.h"
#include "InPlaceList.h"
#include ".\Editlistctrl.h"


CEditListCtrl::CEditListCtrl(void)
{
	m_nColumns = 0;
	memset(m_iPrecision,0, sizeof(m_iPrecision));
	m_iItem = 0;
	m_iSubItem = 0;
	m_bIsEditing = false;
}

CEditListCtrl::~CEditListCtrl(void)
{
}
BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CONTROLLIST, OnEndLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_RELIST2, OnEndLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_COORDLIST, OnEndLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FRAMELIST, OnEndLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FRAMEPOINTS, OnEndLabelEdit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



//Zafir Anjum
// HitTestEx	- Determine the row index and column index for a point
// Returns	- the row index or -1 if point is not over a row
// point	- point to be tested.
// col		- to hold the column index
int CEditListCtrl::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );

	if(col) *col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect(row, &rect, LVIR_BOUNDS);
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left
					&& point.x <= (rect.left + colwidth ) )
				{
					if( col ) *col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}


// EditSubLabel		- Start edit of a sub item label
// Returns		- Temporary pointer to the new edit control
// nItem		- The row index of the item to edit
// nCol			- The column of the sub item.
CInPlaceFloatEdit* CEditListCtrl::EditSubLabel( int nItem, int nCol )
{
	int i;

	if(m_iSubItem==0) return NULL;

	if(nItem==-1) nItem =GetItemCount()-1;
	if(nItem==GetItemCount()) nItem =0;

	// The returned pointer should not be saved

	// Make sure that the item is visible
	if(!EnsureVisible(nItem, TRUE)) return NULL;

	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if(nCol>=nColumnCount || GetColumnWidth(nCol)<5)
		return NULL;

	// Get the column offset
	int offset = 0;
	for(i=0; i < nCol; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect( nItem, &rect, LVIR_BOUNDS );

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn( nCol, &lvcol );
	DWORD dwStyle ;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left + GetColumnWidth( nCol ) - 3 ;
	if( rect.right > rcClient.right) rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;

	CInPlaceFloatEdit *pEdit = new CInPlaceFloatEdit(this, nItem, nCol, GetItemText(nItem, nCol));
	if(pEdit){
		pEdit->Create( dwStyle, rect, this, IDC_PEDIT);
		pEdit->m_ParentList = 1;
		pEdit->m_nColumns = m_nColumns;
		pEdit->m_nRows    = GetItemCount();
		pEdit->m_iPrecision = m_iPrecision[nCol];
		m_bIsEditing = true;
	}
	SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 

	return pEdit;
}


void CEditListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEditListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( GetFocus() != this ) SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

/*
void CEditListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SHORT sh1 = GetKeyState(VK_LCONTROL);
	SHORT sh2 = GetKeyState(VK_RCONTROL);
	POSITION pos = GetFirstSelectedItemPosition();
	int iItem = GetNextSelectedItem(pos);
//	int iSubItem = GetSelectedColumn();
	int iSubItem = m_iSubItem;
	switch (nChar)
	{
	case VK_UP :
		{
			if (iItem > 0)
				EditSubLabel (iItem - 1, iSubItem);
			return;
		}
	case VK_DOWN :
		{
			EditSubLabel (iItem + 1, iSubItem);
			return;
		}
	case VK_LEFT :
		{
			if (iSubItem > 0)
				EditSubLabel (iItem, iSubItem-1);
			return;
		}
	case VK_RIGHT :
		{
			if (iSubItem < m_nColumns-1)	EditSubLabel (iItem , iSubItem+ 1);
			else							EditSubLabel (iItem , 1);
			return;
		}
	case VK_HOME :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				EditSubLabel (0, iSubItem);
				return;
			}
		}
	case VK_END :
		{
			if ((sh1 & 0x8000)||(sh2 & 0x8000)){
				int Count = GetItemCount() - 1;
				EditSubLabel (Count, iSubItem);
				return;
			}
		}
	}
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/
BOOL CEditListCtrl::PreTranslateMessage(MSG* pMsg)
{
/*		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;				// DO NOT process further
		}*/
	return CListCtrl::PreTranslateMessage(pMsg);
}


void CEditListCtrl::HighlightCell(int nItem, int nCol )
{
	CDC *pDC = GetDC();
	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =  ((lvi.state & LVIS_DROPHILITED)
					|| ((lvi.state & LVIS_SELECTED) && ((GetFocus() == this)
					|| (GetStyle() & LVS_SHOWSELALWAYS))));


	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol( rcBounds );


	CString sLabel = GetItemText( nItem, 0 );

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	CRect rcHighlight;
	CRect rcWnd;
	int nExt;
	switch( 1 )
	{
	case 0:
		nExt = pDC->GetOutputTextExtent(sLabel).cx + offset;
		rcHighlight = rcLabel;
		if( rcLabel.left + nExt < rcLabel.right )
			rcHighlight.right = rcLabel.left + nExt;
		break;
	case 1:
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		break;
	case 2:
		GetClientRect(&rcWnd);
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		rcHighlight.right = rcWnd.right;
		break;
	default:
		rcHighlight = rcLabel;
	}

	// Draw the highlighted background color
	pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

	pDC->FillRect(rcHighlight, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));

	// Draw hilghlighted item label - Column 0
	rcLabel.left += offset/2;
	rcLabel.right -= offset;

	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
				| DT_VCENTER | DT_END_ELLIPSIS);

	ReleaseDC(pDC);
}

void CEditListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM	*plvItem = &plvDispInfo->item;
	m_bIsEditing = false;

	if (plvItem->pszText != NULL)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}

	*pResult = FALSE;
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)plvDispInfo );
}

void CEditListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int index;
//	int CurSel = m_iItem;
	CString strong;
//	CListCtrl::OnLButtonDown(nFlags, point);

	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		m_iSubItem = column;
		if(column>0){
			UINT flag = LVIS_FOCUSED;
			if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
				EditSubLabel(index, column);
		}
		else SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 
	}
}

void CEditListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	int index;
	CString strong;

	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		m_iItem = index;
		m_iSubItem = column;
		if(column==0) column=1;
		SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 

	}
//	CListCtrl::OnRButtonDown(nFlags, point);
}

void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int index;
	CString strong;
	CListCtrl::OnLButtonDblClk(nFlags, point);

	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		m_iItem = index;
		m_iSubItem = column;
		UINT flag = LVIS_FOCUSED;
		if( GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )
			EditSubLabel(index, column);
	}
}


BOOL CEditListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(m_bIsEditing) return TRUE;
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
