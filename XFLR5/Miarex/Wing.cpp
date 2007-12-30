/****************************************************************************

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

// Wing.cpp : implementation file
// This class is both the interface class associated to the wing definition dialog box
// and the class which implements all calculation routines associated to the wing,
// including LLT and VLM methods
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../misc/UnitsDlg.h"
#include "Miarex.h"
#include "VLMDlg.h"
#include "Surface.h"
#include  <math.h>
#include ".\wing.h"



/////////////////////////////////////////////////////////////////////////////
// CWing dialog

double CWing::m_DTChord[MAXPANELS+1];	// Chords used for display, with converted units
double CWing::m_DTOffset[MAXPANELS+1];	// Offset used for display, with converted units
double CWing::m_DTPos[MAXPANELS+1];	// Panel positions used for display, with converted units
CWnd* CWing::s_pFrame;		//pointer to the Frame window
CWnd* CWing::s_pMiarex;	//pointer to the Miarex Application window
CWnd* CWing::s_pVLMDlg;	//pointer to the VLM analysis dialog class
CWnd* CWing::s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class


CWing::CWing(CWnd* pParent /*=NULL*/)
	: CDialog(CWing::IDD, pParent)
{
	//CONSTRUCTOR
	// Variable initialization

	//{{AFX_DATA_INIT(CWing)
	//}}AFX_DATA_INIT
	memset(m_Ai, 0, sizeof(m_Ai));
	memset(m_Twist, 0, sizeof(m_Twist));
	memset(m_Cl, 0, sizeof(m_Cl));
	memset(m_PCd, 0, sizeof(m_PCd));
	memset(m_ICd, 0, sizeof(m_ICd));
	memset(m_Cm, 0, sizeof(m_Cm));
	memset(m_CmAirf, 0, sizeof(m_CmAirf));
	memset(m_CmGeom, 0, sizeof(m_CmGeom));
	memset(m_XCPSpanAbs, 0, sizeof(m_XCPSpanAbs));
	memset(m_XCPSpanRel, 0, sizeof(m_XCPSpanRel));
	memset(m_Re, 0, sizeof(m_Re));
	memset(m_Chord, 0, sizeof(m_Chord));
	memset(m_Offset, 0, sizeof(m_Offset));
	memset(m_XTrTop, 0, sizeof(m_XTrTop));
	memset(m_XTrBot, 0, sizeof(m_XTrBot));
	memset(m_BendingMoment, 0, sizeof(m_BendingMoment));
	memset(m_Twist, 0, sizeof(m_Twist));
	memset(m_xPanel, 0, MAXCHORDPANELS*sizeof(double));
	memset(m_xHinge, 0, MAXCHORDPANELS*sizeof(double));
	memset(m_xDist, 0, MAXCHORDPANELS*sizeof(int));

	memset(m_TPos, 0, sizeof(m_TPos));
	memset(m_TYProj, 0, sizeof(m_TYProj));
	memset(m_TChord, 0, sizeof(m_TChord));
	memset(m_TLength, 0, sizeof(m_TLength));
	memset(m_TOffset, 0, sizeof(m_TOffset));
	memset(m_TDihedral, 0, sizeof(m_TDihedral));
	memset(m_TZPos, 0, sizeof(m_TZPos));
	memset(m_TTwist, 0, sizeof(m_TTwist));
	memset(m_DTPos, 0, sizeof(m_DTPos));
	memset(m_DTChord, 0, sizeof(m_DTChord));
	memset(m_DTOffset, 0, sizeof(m_DTOffset));
	memset(m_SpanPos, 0, sizeof(m_SpanPos));

	memset(m_NXPanels, 0, sizeof(m_NXPanels));
	memset(m_NYPanels, 0, sizeof(m_NYPanels));
	memset(m_XPanelDist, 0, sizeof(m_XPanelDist));
	memset(m_YPanelDist, 0, sizeof(m_YPanelDist));
	memset(m_VLMQInf, 0, sizeof(m_VLMQInf));
	memset(m_GeomMoment, 0, sizeof(m_GeomMoment));

	m_bIsFin        = false;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bDoubleSymFin = false;
	m_bTrans        = false;
	m_bLLT          = false;
	m_bVLM1         = true;
	m_bVLMAutoMesh  = true;
	m_bCancel       = false;
	m_bInitCalc     = false;
	m_bSymetric     = true;
	m_bTrace        = true;
	m_bCheckName    = true;
	m_bWingOut      = false;
	m_bConverged    = true;
	m_bRightSide    = true;
	m_bVLMSymetric  = false;
	m_bChanged      = false;

	m_WingName  = "Wing Name";
	m_WingColor =  RGB(0,130,130);

	m_QInf     = 0.0;
	m_QInf0    = 0.0;
	m_Weight   = 0.0;
	m_Maxa     = 0.0;

	m_pXFile     = NULL;
	m_pPanel     = NULL;
	m_pNode      = NULL;
	m_pWakeNode  = NULL; 
	m_pWakePanel = NULL;

	m_CL                = 0.0;
	m_ViscousDrag       = 0.0;
	m_InducedDrag       = 0.0;
	m_RollingMoment     = 0.0;
	m_PitchingMoment    = 0.0;
	m_GYm               = 0.0;
	m_IYm               = 0.0;
	m_VCm               = 0.0;
	m_GCm               = 0.0;
	m_XCP               = 0.0;
	m_YCP               = 0.0;

	m_Type = 1;
	m_Viscosity = 0.0;
	m_Density   = 0.0;

	m_FixedFont.CreatePointFont(85, "Courier New");

	m_AVLIndex = -987654;//improbable value...

	m_ptOffset.x = 50;
	m_ptOffset.y = 350;
	m_fWingScale = -1.0;

	m_iSection   = 0;
	m_nNodes     = 0;
	m_MatSize    = 0;

	pi = 3.141592654;

	m_NStation  = 20;

	m_NSurfaces = 0;

	m_Alpha      = 0.0;
	m_Span       = 0.0;
	m_CvPrec     = 0.01;
	m_Area       = 0.0;
	m_Volume     = 0.0;
	m_AR         = 0.0;// Aspect ratio
	m_TR         = 0.0;// Taper ratio
	m_GChord     = 0.0;// mean geometric chord
	m_MAChord    = 0.0;// mean aero chord
	m_yMac       = 0.0;
	m_RelaxMax   = 10.0;
	m_XCmRef     = 0.0;//=m_TChord[0]/4
	m_SineFactor = 1.0;

	m_NPanel        =  1;
	m_nFlaps        =  0;
	m_NYPanels[0]   = 12;
	m_NXPanels[0]   =  8;
	m_YPanelDist[0] = -1;
	m_NHinges       =  0;

	m_TChord[0]  =  .180;
	m_TChord[1]  =  .120;
	m_TLength[0] =  .0;
	m_TLength[1] = 1.0;
	m_TOffset[0] = 0.;
	m_TOffset[1] = 0.060;

	m_PointDown.SetPoint(0,0);
	m_DrawRect.SetRect(0,0,0,0);

	double length = m_TLength[0];
	for (int i=0; i<=MAXPANELS; i++)
	{
		length += m_TLength[i];
		m_TPos[i]     = length;
		m_DTPos[i]    = m_TPos[i];
		m_DTChord[i]  = m_TChord[i];
		m_DTOffset[i] = m_TOffset[i];	
		m_XPanelDist[i] =  1;
	}

	m_RFoil.SetSize(MAXPANELS+1);
	for (i=0;i<=MAXPANELS; i++){
		m_RFoil[i]    = "";
	}

	m_LFoil.SetSize(MAXPANELS+1);
	for (i=0;i<=MAXPANELS; i++){
		m_LFoil[i]    = "";
	}
}

void CWing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SINEFACTOR, m_ctrlSineFactor);
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
}


BEGIN_MESSAGE_MAP(CWing, CDialog)
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
	ON_NOTIFY(NM_CLICK, IDC_PANELLIST, OnNMClickPanelList)
	ON_NOTIFY(NM_RCLICK, IDC_PANELLIST, OnNMRClickPanelList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PANELLIST, OnLvnEndLabelEditPanelList)
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_RESETSCALE, OnResetscale)
	ON_EN_KILLFOCUS(IDC_SINEFACTOR, OnKillFocusSinefactor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWing message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CWing::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWnd = GetFocus();
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		if(&m_ctrlVLMPanels == pWnd)
		{
			OnUpdateMeshFromTotal();
			return true;
		}
		if(&m_ctrlSineFactor == pWnd)
		{
			m_ctrlOK.SetFocus();
			return true;
		}
		else if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd)
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


