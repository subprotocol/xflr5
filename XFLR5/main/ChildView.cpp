/****************************************************************************

    CChildView Class
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

// ChildView.cpp : implementation of the CChildView class
// The XFLR5 project uses the MainFrame/View architecture offered by VC++
// All drawing operations are redirected from ChildView to the currently selected
// application, i.e. AFoil, XInverse, XFLR5, or Miarex
// This class also redispatches the mouse messages and the other view related messages
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "../misc/PrefsDlg.h"
#include ".\childview.h"
#include "atlimage.h"

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	CFont TempFont;
	TempFont.CreatePointFont(85, "Courier New");
	TempFont.GetLogFont(&m_WndLogFont);

	m_WndTextColor  = RGB(255,255,255);
	m_crBackColor   = RGB(0, 20, 40);
	m_crGraphBkClr  = RGB(0, 30, 50);
	m_crBorderClr   = RGB(200,200,200);
	m_iBorderStyle  = 0;
	m_iBorderWidth  = 1;
	m_bBorder       = true;
	m_dz            = 28;
	m_LetterWidth   = 7;
	m_ImageFileType = 4;

	m_rCltRect.SetRect(0,0, 100,100);

	glDeleteLists(GLF_START_LIST, 256);
}


CChildView::~CChildView()
{
	wglDeleteContext(m_hRC);
	if(m_hPalette){
		DeleteObject(m_hPalette);
	}
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_COMMAND(IDM_CLRSETTINGS, OnClrSettings)
	ON_COMMAND(IDM_SAVEIMAGE, OnSaveImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	//Trace("CChildView::PreCreate - Initializing");
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
    cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;//for OpenGL
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	//Trace("CChildView::PreCreate - Done");
	return TRUE;
}


void CChildView::OnPaint() 
{
	//Trace("CChildView::Painting");
	CPaintDC dc(this); // device context for painting
	
	CMainFrame * pMainFrame  = (CMainFrame*)m_pFrameWnd; 

	int LPY = dc.GetDeviceCaps(LOGPIXELSY);
	m_pXDirect->SetLogPixels(LPY);
	m_pXInverse->SetLogPixels(LPY);

	switch (pMainFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->UpdateView(&dc);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->UpdateView(&dc);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->UpdateView(&dc);
			break;
		}
		case MIAREX:{
			m_pMiarex->UpdateView(&dc);
			break;
		}
		default:{//blank screen
			CDC memdc; 
			memdc.CreateCompatibleDC (&dc);

			CBitmap  bmp;
			bmp.CreateCompatibleBitmap(&dc, m_rCltRect.Width(), m_rCltRect.Height());
			CBitmap * pOldBmp = memdc.SelectObject(&bmp);

			memdc.SetBkColor(m_crBackColor);
			memdc.FillSolidRect(&m_rCltRect,m_crBackColor);

			// Blit back
			dc.BitBlt(m_rCltRect.left,    m_rCltRect.top,
						m_rCltRect.Width(), m_rCltRect.Height(),
						&memdc,0,0,SRCCOPY);
			memdc.SelectObject(pOldBmp);
			memdc.DeleteDC();
			bmp.DeleteObject();
		}
	}
	// Do not call CWnd::OnPaint() for painting messages
}


void CChildView::OnSaveImage() 
{
	//Trace("CChildView::Painting");
	CPaintDC dc(this); // device context for painting
	
//	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
//	int LPY = dc.GetDeviceCaps(LOGPIXELSY);
//	m_pXDirect->SetLogPixels(LPY);
//	m_pXInverse->SetLogPixels(LPY);

	CMainFrame * pMainFrame  = (CMainFrame*)m_pFrameWnd; 
	CImage Image;

	CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;

	strFilter = "Bitmap image|*.bmp|JPEG image|*.jpg|GIF image|*.gif|PNG image|*.png||";

	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,strFilter);
	dlg.m_ofn.nFilterIndex = m_ImageFileType;
	hResult = (int)dlg.DoModal();

	if (hResult != IDOK) 
	{
		return;
	}

    // Add the appropriate extension if the user didn't type one

	CString strFileName;
	CString strExtension;


	strFileName = dlg.m_ofn.lpstrFile;
	strExtension = strFileName.Right(4);
	strExtension.MakeLower();
	if(strExtension == ".bmp" || strExtension == ".jpg" || strExtension == ".gif" || strExtension == ".png")
		strFileName = strFileName.Left(strFileName.GetLength()-4);

	m_ImageFileType = dlg.m_ofn.nFilterIndex;

	switch (m_ImageFileType)
	{
		case 1:
			strExtension = "bmp";
			break;
		case 2:
			strExtension = "jpg";
			break;
		case 3:
			strExtension = "gif";
			break;
		case 4:
			strExtension = "png";
			break;
		default:
			break;
	}

	strFileName = strFileName + '.' + strExtension;

	switch (pMainFrame->m_iApp)
	{
		case XFOILANALYSIS:
		{
			m_pXDirect->PaintImage(&Image, strFileName,m_ImageFileType);
			break;
		}
		case DIRECTDESIGN:
		{
			m_pAFoil->PaintImage(&Image, strFileName,m_ImageFileType);
			break;
		}
		case INVERSEDESIGN:
		{
			m_pXInverse->PaintImage(&Image, strFileName,m_ImageFileType);
			break;
		}
		case MIAREX:
		{
			m_pMiarex->m_bArcball = false;
			m_pMiarex->m_bCrossPoint = false;
			m_pMiarex->UpdateView();
			m_pMiarex->PaintImage(&Image, strFileName,m_ImageFileType);
			break;
		}
		default:
		{//blank screen
			return;
		}
	}
/*	// the extension on the file name will determine the file type that is saved
	hResult = Image.Save(strFileName);
	if (FAILED(hResult)) 
	{
		CString fmt;
		fmt.Format("Save image failed:\n%x ", hResult);
		AfxMessageBox(fmt);
		return;
	}*/
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);

	GetClientRect(&m_rCltRect);
	if(m_rCltRect.IsRectEmpty()) return;

	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->SetFoilScale(m_rCltRect);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->SetScale(m_rCltRect);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->SetRect(m_rCltRect);
			break;
		}
		case MIAREX:
		{
			m_pMiarex->m_bResetglBody2D = true;
			m_pMiarex->SetScale(m_rCltRect);
			m_pMiarex->GLSetViewport();
			break;
		}
	}
}


