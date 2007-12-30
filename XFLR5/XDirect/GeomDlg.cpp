/****************************************************************************

	Geometry Dialog class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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

// GeomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "XDirect.h"
#include "GeomDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CGeomDlg dialog


CGeomDlg::CGeomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeomDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeomDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bModified = false;
	m_bApplied  = true;
	m_bAppliedX = true;
}


void CGeomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeomDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_CAMB, m_ctrlCamb);
	DDX_Control(pDX, IDC_CAMBERSLIDE, m_ctrlCamberSlide);
	DDX_Control(pDX, IDC_THICKSLIDE, m_ctrlThickSlide);
	DDX_Control(pDX, IDC_XTHICKSLIDE, m_ctrlXThickSlide);
	DDX_Control(pDX, IDC_XCAMBERSLIDE, m_ctrlXCamberSlide);
	DDX_Control(pDX, IDC_XCAMBER, m_ctrlXCamber);
	DDX_Control(pDX, IDC_XTHICKNESS, m_ctrlXThickness);
	DDX_Control(pDX, IDC_THICKNESS, m_ctrlThickness);
	DDX_Control(pDX, IDC_CAMBER, m_ctrlCamber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeomDlg, CDialog)
	//{{AFX_MSG_MAP(CGeomDlg)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_CAMBER, OnChanged)
	ON_EN_CHANGE(IDC_THICKNESS, OnChanged)
	ON_EN_CHANGE(IDC_XTHICKNESS, OnChangedX)
	ON_EN_CHANGE(IDC_XCAMBER, OnChangedX)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeomDlg message handlers

BOOL CGeomDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_fCamber     = m_pMemFoil->m_fCamber;
	m_fThickness  = m_pMemFoil->m_fThickness;
	m_fXCamber    = m_pMemFoil->m_fXCamber;
	m_fXThickness = m_pMemFoil->m_fXThickness;

	if(fabs(m_fCamber) <0.0001) {
		m_ctrlCamb.SetWindowText("The foil's camber is too small to be scaled");
		m_ctrlCamberSlide.EnableWindow(false);
		m_ctrlCamber.EnableWindow(false);
	}

	m_ctrlCamber.SetPrecision(2);
	m_ctrlThickness.SetPrecision(2);
	m_ctrlCamber.SetValue(m_fCamber*100.0);
	m_ctrlThickness.SetValue(m_fThickness*100.0);

	m_ctrlCamberSlide.SetRange(0,100,false);
	m_ctrlCamberSlide.SetPos((int)(m_fCamber*1000.0));
	m_ctrlCamberSlide.SetTicFreq(5);

	m_ctrlThickSlide.SetRange(0,200,false);
	m_ctrlThickSlide.SetPos((int)(m_fThickness*1000.0));
	m_ctrlThickSlide.SetTicFreq(10);
	
	m_ctrlXCamber.SetPrecision(2);
	m_ctrlXThickness.SetPrecision(2);
	m_ctrlXCamber.SetValue(m_fXCamber*100.0);
	m_ctrlXThickness.SetValue(m_fXThickness*100.0);

	m_ctrlXCamberSlide.SetRange(0,1000,false);
	m_ctrlXCamberSlide.SetPos((int)(m_fXCamber*1000.0));
	m_ctrlXCamberSlide.SetTicFreq(100);

	m_ctrlXThickSlide.SetRange(0,1000,false);
	m_ctrlXThickSlide.SetPos((int)(m_fXThickness*1000.0));
	m_ctrlXThickSlide.SetTicFreq(100);

	m_bApplied  = true;
	m_bAppliedX = true;

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CGeomDlg::OnOK() 
{
	if(!m_bApplied || !m_bAppliedX)	Apply();
	if(!m_bModified) EndDialog(0);
	else EndDialog(1);
}


void CGeomDlg::OnRestore() 
{
	m_pBufferFoil->CopyFoil(m_pMemFoil);
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_fThickness = m_pMemFoil->m_fThickness;
	m_fCamber    = m_pMemFoil->m_fCamber;
	m_fXThickness = m_pMemFoil->m_fXThickness;
	m_fXCamber    = m_pMemFoil->m_fXCamber;
	m_pXFoil->thickb  = m_fThickness;
	m_pXFoil->cambrb = m_fCamber;

	m_ctrlThickness.SetValue(m_fThickness*100.0);
	m_ctrlCamber.SetValue(m_fCamber*100.0);
	m_ctrlThickSlide.SetPos((int)(m_fThickness*1000.0));
	m_ctrlCamberSlide.SetPos((int)(m_fCamber*1000.0));

	m_ctrlXThickness.SetValue(m_fXThickness*100.0);
	m_ctrlXCamber.SetValue(m_fXCamber*100.0);
	m_ctrlXThickSlide.SetPos((int)(m_fXThickness*1000.0));
	m_ctrlXCamberSlide.SetPos((int)(m_fXCamber*1000.0));

	m_bApplied  = true;
	m_bAppliedX = true;
	m_bModified = false;

	m_pChildView->Invalidate();
}


BOOL CGeomDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDOK) == ppp) 
			OnOK();
		else if(GetDlgItem(IDCANCEL) == ppp) 
			OnCancel();
		else {
			Apply();
			m_ctrlOK.SetFocus();
			m_bApplied  = true;
			m_bAppliedX = true;
		}
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CGeomDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_CAMBERSLIDE)->GetSafeHwnd())    {
		m_fCamber = (double)m_ctrlCamberSlide.GetPos()/10.0;
		m_ctrlCamber.SetValue(m_fCamber);
		Apply();
	}
	else if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_THICKSLIDE)->GetSafeHwnd())    {
		m_fThickness = (double)m_ctrlThickSlide.GetPos()/10.0;
		m_ctrlThickness.SetValue(m_fThickness);
		Apply();
	}
	else if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_XCAMBERSLIDE)->GetSafeHwnd())    {
		m_fXCamber = (double)m_ctrlXCamberSlide.GetPos()/10.0;
		m_ctrlXCamber.SetValue(m_fXCamber);
		Apply();
	}
	else if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_XTHICKSLIDE)->GetSafeHwnd())    {
		m_fXThickness = (double)m_ctrlXThickSlide.GetPos()/10.0;
		m_ctrlXThickness.SetValue(m_fXThickness);
		Apply();
	}
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CGeomDlg::OnChanged() 
{
	m_bApplied = false;	
}
void CGeomDlg::OnChangedX() 
{
	m_bAppliedX = false;	
}

void CGeomDlg::Apply() 
{
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

	if(!m_bApplied){
		double thickness = m_ctrlThickness.GetValue()/100.0;
		double camber    = m_ctrlCamber.GetValue()/100.0;
		m_pXFoil->tcset(camber, thickness);
		m_ctrlCamberSlide.SetPos((int)(camber*100*10));
		m_ctrlThickSlide.SetPos((int)(thickness*100*10));
		m_bApplied = true;
	}

	if(!m_bAppliedX){
		double Xthickness = m_ctrlXThickness.GetValue()/100.0;
		double Xcamber    = m_ctrlXCamber.GetValue()/100.0;
		m_pXFoil->hipnt(Xcamber, Xthickness);
		m_ctrlXCamberSlide.SetPos((int)(Xcamber*100*10));
		m_ctrlXThickSlide.SetPos((int)(Xthickness*100*10));
		m_bAppliedX = true;
	}

	if(m_pXFoil->nb>350) {
		AfxMessageBox("Panel number cannot exceed 350", MB_OK);
		//reset everything and retry
		for (int i=0; i< m_pMemFoil->nb; i++){
			m_pXFoil->x[i+1] = m_pMemFoil->xb[i];
			m_pXFoil->y[i+1] = m_pMemFoil->yb[i];
		}
		m_pXFoil->n = m_pMemFoil->nb;
	}
	else{
		for (int j=0; j< m_pXFoil->nb; j++){
			m_pBufferFoil->xb[j] = m_pXFoil->xb[j+1];
			m_pBufferFoil->yb[j] = m_pXFoil->yb[j+1];
		}
		m_pBufferFoil->nb = m_pXFoil->nb;
//		m_pXFoil->SetFoilFlap(m_pBufferFoil);
		m_pBufferFoil->InitFoil();
		m_pBufferFoil->SetFlap();
	}
	m_bModified = true;
	m_pChildView->Invalidate();

}
