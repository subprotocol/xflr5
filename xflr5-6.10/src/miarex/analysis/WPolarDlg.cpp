/****************************************************************************

	WPolarDlg Class
	Copyright (C) 2009-2014 Andre Deperrois adeperrois@xflr5.com

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
#include <QTabWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <math.h>


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
	connect(m_pctrlAutoName, SIGNAL(toggled(bool)), this, SLOT(OnAutoName()));
	connect(m_pctrlLLTMethod, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlVLM1Method, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlVLM2Method, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlPanelMethod, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));

	connect(m_pctrlUnit1, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));

	connect(m_pctrlType1, SIGNAL(toggled(bool)), this, SLOT(OnPolarType()));
	connect(m_pctrlType2, SIGNAL(toggled(bool)), this, SLOT(OnPolarType()));
	connect(m_pctrlType4, SIGNAL(toggled(bool)), this, SLOT(OnPolarType()));
	connect(m_pctrlType5, SIGNAL(toggled(bool)), this, SLOT(OnPolarType()));

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
	connect(m_pctrlRefArea,    SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlRefSpan,    SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlRefChord,   SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(textEdited ( const QString &  )), this, SLOT(OnPolarName()));

	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea3, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void WPolarDlg::EnableControls()
{
	m_pctrlWPolarName->setEnabled(!m_pctrlAutoName->isChecked());

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
			m_pctrlAlpha->setEnabled(false);
			break;
		}
		case FIXEDAOAPOLAR:
		{
			m_pctrlQInf->setEnabled(false);
			m_pctrlAlpha->setEnabled(true);
			break;
		}
		case BETAPOLAR:
		{
			m_pctrlQInf->setEnabled(true);
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
	m_pctrlBeta->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD && s_WPolar.polarType()!=BETAPOLAR);
	m_pctrlGroundEffect->setEnabled(s_WPolar.m_AnalysisMethod==PANELMETHOD);
	m_pctrlHeight->setEnabled(m_pctrlGroundEffect->isChecked() && s_WPolar.m_AnalysisMethod==PANELMETHOD);

	m_pctrlWeight->setEnabled(!s_WPolar.m_bAutoInertia);
	m_pctrlXCmRef->setEnabled(!s_WPolar.m_bAutoInertia);
	m_pctrlZCmRef->setEnabled(!s_WPolar.m_bAutoInertia);

	m_pctrlVLM1Method->setEnabled(!s_WPolar.isBetaPolar() && fabs(s_WPolar.m_BetaSpec)<PRECISION);

	m_pctrlRefArea->setEnabled(m_pctrlArea3->isChecked());
	m_pctrlRefChord->setEnabled(m_pctrlArea3->isChecked());
	m_pctrlRefSpan->setEnabled(m_pctrlArea3->isChecked());
}



void WPolarDlg::InitDialog(Plane *pPlane, WPolar *pWPolar)
{
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
	}
	else
	{
		s_WPolar.m_AnalysisMethod=VLMMETHOD;
		s_WPolar.m_bThinSurfaces = true;
		m_pctrlPanelMethod->setVisible(false);
	}

	//initialize the name box
	s_WPolar.m_PlaneName = m_pPlane->planeName();

	// initialize units
	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);


	//initialize polar type
	if(s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)     m_pctrlType1->setChecked(true);
	else if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR) m_pctrlType2->setChecked(true);
	else if(s_WPolar.m_WPolarType==FIXEDAOAPOLAR)  m_pctrlType4->setChecked(true);
	else if(s_WPolar.m_WPolarType==BETAPOLAR)      m_pctrlType5->setChecked(true);


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


	m_pctrlXCmRef->SetValue(s_WPolar.m_CoG.x*Units::mtoUnit());
	m_pctrlZCmRef->SetValue(s_WPolar.m_CoG.z*Units::mtoUnit());

	m_pctrlQInf->SetValue(s_WPolar.m_QInfSpec*Units::mstoUnit());
	m_pctrlWeight->SetValue(s_WPolar.m_Mass*Units::kgtoUnit());
	m_pctrlBeta->SetValue(s_WPolar.m_BetaSpec);
	m_pctrlAlpha->SetValue(s_WPolar.m_AlphaSpec);


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
		m_pctrlVLM1Method->setChecked( s_WPolar.m_bVLM1);
		m_pctrlVLM2Method->setChecked(!s_WPolar.m_bVLM1);
		m_pctrlViscous->setEnabled(true);
	}
	else if(s_WPolar.m_AnalysisMethod==PANELMETHOD)
	{
		if(s_WPolar.m_bThinSurfaces)
		{
			m_pctrlVLM1Method->setChecked(s_WPolar.m_bVLM1);
			m_pctrlVLM2Method->setChecked(!s_WPolar.m_bVLM1);
		}
		else
		{
			m_pctrlPanelMethod->setChecked(true);
		}
		m_pctrlViscous->setEnabled(true);
	}


	m_pctrlArea1->setChecked(s_WPolar.m_ReferenceDim==PLANFORMREFDIM);
	m_pctrlArea2->setChecked(s_WPolar.m_ReferenceDim==PROJECTEDREFDIM);
	m_pctrlArea3->setChecked(s_WPolar.m_ReferenceDim==MANUALREFDIM);

	if(m_pctrlArea1->isChecked())
	{
		s_WPolar.m_referenceArea = m_pPlane->planformArea();
		s_WPolar.m_referenceSpanLength = m_pPlane->planformSpan();
		m_pctrlRefArea->SetValue(m_pPlane->planformArea()*Units::m2toUnit());
		m_pctrlRefSpan->SetValue(m_pPlane->planformSpan()*Units::mtoUnit());
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_WPolar.m_referenceArea = m_pPlane->projectedArea();
		s_WPolar.m_referenceSpanLength = m_pPlane->projectedSpan();
		m_pctrlRefArea->SetValue(m_pPlane->projectedArea()*Units::m2toUnit());
		m_pctrlRefSpan->SetValue(m_pPlane->projectedSpan()*Units::mtoUnit());
	}
	else if(m_pctrlArea3->isChecked())
	{
		m_pctrlRefArea->SetValue(s_WPolar.m_referenceArea*Units::m2toUnit());
		m_pctrlRefSpan->SetValue(s_WPolar.m_referenceSpanLength*Units::mtoUnit());
	}
	m_pctrlRefChord->SetValue(m_pPlane->mac()*Units::mtoUnit());

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
		s_WPolar.m_ReferenceDim = PLANFORMREFDIM;
		m_pctrlRefArea->SetValue(m_pPlane->planformArea()*Units::m2toUnit());
		m_pctrlRefChord->SetValue(m_pPlane->mac()*Units::mtoUnit());
		m_pctrlRefSpan->SetValue(m_pPlane->planformSpan()*Units::mtoUnit());
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_WPolar.m_ReferenceDim = PROJECTEDREFDIM;
		m_pctrlRefArea->SetValue(m_pPlane->projectedArea()*Units::m2toUnit());
		m_pctrlRefSpan->SetValue(m_pPlane->projectedSpan()*Units::mtoUnit());
		m_pctrlRefChord->SetValue(m_pPlane->mac()*Units::mtoUnit());
	}
	else if(m_pctrlArea3->isChecked())
	{
		s_WPolar.m_ReferenceDim = MANUALREFDIM;
//		m_pctrlRefArea->SetValue(s_WPolar.m_referenceAreaLength*Units::m2toUnit());
//		m_pctrlRefSpan->SetValue(s_WPolar.m_referenceSpanLength*Units::mtoUnit());
//		m_pctrlRefChord->SetValue(s_WPolar.m_referenceChordLength*Units::mtoUnit());
	}

	SetWPolarName();
	EnableControls();
}



void WPolarDlg::OnEditingFinished()
{
	ReadValues();
	SetReynolds();
	SetWPolarName();
	EnableControls();
}


void WPolarDlg::OnAutoName()
{
	m_bAutoName = m_pctrlAutoName->isChecked();
	if(m_bAutoName) SetWPolarName();
	EnableControls();
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
	else if (m_pctrlVLM1Method->isChecked() || m_pctrlVLM2Method->isChecked())
	{
		s_WPolar.m_bVLM1 = m_pctrlVLM1Method->isChecked();
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


void WPolarDlg::OnPolarName()
{
	m_bAutoName = false;
	m_pctrlAutoName->setChecked(false);
}



void WPolarDlg::OnPolarType()
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
	else if(m_pctrlType5->isChecked())
	{
		s_WPolar.m_WPolarType = BETAPOLAR;
		s_WPolar.m_bVLM1 = false;
		if(m_pctrlVLM1Method->isChecked())
		{
			m_pctrlVLM1Method->blockSignals(true);
			m_pctrlVLM2Method->blockSignals(true);
			m_pctrlVLM2Method->setChecked(true);
			m_pctrlVLM1Method->blockSignals(false);
			m_pctrlVLM2Method->blockSignals(false);
		}
	}
	EnableControls();
	SetReynolds();
	SetWPolarName();
}


void WPolarDlg::ReadValues()
{
	s_WPolar.m_AlphaSpec     = m_pctrlAlpha->Value();
	s_WPolar.m_BetaSpec      = m_pctrlBeta->Value();
	if(fabs(s_WPolar.m_BetaSpec)>PRECISION)
	{
		s_WPolar.m_bVLM1 = false;
		if(m_pctrlVLM1Method->isChecked())
		{
			m_pctrlVLM1Method->blockSignals(true);
			m_pctrlVLM2Method->blockSignals(true);
			m_pctrlVLM2Method->setChecked(true);
			m_pctrlVLM1Method->blockSignals(false);
			m_pctrlVLM2Method->blockSignals(false);
		}
	}

	s_WPolar.m_Mass          = m_pctrlWeight->Value() / Units::kgtoUnit();
	s_WPolar.m_CoG.x         = m_pctrlXCmRef->Value() / Units::mtoUnit();
	s_WPolar.m_CoG.z         = m_pctrlZCmRef->Value() / Units::mtoUnit();
	s_WPolar.m_QInfSpec      = m_pctrlQInf->Value() / Units::mstoUnit();
	s_WPolar.m_Height        = m_pctrlHeight->Value() / Units::mtoUnit();

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
		s_WPolar.m_ReferenceDim = PLANFORMREFDIM;
		s_WPolar.m_referenceArea       = m_pPlane->planformArea();
		s_WPolar.m_referenceSpanLength = m_pPlane->planformSpan();
	}
	else if(m_pctrlArea2->isChecked())
	{
		s_WPolar.m_ReferenceDim = PROJECTEDREFDIM;
		s_WPolar.m_referenceArea       = m_pPlane->projectedArea();
		s_WPolar.m_referenceSpanLength = m_pPlane->projectedSpan();
	}
	else if(m_pctrlArea3->isChecked())
	{
		s_WPolar.m_ReferenceDim = MANUALREFDIM;
		s_WPolar.m_referenceArea       = m_pctrlRefArea->Value() /Units::m2toUnit();
		s_WPolar.m_referenceSpanLength = m_pctrlRefSpan->Value() /Units::mtoUnit();
	}
	s_WPolar.m_referenceChordLength = m_pctrlRefChord->Value() /Units::mtoUnit();

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
	QString strSpeedUnit, strAreaUnit, strLengthUnit, strWeightUnit;

	Units::getSpeedUnitLabel(strSpeedUnit);
	Units::getAreaUnitLabel(strAreaUnit);
	Units::getLengthUnitLabel(strLengthUnit);
	Units::getWeightUnitLabel(strWeightUnit);


	QFont fnt;
	QFont symbolFont("Symbol");

	QFontMetrics fm(fnt);


	QTabWidget *pTabWidget = new QTabWidget(this);
	pTabWidget->setMinimumWidth(fm.averageCharWidth() * 83);
	QWidget *pPolarTypePage = new QWidget(this);
	QWidget *pMethodPage = new QWidget(this);
	QWidget *pInertiaPage = new QWidget(this);
	QWidget *pCoefficientPage = new QWidget(this);
	QWidget *pOptionPage = new QWidget(this);


	QVBoxLayout *pPolarTypePageLayout = new QVBoxLayout;
	{
		QHBoxLayout *pAnalysisSettingsLayout = new QHBoxLayout;
		{
			QVBoxLayout *pTypeLayout = new QVBoxLayout;
			{
				m_pctrlType1 = new QRadioButton(tr("Type 1 (Fixed Speed)"));
				m_pctrlType2 = new QRadioButton(tr("Type 2 (Fixed Lift)"));
				m_pctrlType4 = new QRadioButton(tr("Type 4 (Fixed aoa)"));
				m_pctrlType5 = new QRadioButton(tr("Type 5 (Beta range)"));
				pTypeLayout->addWidget(m_pctrlType1);
				pTypeLayout->addWidget(m_pctrlType2);
				pTypeLayout->addWidget(m_pctrlType4);
				pTypeLayout->addWidget(m_pctrlType5);
				pTypeLayout->addStretch();
			}

			QGridLayout *pTypeDataLayout = new QGridLayout;
			{
				QLabel *lab1 = new QLabel(tr("Free Stream Speed ="));
				QLabel *lab5 = new QLabel("a =");
				QLabel *lab6 = new QLabel("b =");
				lab5->setFont(symbolFont);
				lab6->setFont(symbolFont);
				lab1->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				lab5->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				lab6->setAlignment(Qt::AlignRight | Qt::AlignCenter);
				m_pctrlQInf    = new DoubleEdit(10.05);
				m_pctrlQInf->SetMin(0.0);
				m_pctrlAlpha   = new DoubleEdit(1.00,2);
				m_pctrlBeta    = new DoubleEdit(0.00,2);
				QLabel *labSpeedUnit   = new QLabel(strSpeedUnit);

				QLabel *lab7 = new QLabel(QString::fromUtf8("°"));
				QLabel *lab8 = new QLabel(QString::fromUtf8("°"));
				pTypeDataLayout->addWidget(lab1,1,1);
				pTypeDataLayout->addWidget(lab5,2,1);
				pTypeDataLayout->addWidget(lab6,3,1);
				pTypeDataLayout->addWidget(m_pctrlQInf,1,2);
				pTypeDataLayout->addWidget(m_pctrlAlpha,2,2);
				pTypeDataLayout->addWidget(m_pctrlBeta,3,2);
				pTypeDataLayout->addWidget(labSpeedUnit ,1,3);
				pTypeDataLayout->addWidget(lab7 ,2,3);
				pTypeDataLayout->addWidget(lab8 ,3,3);
				pTypeDataLayout->setRowStretch(4,1);
				pTypeDataLayout->setColumnStretch(3,1);
			}

			pAnalysisSettingsLayout->addLayout(pTypeLayout);
			pAnalysisSettingsLayout->addStretch();
			pAnalysisSettingsLayout->addLayout(pTypeDataLayout);
		}

		QVBoxLayout *pFlightLayout = new QVBoxLayout;
		{
			m_pctrlWingLoad  = new QLabel(tr("Wing Loading = 0.033 kg/dm2"));
			m_pctrlSRe       = new QLabel(tr("SRe"));
			m_pctrlRRe       = new QLabel(tr("RRe"));
			m_pctrlQInfCl    = new QLabel(tr("QInfCl"));

			m_pctrlWingLoad->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlSRe->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlRRe->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			m_pctrlQInfCl->setAlignment(Qt::AlignRight | Qt::AlignCenter);
			pFlightLayout->addWidget(m_pctrlWingLoad);
			pFlightLayout->addWidget(m_pctrlSRe);
			pFlightLayout->addWidget(m_pctrlRRe);
			pFlightLayout->addWidget(m_pctrlQInfCl);
			pFlightLayout->addStretch();
		}

		pPolarTypePageLayout->addLayout(pAnalysisSettingsLayout);
		pPolarTypePageLayout->addStretch();
		pPolarTypePageLayout->addLayout(pFlightLayout);
		pPolarTypePage->setLayout(pPolarTypePageLayout);
	}

	QVBoxLayout *pMethodPageLayout = new QVBoxLayout;
	{
		QGroupBox *pAnalysisMethods = new QGroupBox(tr("Analysis Methods"));
		{
			QVBoxLayout *pMethodLayout = new QVBoxLayout;
			{
				m_pctrlLLTMethod   = new QRadioButton(tr("LLT (Wing only)"));
				m_pctrlVLM1Method = new QRadioButton(tr("Horseshoe vortex")+ " (VLM1) "+tr("(No sideslip)"));
				m_pctrlVLM2Method = new QRadioButton(tr("Ring vortex")+" (VLM2)");
				m_pctrlPanelMethod = new QRadioButton(tr("3D Panels"));

				pMethodLayout->addWidget(m_pctrlLLTMethod);
				pMethodLayout->addWidget(m_pctrlVLM1Method);
				pMethodLayout->addWidget(m_pctrlVLM2Method);
				pMethodLayout->addWidget(m_pctrlPanelMethod);
				pMethodLayout->addStretch();
			}
			pAnalysisMethods->setLayout(pMethodLayout);

		}

		QGroupBox *pOptionsGroupBox = new QGroupBox(tr("Options"));
		{
			QVBoxLayout *pOptionsLayout = new QVBoxLayout;
			{
				m_pctrlViscous = new QCheckBox(tr("Viscous"));
				m_pctrlTiltGeom = new QCheckBox(tr("Tilt. Geom."));
				pOptionsLayout->addWidget(m_pctrlViscous);
				pOptionsLayout->addWidget(m_pctrlTiltGeom);
	//			OptionsLayout->addWidget(m_pctrlIgnoreBody);
				m_pctrlIgnoreBodyPanels = new QCheckBox(tr("Ignore Body Panels"));
				pOptionsLayout->addWidget(m_pctrlIgnoreBodyPanels);
				pOptionsLayout->addStretch();
			}
			pOptionsGroupBox->setLayout(pOptionsLayout);
		}

		pMethodPageLayout->addWidget(pAnalysisMethods);
		pMethodPageLayout->addWidget(pOptionsGroupBox);
		pMethodPage->setLayout(pMethodPageLayout);
	}

	QVBoxLayout *pInertiaPageLayout = new QVBoxLayout;
	{
		QGroupBox *pInertiaBox = new QGroupBox(tr("Inertia properties"));
		{
			QVBoxLayout *pInertiaLayout = new QVBoxLayout;
			{
				m_pctrlPlaneInertia = new QCheckBox(tr("Use plane inertia"));
				m_pctrlPlaneInertia->setToolTip("Activate this checbox for the polar to use dynamically the plane's inertia properties for each analysis");

				QGridLayout *pInertiaDataLayout = new QGridLayout;
				{
					QLabel *lab2 = new QLabel(tr("Plane Mass ="));
					QLabel *lab3 = new QLabel(tr("X_CoG ="));
					QLabel *lab4 = new QLabel(tr("Z_CoG ="));
					lab2->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					lab3->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					lab4->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					pInertiaDataLayout->addWidget(lab2,1,1);
					pInertiaDataLayout->addWidget(lab3,2,1);
					pInertiaDataLayout->addWidget(lab4,3,1);
					m_pctrlWeight  = new DoubleEdit(0,3);
					m_pctrlWeight->SetMin(0.0);
					m_pctrlXCmRef  = new DoubleEdit(100.00,3);
					m_pctrlZCmRef  = new DoubleEdit(100.00,3);
					pInertiaDataLayout->addWidget(m_pctrlWeight,1,2);
					pInertiaDataLayout->addWidget(m_pctrlXCmRef,2,2);
					pInertiaDataLayout->addWidget(m_pctrlZCmRef,3,2);
					QLabel *labWeightUnit  = new QLabel(strWeightUnit);
					QLabel *labLengthUnit1 = new QLabel(strLengthUnit);
					QLabel *labLengthUnit3 = new QLabel(strLengthUnit);

					pInertiaDataLayout->addWidget(labWeightUnit ,1,3);
					pInertiaDataLayout->addWidget(labLengthUnit1 ,2,3);
					pInertiaDataLayout->addWidget(labLengthUnit3 ,3,3);
					pInertiaDataLayout->setColumnStretch(1,1);
				}
				pInertiaLayout->addWidget(m_pctrlPlaneInertia);
				pInertiaLayout->addLayout(pInertiaDataLayout);
				pInertiaLayout->addStretch();
			}
			pInertiaBox->setLayout(pInertiaLayout);
		}

		pInertiaPageLayout->addWidget(pInertiaBox);
		pInertiaPage->setLayout(pInertiaPageLayout);
	}

	QVBoxLayout *pCoefficientPageLayout = new QVBoxLayout;
	{
		QGroupBox *pAreaBox = new QGroupBox(tr("Ref. dimensions for aero coefficients"));
		{
			QVBoxLayout *pAreaOptions = new QVBoxLayout;
			{
				m_pctrlArea1 = new QRadioButton(tr("Wing Planform"));
				m_pctrlArea2 = new QRadioButton(tr("Wing Planform projected on xy plane"));
				m_pctrlArea3 = new QRadioButton(tr("User defined"));

				QGridLayout *pRefAreaLayout = new QGridLayout;
				{
					QLabel *labRefArea  = new QLabel("Ref. area=");
					QLabel *labRefSpan  = new QLabel("Ref. span length=");
					QLabel *labRefChord = new QLabel("Ref. chord length=");
					m_pctrlRefArea  = new DoubleEdit(0.0, 3);
					m_pctrlRefChord = new DoubleEdit(0.0, 3);
					m_pctrlRefSpan  = new DoubleEdit(0.0, 3);
					QLabel *labAreaUnit = new QLabel(strAreaUnit);
					QLabel *labLengthUnit4 = new QLabel(strLengthUnit);
					QLabel *labLengthUnit5 = new QLabel(strLengthUnit);

					labRefArea->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					labRefSpan->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					labRefChord->setAlignment(Qt::AlignRight | Qt::AlignCenter);

					labAreaUnit->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
					labLengthUnit4->setAlignment(Qt::AlignLeft | Qt::AlignCenter);

					pRefAreaLayout->addWidget(labRefArea,1,1);
					pRefAreaLayout->addWidget(m_pctrlRefArea,1,2);
					pRefAreaLayout->addWidget(labAreaUnit,1,3);
					pRefAreaLayout->addWidget(labRefSpan,2,1);
					pRefAreaLayout->addWidget(m_pctrlRefSpan,2,2);
					pRefAreaLayout->addWidget(labLengthUnit4,2,3);
					pRefAreaLayout->addWidget(labRefChord,3,1);
					pRefAreaLayout->addWidget(m_pctrlRefChord,3,2);
					pRefAreaLayout->addWidget(labLengthUnit5,3,3);

					pRefAreaLayout->setColumnStretch(1,1);
				}

				pAreaOptions->addWidget(m_pctrlArea1);
				pAreaOptions->addWidget(m_pctrlArea2);
				pAreaOptions->addWidget(m_pctrlArea3);
				pAreaOptions->addLayout(pRefAreaLayout);
				pAreaOptions->addStretch();
			}
			pAreaBox->setLayout(pAreaOptions);
		}

		pCoefficientPageLayout->addWidget(pAreaBox);
		pCoefficientPage->setLayout(pCoefficientPageLayout);
	}

	QHBoxLayout *pOptionPageLayout =new QHBoxLayout;
	{
		QGroupBox *pAeroDataGroupBox = new QGroupBox(tr("Air Data"));
		{
			QVBoxLayout *pAeroDataLayout = new QVBoxLayout;
			{
				QHBoxLayout *pAeroUnitLayout = new QHBoxLayout;
				{
					QLabel *lab9 = new QLabel(tr("Unit"));
					lab9->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					m_pctrlUnit1 = new QRadioButton(tr("International"));
					m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
					pAeroUnitLayout->addWidget(lab9);
					pAeroUnitLayout->addWidget(m_pctrlUnit1);
					pAeroUnitLayout->addWidget(m_pctrlUnit2);
					pAeroUnitLayout->addStretch();
				}
				QGridLayout *pAeroDataValuesLayout = new QGridLayout;
				{
					m_pctrlRho = new QLabel("r =");
					m_pctrlDensity = new DoubleEdit(1.225,3);
					m_pctrlDensityUnit = new QLabel("kg/m3");
					m_pctrlNu = new QLabel("n =");
					m_pctrlRho->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					m_pctrlNu->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					m_pctrlViscosity = new DoubleEdit(1.500e-5,3);
					m_pctrlViscosityUnit = new QLabel("m2/s");
					m_pctrlRho->setFont(symbolFont);
					m_pctrlNu->setFont(symbolFont);
					m_pctrlDensity->SetPrecision(5);
					m_pctrlViscosity->SetPrecision(3);
					m_pctrlDensity->SetMin(0.0);
					m_pctrlViscosity->SetMin(0.0);
					pAeroDataValuesLayout->addWidget(m_pctrlRho,1,1);
					pAeroDataValuesLayout->addWidget(m_pctrlDensity,1,2);
					pAeroDataValuesLayout->addWidget(m_pctrlDensityUnit,1,3);
					pAeroDataValuesLayout->addWidget(m_pctrlNu,2,1);
					pAeroDataValuesLayout->addWidget(m_pctrlViscosity,2,2);
					pAeroDataValuesLayout->addWidget(m_pctrlViscosityUnit,2,3);
					pAeroDataValuesLayout->setRowStretch(3,1);
					pAeroDataValuesLayout->setColumnStretch(1,3);
					pAeroDataValuesLayout->setColumnStretch(4,3);
				}
				pAeroDataLayout->addLayout(pAeroUnitLayout);
				pAeroDataLayout->addLayout(pAeroDataValuesLayout);
			}
			pAeroDataGroupBox->setLayout(pAeroDataLayout);
		}

		QGroupBox *pGroundGroupBox =  new QGroupBox(tr("Ground Effect"));
		{
			QVBoxLayout * pGroundLayout = new QVBoxLayout;
			{
				m_pctrlGroundEffect = new QCheckBox(tr("Ground Effect"));
				QHBoxLayout *pGroundHeightLayout = new QHBoxLayout;
				{
					QLabel *lab10 = new QLabel(tr("Height ="));
					m_pctrlHeight = new DoubleEdit(0.00,2);
					m_pctrlHeight->setAlignment(Qt::AlignRight | Qt::AlignCenter);
					QLabel *labLengthUnit2 = new QLabel(strLengthUnit);
					pGroundHeightLayout->addStretch();
					pGroundHeightLayout->addWidget(lab10);
					pGroundHeightLayout->addWidget(m_pctrlHeight);
					pGroundHeightLayout->addWidget(labLengthUnit2);
				}
				pGroundLayout->addWidget(m_pctrlGroundEffect);
				pGroundLayout->addLayout(pGroundHeightLayout);
				pGroundLayout->addStretch(1);
			}
			pGroundGroupBox->setLayout(pGroundLayout);
		}

		pOptionPageLayout->addWidget(pAeroDataGroupBox);

		pOptionPageLayout->addWidget(pGroundGroupBox);
		pOptionPage->setLayout(pOptionPageLayout);
	}


	pTabWidget->addTab(pPolarTypePage, tr("Polar Type"));
	pTabWidget->addTab(pMethodPage, tr("Analysis"));
	pTabWidget->addTab(pInertiaPage, tr("Inertia"));
	pTabWidget->addTab(pCoefficientPage, tr("Ref. dimensions"));
	pTabWidget->addTab(pOptionPage, tr("Aero data"));

	pTabWidget->setCurrentIndex(0);
//	connect(m_pTabWidget, SIGNAL(currentChanged (int)), this, SLOT(OnActivePage(int)));

	QHBoxLayout *pCommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		OKButton->setDefault(true);
		CancelButton = new QPushButton(tr("Cancel"));
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(OKButton);
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(CancelButton);
		pCommandButtons->addStretch(1);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout(this);
	{
		QHBoxLayout *pPolarNameLayout = new QHBoxLayout;
		{
			m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
			m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
			pPolarNameLayout->addWidget(m_pctrlAutoName);
			pPolarNameLayout->addWidget(m_pctrlWPolarName);
		}

		pMainLayout->addLayout(pPolarNameLayout);
		pMainLayout->addSpacing(13);
		pMainLayout->addWidget(pTabWidget);
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pCommandButtons);
		pMainLayout->addStretch(1);
	}
	setLayout(pMainLayout);
}



void WPolarDlg::SetWPolarName()
{
	if(!m_bAutoName) return;

	QString strSpeedUnit, strong;
	QString WPolarName;
	Units::getSpeedUnitLabel(strSpeedUnit);

	if (s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)
	{
		WPolarName = QString("T1-%1 ").arg(s_WPolar.m_QInfSpec * Units::mstoUnit(),0,'f',1);
		WPolarName += strSpeedUnit;
	}
	else if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)
	{
		WPolarName = QString("T2");
	}
	else if(s_WPolar.m_WPolarType==FIXEDAOAPOLAR)
	{
		WPolarName = QString(QString::fromUtf8("T4-%1°")).arg(s_WPolar.m_AlphaSpec,0,'f',3);
	}
	else if(s_WPolar.m_WPolarType==BETAPOLAR)
	{
		WPolarName = QString(QString::fromUtf8("T5-a%1°-%2"))
						  .arg(s_WPolar.m_AlphaSpec,0,'f',1)
						  .arg(s_WPolar.m_QInfSpec * Units::mstoUnit(),0,'f',1);
		WPolarName += strSpeedUnit;
	}

	if(s_WPolar.m_AnalysisMethod==LLTMETHOD)
		WPolarName += "-LLT";
	else if(s_WPolar.m_AnalysisMethod==VLMMETHOD)
	{
		if(s_WPolar.m_bVLM1) WPolarName += "-VLM1";
		else		           WPolarName += "-VLM2";
	}
	else if(s_WPolar.m_AnalysisMethod==PANELMETHOD)
	{
		if(m_pPlane->isWing() && !s_WPolar.m_bThinSurfaces) WPolarName += "-Panel";
		if(s_WPolar.m_bThinSurfaces)
		{
			if(s_WPolar.m_bVLM1) WPolarName += "-VLM1";
			else		           WPolarName += "-VLM2";
		}
	}

	if(!s_WPolar.m_bAutoInertia)
	{
		Units::getWeightUnitLabel(strSpeedUnit);
		strong = QString("-%1").arg(s_WPolar.m_Mass*Units::kgtoUnit(),0,'f',3);
		if(s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)   WPolarName += strong+strSpeedUnit;
		Units::getLengthUnitLabel(strSpeedUnit);
		strong = QString("-x%1").arg(s_WPolar.m_CoG.x*Units::mtoUnit(),0,'f',3);
		WPolarName += strong + strSpeedUnit;

		if(qAbs(s_WPolar.m_CoG.z)>=.000001)
		{
			strong = QString("-z%1").arg(s_WPolar.m_CoG.z*Units::mtoUnit(),0,'f',3);
			WPolarName += strong + strSpeedUnit;
		}
	}
//	else WPolarName += "-Plane_Inertia";


	if(qAbs(s_WPolar.m_BetaSpec) > .001 && s_WPolar.polarType()!=BETAPOLAR)
	{
		strong = QString(QString::fromUtf8("-b%1°")).arg(s_WPolar.m_BetaSpec,0,'f',2);
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
	if(s_WPolar.m_AnalysisMethod!=LLTMETHOD && s_WPolar.m_ReferenceDim==PROJECTEDREFDIM) WPolarName += "-proj_area";

	m_pctrlWPolarName->setText(WPolarName);
}





void WPolarDlg::SetWingLoad()
{
	QString str,str1, str2;

	if(s_WPolar.referenceArea()>0)
	{
		m_WingLoad = s_WPolar.m_Mass/s_WPolar.referenceArea();//kg/dm2

		str = QString("%1 ").arg(m_WingLoad * Units::kgtoUnit() / Units::m2toUnit(),7,'f',3);

		Units::getWeightUnitLabel(str1);
		Units::getAreaUnitLabel(str2);
	}
	m_pctrlWingLoad->setText(tr("Wing Loading = ")+str+str1+"/"+str2);
}



void WPolarDlg::SetReynolds()
{
	QString strange, str, strUnit;
	Units::getSpeedUnitLabel(strUnit);

	if(s_WPolar.m_WPolarType == FIXEDSPEEDPOLAR)
	{
		double RRe = m_pPlane->rootChord() * s_WPolar.m_QInfSpec/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, RRe);
		strange = tr("Root Re =");
		m_pctrlRRe->setText(strange+str);

		double SRe = m_pPlane->tipChord() * s_WPolar.m_QInfSpec/s_WPolar.m_Viscosity;
		ReynoldsFormat(str, SRe);
		strange = tr("Tip Re =");
		m_pctrlSRe->setText(strange+str);

		m_pctrlQInfCl->setText(" ");
	}
	else if(s_WPolar.m_WPolarType == FIXEDLIFTPOLAR)
	{
		double QCl =  sqrt(2.* 9.81 /s_WPolar.m_Density* s_WPolar.m_Mass /s_WPolar.referenceArea()) * Units::mstoUnit();
		str = QString("%1 ").arg(QCl,5,'f',2);
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