BOOL CWing::OnInitDialog() 
{
	//Initializes the dialog box with the stored data

	CDialog::OnInitDialog();

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	if(m_bIsFin) SetWindowText("Fin Design");
	else         SetWindowText("Wing Design");

	m_bRightSide = true;

	m_ctrlWingColor.SetColor(m_WingColor);

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

	m_ctrlWingName.SetWindowText(m_WingName);
	if(!m_bCheckName) m_ctrlWingName.EnableWindow(false);
	m_ctrlWingName.SetSel(0,-1);
	m_ctrlWingName.SetFocus();

	CheckRadioButton(IDC_SIDE1, IDC_SIDE2, IDC_SIDE2);
	if(m_bSymetric) m_ctrlSymetric.SetCheck(TRUE);
	OnSymetric();

	ComputeGeometry();

	bool bResetMesh = false;
	for (int i=0;i<m_NPanel; i++){
		if(m_NXPanels[i]>MAXCHORDPANELS || m_NXPanels[i]<=0) 
			bResetMesh = true;
	}
	if(bResetMesh || m_bVLMAutoMesh) VLMSetAutoMesh();


	int exp = (int)log10(m_Span  * pFrame->m_mtoUnit);
	int pres = 4-exp;
	pres = max(pres, 2);

	CRect CltRect;

	len =" ("+len+")";
	m_ctrlPanelList.GetStyle();
	m_ctrlPanelList.GetClientRect(&CltRect);
	int w = CltRect.Width();
	int w1 = (int)((w-15)/11);
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
	m_ctrlPanelList.m_pWing = this;
	
	m_ctrlSineFactor.SetMin(0.0);
	m_ctrlSineFactor.SetMax(1.0);
	m_ctrlSineFactor.SetValue(m_SineFactor);

	m_ctrlPanelList.m_strList.RemoveAll();

	for(i=0; i<pFrame->m_oaFoil.GetSize(); i++){
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
	m_DrawRect.top    = CltRect.top+270;
	m_DrawRect.bottom = CltRect.bottom-20;

	m_ptOffset.x = 50;
	m_ptOffset.y = 330;

	SetScale();

	if(m_bIsFin) {
		m_ctrlSymetric.EnableWindow(false);
	}

	m_bChanged = false;

	m_ctrlPanelList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ctrlPanelList.SetFocus();

	return FALSE; 
}



void CWing::OnResetscale()
{
	//Resets the wing's display scale

	m_ptOffset.x = 50;
	m_ptOffset.y = 330;
	SetScale();
	InvalidateRect(&m_DrawRect);
}


void CWing::SetResults()
{
	//Updates the wing's properties after a change of geometry

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CString str;

	str.Format("%7.2f", m_Area*pFrame->m_m2toUnit);
	m_ctrlSurface.SetWindowText(str);

	str.Format("%5.2e", m_Volume*pFrame->m_mtoUnit*pFrame->m_mtoUnit*pFrame->m_mtoUnit);
	m_ctrlVolume.SetWindowText(str);

	str.Format("%5.2f", m_Span*pFrame->m_mtoUnit);
	m_ctrlSpan.SetWindowText(str);

	str.Format("%5.2f", m_GChord*pFrame->m_mtoUnit);
	m_ctrlMeanChord.SetWindowText(str);

	str.Format("%5.2f", m_MAChord*pFrame->m_mtoUnit);
	m_ctrlAeroChord.SetWindowText(str);

	str.Format("%5.2f", m_yMac*pFrame->m_mtoUnit);
	m_ctrlYMac.SetWindowText(str);

	str.Format("%5.2f", m_AR);
	m_ctrlAspectRatio.SetWindowText(str);

	if(m_TChord[m_NPanel]>0.0) str.Format("%.2f", m_TR);
	else str.Format("Undefined");
	m_ctrlTaperRatio.SetWindowText(str);

	str.Format("%5.2f", GetAverageSweep());
	m_ctrlSweep.SetWindowText(str);

	m_ctrlVLMPanels.SetValue(VLMGetPanelTotal());
}


void CWing::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Message processing for a left-click

	if(m_DrawRect.PtInRect(point))
	{
		int res = IsFoil(point);

		if(res>=0){
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



void CWing::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Display the context menu

	CPoint pt = point;
	ScreenToClient(&pt);
//	GetCursorPos(&pt);

	if(m_DrawRect.PtInRect(pt)){
		int res = IsFoil(pt);

		if(res>=0){
			m_iSection = res;
		}
		if(m_iSection>=0){

			m_ctrlPanelList.SetItemState(res, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			InvalidateRect(&m_DrawRect);

			CMenu menu;
			if (menu.LoadMenu(IDR_CTXWNGMENU)){
				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					point.x, point.y, this); 
			}

		}
	}
}


void CWing::OnMouseMove(UINT nFlags, CPoint point) 
{
	//Message processing for a mouse movement

	SHORT shZ  = GetKeyState(90);

	if(m_bTrans){
		// we translate the wing
		m_ptOffset.x += point.x - m_PointDown.x;
		m_ptOffset.y += point.y - m_PointDown.y;

		m_PointDown = point;
		InvalidateRect(&m_DrawRect);
	}
	else if ((nFlags & MK_MBUTTON) ||(shZ & 0x8000)) {
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

void CWing::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//the user has released the left button
	m_bTrans = false;	
//	Invalidate();
//	CDialog::OnLButtonUp(nFlags, point);
}


void CWing::OnOK() 
{
	// the user has clicked the OK button

	ReadParams(); 
	if(!CheckWing()) return;
	m_ctrlWingName.GetWindowText(m_WingName);
	
	if(m_bSymetric){
		for (int i=0; i<=m_NPanel; i++){
			m_LFoil[i]   = m_RFoil[i];
			
		}
	}

	m_bVLMAutoMesh = false;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	CDialog::OnOK();
}


void CWing::InsertSection(double TPos)
{
	// Inserts a section before an existing sections
	// If the section already exists, does nothing and returns
	// Otherwise copies the data from section n+1
	// and inserts a mid panel

	if(TPos<=0.0) return;

	bool bIsInserted = false;
	
	for (int n=1; n<=m_NPanel; n++){
		if(abs(TPos-m_TPos[n])<0.0000001) {
			break;
		}
		if(TPos < m_TPos[n]){
			for (int m=m_NPanel;m>=n; m--){
				m_TPos[m+1]      = m_TPos[m];
				m_TChord[m+1]    = m_TChord[m];
				m_TOffset[m+1]   = m_TOffset[m];
				m_TZPos[m+1]     = m_TZPos[m];
				m_TDihedral[m+1] = m_TDihedral[m];
				m_TTwist[m+1]    = m_TTwist[m];
				m_RFoil[m+1]     = m_RFoil[m];
				m_LFoil[m+1]     = m_LFoil[m];
				m_NXPanels[m+1]  = m_NXPanels[m];
				m_NYPanels[m+1]  = m_NYPanels[m];
				m_XPanelDist[m+1] = m_XPanelDist[m];
				m_YPanelDist[m+1] = m_YPanelDist[m];
			}
			double tau = (TPos-m_TPos[n-1])/(m_TPos[n]-m_TPos[n-1]);
			m_TPos[n]      = TPos;
			m_TChord[n]    = m_TChord[n-1]    * (1-tau) + m_TChord[n]    * tau;
			m_TOffset[n]   = m_TOffset[n-1]   * (1-tau) + m_TOffset[n]   * tau;
			m_TZPos[n]     = m_TZPos[n-1]     * (1-tau) + m_TZPos[n]     * tau;
			m_TTwist[n]    = m_TTwist[n-1]    * (1-tau) + m_TTwist[n]    * tau;
			m_TDihedral[n] = m_TDihedral[n-1] * (1-tau) + m_TDihedral[n] * tau;
			
			m_RFoil[n]     = m_RFoil[n-1];
			m_LFoil[n]     = m_LFoil[n-1];
			m_NXPanels[n]  = m_NXPanels[n-1];
			m_NYPanels[n]  = m_NYPanels[n-1];
			m_XPanelDist[n] = m_XPanelDist[n-1];
			m_YPanelDist[n] = m_YPanelDist[n-1];
			bIsInserted  = true;
			m_NPanel++;
			break;
		}
	}
}


void CWing::InsertSection(double TPos, double TChord, double TOffset,
						  double TZPos, double Twist, CString Foil,
						  int NChord, int NSpan, int SSpan)
{
	// Inserts a section before an existing sections
	// If the section already exists, does nothing and returns
	// Otherwise copies the data from section n+1
	// and inserts a mid panel

	bool bIsInserted = false;
	
	for (int n=0; n<=m_NPanel; n++){
		if(abs(TPos-m_TPos[n])<0.0000001) {

			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_XPanelDist[n] = 1;
			m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			break;
		}
		if(TPos < m_TPos[n]){
			for (int m=m_NPanel;m>=n; m--){
				m_TPos[m+1]      = m_TPos[m];
				m_TChord[m+1]    = m_TChord[m];
				m_TOffset[m+1]   = m_TOffset[m];
				m_TZPos[m+1]     = m_TZPos[m];
				m_TDihedral[m+1] = m_TDihedral[m];
				m_TTwist[m+1]    = m_TTwist[m];
				m_RFoil[m+1]     = m_RFoil[m];
				m_LFoil[m+1]     = m_LFoil[m];
				m_NXPanels[m+1]  = m_NXPanels[m];
				m_NYPanels[m+1]  = m_NYPanels[m];
				m_XPanelDist[m+1] = m_XPanelDist[m];
				m_YPanelDist[m+1] = m_YPanelDist[m];
			}
			m_TPos[n]      = TPos;
			m_TChord[n]    = TChord;
			m_TOffset[n]   = TOffset;
			m_TZPos[n]     = TZPos;
			m_TDihedral[n] = 180.0/pi *
							 atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
			m_TTwist[n]    = Twist;
			m_RFoil[n]     = Foil;
			m_LFoil[n]     = Foil;
			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			m_NPanel++;
			break;
		}
	}
	if(!bIsInserted){
		if(TPos > m_TPos[m_NPanel]){
			n = m_NPanel +1;
			m_TPos[n]    = TPos;
			m_TChord[n]  = TChord;
			m_TOffset[n] = TOffset;
			m_TZPos[n]   = TZPos;
			m_TDihedral[n-1] = 180.0/pi *
							   atan2(m_TZPos[n]-m_TZPos[n-1], m_TPos[n]-m_TPos[n-1]);
			m_TTwist[n]  = Twist;
			m_RFoil[n]   = Foil;
			m_LFoil[n]   = Foil;
			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_YPanelDist[n] = SSpan;
			m_NPanel++;
		}
		else {//no need to overwrite
		}
	}
}


bool CWing::SerializeWing(CArchive& ar) 
{
	//saves or loads the wing data to the archive ar

	int i;
//	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	int ArchiveFormat;// identifies the format of the file
	if (ar.IsStoring())
	{	// storing code
		ar << 1007;
			//1007 : Changed length units to m
			//1006 : Added Wing Color v2.99-15
			//1005 : Added Chordwise spacing (v2.99-00)
			//1004 : corrected m_NXPanels, m_NYPanels, m_YPanelDist to int (v1.99-33)
			//1003 : AVL Format (v1.99-18)
			//1002 : save VLM Mesh (v1.99-12)
			//1001 : initial format

		ar << m_WingName;
		ar << 0; //non elliptic...

		if(m_bSymetric) ar << 1; else ar<<0;

		ar << m_NPanel;

		for (i=0; i<=m_NPanel; i++) ar << m_RFoil[i];
		for (i=0; i<=m_NPanel; i++) ar << m_LFoil[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TChord[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TPos[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TOffset[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TDihedral[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TTwist[i];

		ar << (float)m_XCmRef;
		
		if(m_bVLMAutoMesh) ar << 1; else ar<<0;
		for (i=0; i<=m_NPanel; i++) ar << m_NXPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_NYPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_XPanelDist[i];//1005
		for (i=0; i<=m_NPanel; i++) ar << m_YPanelDist[i];

		ar << m_WingColor;
		return true;
	}
	else
	{	// loading code
		float f;
		int k;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 || ArchiveFormat>1100) {
			m_WingName = "";
			return false;
		}

		ar >> m_WingName;
		if (m_WingName.GetLength() ==0) {
			return false;
		}
		ar >> k;
		if(k!=0){
			m_WingName = "";
			return false;
		}
		ar >> k;
		if (k==1) m_bSymetric = true;
		else if (k==0) m_bSymetric = false;
		else{
			m_WingName = "";
			return false;
		}
		m_bVLMSymetric = m_bSymetric;

		ar >> m_NPanel;
		if(m_NPanel <=0 || m_NPanel>=100) {
			m_WingName = "";
			return false;
		}
		for (i=0; i<=m_NPanel; i++) ar >> m_RFoil[i];
		for (i=0; i<=m_NPanel; i++) ar >> m_LFoil[i];
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TChord[i]=f;
			if (m_TChord[i] <0.0) {
				m_WingName = "";
				return false;
			}
		}

		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TPos[i]=f;
			if (m_TPos[i] <0.0) {
				m_WingName = "";
				return false;
			}
		}
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TOffset[i]=f;
		}

		if(ArchiveFormat<1007){
			//convert mm to m
			for (i=0; i<=m_NPanel; i++) {
				m_TPos[i]    /= 1000.0;
				m_TChord[i]  /= 1000.0;
				m_TOffset[i] /= 1000.0;
			}

		}
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TDihedral[i]=f;
		}
		for (i=0; i<=m_NPanel; i++)	{
			ar >> f; m_TTwist[i]=f;
		}

		ar >> f; m_XCmRef=f;
		if(ArchiveFormat<1007) m_XCmRef /= 1000.0;

		ComputeGeometry();

		ar >> k;
		if (k==1)      m_bVLMAutoMesh = true;
		else if (k==0) m_bVLMAutoMesh = false;
		else{
			m_WingName = "";
			return false;
		}

		for (i=0; i<=m_NPanel; i++) {
			if(ArchiveFormat<=1003){
				ar >> f;
				m_NXPanels[i] = (int)f;
			}
			else 
				ar >> m_NXPanels[i];
			m_NXPanels[i] = max(1,m_NXPanels[i] );
			m_NXPanels[i] = min(MAXCHORDPANELS, m_NXPanels[i]);
		}
		for (i=0; i<=m_NPanel; i++) {
			if(ArchiveFormat<=1003){
				ar >> f;
				m_NYPanels[i] = (int)f;
			}
			else 	ar >> m_NYPanels[i];
			m_NYPanels[i] = max(1,m_NYPanels[i] );
			m_NYPanels[i] = min(50, m_NYPanels[i]);
		}
		int total = 0;
		for (i=0; i<m_NPanel; i++){
			total += m_NYPanels[i];
		}

		if(total*2>=MAXSTATIONS){
			double ratio = MAXSTATIONS/total/2.0;
			for (i=0; i<=m_NPanel; i++) {
				m_NYPanels[i] = (int) (ratio*m_NYPanels[i]);
			}
		}

		int spanpos = 0;
		int vlmpanels = 0;
		for (int l=0; l<m_NPanel; l++){
			spanpos += m_NYPanels[l];
			vlmpanels +=m_NXPanels[l]*m_NYPanels[l];
		}


		if (ArchiveFormat >=1005){
			for (i=0; i<=m_NPanel; i++) ar >> m_XPanelDist[i];
		}

		for (i=0; i<=m_NPanel; i++) {
			ar >> m_YPanelDist[i];
			if (ArchiveFormat <1003){
				if (m_YPanelDist[i] ==  2) m_YPanelDist[i] = -2;
				if (m_YPanelDist[i] == -2) m_YPanelDist[i] =  2;
			}
		}

		if(ArchiveFormat>=1006){
			ar >> m_WingColor;
		}
		return true;
	}
}


void CWing::ComputeDihedrals()
{
	for (int n=0; n<m_NPanel; n++){
		m_TDihedral[n] = 180.0/pi* atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
	}
}

void CWing::Duplicate(CWing *pWing)
{
	//Copies the wing data froman existing wing

	s_pFrame		= pWing->s_pFrame;
	s_pMiarex       = pWing->s_pMiarex;
	m_NStation		= pWing->m_NStation;
	m_NPanel		= pWing->m_NPanel;
	m_Span			= pWing->m_Span;
	m_CvPrec		= pWing->m_CvPrec;
	m_Area		    = pWing->m_Area;
	m_AR			= pWing->m_AR;
	m_TR			= pWing->m_TR;
	m_GChord		= pWing->m_GChord;
	m_MAChord		= pWing->m_MAChord;
	m_RelaxMax		= pWing->m_RelaxMax;
	m_bTrace		= pWing->m_bTrace;
	m_WingName		= pWing->m_WingName;
	m_bVLMAutoMesh	= pWing->m_bVLMAutoMesh;
	m_bSymetric     = pWing->m_bSymetric;
	m_bIsFin        = pWing->m_bIsFin;
	m_bSymFin       = pWing->m_bSymFin;
	m_bDoubleFin    = pWing->m_bDoubleFin;
	m_bDoubleSymFin = pWing->m_bDoubleSymFin;

	for (int i=0; i<=MAXPANELS; i++){
		m_TChord[i]     = pWing->m_TChord[i];
		m_TPos[i]       = pWing->m_TPos[i];
		m_TOffset[i]    = pWing->m_TOffset[i];
		m_TLength[i]    = pWing->m_TLength[i];
		m_NXPanels[i]   = pWing->m_NXPanels[i] ;
		m_NYPanels[i]   = pWing->m_NYPanels[i]; 
		m_XPanelDist[i] = pWing->m_XPanelDist[i]; 
		m_YPanelDist[i] = pWing->m_YPanelDist[i]; 
		m_RFoil[i]      = pWing->m_RFoil[i];
		m_LFoil[i]      = pWing->m_LFoil[i];
		m_TTwist[i]     = pWing->m_TTwist[i];
		m_TDihedral[i]  = pWing->m_TDihedral[i];
		m_TZPos[i]      = pWing->m_TZPos[i];
		m_TYProj[i]     = pWing->m_TYProj[i];
	}
}

void CWing::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)s_pMiarex;
	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	int i, k;

	double surface = 0.0;
	m_TLength[0] = 0.0;
	m_TYProj[0]  = m_TPos[0];
	for (i=1; i<=m_NPanel; i++){
		m_TLength[i] = m_TPos[i] - m_TPos[i-1];
	}
	for (i=1; i<=m_NPanel; i++){
		m_TYProj[i] = m_TYProj[i-1] + m_TLength[i] * cos(m_TDihedral[i-1]*pi/180.0);
	}

	m_Span    = 2.0 * m_TPos[m_NPanel]; 
	m_MAChord = 0.0;
	m_yMac    = 0.0;
	m_Volume  = 0.0;
	for (k=0; k<m_NPanel; k++){
		pFoilA = pFrame->GetFoil(m_RFoil[k]);
		pFoilB = pFrame->GetFoil(m_RFoil[k+1]);
		surface   += m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0;//m²
		if(pFoilA && pFoilB) 
			m_Volume  += m_TLength[k+1]*(pFoilA->GetArea()*m_TChord[k] + pFoilB->GetArea()*m_TChord[k+1])/2.0;//m3
		m_MAChord += IntegralC2(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
		m_yMac    += IntegralCy(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
	}
	if(!m_bIsFin || m_bSymFin || m_bDoubleFin)
	{
		m_Area    = 2.0 * surface;
		m_Volume *= 2.0;
		m_MAChord = m_MAChord * 2.0 / m_Area;
		m_yMac    = m_yMac          / m_Area;

		m_GChord  = m_Area/m_Span;
		m_AR      = m_Span*m_Span/m_Area;
	}
	else
	{ 
		m_Area = surface;
		m_MAChord = m_MAChord / m_Area;
		m_yMac    = m_yMac    / m_Area;

		m_GChord  = m_Area/m_Span*2.0;
		m_AR      = m_Span*m_Span/m_Area/2.0;
	}

	if(m_TChord[m_NPanel]>0.0)	m_TR = m_TChord[0]/m_TChord[m_NPanel];
	else						m_TR = 99999.0;

	//calculate the number of flaps
	m_nFlaps = 0;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span;

	for (i=1; i<=m_NPanel; i++)
	{
		pFoilA = pFrame->GetFoil(m_RFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_RFoil[i]);
		if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize){
			m_nFlaps++;
		}
		pFoilA = pFrame->GetFoil(m_LFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_LFoil[i]);
		if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize){
			m_nFlaps++;
		}
	}

}

double CWing::IntegralC2(double y1, double y2, double c1, double c2)
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


double CWing::IntegralCy(double y1, double y2, double c1, double c2)
{
	// Auxiliary integral used in LLT calculations

	double res = 0.0;

	if (abs(y2-y1)<1.e-5) return (y1+y2)/2.0 * (c1+c2)/2.0;

	double g = (c2-c1)/(y2-y1);

	res = (c1-g*y1)/2.0 *(y2*y2 - y1*y1) + g/3.0 * (y2*y2*y2-y1*y1*y1);

	return res;
}

void CWing::ComputeChords(int NStation)
{
	//Calculates the chord lengths for NStation in LLT calculations
	int j,k,l,m;
	double y, yob, tau;
	double x0,y0,y1,y2;
	double SpanPosition[MAXSTATIONS];
	CVector C;
		
	if(NStation !=0){//LLT based
		m_NStation = NStation;

		for (k=0; k<=NStation; k++){
			yob   = cos(k*pi/NStation);
			y = abs(yob * m_Span/2);
			for (l=0; l<=m_NPanel; l++){
				if(m_TPos[l] < y && y <=m_TPos[l+1]){
					tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
					m_Chord[k] = m_TChord[l]+(m_TChord[l+1]-m_TChord[l]) * tau;
					m_Offset[k] = m_TOffset[l]+(m_TOffset[l+1]-m_TOffset[l]) * tau;
					break;
				}
			}
		}
	}
	else
	{//VLM Mesh based
		m_NStation    = 0;
		m = 0;

		x0 = m_Surface[m_NSurfaces/2].m_LA.x;
		y0 = m_Surface[m_NSurfaces/2].m_LA.y;

		for (j=m_NSurfaces/2; j<m_NSurfaces; j++)
		{
			for (k=0; k<m_Surface[j].m_NYPanels; k++)
			{
				//calculate span positions at each station
				m_Surface[j].GetyDist(k, y1, y2);
				SpanPosition[m] = y0 + (y1+y2)/2.0*m_Surface[j].m_Length;
				m++;
			}
			y0+=m_Surface[j].m_Length;
		}

		m_NStation = 2*m;
		for (m=0; m<m_NStation/2; m++){
			m_SpanPos[m] = -SpanPosition[m_NStation/2-m-1];
			m_SpanPos[m+m_NStation/2] = SpanPosition[m];
		}

		m=0;
		for (j=0; j<m_NSurfaces; j++)
		{
			for (k=0; k<m_Surface[j].m_NYPanels; k++)
			{
				//calculate chords and offsets at each station
				m_Chord[m]  = m_Surface[j].GetChord(k);

				m_Surface[j].GetLeadingPt(C, k);
				m_Offset[m] = C.x-x0;
				
				m_Twist[m]  = m_Surface[j].GetTwist(k);
				m++;
			}
		}
		m_NStation = m;
	}
}


bool CWing::ExportAVLWing(CStdioFile *pXFile, double x, double z,
						  double Thetax, double Thetay, double bFin)
{ 
	// Exports the current wing to AVL format

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CStdioFile XFile;
	CFileException fe;
	int l;
	CString strong;

	pXFile->WriteString("#=================================================\n");
	pXFile->WriteString("SURFACE                      | (keyword)\n");
	pXFile->WriteString(m_WingName);
	pXFile->WriteString("\n");
	pXFile->WriteString("#Nchord    Cspace   [ Nspan Sspace ]\n");			

	strong.Format("%3d        %3.1f\n", m_NXPanels[0], 1.0);
	pXFile->WriteString(strong);


	pXFile->WriteString("\n");
	pXFile->WriteString("INDEX                        | (keyword)\n");
	pXFile->WriteString("111                          | Lsurf\n");

	if(!bFin){
		pXFile->WriteString("\n");
		pXFile->WriteString("YDUPLICATE\n");
		pXFile->WriteString("0.0\n");
	}

	pXFile->WriteString("\n");
	pXFile->WriteString("SCALE\n");
	pXFile->WriteString("1.0  1.0  1.0\n");

	pXFile->WriteString("\n");
	pXFile->WriteString("TRANSLATE\n");
	pXFile->WriteString("0.0  0.0  0.0\n");

	pXFile->WriteString("\n");
	pXFile->WriteString("ANGLE\n");
	strong.Format("%8.3f                         | dAinc\n", Thetay);
	pXFile->WriteString(strong);

	pXFile->WriteString("\n");
	pXFile->WriteString("#__________________________________\n");

	for (l=0; l<=m_NPanel; l++){
		pXFile->WriteString("SECTION                                          |  (keyword)\n");
		if(!bFin){
			strong.Format("%7.2f %7.2f %7.2f %7.2f %4.1f  %3d  %3d   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n",
				(x+m_TOffset[l])       *pFrame->m_mtoUnit,
				m_TPos[l]              *pFrame->m_mtoUnit,
				(z+GetZPos(m_TPos[l])) *pFrame->m_mtoUnit,
				m_TChord[l]            *pFrame->m_mtoUnit,
				m_TTwist[l],
				m_NYPanels[l],
				m_YPanelDist[l]);
		}
		else{
			strong.Format("%7.2f %7.2f %7.2f %7.2f %4.1f  %3d  %3d   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n",
				(x+m_TOffset[l])       *pFrame->m_mtoUnit,
				0.0,
				m_TPos[l]              *pFrame->m_mtoUnit,
				m_TChord[l]            *pFrame->m_mtoUnit,
				m_TTwist[l],
				m_NYPanels[l],
				m_YPanelDist[l]);
		}
		pXFile->WriteString(strong);
		pXFile->WriteString("\n");
		pXFile->WriteString("AFIL 0.0 1.0\n");
		pXFile->WriteString(m_RFoil[l]+".dat\n");
		pXFile->WriteString("\n");
		pXFile->WriteString("#______________\n");
	}

	return true;
}

 
double CWing::AlphaInduced(int k)
{
	// Calculates the induced angle from the lift coefficient and
	// from the Beta factor in LLT

	double ai = 0.0;

	for (int m=1; m<m_NStation; m++){
		ai += Beta(m,k) * m_Cl[m] * m_Chord[m]/m_Span;
	}
	return ai;
}


double CWing::Beta(int m, int k)
{
	//Auxiliary calculation of the Beta factor in LLT
	double b;
	double fk = (double)k;
	double fm = (double)m;
	double fr = (double)m_NStation;

	if (m==k) b = 180.0*fr/8.0/pi/sin(fk*pi/fr);
	else if (IsEven(m+k)) b=0.0;
	else {
		double c1 = 180.0/4.0/pi/fr/sin(fk*pi/fr);
		double c2 =   1.0/(1.0-cos((fk+fm)*pi/fr)) 
					- 1.0/(1.0-cos((fk-fm)*pi/fr));
		b = c1 * c2;
	}
	return b;
}


double CWing::Eta(int m)
{
	//Auxiliary calculation of the Eta factor in LLT

	return pi/2.0/(double)m_NStation * sin((double)m*pi/(double)m_NStation) ;
}


double CWing::Sigma(int m)
{
	//Auxiliary calculation of the sigma factor in LLT

	return pi/8.0/(double)m_NStation * sin(2.*(double)m*pi/(double)m_NStation) ;
}


void CWing::LLTInitCl()
{
	//Initializes the Reynolds number and lift coefficient for the initial iteration in LLT

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CFoil *pFoil0 = NULL;
	CFoil *pFoil1 = NULL;
	double yob, tau;

	bool bOutRe, bError;

	for (int k=1; k<m_NStation; k++){
		yob   = cos(k*pi/m_NStation);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
		m_Cl[k] = pFrame->GetCl(pFoil0, pFoil1, m_Re[k],
								 m_Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
	}
	if(m_Type == 2){
		double Lift=0.0;// required for Type 2
		for (k=1; k<m_NStation; k++){
			Lift += Eta(k) * m_Cl[k] * m_Chord[k] /m_Span;
		}
		if(Lift<=0.0) return;
		m_QInf  = m_QInf0 / sqrt(Lift);
		for (k=1; k<m_NStation; k++){
			m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
		}
	}
}


void CWing::ScaleSpan(double NewSpan)
{
	// Scales the wing span-wise to the NewSpan value
	for (int i=0; i<=MAXPANELS; i++){
		m_TPos[i]      *= NewSpan/m_Span;
		m_TLength[i]   *= NewSpan/m_Span;
	}
	ComputeGeometry();
	Convert(true);

}

void CWing::ScaleChord(double NewChord)
{
	// Scales the wing chord-wise so that the root chord reaches the NewChord value

	double ratio = NewChord/m_TChord[0];
	for (int i=0; i<=MAXPANELS; i++){
		m_TChord[i]    *= ratio;
		m_TOffset[i]   *= ratio;
	}
	ComputeGeometry();
	Convert(true);

}

void CWing::SetScale()
{
	// Sets the wing scale for diplay in the wing edition dialog box

	double sc1 = 2.0*(m_DrawRect.Width()-130)/m_Span;
	double sc2 = (m_DrawRect.Height()-90)/m_TChord[0];
	double sc3 = (m_DrawRect.Height()-90)/(m_TChord[m_NPanel]+m_TOffset[m_NPanel]);
	m_fWingScale = __min(sc1, sc2);
	m_fWingScale = __min(m_fWingScale, sc3);
}


BOOL CWing::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
//	Message processing for a mouse wheel

//	point is in client coordinates
	if(zDelta<0) m_fWingScale *= 1.06;
	else m_fWingScale /= 1.06;

	InvalidateRect(&m_DrawRect);
	return TRUE;
		
//	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}



void CWing::OnSetFocus(CWnd* pOldWnd) 
{
//	Message processing when the edition dialog box receives the mouse focus

	CDialog::OnSetFocus(pOldWnd);

	ComputeGeometry();
	SetResults();
	m_ctrlPanelList.Invalidate();
	m_ctrlOK.SetFocus();
	InvalidateRect(&m_DrawRect);
}


void CWing::OnPaint() 
{
	// The window has recieved the message to repaint itself

	CPaintDC dc(this); // device context for painting
	
	CFont Arial8;
	Arial8.CreatePointFont(80,"Arial");
	CFont *pOldFont = dc.SelectObject(&Arial8);
	CPoint O(m_ptOffset);

	if(!m_bRightSide) O.x += (int)(m_TPos[m_NPanel]*m_fWingScale);

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


int CWing::IsFoil(CPoint point)
{
	// Returns the sections index where the
	if(m_bRightSide){
		double x = (point.x-m_ptOffset.x)/m_fWingScale;
		double y = (point.y-m_ptOffset.y)/m_fWingScale;
		for (int i=0; i<=m_NPanel; i++){
			if((abs(x-m_TPos[i])/m_Span<0.01) &&
				(y>=m_TOffset[i] && y<=m_TOffset[i]+m_TChord[i]))
				return i;
//				TRACE("returning %d\n",i);
		}
	}
	else{
		int Ox = m_ptOffset.x + (int)(m_TPos[m_NPanel]*m_fWingScale);
		double x = (point.x-Ox)/m_fWingScale;
		double y = (point.y-m_ptOffset.y)/m_fWingScale;
		for (int i=0; i<=m_NPanel; i++){
			if((abs(-x-m_TPos[i])/m_Span<0.01) &&
				(y>=m_TOffset[i] && y<=m_TOffset[i]+m_TChord[i]))
				return i;
		}
	}
	return -1;
}


void CWing::DrawWing(CDC *pDC, CPoint O)
{
	double x, x1, x2;
	int i;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CFoil *pFoil1, *pFoil2;
	CPen BlackPen(PS_SOLID, 2, GetSysColor(COLOR_BTNTEXT));
	CPen *pOldPen = pDC->SelectObject(&BlackPen);

	if(m_bRightSide){
		for (i=0; i<m_NPanel;i++){
			x1 = m_TPos[i];
			x2 = m_TPos[i+1];
			pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x +(int)(x2*m_fWingScale),
						O.y +(int)(m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x +(int)(x2*m_fWingScale),
						O.y +(int)((m_TOffset[i+1]+m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)((m_TOffset[i]+m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x +(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
		}
	}
	else{
		for (i=0; i<m_NPanel;i++){
			x1 = m_TPos[i];
			x2 = m_TPos[i+1];
			pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)(m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)((m_TOffset[i+1]+m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)((m_TOffset[i]+m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
		}
	}
	//Draw Flaps
	CPen DotPen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	pDC->SelectObject(&DotPen);
	if(m_bRightSide){
		for (i=0; i<m_NPanel;i++){
			pFoil1 = pFrame->GetFoil(m_RFoil[i]);
			pFoil2 = pFrame->GetFoil(m_RFoil[i+1]);
			x1 = m_TPos[i];
			x2 = m_TPos[i+1];
			if(pFoil1->m_bTEFlap && pFoil2->m_bTEFlap){
				if(pFoil1->m_bTEFlap){
					pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
								O.y +(int)((m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_TChord[i])*m_fWingScale));
				}
				else {
					pDC->MoveTo(O.x +(int)(x1*m_fWingScale), 
								O.y +(int)((m_TOffset[i]+m_TChord[i])*m_fWingScale));
				}
				if(pFoil2->m_bTEFlap){
					pDC->LineTo(O.x +(int)(x2*m_fWingScale), 
								O.y +(int)((m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_TChord[i+1])*m_fWingScale));
				}
				else {
					pDC->LineTo(O.x +(int)(x2*m_fWingScale), 
								O.y +(int)((m_TOffset[i+1]+m_TChord[i+1])*m_fWingScale));
				}
			}
		}
	}
	else{
		for (i=0; i<m_NPanel;i++){
			pFoil1 = pFrame->GetFoil(m_LFoil[i]);
			pFoil2 = pFrame->GetFoil(m_LFoil[i+1]);
			x1 = m_TPos[i];
			x2 = m_TPos[i+1];
			if(pFoil1->m_bTEFlap && pFoil2->m_bTEFlap){
				if(pFoil1->m_bTEFlap){
					pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
								O.y +(int)((m_TOffset[i]+pFoil1->m_TEXHinge/100.0*m_TChord[i])*m_fWingScale));
				}
				else {
					pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
								O.y +(int)((m_TOffset[i]+m_TChord[i])*m_fWingScale));
				}
				if(pFoil2->m_bTEFlap){
					pDC->LineTo(O.x -(int)(x2*m_fWingScale), 
								O.y +(int)((m_TOffset[i+1]+pFoil2->m_TEXHinge/100.0*m_TChord[i+1])*m_fWingScale));
				}
				else {
					pDC->LineTo(O.x -(int)(x2*m_fWingScale), 
								O.y +(int)((m_TOffset[i+1]+m_TChord[i+1])*m_fWingScale));
				}
			}
		}
/*		for (int i=0; i<m_NPanel;i++){
			x1 = m_TPos[i];
			x2 = m_TPos[i+1];
			pDC->MoveTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)(m_TOffset[i+1]*m_fWingScale));
			pDC->LineTo(O.x -(int)(x2*m_fWingScale),
						O.y +(int)((m_TOffset[i+1]+m_TChord[i+1])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)((m_TOffset[i]+m_TChord[i])*m_fWingScale));
			pDC->LineTo(O.x -(int)(x1*m_fWingScale), 
						O.y +(int)(m_TOffset[i]*m_fWingScale));
		}*/
	}
	//find mean aero chord, and draw it
	if (m_yMac>0.0){
		CPen GreenPen(PS_DOT, 1, RGB(0,170,0));
		pDC->SelectObject(&GreenPen);
		pDC->SetTextColor(RGB(0,170,0));
		pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
		if(m_bRightSide){
			x = GetOffset(2.0*m_yMac/m_Span);
			pDC->MoveTo(O.x + (int)(m_yMac*m_fWingScale), 
						O.y + (int)(x*m_fWingScale));
			pDC->LineTo(O.x + (int)(m_yMac*m_fWingScale), 
						O.y + (int)((x+m_MAChord)*m_fWingScale));
			pDC->TextOut(O.x - 10 + (int)(m_yMac*m_fWingScale),
						 O.y - 20 +(int)(x*m_fWingScale),
						"m.a.c.");
		}
		else{
			x = GetOffset(2.0*m_yMac/m_Span);
			pDC->MoveTo(O.x - (int)(m_yMac*m_fWingScale), 
						O.y + (int)(x*m_fWingScale));
			pDC->LineTo(O.x - (int)(m_yMac*m_fWingScale), 
						O.y + (int)((x+m_MAChord)*m_fWingScale));
			pDC->TextOut(O.x -10- (int)(m_yMac*m_fWingScale),
						 O.y -20+(int)(x*m_fWingScale),
						"m.a.c.");
		}
	}
	pDC->SelectObject(pOldPen);
}
void CWing::DrawVLMMesh(CDC *pDC, CPoint O)
{
	double chord, offset, yl;
	double offset1, offset2, ch1, ch2, x1, x2;
	double dT,dk,dNY;
	double xPoint1[MAXCHORDPANELS],xPoint2[MAXCHORDPANELS]; 
	double sf, snorm, cnorm;
	int i,k,l;
	int NXLead, NXFlap;

	if(m_SineFactor>0.0) sf = 1.0+1.0/m_SineFactor;
	else                 sf = 1000000.0;
	snorm    = 1.0/sin(pi/sf);
	cnorm    = 1.0/cos( pi*(1/2.0-1.0/sf));

	CFoil *pFoil1, *pFoil2;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	CPen MeshPen(PS_SOLID, 1, RGB(80, 80, 255));
	CPen *pOldPen = pDC->SelectObject(&MeshPen);

	if(m_bRightSide){
		for (i=0; i<m_NPanel;i++){
			//first draw lines parallel to chord,
			dT  = m_TPos[i+1]-m_TPos[i];
			dNY = (double)m_NYPanels[i];
			for (k=1;k<m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_YPanelDist[i] == 1)
				{
					//cosine distribution
					yl = (1.0 + cnorm * cos(dk * pi *(1.0-2.0*(1/2.0-1.0/sf))/dNY + pi*(1/2.0-1.0/sf)) ) * dT/2.0;
				}
				else if(m_YPanelDist[i] ==  2){
					//sine distribution
					yl = m_TPos[i+1] - snorm*sin(dk*pi/sf/dNY) * dT;
				}
				else if(m_YPanelDist[i] == -2){
					//-sine distribution
					yl = m_TPos[i]   + snorm*sin(dk*pi/sf/dNY) * dT;
				}
				else {
					//even spacing by default
					yl = m_TPos[i] +(dk*dT/dNY);
				}
				offset = GetOffset(yl*2.0/m_Span);
				chord  = GetChord(yl*2.0/m_Span);
				pDC->MoveTo(O.x +(int)(yl*m_fWingScale), 
							O.y +(int)(offset*m_fWingScale));
				pDC->LineTo(O.x +(int)(yl*m_fWingScale), 
							O.y +(int)((offset+chord)*m_fWingScale));
			}
			//next draw lines parallel to span,

			offset1 = m_TOffset[i];
			offset2 = m_TOffset[i+1];
			ch1     = m_TChord[i];
			ch2     = m_TChord[i+1];
			pFoil1  = pFrame->GetFoil(m_RFoil[i]);
			pFoil2  = pFrame->GetFoil(m_RFoil[i+1]);

			CreateXPoints(m_NXPanels[i], m_XPanelDist[i], pFoil1, pFoil2, 
				          xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				pDC->MoveTo(O.x +(int)(m_TPos[i]*m_fWingScale), 
							O.y +(int)(x1*m_fWingScale));
				pDC->LineTo(O.x +(int)(m_TPos[i+1]*m_fWingScale), 
							O.y +(int)(x2*m_fWingScale));
			}
		}
	}
	else{
		for (i=0; i<m_NPanel;i++)
		{
			//first draw lines parallel to chord,
			dT  = m_TPos[i+1]-m_TPos[i];
			dNY = (double)m_NYPanels[i];
			for (k=1;k<m_NYPanels[i];k++)
			{
				dk = (double)k;
				if(m_YPanelDist[i] == 1)
				{
					//cosine distribution
					yl = (1.0 + cnorm * cos(dk * pi *(1.0-2.0*(1/2.0-1.0/sf))/dNY + pi*(1/2.0-1.0/sf)) ) * dT/2.0;
				}
				else if(m_YPanelDist[i] == -2){
					//-sine distribution
					yl = m_TPos[i]   + snorm*sin(dk*pi/sf/dNY) * dT;
				}
				else if(m_YPanelDist[i] == 2){
					// sine distribution
					yl = m_TPos[i+1] - snorm*sin(dk*pi/sf/dNY) * dT;
				}
				else {
					//even spacing by default
					yl = m_TPos[i] +(dk*dT/dNY);
				}
				offset = GetOffset(yl*2.0/m_Span);
				chord  = GetChord(yl*2.0/m_Span);
				pDC->MoveTo(O.x -(int)(yl*m_fWingScale), 
							O.y +(int)(offset*m_fWingScale));
				pDC->LineTo(O.x -(int)(yl*m_fWingScale), 
							O.y +(int)((offset+chord)*m_fWingScale));
			}

			//next draw lines parallel to span,
			offset1 = m_TOffset[i];
			offset2 = m_TOffset[i+1];
			ch1     = m_TChord[i];
			ch2     = m_TChord[i+1];
			pFoil1  = pFrame->GetFoil(m_LFoil[i]);
			pFoil2  = pFrame->GetFoil(m_LFoil[i+1]);

			CreateXPoints(m_NXPanels[i], m_XPanelDist[i], 
                          pFoil1, pFoil2, xPoint1, xPoint2, NXLead, NXFlap);

			for (l=1; l<m_NXPanels[i];l++)
			{
				x1 = offset1 + ch1*xPoint1[l];
				x2 = offset2 + ch2*xPoint2[l];

				pDC->MoveTo(O.x -(int)(m_TPos[i]*m_fWingScale), 
							O.y +(int)(x1*m_fWingScale));
				pDC->LineTo(O.x -(int)(m_TPos[i+1]*m_fWingScale), 
							O.y +(int)(x2*m_fWingScale));
			}
		}
	}
	pDC->SelectObject(pOldPen);
}


void CWing::DrawDihedral(CDC *pDC, CPoint O)
{
	double x,z;
	//Draw dihedral
	CPen DihPen(PS_SOLID, 2, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&DihPen);
	int yOff = -30;

	if(m_bRightSide){
		pDC->MoveTo(O.x , O.y + yOff);
		for (int i=1; i<=m_NPanel;i++){
			x = m_TYProj[i];
			z = GetZPos(m_TPos[i]);
			pDC->LineTo(O.x +      (int)(x*m_fWingScale),
						O.y + yOff-(int)(z*m_fWingScale));
		}
	}
	else{
		pDC->MoveTo(O.x , O.y + yOff);
		for (int i=1; i<=m_NPanel;i++){
			x = m_TYProj[i];
			z = GetZPos(m_TPos[i]);
			pDC->LineTo(O.x -      (int)(x*m_fWingScale),
						O.y + yOff-(int)(z*m_fWingScale));
		}
	}

	pDC->SelectObject(pOldPen);
}
 
double CWing::GetAverageSweep()
{
	double xroot = m_TChord[0]/4.0;
	double xtip  = m_TOffset[m_NPanel] + m_TChord[m_NPanel]/4.0;
//	double sweep = (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
	return (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
}

void CWing::SetSweep(double Sweep)
{
//	double CurrentSweep = GetAverageSweep();
	double OldTipOffset = m_TOffset[m_NPanel];
	double NewTipOffset = m_TChord[0]/4.0 
						 + tan(Sweep*pi/180.0)*m_Span/2.0
						 - m_TChord[m_NPanel]/4.0;
	if(abs(OldTipOffset)>0.00001f){
		//scale each panel's offset
		double ratio = NewTipOffset/OldTipOffset;
		for(int i=1; i<=m_NPanel; i++){
			m_TOffset[i] *= ratio;
		}
	}
	else{
		//set each panel's offset
		for(int i=1; i<=m_NPanel; i++){
			m_TOffset[i] = NewTipOffset*m_TPos[i]/(m_Span/2.0);
		}
	}
	ComputeGeometry();
	Convert(true);
}

void CWing::SetTwist(double Twist)
{
	if(abs(m_TTwist[m_NPanel])>0.0001) 
	{
		//scale each panel's twist
		double ratio = Twist/m_TTwist[m_NPanel];

		for(int i=1; i<=m_NPanel; i++){
			m_TTwist[i] *= ratio;
		}
	}
	else{
		//Set each panel's twist in the ratio of the span position
		for(int i=1; i<=m_NPanel; i++){
			m_TTwist[i] = Twist*m_TPos[i]/(m_Span/2.0);
		}
	}
	ComputeGeometry();
	Convert(true);
}


void CWing::DrawFoils(CDC *pDC, CPoint O)
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
	else                            MinPanelSize = m_Span;

	if(m_bRightSide){
		for (i=0; i<=m_NPanel;i++){
			x = m_TPos[i];
			y = GetOffset(m_TPos[i]/m_Span*2.0);
			Chord = GetChord(m_TPos[i]/m_Span*2.0);
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

			strong = m_RFoil[i].Left(30);
			pDC->TextOut(O.x + (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
		}
		nFlap = 0;
		//	count left wing flaps
		for (i=1; i<=m_NPanel; i++){
			pFoilA = pFrame->GetFoil(m_LFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_LFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize){
				//we have a flap, so count it
				nFlap++;
			}
		}
		for (i=1; i<=m_NPanel; i++){
			pFoilA = pFrame->GetFoil(m_RFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_RFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize){
				//we have a flap, so write it down
				x = (m_TPos[i]+m_TPos[i-1])/2.0;
				y = GetOffset(x/m_Span*2.0);
				strong.Format("Flap %d", nFlap+1);
				pDC->TextOut(O.x + (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	else{
		for (i=0; i<=m_NPanel;i++){
			x = m_TPos[i];
			y = GetOffset(m_TPos[i]/m_Span*2.0);
			Chord = GetChord(m_TPos[i]/m_Span*2.0);
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

			strong = m_LFoil[i].Left(30);
			pDC->TextOut(O.x - (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale),
						strong);
		}
		nFlap = 0;

//		for (i=1; i<=m_NPanel; i++){
		for (i=m_NPanel; i>=1; i--){
			pFoilA = pFrame->GetFoil(m_LFoil[i-1]);
			pFoilB = pFrame->GetFoil(m_LFoil[i]);
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize){
				//we have a flap, so write it down
				x = (m_TPos[i]+m_TPos[i-1])/2.0;
				y = GetOffset(x/m_Span*2.0);
				strong.Format("Flap %d", nFlap+1);
				pDC->TextOut(O.x - (int)(x*m_fWingScale), O.y + 10+(int)((y+Chord)*m_fWingScale), strong);
				nFlap++;
			}
		}
	}
	pDC->SelectObject(pOldPen);
}


void CWing::OnKillFocusSinefactor()
{
	m_SineFactor = m_ctrlSineFactor.GetValue();
	if(m_SineFactor>1.0) 
	{
		m_SineFactor = 1.0;
		m_ctrlSineFactor.SetValue(1.0);
	}
	else if(m_SineFactor<0.0) 
	{
		m_SineFactor = 0.0;
		m_ctrlSineFactor.SetValue(0.0);
	}
	InvalidateRect(m_DrawRect);

}


void CWing::OnVLMMesh() 
{
	VLMSetAutoMesh();
	FillPanelList();
	SetResults();
	InvalidateRect(m_DrawRect);
	m_bChanged = true;
} 

void CWing::OnWingColor() 
{
	m_WingColor = m_ctrlWingColor.GetColor();
	m_ctrlWingColor.Invalidate();
}

void CWing::GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t)
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	
	if  (y>0.0) {//search Right wing
		for (int i=0; i<m_NPanel; i++){
			if (m_TPos[i]<=y && y<=m_TPos[i+1]){

				*pFoil0 = pFrame->GetFoil(m_RFoil[i]);
				*pFoil1 = pFrame->GetFoil(m_RFoil[i+1]);
				t = (y-m_TPos[i])/(m_TPos[i+1] - m_TPos[i]);
				return;
			}
		}
	}
	else{//search left wing
		y = -y;
		for (int i=0; i<m_NPanel; i++){
			if (m_TPos[i]<=y && y<m_TPos[i+1]){
				*pFoil0 = pFrame->GetFoil(m_LFoil[i]);
				*pFoil1 = pFrame->GetFoil(m_LFoil[i+1]);
				t = (y-m_TPos[i])/(m_TPos[i+1] - m_TPos[i]);
				return;
			}
		}	
	}
	t = 0;
	pFoil0 = NULL;// use linear
	pFoil1 = NULL;// use linear
}


int CWing::LLTIterate()
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	int k ;
	CFoil* pFoil0  = NULL;
	CFoil* pFoil1  = NULL;
	double a, yob, tau, anext;
	bool bOutRe, bError;

	m_Maxa = 0.0;

	for (k=1; k<m_NStation; k++){
		a        = m_Ai[k];
		anext    = -AlphaInduced(k);
		m_Ai[k]  = a +(anext-a)/m_RelaxMax;
		m_Maxa   = __max(m_Maxa, abs(a-anext));
	}

	double Lift=0.0;// required for Type 2
	for (k=1; k<m_NStation; k++)
	{
		yob     = cos(k*pi/m_NStation);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		m_Cl[k] = pFrame->GetCl(pFoil0, pFoil1, m_Re[k], 
								 m_Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
		if (m_Type == 2){
			Lift += Eta(k) * m_Cl[k] * m_Chord[k];
		}
	}

	if(m_Type == 2){
		Lift *= m_AR /m_Span;
		if(Lift<=0.0) {
			CString strong; 
			strong.Format("   Found negative lift for Alpha = %6.1f in Type 2 analysis... Aborting\r\n",m_Alpha);
			if(m_bTrace) m_pXFile->WriteString(strong);
			return -1;
		}
		m_QInf  = m_QInf0 / sqrt(Lift);
		
		for (k=1; k<m_NStation; k++){
			m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
			yob     = cos(k*pi/m_NStation);
			GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
			m_Cl[k] = pFrame->GetCl(pFoil0, pFoil1, m_Re[k], 
									 m_Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
		}
	}

	if (m_Maxa<m_CvPrec) {
		m_bConverged = true;
		return 1;
	}

	return 0;
}

double CWing::GetTwist(double y)
{
	int l;
	double tau;
	// calculate twist at each station
	if (y>=0.0){//right wing
		for (l=0; l<=m_NPanel; l++){
			if(m_TPos[l] <= y && y <=m_TPos[l+1]){
				tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
				return m_TTwist[l]+(m_TTwist[l+1]-m_TTwist[l]) * tau;
			}
		}
	}
	else{//left wing
		y=abs(y);
		for (l=0; l<m_NPanel; l++){
			if(m_TPos[l] <= y && y <=m_TPos[l+1]){
				tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
				return m_TTwist[l]+(m_TTwist[l+1]-m_TTwist[l]) * tau;
			}
		}
	}

	return -1.0;
}

double CWing::GetOffset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= abs(yob*m_Span/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			return Offset;
		}
	}
	
	return -1.0;
}

double CWing::Getyrel(double SpanPos)
{
	double y = abs(SpanPos);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<=y && y <m_TPos[i+1]){
			if(SpanPos>0) return  (y-m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			else          return  (y-m_TPos[i+1])/(m_TPos[i]-m_TPos[i+1]);
		}
	}
	return 1.0;
}

double CWing::GetZPos(double y)
{
	double tau;
	double ZPos =0.0;

	y= abs(y);
	if(y<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++){
		if(m_TPos[i]< y && y<=m_TPos[i+1]){
			for (int k=0; k<i; k++){
				ZPos+=m_TLength[k+1] * sin(m_TDihedral[k]*pi/180.0);
			}
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			ZPos += tau * m_TLength[i+1] * sin(m_TDihedral[i]*pi/180.0);
			return ZPos;
		}
	}
	return 0.0;
}

double CWing::GetViewZPos(double xrel, double y, int pos)
{
	double tau;
	double twist, chord;
	double ZPos =0.0;
	double fy = abs(y);

//	if(fy<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++){
		if(m_TPos[i]< fy && fy<=m_TPos[i+1]){
			for (int k=0; k<i; k++){
				ZPos += m_TLength[k+1] * sin(m_TDihedral[k]*pi/180.0);
			}
			tau = (fy - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			ZPos += tau * m_TLength[i+1] * sin(m_TDihedral[i]*pi/180.0);

			//	add washout calculated about chord quarter line :
			twist = GetTwist(fy)*pi/180.;
			chord = GetChord(fy*2./m_Span);
			ZPos -= (xrel-0.25)*chord*sin(twist);

			CFoil *pFoil0 = NULL;
			CFoil *pFoil1 = NULL;
			GetFoils(&pFoil0, &pFoil1, y,  tau);

			if(!pFoil0 || !pFoil1) return ZPos;

			if(pos==1){
				double z0 = pFoil0->GetUpperY(xrel) * chord;
				double z1 = pFoil1->GetUpperY(xrel) * chord;
				ZPos += z0 + (z1-z0)*tau;
			}
			else if(pos==-1){
				double z0 = pFoil0->GetLowerY(xrel) * chord;
				double z1 = pFoil1->GetLowerY(xrel) * chord;
				ZPos += z0 + (z1-z0)*tau;
			}
			return ZPos;
		}
	}
	return -1.0;
}

void CWing::GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos)
{
	double tau;
	double twist, chord;
	zv = 0.0;
	yv = 0.0;
	double fy = abs(y);
	double sign;
	if(fy<1.0e-10) sign = 1.0;
	else sign = y/fy;

//	if(fy<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++){
		if(m_TPos[i]< fy && fy<=m_TPos[i+1]){
			for (int k=0; k<i; k++){
				yv += m_TLength[k+1] * cos(m_TDihedral[k]*pi/180.0);
				zv += m_TLength[k+1] * sin(m_TDihedral[k]*pi/180.0);
			}
			tau = (fy - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			yv += tau * m_TLength[i+1] * cos(m_TDihedral[i]*pi/180.0);
			zv += tau * m_TLength[i+1] * sin(m_TDihedral[i]*pi/180.0);

			yv *= sign;
			//	add washout calculated about chord quarter line :
			twist = GetTwist(fy)*pi/180.;
			chord = GetChord(fy*2./m_Span);
			zv -= (xrel-0.25)*chord*sin(twist);

			CFoil *pFoil0 = NULL;
			CFoil *pFoil1 = NULL;
			GetFoils(&pFoil0, &pFoil1, y,  tau);

			if(!pFoil0 || !pFoil1) return;

			if(pos==1){
				double z0 = pFoil0->GetUpperY(xrel) * chord;
				double z1 = pFoil1->GetUpperY(xrel) * chord;
				zv += z0 + (z1-z0)*tau;
			}
			else if(pos==-1){
				double z0 = pFoil0->GetLowerY(xrel) * chord;
				double z1 = pFoil1->GetLowerY(xrel) * chord;
				zv += z0 + (z1-z0)*tau;
			}
		}
	}
}

double CWing::GetTopz(double x, double y)
{
	//x is the absolute chord position
	//y is the absolute span position
	double tau, chord, off, xoc, z0,z1;

	CFoil *pFoil0 = NULL;
	CFoil *pFoil1 = NULL;
	//convert absolute x to relative x
	chord = GetChord(2.0*y/m_Span);
	off   = GetOffset(2.0*y/m_Span);
	if(chord>0.0)	xoc = (x-off)/chord;
	else			xoc = 0.5;

	y= abs(y);

	for (int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y<=m_TPos[i+1]){
			GetFoils(&pFoil0, &pFoil1, y,  tau);
			if(!pFoil0 || !pFoil1) 
				return 0.0;
			z0 = chord*pFoil0->GetUpperY(xoc);
			z1 = chord*pFoil1->GetUpperY(xoc);
			return GetZPos(y)+z0+tau*(z1-z0);
		}
	}
	return -1.0;
}


double CWing::GetBotz(double x, double y)
{
	//x is the absolute chord position
	//y is the absolute span position
	double tau;
	double chord;
	double xoc;
	double z0,z1;
	CFoil *pFoil0 = NULL;
	CFoil *pFoil1 = NULL;
	//convert absolute x to relative x
	chord = GetChord(2.0*y/m_Span);
	if(chord>0.0)	xoc = (x-GetOffset(2.0*y/m_Span))/chord;
	else			xoc = 0.5;

	y= abs(y);
//	if(y<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y<=m_TPos[i+1]){
			GetFoils(&pFoil0, &pFoil1, y,  tau);
			if(!pFoil0 || !pFoil1) return 0.0;
			z0 = chord*pFoil0->GetLowerY(xoc);
			z1 = chord*pFoil1->GetLowerY(xoc);
			return GetZPos(y)+z0+tau*(z1-z0);
		}
	}
	return -1.0;
}


double CWing::GetC4(double yob, double xRef)
{	
	//returns the quarter-chord point xposition relative to XCmRef
	double Chord, Offset, tau;
	double C4 = 0.0;
	double y = abs(yob*m_Span/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Chord  = m_TChord[i]  + tau * (m_TChord[i+1] - m_TChord[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			C4 = Offset + Chord/4.0 - xRef;
			return C4;
		}
	}
	
	return C4;
}

void CWing::Getjkp(double y, int &j, int &k, int &p)
{
	// valid for 3D panel analysis only
	//note : y is understood as the postion on the wing spread flat
	int jj,kk,ks,pp,Size;
	double absy, yrel, length = 0;
	kk=0;
	absy = abs(y);
	pp = m_Surface[0].m_NXPanels; //Tip Patch

	Size = m_Surface[0].m_NXPanels + m_Surface[m_NSurfaces-1].m_NXPanels; //Tip Patches
	for(j=0; j<m_NSurfaces; j++) Size += 2*m_Surface[j].m_NXPanels*m_Surface[j].m_NYPanels;
	//Size= m_MatSize...
	pp=Size/2;

	for(j=m_NSurfaces/2; j<m_NSurfaces; j++)
	{
		if(absy<length+m_Surface[j].Length)
		{ //y-position is somewhere in this surface
			yrel = (absy-length)/m_Surface[j].Length;
			ks   = m_Surface[j].Getk(yrel);
			pp  += ks*2*m_Surface[j].m_NXPanels;
			kk  += ks;
			break;
		}
		else
		{
			length  += m_Surface[j].Length;
			pp      += m_Surface[j].m_NElements;//for a 3D panel analysis
			kk      += m_Surface[j].m_NYPanels;
		}
	}
	if(y>=0.0)
	{
		p = pp;
		k = kk;
	}
	else
	{
		k = m_NStation/2-kk-1;
		j = m_NSurfaces-1-j;

		p = m_Surface[0].m_NXPanels; //Tip Patch
		for(jj=0; jj<j; jj++)
			p += 2*m_Surface[jj].m_NXPanels*m_Surface[jj].m_NYPanels;
		p += 2*m_Surface[j].m_NXPanels*(m_Surface[j].m_NYPanels-ks-1);
	}
}

double CWing::GetChord(double yob)
{
	double Chord = 0.0;
	double tau;
	double y;

	y= abs(yob*m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<=y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Chord = m_TChord[i] + tau * (m_TChord[i+1] - m_TChord[i]);
			return Chord;
		}
	}
	return -1.0;
}


double CWing::GetDihedral(double yob)
{
//	double Dih = 0.0;
	double y;

	y= abs(yob*m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			if(yob>=0) return m_TDihedral[i];
			else  return -m_TDihedral[i];
		}
	}
	return 0.0;
}

void CWing::SplitPanel(int i, double tau)
{
	if(m_NPanel>=MAXPANELS-1) return;

	for (int k=m_NPanel; k>=i; k--){
		m_TPos[k+1]       = m_TPos[k];
		m_TChord[k+1]     = m_TChord[k];
		m_TOffset[k+1]    = m_TOffset[k];
		m_TTwist[k+1]     = m_TTwist[k];
		m_TDihedral[k+1]  = m_TDihedral[k];
		m_NXPanels[k+1]   = m_NXPanels[k];	
		m_NYPanels[k+1]   = m_NYPanels[k];	
		m_XPanelDist[k+1] = m_XPanelDist[k];
		m_YPanelDist[k+1] = m_YPanelDist[k];
		m_RFoil[k+1]      = m_RFoil[k];
		m_LFoil[k+1]      = m_LFoil[k];
	}
	if(i==0) i++;

	m_TPos[i]       = m_Span/2.0 * tau;

	double taupanel  = (m_TPos[i]-m_TPos[i-1])/(m_TPos[i+1]-m_TPos[i-1]);
	m_TChord[i]      = m_TChord[i-1]  + taupanel * (m_TChord[i+1] -m_TChord[i-1]);
	m_TOffset[i]     = m_TOffset[i-1] + taupanel * (m_TOffset[i+1]-m_TOffset[i-1]);
	m_TTwist[i]      = m_TTwist[i-1] + taupanel * (m_TTwist[i+1]-m_TTwist[i-1]);
	m_TDihedral[i]   = m_TDihedral[i-1];
	m_NXPanels[i]    = m_NXPanels[i-1];	
	m_NYPanels[i]    = int(m_NYPanels[i-1]*(1-taupanel));
	m_NYPanels[i-1]  = int(m_NYPanels[i-1]* taupanel);
	m_XPanelDist[i]  = m_XPanelDist[i-1];
	m_YPanelDist[i]  = m_YPanelDist[i-1];
	m_RFoil[i]       = m_RFoil[i-1];
	m_LFoil[i]       = m_LFoil[i-1];

	m_NPanel++;

}


bool CWing::CheckWing()
{
	if(!m_WingName.GetLength()){
		AfxMessageBox("Please enter a name for the wing");
		m_ctrlWingName.SetFocus();
		return false;
	}
	for (int k=1; k<=m_NPanel; k++){
		if(m_TPos[k]*1.00001 < m_TPos[k-1]){
			AfxMessageBox("Warning : Panel sequence is inconsistent");
			return false;
		}
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2) {
		AfxMessageBox("Too many panels\nReduce the mesh size");
		return false;
	}

	if(m_nFlaps>=20){
		int res = AfxMessageBox("Only 10 flaps x 2 will be handled", MB_OKCANCEL);
		if (res==IDCANCEL) return false;
	}
	return true;
}



void CWing::Convert(bool bSet)//else retrieve
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;

	if(bSet){//convert data from IS and set it

		for (int i=0; i<=MAXPANELS; i++){
			m_DTChord[i]  = m_TChord[i]  * pFrame->m_mtoUnit;
			m_DTPos[i]    = m_TPos[i]    * pFrame->m_mtoUnit;
			m_DTOffset[i] = m_TOffset[i] * pFrame->m_mtoUnit;
		}

	}
	else{//retreive data and convert it to IS
		//convert and retrieve
		for (int i=0; i<=MAXPANELS; i++){
			m_TChord[i]  = m_DTChord[i]  / pFrame->m_mtoUnit;
			m_TPos[i]    = m_DTPos[i]    / pFrame->m_mtoUnit;
			m_TOffset[i] = m_DTOffset[i] / pFrame->m_mtoUnit;
		}
	}
}

void CWing::SetParams()
{
	Convert(true);//set the data
	FillPanelList();
	SetResults();
}



void CWing::OnInsertBefore() 
{
	if (m_NPanel==MAXPANELS) {
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
	for (k=m_NPanel; k>=n; k--)
	{
		m_DTPos[k+1]      = m_DTPos[k];
		m_DTChord[k+1]    = m_DTChord[k];
		m_DTOffset[k+1]   = m_DTOffset[k];
		m_TTwist[k+1]     = m_TTwist[k];
		m_TDihedral[k+1]  = m_TDihedral[k];
		m_NXPanels[k+1]   = m_NXPanels[k];	
		m_NYPanels[k+1]   = m_NYPanels[k];	
		m_XPanelDist[k+1] = m_XPanelDist[k];
		m_YPanelDist[k+1] = m_YPanelDist[k];
		m_RFoil[k+1]      = m_RFoil[k];
		m_LFoil[k+1]      = m_LFoil[k];
	}

	ny = m_NYPanels[n-1];
	m_DTPos[n]    = (m_DTPos[n+1]    + m_DTPos[n-1])   /2.0;
	m_DTChord[n]  = (m_DTChord[n+1]  + m_DTChord[n-1]) /2.0;
	m_DTOffset[n] = (m_DTOffset[n+1] + m_DTOffset[n-1])/2.0;

	m_NXPanels[n]   = m_NXPanels[n-1];
	m_NYPanels[n]   = (int)(ny/2);
	m_NYPanels[n-1] = ny-m_NYPanels[n];
	if(m_NYPanels[n]==0)   m_NYPanels[n]++;
	if(m_NYPanels[n-1]==0) m_NYPanels[n-1]++;

	m_NPanel++;

//	m_bVLMAutoMesh = true;

	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}

void CWing::OnAppend() 
{
	if (m_NPanel==MAXPANELS)
	{
		AfxMessageBox("The maximum number of panels has been reached", MB_OK);
		return;
	}
	int k,n,ny,total;

	n  = m_iSection;
	ny = m_NYPanels[n];
	total = VLMGetPanelTotal();

	for (k=m_NPanel; k>n; k--)
	{
		m_DTPos[k+1]      = m_DTPos[k];
		m_DTChord[k+1]    = m_DTChord[k];
		m_DTOffset[k+1]   = m_DTOffset[k];
		m_TTwist[k+1]     = m_TTwist[k];
		m_TDihedral[k+1]  = m_TDihedral[k];
		m_NXPanels[k+1]   = m_NXPanels[k];	
		m_NYPanels[k+1]   = m_NYPanels[k];	
		m_XPanelDist[k+1] = m_XPanelDist[k];
		m_YPanelDist[k+1] = m_YPanelDist[k];
		m_RFoil[k+1]      = m_RFoil[k];
	}

	if(n+1<=m_NPanel)
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
	m_TTwist[n+1]     = m_TTwist[n];
	m_TDihedral[n+1]  = m_TDihedral[n];
	m_NXPanels[n+1]   = m_NXPanels[n];	
	m_NYPanels[n+1]   = m_NYPanels[n];	
	m_XPanelDist[n+1] = m_XPanelDist[n];
	m_YPanelDist[n+1] = m_YPanelDist[n];
	m_RFoil[n+1]      = m_RFoil[n];

	m_NYPanels[n+1] = max(1,(int)(ny/2)); 
	m_NYPanels[n]   = max(1,ny-m_NYPanels[n+1]); 
	
	m_NPanel++;

//	m_bVLMAutoMesh = true;
	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}	


void CWing::OnDeleteInput() 
{
	if(m_iSection<=0)
	{
		AfxMessageBox("The first section cannot be deleted", MB_OK);
		return;
	}

	int ny, k, size, total;

	size = m_ctrlPanelList.GetItemCount();
	if(size<=2) return;

	ny = m_NYPanels[m_iSection-1] + m_NYPanels[m_iSection];

	total = VLMGetPanelTotal();
	for (k=m_iSection; k<size; k++)
	{
		m_DTPos[k]      = m_DTPos[k+1];
		m_DTChord[k]    = m_DTChord[k+1];
		m_DTOffset[k]   = m_DTOffset[k+1];
		m_TTwist[k]     = m_TTwist[k+1];
		m_TDihedral[k]  = m_TDihedral[k+1];
		m_NXPanels[k]   = m_NXPanels[k+1];	
		m_NYPanels[k]   = m_NYPanels[k+1];	
		m_XPanelDist[k] = m_XPanelDist[k+1];
		m_YPanelDist[k] = m_YPanelDist[k+1];
		m_RFoil[k]      = m_RFoil[k+1];
		m_LFoil[k]      = m_LFoil[k+1];
	}
	m_NPanel--;
	
	m_NYPanels[m_iSection-1] = ny;

//	m_bVLMAutoMesh = true;
	Convert(false);
//	VLMSetAutoMesh(total);
	FillPanelList();
	ComputeGeometry();
	SetResults();
	m_bChanged = true;
	InvalidateRect(&m_DrawRect);
}


void CWing::OnSymetric() 
{
	if(m_ctrlSymetric.GetCheck())
	{
		m_bSymetric    = true;
		m_bVLMSymetric = true;
		m_bRightSide   = true;
		m_ctrlLeftWing.EnableWindow(false);
		CheckRadioButton(IDC_SIDE1, IDC_SIDE2, IDC_SIDE2);
	}
	else {
		m_bSymetric    = false;
		m_bVLMSymetric = false;
		m_ctrlLeftWing.EnableWindow(true);
	}

	InvalidateRect(&m_DrawRect);
	m_bChanged = true;
}


void CWing::OnSide() 
{
	if(GetCheckedRadioButton(IDC_SIDE1,IDC_SIDE2)==IDC_SIDE2){
		m_bRightSide = true;
	}
	else {
		m_bRightSide = false;
//		m_LFoil[0] = m_RFoil[0];
	}
	FillPanelList();
	m_bChanged = true;
	InvalidateRect(m_DrawRect);
}


void CWing::LLTComputeWing()
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CWPolar* pWPolar = pMiarex->m_pCurWPolar;
	CFoil* pFoil0 = NULL;
	CFoil* pFoil1 = NULL;
	CString strong, string;

	double yob, tau, c4, arad, zpos;
	double Integral0           = 0.0;
	double Integral1           = 0.0;
	double Integral2           = 0.0;
	double InducedDrag         = 0.0;
	double ViscousDrag         = 0.0;
	double InducedYawingMoment = 0.0;
	double ViscousYawingMoment = 0.0;
	double PitchingMoment      = 0.0;
	double PCm                 = 0.0;
	double GCm                 = 0.0;
	double eta, sigma;
	double Cm0;
	bool bOutRe, bError;
	bool bPointOutRe, bPointOutAlpha;
	m_bWingOut = false;

	for (int m=1; m<m_NStation; m++){
		bPointOutRe    = false;
		bPointOutAlpha = false;
		yob   = cos((double)m*pi/(double)m_NStation);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);

		m_Cl[m]     = pFrame->GetCl(pFoil0, pFoil1, m_Re[m],
									m_Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) 
			bPointOutAlpha = true;
		
		m_PCd[m]    = pMiarex->GetCd(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, m_AR, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_ICd[m]    = -m_Cl[m] * (m_Ai[m]* pi/180.0);

		m_XTrTop[m] = pMiarex->GetXTr(pFoil0, pFoil1, m_Re[m],
									m_Alpha+m_Ai[m] + m_Twist[m], tau, true, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;
		
		m_XTrBot[m] = pMiarex->GetXTr(pFoil0, pFoil1, m_Re[m],m_Alpha+m_Ai[m]+m_Twist[m], tau, false, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;
		
		m_CmAirf[m] = pFrame->GetCm(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XCPSpanRel[m]  = pMiarex->GetXCp(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, m_AR, bOutRe, bError);
		if(abs(m_XCPSpanRel[m])<0.000001){//plr mesh was generated prior to v3.15, i.e., without XCp calculations
			Cm0 = pFrame->GetCm0(pFoil0, pFoil1, m_Re[m],tau, bOutRe, bError);
			if(m_Cl[m]!=0.0) m_XCPSpanRel[m] = 0.25 - Cm0/m_Cl[m];
			else             m_XCPSpanRel[m] = 0.25;
//			bError = false;
		}
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		arad = (m_Alpha+m_Ai[m]+m_Twist[m])*pi/180.0;
//		arad = (m_Alpha-m_Ai[m])*pi/180.0;
		c4 = GetC4(yob, m_XCmRef)/m_Chord[m];
		zpos = GetZPos(yob*m_Span/2.0)/m_Chord[m];
		m_CmGeom[m] = - c4  * (m_Cl[m]*cos(arad) + m_PCd[m]*sin(arad))
					  - zpos* (m_Cl[m]*sin(arad) - m_PCd[m]*cos(arad));
		m_Cm[m] = m_CmAirf[m]+m_CmGeom[m];

		if(bPointOutAlpha){
			GetLengthUnit(string, pFrame->m_LengthUnit);
			strong.Format("       Span pos = %9.2f ",	cos(m*pi/m_NStation)*m_Span*pFrame->m_mtoUnit);
			strong += string;
			strong += ",  Re = ";
			ReynoldsFormat(string, m_Re[m]);
			strong += string;

			string.Format(" ,  A+Ai+Twist = %6.1f could not be interpolated\r\n", m_Alpha+m_Ai[m] + m_Twist[m]);
			strong+=string;

			if(m_bTrace) m_pXFile->WriteString(strong);
			m_bWingOut = true;
			m_bConverged = false;
		}
		else if(bPointOutRe){
			GetLengthUnit(string, pFrame->m_LengthUnit);
			strong.Format("       Span pos = %9.2f ",	cos(m*pi/m_NStation)*m_Span*pFrame->m_mtoUnit);
			strong += string;
			strong += ",  Re = ";
			ReynoldsFormat(string, m_Re[m]);
			strong += string;

			string.Format(" ,  A+Ai+Twist = %6.1f is outside the flight envelope\r\n", m_Alpha+m_Ai[m] + m_Twist[m]);
			strong+=string;

			if(m_bTrace) m_pXFile->WriteString(strong);
			m_bWingOut = true;
		}

		eta = Eta(m);
		sigma = Sigma(m);
		Integral0           += eta   * m_Cl[m]  * m_Chord[m];
		Integral1           += sigma * m_Cl[m]  * m_Chord[m];
		Integral2           += eta   * m_Cl[m]  * m_Chord[m] * (m_Offset[m]+m_XCPSpanRel[m]*m_Chord[m]);
		InducedDrag         += eta   * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousDrag         += eta   * m_PCd[m] * m_Chord[m];
		InducedYawingMoment += sigma * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousYawingMoment += sigma * m_PCd[m] * m_Chord[m];
		PitchingMoment      += eta   * m_Cm[m]     * m_Chord[m] * m_Chord[m];
		PCm                 += eta   * m_CmAirf[m] * m_Chord[m] * m_Chord[m];
		GCm                 += eta   * m_CmGeom[m] * m_Chord[m] * m_Chord[m];
	}

	m_CL            =  Integral0   * m_AR /m_Span;
	m_InducedDrag   =  InducedDrag * m_AR /m_Span  * pi / 180.0;
	m_ViscousDrag   =  ViscousDrag / m_GChord;
	m_RollingMoment = -Integral1   * m_AR /m_Span;
	m_IYm = InducedYawingMoment /m_Span * pi * m_AR /180.0;
	m_GYm = ViscousYawingMoment /m_GChord;
	m_PitchingMoment = PitchingMoment / m_GChord / m_MAChord;
	m_VCm = PCm / m_GChord / m_MAChord;
	m_GCm = GCm / m_GChord / m_MAChord;

	if(m_CL !=0.0)	m_XCP = Integral2 * m_AR /m_Span/m_CL;
	else            m_XCP = 0.0;
	if(m_bSymetric) m_YCP = 0.0;
	else            m_YCP = m_AR/m_CL * Integral1; 

	//bending moment

	double StripArea[MAXSTATIONS+1];
	memset(StripArea, 0, sizeof(StripArea));
	int j,jj;

	double yj, yjm, yjp;
	double dy;

	for (j=0; j<=m_NStation; j++)		m_SpanPos[j] = m_Span/2.0 * cos(j*pi/m_NStation);

	for (j=1; j<m_NStation; j++){
		yjp = m_SpanPos[j-1];
		yjm = m_SpanPos[j+1];
		yj  = m_SpanPos[j];

		dy = (yjp-yj)/2.0 + (yj-yjm)/2.0;

		StripArea[j] = m_Chord[j]*dy;//mm²
	}

	//dynamic pressure, kg/m3
	double q = 0.5*pWPolar->m_Density * m_QInf * m_QInf;

	double bm;
	double y, yy;

	for (j=1; j<m_NStation; j++){
		y = m_SpanPos[j];
		bm = 0.0;
		if (y>=0){
			for (jj=0; jj<j; jj++){
				yy =  m_SpanPos[jj];
				bm += (yy-y) * m_Cl[jj] * StripArea[jj];
			}
		}
		else{
			for (jj=j+1; jj<m_NStation; jj++){
				yy =  m_SpanPos[jj];
				bm += (y-yy) * m_Cl[jj] * StripArea[jj];
			}
		}
		m_BendingMoment[j] = bm*q;
	}
}

bool CWing::LLTSetLinearSolution()
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	double aij[MAXSTATIONS*MAXSTATIONS];// coefficient matrix
	double rhs[MAXSTATIONS+1];//right hand side

	memset(aij, 0, sizeof(aij));
	memset(rhs, 0, sizeof(rhs));

	CFoil *pFoil0, *pFoil1;
	int i,j,p;
	double fr  = m_NStation;
	double fj, t0, st0, snt0, c, a0, slope, tau, yob;
	double cs = m_TChord[0];

	for (i=1; i<m_NStation; i++){
		c   = m_Chord[i];
		t0  = i * pi/fr;
		st0 = sin(t0);
		for (j=1; j<m_NStation; j++){
			fj = double(j);
			snt0 = sin(fj*t0);
			p = (i-1)*(m_NStation-1)+j-1;
			aij[p]  = snt0 + c*pi/m_Span/2.0* fj*snt0/st0;
		}
		yob   = cos(i*pi/m_NStation);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		a0 = pMiarex->GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau);
		rhs[i] = c/cs * (m_Alpha-a0+m_Twist[i])/180.0*pi;
	}

	if(Gauss(aij,m_NStation-1, rhs+1,0)){
		for (i=1; i<m_NStation; i++){
			t0  = i * pi/fr;
			m_Cl[i] = 0.0;
			for (j=1; j<m_NStation; j++){
				fj = double(j);
				snt0 = sin(fj*t0);
				m_Cl[i] += rhs[j]* snt0;
			}
			yob   = cos(i*pi/m_NStation);
			GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
			pMiarex->GetLinearizedPolar(pFoil0, pFoil1, m_Re[i], tau, a0, slope);
			a0 = pMiarex->GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau);//better apprixmation ?
			m_Cl[i] *= slope*180.0/pi*cs/m_Chord[i];
			m_Ai[i]  = -(m_Alpha-a0+m_Twist[i]) + m_Cl[i]/slope;
		}
		return true;
	}
	return false;
}


bool CWing::Gauss(double *A, int n, double *B, int m)
{
////////////////////////////////////////////////////////////////////////////////
//  int Gaussian_Elimination(double *A, int n, double *B)                     //
//                                                                            //
//     Solve the linear system of equations AX=B where A is an n x n matrix   //
//     B is an n-dimensional column vector (n x 1 matrix) for the             //
//     n-dimensional column vector (n x 1 matrix) X.                          //
//                                                                            //
//     This routine performs partial pivoting and the elements of A are       //
//     modified during computation.  The result X is returned in B.           //
//     If the matrix A is singular, the return value of the function call is  //
//     false. If the solution was found, the function return value is true    //
//                                                                            //
//  Arguments:                                                                //
//     double *A      On input, the pointer to the first element of the       //
//                    matrix A[n][n].  On output, the matrix A is destroyed.  //
//     int     n      The number of rows and columns of the matrix A and the  //
//                    dimension of B.                                         //
//     double *B      On input, the pointer to the first element of the       //
//                    vector B[n].  On output, the vector B is replaced by the//
//                    vector X, the solution of AX = B.                       //
//     int m          The number of right hand side vectors                   //
//                    The default is m=0                                      //
//                                                                            //
//  Return Values:                                                            //
//     true :  Success                                                        //
//     false :  Failure - The matrix A is singular.                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n) {
		//  find the pivot row
		A_pivot_row = pa;
		max = abs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = abs(*(pA+row))) > max) { 
				max = dum; 
				A_pivot_row = pA; 
				pivot_row = i; 
			}
		if (max <= 0.0) 
			return false;                // the matrix A is singular
		
			// and if it differs from the current row, interchange the two rows.
			
		if (pivot_row != row) {
			for (i = row; i < n; i++) {
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<=m; k++){
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}
		
		// Perform forward substitution
		
		for (i = row+1; i<n; i++) {
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++) 
				B[i+k*n] += dum * B[row+k*n];
		}
	}
	
	// Perform backward substitution
	
	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--) {
		if ( *(pa + row) == 0.0 ) 
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum; 
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++) {
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++) 
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}


