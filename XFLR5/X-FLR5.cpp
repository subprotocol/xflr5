/****************************************************************************

	X-FLR6 Application

    Copyright (C) 2003-2008 André Deperrois XFLR5@yahoo.com

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

// X-FLR6.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "X-FLR5.h"
#include "./main/MainFrm.h"




// CXFLR5App

BEGIN_MESSAGE_MAP(CXFLR5App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// CXFLR5App construction

CXFLR5App::CXFLR5App()
{
}


// The one and only CXFLR5App object

CXFLR5App theApp;


// CXFLR5App initialization

BOOL CXFLR5App::InitInstance()
{
	//protection for beta versions
	SYSTEMTIME time;
	GetSystemTime(&time);
//	if (time.wYear>=2009 || (time.wYear >=2008 && time.wMonth>=6)) 
//	{
//		AfxMessageBox("This version of XFLR5 has expired\nPlease download a new version", MB_OK);
//		return FALSE;
//	}
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	bTrace = false;
	if(bTrace)
	{
		CString strAppDirectory;
		char    szAppPath[MAX_PATH] = "";
		::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
		// Extract directory
		strAppDirectory = szAppPath;
		strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
		TraceFileName = strAppDirectory + "Trace.txt";
		CStdioFile tf;
		tf.Open(TraceFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		tf.Close();
	}
	//Trace("CX5App::InitInstance::Launching app");


//	AfxEnableControlContainer();

	//Trace("CX5App::InitInstance::Containers enabled");
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif*/
	//Trace("CX5App::InitInstance::Controls Enabled");

	// Get command line info in case user has launched app from a file
	CCommandLineInfo CmdInfo;
	ParseCommandLine(CmdInfo);


	//Trace("CX5App::InitInstance::Command Line Info processed");

/*
	if (!FirstInstance(CmdInfo))
	{
		//Trace("CX5App::InitInstance::Redirected input to first instance");
		//Trace("CX5App::InitInstance::Closing App");

		return FALSE;

	}
*/

	//Trace("CX5App::InitInstance::System time acquired");

	g_bBlackAndWhite = false;
	g_bColor         = true;
	g_bWindowBk      = false;
	g_bGraphBk       = false;
	m_PenWidth       = 10.0;

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.
	//Trace("CX5App::InitInstance::Global variables set");

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	//Trace("CX5App::InitInstance::MainFrame created=", pFrame);
	//Trace("CX5App::InitInstance::ChildView created=", &(pFrame->m_wndView));

	CFile fp;
	int k,x,y,cx,cy,l,t,r,b;
	UINT showCmd;

	//load previously saved size and position
	CString str, strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
 	str =strAppDirectory + "XFLR5.set";
	x=0; y=0; cx = GetSystemMetrics(SM_CXSCREEN); cy = GetSystemMetrics(SM_CYSCREEN);
	l=x; t = y;
	r = cx; b = cy;
	showCmd = SW_SHOWMAXIMIZED;

	try
	{
		if(fp.Open(str,CFile::modeRead))
		{
			CArchive ar(&fp, CArchive::load);
			ar >> k;
			if(k!=100320)
			{
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
		//  we're reading/loading
			ar >> l >> t >> r >> b >> showCmd;
			x=l; y=t; cx = max(r-l,200); cy = max(b-t,200);
			ar.Close();
			fp.Close();
		}
	}
	catch (CArchiveException *ex)
	{
		ex->Delete();
	}
	catch (CException *ex)
	{
		ex->Delete();
	}

	pFrame->m_wndpl.rcNormalPosition.left   = l;
	pFrame->m_wndpl.rcNormalPosition.right  = r;
	pFrame->m_wndpl.rcNormalPosition.top    = t;
	pFrame->m_wndpl.rcNormalPosition.bottom = b;
	pFrame->m_wndpl.showCmd = showCmd;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,	NULL);

	//set previously saved size and position
	pFrame->SetWindowPos(&CWnd::wndTop, x, y, cx, cy,SWP_NOREDRAW);
	pFrame->ShowWindow(showCmd);
	
	//Trace("CX5App::InitInstance::Window displayed" );
	pFrame->UpdateWindow();

	if(!CmdInfo.m_strFileName.IsEmpty())
	{
		int app = pFrame->LoadFile(CmdInfo.m_strFileName, CmdInfo.m_strFileName);
		//Trace("CX5App::InitInstance::Loaded File " + CmdInfo.m_strFileName);

		if (app == MIAREX)			pFrame->OnMiarex();
		if (app == XFOILANALYSIS)	pFrame->OnXDirect();
	}
	
	//Trace("CX5App::InitInstance::App launched successfully");

	return TRUE;
}


