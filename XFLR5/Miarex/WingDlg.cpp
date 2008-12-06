f/****************************************************************************

    Wing Class
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

//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../misc/UnitsDlg.h"
#include "Miarex.h"
#include  <math.h>
#include ".\wingdlg.h"

double CWingDlg::m_DTChord[MAXPANELS+1];	// Chords used for display, with converted units
double CWingDlg::m_DTOffset[MAXPANELS+1];	// Offset used for display, with converted units
double CWingDlg::m_DTPos[MAXPANELS+1];	// Panel positions used for display, with converted units
CWnd* CWingDlg::s_pFrame;		//pointer to the Frame window
CWnd* CWingDlg::s_pMiarex;	//pointer to the Miarex Application window


CWingDlg::CWingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWingDlg::IDD, pParent)
{
	m_bTrans        = false;
	m_pWing->m_bVLMAutoMesh  = true;
//	m_pWing->m_bSymetric     = true;
	m_bTrace        = true;
	m_bCheckName    = true;
	m_bRightSide    = true;
	m_bChanged      = false;
	m_pXFile     = NULL;

	m_pWing = NULL;

	m_FixedFont.CreatePointFont(85, "Courier New");

	m_ptOffset.x = 50;
	m_ptOffset.y = 350;
	m_fWingScale = -1.0;

	m_iSection   = 0;

	pi = 3.141592654;

	m_PointDown.SetPoint(0,0);
	m_DrawRect.SetRect(0,0,0,0);

//	m_pWing->m_RFoil.SetSize(MAXPANELS+1);
//	m_pWing->m_LFoil.SetSize(MAXPANELS+1);
}


CWingDlg::CWingDlg(CWnd* pParent /*=NULL*/, CWing* pWing/*=NULL*/)
	: CDialog(CWingDlg::IDD, pParent)
{
	m_bTrans        = false;
//	m_pWing->m_bVLMAutoMesh  = true;
//	m_pWing->m_bSymetric     = true;
	m_bTrace        = true;
	m_bCheckName    = true;
	m_bRightSide    = true;
	m_bChanged      = false;
	m_pXFile     = NULL;

	m_pWing = pWing;

	m_FixedFont.CreatePointFont(85, "Courier New");

	m_ptOffset.x = 50;
	m_ptOffset.y = 350;
	m_fWingScale = -1.0;

	m_iSection   = 0;

	pi = 3.141592654;

	m_PointDown.SetPoint(0,0);
	m_DrawRect.SetRect(0,0,0,0);

//	m_pWing->m_RFoil.SetSize(MAXPANELS+1);
//	m_pWing->m_LFoil.SetSize(MAXPANELS+1);
}


CWingDlg::~CWingDlg()
{
}

void CWingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SWEEP, m_ctrlSweep);
	DDX_Control(pDX, IDC_VLMMESH, m_ctrlVLMMesh);
	DDX_Control(pDX, IDC_LENGTH5, m_ctrlLength5);
	DDX_Control(pDX, IDC_YMAC, m_ctrlYMac);
	DDX_Control(pDX, IDC_SIDE1, m_ctrlLeftWing);
	DDX_Control(pDX, IDC_SYMETRIC, m_ctrlSymetric);
	DDX_Control(pDX, IDC_PANELLIST, m_ctrlPanelList);
	DDX_Control(pDX, IDC_AREA, m_ctrlArea);
	DDX_Control(pDX, IDC_VOLUMEUNIT, m_ctrlVolumeUnit);
	DDX_Control(pDX, IDC_LENGTH3, m_ctrlLength3);
	DDX_Control(pDX, IDC_LENGTH2, m_ctrlLength2);
	DDX_Control(pDX, IDC_LENGTH1, m_ctrlLength1);
	DDX_Control(pDX, IDC_AEROCHORD, m_ctrlAeroChord);
	DDX_Control(pDX, IDC_WINGNAME, m_ctrlWingName);
	DDX_Control(pDX, IDC_SPAN, m_ctrlSpan);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_MEANCHORD, m_ctrlMeanChord);
	DDX_Control(pDX, IDC_TAPERRATIO, m_ctrlTaperRatio);
	DDX_Control(pDX, IDC_SURFACE, m_ctrlSurface);
	DDX_Control(pDX, IDC_VOLUME, m_ctrlVolume);
	DDX_Control(pDX, IDC_WINGCOLOR, m_ctrlWingColor);
	DDX_Control(pDX, IDC_ASPECTRATIO, m_ctrlAspectRatio);
	DDX_Control(pDX, IDC_VLMPANELS, m_ctrlVLMPanels);
	DDX_Control(pDX, IDC_TOTAL3DPANELS, m_ctrlTotal3DPanels);
}


BEGIN_MESSAGE_MAP(CWingDlg, CDialog)
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_COMMAND(IDM_INSERTBEFORE, OnInsertBefore)
	ON_COMMAND(IDM_DELETEINPUT, OnDeleteInput)
	ON_BN_CLICKED(IDC_WINGCOLOR, OnWingColor)
	ON_BN_CLICKED(IDC_SYMETRIC, OnSymetric)
	ON_BN_CLICKED(IDC_SIDE1, OnSide)
	ON_BN_CLICKED(IDC_VLMMESH, OnVLMMesh)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDM_APPEND, OnAppend)
	ON_BN_CLICKED(IDC_SIDE2, OnSide)
//	ON_NOTIFY(NM_RCLICK, IDC_PANELLIST, OnNMRClickPanelList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PANELLIST, OnLvnItemchangedPanellist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PANELLIST, OnLvnEndLabelEditPanelList)
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_RESETSCALE, OnResetscale)
END_MESSAGE_MAP()



BOOL CWingDlg::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWnd = GetFocus();
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		if(&m_ctrlVLMPanels == pWnd)
		{
			OnUpdateMeshFromTotal();
			return true;
		}
		if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd)
		{
			m_ctrlOK.SetFocus();
			ReadParams();
			ComputeGeometry();
			SetResults();
			InvalidateRect(&m_DrawRect);
		}
		else if(GetDlgItem(IDOK) == pWnd) OnOK();
		else if(GetDlgItem(IDCANCEL) == pWnd) OnCancel();

		return true;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE
		     && pWnd == &m_ctrlPanelList)
	{
		OnDeleteInput();
		return true;

	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_INSERT
		     && pWnd == &m_ctrlPanelList)
	{
		OnInsertBefore();
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CWingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	if(m_pWing->m_bIsFin) SetWindowText("Fin Design");
	else         SetWindowText("Wing Design");

	m_bRightSide = true;

	m_ctrlWingColor.SetColor(m_pWing->m_WingColor);

	CString len, surf;
	GetLengthUnit(len,pFrame->m_LengthUnit);
	GetAreaUnit(surf, pFrame->m_AreaUnit);
	
	m_ctrlLength1.SetWindowText(len);
	m_ctrlLength2.SetWindowText(len);
	m_ctrlLength3.SetWindowText(len);
	m_ctrlLength5.SetWindowText(len);
	m_ctrlArea.SetWindowText(surf);
	m_ctrlVolumeUnit.SetWindowText(len+"3");

	CFoil*pFoil= NULL;

	m_ctrlWingName.SetWindowText(m_pWing->m_WingName);
	if(!m_bCheckName) m_ctrlWingName.EnableWindow(false);
	m_ctrlWingName.SetSel(0,-1);
	m_ctrlWingName.SetFocus();

	CheckRadioButton(IDC_SIDE1, IDC_SIDE2, IDC_SIDE2);
	if(m_pWing->m_bSymetric) m_ctrlSymetric.SetCheck(TRUE);
	OnSymetric();

	ComputeGeometry();

	bool bResetMesh = false;
	for (i=0;i<m_pWing->m_NPanel; i++){
		if(m_pWing->m_NXPanels[i]>MAXCHORDPANELS || m_pWing->m_NXPanels[i]<=0) 
			bResetMesh = true;
	}
	if(bResetMesh || m_pWing->m_bVLMAutoMesh) VLMSetAutoMesh();


	int exp = (int)log10(m_pWing->m_Span  * pFrame->m_mtoUnit);
	int pres = 4-exp;
	pres = max(pres, 2);

	CRect CltRect;

	len =" ("+len+")";
	m_ctrlPanelList.GetStyle();
	m_ctrlPanelList.GetClientRect(&CltRect);
	int w = CltRect.Width();
	int w1 = (int)((w-25)/11);
	m_ctrlPanelList.InsertColumn(0," ",LVCFMT_RIGHT, 15);
	m_ctrlPanelList.InsertColumn(1,"Pos."+len,LVCFMT_RIGHT, w1);
	m_ctrlPanelList.InsertColumn(2,"Chord"+len,LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(3,"Offset"+len,LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(4,"Dihedral (°)",LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(5,"Twist (°)",LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(6,"FoilName",LVCFMT_LEFT,2*w1);
	m_ctrlPanelList.InsertColumn(7,"X-Panels",LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(8,"X-Dist",LVCFMT_LEFT,w1);
	m_ctrlPanelList.InsertColumn(9,"Y-Panels",LVCFMT_RIGHT,w1);
	m_ctrlPanelList.InsertColumn(10,"Y-Dist",LVCFMT_LEFT,w1);
	m_ctrlPanelList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlPanelList.m_nColumns = 11;
	m_ctrlPanelList.m_pWingDlg = this;
	

	m_ctrlPanelList.m_strList.RemoveAll();

	for(i=0; i<pFrame->m_oaFoil.GetSize(); i++)
	{
		pFoil = (CFoil*)pFrame->m_oaFoil.GetAt(i);
		m_ctrlPanelList.m_strList.Add(pFoil->m_FoilName);
	}

	m_iSection = 0;
	SetSectionData();

	SetParams();
	SetResults();

	GetClientRect(&CltRect);
	
	m_DrawRect.left   = CltRect.left+20;
	m_DrawRect.right  = CltRect.right-20;
	m_DrawRect.top    = CltRect.top+280;
	m_DrawRect.bottom = CltRect.bottom-20;

	m_ptOffset.x = 50;
	m_ptOffset.y = 330;

	SetScale();

//	if(m_pWing->m_bIsFin)	m_ctrlSymetric.EnableWindow(false);
	

	m_bChanged = false;

	m_ctrlPanelList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ctrlPanelList.SetFocus();

	return FALSE; 
}



void CWingDlg::OnResetscale()
{
	//Resets the wing's display scale

	m_ptOffset.x = 50;
	m_ptOffset.y = 330;
	SetScale();
	InvalidateRect(&m_DrawRect);
}


void CWingDlg::SetResults()
{
	//Updates the wing's properties after a change of geometry

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CString str;

	str.Format("%7.2f", m_pWing->m_Area*pFrame->m_m2toUnit);
	m_ctrlSurface.SetWindowText(str);

	str.Format("%5.2e", m_pWing->m_Volume*pFrame->m_mtoUnit*pFrame->m_mtoUnit*pFrame->m_mtoUnit);
	m_ctrlVolume.SetWindowText(str);

	str.Format("%5.2f", m_pWing->m_Span*pFrame->m_mtoUnit);
	m_ctrlSpan.SetWindowText(str);

	str.Format("%5.2f", m_pWing->m_GChord*pFrame->m_mtoUnit);
	m_ctrlMeanChord.SetWindowText(str);

	str.Format("%5.2f", m_pWing->m_MAChord*pFrame->m_mtoUnit);
	m_ctrlAeroChord.SetWindowText(str);

	str.Format("%5.2f", m_yMac*pFrame->m_mtoUnit);
	m_ctrlYMac.SetWindowText(str);

	str.Format("%5.2f", m_pWing->m_AR);
	m_ctrlAspectRatio.SetWindowText(str);

	if(m_pWing->m_TChord[m_pWing->m_NPanel]>0.0) str.Format("%.2f", m_pWing->m_TR);
	else str.Format("Undefined");
	m_ctrlTaperRatio.SetWindowText(str);

	str.Format("%5.2f", GetAverageSweep());
	m_ctrlSweep.SetWindowText(str);

	int VLMTotal = VLMGetPanelTotal();
	m_ctrlVLMPanels.SetValue(VLMTotal);

	VLMTotal *= 2;
	VLMTotal += 2*m_pWing->m_NXPanels[m_pWing->m_NPanel-1];
	str.Format("Total 3D Panels = %4d   (Max is 2000)", VLMTotal);
	m_ctrlTotal3DPanels.SetWindowText(str);
}


void CWingDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Message processing for a left-click

	if(m_DrawRect.PtInRect(point))
	{
		int res = IsFoil(point);

		if(res>=0)
		{
			m_iSection = res;
			m_ctrlPanelList.SetItemState(res, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			m_ctrlPanelList.SetFocus();
			InvalidateRect(&m_DrawRect);
			return;
		}
		else SetFocus();
		m_PointDown.x = point.x;
		m_PointDown.y = point.y;

		m_bTrans = true;
	}
	else 
	{
		m_bTrans = false;
		CDialog::OnLButtonDown(nFlags, point);
	}
}



void CWingDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Display the context menu

	CPoint pt = point;
	ScreenToClient(&pt);
	CRect rect;
	m_ctrlPanelList.GetWindowRect(&rect);

	if(m_DrawRect.PtInRect(pt))
	{
		int res = IsFoil(pt);

		m_ctrlPanelList.CloseEdit(); 
		if(res>=0)
		{
			m_iSection = res;
		}
		if(m_iSection>=0)
		{

			m_ctrlPanelList.SetItemState(res, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			InvalidateRect(&m_DrawRect);

			CMenu menu;
			if (menu.LoadMenu(IDR_CTXWNGMENU))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					point.x, point.y, this); 
			}

		}
	}
	if(rect.PtInRect(point))
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXWNGMENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this); 
		}

	}
}


void CWingDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	//Message processing for a mouse movement

	SHORT shZ  = GetKeyState(90);

	if(m_bTrans)
	{
		// we translate the wing
		m_ptOffset.x += point.x - m_PointDown.x;
		m_ptOffset.y += point.y - m_PointDown.y;

		m_PointDown = point;
		InvalidateRect(&m_DrawRect);
	}
	else if ((nFlags & MK_MBUTTON) ||(shZ & 0x8000)) 
	{
		// we zoom the graph or the wing	
		if(point.y-m_PointDown.y>0) 
			m_fWingScale *=1.06;
		else 
			m_fWingScale /= 1.06;
		InvalidateRect(&m_DrawRect);

	}
	m_PointDown = point;

//	CDialog::OnMouseMove(nFlags, point);
}

void CWingDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//the user has released the left button
	m_bTrans = false;	
//	Invalidate();
//	CDialog::OnLButtonUp(nFlags, point);
}


void CWingDlg::OnOK() 
{
	// the user has clicked the OK button
	ReadParams(); 
	if(!CheckWing()) return;
	m_ctrlWingName.GetWindowText(m_pWing->m_WingName);
	
	if(m_pWing->m_bSymetric){
		for (int i=0; i<=m_pWing->m_NPanel; i++)
		{
			m_pWing->m_LFoil[i]   = m_pWing->m_RFoil[i];	
		}
	}

	m_pWing->m_bVLMAutoMesh = false;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	m_pWing->ComputeGeometry();
	CDialog::OnOK();
}


void CWingDlg::InsertSection(double TPos)
{
	// Inserts a section before an existing sections
	// If the section already exists, does nothing and returns
	// Otherwise copies the data from section n+1
	// and inserts a mid panel

	if(TPos<=0.0) return;

	bool bIsInserted = false;
	
	for (int n=1; n<=m_pWing->m_NPanel; n++){
		if(abs(TPos-m_pWing->m_TPos[n])<0.0000001) {
			break;
		}
		if(TPos < m_pWing->m_TPos[n]){
			for (int m=m_pWing->m_NPanel;m>=n; m--){
				m_pWing->m_TPos[m+1]      = m_pWing->m_TPos[m];
				m_pWing->m_TChord[m+1]    = m_pWing->m_TChord[m];
				m_pWing->m_TOffset[m+1]   = m_pWing->m_TOffset[m];
				m_pWing->m_TZPos[m+1]     = m_pWing->m_TZPos[m];
				m_pWing->m_TDihedral[m+1] = m_pWing->m_TDihedral[m];
				m_pWing->m_TTwist[m+1]    = m_pWing->m_TTwist[m];
				m_pWing->m_RFoil[m+1]     = m_pWing->m_RFoil[m];
				m_pWing->m_LFoil[m+1]     = m_pWing->m_LFoil[m];
				m_pWing->m_NXPanels[m+1]  = m_pWing->m_NXPanels[m];
				m_pWing->m_NYPanels[m+1]  = m_pWing->m_NYPanels[m];
				m_pWing->m_XPanelDist[m+1] = m_pWing->m_XPanelDist[m];
				m_pWing->m_YPanelDist[m+1] = m_pWing->m_YPanelDist[m];
			}
			double tau = (TPos-m_pWing->m_TPos[n-1])/(m_pWing->m_TPos[n]-m_pWing->m_TPos[n-1]);
			m_pWing->m_TPos[n]      = TPos;
			m_pWing->m_TChord[n]    = m_pWing->m_TChord[n-1]    * (1-tau) + m_pWing->m_TChord[n]    * tau;
			m_pWing->m_TOffset[n]   = m_pWing->m_TOffset[n-1]   * (1-tau) + m_pWing->m_TOffset[n]   * tau;
			m_pWing->m_TZPos[n]     = m_pWing->m_TZPos[n-1]     * (1-tau) + m_pWing->m_TZPos[n]     * tau;
			m_pWing->m_TTwist[n]    = m_pWing->m_TTwist[n-1]    * (1-tau) + m_pWing->m_TTwist[n]    * tau;
			m_pWing->m_TDihedral[n] = m_pWing->m_TDihedral[n-1] * (1-tau) + m_pWing->m_TDihedral[n] * tau;
			
			m_pWing->m_RFoil[n]     = m_pWing->m_RFoil[n-1];
			m_pWing->m_LFoil[n]     = m_pWing->m_LFoil[n-1];
			m_pWing->m_NXPanels[n]  = m_pWing->m_NXPanels[n-1];
			m_pWing->m_NYPanels[n]  = m_pWing->m_NYPanels[n-1];
			m_pWing->m_XPanelDist[n] = m_pWing->m_XPanelDist[n-1];
			m_pWing->m_YPanelDist[n] = m_pWing->m_YPanelDist[n-1];
			bIsInserted  = true;
			m_pWing->m_NPanel++;
			break;
		}
	}
}