LRESULT CChildView::OnNcHitTest(CPoint point) 
{
	CPoint ptClient(point);

	ScreenToClient(&ptClient);
	ptClient.y -= m_dz;
	if(m_rCltRect.PtInRect(ptClient))
	{
		return (HTCLIENT);
	}
	else 
	{
		return (CWnd::OnNcHitTest(point));
	}

//	return CWnd ::OnNcHitTest(point);
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


void CChildView::SaveSettings(CArchive &ar)
{
	ar << m_crBackColor;
	ar << m_WndTextColor;
	ar << m_crGraphBkClr;
	ar << m_crBorderClr;
	ar << m_iBorderStyle;
	ar << m_iBorderWidth;
	if (m_bBorder) ar << 1;
	else ar << 0;
	if (IsBlackAndWhite()) ar << 1;
	else                   ar << 0;

	if(m_WndLogFont.lfCharSet == 0) m_WndLogFont.lfCharSet = DEFAULT_CHARSET;
	ar << m_WndLogFont.lfCharSet;
	ar << m_WndLogFont.lfClipPrecision;
	ar << m_WndLogFont.lfEscapement;
	ar << m_WndLogFont.lfHeight;
	ar << m_WndLogFont.lfItalic;
	ar << m_WndLogFont.lfOrientation;
	ar << m_WndLogFont.lfOutPrecision;
	ar << m_WndLogFont.lfPitchAndFamily;
	ar << m_WndLogFont.lfQuality;
	ar << m_WndLogFont.lfStrikeOut;
	ar << m_WndLogFont.lfUnderline;
	ar << m_WndLogFont.lfWeight;
	ar << m_WndLogFont.lfWidth;
	CString strong;
	strong = m_WndLogFont.lfFaceName;//let MFC do the conversion...
	ar << strong.GetLength();
	ar.WriteString(strong);
	m_DefaultGraph.Serialize(ar);
}


bool CChildView::LoadSettings(CArchive &ar)
{
	int k;
	try{
		ar >> k;
		if (k<0 || k>RGB(255,255,255)) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_crBackColor = k;
		ar >> k;
		if (k<0 || k>RGB(255,255,255)) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_WndTextColor = k;
		ar >> k;
		if (k<0 || k>RGB(255,255,255)) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_crGraphBkClr = k;

		ar >> k;
		if (k<0 || k>RGB(255,255,255)) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_crBorderClr = k;
		ar >> k;
		if (k<0 || k>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iBorderStyle = k;
		ar >> k;
		if (k<0 || k>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iBorderWidth = k;
		ar >> k;
		if (k!=1 && k!=0) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bBorder = true;
		else m_bBorder = false;

		ar >> k;
		if (k!=1 && k!=0) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) SetBlackAndWhite(true);
		else SetBlackAndWhite(false);

		ar >> m_WndLogFont.lfCharSet;
		if(m_WndLogFont.lfCharSet == 0) m_WndLogFont.lfCharSet = DEFAULT_CHARSET;
		ar >> m_WndLogFont.lfClipPrecision;
		ar >> m_WndLogFont.lfEscapement;
		ar >> m_WndLogFont.lfHeight;
		ar >> m_WndLogFont.lfItalic;
		ar >> m_WndLogFont.lfOrientation;
		ar >> m_WndLogFont.lfOutPrecision;
		ar >> m_WndLogFont.lfPitchAndFamily;
		ar >> m_WndLogFont.lfQuality;
		ar >> m_WndLogFont.lfStrikeOut;
		ar >> m_WndLogFont.lfUnderline;
		ar >> m_WndLogFont.lfWeight;
		ar >> m_WndLogFont.lfWidth;

		ar >> k;
		if(k<0 || k>32) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		ar.ReadString(m_WndLogFont.lfFaceName, k);

		m_DefaultGraph.Serialize(ar);


//-------------	

	}
	catch (CArchiveException *ex){
//		AfxMessageBox("Failed to load settings",MB_OK);
		ex->Delete();
		m_pXDirect->SetDisplayDefault();
		return false;
	}
	catch (CException *ex){
//		AfxMessageBox("Failed to load settings",MB_OK);
		ex->Delete();
		return false;
	}
	return true;
}


void CChildView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnMButtonDown(nFlags, point);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnMButtonDown(nFlags, point);
			break;
		}
		case INVERSEDESIGN:{
			break;
		}
		case MIAREX:{
			break;
		}
	}	
	
	CWnd ::OnMButtonDown(nFlags, point);
}



