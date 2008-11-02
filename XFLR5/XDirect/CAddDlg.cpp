/****************************************************************************

	Corner add dialog

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
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "XDirect.h"
#include "CAddDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CCAddDlg dialog


CCAddDlg::CCAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCAddDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCAddDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pChildView  = NULL;
	m_pBufferFoil = NULL;
	m_pMemFoil    = NULL;
	m_pXFoil      = NULL;

	m_iSplineType = 0;
}


void CCAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCAddDlg)
	DDX_Control(pDX, IDC_APPLY, m_ctrlApply);
	DDX_Control(pDX, IDC_ATPANEL, m_ctrlAtPanel);
	DDX_Control(pDX, IDC_TOTAL, m_ctrlTotal);
	DDX_Control(pDX, IDC_ADDED, m_ctrlAdded);
	DDX_Control(pDX, IDC_MAXANGLE, m_ctrlMaxAngle);
	DDX_Control(pDX, IDC_UNIFORM, m_ctrlUniform);
	DDX_Control(pDX, IDC_TO, m_ctrlTo);
	DDX_Control(pDX, IDC_FROM, m_ctrlFrom);
	DDX_Control(pDX, IDC_ANGTOL, m_ctrlAngTol);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCAddDlg, CDialog)
	//{{AFX_MSG_MAP(CCAddDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_UNIFORM, OnUniform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCAddDlg message handlers

void CCAddDlg::OnApply() 
{
	int i;
	for (i=0; i< m_pMemFoil->nb; i++)
	{
		m_pXFoil->xb[i+1] = m_pMemFoil->xb[i] ;
		m_pXFoil->yb[i+1] = m_pMemFoil->yb[i];
	}
	m_pXFoil->nb = m_pMemFoil->nb;

	m_pXFoil->lflap = false;
	m_pXFoil->lbflap = false;

	if(m_pXFoil->Preprocess())
	{
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

	if (IDC_UNIFORM == GetCheckedRadioButton(IDC_UNIFORM, IDC_UNIFORM2)) 
		m_iSplineType = 1;
	else
		m_iSplineType = 2;

	int added = m_pXFoil->cadd(m_iSplineType, m_ctrlAngTol.GetValue(),
				               m_ctrlFrom.GetValue(), m_ctrlTo.GetValue());
	m_pXFoil->abcopy();

	CString strong;
	strong.Format("Total number of points is %d", m_pXFoil->n);
	m_ctrlTotal.SetWindowText(strong);
	strong.Format("(added %d points to original foil)", added);
	m_ctrlAdded.SetWindowText(strong);

	for (i=0; i< m_pXFoil->n; i++){
		m_pBufferFoil->xb[i] = m_pXFoil->x[i+1];
		m_pBufferFoil->yb[i] = m_pXFoil->y[i+1];
	}
	m_pBufferFoil->nb = m_pXFoil->n;
//		m_pXFoil->SetFoilFlap(m_pBufferFoil);
	m_pBufferFoil->InitFoil();
	m_pBufferFoil->SetFlap();

	m_pXFoil->CheckAngles();
	strong.Format( "Maximum panel angle is %.1f", m_pXFoil->amax);
	m_ctrlMaxAngle.SetWindowText(strong);
	strong.Format( "at panel position %d", m_pXFoil->imax);
	m_ctrlAtPanel.SetWindowText(strong);

	m_pChildView->Invalidate();
}

void CCAddDlg::OnUniform() 
{
	if(m_ctrlUniform.GetCheck()) m_iSplineType = 1;
	else m_iSplineType = 2;	
}

BOOL CCAddDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	double xbmin, xbmax, xrf1, xrf2;
	int ispl;

	xbmin = m_pXFoil->xb[1];
	xbmax = m_pXFoil->xb[1];

	for( int i=1; i<= m_pXFoil->nb; i++){
		xbmin = __min(xbmin,m_pXFoil->xb[i]);
		xbmax = __max(xbmax,m_pXFoil->xb[i]);
	}

	//----- default inputs
	atol = 0.5f*m_pXFoil->amax;
	ispl = 1;
	xrf1 = xbmin - 0.1f*(xbmax-xbmin);
	xrf2 = xbmax + 0.1f*(xbmax-xbmin);

	m_ctrlUniform.SetCheck(1);
	m_iSplineType = 1;
	m_ctrlFrom.SetValue(xrf1);
	m_ctrlTo.SetValue(xrf2);
	m_ctrlAngTol.SetValue(atol);

	CString strong;
	m_pXFoil->CheckAngles();
	strong.Format( "Maximum panel angle is %.1f°", m_pXFoil->amax);
	m_ctrlMaxAngle.SetWindowText(strong);
	strong.Format( "at panel position %d", m_pXFoil->imax);
	m_ctrlAtPanel.SetWindowText(strong);
	m_ctrlAdded.SetWindowText("  ");
	strong.Format("Total number of points is %d", m_pXFoil->n);
	m_ctrlTotal.SetWindowText(strong);

	SetWindowText(Title);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CCAddDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();
		if (ppp == GetDlgItem(IDC_ANGTOL) )	{
			OnApply();
		}
		else {
			m_ctrlApply.SetFocus();
		}
		return true;

	}

	return CDialog::PreTranslateMessage(pMsg);
}

