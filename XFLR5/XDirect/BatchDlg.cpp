/****************************************************************************

	Batch Analysis class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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
//

#include "stdafx.h"
#include "../main/MainFrm.h"
#include "../X-FLR5.h"
#include "BatchDlg.h"
#include "ReListDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog


CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchDlg)
	m_ReInc =  50000.0;
	m_ReMax = 300000.0;
	m_ReMin = 100000.0;
	m_Mach  = 0.0;
	m_SpInc = 0.5;
	m_SpMax = 1.0;
	m_SpMin = 0.0;
	//}}AFX_DATA_INIT

	m_IterRect.SetRect(327,158,620,398);

	m_RmsGraph.SetXTitle("Iter");
	m_RmsGraph.SetYTitle("");//Change from bl newton system solution

	m_RmsGraph.SetAuto(true);

	m_RmsGraph.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_RmsGraph.SetYMajGrid(true, RGB(120,120,120),2,1);

	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax(50);
	m_RmsGraph.SetYMin(-1.0);
	m_RmsGraph.SetYMax( 1.0);
	m_RmsGraph.SetType(1);
	m_RmsGraph.SetMargin(40);


	m_pXDirect      = pParent;
	m_bAlpha       = true;
	m_bFromList    = false;
	m_bFromZero    = false;
	m_bShowTextOutput = true;
	m_pBatchThread = NULL;
	m_pXFoil       = NULL;
	m_Iterations = 0;
	pi = 3.141592654;
}


void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchDlg)
	DDX_Control(pDX, IDC_ITERGRAPH, m_ctrlIterGraph);
	DDX_Control(pDX, IDC_SKIPPOLAR, m_ctrlSkipPolar);
	DDX_Control(pDX, IDC_SKIPPOINT, m_ctrlSkipPoint);
	DDX_Control(pDX, IDC_FROMZERO, m_ctrlFromZero);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlList);
	DDX_Control(pDX, IDC_RADIO1, m_ctrlRange);
	DDX_Control(pDX, IDC_CREATELIST, m_ctrlCreateList);
	DDX_Control(pDX, IDC_XTRIP2, m_ctrlXBotTr);
	DDX_Control(pDX, IDC_XTRIP1, m_ctrlXTopTr);
	DDX_Control(pDX, IDC_NCRIT, m_ctrlNCrit);
	DDX_Control(pDX, IDC_REINC, m_ctrlReInc);
	DDX_Control(pDX, IDC_ACL1, m_ctrlAcl1);
	DDX_Control(pDX, IDC_ACL2, m_ctrlAcl2);
	DDX_Control(pDX, IDC_INITBL, m_ctrlInitBL);
	DDX_Control(pDX, IDC_CLOSE, m_ctrlClose);
	DDX_Control(pDX, IDC_ANALYSIS, m_ctrlAnalysis);
	DDX_Control(pDX, IDC_SPMIN, m_ctrlSpMin);
	DDX_Control(pDX, IDC_SPMAX, m_ctrlSpMax);
	DDX_Control(pDX, IDC_SPINC, m_ctrlSpInc);
	DDX_Control(pDX, IDC_SPECTYP, m_ctrlSpecTyp);
	DDX_Control(pDX, IDC_RETYP, m_ctrlReTyp);
	DDX_Control(pDX, IDC_TYPE1, m_ctrlType);
	DDX_Control(pDX, IDC_REMIN, m_ctrlReMin);
	DDX_Control(pDX, IDC_REMAX, m_ctrlReMax);
	DDX_Control(pDX, IDC_OUTPUT, m_ctrlOutput);
	DDX_Control(pDX, IDC_SHOWTEXTOUTPUT, m_ctrlShowTextOutput);
	DDX_Control(pDX, IDC_MATYP, m_ctrlMaTyp);
	DDX_Control(pDX, IDC_MACH, m_ctrlMach);
	DDX_Text(pDX, IDC_REINC, m_ReInc);
	DDX_Text(pDX, IDC_REMAX, m_ReMax);
	DDX_Text(pDX, IDC_REMIN, m_ReMin);
	DDX_Text(pDX, IDC_MACH, m_Mach);
	DDX_Text(pDX, IDC_SPINC, m_SpInc);
	DDX_Text(pDX, IDC_SPMAX, m_SpMax);
	DDX_Text(pDX, IDC_SPMIN, m_SpMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchDlg)
	ON_BN_CLICKED(IDC_TYPE1, OnType1)
	ON_BN_CLICKED(IDC_ANALYSIS, OnAnalysis)
	ON_BN_CLICKED(IDC_ACL1, OnAcl)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_INITBL, OnInitBL)
	ON_BN_CLICKED(IDC_SHOWTEXTOUTPUT, OnShowTextOutput)
	ON_BN_CLICKED(IDC_RADIO2, OnRange)
	ON_BN_CLICKED(IDC_CREATELIST, OnCreateList)
	ON_BN_CLICKED(IDC_FROMZERO, OnFromZero)
	ON_BN_CLICKED(IDC_SKIPPOINT, OnSkipPoint)
	ON_BN_CLICKED(IDC_SKIPPOLAR, OnSkipPolar)
	ON_BN_CLICKED(IDC_ACL2, OnAcl)
	ON_BN_CLICKED(IDC_TYPE2, OnType1)
	ON_BN_CLICKED(IDC_TYPE3, OnType1)
	ON_BN_CLICKED(IDC_TYPE4, OnType1)
	ON_BN_CLICKED(IDC_RADIO1, OnRange)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg message handlers

BOOL CBatchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect WndRect;
	GetWindowRect(WndRect);
	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-WndRect.Width()-10,60,0,0,SWP_NOSIZE);
	
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;

	m_FoilName = pXDirect->m_pCurFoil->m_FoilName;
	CString str = "Batch analysis for "+ m_FoilName;
	SetWindowText(str);

	m_ctrlOutput.SetLimitText(100000);
	if(m_bShowTextOutput)	m_ctrlShowTextOutput.SetCheck(TRUE);
	else					m_ctrlShowTextOutput.SetCheck(FALSE);

	m_ctrlMach.SetPrecision(2);

	if(m_Type!=4){
		m_ctrlReMin.SetPrecision(0);
		m_ctrlReMax.SetPrecision(0);
		m_ctrlReInc.SetPrecision(0);

		m_ctrlSpMin.SetPrecision(1);
		m_ctrlSpMax.SetPrecision(1);
		m_ctrlSpInc.SetPrecision(1);
	}
	else{
		m_ctrlReMin.SetPrecision(1);
		m_ctrlReMax.SetPrecision(1);
		m_ctrlReInc.SetPrecision(1);

		m_ctrlSpMin.SetPrecision(0);
		m_ctrlSpMax.SetPrecision(0);
		m_ctrlSpInc.SetPrecision(0);
	}
	
	if(m_Type!=4){
		m_ctrlReMin.SetValue(m_ReMin);
		m_ctrlReMax.SetValue(m_ReMax);
		m_ctrlReInc.SetValue(m_ReInc);
		m_ctrlSpMin.SetValue(m_SpMin);
		m_ctrlSpMax.SetValue(m_SpMax);
		m_ctrlSpInc.SetValue(m_SpInc);
	}
	else{
		m_ctrlReMin.SetValue(m_SpMin);
		m_ctrlReMax.SetValue(m_SpMax);
		m_ctrlReInc.SetValue(m_SpInc);
		m_ctrlSpMin.SetValue(m_ReMin);
		m_ctrlSpMax.SetValue(m_ReMax);
		m_ctrlSpInc.SetValue(m_ReInc);
	}
	m_ctrlMach.SetValue(m_Mach);
	m_ctrlNCrit.SetValue(m_NCrit);
	m_ctrlXTopTr.SetValue(m_XTopTr);
	m_ctrlXBotTr.SetValue(m_XBotTr);

	if(m_bAlpha) CheckRadioButton(IDC_ACL1, IDC_ACL2, IDC_ACL1);
	else         CheckRadioButton(IDC_ACL1, IDC_ACL2, IDC_ACL2);
	OnAcl();

	CheckRadioButton(IDC_TYPE1,IDC_TYPE4, IDC_TYPE1+m_Type-1);
	OnType1();

	if(m_bFromList)  CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	else             CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	OnRange();

	if(m_bFromZero)  m_ctrlFromZero.SetCheck(1);
	else             m_ctrlFromZero.SetCheck(0);
	
	m_ctrlInitBL.SetCheck(TRUE);
	m_ctrlSkipPoint.EnableWindow(false);
	m_ctrlSkipPolar.EnableWindow(false);


	if (IsBlackAndWhite()) {
		m_RmsGraph.SetAxisColor(RGB(0,0,0));
		m_RmsGraph.SetBkColor(RGB(255,255,255));
		m_RmsGraph.SetLabelColor(RGB(0,0,0));
		m_RmsGraph.SetLegendColor(RGB(0,0,0));
	}

	CClientDC dc(this);

	LOGFONT LgFt; 
	ZeroMemory(&LgFt, sizeof(LOGFONT));
	LgFt.lfHeight = - MulDiv(10, dc.GetDeviceCaps(LOGPIXELSY), 72);
	strcpy(LgFt.lfFaceName,"Comic sans MS"); 
	m_RmsGraph.SetTitleLogFont(&LgFt);
	LgFt.lfHeight = - MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
	m_RmsGraph.SetLabelLogFont(&LgFt);

//	m_TitleFont.CreateFontIndirect(&LgFt);
	m_TitleFont.CreatePointFont(85, "Courier New");

	m_Iterations = 0;
	ResetCurves();
	if (IsBlackAndWhite()) {
		m_RmsGraph.SetAxisColor(RGB(0,0,0));
		m_RmsGraph.SetBkColor(RGB(255,255,255));
		m_RmsGraph.SetLabelColor(RGB(0,0,0));
		m_RmsGraph.SetLegendColor(RGB(0,0,0));
	}
	

	m_ctrlReMin.SetFocus();

	return TRUE;
}

void CBatchDlg::ResetCurves()
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
	m_RmsGraph.SetYMin(-1.0);
	m_RmsGraph.SetYMax( 1.0);
	m_RmsGraph.SetX0(0.0);
	m_RmsGraph.SetY0(0.0);
}


void CBatchDlg::SetFileHeader()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	CMainFrame * pFrame = (CMainFrame*)m_pFrame;
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pFrame->m_VersionName);
	m_XFile.WriteString("\n\n");
	m_XFile.WriteString(m_FoilName);
	m_XFile.WriteString("\n\n");

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



void CBatchDlg::OnType1() 
{
	int idc = GetCheckedRadioButton(IDC_TYPE1, IDC_TYPE4);
	switch(idc)	{
		case IDC_TYPE1:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE1);
			m_ctrlReTyp.SetWindowText("&Reynolds =");
			m_ctrlMaTyp.SetWindowText("&Mach =");
			m_ctrlList.EnableWindow(true);
			m_Type = 1;
			break;
		case IDC_TYPE2:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE2);
			m_ctrlReTyp.SetWindowText("&Re.sqrt(Cl) =");
			m_ctrlMaTyp.SetWindowText("&Ma.sqrt(Cl) =");
			m_ctrlList.EnableWindow(true);
			m_Type = 2;
			break;
		case IDC_TYPE3:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE3);
			m_ctrlReTyp.SetWindowText("&Re.Cl =");
			m_ctrlMaTyp.SetWindowText("&Mach =");
			m_ctrlList.EnableWindow(true);
			m_Type = 3;
			break;
		case IDC_TYPE4:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE4);
			m_ctrlReTyp.SetWindowText("&Alpha =");
			m_ctrlMaTyp.SetWindowText("&Mach =");
			m_ctrlList.EnableWindow(false);
			CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
			m_Type = 4;
			break;
		default:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE1);
			m_ctrlReTyp.SetWindowText("&Reynolds =");
			m_ctrlMaTyp.SetWindowText("&Mach =");
			m_ctrlList.EnableWindow(true);
			m_Type = 1;
			break;
	}
	m_ctrlReTyp.Invalidate();
	m_ctrlMaTyp.Invalidate();
	if(m_Type !=4){
		m_ctrlReMin.SetPrecision(0);
		m_ctrlReMax.SetPrecision(0);
		m_ctrlReInc.SetPrecision(0);
		m_ctrlSpMin.SetPrecision(2);
		m_ctrlSpMax.SetPrecision(2);
		m_ctrlSpInc.SetPrecision(2);
		m_ctrlAcl1.EnableWindow(true);
		m_ctrlAcl2.EnableWindow(true);
		OnAcl();

		m_ctrlReMin.SetValue(m_ReMin);
		m_ctrlReMax.SetValue(m_ReMax);
		m_ctrlReInc.SetValue(m_ReInc);
		
		m_ctrlSpMin.SetValue(m_SpMin);
		m_ctrlSpMax.SetValue(m_SpMax);
		m_ctrlSpInc.SetValue(m_SpInc);
	}
	else{
		m_ctrlReMin.SetPrecision(2);
		m_ctrlReMax.SetPrecision(2);
		m_ctrlReInc.SetPrecision(2);
		m_ctrlSpMin.SetPrecision(0);
		m_ctrlSpMax.SetPrecision(0);
		m_ctrlSpInc.SetPrecision(0);
		m_ctrlSpecTyp.SetWindowText("&Reynolds =");
		m_ctrlAcl1.EnableWindow(false);
		m_ctrlAcl2.EnableWindow(false);

		m_ctrlReMin.SetValue(m_SpMin);
		m_ctrlReMax.SetValue(m_SpMax);
		m_ctrlReInc.SetValue(m_SpInc);
		
		m_ctrlSpMin.SetValue(m_ReMin);
		m_ctrlSpMax.SetValue(m_ReMax);
		m_ctrlSpInc.SetValue(m_ReInc);
	}
}


void CBatchDlg::OnAcl() 
{
	if(m_Type==4) return;
	if(GetCheckedRadioButton(IDC_ACL1,IDC_ACL2)==IDC_ACL1){
		m_ctrlSpecTyp.SetWindowText("A&lpha =");
		m_ctrlSpMax.SetValue(1.f);
		m_ctrlSpInc.SetValue(0.5f);
		m_bAlpha = true;
		m_ctrlFromZero.EnableWindow(true);
	}
	else{
		m_ctrlSpecTyp.SetWindowText("C&l =");
		m_ctrlSpMax.SetValue(0.5f);
		m_ctrlSpInc.SetValue(0.1f);
		m_bAlpha = false;
		m_ctrlFromZero.EnableWindow(false);
	}	
}


void CBatchDlg::ReadParams()
{
	if(m_Type !=4){
		m_ReInc = m_ctrlReInc.GetValue();
		m_ReMax = m_ctrlReMax.GetValue();
		m_ReMin = m_ctrlReMin.GetValue();

		m_SpInc = m_ctrlSpInc.GetValue();
		m_SpMax = m_ctrlSpMax.GetValue();
		m_SpMin = m_ctrlSpMin.GetValue();
	}
	else{
		m_SpInc = m_ctrlReInc.GetValue();
		m_SpMax = m_ctrlReMax.GetValue();
		m_SpMin = m_ctrlReMin.GetValue();

		m_ReInc = m_ctrlSpInc.GetValue();
		m_ReMax = m_ctrlSpMax.GetValue();
		m_ReMin = m_ctrlSpMin.GetValue();
	}

	m_Mach     = m_ctrlMach.GetValue();
	m_NCrit    = m_ctrlNCrit.GetValue();
	m_XTopTr   = m_ctrlXTopTr.GetValue();
	m_XBotTr   = m_ctrlXBotTr.GetValue();
}



bool CBatchDlg::InitXFoil2()
{
	m_pXFoil->Initialize();
	m_pXFoil->reinf1 = m_ReMin;
	m_pXFoil->minf1  = m_Mach;
	switch (m_Type){
		case 1:
			m_pXFoil->retyp  = 1;
			m_pXFoil->matyp  = 1;
			break;
		case 2:
			m_pXFoil->retyp  = 2;
			m_pXFoil->matyp  = 2;
			break;
		case 3:
			m_pXFoil->retyp  = 3;
			m_pXFoil->matyp  = 1;
			break;
		case 4:
			m_pXFoil->retyp  = 1;
			m_pXFoil->matyp  = 1;
			break;
		default:
			m_pXFoil->retyp  = 1;
			m_pXFoil->matyp  = 1;
			break;
	}
		
	m_pXFoil->lalfa = true;
	m_pXFoil->qinf = 1.0;

	m_pXFoil->acrit      = m_NCrit;
	m_pXFoil->xstrip[1]  = m_XTopTr;
	m_pXFoil->xstrip[2]  = m_XBotTr;

	if (m_Mach !=0.f){
		if(!m_pXFoil->SetMach()){
			CString str;
			str.Format("Invalid Analysis Settings\nCPCALC: local speed too large \n Compressibility corrections invalid ");
			AfxMessageBox(str, MB_OK);
		}
	}
	
	for(int i =0; i<m_pCurFoil->n; i++){
		m_pXFoil->xb[i+1] = m_pCurFoil->x[i];
		m_pXFoil->yb[i+1] = m_pCurFoil->y[i];
	}
	m_pXFoil->nb     = m_pCurFoil->n;
	m_pXFoil->lbflap = false;
	m_pXFoil->ddef   = 0.0;
	m_pXFoil->xbf    = 1.0;
	m_pXFoil->ybf    = 0.0;
// end "load"

	if(m_pXFoil->Preprocess()){
		m_pXFoil->abcopy();
	}
	else{
			//what do I do now ?
	}
	return true;
}


void CBatchDlg::UpdateOutput(CString &str)
{
	if(m_bShowTextOutput)
		m_ctrlOutput.ReplaceSel(str);
}

void CBatchDlg::UpdateGraph(double Re, double Alpha)
{
	CClientDC dc(this); // device context for painting

	CString strong;
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	dcMem.SetBkMode(TRANSPARENT);

	CRect IterRect;
	m_ctrlIterGraph.GetWindowRect(IterRect);
	ScreenToClient(IterRect);

	CRect dcMemRect(0,0,IterRect.Width(), IterRect.Height());
	CBitmap bmb;
	bmb.CreateCompatibleBitmap(&dc,IterRect.Width(), IterRect.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(&bmb);

	CBrush FillBrush(GetSysColor(COLOR_3DFACE));
	dcMem.FillRect(&dcMemRect,&FillBrush);

	CRect BltRect(0,0,IterRect.Width(), IterRect.Height());
	m_RmsGraph.DrawGraph(&dcMem, &BltRect,false);

	if(Re>0.0){
		strong.Format("Re=%8.0f  /  Alpha=%5.2f", Re, Alpha*180.0/pi);
		CPoint Place(int((BltRect.left+BltRect.right)/2), BltRect.top+10);
		CFont *pOldFont = dcMem.SelectObject(&m_TitleFont);
		dcMem.SetTextAlign(TA_CENTER);
		dcMem.TextOut(Place.x, Place.y, strong);
		dcMem.SelectObject(pOldFont);
	}

	dc.BitBlt(IterRect.left, IterRect.top, IterRect.Width(), IterRect.Height(),
			  &dcMem,0, 0, SRCCOPY);

	dcMem.SelectObject(pOldBitmap);
	bmb.DeleteObject();
	FillBrush.DeleteObject();
}

BOOL CBatchDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();
		if(ppp==GetDlgItem(IDC_CLOSE)){
			EndDialog(1);
			return true;
		}
		else if (ppp != GetDlgItem(IDC_ANALYSIS))	{
			m_ctrlAnalysis.SetFocus();
			return true;
		}
		else if(ppp==GetDlgItem(IDC_ANALYSIS)){
			OnAnalysis();
			return true;
		}
		else m_ctrlAnalysis.SetFocus();
		return true;
	}
	else if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)){
		if(m_pBatchThread) {// stop thread immediately
			m_pBatchThread->m_bCancel = true;
			return true;
		}
		else{//thread is not running, so just PreTranslate normally and exit
		}
	}
	else if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == 'Z')){
			OnAnalysis();
			return true;
	}
/*	else if(pMsg->message == V_ENDTHREAD){// notification that thread has stopped
		m_ctrlClose.EnableWindow(true);
		m_ctrlSkipPoint.EnableWindow(false);
		m_ctrlSkipPolar.EnableWindow(false);
		m_ctrlAnalysis.SetWindowText("Analyze");
		return true; // no need to process further
	}*/
	return CDialog::PreTranslateMessage(pMsg);
}