void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{

	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnLButtonDown(nFlags, point);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnLButtonDown(nFlags, point);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnLButtonDown(nFlags, point);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnLButtonDown(nFlags, point);
			break;
		}
	}	
}


void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnLButtonDblClk(nFlags, point);
			break;
		}
//		case DIRECTDESIGN:{
//			m_pAFoil->OnLButtonUp(nFlags, point);
//			break;
///		}
		case INVERSEDESIGN:{
			m_pXInverse->OnLButtonDblClk(nFlags, point);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnLButtonDblClk(nFlags, point);
			break;
		}
	}	

//	CWnd ::OnLButtonDblClk(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnLButtonUp(nFlags, point);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnLButtonUp(nFlags, point);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnLButtonUp(nFlags, point);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnLButtonUp(nFlags, point);
			break;
		}
	}	

//	CWnd ::OnLButtonDown(nFlags, point);
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnMouseMove(nFlags,  point);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnMouseMove(nFlags,  point);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnMouseMove(nFlags,  point);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnMouseMove(nFlags,  point);
			break;
		}
	}	
	
//	CWnd ::OnMouseMove(nFlags, point);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(GetFocus() != this) return FALSE;
	ScreenToClient(&pt);
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnMouseWheel(nFlags, zDelta, pt);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnMouseWheel(nFlags, zDelta, pt);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnMouseWheel(nFlags, zDelta, pt);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnMouseWheel(nFlags, zDelta, pt);
			break;
		}
	}	

