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

****************************************************************************/

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <math.h>


#include "../../globals.h"
#include "../../misc/Units.h"
#include "./../Objects3D.h"
#include "../mgt/ImportObjectDlg.h"
#include "PlaneDlg.h"
#include "GL3dWingDlg.h"
#include "GL3dBodyDlg.h"
#include "InertiaDlg.h"



PlaneDlg::PlaneDlg(QWidget *parent) :QDialog(parent)
{
	setWindowTitle(tr("Plane Editor"));
	m_pPlane = NULL;


	m_bAcceptName         = true;
	m_bChanged            = false;
	m_bDescriptionChanged = false;

	SetupLayout();

	connect(m_pctrlBiplane,   SIGNAL(clicked()), this, SLOT(OnBiplane()));
	connect(m_pctrlStabCheck, SIGNAL(clicked()), this, SLOT(OnStab()));
	connect(m_pctrlFinCheck,  SIGNAL(clicked()), this, SLOT(OnFin()));

	connect(m_pctrlSymFin,    SIGNAL(clicked()), this, SLOT(OnSymFin()));
	connect(m_pctrlDoubleFin, SIGNAL(clicked()), this, SLOT(OnDoubleFin()));

	connect(m_pctrlDefineWing,  SIGNAL(clicked()), this, SLOT(OnDefineWing()));
	connect(m_pctrlImportWing,  SIGNAL(clicked()), this, SLOT(OnImportWing()));
	connect(m_pctrlDefineWing2, SIGNAL(clicked()), this, SLOT(OnDefineWing2()));
	connect(m_pctrlImportWing2, SIGNAL(clicked()), this, SLOT(OnImportWing2()));
	connect(m_pctrlDefineStab,  SIGNAL(clicked()), this, SLOT(OnDefineStab()));
	connect(m_pctrlDefineFin,   SIGNAL(clicked()), this, SLOT(OnDefineFin()));

	connect(m_pctrlPlaneInertia, SIGNAL(clicked()), this, SLOT(OnInertia()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlBody,       SIGNAL(clicked()), this, SLOT(OnBodyCheck()));
	connect(m_pctrlDefineBody, SIGNAL(clicked()), this, SLOT(OnDefineBody()));
	connect(m_pctrlImportBody, SIGNAL(clicked()), this, SLOT(OnImportBody()));

	connect(m_pctrlPlaneDescription, SIGNAL(textChanged()), this, SLOT(OnDescriptionChanged()));

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
	if(m_pPlane->stab())
	{
		double SLA = m_pPlane->m_WingLE[2].x + m_pPlane->stab()->Chord(0)/4.0 - m_pPlane->m_WingLE[0].x -m_pPlane->wing()->Chord(0)/4.0;
		double area = m_pPlane->wing()->m_ProjectedArea;
		if(m_pPlane->wing2()) area += m_pPlane->wing2()->m_ProjectedArea;

		double ProjectedArea = 0.0;
		for (int i=0;i<m_pPlane->stab()->NWingSection()-1; i++)
		{
			ProjectedArea += m_pPlane->stab()->Length(i+1)*(m_pPlane->stab()->Chord(i)+m_pPlane->stab()->Chord(i+1))/2.0
							*cos(m_pPlane->stab()->Dihedral(i)*PI/180.0)*cos(m_pPlane->stab()->Dihedral(i)*PI/180.0);

		}
		ProjectedArea *=2.0;
		m_pPlane->m_TailVolume = ProjectedArea * SLA / area/m_pPlane->wing()->m_MAChord ;
	}
	else m_pPlane->m_TailVolume = 0.0;

	if(m_pPlane->fin())
	{
		m_pPlane->fin()->m_bDoubleFin = m_pPlane->m_bDoubleFin;
		m_pPlane->fin()->m_bSymFin    = m_pPlane->m_bSymFin;
	}
}


void PlaneDlg::InitDialog()
{
	QString len, surf;
	Units::getLengthUnitLabel(len);
	Units::getAreaUnitLabel(surf);

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

	m_pctrlPlaneName->setText(m_pPlane->planeName());

	if(m_pPlane->m_PlaneDescription.length()) m_pctrlPlaneDescription->setPlainText(m_pPlane->m_PlaneDescription);
	else                                      m_pctrlPlaneDescription->setPlainText("");

	SetParams();
	SetResults();

	if(!m_bAcceptName) m_pctrlPlaneName->setEnabled(false);
	m_bChanged = false;

	m_pPlane->m_bDoubleSymFin = true;
	m_pPlane->m_Wing[0].CreateSurfaces(m_pPlane->m_WingLE[0],   0.0, m_pPlane->m_WingTiltAngle[0]);//necessary for eventual inertia calculations
	m_pPlane->m_Wing[1].CreateSurfaces(m_pPlane->m_WingLE[1],   0.0, m_pPlane->m_WingTiltAngle[1]);//necessary for eventual inertia calculations
	m_pPlane->m_Wing[2].CreateSurfaces(m_pPlane->m_WingLE[2],   0.0, m_pPlane->m_WingTiltAngle[2]);//necessary for eventual inertia calculations
	m_pPlane->m_Wing[3].CreateSurfaces(m_pPlane->m_WingLE[3], -90.0, m_pPlane->m_WingTiltAngle[3]);//necessary for eventual inertia calculations
}


void PlaneDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
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
	if(m_pPlane->wing2())
	{
		m_pctrlDefineWing2->setEnabled(true);
		m_pctrlImportWing2->setEnabled(true);
		m_pctrlWingTilt2->setEnabled(true);
		m_pctrlZLEWing2->setEnabled(true);
		m_pctrlXLEWing2->setEnabled(true);
	}
	else
	{
		m_pctrlDefineWing2->setEnabled(false);
		m_pctrlImportWing2->setEnabled(false);
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
		m_pPlane->m_bBody= true;
		if(!m_pPlane->body())
		{
			m_pPlane->setBody(new Body);
		}
	}
	else
	{
		m_pPlane->m_bBody= false;
	}

	m_pctrlXBody->setEnabled(m_pctrlBody->isChecked());
	m_pctrlZBody->setEnabled(m_pctrlBody->isChecked());
	m_pctrlDefineBody->setEnabled(m_pctrlBody->isChecked());
	m_pctrlImportBody->setEnabled(m_pctrlBody->isChecked());

	SetResults();
}



