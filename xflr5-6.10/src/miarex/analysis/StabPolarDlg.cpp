/****************************************************************************

	StabPolarDlg Class
	Copyright (C) 2010-2014 Andre Deperrois adeperrois@xflr5.com

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
  
#include "../../misc/Settings.h"
#include "../../globals.h"
#include "../../objects/WPolar.h"
#include "../../misc/Units.h"
#include "../Miarex.h"
#include "StabPolarDlg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <math.h>



void *StabPolarDlg::s_pMiarex;
WPolar StabPolarDlg::s_StabPolar;

StabPolarDlg::StabPolarDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Stability Polar Definition"));

	m_bAutoName = true;
	m_UnitType   = 1;

	m_pWingList[0]  = NULL;
	m_pWingList[1] = NULL;
	m_pWingList[2]  = NULL;
	m_pWingList[3]   = NULL;
	m_pctrlControlTable = NULL;
	m_pControlModel   = NULL;
	m_pCtrlDelegate   = NULL;

/*	s_StabPolar.m_bAutoInertia = true;

	s_StabPolar.m_CoG.Set(0.0,0.0,0.0);
	s_StabPolar.m_CoGIxx = s_StabPolar.m_CoGIyy = s_StabPolar.m_CoGIzz = s_StabPolar.m_CoGIxz = 0.0;

	s_StabPolar.m_Mass     = 0.0;

	memset(s_StabPolar.m_MinControl, 0, sizeof(s_StabPolar.m_MinControl));
	memset(s_StabPolar.m_MaxControl, 0, sizeof(s_StabPolar.m_MaxControl));
	memset(s_StabPolar.m_bActiveControl, 0, sizeof(s_StabPolar.m_bActiveControl));

	s_StabPolar.m_RefAreaType = 1;

	s_StabPolar.m_Density    = 1.225;
	s_StabPolar.m_Viscosity  = 1.5e-5;

	s_StabPolar.m_Beta  = 0.0;
	s_StabPolar.m_BankAngle = 0.0;

	s_StabPolar.m_nControls = 0;
	
	s_StabPolar.m_bViscous  = true;
	s_StabPolar.m_bThinSurfaces = true;
	s_StabPolar.m_AnalysisMethod = VLMMETHOD; */
	s_StabPolar.m_WPolarType = STABILITYPOLAR;

	SetupLayout();
	Connect();
}


void StabPolarDlg::Connect()
{
	connect(m_pctrlUnit1,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlIgnoreBodyPanels, SIGNAL(clicked()), this, SLOT(OnIgnoreBodyPanels()));
	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(m_pctrlWingMethod2, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlWingMethod3, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));

	connect(m_pctrlDensity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlBeta, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlPhi, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlMass, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlCoGx, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlCoGz, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlIxx, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlIyy, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlIzz, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlIxz, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));

	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnWPolarName()));
	connect(m_pctrlAutoName, SIGNAL(clicked()), this, SLOT(OnAutoName()));

	connect(m_pctrlPlaneInertia, SIGNAL(clicked()), this, SLOT(OnAutoInertia()));

//	connect(m_pctrlAVLControls, SIGNAL(clicked()), this, SLOT(OnAVLControls()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}