void CWingDlg::InsertSection(double TPos, double TChord, double TOffset,
						  double TZPos, double Twist, CString Foil,
						  int NChord, int NSpan, int SSpan)
{
	// Inserts a section before an existing sections
	// If the section already exists, does nothing and returns
	// Otherwise copies the data from section n+1
	// and inserts a mid panel

	int n, m;
	bool bIsInserted = false;
	
	for (n=0; n<=m_pWing->m_NPanel; n++){
		if(abs(TPos-m_pWing->m_TPos[n])<0.0000001) {

			m_pWing->m_NXPanels[n]  = NChord;
			m_pWing->m_NYPanels[n]  = NSpan;
			m_pWing->m_XPanelDist[n] = 1;
			m_pWing->m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			break;
		}
		if(TPos < m_pWing->m_TPos[n])
		{
			for (m=m_pWing->m_NPanel;m>=n; m--)
			{
				m_pWing->m_TPos[m+1]      = m_pWing->m_TPos[m];
				m_pWing->m_TChord[m+1]    = m_pWing->m_TChord[m];
				m_pWing->m_TOffset[m+1]   = m_pWing->m_TOffset[m];
				m_pWing->m_TZPos[m+1]     = m_pWing->m_TZPos[m];
				m_pWing->m_TDihedral[m+1] = m_pWing->m_TDihedral[m];
				m_pWing->m_TTwist[m+1]    = m_pWing->m_TTwist[m];
				m_pWing->m_RFoil[m+1]     = m_pWing->m_RFoil[m];
				m_pWing->m_LFoil[m+1]     = m_pWing->m_LFoil[m];
				m_pWing->m_NXPanels[m+1]  = m_pWing->m_NXPanels[m];
				m_pWing->m_NYPanels[m+1]  = m_pWing->m_NYPanels[m];
				m_pWing->m_XPanelDist[m+1] = m_pWing->m_XPanelDist[m];
				m_pWing->m_YPanelDist[m+1] = m_pWing->m_YPanelDist[m];
			}
			m_pWing->m_TPos[n]      = TPos;
			m_pWing->m_TChord[n]    = TChord;
			m_pWing->m_TOffset[n]   = TOffset;
			m_pWing->m_TZPos[n]     = TZPos;
			m_pWing->m_TDihedral[n] = 180.0/pi *
							 atan2(m_pWing->m_TZPos[n+1]-m_pWing->m_TZPos[n], m_pWing->m_TPos[n+1]-m_pWing->m_TPos[n]);
			m_pWing->m_TTwist[n]    = Twist;
			m_pWing->m_RFoil[n]     = Foil;
			m_pWing->m_LFoil[n]     = Foil;
			m_pWing->m_NXPanels[n]  = NChord;
			m_pWing->m_NYPanels[n]  = NSpan;
			m_pWing->m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			m_pWing->m_NPanel++;
			break;
		}
	}
	if(!bIsInserted)
	{
		if(TPos > m_pWing->m_TPos[m_pWing->m_NPanel]){
			n = m_pWing->m_NPanel +1;
			m_pWing->m_TPos[n]    = TPos;
			m_pWing->m_TChord[n]  = TChord;
			m_pWing->m_TOffset[n] = TOffset;
			m_pWing->m_TZPos[n]   = TZPos;
			m_pWing->m_TDihedral[n-1] = 180.0/pi *
							   atan2(m_pWing->m_TZPos[n]-m_pWing->m_TZPos[n-1], m_pWing->m_TPos[n]-m_pWing->m_TPos[n-1]);
			m_pWing->m_TTwist[n]  = Twist;
			m_pWing->m_RFoil[n]   = Foil;
			m_pWing->m_LFoil[n]   = Foil;
			m_pWing->m_NXPanels[n]  = NChord;
			m_pWing->m_NYPanels[n]  = NSpan;
			m_pWing->m_YPanelDist[n] = SSpan;
			m_pWing->m_NPanel++;
		}
		else {//no need to overwrite
		}
	}
}


double CWingDlg::IntegralC2(double y1, double y2, double c1, double c2)
{
	// Auxiliary integral used in LLT calculations

	double res = 0.0;

	if (abs(y2-y1)<1.e-5) return 0.0;
	double g = (c2-c1)/(y2-y1);

	res = (c1-g*y1)*(c1-g*y1)*(y2-y1) +
		  g * (c1-g*y1)      *(y2*y2-y1*y1)+
		  g*g/3.0            *(y2*y2*y2-y1*y1*y1);

	return res;
}


double CWingDlg::IntegralCy(double y1, double y2, double c1, double c2)
{
	// Auxiliary integral used in LLT calculations

	double res = 0.0;

	if (abs(y2-y1)<1.e-5) return (y1+y2)/2.0 * (c1+c2)/2.0;

	double g = (c2-c1)/(y2-y1);

	res = (c1-g*y1)/2.0 *(y2*y2 - y1*y1) + g/3.0 * (y2*y2*y2-y1*y1*y1);

	return res;
}


void CWingDlg::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)s_pMiarex;
	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	int i, k;

	double surface = 0.0;
	m_pWing->m_TLength[0] = 0.0;
	m_pWing->m_TYProj[0]  = m_pWing->m_TPos[0];
	for (i=1; i<=m_pWing->m_NPanel; i++){
		m_pWing->m_TLength[i] = m_pWing->m_TPos[i] - m_pWing->m_TPos[i-1];
	}
	for (i=1; i<=m_pWing->m_NPanel; i++){
		m_pWing->m_TYProj[i] = m_pWing->m_TYProj[i-1] + m_pWing->m_TLength[i] * cos(m_pWing->m_TDihedral[i-1]*pi/180.0);
	}

	m_pWing->m_Span    = 2.0 * m_pWing->m_TPos[m_pWing->m_NPanel]; 
	m_pWing->m_MAChord = 0.0;
	m_yMac    = 0.0;
	m_pWing->m_Volume  = 0.0;
	for (k=0; k<m_pWing->m_NPanel; k++)
	{
		pFoilA = pFrame->GetFoil(m_pWing->m_RFoil[k]);
		pFoilB = pFrame->GetFoil(m_pWing->m_RFoil[k+1]);
		surface   += m_pWing->m_TLength[k+1]*(m_pWing->m_TChord[k]+m_pWing->m_TChord[k+1])/2.0;//m²
		if(pFoilA && pFoilB) 
			m_pWing->m_Volume  += m_pWing->m_TLength[k+1]*(pFoilA->GetArea()*m_pWing->m_TChord[k] + pFoilB->GetArea()*m_pWing->m_TChord[k+1])/2.0;//m3
		m_pWing->m_MAChord += IntegralC2(m_pWing->m_TPos[k], m_pWing->m_TPos[k+1], m_pWing->m_TChord[k], m_pWing->m_TChord[k+1]);
		m_yMac    += IntegralCy(m_pWing->m_TPos[k], m_pWing->m_TPos[k+1], m_pWing->m_TChord[k], m_pWing->m_TChord[k+1]);
	}
	if(!m_pWing->m_bIsFin || m_pWing->m_bSymFin || m_pWing->m_bDoubleFin)
	{
		m_pWing->m_Area    = 2.0 * surface;
		m_pWing->m_Volume *= 2.0;
		m_pWing->m_MAChord = m_pWing->m_MAChord * 2.0 / m_pWing->m_Area;
		m_yMac             = m_yMac             * 2.0 / m_pWing->m_Area;

		m_pWing->m_GChord  = m_pWing->m_Area/m_pWing->m_Span;
		m_pWing->m_AR      = m_pWing->m_Span*m_pWing->m_Span/m_pWing->m_Area;
	}
	else
	{ 
		m_pWing->m_Area = surface;
		m_pWing->m_MAChord = m_pWing->m_MAChord / m_pWing->m_Area;
		m_yMac    = m_yMac    / m_pWing->m_Area;

		m_pWing->m_GChord  = m_pWing->m_Area/m_pWing->m_Span*2.0;
		m_pWing->m_AR      = m_pWing->m_Span*m_pWing->m_Span/m_pWing->m_Area/2.0;
	}

	if(m_pWing->m_TChord[m_pWing->m_NPanel]>0.0)	m_pWing->m_TR = m_pWing->m_TChord[0]/m_pWing->m_TChord[m_pWing->m_NPanel];
	else						m_pWing->m_TR = 99999.0;

	//calculate the number of flaps
	m_pWing->m_nFlaps = 0;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span;

	for (i=1; i<=m_pWing->m_NPanel; i++)
	{
		pFoilA = pFrame->GetFoil(m_pWing->m_RFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_pWing->m_RFoil[i]);
		if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize){
			m_pWing->m_nFlaps++;
		}
		pFoilA = pFrame->GetFoil(m_pWing->m_LFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_pWing->m_LFoil[i]);
		if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize){
			m_pWing->m_nFlaps++;
		}
	}

}



void CWingDlg::SetScale()
{
	// Sets the wing scale for diplay in the wing edition dialog box

	double sc1 = 2.0*(m_DrawRect.Width()-130)/m_pWing->m_Span;
	double sc2 = (m_DrawRect.Height()-90)/m_pWing->m_TChord[0];
	double sc3 = (m_DrawRect.Height()-90)/(m_pWing->m_TChord[m_pWing->m_NPanel]+m_pWing->m_TOffset[m_pWing->m_NPanel]);
	m_fWingScale = __min(sc1, sc2);
	m_fWingScale = __min(m_fWingScale, sc3);
}


BOOL CWingDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
//	Message processing for a mouse wheel

//	point is in client coordinates
	if(zDelta<0) m_fWingScale *= 1.06;
	else m_fWingScale /= 1.06;

	InvalidateRect(&m_DrawRect);
	return TRUE;
		
//	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}



void CWingDlg::OnSetFocus(CWnd* pOldWnd) 
{
//	Message processing when the edition dialog box receives the mouse focus

	CDialog::OnSetFocus(pOldWnd);

	ComputeGeometry();
	SetResults();
	m_ctrlPanelList.Invalidate();
	m_ctrlOK.SetFocus();
	InvalidateRect(&m_DrawRect);
}