void PlaneDlg::OnChanged()
{
	m_bChanged = true;
	ReadParams();
	SetResults();
}


void PlaneDlg::OnDescriptionChanged()
{
	m_bDescriptionChanged = true;
}



void PlaneDlg::OnDefineWing()
{
	Wing *pSaveWing = new Wing();
	pSaveWing->Duplicate(m_pPlane->wing());

	GL3dWingDlg wingDlg(this);

	wingDlg.m_bAcceptName = false;
	wingDlg.InitDialog(m_pPlane->wing());

	if(wingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();
		m_bChanged = true;
	}
	else   m_pPlane->wing()->Duplicate(pSaveWing);
	m_pPlane->wing()->CreateSurfaces(m_pPlane->m_WingLE[0], 0.0, m_pPlane->m_WingTiltAngle[0]);//necessary for eventual inertia calculations

	delete pSaveWing;
}


void PlaneDlg::OnDefineFin() 
{
	Wing *pSaveWing = new Wing();
	pSaveWing->Duplicate(m_pPlane->fin());

	GL3dWingDlg wingDlg(this);
	wingDlg.m_bAcceptName = false;
	wingDlg.InitDialog(m_pPlane->fin());


	if(wingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();	
		m_bChanged = true;
	}
	else   m_pPlane->fin()->Duplicate(pSaveWing);
	m_pPlane->fin()->CreateSurfaces(m_pPlane->m_WingLE[3], -90.0, m_pPlane->m_WingTiltAngle[3]);//necessary for eventual inertia calculations

	delete pSaveWing;
}


void PlaneDlg::OnDefineStab() 
{
	Wing *pSaveWing = new Wing();
	pSaveWing->Duplicate(m_pPlane->stab());

	GL3dWingDlg wingDlg(this);
	wingDlg.m_bAcceptName = false;
	wingDlg.InitDialog(m_pPlane->stab());

	if(wingDlg.exec() == QDialog::Accepted)
	{
		SetResults();	
		m_bChanged = true;
	}
	else  m_pPlane->stab()->Duplicate(pSaveWing);
	m_pPlane->stab()->CreateSurfaces(m_pPlane->m_WingLE[2], 0.0, m_pPlane->m_WingTiltAngle[2]);//necessary for eventual inertia calculations

	delete pSaveWing;
}



