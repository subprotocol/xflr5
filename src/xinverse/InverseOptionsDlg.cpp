/****************************************************************************

	InverseOptionsDlg  Classes
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

#include "XInverse.h"
#include "InverseOptionsDlg.h"
#include "../misc/LinePickerDlg.h"

InverseOptionsDlg::InverseOptionsDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("XInverse Style"));
	m_pXInverse = NULL;
	SetupLayout();

}

void InverseOptionsDlg::SetupLayout()
{
	m_pctrlRefFoil   = new LineBtn(this);
	m_pctrlModFoil   = new LineBtn(this);
	m_pctrlSpline    = new LineBtn(this);
	m_pctrlReflected = new LineBtn(this);

	QLabel * lab1 = new QLabel(tr("Reference Foil"));
	QLabel * lab2 = new QLabel(tr("Modified Foil"));
	QLabel * lab3 = new QLabel(tr("Spline"));
	QLabel * lab4 = new QLabel(tr("Reflected Curve"));
	lab1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab2->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab3->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	lab4->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	QGridLayout *StyleLayout = new QGridLayout;
	StyleLayout->addWidget(lab1,1,1);
	StyleLayout->addWidget(lab2,2,1);
	StyleLayout->addWidget(lab3,3,1);
	StyleLayout->addWidget(lab4,4,1);
	StyleLayout->addWidget(m_pctrlRefFoil,1,2);
	StyleLayout->addWidget(m_pctrlModFoil,2,2);
	StyleLayout->addWidget(m_pctrlSpline,3,2);
	StyleLayout->addWidget(m_pctrlReflected,4,2);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton      = new QPushButton(tr("OK"));
	QPushButton *CancelButton  = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(StyleLayout);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);

    connect(m_pctrlRefFoil,   SIGNAL(clickedLB()),this, SLOT(OnRefStyle()));
    connect(m_pctrlModFoil,   SIGNAL(clickedLB()),this, SLOT(OnModStyle()));
    connect(m_pctrlSpline,    SIGNAL(clickedLB()),this, SLOT(OnSplineStyle()));
    connect(m_pctrlReflected, SIGNAL(clickedLB()),this, SLOT(OnReflectedStyle()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

void InverseOptionsDlg::InitDialog()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	m_pctrlRefFoil->SetStyle(pXInverse->m_pRefFoil->m_FoilStyle, pXInverse->m_pRefFoil->m_FoilWidth, pXInverse->m_pRefFoil->m_FoilColor);
	m_pctrlModFoil->SetStyle(pXInverse->m_pModFoil->m_FoilStyle, pXInverse->m_pModFoil->m_FoilWidth, pXInverse->m_pModFoil->m_FoilColor);
	m_pctrlSpline->SetStyle(pXInverse->m_Spline.style(), pXInverse->m_Spline.width(), pXInverse->m_Spline.color());
	m_pctrlReflected->SetStyle(pXInverse->m_ReflectedStyle, pXInverse->m_ReflectedWidth, pXInverse->m_ReflectedClr);
}

void InverseOptionsDlg::OnRefStyle()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
    LinePickerDlg dlg(this);
	dlg.InitDialog(pXInverse->m_pRefFoil->m_FoilStyle, pXInverse->m_pRefFoil->m_FoilWidth, pXInverse->m_pRefFoil->m_FoilColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pctrlRefFoil->SetStyle(dlg.GetStyle(),dlg.GetWidth(),dlg.GetColor());
		pXInverse->m_pRefFoil->m_FoilStyle = dlg.GetStyle();
		pXInverse->m_pRefFoil->m_FoilWidth = dlg.GetWidth();
		pXInverse->m_pRefFoil->m_FoilColor  = dlg.GetColor();
	}
}


void InverseOptionsDlg::OnModStyle()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
    LinePickerDlg dlg(this);
	dlg.InitDialog(pXInverse->m_pModFoil->m_FoilStyle, pXInverse->m_pModFoil->m_FoilWidth, pXInverse->m_pModFoil->m_FoilColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pctrlModFoil->SetStyle(dlg.GetStyle(),dlg.GetWidth(),dlg.GetColor());
		pXInverse->m_pModFoil->m_FoilStyle = dlg.GetStyle();
		pXInverse->m_pModFoil->m_FoilWidth = dlg.GetWidth();
		pXInverse->m_pModFoil->m_FoilColor  = dlg.GetColor();
	}
}

void InverseOptionsDlg::OnSplineStyle()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
    LinePickerDlg dlg(this);
	dlg.InitDialog(pXInverse->m_Spline.style(), pXInverse->m_Spline.width(), pXInverse->m_Spline.color());

	if(QDialog::Accepted==dlg.exec())
	{
		m_pctrlModFoil->SetStyle(dlg.GetStyle(),dlg.GetWidth(),dlg.GetColor());
		pXInverse->m_Spline.SetStyle(dlg.GetStyle());
		pXInverse->m_Spline.SetWidth(dlg.GetWidth());
		pXInverse->m_Spline.SetColor(dlg.GetColor());
	}
}

void InverseOptionsDlg::OnReflectedStyle()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
    LinePickerDlg dlg(this);
	dlg.InitDialog(pXInverse->m_ReflectedStyle, pXInverse->m_ReflectedWidth, pXInverse->m_ReflectedClr);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pctrlModFoil->SetStyle(dlg.GetStyle(),dlg.GetWidth(),dlg.GetColor());
		pXInverse->m_ReflectedStyle = dlg.GetStyle();
		pXInverse->m_ReflectedWidth = dlg.GetWidth();
		pXInverse->m_ReflectedClr   = dlg.GetColor();
	}
}