void CBatchDlg::CleanUp()
{
	if(m_pBatchThread) m_pBatchThread = NULL;
	m_ctrlAnalysis.SetWindowText("Start Analysis");
	m_XFile.Close();
	m_ctrlClose.EnableWindow(true);
	m_ctrlSkipPoint.EnableWindow(false);
	m_ctrlSkipPolar.EnableWindow(false);
	m_ctrlAnalysis.SetWindowText("Analyze");
	m_ctrlClose.SetFocus();
}

void CBatchDlg::OnClose() 
{
	ReadParams();
	EndDialog(1);
}

void CBatchDlg::OnCancel() 
{
	if(!m_ctrlClose.IsWindowEnabled()) return;
	else CDialog::OnCancel();
}

void CBatchDlg::OutputIter(int iter, double Re, double Alpha)
{
	if(iter){
		m_RmsGraph.GetCurve(0)->AddPoint(iter, m_pXFoil->rmsbl);
		m_RmsGraph.GetCurve(1)->AddPoint(iter, m_pXFoil->rmxbl);
		UpdateGraph(Re, Alpha);
	}
}

void CBatchDlg::OnAnalysis() 
{
	if(!m_pBatchThread)
	{
		m_ctrlOutput.SetWindowText("");
		m_ctrlClose.EnableWindow(false);
		CString str;
		CString strAppDirectory;
		char    szAppPath[MAX_PATH] = "";
		GetTempPath(MAX_PATH,szAppPath);
		strAppDirectory = szAppPath;
		str =strAppDirectory + "XFLR5.log";
		BOOL bOpen = m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
		if(bOpen) m_pXFoil->pXFile = &m_XFile;
		else      m_pXFoil->pXFile = NULL;

		ReadParams();
		SetFileHeader();

		m_pBatchThread = new CBatchThread(this);
		m_pBatchThread->m_pXDirect      = (CXDirect*)m_pXDirect;
		m_pBatchThread->m_IterLim     = m_IterLim;
		m_pBatchThread->m_bAutoDelete = true;
		m_pBatchThread->m_pXFoil      = m_pXFoil;
		m_pBatchThread->m_pXFile      = &m_XFile; 
		m_pBatchThread->m_bFromList   = m_bFromList;
		m_pBatchThread->m_NRe         = m_NRe;
		m_pBatchThread->m_ReList      = m_ReList;
		m_pBatchThread->m_MachList    = m_MachList;
		m_pBatchThread->m_NCritList   = m_NCritList;
		m_pBatchThread->m_bShowTextOutput = m_bShowTextOutput;


		if (m_ctrlInitBL.GetCheck()) 
		{
			m_pBatchThread->m_bInitBL = true;
		}
		else 
		{
			m_pBatchThread->m_bInitBL = false;
		}
		if(m_Type!=4) 
			Analysis2();
		else
			Analysis3();
		m_pBatchThread->CreateThread(CREATE_SUSPENDED);
		VERIFY(m_pBatchThread->SetThreadPriority(THREAD_PRIORITY_LOWEST));

		m_pXFoil->lvisc = true;	
		m_pXFoil->m_bTrace = false;

		m_ctrlAnalysis.SetWindowText("Cancel");
		m_ctrlSkipPoint.EnableWindow(true);
		m_ctrlSkipPolar.EnableWindow(true);

		m_pBatchThread->ResumeThread();
		m_ctrlClose.SetFocus();
	}
	else
	{
		m_pBatchThread->Cancel();
	}
	SetFocus();
}