//	return CWnd ::OnMouseWheel(nFlags, zDelta, pt);
	return FALSE;
}


void CChildView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//point is in screen coordinates...
	// so convert to client and send to app
	CPoint CltPt = point;
	ScreenToClient(&CltPt);

	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->OnContextMenu(point, CltPt);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->OnContextMenu(point, CltPt);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnContextMenu(point, CltPt);
			break;
		}
		case MIAREX:{
			m_pMiarex->OnContextMenu(point, CltPt);
			break;
		}
	}
}


void CChildView::SetScale()
{
	if(!IsWindowVisible()) return;
	GetClientRect(&m_rCltRect);
	if(m_rCltRect.IsRectEmpty()) return;

	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->SetFoilScale(m_rCltRect);
			break;
		}
		case DIRECTDESIGN:{
			m_pAFoil->SetScale(m_rCltRect);
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->SetRect(m_rCltRect);
			break;
		}
		case MIAREX:{
			m_pMiarex->SetScale(m_rCltRect);
			break;
		}
	}
}


BOOL CChildView::PreTranslateMessage(MSG* pMsg) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	if (pMsg->message == WM_KEYUP && pFrame->m_iApp==MIAREX)
	{
		if (pMsg->wParam == VK_CONTROL)
		{
			if(m_pMiarex->m_iView==3 || m_pMiarex->m_iView==5)
			{
				m_pMiarex->m_bArcball = false;
				Invalidate();
			}
		}
		return true;
	}
	else return CWnd ::PreTranslateMessage(pMsg);
}


void CChildView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	//Trace("CChildView::Showing Window - status=%d",bShow);
	CWnd ::OnShowWindow(bShow, nStatus);
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 

	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			break;
		}
		case DIRECTDESIGN:{
			break;
		}
		case INVERSEDESIGN:{
			m_pXInverse->OnShowWindow(bShow, nStatus);
			break;
		}
		case MIAREX:
		{
			break;
		}
	}		
}


BOOL CChildView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(&ptMouse);

	switch (pFrame->m_iApp){
		case XFOILANALYSIS:{
			m_pXDirect->SetXFLR5Cursor(pWnd, ptMouse, message);
			return TRUE;
		}
		case DIRECTDESIGN:{
			m_pAFoil->SetAFoilCursor(pWnd, ptMouse, message);
			return TRUE;
		}
		case INVERSEDESIGN:{
			m_pXInverse->SetXInverseCursor(pWnd, ptMouse, message);
			return TRUE;
		}
		case MIAREX:{
			m_pMiarex->SetMiarexCursor(pWnd, ptMouse, message);
			return TRUE;
		}
	}	
	return CWnd ::OnSetCursor(pWnd, nHitTest, message);
}


