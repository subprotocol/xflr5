/****************************************************************************

	WPolarDlg Class
	Copyright (C) 2009-2010 Andre Deperrois XFLR5@yahoo.com

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

#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/WPolar.h"
#include "WPolarDlg.h"
#include "Miarex.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <math.h>
#include <QtDebug>

WPolarDlg::WPolarDlg()
{
	setWindowTitle(tr("Analysis Definition"));
	m_pMainFrame = NULL;
	m_pMiarex    = NULL;

	m_pWing      = NULL;
	m_pPlane     = NULL;

	m_WPolarName = "WPolar Name";

	m_bAutoName = true;
	
	m_QInf       = 10.0;//m/s
	m_Weight     = 0.0;
	m_Alpha      = 0.0;
	m_Beta       = 0.0;
	m_PolarType  = FIXEDSPEEDPOLAR;
	m_WingLoad   = 0.0;
	m_Density    = 1.225;
	m_Viscosity  = 1.5e-5;
	m_Height     = 0.0;
	m_pWing	     = NULL;
	m_pPlane     = NULL;

	m_CoG.Set(0.0,0.0,0.0);
	m_AnalysisMethod = LLTMETHOD;

	m_bThinSurfaces = true;
	m_bTiltedGeom   = false;
	m_bWakeRollUp   = false;
	m_bViscous      = true;
	m_bGround       = false;
	m_bPlaneInertia = false;

	m_NXWakePanels    = 1;
	m_TotalWakeLength = 100.0;//x mac
	m_WakePanelFactor = 1.1;

	m_RefAreaType = 1;

//	m_SymbolFont.CreatePointFont(100, "Symbol");
	m_UnitType  = 1;

	SetupLayout();
	Connect();
}

void WPolarDlg::Connect()
{
	connect(m_pctrlWingMethod1, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlWingMethod2, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlWingMethod3, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));

	connect(m_pctrlUnit1, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));

	connect(m_pctrlType1, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));
	connect(m_pctrlType2, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));
	connect(m_pctrlType4, SIGNAL(toggled(bool)), this, SLOT(OnWPolarType()));

	connect(m_pctrlAutoName, SIGNAL(clicked()), this, SLOT(OnAutoName()));
//	connect(m_pctrlWakeRollUp, SIGNAL(clicked()), this, SLOT(OnWakeRollUp()));
	connect(m_pctrlTiltGeom, SIGNAL(clicked()), this, SLOT(OnTiltedGeom()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlGroundEffect, SIGNAL(clicked()), this, SLOT(OnGroundEffect()));
	connect(m_pctrlPlaneInertia, SIGNAL(clicked()), this, SLOT(OnPlaneInertia()));

	connect(m_pctrlXCmRef, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlZCmRef, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlDensity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlAlpha, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlBeta, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWeight, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlQInf, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlHeight, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(textEdited ( const QString &  )), this, SLOT(OnWPolarName()));

	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void WPolarDlg::EnableControls()
{
	switch (m_PolarType)
	{
		case 1:
		{
			m_pctrlQInf->setEnabled(true);
			m_pctrlAlpha->setEnabled(false);
			break;
		}
		case 2:
		{
			m_pctrlQInf->setEnabled(false);
			m_pctrlRRe->setText(" ");
			m_pctrlSRe->setText(" ");
			m_pctrlAlpha->setEnabled(false);
			break;
		}
		case 4:
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

	m_pctrlViscous->setEnabled(m_AnalysisMethod==PANELMETHOD);
	m_pctrlTiltGeom->setEnabled(m_AnalysisMethod==PANELMETHOD);
	m_pctrlBeta->setEnabled(m_AnalysisMethod==PANELMETHOD);
	m_pctrlGroundEffect->setEnabled(m_AnalysisMethod==PANELMETHOD);
	m_pctrlHeight->setEnabled(m_pctrlGroundEffect->isChecked() && m_AnalysisMethod==PANELMETHOD);

	m_pctrlWeight->setEnabled(!m_bPlaneInertia);
	m_pctrlXCmRef->setEnabled(!m_bPlaneInertia);
	m_pctrlZCmRef->setEnabled(!m_bPlaneInertia);
}



void WPolarDlg::InitDialog()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, str1;

	int i;

	if(m_pPlane)
	{
		if(m_AnalysisMethod==0 || m_AnalysisMethod==1) m_AnalysisMethod=VLMMETHOD;
//		m_pctrlWingMethod1->setEnabled(false);
		m_pctrlAnalysisControls->setCurrentIndex(1);
	}
	else m_pctrlAnalysisControls->setCurrentIndex(0);

	bool bWarning = false;
	for (i=0; i<m_pWing->m_NPanel-1; i++)
	{
		if(m_pWing->m_NXPanels[i]!= m_pWing->m_NXPanels[i+1])
			{
				QMessageBox::warning(this, tr("Warning"),tr("The number of chordwise VLM-panels across the span is not uniform\n Panel method is disabled"));
				bWarning = true;
				break;
			}
	}

	m_bAutoName = true;
	m_pctrlAutoName->setChecked(true);
	
	m_pctrlQInf->SetValue(m_QInf);
	m_pctrlWeight->SetValue(m_Weight);


	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);

	if(m_PolarType==1) m_pctrlType1->setChecked(true);
	else if(m_PolarType==2) m_pctrlType2->setChecked(true);
	else if(m_PolarType==4) m_pctrlType4->setChecked(true);


	OnUnit();

	m_pctrlHeight->SetValue(m_Height*pMainFrame->m_mtoUnit);

	if(m_bGround)
	{
		m_pctrlHeight->setEnabled(true);
		m_pctrlGroundEffect->setChecked(true);
	}
	else
	{
		m_pctrlHeight->setEnabled(false);
		m_pctrlGroundEffect->setChecked(false);
	}

	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	m_pctrlSpeedUnit->setText(str);

	SetWingLoad();

	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit2->setText(str);
	m_pctrlLengthUnit3->setText(str);
	m_pctrlXCmRef->SetValue(m_CoG.x*pMainFrame->m_mtoUnit);
	m_pctrlZCmRef->SetValue(m_CoG.z*pMainFrame->m_mtoUnit);

	GetWeightUnit(str1, pMainFrame->m_WeightUnit);
	m_pctrlWeightUnit->setText(str1);

	m_pctrlQInf->SetValue(m_QInf*pMainFrame->m_mstoUnit);
	m_pctrlWeight->SetValue(m_Weight*pMainFrame->m_kgtoUnit);

	m_pctrlBeta->SetValue(m_Beta);

	m_pctrlAlpha->SetValue(m_Alpha);

	SetWingLoad();
	SetReynolds();

	if(bWarning)
	{
//		m_pctrlWingMethod4->setEnabled(false);
		if(m_AnalysisMethod==3) m_AnalysisMethod=2;
	}

	if(m_bViscous)		m_pctrlViscous->setChecked(true);
	if(m_bTiltedGeom)	m_pctrlTiltGeom->setChecked(true);
//	if(m_bWakeRollUp) 	m_pctrlWakeRollUp->setChecked(true);

	OnTiltedGeom();

	if(m_AnalysisMethod==0) m_AnalysisMethod=VLMMETHOD;//former m_bLLT=false;
	if(m_AnalysisMethod==LLTMETHOD)
	{
		m_pctrlWingMethod1->setChecked(true);
		m_pctrlViscous->setChecked(true);
		m_pctrlViscous->setEnabled(false);
	}
	else if(m_AnalysisMethod==VLMMETHOD)
	{
		m_pctrlWingMethod2->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}
	else if(m_AnalysisMethod==PANELMETHOD)
	{
		if(!m_pPlane)
		{
			if(m_bThinSurfaces)  m_pctrlWingMethod2->setChecked(true);
			else                 m_pctrlWingMethod3->setChecked(true);
		}
		m_pctrlPanelMethod->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}

	if(m_pPlane) m_bThinSurfaces = true;

	m_pctrlPanelMethod->setChecked(m_pPlane);

	m_pctrlArea1->setChecked(m_RefAreaType==1);
	m_pctrlArea2->setChecked(m_RefAreaType==2);

	m_bWakeRollUp = false;


	SetWPolarName();


	m_pctrlWingName->setText(m_UFOName);
//	CheckType();
//	EnableControls();

	OnMethod();

	m_pctrlPlaneInertia->setChecked(m_bPlaneInertia);
	OnPlaneInertia();

//	m_pctrlThinSurfaces->setEnabled(false);
//	if(m_bThinSurfaces)	m_pctrlThinSurfaces.SetCheck(true);
//	else                m_pctrlThinSurfaces.SetCheck(false);

//	m_WakeParamsdlg.m_NXWakePanels    = m_NXWakePanels;
//	m_WakeParamsdlg.m_TotalWakeLength = m_TotalWakeLength;
//	m_WakeParamsdlg.m_WakePanelFactor = m_WakePanelFactor;

	m_pctrlQInf->setSelection(0,-1);
	m_pctrlQInf->setFocus();
}



void WPolarDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
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
		m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		m_RefAreaType = 2;
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
	m_bTiltedGeom = m_pctrlTiltGeom->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnPlaneInertia()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_pctrlPlaneInertia->isChecked())
	{
		if(m_pPlane)
		{
			m_pctrlWeight->SetValue(m_pPlane->GetTotalMass() * pMainFrame->m_kgtoUnit);
			m_pctrlXCmRef->SetValue(m_pPlane->m_CoG.x * pMainFrame->m_mtoUnit);
			m_pctrlZCmRef->SetValue(m_pPlane->m_CoG.z * pMainFrame->m_mtoUnit);
			m_Weight    = m_pPlane->GetTotalMass();
			m_CoG.x     = m_pPlane->m_CoG.x;
			m_CoG.z     = m_pPlane->m_CoG.z;
		}
		else if(m_pWing)
		{
			m_pctrlWeight->SetValue(m_pWing->GetTotalMass() * pMainFrame->m_kgtoUnit);
			m_pctrlXCmRef->SetValue(m_pWing->m_CoG.x * pMainFrame->m_mtoUnit);
			m_pctrlZCmRef->SetValue(m_pWing->m_CoG.z * pMainFrame->m_mtoUnit);
			m_Weight    = m_pWing->GetTotalMass();
			m_CoG.x     = m_pWing->m_CoG.x;
			m_CoG.z     = m_pWing->m_CoG.z;
		}
	}
	else
	{
		m_Weight    = m_pctrlWeight->GetValue() / pMainFrame->m_kgtoUnit;
		m_CoG.x     = m_pctrlXCmRef->GetValue() / pMainFrame->m_mtoUnit;
		m_CoG.z     = m_pctrlZCmRef->GetValue() / pMainFrame->m_mtoUnit;
	}
	m_bPlaneInertia = m_pctrlPlaneInertia->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnViscous()
{
	m_bViscous = m_pctrlViscous->isChecked();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnGroundEffect()
{
	m_bGround = m_pctrlGroundEffect->isChecked();
	m_pctrlHeight->setEnabled(m_bGround);
	SetWPolarName();
}


void WPolarDlg::OnMethod()
{
	if (m_pctrlWingMethod1->isChecked())
	{
		m_bViscous      = true;
		m_bThinSurfaces = true;
		m_bWakeRollUp   = false;
		m_bTiltedGeom   = false;
		m_AnalysisMethod  = LLTMETHOD;
		m_pctrlTiltGeom->setChecked(false);
	}
	else if (m_pctrlWingMethod2->isChecked())
	{
		m_bThinSurfaces = true;
		m_AnalysisMethod = PANELMETHOD;
	}
	else if (m_pctrlWingMethod3->isChecked())
	{
		m_bThinSurfaces = false;
		m_AnalysisMethod = PANELMETHOD;
	}

	EnableControls();
	SetWPolarName();
}


void WPolarDlg::OnOK()
{
	CWPolar * pWPolarNew;

	m_WPolarName = m_pctrlWPolarName->text();

	int LineLength = m_WPolarName.length();
	if(!LineLength)
	{
		QMessageBox::warning(this, tr("Warning"),tr("Must enter a name for the polar"));
		m_pctrlWPolarName->setFocus();
		return;
	}
	else
	{
		int size = (int)m_poaWPolar->size();
		for (int j=0; j<size; j++)
		{
			pWPolarNew = (CWPolar*)m_poaWPolar->at(j);
			if (pWPolarNew->m_PlrName == m_WPolarName &&pWPolarNew->m_UFOName  == m_UFOName)
			{
				QMessageBox::warning(this, tr("Warning"),tr("The polar's name already exists"));
				return;
			}
		}
	}


	if(m_pPlane && m_AnalysisMethod==3) m_bThinSurfaces = true;

	accept();
}


void WPolarDlg::OnUnit()
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m2/s");
	}
	else
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft2/s");
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
		m_PolarType = 1;
	}
	else if(m_pctrlType2->isChecked())
	{
		m_PolarType = 2;
	}
	else if(m_pctrlType4->isChecked())
	{
		m_PolarType = 4;
	}
	EnableControls();
	SetReynolds();
	SetWPolarName();
}


void WPolarDlg::ReadValues()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	m_Alpha     = m_pctrlAlpha->GetValue();
	m_Beta      = m_pctrlBeta->GetValue();
	m_Weight    = m_pctrlWeight->GetValue() / pMainFrame->m_kgtoUnit;
	m_CoG.x     = m_pctrlXCmRef->GetValue() / pMainFrame->m_mtoUnit;
	m_CoG.z     = m_pctrlZCmRef->GetValue() / pMainFrame->m_mtoUnit;
	m_QInf      = m_pctrlQInf->GetValue() / pMainFrame->m_mstoUnit;
	m_Height    = m_pctrlHeight->GetValue() / pMainFrame->m_mtoUnit;

	if(m_pctrlUnit1->isChecked())
	{
		m_Viscosity = m_pctrlViscosity->GetValue();
		m_Density   = m_pctrlDensity->GetValue();
	}
	else
	{
		m_Density   = m_pctrlDensity->GetValue() / 0.00194122;
		m_Viscosity = m_pctrlViscosity->GetValue() / 10.7182881;
	}

	if(m_pctrlArea1->isChecked())
	{
		m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		m_RefAreaType = 2;
	}

	SetDensity();

	SetWingLoad();


}



void WPolarDlg::SetDensity()
{
	int exp, precision;
	if(m_pctrlUnit1->isChecked())
	{
		exp = (int)log(m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(m_Density);
	}
	else
	{
		exp = (int)log(m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(m_Density* 0.00194122);
	}
}


void WPolarDlg::SetupLayout()
{
	QFont SymbolFont("Symbol");

	QGroupBox *NameGroup = new QGroupBox(tr("Polar Name"));
	{
		QVBoxLayout *NameLayout = new QVBoxLayout;
		m_pctrlWingName = new QLabel(tr("Wing Name"));
		m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
		m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
		NameLayout->addWidget(m_pctrlWingName);
		NameLayout->addWidget(m_pctrlAutoName);
		NameLayout->addWidget(m_pctrlWPolarName);
		NameGroup->setLayout(NameLayout);
	}

	QGroupBox *TypeGroup = new QGroupBox(tr("Polar Type"));
	{
		QVBoxLayout *TypeLayout = new QVBoxLayout;
		m_pctrlType1 = new QRadioButton(tr("Type 1 (Fixed Speed)"));
		m_pctrlType2 = new QRadioButton(tr("Type 2 (Fixed Lift)"));
		m_pctrlType4 = new QRadioButton(tr("Type 4 (Fixed aoa)"));
		TypeLayout->addWidget(m_pctrlType1);
		TypeLayout->addWidget(m_pctrlType2);
		TypeLayout->addWidget(m_pctrlType4);
		TypeGroup->setLayout(TypeLayout);
	}

	QGroupBox *FlightDataBox = new QGroupBox(tr("Plane and Flight Data"));
	{
		QGridLayout *PlaneLayout = new QGridLayout;
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
		m_pctrlQInf    = new FloatEdit(10.05);
		m_pctrlQInf->SetMin(0.0);
		m_pctrlAlpha   = new FloatEdit(1.00,2);
		m_pctrlBeta    = new FloatEdit(0.00,2);
		PlaneLayout->addWidget(m_pctrlQInf,1,2);
		PlaneLayout->addWidget(m_pctrlAlpha,2,2);
		PlaneLayout->addWidget(m_pctrlBeta,3,2);
		m_pctrlSpeedUnit   = new QLabel("m/s");
		QLabel *lab7 = new QLabel(QString::fromUtf8("°"));
		QLabel *lab8 = new QLabel(QString::fromUtf8("°"));
		PlaneLayout->addWidget(m_pctrlSpeedUnit ,1,3);
		PlaneLayout->addWidget(lab7 ,2,3);
		PlaneLayout->addWidget(lab8 ,3,3);
		FlightDataBox->setLayout(PlaneLayout);
	}

	QGroupBox *InertiaBox = new QGroupBox(tr("Inertia properties"));
	{
		m_pctrlPlaneInertia = new QCheckBox(tr("Use plane inertia"));
		m_pctrlPlaneInertia->setToolTip("Activate this checbox for the polar to use dynamically the plane's inertia properties for each analysis");
		QGridLayout *InertiaDataLayout = new QGridLayout;
		QLabel *lab2 = new QLabel(tr("Plane Mass ="));
		QLabel *lab3 = new QLabel(tr("X_CoG ="));
		QLabel *lab4 = new QLabel(tr("Z_CoG ="));
		lab2->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		lab3->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		lab4->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		InertiaDataLayout->addWidget(lab2,1,1);
		InertiaDataLayout->addWidget(lab3,2,1);
		InertiaDataLayout->addWidget(lab4,3,1);
		m_pctrlWeight  = new FloatEdit(1.234);
		m_pctrlWeight->SetPrecision(3);
		m_pctrlWeight->SetMin(0.0);
		m_pctrlXCmRef  = new FloatEdit(100.00,2);
		m_pctrlZCmRef  = new FloatEdit(100.00,2);
		InertiaDataLayout->addWidget(m_pctrlWeight,1,2);
		InertiaDataLayout->addWidget(m_pctrlXCmRef,2,2);
		InertiaDataLayout->addWidget(m_pctrlZCmRef,3,2);
		m_pctrlWeightUnit  = new QLabel("kg");
		m_pctrlLengthUnit1 = new QLabel("mm");
		m_pctrlLengthUnit3 = new QLabel("mm");
		InertiaDataLayout->addWidget(m_pctrlWeightUnit ,1,3);
		InertiaDataLayout->addWidget(m_pctrlLengthUnit1 ,2,3);
		InertiaDataLayout->addWidget(m_pctrlLengthUnit3 ,3,3);
		QVBoxLayout *InertiaLayout = new QVBoxLayout;
		InertiaLayout->addWidget(m_pctrlPlaneInertia);
		InertiaLayout->addLayout(InertiaDataLayout);
		InertiaBox->setLayout(InertiaLayout);
	}

	QGroupBox *FlightGroup = new  QGroupBox(tr("Flight Characteristics"));
	{
		QVBoxLayout *FlightLayout = new QVBoxLayout;
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

		FlightGroup->setLayout(FlightLayout);
	}

//_________________________Main Layout
	m_pctrlAnalysisControls = new QStackedWidget;
	{
		QVBoxLayout *WingMethodLayout = new QVBoxLayout;
		m_pctrlWingMethod1 = new QRadioButton(tr("LLT"));
		m_pctrlWingMethod2 = new QRadioButton(tr("VLM"));
		m_pctrlWingMethod3 = new QRadioButton(tr("3D Panels"));

		WingMethodLayout->addWidget(m_pctrlWingMethod1);
		WingMethodLayout->addWidget(m_pctrlWingMethod2);
		WingMethodLayout->addWidget(m_pctrlWingMethod3);
		QGroupBox *WingMethodBox = new QGroupBox("Wing analysis methods");
		WingMethodBox->setLayout(WingMethodLayout);

		m_pctrlPanelMethod = new QRadioButton(tr("Mix 3D Panels/VLM"));
		QHBoxLayout *PlaneMethodLayout = new QHBoxLayout;
		PlaneMethodLayout->addWidget(m_pctrlPanelMethod);
		QGroupBox *PlaneMethodBox = new QGroupBox("Plane analysis methods");
		PlaneMethodBox->setLayout(PlaneMethodLayout);

		m_pctrlAnalysisControls->addWidget(WingMethodBox);
		m_pctrlAnalysisControls->addWidget(PlaneMethodBox);
	}

	QGroupBox *AeroDataGroup = new QGroupBox(tr("Aerodynamic Data"));
	{
		QGridLayout *AeroDataLayout = new QGridLayout;
		QLabel *lab9 = new QLabel(tr("Unit"));
		m_pctrlUnit1 = new QRadioButton(tr("International"));
		m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
		m_pctrlRho = new QLabel("r =");
		m_pctrlDensity = new FloatEdit(1.500e-5,3);
		m_pctrlDensityUnit = new QLabel("kg/m3");
		m_pctrlNu = new QLabel("n =");
		m_pctrlRho->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlNu->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlViscosity = new FloatEdit(1.225,3);
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
		AeroDataGroup->setLayout(AeroDataLayout);
	}

	QGroupBox *OptionsGroup = new QGroupBox(tr("Options"));
	{
		QVBoxLayout *OptionsLayout = new QVBoxLayout;
		m_pctrlViscous = new QCheckBox(tr("Viscous"));
		m_pctrlTiltGeom = new QCheckBox(tr("Tilt. Geom."));
		OptionsLayout->addWidget(m_pctrlViscous);
		OptionsLayout->addWidget(m_pctrlTiltGeom);
		OptionsGroup->setLayout(OptionsLayout);
	}

	QGroupBox *GroundGroup =  new QGroupBox(tr("Ground Effect"));
	{
		QGridLayout * GroundLayout =new QGridLayout;
		m_pctrlGroundEffect = new QCheckBox(tr("Ground Effect"));
		QLabel *lab10 = new QLabel(tr("Height ="));
		m_pctrlHeight = new FloatEdit(0.00,2);
		m_pctrlLengthUnit2 = new QLabel("mm");
		GroundLayout->addWidget(m_pctrlGroundEffect,1,1);
		GroundLayout->addWidget(lab10,2,1);
		GroundLayout->addWidget(m_pctrlHeight,2,2);
		GroundLayout->addWidget(m_pctrlLengthUnit2,2,3);
		GroundGroup->setLayout(GroundLayout);
	}


	QGroupBox *AreaBox = new QGroupBox(tr("Reference Area and Span for Aero Coefficients"));
	{
		QVBoxLayout *AreaOptions = new QVBoxLayout;
		m_pctrlArea1 = new QRadioButton(tr("Wing Planform"));
		m_pctrlArea2 = new QRadioButton(tr("Wing Planform projected on xy plane"));
		AreaOptions->addWidget(m_pctrlArea1);
		AreaOptions->addWidget(m_pctrlArea2);
		AreaBox->setLayout(AreaOptions);
	}

	QGridLayout *DataLayout = new QGridLayout;
	DataLayout->addWidget(NameGroup,1,1);
	DataLayout->addWidget(FlightDataBox,2,1);
	DataLayout->addWidget(InertiaBox,3,1);
	DataLayout->addWidget(m_pctrlAnalysisControls,4,1);
	DataLayout->addWidget(OptionsGroup,5,1);
	DataLayout->addWidget(TypeGroup, 1,2);
	DataLayout->addWidget(FlightGroup,2,2);
	DataLayout->addWidget(AeroDataGroup,3,2);
	DataLayout->addWidget(GroundGroup,4,2);
	DataLayout->addWidget(AreaBox,5,2);


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setDefault(true);
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addLayout(DataLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
}



void WPolarDlg::SetWPolarName()
{
	if(!m_bAutoName) return;
	QString str, strong;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	if (m_PolarType==FIXEDSPEEDPOLAR)
	{
		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		m_WPolarName = QString("T1-%1 ").arg(m_QInf * pMainFrame->m_mstoUnit,0,'f',1);
		m_WPolarName += str;
	}
	else if(m_PolarType==FIXEDLIFTPOLAR)
	{
		m_WPolarName = QString("T2");
	}
	else if(m_PolarType==FIXEDAOAPOLAR)
	{
		m_WPolarName = QString(QString::fromUtf8("T4-%1°")).arg(m_Alpha,0,'f',3);
	}

	if(m_AnalysisMethod==LLTMETHOD) m_WPolarName += "-LLT";
	else if(m_AnalysisMethod==VLMMETHOD)
	{
		if(pMiarex->m_bVLM1) m_WPolarName += "-VLM1";
		else		         m_WPolarName += "-VLM2";
	}
	else if(m_AnalysisMethod==PANELMETHOD)
	{
		if(m_pPlane || !m_bThinSurfaces) m_WPolarName += "-Panel";
		if(m_bThinSurfaces)
		{
			if(pMiarex->m_bVLM1) m_WPolarName += "-VLM1";
			else		         m_WPolarName += "-VLM2";
		}
	}

/*	if(m_bThinSurfaces && m_AnalysisMethod==3)
	{
		m_WPolarName += "-Thin";
	}*/

	if(!m_bPlaneInertia)
	{
		GetWeightUnit(str, pMainFrame->m_WeightUnit);
		strong = QString("-%1 ").arg(m_Weight*pMainFrame->m_kgtoUnit,0,'f',3);
		m_WPolarName += strong+str;
		GetLengthUnit(str, pMainFrame->m_LengthUnit);
		strong = QString("-x%1").arg(m_CoG.x*pMainFrame->m_mtoUnit,0,'f',3);
		m_WPolarName += strong + str;

		if(fabs(m_CoG.z)>=.000001)
		{
			strong = QString("-z%1").arg(m_CoG.z*pMainFrame->m_mtoUnit,0,'f',3);
			m_WPolarName += strong + str;
		}
	}
	else
	{
		m_WPolarName += "-Plane_Inertia";
	}

	if(fabs(m_Beta) > .001)
	{
		strong = QString(QString::fromUtf8("-b%1°")).arg(m_Beta,0,'f',2);
		m_WPolarName += strong;
	}

	if(m_bTiltedGeom)
	{
		m_WPolarName += "-TG";
		if(m_bWakeRollUp) m_WPolarName += "-W";
	}

	if(!m_bViscous)
	{
		m_WPolarName += "-Inviscid";
	}

	if(m_bGround)
	{
		strong = QString("%1").arg(m_Height,0,'f',2),
		m_WPolarName += "-G"+strong;
	}
	if(m_AnalysisMethod!=1 && m_RefAreaType==PROJECTEDAREA) m_WPolarName += "-proj_area";

	m_pctrlWPolarName->setText(m_WPolarName);
}



