#pragma once


// CViewBitmap dialog

class CViewBitmap : public CDialog
{
	DECLARE_DYNAMIC(CViewBitmap)

public:
	CViewBitmap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewBitmap();

// Dialog Data
	enum { IDD = IDD_VIEWBITMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public: 
	CBitmap *m_pBmp;
};
