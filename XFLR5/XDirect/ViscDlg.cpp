/****************************************************************************

    ViscDlg Class
    Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

// ViscDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "XDirect.h"
#include "ViscDlg.h"
#include "../main/MainFrm.h"
#include ".\viscdlg.h"


/////////////////////////////////////////////////////////////////////////////
// CViscDlg dialog


CViscDlg::CViscDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViscDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViscDlg)
	//}}AFX_DATA_INIT
	m_ViscRect.SetRect(25,30,420,270);
	m_RmsGraph.SetDrawRect(m_ViscRect);

	m_RmsGraph.SetXTitle("Iter");
	m_RmsGraph.SetYTitle("");//Change from bl newton system solution

	m_RmsGraph.SetAuto(true);

	m_RmsGraph.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_RmsGraph.SetYMajGrid(true, RGB(120,120,120),2,1);

	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax(50);
	m_RmsGraph.SetYMin(0.0);
	m_RmsGraph.SetYMax(1.0);
	m_RmsGraph.SetType(1);

	m_Iterations = 0;
	m_bType4 = false;
	m_LegendPlace.x = 70;
	m_LegendPlace.y = 50;

	m_pFrame = NULL;
	m_pXDirect = NULL;

	m_AlphaMin = 0.0;
	m_AlphaMax = 1.0;
	m_DeltaAlpha = 0.5;
	m_ClMin = 0.0;
	m_ClMax = 1.0;
	m_DeltaCl = 0.1;
	m_ReMin = 10000.0;
	m_ReMax = 100000.0;
	m_DeltaRe = 10000.0;
}


void CViscDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViscDlg)
	DDX_Control(pDX, IDC_SKIP, m_ctrlSkip);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViscDlg, CDialog)
	//{{AFX_MSG_MAP(CViscDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViscDlg message handlers



BOOL CViscDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect WndRect;
	GetWindowRect(WndRect);
	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-WndRect.Width()-10,60,0,0,SWP_NOSIZE);
	
	CString str;
	CString strAppDirectory;

	CXDirect *pXDirect = (CXDirect*)m_pXDirect;

	char    szAppPath[MAX_PATH] = "";
	GetTempPath(MAX_PATH,szAppPath);
	strAppDirectory = szAppPath;
	str =strAppDirectory + "XFLR5.log";
	m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
	m_pXFoil->pXFile = &m_XFile;

	SetFileHeader();

	CClientDC dc(this);

	LOGFONT LgFt; 
	ZeroMemory(&LgFt, sizeof(LOGFONT));
	LgFt.lfHeight = - MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
	strcpy(LgFt.lfFaceName,"Arial"); 
	m_RmsGraph.SetTitleLogFont(&LgFt);
	LgFt.lfHeight = - MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
	m_RmsGraph.SetLabelLogFont(&LgFt);
	m_Iterations = 0;
	ResetCurves();
	if (IsBlackAndWhite()) 
	{
		m_RmsGraph.SetAxisColor(RGB(0,0,0));
		m_RmsGraph.SetBkColor(RGB(255,255,255));
		m_RmsGraph.SetLabelColor(RGB(0,0,0));
		m_RmsGraph.SetLegendColor(RGB(0,0,0));
	}

	m_pIterThread = new CViscThread(this);

	m_pIterThread->CreateThread(CREATE_SUSPENDED);
	m_pIterThread->SetThreadPriority(THREAD_PRIORITY_LOWEST);
	m_pIterThread->m_pParent = this;
	m_pIterThread->m_pXFoil = m_pXFoil;
	m_pIterThread->m_bType4 = m_bType4;

	if(!m_bType4)
	{
		if(m_bAlpha) m_pIterThread->SetAlpha(m_AlphaMin,m_AlphaMax,m_DeltaAlpha);
		else         m_pIterThread->SetCl(m_ClMin,m_ClMax,m_DeltaCl);
	}
	else m_pIterThread->SetRe(m_ReMin,m_ReMax,m_DeltaRe);
	
	m_pIterThread->m_bAutoDelete = false;
	m_pIterThread->m_bAutoInitBL = pXDirect->m_bAutoInitBL;

	m_pIterThread->m_IterLim     = m_IterLim;
	m_pIterThread->ResumeThread();
	SetTimer(ID_THREADTIMER, 100, NULL);
	return false;  
}


void CViscDlg::OnTimer(UINT nIDEvent)
{
	if(m_pIterThread && m_pIterThread->m_bFinished){
		if(!m_pIterThread) return;
		HANDLE hThread = m_pIterThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		delete m_pIterThread;
		m_pIterThread = NULL;
		m_XFile.Close();
		EndDialog(0);
	}

	CDialog::OnTimer(nIDEvent);
}

/*
void CViscDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
//	HANDLE hThread = m_pIterThread->m_hThread;
//	WaitForSingleObject(hThread, INFINITE);
	DWORD response = WaitForSingleObject(m_pIterThread->m_hThread, INFINITE);
	if(response == WAIT_ABANDONED){
		Trace("Abandoned thread");
//		TRACE("Abandoned thread\n");
	}
	else if(response == WAIT_TIMEOUT){
//		Trace("Thread time out");
		TRACE("Thread time out\n");
	}
	else if(response == WAIT_OBJECT_0){
		Trace("Thread returned correctly");
//		TRACE("Thread returned correctly\n");
	}
	delete m_pIterThread;
	m_XFile.Close();
}*/


