/****************************************************************************

    CLLTDlg Class
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

// IterDlg.cpp : implementation file
// Sets up the dialog box which will run the LLT thread
// We need a thread to authorize user interruption of the calculation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "LLTDlg.h"
#include "Miarex.h"
#include ".\lltdlg.h"



/////////////////////////////////////////////////////////////////////////////
// CLLTDlg dialog


CLLTDlg::CLLTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLLTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLLTDlg)
	//}}AFX_DATA_INIT
	m_IterGraph.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_IterGraph.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_IterGraph.SetMargin(70);
	m_IterGraph.SetAutoY(true);
	m_IterGraph.SetXMin(0.0);
	m_IterGraph.SetXMax(50);
	m_IterGraph.SetYMin(0.0);
	m_IterGraph.SetYMax(1.0);
	m_IterGraph.SetType(1);
	m_pIterCurve = NULL;

	m_State = 0;
	m_DlgPos.x = 0;
	m_DlgPos.y = 0;

	m_pWThread   = NULL;
	m_pWing      = NULL;

	m_bSequence = false;
	m_bType4    = false;
	m_bWarning  = false;
}


void CLLTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLLTDlg)
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDC_SKIP, m_ctrlSkip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLLTDlg, CDialog)
	//{{AFX_MSG_MAP(CLLTDlg)
	ON_BN_CLICKED(IDC_SKIP, OnSkip)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLLTDlg message handlers

BOOL CLLTDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_DlgPos.x==0 && m_DlgPos.y==0)
		SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-600,30,0,0,SWP_NOSIZE);	

	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	GetTempPath(MAX_PATH,szAppPath);
	strAppDirectory = szAppPath;
	str =strAppDirectory + "XFLR5.log";
	m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
	m_pWing->m_pXFile = &m_XFile;

	SetFileHeader();

	m_IterRect.SetRect(20,20,460,320);
	m_IterGraph.DeleteCurves();
	m_pIterCurve = m_IterGraph.AddCurve();

	if (IsBlackAndWhite()) {
		m_IterGraph.SetAxisColor(RGB(0,0,0));
		m_IterGraph.SetBkColor(RGB(255,255,255));
		m_IterGraph.SetLabelColor(RGB(0,0,0));
		m_IterGraph.SetLegendColor(RGB(0,0,0));
	}

	CClientDC dc(this);

	LOGFONT LgFt; 
	ZeroMemory(&LgFt, sizeof(LOGFONT));
	LgFt.lfHeight = - MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
	LgFt.lfWeight = FW_BOLD;
	strcpy(LgFt.lfFaceName,"Comic sans MS"); 
	m_IterGraph.SetTitleLogFont(&LgFt);
	LgFt.lfHeight = - MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
	m_IterGraph.SetLabelLogFont(&LgFt);

	if(m_pWing->m_bLLT){ 
		m_pWThread = new CLLTThread(this);
		m_pWThread->m_pParent = this;
//		m_pWThread->m_bAutoDelete = true;
		m_pWThread->m_bAutoDelete = false;
		m_pWThread->m_bSequence  = m_bSequence;
		m_pWThread->m_bType4     = m_bType4;
		m_pWThread->m_Alpha      = m_Alpha;
		m_pWThread->m_AlphaMax   = m_AlphaMax;
		m_pWThread->m_DeltaAlpha = m_DeltaAlpha;
		m_pWThread->m_pWing      = m_pWing;
		m_pWThread->m_pMiarex    = m_pMiarex;
				
		m_pWThread->CreateThread(CREATE_SUSPENDED);
		VERIFY(m_pWThread->SetThreadPriority(THREAD_PRIORITY_LOWEST));

		m_pWThread->ResumeThread();
	}
	else PostMessage(WM_CLOSE);

	GetDlgItem(IDCANCEL)->SetFocus();
	SetTimer(ID_THREADTIMER, 100, NULL);
	return FALSE; 
}


void CLLTDlg::OnTimer(UINT nIDEvent)
{
	if(m_pWThread && m_pWThread->m_bFinished){
		HANDLE hThread = m_pWThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		delete m_pWThread;
		m_XFile.Close();
		EndDialog(0);
	}
	CDialog::OnTimer(nIDEvent);
}


void CLLTDlg::UpdateView(double Alpha)
{
	CClientDC dc(this); // device context for painting

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	dcMem.SetBkMode(TRANSPARENT);

	CRect dcMemRect(0,0,m_IterRect.Width(), m_IterRect.Height());
	CBitmap bmb;
	bmb.CreateCompatibleBitmap(&dc,m_IterRect.Width(), m_IterRect.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(&bmb);

	CBrush FillBrush(GetSysColor(COLOR_3DFACE));
	dcMem.FillRect(&dcMemRect,&FillBrush);

	CRect BltRect(0,0,m_IterRect.Width(), m_IterRect.Height());
	m_IterGraph.DrawGraph(&dcMem, &BltRect,false);

	dcMem.SetTextAlign(TA_LEFT);
	if(IsBlackAndWhite())	dcMem.SetTextColor(0);
	else					dcMem.SetTextColor(RGB(150,150,150));

	CFont TFont;
	TFont.CreatePointFont(85, "Symbol");
	CFont* pOldFont = dcMem.SelectObject(&TFont);
	dcMem.TextOut(50,35,"(|Da|)");
	CFont MFont;
	MFont.CreatePointFont(85, "Arial");
	dcMem.SelectObject(&MFont);
	
	CString strong;
	strong.Format("max = %.4f", m_pIterCurve->y[m_pIterCurve->n-1]);

	dcMem.TextOut(80,35,strong);

	CFont CvFont;
	CvFont.CreateFont(18,0,0,0,FW_BOLD,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");

	dcMem.SelectObject(&CvFont);

	dcMem.SetBkMode(TRANSPARENT);
	CString Title;

	if(!m_bType4){
		Title.Format("Alpha = %.2f°", Alpha);
	}
	else{
		Title.Format(" Iterations for QInf = %.2f m/s", Alpha);
	}
	dcMem.TextOut(200,5,Title);

	if(m_State ==1){
		if(IsBlackAndWhite()){
			dcMem.SetTextColor(0);
		}
		else{
			dcMem.SetTextColor(RGB(0,150,0));
		}
		dcMem.TextOut(200,35,"CONVERGED !");
	}
	else if (m_State ==2){
		if(IsBlackAndWhite()) 	dcMem.SetTextColor(0);
		else dcMem.SetTextColor(RGB(255,0,0));
		dcMem.TextOut(200,35,"UNCONVERGED...");
	}


	dc.BitBlt(m_IterRect.left, m_IterRect.top, m_IterRect.Width(), m_IterRect.Height(),
			  &dcMem,0, 0, SRCCOPY);
	dcMem.SelectObject(pOldFont);
	dcMem.SelectObject(pOldBitmap);
	bmb.DeleteObject();
	CvFont.DeleteObject();
	TFont.DeleteObject();
	MFont.DeleteObject();
	FillBrush.DeleteObject();
}

void CLLTDlg::OnCancel() 
{
	if (m_pWThread && !m_pWThread->m_bFinished)
		 m_pWThread->Cancel();
	
//	CDialog::OnCancel();
}



void CLLTDlg::OnSkip() 
{
	m_pWThread->m_bSkip = true;
}
/*
void CLLTDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	HANDLE hThread = m_pWThread->m_hThread;
	WaitForSingleObject(hThread, INFINITE);

//	m_XFile.Close();
}
*/