BOOL CXFLR5App::OnIdle(LONG lCount) 
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(pFrame->m_iApp == XFOILANALYSIS)
	{
		if(pFrame->XDirect.KickIdle()) return 1;
	}
	else if(pFrame->m_iApp == MIAREX)
	{
		if(pFrame->Miarex.KickIdle()) return 1;
	}
//	//Trace("Idle ...bored...%d\n", lCount);
	return CWinApp::OnIdle(lCount);
}


BOOL CXFLR5App::FirstInstance(CCommandLineInfo &CmdInfo)
{
	HWND hWnd, hWndChild;
	
	// Determine if another window with our class name and Window title exists...
	// The title "Instance " is set up later, in the InitDialog function.
//	hWnd = FindWindow(_T("#32769"),"XFLR5 ");// see MSDN : #32769 The class for the desktop window. 
	hWnd = FindWindow(NULL,"XFLR5 ");
	
	if (hWnd) 
	{
		//Trace("Previous window ref : ", hWnd);

		hWndChild = GetLastActivePopup(hWnd); // if so, does it have any popups?
	
		if (IsIconic(hWnd)) 
			ShowWindow(hWnd, SW_RESTORE);		// If iconic, restore the main window
		SetForegroundWindow(hWndChild);			// Bring the main window or it's popup to
												// the foreground
// post the command line info to the other app
		char* filename = NULL;
		filename = new char[100];
		strcpy(filename, CmdInfo.m_strFileName);
			
		if(!CmdInfo.m_strFileName.IsEmpty())
		{
			COPYDATASTRUCT cpd;
			cpd.dwData = XFLR5_DATA; // Identification..
			cpd.cbData = (DWORD)strlen(CmdInfo.m_strFileName);
			cpd.lpData = &CmdInfo.m_strFileName;
			cpd.lpData = filename;
			SendMessage(hWnd,WM_COPYDATA, NULL, (LPARAM)&cpd);
			//Trace("Copying data to ", hWnd);
		} 
		delete filename;
//and close this app
		return FALSE;                             
	}
	else return TRUE;			// This is the first instance. Proceed as normal.
}




CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_rectText.left   =   10;
	m_rectText.top    =  305;
	m_rectText.right  =  150;
	m_rectText.bottom =  325;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SLINK, m_ctrlSLink);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP() 



void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	if (m_rectText.PtInRect(point))
	{
		if(GetFocus()!=this && (GetStyle() & WS_TABSTOP))
			SetFocus();
		SetCapture();
	}
	return;
//	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture()==this)
	{
		ReleaseCapture();
		if(m_rectText.PtInRect(point)){
			ExecuteLink();
		}
	}
	return;

//	CDialog::OnLButtonUp(nFlags, point);
}


UINT CAboutDlg::OnNcHitTest(CPoint point) 
{
	CRect rectClient;
	CPoint ptClient(point);

	GetClientRect(rectClient);
	ScreenToClient(&ptClient);
	if(rectClient.PtInRect(ptClient))
	{
		return (HTCLIENT);
	}
	else 
	{
		return (CWnd::OnNcHitTest(point));
	}
//	return CDialog::OnNcHitTest(point);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);
	m_hcHand = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HANDCURSOR));

	m_ctrlSLink.SetCursor(m_hcHand);
	m_ctrlSLink.SetFocus();

	CFont* pFont = m_ctrlSLink.GetFont();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);

	logFont.lfHeight = 10;
	logFont.lfWeight = FW_MEDIUM;
	logFont.lfCharSet = ANSI_CHARSET;
	logFont.lfWidth = 0;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfUnderline = TRUE;
	m_fontControl.CreateFontIndirect(&logFont);

	CDC* pDC = m_ctrlSLink.GetDC();
	m_ctrlSLink.SetFont(&m_fontControl,TRUE);
	m_ctrlSLink.Invalidate();
	m_ctrlSLink.ReleaseDC(pDC);// not necessary according to MSDN, still...

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