void CBatchDlg::Analysis2()
{
	m_pBatchThread->m_bType4 = false;
	m_pBatchThread->m_bAlpha = m_bAlpha;
	m_pBatchThread->m_bFromZero = m_bFromZero;

	if(m_bAlpha){
		m_pBatchThread->m_SpMin = m_SpMin *pi/180.;
		m_pBatchThread->m_SpMax = m_SpMax *pi/180.;

		if(m_SpMin< m_SpMax) m_pBatchThread->m_SpInc = abs(m_SpInc)*pi/180.;
		else m_pBatchThread->m_SpInc =                -abs(m_SpInc)*pi/180.;
	}
	else{
		m_pBatchThread->m_SpMin = m_SpMin ;
		m_pBatchThread->m_SpMax = m_SpMax ;
		if(m_SpMin< m_SpMax) m_pBatchThread->m_SpInc = abs(m_SpInc);
		else m_pBatchThread->m_SpInc =                -abs(m_SpInc);
	}
	m_pBatchThread->m_ReMin = m_ReMin;
	m_pBatchThread->m_ReMax = m_ReMax;

	if(m_ReMin < m_ReMax) m_pBatchThread->m_ReInc = abs(m_ReInc);
	else m_pBatchThread->m_ReInc =                 -abs(m_ReInc);
	InitXFoil2();
}


