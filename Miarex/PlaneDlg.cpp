/****************************************************************************

	PlaneDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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
#include "../Globals.h"
#include "../MainFrame.h" 
#include "Miarex.h"
#include "WingDlg.h"
#include "PlaneDlg.h"
#include "ImportWingDlg.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <math.h>

QList <void*> *PlaneDlg::s_poaWing;
QList <void*> *PlaneDlg::s_poaBody;
void *PlaneDlg::s_pMiarex;
void *PlaneDlg::s_pMainFrame;


PlaneDlg::PlaneDlg()
{
	setWindowTitle("Plane Editor");
	m_pPlane = NULL;

	pi = 3.141592654;

	m_bAcceptName    = true;
	m_bChanged      = false;
	SetupLayout();

	connect(m_pctrlBiplane, SIGNAL(clicked()), this, SLOT(OnBiplane()));
	connect(m_pctrlStabCheck,     SIGNAL(clicked()), this, SLOT(OnStab()));
	connect(m_pctrlFinCheck,     SIGNAL(clicked()), this, SLOT(OnFin()));

	connect(m_pctrlSymFin,    SIGNAL(clicked()), this, SLOT(OnSymFin()));
	connect(m_pctrlDoubleFin, SIGNAL(clicked()), this, SLOT(OnDoubleFin()));

	connect(m_pctrlDefineWing,  SIGNAL(clicked()), this, SLOT(OnDefineWing()));
	connect(m_pctrlImportWing,  SIGNAL(clicked()), this, SLOT(OnImportWing()));
	connect(m_pctrlExportWing,  SIGNAL(clicked()), this, SLOT(OnExportWing()));
	connect(m_pctrlDefineWing2, SIGNAL(clicked()), this, SLOT(OnDefineWing2()));
	connect(m_pctrlImportWing2, SIGNAL(clicked()), this, SLOT(OnImportWing2()));
	connect(m_pctrlExportWing2, SIGNAL(clicked()), this, SLOT(OnExportWing2()));
	connect(m_pctrlDefineStab, SIGNAL(clicked()), this, SLOT(OnDefineStab()));
	connect(m_pctrlDefineFin, SIGNAL(clicked()), this, SLOT(OnDefineFin()));
	connect(m_pctrlEditBody, SIGNAL(clicked()), this, SLOT(OnEditBody()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlBody, SIGNAL(clicked()), this, SLOT(OnBodyCheck()));
	connect(m_pctrlBodyList,  SIGNAL(activated(int)),    this, SLOT(OnSelChangeBodyList(int)));

	connect(m_pctrlXLEStab,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlWingTilt, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlStabTilt, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlFinTilt,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEStab,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXLEFin,   SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEFin,   SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXBody,    SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZBody,    SIGNAL(editingFinished()), this, SLOT(OnChanged()));
}



void PlaneDlg::ComputePlane(void)
{
	if(m_pPlane->m_bStab)
	{
		double SLA = m_pPlane->m_LEStab.x + m_pPlane->m_Stab.m_TChord[0]/4.0 - m_pPlane->m_Wing.m_TChord[0]/4.0;
		double area = m_pPlane->m_Wing.m_ProjectedArea;
		if(m_pPlane->m_bBiplane) area += m_pPlane->m_Wing2.m_ProjectedArea;

		double ProjectedArea = 0.0;
		for (int i=0;i<m_pPlane->m_Stab.m_NPanel; i++)
		{
			ProjectedArea += m_pPlane->m_Stab.m_TLength[i+1]*(m_pPlane->m_Stab.m_TChord[i]+m_pPlane->m_Stab.m_TChord[i+1])/2.0
							*cos(m_pPlane->m_Stab.m_TDihedral[i]*pi/180.0)*cos(m_pPlane->m_Stab.m_TDihedral[i]*pi/180.0);

		}
		ProjectedArea *=2.0;
		m_pPlane->m_TailVolume = ProjectedArea * SLA / area/m_pPlane->m_Wing.m_MAChord ;
	}
	else m_pPlane->m_TailVolume = 0.0;

	m_pPlane->m_Fin.m_bDoubleFin = m_pPlane->m_bDoubleFin;
	m_pPlane->m_Fin.m_bSymFin    = m_pPlane->m_bSymFin;
	m_pPlane->m_Volume = m_pPlane->m_Wing.m_Volume;
	if(m_pPlane->m_bBiplane)	m_pPlane->m_Volume += m_pPlane->m_Wing2.m_Volume;
	if(m_pPlane->m_bStab)		m_pPlane->m_Volume += m_pPlane->m_Stab.m_Volume;
	if(m_pPlane->m_bFin)
	{
		m_pPlane->m_Volume += m_pPlane->m_Fin.m_Volume;
		if(m_pPlane->m_bDoubleFin || m_pPlane->m_bSymFin) m_pPlane->m_Volume += m_pPlane->m_Fin.m_Volume;
	}
}


void PlaneDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_pctrlWingTilt->SetPrecision(1);
	m_pctrlStabTilt->SetPrecision(1);
	m_pctrlFinTilt->SetPrecision(1);

	QString len, surf;
	GetLengthUnit(len, pMainFrame->m_LengthUnit);
	GetAreaUnit(surf,  pMainFrame->m_AreaUnit);

	m_pctrlLen1->setText(len);
	m_pctrlLen2->setText(len);
	m_pctrlLen3->setText(len);
	m_pctrlLen4->setText(len);
	m_pctrlLen5->setText(len);
	m_pctrlLen6->setText(len);
	m_pctrlLen7->setText(len);
	m_pctrlLen8->setText(len);
	m_pctrlLen9->setText(len);
	m_pctrlLen10->setText(len);
	m_pctrlLen11->setText(len);

	m_pctrlSurf1->setText(surf);
	m_pctrlSurf2->setText(surf);
	m_pctrlSurf3->setText(surf);

	m_pctrlVolume->setText(len+"3");

	m_pctrlPlaneName->setText(m_pPlane->m_PlaneName);

	SetParams();
	SetResults();

	if(!m_bAcceptName) m_pctrlPlaneName->setEnabled(false);
	m_bChanged = false;

	m_pPlane->m_bDoubleSymFin = true;
}


void PlaneDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
				return;
			}
			else
			{
				OnOK();
				return;
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}
}


void PlaneDlg::OnBiplane()
{
	if(m_pctrlBiplane->isChecked()) m_pPlane->m_bBiplane = true; else m_pPlane->m_bBiplane = false;
	if(m_pPlane->m_bBiplane)
	{
		m_pctrlDefineWing2->setEnabled(true);
		m_pctrlImportWing2->setEnabled(true);
		m_pctrlExportWing2->setEnabled(true);
		m_pctrlWingTilt2->setEnabled(true);
		m_pctrlZLEWing2->setEnabled(true);
		m_pctrlXLEWing2->setEnabled(true);
	}
	else
	{
		m_pctrlDefineWing2->setEnabled(false);
		m_pctrlImportWing2->setEnabled(false);
		m_pctrlExportWing2->setEnabled(false);
		m_pctrlWingTilt2->setEnabled(false);
		m_pctrlZLEWing2->setEnabled(false);
		m_pctrlXLEWing2->setEnabled(false);
	}
}



void PlaneDlg::OnBodyCheck()
{
	m_bChanged = true;
	if(m_pctrlBody->isChecked())
	{
		if(m_pctrlBodyList->count())
		{
			m_pctrlBodyList->setEnabled(true);
			m_pctrlXBody->setEnabled(true);
			m_pctrlZBody->setEnabled(true);
			m_pctrlEditBody->setEnabled(true);
			m_pPlane->m_bBody=true;

			OnSelChangeBodyList();
		}
		else
		{
			m_pPlane->m_bBody=false;
			m_pctrlBodyList->setEnabled(false);
			m_pctrlXBody->setEnabled(false);
			m_pctrlZBody->setEnabled(false);
			m_pctrlEditBody->setEnabled(false);
		}
	}
	else
	{
		m_pPlane->m_bBody=false;
		m_pctrlBodyList->setEnabled(false);
		m_pctrlXBody->setEnabled(false);
		m_pctrlZBody->setEnabled(false);
		m_pctrlEditBody->setEnabled(false);
	}
	SetResults();
}


void PlaneDlg::OnCancel()
{
	if(m_bChanged)
	{
		QString strong = tr("Discard the changes ?");
		if (QMessageBox::Yes != QMessageBox::question(this, "Question", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
			return;
	}
//	reject();
	done(QDialog::Rejected);
}



void PlaneDlg::OnChanged()
{
	m_bChanged = true;
	ReadParams();
	SetResults();
}



void PlaneDlg::OnDefineWing()
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Wing);
//	WingDlg WingDlg;
	pMiarex->m_WingDlg.InitDialog(&m_pPlane->m_Wing);
	if(pMiarex->m_WingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();
		if(pMiarex->m_WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Wing.Duplicate(&SaveWing);
}


void PlaneDlg::OnDefineFin() 
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Fin);
//	WingDlg WingDlg;
	pMiarex->m_WingDlg.InitDialog(&m_pPlane->m_Fin);


	if(pMiarex->m_WingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();	
		if(pMiarex->m_WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Fin.Duplicate(&SaveWing);
}


void PlaneDlg::OnDefineStab() 
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Stab);
//	WingDlg WingDlg;
	pMiarex->m_WingDlg.InitDialog(&m_pPlane->m_Stab);
	if(pMiarex->m_WingDlg.exec() == QDialog::Accepted)
	{
		SetResults();	
		if(pMiarex->m_WingDlg.m_bChanged) m_bChanged = true;
	}
	else  m_pPlane->m_Stab.Duplicate(&SaveWing);
}



void PlaneDlg::OnDefineWing2()
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Wing2);
//	WingDlg WingDlg;
	pMiarex->m_WingDlg.InitDialog(&m_pPlane->m_Wing2);
	if(pMiarex->m_WingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();
		if(pMiarex->m_WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Wing2.Duplicate(&SaveWing);
}


void PlaneDlg::OnDoubleFin()
{
	if (m_pctrlDoubleFin->isChecked())
	{
		m_pctrlYLEFin->setEnabled(true);
		m_pPlane->m_bDoubleFin = true;
		m_pPlane->m_bSymFin    = false;
		m_pctrlSymFin->setChecked(false);
	}
	else
	{
			m_pctrlYLEFin->setEnabled(false);
		m_pPlane->m_bDoubleFin = false;
	}
	m_bChanged = true;
	SetResults();
}


void PlaneDlg::OnEditBody()
{
	if(!m_pPlane->m_pBody) return;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	CBody memBody;
	memBody.Duplicate(m_pPlane->m_pBody);
	pMiarex->m_GL3dBody.SetBody(m_pPlane->m_pBody);
	pMiarex->m_GL3dBody.m_bEnableName = false;

	if(pMiarex->m_GL3dBody.exec() == QDialog::Accepted)
	{
		pMiarex->m_bResetglBody = true;
		pMiarex->m_bResetglBodyMesh = true;
		pMiarex->m_bResetglGeom = true;
		pMiarex->m_bResetglMesh = true;
		pMainFrame->SetSaveState(false);
	}
	else m_pPlane->m_pBody->Duplicate(&memBody);
}


void PlaneDlg::OnExportWing() 
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_pPlane->m_Wing);
	pNewWing->m_WingName = "";
	pMiarex->AddWing(pNewWing);
	pMainFrame->UpdateUFOs();
	pMiarex->UpdateView();
}


void PlaneDlg::OnExportWing2()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_pPlane->m_Wing2);
	pNewWing->m_WingName = "";
	pMiarex->AddWing(pNewWing);
	pMainFrame->UpdateUFOs();
	pMiarex->UpdateView();
}


void PlaneDlg::OnImportWing()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ImportWingDlg dlg;
	dlg.m_WingName = m_pPlane->m_Wing.m_WingName;
	dlg.m_poaWing = s_poaWing;
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		CWing *pWing = pMainFrame->GetWing(dlg.m_WingName);
		if(pWing)
		{
			m_pPlane->m_Wing.Duplicate(pWing);
			m_pPlane->m_Wing.m_WingColor = pWing->m_WingColor;
		}
	}
}


void PlaneDlg::OnImportWing2() 
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ImportWingDlg dlg;
	dlg.m_WingName = m_pPlane->m_Wing.m_WingName;
	dlg.m_poaWing = s_poaWing;
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		CWing *pWing = pMainFrame->GetWing(dlg.m_WingName);
		if(pWing)
		{
			m_pPlane->m_Wing2.Duplicate(pWing);
			m_pPlane->m_Wing2.m_WingColor = pWing->m_WingColor;
		}
	}
}


void PlaneDlg::OnFin()
{
	m_bChanged = true;
	if(m_pctrlFinCheck->isChecked())
	{
		m_pctrlSymFin->setEnabled(true);
		m_pctrlDoubleFin->setEnabled(true);
		m_pctrlFinTilt->setEnabled(true);
		m_pctrlXLEFin->setEnabled(true);
		if (m_pctrlDoubleFin->isChecked()) m_pctrlYLEFin->setEnabled(true);
		else                               m_pctrlYLEFin->setEnabled(false);

		m_pctrlZLEFin->setEnabled(true);
		m_pctrlDefineFin->setEnabled(true);
		m_pPlane->m_bFin = true;
	}
	else
	{
		m_pctrlSymFin->setEnabled(false);
		m_pctrlDoubleFin->setEnabled(false);
		m_pctrlFinTilt->setEnabled(false);
		m_pctrlXLEFin->setEnabled(false);
		m_pctrlYLEFin->setEnabled(false);
		m_pctrlZLEFin->setEnabled(false);
		m_pctrlDefineFin->setEnabled(false);
		m_pPlane->m_bFin = false;
	}
	SetResults();
}



void PlaneDlg::OnPlaneName() 
{
	m_pPlane->m_PlaneName = m_pctrlPlaneName->text();
	m_pPlane->m_Wing.m_WingName = m_pPlane->m_PlaneName+"_Wing";
	m_pPlane->m_Stab.m_WingName = m_pPlane->m_PlaneName+"_Elev";
	m_pPlane->m_Fin.m_WingName = m_pPlane->m_PlaneName+"_Fin";
}


void PlaneDlg::OnOK()
{
	int j, n;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	QString strong;
	ReadParams();
	ComputePlane();

	n = m_pPlane->m_Wing.VLMGetPanelTotal();
	if(m_pPlane->m_bStab) n+= m_pPlane->m_Stab.VLMGetPanelTotal();
	if(m_pPlane->m_bFin)  n+= m_pPlane->m_Fin.VLMGetPanelTotal();
	if(n>VLMMATSIZE)
	{
		strong = QString("Total number of VLM panels =%1\n Max Number =%2\nA reduction of the number of VLM panels is required").arg(n).arg(VLMMATSIZE);
		QMessageBox::warning(this, "QFLR5",strong);
		return ;
	}

	//check the number of surfaces
	int nSurfaces = 0;
	for (j=0; j<m_pPlane->m_Wing.m_NPanel; j++)
	{
		if(fabs(m_pPlane->m_Wing.m_TPos[j]-m_pPlane->m_Wing.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
	}
	if(m_pPlane->m_bStab)
	{
		for (j=0; j<m_pPlane->m_Stab.m_NPanel; j++)
		{
			if(fabs(m_pPlane->m_Stab.m_TPos[j]-m_pPlane->m_Stab.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
		}
	}

	if(m_pPlane->m_bFin)
	{
		for (j=0; j<m_pPlane->m_Fin.m_NPanel; j++)
		{
			if(fabs(m_pPlane->m_Fin.m_TPos[j]-m_pPlane->m_Fin.m_TPos[j+1]) > pMiarex->m_MinPanelSize)
			{
				if((m_pPlane->m_bSymFin) || (m_pPlane->m_bDoubleFin && m_pPlane->m_bDoubleSymFin))
					nSurfaces += 2;
				else
					nSurfaces += 1;
			}
		}
	}
	if(nSurfaces >2*MAXPANELS)
	{
		strong = QString("Total number of wing panels =%1\n Max Number =%2\nA reduction of the number of wing panels is required")
			.arg(nSurfaces).arg(MAXPANELS);
		QMessageBox::warning(this, "QFLR5", strong);
		return ;
	}

	accept();
}



void PlaneDlg::OnStab()
{
	m_bChanged = true;
	if(m_pctrlStabCheck->isChecked())
	{
		m_pctrlDefineStab->setEnabled(true);
		m_pctrlXLEStab->setEnabled(true);
		m_pctrlZLEStab->setEnabled(true);
		m_pctrlStabTilt->setEnabled(true);
		m_pPlane->m_bStab = true;
	}
	else
	{
		m_pctrlDefineStab->setEnabled(false);
		m_pctrlXLEStab->setEnabled(false);
		m_pctrlZLEStab->setEnabled(false);
		m_pctrlStabTilt->setEnabled(false);
		m_pPlane->m_bStab = false;
	}
	SetResults();
}


void PlaneDlg::OnSymFin()
{	
	if (m_pctrlSymFin->isChecked()) 
	{
		m_pctrlYLEFin->setEnabled(false);
		m_pPlane->m_bSymFin    = true;
		m_pPlane->m_bDoubleFin = false;
		m_pctrlDoubleFin->setChecked(false);
		m_pctrlYLEFin->setEnabled(false);
	}
	else
	{
//		m_pctrlDoubleFin->setEnabled(true);
		m_pctrlYLEFin->setEnabled(true);
		m_pPlane->m_bSymFin = false;
	}
	m_bChanged = true;
	SetResults();
}
/*
BOOL PlaneDlg::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWnd = GetFocus();
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd) {
			m_pctrlOK.SetFocus();
//			ReadParams();
//			SetResults();
		}
		else if(GetDlgItem(IDOK) == pWnd ) OnOK();
		else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();

		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}*/