HINSTANCE CAboutDlg::ExecuteLink()
{
	HINSTANCE hInstReturn;
	CString strText("http://xflr5.sourceforge.net");
	hInstReturn = ShellExecute(GetSafeHwnd(),
		_T("open"),
		strText,
		NULL,
		NULL,
		SW_SHOWNORMAL);
	return hInstReturn;
}



HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (pWnd->GetDlgCtrlID() == IDC_SLINK)
		pDC->SetTextColor(RGB(0,0,255));

	return hbr;
}
 

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(&ptMouse);
	if (pWnd ==this && (m_rectText.PtInRect(ptMouse)))
	{
		if (m_hcHand==(HCURSOR)0)
		{
			m_hcHand = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
			ASSERT (m_hcHand);
		}
		SetCursor(m_hcHand);
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


// CXFLR5App message handlers




// App command to run the dialog
void CXFLR5App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CX5App message handlers



// CXFLR5App message handlers


/*_____________________________________________________________________

                            Global Methods
_____________________________________________________________________*/

void ExpFormat(double &f, int &exp)
{

	if (f==0.0){
		exp = 0;
		f = 0.0;
		return;
	}
	double f1 = abs(f);
//	int sgn = int(f/f1);
	if(f1<1) 
		exp = (int)log10(f1)-1;
	else 
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);
	
}

void GetPaperSize(DEVMODE *pDevMode, int &pw, int &ph)
{

	int c = 254;//conversion factor from inches to tenths of millimeters

	switch(pDevMode->dmPaperSize)
	{
		case (DMPAPER_LETTER):
			pw = (int)(8.5*c);
			ph = 11*c;
			break;
		case (DMPAPER_LEGAL):
			pw = (int)(8.5f*c);
			ph = 14*c;
			break;
		case (DMPAPER_A4):
			pw = 2100;
			ph = 2970;
			break;
		case (DMPAPER_CSHEET):
			pw = 17*c;
			ph = 22*c;
			break;
		case (DMPAPER_DSHEET):
			pw = 22*c;
			ph = 34*c;
			break;
		case (DMPAPER_ESHEET):
			pw = 34*c;
			ph = 44*c;
			break;
		case (DMPAPER_LETTERSMALL):
			pw = (int)(8.5*c);
			ph = 11*c;
			break;
		case (DMPAPER_TABLOID):
			pw = 11*c;
			ph = 17*c;
			break;
		case (DMPAPER_LEDGER):
			pw = 17*c;
			ph = 11*c;
			break;
		case (DMPAPER_STATEMENT):
			pw = (int)(5.5*c);
			ph = (int)(8.5*c);
			break;
		case (DMPAPER_EXECUTIVE):
			pw = (int)(7.25*c);
			ph = (int)(10.5*c);
			break;
		case (DMPAPER_A3):
			pw = 2970;
			ph = 4200;
			break;
		case (DMPAPER_A4SMALL):
			pw = 2100;
			ph = 2970;
			break;
		case (DMPAPER_A5):
			pw = 1480;
			ph = 2100;
			break;
		case (DMPAPER_B4):
			pw = 2500;
			ph = 3540;
			break;
		case (DMPAPER_B5):
			pw = 1820;
			ph = 2570;
			break;
		case (DMPAPER_FOLIO):
			pw = (int)(8.5*c);
			ph = 13*c;
			break;
		case (DMPAPER_QUARTO):
			pw = 2150;
			ph = 2750;
			break;
		case (DMPAPER_10X14):
			pw = 10*c;
			ph = 14*c;
			break;
		case (DMPAPER_11X17):
			pw = 11*c;
			ph = 17*c;
			break;
		case (DMPAPER_NOTE):
			pw = (int)(8.5*c);
			ph = 11*c;
			break;
		case (DMPAPER_ENV_9):
			pw = (int)(3.875*c);
			ph = (int)(8.875*c);
			break;
		case (DMPAPER_ENV_10):
			pw = (int)(4.125*c);
			ph = (int)(9.5*c);
			break;
		case (DMPAPER_ENV_11):
			pw = (int)(4.5*c);
			ph = (int)(10.375*c);
			break;
		case (DMPAPER_ENV_12):
			pw = (int)(4.75*c);
			ph = 11*c;
			break;
		case (DMPAPER_ENV_14):
			pw = 5*c;
			ph = (int)(11.5*c);
			break;
		case (DMPAPER_ENV_DL):
			pw = 1100;
			ph = 2200;
			break;
		case (DMPAPER_ENV_C5):
			pw = 1620;
			ph = 2290;
			break;
		case (DMPAPER_ENV_C3):
			pw = 3240;
			ph = 4580;
			break;
		case (DMPAPER_ENV_C4):
			pw = 2290;
			ph = 3240;
			break;
		case (DMPAPER_ENV_C6):
			pw = 1140;
			ph = 1620;
			break;
		case (DMPAPER_ENV_C65):
			pw = 1140;
			ph = 2290;
			break;
		case (DMPAPER_ENV_B4):
			pw = 2500;
			ph = 3530;
			break;
		case (DMPAPER_ENV_B5):
			pw = 1760;
			ph = 2500;
			break;
		case (DMPAPER_ENV_B6):
			pw = 1760;
			ph = 1250;
			break;
		case (DMPAPER_ENV_ITALY):
			pw = 1100;
			ph = 2300;
			break;
		case (DMPAPER_ENV_MONARCH):
			pw = (int)(3.875*c);
			ph = (int)(7.5*c);
			break;
		case (DMPAPER_ENV_PERSONAL):
			pw = (int)(3.625*c);
			ph = (int)(6.5*c);
			break;
		case (DMPAPER_FANFOLD_US):
			pw = (int)(14.875*c);
			ph = 11*c;
			break;
		case (DMPAPER_FANFOLD_STD_GERMAN):
			pw = (int)(8.5*c);
			ph = 12*c;
			break;
		case (DMPAPER_FANFOLD_LGL_GERMAN):
			pw = (int)(8.5*c);
			ph = 13*c;
			break;
		default:
			// paper unknown - use A4 or whatever is best
			pw = 2100;
			ph = 2790;
	}
	pDevMode->dmPaperWidth  = (short)pw;
	pDevMode->dmPaperLength = (short)ph;
}



