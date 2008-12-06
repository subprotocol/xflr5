/****************************************************************************

    CUFOListDlg Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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

// WingListDlg.cpp 
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../Miarex/Miarex.h"
#include "../Miarex/Plane.h"
#include "../XDirect/XDirect.h"
#include "../main/MainFrm.h"
#include ".\ufolistdlg.h"


IMPLEMENT_DYNAMIC(CUFOListDlg, CDialog)
CUFOListDlg::CUFOListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUFOListDlg::IDD, pParent)
{
	m_pMiarex    = NULL;
	m_pXDirect     = NULL;
	m_pFrame     = NULL;
	m_bWing      = false;
	m_iSelection = 0;
	m_poaWing  = NULL;
	m_poaFoil  = NULL;
	m_poaPlane = NULL;
	memset(m_ColSize, 0 , sizeof(m_ColSize));

	if(m_bWing)
	{
		m_ColSize[0] = 130;
		m_ColSize[1] = 70;
		m_ColSize[2] = 70;
		m_ColSize[3] = 60;
		m_ColSize[4] = 60;
		m_ColSize[5] = 60;
		m_ColSize[6] = 60;
		m_ColSize[7] = 70;
		m_ColSize[8] = 70;
	}
	else
	{
		m_ColSize[0] = 160;
		m_ColSize[1] = 70;
		m_ColSize[2] = 70;
		m_ColSize[3] = 70;
		m_ColSize[4] = 70;
		m_ColSize[5] = 70;
		m_ColSize[6] = 70;
		m_ColSize[7] = 70;
		m_ColSize[8] = 70;
		m_ColSize[9] = 70;
		m_ColSize[10] = 70;
		m_ColSize[11] = 70;
	}
}


CUFOListDlg::~CUFOListDlg()
{
}

void CUFOListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UFOLIST, m_ctrlUFOList);
	DDX_Control(pDX, IDC_DELETE,  m_ctrlDelete);
	DDX_Control(pDX, IDC_SELECT,  m_ctrlSelect);
	DDX_Control(pDX, IDC_RENAME,  m_ctrlRename);
	DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CUFOListDlg, CDialog)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_NOTIFY(NM_CLICK,  IDC_UFOLIST, OnNMClickUFOList)
	ON_NOTIFY(NM_RCLICK, IDC_UFOLIST, OnNMRClickUFOList)
	ON_NOTIFY(NM_DBLCLK, IDC_UFOLIST, OnNMDblClickUFOList)
	ON_NOTIFY(HDN_ITEMCHANGED, 0, OnHdnItemchangedUfolist)
END_MESSAGE_MAP()


// Gestionnaires de messages CUFOListDlg

BOOL CUFOListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString Name;

	m_ctrlUFOList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	if(m_bWing)
	{
		SetWindowText("Wing Management");
		m_ctrlUFOList.InsertColumn(0,"Name",LVCFMT_LEFT, m_ColSize[0]);
		m_ctrlUFOList.InsertColumn(1,"Span",LVCFMT_RIGHT,m_ColSize[1]);
		m_ctrlUFOList.InsertColumn(2,"Wing Area",LVCFMT_RIGHT,m_ColSize[2]);
		m_ctrlUFOList.InsertColumn(3,"AR",LVCFMT_RIGHT,m_ColSize[3]);
		m_ctrlUFOList.InsertColumn(4,"TR",LVCFMT_RIGHT,m_ColSize[4]);
		m_ctrlUFOList.InsertColumn(5,"MAC",LVCFMT_RIGHT,m_ColSize[5]);
		m_ctrlUFOList.InsertColumn(6,"Sweep",LVCFMT_RIGHT,m_ColSize[6]);
		m_ctrlUFOList.InsertColumn(7,"Elev Area",LVCFMT_RIGHT,m_ColSize[7]);
		m_ctrlUFOList.InsertColumn(8,"Fin Area",LVCFMT_RIGHT,m_ColSize[8]);
		FillUFOList();

		CMiarex *pMiarex = (CMiarex*)m_pMiarex;
		if(pMiarex->m_pCurPlane)     Name = pMiarex->m_pCurPlane->m_PlaneName;
		else if(pMiarex->m_pCurWing) Name = pMiarex->m_pCurWing->m_WingName;
	}
	else
	{
		SetWindowText("Foil Management");
		m_ctrlUFOList.InsertColumn(0,"Foil Name",LVCFMT_LEFT, m_ColSize[0]);
		m_ctrlUFOList.InsertColumn(1,"Thickness",LVCFMT_RIGHT,m_ColSize[1]);
		m_ctrlUFOList.InsertColumn(2,"At",LVCFMT_RIGHT,m_ColSize[2]);
		m_ctrlUFOList.InsertColumn(3,"Camber",LVCFMT_RIGHT,m_ColSize[3]);
		m_ctrlUFOList.InsertColumn(4,"At",LVCFMT_RIGHT,m_ColSize[4]);
		m_ctrlUFOList.InsertColumn(5,"Points",LVCFMT_RIGHT,m_ColSize[5]);
		m_ctrlUFOList.InsertColumn(6,"TEFlap Angle",LVCFMT_RIGHT,m_ColSize[6]);
		m_ctrlUFOList.InsertColumn(7,"TEXHinge",LVCFMT_RIGHT,m_ColSize[7]);
		m_ctrlUFOList.InsertColumn(8,"TEYHinge",LVCFMT_RIGHT,m_ColSize[8]);
		m_ctrlUFOList.InsertColumn(9,"LEFlap Angle",LVCFMT_RIGHT,m_ColSize[9]);
		m_ctrlUFOList.InsertColumn(10,"LEXHinge",LVCFMT_RIGHT,m_ColSize[10]);
		m_ctrlUFOList.InsertColumn(11,"LEYHinge",LVCFMT_RIGHT,m_ColSize[11]);
		FillFoilList();
		CXDirect *pXDirect = (CXDirect*)m_pXDirect;
		if(pXDirect->m_pCurFoil) Name = pXDirect->m_pCurFoil->m_FoilName;
	}

	for (int i=0; i<m_ctrlUFOList.GetItemCount(); i++){
		if(Name==m_ctrlUFOList.GetItemText(i,0)){
			m_ctrlUFOList.EnsureVisible(i,FALSE);
			m_ctrlUFOList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}

	if(!m_ctrlUFOList.GetItemCount()) EnableCommands(false);
	m_ctrlUFOList.SetFocus();
	return FALSE;  
}

void CUFOListDlg::FillFoilList()
{
	CFoil *pFoil;
	CString strong;
	CString percent = "%";
	m_ctrlUFOList.DeleteAllItems();

	for(int i=0;i<m_poaFoil->GetSize();i++)
	{
		pFoil = (CFoil*)m_poaFoil->GetAt(i);
		m_ctrlUFOList.InsertItem(i,pFoil->m_FoilName);
		strong.Format("%.2f", pFoil->m_fThickness*100.0);
		strong+= percent;
		m_ctrlUFOList.SetItemText(i,1,strong);

		strong.Format("%.1f", pFoil->m_fXThickness*100.0);
		strong+= percent;
		m_ctrlUFOList.SetItemText(i,2,strong);

		strong.Format("%.2f", pFoil->m_fCamber*100.0);
		strong+= percent;
		m_ctrlUFOList.SetItemText(i,3,strong);

		strong.Format("%.1f", pFoil->m_fXCamber*100.0);
		strong+= percent;
		m_ctrlUFOList.SetItemText(i,4,strong);

		strong.Format("%d", pFoil->n);
		m_ctrlUFOList.SetItemText(i,5,strong);

		if(pFoil->m_bTEFlap){
			strong.Format("%.1f°", pFoil->m_TEFlapAngle);
			m_ctrlUFOList.SetItemText(i,6,strong);

			strong.Format("%.1f", pFoil->m_TEXHinge);
			strong+= percent;
			m_ctrlUFOList.SetItemText(i,7,strong);

			strong.Format("%.1f", pFoil->m_TEYHinge);
			strong+= percent;
			m_ctrlUFOList.SetItemText(i,8,strong);
		}
		if(pFoil->m_bLEFlap){
			strong.Format("%.1f°", pFoil->m_LEFlapAngle);
			m_ctrlUFOList.SetItemText(i,9,strong);

			strong.Format("%.1f", pFoil->m_LEXHinge);
			strong+= percent;
			m_ctrlUFOList.SetItemText(i,10,strong);

			strong.Format("%.1f", pFoil->m_LEYHinge);
			strong+= percent;
			m_ctrlUFOList.SetItemText(i,11,strong);
		}
	}
}
void CUFOListDlg::FillUFOList()
{
	int i, k;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString str;
	CString strong;
	CWing *pWing;
//	CWPolar * pWPolar;
	CPlane *pPlane;
	CString len, surf;
	GetLengthUnit(len,pFrame->m_LengthUnit);
	GetAreaUnit(surf, pFrame->m_AreaUnit);

	m_ctrlUFOList.DeleteAllItems();

	for (i=0; i<m_poaWing->GetSize(); i++){
		pWing = (CWing*)m_poaWing->GetAt(i);
		m_ctrlUFOList.InsertItem(i,pWing->m_WingName);

		strong.Format("%.2f", pWing->m_Span*pFrame->m_mtoUnit);
		m_ctrlUFOList.SetItemText(i,1,strong+len);

		strong.Format("%.2f", pWing->m_Area*pFrame->m_m2toUnit);
		m_ctrlUFOList.SetItemText(i,2,strong+surf);

		strong.Format("%.1f", pWing->m_AR);
		m_ctrlUFOList.SetItemText(i,3,strong);

		strong.Format("%.1f", pWing->m_TR);
		m_ctrlUFOList.SetItemText(i,4,strong);

		strong.Format("%.2f", pWing->m_MAChord*pFrame->m_mtoUnit);
		m_ctrlUFOList.SetItemText(i,5,strong+len);

		strong.Format("%.1f°", pWing->GetAverageSweep());
		m_ctrlUFOList.SetItemText(i,6,strong);

		m_ctrlUFOList.SetItemText(i,7," ");

		m_ctrlUFOList.SetItemText(i,8," ");

/*		nPlrs = 0;
		for(j=0; j<m_poaWPolar->GetSize();j++){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(j);
			if(pWPolar->m_UFOName==pWing->m_WingName) nPlrs++;
		}

		strong.Format("%d", nPlrs);
		m_ctrlUFOList.SetItemText(i,9,strong);*/
	}

	for (k=0; k<m_poaPlane->GetSize(); k++){
		pPlane = (CPlane*)m_poaPlane->GetAt(k);
		pWing  = &pPlane->m_Wing;
		m_ctrlUFOList.InsertItem(k+i,pPlane->m_PlaneName);

		strong.Format("%.2f", pWing->m_Span*pFrame->m_mtoUnit);
		m_ctrlUFOList.SetItemText(k+i,1,strong+len);

		strong.Format("%.2f", pWing->m_Area*pFrame->m_m2toUnit);
		m_ctrlUFOList.SetItemText(k+i,2,strong+surf);


		strong.Format("%.1f", pWing->m_AR);
		m_ctrlUFOList.SetItemText(k+i,3,strong);

		strong.Format("%.1f", pWing->m_TR);
		m_ctrlUFOList.SetItemText(k+i,4,strong);

		strong.Format("%.2f", pWing->m_MAChord*pFrame->m_mtoUnit);
		m_ctrlUFOList.SetItemText(k+i,5,strong+len);

		strong.Format("%.1f°", pWing->GetAverageSweep());
		m_ctrlUFOList.SetItemText(k+i,6,strong);

		strong.Format("%.2f", pPlane->m_Stab.m_Area*pFrame->m_m2toUnit);
		m_ctrlUFOList.SetItemText(k+i,7,strong+surf);

		strong.Format("%.2f", pPlane->m_Fin.m_Area*pFrame->m_m2toUnit);
		m_ctrlUFOList.SetItemText(k+i,8,strong+surf);
/*		nPlrs = 0;
		for(j=0; j<m_poaWPolar->GetSize();j++){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(j);
			if(pWPolar->m_UFOName==pPlane->m_PlaneName) nPlrs++;
		}
		strong.Format("%d", nPlrs);
		m_ctrlUFOList.SetItemText(k+i,9,strong);*/

	}
}

