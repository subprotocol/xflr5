/****************************************************************************

    CNoBeepListCtrl Class
	Copyright (C) 2008 André Deperrois xflr5@yahoo.com

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

#include "stdafx.h"
#include "../X-FLR5.h"
#include "..\Design\AFoilCtrlBar.h"
#include ".\nobeeplistctrl.h"


// CNoBeepListCtrl

IMPLEMENT_DYNAMIC(CNoBeepListCtrl, CListCtrl)
CNoBeepListCtrl::CNoBeepListCtrl()
{
	m_pParent = NULL;
}

CNoBeepListCtrl::~CNoBeepListCtrl()
{
}


BEGIN_MESSAGE_MAP(CNoBeepListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



int CNoBeepListCtrl::HitTestEx(CPoint &point, int *col) const
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


void CNoBeepListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int index;
	CString strong;
	CAFoilCtrlBar *pACtrlBar = (CAFoilCtrlBar*)m_pParent;

	int column;
	if((index = HitTestEx(point, &column)) != -1)
	{
		pACtrlBar->m_FoilSelection = index;
		SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 
	}
//	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), 0 );
//	CListCtrl::OnLButtonDown(nFlags, point);// no annoying Vista beep, thank you
}

BOOL CNoBeepListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		CAFoilCtrlBar *pACtrlBar = (CAFoilCtrlBar*)m_pParent;
		if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		if(pMsg->wParam == VK_DOWN)
		{

//			POSITION pos = GetFirstSelectedItemPosition();
//			int iItem = GetNextSelectedItem(pos);
//			int iSubItem = m_iSubItem;

			if (pACtrlBar->m_FoilSelection < GetItemCount()-1) pACtrlBar->m_FoilSelection ++;

			EnsureVisible(pACtrlBar->m_FoilSelection, true);

			SetItemState(pACtrlBar->m_FoilSelection, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED); 
			return TRUE;
		}
		if(pMsg->wParam == VK_UP)
		{

//			POSITION pos = GetFirstSelectedItemPosition();
//			int iItem = GetNextSelectedItem(pos);
//			int iSubItem = m_iSubItem;

			if(pACtrlBar->m_FoilSelection>0) pACtrlBar->m_FoilSelection--;
			EnsureVisible(pACtrlBar->m_FoilSelection, true);
			SetItemState(pACtrlBar->m_FoilSelection, LVIS_SELECTED | LVIS_FOCUSED,	LVIS_SELECTED | LVIS_FOCUSED); 
	
			return TRUE;
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}

