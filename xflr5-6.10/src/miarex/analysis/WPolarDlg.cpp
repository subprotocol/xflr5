/****************************************************************************

	WPolarDlg Class
	Copyright (C) 2009-2013 Andre Deperrois adeperrois@xflr5.com

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

#include "../../globals.h"
#include "../../objects/WPolar.h"
#include "../../misc/Units.h"
#include "../Miarex.h"
#include "WPolarDlg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <math.h>


void* WPolarDlg::s_pMiarex;
WPolar WPolarDlg::s_WPolar;

WPolarDlg::WPolarDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Analysis Definition"));

	m_pPlane     = NULL;
	m_bAutoName = true;
	m_WingLoad   = 0.0;
	m_UnitType  = 1;

	SetupLayout();
	Connect();
}


void WPolarDlg::Connect()
{
	connect(m_pctrlLLTMethod, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlVLMMethod, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlPanelMethod, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));

	connect(m_pctrlUnit1, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));

	connect(m_pctrlType1, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));
	connect(m_pctrlType2, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));
	connect(m_pctrlType4, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));

	connect(m_pctrlAutoName, SIGNAL(clicked()), this, SLOT(OnAutoName()));
//	connect(m_pctrlWakeRollUp, SIGNAL(clicked()), this, SLOT(OnWakeRollUp()));
	connect(m_pctrlTiltGeom, SIGNAL(clicked()), this, SLOT(OnTiltedGeom()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlIgnoreBodyPanels, SIGNAL(clicked()), this, SLOT(OnIgnoreBodyPanels()));

	connect(m_pctrlGroundEffect, SIGNAL(clicked()), this, SLOT(OnGroundEffect()));
	connect(m_pctrlPlaneInertia, SIGNAL(clicked()), this, SLOT(OnPlaneInertia()));

	connect(m_pctrlXCmRef,     SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlZCmRef,     SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlDensity,    SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity,  SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlAlpha,      SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlBeta,       SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWeight,     SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlQInf,       SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlHeight,     SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(textEdited ( const QString &  )), this, SLOT(OnWPolarName()));

	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void WPolarDlg::EnableControls()
{
	switch (s_WPolar.m_WPolarType)
	{
		case FIXEDSPEEDPOLAR:
		{
			m_pctrlQInf->setEnabled(true);
			m_pctrlAlpha->setEnabled(false);
			break;
		}
		case FIXEDLIFTPOLAR:
		{
			m_pctrlQInf->setEnabled(false);
			m_pctrlRRe->setText(" ");
			m_pctrlSRe->setText(" ");
			m_pctrlAlpha->setEnabled(false);
			break;
		}
		case FIXEDAOAPOLAR:
		{
			m_pctrlQInf->setEnabled(false);
			m_pctrlRRe->setText(" ");
			m_pctrlSRe->setText(" ");
			m_pctrlAlpha->setEnabled(true);
			break;
		}
		default:
		{
			m_pctrlQInf->setEnabled(true);
			break;
		}
	}

	m_pctrlViscous->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD);
	m_pctrlTiltGeom->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD);
	m_pctrlIgnoreBodyPanels->setEnabled(m_pPlane && m_pPlane->body());
	m_pctrlBeta->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD);
	m_pctrlGroundEffect->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD);
	m_pctrlHeight->setEnabled(m_pctrlGroundEffect->isChecked() && s_WPolar.m_AnalysisMethod==PANELMETHOD);

	m_pctrlWeight->setEnabled(!s_WPolar.m_bAutoInertia);
	m_pctrlXCmRef->setEnabled(!s_WPolar.m_bAutoInertia);
	m_pctrlZCmRef->setEnabled(!s_WPolar.m_bAutoInertia);

}



void WPolarDlg::InitDialog(Plane *pPlane, WPolar *pWPolar)
{
	QString str, str1;

	m_pPlane = pPlane;

	if(pWPolar)
	{
		m_pctrlAutoName->setChecked(false);
		m_pctrlWPolarName->setText(pWPolar->m_WPlrName);
		s_WPolar.DuplicateSpec(pWPolar);
	}
	else
	{
		m_pctrlAutoName->setChecked(true);
	}


	if(m_pPlane->isWing())
	{
		m_pctrlLLTMethod->setText("LLT");
		m_pctrlVLMMethod->setText("VLM");
	}
	else
	{
		s_WPolar.m_AnalysisMethod=VLMMETHOD;
		s_WPolar.m_bThinSurfaces = true;
		m_pctrlVLMMethod->setText(tr("Mix 3D Panels/VLM"));
		m_pctrlPanelMethod->setVisible(false);
	}

	//initialize the name box
	s_WPolar.m_PlaneName = m_pPlane->planeName();
	m_pctrlUFOName->setText(m_pPlane->planeName());


	// initialize units
	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);


	//initialize polar type
	if(s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)     m_pctrlType1->setChecked(true);
	else if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR) m_pctrlType2->setChecked(true);
	else if(s_WPolar.m_WPolarType==FIXEDAOAPOLAR)  m_pctrlType4->setChecked(true);


	//initialize inertia
	if(s_WPolar.m_bAutoInertia)
	{
		m_pctrlWeight->SetValue(m_pPlane->TotalMass() * Units::kgtoUnit());
		m_pctrlXCmRef->SetValue(m_pPlane->CoG().x * Units::mtoUnit());
		m_pctrlZCmRef->SetValue(m_pPlane->CoG().z * Units::mtoUnit());
		s_WPolar.m_Mass    = m_pPlane->TotalMass();
		s_WPolar.m_CoG.x   = m_pPlane->CoG().x;
		s_WPolar.m_CoG.z   = m_pPlane->CoG().z;
	}
	else
	{
		m_pctrlWeight->SetValue(s_WPolar.m_Mass  * Units::kgtoUnit());
		m_pctrlXCmRef->SetValue(s_WPolar.m_CoG.y * Units::mtoUnit());
		m_pctrlZCmRef->SetValue(s_WPolar.m_CoG.z * Units::mtoUnit());
	}


	//initialize ground data
	m_pctrlHeight->SetValue(s_WPolar.m_Height*Units::mtoUnit());
	if(s_WPolar.m_bGround)
	{
		m_pctrlHeight->setEnabled(true);
		m_pctrlGroundEffect->setChecked(true);
	}
	else
	{
		m_pctrlHeight->setEnabled(false);
		m_pctrlGroundEffect->setChecked(false);
	}

	Units::getSpeedUnitLabel(str);
	m_pctrlSpeedUnit->setText(str);

	Units::getLengthUnitLabel(str);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit2->setText(str);
	m_pctrlLengthUnit3->setText(str);
	m_pctrlXCmRef->SetValue(s_WPolar.m_CoG.x*Units::mtoUnit());
	m_pctrlZCmRef->SetValue(s_WPolar.m_CoG.z*Units::mtoUnit());

	Units::getWeightUnitLabel(str1);
	m_pctrlWeightUnit->setText(str1);

	m_pctrlQInf->SetValue(s_WPolar.m_QInf*Units::mstoUnit());
	m_pctrlWeight->SetValue(s_WPolar.m_Mass*Units::kgtoUnit());
	m_pctrlBeta->SetValue(s_WPolar.m_Beta);
	m_pctrlAlpha->SetValue(s_WPolar.m_ASpec);


	m_pctrlViscous->setChecked(s_WPolar.m_bViscous);
	m_pctrlTiltGeom->setChecked(s_WPolar.m_bTiltedGeom);

	m_pctrlIgnoreBodyPanels->setChecked(m_pPlane && m_pPlane->body() && s_WPolar.m_bIgnoreBodyPanels);
	if(!m_pPlane) s_WPolar.m_bIgnoreBodyPanels=false;


	if(s_WPolar.m_AnalysisMethod==LLTMETHOD)
	{
		m_pctrlLLTMethod->setChecked(true);
		m_pctrlViscous->setChecked(true);
		m_pctrlViscous->setEnabled(false);
	}
	else if(s_WPolar.m_AnalysisMethod==VLMMETHOD)
	{
		m_pctrlVLMMethod->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}
	else if(s_WPolar.m_AnalysisMethod==PANELMETHOD)
	{
		if(s_WPolar.m_bThinSurfaces)  m_pctrlVLMMethod->setChecked(true);
		else                          m_pctrlPanelMethod->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}


	m_pctrlArea1->setChecked(s_WPolar.m_RefAreaType==1);
	m_pctrlArea2->setChecked(s_WPolar.m_RefAreaType==2);

	s_WPolar.m_bWakeRollUp = false;

	m_pctrlPlaneInertia->setChecked(s_WPolar.m_bAutoInertia);


	SetDensity();
	SetWingLoad();
	SetReynolds();
	SetWPolarName();

	EnableControls();


	m_pctrlQInf->setSelection(0,-1);
	m_pctrlQInf->setFocus();
}



void WPolarDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				ReadValues();
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


void WPolarDlg::OnArea()
{
	if(m_pctrlArea1->isChecked())
	{
		s_WPolar.m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_WPolar.m_RefAreaType = 2;
	}
	SetWPolarName();
}



void WPolarDlg::OnEditingFinished()
{
	ReadValues();
	SetReynolds();
	SetWPolarName();
}


void WPolarDlg::OnAutoName()
{
	m_bAutoName = m_pctrlAutoName->isChecked();
	if(m_bAutoName) SetWPolarName();
}


void WPolarDlg::OnTiltedGeom()
{
	s_WPolar.m_bTiltedGeom = m_pctrlTiltGeom->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnPlaneInertia()
{
	if(m_pctrlPlaneInertia->isChecked())
	{
		if(m_pPlane)
		{
			m_pctrlWeight->SetValue(m_pPlane->TotalMass() * Units::kgtoUnit());
			m_pctrlXCmRef->SetValue(m_pPlane->CoG().x * Units::mtoUnit());
			m_pctrlZCmRef->SetValue(m_pPlane->CoG().z * Units::mtoUnit());
			s_WPolar.m_Mass    = m_pPlane->TotalMass();
			s_WPolar.m_CoG.x   = m_pPlane->CoG().x;
			s_WPolar.m_CoG.z   = m_pPlane->CoG().z;
		}
	}
	else
	{
		s_WPolar.m_Mass    = m_pctrlWeight->Value() / Units::kgtoUnit();
		s_WPolar.m_CoG.x   = m_pctrlXCmRef->Value() / Units::mtoUnit();
		s_WPolar.m_CoG.z   = m_pctrlZCmRef->Value() / Units::mtoUnit();
	}
	s_WPolar.m_bAutoInertia = m_pctrlPlaneInertia->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnViscous()
{
	s_WPolar.m_bViscous = m_pctrlViscous->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnIgnoreBodyPanels()
{
	s_WPolar.m_bIgnoreBodyPanels = m_pctrlIgnoreBodyPanels->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnGroundEffect()
{
	s_WPolar.m_bGround = m_pctrlGroundEffect->isChecked();
	m_pctrlHeight->setEnabled(s_WPolar.m_bGround);
	SetWPolarName();
}


void WPolarDlg::OnMethod()
{
	if (m_pctrlLLTMethod->isChecked())
	{
		s_WPolar.m_bViscous      = true;
		s_WPolar.m_bThinSurfaces = true;
		s_WPolar.m_bWakeRollUp   = false;
		s_WPolar.m_bTiltedGeom   = false;
		s_WPolar.m_AnalysisMethod  = LLTMETHOD;
		m_pctrlTiltGeom->setChecked(false);
	}
	else if (m_pctrlVLMMethod->isChecked())
	{
		s_WPolar.m_bThinSurfaces = true;
		s_WPolar.m_AnalysisMethod = PANELMETHOD;
	}
	else if (m_pctrlPanelMethod->isChecked())
	{
		s_WPolar.m_bThinSurfaces = false;
		s_WPolar.m_AnalysisMethod = PANELMETHOD;
	}

	EnableControls();
	SetWPolarName();
}


void WPolarDlg::OnOK()
{

	if(!m_pctrlWPolarName->text().length())
	{
		QMessageBox::warning(this, tr("Warning"),tr("Must enter a name for the polar"));
		m_pctrlWPolarName->setFocus();
		return;
	}
	s_WPolar.m_WPlrName = m_pctrlWPolarName->text();

	if(qAbs(s_WPolar.m_Mass)<PRECISION && s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)
	{
		QMessageBox::warning(this, tr("Warning"),tr("Mass must be non-zero for type 2 polars"));
		m_pctrlWeight->setFocus();
		return;
	}
	if(!m_pPlane->isWing() && s_WPolar.m_AnalysisMethod==PANELMETHOD) s_WPolar.m_bThinSurfaces = true;

	accept();
}


void WPolarDlg::OnUnit()
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(s_WPolar.m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m"+QString::fromUtf8("²")+"/s");
	}
	else
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(s_WPolar.m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft"+QString::fromUtf8("²")+"/s");
	}
	SetDensity();
}


void WPolarDlg::OnWPolarName()
{
	m_bAutoName = false;
	m_pctrlAutoName->setChecked(false);
}



void WPolarDlg::OnWPolarType()
{
	if (m_pctrlType1->isChecked())
	{
		s_WPolar.m_WPolarType = FIXEDSPEEDPOLAR;
	}
	else if(m_pctrlType2->isChecked())
	{
		s_WPolar.m_WPolarType = FIXEDLIFTPOLAR;
	}
	else if(m_pctrlType4->isChecked())
	{
		s_WPolar.m_WPolarType = FIXEDAOAPOLAR;
	}
	EnableControls();
	SetReynolds();
	SetWPolarName();
}


void WPolarDlg::ReadValues()
{
	s_WPolar.m_ASpec     = m_pctrlAlpha->Value();
	s_WPolar.m_Beta      = m_pctrlBeta->Value();
	s_WPolar.m_Mass      = m_pctrlWeight->Value() / Units::kgtoUnit();
	s_WPolar.m_CoG.x     = m_pctrlXCmRef->Value() / Units::mtoUnit();
	s_WPolar.m_CoG.z     = m_pctrlZCmRef->Value() / Units::mtoUnit();
	s_WPolar.m_QInf      = m_pctrlQInf->Value() / Units::mstoUnit();
	s_WPolar.m_Height    = m_pctrlHeight->Value() / Units::mtoUnit();

	if(m_pctrlUnit1->isChecked())
	{
		s_WPolar.m_Viscosity = m_pctrlViscosity->Value();
		s_WPolar.m_Density   = m_pctrlDensity->Value();
	}
	else
	{
		s_WPolar.m_Density   = m_pctrlDensity->Value() / 0.00194122;
		s_WPolar.m_Viscosity = m_pctrlViscosity->Value() / 10.7182881;
	}

	if(m_pctrlArea1->isChecked())
	{
		s_WPolar.m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_WPolar.m_RefAreaType = 2;
	}

	SetDensity();

	SetWingLoad();
}



void WPolarDlg::SetDensity()
{
	int exp, precision;
	if(m_pctrlUnit1->isChecked())
	{
		exp = (int)log(s_WPolar.m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_WPolar.m_Density);
	}
	else
	{
		exp = (int)log(s_WPolar.m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(s_WPolar.m_Density* 0.00194122);
	}
}


void WPolarDlg::SetupLayout()
{
	QFont SymbolFont("Symbol");

	QGroupBox *NameGroup = new QGroupBox(tr("Polar Name"));
	{
		QVBoxLayout *NameLayout = new QVBoxLayout;
		{
			m_pctrlUFOName = new QLabel(tr("Wing Name"));
			m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
			m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
			NameLayout->addWidget(m_pctrlUFOName);
			NameLayout->addWidget(m_pctrlAutoName);
			NameLayout->addWidget(m_pctrlWPolarName);
		}
		NameGroup->setLayout(NameLayout);
	}

	QGroupBox *TypeGroup = new QGroupBox(tr("Polar Type"));
	{
		QVBoxLayout *TypeLayout = new QVBoxLayout;
		{
			m_pctrlType1 = new QRadioButton(tr("Type 1 (Fixed Speed)"));
			m_pctrlType2 = new QRadioButton(tr("Type 2 (Fixed Lift)"));
			m_pctrlType4 = new QRadioButton(tr("Type 4 (Fixed aoa)"));
			TypeLayout->addWidget(m_pctrlType1);
			TypeLayout->addWidget(m_pctrlType2);
			TypeLayout->addWidget(m_pctrlType4);
		}
		TypeGroup->setLayout(TypeLayout);
	}

	QGroupBox *FlightDataBox = new QGroupBox(tr("Plane and Flight Data"));
	{
		QGridLayout *PlaneLayout = new QGridLayout;
		{
			QLabel *lab1 = new QLabel(tr("Free Stream Speed ="));
			QLabel *lab5 = new QLabel("a =");
			QLabel *lab6 = new QLabel("b =");
			lab5->setFont(SymbolFont);
			lab6->setFont(SymbolFont);
			lab1->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			lab5->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			lab6->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			PlaneLayout->addWidget(lab1,1,1);
			PlaneLayout->addWidget(lab5,2,1);
			PlaneLayout->addWidget(lab6,3,1);
			m_pctrlQInf    = new DoubleEdit(10.05);
			m_pctrlQInf->SetMin(0.0);
			m_pctrlAlpha   = new DoubleEdit(1.00,2);
			m_pctrlBeta    = new DoubleEdit(0.00,2);
			PlaneLayout->addWidget(m_pctrlQInf,1,2);
			PlaneLayout->addWidget(m_pctrlAlpha,2,2);
			PlaneLayout->addWidget(m_pctrlBeta,3,2);
			m_pctrlSpeedUnit   = new QLabel("m/s");
			QLabel *lab7 = new QLabel(QString::fromUtf8("°"));
			QLabel *lab8 = new QLabel(QString::fromUtf8("°"));
			PlaneLayout->addWidget(m_pctrlSpeedUnit ,1,3);
			PlaneLayout->addWidget(lab7 ,2,3);
			PlaneLayout->addWidget(lab8 ,3,3);
		}
		FlightDataBox->setLayout(PlaneLayout);
	}

	QGroupBox *InertiaBox = new QGroupBox(tr("Inertia properties"));
	{
		QVBoxLayout *InertiaLayout = new QVBoxLayout;
		{
			m_pctrlPlaneInertia = new QCheckBox(tr("Use plane inertia"));
			m_pctrlPlaneInertia->setToolTip("Activate this checbox for the polar to use dynamically the plane's inertia properties for each analysis");

			QGridLayout *InertiaDataLayout = new QGridLayout;
			{
				QLabel *lab2 = new QLabel(tr("Plane Mass ="));
				QLabel *lab3 = new QLabel(tr("X_CoG ="));
				QLabel *lab4 = new QLabel(tr("Z_CoG ="));
				lab2->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				lab3->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				lab4->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				InertiaDataLayout->addWidget(lab2,1,1);
				InertiaDataLayout->addWidget(lab3,2,1);
				InertiaDataLayout->addWidget(lab4,3,1);
				m_pctrlWeight  = new DoubleEdit(1.234);
				m_pctrlWeight->SetPrecision(3);
				m_pctrlWeight->SetMin(0.0);
				m_pctrlXCmRef  = new DoubleEdit(100.00,3);
				m_pctrlZCmRef  = new DoubleEdit(100.00,3);
				InertiaDataLayout->addWidget(m_pctrlWeight,1,2);
				InertiaDataLayout->addWidget(m_pctrlXCmRef,2,2);
				InertiaDataLayout->addWidget(m_pctrlZCmRef,3,2);
				m_pctrlWeightUnit  = new QLabel("kg");
				m_pctrlLengthUnit1 = new QLabel("mm");
				m_pctrlLengthUnit3 = new QLabel("mm");
				InertiaDataLayout->addWidget(m_pctrlWeightUnit ,1,3);
				InertiaDataLayout->addWidget(m_pctrlLengthUnit1 ,2,3);
				InertiaDataLayout->addWidget(m_pctrlLengthUnit3 ,3,3);
			}
			InertiaLayout->addWidget(m_pctrlPlaneInertia);
			InertiaLayout->addLayout(InertiaDataLayout);
		}
		InertiaBox->setLayout(InertiaLayout);
	}

	QGroupBox *FlightGroup = new  QGroupBox(tr("Flight Characteristics"));
	{
		QVBoxLayout *FlightLayout = new QVBoxLayout;
		{
			m_pctrlWingLoad  = new QLabel(tr("Wing Loading = 0.033 kg/dm2"));
			m_pctrlSRe       = new QLabel(tr("SRe"));
			m_pctrlRRe       = new QLabel(tr("RRe"));
			m_pctrlQInfCl    = new QLabel(tr("QInfCl"));
			m_pctrlWingLoad->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlSRe->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlRRe->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlQInfCl->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			FlightLayout->addWidget(m_pctrlWingLoad);
			FlightLayout->addWidget(m_pctrlSRe);
			FlightLayout->addWidget(m_pctrlRRe);
			FlightLayout->addWidget(m_pctrlQInfCl);
		}
		FlightGroup->setLayout(FlightLayout);
	}

//_________________________Main Layout

	QGroupBox *pAnalysisMethods = new QGroupBox(tr("Analysis Methods"));
	{
		QVBoxLayout *pMethodLayout = new QVBoxLayout;
		{
			m_pctrlLLTMethod   = new QRadioButton(tr("LLT (Wing only)"));
			m_pctrlVLMMethod   = new QRadioButton(tr("VLM"));
			m_pctrlPanelMethod = new QRadioButton(tr("3D Panels"));

			pMethodLayout->addWidget(m_pctrlLLTMethod);
			pMethodLayout->addWidget(m_pctrlVLMMethod);
			pMethodLayout->addWidget(m_pctrlPanelMethod);
		}
		pAnalysisMethods->setLayout(pMethodLayout);

	}



	QGroupBox *AeroDataGroupBox = new QGroupBox(tr("Aerodynamic Data"));
	{
		QGridLayout *AeroDataLayout = new QGridLayout;
		{
			QLabel *lab9 = new QLabel(tr("Unit"));
			m_pctrlUnit1 = new QRadioButton(tr("International"));
			m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
			m_pctrlRho = new QLabel("r =");
			m_pctrlDensity = new DoubleEdit(1.225,3);
			m_pctrlDensityUnit = new QLabel("kg/m3");
			m_pctrlNu = new QLabel("n =");
			m_pctrlRho->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlNu->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlViscosity = new DoubleEdit(1.500e-5,3);
			m_pctrlViscosityUnit = new QLabel("m2/s");
			m_pctrlRho->setFont(SymbolFont);
			m_pctrlNu->setFont(SymbolFont);
			m_pctrlDensity->SetPrecision(5);
			m_pctrlViscosity->SetPrecision(3);
			m_pctrlDensity->SetMin(0.0);
			m_pctrlViscosity->SetMin(0.0);
			AeroDataLayout->addWidget(lab9,1,1);
			AeroDataLayout->addWidget(m_pctrlUnit1,1,2);
			AeroDataLayout->addWidget(m_pctrlUnit2,1,3);
			AeroDataLayout->addWidget(m_pctrlRho,2,1);
			AeroDataLayout->addWidget(m_pctrlDensity,2,2);
			AeroDataLayout->addWidget(m_pctrlDensityUnit,2,3);
			AeroDataLayout->addWidget(m_pctrlNu,3,1);
			AeroDataLayout->addWidget(m_pctrlViscosity,3,2);
			AeroDataLayout->addWidget(m_pctrlViscosityUnit,3,3);
			AeroDataLayout->setRowStretch(4,1);
			AeroDataLayout->setColumnStretch(1,1);
		}
		AeroDataGroupBox->setLayout(AeroDataLayout);
	}
	QGroupBox *OptionsGroupBox = new QGroupBox(tr("Options"));
	{
		QVBoxLayout *OptionsLayout = new QVBoxLayout;
		{
			m_pctrlViscous = new QCheckBox(tr("Viscous"));
			m_pctrlTiltGeom = new QCheckBox(tr("Tilt. Geom."));
			OptionsLayout->addWidget(m_pctrlViscous);
			OptionsLayout->addWidget(m_pctrlTiltGeom);
//			OptionsLayout->addWidget(m_pctrlIgnoreBody);
			m_pctrlIgnoreBodyPanels = new QCheckBox(tr("Ignore Body Panels"));
			OptionsLayout->addWidget(m_pctrlIgnoreBodyPanels);
		}
		OptionsGroupBox->setLayout(OptionsLayout);
	}

	QGroupBox *GroundGroupBox =  new QGroupBox(tr("Ground Effect"));
	{
		QVBoxLayout * pGroundLayout = new QVBoxLayout;
		{
			m_pctrlGroundEffect = new QCheckBox(tr("Ground Effect"));
			QHBoxLayout *pGroundHeightLayout = new QHBoxLayout;
			{
				QLabel *lab10 = new QLabel(tr("Height ="));
				m_pctrlHeight = new DoubleEdit(0.00,2);
				m_pctrlHeight->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				m_pctrlLengthUnit2 = new QLabel("mm");
				pGroundHeightLayout->addWidget(lab10);
				pGroundHeightLayout->addWidget(m_pctrlHeight);
				pGroundHeightLayout->addWidget(m_pctrlLengthUnit2);
			}
			pGroundLayout->addWidget(m_pctrlGroundEffect);
			pGroundLayout->addLayout(pGroundHeightLayout);
			pGroundLayout->addStretch(1);
		}
		GroundGroupBox->setLayout(pGroundLayout);
	}


	QGroupBox *AreaBox = new QGroupBox(tr("Reference Area and Span for Aero Coefficients"));
	{
		QVBoxLayout *AreaOptions = new QVBoxLayout;
		{
			m_pctrlArea1 = new QRadioButton(tr("Wing Planform"));
			m_pctrlArea2 = new QRadioButton(tr("Wing Planform projected on xy plane"));
			AreaOptions->addWidget(m_pctrlArea1);
			AreaOptions->addWidget(m_pctrlArea2);
		}
		AreaBox->setLayout(AreaOptions);
	}

	QGridLayout *DataLayout = new QGridLayout;
	{
		DataLayout->addWidget(NameGroup,1,1);
		DataLayout->addWidget(FlightDataBox,2,1);
		DataLayout->addWidget(InertiaBox,3,1);
		DataLayout->addWidget(pAnalysisMethods,4,1);
		DataLayout->addWidget(OptionsGroupBox,5,1);
		DataLayout->addWidget(TypeGroup, 1,2);
		DataLayout->addWidget(FlightGroup,2,2);
		DataLayout->addWidget(AeroDataGroupBox,3,2);
		DataLayout->addWidget(GroundGroupBox,4,2);
		DataLayout->addWidget(AreaBox,5,2);
	}

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
		MainLayout->addLayout(DataLayout);
		MainLayout->addStretch(1);
		MainLayout->addLayout(CommandButtons);
		MainLayout->addStretch(1);
	}
	setLayout(MainLayout);
}



void WPolarDlg::SetWPolarName()
{
	if(!m_bAutoName) return;
	QString str, strong;
	QString WPolarName;

	if (s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)
	{
		Units::getSpeedUnitLabel(str);
		WPolarName = QString("T1-%1 ").arg(s_WPolar.m_QInf * Units::mstoUnit(),0,'f',1);
		WPolarName += str;
	}
	else if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)
	{
		WPolarName = QString("T2");
	}
	else if(s_WPolar.m_WPolarType==FIXEDAOAPOLAR)
	{
		WPolarName = QString(QString::fromUtf8("T4-%1°")).arg(s_WPolar.m_ASpec,0,'f',3);
	}

	if(s_WPolar.m_AnalysisMethod==LLTMETHOD) WPolarName += "-LLT";
	else if(s_WPolar.m_AnalysisMethod==VLMMETHOD)
	{
		if(QMiarex::s_bVLM1) WPolarName += "-VLM1";
		else		           WPolarName += "-VLM2";
	}
	else if(s_WPolar.m_AnalysisMethod==PANELMETHOD)
	{
		if(m_pPlane->isWing() && !s_WPolar.m_bThinSurfaces) WPolarName += "-Panel";
		if(s_WPolar.m_bThinSurfaces)
		{
			if(QMiarex::s_bVLM1) WPolarName += "-VLM1";
			else		           WPolarName += "-VLM2";
		}
	}

	if(!s_WPolar.m_bAutoInertia)
	{
		Units::getWeightUnitLabel(str);
		strong = QString("-%1").arg(s_WPolar.m_Mass*Units::kgtoUnit(),0,'f',3);
		if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)   WPolarName += strong+str;
		Units::getLengthUnitLabel(str);
		strong = QString("-x%1").arg(s_WPolar.m_CoG.x*Units::mtoUnit(),0,'f',3);
		WPolarName += strong + str;

		if(qAbs(s_WPolar.m_CoG.z)>=.000001)
		{
			strong = QString("-z%1").arg(s_WPolar.m_CoG.z*Units::mtoUnit(),0,'f',3);
			WPolarName += strong + str;
		}
	}
//	else WPolarName += "-Plane_Inertia";


	if(qAbs(s_WPolar.m_Beta) > .001)
	{
		strong = QString(QString::fromUtf8("-b%1°")).arg(s_WPolar.m_Beta,0,'f',2);
		WPolarName += strong;
	}

	if(s_WPolar.m_bTiltedGeom)
	{
		WPolarName += "-TG";
		if(s_WPolar.m_bWakeRollUp) WPolarName += "-W";
	}

	if(!s_WPolar.m_bViscous)
	{
		WPolarName += "-Inviscid";
	}

	if(s_WPolar.m_bIgnoreBodyPanels && m_pPlane && m_pPlane->body())
    {
		WPolarName += "-NoBodyPanels";
    }

	if(s_WPolar.m_bGround)
	{
		strong = QString("%1").arg(s_WPolar.m_Height,0,'f',2),
		WPolarName += "-G"+strong;
	}
	if(s_WPolar.m_AnalysisMethod!=1 && s_WPolar.m_RefAreaType==PROJECTEDAREA) WPolarName += "-proj_area";

	m_pctrlWPolarName->setText(WPolarName);
}



void WPolarDlg::SetReynolds()
{
	QString strange, str, strUnit;
	Units::getSpeedUnitLabel(strUnit);

	if(s_WPolar.m_WPolarType ==FIXEDSPEEDPOLAR)
	{
		double RRe = m_pPlane->rootChord() * s_WPolar.m_QInf/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, RRe);
		strange = tr("Root Re =");
		m_pctrlRRe->setText(strange+str);

		double SRe = m_pPlane->tipChord() * s_WPolar.m_QInf/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, SRe);
		strange = tr("Tip Re =");
		m_pctrlSRe->setText(strange+str);

		m_pctrlQInfCl->setText(" ");
	}
	else if(s_WPolar.m_WPolarType ==FIXEDLIFTPOLAR)
	{
		double area;
		if (s_WPolar.m_RefAreaType==1) area = m_pPlane->planformArea();
		else                           area = m_pPlane->projectedArea();
		double QCl =  sqrt(2.* 9.81 /s_WPolar.m_Density* s_WPolar.m_Mass /area) * Units::mstoUnit();
		str = QString("%1").arg(QCl,5,'f',2);
		str += strUnit;
		strange = tr("Vinf.sqrt(Cl) =");
		m_pctrlQInfCl->setText(strange+str);

		double RRe = m_pPlane->rootChord() * QCl/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, RRe);
		strange = tr("Root Re.sqrt(Cl) =");
		m_pctrlRRe->setText(strange+str);

		double SRe = m_pPlane->tipChord() * QCl/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, SRe);
		strange = tr("Tip Re.sqrt(Cl) =");
		m_pctrlSRe->setText(strange+str);
	}
	else if(s_WPolar.m_WPolarType ==FIXEDAOAPOLAR)
	{
		m_pctrlQInfCl->setText(" ");
		m_pctrlRRe->setText(" ");
		m_pctrlSRe->setText(" ");
	}
}




void WPolarDlg::SetWingLoad()
{
	QString str,str1, str2;
	double area;
	if (s_WPolar.m_RefAreaType==1) area = m_pPlane->planformArea();
	else                           area = m_pPlane->projectedArea();
	m_WingLoad = s_WPolar.m_Mass/area;//kg/dm2

	str = QString("%1").arg(m_WingLoad * Units::kgtoUnit() / Units::m2toUnit(),7,'f',3);

	Units::getWeightUnitLabel(str1);
	Units::getAreaUnitLabel(str2);
	m_pctrlWingLoad->setText(tr("Wing Loading = ")+str+str1+"/"+str2);
}













