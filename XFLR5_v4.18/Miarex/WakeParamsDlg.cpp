// WakeParamsDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "WakeParamsDlg.h"
#include ".\wakeparamsdlg.h"


// Boîte de dialogue CWakeParamsDlg

IMPLEMENT_DYNAMIC(CWakeParamsDlg, CDialog)
CWakeParamsDlg::CWakeParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWakeParamsDlg::IDD, pParent)
{
	m_NXWakePanels = 1;
	m_TotalWakeLength  = 100.0;//% mac
	m_WakePanelFactor = 1.1;
}

CWakeParamsDlg::~CWakeParamsDlg()
{
}

void CWakeParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NXWAKEPANELS, m_ctrlNXWakePanels);
	DDX_Control(pDX, IDC_WAKEPANELFACTOR, m_ctrlWakePanelFactor);
	DDX_Control(pDX, IDC_TOTALWAKELENGTH, m_ctrlTotalWakeLength);
	DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CWakeParamsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()


// Gestionnaires de messages CWakeParamsDlg

void CWakeParamsDlg::OnOK()
{
	m_NXWakePanels    = m_ctrlNXWakePanels.GetValue();
	m_TotalWakeLength = m_ctrlTotalWakeLength.GetValue();
	m_WakePanelFactor = m_ctrlWakePanelFactor.GetValue();
	CDialog::OnOK();
}

BOOL CWakeParamsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlNXWakePanels.SetValue(m_NXWakePanels);
	m_ctrlTotalWakeLength.SetValue(m_TotalWakeLength);
	m_ctrlWakePanelFactor.SetValue(m_WakePanelFactor);
	m_ctrlNXWakePanels.SetFocus();
	return FALSE;
}

BOOL CWakeParamsDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN ){ 
		CWnd* pWnd = GetFocus();

		if(pMsg->wParam == VK_RETURN){

			if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd){
				// we don't want to exit on OK on CEdit

				m_ctrlOK.SetFocus();
				return true;
			}
			else if(GetDlgItem(IDOK) == pWnd ) OnOK();
			else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