void PlaneDlg::OnSelChangeBodyList(int pos)
{
	QString strong;
	m_bChanged = true;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int sel = m_pctrlBodyList->currentIndex();
	if (sel >= 0) strong = m_pctrlBodyList->itemText(sel);

	m_pPlane->m_pBody = pMiarex->GetBody(strong);
}


void PlaneDlg::ReadParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex  = (QMiarex*)s_pMiarex;
	int sel;
	QString strong;

	OnPlaneName();
	m_pPlane->m_WingTilt      = m_pctrlWingTilt->GetValue();
	m_pPlane->m_WingTilt2     = m_pctrlWingTilt2->GetValue();
	m_pPlane->m_StabTilt      = m_pctrlStabTilt->GetValue();
	m_pPlane->m_FinTilt       = m_pctrlFinTilt->GetValue();

	m_pPlane->m_LEWing.x      = m_pctrlXLEWing->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_LEWing.z      = m_pctrlZLEWing->GetValue() / pMainFrame->m_mtoUnit;

	m_pPlane->m_LEStab.x      = m_pctrlXLEStab->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_LEStab.z      = m_pctrlZLEStab->GetValue() / pMainFrame->m_mtoUnit;

	m_pPlane->m_LEWing2.x     = m_pctrlXLEWing2->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_LEWing2.z     = m_pctrlZLEWing2->GetValue() / pMainFrame->m_mtoUnit;

	m_pPlane->m_LEFin.x       = m_pctrlXLEFin->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_LEFin.y       = m_pctrlYLEFin->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_LEFin.z       = m_pctrlZLEFin->GetValue() / pMainFrame->m_mtoUnit;

	m_pPlane->m_BodyPos.x     = m_pctrlXBody->GetValue() / pMainFrame->m_mtoUnit;
	m_pPlane->m_BodyPos.z     = m_pctrlZBody->GetValue() / pMainFrame->m_mtoUnit;

	if(m_pctrlStabCheck->isChecked()) m_pPlane->m_bStab = true;
	else                              m_pPlane->m_bStab = false;
	if(m_pctrlFinCheck->isChecked())  m_pPlane->m_bFin  = true;
	else                              m_pPlane->m_bFin  = false;

	sel = m_pctrlBodyList->currentIndex();
	if (sel >= 0) strong = m_pctrlBodyList->itemText(sel);
	m_pPlane->m_pBody = pMiarex->GetBody(strong);
}


