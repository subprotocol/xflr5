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

#include "../../globals.h"
#include "../../misc/Units.h"
#include "../../misc/LinePickerDlg.h"
#include "BodyGridDlg.h"


bool BodyGridDlg::s_bScale = false;

bool BodyGridDlg::s_bGrid = false;
int BodyGridDlg::s_Style = 1;
int BodyGridDlg::s_Width =1;
QColor BodyGridDlg::s_Color = QColor(150,150,150);
double BodyGridDlg::s_Unit = 0.2;

bool BodyGridDlg::s_bMinGrid = false;
int BodyGridDlg::s_MinStyle = 2;
int BodyGridDlg::s_MinWidth = 1;
QColor BodyGridDlg::s_MinColor=QColor(75,75,75);
double BodyGridDlg::s_MinorUnit=0.05;


bool BodyGridDlg::s_bGrid2 = false;
int BodyGridDlg::s_Style2 = 1;
int BodyGridDlg::s_Width2 = 1;
QColor BodyGridDlg::s_Color2 = QColor(150,150,150);
double BodyGridDlg::s_Unit2 = 0.01;

bool BodyGridDlg::s_bMinGrid2 = false;
int BodyGridDlg::s_MinStyle2 = 2;
int BodyGridDlg::s_MinWidth2 = 1;
QColor BodyGridDlg::s_MinColor2=QColor(75,75,75);
double BodyGridDlg::s_MinorUnit2=0.002;


BodyGridDlg::BodyGridDlg(QWidget *pParent):QDialog(pParent)
{
	setWindowTitle(tr("Body Grid Dialog"));

	SetupLayout();
}



void BodyGridDlg::InitDialog()
{
	QString length;
	Units::getLengthUnitLabel(length);
	m_pctrlLength1->setText(length);
	m_pctrlLength2->setText(length);
	m_pctrlLength3->setText(length);
	m_pctrlLength4->setText(length);

	m_pctrlScales->setChecked(s_bScale);
	m_pctrlGrid->setChecked(s_bGrid);
	m_pctrlLine->SetStyle(s_Style);
	m_pctrlLine->SetWidth(s_Width);
	m_pctrlLine->SetColor(s_Color);
	m_pctrlMinGrid->setChecked(s_bMinGrid);
	m_pctrlMinLine->SetStyle(s_MinStyle);
	m_pctrlMinLine->SetWidth(s_MinWidth);
	m_pctrlMinLine->SetColor(s_MinColor);
	m_pctrlUnit->SetValue(s_Unit*Units::mtoUnit());
	m_pctrlMinUnit->SetValue(s_MinorUnit*Units::mtoUnit());

	m_pctrlGrid2->setChecked(s_bGrid2);
	m_pctrlLine2->SetStyle(s_Style2);
	m_pctrlLine2->SetWidth(s_Width2);
	m_pctrlLine2->SetColor(s_Color2);
	m_pctrlMinGrid2->setChecked(s_bMinGrid2);
	m_pctrlMinLine2->SetStyle(s_MinStyle2);
	m_pctrlMinLine2->SetWidth(s_MinWidth2);
	m_pctrlMinLine2->SetColor(s_MinColor2);
	m_pctrlUnit2->SetValue(s_Unit2*Units::mtoUnit());
	m_pctrlMinUnit2->SetValue(s_MinorUnit2*Units::mtoUnit());

	EnableControls();
}