int GetPenWidth(int w, bool IsPrinting)
{
	if(IsPrinting){
		// nominal print width is 30 = 1/3 of a mm
		if(w==0) 
			return int(theApp.m_PenWidth/2);//half the nominal
		else 
			return(int)(w*theApp.m_PenWidth);
	}
	else{
		if(w==0) return 1;//nothing between 0 and 1
		else return(w);
	}
}


bool IsBlackAndWhite()
{
	return theApp.g_bBlackAndWhite;
//	if(((CXFLR5App*)AfxGetApp())->g_bBlackAndWhite) return true; else return false;
}


void GetBWColor(COLORREF &color, int &nStyle, int &nWidth)
{
	if (IsBlackAndWhite()) {
		UINT r,g,b;
		DecompRGB(color,r,g,b);
		if(r!=g || g!=b){
			UINT grey = (int)((r+g+b)/3);
			color = RGB(grey, grey, grey);
			if(nStyle==0) {
				nStyle = ((int)((2.7*g+1.4*b+6.25*r)))%5;
				if(nWidth<=1) nWidth = ((int)((3.6*g+3.2*b+3.1*r)))%3;
			}//else return the current style & width;
		}
	}
	else return;// the current colors
}

void SetBlackAndWhite(bool bBW)
{
	((CXFLR5App*)AfxGetApp())->g_bBlackAndWhite = bBW;
}

bool IsEven(int n)
{
	if(n%2==0) return true;
	else return false;
}


double sign(double a)
{
	if(a<0.0) return -1.0;
	else return 1.0;
}

/*
double pow(int a, int b){
	double res = 1.0;
	if (a==0) return 0.0;
	if (b==0) return 1.0;
	if(b>0){
		for (int i=0; i<b; i++){
			res = res * (double)a;
		}
	}
	else{
		for (int i=0; i>b; i--){
			res = res / (double)a;
		}
	}
	return res;
}*/