void PlaneDlg::OnDefineWing2()
{
	Wing *pSaveWing = new Wing();
	pSaveWing->Duplicate(m_pPlane->wing2());

	GL3dWingDlg wingDlg(this);
	wingDlg.m_bAcceptName = false;
	wingDlg.InitDialog(m_pPlane->wing2());

	if(wingDlg.exec() ==QDialog::Accepted)
	{
		SetResults();
		m_bChanged = true;
	}
	else   m_pPlane->wing2()->Duplicate(pSaveWing);
	m_pPlane->wing2()->CreateSurfaces(m_pPlane->m_WingLE[1], 0.0, m_pPlane->m_WingTiltAngle[1]);//necessary for eventual inertia calculations

	delete pSaveWing;
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


void PlaneDlg::OnDefineBody()
{
	if(!m_pPlane->m_pBody) return;

	Body memBody;
	memBody.Duplicate(m_pPlane->m_pBody);

	GL3dBodyDlg glbDlg(this);
	glbDlg.m_bEnableName = false;
	glbDlg.InitDialog(m_pPlane->m_pBody);
	glbDlg.setWindowState(Qt::WindowMaximized);

	if(glbDlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		SetResults();
	}
	else m_pPlane->m_pBody->Duplicate(&memBody);
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


void PlaneDlg::OnImportBody()
{
	ImportObjectDlg dlg(this);
	if(m_pPlane->m_bBody && m_pPlane->m_pBody) dlg.m_ObjectName = m_pPlane->body()->m_BodyName;
	else                                       dlg.m_ObjectName.clear();
	dlg.InitDialog(false);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		Body *pOldBody = Objects3D::getBody(dlg.m_ObjectName);
		if(pOldBody)
		{
			Body *pNewBody = new Body;
			pNewBody->Duplicate(pOldBody);
            m_pPlane->setBody(pNewBody);
		}
	}
}


void PlaneDlg::OnImportWing()
{
	ImportObjectDlg dlg(this);
	dlg.m_ObjectName = m_pPlane->planeName();
	dlg.InitDialog(true);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		dlg.m_ObjectName.replace("/Main wing","");
		Wing *pWing = Objects3D::getWing(dlg.m_ObjectName);
		if(pWing)
		{
			m_pPlane->wing()->Duplicate(pWing);
			m_pPlane->wing()->setWingColor(pWing->wingColor());
		}
	}
}