void CBatchDlg::Analysis3()
{
	m_pBatchThread->m_bType4 = true;
	m_pBatchThread->m_bAlpha = false;

	m_pBatchThread->m_SpMin = m_SpMin ;
	m_pBatchThread->m_SpMax = m_SpMax ;
	if(m_SpMin< m_SpMax)	m_pBatchThread->m_SpInc = abs(m_SpInc);
	else m_pBatchThread->m_SpInc =                   -abs(m_SpInc);

	m_pBatchThread->m_ReMin = m_ReMin;
	m_pBatchThread->m_ReMax = m_ReMax;
	
	if(m_ReMin < m_ReMax) m_pBatchThread->m_ReInc = abs(m_ReInc);
	else m_pBatchThread->m_ReInc =                 -abs(m_ReInc);

	InitXFoil2();
}

void CBatchDlg::OnInitBL()
{
	if (m_ctrlInitBL.GetCheck()) {
		if(m_pBatchThread) m_pBatchThread->m_bInitBL = true;
	}
	else {
		if(m_pBatchThread) m_pBatchThread->m_bInitBL = false;
	}
}

void CBatchDlg::OnShowTextOutput()
{
	if (m_ctrlShowTextOutput.GetCheck()) {
		m_bShowTextOutput = true;
		if(m_pBatchThread) m_pBatchThread->m_bShowTextOutput = true;
	}
	else {
		m_bShowTextOutput = false;
		if(m_pBatchThread) m_pBatchThread->m_bShowTextOutput = false;
	}
}