void BodyGridDlg::SetupLayout()
{
	setWindowTitle(tr("Grid Parameters"));

	m_pctrlScales   = new QCheckBox(tr("Show Scales"));
	m_pctrlGrid     = new QCheckBox(tr("Main Grid"));
	m_pctrlGrid2    = new QCheckBox(tr("Main Grid"));
	m_pctrlMinGrid  = new QCheckBox(tr("Minor Grid"));
	m_pctrlMinGrid2 = new QCheckBox(tr("Minor Grid"));

	m_pctrlLine  = new LineBtn(this);
	m_pctrlLine2 = new LineBtn(this);
	m_pctrlMinLine  = new LineBtn(this);
	m_pctrlMinLine2 = new LineBtn(this);

	m_pctrlUnit  = new DoubleEdit(100.00);
	m_pctrlUnit2 = new DoubleEdit(101.00);
	m_pctrlMinUnit  = new DoubleEdit(102.00);
	m_pctrlMinUnit2 = new DoubleEdit(103.00);
	m_pctrlUnit->SetPrecision(3);
	m_pctrlUnit2->SetPrecision(3);
	m_pctrlMinUnit->SetPrecision(3);
	m_pctrlMinUnit2->SetPrecision(3);

	m_pctrlLength1 = new QLabel("mm");
	m_pctrlLength2 = new QLabel("mm");
	m_pctrlLength3 = new QLabel("mm");
	m_pctrlLength4 = new QLabel("mm");

	QGroupBox *pBodyBox = new QGroupBox(tr("Body Grid"));
	{
		QGridLayout *pBodyLayout = new QGridLayout;
		{
			pBodyLayout->addWidget(m_pctrlGrid,1,1);
			pBodyLayout->addWidget(m_pctrlLine,1,2);
			pBodyLayout->addWidget(m_pctrlUnit,1,3);
			pBodyLayout->addWidget(m_pctrlLength1, 1,4);
			pBodyLayout->addWidget(m_pctrlMinGrid,2,1);
			pBodyLayout->addWidget(m_pctrlMinLine,2,2);
			pBodyLayout->addWidget(m_pctrlMinUnit,2,3);
			pBodyLayout->addWidget(m_pctrlLength2, 2,4);
		}
		pBodyBox->setLayout(pBodyLayout);
	}


	QGroupBox *pFrameBox = new QGroupBox(tr("Frame Grid"));
	{
		QGridLayout *pFrameLayout = new QGridLayout;
		{
			pFrameLayout->addWidget(m_pctrlGrid2,1,1);
			pFrameLayout->addWidget(m_pctrlLine2,1,2);
			pFrameLayout->addWidget(m_pctrlUnit2,1,3);
			pFrameLayout->addWidget(m_pctrlLength3, 1,4);
			pFrameLayout->addWidget(m_pctrlMinGrid2,2,1);
			pFrameLayout->addWidget(m_pctrlMinLine2,2,2);
			pFrameLayout->addWidget(m_pctrlMinUnit2,2,3);
			pFrameLayout->addWidget(m_pctrlLength4, 2,4);
		}
		pFrameBox->setLayout(pFrameLayout);
	}

	QHBoxLayout *pCommandButtons = new QHBoxLayout;
	{
		QPushButton *OKButton = new QPushButton(tr("OK"));
		QPushButton *Cancel = new QPushButton(tr("Cancel"));
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(OKButton);
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(Cancel);
		pCommandButtons->addStretch(1);
		connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
		connect(Cancel, SIGNAL(clicked()), this, SLOT(reject()));
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addWidget(m_pctrlScales);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pBodyBox);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pFrameBox);
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pCommandButtons);
		pMainLayout->addStretch(1);
	}

	setLayout(pMainLayout);


	connect(m_pctrlGrid, SIGNAL(clicked()), this, SLOT(OnGrid()));
	connect(m_pctrlMinGrid, SIGNAL(clicked()), this, SLOT(OnMinGrid()));
	connect(m_pctrlGrid2, SIGNAL(clicked()), this, SLOT(OnGrid2()));
	connect(m_pctrlMinGrid2, SIGNAL(clicked()), this, SLOT(OnMinGrid2()));

	connect(m_pctrlLine, SIGNAL(clickedLB()), this, SLOT(OnLineStyle()));
	connect(m_pctrlLine2, SIGNAL(clickedLB()), this, SLOT(OnLine2Style()));
	connect(m_pctrlMinLine, SIGNAL(clickedLB()), this, SLOT(OnMinLineStyle()));
	connect(m_pctrlMinLine2, SIGNAL(clickedLB()), this, SLOT(OnMinLine2Style()));
}


void BodyGridDlg::EnableControls()
{
	m_pctrlLine->setEnabled(s_bGrid);
	m_pctrlUnit->setEnabled(s_bGrid);
	m_pctrlMinLine->setEnabled(s_bMinGrid);
	m_pctrlMinUnit->setEnabled(s_bMinGrid);

	m_pctrlLine2->setEnabled(s_bGrid2);
	m_pctrlUnit2->setEnabled(s_bGrid2);
	m_pctrlMinLine2->setEnabled(s_bMinGrid2);
	m_pctrlMinUnit2->setEnabled(s_bMinGrid2);
}



void BodyGridDlg::OnOK()
{
	s_bScale = m_pctrlScales->isChecked();
	accept();
}



void BodyGridDlg::OnGrid()
{
	s_bGrid = m_pctrlGrid->isChecked();
	EnableControls();
}


void BodyGridDlg::OnGrid2()
{
	s_bGrid2 = m_pctrlGrid2->isChecked();
	EnableControls();
}

void BodyGridDlg::OnMinGrid()
{
	s_bMinGrid = m_pctrlMinGrid->isChecked();
	EnableControls();
}


void BodyGridDlg::OnMinGrid2()
{
	s_bMinGrid2 = m_pctrlMinGrid2->isChecked();
	EnableControls();
}


void BodyGridDlg::OnLineStyle()
{
	LinePickerDlg dlg(this);
	dlg.InitDialog(s_Style, s_Width, s_Color);

	if(QDialog::Accepted==dlg.exec())
	{
		s_Style = dlg.GetStyle();
		s_Width = dlg.GetWidth();
		s_Color = dlg.GetColor();
		m_pctrlLine->SetStyle(s_Style);
		m_pctrlLine->SetWidth(s_Width);
		m_pctrlLine->SetColor(s_Color);
	}
}


