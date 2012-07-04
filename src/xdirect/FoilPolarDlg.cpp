/****************************************************************************

	FoilPolarDlg Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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

#include <QGroupBox>

#include "FoilPolarDlg.h"
#include "XDirect.h"
#include "../globals.h"
#include "../mainframe.h"

extern CFoil *g_pCurFoil;

void *FoilPolarDlg::s_pXDirect;
void *FoilPolarDlg::s_pMainFrame;

FoilPolarDlg::FoilPolarDlg(void *pParent)
{
	setWindowTitle(tr("Foil Polar Definition"));
	m_PolarType = FIXEDSPEEDPOLAR;
	m_NCrit    = 9.0;
	m_XTopTr   = 1.0;
	m_XBotTr   = 1.0;
	m_Mach     = 0.0;
	m_Reynolds = 100000.0;
	m_ASpec    = 0.0;
	m_bAutoName = true;
	m_UnitType = 1;
	m_Viscosity = 1.5e-5;
	m_Density   = 1.225;
	m_Chord = m_Span = m_Mass;
	SetupLayout();
}


void FoilPolarDlg::SetupLayout()
{
	QFont SymbolFont("Symbol");

	QVBoxLayout *Analysis = new QVBoxLayout;
	QHBoxLayout *autoname = new QHBoxLayout;
	m_pctrlAuto1 = new QRadioButton(tr("Automatic"));
	m_pctrlAuto2 = new QRadioButton(tr("User Defined"));
	m_pctrlAnalysisName = new QLineEdit(tr("Analysis Name"));
	autoname->addStretch(1);
	autoname->addWidget(m_pctrlAuto1);
	autoname->addStretch(1);
	autoname->addWidget(m_pctrlAuto2);
	autoname->addStretch(1);
	Analysis->addLayout(autoname);
	Analysis->addWidget(m_pctrlAnalysisName);
	QGroupBox *NameGroup = new QGroupBox(tr("Analysis Name"));
	NameGroup->setLayout(Analysis);

	QHBoxLayout *AnalysisType = new QHBoxLayout;
	m_rbtype1 = new		QRadioButton(tr("Type 1"));
	m_rbtype2 = new QRadioButton(tr("Type 2"));
	m_rbtype3 = new QRadioButton(tr("Type 3"));
	m_rbtype4 = new QRadioButton(tr("Type 4"));
	AnalysisType->addWidget(m_rbtype1);
	AnalysisType->addWidget(m_rbtype2);
	AnalysisType->addWidget(m_rbtype3);
	AnalysisType->addWidget(m_rbtype4);
	QGroupBox *TypeGroup = new QGroupBox(tr("Analysis Type"));
	TypeGroup->setLayout(AnalysisType);


	//type 2 input data
	QGroupBox *PlaneDataGroup = new QGroupBox(tr("Plane Data"));
	{
		QGridLayout *PlaneDataLayout = new QGridLayout;
		m_pctrlChord = new FloatEdit(0,3);
		m_pctrlMass = new FloatEdit(0,3);
		m_pctrlSpan = new FloatEdit(0,3);
		QLabel *ChordLab = new QLabel(tr("Chord"));
		QLabel *MassLab = new QLabel(tr("Mass"));
		QLabel *SpanLab = new QLabel(tr("Span"));
		m_pctrlLengthUnit1 = new QLabel("m");
		m_pctrlLengthUnit2 = new QLabel("m");
		m_pctrlMassUnit = new QLabel("kg");
		PlaneDataLayout->addWidget(ChordLab,1,1);
		PlaneDataLayout->addWidget(m_pctrlChord,1,2);
		PlaneDataLayout->addWidget(m_pctrlLengthUnit1,1,3);
		PlaneDataLayout->addWidget(SpanLab,2,1);
		PlaneDataLayout->addWidget(m_pctrlSpan,2,2);
		PlaneDataLayout->addWidget(m_pctrlLengthUnit2,2,3);
		PlaneDataLayout->addWidget(MassLab,3,1);
		PlaneDataLayout->addWidget(m_pctrlMass,3,2);
		PlaneDataLayout->addWidget(m_pctrlMassUnit,3,3);
		PlaneDataGroup->setLayout(PlaneDataLayout);
	}
	QGroupBox *AeroDataGroup = new QGroupBox(tr("Aerodynamic Data"));
	{
		QGridLayout *AeroDataLayout = new QGridLayout;
		QLabel *lab9 = new QLabel(tr("Unit"));
		m_pctrlUnit1 = new QRadioButton(tr("International"));
		m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
		m_pctrlRho = new QLabel("r =");
		m_pctrlDensity = new FloatEdit(1.225,3);
		m_pctrlDensityUnit = new QLabel("kg/m3");
		m_pctrlNu = new QLabel("n =");
		m_pctrlRho->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlNu->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		m_pctrlViscosity = new FloatEdit(1.500e-5,3);
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
	QHBoxLayout *Type2DataLayout = new QHBoxLayout;
	Type2DataLayout->addWidget(PlaneDataGroup);
	Type2DataLayout->addWidget(AeroDataGroup);

	QHBoxLayout *ReMachResultsLayout = new QHBoxLayout;
	m_pctrlReLabel   = new QLabel(tr("  Re ="));
	m_pctrlReLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlMachLabel = new QLabel(tr("Mach ="));
	m_pctrlMachLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlReynolds = new FloatEdit();
	m_pctrlReynolds->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlMach = new FloatEdit();
	m_pctrlMach->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ReMachResultsLayout->addWidget(m_pctrlReLabel);
	ReMachResultsLayout->addWidget(m_pctrlReynolds);
	ReMachResultsLayout->addStretch(1);
	ReMachResultsLayout->addWidget(m_pctrlMachLabel);
	ReMachResultsLayout->addWidget(m_pctrlMach);

	QVBoxLayout *ReMachLayout = new QVBoxLayout;
	ReMachLayout->addLayout(Type2DataLayout);
	ReMachLayout->addSpacing(15);
	ReMachLayout->addLayout(ReMachResultsLayout);

	QGroupBox *AeroGroup = new QGroupBox(tr("Reynolds and Mach Numbers"));
	AeroGroup->setLayout(ReMachLayout);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *Transitions = new QGridLayout;
	QLabel *FreeTransLabel   = new QLabel(tr("Free transitions (e^n) method"));
	QLabel *ForceTransLabel  = new QLabel(tr("Forced transition:"));
	QLabel	*NCritLabel      = new QLabel(tr("NCrit="));
	QLabel *TopTripLabel     = new QLabel(tr("TripLocation (top)"));
	QLabel *BotTripLabel     = new QLabel(tr("TripLocation (bot)"));
	m_pctrlNCrit    = new FloatEdit();
	m_pctrlTopTrans = new FloatEdit();
	m_pctrlBotTrans = new FloatEdit();

	m_pctrlNCrit->setAlignment(   Qt::AlignRight);
	m_pctrlTopTrans->setAlignment(Qt::AlignRight);
	m_pctrlBotTrans->setAlignment(Qt::AlignRight);
	Transitions->addWidget(FreeTransLabel,   1,1, 1,1, Qt::AlignLeft| Qt::AlignVCenter);
	Transitions->addWidget(ForceTransLabel,  2,1, 1,1, Qt::AlignLeft| Qt::AlignVCenter);
	Transitions->addWidget(NCritLabel,       1,2, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	Transitions->addWidget(TopTripLabel,     2,2, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	Transitions->addWidget(BotTripLabel,     3,2, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	Transitions->addWidget(m_pctrlNCrit,     1,3, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	Transitions->addWidget(m_pctrlTopTrans,  2,3, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	Transitions->addWidget(m_pctrlBotTrans,  3,3, 1,1, Qt::AlignRight| Qt::AlignVCenter);
	QGroupBox *TransGroup = new QGroupBox(tr("Transition settings"));
	TransGroup->setLayout(Transitions);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setSpacing(10);
	mainLayout->addWidget(NameGroup);

	mainLayout->addWidget(TypeGroup);
	mainLayout->addWidget(AeroGroup);
	mainLayout->addWidget(TransGroup);
	mainLayout->addLayout(CommandButtons);

	setLayout(mainLayout);

	m_pctrlTopTrans->SetPrecision(2);
	m_pctrlTopTrans->SetMin(0.0);
	m_pctrlTopTrans->SetMax(1.0);

	m_pctrlBotTrans->SetPrecision(2);
	m_pctrlBotTrans->SetMin(0.0);
	m_pctrlBotTrans->SetMax(1.0);


	m_pctrlNCrit->SetPrecision(3);
	m_pctrlNCrit->SetMin(0.0);
	m_pctrlNCrit->SetMax(1000000.0);


	m_pctrlReynolds->SetPrecision(0);
	m_pctrlReynolds->SetMin(-1.0e10);
	m_pctrlReynolds->SetMax(1.e10);


	m_pctrlMach->SetPrecision(2);
	m_pctrlMach->SetMin(0.0);
	m_pctrlMach->SetMax(1000.0);

	connect(m_pctrlAuto1, SIGNAL(clicked()), this, SLOT(OnAutoName()));
	connect(m_pctrlAuto2, SIGNAL(clicked()), this, SLOT(OnAutoName()));

	connect(m_rbtype1, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype2, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype3, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype4, SIGNAL(clicked()), this, SLOT(OnPolarType()));

	connect(m_pctrlReynolds, SIGNAL(editingFinished()), this, SLOT(EditingFinished()));
	connect(m_pctrlMach, SIGNAL(editingFinished()), this, SLOT(EditingFinished()));
	connect(m_pctrlNCrit, SIGNAL(editingFinished()), this, SLOT(EditingFinished()));
	connect(m_pctrlTopTrans, SIGNAL(editingFinished()), this, SLOT(EditingFinished()));
	connect(m_pctrlBotTrans, SIGNAL(editingFinished()), this, SLOT(EditingFinished()));

	connect(m_pctrlAnalysisName, SIGNAL(textEdited (const QString &)), this, SLOT(OnNameChanged()));

	connect(m_pctrlUnit1, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2, SIGNAL(toggled(bool)), this, SLOT(OnUnit()));

	connect(m_pctrlChord, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlSpan, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlMass, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlDensity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}


void FoilPolarDlg::EditingFinished()
{
	SetPlrName();
//	OKButton->setFocus();
}


void FoilPolarDlg::InitDialog()
{
	QXDirect* pXDirect = (QXDirect*)s_pXDirect;
	MainFrame *pMainFrame =(MainFrame*)s_pMainFrame;
	if(pXDirect && g_pCurFoil) m_FoilName = g_pCurFoil->m_FoilName;
	else                       m_FoilName = "";

	QString str = tr("Analysis parameters for ");
	setWindowTitle(str+ m_FoilName);

	m_pctrlReynolds->SetValue(m_Reynolds);
	m_pctrlMach->SetValue(m_Mach);
	m_pctrlNCrit->SetValue(m_NCrit);
	m_pctrlTopTrans->SetValue(m_XTopTr);
	m_pctrlBotTrans->SetValue(m_XBotTr);

	m_bAutoName = true;
	m_pctrlAuto1->setChecked(true);

	switch(m_PolarType)
	{
		case FIXEDSPEEDPOLAR:
		{
			m_rbtype1->setChecked(true);
			break;
		}
		case FIXEDLIFTPOLAR:
		{
			m_rbtype2->setChecked(true);
			break;
		}
		case RUBBERCHORDPOLAR:
		{
			m_rbtype3->setChecked(true);
			break;
		}
		case FIXEDAOAPOLAR:
		{
			m_rbtype4->setChecked(true);
			break;
		}
	}

	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit2->setText(str);

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pctrlMassUnit->setText(str);

	m_pctrlUnit1->setChecked(m_UnitType==1);
	m_pctrlUnit2->setChecked(m_UnitType!=1);
	m_pctrlViscosity->SetValue(m_Viscosity);
	m_pctrlDensity->SetValue(m_Density);
	OnUnit();

	m_pctrlMass->SetValue(m_Mass);
	m_pctrlSpan->SetValue(m_Span);
	m_pctrlChord->SetValue(m_Chord);

	OnPolarType();	
}


void FoilPolarDlg::keyPressEvent(QKeyEvent *event)
{
    // Prevent Return Key from closing App
    switch (event->key())
    {
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				ReadParams();
				SetPlrName();
				OKButton->setFocus();
				return;
			}
			else if(OKButton->hasFocus())
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


void FoilPolarDlg::OnAutoName()
{
	if(m_pctrlAuto2->isChecked())
	{
		m_bAutoName = false;
		m_pctrlAnalysisName->setFocus();
		m_pctrlAnalysisName->selectAll();
	}
	else
	{
		m_bAutoName = true;
		SetPlrName();
	}
}


void FoilPolarDlg::OnNameChanged()
{
	m_bAutoName = false;
	m_pctrlAuto1->setChecked(false);
	m_pctrlAuto2->setChecked(true);
}


void FoilPolarDlg::OnOK()
{
	m_PlrName = m_pctrlAnalysisName->text();
	accept();
}


void FoilPolarDlg::OnPolarType()
{
	if(m_rbtype1->isChecked())
	{
		m_pctrlReLabel->setText(tr("Reynolds ="));
		m_pctrlMachLabel->setText(tr("Mach ="));
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);
		m_PolarType = FIXEDSPEEDPOLAR;
	}
	else if(m_rbtype2->isChecked())
	{
		m_pctrlReLabel->setText(tr("Re.sqrt(Cl) ="));
		m_pctrlMachLabel->setText(tr("Ma.sqrt(Cl) ="));
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);		
		m_PolarType = FIXEDLIFTPOLAR;
	}
	else if(m_rbtype3->isChecked())
	{
		m_pctrlReLabel->setText(tr("Re.Cl ="));
		m_pctrlMachLabel->setText(tr("Mach ="));
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);
		m_PolarType = RUBBERCHORDPOLAR;
	}
	else if(m_rbtype4->isChecked())
	{
		m_pctrlReLabel->setText(tr("Alpha ="));
		m_pctrlMachLabel->setText(tr("Mach ="));
		m_pctrlReynolds->SetPrecision(2);
		m_pctrlReynolds->SetValue(m_ASpec);
		m_PolarType = FIXEDAOAPOLAR;
	}

	m_pctrlChord->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlSpan->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlMass->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlViscosity->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlDensity->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlUnit1->setEnabled(m_PolarType==FIXEDLIFTPOLAR);
	m_pctrlUnit2->setEnabled(m_PolarType==FIXEDLIFTPOLAR);

	SetPlrName();
}


void FoilPolarDlg::SetPlrName()
{
	ReadParams();

	if(m_bAutoName)
	{
		if(m_PolarType!=FIXEDAOAPOLAR)
		{
			double Re = m_Reynolds/1000000.;
			m_PlrName=QString("T%1_Re%2_M%3").arg(m_PolarType+1).arg(Re,4,'f',3).arg(m_Mach,4,'f',2);
		}
		else
		{
			m_PlrName = QString("T4_Al%1_M%2").arg(m_ASpec,5,'f',2).arg(m_Mach,4,'f',2);
		}
		QString str = QString("_N%1").arg(m_NCrit,3,'f',1);
		m_PlrName += str;

		m_pctrlAnalysisName->setText(m_PlrName);
	}
}




void FoilPolarDlg::OnUnit()
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m"+QString::fromUtf8("²")+"/s");
	}
	else
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft"+QString::fromUtf8("²")+"/s");
	}
	SetDensity();
}


void FoilPolarDlg::ReadParams()
{
    QString str;
    str = m_pctrlReynolds->text();
    str.replace(" ","");
	if(m_PolarType==FIXEDAOAPOLAR) m_ASpec    = str.toDouble();
    else          m_Reynolds = str.toDouble();

    m_Mach     = m_pctrlMach->text().toDouble();
    m_pctrlMach->clear();
    m_pctrlMach->insert(str.setNum(m_Mach,'f',2));

    m_NCrit  = m_pctrlNCrit->text().toDouble();
    m_XTopTr = m_pctrlTopTrans->text().toDouble();
    m_XBotTr = m_pctrlBotTrans->text().toDouble();

    m_Mass = m_pctrlMass->Value();
    m_Chord = m_pctrlChord->Value();
    m_Span = m_pctrlSpan->Value();
    m_Viscosity = m_pctrlViscosity->Value();
    m_Density = m_pctrlDensity->Value();
}



void FoilPolarDlg::SetDensity()
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



void FoilPolarDlg::OnEditingFinished()
{
	ReadParams();
	if(m_PolarType==2)
	{
		//compute Re.sqrt(Cl)
		m_pctrlReynolds->SetValue(sqrt(2.0*m_Mass*9.81*m_Density*m_Chord/m_Viscosity/m_Viscosity/m_Span));
	}
}


