void CUFOListDlg::OnDelete()
{
	if(!m_ctrlUFOList.GetItemCount()) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CXDirect *pXDirect = (CXDirect*)m_pXDirect;
	POSITION pos = m_ctrlUFOList.GetFirstSelectedItemPosition();
	int sel = m_ctrlUFOList.GetNextSelectedItem(pos);
	CString Name, strong;
	Name = m_ctrlUFOList.GetItemText(sel,0);

	if(m_bWing){
		CWing *pWing = pMiarex->GetWing(Name);
		CPlane *pPlane = pMiarex->GetPlane(Name);
		if(pPlane)	   strong = "Are you sure you want to delete the plane :\n" + pPlane->m_PlaneName +"?\n";
		else if(pWing) strong = "Are you sure you want to delete the wing :\n" +  pWing->m_WingName +"?\n";
		if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;
		if(pWing) {
			pFrame->DeleteWing(pWing);
		}
		else if(pPlane) {
			pFrame->DeletePlane(pPlane);
		}
		FillUFOList();
	}
	else{
		CFoil *pFoil = pFrame->GetFoil(Name);
		if (pFoil) {
			pFrame->DeleteFoil(pFoil,true);
		}
		FillFoilList();
	}

	if(m_ctrlUFOList.GetItemCount()){
		sel--;
		if(sel<0) sel = 0;
		m_ctrlUFOList.EnsureVisible(sel,FALSE);
		m_ctrlUFOList.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		Name = m_ctrlUFOList.GetItemText(sel,0);
	}
	else EnableCommands(false);

	if(m_bWing)	{
		pMiarex->SetUFO(Name);
		if(pMiarex->m_iView==1)			pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==2)	pMiarex->CreateWPolarCurves();
		pFrame->UpdateUFOs();
		pMiarex->UpdateView();
	}
	else{
		pXDirect->SetFoil(Name);
		if(!pXDirect->m_bPolar)	pXDirect->CreateOppCurves();
		else					pXDirect->CreatePolarCurves();
		pFrame->UpdateFoils();
		pXDirect->UpdateView();
	}
}