bool CWing::LLTInitialize()
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	double y ;

	CString str;
	str.Format("Initializing analysis...\r\n");
	if(m_bTrace) m_pXFile->WriteString(str);

	if(m_Type == 2)	m_QInf0 = sqrt(2.*m_Weight* 9.81 /m_Density/m_Area);
	else m_QInf0 = 0.0;

	m_bConverged = true;
	m_bWingOut = false;

	m_RelaxMax = pMiarex->m_Relax;

	if(m_bTrace) m_pXFile->WriteString("Initializing chords...\r\n");
	ComputeChords(m_NStation);
	for (int k=0; k<=m_NStation; k++){
		y   = cos(k*pi/m_NStation)* m_Span/2.0;
		m_Twist[k] = GetTwist(y);
	}
	
	if(m_bTrace) m_pXFile->WriteString("Initializing Reynolds numbers...\r\n");
	
	for (k=0; k<=m_NStation; k++) {
		m_Re[k] = m_Chord[k] * m_QInf/m_Viscosity;
	}

	return true;
}



int CWing::VLMGetPanelTotal()
{
	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span/1000.0;

	int total = 0;
	for (int i=0; i<m_NPanel; i++){
			//do not create a surface if its length is less than the critical size
//			if(abs(m_TPos[j]-m_TPos[j+1])/m_Span >0.001){
			if (abs(m_TPos[i]-m_TPos[i+1]) > MinPanelSize)
				total +=m_NXPanels[i]*m_NYPanels[i];
	}
//	if(!m_bMiddle) total *=2;
	if(!m_bIsFin) return total*2;
	else          return total;
}


