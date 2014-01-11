/****************************************************************************

All Color and style selection classes 
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

// ClrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ClrClasses.h"



/////////////////////////////////////////////////////////////////////////////
// CClrBtn
int CClrBtn::m_iDefined = 0;

COLORREF CClrBtn::m_clrUser[6]={
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255),
	RGB(255,255,255)
};



CClrBtn::CClrBtn(CWnd* pParent){
}

CClrBtn::~CClrBtn(){
}


BEGIN_MESSAGE_MAP(CClrBtn, CButton)
	//{{AFX_MSG_MAP(CClrBtn)
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CClrBtn::OnClicked() {
    // When the button is clicked, show the dialog.
	CClrDlg dlg(this);
	for (int i=0; i<6; i++) dlg.SetUserColor(i,m_clrUser[i]);
	dlg.SetUserDefined(m_iDefined);
	dlg.SetCurrentColor(m_clrCurrent);

	if (dlg.DoModal() == IDOK) {
		m_clrCurrent = dlg.GetCurrentColor();
		for (int j=0; j<6; j++) m_clrUser[j] = dlg.GetUserColor(j);
		m_iDefined = dlg.GetUserDefined();
		Invalidate();
	}	
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CClrBtn message handlers


void CClrBtn::DrawItem(LPDRAWITEMSTRUCT lpd)
{
    // Draw the button
	CDC* pDC = CDC::FromHandle(lpd->hDC);
	
	// Store this for convenience
	int top    = lpd->rcItem.top;
	int left   = lpd->rcItem.left;
	int bottom = lpd->rcItem.bottom;
	int right  = lpd->rcItem.right;
	
	CPen NullPen;
	NullPen.CreateStockObject(NULL_PEN);
	CBrush colorbrush(m_clrCurrent);
	CPen BlackPen(PS_SOLID, 1, RGB(0,0,0));
	CPen WhitePen(PS_SOLID, 1, RGB(255,255,255));
	CPen GrayPen(PS_SOLID,1,RGB(128,128,128));
	CBrush BackBrush(GetSysColor(COLOR_3DFACE));
	
	CPen *pOldPen = pDC->SelectObject(&NullPen);
	CBrush *pOldBrush= pDC->SelectObject(&BackBrush);
	
	
	// Clear the background using the 3DFACE color.
	
	pDC->Rectangle(&lpd->rcItem);
	
	// Draw the border
	
	if (!(lpd->itemState & ODS_SELECTED)){
		// Button is up
		pDC->SelectObject(&BlackPen);
		pDC->MoveTo(left,bottom-1);
		pDC->LineTo(right-1,bottom-1);
		pDC->LineTo(right-1,top);
		
		pDC->SelectObject(&GrayPen);
		pDC->MoveTo(left+1,bottom-2);
		pDC->LineTo(right-2,bottom-2);
		pDC->LineTo(right-2,top+1);
		
		pDC->SelectObject(&WhitePen);
		pDC->LineTo(left+1,top+1);
		pDC->LineTo(left+1,bottom-2);	
	}
	else{
		// Button is down
		
		pDC->SelectObject(&GrayPen);
		pDC->MoveTo(left,bottom-1);
		pDC->LineTo(left,top);
		pDC->LineTo(right-1,top);
		
		pDC->SelectObject(&WhitePen);
		pDC->MoveTo(right-1,top-1);
		pDC->LineTo(right-1,bottom-1);
		pDC->LineTo(left+1,bottom-1);
		
		pDC->SelectObject(&BlackPen);
		pDC->MoveTo(left+1,bottom-2);
		pDC->LineTo(left+1,top+1);
		pDC->LineTo(right-2,top+1);
		
		// by moving this, we get the things inside the button
		// to draw themselves one pixel down and one to the right.
		// This completes the "pushed" effect

		left++;
		right++;
		bottom++;
		top++;		
	}
	
	// The division
	
	pDC->SelectObject(&WhitePen);
	pDC->MoveTo(right-10,top+4);
	pDC->LineTo(right-10,bottom-4);
	
	pDC->SelectObject(&GrayPen);
	pDC->MoveTo(right-11,top+4);
	pDC->LineTo(right-11,bottom-4);
	
	// The triangle

	if (lpd->itemState & ODS_DISABLED)
		pDC->SelectObject(&GrayPen);
	else
		pDC->SelectObject(&BlackPen);
	
	pDC->MoveTo(right-4,(bottom/2)-1);
	pDC->LineTo(right-9,(bottom/2)-1);
	
	pDC->MoveTo(right-5,(bottom/2));
	pDC->LineTo(right-8,(bottom/2));
	
	if (lpd->itemState & ODS_DISABLED) {
		pDC->SetPixel(right-4,(bottom/2)-1,RGB(255,255,255));
		pDC->SetPixel(right-5,(bottom/2),RGB(255,255,255));
		pDC->SetPixel(right-6,(bottom/2)+1,RGB(255,255,255));
	}
	else  {
		pDC->SetPixel(right-6,(bottom/2)+1,RGB(0,0,0));
	}
	
	if (!(lpd->itemState & ODS_DISABLED)) {
		// The color rectangle, only if enabled
		pDC->SelectObject(&colorbrush);
		pDC->Rectangle(left+5,top+4,right-15,bottom-4);    
	}
	
	if (lpd->itemState & ODS_FOCUS) {
		// Draw the focus
		//
		// It would have been nice just to draw a rectangle using a pen created
		// with the PS_ALTERNATE style, but this is not supported by WIN95
		
		int i;
		
		for (i=left+3;i<right-4;i+=2)  {
			pDC->SetPixel(i,top+3,RGB(0,0,0));
			pDC->SetPixel(i,bottom-4,RGB(0,0,0));
		}
		
		for (i=top+3;i<bottom-4;i+=2){
			pDC->SetPixel(left+3,i,RGB(0,0,0));
			pDC->SetPixel(right-4,i,RGB(0,0,0));
		}       
	}	
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}




COLORREF CClrBtn::GetColor(){
	return m_clrCurrent;
}


void CClrBtn::SetColor(COLORREF clr)
{
	m_clrCurrent = clr;
}

/////////////////////////////////////////////////////////////////////////////
// CClrDlg dialog

void CClrDlg::InitColors()
{
// first line
	m_crColors[0] = RGB(0,0,0),
	m_crColors[1] = RGB(64,64,64),
	m_crColors[2] = RGB(110,110,110),
	m_crColors[3] = RGB(160,160,160),
	m_crColors[4] = RGB(210,210,210),
	m_crColors[5] = RGB(255,255,255),

// second line
	m_crColors[6] = RGB(255,0,0),
	m_crColors[7] = RGB(0,255,0),
	m_crColors[8] = RGB(0,0,255),
	m_crColors[9] = RGB(255,255,0),
	m_crColors[10] = RGB(255,0,255),
	m_crColors[11] = RGB(0,255,255),

// third line
	m_crColors[12]  = RGB(200,60,60),
	m_crColors[13]  = RGB(0,160,0),
	m_crColors[14]  = RGB(60,60,180),
	m_crColors[15]  = RGB(170,170,0),
	m_crColors[16]  = RGB(130,0,130),
	m_crColors[17]  = RGB(0,130,130),

// fourth line
	m_crColors[18] = RGB(255,128,128),
	m_crColors[19] = RGB(0,255,128),
	m_crColors[20] = RGB(64,200,255),
	m_crColors[21] = RGB(170,170,100),
	m_crColors[22] = RGB(190,100,190),
	m_crColors[23] = RGB(100,170,170),

// fifth line
	m_crColors[24] = RGB(228,150,70),
	m_crColors[25] = RGB(170,255,170),
	m_crColors[26] = RGB(120,120,255),
	m_crColors[27] = RGB(228,228,128),
	m_crColors[28] = RGB(255,170,255),
	m_crColors[29] = RGB(170,255,255),

//UserColors
	m_crColors[30] = RGB(255,255,255),
	m_crColors[31] = RGB(255,255,255),
	m_crColors[32] = RGB(255,255,255),
	m_crColors[33] = RGB(255,255,255),
	m_crColors[34] = RGB(255,255,255),
	m_crColors[35] = RGB(255,255,255);
}
/////////////////////////////////////////////////////////////////////////////
// CClrDlg dialog





CClrDlg::CClrDlg(CWnd* pParent)
	: CDialog(CClrDlg::IDD, pParent){
	//{{AFX_DATA_INIT(CClrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = (CButton*)pParent;
	InitColors();
} 


void CClrDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClrDlg)
	DDX_Control(pDX, IDC_OTHERCLR, m_ctrlOtherClr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClrDlg, CDialog)
	//{{AFX_MSG_MAP(CClrDlg)
	ON_BN_CLICKED(IDC_OTHERCLR, OnOtherClr)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDC_CLR1,IDC_CLR36,OnColor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrDlg message handlers

void CClrDlg::OnOtherClr() 
{

    // A well has been clicked, set the color index and close.
	int i;

	// The "Other" button.
	ReleaseCapture();
	CColorDialog dlg(m_clrCurrent);
	dlg.m_cc.Flags |= CC_FULLOPEN;
	for (int k=0; k<6; k++){
		dlg.m_cc.lpCustColors[k]=m_crColors[30+k];
	}
	if (dlg.DoModal() == IDOK){
		// The user clicked OK.
		// set the color and close
		m_clrCurrent = dlg.GetColor();

		// Check to see if the selected color is already in the table.
		m_iSelected = -1;
		for (i=0;i<36;i++){
			if (m_crColors[i] == m_clrCurrent){
				m_iSelected = i;
				break;
			}
		}
		
		// If the color was not found, add to the user defined colors
		if (m_iSelected == -1){
			m_iSelected = 30+m_iDefined;
			m_crColors[m_iSelected] = m_clrCurrent;
			GetDlgItem(IDC_CLR1+m_iSelected)->Invalidate();
			m_iDefined = (m_iDefined+1)%6;
		}
		EndDialog(IDOK);
	}
	
	// If the user clicked "Cancel" reclaim the mouse capture.
	else {
		SetCapture();
	}
}

void CClrDlg::OnLButtonDown(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
	CRect r;
	CPoint p;
	p.x = point.x;
	p.y = point.y;
	
	ClientToScreen(&p);
	GetWindowRect(&r);
	
	// The user clicked...
	if (!r.PtInRect(p)){
		//  ...outside the dialog, close.
		EndDialog(IDCANCEL);
	}
	else {
		//  ...inside the dialog. Since this window
		//     has the mouse captured, its children
		//     get no messages. So, check to see
		//     if the click was in one of its children
		//     and tell him.
		
		//     If the user clicks inside the dialog
		//     but not on any of the controls,
		//     ChildWindowFromPoint returns a
		//     pointer to the dialog. In this
		//     case we do not resend the message
		//     (obviously) because it would cause
		//     a stack overflow.
		
		CWnd *child = ChildWindowFromPoint(point);
		if (child && child != this){
			child->SendMessage(WM_LBUTTONDOWN);
			for (int i=0; i<36; i++){
				if(child == GetDlgItem(i + IDC_CLR1)){
					m_iSelected = i;
					m_clrCurrent = m_crColors[i];
					break;
				}
			}
		}
	}

//	CDialog::OnLButtonDown(nFlags, point);
}

void CClrDlg::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: Add your message handler code here and/or call default
    // See notes for OnLButtonDown.
    
    CWnd *child = ChildWindowFromPoint(point,CWP_ALL);
    if (child && child != this)
        child->SendMessage(WM_LBUTTONDOWN,0,0l);	
	
	CDialog::OnLButtonUp(nFlags, point);
}


void CClrDlg::OnColor(UINT id){
	m_iSelected = id-IDC_CLR1;
	m_clrCurrent = m_crColors[m_iSelected];
	EndDialog(IDOK);
}


void CClrDlg::EndDialog(int nResult){
    ReleaseCapture();
    CDialog::EndDialog(nResult);
}


void CClrDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

    // Draw the wells using the current color table
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if(m_iSelected == nIDCtl-IDC_CLR1){
		CPen pen(PS_SOLID, 1, RGB(160,160,160));
		CBrush brush(m_crColors[nIDCtl-IDC_CLR1]);

		CPen *oldpen;
		CBrush *oldbrush;
		oldpen = pDC->SelectObject(&pen);
		oldbrush = pDC->SelectObject(&brush);

		lpDrawItemStruct->rcItem.right;
		lpDrawItemStruct->rcItem.bottom;

		pDC->Rectangle(&lpDrawItemStruct->rcItem);

		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

	else{
		CPen curpen(PS_SOLID, 1, RGB(0,0,0));
		CBrush brush;
		CPen *oldpen;
		CBrush *oldbrush;
		brush.CreateSolidBrush(m_crColors[nIDCtl-IDC_CLR1]);
		oldpen = pDC->SelectObject(&curpen);
		oldbrush = pDC->SelectObject(&brush);

		lpDrawItemStruct->rcItem.right;
		lpDrawItemStruct->rcItem.bottom;

		pDC->Rectangle(&lpDrawItemStruct->rcItem);

		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		brush.DeleteObject();
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

BOOL CClrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
    CRect r,r2;
	m_pParent->GetWindowRect(&r);
    
    // Move the dialog to be below the button
    SetWindowPos(NULL,r.left,r.bottom,0,0,SWP_NOSIZE|SWP_NOZORDER);
    GetWindowRect(&r2);

    // Check to see if the dialog has a portion outside the
    // screen, if so, adjust.
    
    if (r2.bottom >= GetSystemMetrics(SM_CYSCREEN))  {   
        r2.top = r.top-(r2.bottom-r2.top);
    }

    if (r2.right >= GetSystemMetrics(SM_CXSCREEN))  {
        r2.left = GetSystemMetrics(SM_CXSCREEN) - (r2.right-r2.left);
    }

	CButton* pWnd;

	for (int i=IDC_CLR1; i<=IDC_CLR36; i++){
		pWnd = (CButton*)GetDlgItem(i);
		pWnd->SetButtonStyle(BS_OWNERDRAW);
	}

    SetWindowPos(NULL,r2.left,r2.top,0,0,SWP_NOSIZE|SWP_NOZORDER);

    // Capture the mouse, this allows the dialog to close when
    // the user clicks outside.
    // Remember that the dialog has no "close" button.
	GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
	SetCapture();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}



BOOL CClrDlg::PreTranslateMessage(MSG* pMsg) {
	// TODO: Add your specialized code here and/or call the base class
	int oldsel = m_iSelected;

	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_DOWN){ 
			m_iSelected +=6;
			if(m_iSelected>=36) m_iSelected -= 36;
			m_clrCurrent = m_crColors[m_iSelected];
			CWnd *old = GetDlgItem(oldsel+ IDC_CLR1);
			old->Invalidate(true);
			GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
			GetDlgItem(m_iSelected + IDC_CLR1)->Invalidate(true);
			return true; // no need to process further
		} 

		else if(pMsg->wParam == VK_UP){ 
			m_iSelected -=6;
			if(m_iSelected<0) m_iSelected += 36;
			m_clrCurrent = m_crColors[m_iSelected];
			CWnd *old = GetDlgItem(oldsel+ IDC_CLR1);
			old->Invalidate(true);
			GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
			GetDlgItem(m_iSelected + IDC_CLR1)->Invalidate(true);
			return true; // no need to process further
		} 

		else if(pMsg->wParam == VK_LEFT){ 
			m_iSelected--;
			if(m_iSelected<0) m_iSelected += 36;
			m_clrCurrent = m_crColors[m_iSelected];
			CWnd *old = GetDlgItem(oldsel+ IDC_CLR1);
			old->Invalidate(true);
			GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
			GetDlgItem(m_iSelected + IDC_CLR1)->Invalidate(true);
			return true; // no need to process further
		} 

		else if(pMsg->wParam == VK_RIGHT){ 
			m_iSelected ++;
			if(m_iSelected>=36) m_iSelected -= 36;
			m_clrCurrent = m_crColors[m_iSelected];
			CWnd *old = GetDlgItem(oldsel+ IDC_CLR1);
			old->Invalidate(true);
			GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
			GetDlgItem(m_iSelected + IDC_CLR1)->Invalidate(true);
			return true; // no need to process further
		} 

		else if(pMsg->wParam == VK_TAB){ 
			bool found = false;
			for (int i=0; i<36; i++){
				if(GetFocus() == GetDlgItem(i + IDC_CLR1)){
					m_ctrlOtherClr.SetFocus();
					CDC *pDC = m_ctrlOtherClr.GetDC();
					CRect r;
					m_ctrlOtherClr.GetClientRect(&r);
					r.DeflateRect(2,2);
					pDC->DrawFocusRect(&r);
					m_ctrlOtherClr.ReleaseDC(pDC);
					found = true;
					break;
				}
			} 
			if (!found){
				GetDlgItem(m_iSelected + IDC_CLR1)->SetFocus();
			}
			return true; // no need to process further
		} 

		else if(pMsg->wParam == VK_RETURN){ 
			if (GetFocus() == GetDlgItem(IDC_OTHERCLR)) {
				OnOtherClr();
				return true;
			}
			else{
				CWnd *child = GetDlgItem(m_iSelected + IDC_CLR1);
				if (child && child != this)
					child->SendMessage(WM_LBUTTONDOWN,0,0l);
	//		do not return true : user has selected a color and wants to exit...
			}
		} 
	}
	return CDialog::PreTranslateMessage(pMsg);
}


COLORREF CClrDlg::GetCurrentColor(){
	return m_clrCurrent;
}
 

COLORREF CClrDlg::GetUserColor(int i){
	if(i>=0 && i<6) return m_crColors[i+30];
	else return RGB(255,255,255);      //!!!
}


void CClrDlg::SetCurrentColor(COLORREF clr){
	m_clrCurrent = clr;
	m_iSelected  = 0;
	for (int i=0; i<36; i++){
		if(m_crColors[i] == m_clrCurrent){
			m_iSelected = i;
			break;
		}
	}
}


void CClrDlg::SetUserColor(int i, COLORREF clr){
	m_crColors[i+30] = clr;
}


void CClrDlg::SetUserDefined(int i){
	m_iDefined = i;
}


int CClrDlg::GetUserDefined(){
	return m_iDefined;
}

BOOL CClrBtn::PreTranslateMessage(MSG* pMsg) {
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN){ 
		OnClicked();
		return true; // no need to process further
		
	} 

	return CButton::PreTranslateMessage(pMsg);
}