void CUFOListDlg::OnSelect()
{	
	if(!m_ctrlUFOList.GetItemCount()) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	POSITION pos = m_ctrlUFOList.GetFirstSelectedItemPosition();
	int sel = m_ctrlUFOList.GetNextSelectedItem(pos);
	CString Name;
	Name = m_ctrlUFOList.GetItemText(sel,0);

	if(m_bWing){
		CMiarex *pMiarex = (CMiarex*)m_pMiarex;
		pMiarex->SetUFO(Name);
		pFrame->UpdateUFOs();
		pMiarex->UpdateView();
	}
	else{
		CXDirect *pXDirect = (CXDirect*)m_pXDirect;
		pXDirect->SetFoil(Name);
		pFrame->UpdateFoils();
		pXDirect->UpdateView();
	}
}

void CUFOListDlg::OnRename()
{
	if(!m_ctrlUFOList.GetItemCount()) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CXDirect *pXDirect = (CXDirect*)m_pXDirect;
	POSITION pos = m_ctrlUFOList.GetFirstSelectedItemPosition();
	int sel = m_ctrlUFOList.GetNextSelectedItem(pos);
	CString Name;
	Name = m_ctrlUFOList.GetItemText(sel,0);

	if(m_bWing){
		Name = pMiarex->RenameUFO(Name);
		FillUFOList();
		pFrame->UpdateUFOs();
	}
	else {
		CFoil *pFoil = pFrame->GetFoil(Name);
		if (pFoil) {
			pFrame->RenameFoil(pFoil);
			Name = pFoil->m_FoilName;
		}
		FillFoilList();
		pFrame->UpdateFoils();
	}

	for (int i=0; i<m_ctrlUFOList.GetItemCount(); i++){
		if(Name==m_ctrlUFOList.GetItemText(i,0)){
			m_ctrlUFOList.EnsureVisible(i,FALSE);
			m_ctrlUFOList.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			break;
		}
	}
	if(m_bWing)	{
		if(pMiarex->m_iView==1)			pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==2)	pMiarex->CreateWPolarCurves();
		pFrame->UpdateUFOs();
		pMiarex->UpdateView();
	}
	else{
		if(!pXDirect->m_bPolar)	pXDirect->CreateOppCurves();
		else					pXDirect->CreatePolarCurves();
		pFrame->UpdateFoils();
		pXDirect->UpdateView();
	}
}





