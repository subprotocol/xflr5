/****************************************************************************

	BodyGridDlg Class
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


#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QLabel>
#include <QPushButton>
#include "../mainframe.h"
#include "../globals.h"
#include "../misc/LinePickerDlg.h"
#include "BodyGridDlg.h"

void *BodyGridDlg::s_pMainFrame;

BodyGridDlg::BodyGridDlg(QWidget *pParent):QDialog(pParent)
{
	setWindowTitle(tr("Body Grid Dialog"));
	m_bScale = false;

	m_bGrid = false;
	m_Style = 1; m_Width =1;
	m_Color = QColor(150,150,150);
	m_Unit = 0.2;

	m_bMinGrid = false;
	m_MinStyle=2; m_MinWidth =1;
	m_MinColor=QColor(75,75,75);
	m_MinorUnit=0.05;


	m_bGrid2 = false;
	m_Style2 = 1; m_Width2 =1;
	m_Color2 = QColor(150,150,150);
	m_Unit2 = 0.01;

	m_bMinGrid2 = false;
	m_MinStyle2=2; m_MinWidth2 =1;
	m_MinColor2=QColor(75,75,75);
	m_MinorUnit2=0.002;

	SetupLayout();
}



void BodyGridDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString length;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	m_pctrlLength1->setText(length);
	m_pctrlLength2->setText(length);
	m_pctrlLength3->setText(length);
	m_pctrlLength4->setText(length);

	m_pctrlScales->setChecked(m_bScale);
	m_pctrlGrid->setChecked(m_bGrid);
	m_pctrlLine->SetStyle(m_Style);
	m_pctrlLine->SetWidth(m_Width);
	m_pctrlLine->SetColor(m_Color);
	m_pctrlMinGrid->setChecked(m_bMinGrid);
	m_pctrlMinLine->SetStyle(m_MinStyle);
	m_pctrlMinLine->SetWidth(m_MinWidth);
	m_pctrlMinLine->SetColor(m_MinColor);
	m_pctrlUnit->SetValue(m_Unit);
	m_pctrlMinUnit->SetValue(m_MinorUnit);

	m_pctrlGrid2->setChecked(m_bGrid2);
	m_pctrlLine2->SetStyle(m_Style2);
	m_pctrlLine2->SetWidth(m_Width2);
	m_pctrlLine2->SetColor(m_Color2);
	m_pctrlMinGrid2->setChecked(m_bMinGrid2);
	m_pctrlMinLine2->SetStyle(m_MinStyle2);
	m_pctrlMinLine2->SetWidth(m_MinWidth2);
	m_pctrlMinLine2->SetColor(m_MinColor2);
	m_pctrlUnit2->SetValue(m_Unit2);
	m_pctrlMinUnit2->SetValue(m_MinorUnit2);

	EnableControls();
}



void BodyGridDlg::SetupLayout()
{
	setWindowTitle(tr("Grid Parameters"));
//	QDesktopWidget desktop;
//	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
//	move(r.width()/3, r.height()/6);

	m_pctrlScales   = new QCheckBox(tr("Show Scales"));
	m_pctrlGrid     = new QCheckBox(tr("Main Grid"));
	m_pctrlGrid2    = new QCheckBox(tr("Main Grid"));
	m_pctrlMinGrid  = new QCheckBox(tr("Minor Grid"));
	m_pctrlMinGrid2 = new QCheckBox(tr("Minor Grid"));

	m_pctrlLine  = new LineButton;
	m_pctrlLine2 = new LineButton;
	m_pctrlMinLine  = new LineButton;
	m_pctrlMinLine2 = new LineButton;

	m_pctrlUnit  = new FloatEdit(100.00);
	m_pctrlUnit2 = new FloatEdit(101.00);
	m_pctrlMinUnit  = new FloatEdit(102.00);
	m_pctrlMinUnit2 = new FloatEdit(103.00);
	m_pctrlUnit->SetPrecision(3);
	m_pctrlUnit2->SetPrecision(3);
	m_pctrlMinUnit->SetPrecision(3);
	m_pctrlMinUnit2->SetPrecision(3);

	m_pctrlLength1 = new QLabel("mm");
	m_pctrlLength2 = new QLabel("mm");

	QGridLayout *BodyLayout = new QGridLayout;
	BodyLayout->addWidget(m_pctrlGrid,1,1);
	BodyLayout->addWidget(m_pctrlLine,1,2);
	BodyLayout->addWidget(m_pctrlUnit,1,3);
	BodyLayout->addWidget(m_pctrlLength1, 1,4);
	BodyLayout->addWidget(m_pctrlMinGrid,2,1);
	BodyLayout->addWidget(m_pctrlMinLine,2,2);
	BodyLayout->addWidget(m_pctrlMinUnit,2,3);
	BodyLayout->addWidget(m_pctrlLength2, 2,4);

	QGroupBox *BodyBox = new QGroupBox(tr("Body Grid"));
	BodyBox->setLayout(BodyLayout);

	m_pctrlLength3 = new QLabel("mm");
	m_pctrlLength4 = new QLabel("mm");

	QGridLayout *FrameLayout = new QGridLayout;
	FrameLayout->addWidget(m_pctrlGrid2,1,1);
	FrameLayout->addWidget(m_pctrlLine2,1,2);
	FrameLayout->addWidget(m_pctrlUnit2,1,3);
	FrameLayout->addWidget(m_pctrlLength3, 1,4);
	FrameLayout->addWidget(m_pctrlMinGrid2,2,1);
	FrameLayout->addWidget(m_pctrlMinLine2,2,2);
	FrameLayout->addWidget(m_pctrlMinUnit2,2,3);
	FrameLayout->addWidget(m_pctrlLength4, 2,4);

	QGroupBox *FrameBox = new QGroupBox(tr("Frame Grid"));
	FrameBox->setLayout(FrameLayout);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	QPushButton *Cancel = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(Cancel);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pctrlScales);
	MainLayout->addStretch(1);
	MainLayout->addWidget(BodyBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(FrameBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(Cancel, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlGrid, SIGNAL(clicked()), this, SLOT(OnGrid()));
	connect(m_pctrlMinGrid, SIGNAL(clicked()), this, SLOT(OnMinGrid()));
	connect(m_pctrlGrid2, SIGNAL(clicked()), this, SLOT(OnGrid2()));
	connect(m_pctrlMinGrid2, SIGNAL(clicked()), this, SLOT(OnMinGrid2()));

	connect(m_pctrlLine, SIGNAL(clicked()), this, SLOT(OnLineStyle()));
	connect(m_pctrlLine2, SIGNAL(clicked()), this, SLOT(OnLine2Style()));
	connect(m_pctrlMinLine, SIGNAL(clicked()), this, SLOT(OnMinLineStyle()));
	connect(m_pctrlMinLine2, SIGNAL(clicked()), this, SLOT(OnMinLine2Style()));
}


void BodyGridDlg::EnableControls()
{
	m_pctrlLine->setEnabled(m_bGrid);
	m_pctrlUnit->setEnabled(m_bGrid);
	m_pctrlMinLine->setEnabled(m_bMinGrid);
	m_pctrlMinUnit->setEnabled(m_bMinGrid);

	m_pctrlLine2->setEnabled(m_bGrid2);
	m_pctrlUnit2->setEnabled(m_bGrid2);
	m_pctrlMinLine2->setEnabled(m_bMinGrid2);
	m_pctrlMinUnit2->setEnabled(m_bMinGrid2);
}



void BodyGridDlg::OnOK()
{
	m_bScale = m_pctrlScales->isChecked();
	accept();
}



void BodyGridDlg::OnGrid()
{
	m_bGrid = m_pctrlGrid->isChecked();
	EnableControls();
}


void BodyGridDlg::OnGrid2()
{
	m_bGrid2 = m_pctrlGrid2->isChecked();
	EnableControls();
}

void BodyGridDlg::OnMinGrid()
{
	m_bMinGrid = m_pctrlMinGrid->isChecked();
	EnableControls();
}


void BodyGridDlg::OnMinGrid2()
{
	m_bMinGrid2 = m_pctrlMinGrid2->isChecked();
	EnableControls();
}


void BodyGridDlg::OnLineStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_Style, m_Width, m_Color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_Style = dlg.GetStyle();
		m_Width = dlg.GetWidth();
		m_Color = dlg.GetColor();
		m_pctrlLine->SetStyle(m_Style);
		m_pctrlLine->SetWidth(m_Width);
		m_pctrlLine->SetColor(m_Color);
	}
}


void BodyGridDlg::OnLine2Style()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_Style2, m_Width2, m_Color2);

	if(QDialog::Accepted==dlg.exec())
	{
		m_Style2 = dlg.GetStyle();
		m_Width2 = dlg.GetWidth();
		m_Color2 = dlg.GetColor();
		m_pctrlLine2->SetStyle(m_Style2);
		m_pctrlLine2->SetWidth(m_Width2);
		m_pctrlLine2->SetColor(m_Color2);
	}
}


void BodyGridDlg::OnMinLineStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_MinStyle, m_MinWidth, m_MinColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_MinStyle = dlg.GetStyle();
		m_MinWidth = dlg.GetWidth();
		m_MinColor = dlg.GetColor();
		m_pctrlMinLine->SetStyle(m_MinStyle);
		m_pctrlMinLine->SetWidth(m_MinWidth);
		m_pctrlMinLine->SetColor(m_MinColor);
	}
}


void BodyGridDlg::OnMinLine2Style()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_MinStyle2, m_MinWidth2, m_MinColor2);

	if(QDialog::Accepted==dlg.exec())
	{
		m_MinStyle2 = dlg.GetStyle();
		m_MinWidth2 = dlg.GetWidth();
		m_MinColor2 = dlg.GetColor();
		m_pctrlMinLine2->SetStyle(m_MinStyle2);
		m_pctrlMinLine2->SetWidth(m_MinWidth2);
		m_pctrlMinLine2->SetColor(m_MinColor2);
	}
}










