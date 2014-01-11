#pragma once

#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
// Boîte de dialogue CWakeParamsDlg

class CWakeParamsDlg : public CDialog
{
	DECLARE_DYNAMIC(CWakeParamsDlg)

public:
	CWakeParamsDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CWakeParamsDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_WAKEPARAMSDLG };

	CButton m_ctrlOK;
	CNumEdit   m_ctrlNXWakePanels;
	CFloatEdit m_ctrlTotalWakeLength;
	CFloatEdit m_ctrlWakePanelFactor;

	int m_NXWakePanels;
	double m_TotalWakeLength;//% mac
	double m_WakePanelFactor;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