void PlaneDlg::reject()
{
	OnCancel();
}


void PlaneDlg::SetParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	int i, pos;
	CBody *pBody = NULL;

	if(m_pPlane->m_bBody) m_pctrlBody->setChecked(true);
	else                  m_pctrlBody->setChecked(false);

	for(i=0; i<s_poaBody->size(); i++)
	{
		pBody = (CBody*)s_poaBody->at(i);
		m_pctrlBodyList->addItem(pBody->m_BodyName);
	}

	if(m_pPlane->m_bBody && pBody)
	{
		pos = m_pctrlBodyList->findText(m_pPlane->m_pBody->m_BodyName);
		m_pctrlBodyList->setCurrentIndex(pos);
	}
	else
	{
		if(m_pctrlBodyList->count()) m_pctrlBodyList->setCurrentIndex(0);
		else						 m_pctrlBody->setEnabled(false);
		m_pctrlBodyList->setEnabled(false);
		m_pctrlXBody->setEnabled(false);
		m_pctrlZBody->setEnabled(false);
		m_pctrlEditBody->setEnabled(false);
	}

	m_pctrlPlaneName->setText(m_pPlane->m_PlaneName);
	m_pctrlWingTilt->SetValue(m_pPlane->m_WingTilt);
	m_pctrlWingTilt2->SetValue(m_pPlane->m_WingTilt2);
	m_pctrlStabTilt->SetValue(m_pPlane->m_StabTilt);
	m_pctrlFinTilt->SetValue(m_pPlane->m_FinTilt);

	m_pctrlXLEWing->SetValue(m_pPlane->m_LEWing.x * pMainFrame->m_mtoUnit);
	m_pctrlZLEWing->SetValue(m_pPlane->m_LEWing.z * pMainFrame->m_mtoUnit);

	m_pctrlXLEStab->SetValue(m_pPlane->m_LEStab.x * pMainFrame->m_mtoUnit);
	m_pctrlZLEStab->SetValue(m_pPlane->m_LEStab.z * pMainFrame->m_mtoUnit);

	m_pctrlXLEWing2->SetValue(m_pPlane->m_LEWing2.x * pMainFrame->m_mtoUnit);
	m_pctrlZLEWing2->SetValue(m_pPlane->m_LEWing2.z * pMainFrame->m_mtoUnit);

	m_pctrlXBody->SetValue(m_pPlane->m_BodyPos.x * pMainFrame->m_mtoUnit);
	m_pctrlZBody->SetValue(m_pPlane->m_BodyPos.z * pMainFrame->m_mtoUnit);

	m_pctrlBiplane->setChecked(m_pPlane->m_bBiplane);
	OnBiplane();

	m_pctrlXLEFin->SetValue(m_pPlane->m_LEFin.x* pMainFrame->m_mtoUnit);
	m_pctrlYLEFin->SetValue(m_pPlane->m_LEFin.y* pMainFrame->m_mtoUnit);
	m_pctrlZLEFin->SetValue(m_pPlane->m_LEFin.z* pMainFrame->m_mtoUnit);
	m_pctrlFinCheck->setChecked(m_pPlane->m_bFin);
	m_pctrlDoubleFin->setChecked(m_pPlane->m_bDoubleFin);
	m_pctrlSymFin->setChecked(m_pPlane->m_bSymFin);
	OnFin();
	m_pctrlStabCheck->setChecked(m_pPlane->m_bStab);
	OnStab();
}


