/****************************************************************************

	WAdvancedDlg Class
	Copyright (C) 2009-2010 Andre Deperrois adeperrois@xflr5.com

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

#include "../globals.h"
#include "../mainframe.h"
#include "WAdvancedDlg.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>



WAdvancedDlg::WAdvancedDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Wing Analysis Advanced Settings"));

	m_NStation  = 20;
	m_AlphaPrec = 0.01;
	m_Relax     = 20.;
	m_Iter      = 100;

	m_MaxWakeIter     = 1;
	m_CoreSize        = 0.0001;// 0.1mm
	m_WakeInterNodes  = 6;
	m_MinPanelSize    = .001;

	m_InducedDragPoint = 0;

	m_bResetWake      = true;
	m_bDirichlet      = true;
	m_bLogFile        = true;
	m_bKeepOutOpps    = true;

	m_ControlPos = 0.75;
	m_VortexPos  = 0.25;

	m_bVLM1 = true;

	SetupLayout();
}


void WAdvancedDlg::SetupLayout()
{
	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	m_pctrlAStat   = new QLabel("");
	m_pctrlLength  = new QLabel("");
	m_pctrlLength2 = new QLabel("");

	m_pctrlLogFile     = new QCheckBox(tr("View Log File after errors"));
	m_pctrlResetWake   = new QCheckBox(tr("Reset Wake between each angle"));
	m_pctrlKeepOutOpps = new QCheckBox(tr("Store points outside the polar mesh"));
	m_pctrlLogFile->setSizePolicy(szPolicyMinimum);
	m_pctrlResetWake->setSizePolicy(szPolicyMinimum);
	m_pctrlKeepOutOpps->setSizePolicy(szPolicyMinimum);

	m_pctrlDirichlet = new QRadioButton("Dirichlet");
	m_pCtrlNeumann = new QRadioButton("Neumann");
	m_pctrlDirichlet->setSizePolicy(szPolicyMinimum);
	m_pCtrlNeumann->setSizePolicy(szPolicyMinimum);

	m_pctrlVLM1 = new QRadioButton(tr("Horseshoe vortex")+" (VLM1)");
	m_pctrlVLM2 = new QRadioButton(tr("Ring vortex")+" (VLM2)");
	m_pctrlVLM1->setSizePolicy(szPolicyMinimum);
	m_pctrlVLM2->setSizePolicy(szPolicyMinimum);

    m_pctrlInterNodes   = new DoubleEdit();
    m_pctrlRelax        = new DoubleEdit(20,1);
    m_pctrlAlphaPrec    = new DoubleEdit(.01, 4);
    m_pctrlMinPanelSize = new DoubleEdit(1.00,2);
    m_pctrlNStation     = new DoubleEdit(20,0);
    m_pctrlIterMax      = new DoubleEdit(100,0);
    m_pctrlMaxWakeIter  = new DoubleEdit(5,0);
    m_pctrlCoreSize     = new DoubleEdit(.0001, 4);
    m_pctrlVortexPos    = new DoubleEdit(25.0, 2);
    m_pctrlControlPos   = new DoubleEdit(75.0, 2);
	m_pctrlInterNodes->setSizePolicy(szPolicyMaximum);
	m_pctrlRelax->setSizePolicy(szPolicyMaximum);
	m_pctrlAlphaPrec->setSizePolicy(szPolicyMaximum);
	m_pctrlMinPanelSize->setSizePolicy(szPolicyMaximum);
	m_pctrlNStation->setSizePolicy(szPolicyMaximum);
	m_pctrlIterMax->setSizePolicy(szPolicyMaximum);
	m_pctrlMaxWakeIter->setSizePolicy(szPolicyMaximum);
	m_pctrlCoreSize->setSizePolicy(szPolicyMaximum);
	m_pctrlVortexPos->setSizePolicy(szPolicyMaximum);
	m_pctrlControlPos->setSizePolicy(szPolicyMaximum);

	QGroupBox *AllBox = new QGroupBox(tr("All Analysis"));
	QHBoxLayout *AllLayout = new QHBoxLayout;
	AllLayout->addWidget(m_pctrlLogFile);
	AllLayout->addWidget(m_pctrlKeepOutOpps);
	AllBox->setLayout(AllLayout);

	QGroupBox *VLMPanelBox = new QGroupBox(tr("VLM and Panel Methods"));
	QHBoxLayout *WingPanelLayout = new QHBoxLayout;
	QLabel *lab5 = new QLabel(tr("Ignore wing panels with span <"));
	lab5->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	WingPanelLayout->addStretch(1);
	WingPanelLayout->addWidget(lab5);
	WingPanelLayout->addWidget(m_pctrlMinPanelSize);
	WingPanelLayout->addWidget(m_pctrlLength);
	QHBoxLayout *CoreSizeLayout = new QHBoxLayout;
	QLabel *lab10 = new QLabel(tr("Core Size"));
	lab10->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	CoreSizeLayout->addStretch(1);
	CoreSizeLayout->addWidget(lab10);
	CoreSizeLayout->addWidget(m_pctrlCoreSize);
	CoreSizeLayout->addWidget(m_pctrlLength2);
	QVBoxLayout *VLMPanelLayout = new QVBoxLayout;
	VLMPanelLayout->addLayout(WingPanelLayout);
	VLMPanelLayout->addLayout(CoreSizeLayout);
	VLMPanelBox->setLayout(VLMPanelLayout);

	QGroupBox *VLMBox = new QGroupBox(tr("VLM Method"));
	QGridLayout *VLMLayout = new QGridLayout;
	QLabel *lab6 = new QLabel(tr("Vortex Position"));
	QLabel *lab7 = new QLabel(tr("Control Point Position"));
	QLabel *lab8 = new QLabel("%");
	QLabel *lab9 = new QLabel("%");
	QVBoxLayout *VLMMethodLayout = new QVBoxLayout;
	VLMMethodLayout->addWidget(m_pctrlVLM1);
	VLMMethodLayout->addWidget(m_pctrlVLM2);
	lab6->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab7->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab8->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab9->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	VLMLayout->addWidget(lab6,1,1);
	VLMLayout->addWidget(lab7,2,1);
	VLMLayout->addWidget(m_pctrlVortexPos,1,2);
	VLMLayout->addWidget(m_pctrlControlPos,2,2);
	VLMLayout->addWidget(lab8,1,3);
	VLMLayout->addWidget(lab9,2,3);
	VLMLayout->addLayout(VLMMethodLayout,4,1,1,2, Qt::AlignLeft);
	VLMBox->setLayout(VLMLayout);

	QGroupBox *LLTBox = new QGroupBox(tr("Lifting Line Method"));
	QGridLayout *LLTLayout = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("Relax. factor"));
	QLabel *lab2 = new QLabel(tr("Alpha Precision"));
	QLabel *lab3 = new QLabel(tr("Max. Iterations"));
	QLabel *lab4 = new QLabel(tr("Number of spanwise stations"));
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

	QGroupBox *PanelBCBox = new QGroupBox(tr("3D Panel boundary conditions"));
	QVBoxLayout *PanelBCLayout = new QVBoxLayout;
	PanelBCLayout->addWidget(m_pctrlDirichlet);
	PanelBCLayout->addWidget(m_pCtrlNeumann);
	PanelBCBox->setLayout((PanelBCLayout));

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
	LeftSide->addStretch(1);
	LeftSide->addWidget(PanelBCBox);
	LeftSide->addStretch(1);
	RightSide->addWidget(VLMBox);
	RightSide->addStretch(1);
	RightSide->addWidget(VLMPanelBox);
	RightSide->addStretch(1);
	BothSides->addLayout(LeftSide);
	BothSides->addLayout(RightSide);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(BothSides);
	MainLayout->addStretch(1);
	MainLayout->addWidget(AllBox);
	MainLayout->addStretch(1);
	MainLayout->addSpacing(30);
	MainLayout->addLayout(CommandButtons);

	setSizePolicy(szPolicyMaximum);

	setLayout(MainLayout);

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
	GetLengthUnit(len,MainFrame::s_LengthUnit);

	m_pctrlAStat->setText(len);
	m_pctrlLength->setText(len);
	m_pctrlLength2->setText(len);

	m_pctrlVortexPos->setEnabled(false);
	m_pctrlControlPos->setEnabled(false);
	m_pctrlDirichlet->setChecked(m_bDirichlet);
	m_pCtrlNeumann->setChecked(!m_bDirichlet);
	m_pctrlVLM1->setChecked( m_bVLM1);
	m_pctrlVLM2->setChecked(!m_bVLM1);
}


void WAdvancedDlg::OnOK()
{
	ReadParams();
	accept();
}


void WAdvancedDlg::OnResetDefaults()
{
	m_Relax            = 20.0;
	m_AlphaPrec        = 0.01;
	m_Iter             = 100;
	m_NStation         = 20;
	m_MaxWakeIter      = 5;
	m_CoreSize         = 0.0001;
	m_MinPanelSize     = .001;
	m_WakeInterNodes   = 6;
	m_bLogFile         = true;
	m_VortexPos        = 0.25;
	m_ControlPos       = 0.75;
	m_bDirichlet       = true;
	m_bResetWake       = true;
	m_bTrefftz         = true;
	m_bKeepOutOpps     = false;
	m_bVLM1            = true;
	SetParams();
}


void WAdvancedDlg::ReadParams()
{
	m_Relax           = m_pctrlRelax->Value();
	m_AlphaPrec       = m_pctrlAlphaPrec->Value();
	m_MaxWakeIter     = m_pctrlMaxWakeIter->Value();
	m_CoreSize        = m_pctrlCoreSize->Value() / MainFrame::s_mtoUnit;
	m_MinPanelSize    = m_pctrlMinPanelSize->Value() / MainFrame::s_mtoUnit;
	m_VortexPos       = m_pctrlVortexPos->Value()/100.0;
	m_ControlPos      = m_pctrlControlPos->Value()/100.0;
	m_WakeInterNodes  = (int)m_pctrlInterNodes->Value();
	m_Iter            = (int)m_pctrlIterMax->Value();;
	m_NStation        = (int)m_pctrlNStation->Value();
	m_bDirichlet      = m_pctrlDirichlet->isChecked();
	m_bTrefftz        = true;
	m_bResetWake      = m_pctrlResetWake->isChecked();
	m_bKeepOutOpps    = m_pctrlKeepOutOpps->isChecked();
	m_bLogFile        = m_pctrlLogFile->isChecked();
	m_bVLM1           = m_pctrlVLM1->isChecked();
}


void WAdvancedDlg::SetParams()
{
	m_pctrlIterMax->SetValue(m_Iter);
	m_pctrlRelax->SetValue(m_Relax);
	m_pctrlAlphaPrec->SetValue(m_AlphaPrec);
	m_pctrlNStation->SetValue(m_NStation);

	m_pctrlCoreSize->SetValue(m_CoreSize* MainFrame::s_mtoUnit);
	m_pctrlMaxWakeIter->SetValue(m_MaxWakeIter);
	m_pctrlInterNodes->SetValue(m_WakeInterNodes);

	m_pctrlMinPanelSize->SetValue(m_MinPanelSize * MainFrame::s_mtoUnit);

	m_pctrlResetWake->setChecked(m_bResetWake);
	m_pctrlLogFile->setChecked(m_bLogFile);
	m_pctrlKeepOutOpps->setChecked(m_bKeepOutOpps);

	m_pctrlControlPos->SetValue(m_ControlPos*100.0);
	m_pctrlVortexPos->SetValue(m_VortexPos*100.0);
	m_pctrlVLM1->setChecked(m_bVLM1);
}