void StabPolarDlg::FillUFOInertia()
{
	if(m_pPlane)
	{
		s_StabPolar.m_Mass = m_pPlane->TotalMass();
		s_StabPolar.m_CoG = m_pPlane->CoG();

		s_StabPolar.m_CoGIxx = m_pPlane->m_CoGIxx;
		s_StabPolar.m_CoGIyy = m_pPlane->m_CoGIyy;
		s_StabPolar.m_CoGIzz = m_pPlane->m_CoGIzz;
		s_StabPolar.m_CoGIxz = m_pPlane->m_CoGIxz;
	}
	else if(m_pWingList[0])
	{
		s_StabPolar.m_Mass = m_pWingList[0]->TotalMass();
		s_StabPolar.m_CoG = m_pWingList[0]->CoG();
		s_StabPolar.m_CoGIxx = m_pWingList[0]->m_CoGIxx;
		s_StabPolar.m_CoGIyy = m_pWingList[0]->m_CoGIyy;
		s_StabPolar.m_CoGIzz = m_pWingList[0]->m_CoGIzz;
		s_StabPolar.m_CoGIxz = m_pWingList[0]->m_CoGIxz;
	}

	m_pctrlMass->SetValue(s_StabPolar.m_Mass*Units::kgtoUnit());
	m_pctrlCoGx->SetValue(s_StabPolar.m_CoG.x*Units::mtoUnit());
	m_pctrlCoGz->SetValue(s_StabPolar.m_CoG.z*Units::mtoUnit());
	m_pctrlIxx->SetValue(s_StabPolar.m_CoGIxx*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
	m_pctrlIyy->SetValue(s_StabPolar.m_CoGIyy*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
	m_pctrlIzz->SetValue(s_StabPolar.m_CoGIzz*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
	m_pctrlIxz->SetValue(s_StabPolar.m_CoGIxz*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
}



void StabPolarDlg::FillControlList()
{
	m_pControlModel->setRowCount(s_StabPolar.m_nControls);//temporary
	QString str, strong;
	QModelIndex ind;
	int i;
	Units::getLengthUnitLabel(str);

	s_StabPolar.m_nControls = 0;
	if(m_pPlane)
	{
		ind = m_pControlModel->index(s_StabPolar.m_nControls, 0, QModelIndex());
		m_pControlModel->setData(ind, tr("Wing Tilt (")+QString::fromUtf8("°")+")");

		ind = m_pControlModel->index(s_StabPolar.m_nControls, 1, QModelIndex());
		m_pControlModel->setData(ind, s_StabPolar.m_ControlGain[0]);

		++s_StabPolar.m_nControls;

		if(m_pWingList[2])
		{
			ind = m_pControlModel->index(s_StabPolar.m_nControls, 0, QModelIndex());
			m_pControlModel->setData(ind, tr("Elevator Tilt ")+QString::fromUtf8("(°)"));


			ind = m_pControlModel->index(s_StabPolar.m_nControls, 1, QModelIndex());
			m_pControlModel->setData(ind, s_StabPolar.m_ControlGain[1]);

			++s_StabPolar.m_nControls;
		}
	}

	for(i=0; i<m_pWingList[0]->m_nFlaps; i++)
	{
		ind = m_pControlModel->index(i+s_StabPolar.m_nControls, 0, QModelIndex());
		strong = QString(tr("Wing Flap angle %1 ")+QString::fromUtf8("(°)")).arg(i+1);
		m_pControlModel->setData(ind, strong);

		ind = m_pControlModel->index(i+s_StabPolar.m_nControls, 1, QModelIndex());
		m_pControlModel->setData(ind, s_StabPolar.m_ControlGain[i+s_StabPolar.m_nControls]);
	}
	s_StabPolar.m_nControls += m_pWingList[0]->m_nFlaps;


	if(m_pWingList[2])
	{
		for(i=0; i<m_pWingList[2]->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+s_StabPolar.m_nControls, 0, QModelIndex());
			strong = QString(tr("Elevator Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

			ind = m_pControlModel->index(s_StabPolar.m_nControls, 1, QModelIndex());
			m_pControlModel->setData(ind, s_StabPolar.m_ControlGain[i+s_StabPolar.m_nControls]);
		}
		s_StabPolar.m_nControls += m_pWingList[2]->m_nFlaps;
	}
	if(m_pWingList[3])
	{
		for(i=0; i<m_pWingList[3]->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+s_StabPolar.m_nControls, 0, QModelIndex());
			strong = QString(tr("Fin Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

			ind = m_pControlModel->index(s_StabPolar.m_nControls, 1, QModelIndex());
			m_pControlModel->setData(ind, s_StabPolar.m_ControlGain[i+s_StabPolar.m_nControls]);
		}
		s_StabPolar.m_nControls += m_pWingList[3]->m_nFlaps;
	}
}


void StabPolarDlg::SetViscous()
{
	bool bViscous=true;
	int nCtrl = 0;

	if(m_pPlane) nCtrl++;
	if(m_pPlane && m_pWingList[2]) nCtrl++;

	for(int i=nCtrl; i<s_StabPolar.m_nControls; i++)
	{
        if(s_StabPolar.m_bThinSurfaces && qAbs(s_StabPolar.m_ControlGain[i])>PRECISION)
		{
			bViscous = false;
			break;
		}
	}

	if(!bViscous)
	{
		m_pctrlViscous->setChecked(false);
		m_pctrlViscous->setEnabled(false);
		s_StabPolar.m_bViscous = false;
	}
	else
	{
		if(s_StabPolar.m_bViscous) m_pctrlViscous->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}
}


void StabPolarDlg::InitDialog(Plane *pPlane, WPolar *pWPolar)
{
	QString strLen, strMass, strInertia;

	m_pPlane = pPlane;
	if(m_pPlane)
	{
		m_pWingList[0] = pPlane->wing();
		m_pWingList[1] = pPlane->wing2();
		m_pWingList[2] = pPlane->stab();
		m_pWingList[3] = pPlane->fin();
	}

	Units::getLengthUnitLabel(strLen);
	Units::getWeightUnitLabel(strMass);
	strInertia = strMass+"."+strLen+QString::fromUtf8("²");
	m_pctrlLab299->setText(strMass);
	m_pctrlLab300->setText(strLen);
	m_pctrlLab301->setText(strLen);
	m_pctrlLab302->setText(strInertia);
	m_pctrlLab303->setText(strInertia);
	m_pctrlLab304->setText(strInertia);
	m_pctrlLab305->setText(strInertia);

	m_pctrlDensity->SetPrecision(5);
	m_pctrlViscosity->SetPrecision(3);

	QString str;
	Units::getSpeedUnitLabel(str);

	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);
	OnUnit();

	if(s_StabPolar.m_RefAreaType==1) m_pctrlArea1->setChecked(true);
	else                             m_pctrlArea2->setChecked(true);

	if(pWPolar && pWPolar->m_WPolarType==STABILITYPOLAR)
	{
//		m_bAutoName = false;
//		m_pctrlWPolarName->setText(pWPolar->m_PlrName);
		s_StabPolar.DuplicateSpec(pWPolar);
	}
	else
	{
	}
	m_bAutoName = true;
	m_pctrlAutoName->setChecked(m_bAutoName);

	if(m_pPlane)
	{
		s_StabPolar.m_AnalysisMethod = PANELMETHOD;
		s_StabPolar.m_PlaneName = m_pPlane->planeName();
		m_pctrlUFOName->setText(m_pPlane->planeName());
		m_pctrlAnalysisControls->setCurrentIndex(1);
	}
	else if(m_pWingList[0])
	{
		s_StabPolar.m_PlaneName = m_pWingList[0]->WingName();
		m_pctrlUFOName->setText(m_pWingList[0]->WingName());
		m_pctrlAnalysisControls->setCurrentIndex(0);
	}
	else return;

	m_pctrlWPolarName->setText(s_StabPolar.m_WPlrName);

	if(m_pPlane)
	{
		m_pctrlAnalysisControls->setCurrentIndex(1);
	}
	else m_pctrlAnalysisControls->setCurrentIndex(0);

	s_StabPolar.m_nControls = 0;
	s_StabPolar.m_nControls += m_pWingList[0]->m_nFlaps;

	if(m_pPlane) 
	{
		s_StabPolar.m_nControls++; // Wing Tilt
		if(m_pWingList[2])
		{
			s_StabPolar.m_nControls++;//stab tilt
			s_StabPolar.m_nControls += m_pWingList[2]->m_nFlaps;
		}
		if(m_pWingList[3]) s_StabPolar.m_nControls+=m_pWingList[3]->m_nFlaps;

	}

	if(m_pPlane) m_pctrlUFOName->setText(m_pPlane->planeName());
	else         m_pctrlUFOName->setText(m_pWingList[0]->WingName());


	m_pctrlBeta->SetValue(s_StabPolar.m_Beta);
	m_pctrlPhi->SetValue(s_StabPolar.m_BankAngle);

	if(s_StabPolar.m_AnalysisMethod==LLTMETHOD)
	{
		s_StabPolar.m_AnalysisMethod = PANELMETHOD;
		s_StabPolar.m_bThinSurfaces = true;
	}

	if(m_pPlane) m_pctrlPanelMethod->setChecked(true);
	else
	{
		m_pctrlPanelMethod->setChecked(false);

		m_pctrlWingMethod2->setChecked(s_StabPolar.m_AnalysisMethod==VLMMETHOD);
		m_pctrlWingMethod3->setChecked(s_StabPolar.m_AnalysisMethod==PANELMETHOD);
//		m_pctrlWingMethod2->setChecked(s_StabPolar.m_bThinSurfaces);
//		m_pctrlWingMethod3->setChecked(!s_StabPolar.m_bThinSurfaces);
	}

	m_pctrlPlaneInertia->setChecked(s_StabPolar.m_bAutoInertia);
	m_pctrlViscous->setChecked(s_StabPolar.m_bViscous);
	m_pctrlIgnoreBodyPanels->setEnabled(m_pPlane && m_pPlane->body());
	m_pctrlIgnoreBodyPanels->setChecked(m_pPlane && m_pPlane->body() && s_StabPolar.m_bIgnoreBodyPanels);
	if(!m_pPlane || !m_pPlane->body()) s_StabPolar.m_bIgnoreBodyPanels=false;
//	m_pctrlAVLControls->setChecked(s_StabPolar.m_bThinSurfaces);

	OnAutoInertia();


	m_pControlModel->setColumnCount(2);
	m_pControlModel->setHeaderData(0, Qt::Horizontal, tr("Control Name"));
	m_pControlModel->setHeaderData(1, Qt::Horizontal, tr("Gain")+QString::fromUtf8("(°/unit)"));
	m_pCtrlDelegate->m_Precision[1] = 2;

	FillControlList();
	SetWPolarName();

	m_pctrlControlTable->setFocus();
}


void StabPolarDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				ReadCtrlData();
				SetWPolarName();
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
		}
		default:
			event->ignore();
	}

}



void StabPolarDlg::OnArea()
{
	if(m_pctrlArea1->isChecked())
	{
		s_StabPolar.m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_StabPolar.m_RefAreaType = 2;
	}
	SetWPolarName();
}



void StabPolarDlg::OnAutoName()
{
	m_bAutoName = m_pctrlAutoName->isChecked();
	if(m_bAutoName) SetWPolarName();
}


void StabPolarDlg::OnCellChanged(QWidget *)
{
	ReadCtrlData();
	SetWPolarName();
}




void StabPolarDlg::OnEditingFinished()
{
	ReadParams();
	SetWPolarName();
}



void StabPolarDlg::OnUnit() 
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(s_StabPolar.m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m"+QString::fromUtf8("²")+"/s");
	}
	else 
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(s_StabPolar.m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft"+QString::fromUtf8("²")+"/s");
	}
	SetDensity();
}


void StabPolarDlg::OnOK()
{
	ReadCtrlData();
	ReadParams();

    if(qAbs(s_StabPolar.m_Mass)<PRECISION)
	{
		QMessageBox::warning(this, tr("Warning"),tr("Mass must be non-zero for type 7 polars"));
		m_pctrlMass->setFocus();
		return;
	}



	if(!m_pctrlWPolarName->text().length())
	{
		QMessageBox::warning(this, tr("Warning"), tr("Must enter a name"));
		m_pctrlWPolarName->setFocus();
		return;
	}
	s_StabPolar.m_WPlrName = m_pctrlWPolarName->text();

	accept();
}



void StabPolarDlg::OnViscous()
{
	s_StabPolar.m_bViscous = m_pctrlViscous->isChecked();
	SetWPolarName();
}



void StabPolarDlg::OnIgnoreBodyPanels()
{
	s_StabPolar.m_bIgnoreBodyPanels = m_pctrlIgnoreBodyPanels->isChecked();
	SetWPolarName();
}


void StabPolarDlg::OnWPolarName()
{
	m_pctrlAutoName->setChecked(false);
	m_bAutoName = false;
}


void StabPolarDlg::ReadCtrlData()
{
	int i;

	s_StabPolar.m_ControlGain.clear();
	for(i=0; i<s_StabPolar.m_nControls; i++)
	{
//		k    = m_pControlModel->index(i, 1, QModelIndex()).data().toInt();

		s_StabPolar.m_ControlGain.append(m_pControlModel->index(i, 1, QModelIndex()).data().toDouble()); //is the gain, AVL-like
	}

	SetViscous();

}


void StabPolarDlg::ReadParams()
{
	if(m_pctrlUnit1->isChecked())
	{
		s_StabPolar.m_Density   = m_pctrlDensity->Value();
		s_StabPolar.m_Viscosity = m_pctrlViscosity->Value();
	}
	else
	{
		s_StabPolar.m_Density   = m_pctrlDensity->Value()   / 0.00194122;
		s_StabPolar.m_Viscosity = m_pctrlViscosity->Value() / 10.7182881;
	}

	s_StabPolar.m_Beta  = m_pctrlBeta->Value();
	s_StabPolar.m_BankAngle = m_pctrlPhi->Value();
	SetDensity();

	s_StabPolar.m_Mass  = m_pctrlMass->Value() / Units::kgtoUnit();
	s_StabPolar.m_CoG.x = m_pctrlCoGx->Value() / Units::mtoUnit();
	s_StabPolar.m_CoG.z = m_pctrlCoGz->Value() / Units::mtoUnit();
	s_StabPolar.m_CoGIxx = m_pctrlIxx->Value()  / Units::kgtoUnit() / Units::mtoUnit() / Units::mtoUnit();
	s_StabPolar.m_CoGIyy = m_pctrlIyy->Value()  / Units::kgtoUnit() / Units::mtoUnit() / Units::mtoUnit();
	s_StabPolar.m_CoGIzz = m_pctrlIzz->Value()  / Units::kgtoUnit() / Units::mtoUnit() / Units::mtoUnit();
	s_StabPolar.m_CoGIxz = m_pctrlIxz->Value()  / Units::kgtoUnit() / Units::mtoUnit() / Units::mtoUnit();

	s_StabPolar.m_bViscous = m_pctrlViscous->isChecked();
	s_StabPolar.m_bIgnoreBodyPanels = m_pctrlIgnoreBodyPanels->isChecked();
}




void StabPolarDlg::SetDensity()
{
	int exp, precision;
	if(m_pctrlUnit1->isChecked())
	{
		exp = (int)log(s_StabPolar.m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_StabPolar.m_Density);
	}
	else
	{
		exp = (int)log(s_StabPolar.m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_StabPolar.m_Density* 0.00194122);
	}
}



void StabPolarDlg::SetupLayout()
{
	QGroupBox *pNameGroupBox = new QGroupBox(tr("Polar Name"));
	{
		QVBoxLayout *NameLayout = new QVBoxLayout;
		{
			QHBoxLayout *MiscLayout = new QHBoxLayout;
			{
				m_pctrlUFOName = new QLabel(tr("Wing Name"));
				MiscLayout->addWidget(m_pctrlUFOName);
				MiscLayout->addStretch(2);
			}
			m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
			m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
			QHBoxLayout *AutoNameLayout = new QHBoxLayout;
			{
				AutoNameLayout->addWidget(m_pctrlAutoName);
				AutoNameLayout->addWidget(m_pctrlWPolarName);
			}
			NameLayout->addLayout(MiscLayout);
			NameLayout->addLayout(AutoNameLayout);
		}
		pNameGroupBox->setLayout(NameLayout);
	}

	QGroupBox *pPlaneGroupBox = new QGroupBox(tr("Plane and Flight Data"));
	{
		QVBoxLayout *PlaneFlightLayout = new QVBoxLayout;
		{
			QGridLayout *PlaneLayout = new QGridLayout;
			{
				QLabel *lab2 = new QLabel(tr("b ="));
				QLabel *lab3 = new QLabel(tr("f ="));
				lab2->setFont(QFont("Symbol"));
				lab3->setFont(QFont("Symbol"));
				lab2->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
				lab3->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
				QLabel *lab4 = new QLabel(QString::fromUtf8("°"));
				QLabel *lab5 = new QLabel(QString::fromUtf8("°"));
                m_pctrlBeta  = new DoubleEdit(0.818,2);
                m_pctrlPhi   = new DoubleEdit(0.414,2);

				PlaneLayout->addWidget(lab2,1,1);
				PlaneLayout->addWidget(m_pctrlBeta,1,2);
				PlaneLayout->addWidget(lab4 ,1,3);
				PlaneLayout->addWidget(lab3,2,1);
				PlaneLayout->addWidget(m_pctrlPhi,2,2);
				PlaneLayout->addWidget(lab5,2,3);
			}

			m_pctrlViscous = new QCheckBox(tr("Viscous Analysis"));
		//	PlaneLayout->addWidget(m_pctrlViscous,5,1,1,3);
			PlaneFlightLayout->addLayout(PlaneLayout);
			PlaneFlightLayout->addWidget(m_pctrlViscous);
			QLabel *lab11 = new QLabel(tr("Note : the analysis may be of the viscous type\nonly if all the flap controls are inactive"));
			PlaneFlightLayout->addWidget(lab11);
			PlaneFlightLayout->addStretch(1);

			pPlaneGroupBox->setLayout(PlaneFlightLayout);
		}
	}


	QGroupBox *pAeroDataGroupBox = new QGroupBox(tr("Aerodynamic Data"));
	{
		QGridLayout *AeroDataLayout = new QGridLayout;
		{
			QLabel *lab9 = new QLabel(tr("Unit"));
			m_pctrlUnit1 = new QRadioButton(tr("International"));
			m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
			m_pctrlRho           = new QLabel("r =");
            m_pctrlDensity       = new DoubleEdit(1.500e-5,3);
			m_pctrlDensityUnit   = new QLabel("kg/m3");
			m_pctrlNu            = new QLabel("n =");
			m_pctrlRho->setFont(QFont("Symbol"));
			m_pctrlNu->setFont(QFont("Symbol"));

            m_pctrlViscosity     = new DoubleEdit(1.225,3);
			m_pctrlViscosityUnit = new QLabel("m2/s");
			AeroDataLayout->addWidget(lab9,1,1);
			AeroDataLayout->addWidget(m_pctrlUnit1,1,2);
			AeroDataLayout->addWidget(m_pctrlUnit2,1,3);
			AeroDataLayout->addWidget(m_pctrlRho,2,1);
			AeroDataLayout->addWidget(m_pctrlDensity,2,2);
			AeroDataLayout->addWidget(m_pctrlDensityUnit,2,3);
			AeroDataLayout->addWidget(m_pctrlNu,3,1);
			AeroDataLayout->addWidget(m_pctrlViscosity,3,2);
			AeroDataLayout->addWidget(m_pctrlViscosityUnit,3,3);
		}
		pAeroDataGroupBox->setLayout(AeroDataLayout);
	}


	QGroupBox *pAreaBox = new QGroupBox(tr("Reference Area for Aero Coefficients"));
	{
		QVBoxLayout *AreaOptions = new QVBoxLayout;
		{
			m_pctrlArea1 = new QRadioButton(tr("Wing Planform Area"));
			m_pctrlArea2 = new QRadioButton(tr("Wing Planform Area projected on xy plane"));
			AreaOptions->addWidget(m_pctrlArea1);
			AreaOptions->addWidget(m_pctrlArea2);
		}
		pAreaBox->setLayout(AreaOptions);
	}


	QVBoxLayout *LeftSideLayout = new QVBoxLayout;
	{
		LeftSideLayout->addWidget(pPlaneGroupBox);
		LeftSideLayout->addWidget(pAeroDataGroupBox);
		LeftSideLayout->addWidget(pAreaBox);
	}


	QGroupBox *InertiaBox = new QGroupBox("Inertia");
	{
		QVBoxLayout *InertiaLayout = new QVBoxLayout;
		{
			m_pctrlPlaneInertia = new QCheckBox(tr("Use plane inertia"));
			m_pctrlPlaneInertia->setToolTip("Activate this checbox for the polar to use dynamically the plane's inertia properties for each analysis");

			QGridLayout *InertiaDataLayout = new QGridLayout;
			{
				QLabel *Lab099 = new QLabel("Mass=");
				QLabel *Lab100 = new QLabel("CoG.x=");
				QLabel *Lab101 = new QLabel("CoG.z=");
				QLabel *Lab102 = new QLabel("Ixx=");
				QLabel *Lab103 = new QLabel("Iyy=");
				QLabel *Lab104 = new QLabel("Izz=");
				QLabel *Lab105 = new QLabel("Ixz=");
				Lab099->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab100->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab101->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab102->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab103->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab104->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				Lab105->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
				m_pctrlLab299 = new QLabel;
				m_pctrlLab300 = new QLabel;
				m_pctrlLab301 = new QLabel;
				m_pctrlLab302 = new QLabel;
				m_pctrlLab303 = new QLabel;
				m_pctrlLab304 = new QLabel;
				m_pctrlLab305 = new QLabel;

                m_pctrlMass = new DoubleEdit(0.0,3);
                m_pctrlCoGx = new DoubleEdit(0.0,3);
                m_pctrlCoGz = new DoubleEdit(0.0,3);
                m_pctrlIxx  = new DoubleEdit(0.0);
                m_pctrlIyy  = new DoubleEdit(0.0);
                m_pctrlIzz  = new DoubleEdit(0.0);
                m_pctrlIxz  = new DoubleEdit(0.0);
				InertiaDataLayout->addWidget(Lab099,1,1);
				InertiaDataLayout->addWidget(Lab100,2,1);
				InertiaDataLayout->addWidget(Lab101,3,1);
				InertiaDataLayout->addWidget(Lab102,4,1);
				InertiaDataLayout->addWidget(Lab103,5,1);
				InertiaDataLayout->addWidget(Lab104,6,1);
				InertiaDataLayout->addWidget(Lab105,7,1);
				InertiaDataLayout->addWidget(m_pctrlMass,1,2);
				InertiaDataLayout->addWidget(m_pctrlCoGx,2,2);
				InertiaDataLayout->addWidget(m_pctrlCoGz,3,2);
				InertiaDataLayout->addWidget(m_pctrlIxx, 4,2);
				InertiaDataLayout->addWidget(m_pctrlIyy, 5,2);
				InertiaDataLayout->addWidget(m_pctrlIzz, 6,2);
				InertiaDataLayout->addWidget(m_pctrlIxz, 7,2);
				InertiaDataLayout->addWidget(m_pctrlLab299,1,3);
				InertiaDataLayout->addWidget(m_pctrlLab300,2,3);
				InertiaDataLayout->addWidget(m_pctrlLab301,3,3);
				InertiaDataLayout->addWidget(m_pctrlLab302,4,3);
				InertiaDataLayout->addWidget(m_pctrlLab303,5,3);
				InertiaDataLayout->addWidget(m_pctrlLab304,6,3);
				InertiaDataLayout->addWidget(m_pctrlLab305,7,3);
			}

			InertiaLayout->addWidget(m_pctrlPlaneInertia);
		//	InertiaLayout->addWidget(m_pctrlEstimation);
			InertiaLayout->addLayout(InertiaDataLayout);
			InertiaLayout->addStretch(1);
		}
		InertiaBox->setLayout(InertiaLayout);
	}

	//Analysis method
	m_pctrlAnalysisControls = new QStackedWidget;
	{
		QGroupBox *WingMethodBox = new QGroupBox(tr("Wing analysis methods"));
		{
			QVBoxLayout *WingMethodLayout = new QVBoxLayout;
			{
				m_pctrlWingMethod2 = new QRadioButton(tr("VLM"));
				m_pctrlWingMethod3 = new QRadioButton(tr("3D Panels"));
				WingMethodLayout->addWidget(m_pctrlWingMethod2);
				WingMethodLayout->addWidget(m_pctrlWingMethod3);
				WingMethodBox->setLayout(WingMethodLayout);
			}
		}

		QGroupBox *PlaneMethodBox = new QGroupBox(tr("Plane analysis methods"));
		{
			QVBoxLayout *PlaneMethodLayout = new QVBoxLayout;
			{
				m_pctrlPanelMethod = new QRadioButton(tr("Mix 3D Panels/VLM"));
				m_pctrlIgnoreBodyPanels = new QCheckBox(tr("Ignore Body Panels"));
				PlaneMethodLayout->addStretch();
				PlaneMethodLayout->addWidget(m_pctrlPanelMethod);
				PlaneMethodLayout->addStretch();
				PlaneMethodLayout->addWidget(m_pctrlIgnoreBodyPanels);
				PlaneMethodBox->setLayout(PlaneMethodLayout);
			}
		}
		m_pctrlAnalysisControls->addWidget(WingMethodBox);
		m_pctrlAnalysisControls->addWidget(PlaneMethodBox);
	}
//	m_pctrlAVLControls = new QCheckBox(tr("AVL Controls"));

	QVBoxLayout *RightSideLayout = new QVBoxLayout;
	{
		RightSideLayout->addWidget(InertiaBox);
		RightSideLayout->addWidget(m_pctrlAnalysisControls);
	//	RightSideLayout->addWidget(m_pctrlAVLControls);
	}


	QHBoxLayout *DataLayout = new QHBoxLayout;
	{
		DataLayout->addLayout(LeftSideLayout);
		DataLayout->addLayout(RightSideLayout);
	}

	m_pctrlControlTable = new QTableView(this);
	m_pctrlControlTable->setFont(Settings::s_TableFont);

	m_pctrlControlTable->setWindowTitle(tr("Controls"));
	m_pctrlControlTable->setMinimumWidth(400);
	m_pctrlControlTable->setMinimumHeight(150);
	m_pctrlControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlControlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlControlTable->horizontalHeader()->setStretchLastSection(true);
	
	m_pControlModel = new QStandardItemModel(this);
	m_pControlModel->setRowCount(10);//temporary
	m_pControlModel->setColumnCount(4);//temporary
	m_pctrlControlTable->setModel(m_pControlModel);

	m_pCtrlDelegate = new CtrlTableDelegate(this);
	m_pctrlControlTable->setItemDelegate(m_pCtrlDelegate);
	m_pCtrlDelegate->m_pCtrlModel = m_pControlModel;
	connect(m_pCtrlDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	int  *precision = new int[6];
	precision[0]  = 2;
	precision[1]  = 0;
	precision[2]  = 3;
	precision[3]  = 3;
	precision[4]  = 3;
	precision[5]  = 3;

	m_pCtrlDelegate->m_Precision = precision;


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		OKButton->setDefault(true);
		CancelButton = new QPushButton(tr("Cancel"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	{
		QLabel* SignLabel = new QLabel(tr("Note: + sign means trailing edge down"));
		MainLayout->addWidget(pNameGroupBox);
		MainLayout->addLayout(DataLayout);
		MainLayout->addWidget(m_pctrlControlTable);
		MainLayout->addStretch(1);
		MainLayout->addWidget(SignLabel);
		MainLayout->addStretch(1);
		MainLayout->addLayout(CommandButtons);
		MainLayout->addStretch(1);
	}

	setLayout(MainLayout);
}




void StabPolarDlg::SetWPolarName()
{
	if(!m_bAutoName || !m_pPlane) return;
	QString str, strong;
	int i, nCtrl;
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	Units::getSpeedUnitLabel(str);
	QString WPolarName = "T7";

	if(!m_pPlane && !s_StabPolar.m_bThinSurfaces) WPolarName += "-Panel";
	if(s_StabPolar.m_bThinSurfaces)
	{
		if(pMiarex->s_bVLM1) WPolarName += "-VLM1";
		else		         WPolarName += "-VLM2";
	}

	nCtrl = 0;

	if(qAbs(s_StabPolar.m_ControlGain[0])>PRECISION && m_pPlane)
	{
		strong = QString(QString::fromUtf8("-Wing(%g1)"))
						   .arg(s_StabPolar.m_ControlGain[0],0,'f',1);
		WPolarName += strong;
	}
	nCtrl++;
	if(m_pWingList[2])
	{
		if(qAbs(s_StabPolar.m_ControlGain[1])>PRECISION)
		{
			strong = QString(QString::fromUtf8("-Elev(g%1)")).arg(s_StabPolar.m_ControlGain[1],0,'f',1);
			WPolarName += strong;
		}
		nCtrl++;
	}

	for(i=0; i<m_pWingList[0]->m_nFlaps; i++)
	{
		if(qAbs(s_StabPolar.m_ControlGain[i+nCtrl])>PRECISION)
		{
			strong = QString(QString::fromUtf8("-WF%1(g%2)"))
					 .arg(i+1)
					 .arg(s_StabPolar.m_ControlGain[i+nCtrl],0,'f',1);
			WPolarName += strong;
		}
	}
	nCtrl += m_pWingList[0]->m_nFlaps;

	if(m_pWingList[2])
	{
		for(i=0; i<m_pWingList[2]->m_nFlaps; i++)
		{
			if(qAbs(s_StabPolar.m_ControlGain[i+nCtrl])>PRECISION)
			{
				strong = QString(QString::fromUtf8("-EF%1(g%2)"))
						 .arg(i+1).arg(s_StabPolar.m_ControlGain[i+nCtrl]);
				WPolarName += strong;
			}
		}
		nCtrl += m_pWingList[2]->m_nFlaps;
	}

	if(m_pWingList[3])
	{
		for(i=0; i<m_pWingList[3]->m_nFlaps; i++)
		{
			if(qAbs(s_StabPolar.m_ControlGain[i+nCtrl])>PRECISION)
			{
				strong = QString(QString::fromUtf8("-FF%1(g%2)"))
						 .arg(i+1).arg(s_StabPolar.m_ControlGain[i+nCtrl]);
				WPolarName += strong;
			}
		}
	}


    if(qAbs(s_StabPolar.m_Beta) > .001)
	{
		strong = QString(QString::fromUtf8("-b%1°")).arg(s_StabPolar.m_Beta,0,'f',1);
		WPolarName += strong;
	}
	
    if(qAbs(s_StabPolar.m_BankAngle) > .001)
	{
		strong = QString(QString::fromUtf8("-B%1°")).arg(s_StabPolar.m_BankAngle,0,'f',1);
		WPolarName += strong;
	}

	if(!s_StabPolar.m_bAutoInertia)
	{
		Units::getWeightUnitLabel(str);
		strong = QString("-%1").arg(s_StabPolar.m_Mass*Units::kgtoUnit(),0,'f',3);
		WPolarName += strong+str;
		Units::getLengthUnitLabel(str);
		strong = QString("-x%1").arg(s_StabPolar.m_CoG.x*Units::mtoUnit(),0,'f',3);
		WPolarName += strong + str;

        if(qAbs(s_StabPolar.m_CoG.z)>=.000001)
		{
			strong = QString("-z%1").arg(s_StabPolar.m_CoG.z*Units::mtoUnit(),0,'f',3);
			WPolarName += strong + str;
		}
	}
//	else m_WPolarName += "-Plane_Inertia";

	if(!s_StabPolar.m_bViscous)
	{
		WPolarName += "-Inviscid";
	}
	if(s_StabPolar.m_bIgnoreBodyPanels)
	{
		WPolarName += "-NoBodyPanels";
	}
	if(s_StabPolar.m_RefAreaType==PROJECTEDAREA) WPolarName += "-proj_area";
	
	
	m_pctrlWPolarName->setText(WPolarName);
}



void StabPolarDlg::OnMethod()
{
	if (m_pctrlWingMethod2->isChecked())
	{
		s_StabPolar.m_bThinSurfaces = true;
		s_StabPolar.m_AnalysisMethod = VLMMETHOD;
	}
	else if (m_pctrlWingMethod3->isChecked())
	{
		s_StabPolar.m_bThinSurfaces = false;
		s_StabPolar.m_AnalysisMethod = PANELMETHOD;
	}

//	EnableControls();
	SetWPolarName();
}



void StabPolarDlg::showEvent(QShowEvent *event)
{
	int w, w2, w3, w6;
	w = m_pctrlControlTable->width();
	w2 = (int)((double)w/2.0);
	w3 = (int)((double)w/3.0);
	w6 = (int)((double)w/6.0);

	if(!s_StabPolar.m_bThinSurfaces)
	{
		m_pctrlControlTable->setColumnWidth(0,w3);
		m_pctrlControlTable->setColumnWidth(1,w6);
		m_pctrlControlTable->setColumnWidth(2,w6);
		m_pctrlControlTable->setColumnWidth(3,w6);
	}
	else
	{
		m_pctrlControlTable->setColumnWidth(0,w2);
		m_pctrlControlTable->setColumnWidth(1,2*w6);
	}
	event->accept();
}


void StabPolarDlg::OnAutoInertia()
{
	s_StabPolar.m_bAutoInertia = m_pctrlPlaneInertia->isChecked();
	if(s_StabPolar.m_bAutoInertia)
	{
		FillUFOInertia();
	}
	else
	{
		m_pctrlMass->SetValue(s_StabPolar.m_Mass*Units::kgtoUnit());
		m_pctrlCoGx->SetValue(s_StabPolar.m_CoG.x*Units::mtoUnit());
		m_pctrlCoGz->SetValue(s_StabPolar.m_CoG.z*Units::mtoUnit());
		m_pctrlIxx->SetValue(s_StabPolar.m_CoGIxx*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
		m_pctrlIyy->SetValue(s_StabPolar.m_CoGIyy*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
		m_pctrlIzz->SetValue(s_StabPolar.m_CoGIzz*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());
		m_pctrlIxz->SetValue(s_StabPolar.m_CoGIxz*Units::kgtoUnit()*Units::mtoUnit()*Units::mtoUnit());

	}
	m_pctrlMass->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlCoGx->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlCoGz->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlIxx->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlIyy->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlIzz->setEnabled(!s_StabPolar.m_bAutoInertia);
	m_pctrlIxz->setEnabled(!s_StabPolar.m_bAutoInertia);

	SetWPolarName();
}