void PlaneDlg::SetResults()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

	double area = m_pPlane->m_Wing.m_Area;
	if(m_pPlane->m_bBiplane) area += m_pPlane->m_Wing2.m_Area;

	str = QString("%1").arg(area*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlWingSurface->setText(str);

	if(m_pPlane->m_bStab)   str = QString("%1").arg(m_pPlane->m_Stab.m_Area*pMainFrame->m_m2toUnit,7,'f',2);
	else str = " ";
	m_pctrlStabSurface->setText(str);

	if(m_pPlane->m_bFin)    str = QString("%1").arg(m_pPlane->m_Fin.m_Area*pMainFrame->m_m2toUnit,7,'f',2);
	else str=" ";
	m_pctrlFinSurface->setText(str);

	double span = m_pPlane->m_Wing.m_Span;
	if(m_pPlane->m_bBiplane) span = qMax(m_pPlane->m_Wing.m_Span, m_pPlane->m_Wing2.m_Span);
	str = QString("%1").arg(span*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlWingSpan->setText(str);

	ComputePlane();
	if(m_pPlane->m_bStab)
	{
		double SLA = m_pPlane->m_LEStab.x + m_pPlane->m_Stab.m_TChord[0]/4.0 - m_pPlane->m_Wing.m_TChord[0]/4.0;
		str = QString("%1").arg(SLA*pMainFrame->m_mtoUnit,5,'f',2);
	}
	else  str=" ";
	m_pctrlStabLeverArm->setText(str);

	if(m_pPlane->m_bStab)
	{
		str = QString("%1").arg(m_pPlane->m_TailVolume,5,'f',2);
	}
	else str =" ";
	m_pctrlStabVolume->setText(str);

	int total = m_pPlane->m_Wing.VLMGetPanelTotal();
	total += m_pPlane->m_Wing.m_NXPanels[m_pPlane->m_Wing.m_NPanel-1];
	if(m_pPlane->m_bBiplane)
	{
		total += m_pPlane->m_Wing2.VLMGetPanelTotal();
		total += m_pPlane->m_Wing2.m_NXPanels[m_pPlane->m_Wing2.m_NPanel-1];
	}

	if(m_pPlane->m_bStab)
	{
		total += m_pPlane->m_Stab.VLMGetPanelTotal();
		total += m_pPlane->m_Stab.m_NXPanels[m_pPlane->m_Stab.m_NPanel-1];
	}

	if(m_pPlane->m_bFin)
	{
		if(m_pPlane->m_bDoubleFin ||m_pPlane->m_bSymFin)  total += 2*m_pPlane->m_Fin.VLMGetPanelTotal();
		else                          total +=   m_pPlane->m_Fin.VLMGetPanelTotal();
		total += m_pPlane->m_Fin.m_NXPanels[m_pPlane->m_Fin.m_NPanel-1];
	}

	total *=2;//in case we have a panel analysis

	if(m_pPlane->m_bBody && m_pPlane->m_pBody) total += m_pPlane->m_pBody->m_nxPanels * m_pPlane->m_pBody->m_nhPanels * 2;

	str = QString("%1").arg(total);
	m_pctrlVLMTotalPanels->setText(str);

	str = QString("%1").arg(m_pPlane->m_Volume*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit,5,'e',2);
	m_pctrlVolume->setText(str);

}



void PlaneDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/2);
	move(r.width()/4, r.height()/8);

	QVBoxLayout *NameLayout = new QVBoxLayout;
	m_pctrlPlaneName = new QLineEdit("PlaneName");
	m_pctrlPlaneName->setMinimumWidth(120);
	NameLayout->addWidget(m_pctrlPlaneName);
	QGroupBox *NameBox = new QGroupBox("Plane Name");
	NameBox->setLayout(NameLayout);

	QGridLayout *MainWing = new QGridLayout;
	m_pctrlDefineWing = new QPushButton("Define");
	m_pctrlImportWing = new QPushButton("Import");
	m_pctrlExportWing = new QPushButton("Export");
	QLabel *lab1 = new QLabel("x=");
	QLabel *lab2 = new QLabel("z=");
	QLabel *lab3 = new QLabel("Tilt Angle=");
	lab1->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab2->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab3->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pctrlXLEWing = new FloatEdit(0.00);
	m_pctrlZLEWing = new FloatEdit(0.00);
	m_pctrlWingTilt = new FloatEdit(0.0);
	m_pctrlLen1 = new QLabel("mm");
	m_pctrlLen2 = new QLabel("mm");
	QLabel *lab4 = new QLabel("deg");
	MainWing->addWidget(m_pctrlDefineWing, 1,1);
	MainWing->addWidget(m_pctrlImportWing, 2,1);
	MainWing->addWidget(m_pctrlExportWing, 3,1);
	MainWing->addWidget(lab1,1,2);
	MainWing->addWidget(lab2,2,2);
	MainWing->addWidget(lab3,3,2);
	MainWing->addWidget(m_pctrlXLEWing,1,3);
	MainWing->addWidget(m_pctrlZLEWing,2,3);
	MainWing->addWidget(m_pctrlWingTilt,3,3);
	MainWing->addWidget(m_pctrlLen1,1,4);
	MainWing->addWidget(m_pctrlLen2,2,4);
	MainWing->addWidget(lab4,3,4);
	QGroupBox *MainWingBox = new QGroupBox("Main Wing");
	MainWingBox->setLayout(MainWing);

	QGridLayout *MainWing2 = new QGridLayout;
	m_pctrlBiplane = new QCheckBox("Biplane");
	MainWing2->addWidget(m_pctrlBiplane,1,1);
	m_pctrlDefineWing2 = new QPushButton("Define");
	m_pctrlImportWing2 = new QPushButton("Import");
	m_pctrlExportWing2 = new QPushButton("Export");
	QLabel *lab11 = new QLabel("x=");
	QLabel *lab12 = new QLabel("z=");
	QLabel *lab13 = new QLabel("Tilt Angle=");
	lab11->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab12->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab13->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pctrlXLEWing2 = new FloatEdit(0.00);
	m_pctrlZLEWing2 = new FloatEdit(0.00);
	m_pctrlWingTilt2 = new FloatEdit(0.0);
	m_pctrlLen3 = new QLabel("mm");
	m_pctrlLen4 = new QLabel("mm");
	QLabel *lab14 = new QLabel("deg");
	MainWing2->addWidget(m_pctrlDefineWing2, 2,1);
	MainWing2->addWidget(m_pctrlImportWing2, 3,1);
	MainWing2->addWidget(m_pctrlExportWing2, 4,1);
	MainWing2->addWidget(lab11,2,2);
	MainWing2->addWidget(lab12,3,2);
	MainWing2->addWidget(lab13,4,2);
	MainWing2->addWidget(m_pctrlXLEWing2,2,3);
	MainWing2->addWidget(m_pctrlZLEWing2,3,3);
	MainWing2->addWidget(m_pctrlWingTilt2,4,3);
	MainWing2->addWidget(m_pctrlLen3,2,4);
	MainWing2->addWidget(m_pctrlLen4,3,4);
	MainWing2->addWidget(lab14,4,4);
	QGroupBox *MainWing2Box = new QGroupBox("Wing 2");
	MainWing2Box->setLayout(MainWing2);

	QGridLayout *StabLayout = new QGridLayout;
	m_pctrlStabCheck = new QCheckBox("Elevator");
	m_pctrlDefineStab = new QPushButton("Define");
	QLabel *lab21 = new QLabel("x=");
	QLabel *lab22 = new QLabel("z=");
	QLabel *lab23 = new QLabel("Tilt Angle=");
	lab21->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab22->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab23->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pctrlXLEStab = new FloatEdit(550.00);
	m_pctrlZLEStab = new FloatEdit(550.00);
	m_pctrlStabTilt = new FloatEdit(0.0);
	m_pctrlLen5 = new QLabel("mm");
	m_pctrlLen6 = new QLabel("mm");
	QLabel *lab24 = new QLabel("deg");
	StabLayout->addWidget(m_pctrlStabCheck,1,1);
	StabLayout->addWidget(m_pctrlDefineStab, 2,1);
	StabLayout->addWidget(lab21,2,2);
	StabLayout->addWidget(lab22,4,2);
	StabLayout->addWidget(lab23,5,2);
	StabLayout->addWidget(m_pctrlXLEStab,2,3);
	StabLayout->addWidget(m_pctrlZLEStab,4,3);
	StabLayout->addWidget(m_pctrlStabTilt,5,3);
	StabLayout->addWidget(m_pctrlLen5,2,4);
	StabLayout->addWidget(m_pctrlLen6,4,4);
	StabLayout->addWidget(lab24,5,4);
	QGroupBox *StabBox = new QGroupBox("Elevator");
	StabBox->setLayout(StabLayout);

	QGridLayout *FinLayout = new QGridLayout;
	m_pctrlFinCheck = new QCheckBox("Fin");
	m_pctrlDefineFin = new QPushButton("Define");
	m_pctrlDoubleFin = new QCheckBox("Double Fin");
	m_pctrlSymFin = new QCheckBox("Symetric Fin");
	QLabel *lab31 = new QLabel("x=");
	QLabel *lab32 = new QLabel("y=");
	QLabel *lab33 = new QLabel("z=");
	QLabel *lab34 = new QLabel("Tilt Angle=");
	lab31->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab32->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab33->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab34->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pctrlXLEFin = new FloatEdit(600.00);
	m_pctrlYLEFin = new FloatEdit(0.00);
	m_pctrlZLEFin = new FloatEdit(50.00);
	m_pctrlFinTilt = new FloatEdit(0.0);
	m_pctrlLen5 = new QLabel("mm");
	m_pctrlLen6 = new QLabel("mm");
	m_pctrlLen7 = new QLabel("mm");
	QLabel *lab35 = new QLabel("deg");
	FinLayout->addWidget(m_pctrlFinCheck,1,1);
	FinLayout->addWidget(m_pctrlDefineFin, 2,1);
	FinLayout->addWidget(m_pctrlSymFin, 3,1);
	FinLayout->addWidget(m_pctrlDoubleFin, 4,1);
	FinLayout->addWidget(lab31,2,2);
	FinLayout->addWidget(lab32,3,2);
	FinLayout->addWidget(lab33,4,2);
	FinLayout->addWidget(lab34,5,2);
	FinLayout->addWidget(m_pctrlXLEFin,2,3);
	FinLayout->addWidget(m_pctrlYLEFin,3,3);
	FinLayout->addWidget(m_pctrlZLEFin,4,3);
	FinLayout->addWidget(m_pctrlFinTilt,5,3);
	FinLayout->addWidget(m_pctrlLen5,2,4);
	FinLayout->addWidget(m_pctrlLen6,3,4);
	FinLayout->addWidget(m_pctrlLen7,4,4);
	FinLayout->addWidget(lab35,5,4);
	QGroupBox *FinBox = new QGroupBox("Fin");
	FinBox->setLayout(FinLayout);

	QHBoxLayout *BodyName = new QHBoxLayout;
	m_pctrlBody = new QCheckBox("Body");
	m_pctrlEditBody = new QPushButton("Edit...");
	m_pctrlBodyList = new QComboBox;
	BodyName->addWidget(m_pctrlBody);
	BodyName->addWidget(m_pctrlBodyList);
	BodyName->addWidget(m_pctrlEditBody);
	m_pctrlBodyList->setMinimumWidth(250);
	BodyName->addStretch(1);
	QHBoxLayout *BodyPos = new QHBoxLayout;
	m_pctrlXBody = new FloatEdit(0.00);
	m_pctrlZBody = new FloatEdit(0.00);
	QLabel *lab41 = new QLabel("x=");
	QLabel *lab42 = new QLabel("z=");
	m_pctrlLen8 = new QLabel("mm");
	m_pctrlLen9 = new QLabel("mm");
	BodyPos->addStretch(1);
	BodyPos->addWidget(lab41);
	BodyPos->addWidget(m_pctrlXBody);
	BodyPos->addWidget(m_pctrlLen8);
	BodyPos->addStretch(1);
	BodyPos->addWidget(lab42);
	BodyPos->addWidget(m_pctrlZBody);
	BodyPos->addWidget(m_pctrlLen9);
	BodyPos->addStretch(1);
	QVBoxLayout *BodyLayout = new QVBoxLayout;
	BodyLayout->addLayout(BodyName);
	BodyLayout->addLayout(BodyPos);
	QGroupBox *BodyBox = new QGroupBox("Body");
	BodyBox->setLayout(BodyLayout);

	QLabel *lab101 = new QLabel("Wing Area = ");
	QLabel *lab102 = new QLabel("Wing Span = ");
	QLabel *lab103 = new QLabel("Elev. Area = ");
	QLabel *lab104 = new QLabel("Elev. Lever Arm = ");
	QLabel *lab105 = new QLabel("Fin Area = ");
	QLabel *lab106 = new QLabel("TailVolume = ");
	QLabel *lab107 = new QLabel("Plane Volume = ");
	QLabel *lab108 = new QLabel("Total Panels = ");
	lab101->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab102->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab103->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab104->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab105->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab106->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab107->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	lab108->setAlignment(Qt::AlignRight | Qt::AlignCenter);
	m_pctrlWingSurface    = new QLabel("1.00");
	m_pctrlStabSurface    = new QLabel("2.00");
	m_pctrlWingSpan       = new QLabel("3.00");
	m_pctrlStabLeverArm   = new QLabel("4.00");
	m_pctrlFinSurface     = new QLabel("5.00");
	m_pctrlStabVolume     = new QLabel("6.00");
	m_pctrlPlaneVolume    = new QLabel("7.00");
	m_pctrlVLMTotalPanels = new QLabel("1234");
	m_pctrlWingSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlWingSpan->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabLeverArm->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlFinSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabVolume->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlPlaneVolume->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlVLMTotalPanels->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pctrlSurf1 = new QLabel("dm2");
	m_pctrlSurf2 = new QLabel("dm2");
	m_pctrlSurf3 = new QLabel("dm2");
	m_pctrlLen10 = new QLabel("mm");
	m_pctrlLen11 = new QLabel("mm");
	m_pctrlVolume = new QLabel("mm3");
	QGridLayout *Data1Layout = new QGridLayout;
	Data1Layout->addWidget(lab101, 1, 1);
	Data1Layout->addWidget(m_pctrlWingSurface,1,2);
	Data1Layout->addWidget(m_pctrlSurf1, 1, 3);
	Data1Layout->addWidget(lab102, 2, 1);
	Data1Layout->addWidget(m_pctrlWingSpan, 2, 2);
	Data1Layout->addWidget(m_pctrlLen10, 2, 3);
	Data1Layout->addWidget(lab103, 3, 1);
	Data1Layout->addWidget(m_pctrlStabSurface, 3, 2);
	Data1Layout->addWidget(m_pctrlSurf2, 3, 3);
	Data1Layout->addWidget(lab104, 4, 1);
	Data1Layout->addWidget(m_pctrlStabLeverArm, 4, 2);
	Data1Layout->addWidget(m_pctrlLen11, 4, 3);
	QGridLayout *Data2Layout = new QGridLayout;
	Data2Layout->addWidget(lab105, 1, 1);
	Data2Layout->addWidget(m_pctrlFinSurface, 1, 2);
	Data2Layout->addWidget(m_pctrlSurf3, 1, 3);
	Data2Layout->addWidget(lab107, 2, 1);
	Data2Layout->addWidget(m_pctrlPlaneVolume, 2, 2);
	Data2Layout->addWidget(m_pctrlVolume, 2, 3);
	Data2Layout->addWidget(lab106, 3, 1);
	Data2Layout->addWidget(m_pctrlStabVolume, 3, 2);
	Data2Layout->addWidget(lab108, 4, 1);
	Data2Layout->addWidget(m_pctrlVLMTotalPanels, 4, 2);


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);


	QGridLayout *GeomLayout = new QGridLayout;
	GeomLayout->addWidget(NameBox,1,1);
	GeomLayout->addWidget(BodyBox,1,2);
	GeomLayout->addWidget(MainWingBox,2,1);
	GeomLayout->addWidget(MainWing2Box,2,2);
	GeomLayout->addWidget(StabBox,3,1);
	GeomLayout->addWidget(FinBox,3,2);
	GeomLayout->addLayout(Data1Layout,4,1);
	GeomLayout->addLayout(Data2Layout,4,2);


	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(GeomLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}



























