void CLLTDlg::SetFileHeader()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!pMiarex) return;
	if(!pMiarex->m_pCurWing) return;
	if(!pMiarex->m_pCurWPolar) return;
	pMiarex->m_pXFile = &m_XFile;
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pFrame->m_VersionName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pMiarex->m_pCurWing->m_WingName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pMiarex->m_pCurWPolar->m_PlrName);
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

bool CLLTDlg::EndSequence()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(!m_pWThread) return false;

	m_XFile.Close();
	if(m_bWarning && pMiarex->m_bLogFile){
		if(IDYES == AfxMessageBox("Some points were found outside the available flight envelope\r\nView the Log file for details ?", MB_YESNOCANCEL)){
			CFile lf;
			CString str;
			CString strAppDirectory;
			char    szAppPath[MAX_PATH] = "";
			::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
			// Extract directory
			strAppDirectory = szAppPath;
			strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
			str =strAppDirectory + "XFLR5.log";

			if(lf.Open(str, CFile::modeRead)){// file exists (there should be a better way to do this)
				lf.Close();
				
				ShellExecute(GetSafeHwnd(),
					_T("open"),
					str,
					NULL,
					NULL,
					SW_SHOWNORMAL);	
			}
		}
	}
	delete m_pWThread;
	m_pWThread = NULL;
	EndDialog(0);
	return true;
}


