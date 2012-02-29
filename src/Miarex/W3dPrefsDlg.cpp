/****************************************************************************

	W3dPrefsDlg Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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

#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QColorDialog>
#include "W3dPrefsDlg.h"
#include "../Misc/LinePickerDlg.h"

W3dPrefsDlg::W3dPrefsDlg()
{
	setWindowTitle(tr("3D Styles"));
	m_pMiarex = NULL;
	SetupLayout();

	connect(m_pctrlAxis, SIGNAL(clicked()), SLOT(On3DAxis()));
	connect(m_pctrlOutline, SIGNAL(clicked()), SLOT(OnWingColor()));
	connect(m_pctrlVLMMesh, SIGNAL(clicked()), SLOT(OnVLMMesh()));
	connect(m_pctrlTopTrans, SIGNAL(clicked()), SLOT(OnTopTrans()));
	connect(m_pctrlBotTrans, SIGNAL(clicked()), SLOT(OnBotTrans()));
	connect(m_pctrlLift, SIGNAL(clicked()), SLOT(OnXCP()));
	connect(m_pctrlMoments, SIGNAL(clicked()), SLOT(OnMoments()));
	connect(m_pctrlInducedDrag, SIGNAL(clicked()), SLOT(OnIDrag()));
	connect(m_pctrlViscousDrag, SIGNAL(clicked()), SLOT(OnVDrag()));
	connect(m_pctrlDownwash, SIGNAL(clicked()), SLOT(OnDownwash()));
	connect(m_pctrlStreamLines, SIGNAL(clicked()), SLOT(OnStreamLines()));
	connect(m_pctrlWakePanels, SIGNAL(clicked()), SLOT(OnWakePanels()));
	connect(m_pctrlShowWake, SIGNAL(clicked()), SLOT(OnShowWake()));
	connect(m_pctrlMassColor, SIGNAL(clicked()), SLOT(OnMasses()));
}


void W3dPrefsDlg::InitDialog()
{
	m_pctrlAxis->SetStyle(m_3DAxisStyle,m_3DAxisWidth, m_3DAxisColor);
	m_pctrlOutline->SetStyle(m_OutlineStyle, m_OutlineWidth, m_OutlineColor);
	m_pctrlVLMMesh->SetStyle(m_VLMStyle, m_VLMWidth, m_VLMColor);
	m_pctrlTopTrans->SetStyle(m_TopStyle, m_TopWidth, m_TopColor);
	m_pctrlBotTrans->SetStyle(m_BotStyle, m_BotWidth, m_BotColor);
	m_pctrlLift->SetStyle(m_XCPStyle, m_XCPWidth, m_XCPColor);
	m_pctrlMoments->SetStyle(m_MomentStyle, m_MomentWidth, m_MomentColor);
	m_pctrlInducedDrag->SetStyle(m_IDragStyle, m_IDragWidth, m_IDragColor);
	m_pctrlViscousDrag->SetStyle(m_VDragStyle, m_VDragWidth, m_VDragColor);
	m_pctrlDownwash->SetStyle(m_DownwashStyle, m_DownwashWidth, m_DownwashColor);
	m_pctrlWakePanels->SetStyle(m_WakeStyle, m_WakeWidth, m_WakeColor);
	m_pctrlStreamLines->SetStyle(m_StreamLinesStyle, m_StreamLinesWidth, m_StreamLinesColor);
	m_pctrlMassColor->SetColor(m_MassColor);
	m_pctrlShowWake->setChecked(m_bWakePanels);
}

void W3dPrefsDlg::SetupLayout()
{
	QLabel *lab1 = new QLabel(tr("Axis"));
	QLabel *lab2 = new QLabel(tr("Outline"));
	QLabel *lab3 = new QLabel(tr("VLM Mesh"));
	QLabel *lab4 = new QLabel(tr("Top transition"));
	QLabel *lab5 = new QLabel(tr("Bottom transition"));
	QLabel *lab6 = new QLabel(tr("Lift"));
	QLabel *lab7 = new QLabel(tr("Moments"));
	QLabel *lab8 = new QLabel(tr("Induced Drag"));
	QLabel *lab9 = new QLabel(tr("Viscous Drag"));
	QLabel *lab10 = new QLabel(tr("Downwash"));
	QLabel *lab11 = new QLabel(tr("WakePanels"));
	QLabel *lab12 = new QLabel(tr("Streamlines"));
	QLabel *lab13 = new QLabel(tr("Masses"));
	lab1->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab2->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab3->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab4->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab5->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab6->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab7->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab8->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab9->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab10->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab11->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab12->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab13->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

	m_pctrlAxis     = new LineButton;
	m_pctrlOutline  = new LineButton;
	m_pctrlVLMMesh  = new LineButton;
	m_pctrlTopTrans = new LineButton;
	m_pctrlBotTrans = new LineButton;
	m_pctrlLift     = new LineButton;
	m_pctrlMoments  = new LineButton;
	m_pctrlInducedDrag = new LineButton;
	m_pctrlViscousDrag = new LineButton;
	m_pctrlDownwash    = new LineButton;
	m_pctrlWakePanels  = new LineButton;
	m_pctrlStreamLines  = new LineButton;
	m_pctrlMassColor = new ColorButton;
	m_pctrlShowWake = new QCheckBox(tr("Show Wake Panels"));

	QGridLayout *PrefsLayout = new QGridLayout;
	PrefsLayout->setColumnStretch(1,1);
	PrefsLayout->setColumnStretch(2,2);
	PrefsLayout->setColumnStretch(3,1);
	PrefsLayout->setColumnStretch(4,2);
	PrefsLayout->addWidget(lab1,1,1);
	PrefsLayout->addWidget(lab2,2,1);
	PrefsLayout->addWidget(lab3,3,1);
	PrefsLayout->addWidget(lab4,4,1);
	PrefsLayout->addWidget(lab5,5,1);
	PrefsLayout->addWidget(lab6,6,1);
	PrefsLayout->addWidget(lab7,1,3);
	PrefsLayout->addWidget(lab8,2,3);
	PrefsLayout->addWidget(lab9,3,3);
	PrefsLayout->addWidget(lab10,4,3);
	PrefsLayout->addWidget(lab11,5,3);
	PrefsLayout->addWidget(lab12,6,3);
	PrefsLayout->addWidget(lab13,7,3);

	PrefsLayout->addWidget(m_pctrlAxis,1,2);
	PrefsLayout->addWidget(m_pctrlOutline,2,2);
	PrefsLayout->addWidget(m_pctrlVLMMesh,3,2);
	PrefsLayout->addWidget(m_pctrlTopTrans,4,2);
	PrefsLayout->addWidget(m_pctrlBotTrans,5,2);
	PrefsLayout->addWidget(m_pctrlLift,6,2);
	PrefsLayout->addWidget(m_pctrlShowWake,7,1,1,2);
	PrefsLayout->addWidget(m_pctrlMoments,1,4);
	PrefsLayout->addWidget(m_pctrlInducedDrag,2,4);
	PrefsLayout->addWidget(m_pctrlViscousDrag,3,4);
	PrefsLayout->addWidget(m_pctrlDownwash,4,4);
	PrefsLayout->addWidget(m_pctrlWakePanels,5,4);
	PrefsLayout->addWidget(m_pctrlStreamLines,6,4);
	PrefsLayout->addWidget(m_pctrlMassColor,7,4);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(PrefsLayout);
	MainLayout->addStretch(1);
	MainLayout->addSpacing(20);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}

void W3dPrefsDlg::OnMasses()
{
	QColor Color = QColorDialog::getColor(m_MassColor,
								   this, "Select the color", QColorDialog::ShowAlphaChannel);
	if(Color.isValid()) m_MassColor = Color;
	m_pctrlMassColor->SetColor(m_MassColor);

	update();
}

void W3dPrefsDlg::OnWingColor()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_OutlineColor);
	LPdlg.SetStyle(m_OutlineStyle);
	LPdlg.SetWidth(m_OutlineWidth);
	LPdlg.InitDialog();
	if (QDialog::Accepted == LPdlg.exec())
	{
		m_OutlineColor = LPdlg.GetColor();
		m_OutlineStyle = LPdlg.GetStyle();
		m_OutlineWidth = LPdlg.GetWidth();
		m_pctrlOutline->SetStyle(m_OutlineStyle, m_OutlineWidth, m_OutlineColor);
	}
}


void W3dPrefsDlg::On3DAxis()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_3DAxisColor);
	LPdlg.SetStyle(m_3DAxisStyle);
	LPdlg.SetWidth(m_3DAxisWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_3DAxisColor = LPdlg.GetColor();
		m_3DAxisStyle = LPdlg.GetStyle();
		m_3DAxisWidth = LPdlg.GetWidth();
		m_pctrlAxis->SetStyle(m_3DAxisStyle,m_3DAxisWidth, m_3DAxisColor);
	}
}


void W3dPrefsDlg::OnTopTrans()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_TopColor);
	LPdlg.SetStyle(m_TopStyle);
	LPdlg.SetWidth(m_TopWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_TopColor = LPdlg.GetColor();
		m_TopStyle = LPdlg.GetStyle();
		m_TopWidth = LPdlg.GetWidth();
		m_pctrlTopTrans->SetStyle(m_TopStyle, m_TopWidth, m_TopColor);
	}
}

void W3dPrefsDlg::OnBotTrans()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_BotColor);
	LPdlg.SetStyle(m_BotStyle);
	LPdlg.SetWidth(m_BotWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_BotColor = LPdlg.GetColor();
		m_BotStyle = LPdlg.GetStyle();
		m_BotWidth = LPdlg.GetWidth();
		m_pctrlBotTrans->SetStyle(m_BotStyle, m_BotWidth, m_BotColor);
	}
}

void W3dPrefsDlg::OnIDrag()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_IDragColor);
	LPdlg.SetStyle(m_IDragStyle);
	LPdlg.SetWidth(m_IDragWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_IDragColor = LPdlg.GetColor();
		m_IDragStyle = LPdlg.GetStyle();
		m_IDragWidth = LPdlg.GetWidth();
		m_pctrlInducedDrag->SetStyle(m_IDragStyle, m_IDragWidth, m_IDragColor);
	}
}

void W3dPrefsDlg::OnVDrag()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_VDragColor);
	LPdlg.SetStyle(m_VDragStyle);
	LPdlg.SetWidth(m_VDragWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_VDragColor = LPdlg.GetColor();
		m_VDragStyle = LPdlg.GetStyle();
		m_VDragWidth = LPdlg.GetWidth();
		m_pctrlViscousDrag->SetStyle(m_VDragStyle, m_VDragWidth, m_VDragColor);
	}
}

void W3dPrefsDlg::OnXCP()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_XCPColor);
	LPdlg.SetStyle(m_XCPStyle);
	LPdlg.SetWidth(m_XCPWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_XCPColor = LPdlg.GetColor();
		m_XCPStyle = LPdlg.GetStyle();
		m_XCPWidth = LPdlg.GetWidth();
		m_pctrlLift->SetStyle(m_XCPStyle, m_XCPWidth, m_XCPColor);
	}
}

void W3dPrefsDlg::OnMoments()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_MomentColor);
	LPdlg.SetStyle(m_MomentStyle);
	LPdlg.SetWidth(m_MomentWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_MomentColor = LPdlg.GetColor();
		m_MomentStyle = LPdlg.GetStyle();
		m_MomentWidth = LPdlg.GetWidth();
		m_pctrlMoments->SetStyle(m_MomentStyle, m_MomentWidth, m_MomentColor);
	}
}

void W3dPrefsDlg::OnDownwash()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_DownwashColor);
	LPdlg.SetStyle(m_DownwashStyle);
	LPdlg.SetWidth(m_DownwashWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_DownwashColor = LPdlg.GetColor();
		m_DownwashStyle = LPdlg.GetStyle();
		m_DownwashWidth = LPdlg.GetWidth();
		m_pctrlDownwash->SetStyle(m_DownwashStyle, m_DownwashWidth, m_DownwashColor);
	}
}

void W3dPrefsDlg::OnStreamLines()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_StreamLinesColor);
	LPdlg.SetStyle(m_StreamLinesStyle);
	LPdlg.SetWidth(m_StreamLinesWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_StreamLinesColor = LPdlg.GetColor();
		m_StreamLinesStyle = LPdlg.GetStyle();
		m_StreamLinesWidth = LPdlg.GetWidth();
		m_pctrlStreamLines->SetStyle(m_StreamLinesStyle, m_StreamLinesWidth, m_StreamLinesColor);
	}
	update();
}


void W3dPrefsDlg::OnWakePanels()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_WakeColor);
	LPdlg.SetStyle(m_WakeStyle);
	LPdlg.SetWidth(m_WakeWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_WakeColor = LPdlg.GetColor();
		m_WakeStyle = LPdlg.GetStyle();
		m_WakeWidth = LPdlg.GetWidth();
		m_pctrlWakePanels->SetStyle(m_WakeStyle, m_WakeWidth, m_WakeColor);
	}
}


void W3dPrefsDlg::OnVLMMesh()
{
	LinePickerDlg LPdlg;
	LPdlg.SetColor(m_VLMColor);
	LPdlg.SetStyle(m_VLMStyle);
	LPdlg.SetWidth(m_VLMWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		m_VLMColor = LPdlg.GetColor();
		m_VLMStyle = LPdlg.GetStyle();
		m_VLMWidth = LPdlg.GetWidth();
		m_pctrlVLMMesh->SetStyle(m_VLMStyle, m_VLMWidth, m_VLMColor);
	}
}


void W3dPrefsDlg::OnShowWake()
{
	m_bWakePanels = m_pctrlShowWake->isChecked();

}


