/****************************************************************************

    Body Control Class
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

// AFoilCtrl.h: interface for the CBodyCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../misc/InitDialogBar.h"
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "../misc/EditListCtrl.h"

 
class CBodyCtrlBar : public CInitDialogBar
{ 
	DECLARE_DYNAMIC(CBodyCtrlBar)
	friend class CMainFrame;
	friend class CChildView;
	friend class CMiarex;
 
	// Construction
	public:
	CBodyCtrlBar(CWnd* pParent = NULL);
	virtual ~CBodyCtrlBar();

	// Attributes
protected:
	CButton m_ctrlRadio1, m_ctrlRadio2;
	CButton	m_ctrlBodyColor;
	CButton m_ctrlLocked;
	CStatic m_ctrlBodyArea;
	CNumEdit m_ctrlnxPanels, m_ctrlnhPanels;
	CComboBox m_ctrlXDegree;
	CComboBox m_ctrlHoopDegree;
	CComboBox m_ctrlBodyList;
	CEditListCtrl m_ctrlFrameList;
	CEditListCtrl m_ctrlFramePoints;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBodyCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();
 
	// Implementation
	public:
private:
	void FillFrameList();
	void FillPointList();
	void UpdateBodies();
	void UpdateBodyCtrls();
	void ReSizeCtrls();
	void SetBody(CBody *pBody/* = NULL*/);
	void SetFrameSel(int sel);
	void SetPointSel(int sel);
	void SetFrame(int iFrame);
	void SetPoint(int iPoint);
	void ReadFrameData(int sel, double &x, double &z, int &n);
	void ReadPointData(int sel, double &y, double &z, int &n);
	void OnUndo();
	void OnRedo();
	void SetColumnTitle();
	void SetPicture();
	void StorePicture();
	void TakePicture();

	CBody m_TmpPic;
	CBody m_UndoPic[20];
	int m_StackPos, m_StackSize;// undo : current stack position and current stack size
	bool m_bStored;

	CFont m_FixedFont;
	CWnd* m_pMiarex;
	CWnd* m_pMainFrame;
	CWnd* m_pChildView;
	
	CObArray *m_poaBody;
	CBody **m_ppCurBody;

	// Generated message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBodyColor();
//	afx_msg void OnNMClickFrameList(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMClickFramePoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelChangeBodyList();
	afx_msg void OnLvnEndLabelEditFrameList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndLabelEditFramePoints(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInsertPoint();
	afx_msg void OnDeletePoint();
	afx_msg void OnInsertFrame();
	afx_msg void OnDeleteFrame();
	afx_msg void OnRadio();
	afx_msg void OnSelChangeSplineDegree();
	afx_msg void OnKillFocusPanels();
	afx_msg void OnLocked();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLvnItemchangedFramelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedFramepoints(NMHDR *pNMHDR, LRESULT *pResult);
};