void CBatchDlg::OnRange() 
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1)
		m_bFromList = false;
	else 
		m_bFromList = true;
	
	m_ctrlCreateList.EnableWindow(m_bFromList);
	m_ctrlReMin.EnableWindow(!m_bFromList);
	m_ctrlReMax.EnableWindow(!m_bFromList);
	m_ctrlReInc.EnableWindow(!m_bFromList);
}


void CBatchDlg::OnCreateList() 
{
	CReListDlg dlg(this);
	for (int i=0; i<m_NRe; i++){
		dlg.m_ReList[i]    = m_ReList[i];
		dlg.m_MachList[i]  = m_MachList[i];
		dlg.m_NCritList[i] = m_NCritList[i];
	}
	dlg.m_NRe = m_NRe;

	if(IDOK == dlg.DoModal()){
		for (int i=0; i<dlg.m_NRe; i++){
			m_ReList[i]    = dlg.m_ReList[i];
			m_MachList[i]  = dlg.m_MachList[i];
			m_NCritList[i] = dlg.m_NCritList[i];
		}
		m_NRe = dlg.m_NRe;
	}
}


void CBatchDlg::OnFromZero() 
{
	if(m_ctrlFromZero.GetCheck()) m_bFromZero = true;
	else                          m_bFromZero = false;
}


void CBatchDlg::OnSkipPoint() 
{
	m_pBatchThread->m_bSkipPoint = true;
}


void CBatchDlg::OnSkipPolar() 
{
	m_pBatchThread->m_bSkipPolar = true;	
}


void CBatchDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
	int nSavedDC = pDC->SaveDC();
	if(nIDCtl==IDC_ITERGRAPH){
		UpdateGraph(-100.0,0.);
	}
	pDC->RestoreDC(nSavedDC);

//	CDialog::OnDrawItem(nIDCt, lpDrawItemStruct);}
}