void CWingDlg::OnPaint() 
{
	// The window has recieved the message to repaint itself

	CPaintDC dc(this); // device context for painting
	
	CFont Arial8;
	Arial8.CreatePointFont(80,"Arial");
	CFont *pOldFont = dc.SelectObject(&Arial8);
	CPoint O(m_ptOffset);

	if(!m_bRightSide) O.x += (int)(m_pWing->m_TPos[m_pWing->m_NPanel]*m_fWingScale);

	DrawVLMMesh(&dc, O);
	DrawWing(&dc, O);
	DrawFoils(&dc, O);
	DrawDihedral(&dc, O);

	CPen SymPen(PS_DASHDOT, 1, RGB(0,0,0));
	dc.SetBkMode(TRANSPARENT);
	CPen *pOldPen = dc.SelectObject(&SymPen);
	dc.MoveTo(O.x, O.y-70);
	dc.LineTo(O.x, O.y+140);


	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldFont);
//	BlackPen.DeleteObject();
	SymPen.DeleteObject();
	// Do not call CDialog::OnPaint() for painting messages
}


int CWingDlg::IsFoil(CPoint point)
{
	// Returns the sections index where the
	if(m_bRightSide){
		double x = (point.x-m_ptOffset.x)/m_fWingScale;
		double y = (point.y-m_ptOffset.y)/m_fWingScale;
		for (int i=0; i<=m_pWing->m_NPanel; i++){
			if((abs(x-m_pWing->m_TPos[i])/m_pWing->m_Span<0.01) &&
				(y>=m_pWing->m_TOffset[i] && y<=m_pWing->m_TOffset[i]+m_pWing->m_TChord[i]))
				return i;
//				TRACE("returning %d\n",i);
		}
	}
	else{
		int Ox = m_ptOffset.x + (int)(m_pWing->m_TPos[m_pWing->m_NPanel]*m_fWingScale);
		double x = (point.x-Ox)/m_fWingScale;
		double y = (point.y-m_ptOffset.y)/m_fWingScale;
		for (int i=0; i<=m_pWing->m_NPanel; i++){
			if((abs(-x-m_pWing->m_TPos[i])/m_pWing->m_Span<0.01) &&
				(y>=m_pWing->m_TOffset[i] && y<=m_pWing->m_TOffset[i]+m_pWing->m_TChord[i]))
				return i;
		}
	}
	return -1;
}


void CWingDlg::DrawWing(CDC *pDC, CPoint O)
{
	double x, x1, x2;
	int i;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CFoil *pFoil1, *pFoil2;
	CPen BlackPen(PS_SOLID, 2, GetSysColor(COLOR_BTNTEXT));
	CPen *pOldPen = pDC->SelectObject(&BlackPen);

	if(m_bRightSide){
		for (i=0; i<m_pWing->m_NPanel;i++){
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x +(int)(x2*m_fWingScale),
						O.y +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x +(int)(x2*m_fWingScale),
						O.y +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
		}
	}
	else{
		for (i=0; i<m_pWing->m_NPanel;i++){
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
		}
	}
	//Draw Flaps
	CPen DotPen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	pDC->SelectObject(&DotPen);
	if(m_bRightSide){
		for (i=0; i<m_pWing->m_NPanel;i++){
			pFoil1 = pFrame->GetFoil(m_pWing->m_RFoil[i]);
			pFoil2 = pFrame->GetFoil(m_pWing->m_RFoil[i+1]);
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			if(pFoil1->m_bTEFlap && pFoil2->m_bTEFlap){
				if(pFoil1->m_bTEFlap){
					pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_pWing->m_TChord[i])*m_fWingScale));
				}
				else {
					pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
				}
				if(pFoil2->m_bTEFlap){
					pDC->LineTo(O.x +(int)(x2*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_pWing->m_TChord[i+1])*m_fWingScale));
				}
				else {
					pDC->LineTo(O.x +(int)(x2*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
				}
			}
		}
	}
	else{
		for (i=0; i<m_pWing->m_NPanel;i++){
			pFoil1 = pFrame->GetFoil(m_pWing->m_LFoil[i]);
			pFoil2 = pFrame->GetFoil(m_pWing->m_LFoil[i+1]);
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			if(pFoil1->m_bTEFlap && pFoil2->m_bTEFlap){
				if(pFoil1->m_bTEFlap){
					pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_pWing->m_TChord[i])*m_fWingScale));
				}
				else {
					pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
				}
				if(pFoil2->m_bTEFlap){
					pDC->LineTo(O.x -(int)(x2*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_pWing->m_TChord[i+1])*m_fWingScale));
				}
				else {
					pDC->LineTo(O.x -(int)(x2*m_fWingScale), 
								O.y +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
				}
			}
		}
