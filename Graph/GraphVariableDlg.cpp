/****************************************************************************

	GraphVariableDlg  Classes
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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

#include <QtGui>
#include "GraphVariableDlg.h"

GraphVariableDlg::GraphVariableDlg()
{
	m_XSel = 0;
	m_YSel = 1;
	SetupLayout();
}


void GraphVariableDlg::SetupLayout()
{
	QHBoxLayout *AxisNames = new QHBoxLayout;
	QLabel *YAxis = new QLabel("YAxis");
	QLabel *vs = new QLabel("vs.");
	QLabel *XAxis = new QLabel("XAxis");
	AxisNames->addStretch(1);
	AxisNames->addWidget(YAxis);
	AxisNames->addStretch(1);
	AxisNames->addWidget(vs);
	AxisNames->addStretch(1);
	AxisNames->addWidget(XAxis);
	AxisNames->addStretch(1);

	QHBoxLayout *VariableBox = new QHBoxLayout;
	m_pctrlXSel = new QListWidget;
	m_pctrlYSel = new QListWidget;
	m_pctrlXSel->setMinimumWidth(40);
	m_pctrlYSel->setMinimumWidth(40);
	m_pctrlXSel->setMinimumHeight(350);
	m_pctrlYSel->setMinimumHeight(350);
	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlYSel);
	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlXSel);
	VariableBox->addStretch(1);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout(this);
	MainLayout->addStretch(1);
	MainLayout->addLayout(AxisNames);
	MainLayout->addLayout(VariableBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}

void GraphVariableDlg::InitDialog(int iGraphType)
{
	FillLists(iGraphType);
}

void GraphVariableDlg::FillLists(int iGraphType)
{
	if(iGraphType==1)
	{
		//foil polar graph variables
		m_pctrlXSel->addItem("Alpha");
		m_pctrlXSel->addItem("Cl");
		m_pctrlXSel->addItem("Cd");
		m_pctrlXSel->addItem("Cd x 10000");
		m_pctrlXSel->addItem("Cdp");
		m_pctrlXSel->addItem("Cm");
		m_pctrlXSel->addItem("Xtr1");
		m_pctrlXSel->addItem("Xtr2");
		m_pctrlXSel->addItem("HMom");
		m_pctrlXSel->addItem("Cpmin");
		m_pctrlXSel->addItem("Cl/Cd");
		m_pctrlXSel->addItem("|Cl|^(3/2)/Cd");
		m_pctrlXSel->addItem("1/Cl^1/2");
		m_pctrlXSel->addItem("Re");
		m_pctrlXSel->addItem("XCp");

		m_pctrlYSel->addItem("Alpha");
		m_pctrlYSel->addItem("Cl");
		m_pctrlYSel->addItem("Cd");
		m_pctrlYSel->addItem("Cd x 10000");
		m_pctrlYSel->addItem("Cdp");
		m_pctrlYSel->addItem("Cm");
		m_pctrlYSel->addItem("Xtr1");
		m_pctrlYSel->addItem("Xtr2");
		m_pctrlYSel->addItem("HMom");
		m_pctrlYSel->addItem("Cpmin");
		m_pctrlYSel->addItem("Cl/Cd");
		m_pctrlYSel->addItem("|Cl|^(3/2)/Cd");
		m_pctrlYSel->addItem("1/Cl^1/2");
		m_pctrlYSel->addItem("Re");
		m_pctrlYSel->addItem("XCp");
	}
	m_pctrlXSel->adjustSize();
	m_pctrlYSel->adjustSize();
}

void GraphVariableDlg::SetSelection(int const &XSel, int const &YSel)
{
	m_XSel = XSel;
	m_YSel = YSel;
	m_pctrlXSel->setCurrentRow(m_XSel);
	m_pctrlYSel->setCurrentRow(m_YSel);
}

void GraphVariableDlg::GetSelection(int &XSel, int &YSel)
{
	XSel = m_XSel;
	YSel = m_YSel;
}

void GraphVariableDlg::mouseDoubleClickEvent ( QMouseEvent * event )
{
	OnOK();
}


void GraphVariableDlg::OnOK()
{
	m_XSel = m_pctrlXSel->currentRow();
	m_YSel = m_pctrlYSel->currentRow();

	QDialog::accept();
}

















