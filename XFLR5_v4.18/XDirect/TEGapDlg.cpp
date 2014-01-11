/****************************************************************************

    CTEGapDlg Class
    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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

// TEGapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "TEGapDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CTEGapDlg dialog


CTEGapDlg::CTEGapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTEGapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTEGapDlg)
	//}}AFX_DATA_INIT
	m_Gap = 0.0;
	m_Blend = 0.8f;
//	m_pParent = pParent;
	m_bModified = false;
	m_bApplied = true;
}


void CTEGapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTEGapDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_BLEND, m_ctrlBlend);
	DDX_Control(pDX, IDC_GAP, m_ctrlGap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTEGapDlg, CDialog)
	//{{AFX_MSG_MAP(CTEGapDlg)
	ON_EN_CHANGE(IDC_BLEND, OnChanged)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_EN_CHANGE(IDC_GAP, OnChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTEGapDlg message handlers

BOOL CTEGapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlGap.SetMin(  0.0);
	m_ctrlGap.SetMax(100.0);
	
	m_ctrlBlend.SetMin(  0.0);
	m_ctrlBlend.SetMax(100.0);

	m_ctrlGap.SetValue(m_pMemFoil->m_Gap*100.0);
	m_ctrlBlend.SetValue(m_Blend*100.0);
	m_ctrlGap.SetFocus();

	return FALSE;
}

void CTEGapDlg::OnChanged() 
{
	m_bApplied = false;
}
	
BOOL CTEGapDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDOK) == ppp) 
			OnOK();
		else if(GetDlgItem(IDCANCEL) == ppp) 
			OnCancel();
		else {
			OnApply();
			m_ctrlOK.SetFocus();
		}
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CTEGapDlg::OnApply() 
{
	if(m_bApplied) return;
	//reset everything and retry

	for (int i=0; i< m_pMemFoil->nb; i++){
		m_pXFoil->xb[i+1] = m_pMemFoil->xb[i] ;
		m_pXFoil->yb[i+1] = m_pMemFoil->yb[i];
	}
	m_pXFoil->nb = m_pMemFoil->nb;

	m_pXFoil->lflap = false;
	m_pXFoil->lbflap = false;

	if(m_pXFoil->Preprocess()){
		m_pXFoil->CheckAngles();
/*		for (int k=0; k<m_pXFoil->n;k++){
			m_pMemFoil->nx[k] = m_pXFoil->nx[k+1];
			m_pMemFoil->ny[k] = m_pXFoil->ny[k+1];
		}
		m_pMemFoil->n = m_pXFoil->n;*/
	}
	else {
		AfxMessageBox("Unrecognized foil format", MB_OK);
		return;
	}

	m_ctrlGap.GetValue(m_Gap);
	m_ctrlBlend.GetValue(m_Blend);

	m_pXFoil->tgap(m_Gap/100.0,m_Blend/100.0);
	if(m_pXFoil->n>350) {
		AfxMessageBox("Panel number cannot exceed 350", MB_OK);
		//reset everything and retry
		for (int i=0; i< m_pMemFoil->nb; i++){
			m_pXFoil->x[i+1] = m_pMemFoil->xb[i] ;
			m_pXFoil->y[i+1] = m_pMemFoil->yb[i];
		}
		m_pXFoil->n = m_pMemFoil->nb;
	}
	else{
		for (int j=0; j< m_pXFoil->n; j++){
			m_pBufferFoil->xb[j] = m_pXFoil->xb[j+1];
			m_pBufferFoil->yb[j] = m_pXFoil->yb[j+1];
		}
		m_pBufferFoil->nb = m_pXFoil->nb;
//		m_pXFoil->SetFoilFlap(m_pBufferFoil);
		m_pBufferFoil->InitFoil();
		m_pBufferFoil->SetFlap();
	}
	m_bApplied = true;
	m_bModified = true;
	m_pChildView->Invalidate();
		
}

void CTEGapDlg::OnOK() 
{
	if(!m_bApplied)	OnApply();
	if(!m_bModified) EndDialog(0);
	else EndDialog(1);
//	CDialog::OnOK();
}