bool CWing::VLMSetAutoMesh(int total)
{
	//set automatic mesh : keep it simple
	m_bVLMAutoMesh = true;
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

	for (int i=0; i<m_NPanel;i++)
	{
		d1 = 5./2./m_Span/m_Span/m_Span *8. * pow(m_TPos[i],  3) + 0.5;
		d2 = 5./2./m_Span/m_Span/m_Span *8. * pow(m_TPos[i+1],3) + 0.5;
		m_NYPanels[i] = (int) (NYTotal * (0.8*d1+0.2*d2)* (m_TPos[i+1]-m_TPos[i])/m_Span);
		m_NXPanels[i] = (int) (size/NYTotal);
		m_NXPanels[i] = min(m_NXPanels[i], MAXCHORDPANELS);

		if(m_NYPanels[i]==0) m_NYPanels[i] = 1;
		if(m_NXPanels[i]==0) m_NXPanels[i] = 1;
//		m_XPanelDist[i] = 1;//cosine distribution
//		m_YPanelDist[i] = 0;//uniformly distributed, except at the root and tip (see next)
	}

	if(VLMGetPanelTotal()>VLMMATSIZE/2){
		AfxMessageBox("Too many panels\nReduce the mesh size", MB_OK);
		return false;
	}
	return true;
}


