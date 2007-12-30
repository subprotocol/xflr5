/****************************************************************************

    Miarex Class
	Copyright (C) 2006-2008 André Deperrois xflr5@yahoo.com

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
#include "../misc/FloatEdit.h"

// Boîte de dialogue CBodyGridDlg

class CBodyGridDlg : public CDialog
{
	friend class CMiarex;
	DECLARE_DYNAMIC(CBodyGridDlg)

public:
	CBodyGridDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CBodyGridDlg();

	CButton	m_ctrlScale;
	CButton	m_ctrlMinStyle;
	CButton	m_ctrlMinCheck;
	CButton	m_ctrlMainCheck;
	CButton	m_ctrlMainStyle;
	CFloatEdit	m_ctrlMinorUnit;
	CFloatEdit	m_ctrlUnit;

	CButton	m_ctrlMinStyle2;
	CButton	m_ctrlMinCheck2;
	CButton	m_ctrlMainCheck2;
	CButton	m_ctrlMainStyle2;
	CFloatEdit	m_ctrlMinorUnit2;
	CFloatEdit	m_ctrlUnit2;

	CStatic	m_ctrlLength1;
	CStatic	m_ctrlLength2;
	CStatic	m_ctrlLength3;
	CStatic	m_ctrlLength4;


// Données de boîte de dialogue
	enum { IDD = IDD_BGRIDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV


	CWnd* m_pFrame;
	CWnd* m_pMiarex;

	bool m_bScale;

	bool m_bGrid;
	int m_Style, m_Width;
	COLORREF m_Color;
	double m_Unit;
	bool m_bMinGrid;
	int m_MinStyle, m_MinWidth;
	COLORREF m_MinColor;
	double m_MinorUnit;

	bool m_bGrid2;
	int m_Style2, m_Width2;
	COLORREF m_Color2;
	double m_Unit2;
	bool m_bMinGrid2;
	int m_MinStyle2, m_MinWidth2;
	COLORREF m_MinColor2;
	double m_MinorUnit2;


	// Generated message map functions
	//{{AFX_MSG(CAGridDlg)
	afx_msg void OnScale();
	afx_msg void OnGridStyle();
	afx_msg void OnGridCheck();
	afx_msg void OnMinStyle();
	afx_msg void OnMinCheck();

	afx_msg void OnGridStyle2();
	afx_msg void OnGridCheck2();
	afx_msg void OnMinStyle2();
	afx_msg void OnMinCheck2();


	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