void ReynoldsFormat(CString &str, double f)
{
	int q, r, exp;
	exp = (int)log10(f);
	r = exp%3;
	q = (exp-r)/3;

	CString strong;
	strong.Format("%.0f", f);

	int l = strong.GetLength();

	for (int i=0; i<q; i++){
		strong.Insert(l-3*(i+1)-i," ");
		l++;
	}

	for (i=strong.GetLength(); i<9; i++){
		strong = " "+strong;
	}

	str = strong;
}



void GetSpeedUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="m/s";
			break;
		}
		case 1:{
			str="km/h";
			break;
		}
		case 2:{
			str="ft/s";
			break;
		}
		case 3:{
			str="kt";
			break;
		}
		case 4:{
			str="mph";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}


void GetWeightUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="g";
			break;
		}
		case 1:{
			str="kg";
			break;
		}
		case 2:{
			str="oz";
			break;
		}
		case 3:{
			str="lb";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

void GetAreaUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="mm²";
			break;
		}
		case 1:{
			str="cm²";
			break;
		}
		case 2:{
			str="dm²";
			break;
		}
		case 3:{
			str="m²";
			break;
		}
		case 4:{
			str="in²";
			break;
		}
		case 5:{
			str="ft²";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}
void GetLengthUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="mm";
			break;
		}
		case 1:{
			str="cm";
			break;
		}
		case 2:{
			str="dm";
			break;
		}
		case 3:{
			str="m";
			break;
		}
		case 4:{
			str="in";
			break;
		}
		case 5:{
			str="ft";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

void GetForceUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="N.m";
			break;
		}
		case 1:{
			str="lbf";
			break;
		}

		default:{
			str=" ";
			break;
		}
	}
}