bool CWing::CreateSurfaces(CVector T, double XTilt, double YTilt)
{
	//generic surface, LLT, VLM or Panel
	int j, Surf;
	CFoil *pFoilA, *pFoilB;
	CVector PLA, PTA, PLB, PTB, Offset, T1, NA, NB;
	CVector Trans(T.x, 0.0, T.z);
	CVector O(0.0,0.0,0.0);
	double MinPanelSize;

	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;

	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span;

	//first generate the surfaces - from left tip to right tip
	// so start with left wing, in reverse order
	m_NSurfaces = 0;
	m_MatSize = 0;

	NA.Set(0.0, 0.0, 1.0);
	NA.RotateX(O, -m_TDihedral[m_NPanel-1]);
	NB = NA;

	for (j=m_NPanel-1; j>=0; j--)
	{
		//do not create a surface if its length is less than the critical size
		if (abs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize){
			m_Surface[m_NSurfaces].m_pFoilA   = pFrame->GetFoil(m_LFoil[j+1]);
			m_Surface[m_NSurfaces].m_pFoilB   = pFrame->GetFoil(m_LFoil[j]);

			m_Surface[m_NSurfaces].m_Dihedral = -m_TDihedral[j];
			m_Surface[m_NSurfaces].m_Length   =  m_TPos[j+1] - m_TPos[j];

			PLB.x =  m_TOffset[j];		PLA.x =  m_TOffset[j+1];
			PLB.y = -m_TPos[j];			PLA.y = -m_TPos[j+1];
			PLB.z =  0.0;				PLA.z =  0.0;

			PTB.x = PLB.x+m_TChord[j];	PTA.x = PLA.x+m_TChord[j+1];
			PTB.y = PLB.y;				PTA.y = PLA.y;
			PTB.z =  0.0;				PTA.z =  0.0;

			m_Surface[m_NSurfaces].m_TwistA   =  m_TTwist[j+1];
			m_Surface[m_NSurfaces].m_TwistB   =  m_TTwist[j];

			m_Surface[m_NSurfaces].m_LA.Copy(PLA);
			m_Surface[m_NSurfaces].m_LB.Copy(PLB);
			m_Surface[m_NSurfaces].m_TA.Copy(PTA);
			m_Surface[m_NSurfaces].m_TB.Copy(PTB);

			m_Surface[m_NSurfaces].SetNormal();
			m_Surface[m_NSurfaces].RotateX(PLB, -m_TDihedral[j]);		

			//Set surface normals
			if(j>0)
			{
				NB.Set(0.0, 0.0, 1.0);
				NB.RotateX(O, -m_TDihedral[j-1]);	//is normal of next right surface
				NB += m_Surface[m_NSurfaces].Normal;	//average both normals
				NB.Normalize();							//and normalize
			}
			else NB.Set(0.0, 0.0, 1.0);	//center normal is necessarily vertical
			m_Surface[m_NSurfaces].NormalA = NA;
			m_Surface[m_NSurfaces].NormalB = NB;
			NA = NB; // this panel's right normal is next panel's left
			m_Surface[m_NSurfaces].SetTwist();

			T1.x = 0.0;
			T1.y = -m_TYProj[j] - PLB.y;
			T1.z = GetZPos(m_TPos[j]);
			m_Surface[m_NSurfaces].Translate(T1);

			m_Surface[m_NSurfaces].m_NXPanels = m_NXPanels[j];
			m_Surface[m_NSurfaces].m_NYPanels = m_NYPanels[j];

			//AVL coding + invert sine and -sine for left wing
			m_Surface[m_NSurfaces].m_XDistType = m_XPanelDist[j];

			if(m_YPanelDist[j] ==2)        m_Surface[m_NSurfaces].m_YDistType = -2;
			else if(m_YPanelDist[j] ==  1) m_Surface[m_NSurfaces].m_YDistType =  1;
			else if(m_YPanelDist[j] == -2) m_Surface[m_NSurfaces].m_YDistType =  2;
			else                           m_Surface[m_NSurfaces].m_YDistType =  0;
			m_MatSize += m_NXPanels[j]*m_NYPanels[j];

			pFoilA = m_Surface[m_NSurfaces].m_pFoilA;
			pFoilB = m_Surface[m_NSurfaces].m_pFoilB;

/*			if(j<m_NPanel-2 && !pFoilA->m_bTEFlap)
			{
				pFoil = pFrame->GetFoil(m_LFoil[j+2]);
				if(pFoil->m_bTEFlap)
					//use previous foil's hinge position to set panel positions
					pFoilA = pFoil; 
			}

			if(j>0 && !pFoilB->m_bTEFlap && pFoil->m_bTEFlap)
			{
				pFoil = pFrame->GetFoil(m_LFoil[j-1]);
				if(pFoil->m_bTEFlap)
					//use next foil's hinge position to set panel positions
					pFoilB = pFoil; 
			}*/

			CreateXPoints(m_NXPanels[j], m_XPanelDist[j], pFoilA,  pFoilB,
                          m_Surface[m_NSurfaces].m_xPointA, m_Surface[m_NSurfaces].m_xPointB,
                          m_Surface[m_NSurfaces].m_NXLead,  m_Surface[m_NSurfaces].m_NXFlap);
			m_Surface[m_NSurfaces].SetFlap();
			m_Surface[m_NSurfaces].Init();
			m_Surface[m_NSurfaces].m_bIsLeftSurf = true;
			m_Surface[m_NSurfaces].m_SineFactor = m_SineFactor;
			m_NSurfaces++;
		}
	}
	
	//next right wing
	if(!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin && m_bDoubleSymFin))
	{// do not include right surfaces for symetric fins
		for (j=0; j<m_NPanel; j++)
		{
			if(abs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize){
				m_Surface[m_NSurfaces].m_Dihedral = m_TDihedral[j];
				m_Surface[m_NSurfaces].m_Length   = m_TPos[j+1] - m_TPos[j];
				m_Surface[m_NSurfaces].m_TwistA   = m_TTwist[j];
				m_Surface[m_NSurfaces].m_TwistB   = m_TTwist[j+1];
				m_Surface[m_NSurfaces].m_pFoilA   = pFrame->GetFoil(m_RFoil[j]);
				m_Surface[m_NSurfaces].m_pFoilB   = pFrame->GetFoil(m_RFoil[j+1]);

				PLA.x =  m_TOffset[j];		PLB.x =  m_TOffset[j+1];
				PLA.y =  m_TPos[j];			PLB.y =  m_TPos[j+1];
				PLA.z =  0.0;				PLB.z =  0.0;

				PTA.x = PLA.x+m_TChord[j];	PTB.x = PLB.x+m_TChord[j+1];
				PTA.y = PLA.y;				PTB.y = PLB.y;
				PTA.z = 0.0;				PTB.z = 0.0;

				m_Surface[m_NSurfaces].m_LA.Copy(PLA);
				m_Surface[m_NSurfaces].m_LB.Copy(PLB);
				m_Surface[m_NSurfaces].m_TA.Copy(PTA);
				m_Surface[m_NSurfaces].m_TB.Copy(PTB);


				//Set surface normals
				m_Surface[m_NSurfaces].SetNormal();
				m_Surface[m_NSurfaces].RotateX(PLA, m_TDihedral[j]);

				if(j<m_NPanel-1)  	
				{
					NB.Set(0.0, 0.0, 1.0);
					NB.RotateX(O, +m_TDihedral[j+1]);		//is normal of next right surface
					NB += m_Surface[m_NSurfaces].Normal;	//average both normals
					NB.Normalize();							//and normalize
				}
				else NB = m_Surface[m_NSurfaces].Normal;	//no next right surface for average
				m_Surface[m_NSurfaces].NormalA = NA;
				m_Surface[m_NSurfaces].NormalB = NB;
				NA = NB; // this panel's right normal is next panel's left

				m_Surface[m_NSurfaces].SetTwist();

				T1.x = 0.0;
				T1.y = +m_TYProj[j] - PLA.y;
				T1.z = GetZPos(m_TPos[j]);
				m_Surface[m_NSurfaces].Translate(T1);

				m_Surface[m_NSurfaces].m_NXPanels   = m_NXPanels[j];
				m_Surface[m_NSurfaces].m_NYPanels   = m_NYPanels[j];
				m_Surface[m_NSurfaces].m_XDistType  = m_XPanelDist[j];
				m_Surface[m_NSurfaces].m_YDistType  = m_YPanelDist[j];//AVL coding

				m_MatSize += m_NXPanels[j]*m_NYPanels[j];

				pFoilA = m_Surface[m_NSurfaces].m_pFoilA;
				pFoilB = m_Surface[m_NSurfaces].m_pFoilB;

/*				if(j>0 && !pFoilA->m_bTEFlap)
				{
					pFoil = pFrame->GetFoil(m_RFoil[j-1]);
					if(pFoil->m_bTEFlap) //use previous foil's hinge position to set panel positions
						pFoilA = pFoil; 
				}

				if(j<m_NPanel-2 && !pFoilB->m_bTEFlap)
				{
					pFoil = pFrame->GetFoil(m_RFoil[j+2]);
					if(pFoil->m_bTEFlap)	//use next foil's hinge position to set panel positions
						pFoilB = pFoil; 
				}*/

				CreateXPoints(m_NXPanels[j], m_XPanelDist[j], pFoilA,  pFoilB,
                                              m_Surface[m_NSurfaces].m_xPointA,  m_Surface[m_NSurfaces].m_xPointB,
                                              m_Surface[m_NSurfaces].m_NXLead,   m_Surface[m_NSurfaces].m_NXFlap);
				m_Surface[m_NSurfaces].SetFlap();
				m_Surface[m_NSurfaces].Init();
				m_Surface[m_NSurfaces].m_bIsRightSurf = true;
				m_Surface[m_NSurfaces].m_SineFactor = m_SineFactor;
				m_NSurfaces++;
			}
		}
	}
	CVector Or(0.0,0.0,0.0);
	if(!m_bIsFin || (m_bIsFin && m_bSymFin)){
		for (j=0; j<m_NSurfaces; j++){
			m_Surface[j].RotateX(Or, XTilt);
			m_Surface[j].RotateY(Or, YTilt);
			m_Surface[j].Translate(Trans);
		}
	}
	else {
		if(m_bDoubleFin){
			if(!m_bDoubleSymFin){
			// duplicate surfaces
				Surf = m_NSurfaces;
				for(j=0; j<Surf; j++){
					m_Surface[m_NSurfaces].Copy(m_Surface[j]);
					m_Surface[m_NSurfaces].m_bIsRightSurf = true;
					m_Surface[m_NSurfaces].m_SineFactor = m_SineFactor;
					m_NSurfaces++;
				}
				m_Surface[Surf-1].m_bIsTipRight = true;
				m_Surface[Surf-1].m_bIsTipLeft  = true;
				m_Surface[0].m_bIsTipRight      = true;
				m_Surface[0].m_bIsTipLeft       = true;

				m_Surface[Surf].m_bIsTipRight = true;
				m_Surface[Surf].m_bIsTipLeft  = true;
				m_Surface[m_NSurfaces-1].m_bIsTipRight  = true;
				m_Surface[m_NSurfaces-1].m_bIsTipLeft   = true;

				Offset.Set(0.0, -T.y, 0.0);
				for(j=0; j<m_NSurfaces/2; j++){
					m_Surface[j].RotateX(Or, XTilt);
					m_Surface[j].RotateX(Or, m_Surface[j].m_Dihedral);
					m_Surface[j].RotateZ(Or, YTilt);
					m_Surface[j].Translate(Trans);
					m_Surface[j].Translate(Offset);
				}
				Offset.y = -Offset.y;
				for(j=m_NSurfaces/2; j< m_NSurfaces; j++){
					m_Surface[j].RotateX(Or, XTilt);
					m_Surface[j].RotateX(Or, -2.0*m_Surface[j].m_Dihedral);
					m_Surface[j].RotateZ(Or, -YTilt);
					m_Surface[j].Translate(Trans);
					m_Surface[j].Translate(Offset);
				}
			}
			else{
				//rotate surfaces symetrically
				int ns2 = (int)(m_NSurfaces/2);
				Offset.Set(0.0, -T.y, 0.0);
				for(j=0; j<ns2; j++){
					m_Surface[j].RotateX(Or, +XTilt);
					m_Surface[j].RotateZ(Or, YTilt);
					m_Surface[j].Translate(Trans);
					m_Surface[j].Translate(Offset);
				}
				Offset.y = -Offset.y;
				for(j=ns2; j< m_NSurfaces; j++){
					m_Surface[j].RotateX(Or, -XTilt);
					m_Surface[j].RotateZ(Or, -YTilt);
					m_Surface[j].Translate(Trans);
					m_Surface[j].Translate(Offset);
				}
				m_Surface[ns2].m_bIsTipRight = true;
				m_Surface[ns2].m_bIsTipLeft  = true;
				m_Surface[0].m_bIsTipRight   = true;
				m_Surface[0].m_bIsTipLeft    = true;

				m_Surface[ns2+1].m_bIsTipRight          = true;
				m_Surface[ns2+1].m_bIsTipLeft           = true;
				m_Surface[m_NSurfaces-1].m_bIsTipRight  = true;
				m_Surface[m_NSurfaces-1].m_bIsTipLeft   = true;

			}
		}
		else {//Not a double fin, so just a simple singel-side fin
			for (j=0; j<m_NSurfaces; j++)
			{
				m_Surface[j].RotateX(Or, XTilt);
				m_Surface[j].RotateZ(Or, YTilt);
				m_Surface[j].Translate(Trans);
				m_Surface[j].m_bIsLeftSurf  = true;
				m_Surface[j].m_bIsRightSurf = true;
			}
			m_Surface[0].m_bIsTipLeft  = true;
			m_Surface[j].m_bIsTipRight = true;//TODO check
		}
	}

	m_Surface[0].m_bIsTipLeft         = true;
	m_Surface[m_NSurfaces-1].m_bIsTipRight =  true;

	m_bWingOut = false;
	
	ComputeChords();

	return true;
}