/*		for (int i=0; i<m_pWing->m_NPanel;i++){
			x1 = m_pWing->m_TPos[i];
			x2 = m_pWing->m_TPos[i+1];
			pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)(m_pWing->m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)((m_pWing->m_TOffset[i+1]+m_pWing->m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)((m_pWing->m_TOffset[i]+m_pWing->m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_pWing->m_TOffset[i]*m_fWingScale));
		}*/
	}
	//find mean aero chord, and draw it
	if (m_yMac>0.0){
		CPen GreenPen(PS_DOT, 1, RGB(0,170,0));
		pDC->SelectObject(&GreenPen);
		pDC->SetTextColor(RGB(0,170,0));
		pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
		if(m_bRightSide){
			x = GetOffset(2.0*m_yMac/m_pWing->m_Span);
			pDC->MoveTo(O.x + (int)(m_yMac*m_fWingScale), 
						O.y + (int)(x*m_fWingScale));
			pDC->LineTo(O.x + (int)(m_yMac*m_fWingScale), 
						O.y + (int)((x+m_pWing->m_MAChord)*m_fWingScale));
			pDC->TextOut(O.x - 10 + (int)(m_yMac*m_fWingScale),
						 O.y - 20 +(int)(x*m_fWingScale),
						"m.a.c.");
		}
		else{
			x = GetOffset(2.0*m_yMac/m_pWing->m_Span);
			pDC->MoveTo(O.x - (int)(m_yMac*m_fWingScale), 
						O.y + (int)(x*m_fWingScale));
			pDC->LineTo(O.x - (int)(m_yMac*m_fWingScale), 
						O.y + (int)((x+m_pWing->m_MAChord)*m_fWingScale));
			pDC->TextOut(O.x -10- (int)(m_yMac*m_fWingScale),
						 O.y -20+(int)(x*m_fWingScale),
						"m.a.c.");
		}
	}
	pDC->SelectObject(pOldPen);
}
void CWingDlg::DrawVLMMesh(CDC *pDC, CPoint O)
{
	double chord, offset, yl;
	double offset1, offset2, ch1, ch2, x1, x2, y;
	double dT,dk,dNY;
	double xPoint1[MAXCHORDPANELS+1],xPoint2[MAXCHORDPANELS+1]; 
	int i,k,l;
	int NXLead, NXFlap;

	CFoil *pFoil1, *pFoil2;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	CPen MeshPen(PS_SOLID, 1, RGB(80, 80, 255));
	CPen *pOldPen = pDC->SelectObject(&MeshPen);


	if(m_bRightSide)
	{
		for (i=0; i<m_pWing->m_NPanel;i++){
			//first draw lines parallel to chord,
			dT  = m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i];
			dNY = (double)m_pWing->m_NYPanels[i];
			for (k=1;k<m_pWing->m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_pWing->m_YPanelDist[i] == 1)
				{
					//cosine distribution
					y = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i+1])/2.0;
					yl = y +cos(k*pi/m_pWing->m_NYPanels[i]) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/2.0;
				}
				else if(m_pWing->m_YPanelDist[i] ==  2){
					//sine distribution
					yl = m_pWing->m_TPos[i+1] -(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else if(m_pWing->m_YPanelDist[i] == -2){
					//-sine distribution
					yl = m_pWing->m_TPos[i] +(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else {
					//even spacing by default
					yl = m_pWing->m_TPos[i] +(k*(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_NYPanels[i]);
				}
				offset = GetOffset(yl*2.0/m_pWing->m_Span);
				chord  = GetChord(yl*2.0/m_pWing->m_Span);
				pDC->MoveTo(O.x +(int)(yl*m_fWingScale), 
							O.y +(int)(offset*m_fWingScale));
				pDC->LineTo(O.x +(int)(yl*m_fWingScale), 
							O.y +(int)((offset+chord)*m_fWingScale));
			}
			//next draw lines parallel to span,

			offset1 = m_pWing->m_TOffset[i];
			offset2 = m_pWing->m_TOffset[i+1];
			ch1     = m_pWing->m_TChord[i];
			ch2     = m_pWing->m_TChord[i+1];
			pFoil1  = pFrame->GetFoil(m_pWing->m_RFoil[i]);
			pFoil2  = pFrame->GetFoil(m_pWing->m_RFoil[i+1]);

			CreateXPoints(m_pWing->m_NXPanels[i], m_pWing->m_XPanelDist[i], pFoil1, pFoil2, 
				          xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_pWing->m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				pDC->MoveTo(O.x +(int)(m_pWing->m_TPos[i]*m_fWingScale), 
							O.y +(int)(x1*m_fWingScale));
				pDC->LineTo(O.x +(int)(m_pWing->m_TPos[i+1]*m_fWingScale), 
							O.y +(int)(x2*m_fWingScale));
			}
		}
	}
	else
	{
		for (i=0; i<m_pWing->m_NPanel;i++)
		{
			//first draw lines parallel to chord,
			dT  = m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i];
			dNY = (double)m_pWing->m_NYPanels[i];
			for (k=1;k<m_pWing->m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_pWing->m_YPanelDist[i] == 1)
				{
					//cosine distribution
					y = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i+1])/2.0;
					yl = y +cos(k*pi/m_pWing->m_NYPanels[i]) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/2.0;
				}
				else if(m_pWing->m_YPanelDist[i] == -2){
					//-sine distribution
					yl = m_pWing->m_TPos[i]   +(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else if(m_pWing->m_YPanelDist[i] == 2){
					// sine distribution
					yl = m_pWing->m_TPos[i+1] -(sin(k*pi/2.0/m_pWing->m_NYPanels[i])) * (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
				}
				else 
				{
					//even spacing by default
					yl = m_pWing->m_TPos[i] +(k*(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_NYPanels[i]);
				}
				offset = GetOffset(yl*2.0/m_pWing->m_Span);
				chord  = GetChord(yl*2.0/m_pWing->m_Span);
				pDC->MoveTo(O.x -(int)(yl*m_fWingScale), 
							O.y +(int)(offset*m_fWingScale));
				pDC->LineTo(O.x -(int)(yl*m_fWingScale), 
							O.y +(int)((offset+chord)*m_fWingScale));
			}

			//next draw lines parallel to span,
			offset1 = m_pWing->m_TOffset[i];
			offset2 = m_pWing->m_TOffset[i+1];
			ch1     = m_pWing->m_TChord[i];
			ch2     = m_pWing->m_TChord[i+1];
			pFoil1  = pFrame->GetFoil(m_pWing->m_LFoil[i]);
			pFoil2  = pFrame->GetFoil(m_pWing->m_LFoil[i+1]);

			CreateXPoints(m_pWing->m_NXPanels[i], m_pWing->m_XPanelDist[i], 
                          pFoil1, pFoil2, xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_pWing->m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				pDC->MoveTo(O.x -(int)(m_pWing->m_TPos[i]*m_fWingScale), 
							O.y +(int)(x1*m_fWingScale));
				pDC->LineTo(O.x -(int)(m_pWing->m_TPos[i+1]*m_fWingScale), 
							O.y +(int)(x2*m_fWingScale));
			}
		}
	}
	pDC->SelectObject(pOldPen);
}


void CWingDlg::DrawDihedral(CDC *pDC, CPoint O)
{
	double x,z;
	//Draw dihedral
	CPen DihPen(PS_SOLID, 2, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&DihPen);
	int yOff = -30;

	if(m_bRightSide){
		pDC->MoveTo(O.x , O.y + yOff);
		for (int i=1; i<=m_pWing->m_NPanel;i++){
			x = m_pWing->m_TYProj[i];
			z = GetZPos(m_pWing->m_TPos[i]);
			pDC->LineTo(O.x +      (int)(x*m_fWingScale),
						O.y + yOff-(int)(z*m_fWingScale));
		}
	}
	else{
		pDC->MoveTo(O.x , O.y + yOff);
		for (int i=1; i<=m_pWing->m_NPanel;i++){
			x = m_pWing->m_TYProj[i];
			z = GetZPos(m_pWing->m_TPos[i]);
			pDC->LineTo(O.x -      (int)(x*m_fWingScale),
						O.y + yOff-(int)(z*m_fWingScale));
		}
	}

	pDC->SelectObject(pOldPen);
}
 
double CWingDlg::GetAverageSweep()
{
	double xroot = m_pWing->m_TChord[0]/4.0;
	double xtip  = m_pWing->m_TOffset[m_pWing->m_NPanel] + m_pWing->m_TChord[m_pWing->m_NPanel]/4.0;
//	double sweep = (atan2(xtip-xroot, m_pWing->m_Span/2.0)) * 180.0/pi;
	return (atan2(xtip-xroot, m_pWing->m_Span/2.0)) * 180.0/pi;
}



void CWingDlg::DrawFoils(CDC *pDC, CPoint O)
{
	double x, y, Chord;
	int i, nFlap;
	CString strong;
	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)s_pMiarex;

	pDC->SetTextAlign(TA_CENTER);
	pDC->SetBkColor(GetSysColor(COLOR_3DFACE));

	LOGBRUSH lb1;
	lb1.lbStyle = BS_SOLID;
	lb1.lbColor = RGB(0,0,0);
	CPen FoilPen(PS_GEOMETRIC | PS_DASH, 2, &lb1);

	LOGBRUSH lb2;
	lb2.lbStyle = BS_SOLID;
	lb2.lbColor = RGB(255,0,0);
	CPen HighPen(PS_GEOMETRIC | PS_DASH, 3, &lb2);

	CPen *pOldPen = (CPen*)pDC->SelectObject(&FoilPen);

	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span;

	if(m_bRightSide){
		for (i=0; i<=m_pWing->m_NPanel;i++){
			x = m_pWing->m_TPos[i];
			y = GetOffset(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			Chord = GetChord(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			if(Chord<0.0) break;

			if (i==m_iSection){
				pDC->SelectObject(HighPen);
				pDC->SetTextColor(RGB(255,0,0));
			}
			else{
				pDC->SelectObject(FoilPen);
				pDC->SetTextColor(RGB(0,0,0));
			}
			pDC->MoveTo(O.x + (int)(x*m_fWingScale), 
						O.y + (int)(y*m_fWingScale));
			pDC->LineTo(O.x + (int)(x*m_fWingScale), 
						O.y + (int)((y+Chord)*m_fWingScale));

			strong = m_pWing->m_RFoil[i].Left(30);
			pDC->TextOut(O.x + (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
		}
		nFlap = 0;
		//	count left wing flaps
		for (i=1; i<=m_pWing->m_NPanel; i++){
			pFoilA = pFrame->GetFoil(m_pWing->m_LFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_pWing->m_LFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize){
				//we have a flap, so count it
				nFlap++;
			}
		}
		for (i=1; i<=m_pWing->m_NPanel; i++){
			pFoilA = pFrame->GetFoil(m_pWing->m_RFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_pWing->m_RFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize){
				//we have a flap, so write it down
				x = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i-1])/2.0;
				y = GetOffset(x/m_pWing->m_Span*2.0);
				strong.Format("Flap %d", nFlap+1);
				pDC->TextOut(O.x + (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	else{
		for (i=0; i<=m_pWing->m_NPanel;i++){
			x = m_pWing->m_TPos[i];
			y = GetOffset(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			Chord = GetChord(m_pWing->m_TPos[i]/m_pWing->m_Span*2.0);
			if(Chord<0.0 || y<0.0) break;

			if (i==m_iSection){
				pDC->SelectObject(HighPen);
				pDC->SetTextColor(RGB(255,0,0));
			}
			else{ 
				pDC->SelectObject(FoilPen);
				pDC->SetTextColor(RGB(0,0,0));
			}
			pDC->MoveTo(O.x - (int)(x*m_fWingScale), 
						O.y + (int)(y*m_fWingScale));
			pDC->LineTo(O.x - (int)(x*m_fWingScale), 
						O.y + (int)((y+Chord)*m_fWingScale));

			strong = m_pWing->m_LFoil[i].Left(30);
			pDC->TextOut(O.x - (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale),
						strong);
		}
		nFlap = 0;

//		for (i=1; i<=m_pWing->m_NPanel; i++){
		for (i=m_pWing->m_NPanel; i>=1; i--){
			pFoilA = pFrame->GetFoil(m_pWing->m_LFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_pWing->m_LFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])>MinPanelSize){
				//we have a flap, so write it down
				x = (m_pWing->m_TPos[i]+m_pWing->m_TPos[i-1])/2.0;
				y = GetOffset(x/m_pWing->m_Span*2.0);
				strong.Format("Flap %d", nFlap+1);
				pDC->TextOut(O.x - (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	pDC->SelectObject(pOldPen);
}



void CWingDlg::OnVLMMesh() 
{
	VLMSetAutoMesh();
	FillPanelList();
	SetResults();
	InvalidateRect(m_DrawRect);
	m_bChanged = true;
} 

void CWingDlg::OnWingColor() 
{
	m_pWing->m_WingColor = m_ctrlWingColor.GetColor();
	m_ctrlWingColor.Invalidate();
}

double CWingDlg::GetOffset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= abs(yob*m_pWing->m_Span/2.0);
	for(int i=0; i<m_pWing->m_NPanel; i++){
		if(m_pWing->m_TPos[i]<= y && y <=m_pWing->m_TPos[i+1]){
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			Offset = m_pWing->m_TOffset[i] + tau * (m_pWing->m_TOffset[i+1] - m_pWing->m_TOffset[i]);
			return Offset;
		}
	}
	
	return -1.0;
}

double CWingDlg::GetZPos(double y)
{
	double tau;
	double ZPos =0.0;

	y= abs(y);
	if(y<=0.0) return 0.0;
	for (int i=0; i<m_pWing->m_NPanel; i++)
	{
		if(m_pWing->m_TPos[i]< y && y<=m_pWing->m_TPos[i+1])
		{
			for (int k=0; k<i; k++)
			{
				ZPos+=m_pWing->m_TLength[k+1] * sin(m_pWing->m_TDihedral[k]*pi/180.0);
			}
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			ZPos += tau * m_pWing->m_TLength[i+1] * sin(m_pWing->m_TDihedral[i]*pi/180.0);
			return ZPos;
		}
	}
	return 0.0;
}

double CWingDlg::GetChord(double yob)
{
	double Chord = 0.0;
	double tau;
	double y;

	y= abs(yob*m_pWing->m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_pWing->m_NPanel; i++){
		if(m_pWing->m_TPos[i]<=y && y <=m_pWing->m_TPos[i+1]){
			tau = (y - m_pWing->m_TPos[i])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i]);
			Chord = m_pWing->m_TChord[i] + tau * (m_pWing->m_TChord[i+1] - m_pWing->m_TChord[i]);
			return Chord;
		}
	}
	return -1.0;
}


void CWingDlg::SplitPanel(int i, double tau)
{
	if(m_pWing->m_NPanel>=MAXPANELS-1) return;

	for (int k=m_pWing->m_NPanel; k>=i; k--)
	{
		m_pWing->m_TPos[k+1]       = m_pWing->m_TPos[k];
		m_pWing->m_TChord[k+1]     = m_pWing->m_TChord[k];
		m_pWing->m_TOffset[k+1]    = m_pWing->m_TOffset[k];
		m_pWing->m_TTwist[k+1]     = m_pWing->m_TTwist[k];
		m_pWing->m_TDihedral[k+1]  = m_pWing->m_TDihedral[k];
		m_pWing->m_NXPanels[k+1]   = m_pWing->m_NXPanels[k];	
		m_pWing->m_NYPanels[k+1]   = m_pWing->m_NYPanels[k];	
		m_pWing->m_XPanelDist[k+1] = m_pWing->m_XPanelDist[k];
		m_pWing->m_YPanelDist[k+1] = m_pWing->m_YPanelDist[k];
		m_pWing->m_RFoil[k+1]      = m_pWing->m_RFoil[k];
		m_pWing->m_LFoil[k+1]      = m_pWing->m_LFoil[k];
	}
	if(i==0) i++;

	m_pWing->m_TPos[i]       = m_pWing->m_Span/2.0 * tau;

	double taupanel  = (m_pWing->m_TPos[i]-m_pWing->m_TPos[i-1])/(m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i-1]);
	m_pWing->m_TChord[i]      = m_pWing->m_TChord[i-1]  + taupanel * (m_pWing->m_TChord[i+1] -m_pWing->m_TChord[i-1]);
	m_pWing->m_TOffset[i]     = m_pWing->m_TOffset[i-1] + taupanel * (m_pWing->m_TOffset[i+1]-m_pWing->m_TOffset[i-1]);
	m_pWing->m_TTwist[i]      = m_pWing->m_TTwist[i-1] + taupanel * (m_pWing->m_TTwist[i+1]-m_pWing->m_TTwist[i-1]);
	m_pWing->m_TDihedral[i]   = m_pWing->m_TDihedral[i-1];
	m_pWing->m_NXPanels[i]    = m_pWing->m_NXPanels[i-1];	
	m_pWing->m_NYPanels[i]    = int(m_pWing->m_NYPanels[i-1]*(1-taupanel));
	m_pWing->m_NYPanels[i-1]  = int(m_pWing->m_NYPanels[i-1]* taupanel);
	m_pWing->m_XPanelDist[i]  = m_pWing->m_XPanelDist[i-1];
	m_pWing->m_YPanelDist[i]  = m_pWing->m_YPanelDist[i-1];
	m_pWing->m_RFoil[i]       = m_pWing->m_RFoil[i-1];
	m_pWing->m_LFoil[i]       = m_pWing->m_LFoil[i-1];

	m_pWing->m_NPanel++;

}


bool CWingDlg::CheckWing()
{
	if(!m_pWing->m_WingName.GetLength())
	{
		AfxMessageBox("Please enter a name for the wing");
		m_ctrlWingName.SetFocus();
		return false;
	}
	for (int k=1; k<=m_pWing->m_NPanel; k++)
	{
		if(m_pWing->m_TPos[k]*1.00001 < m_pWing->m_TPos[k-1]){
			AfxMessageBox("Warning : Panel sequence is inconsistent");
			return false;
		}
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2) 
	{
		AfxMessageBox("Too many panels\nReduce the mesh size");
		return false;
	}

	if(m_pWing->m_nFlaps>=20)
	{
		int res = AfxMessageBox("Only 10 flaps x 2 will be handled", MB_OKCANCEL);
		if (res==IDCANCEL) return false;
	}
	return true;
}



void CWingDlg::Convert(bool bSet)//else retrieve
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	if(bSet){//convert data from IS and set it

		for (int i=0; i<=MAXPANELS; i++){
			m_DTChord[i]  = m_pWing->m_TChord[i]  * pFrame->m_mtoUnit;
			m_DTPos[i]    = m_pWing->m_TPos[i]    * pFrame->m_mtoUnit;
			m_DTOffset[i] = m_pWing->m_TOffset[i] * pFrame->m_mtoUnit;
		}

	}
	else{//retreive data and convert it to IS
		//convert and retrieve
		for (int i=0; i<=MAXPANELS; i++){
			m_pWing->m_TChord[i]  = m_DTChord[i]  / pFrame->m_mtoUnit;
			m_pWing->m_TPos[i]    = m_DTPos[i]    / pFrame->m_mtoUnit;
			m_pWing->m_TOffset[i] = m_DTOffset[i] / pFrame->m_mtoUnit;
		}
	}
}

void CWingDlg::SetParams()
{
	Convert(true);//set the data
	FillPanelList();
	SetResults();
}



void CWingDlg::OnInsertBefore() 
{
	if (m_pWing->m_NPanel==MAXPANELS) {
		AfxMessageBox("The maximum number of panels has been reached", MB_OK);
		return;
	}
	if(m_iSection<=0)
	{
		AfxMessageBox("No insertion possible before the first section", MB_OK);
		return;
	}
	int k,n,total, ny;
	total = VLMGetPanelTotal();
	n  = m_iSection;
	for (k=m_pWing->m_NPanel; k>=n; k--)
	{
		m_DTPos[k+1]      = m_DTPos[k];
		m_DTChord[k+1]    = m_DTChord[k];
		m_DTOffset[k+1]   = m_DTOffset[k];
		m_pWing->m_TTwist[k+1]     = m_pWing->m_TTwist[k];
		m_pWing->m_TDihedral[k+1]  = m_pWing->m_TDihedral[k];
		m_pWing->m_NXPanels[k+1]   = m_pWing->m_NXPanels[k];	
		m_pWing->m_NYPanels[k+1]   = m_pWing->m_NYPanels[k];	
		m_pWing->m_XPanelDist[k+1] = m_pWing->m_XPanelDist[k];
		m_pWing->m_YPanelDist[k+1] = m_pWing->m_YPanelDist[k];
		m_pWing->m_RFoil[k+1]      = m_pWing->m_RFoil[k];
		m_pWing->m_LFoil[k+1]      = m_pWing->m_LFoil[k];
	}

	ny = m_pWing->m_NYPanels[n-1];
	m_DTPos[n]    = (m_DTPos[n+1]    + m_DTPos[n-1])   /2.0;
	m_DTChord[n]  = (m_DTChord[n+1]  + m_DTChord[n-1]) /2.0;
	m_DTOffset[n] = (m_DTOffset[n+1] + m_DTOffset[n-1])/2.0;

	m_pWing->m_NXPanels[n]   = m_pWing->m_NXPanels[n-1];
	m_pWing->m_NYPanels[n]   = (int)(ny/2);
	m_pWing->m_NYPanels[n-1] = ny-m_pWing->m_NYPanels[n];
	if(m_pWing->m_NYPanels[n]==0)   m_pWing->m_NYPanels[n]++;
	if(m_pWing->m_NYPanels[n-1]==0) m_pWing->m_NYPanels[n-1]++;

	m_pWing->m_NPanel++;

//	m_pWing->m_bVLMAutoMesh = true;

	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}

void CWingDlg::OnAppend() 
{
	if (m_pWing->m_NPanel==MAXPANELS)
	{
		AfxMessageBox("The maximum number of panels has been reached", MB_OK);
		return;
	}
	int k,n,ny,total;

	n  = m_iSection;
	if(n<0) n=m_pWing->m_NPanel;
	ny = m_pWing->m_NYPanels[n];
	total = VLMGetPanelTotal();

	for (k=m_pWing->m_NPanel; k>n; k--)
	{
		m_DTPos[k+1]      = m_DTPos[k];
		m_DTChord[k+1]    = m_DTChord[k];
		m_DTOffset[k+1]   = m_DTOffset[k];
		m_pWing->m_TTwist[k+1]     = m_pWing->m_TTwist[k];
		m_pWing->m_TDihedral[k+1]  = m_pWing->m_TDihedral[k];
		m_pWing->m_NXPanels[k+1]   = m_pWing->m_NXPanels[k];	
		m_pWing->m_NYPanels[k+1]   = m_pWing->m_NYPanels[k];	
		m_pWing->m_XPanelDist[k+1] = m_pWing->m_XPanelDist[k];
		m_pWing->m_YPanelDist[k+1] = m_pWing->m_YPanelDist[k];
		m_pWing->m_RFoil[k+1]      = m_pWing->m_RFoil[k];
	}

	if(n+1<=m_pWing->m_NPanel)
	{
		m_DTPos[n+1]    = (m_DTPos[n]    + m_DTPos[n+2])   /2.0;
		m_DTChord[n+1]  = (m_DTChord[n]  + m_DTChord[n+2]) /2.0;
		m_DTOffset[n+1] = (m_DTOffset[n] + m_DTOffset[n+2])/2.0;
	}
	else
	{
		m_DTPos[n+1]     = m_DTPos[n]*1.1;
		m_DTChord[n+1]   = m_DTChord[n]/1.1; 
		m_DTOffset[n+1]  = m_DTOffset[n] + m_DTChord[n] - m_DTChord[n+1] ;
	}
	m_pWing->m_TTwist[n+1]     = m_pWing->m_TTwist[n];
	m_pWing->m_TDihedral[n+1]  = m_pWing->m_TDihedral[n];
	m_pWing->m_NXPanels[n+1]   = m_pWing->m_NXPanels[n];	
	m_pWing->m_NYPanels[n+1]   = m_pWing->m_NYPanels[n];	
	m_pWing->m_XPanelDist[n+1] = m_pWing->m_XPanelDist[n];
	m_pWing->m_YPanelDist[n+1] = m_pWing->m_YPanelDist[n];
	m_pWing->m_RFoil[n+1]      = m_pWing->m_RFoil[n];

	m_pWing->m_NYPanels[n+1] = max(1,(int)(ny/2)); 
	m_pWing->m_NYPanels[n]   = max(1,ny-m_pWing->m_NYPanels[n+1]); 
	
	m_pWing->m_NPanel++;

//	m_pWing->m_bVLMAutoMesh = true;
	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}	


void CWingDlg::OnDeleteInput() 
{
	if(m_iSection<=0)
	{
		AfxMessageBox("The first section cannot be deleted", MB_OK);
		return;
	}

	int ny, k, size, total;

	size = m_ctrlPanelList.GetItemCount();
	if(size<=2) return;

	ny = m_pWing->m_NYPanels[m_iSection-1] + m_pWing->m_NYPanels[m_iSection];

	total = VLMGetPanelTotal();
	for (k=m_iSection; k<size; k++)
	{
		m_DTPos[k]      = m_DTPos[k+1];
		m_DTChord[k]    = m_DTChord[k+1];
		m_DTOffset[k]   = m_DTOffset[k+1];
		m_pWing->m_TTwist[k]     = m_pWing->m_TTwist[k+1];
		m_pWing->m_TDihedral[k]  = m_pWing->m_TDihedral[k+1];
		m_pWing->m_NXPanels[k]   = m_pWing->m_NXPanels[k+1];	
		m_pWing->m_NYPanels[k]   = m_pWing->m_NYPanels[k+1];	
		m_pWing->m_XPanelDist[k] = m_pWing->m_XPanelDist[k+1];
		m_pWing->m_YPanelDist[k] = m_pWing->m_YPanelDist[k+1];
		m_pWing->m_RFoil[k]      = m_pWing->m_RFoil[k+1];
		m_pWing->m_LFoil[k]      = m_pWing->m_LFoil[k+1];
	}
	m_pWing->m_NPanel--;
	
	m_pWing->m_NYPanels[m_iSection-1] = ny;

//	m_pWing->m_bVLMAutoMesh = true;
	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}


void CWingDlg::OnSymetric() 
{
	if(m_ctrlSymetric.GetCheck())
	{
		m_pWing->m_bSymetric    = true;
		m_bRightSide   = true;
		m_ctrlLeftWing.EnableWindow(false);
		CheckRadioButton(IDC_SIDE1, IDC_SIDE2, IDC_SIDE2);
	}
	else {
		m_pWing->m_bSymetric    = false;
		m_ctrlLeftWing.EnableWindow(true);
	}

	InvalidateRect(&m_DrawRect);
	m_bChanged = true;
}


void CWingDlg::OnSide() 
{
	if(GetCheckedRadioButton(IDC_SIDE1,IDC_SIDE2)==IDC_SIDE2){
		m_bRightSide = true;
	}
	else {
		m_bRightSide = false;
//		m_pWing->m_LFoil[0] = m_pWing->m_RFoil[0];
	}
	FillPanelList();
	m_bChanged = true;
	InvalidateRect(m_DrawRect);
}




int CWingDlg::VLMGetPanelTotal()
{
	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_pWing->m_Span/1000.0;

	int total = 0;
	for (int i=0; i<m_pWing->m_NPanel; i++){
			//do not create a surface if its length is less than the critical size
//			if(abs(m_pWing->m_TPos[j]-m_pWing->m_TPos[j+1])/m_pWing->m_Span >0.001){
			if (abs(m_pWing->m_TPos[i]-m_pWing->m_TPos[i+1]) > MinPanelSize)
				total +=m_pWing->m_NXPanels[i]*m_pWing->m_NYPanels[i];
	}
//	if(!m_bMiddle) total *=2;
	if(!m_pWing->m_bIsFin) return total*2;
	else          return total;
}


bool CWingDlg::VLMSetAutoMesh(int total)
{
	//set automatic mesh : keep it simple
	m_pWing->m_bVLMAutoMesh = true;
	m_bChanged = true;
	//split (NYTotal) panels on each side proportionnaly to length, and space evenly
	//Set VLMMATSIZE/NYTotal panels along chord
	int NYTotal, size;
	
	if(!total){
		size = (int)(VLMMATSIZE/4);//why not ? Too much refinement isn't worthwile
		NYTotal = 22;
	}
	else{
		size = total;
		NYTotal = (int)sqrt((float)size);
	}

	NYTotal *= 2;

	double d1, d2; //spanwise panel densities at i and i+1

	for (int i=0; i<m_pWing->m_NPanel;i++)
	{
		d1 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->m_TPos[i],  3) + 0.5;
		d2 = 5./2./m_pWing->m_Span/m_pWing->m_Span/m_pWing->m_Span *8. * pow(m_pWing->m_TPos[i+1],3) + 0.5;
		m_pWing->m_NYPanels[i] = (int) (NYTotal * (0.8*d1+0.2*d2)* (m_pWing->m_TPos[i+1]-m_pWing->m_TPos[i])/m_pWing->m_Span);
		m_pWing->m_NXPanels[i] = (int) (size/NYTotal);
		m_pWing->m_NXPanels[i] = min(m_pWing->m_NXPanels[i], MAXCHORDPANELS);

		if(m_pWing->m_NYPanels[i]==0) m_pWing->m_NYPanels[i] = 1;
		if(m_pWing->m_NXPanels[i]==0) m_pWing->m_NXPanels[i] = 1;
//		m_pWing->m_XPanelDist[i] = 1;//cosine distribution
//		m_pWing->m_YPanelDist[i] = 0;//uniformly distributed, except at the root and tip (see next)
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2)
	{
		AfxMessageBox("Too many panels\nReduce the mesh size", MB_OK);
		return false;
	}
	return true;
}

void CWingDlg::FillCell(int iItem, int iSubItem)
{
	CString strong;
	switch (iSubItem)
	{
		case 0:
		{
			strong.Format("%d", iItem);
			m_ctrlPanelList.SetItemText(iItem, 0, strong);
			break;
		}
		case 1:
		{
			strong = GetFormat(m_DTPos[iItem],3);
			m_ctrlPanelList.SetItemText(iItem,1,strong);

			break;
		}
		case 2:
		{
			strong = GetFormat(m_DTChord[iItem],3);
			m_ctrlPanelList.SetItemText(iItem,2,strong);

			break;
		}
		case 3:
		{
			strong = GetFormat(m_DTOffset[iItem],3);
			m_ctrlPanelList.SetItemText(iItem,3,strong);
			break;
		}
		case 4:
		{
			if(iItem<m_pWing->m_NPanel)	strong.Format("%.2f", m_pWing->m_TDihedral[iItem]);
			else			strong = " ";
			m_ctrlPanelList.SetItemText(iItem,4,strong);

			break;
		}
		case 5:
		{
			strong.Format("%.2f", m_pWing->m_TTwist[iItem]);
			m_ctrlPanelList.SetItemText(iItem,5,strong);
			break;
		}
		case 6:
		{
			if(m_bRightSide) m_ctrlPanelList.SetItemText(iItem,6,m_pWing->m_RFoil[iItem]);
			else             m_ctrlPanelList.SetItemText(iItem,6,m_pWing->m_LFoil[iItem]);
			break;
		}
		case 7:
		{
			if(iItem<m_pWing->m_NPanel)
			{
				strong.Format("%d", m_pWing->m_NXPanels[iItem]);
				m_ctrlPanelList.SetItemText(iItem,7,strong);
			}
			else m_ctrlPanelList.SetItemText(iItem,7,"");
			break;
		}
		case 8:
		{
			if(iItem<m_pWing->m_NPanel)
			{
				if(m_pWing->m_XPanelDist[iItem]==0)			strong = "Uniform";
				else if(m_pWing->m_XPanelDist[iItem]==1)	strong = "Cosine";
				m_ctrlPanelList.SetItemText(iItem,8,strong);
			}
			else m_ctrlPanelList.SetItemText(iItem,8,"");
			break;
		}
		case 9:
		{
			if(iItem<m_pWing->m_NPanel)
			{
				strong.Format("%d", m_pWing->m_NYPanels[iItem]);
				m_ctrlPanelList.SetItemText(iItem,9,strong);
			}
			else m_ctrlPanelList.SetItemText(iItem,9,"");
			break;
		}
		case 10:
		{
			if(iItem<m_pWing->m_NPanel)
			{
				if(m_pWing->m_YPanelDist[iItem]==0)			strong = "Uniform";
				else if(m_pWing->m_YPanelDist[iItem]==1)	strong = "Cosine";
				else if(m_pWing->m_YPanelDist[iItem]==2)	strong = "Sine";
				else if(m_pWing->m_YPanelDist[iItem]==-2)	strong = "-Sine";
				m_ctrlPanelList.SetItemText(iItem,10,strong);
			}
			else m_ctrlPanelList.SetItemText(iItem,10,"");
			break;
		}
		default:
		{
			break;
		}
	}
}

void CWingDlg::FillPanelList()
{
	int i;
	CString str, strong;
	m_ctrlPanelList.DeleteAllItems();
//	if(m_pWing->m_bVLMAutoMesh) VLMSetAutoMesh();

	for(i=0; i<=m_pWing->m_NPanel; i++)
	{
		strong.Format("%d", i);
		m_ctrlPanelList.InsertItem(i, strong);

		strong = GetFormat(m_DTPos[i],3);
		m_ctrlPanelList.SetItemText(i,1,strong);

		strong = GetFormat(m_DTChord[i],3);
		m_ctrlPanelList.SetItemText(i,2,strong);

		strong = GetFormat(m_DTOffset[i],3);
		m_ctrlPanelList.SetItemText(i,3,strong);

		if(i<m_pWing->m_NPanel)	strong.Format("%.2f", m_pWing->m_TDihedral[i]);
		else			strong = " ";
		m_ctrlPanelList.SetItemText(i,4,strong);

		strong.Format("%.2f", m_pWing->m_TTwist[i]);
		m_ctrlPanelList.SetItemText(i,5,strong);

		if(m_bRightSide) m_ctrlPanelList.SetItemText(i,6,m_pWing->m_RFoil[i]);
		else             m_ctrlPanelList.SetItemText(i,6,m_pWing->m_LFoil[i]);

		if(i<m_pWing->m_NPanel)
		{
			strong.Format("%d", m_pWing->m_NXPanels[i]);
			m_ctrlPanelList.SetItemText(i,7,strong);

			if(m_pWing->m_XPanelDist[i]==0)		strong = "Uniform";
			else if(m_pWing->m_XPanelDist[i]==1)	strong = "Cosine";
			m_ctrlPanelList.SetItemText(i,8,strong);

			strong.Format("%d", m_pWing->m_NYPanels[i]);
			m_ctrlPanelList.SetItemText(i,9,strong);

			if(m_pWing->m_YPanelDist[i]==0)			strong = "Uniform";
			else if(m_pWing->m_YPanelDist[i]==1)		strong = "Cosine";
			else if(m_pWing->m_YPanelDist[i]==2)		strong = "Sine";
			else if(m_pWing->m_YPanelDist[i]==-2)	strong = "-Sine";
			m_ctrlPanelList.SetItemText(i,10,strong);
		}
		else
		{
			strong = " ";
			m_ctrlPanelList.SetItemText(i,7,strong);
			m_ctrlPanelList.SetItemText(i,8,strong);
			m_ctrlPanelList.SetItemText(i,9,strong);
			m_ctrlPanelList.SetItemText(i,10,strong);
		}
	}
}	


CString CWingDlg::GetFormat(double f, int iPrecision)
{
 	CString str, str1;
	if ((f==0.0 || abs(f)>pow(10.0, -iPrecision)) && f <1000.0){
		switch (iPrecision){
			//there probably is a more elegant way to do this,
			case 0:
				str.Format("%.0f", f);
				break;
			case 1:
				str.Format("%.1f", f);
				break;
			case 2:
				str.Format("%.2f", f);
				break;
			case 3:
				str.Format("%.3f", f);
				break;
			case 4:
				str.Format("%.4f", f);
				break;
			case 5:
				str.Format("%.5f", f);
				break;
			case 6:
				str.Format("%.6f", f);
				break;
			case 7:
				str.Format("%.7f", f);
				break;
			case 8:
				str.Format("%.8f", f);
				break;
			case 9:
				str.Format("%.9f", f);
				break;
			default:
				str.Format("%.2f", f);
		}
	}
	else if(f>=1000.0){
		int q, r, exp;
		exp = (int)log10(f);
		r = exp%3;
		q = (exp-r)/3;

		CString strong;
		strong.Format("%.0f", f);

		switch (iPrecision)
		{
			//there probably is a more elegant way to do this,
			case 0:
				strong.Format("%.0f", f);
				break;
			case 1:
				strong.Format("%.1f", f);
				break;
			case 2:
				strong.Format("%.2f", f);
				break;
			case 3:
				strong.Format("%.3f", f);
				break;
			case 4:
				strong.Format("%.4f", f);
				break;
			case 5:
				strong.Format("%.5f", f);
				break;
			case 6:
				strong.Format("%.6f", f);
				break;
			case 7:
				strong.Format("%.7f", f);
				break;
			case 8:
				strong.Format("%.8f", f);
				break;
			case 9:
				strong.Format("%.9f", f);
				break;
			default:
				strong.Format("%.2f", f);
		}

		int l = strong.GetLength();
		int pos = strong.Find(".");
		if(pos<0) pos = l;
		pos = l-pos;

		for (int i=0; i<q; i++)
		{
			strong.Insert(l-3*(i+1)-i-pos," ");
			l++;
		}

		str = strong;
	}
	else 
	{
		int exp  = (int)log10(f)-1;
		str1.Format("e%d", exp);

		float main = (float)(f/pow(10.0, exp)*1.000001);
		switch (iPrecision)
		{
			case 0:
				str.Format("%.0f", main);
				break;
			case 1:
				str.Format("%.1f", main);
				break;
			case 2:
				str.Format("%.2f", main);
				break;
			case 3:
				str.Format("%.3f", main);
				break;
			case 4:
				str.Format("%.4f", main);
				break;
			case 5:
				str.Format("%.5f", main);
				break;
			default:
				str.Format("%.2f", main);
		}
		str += str1;
	}
 	return str;
}

void CWingDlg::OnLvnItemchangedPanellist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_iSection = pNMLV->iItem;
	InvalidateRect(&m_DrawRect);
	*pResult = 0;
}


/*
void CWingDlg::OnNMRClickPanelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSection = pNMListView->iItem;


	*pResult = 0;
	POSITION pos = m_ctrlPanelList.GetFirstSelectedItemPosition();
	m_iSection = m_ctrlPanelList.GetNextSelectedItem(pos);

	InvalidateRect(&m_DrawRect);
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTXWNGMENU))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	pt.x, pt.y, this); 
	}
}
*/

void CWingDlg::OnLvnEndLabelEditPanelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = (NMLVDISPINFO*)pNMHDR;

	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}
	m_iSection = pDispInfo->item.iItem;
	
	ReadSectionData(pDispInfo->item.iItem);
    m_bChanged = true;

	FillCell(pDispInfo->item.iItem, pDispInfo->item.iSubItem);
//	FillPanelList();
//	m_ctrlPanelList.SetItemState(m_iSection, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	ComputeGeometry();
	SetResults();
	InvalidateRect(&m_DrawRect);
}



void CWingDlg::SetSectionData()
{
	CString strong;

 	strong.Format("%.2f",m_DTPos[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,0, strong);

	strong.Format("%.2f",m_DTChord[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,1, strong);

	strong.Format("%.2f",m_DTOffset[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,2, strong);

	strong.Format("%.2f",m_pWing->m_TDihedral[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,3, strong);

	strong.Format("%.2f",m_pWing->m_TTwist[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,3, strong);

	if(m_bRightSide)   m_ctrlPanelList.SetItemText(m_iSection,5,m_pWing->m_RFoil[m_iSection]);
	else               m_ctrlPanelList.SetItemText(m_iSection,5,m_pWing->m_LFoil[m_iSection]);

}

void CWingDlg::ReadSectionData(int sel)
{
	double d;
	int res, k;
	CString strong;

	strong = m_ctrlPanelList.GetItemText(sel,1);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_DTPos[sel] =d;

	strong = m_ctrlPanelList.GetItemText(sel,2);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_DTChord[sel] =d;

	strong = m_ctrlPanelList.GetItemText(sel,3);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_DTOffset[sel] =d;

	strong = m_ctrlPanelList.GetItemText(sel,4);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_pWing->m_TDihedral[sel] =d;

	strong = m_ctrlPanelList.GetItemText(sel,5);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_pWing->m_TTwist[sel] =d;

	if(m_bRightSide)	m_pWing->m_RFoil[sel] = m_ctrlPanelList.GetItemText(sel,6);
	else                m_pWing->m_LFoil[sel] = m_ctrlPanelList.GetItemText(sel,6);

	strong = m_ctrlPanelList.GetItemText(sel,7);
	strong.Replace(" ","");
	res = sscanf(strong, "%d",&k);
	if(res==1) m_pWing->m_NXPanels[sel] =max(0,k);
	m_pWing->m_NXPanels[sel] = min(m_pWing->m_NXPanels[sel], MAXCHORDPANELS);

	strong = m_ctrlPanelList.GetItemText(sel,8);
	strong.Replace(" ","");
	if(strong=="Uniform")		m_pWing->m_XPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_pWing->m_XPanelDist[sel] = 1;
	else if(strong=="Sine")		m_pWing->m_XPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_pWing->m_XPanelDist[sel] = -2;

	strong = m_ctrlPanelList.GetItemText(sel,9);
	strong.Replace(" ","");
	res = sscanf(strong, "%d",&k);
	if(res==1) m_pWing->m_NYPanels[sel] = k;

	strong = m_ctrlPanelList.GetItemText(sel,10);
	strong.Replace(" ","");
	if(strong=="Uniform")		m_pWing->m_YPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_pWing->m_YPanelDist[sel] = 1;
	else if(strong=="Sine")		m_pWing->m_YPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_pWing->m_YPanelDist[sel] = -2;

	//Update Geometry
	Convert(false);// retrieve the data
	//
}


void CWingDlg::ReadParams()
{
//	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	m_ctrlWingName.GetWindowText(m_pWing->m_WingName);
	ReadSectionData(m_iSection);

	//Update Geometry
	Convert(false);// retrieve the data
	//
}


void CWingDlg::OnUpdateMeshFromTotal() 
{
	int Total = m_ctrlVLMPanels.GetValue();	
	VLMSetAutoMesh(Total);
	FillPanelList();
	SetResults();
	InvalidateRect(m_DrawRect);
	m_ctrlOK.SetFocus();
}



void CWingDlg::CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB,
                          double *xPointA, double *xPointB, int &NXLead, int &NXFlap)
{
	// the chordwise panel distribution is set i.a.w. with the flap hinges;

	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	
	if(pFoilA->m_bTEFlap) xHingeA=pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(pFoilB->m_bTEFlap) xHingeB=pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * NXPanels); 
	NXFlapB = (int)((1.0-xHingeB) * NXPanels);
	if(pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;
	NXLeadA = NXPanels - NXFlapA;
	NXLeadB = NXPanels - NXFlapB;

	NXFlap  = max(NXFlapA, NXFlapB);
	if(NXFlap>NXPanels/2) NXFlap=(int)NXPanels/2;
	NXLead  = NXPanels - NXFlap;

	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(XDist==1) 
			xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else 
			xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}
	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(XDist==1)
			xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}
	
	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(XDist==1) 
			xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else 
			xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}
	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(XDist==1)
			xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}
	
	xPointA[NXPanels] = 0.0;
	xPointB[NXPanels] = 0.0;
}










