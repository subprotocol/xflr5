/****************************************************************************

	FoilPolarDlg Class
	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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

#include "FoilPolarDlg.h"
#include "XDirect.h"

FoilPolarDlg::FoilPolarDlg(void *pParent)
{
	m_pParent = pParent;
	m_Type     = 1;
	m_NCrit    = 9.0;
	m_XTopTr   = 1.0;
	m_XBotTr   = 1.0;
	m_Mach     = 0.0;
	m_Reynolds = 100000.0;
	m_ASpec    = 0.0;
	m_bAutoName = true;

//	setMinimumHeight(400);
	SetupLayout();
}

void FoilPolarDlg::SetupLayout()
{
	QVBoxLayout *Analysis = new QVBoxLayout;
	QHBoxLayout *autoname = new QHBoxLayout;
	m_pctrlAuto1 = new QRadioButton("Automatic");
	m_pctrlAuto2 = new QRadioButton("User Defined");
	m_pctrlAnalysisName = new QLineEdit("Analysis Name");
	autoname->addStretch(1);
	autoname->addWidget(m_pctrlAuto1);
	autoname->addStretch(1);
	autoname->addWidget(m_pctrlAuto2);
	autoname->addStretch(1);
	Analysis->addLayout(autoname);
	Analysis->addWidget(m_pctrlAnalysisName);
	QGroupBox *NameGroup = new QGroupBox("Analysis Name");
	NameGroup->setLayout(Analysis);

	QHBoxLayout *AnalysisType = new QHBoxLayout;
	m_rbtype1 = new		QRadioButton("Type 1");
	m_rbtype2 = new QRadioButton("Type 2");
	m_rbtype3 = new QRadioButton("Type 3");
	m_rbtype4 = new QRadioButton("Type 4");
	AnalysisType->addWidget(m_rbtype1);
	AnalysisType->addWidget(m_rbtype2);
	AnalysisType->addWidget(m_rbtype3);
	AnalysisType->addWidget(m_rbtype4);
	QGroupBox *TypeGroup = new QGroupBox("Analysis Type");
	TypeGroup->setLayout(AnalysisType);


	QHBoxLayout *AeroData = new QHBoxLayout;
	m_pctrlReLabel   = new QLabel("  Re =");
	m_pctrlReLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlMachLabel = new QLabel("Mach =");
	m_pctrlMachLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlReynolds = new FloatEdit();
	m_pctrlReynolds->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlMach = new FloatEdit();
	m_pctrlMach->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	AeroData->addWidget(m_pctrlReLabel);
	AeroData->addWidget(m_pctrlReynolds);
	AeroData->addWidget(m_pctrlMachLabel);
	AeroData->addWidget(m_pctrlMach);
	QGroupBox *AeroGroup = new QGroupBox("Aerodynamic data");
	AeroGroup->setLayout(AeroData);

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
	QLabel *FreeTransLabel   = new QLabel("Free transitions (e^n) method");
	QLabel *ForceTransLabel  = new QLabel("Forced transition:");
	QLabel	*NCritLabel      = new QLabel("NCrit=");
	QLabel *TopTripLabel     = new QLabel("TripLocation (top)");
	QLabel *BotTripLabel     = new QLabel("TripLocation (bot)");
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
	QGroupBox *TransGroup = new QGroupBox("Transition settings");
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
}

void FoilPolarDlg::EditingFinished()
{
	SetPlrName();
//	OKButton->setFocus();
}

void FoilPolarDlg::InitDialog()
{
	QXDirect* pXDirect = (QXDirect*)m_pParent;

	if(pXDirect && pXDirect->m_pCurFoil) m_FoilName = pXDirect->m_pCurFoil->m_FoilName;

	QString str = "Analysis parameters for "+ m_FoilName;
	setWindowTitle(str);

	m_pctrlReynolds->SetValue(m_Reynolds);
	m_pctrlMach->SetValue(m_Mach);
	m_pctrlNCrit->SetValue(m_NCrit);
	m_pctrlTopTrans->SetValue(m_XTopTr);
	m_pctrlBotTrans->SetValue(m_XBotTr);

	m_bAutoName = true;
	m_pctrlAuto1->setChecked(true);

	switch(m_Type)
	{
		case 1:
		{
			m_rbtype1->setChecked(true);
			break;
		}
		case 2:
		{
			m_rbtype2->setChecked(true);
			break;
		}
		case 3:
		{
			m_rbtype3->setChecked(true);
			break;
		}
		case 4:
		{
			m_rbtype4->setChecked(true);
			break;
		}
	}

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
		m_pctrlReLabel->setText("Reynolds =");
		m_pctrlMachLabel->setText("Mach =");
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);
		m_Type = 1;
	}
	else if(m_rbtype2->isChecked())
	{
		m_pctrlReLabel->setText("Re.sqrt(Cl) =");
		m_pctrlMachLabel->setText("Ma.sqrt(Cl) =");
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);
		m_Type = 2;
	}
	else if(m_rbtype3->isChecked())
	{
		m_pctrlReLabel->setText("Re.Cl =");
		m_pctrlMachLabel->setText("Mach =");
		m_pctrlReynolds->SetPrecision(0);
		m_pctrlReynolds->SetValue(m_Reynolds);
		m_Type = 3;
	}
	else if(m_rbtype4->isChecked())
	{
		m_pctrlReLabel->setText("Alpha =");
		m_pctrlMachLabel->setText("Mach =");
		m_pctrlReynolds->SetPrecision(2);
		m_pctrlReynolds->SetValue(m_ASpec);
		m_Type = 4;
	}

//	m_ctrlRetyp.Invalidate();
//	m_ctrlMatyp.Invalidate();
	SetPlrName();
}


void FoilPolarDlg::SetPlrName()
{
    ReadParams();
    QString Name;

    if(m_bAutoName)
    {
        if(m_Type!=4)
        {
                double Re = m_Reynolds/1000000.;

                m_PlrName=QString("T%1_Re%2_M%3").arg(m_Type).arg(Re,4,'f',2).arg(m_Mach,4,'f',2);
        }
        else
        {
                m_PlrName = QString("T%1_Al%2_M%3").arg(m_Type).arg(m_ASpec,5,'f',2).arg(m_Mach,4,'f',2);
        }
		QString str = QString("_N%1").arg(m_NCrit,3,'f',1);
        m_PlrName += str;

        m_pctrlAnalysisName->setText(m_PlrName);
    }
}


void FoilPolarDlg::ReadParams()
{
    QString str;
    str = m_pctrlReynolds->text();
    str.replace(" ","");
    if(m_Type==4) m_ASpec    = str.toDouble();
    else          m_Reynolds = str.toDouble();

    m_Mach     = m_pctrlMach->text().toDouble();
    m_pctrlMach->clear();
    m_pctrlMach->insert(str.setNum(m_Mach,'f',2));

    m_NCrit  = m_pctrlNCrit->text().toDouble();
    m_XTopTr = m_pctrlTopTrans->text().toDouble();
    m_XBotTr = m_pctrlBotTrans->text().toDouble();
}






