void CWing::VLMComputeWing(double *Gamma, double *Cp, 
						   double &VDrag, double &XCP, double &YCP,
					       double &Pm,    double &Rm,    double &IYm,   double &GYm, bool bViscous)
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex   = (CMiarex*)s_pMiarex;
	CVLMDlg *pVLMDlg   = (CVLMDlg*)s_pVLMDlg;
	// calculates the lift coefficients from the vortices strengths
	int  j, k, l, p, m;
//	int side, nSide;
	CString string, strong;
	CFoil *pFoil0, *pFoil1;
	CVector Force, Normal, LeverArm, LeverArmC4, PanelForce, StripForce,Moment0,Moment1;
	CVector H, HA, HB, V1, HingeLeverArm, HingeMoment;
	double StripArea, CP, Cm4, y, z, dl, tau,NForce;
	double WXCP = 0.0;//this wing's center of pressure
	double WYCP = 0.0;
	double WZCP = 0.0;
	double PCm  = 0.0;
	double GCm  = 0.0;
	double InducedYawingMoment  = 0.0;
	double WingForce = 0.0;
	bool bOutRe      = false;
	bool bError      = false;
	bool bPointOutRe, bPointOutCl;

	int nFlap = 0;

	m_ViscousDrag = 0.0;
	p=0;
	CVector PtC4(0.0,0.0,0.0);
	CVector UInf(1.0, 0.0, 0.0);//is parallel to the horseshoe vortex trailing legs

	CVector DragVector(0.0, 0.0, 0.0);
	Moment1.Set(0.0,0.0,0.0);

	for (m=0; m< m_NStation; m++)
		m_Re[m] = m_Chord[m] * m_QInf /m_Viscosity;

	m=0;
	for (j=0; j<m_NSurfaces; j++)
	{
		pFoil0 = m_Surface[j].m_pFoilA;
		pFoil1 = m_Surface[j].m_pFoilB;
		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap)
		{
			//create a hinge unit vector and initialize hinge moment
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 0.0, HA, 0);
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 1.0, HB, 0);
			H = HB-HA;
			H.Normalize();
			m_FlapMoment[nFlap] = 0.0;
		}

		Normal = m_Surface[j].Normal;
		for (k=0; k<m_Surface[j].m_NYPanels; k++){

			bPointOutRe = false;
			bPointOutCl = false;
			StripForce.Set(0.0,0.0,0.0);//sum lift of all panels along chord
			StripArea    = 0.0;
			CP        = 0.0;
			m_CmAirf[m] = 0.0;
			dl        = m_Surface[j].Getdl(k);
			m_Surface[j].Getyz(k,y,z);
			m_Surface[j].GetC4(k, PtC4);
			tau = m_Surface[j].Getyrel(y);
			Cm4       = 0.0;

			LeverArm   = PtC4;
			LeverArm.x -= m_XCmRef;
			LeverArm.RotateY(m_Alpha);

			StripArea = m_Chord[m]* dl;

			for (l=0; l<m_Surface[j].m_NXPanels; l++){
				// for each panel along the chord, add the lift coef
				if(m_bVLM1 || m_pPanel[p].m_bIsLeading)
				{
					PanelForce  = UInf * m_pPanel[p].Vortex;
					PanelForce *= Gamma[p];
					LeverArmC4  = m_pPanel[p].VortexPos - PtC4;
					LeverArmC4.RotateY(m_Alpha);
					Moment0 = LeverArmC4 * PanelForce;
					PCm          += Moment0.y;
					m_CmAirf[m]  += Moment0.y;
				}
				else
				{
					PanelForce  = UInf * m_pPanel[p].Vortex;
					PanelForce *= Gamma[p];
					LeverArmC4  = m_pPanel[p].VortexPos - PtC4;
					LeverArmC4.RotateY(m_Alpha);
					Moment0 = LeverArmC4 * PanelForce;

					Force       = UInf* m_pPanel[p].Vortex;
					Force      *= Gamma[p+1];
					PanelForce -= Force;
					LeverArmC4  = m_pPanel[p+1].VortexPos - PtC4;
					LeverArmC4.RotateY(m_Alpha);
					Moment1 = LeverArmC4 * Force;
					PCm          += Moment0.y - Moment1.y;
					m_CmAirf[m]  += Moment0.y - Moment1.y;
				}

				Cm4        += (PtC4.x-m_pPanel[p].VortexPos.x) * PanelForce.z;
				StripForce += PanelForce;
				NForce = PanelForce.dot(Normal);
				
				XCP       += m_pPanel[p].VortexPos.x * NForce; //global center of pressure
				YCP       += m_pPanel[p].VortexPos.y * NForce; 
				WXCP      += m_pPanel[p].VortexPos.x * NForce; //this wing's center of pressure
				WYCP      += m_pPanel[p].VortexPos.y * NForce; 
				WingForce += NForce;
				CP        += m_pPanel[p].VortexPos.x * NForce;

				Cp[p]  = 2.0 * PanelForce.dot(m_pPanel[p].Normal) /m_QInf/m_pPanel[p].Area;

				if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap){
					//add hinge moment contribution
					V1 = m_pPanel[p].VortexPos - HA;
					HingeLeverArm = V1 - H * V1.dot(H);
					if(HingeLeverArm.x>0.0){
						HingeMoment = HingeLeverArm * PanelForce;
						m_FlapMoment[nFlap] += HingeMoment.dot(H) * m_Density * m_QInf;
					}
				}
				p++;
			}