void WPolarDlg::SetReynolds()
{
	QString strange, str, strUnit;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	GetSpeedUnit(strUnit, pMainFrame->m_SpeedUnit);

	if (m_PolarType ==FIXEDSPEEDPOLAR)
	{
		double RRe = m_pWing->m_TChord[0] * m_QInf/m_Viscosity;
		ReynoldsFormat(str, RRe);
		strange = tr("Root Re =");
		m_pctrlRRe->setText(strange+str);

		double SRe = m_pWing->m_TChord[m_pWing->m_NPanel] * m_QInf/m_Viscosity;
		ReynoldsFormat(str, SRe);
		strange = tr("Tip Re =");
		m_pctrlSRe->setText(strange+str);

		m_pctrlQInfCl->setText(" ");
	}
	else if (m_PolarType ==FIXEDLIFTPOLAR)
	{
		double area;
		if (m_RefAreaType==1) area =m_pWing->m_PlanformArea;
		else area = m_pWing->m_ProjectedArea;
		double QCl =  sqrt(2.* 9.81 /m_Density* m_Weight /area) * pMainFrame->m_mstoUnit;
		str = QString("%1").arg(QCl,5,'f',2);
		str += strUnit;
		strange = tr("Qinf.sqrt(Cl) =");
		m_pctrlQInfCl->setText(strange+str);

		double RRe = m_pWing->m_TChord[0] * QCl/m_Viscosity;
		ReynoldsFormat(str, RRe);
		strange = tr("Root Re.sqrt(Cl) =");
		m_pctrlRRe->setText(strange+str);

		double SRe = m_pWing->m_TChord[m_pWing->m_NPanel] * QCl/m_Viscosity;
		ReynoldsFormat(str, SRe);
		strange = tr("Tip Re.sqrt(Cl) =");
		m_pctrlSRe->setText(strange+str);
	}
	else if (m_PolarType ==FIXEDAOAPOLAR)
	{
		m_pctrlQInfCl->setText(" ");
		m_pctrlRRe->setText(" ");
		m_pctrlSRe->setText(" ");
	}
}




void WPolarDlg::SetWingLoad()
{
	QString str,str1, str2;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	double area;
	if (m_RefAreaType==1) area =m_pWing->m_PlanformArea;
	else area = m_pWing->m_ProjectedArea;
	m_WingLoad = m_Weight/area;//kg/dm2

	str = QString("%1").arg(m_WingLoad * pMainFrame->m_kgtoUnit / pMainFrame->m_m2toUnit,7,'f',3);

	GetWeightUnit(str1, pMainFrame->m_WeightUnit);
	GetAreaUnit(str2, pMainFrame->m_AreaUnit);
	m_pctrlWingLoad->setText(tr("Wing Loading = ")+str+str1+"/"+str2);
}













