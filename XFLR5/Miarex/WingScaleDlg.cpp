/****************************************************************************

    CWingScaleDlg Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// WingScaleDlg.cpp : implementation file
// The class associated to the dialog box for the definition of scaling factors
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "WingScaleDlg.h"
#include "Miarex.h"


/////////////////////////////////////////////////////////////////////////////
// CWingScaleDlg dialog


CWingScaleDlg::CWingScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWingScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWingScaleDlg)
	//}}AFX_DATA_INIT
	m_pMiarex       = pParent;
	m_SpanFactor    = 1.0;
	m_ChordFactor   = 1.0;
	m_InitialSpan   = 1.0;
	m_NewSpan       = 1.0;
	m_InitialChord  = 0.10;
	m_NewChord      = 0.10;
	m_NewSweep      = 0.0;
	m_Twist         = 0.0;
	m_mtoUnit      = 1.0;
	m_bScaleSpan  = false;
	m_bScaleChord = false;
	m_bScaleSweep = false;
	m_bScaleTwist = false;
}


void CWingScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWingScaleDlg)
	DDX_Control(pDX, IDC_SPANSCALE, m_ctrlSpanScale);
	DDX_Control(pDX, IDC_CHORDSCALE, m_ctrlChordScale);
	DDX_Control(pDX, IDC_NEWCHORD, m_ctrlNewChord);
	DDX_Control(pDX, IDC_LUNIT2, m_ctrlLUnit2);
	DDX_Control(pDX, IDC_CHORDFACTOR, m_ctrlChordFactor);
	DDX_Control(pDX, IDC_SPANFACTOR, m_ctrlSpanFactor);
	DDX_Control(pDX, IDC_TWISTSCALE, m_ctrlTwistScale);
	DDX_Control(pDX, IDC_SWEEPSCALE, m_ctrlSweepScale);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_LUNIT, m_ctrlLUnit);
	DDX_Control(pDX, IDC_SWEEP, m_ctrlSweep);
	DDX_Control(pDX, IDC_NEWSPAN, m_ctrlNewSpan);
	DDX_Control(pDX, IDC_TWIST, m_ctrlTwist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWingScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CWingScaleDlg)
	ON_BN_CLICKED(IDC_SPANSCALE, OnSpanScale)
	ON_BN_CLICKED(IDC_CHORDSCALE, OnChordScale)
	ON_BN_CLICKED(IDC_SWEEPSCALE, OnSweepScale)
	ON_BN_CLICKED(IDC_TWISTSCALE, OnTwistScale)
	ON_EN_KILLFOCUS(IDC_NEWSPAN, OnKillFocusNewSpan)
	ON_EN_KILLFOCUS(IDC_SWEEP, OnKillFocusSweep)
	ON_EN_KILLFOCUS(IDC_TWIST, OnKillFocusTwist)
	ON_EN_KILLFOCUS(IDC_NEWCHORD, OnKillFocusNewChord)
	ON_EN_KILLFOCUS(IDC_SPANFACTOR, OnKillFocusSpanFactor)
	ON_EN_KILLFOCUS(IDC_CHORDFACTOR, OnKillFocusChordFactor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWingScaleDlg message handlers

BOOL CWingScaleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;

	CMainFrame*pFrame = (CMainFrame*)m_pFrame;
	GetLengthUnit(str, pFrame->m_LengthUnit);
	m_ctrlLUnit.SetWindowText(str);
	m_ctrlLUnit2.SetWindowText(str);
	m_mtoUnit = pFrame->m_mtoUnit;

	m_ctrlNewSpan.SetPrecision(3);
	m_ctrlNewSpan.SetMin(0.0000001);
	m_ctrlNewSpan.SetMax(100000000.0);
	m_ctrlNewSpan.SetValue(m_InitialSpan*m_mtoUnit);

	m_ctrlSpanFactor.SetPrecision(4);
	m_ctrlSpanFactor.SetMin(0.0000001);
	m_ctrlSpanFactor.SetMax(100000000.0);
	m_ctrlSpanFactor.SetValue(m_SpanFactor);

	m_ctrlNewChord.SetPrecision(3);
	m_ctrlNewChord.SetMin(0.0000001);
	m_ctrlNewChord.SetMax(100000000.0);
	m_ctrlNewChord.SetValue(m_InitialChord*m_mtoUnit);

	m_ctrlChordFactor.SetPrecision(4);
	m_ctrlChordFactor.SetMin(0.0000001);
	m_ctrlChordFactor.SetMax(100000000.0);
	m_ctrlChordFactor.SetValue(m_ChordFactor);

	m_ctrlSweep.SetValue(m_InitialSweep);
	m_ctrlTwist.SetValue(m_Twist);

	m_ctrlNewSpan.EnableWindow(false);
	m_ctrlSpanFactor.EnableWindow(false);
	m_ctrlNewChord.EnableWindow(false);
	m_ctrlChordFactor.EnableWindow(false);
	m_ctrlSweep.EnableWindow(false);
	m_ctrlTwist.EnableWindow(false);


	m_ctrlSpanScale.SetFocus();

	return FALSE;
}


void CWingScaleDlg::OnOK() 
{
	ReadParams();
	CDialog::OnOK();
}


BOOL CWingScaleDlg::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWnd = GetFocus();
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd) {
			ReadParams();
			m_ctrlOK.SetFocus();
		}
		else if(GetDlgItem(IDOK) == pWnd ) OnOK();
		else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();

		return true;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CWingScaleDlg::ReadParams()
{
	m_NewSweep    = m_ctrlSweep.GetValue();	
//	m_NewSpan     = m_ctrlNewSpan.GetValue()/m_mtoUnit;
	m_SpanFactor  = m_ctrlSpanFactor.GetValue();
//	m_NewChord     = m_ctrlNewChord.GetValue()/m_mtoUnit;
	m_ChordFactor  = m_ctrlChordFactor.GetValue();
	m_Twist       = m_ctrlTwist.GetValue();
	if(m_ctrlSpanScale.GetCheck())  m_bScaleSpan = true;
	if(m_ctrlChordScale.GetCheck()) m_bScaleChord = true;
	if(m_ctrlSweepScale.GetCheck()) m_bScaleSweep = true;
	if(m_ctrlTwistScale.GetCheck()) m_bScaleTwist = true;
}



void CWingScaleDlg::OnSpanScale() 
{
	if(m_ctrlSpanScale.GetCheck()){
		m_ctrlNewSpan.EnableWindow(true);
		m_ctrlSpanFactor.EnableWindow(true);
	}
	else{
		m_ctrlNewSpan.EnableWindow(false);
		m_ctrlSpanFactor.EnableWindow(false);
	}
}

void CWingScaleDlg::OnChordScale() 
{
	if(m_ctrlChordScale.GetCheck()){
		m_ctrlNewChord.EnableWindow(true);
		m_ctrlChordFactor.EnableWindow(true);
	}
	else{
		m_ctrlNewChord.EnableWindow(false);
		m_ctrlChordFactor.EnableWindow(false);
	}	
}

void CWingScaleDlg::OnSweepScale() 
{
	if(m_ctrlSweepScale.GetCheck()){
		m_ctrlSweep.EnableWindow(true);
	}
	else{
		m_ctrlSweep.EnableWindow(false);
	}		
}

void CWingScaleDlg::OnTwistScale() 
{
	if(m_ctrlTwistScale.GetCheck()){
		m_ctrlTwist.EnableWindow(true);
	}
	else{
		m_ctrlTwist.EnableWindow(false);
	}		
}


void CWingScaleDlg::OnKillFocusSweep() 
{
	m_NewSweep = m_ctrlSweep.GetValue();
}
void CWingScaleDlg::OnKillFocusTwist() 
{
	m_Twist = m_ctrlTwist.GetValue();
}

void CWingScaleDlg::OnKillFocusNewSpan() 
{
	m_NewSpan = m_ctrlNewSpan.GetValue()/m_mtoUnit;
	m_SpanFactor = m_NewSpan/m_InitialSpan;
	m_ctrlSpanFactor.SetValue(m_SpanFactor);
}


void CWingScaleDlg::OnKillFocusNewChord() 
{
	m_NewChord = m_ctrlNewChord.GetValue()/m_mtoUnit;
	m_ChordFactor = m_NewChord/m_InitialChord;
	m_ctrlChordFactor.SetValue(m_ChordFactor);
}

void CWingScaleDlg::OnKillFocusSpanFactor() 
{
	m_SpanFactor = m_ctrlSpanFactor.GetValue();
	m_NewSpan = m_InitialSpan * m_SpanFactor;
	m_ctrlNewSpan.SetValue(m_NewSpan*m_mtoUnit);
}

void CWingScaleDlg::OnKillFocusChordFactor() 
{
	m_ChordFactor = m_ctrlChordFactor.GetValue();
	m_NewChord = m_InitialChord * m_ChordFactor;
	m_ctrlNewChord.SetValue(m_NewChord*m_mtoUnit);
}
