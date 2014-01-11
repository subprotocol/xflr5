/****************************************************************************

    CPanelListCtrl Class
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


#pragma once
#include "afxcmn.h"
#include "../misc/InPlaceNumEdit.h"
#include "../misc/InPlaceFloatEdit.h"
#include "../misc/InPlaceList.h"

//  	 Zafir Anjum @ CODEGURU

class CPanelListCtrl : public CListCtrl
{
public:
	CPanelListCtrl(void);
	virtual ~CPanelListCtrl(void);

	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );
	CComboBox* ShowInPlaceList(int nItem, int nCol);
	void HighlightCell(int nItem, int nCol );
	void CloseEdit();
	CStringArray m_strList;
	int m_nColumns;
	int m_iItem, m_iSubItem;
	bool m_bIsEditing;
	CWnd *m_pWingDlg;

	CInPlaceFloatEdit m_IPFloatEdit;
	CInPlaceNumEdit m_IPNumEdit;
	CInPlaceList m_IPList;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