int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//Trace("CChildView::OnCreate - Initializing OpenGL");
	if (CWnd::OnCreate(lpCreateStruct) == -1){
		//Trace("CChildView::OnCreate - Failed");
		return -1;
	}
	
	// Initialize OpenGL parameters
	PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // Structure size
        1,                             // Version number
        PFD_DRAW_TO_WINDOW |           // Property flags
            PFD_SUPPORT_OPENGL |
            PFD_DOUBLEBUFFER,          // (remove if no double buf)
        PFD_TYPE_RGBA,                 // PixelType
        24,                            // 24-bit color
        0, 0, 0, 0, 0, 0,              // Color bits and shift
        0, 0, 0, 0, 0, 0, 0,           // Alpha and accum buffer bits
        32,                            // 32-bit depth buffer
        0, 0,                          // No stencil or aux buffer
        PFD_MAIN_PLANE,                // Layer type
        0,                             // Reserved
        0, 0, 0                        // Unsupported
    };

	// Tell GDI to convert device context from Win32 to OpenGL.
    CClientDC dcClient(this);
    int iPixelFormat = ChoosePixelFormat(dcClient.m_hDC,&pfd);
    if ( !iPixelFormat ) {
		// This system cannot run OpenGL
		//Trace("CChildView::OnCreate - Error retrieving pixel format index...");
		ASSERT(FALSE);
		AfxMessageBox("Cannot initialize OpenGL...quitting.",
						MB_OK | MB_ICONERROR);
		return -1; // will fail new document creation...
	} // if
	//Trace("CChildView::OnCreate - OpenGL - Chosen Pixel Format");

    if ( !SetPixelFormat(dcClient.m_hDC,iPixelFormat,&pfd) ) {
		// This system cannot run OpenGL
		//Trace("CChildView::OnCreate - Error setting new pixel format...");
		ASSERT(FALSE);
		AfxMessageBox("Cannot initialize OpenGL...quitting.",
						MB_OK | MB_ICONERROR);
		return -1; // will fail new document creation...
	} // if
	//Trace("CChildView::OnCreate - OpenGL - Pixel Format is set");

	// Update the PIXELFORMATDESCRIPTOR structure once
	// the device context has been modified.
    DescribePixelFormat(dcClient.m_hDC,iPixelFormat,
						sizeof(pfd),&pfd);

	// The PIXELFORMATDESCRIPTOR has been updated, so we now
	// determine whether to create and manage a custom
	// palette.
    if ( pfd.dwFlags & PFD_NEED_PALETTE ) {
		// We do, so build a new palette...
		//Trace("CChildView::OnCreate - Setting up palette...");
        GLSetupPalette();
		//Trace("CChildView::OnCreate - Palette is set...");
	} // if
	
	// Create the OpenGL rendering context
    m_hRC = wglCreateContext(dcClient.m_hDC);
    if ( m_hRC == NULL ) {
		// This system cannot run OpenGL
		//Trace("CChildView::OnCreate - Error creating OpenGL rendering context...\n");
		ASSERT(FALSE);
		AfxMessageBox("Cannot initialize OpenGL...quitting.",
						MB_OK | MB_ICONERROR);
		return -1; // will fail new document creation...
	} // if
	//Trace("CChildView::OnCreate - OpenGL - created rendering context");

	// We now make it the current rendering context so
	// we might set our clear color
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);

	wglMakeCurrent(dcClient.m_hDC,m_hRC);
	GLLoadFont();
	glClearColor(0.0,0.0,0.0,0.0); // black
	wglMakeCurrent(dcClient.m_hDC,NULL);


	//Trace("CChildView::OnCreate - Done");

	return 0;
}


void CChildView::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CWnd::OnPaletteChanged(pFocusWnd);
	
	// Manage our palette
	if ( m_hPalette ) {
		CDC* pDC = GetDC();
	    SelectPalette(pDC->m_hDC,m_hPalette,FALSE);
	    RealizePalette(pDC->m_hDC);
		ReleaseDC(pDC);

		// Redraw
		Invalidate();
	} // if
}


BOOL CChildView::OnQueryNewPalette() 
{
	// Manage our palette
	if ( m_hPalette ) {
		CDC* pDC = GetDC();
	    SelectPalette(pDC->m_hDC,m_hPalette,FALSE);
	    RealizePalette(pDC->m_hDC);
		ReleaseDC(pDC);

		// Redraw
		Invalidate();
	} // if

	return CWnd::OnQueryNewPalette();
}