void CViscDlg::ResetCurves()
{
	m_RmsGraph.DeleteCurves();
	m_RmsGraph.AddCurve();
	m_RmsGraph.AddCurve();
	if(IsBlackAndWhite()){
		m_RmsGraph.GetCurve(0)->SetColor(635100);
		m_RmsGraph.GetCurve(1)->SetColor(4);
	}
	m_RmsGraph.GetCurve(0)->SetTitle("rms");
	m_RmsGraph.GetCurve(1)->SetTitle("max");
	m_RmsGraph.GetCurve(1)->SetStyle(PS_SOLID);
	m_RmsGraph.SetAutoX(false);
	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax((double)m_IterLim);
	m_RmsGraph.SetXUnit((int)(m_IterLim/10.0));
	m_RmsGraph.SetYMin(0.0);
	m_RmsGraph.SetYMax(1.0);
	m_RmsGraph.SetX0(0.0);
	m_RmsGraph.SetY0(0.0);
}


void CViscDlg::OnCancel()
{
	if(m_pIterThread && !m_pIterThread->m_bFinished) {
		m_pIterThread->m_bSkip = true;
		m_pIterThread->m_bExit = true;
		m_pIterThread->m_bSuspend = false;
		HANDLE hThread = m_pIterThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
	}
//	OnSuspend();

//	CDialog::OnCancel();
}

void CViscDlg::SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha)
{
	m_AlphaMin = AlphaMin;
	m_AlphaMax = AlphaMax;
	m_DeltaAlpha = DeltaAlpha;
}

void CViscDlg::SetCl(double ClMin, double ClMax, double DeltaCl)
{
	m_ClMin = ClMin;
	m_ClMax = ClMax;
	m_DeltaCl = DeltaCl;
}

void CViscDlg::SetRe(double ReMin, double ReMax, double DeltaRe)
{
	m_ReMin = ReMin;
	m_ReMax = ReMax;
	m_DeltaRe = DeltaRe;
}

void CViscDlg::OnSkip() 
{
	m_pIterThread->m_bSkip = true;
}


