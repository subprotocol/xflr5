// ViewBitmap.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ViewBitmap.h"


// CViewBitmap dialog

IMPLEMENT_DYNAMIC(CViewBitmap, CDialog)

CViewBitmap::CViewBitmap(CWnd* pParent /*=NULL*/)
	: CDialog(CViewBitmap::IDD, pParent)
{

}

CViewBitmap::~CViewBitmap()
{
}

void CViewBitmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewBitmap, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CViewBitmap message handlers

BOOL CViewBitmap::OnInitDialog()
{
	CDialog::OnInitDialog();

	return FALSE;  
}


void CViewBitmap::OnPaint() 
{
	// The window has recieved the message to repaint itself

	CPaintDC dc(this); // device context for painting
	CBitmap* pOldBmp;
	CRect rect;
	GetClientRect(&rect);
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	pOldBmp = memdc.SelectObject(m_pBmp);

	dc.BitBlt(0,0, 800, 600, &memdc,0,0,SRCCOPY);
	memdc.SelectObject(pOldBmp);
	// Do not call CDialog::OnPaint() for painting messages
}