void BodyGridDlg::OnLine2Style()
{
	LinePickerDlg dlg(this);
	dlg.InitDialog(s_Style2, s_Width2, s_Color2);

	if(QDialog::Accepted==dlg.exec())
	{
		s_Style2 = dlg.GetStyle();
		s_Width2 = dlg.GetWidth();
		s_Color2 = dlg.GetColor();
		m_pctrlLine2->SetStyle(s_Style2);
		m_pctrlLine2->SetWidth(s_Width2);
		m_pctrlLine2->SetColor(s_Color2);
	}
}


void BodyGridDlg::OnMinLineStyle()
{
	LinePickerDlg dlg(this);
	dlg.InitDialog(s_MinStyle, s_MinWidth, s_MinColor);

	if(QDialog::Accepted==dlg.exec())
	{
		s_MinStyle = dlg.GetStyle();
		s_MinWidth = dlg.GetWidth();
		s_MinColor = dlg.GetColor();
		m_pctrlMinLine->SetStyle(s_MinStyle);
		m_pctrlMinLine->SetWidth(s_MinWidth);
		m_pctrlMinLine->SetColor(s_MinColor);
	}
}


void BodyGridDlg::OnMinLine2Style()
{
	LinePickerDlg dlg(this);
	dlg.InitDialog(s_MinStyle2, s_MinWidth2, s_MinColor2);

	if(QDialog::Accepted==dlg.exec())
	{
		s_MinStyle2 = dlg.GetStyle();
		s_MinWidth2 = dlg.GetWidth();
		s_MinColor2 = dlg.GetColor();
		m_pctrlMinLine2->SetStyle(s_MinStyle2);
		m_pctrlMinLine2->SetWidth(s_MinWidth2);
		m_pctrlMinLine2->SetColor(s_MinColor2);
	}
}



void BodyGridDlg::LoadSettings (QSettings *pSettings)
{
	pSettings->beginGroup("GL3dBodyGrid");
	{
		s_bGrid      = pSettings->value("Grid").toBool();
		s_bMinGrid   = pSettings->value("MinGrid").toBool();
		s_bGrid2     = pSettings->value("Grid2").toBool();
		s_bMinGrid2  = pSettings->value("MinGrid2").toBool();
		s_Style      = pSettings->value("Style").toInt();
		s_MinStyle   = pSettings->value("MinStyle").toInt();
		s_Style2     = pSettings->value("Style2").toInt();
		s_MinStyle2  = pSettings->value("MinStyle2").toInt();
		s_Width      = pSettings->value("Width").toInt();
		s_MinWidth   = pSettings->value("MinWidth").toInt();
		s_Width2     = pSettings->value("Width2").toInt();
		s_MinWidth2  = pSettings->value("MinWidth2").toInt();
		s_Color      = pSettings->value("Color").value<QColor>();
		s_MinColor   = pSettings->value("MinColor").value<QColor>();
		s_Color2     = pSettings->value("Color2").value<QColor>();
		s_MinColor2  = pSettings->value("MinColor2").value<QColor>();
		s_Unit       = pSettings->value("Unit").toDouble();
		s_MinorUnit  = pSettings->value("MinorUnit").toDouble();
		s_Unit2      = pSettings->value("Unit2").toDouble();
		s_MinorUnit2 = pSettings->value("MinorUnit2").toDouble();
		s_bScale     = pSettings->value("bScale").toBool();
	}
	pSettings->endGroup();
}

void BodyGridDlg::SaveSettings (QSettings *pSettings)
{
	pSettings->beginGroup("GL3dBodyGrid");
	{
		pSettings->setValue("Grid", s_bGrid);
		pSettings->setValue("MinGrid", s_bMinGrid);
		pSettings->setValue("Grid2", s_bGrid2);
		pSettings->setValue("MinGrid2", s_bMinGrid2);
		pSettings->setValue("Style", s_Style);
		pSettings->setValue("MinStyle", s_MinStyle);
		pSettings->setValue("Style2", s_Style2);
		pSettings->setValue("MinStyle2", s_MinStyle2);
		pSettings->setValue("Width", s_Width);
		pSettings->setValue("MinWidth", s_MinWidth);
		pSettings->setValue("Width2", s_Width2);
		pSettings->setValue("MinWidth2", s_MinWidth2);
		pSettings->setValue("Color", s_Color);
		pSettings->setValue("MinColor", s_MinColor);
		pSettings->setValue("Color2", s_Color2);
		pSettings->setValue("MinColor2", s_MinColor2);
		pSettings->setValue("Unit", s_Unit);
		pSettings->setValue("MinorUnit", s_MinorUnit);
		pSettings->setValue("Unit2", s_Unit2);
		pSettings->setValue("MinorUnit2", s_MinorUnit2);
		pSettings->setValue("bScale", s_bScale);
	}
	pSettings->endGroup();
}






