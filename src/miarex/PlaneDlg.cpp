/****************************************************************************

	PlaneDlg Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

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

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <math.h>


#include "../globals.h"
#include "../mainframe.h" 
#include "Miarex.h"
#include "PlaneDlg.h"
#include "ImportWingDlg.h"
#include "InertiaDlg.h"


QList <void*> *PlaneDlg::s_poaWing;
QList <void*> *PlaneDlg::s_poaBody;
void *PlaneDlg::s_pMiarex;
void *PlaneDlg::s_pMainFrame;


PlaneDlg::PlaneDlg(QWidget *parent) :QDialog(parent)
{
	setWindowTitle(tr("Plane Editor"));
	m_pPlane = NULL;


	m_bAcceptName    = true;
	m_bChanged      = false;
	SetupLayout();

	connect(m_pctrlBiplane,   SIGNAL(clicked()), this, SLOT(OnBiplane()));
	connect(m_pctrlStabCheck, SIGNAL(clicked()), this, SLOT(OnStab()));
	connect(m_pctrlFinCheck,  SIGNAL(clicked()), this, SLOT(OnFin()));

	connect(m_pctrlSymFin,    SIGNAL(clicked()), this, SLOT(OnSymFin()));
	connect(m_pctrlDoubleFin, SIGNAL(clicked()), this, SLOT(OnDoubleFin()));

	connect(m_pctrlDefineWing,  SIGNAL(clicked()), this, SLOT(OnDefineWing()));
	connect(m_pctrlImportWing,  SIGNAL(clicked()), this, SLOT(OnImportWing()));
	connect(m_pctrlExportWing,  SIGNAL(clicked()), this, SLOT(OnExportWing()));
	connect(m_pctrlDefineWing2, SIGNAL(clicked()), this, SLOT(OnDefineWing2()));
	connect(m_pctrlImportWing2, SIGNAL(clicked()), this, SLOT(OnImportWing2()));
	connect(m_pctrlExportWing2, SIGNAL(clicked()), this, SLOT(OnExportWing2()));
	connect(m_pctrlDefineStab,  SIGNAL(clicked()), this, SLOT(OnDefineStab()));
	connect(m_pctrlDefineFin,   SIGNAL(clicked()), this, SLOT(OnDefineFin()));
	connect(m_pctrlEditBody,    SIGNAL(clicked()), this, SLOT(OnEditBody()));

	connect(m_pctrlPlaneInertia, SIGNAL(clicked()), this, SLOT(OnInertia()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlBody, SIGNAL(clicked()), this, SLOT(OnBodyCheck()));
	connect(m_pctrlBodyList,  SIGNAL(activated(int)),    this, SLOT(OnSelChangeBodyList(int)));

	connect(m_pctrlWingTilt, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlStabTilt, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlFinTilt,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXLEWing,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEWing,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXLEWing2, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEWing2, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXLEStab,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEStab,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXLEFin,   SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZLEFin,   SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXBody,    SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlZBody,    SIGNAL(editingFinished()), this, SLOT(OnChanged()));
}



void PlaneDlg::ComputePlane(void)
{
	if(m_pPlane->Stab())
	{
		double SLA = m_pPlane->m_WingLE[2].x + m_pPlane->Stab()->Chord(0)/4.0 - m_pPlane->m_WingLE[0].x -m_pPlane->Wing()->Chord(0)/4.0;
		double area = m_pPlane->Wing()->m_ProjectedArea;
		if(m_pPlane->Wing2()) area += m_pPlane->Wing2()->m_ProjectedArea;

		double ProjectedArea = 0.0;
		for (int i=0;i<m_pPlane->Stab()->NWingSection()-1; i++)
		{
			ProjectedArea += m_pPlane->Stab()->Length(i+1)*(m_pPlane->Stab()->Chord(i)+m_pPlane->Stab()->Chord(i+1))/2.0
							*cos(m_pPlane->Stab()->Dihedral(i)*PI/180.0)*cos(m_pPlane->Stab()->Dihedral(i)*PI/180.0);

		}
		ProjectedArea *=2.0;
		m_pPlane->m_TailVolume = ProjectedArea * SLA / area/m_pPlane->Wing()->m_MAChord ;
	}
	else m_pPlane->m_TailVolume = 0.0;

	if(m_pPlane->Fin())
	{
		m_pPlane->Fin()->m_bDoubleFin = m_pPlane->m_bDoubleFin;
		m_pPlane->Fin()->m_bSymFin    = m_pPlane->m_bSymFin;
	}
}


void PlaneDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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
	m_pctrlLen12->setText(len);
	m_pctrlLen13->setText(len);

	m_pctrlSurf1->setText(surf);
	m_pctrlSurf2->setText(surf);
	m_pctrlSurf3->setText(surf);

	m_pctrlPlaneName->setText(m_pPlane->PlaneName());

	if(m_pPlane->m_PlaneDescription.length()) m_pctrlPlaneDescription->setPlainText(m_pPlane->m_PlaneDescription);
	else                                      m_pctrlPlaneDescription->setPlainText("");

	SetParams();
	SetResults();

	if(!m_bAcceptName) m_pctrlPlaneName->setEnabled(false);
	m_bChanged = false;

	m_pPlane->m_bDoubleSymFin = true;
	m_pPlane->m_Wing.CreateSurfaces( m_pPlane->m_WingLE[0],   0.0, m_pPlane->m_WingTiltAngle[0]);//necessary for eventual inertia calculations
	m_pPlane->m_Wing2.CreateSurfaces(m_pPlane->m_WingLE[1],   0.0, m_pPlane->m_WingTiltAngle[1]);//necessary for eventual inertia calculations
	m_pPlane->m_Stab.CreateSurfaces( m_pPlane->m_WingLE[2],   0.0, m_pPlane->m_WingTiltAngle[2]);//necessary for eventual inertia calculations
	m_pPlane->m_Fin.CreateSurfaces(  m_pPlane->m_WingLE[3], -90.0, m_pPlane->m_WingTiltAngle[3]);//necessary for eventual inertia calculations
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
	m_bChanged = true;
	m_pPlane->m_bBiplane = m_pctrlBiplane->isChecked();
	if(m_pPlane->Wing2())
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



void PlaneDlg::OnChanged()
{
	m_bChanged = true;
	ReadParams();
	SetResults();
}



void PlaneDlg::OnDefineWing()
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	m_SaveWing.Duplicate(m_pPlane->Wing());

    pMiarex->m_pWingDlg->m_bAcceptName = false;
    pMiarex->m_pWingDlg->InitDialog(m_pPlane->Wing());

    if(pMiarex->m_pWingDlg->exec() ==QDialog::Accepted)
	{
		SetResults();
		m_bChanged = true;
	}
	else   m_pPlane->Wing()->Duplicate(&m_SaveWing);
	m_pPlane->Wing()->CreateSurfaces(m_pPlane->m_WingLE[0], 0.0, m_pPlane->m_WingTiltAngle[0]);//necessary for eventual inertia calculations
}


void PlaneDlg::OnDefineFin() 
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	m_SaveWing.Duplicate(m_pPlane->Fin());

    pMiarex->m_pWingDlg->m_bAcceptName = false;
    pMiarex->m_pWingDlg->InitDialog(m_pPlane->Fin());


    if(pMiarex->m_pWingDlg->exec() ==QDialog::Accepted)
	{
		SetResults();	
		m_bChanged = true;
	}
	else   m_pPlane->Fin()->Duplicate(&m_SaveWing);
	m_pPlane->Fin()->CreateSurfaces(m_pPlane->m_WingLE[3], -90.0, m_pPlane->m_WingTiltAngle[3]);//necessary for eventual inertia calculations
}


void PlaneDlg::OnDefineStab() 
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	m_SaveWing.Duplicate(m_pPlane->Stab());

    pMiarex->m_pWingDlg->m_bAcceptName = false;
    pMiarex->m_pWingDlg->InitDialog(m_pPlane->Stab());

    if(pMiarex->m_pWingDlg->exec() == QDialog::Accepted)
	{
		SetResults();	
		m_bChanged = true;
	}
	else  m_pPlane->Stab()->Duplicate(&m_SaveWing);
	m_pPlane->Stab()->CreateSurfaces(m_pPlane->m_WingLE[2], 0.0, m_pPlane->m_WingTiltAngle[2]);//necessary for eventual inertia calculations
}



void PlaneDlg::OnDefineWing2()
{
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	m_SaveWing.Duplicate(m_pPlane->Wing2());

    pMiarex->m_pWingDlg->m_bAcceptName = false;
    pMiarex->m_pWingDlg->InitDialog(m_pPlane->Wing2());

    if(pMiarex->m_pWingDlg->exec() ==QDialog::Accepted)
	{
		SetResults();
		m_bChanged = true;
	}
	else   m_pPlane->Wing2()->Duplicate(&m_SaveWing);
	m_pPlane->Wing2()->CreateSurfaces(m_pPlane->m_WingLE[1], 0.0, m_pPlane->m_WingTiltAngle[1]);//necessary for eventual inertia calculations
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
    pMiarex->m_pGL3dBody->m_bEnableName = false;
    pMiarex->m_pGL3dBody->InitDialog(m_pPlane->m_pBody);
    pMiarex->m_pGL3dBody->setWindowState(Qt::WindowMaximized);

    if(pMiarex->m_pGL3dBody->exec() == QDialog::Accepted)
	{
		pMiarex->m_bResetglBody = true;
		pMiarex->m_bResetglBodyMesh = true;
		pMiarex->m_bResetglGeom = true;
		pMiarex->m_bResetglMesh = true;
		pMainFrame->SetSaveState(false);
		m_bChanged = true;
	}
	else m_pPlane->m_pBody->Duplicate(&memBody);
}


void PlaneDlg::OnExportWing() 
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(m_pPlane->Wing());
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
	pNewWing->Duplicate(m_pPlane->Wing2());
	pNewWing->m_WingName = "";
	pMiarex->AddWing(pNewWing);
	pMainFrame->UpdateUFOs();
	pMiarex->UpdateView();
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


void PlaneDlg::OnImportWing()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
    ImportWingDlg dlg(this);
	dlg.m_WingName = m_pPlane->Wing()->m_WingName;
	dlg.m_poaWing = s_poaWing;
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		CWing *pWing = pMiarex->GetWing(dlg.m_WingName);
		if(pWing)
		{
			m_pPlane->Wing()->Duplicate(pWing);
			m_pPlane->Wing()->m_WingColor = pWing->m_WingColor;
		}
	}
}


void PlaneDlg::OnImportWing2() 
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
    ImportWingDlg dlg(this);
	dlg.m_WingName = m_pPlane->Wing()->m_WingName;
	dlg.m_poaWing = s_poaWing;
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		CWing *pWing = pMiarex->GetWing(dlg.m_WingName);
		if(pWing)
		{
			m_pPlane->Wing2()->Duplicate(pWing);
			m_pPlane->Wing2()->m_WingColor = pWing->m_WingColor;
		}
	}
}



void PlaneDlg::OnInertia()
{
	if(!m_pPlane) return;
	ReadParams();

	m_pPlane->CreateSurfaces();//necessary for inertia calculations

    InertiaDlg dlg(this);
	dlg.m_pBody = NULL;
	dlg.m_pWing = NULL;
	dlg.m_pPlane = m_pPlane;

	//save inertia properties
    QList<double> MassValue = m_pPlane->m_MassValue;
    QList<CVector> MassPosition = m_pPlane->m_MassPosition;
    QStringList MassTag = m_pPlane->m_MassTag;
	
	dlg.InitDialog();
	if(dlg.exec()==QDialog::Accepted)
	{
		if(dlg.m_bChanged) m_bChanged = true;
	}
	else
	{
		//restore everything
        m_pPlane->m_MassValue = MassValue;
        m_pPlane->m_MassPosition = MassPosition;
        m_pPlane->m_MassTag = MassTag;
	}
}


void PlaneDlg::OnPlaneName() 
{
	m_pPlane->m_PlaneName = m_pctrlPlaneName->text();
	m_pPlane->RenameWings();
}


void PlaneDlg::OnOK()
{
	int j, n;
//	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	QString strong;
	ReadParams();

	m_pPlane->m_PlaneDescription = m_pctrlPlaneDescription->toPlainText();

	ComputePlane();

	n = m_pPlane->Wing()->VLMGetPanelTotal();
	if(m_pPlane->Stab()) n+= m_pPlane->Stab()->VLMGetPanelTotal();
	if(m_pPlane->m_bFin)  n+= m_pPlane->Fin()->VLMGetPanelTotal();
	if(n>VLMMAXMATSIZE)
	{
		strong = QString(tr("Total number of VLM panels =%1\n Max Number =%2\nA reduction of the number of VLM panels is required")).arg(n).arg(VLMMAXMATSIZE);
		QMessageBox::warning(this, tr("Warning"),strong);
		return ;
	}

	//check the number of surfaces
	int nSurfaces = 0;
	for (j=0; j<m_pPlane->Wing()->NWingSection()-1; j++)
	{
		if(fabs(m_pPlane->Wing()->YPosition(j)-m_pPlane->Wing()->YPosition(j+1)) > QMiarex::s_MinPanelSize) nSurfaces+=2;
	}
	if(m_pPlane->Stab())
	{
		for (j=0; j<m_pPlane->Stab()->NWingSection()-1; j++)
		{
			if(fabs(m_pPlane->Stab()->YPosition(j)-m_pPlane->Stab()->YPosition(j+1)) > QMiarex::s_MinPanelSize) nSurfaces+=2;
		}
	}

	if(m_pPlane->Fin())
	{
		for (j=0; j<m_pPlane->Fin()->NWingSection()-1; j++)
		{
			if(fabs(m_pPlane->Fin()->YPosition(j)-m_pPlane->Fin()->YPosition(j+1)) > QMiarex::s_MinPanelSize)
			{
				if((m_pPlane->m_bSymFin) || (m_pPlane->m_bDoubleFin && m_pPlane->m_bDoubleSymFin))
					nSurfaces += 2;
				else
					nSurfaces += 1;
			}
		}
	}
	if(nSurfaces >2*MAXSPANSECTIONS)
	{
		strong = QString(tr("Total number of wing panels =%1\n Max Number =%2\nA reduction of the number of wing panels is required"))
			.arg(nSurfaces).arg(MAXSPANSECTIONS);
		QMessageBox::warning(this, tr("Warning"), strong);
		return ;
	}

	m_pPlane->ComputeBodyAxisInertia();
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


void PlaneDlg::OnSelChangeBodyList(int pos)
{
	QString strong;
	m_bChanged = true;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int sel = m_pctrlBodyList->currentIndex();
	if (sel >= 0) strong = m_pctrlBodyList->itemText(sel);
	m_pPlane->SetBody(pMiarex->GetBody(strong));
}


void PlaneDlg::ReadParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex  = (QMiarex*)s_pMiarex;
	int sel;
	QString strong;

	OnPlaneName();
	m_pPlane->m_WingTiltAngle[0] = m_pctrlWingTilt->Value();
	m_pPlane->m_WingTiltAngle[1] = m_pctrlWingTilt2->Value();
	m_pPlane->m_WingTiltAngle[2] = m_pctrlStabTilt->Value();
	m_pPlane->m_WingTiltAngle[3] = m_pctrlFinTilt->Value();

	m_pPlane->m_WingLE[0].x = m_pctrlXLEWing->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_WingLE[0].z = m_pctrlZLEWing->Value() / pMainFrame->m_mtoUnit;

	m_pPlane->m_WingLE[1].x = m_pctrlXLEWing2->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_WingLE[1].z = m_pctrlZLEWing2->Value() / pMainFrame->m_mtoUnit;

	m_pPlane->m_WingLE[2].x = m_pctrlXLEStab->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_WingLE[2].z = m_pctrlZLEStab->Value() / pMainFrame->m_mtoUnit;

	m_pPlane->m_WingLE[3].x = m_pctrlXLEFin->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_WingLE[3].y = m_pctrlYLEFin->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_WingLE[3].z = m_pctrlZLEFin->Value() / pMainFrame->m_mtoUnit;

	m_pPlane->m_BodyPos.x = m_pctrlXBody->Value() / pMainFrame->m_mtoUnit;
	m_pPlane->m_BodyPos.z = m_pctrlZBody->Value() / pMainFrame->m_mtoUnit;

	if(m_pctrlBiplane->isChecked())   m_pPlane->m_bBiplane = true;
	else                              m_pPlane->m_bBiplane = false;
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
	if(m_bChanged)
	{
		QString strong = tr("Save the changes ?");
		int Ans = QMessageBox::question(this, tr("Question"), strong,
										QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if (QMessageBox::Yes == Ans)
		{
			OnOK();
			return;
		}
		else if(QMessageBox::Cancel == Ans) return;
	}
//	reject();
	done(QDialog::Rejected);
}


void PlaneDlg::SetParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i, pos;
	CBody *pBody = NULL;
	if(m_pPlane->Body()) m_pctrlBody->setChecked(true);
	else                 m_pctrlBody->setChecked(false);
	m_pctrlBodyList->clear();
	for(i=0; i<s_poaBody->size(); i++)
	{
		pBody = (CBody*)s_poaBody->at(i);
		m_pctrlBodyList->addItem(pBody->m_BodyName);
	}
	if(m_pPlane->Body() && pBody)
	{
		pos = m_pctrlBodyList->findText(m_pPlane->m_pBody->m_BodyName);
		m_pctrlBodyList->setCurrentIndex(pos);
	}
	else
	{
		if(m_pctrlBodyList->count())
		{
			m_pctrlBodyList->setCurrentIndex(0);
			m_pctrlBody->setEnabled(true);
		}
		else m_pctrlBody->setEnabled(false);
		m_pctrlBodyList->setEnabled(false);
		m_pctrlXBody->setEnabled(false);
		m_pctrlZBody->setEnabled(false);
		m_pctrlEditBody->setEnabled(false);
	}

	m_pctrlPlaneName->setText(m_pPlane->PlaneName());
	m_pctrlWingTilt->SetValue(m_pPlane->m_WingTiltAngle[0]);
	m_pctrlWingTilt2->SetValue(m_pPlane->m_WingTiltAngle[1]);
	m_pctrlStabTilt->SetValue(m_pPlane->m_WingTiltAngle[2]);
	m_pctrlFinTilt->SetValue(m_pPlane->m_WingTiltAngle[3]);

	m_pctrlXLEWing->SetValue(m_pPlane->m_WingLE[0].x * pMainFrame->m_mtoUnit);
	m_pctrlZLEWing->SetValue(m_pPlane->m_WingLE[0].z * pMainFrame->m_mtoUnit);

	m_pctrlXLEWing2->SetValue(m_pPlane->m_WingLE[1].x * pMainFrame->m_mtoUnit);
	m_pctrlZLEWing2->SetValue(m_pPlane->m_WingLE[1].z * pMainFrame->m_mtoUnit);

	m_pctrlXLEStab->SetValue(m_pPlane->m_WingLE[2].x * pMainFrame->m_mtoUnit);
	m_pctrlZLEStab->SetValue(m_pPlane->m_WingLE[2].z * pMainFrame->m_mtoUnit);

	m_pctrlXBody->SetValue(m_pPlane->m_BodyPos.x * pMainFrame->m_mtoUnit);
	m_pctrlZBody->SetValue(m_pPlane->m_BodyPos.z * pMainFrame->m_mtoUnit);

	m_pctrlBiplane->setChecked(m_pPlane->Wing2());
	OnBiplane();

	m_pctrlXLEFin->SetValue(m_pPlane->m_WingLE[3].x* pMainFrame->m_mtoUnit);
	m_pctrlYLEFin->SetValue(m_pPlane->m_WingLE[3].y* pMainFrame->m_mtoUnit);
	m_pctrlZLEFin->SetValue(m_pPlane->m_WingLE[3].z* pMainFrame->m_mtoUnit);
	m_pctrlFinCheck->setChecked(m_pPlane->m_bFin);
	m_pctrlDoubleFin->setChecked(m_pPlane->m_bDoubleFin);
	m_pctrlSymFin->setChecked(m_pPlane->m_bSymFin);
	OnFin();
	m_pctrlStabCheck->setChecked(m_pPlane->Stab());
	OnStab();
}


void PlaneDlg::SetResults()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString str;

//	double area = m_pPlane->Wing()->s_Area;
//	if(m_pPlane->m_bBiplane) area += m_pPlane->Wing2()->m_Area;

	str = QString("%1").arg(m_pPlane->Wing()->m_PlanformArea*pMainFrame->m_m2toUnit,7,'f',2);
	m_pctrlWingSurface->setText(str);

	if(m_pPlane->Stab())   str = QString("%1").arg(m_pPlane->Stab()->m_PlanformArea*pMainFrame->m_m2toUnit,7,'f',2);
	else str = " ";
	m_pctrlStabSurface->setText(str);

	if(m_pPlane->Fin())    str = QString("%1").arg(m_pPlane->Fin()->m_PlanformArea*pMainFrame->m_m2toUnit,7,'f',2);
	else str=" ";
	m_pctrlFinSurface->setText(str);

	double span = m_pPlane->Wing()->m_PlanformSpan;
	if(m_pPlane->Wing2()) span = qMax(m_pPlane->Wing()->m_PlanformSpan, m_pPlane->Wing2()->m_PlanformSpan);
	str = QString("%1").arg(span*pMainFrame->m_mtoUnit,5,'f',2);
	m_pctrlWingSpan->setText(str);

	ComputePlane();
	if(m_pPlane->Stab())
	{
		double SLA = m_pPlane->m_WingLE[2].x + m_pPlane->Stab()->Chord(0)/4.0 - m_pPlane->Wing()->Chord(0)/4.0;
		str = QString("%1").arg(SLA*pMainFrame->m_mtoUnit,5,'f',2);
	}
	else  str=" ";
	m_pctrlStabLeverArm->setText(str);

	if(m_pPlane->Stab())
	{
		str = QString("%1").arg(m_pPlane->TailVolume(),5,'f',2);
	}
	else str =" ";
	m_pctrlStabVolume->setText(str);

	int total = m_pPlane->Wing()->VLMGetPanelTotal();
	total += m_pPlane->Wing()->NXPanels(m_pPlane->Wing()->NWingSection()-2);
	if(m_pPlane->Wing2())
	{
		total += m_pPlane->Wing2()->VLMGetPanelTotal();
		total += m_pPlane->Wing2()->NXPanels(m_pPlane->Wing2()->NWingSection()-2);
	}

	if(m_pPlane->Stab())
	{
		total += m_pPlane->Stab()->VLMGetPanelTotal();
		total += m_pPlane->Stab()->NXPanels(m_pPlane->Stab()->NWingSection()-2);
	}

	if(m_pPlane->Fin())
	{
		if(m_pPlane->m_bDoubleFin ||m_pPlane->m_bSymFin)  total += 2*m_pPlane->Fin()->VLMGetPanelTotal();
		else                                              total +=   m_pPlane->Fin()->VLMGetPanelTotal();
		total += m_pPlane->Fin()->NXPanels(m_pPlane->Fin()->NWingSection()-2);
	}

	total *=2;//in case we have a panel analysis

	if(m_pPlane->Body()) total += m_pPlane->Body()->m_nxPanels * m_pPlane->Body()->m_nhPanels * 2;

	str = QString("%1").arg(total);
	m_pctrlVLMTotalPanels->setText(str);
}



void PlaneDlg::SetupLayout()
{
	QGroupBox *NameBox = new QGroupBox(tr("Plane Description"));
	{
		QVBoxLayout *NameLayout = new QVBoxLayout;
		m_pctrlPlaneName = new QLineEdit(tr("Plane Name"));
	//	m_pctrlPlaneName->setMinimumWidth(120);
		m_pctrlPlaneDescription = new QTextEdit();
		m_pctrlPlaneDescription->setToolTip(tr("Enter here a short description for the plane"));
		QLabel *PlaneDescription = new QLabel(tr("Description:"));
		m_pctrlPlaneInertia = new QPushButton(tr("Plane Inertia"));
		NameLayout->addWidget(m_pctrlPlaneName);
		NameLayout->addWidget(PlaneDescription);
		NameLayout->addWidget(m_pctrlPlaneDescription);
		NameLayout->addWidget(m_pctrlPlaneInertia);
		NameBox->setLayout(NameLayout);
	}

	QGroupBox *MainWingBox = new QGroupBox(tr("Main Wing"));
	{
		QGridLayout *MainWing = new QGridLayout;
		m_pctrlDefineWing = new QPushButton(tr("Define"));
		m_pctrlImportWing = new QPushButton(tr("Import"));
		m_pctrlExportWing = new QPushButton(tr("Export"));
		QLabel *lab1 = new QLabel(tr("x="));
		QLabel *lab2 = new QLabel(tr("z="));
		QLabel *lab3 = new QLabel(tr("Tilt Angle="));
		lab1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlXLEWing = new FloatEdit(0.00);
		m_pctrlZLEWing = new FloatEdit(0.00);
		m_pctrlWingTilt = new FloatEdit(0.0,2);
		m_pctrlLen1 = new QLabel("mm");
		m_pctrlLen2 = new QLabel("mm");
		QLabel *lab4 = new QLabel(QString::fromUtf8("°"));
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
		MainWingBox->setLayout(MainWing);
	}

	QGroupBox *MainWing2Box = new QGroupBox(tr("Wing 2"));
	{
		QGridLayout *MainWing2 = new QGridLayout;
		m_pctrlBiplane = new QCheckBox(tr("Biplane"));
		MainWing2->addWidget(m_pctrlBiplane,1,1);
		m_pctrlDefineWing2 = new QPushButton(tr("Define"));
		m_pctrlImportWing2 = new QPushButton(tr("Import"));
		m_pctrlExportWing2 = new QPushButton(tr("Export"));
		QLabel *lab11 = new QLabel(tr("x="));
		QLabel *lab12 = new QLabel(tr("z="));
		QLabel *lab13 = new QLabel(tr("Tilt Angle="));
		lab11->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab12->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab13->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlXLEWing2 = new FloatEdit(0.00);
		m_pctrlZLEWing2 = new FloatEdit(0.00);
		m_pctrlWingTilt2 = new FloatEdit(0.0,2);
		m_pctrlLen3 = new QLabel("mm");
		m_pctrlLen4 = new QLabel("mm");
		QLabel *lab14 = new QLabel(QString::fromUtf8("°"));
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
		MainWing2Box->setLayout(MainWing2);
	}

	QGroupBox *StabBox = new QGroupBox(tr("Elevator"));
	{
		QGridLayout *StabLayout = new QGridLayout;
		m_pctrlStabCheck = new QCheckBox(tr("Elevator"));
		m_pctrlDefineStab = new QPushButton(tr("Define"));
		QLabel *lab21 = new QLabel(tr("x="));
		QLabel *lab22 = new QLabel(tr("z="));
		QLabel *lab23 = new QLabel(tr("Tilt Angle="));
		lab21->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab22->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab23->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlXLEStab = new FloatEdit(550.00);
		m_pctrlZLEStab = new FloatEdit(550.00);
		m_pctrlStabTilt = new FloatEdit(0.0,1);
		m_pctrlLen5 = new QLabel("mm");
		m_pctrlLen6 = new QLabel("mm");
		QLabel *lab24 = new QLabel(QString::fromUtf8("°"));
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
		StabBox->setLayout(StabLayout);
	}

	QGroupBox *FinBox = new QGroupBox(tr("Fin"));
	{
		QGridLayout *FinLayout = new QGridLayout;
		m_pctrlFinCheck = new QCheckBox(tr("Fin"));
		m_pctrlDefineFin = new QPushButton(tr("Define"));
		m_pctrlDoubleFin = new QCheckBox(tr("Double Fin"));
		m_pctrlSymFin = new QCheckBox(tr("Two-sided Fin"));
		QLabel *lab31 = new QLabel(tr("x="));
		QLabel *lab32 = new QLabel(tr("y="));
		QLabel *lab33 = new QLabel(tr("z="));
		QLabel *lab34 = new QLabel(tr("Tilt Angle="));
		lab31->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab32->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab33->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab34->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlXLEFin = new FloatEdit(600.00);
		m_pctrlYLEFin = new FloatEdit(0.00);
		m_pctrlZLEFin = new FloatEdit(50.00);
		m_pctrlFinTilt = new FloatEdit(0.0,2);
		m_pctrlLen7= new QLabel("mm");
		m_pctrlLen8 = new QLabel("mm");
		m_pctrlLen9 = new QLabel("mm");
		QLabel *lab35 = new QLabel(QString::fromUtf8("°"));
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
		FinLayout->addWidget(m_pctrlLen7,2,4);
		FinLayout->addWidget(m_pctrlLen8,3,4);
		FinLayout->addWidget(m_pctrlLen9,4,4);
		FinLayout->addWidget(lab35,5,4);
		FinBox->setLayout(FinLayout);
	}

	QGroupBox *BodyBox = new QGroupBox(tr("Body"));
	{
		QHBoxLayout *BodyName = new QHBoxLayout;
		QLabel *BodyWarning = new QLabel(tr("Warning:\nIncluding the body in the analysis is not recommended.\nCheck the guidelines for explanations."));
		m_pctrlBody = new QCheckBox(tr("Body"));
		m_pctrlEditBody = new QPushButton(tr("Edit..."));
		m_pctrlBodyList = new QComboBox;
		BodyName->addWidget(m_pctrlBody);
		BodyName->addWidget(m_pctrlBodyList);
		BodyName->addWidget(m_pctrlEditBody);
	//	m_pctrlBodyList->setMinimumWidth(250);
		BodyName->addStretch(1);
		QGridLayout *BodyPos = new QGridLayout;
		BodyPos->setColumnStretch(0,3);
		BodyPos->setColumnStretch(1,0);
		BodyPos->setColumnStretch(2,0);
		m_pctrlXBody = new FloatEdit(0.00);
		m_pctrlZBody = new FloatEdit(0.00);
		QLabel *lab41 = new QLabel(tr("x="));
		QLabel *lab42 = new QLabel(tr("z="));
		m_pctrlLen10 = new QLabel("mm");
		m_pctrlLen11 = new QLabel("mm");
		BodyPos->addWidget(lab41,1,1);
		BodyPos->addWidget(m_pctrlXBody,1,2);
		BodyPos->addWidget(m_pctrlLen10,1,3);
		BodyPos->addWidget(lab42,2,1);
		BodyPos->addWidget(m_pctrlZBody,2,2);
		BodyPos->addWidget(m_pctrlLen11,2,3);
		QVBoxLayout *BodyLayout = new QVBoxLayout;
		BodyLayout->addWidget(BodyWarning);
		BodyLayout->addLayout(BodyName);
		BodyLayout->addLayout(BodyPos);
		BodyBox->setLayout(BodyLayout);
	}

	QLabel *lab101 = new QLabel(tr("Wing Area = "));
	QLabel *lab102 = new QLabel(tr("Wing Span = "));
	QLabel *lab103 = new QLabel(tr("Elev. Area = "));
	QLabel *lab104 = new QLabel(tr("Elev. Lever Arm = "));
	QLabel *lab105 = new QLabel(tr("Fin Area = "));
	QLabel *lab106 = new QLabel(tr("TailVolume = "));
	QLabel *lab108 = new QLabel(tr("Total Panels = "));
	lab101->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab102->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab103->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab104->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab105->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab106->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab108->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlWingSurface    = new QLabel("1.00");
	m_pctrlStabSurface    = new QLabel("2.00");
	m_pctrlWingSpan       = new QLabel("3.00");
	m_pctrlStabLeverArm   = new QLabel("4.00");
	m_pctrlFinSurface     = new QLabel("5.00");
	m_pctrlStabVolume     = new QLabel("6.00");
	m_pctrlVLMTotalPanels = new QLabel("1234");
	m_pctrlWingSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlWingSpan->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabLeverArm->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlFinSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlStabVolume->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlVLMTotalPanels->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pctrlSurf1 = new QLabel("dm2");
	m_pctrlSurf2 = new QLabel("dm2");
	m_pctrlSurf3 = new QLabel("dm2");
	m_pctrlLen12 = new QLabel("mm");
	m_pctrlLen13 = new QLabel("mm");
	m_pctrlVolume = new QLabel("mm3");
	QGridLayout *Data1Layout = new QGridLayout;
	Data1Layout->addWidget(lab101, 1, 1);
	Data1Layout->addWidget(m_pctrlWingSurface,1,2);
	Data1Layout->addWidget(m_pctrlSurf1, 1, 3);
	Data1Layout->addWidget(lab102, 2, 1);
	Data1Layout->addWidget(m_pctrlWingSpan, 2, 2);
	Data1Layout->addWidget(m_pctrlLen12, 2, 3);
	Data1Layout->addWidget(lab103, 3, 1);
	Data1Layout->addWidget(m_pctrlStabSurface, 3, 2);
	Data1Layout->addWidget(m_pctrlSurf2, 3, 3);
	Data1Layout->addWidget(lab104, 4, 1);
	Data1Layout->addWidget(m_pctrlStabLeverArm, 4, 2);
	Data1Layout->addWidget(m_pctrlLen13, 4, 3);
	QGridLayout *Data2Layout = new QGridLayout;
	Data2Layout->addWidget(lab105, 1, 1);
	Data2Layout->addWidget(m_pctrlFinSurface, 1, 2);
	Data2Layout->addWidget(m_pctrlSurf3, 1, 3);
	Data2Layout->addWidget(lab106, 2, 1);
	Data2Layout->addWidget(m_pctrlStabVolume, 2, 2);
	Data2Layout->addWidget(lab108, 3, 1);
	Data2Layout->addWidget(m_pctrlVLMTotalPanels, 3, 2);


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





