void CUFOListDlg::OnNMClickUFOList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	*pResult = 0;

	m_iSelection = pNMListView->iItem;
}

void CUFOListDlg::OnNMRClickUFOList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSelection = pNMListView->iItem;


	*pResult = 0;
}

void CUFOListDlg::OnNMDblClickUFOList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSelection = pNMListView->iItem;

	OnSelect();
	OnOK();

	*pResult = 0;
}

void CUFOListDlg::EnableCommands(bool bEnable)
{
	m_ctrlDelete.EnableWindow(bEnable);
	m_ctrlSelect.EnableWindow(bEnable);
	m_ctrlRename.EnableWindow(bEnable);
}


BOOL CUFOListDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		if(GetDlgItem(IDOK) != GetFocus())
		{
			OnSelect();
			m_ctrlOK.SetFocus();
		}
		else OnOK();
		return true;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}


void CUFOListDlg::OnHdnItemchangedUfolist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	*pResult = 0;
	int i;
	if(m_bWing)
	{
		for (i=0; i<9; i++)
		{
			m_ColSize[i] = m_ctrlUFOList.GetColumnWidth(i);
		}
	}
	else
	{
		for (i=0; i<12; i++)
		{
			m_ColSize[i] = m_ctrlUFOList.GetColumnWidth(i);
		}
	}
}
