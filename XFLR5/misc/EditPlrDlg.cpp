/****************************************************************************
	
	CEditPlrDlg	class
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

// EditPlrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "EditPlrDlg.h"
#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"

/////////////////////////////////////////////////////////////////////////////
// CEditPlrDlg dialog


CEditPlrDlg::CEditPlrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditPlrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditPlrDlg)
	m_VariableType = _T("");
	//}}AFX_DATA_INIT
	m_pPolar  = NULL;
	m_pWPolar = NULL;
	m_pParent = pParent;
}


void CEditPlrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPlrDlg)
	DDX_Control(pDX, IDC_POINTLIST, m_ctrlPointList);
	DDX_Text(pDX, IDC_VARIABLETYPE, m_VariableType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditPlrDlg, CDialog)
	//{{AFX_MSG_MAP(CEditPlrDlg)
	ON_BN_CLICKED(IDC_DELETEPT, OnDeletePt)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPlrDlg message handlers

BOOL CEditPlrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-400,60,0,0,SWP_NOSIZE);

	CString strong;
	
	if(m_pPolar){
		if(m_pPolar->m_Type !=4){
			m_VariableType = "Alpha";
			for (int i=0; i<m_pPolar->m_Alpha.GetSize(); i++){
				strong.Format("%8.3f", m_pPolar->m_Alpha.GetAt(i));
				m_ctrlPointList.AddString(strong);
			}
		}
		else {
			m_VariableType = "Reynolds";
			for (int i=0; i<m_pPolar->m_Alpha.GetSize(); i++){
				strong.Format("%8.0f", m_pPolar->m_Re.GetAt(i));
				m_ctrlPointList.AddString(strong);
			}
		}
	}

	else if(m_pWPolar){
		if(m_pWPolar->m_Type !=4){
			m_VariableType = "Alpha";
			for (int i=0; i<m_pWPolar->m_Alpha.GetSize(); i++){
				strong.Format("%8.3f", m_pWPolar->m_Alpha.GetAt(i));
				m_ctrlPointList.AddString(strong);
			}
		}
		else {
			m_VariableType = "Qinf";
			for (int i=0; i<m_pWPolar->m_Alpha.GetSize(); i++){
				strong.Format("%8.0f", m_pWPolar->m_QInfinite.GetAt(i));
				m_ctrlPointList.AddString(strong);
			}
		}
	}

	m_ctrlPointList.SetFocus();	
	UpdateData(FALSE);
	return FALSE; 
}

void CEditPlrDlg::OnDeletePt() 
{
	CString strong;
	double val;
	int i;
	int pos = m_ctrlPointList.GetCurSel();
	m_ctrlPointList.GetText(pos, strong);
	sscanf(strong, "%lf", &val);
	m_ctrlPointList.DeleteString(pos);

	if(LB_ERR == m_ctrlPointList.GetCount()) 
	{
		m_ctrlPointList.EnableWindow(FALSE);
		return;
	}

	pos--;
	if(pos<0) pos = 0;
	m_ctrlPointList.SetCurSel(pos);

	if(m_pPolar)
	{
		for (i=0; i<m_pPolar->m_Alpha.GetSize();i++)
		{
			if(m_pPolar->m_Type!=4)
			{
				if(abs(m_pPolar->m_Alpha[i]-val) <0.001) m_pPolar->Remove(i);
			}
			else if(m_pPolar->m_Type==4)
			{
				if(abs(m_pPolar->m_Re[i] - val)<0.1) m_pPolar->Remove(i);
			}
		}

		CXDirect *pXDirect = (CXDirect*)m_pParent;
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	else if(m_pWPolar)
	{
		for (i=0; i<m_pWPolar->m_Alpha.GetSize();i++)
		{
			if(m_pWPolar->m_Type!=4)
			{
				if(abs(m_pWPolar->m_Alpha[i]-val) <0.001) m_pWPolar->Remove(i);
			}
			else if(m_pWPolar->m_Type==4)
			{
				if(abs(m_pWPolar->m_QInfinite[i] - val)<0.1) m_pWPolar->Remove(i);
			}
		}

		CMiarex *pMiarex = (CMiarex*)m_pParent;
		pMiarex->CreateWPolarCurves();
		pMiarex->UpdateView();
	}
}


void CEditPlrDlg::OnDeleteAll() 
{
	m_ctrlPointList.ResetContent();
	if(LB_ERR == m_ctrlPointList.GetCount()) {
		m_ctrlPointList.EnableWindow(FALSE);
	}
	if(m_pPolar){
		for (int i=(int)m_pPolar->m_Alpha.GetSize()-1;i>=0;i--){
			m_pPolar->Remove(i);
		}
		CXDirect *pXDirect = (CXDirect*)m_pParent;
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	else if(m_pWPolar){
		for (int i=(int)m_pWPolar->m_Alpha.GetSize()-1;i>=0;i--){
			m_pWPolar->Remove(i);
		}
		CMiarex *pMiarex = (CMiarex*)m_pParent;
		pMiarex->CreateWPolarCurves();
		pMiarex->UpdateView();
	}
}


void CEditPlrDlg::OnOK() 
{
	CString strong;
	for (int i=0; i<m_ctrlPointList.GetCount(); i++){
		m_ctrlPointList.GetText(i, strong);
		m_Points.Add(strong);
	}
	
	CDialog::OnOK();
}