void GetMomentUnit(CString &str, UINT unit)
{
	switch(unit){
		case 0:{
			str="N.m";
			break;
		}
		case 1:{
			str="lbf.in";
			break;
		}
		case 2:{
			str="lbf.ft";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}


void Trace(int n)
{
	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	CStdioFile tf;
	tf.Open(((CXFLR5App*)AfxGetApp())->TraceFileName, CFile::modeWrite | CFile::typeText);
	tf.SeekToEnd();
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str;
	str.Format("time = %2d:%2d:%2d.%03d    nnn=%d \r\n", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds,n);
	tf.WriteString(str);

	tf.Close();

}

void Trace(CString msg)
{
	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str;
	str.Format("time = %2d:%2d:%2d.%03d  ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	str += msg;
	str += "\r\n";
	CStdioFile tf;
	tf.Open(((CXFLR5App*)AfxGetApp())->TraceFileName, CFile::modeWrite | CFile::typeText);
	tf.SeekToEnd();
	tf.WriteString(str);
	tf.Close();

}


void Trace(CString msg, int n)
{
	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	CStdioFile tf;
	CString strong;
	strong.Format("  %d",n);
	strong = msg + strong;
	tf.Open(((CXFLR5App*)AfxGetApp())->TraceFileName, CFile::modeWrite | CFile::typeText);
	tf.SeekToEnd();
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str;
	str.Format("time = %2d:%2d:%2d.%03d  ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	str += strong;
	str += "\r\n";
	tf.WriteString(str);
	tf.Close();
}

void Trace(CString msg, double f)
{
	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	CStdioFile tf;
	CString strong;
	strong.Format("  %f",f);
	strong = msg + strong;
	tf.Open(((CXFLR5App*)AfxGetApp())->TraceFileName, CFile::modeWrite | CFile::typeText);
	tf.SeekToEnd();
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str;
	str.Format("time = %2d:%2d:%2d.%03d  ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	str += strong;
	str += "\r\n";
	tf.WriteString(str);

	tf.Close();
}

void Trace(CString msg, void *p)
{
	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	CStdioFile tf;
	CString strong;
	strong.Format("  %p",p);
	strong = msg + strong;
	tf.Open(((CXFLR5App*)AfxGetApp())->TraceFileName, CFile::modeWrite | CFile::typeText);
	tf.SeekToEnd();
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str;
	str.Format("time = %2d:%2d:%2d.%03d  ", tm.wHour, tm.wMinute, tm.wSecond, tm.wMilliseconds);
	str += strong;
	str += "\r\n";
	tf.WriteString(str);

	tf.Close();
}


void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit,
			  double &mtoUnit, double &m2toUnit, double &mstoUnit, 
			  double &kgtoUnit, double &NtoUnit, double &NmtoUnit) 
{

	switch(LUnit){
		case 0:{//mdm
			mtoUnit  = 1000.0;
			break;
		}
		case 1:{//cm
			mtoUnit  = 100.0;
			break;
		}
		case 2:{//dm
			mtoUnit  = 10.0;
			break;
		}
		case 3:{//m
			mtoUnit  = 1.0;
			break;
		}
		case 4:{//in
			mtoUnit  = 1000.0/25.4;
			break;
		}
		case 5:{///ft
			mtoUnit  = 1000.0/25.4/12.0;
			break;
		}
		default:{//m
			mtoUnit  = 1.0;
			break;
		}
	}
	switch(AUnit){
		case 0:{//mm²
			m2toUnit = 1000000.0;
			break;
		}
		case 1:{//cm²
			m2toUnit = 10000.0;
			break;
		}
		case 2:{//dm²
			m2toUnit = 100.0;
			break;
		}
		case 3:{//m²
			m2toUnit = 1.0;
			break;
		}
		case 4:{//in²
			m2toUnit = 1./0.254/0.254*100.0;
			break;
		}
		case 5:{//ft²
			m2toUnit = 1./0.254/0.254/144.0*100.0;
			break;
		}
		default:{
			m2toUnit = 1.0;
			break;
		}
	}

	switch(WUnit){
		case 0:{///g
			kgtoUnit = 1000.0;
			break;
		}
		case 1:{//kg
			kgtoUnit = 1.0;

			break;
		}
		case 2:{//oz
			kgtoUnit = 1./ 2.83495e-2;
			break;
		}
		case 3:{//lb
			kgtoUnit = 1.0/0.45359237;
			break;
		}
		default:{
			kgtoUnit = 1.0;
			break;
		}
	}
	switch(SUnit){
		case 0:{// m/s
			mstoUnit = 1.0;
			break;
		}
		case 1:{// km/h
			mstoUnit = 3600.0/1000.0;
			break;
		}
		case 2:{// ft/s
			mstoUnit = 100.0/2.54/12.0;
			break;
		}
		case 3:{// kt (int.)
			mstoUnit = 1.0/0.514444;
			break;
		}
		case 4:{// mph
			mstoUnit = 3600.0/1609.344;
			break;
		}
		default:{
			mstoUnit = 1.0;
			break;
		}
	}
	
	switch(FUnit){
		case 0:{//N
			NtoUnit = 1.0;
			break;
		}
		case 1:{//lbf
			NtoUnit = 1.0/4.44822;
			break;
		}
		default:{
			NtoUnit = 1.0;
			break;
		}
	}
	switch(MUnit){
		case 0:{//N.m
			NmtoUnit = 1.0;
			break;
		}
		case 1:{//lbf.in
			NmtoUnit = 1.0/4.44822/0.0254;
			break;
		}
		case 2:{//lbf.0t
			NmtoUnit = 1.0/4.44822/12.0/0.0254;
			break;
		}
		default:{
			NmtoUnit = 1.0;
			break;
		}
	}
}

bool Rewind1Line(CStdioFile *pXFile, int &Line, CString &strong)
{
	int length = strong.GetLength() * 1+2;//1 char takes one byte in the file ?
	pXFile->Seek(-length, CFile::current);
	Line--;
	return true;
}

BOOL ReadAVLString(CStdioFile *pXFile, int &Line, CString &strong)
{
	BOOL bRead = TRUE;
	bool bComment = true;
	int pos;
	while(bComment && bRead){
		bComment = false;

		bRead = pXFile->ReadString(strong);

		pos = strong.Find("#",0);
		if(pos==0) bComment = true;
		pos = strong.Find("!",0);
		if(pos==0) bComment = true;
	
		Line++;
	}
	
	return bRead;
}


void DecompRGB(COLORREF rgb, double &r, double &g, double &b)
{
	int ir, ig, ib;
	ir  = rgb%256;
	rgb = (rgb-ir)/256;
	ig  = rgb%256;
	ib  = (rgb-ig)/256;

	r = (double)(ir/255.0);
	g = (double)(ig/255.0);
	b = (double)(ib/255.0);
}

void DecompRGB(COLORREF rgb, UINT &r, UINT &g, UINT &b)
{
	r  = rgb%256;
	rgb = (rgb-r)/256;
	g  = rgb%256;
	b  = (rgb-g)/256;
}


double GLGetRed(double tau)
{
	if(tau>2.0/3.0)       return 1.0;
	else if(tau>1.0/3.0)  return (GLfloat)(3.0*(tau-1.0/3.0));
	else				  return 0.0;
}

double GLGetGreen(double tau)
{
	if(tau<0.f || tau>1.0) 	return 0.0;
	else if(tau<1.0/4.0) 	return (GLfloat)(4.0*tau);
	else if(tau>3.0/4.0) 	return (GLfloat)(1.0-4.0*(tau-3.0/4.0));
	else					return 1.0;
}

double GLGetBlue(double tau)
{
	if(tau>2.0/3.0) 		return 0.0;
	else if(tau>1.0/3.0)	return (GLfloat)(1.0-3.0*(tau-1.0/3.0));
	else		    		return 1.0;
}


BOOL bSetupPixelFormat(HDC hDC)
{
    static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,				// version number
		PFD_DRAW_TO_WINDOW |		// support window
		PFD_DOUBLEBUFFER |
		PFD_SUPPORT_OPENGL,		// support OpenGL
		PFD_TYPE_RGBA,			// RGBA type
		24,				// 24-bit color depth
		0, 0, 0, 0, 0, 0,		// color bits ignored
		0,				// no alpha buffer
		0,				// shift bit ignored
		0,				// no accumulation buffer
		0, 0, 0, 0, 			// accum bits ignored
		32,				// 32-bit z-buffer	
		0,				// no stencil buffer
		0,				// no auxiliary buffer
		PFD_MAIN_PLANE,			// main layer
		0,				// reserved
		0, 0, 0				// layer masks ignored
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(hDC, &pfd)) == 0 )
    {
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

    if (SetPixelFormat(hDC, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
        return FALSE;
    }

//    CreateRGBPalette(hDC);

    return TRUE;
}

void SetGraphDlgPage(int page)
{
	if (page<0) page = 0;
	if (page>5) page = 5;
	((CXFLR5App*)AfxGetApp())->g_GraphDlgPage = page;
}

int GetGraphDlgPage()
{
	return ((CXFLR5App*)AfxGetApp())->g_GraphDlgPage;
}

void FlushMouse(HWND hWnd)
{
	MSG msg;
	bool bPurged = false;

	while (!bPurged){
		if(PeekMessage(&msg, hWnd, WM_MOUSEFIRST, WM_MOUSEWHEEL, PM_REMOVE) == 0) 
			bPurged = true;
	}
}

bool GetGraphBk()
{
	return ((CXFLR5App*)AfxGetApp())->g_bGraphBk;
}

bool GetWindowBk()
{
	return ((CXFLR5App*)AfxGetApp())->g_bWindowBk;
}

bool GetPrintColor()
{
	return ((CXFLR5App*)AfxGetApp())->g_bColor;
}

void SetPrintOptions(bool bWindowBk, bool bGraphBk, bool bColor)
{
	((CXFLR5App*)AfxGetApp())->g_bWindowBk = bWindowBk;
	((CXFLR5App*)AfxGetApp())->g_bGraphBk  = bGraphBk;
	((CXFLR5App*)AfxGetApp())->g_bColor    = bColor;
}

/*
bool operator==(CVector V1, CVector V2)
{
	double d2 = (V1.x-V2.x)*(V1.x-V2.x) + (V1.y-V2.y)*(V1.y-V2.y) + (V1.z-V2.z)*(V1.z-V2.z);
	double L = V1.VAbs();
	double L2 = L*L;
	if(d2==0.0) return true;
	if(d2/L2 < 1/1000000.0) 
		return true;
	else 
		return false;
}*/


bool IsBetween(int f, int f1, int f2)
{
	if (f2 < f1){
		int tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(f<f1) return false;
	else if(f>f2) return false;
	return true;
}

bool IsBetween(int f, double f1, double f2)
{
	double ff = f;
	if (f2 < f1){
		double tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(ff<f1) return false;
	else if(ff>f2) return false;
	return true;
}

bool Intersect(int &x, int &y, CRect &DRect, CPoint &Pt1, CPoint &Pt2)
{
	int xt, yt;
	double x1 = (double)Pt1.x;
	double x2 = (double)Pt2.x;
	double y1 = (double)Pt1.y;
	double y2 = (double)Pt2.y;


	if (IsBetween(DRect.left, Pt1.x, Pt2.x)){
		yt = int(y1 + (y2-y1)/(x2-x1) * ((double)DRect.left-x1));
		if (IsBetween(yt, DRect.bottom, DRect.top)) {
			x = DRect.left;
			y = yt;
			return true;
		}
	}
	if (IsBetween(DRect.right, Pt1.x, Pt2.x)){
		yt = int(y1 + (y2-y1)/(x2-x1) * ((double)DRect.right-x1));
		if (IsBetween(yt, DRect.bottom, DRect.top)) {
			x = DRect.right;
			y = yt;
			return true;
		}
	}
	if (IsBetween(DRect.bottom, Pt1.y, Pt2.y)){
		xt = int(x1 + (x2-x1)/(y2-y1) * ((double)DRect.bottom-y1));
		if (IsBetween(xt, DRect.left, DRect.right)) {
			x = xt;
			y = DRect.bottom;
			return true;
		}
	}
	if (IsBetween(DRect.top, Pt1.y, Pt2.y)){
		xt = int(x1 + (x2-x1)/(y2-y1) * ((double)DRect.top-y1));
		if (IsBetween(xt, DRect.left, DRect.right)) {
			x = xt;
			y = DRect.top;
			return true;
		}
	}
	return false;
}



bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M)
{//ABCD are assumed to lie in the xy plane
	//returns true and intersection point M if AB and CD intersect inside
	//returns false and intersection point M if AB and CD intersect outside
	M->x = 0.0;
	M->y = 0.0;
	M->z = 0.0;
	CVector AB(B.x-A.x, B.y-A.y, B.z-A.z); 
	CVector CD(D.x-C.x, D.y-C.y, D.z-C.z); 

	//Cramer's rule

	double Det  = -AB.x * CD.y + CD.x * AB.y;
	if(Det==0.0) {//vectors are parallel, no intersection
		return false;
	}
	double Det1 = -(C.x-A.x)*CD.y + (C.y-A.y)*CD.x;
	double Det2 = -(C.x-A.x)*AB.y + (C.y-A.y)*AB.x;

	double t = Det1/Det;
	double u = Det2/Det;

	M->x = A.x + t*AB.x;
	M->y = A.y + t*AB.y;

	if (0.0<=t && t<=1.0 && 0.0<=u && u<=1.0)	return true;//M is between A and B
	else										return false;//M is outside
}




bool GaussSeidel (double *a, int MatSize, double *b, double *xk, double eps, int IterMax)
{
	int iter, i, j;
	double sum, max;
	double xk1[VLMMATSIZE];

	for (iter=0; iter<IterMax; iter++)
	{
		for (i=0; i<MatSize ; i++)
		{
			sum = b[i];
			for (j=0;   j<i;        j++)	sum -= a[i*MatSize +j]*xk1[j];
			for (j=i+1; j<MatSize ; j++)	sum -= a[i*MatSize +j]*xk[j];
			xk1[i] = sum/a[i*MatSize +i];
		}

		max = 0.0;
		for (i=0; i<MatSize ; i++) 
			if(abs(xk1[i]-xk[i])>max) max=abs(xk1[i]-xk[i]);

//		TRACE("%3d   %10.3e\n",iter, max);
		if(max<eps) return true;
		memcpy(xk, xk1, MatSize*sizeof(double));
	}
	return false;
}

CDoubleRect::CDoubleRect()
{
	left   = 0.0;
	right  = 0.0;
	top    = 0.0;
	bottom = 0.0;
}

bool CDoubleRect::PtInRect(double x, double y)
{
	if(x>=left && x<=right && y<=top && y>=bottom) return true;
	else return false;
}


bool CDoubleRect::PtInRect(CVector Real)
{
	if(Real.x>=left && Real.x<=right && Real.y<=top && Real.y>=bottom) return true;
	else return false;
}