/////////////////////////////////////////////////////////////////////
// CGLView helpers

void CChildView::GLSetupPalette()
{
	LOGICALPALETTE pal = { 0x300, 256 };
	BYTE baRed[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE baGreen[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE baBlue[] = {0, 85, 170, 255};
	int iColor;

	// Fill our logical palette structure with color data
	for (iColor = 0; iColor < 256; ++iColor ) 
	{
		pal.peaEntries[iColor].peRed   = baRed[iColor & 0x07];
		pal.peaEntries[iColor].peGreen = baGreen[(iColor >> 0x03) & 0x07];
		pal.peaEntries[iColor].peBlue  = baBlue[(iColor >> 0x06) & 0x03];
		pal.peaEntries[iColor].peFlags = 0;
	} // for

	// Build our palette
	m_hPalette = CreatePalette((LOGPALETTE*)&pal);
}


bool CChildView::GLLoadFont()
{
	CFont    ThisFont;
	CFont*   pOldFont;

	int l;

	CPaintDC dc(this); // device context for painting
	HDC hDC = dc.m_hDC;
	wglMakeCurrent(dc.m_hDC,m_hRC);

	CFont TempFont;
	LOGFONT lf;
 
	if(!TempFont.CreatePointFont(85, "Courier New"))
	{
//		AfxMessageBox("Font Courrier New is not present\n switching to Courier Font");
		if(!TempFont.CreatePointFont(85, "Courier"))
		{
//			AfxMessageBox("Couldn't find Courier font either. Exiting...");
		TempFont.CreateFont(
			12,                        // nHeight
			8,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			FIXED_PITCH | FF_MODERN,  // nPitchAndFamily
			"DefaultFont");                 // lpszFacename
		}
	}

	TempFont.GetLogFont(&lf);
	lf.lfHeight = m_WndLogFont.lfHeight;
	if(!ThisFont.CreateFontIndirect(&lf))
	{
		AfxMessageBox("Couldn't create indirect Font. Exiting...");
		return false;
	}
	pOldFont = dc.SelectObject(&ThisFont);


	if(!wglUseFontBitmaps(hDC, 0,255, GLF_START_LIST))
	{
		AfxMessageBox("Couldn't create GL Font Bitmaps. Exiting...");
		return false;
	}

	MAT2 m2 = {{0, 1}, {0, 0}, {0, 0}, {0, 1}}; 
	GLYPHMETRICS gm;

	m_LetterWidth = 0;
	DWORD res;
	for (l=42; l<122; l++)
	{
		res = dc.GetGlyphOutline(l, 0, &gm, 0, NULL, &m2);
		if(res==-1)
		{
			AfxMessageBox("Couldn't create GL Glyph Outline. Exiting...");
			return false;
		}
		m_LetterWidth = max (m_LetterWidth,(int)(gm.gmBlackBoxX));
	}

	dc.SelectObject(pOldFont);
	wglMakeCurrent(dc.m_hDC,NULL);
	return true;
}


int CChildView::GetStringWidth(CString strong)
{
	int width=0;
	CFont    ThisFont;
	CFont*   pOldFont;
	GLYPHMETRICS gm;
	DWORD res;
	MAT2 m2 = {{0, 1}, {0, 0}, {0, 0}, {0, 1}}; 
	CPaintDC dc(this); // device context for painting
	HDC hDC = dc.m_hDC;

	ThisFont.CreateFontIndirect(&m_WndLogFont);
	pOldFont = dc.SelectObject(&ThisFont);
	res = GetGlyphOutline(hDC,32,GGO_BITMAP,&gm,32,NULL, &m2);

	for (int l=0; l<strong.GetLength(); l++){
		res = dc.GetGlyphOutline(strong[l], 0, &gm, 0, NULL, &m2);
		res = GetGlyphOutline(hDC,strong[l],GGO_BITMAP,&gm,32,NULL, &m2);
		width += (int)(gm.gmBlackBoxX);
	}
	dc.SelectObject(pOldFont);
	return max(width,0);
}


void CChildView::OnClrSettings() 
{
	m_DefaultGraph.SetDrawRect(m_rCltRect);
	m_DefaultGraph.Init();

	CPrefsDlg dlg(this);
	dlg.m_crWnd        = m_crBackColor;
	dlg.m_WndTextColor = m_WndTextColor;
	memcpy(&dlg.m_WndLogFont, &m_WndLogFont, sizeof(m_WndLogFont));
	dlg.m_pGraph = &m_DefaultGraph;
	dlg.m_bBlackAndWhite = IsBlackAndWhite();

	if(IDOK == dlg.DoModal())
	{
		SetBlackAndWhite(dlg.m_bBlackAndWhite);
		m_crBackColor  = dlg.m_crWnd;
		memcpy(&m_WndLogFont, &dlg.m_WndLogFont, sizeof(dlg.m_WndLogFont));
		m_WndTextColor  = dlg.m_WndTextColor;
		GLLoadFont();

		m_pXDirect->SetColors(&m_DefaultGraph);
		m_pXInverse->SetColors(&m_DefaultGraph);
		m_pMiarex->SetColors(&m_DefaultGraph);
		m_pMiarex->m_bResetglGeom     = true;
		m_pMiarex->m_bResetglMesh     = true;
		m_pMiarex->m_bResetglDownwash = true;
		m_pMiarex->m_bResetglLift     = true;
		m_pMiarex->m_bResetglOpp      = true;
		m_pMiarex->m_bResetglWake     = true;
		m_pMiarex->m_bResetglLegend   = true;


		CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
		switch (pFrame->m_iApp){
			case XFOILANALYSIS:{
				pFrame->m_PolarDlgBar.SetParams();
				break;
			}
			case DIRECTDESIGN:{
				pFrame->m_AFoilCtrlBar.SetParams();
				break;
			}
			case MIAREX:{
				pFrame->m_PolarDlgBar.SetParams();
				break;
			}
		}	
		Invalidate();
	}	
}
void CChildView::ApplyClrSettings(bool bBW, COLORREF WingPanels, COLORREF XCPColor,
						  COLORREF XTopColor, COLORREF XBotColor, COLORREF DownwashColor,
						  COLORREF crFoilColor, COLORREF crBLColor, COLORREF crPressureColor) 
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	pFrame->m_OutlineColor = WingPanels;

	pFrame->m_XCPColor = XCPColor;
	pFrame->m_XTopColor = XTopColor;
	pFrame->m_XBotColor = XBotColor;
	pFrame->m_DownwashColor = DownwashColor;
	m_pMiarex->m_bResetglOpp = true;

	m_pXDirect->m_crFoilColor = crFoilColor;
	m_pXDirect->m_crBLColor = crBLColor;
	m_pXDirect->m_crPressureColor = crPressureColor;

	m_pXDirect->SetColors(&m_DefaultGraph);
	m_pMiarex->SetColors(&m_DefaultGraph);
	m_pXInverse->SetColors(&m_DefaultGraph);

	SetBlackAndWhite(bBW);

	Invalidate();	
}


void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMainFrame * pFrame  = (CMainFrame*)m_pFrameWnd; 
	switch (pFrame->m_iApp)
	{
		case MIAREX:
		{
			if(m_pMiarex->m_iView==3 || m_pMiarex->m_iView==5) 
			{
				SHORT sh   = GetKeyState(VK_CONTROL);
				if(!(sh & 0x8000)) 
				{
					m_pMiarex->m_bArcball = false;
					m_pMiarex->UpdateView();
				}
			}
			break;
		}
	}
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