//			m_CmAirf[m]     = Cm4*2.0      /StripArea/m_QInf/100.0;//error up to v2.00, corrected in v2.01
//			m_CmAirf[m]     = Cm4*2.0/chord/StripArea/m_QInf;//up to v2.04
//			Rm             += m_Cl[m] * chord * y * dl;
			m_CmAirf[m]     *=   2.0/m_Chord[m]/StripArea/m_QInf;//vectorial formulation

			NForce = StripForce.dot(Normal);
			
			m_XCPSpanRel[m]    = (CP/NForce - GetOffset(2.0*y/m_Span))/m_Chord[m];
			m_XCPSpanAbs[m]    =  CP/NForce ;

//			IYm            += m_ICd[m] * chord * y * dl;

			if(bViscous)
			{
				m_PCd[m]    = pFrame->VLMGetVar(2, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;

				m_XTrTop[m] = pFrame->VLMGetVar(5, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				
				m_XTrBot[m] = pFrame->VLMGetVar(6, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				m_ViscousDrag += m_PCd[m] * StripArea;
				VDrag         += m_PCd[m] * StripArea;
				GYm           += m_PCd[m] * StripArea * y;
				DragVector.x = m_PCd[m]*StripArea*m_QInf/2.0;
			}
			else{
				m_PCd[m]     = 0.0;
				m_XTrBot[m]  = 1.0;
				m_XTrTop[m]  = 1.0;
				DragVector.x = 0.0;
			}

			GYm           += m_F[m].y * StripArea * PtC4.x ;

			StripForce += DragVector;
			m_GeomMoment[m] = LeverArm * StripForce;
			m_CmGeom[m] = m_GeomMoment[m].y/m_Chord[m]*2.0/StripArea/m_QInf;
			m_Cm[m]     = m_CmAirf[m]+m_CmGeom[m];
			GCm += m_CmGeom[m] * m_Chord[m] * StripArea;//TODO : check formula
			Pm  +=m_GeomMoment[m].y * m_QInf;
			Rm  +=m_GeomMoment[m].x * m_QInf;
			GYm +=m_GeomMoment[m].z * m_QInf;
			InducedYawingMoment += m_ICd[m] * StripArea * m_pPanel[p].VortexPos.y ;

			if(bPointOutCl){
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f could not be interpolated\r\n", m_Cl[m]);
				strong+=string;
				if(m_bTrace) pVLMDlg->AddString(strong);
				m_bWingOut = true;

			}
			else if(bPointOutRe){
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f is outside the flight envelope\r\n", m_Cl[m]);
				strong +=string;
				if(m_bTrace) pVLMDlg->AddString(strong);
				m_bWingOut = true;
			}
			m++;
		}
		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap) nFlap++;
	}

	m_VCm  =  2.0*PCm                 /m_Area /m_MAChord /m_QInf;
	m_IYm  =      InducedYawingMoment /m_Area /m_Span    /m_QInf/m_QInf;
}


void CWing::VLMSetBending()
{
//	double StripArea[MAXSTATIONS+1];
	double ypos[MAXSTATIONS+1], zpos[MAXSTATIONS+1];
	int j,k,jj;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;
	int m = 0;
	int p = 0;

	//dynamic pressure, kg/m3
	double q = 0.5 * m_Density * m_QInf * m_QInf;

	for (j=0; j<m_NSurfaces; j++){//All surfaces
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			ypos[m] = m_pPanel[p].CollPt.y;
			zpos[m] = m_pPanel[p].CollPt.z;

			p+=m_Surface[j].m_NXPanels;
			m++;
		}
	}

	for (j=0; j<m_NStation; j++)
	{
		bm = 0.0;
		if (ypos[j]<=0){
			for (jj=0; jj<j; jj++){
				Dist.y =  -ypos[jj]+ypos[j];
				Dist.z =  -zpos[jj]+zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		else{
			for (jj=j+1; jj<m_NStation; jj++){
				Dist.y =  ypos[jj]-ypos[j];
				Dist.z =  zpos[jj]-zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		m_BendingMoment[j] = bm;
	}
}



void CWing::VLMTrefftz(double *Gamma, double & Lift, double & Drag)
{
	// applicable only for the quad vortices
	// calculates the lift and induced drag from the vortices strengths
	// using a farfield method
	// Downwash is evaluated at a distance 10 times the span downstream (i.e. infinite)
	//
	CVLMDlg *pVLMDlg = (CVLMDlg*)s_pVLMDlg;
	int j , k, l,  p,  m;
	double StripArea;
	CVector  C, V, dF, StripForce;
	CVector Wg (0.0,0.0,0.0);
	CVector VInf;
//	VInf.Set(m_QInf,0.0,0.0);
	VInf.Set(m_QInf*cos(m_Alpha*pi/180.0),0.0,m_QInf*sin(m_Alpha*pi/180.0));

	pVLMDlg->AddString("          Calculating induced drag...\r\n");

	m_CL = 0.0;
	m_InducedDrag = 0.0;

	p=0;
	m=0;
	for (j=0; j<m_NSurfaces; j++)
	{
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			StripArea  = m_Surface[j].GetStripArea(k);
			StripForce.Set(0.0,0.0,0.0);
			m_Cl[m] = 0.0;
			m_ICd[m] = 0.0;

			for(l=0; l<m_Surface[j].m_NXPanels; l++)
			{
				if(m_bVLM1 || m_pPanel[p].m_bIsTrailing)
				{
					C = m_pPanel[p].CtrlPt;
					C.x = m_Span * 100.0;

					//induced lift
					dF  = VInf * m_pPanel[p].Vortex;
					dF *= Gamma[p];
					m_Cl[m]    += 2.0*dF.z/StripArea/m_QInf/m_QInf;
					m_CL       += dF.z;
					StripForce += dF;

					//induced drag
					Wg = pVLMDlg->GetSpeedVector(C, Gamma);
					dF  = Wg * m_pPanel[p].Vortex;
					dF *= Gamma[p];
					m_ICd[m]       +=  dF.x;//*cos(m_Alpha*pi/180.0)-dF.z*sin(m_Alpha*pi/180.0);
					m_InducedDrag  +=  dF.x;//*cos(m_Alpha*pi/180.0)-dF.z*sin(m_Alpha*pi/180.0);

				}
				p++;
			}
			m_ICd[m]  *= 1.0/StripArea/m_QInf/m_QInf;
			m_F[m] = StripForce * m_Density; //F = rho * (V x Gamma) Kutta_Joukowski
			m++;
		}
	}
	Lift       += m_CL;
	Drag       +=  m_InducedDrag;

	m_InducedDrag *= -1.0       /m_Area/m_QInf;
	m_CL          *=  1.0       /m_Area/m_QInf;
}


void CWing::FillPanelList()
{
	CString str, strong;
	m_ctrlPanelList.DeleteAllItems();
//	if(m_bVLMAutoMesh) VLMSetAutoMesh();

	for(int i=0; i<=m_NPanel; i++){
		strong.Format("%d", i);
		m_ctrlPanelList.InsertItem(i, strong);

		strong = GetFormat(m_DTPos[i],2);
		m_ctrlPanelList.SetItemText(i,1,strong);

		strong = GetFormat(m_DTChord[i],2);
		m_ctrlPanelList.SetItemText(i,2,strong);

		strong = GetFormat(m_DTOffset[i],2);
		m_ctrlPanelList.SetItemText(i,3,strong);

		if(i<m_NPanel)	strong.Format("%.2f", m_TDihedral[i]);
		else			strong = " ";
		m_ctrlPanelList.SetItemText(i,4,strong);

		strong.Format("%.2f", m_TTwist[i]);
		m_ctrlPanelList.SetItemText(i,5,strong);

		if(m_bRightSide) m_ctrlPanelList.SetItemText(i,6,m_RFoil[i]);
		else             m_ctrlPanelList.SetItemText(i,6,m_LFoil[i]);

		if(i<m_NPanel){
			strong.Format("%d", m_NXPanels[i]);
			m_ctrlPanelList.SetItemText(i,7,strong);

			if(m_XPanelDist[i]==0)		strong = "Uniform";
			else if(m_XPanelDist[i]==1)	strong = "Cosine";
			m_ctrlPanelList.SetItemText(i,8,strong);

			strong.Format("%d", m_NYPanels[i]);
			m_ctrlPanelList.SetItemText(i,9,strong);

			if(m_YPanelDist[i]==0)			strong = "Uniform";
			else if(m_YPanelDist[i]==1)		strong = "Cosine";
			else if(m_YPanelDist[i]==2)		strong = "Sine";
			else if(m_YPanelDist[i]==-2)	strong = "-Sine";
			m_ctrlPanelList.SetItemText(i,10,strong);
		}
		else{
			strong = " ";
			m_ctrlPanelList.SetItemText(i,7,strong);
			m_ctrlPanelList.SetItemText(i,8,strong);
			m_ctrlPanelList.SetItemText(i,9,strong);
			m_ctrlPanelList.SetItemText(i,10,strong);
		}
	}
/*
	//Update Geometry
	Convert(false);// retrieve the data
	//
	ComputeGeometry();
	SetResults();
	InvalidateRect(&m_DrawRect);*/
}	


CString CWing::GetFormat(double f, int iPrecision)
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

		switch (iPrecision){
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

		for (int i=0; i<q; i++){
			strong.Insert(l-3*(i+1)-i-pos," ");
			l++;
		}

		str = strong;
	}
	else {
		int exp  = (int)log10(f)-1;
		str1.Format("e%d", exp);

		float main = (float)(f/pow(10.0, exp)*1.000001);
		switch (iPrecision){
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

void CWing::OnNMClickPanelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}

	POSITION pos = m_ctrlPanelList.GetFirstSelectedItemPosition();
	int sel = m_ctrlPanelList.GetNextSelectedItem(pos);

	m_iSection = pNMListView->iItem;


	InvalidateRect(&m_DrawRect);


	*pResult = 0;
}

void CWing::OnNMRClickPanelList(NMHDR *pNMHDR, LRESULT *pResult)
{
/*	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSection = pNMListView->iItem;


	*pResult = 0;*/
	POSITION pos = m_ctrlPanelList.GetFirstSelectedItemPosition();
	m_iSection = m_ctrlPanelList.GetNextSelectedItem(pos);

	InvalidateRect(&m_DrawRect);
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTXWNGMENU)){
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	pt.x, pt.y, this); 
	}
}


void CWing::OnLvnEndLabelEditPanelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}
	m_iSection = pDispInfo->item.iItem;
	
	ReadSectionData(pDispInfo->item.iItem);
    m_bChanged = true;

	ComputeGeometry();
	SetResults();
	InvalidateRect(&m_DrawRect);
}



void CWing::SetSectionData()
{
	CString strong;

 	strong.Format("%.2f",m_DTPos[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,0, strong);

	strong.Format("%.2f",m_DTChord[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,1, strong);

	strong.Format("%.2f",m_DTOffset[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,2, strong);

	strong.Format("%.2f",m_TDihedral[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,3, strong);

	strong.Format("%.2f",m_TTwist[m_iSection]);
	m_ctrlPanelList.SetItemText(m_iSection,3, strong);

	if(m_bRightSide)   m_ctrlPanelList.SetItemText(m_iSection,5,m_RFoil[m_iSection]);
	else               m_ctrlPanelList.SetItemText(m_iSection,5,m_LFoil[m_iSection]);

}

void CWing::ReadSectionData(int sel)
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
	if(res==1) m_TDihedral[sel] =d;

	strong = m_ctrlPanelList.GetItemText(sel,5);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_TTwist[sel] =d;

	if(m_bRightSide)	m_RFoil[sel] = m_ctrlPanelList.GetItemText(sel,6);
	else                m_LFoil[sel] = m_ctrlPanelList.GetItemText(sel,6);

	strong = m_ctrlPanelList.GetItemText(sel,7);
	strong.Replace(" ","");
	res = sscanf(strong, "%d",&k);
	if(res==1) m_NXPanels[sel] =k;
	m_NXPanels[sel] = min(m_NXPanels[sel], MAXCHORDPANELS);

	strong = m_ctrlPanelList.GetItemText(sel,8);
	strong.Replace(" ","");
	if(strong=="Uniform")		m_XPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_XPanelDist[sel] = 1;
	else if(strong=="Sine")		m_XPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_XPanelDist[sel] = -2;

	strong = m_ctrlPanelList.GetItemText(sel,9);
	strong.Replace(" ","");
	res = sscanf(strong, "%d",&k);
	if(res==1) m_NYPanels[sel] =k;

	strong = m_ctrlPanelList.GetItemText(sel,10);
	strong.Replace(" ","");
	if(strong=="Uniform")		m_YPanelDist[sel] = 0;
	else if(strong=="Cosine")	m_YPanelDist[sel] = 1;
	else if(strong=="Sine")		m_YPanelDist[sel] = 2;
	else if(strong=="-Sine")	m_YPanelDist[sel] = -2;

	//Update Geometry
	Convert(false);// retrieve the data
	//
}


void CWing::ReadParams()
{
//	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	m_ctrlWingName.GetWindowText(m_WingName);
	ReadSectionData(m_iSection);

	//Update Geometry
	Convert(false);// retrieve the data
	//
}


void CWing::OnUpdateMeshFromTotal() 
{
	int Total = m_ctrlVLMPanels.GetValue();	
	VLMSetAutoMesh(Total);
	FillPanelList();
	SetResults();
	InvalidateRect(m_DrawRect);
	m_ctrlOK.SetFocus();
}


void CWing::PanelComputeWing(double *Cp, 
							 double &VDrag, double &XCP, double &YCP,
							 double &Pm,    double &Rm,    double &IYm,   double &GYm, bool bViscous)
{
	CMainFrame *pFrame    = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex      = (CMiarex*)s_pMiarex;
	C3DPanelDlg *p3DDlg   = (C3DPanelDlg*)s_p3DPanelDlg;

	// calculates the lift coefficients from the vortices strengths
	int  j, k, l, p, m;
	CString string, strong;
	CFoil *pFoil0, *pFoil1;
	CVector H, HA, HB, V1, HingeLeverArm, HingeMoment;
	CVector Force, Normal, LeverArm, LeverArmC4, PanelForce, StripForce, Moment0, Moment1;
	double StripArea, CP, Cm4, chord, y, z, dl, tau,NForce;
	double WXCP = 0.0;//this wing's center of pressure
	double WYCP = 0.0;
	double WZCP = 0.0;
	double PCm  = 0.0;
	double GCm  = 0.0;
	double WingForce = 0.0;
	bool bOutRe      = false;
	bool bError      = false;
	bool bPointOutRe, bPointOutCl;
	int nFlap = 0;

	m_CL = 0.0;
	m_InducedDrag = 0.0;
	m_ViscousDrag = 0.0;
	CVector PtC4(0.0,0.0,0.0);
	CVector DragVector(0.0, 0.0, 0.0);
	Moment1.Set(0.0,0.0,0.0);

	for (m=0; m< m_NStation; m++)
		m_Re[m] = m_Chord[m] * m_QInf /m_Viscosity;

	m=0;
	p = m_Surface[0].m_NXPanels;//do not consider left tip patch

	for (j=0; j<m_NSurfaces; j++)//All surfaces
	{
		pFoil0 = m_Surface[j].m_pFoilA;
		pFoil1 = m_Surface[j].m_pFoilB;

		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap){
			//create a hinge unit vector and initialize hinge moment
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 0.0, HA, 0);
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 1.0, HB, 0);
			H = HB-HA;
			H.Normalize();
			m_FlapMoment[nFlap] = 0.0;
		}

//		nPanel = m_Surface[j].GetNPanels();
		Normal = m_Surface[j].Normal;
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			bPointOutRe = false;
			bPointOutCl = false;
			StripForce.Set(0.0,0.0,0.0);//sum lift of all panels along chord
			StripArea   = 0.0;
			CP          = 0.0;
			m_CmAirf[m] = 0.0;

			dl          = m_Surface[j].Getdl(k);
			m_Surface[j].Getyz(k,y,z);
			m_Surface[j].GetC4(k, PtC4);

			tau       = m_Surface[j].Getyrel(y);
			chord     = m_Surface[j].GetChord(k);
			Cm4       = 0.0;

			LeverArm = PtC4;
			LeverArm.x -= m_XCmRef;
			LeverArm.RotateY(m_Alpha);

			for (l=0; l<2*m_Surface[j].m_NXPanels; l++)
			{
				PanelForce = m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;
				StripForce += PanelForce;
				NForce = PanelForce.dot(Normal);

				LeverArmC4 = m_pPanel[p].CollPt- PtC4;
				LeverArmC4.RotateY(m_Alpha);

				Moment0 = LeverArmC4 * PanelForce;
				PCm          += Moment0.y;
				m_CmAirf[m]  += Moment0.y;

				StripArea    += m_pPanel[p].Area;

				Cm4   += (PtC4.x-m_pPanel[p].CollPt.x) * PanelForce.z;
				
				XCP       += m_pPanel[p].CollPt.x * NForce; //global center of pressure
				YCP       += m_pPanel[p].CollPt.y * NForce; 
				WXCP      += m_pPanel[p].CollPt.x * NForce; //this wing's center of pressure
				WYCP      += m_pPanel[p].CollPt.y * NForce; 
				WingForce += NForce;
				CP        += m_pPanel[p].CollPt.x * NForce;

				if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap){
					//add hinge moment contribution
					V1 = m_pPanel[p].VortexPos - HA;
					HingeLeverArm = V1 - H * V1.dot(H);
					if(HingeLeverArm.x>0.0){
						HingeMoment = HingeLeverArm * PanelForce;
						m_FlapMoment[nFlap] += HingeMoment.dot(H) * m_Density * m_QInf * m_QInf/2.0;
					}
				}
				p++;
			}

			StripArea /=2.0; //average over top and bottom

//			m_Cl[m]  = StripForce.z/StripArea ;
///			m_CL    +=     StripForce.z;


			NForce = StripForce.dot(Normal);
			
			m_XCPSpanRel[m]    = (CP/NForce - GetOffset(2.0*y/m_Span))/chord;
			m_XCPSpanAbs[m]    =  CP/NForce ;

//			if(pMiarex->m_bTrefftz)
//				Ft = StripForce.z * sin(Ai[m]*pi/180.0);// + StripForce.x * cos(Ai[m]*pi/180.0);
//			else 
//				Ft = StripForce.x;

//			m_ICd[m]        = -1.0 * Ft/StripArea ;

//			m_InducedDrag  += Ft ;

			if(bViscous){
				m_PCd[m]    = pFrame->VLMGetVar(2, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
//TRACE("m_Cl[%d]=%10.3f     m_PCd[%d]=%10.3f\n", m, m_Cl[m],m, m_PCd[m]);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;

				m_XTrTop[m] = pFrame->VLMGetVar(5, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				
				m_XTrBot[m] = pFrame->VLMGetVar(6, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				m_ViscousDrag += m_PCd[m] * m_Chord[m] * dl;
				VDrag         += m_PCd[m] * m_Chord[m] * dl;
				GYm           += m_PCd[m] * m_Chord[m] * y* dl;
				DragVector.x = m_PCd[m]*StripArea*m_QInf/2.0;
			}
			else{
				m_PCd[m]     = 0.0;
				m_XTrBot[m]  = 1.0;
				m_XTrTop[m]  = 1.0;
				DragVector.x = 0.0;
			}

			GYm           += m_F[m].y * m_Chord[m] * PtC4.x * dl;

			StripForce += DragVector;

			m_CmAirf[m]    *= 1.0/chord/StripArea;//vectorial formulation
			m_GeomMoment[m] = LeverArm * StripForce;
			m_CmGeom[m]     = m_GeomMoment[m].y/chord/StripArea;
			m_Cm[m]         = m_CmAirf[m]+m_CmGeom[m];

			GCm += m_CmGeom[m] * m_Chord[m] * m_Chord[m] * dl;
			Pm  +=m_GeomMoment[m].y;
			Rm  +=m_GeomMoment[m].x;
			GYm +=m_GeomMoment[m].z;

			if(bPointOutCl){
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f could not be interpolated\r\n", m_Cl[m]);
				strong+=string;
				if(m_bTrace) p3DDlg->AddString(strong);
				m_bWingOut = true;

			}
			else if(bPointOutRe){
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f is outside the flight envelope\r\n", m_Cl[m]);
				strong +=string;
				if(m_bTrace) p3DDlg->AddString(strong);
				m_bWingOut = true;
			}
			m++;
//			p3DDlg->SetProgress((int)(10.0*(double)m_MatSize/(double)p3DDlg->m_MatSize),(double)m/(double)m_NStation);
		}
		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap) nFlap++;
	}

//	Lift    += m_CL;
//	IDrag   +=  m_InducedDrag;
//	m_CL    /=  m_Area;

	m_VCm    =  PCm  /m_Area/m_MAChord;
}


void CWing::PanelTrefftz(double *Cp, double *Mu, double *Sigma, double &Lift, double &Drag, bool bTilted)
{
	// calculates the induced lift and drag from the vortices or wake panels strength
	// using a farfield method
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
	//
	C3DPanelDlg *p3DDlg = (C3DPanelDlg*)s_p3DPanelDlg;

	int j , k, l,  p,  m;
	double StripArea;
	double InducedAngle, Wgz;
	double IYm;
	CVector  C, V, Wg, StripForce;
	CVector VInf;
	VInf.Set(m_QInf*cos(m_Alpha*pi/180.0),0.0,m_QInf*sin(m_Alpha*pi/180.0));
//	VInf.set(1.0, 0.0, 0.0));

	//dynamic pressure, kg/m3
	double q = 0.5 * m_Density * m_QInf * m_QInf;

	p3DDlg->AddString( "       Calculating induced drag...\r\n");

	m_CL          = 0.0;
	m_InducedDrag = 0.0;
	IYm           = 0.0;

	p=0;
	m=0;
	for (j=0; j<m_NSurfaces; j++)
	{
		if(m_Surface[j].m_bIsTipLeft) p+=m_Surface[j].m_NXPanels;//tip patch panels
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			m_Surface[j].GetTrailingPt(C, k);
			C.x = p3DDlg->m_pWPolar->m_TotalWakeLength * m_MAChord;
			StripArea  = m_Surface[j].GetStripArea(k);
			m_Cl[m]    = 0.0;
			m_ICd[m]   = 0.0;
			StripForce.Set(0.0, 0.0, 0.0);

			//induced lift
			for(l=0; l<2*m_Surface[j].m_NXPanels; l++)
			{
				StripForce += m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;
				p++;
			}
			m_Cl[m]  = StripForce.z/StripArea ;
			m_CL    += StripForce.z;

			//the induced drag is calculated in wind axis, whether the geom is tilted or not
			Wg = p3DDlg->GetSpeedVector(C, Mu, Sigma);
			//convert Wg in wind axis
			Wgz = -Wg.x*sin(m_Alpha*pi/180.0) + Wg.z * cos(m_Alpha*pi/180.0);
			InducedAngle = atan2(Wg.z, m_QInf);// apparent induced angle, in rad
			
			m_ICd[m]       = -(StripForce.z * sin(InducedAngle))/StripArea;
//			m_ICd[m]       = (StripForce.x * cos(InducedAngle) + StripForce.z * sin(InducedAngle))/StripArea;
			m_InducedDrag +=  StripForce.z * sin(InducedAngle) ;//+ StripForce.x * cos(InducedAngle);
			m_Ai[m]        = InducedAngle*180/pi;
			IYm           += m_ICd[m] * StripArea * C.y;

			m_F[m]   = StripForce * q; 

			m++;
		}		
	}

	Lift      += m_CL;
	Drag      += m_InducedDrag;

	m_CL          *=  1.0       /m_Area;
	m_InducedDrag *= -1.0       /m_Area;
	m_IYm = IYm/m_Area;
}



void CWing::PanelSetBending()
{
	double ypos[MAXSTATIONS+1], zpos[MAXSTATIONS+1];
	int j,k,jj;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;
	int m =0;
	int p= m_Surface[0].m_NXPanels;

	for (j=0; j<m_NSurfaces; j++){//All surfaces
		for (k=0; k<m_Surface[j].m_NYPanels; k++){
			ypos[m] = m_pPanel[p].CollPt.y;
			zpos[m] = m_pPanel[p].CollPt.z;

			p+=2*m_Surface[j].m_NXPanels;
			m++;
		}
	}

	for (j=0; j<m_NStation; j++){
		bm = 0.0;
		if (ypos[j]<=0){
			for (jj=0; jj<j; jj++){
				Dist.y =  -ypos[jj]+ypos[j];
				Dist.z =  -zpos[jj]+zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		else{
			for (jj=j+1; jj<m_NStation; jj++){
				Dist.y =  ypos[jj]-ypos[j];
				Dist.z =  zpos[jj]-zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		m_BendingMoment[j] = bm;
	}
}


void CWing::CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB,
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









