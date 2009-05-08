/****************************************************************************

	WAdvancedDlg Class
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
#include "WAdvancedDlg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>


WAdvancedDlg::WAdvancedDlg()
{
	setWindowTitle("Wing Analysis Advanced Settings");
	m_NStation  = 20;
	m_AlphaPrec = 0.01;
	m_Relax     = 20.;
	m_Iter      = 100;

	m_MaxWakeIter     = 1;
	m_CoreSize        = 0.0;
	m_WakeInterNodes  = 6;
	m_MinPanelSize    = .001;

	m_InducedDragPoint = 0;

	m_bResetWake      = true;
	m_bDirichlet      = true;
	m_bLogFile        = true;
	m_bKeepOutOpps    = true;


	m_pMainFrame  = NULL;
	m_ControlPos = 0.75;
	m_VortexPos  = 0.25;

	SetupLayout();
}


void WAdvancedDlg::SetupLayout()
{
	m_pctrlAStat   = new QLabel("");
	m_pctrlLength  = new QLabel("");
	m_pctrlLength2 = new QLabel("");

	m_pctrlLogFile     = new QCheckBox("View Log File after errors");
	m_pctrlResetWake   = new QCheckBox("Reset Wake between each angle");
	m_pctrlKeepOutOpps = new QCheckBox("Store points outside the polar mesh");

	m_pctrlInterNodes   = new FloatEdit();
	m_pctrlRelax        = new FloatEdit(20,1);
	m_pctrlAlphaPrec    = new FloatEdit(.01, 4);
	m_pctrlMinPanelSize = new FloatEdit(1.00,2);
	m_pctrlNStation     = new FloatEdit(20,0);
	m_pctrlIterMax      = new FloatEdit(100,0);
	m_pctrlMaxWakeIter  = new FloatEdit(5,0);
	m_pctrlCoreSize     = new FloatEdit(.0001, 4);
	m_pctrlVortexPos    = new FloatEdit(25.0, 2);
	m_pctrlControlPos   = new FloatEdit(75.0, 2);

	QGroupBox *AllBox = new QGroupBox("All Analysis");
	QVBoxLayout *AllLayout = new QVBoxLayout;
	AllLayout->addWidget(m_pctrlLogFile);
	AllLayout->addWidget(m_pctrlKeepOutOpps);
	AllBox->setLayout(AllLayout);

	QGroupBox *VLMPanelBox = new QGroupBox("VLM and Panel Methods");
	QHBoxLayout *VLMPanelLayout = new QHBoxLayout;
	QLabel *lab10 = new QLabel("Core Size");
	lab10->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	VLMPanelLayout->addStretch(1);
	VLMPanelLayout->addWidget(lab10);
	VLMPanelLayout->addWidget(m_pctrlCoreSize);
	VLMPanelLayout->addWidget(m_pctrlLength2);
	VLMPanelBox->setLayout(VLMPanelLayout);

	QGroupBox *VLMBox = new QGroupBox("VLM Method");
	QGridLayout *VLMLayout = new QGridLayout;
	QLabel *lab5 = new QLabel("Ignore wing panels with span <");
	QLabel *lab6 = new QLabel("Vortex Position");
	QLabel *lab7 = new QLabel("Control Point Position");
	QLabel *lab8 = new QLabel("%");
	QLabel *lab9 = new QLabel("%");
	lab5->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab6->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab7->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab8->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab9->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	VLMLayout->addWidget(lab5,1,1);
	VLMLayout->addWidget(lab6,2,1);
	VLMLayout->addWidget(lab7,3,1);
	VLMLayout->addWidget(m_pctrlMinPanelSize,1,2);
	VLMLayout->addWidget(m_pctrlVortexPos,2,2);
	VLMLayout->addWidget(m_pctrlControlPos,3,2);
	VLMLayout->addWidget(m_pctrlLength,1,3);
	VLMLayout->addWidget(lab8,2,3);
	VLMLayout->addWidget(lab9,3,3);
	VLMBox->setLayout(VLMLayout);

	QGroupBox *LLTBox = new QGroupBox("Lifitng Line Method");
	QGridLayout *LLTLayout = new QGridLayout;
	QLabel *lab1 = new QLabel("Relax. factor");
	QLabel *lab2 = new QLabel("Alpha Precision");
	QLabel *lab3 = new QLabel("Max. Iterations");
	QLabel *lab4 = new QLabel("Number of spanwise stations");
	lab1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab4->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LLTLayout->addWidget(lab1,1,1);
	LLTLayout->addWidget(lab2,2,1);
	LLTLayout->addWidget(lab3,3,1);
	LLTLayout->addWidget(lab4,4,1);
	LLTLayout->addWidget(m_pctrlRelax,1,2);
	LLTLayout->addWidget(m_pctrlAlphaPrec,2,2);
	LLTLayout->addWidget(m_pctrlIterMax,3,2);
	LLTLayout->addWidget(m_pctrlNStation,4,2);
	LLTBox->setLayout(LLTLayout);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	QPushButton *ResetButton = new QPushButton(tr("Reset Defaults"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ResetButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ResetButton, SIGNAL(clicked()), this, SLOT(OnResetDefaults()));

	QVBoxLayout *LeftSide  = new QVBoxLayout;
	QVBoxLayout *RightSide = new QVBoxLayout;
	QHBoxLayout *BothSides = new QHBoxLayout;
	LeftSide->addWidget(LLTBox);
	LeftSide->addWidget(VLMPanelBox);
	LeftSide->addStretch(1);
	RightSide->addWidget(VLMBox);
	RightSide->addWidget(AllBox);
	RightSide->addStretch(1);
	BothSides->addLayout(LeftSide);
	BothSides->addLayout(RightSide);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(BothSides);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);

//	connect(m_pctrlLogFile, SIGNAL(clicked()), SLOT(OnLogFile()));
//	connect(m_pctrlResetWake, SIGNAL(clicked()), SLOT(OnResetWake()));
//	connect(m_pctrlKeepOutOpps, SIGNAL(clicked()), SLOT(OnKeepOutOpps()));
}


void WAdvancedDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			done(0);
			return;
		}
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus())
			{
				OKButton->setFocus();
				return;
			}
			break;
		}
		default:
			event->ignore();
			break;
	}
}



void WAdvancedDlg::InitDialog()
{
	SetParams();

	QString len;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	GetLengthUnit(len,pMainFrame->m_LengthUnit);

	m_pctrlAStat->setText(len);
	m_pctrlLength->setText(len);
	m_pctrlLength2->setText(len);

	m_pctrlVortexPos->setEnabled(false);
	m_pctrlControlPos->setEnabled(false);
}


void WAdvancedDlg::OnOK()
{
	ReadParams();
	accept();
}


void WAdvancedDlg::OnResetDefaults()
{
	m_Relax           = 20.0;
	m_AlphaPrec       = 0.01;
	m_Iter            = 100;
	m_NStation        = 20;
	m_MaxWakeIter     = 5;
	m_CoreSize        = 0.00001;
	m_MinPanelSize    = .001;
	m_WakeInterNodes  = 6;
	m_bLogFile        = true;
	m_VortexPos       = 0.25;
	m_ControlPos      = 0.75;
	m_bDirichlet      = true;
	m_bResetWake      = true;
	m_bTrefftz        = true;
	m_bKeepOutOpps    = false;
	SetParams();
}


void WAdvancedDlg::ReadParams()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_Relax           = m_pctrlRelax->GetValue();
	m_AlphaPrec       = m_pctrlAlphaPrec->GetValue();
	m_MaxWakeIter     = m_pctrlMaxWakeIter->GetValue();
	m_CoreSize        = m_pctrlCoreSize->GetValue() / pMainFrame->m_mtoUnit;
	m_MinPanelSize    = m_pctrlMinPanelSize->GetValue() / pMainFrame->m_mtoUnit;
	m_VortexPos       = m_pctrlVortexPos->GetValue()/100.0;
	m_ControlPos      = m_pctrlControlPos->GetValue()/100.0;
	m_WakeInterNodes  = (int)m_pctrlInterNodes->GetValue();
	m_Iter            = (int)m_pctrlIterMax->GetValue();;
	m_NStation        = (int)m_pctrlNStation->GetValue();
	m_bDirichlet      = true;
	m_bTrefftz        = true;
	m_bResetWake      = m_pctrlResetWake->isChecked();
	m_bKeepOutOpps    = m_pctrlKeepOutOpps->isChecked();
	m_bLogFile        = m_pctrlLogFile->isChecked();

}


void WAdvancedDlg::SetParams()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_pctrlIterMax->SetValue(m_Iter);
	m_pctrlRelax->SetValue(m_Relax);
	m_pctrlAlphaPrec->SetValue(m_AlphaPrec);
	m_pctrlNStation->SetValue(m_NStation);

	m_pctrlCoreSize->SetValue(m_CoreSize* pMainFrame->m_mtoUnit);
	m_pctrlMaxWakeIter->SetValue(m_MaxWakeIter);
	m_pctrlInterNodes->SetValue(m_WakeInterNodes);

	m_pctrlMinPanelSize->SetValue(m_MinPanelSize * pMainFrame->m_mtoUnit);

	m_pctrlResetWake->setChecked(m_bResetWake);
	m_pctrlLogFile->setChecked(m_bLogFile);
	m_pctrlKeepOutOpps->setChecked(m_bKeepOutOpps);

/*	if(m_bDirichlet) CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	else			 CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);

	if(m_bTrefftz)   CheckRadioButton(IDC_RADIO3, IDC_RADIO4, IDC_RADIO3);
	else			 CheckRadioButton(IDC_RADIO3, IDC_RADIO4, IDC_RADIO4);

	if(m_InducedDragPoint==0)
					CheckRadioButton(IDC_RADIO5, IDC_RADIO6, IDC_RADIO5);
	else			CheckRadioButton(IDC_RADIO5, IDC_RADIO6, IDC_RADIO6);*/

	m_pctrlControlPos->SetValue(m_ControlPos*100.0);
	m_pctrlVortexPos->SetValue(m_VortexPos*100.0);
}



