/****************************************************************************

	WingGraphVarDlg  Classes
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

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "WingGraphVarDlg.h"

WingGraphVarDlg::WingGraphVarDlg()
{
	m_iVar = 0;
	SetupLayout();
}

void WingGraphVarDlg::SetupLayout()
{
	QLabel *YAxis = new QLabel("YAxis");
	m_pctrlYSel = new QListWidget;
	m_pctrlYSel->setMinimumWidth(40);
	m_pctrlYSel->setMinimumHeight(350);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout(this);
	MainLayout->addStretch(1);
	MainLayout->addWidget(YAxis);
	MainLayout->addWidget(m_pctrlYSel);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);

	connect(m_pctrlYSel, SIGNAL(itemDoubleClicked (QListWidgetItem *)), SLOT(OnOK()));
}

void WingGraphVarDlg::InitDialog()
{
	m_pctrlYSel->addItem("Induced Angle");						//0
	m_pctrlYSel->addItem("Total Angle");						//1
	m_pctrlYSel->addItem("Local lift coef.");					//2
	m_pctrlYSel->addItem("Local Lift C.Cl/M.A.C.");				//3
	m_pctrlYSel->addItem("Airfoil viscous drag coef.");			//4
	m_pctrlYSel->addItem("Induced drag coef.");					//5
	m_pctrlYSel->addItem("Total drag coef.");					//6
	m_pctrlYSel->addItem("Local Drag C.Cd/M.A.C.");				//7
	m_pctrlYSel->addItem("Airfoil Pitching moment coef.");		//8
	m_pctrlYSel->addItem("Geom. Pitching moment coef.");		//9
	m_pctrlYSel->addItem("Total Pitching moment coef.");		//10
	m_pctrlYSel->addItem("Reynolds");							//11
	m_pctrlYSel->addItem("Top Transition x-pos%");				//12
	m_pctrlYSel->addItem("Bottom Transition x-pos%");			//13
	m_pctrlYSel->addItem("Centre of Pressure x-pos%");			//14
	m_pctrlYSel->addItem("Bending moment");						//15
	m_pctrlYSel->setCurrentRow(m_iVar);
}


void WingGraphVarDlg::OnOK()
{
	m_iVar = m_pctrlYSel->currentRow();

	QDialog::accept();
}