BOOL CViscDlg::PreTranslateMessage(MSG* pMsg) 
{
/*	if(pMsg->message == V_ENDTHREAD){
		Trace("Received V_ENDTHREAD message");
		EndDialog(0);
		Trace("Ended dialog");
		return true; // no need to process further
	} 
	else */
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)){
		m_pIterThread->m_bSkip = true;
		return true;
	}
	else if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		if(GetDlgItem(IDCANCEL) == GetFocus())	{
			OnCancel();
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CViscDlg::AddOpPoint()
{
	CXDirect *pXDirect = (CXDirect*)m_pXDirect;
	pXDirect->AddOpPoint();
}


void CViscDlg::SetFileHeader()
{
	CXDirect *pXDirect = (CXDirect*)m_pXDirect;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pFrame->m_VersionName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(m_FoilName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pXDirect->m_pCurPolar->m_PlrName);
	m_XFile.WriteString("\n");

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str, strong;
	switch (tm.wMonth){
		case 1:{
			strong = "January";
			break;
		}
		case 2:{
			strong = "February";
			break;
		}
		case 3:{
			strong = "March";
			break;
		}
		case 4:{
			strong = "April";
			break;
		}
		case 5:{
			strong = "May";
			break;
		}
		case 6:{
			strong = "June";
			break;
		}
		case 7:{
			strong = "July";
			break;
		}
		case 8:{
			strong = "August";
			break;
		}
		case 9:{
			strong = "September";
			break;
		}
		case 10:{
			strong = "October";
			break;
		}
		case 11:{
			strong = "November";
			break;
		}
		case 12:{
			strong = "December";
			break;
		}
	}
	str.Format(" %02d, %d  at  %02d:%02d:%02d \n\n",
		 tm.wDay, tm.wYear,
		tm.wHour, tm.wMinute, tm.wSecond);

	m_XFile.WriteString(strong + str);
	m_XFile.WriteString("\n___________________________________\n\n");
}



void CViscDlg::UpdateView()
{
	CClientDC dc(this); // device context for painting
	int btnHeight = 50;

	CRect CltRect;
	GetClientRect(&CltRect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	dcMem.SetBkMode(TRANSPARENT);

	CRect dcMemRect(0,0,CltRect.Width(), CltRect.Height()-btnHeight);
	CBitmap bmb;
	bmb.CreateCompatibleBitmap(&dc,CltRect.Width(),  CltRect.Height()-btnHeight);
	CBitmap * pOldBmp = dcMem.SelectObject(&bmb);

	CBrush FillBrush(GetSysColor(COLOR_3DFACE));
	dcMem.FillRect(&dcMemRect,&FillBrush);
	
	m_RmsGraph.DrawGraph(&dcMem, &m_ViscRect,false);
	m_RmsGraph.DrawLegend(&dcMem, false, m_LegendPlace);

	CFont CvFont;
	CvFont.CreateFont(18,0,0,0,FW_BOLD,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");
	CFont* pOldFont = dcMem.SelectObject(&CvFont);
	
	dcMem.SetTextAlign(TA_LEFT);
	dcMem.SetBkMode(TRANSPARENT);
	CString Title;
	dcMem.SetTextColor(RGB(0,0,0));

	if(!m_bType4){
		if (m_bAlpha)
			Title.Format("Alfa = %7.2f°", m_pXFoil->alfa*180.0/3.141592654);
		else
			Title.Format("Cl = %7.2f", m_pXFoil->clspec);
	}
	else{
		ReynoldsFormat(Title, m_pXFoil->reinf);
		Title = "Re = " + Title;
	}
	dcMem.TextOut(150,5,Title);

	if(m_pXFoil->lvconv){
		dcMem.SetTextColor(RGB(0,150,0));
		dcMem.TextOut(160,45,"CONVERGED !");
	}
	else {
		if(m_pIterThread->m_Iterations>=m_IterLim && !m_pXFoil->lvconv){
			dcMem.SetTextColor(RGB(255,0,0));
			dcMem.TextOut(160,45,"UNCONVERGED...");
		}
	}

	dc.BitBlt(0, CltRect.top, CltRect.Width(), m_ViscRect.Height()+42, &dcMem,0, 0, SRCCOPY);	
	dcMem.SelectObject(pOldFont);
	dcMem.SelectObject(pOldBmp);
	bmb.DeleteObject();
	CvFont.DeleteObject();
	FillBrush.DeleteObject();
}