void PlaneDlg::OnImportWing2() 
{
	ImportObjectDlg dlg(this);
	dlg.m_ObjectName = m_pPlane->wing()->m_WingName;
	dlg.InitDialog(true);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bChanged = true;
		Wing *pWing = Objects3D::getWing(dlg.m_ObjectName);
		if(pWing)
		{
			m_pPlane->wing2()->Duplicate(pWing);
			m_pPlane->wing2()->setWingColor(pWing->wingColor());
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
	QList<PointMass*> memPtMass;
	memPtMass.clear();
	for(int im=0; im<m_pPlane->m_PointMass.size(); im++)
	{
		memPtMass.append(new PointMass(m_pPlane->m_PointMass[im]));
	}
	
	dlg.InitDialog();
	if(dlg.exec()==QDialog::Accepted)
	{
		if(dlg.m_bChanged) m_bChanged = true;
	}
	else
	{
		//restore everything
		m_pPlane->ClearPointMasses();
		for(int im=0; im<memPtMass.size(); im++)
		{
			m_pPlane->m_PointMass.append(new PointMass(memPtMass[im]));
		}
	}
}


void PlaneDlg::OnPlaneName() 
{
	m_pPlane->setPlaneName(m_pctrlPlaneName->text());
}


void PlaneDlg::OnOK()
{
	int j;

	ReadParams();

	m_pPlane->m_PlaneDescription = m_pctrlPlaneDescription->toPlainText();

	ComputePlane();


	//check the number of surfaces
	int nSurfaces = 0;
	for (j=0; j<m_pPlane->wing()->NWingSection()-1; j++)
	{
		if(qAbs(m_pPlane->wing()->YPosition(j)-m_pPlane->wing()->YPosition(j+1)) > Wing::s_MinPanelSize) nSurfaces+=2;
	}
	if(m_pPlane->stab())
	{
		for (j=0; j<m_pPlane->stab()->NWingSection()-1; j++)
		{
			if(qAbs(m_pPlane->stab()->YPosition(j)-m_pPlane->stab()->YPosition(j+1)) > Wing::s_MinPanelSize) nSurfaces+=2;
		}
	}

	if(m_pPlane->fin())
	{
		for (j=0; j<m_pPlane->fin()->NWingSection()-1; j++)
		{
			if(qAbs(m_pPlane->fin()->YPosition(j)-m_pPlane->fin()->YPosition(j+1)) > Wing::s_MinPanelSize)
			{
				if((m_pPlane->m_bSymFin) || (m_pPlane->m_bDoubleFin && m_pPlane->m_bDoubleSymFin))
					nSurfaces += 2;
				else
					nSurfaces += 1;
			}
		}
	}

	m_pPlane->ComputeBodyAxisInertia();

	if(!m_pctrlBody->isChecked())
	{
		delete m_pPlane->m_pBody;
		m_pPlane->m_pBody = NULL;
		m_pPlane->m_bBody = false;
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




void PlaneDlg::ReadParams()
{
	OnPlaneName();
	m_pPlane->m_WingTiltAngle[0] = m_pctrlWingTilt->Value();
	m_pPlane->m_WingTiltAngle[1] = m_pctrlWingTilt2->Value();
	m_pPlane->m_WingTiltAngle[2] = m_pctrlStabTilt->Value();
	m_pPlane->m_WingTiltAngle[3] = m_pctrlFinTilt->Value();

	m_pPlane->m_WingLE[0].x = m_pctrlXLEWing->Value() / Units::mtoUnit();
	m_pPlane->m_WingLE[0].z = m_pctrlZLEWing->Value() / Units::mtoUnit();

	m_pPlane->m_WingLE[1].x = m_pctrlXLEWing2->Value() / Units::mtoUnit();
	m_pPlane->m_WingLE[1].z = m_pctrlZLEWing2->Value() / Units::mtoUnit();

	m_pPlane->m_WingLE[2].x = m_pctrlXLEStab->Value() / Units::mtoUnit();
	m_pPlane->m_WingLE[2].z = m_pctrlZLEStab->Value() / Units::mtoUnit();

	m_pPlane->m_WingLE[3].x = m_pctrlXLEFin->Value() / Units::mtoUnit();
	m_pPlane->m_WingLE[3].y = m_pctrlYLEFin->Value() / Units::mtoUnit();
	m_pPlane->m_WingLE[3].z = m_pctrlZLEFin->Value() / Units::mtoUnit();

	m_pPlane->m_BodyPos.x = m_pctrlXBody->Value() / Units::mtoUnit();
	m_pPlane->m_BodyPos.z = m_pctrlZBody->Value() / Units::mtoUnit();

	if(m_pctrlBiplane->isChecked())   m_pPlane->m_bBiplane = true;
	else                              m_pPlane->m_bBiplane = false;
	if(m_pctrlStabCheck->isChecked()) m_pPlane->m_bStab = true;
	else                              m_pPlane->m_bStab = false;
	if(m_pctrlFinCheck->isChecked())  m_pPlane->m_bFin  = true;
	else                              m_pPlane->m_bFin  = false;

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
	if(m_pPlane->body()) m_pctrlBody->setChecked(true);
	else                 m_pctrlBody->setChecked(false);

	m_pctrlBody->setEnabled(true);
	m_pctrlXBody->setEnabled(m_pPlane->m_bBody);
	m_pctrlZBody->setEnabled(m_pPlane->m_bBody);
	m_pctrlDefineBody->setEnabled(m_pPlane->m_bBody);
	m_pctrlImportBody->setEnabled(m_pPlane->m_bBody);

	m_pctrlPlaneName->setText(m_pPlane->planeName());
	m_pctrlWingTilt->SetValue(m_pPlane->m_WingTiltAngle[0]);
	m_pctrlWingTilt2->SetValue(m_pPlane->m_WingTiltAngle[1]);
	m_pctrlStabTilt->SetValue(m_pPlane->m_WingTiltAngle[2]);
	m_pctrlFinTilt->SetValue(m_pPlane->m_WingTiltAngle[3]);

	m_pctrlXLEWing->SetValue(m_pPlane->m_WingLE[0].x * Units::mtoUnit());
	m_pctrlZLEWing->SetValue(m_pPlane->m_WingLE[0].z * Units::mtoUnit());

	m_pctrlXLEWing2->SetValue(m_pPlane->m_WingLE[1].x * Units::mtoUnit());
	m_pctrlZLEWing2->SetValue(m_pPlane->m_WingLE[1].z * Units::mtoUnit());

	m_pctrlXLEStab->SetValue(m_pPlane->m_WingLE[2].x * Units::mtoUnit());
	m_pctrlZLEStab->SetValue(m_pPlane->m_WingLE[2].z * Units::mtoUnit());

	m_pctrlXBody->SetValue(m_pPlane->m_BodyPos.x * Units::mtoUnit());
	m_pctrlZBody->SetValue(m_pPlane->m_BodyPos.z * Units::mtoUnit());

	m_pctrlBiplane->setChecked(m_pPlane->wing2());
	OnBiplane();

	m_pctrlXLEFin->SetValue(m_pPlane->m_WingLE[3].x* Units::mtoUnit());
	m_pctrlYLEFin->SetValue(m_pPlane->m_WingLE[3].y* Units::mtoUnit());
	m_pctrlZLEFin->SetValue(m_pPlane->m_WingLE[3].z* Units::mtoUnit());
	m_pctrlFinCheck->setChecked(m_pPlane->m_bFin);
	m_pctrlDoubleFin->setChecked(m_pPlane->m_bDoubleFin);
	m_pctrlSymFin->setChecked(m_pPlane->m_bSymFin);
	OnFin();
	m_pctrlStabCheck->setChecked(m_pPlane->stab());
	OnStab();
}


void PlaneDlg::SetResults()
{
	QString str;

//	double area = m_pPlane->Wing()->s_Area;
//	if(m_pPlane->m_bBiplane) area += m_pPlane->Wing2()->m_Area;

	str = QString("%1").arg(m_pPlane->wing()->m_PlanformArea*Units::m2toUnit(),7,'f',2);
	m_pctrlWingSurface->setText(str);

	if(m_pPlane->stab())   str = QString("%1").arg(m_pPlane->stab()->m_PlanformArea*Units::m2toUnit(),7,'f',2);
	else str = " ";
	m_pctrlStabSurface->setText(str);

	if(m_pPlane->fin())    str = QString("%1").arg(m_pPlane->fin()->m_PlanformArea*Units::m2toUnit(),7,'f',2);
	else str=" ";
	m_pctrlFinSurface->setText(str);

	double span = m_pPlane->wing()->m_PlanformSpan;
	if(m_pPlane->wing2()) span = qMax(m_pPlane->wing()->m_PlanformSpan, m_pPlane->wing2()->m_PlanformSpan);
	str = QString("%1").arg(span*Units::mtoUnit(),5,'f',2);
	m_pctrlWingSpan->setText(str);

	ComputePlane();
	if(m_pPlane->stab())
	{
		double SLA = m_pPlane->m_WingLE[2].x + m_pPlane->stab()->Chord(0)/4.0 - m_pPlane->wing()->Chord(0)/4.0;
		str = QString("%1").arg(SLA*Units::mtoUnit(),5,'f',2);
	}
	else  str=" ";
	m_pctrlStabLeverArm->setText(str);

	if(m_pPlane->stab())
	{
		str = QString("%1").arg(m_pPlane->TailVolume(),5,'f',2);
	}
	else str =" ";
	m_pctrlStabVolume->setText(str);


	str = QString("%1").arg(m_pPlane->VLMPanelTotal());
	m_pctrlVLMTotalPanels->setText(str);
}



void PlaneDlg::SetupLayout()
{
	QGroupBox *pNameBox = new QGroupBox(tr("Plane Description"));
	{
		QVBoxLayout *pNameLayout = new QVBoxLayout;
		{
			m_pctrlPlaneName = new QLineEdit(tr("Plane Name"));
			m_pctrlPlaneDescription = new QTextEdit();
			m_pctrlPlaneDescription->setToolTip(tr("Enter here a short description for the plane"));
			QLabel *PlaneDescription = new QLabel(tr("Description:"));
			m_pctrlPlaneInertia = new QPushButton(tr("Plane Inertia"));
			pNameLayout->addWidget(m_pctrlPlaneName);
			pNameLayout->addWidget(PlaneDescription);
			pNameLayout->addWidget(m_pctrlPlaneDescription);
			pNameLayout->addWidget(m_pctrlPlaneInertia);
		}
		pNameBox->setLayout(pNameLayout);
	}

	QGroupBox *pMainWingBox = new QGroupBox(tr("Main Wing"));
	{
		QGridLayout *pMainWingLayout = new QGridLayout;
		{
			QCheckBox *pMainWing = new QCheckBox(tr("Main wing"));
			pMainWing->setChecked(true);
			pMainWing->setEnabled(false);
			pMainWingLayout->addWidget(pMainWing,1,1);
			m_pctrlDefineWing = new QPushButton(tr("Define"));
			m_pctrlImportWing = new QPushButton(tr("Import"));
			QLabel *lab1 = new QLabel(tr("x="));
			QLabel *lab2 = new QLabel(tr("z="));
			QLabel *lab3 = new QLabel(tr("Tilt Angle="));
			lab1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlXLEWing = new DoubleEdit(0.00);
			m_pctrlZLEWing = new DoubleEdit(0.00);
			m_pctrlWingTilt = new DoubleEdit(0.0,2);
			m_pctrlLen1 = new QLabel("mm");
			m_pctrlLen2 = new QLabel("mm");
			QLabel *lab4 = new QLabel(QString::fromUtf8("°"));
			pMainWingLayout->addWidget(m_pctrlDefineWing, 2,1);
			pMainWingLayout->addWidget(m_pctrlImportWing, 3,1);
			pMainWingLayout->addWidget(lab1,2,2);
			pMainWingLayout->addWidget(lab2,3,2);
			pMainWingLayout->addWidget(lab3,4,2);
			pMainWingLayout->addWidget(m_pctrlXLEWing,2,3);
			pMainWingLayout->addWidget(m_pctrlZLEWing,3,3);
			pMainWingLayout->addWidget(m_pctrlWingTilt,4,3);
			pMainWingLayout->addWidget(m_pctrlLen1,2,4);
			pMainWingLayout->addWidget(m_pctrlLen2,3,4);
			pMainWingLayout->addWidget(lab4,4,4);
			pMainWingLayout->setRowStretch(5,1);
		}
		pMainWingBox->setLayout(pMainWingLayout);
	}

	QGroupBox *pMainWing2Box = new QGroupBox(tr("Wing 2"));
	{
		QGridLayout *pMainWing2Layout = new QGridLayout;
		{
			m_pctrlBiplane = new QCheckBox(tr("Biplane"));
			pMainWing2Layout->addWidget(m_pctrlBiplane,1,1);
			m_pctrlDefineWing2 = new QPushButton(tr("Define"));
			m_pctrlImportWing2 = new QPushButton(tr("Import"));
			QLabel *lab11 = new QLabel(tr("x="));
			QLabel *lab12 = new QLabel(tr("z="));
			QLabel *lab13 = new QLabel(tr("Tilt Angle="));
			lab11->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab12->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab13->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlXLEWing2 = new DoubleEdit(0.00);
			m_pctrlZLEWing2 = new DoubleEdit(0.00);
			m_pctrlWingTilt2 = new DoubleEdit(0.0,2);
			m_pctrlLen3 = new QLabel("mm");
			m_pctrlLen4 = new QLabel("mm");
			QLabel *lab14 = new QLabel(QString::fromUtf8("°"));
			pMainWing2Layout->addWidget(m_pctrlDefineWing2, 2,1);
			pMainWing2Layout->addWidget(m_pctrlImportWing2, 3,1);
			pMainWing2Layout->addWidget(lab11,2,2);
			pMainWing2Layout->addWidget(lab12,3,2);
			pMainWing2Layout->addWidget(lab13,4,2);
			pMainWing2Layout->addWidget(m_pctrlXLEWing2,2,3);
			pMainWing2Layout->addWidget(m_pctrlZLEWing2,3,3);
			pMainWing2Layout->addWidget(m_pctrlWingTilt2,4,3);
			pMainWing2Layout->addWidget(m_pctrlLen3,2,4);
			pMainWing2Layout->addWidget(m_pctrlLen4,3,4);
			pMainWing2Layout->addWidget(lab14,4,4);
		}
		pMainWing2Box->setLayout(pMainWing2Layout);
	}

	QGroupBox *pStabBox = new QGroupBox(tr("Elevator"));
	{
		QGridLayout *pStabLayout = new QGridLayout;
		{
			m_pctrlStabCheck = new QCheckBox(tr("Elevator"));
			m_pctrlDefineStab = new QPushButton(tr("Define"));
			QLabel *lab21 = new QLabel(tr("x="));
			QLabel *lab22 = new QLabel(tr("z="));
			QLabel *lab23 = new QLabel(tr("Tilt Angle="));
			lab21->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab22->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			lab23->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlXLEStab = new DoubleEdit(550.00);
			m_pctrlZLEStab = new DoubleEdit(550.00);
			m_pctrlStabTilt = new DoubleEdit(0.0,2);
			m_pctrlLen5 = new QLabel("mm");
			m_pctrlLen6 = new QLabel("mm");
			QLabel *lab24 = new QLabel(QString::fromUtf8("°"));
			pStabLayout->addWidget(m_pctrlStabCheck,1,1);
			pStabLayout->addWidget(m_pctrlDefineStab, 2,1);
			pStabLayout->addWidget(lab21,2,2);
			pStabLayout->addWidget(lab22,4,2);
			pStabLayout->addWidget(lab23,5,2);
			pStabLayout->addWidget(m_pctrlXLEStab,2,3);
			pStabLayout->addWidget(m_pctrlZLEStab,4,3);
			pStabLayout->addWidget(m_pctrlStabTilt,5,3);
			pStabLayout->addWidget(m_pctrlLen5,2,4);
			pStabLayout->addWidget(m_pctrlLen6,4,4);
			pStabLayout->addWidget(lab24,5,4);
			pStabLayout->setRowStretch(6,1);
		}
		pStabBox->setLayout(pStabLayout);
	}

	QGroupBox *pFinBox = new QGroupBox(tr("Fin"));
	{
		QGridLayout *pFinLayout = new QGridLayout;
		{
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
			m_pctrlXLEFin = new DoubleEdit(600.00);
			m_pctrlYLEFin = new DoubleEdit(0.00);
			m_pctrlZLEFin = new DoubleEdit(50.00);
			m_pctrlFinTilt = new DoubleEdit(0.0,2);
			m_pctrlLen7= new QLabel("mm");
			m_pctrlLen8 = new QLabel("mm");
			m_pctrlLen9 = new QLabel("mm");
			QLabel *lab35 = new QLabel(QString::fromUtf8("°"));
			pFinLayout->addWidget(m_pctrlFinCheck,1,1);
			pFinLayout->addWidget(m_pctrlDefineFin, 2,1);
			pFinLayout->addWidget(m_pctrlSymFin, 3,1);
			pFinLayout->addWidget(m_pctrlDoubleFin, 4,1);
			pFinLayout->addWidget(lab31,2,2);
			pFinLayout->addWidget(lab32,3,2);
			pFinLayout->addWidget(lab33,4,2);
			pFinLayout->addWidget(lab34,5,2);
			pFinLayout->addWidget(m_pctrlXLEFin,2,3);
			pFinLayout->addWidget(m_pctrlYLEFin,3,3);
			pFinLayout->addWidget(m_pctrlZLEFin,4,3);
			pFinLayout->addWidget(m_pctrlFinTilt,5,3);
			pFinLayout->addWidget(m_pctrlLen7,2,4);
			pFinLayout->addWidget(m_pctrlLen8,3,4);
			pFinLayout->addWidget(m_pctrlLen9,4,4);
			pFinLayout->addWidget(lab35,5,4);
		}
		pFinBox->setLayout(pFinLayout);
	}

	QGroupBox *pBodyBox = new QGroupBox(tr("Body"));
	{
		QHBoxLayout *pBodyName = new QHBoxLayout;
		{
			m_pctrlBody = new QCheckBox(tr("Body"));
			m_pctrlDefineBody = new QPushButton(tr("Define"));
			m_pctrlImportBody = new QPushButton(tr("Import"));
			pBodyName->addWidget(m_pctrlBody);
			pBodyName->addWidget(m_pctrlDefineBody);
			pBodyName->addWidget(m_pctrlImportBody);
			pBodyName->addStretch(1);
		}
		QGridLayout *pBodyPos = new QGridLayout;
		{
			pBodyPos->setColumnStretch(0,3);
			pBodyPos->setColumnStretch(1,0);
			pBodyPos->setColumnStretch(2,0);
			m_pctrlXBody = new DoubleEdit(0.00);
			m_pctrlZBody = new DoubleEdit(0.00);
			QLabel *lab41 = new QLabel(tr("x="));
			QLabel *lab42 = new QLabel(tr("z="));
			m_pctrlLen10 = new QLabel("mm");
			m_pctrlLen11 = new QLabel("mm");
			pBodyPos->addWidget(lab41,1,1);
			pBodyPos->addWidget(m_pctrlXBody,1,2);
			pBodyPos->addWidget(m_pctrlLen10,1,3);
			pBodyPos->addWidget(lab42,2,1);
			pBodyPos->addWidget(m_pctrlZBody,2,2);
			pBodyPos->addWidget(m_pctrlLen11,2,3);
		}
		QVBoxLayout *pBodyLayout = new QVBoxLayout;
		{
			QLabel *BodyWarning = new QLabel(tr("Warning:\nIncluding the body in the analysis is not recommended.\nCheck the guidelines for explanations."));
			pBodyLayout->addWidget(BodyWarning);
			pBodyLayout->addLayout(pBodyName);
			pBodyLayout->addLayout(pBodyPos);
			pBodyLayout->addStretch(1);
		}
		pBodyBox->setLayout(pBodyLayout);
	}

	QGridLayout *pData1Layout = new QGridLayout;
	{
		QLabel *lab101 = new QLabel(tr("Wing Area = "));
		QLabel *lab102 = new QLabel(tr("Wing Span = "));
		QLabel *lab103 = new QLabel(tr("Elev. Area = "));
		QLabel *lab104 = new QLabel(tr("Elev. Lever Arm = "));
		lab101->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab102->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab103->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab104->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
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

		m_pctrlSurf1 = new QLabel("dm2", this);
		m_pctrlSurf2 = new QLabel("dm2", this);
		m_pctrlSurf3 = new QLabel("dm2", this);
		m_pctrlLen12 = new QLabel("mm", this);
		m_pctrlLen13 = new QLabel("mm", this);
//		m_pctrlVolume = new QLabel("mm3", this);
		pData1Layout->addWidget(lab101, 1, 1);
		pData1Layout->addWidget(m_pctrlWingSurface,1,2);
		pData1Layout->addWidget(m_pctrlSurf1, 1, 3);
		pData1Layout->addWidget(lab102, 2, 1);
		pData1Layout->addWidget(m_pctrlWingSpan, 2, 2);
		pData1Layout->addWidget(m_pctrlLen12, 2, 3);
		pData1Layout->addWidget(lab103, 3, 1);
		pData1Layout->addWidget(m_pctrlStabSurface, 3, 2);
		pData1Layout->addWidget(m_pctrlSurf2, 3, 3);
		pData1Layout->addWidget(lab104, 4, 1);
		pData1Layout->addWidget(m_pctrlStabLeverArm, 4, 2);
		pData1Layout->addWidget(m_pctrlLen13, 4, 3);
	}

	QGridLayout *pData2Layout = new QGridLayout;
	{
		QLabel *lab105 = new QLabel(tr("Fin Area = "));
		QLabel *lab106 = new QLabel(tr("TailVolume = "));
		QLabel *lab108 = new QLabel(tr("Total Panels = "));
		lab105->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab106->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		lab108->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlFinSurface->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlStabVolume->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlVLMTotalPanels->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		pData2Layout->addWidget(lab105, 1, 1);
		pData2Layout->addWidget(m_pctrlFinSurface, 1, 2);
		pData2Layout->addWidget(m_pctrlSurf3, 1, 3);
		pData2Layout->addWidget(lab106, 2, 1);
		pData2Layout->addWidget(m_pctrlStabVolume, 2, 2);
		pData2Layout->addWidget(lab108, 3, 1);
		pData2Layout->addWidget(m_pctrlVLMTotalPanels, 3, 2);
	}

	QHBoxLayout *pCommandButtonsLayout = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		CancelButton = new QPushButton(tr("Cancel"));
		pCommandButtonsLayout->addStretch(1);
		pCommandButtonsLayout->addWidget(OKButton);
		pCommandButtonsLayout->addStretch(1);
		pCommandButtonsLayout->addWidget(CancelButton);
		pCommandButtonsLayout->addStretch(1);
	}


	QGridLayout *pGeomLayout = new QGridLayout;
	{
		pGeomLayout->addWidget(pNameBox,1,1);
		pGeomLayout->addWidget(pBodyBox,1,2);
		pGeomLayout->addWidget(pMainWingBox,2,1);
		pGeomLayout->addWidget(pMainWing2Box,2,2);
		pGeomLayout->addWidget(pStabBox,3,1);
		pGeomLayout->addWidget(pFinBox,3,2);
		pGeomLayout->addLayout(pData1Layout,4,1);
		pGeomLayout->addLayout(pData2Layout,4,2);
	}


	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pGeomLayout);
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pCommandButtonsLayout);
		pMainLayout->addStretch(1);
	}
	setLayout(pMainLayout);
}
















