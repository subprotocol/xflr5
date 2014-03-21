/****************************************************************************

	W3dPrefsDlg Class
    Copyright (C) 2009-2013 Andre Deperrois XFLR5@yahoo.com

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
#include <QPushButton>
#include "../params.h"
#include "W3dPrefsDlg.h"
#include "../misc/LinePickerDlg.h"



bool W3dPrefsDlg::s_bWakePanels = false;

double W3dPrefsDlg::s_MassRadius = .01;
QColor W3dPrefsDlg::s_MassColor = QColor(130, 170, 130);

int W3dPrefsDlg::s_VLMStyle       = 0;
int W3dPrefsDlg::s_VLMWidth       = 1;
QColor W3dPrefsDlg::s_VLMColor       = QColor(180,180,180);
int W3dPrefsDlg::s_3DAxisStyle    = 3;
int W3dPrefsDlg::s_3DAxisWidth    = 1;
QColor W3dPrefsDlg::s_3DAxisColor    = QColor(150,150,150);
int W3dPrefsDlg::s_OutlineStyle   = 0;
int W3dPrefsDlg::s_OutlineWidth   = 1;
QColor W3dPrefsDlg::s_OutlineColor   = QColor(255,255,255);
int W3dPrefsDlg::s_XCPStyle       = 0;
int W3dPrefsDlg::s_XCPWidth       = 1;
QColor W3dPrefsDlg::s_XCPColor       = QColor(50, 150, 50);
int W3dPrefsDlg::s_MomentStyle    = 0;
int W3dPrefsDlg::s_MomentWidth    = 1;
QColor W3dPrefsDlg::s_MomentColor    = QColor(200, 100, 100);
int W3dPrefsDlg::s_IDragStyle     = 0;
int W3dPrefsDlg::s_IDragWidth     = 1;
QColor W3dPrefsDlg::s_IDragColor     = QColor(255,200,0);

int W3dPrefsDlg::s_DownwashStyle  = 0;
int W3dPrefsDlg::s_DownwashWidth  = 1;
QColor W3dPrefsDlg::s_DownwashColor  = QColor(255, 100, 100);
int W3dPrefsDlg::s_WakeStyle      = 0;
int W3dPrefsDlg::s_WakeWidth      = 1;
QColor W3dPrefsDlg::s_WakeColor      = QColor(0, 150, 200);
int W3dPrefsDlg::s_CpStyle        = 0;
int W3dPrefsDlg::s_CpWidth        = 1;
QColor W3dPrefsDlg::s_CpColor        = QColor(255,0,0);
int W3dPrefsDlg::s_StreamLinesStyle  = 0;
int W3dPrefsDlg::s_StreamLinesWidth  = 1;
QColor W3dPrefsDlg::s_StreamLinesColor  = QColor(200, 150, 255);

int W3dPrefsDlg::s_VDragStyle     = 0;
int W3dPrefsDlg::s_VDragWidth     = 1;
QColor W3dPrefsDlg::s_VDragColor  = QColor(200,100,220);
int W3dPrefsDlg::s_TopStyle = 0;
int W3dPrefsDlg::s_TopWidth = 1;
QColor W3dPrefsDlg::s_TopColor = QColor(171, 103, 220);
int W3dPrefsDlg::s_BotStyle = 1;
int W3dPrefsDlg::s_BotWidth = 1;
QColor W3dPrefsDlg::s_BotColor = QColor(171, 103, 220);


W3dPrefsDlg::W3dPrefsDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("3D Styles"));
//	s_pSail7  = NULL;

	SetupLayout();

	connect(m_pctrlAxis, SIGNAL(clickedLB()), SLOT(On3DAxis()));
	connect(m_pctrlOutline, SIGNAL(clickedLB()), SLOT(OnOutline()));
	connect(m_pctrlVLMMesh, SIGNAL(clickedLB()), SLOT(OnVLMMesh()));
	connect(m_pctrlTopTrans, SIGNAL(clickedLB()), SLOT(OnTopTrans()));
	connect(m_pctrlBotTrans, SIGNAL(clickedLB()), SLOT(OnBotTrans()));
	connect(m_pctrlLift, SIGNAL(clickedLB()), SLOT(OnXCP()));
	connect(m_pctrlMoments, SIGNAL(clickedLB()), SLOT(OnMoments()));
	connect(m_pctrlInducedDrag, SIGNAL(clickedLB()), SLOT(OnIDrag()));
	connect(m_pctrlViscousDrag, SIGNAL(clickedLB()), SLOT(OnVDrag()));
	connect(m_pctrlDownwash, SIGNAL(clickedLB()), SLOT(OnDownwash()));
	connect(m_pctrlStreamLines, SIGNAL(clickedLB()), SLOT(OnStreamLines()));
	connect(m_pctrlWakePanels, SIGNAL(clickedLB()), SLOT(OnWakePanels()));
	connect(m_pctrlShowWake, SIGNAL(clicked()), SLOT(OnShowWake()));
	connect(m_pctrlMassColor, SIGNAL(clicked()), SLOT(OnMasses()));
}


void W3dPrefsDlg::InitDialog()
{
	m_pctrlAxis->SetStyle(s_3DAxisStyle,s_3DAxisWidth, s_3DAxisColor);
	m_pctrlOutline->SetStyle(s_OutlineStyle, s_OutlineWidth, s_OutlineColor);
	m_pctrlVLMMesh->SetStyle(s_VLMStyle, s_VLMWidth, s_VLMColor);
	m_pctrlLift->SetStyle(s_XCPStyle, s_XCPWidth, s_XCPColor);
	m_pctrlMoments->SetStyle(s_MomentStyle, s_MomentWidth, s_MomentColor);
	m_pctrlInducedDrag->SetStyle(s_IDragStyle, s_IDragWidth, s_IDragColor);
	m_pctrlViscousDrag->SetStyle(s_VDragStyle, s_VDragWidth, s_VDragColor);
	m_pctrlDownwash->SetStyle(s_DownwashStyle, s_DownwashWidth, s_DownwashColor);
	m_pctrlWakePanels->SetStyle(s_WakeStyle, s_WakeWidth, s_WakeColor);
	m_pctrlStreamLines->SetStyle(s_StreamLinesStyle, s_StreamLinesWidth, s_StreamLinesColor);
	m_pctrlTopTrans->SetStyle(s_TopStyle, s_TopWidth, s_TopColor);
	m_pctrlBotTrans->SetStyle(s_BotStyle, s_BotWidth, s_BotColor);

	m_pctrlShowWake->setChecked(s_bWakePanels);
	m_pctrlMassColor->SetColor(s_MassColor);
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

	m_pctrlAxis     = new LineBtn(this);
	m_pctrlOutline  = new LineBtn(this);
	m_pctrlVLMMesh  = new LineBtn(this);
	m_pctrlTopTrans = new LineBtn(this);
	m_pctrlBotTrans = new LineBtn(this);
	m_pctrlLift     = new LineBtn(this);
	m_pctrlMoments  = new LineBtn(this);
	m_pctrlInducedDrag = new LineBtn(this);
	m_pctrlViscousDrag = new LineBtn(this);
	m_pctrlDownwash    = new LineBtn(this);
	m_pctrlWakePanels  = new LineBtn(this);
	m_pctrlStreamLines  = new LineBtn(this);
	m_pctrlMassColor = new ColorButton;
	m_pctrlShowWake = new QCheckBox(tr("Show Wake Panels"));

	QGridLayout *PrefsLayout = new QGridLayout;
    {
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
    }

	QHBoxLayout *CommandButtons = new QHBoxLayout;
    {
        QPushButton *OKButton = new QPushButton(tr("Close"));
        QPushButton *ResetButton = new QPushButton(tr("Reset Defaults"));
        CommandButtons->addStretch(1);
        CommandButtons->addWidget(ResetButton);
        CommandButtons->addStretch(1);
        CommandButtons->addWidget(OKButton);
        CommandButtons->addStretch(1);
        connect(ResetButton, SIGNAL(clicked()),this, SLOT(OnResetDefaults()));
        connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
    }

	QVBoxLayout *MainLayout = new QVBoxLayout;
    {
        MainLayout->addStretch(1);
        MainLayout->addLayout(PrefsLayout);
        MainLayout->addStretch(1);
        MainLayout->addSpacing(20);
        MainLayout->addLayout(CommandButtons);
        MainLayout->addStretch(1);
    }
	setLayout(MainLayout);
}


void W3dPrefsDlg::OnOutline()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_OutlineColor);
	LPdlg.SetStyle(s_OutlineStyle);
	LPdlg.SetWidth(s_OutlineWidth);
	LPdlg.InitDialog();
	if (QDialog::Accepted == LPdlg.exec())
	{
		s_OutlineColor = LPdlg.GetColor();
		s_OutlineStyle = LPdlg.GetStyle();
		s_OutlineWidth = LPdlg.GetWidth();
        m_pctrlOutline->SetStyle(s_OutlineStyle, s_OutlineWidth, s_OutlineColor);
	}
}

void W3dPrefsDlg::On3DAxis()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_3DAxisColor);
	LPdlg.SetStyle(s_3DAxisStyle);
	LPdlg.SetWidth(s_3DAxisWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_3DAxisColor = LPdlg.GetColor();
		s_3DAxisStyle = LPdlg.GetStyle();
		s_3DAxisWidth = LPdlg.GetWidth();
        m_pctrlAxis->SetStyle(s_3DAxisStyle, s_3DAxisWidth, s_3DAxisColor);
	}
}

void W3dPrefsDlg::OnTopTrans()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_TopColor);
	LPdlg.SetStyle(s_TopStyle);
	LPdlg.SetWidth(s_TopWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_TopColor = LPdlg.GetColor();
		s_TopStyle = LPdlg.GetStyle();
		s_TopWidth = LPdlg.GetWidth();
		m_pctrlTopTrans->SetStyle(s_TopStyle, s_TopWidth, s_TopColor);
	}
}

void W3dPrefsDlg::OnBotTrans()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_BotColor);
	LPdlg.SetStyle(s_BotStyle);
	LPdlg.SetWidth(s_BotWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_BotColor = LPdlg.GetColor();
		s_BotStyle = LPdlg.GetStyle();
		s_BotWidth = LPdlg.GetWidth();
		m_pctrlBotTrans->SetStyle(s_BotStyle, s_BotWidth, s_BotColor);
	}
}

void W3dPrefsDlg::OnIDrag()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_IDragColor);
	LPdlg.SetStyle(s_IDragStyle);
	LPdlg.SetWidth(s_IDragWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_IDragColor = LPdlg.GetColor();
		s_IDragStyle = LPdlg.GetStyle();
		s_IDragWidth = LPdlg.GetWidth();
		m_pctrlInducedDrag->SetStyle(s_IDragStyle, s_IDragWidth, s_IDragColor);
	}
}

void W3dPrefsDlg::OnVDrag()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_VDragColor);
	LPdlg.SetStyle(s_VDragStyle);
	LPdlg.SetWidth(s_VDragWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_VDragColor = LPdlg.GetColor();
		s_VDragStyle = LPdlg.GetStyle();
		s_VDragWidth = LPdlg.GetWidth();
		m_pctrlViscousDrag->SetStyle(s_VDragStyle, s_VDragWidth, s_VDragColor);
	}
}

void W3dPrefsDlg::OnXCP()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_XCPColor);
	LPdlg.SetStyle(s_XCPStyle);
	LPdlg.SetWidth(s_XCPWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_XCPColor = LPdlg.GetColor();
		s_XCPStyle = LPdlg.GetStyle();
		s_XCPWidth = LPdlg.GetWidth();
        m_pctrlLift->SetStyle(s_XCPStyle, s_XCPWidth, s_XCPColor);
    }
}

void W3dPrefsDlg::OnMoments()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_MomentColor);
	LPdlg.SetStyle(s_MomentStyle);
	LPdlg.SetWidth(s_MomentWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_MomentColor = LPdlg.GetColor();
		s_MomentStyle = LPdlg.GetStyle();
		s_MomentWidth = LPdlg.GetWidth();
        m_pctrlMoments->SetStyle(s_MomentStyle, s_MomentWidth, s_MomentColor);
    }
}

void W3dPrefsDlg::OnDownwash()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_DownwashColor);
	LPdlg.SetStyle(s_DownwashStyle);
	LPdlg.SetWidth(s_DownwashWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_DownwashColor = LPdlg.GetColor();
		s_DownwashStyle = LPdlg.GetStyle();
		s_DownwashWidth = LPdlg.GetWidth();
        m_pctrlDownwash->SetStyle(s_DownwashStyle, s_DownwashWidth,s_DownwashColor);
	}
}

void W3dPrefsDlg::OnStreamLines()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_StreamLinesColor);
	LPdlg.SetStyle(s_StreamLinesStyle);
	LPdlg.SetWidth(s_StreamLinesWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_StreamLinesColor = LPdlg.GetColor();
		s_StreamLinesStyle = LPdlg.GetStyle();
		s_StreamLinesWidth = LPdlg.GetWidth();
        m_pctrlStreamLines->SetStyle(s_StreamLinesStyle, s_StreamLinesWidth, s_StreamLinesColor);
	}
}

void W3dPrefsDlg::OnWakePanels()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_WakeColor);
	LPdlg.SetStyle(s_WakeStyle);
	LPdlg.SetWidth(s_WakeWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_WakeColor = LPdlg.GetColor();
		s_WakeStyle = LPdlg.GetStyle();
		s_WakeWidth = LPdlg.GetWidth();
        m_pctrlWakePanels->SetStyle(s_WakeStyle, s_WakeWidth, s_WakeColor);
	}
}

void W3dPrefsDlg::OnVLMMesh()
{
    LinePickerDlg LPdlg(this);
	LPdlg.SetColor(s_VLMColor);
	LPdlg.SetStyle(s_VLMStyle);
	LPdlg.SetWidth(s_VLMWidth);
	LPdlg.InitDialog();

	if (QDialog::Accepted == LPdlg.exec())
	{
		s_VLMColor = LPdlg.GetColor();
		s_VLMStyle = LPdlg.GetStyle();
		s_VLMWidth = LPdlg.GetWidth();
        m_pctrlVLMMesh->SetStyle(s_VLMStyle, s_VLMWidth, s_VLMColor);
	}
	repaint();
}

void W3dPrefsDlg::OnMasses()
{
    QColorDialog::ColorDialogOptions dialogOptions = QColorDialog::ShowAlphaChannel;
#ifdef Q_WS_MAC
#if QT_VERSION >= 0x040700
    dialogOptions |= QColorDialog::DontUseNativeDialog;
#endif
#endif
    QColor Color = QColorDialog::getColor(s_MassColor,
                                   this, "Select the color", dialogOptions);
	if(Color.isValid()) s_MassColor = Color;
	m_pctrlMassColor->SetColor(s_MassColor);

	update();
}

void W3dPrefsDlg::OnShowWake()
{
	s_bWakePanels = m_pctrlShowWake->isChecked();
}


void W3dPrefsDlg::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("3DPrefs");
	{
		pSettings->setValue("3DAxisStyle", s_3DAxisStyle );
		pSettings->setValue("3DAXisWidth", s_3DAxisWidth );
		pSettings->setValue("3DAxisColor", s_3DAxisColor);

		pSettings->setValue("VLMStyle", s_VLMStyle);
		pSettings->setValue("VLMWidth", s_VLMWidth);
		pSettings->setValue("VLMColor", s_VLMColor);

		pSettings->setValue("OutlineStyle", s_OutlineStyle);
		pSettings->setValue("OutlineWidth", s_OutlineWidth);
		pSettings->setValue("OutlineColor", s_OutlineColor);

		pSettings->setValue("XCPStyle", s_XCPStyle);
		pSettings->setValue("XCPWidth", s_XCPWidth);
		pSettings->setValue("XCPColor", s_XCPColor);

		pSettings->setValue("MomentStyle", s_MomentStyle);
		pSettings->setValue("MomentWidth", s_MomentWidth);
		pSettings->setValue("MomentColor", s_MomentColor);

		pSettings->setValue("IDragStyle", s_IDragStyle);
		pSettings->setValue("IDragWidth", s_IDragWidth);
		pSettings->setValue("IDragColor", s_IDragColor);

		pSettings->setValue("VDragStyle", s_VDragStyle);
		pSettings->setValue("VDragWidth", s_VDragWidth);
		pSettings->setValue("VDragColor", s_VDragColor);

		pSettings->setValue("DownwashStyle", s_DownwashStyle );
		pSettings->setValue("DownwashWidth", s_DownwashWidth );
		pSettings->setValue("DownwashColor", s_DownwashColor);

		pSettings->setValue("WakeStyle", s_WakeStyle );
		pSettings->setValue("WakeWidth", s_WakeWidth );
		pSettings->setValue("WakeColor", s_WakeColor);

		pSettings->setValue("CpStyle", s_CpStyle);
		pSettings->setValue("CpWidth", s_CpWidth);
		pSettings->setValue("CpColor", s_CpColor);

		pSettings->setValue("TopStyle", s_TopStyle);
		pSettings->setValue("TopWidth", s_TopWidth);
		pSettings->setValue("TopColor", s_TopColor);

		pSettings->setValue("BotStyle", s_BotStyle);
		pSettings->setValue("BotWidth", s_BotWidth);
		pSettings->setValue("BotColor", s_BotColor);

		pSettings->setValue("StreamLinesStyle", s_StreamLinesStyle);
		pSettings->setValue("StreamLinesWidth", s_StreamLinesWidth);
		pSettings->setValue("StreamLinesColor", s_StreamLinesColor);

		pSettings->setValue("showWakePanels", s_bWakePanels);
	}
	pSettings->endGroup();
}


void W3dPrefsDlg::LoadSettings(QSettings *pSettings)
{
	ResetDefaults();
	pSettings->beginGroup("3DPrefs");
	{
		s_3DAxisStyle   = pSettings->value("3DAxisStyle", 3).toInt();
		s_3DAxisWidth   = pSettings->value("3DAXisWidth",1).toInt();
		s_3DAxisColor = pSettings->value("3DAxisColor").value<QColor>();

		s_VLMStyle = pSettings->value("VLMStyle", 0).toInt();
		s_VLMWidth = pSettings->value("VLMWidth",1).toInt();
		s_VLMColor = pSettings->value("VLMColor").value<QColor>();

		s_OutlineStyle = pSettings->value("OutlineStyle",0).toInt();
		s_OutlineWidth = pSettings->value("OutlineWidth",1).toInt();
		s_OutlineColor = pSettings->value("OutlineColor").value<QColor>();

		s_XCPStyle = pSettings->value("XCPStyle",0).toInt();
		s_XCPWidth = pSettings->value("XCPWidth",2).toInt();
		s_XCPColor = pSettings->value("XCPColor").value<QColor>();

		s_MomentStyle = pSettings->value("MomentStyle",0).toInt();
		s_MomentWidth = pSettings->value("MomentWidth",3).toInt();
		s_MomentColor = pSettings->value("MomentColor").value<QColor>();

		s_IDragStyle = pSettings->value("IDragStyle",0).toInt();
		s_IDragWidth = pSettings->value("IDragWidth",1).toInt();
		s_IDragColor = pSettings->value("IDragColor").value<QColor>();

		s_VDragStyle = pSettings->value("VDragStyle",0).toInt();
		s_VDragWidth = pSettings->value("VDragWidth",1).toInt();
		s_VDragColor = pSettings->value("VDragColor").value<QColor>();

		s_DownwashStyle = pSettings->value("DownwashStyle",0).toInt();
		s_DownwashWidth = pSettings->value("DownwashWidth",1).toInt();
		s_DownwashColor = pSettings->value("DownwashColor").value<QColor>();

		s_WakeStyle = pSettings->value("WakeStyle",2).toInt();
		s_WakeWidth = pSettings->value("WakeWidth",1).toInt();
		s_WakeColor = pSettings->value("WakeColor").value<QColor>();

		s_CpStyle = pSettings->value("CpStyle",0).toInt();
		s_CpWidth = pSettings->value("CpWidth",1).toInt();
		s_CpColor = pSettings->value("CpColor").value<QColor>();

		s_TopStyle = pSettings->value("TopStyle",0).toInt();
		s_TopWidth = pSettings->value("TopWidth",1).toInt();
		s_TopColor = pSettings->value("TopColor",QColor(171, 103, 220)).value<QColor>();

		s_BotStyle = pSettings->value("BotStyle",1).toInt();
		s_BotWidth = pSettings->value("BotWidth",1).toInt();
		s_BotColor = pSettings->value("BotColor", QColor(171, 103, 220)).value<QColor>();

		s_StreamLinesStyle = pSettings->value("StreamLinesStyle", 0).toInt();
		s_StreamLinesWidth = pSettings->value("StreamLinesWidth", 1).toInt();
		s_StreamLinesColor = pSettings->value("StreamLinesColor", QColor(150, 140, 255)).value<QColor>();

		s_bWakePanels = pSettings->value("showWakePanels", true).toBool();
	}
	pSettings->endGroup();
}


void W3dPrefsDlg::OnResetDefaults()
{
	ResetDefaults();
	InitDialog();
}

void W3dPrefsDlg::ResetDefaults()
{
	s_bWakePanels = false;

    s_MassColor = QColor(130, 170, 130);

    s_VLMStyle       = 0;
    s_VLMWidth       = 1;
    s_VLMColor       = QColor(180,180,180);
    s_3DAxisStyle    = 3;
    s_3DAxisWidth    = 1;
    s_3DAxisColor    = QColor(150,150,150);
    s_OutlineStyle   = 0;
    s_OutlineWidth   = 1;
    s_OutlineColor   = QColor(255,255,255);
    s_XCPStyle       = 0;
    s_XCPWidth       = 1;
    s_XCPColor       = QColor(50, 150, 50);
    s_MomentStyle    = 0;
    s_MomentWidth    = 1;
    s_MomentColor    = QColor(200, 100, 100);

    s_IDragStyle     = 0;
    s_IDragWidth     = 1;
    s_IDragColor     = QColor(255,200,0);
	s_VDragStyle     = 0;
	s_VDragWidth     = 1;
	s_VDragColor     = QColor(200,100,220);

    s_DownwashStyle  = 0;
    s_DownwashWidth  = 1;
    s_DownwashColor  = QColor(255, 100, 100);
    s_WakeStyle      = 0;
    s_WakeWidth      = 1;
    s_WakeColor      = QColor(0, 150, 200);
    s_CpStyle        = 0;
    s_CpWidth        = 1;
    s_CpColor        = QColor(255,0,0);
    s_StreamLinesStyle  = 0;
    s_StreamLinesWidth  = 1;
    s_StreamLinesColor  = QColor(200, 150, 255);

    s_TopStyle = 0;
    s_TopWidth = 1;
	s_TopColor = QColor(171, 103, 220);
    s_BotStyle = 1;
    s_BotWidth = 1;
	s_BotColor = QColor(171, 103, 220);

}








