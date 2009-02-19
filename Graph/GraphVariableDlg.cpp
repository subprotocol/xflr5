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
	else if(iGraphType == 2)
	{
		m_pctrlXSel->addItem("Alpha");						//0
		m_pctrlXSel->addItem("Lift coef.");					//1
		m_pctrlXSel->addItem("Viscous drag coef.");			//2
		m_pctrlXSel->addItem("Induced drag coef.");			//3
		m_pctrlXSel->addItem("Total drag coef.");			//4

		m_pctrlXSel->addItem("Total pitching moment coef.");//5
		m_pctrlXSel->addItem("Total rolling moment coef.");		//6
		m_pctrlXSel->addItem("Total yawing moment coef.");  //7
		m_pctrlXSel->addItem("Viscous yawing moment coef.");//8
		m_pctrlXSel->addItem("Induced yawing moment coef.");//9

		m_pctrlXSel->addItem("Glide ratio Cl/Cd");			//10
		m_pctrlXSel->addItem("Power factor Cl^(3/2)/Cd");	//11
		m_pctrlXSel->addItem("1/Rt(Cl)");					//12
		m_pctrlXSel->addItem("Lift");						//13
		m_pctrlXSel->addItem("Drag");						//14
		m_pctrlXSel->addItem("Vx");							//15
		m_pctrlXSel->addItem("Vz");							//16
		m_pctrlXSel->addItem("VInf");						//17
		m_pctrlXSel->addItem("Descent angle atan(Cd/Cl)");	//18
		m_pctrlXSel->addItem("Pitching Moment");			//19
		m_pctrlXSel->addItem("Rolling Moment");				//20
		m_pctrlXSel->addItem("Yawing Moment");				//21
		m_pctrlXSel->addItem("Centre of pressure X-Pos");	//22
		m_pctrlXSel->addItem("Centre of pressure Y-Pos");	//23
		m_pctrlXSel->addItem("Bending moment");				//24
		m_pctrlXSel->addItem("m.g.Vz");						//25
		m_pctrlXSel->addItem("Efficiency");					//26
		m_pctrlXSel->addItem("(XCp-Xcg)/MAC");				//27
		m_pctrlXSel->addItem("Control Variable");			//28
		m_pctrlXSel->addItem("Cy - Lateral force coef.");	//29

		m_pctrlYSel->addItem("Alpha");						//0
		m_pctrlYSel->addItem("Lift coef.");					//1
		m_pctrlYSel->addItem("Viscous drag coef.");			//2
		m_pctrlYSel->addItem("Induced drag coef.");			//3
		m_pctrlYSel->addItem("Total drag coef.");			//4

		m_pctrlYSel->addItem("Total pitching moment coef.");//5
		m_pctrlYSel->addItem("Total rolling moment coef.");		//6
		m_pctrlYSel->addItem("Total yawing moment coef.");  //7
		m_pctrlYSel->addItem("Viscous yawing moment coef.");//8
		m_pctrlYSel->addItem("Induced yawing moment coef.");//9

		m_pctrlYSel->addItem("Glide ratio Cl/Cd");			//10
		m_pctrlYSel->addItem("Power factor Cl^(3/2)/Cd");	//11
		m_pctrlYSel->addItem("1/Rt(Cl)");					//12
		m_pctrlYSel->addItem("Lift");						//13
		m_pctrlYSel->addItem("Drag");						//14
		m_pctrlYSel->addItem("Vx");							//15
		m_pctrlYSel->addItem("Vz");							//16
		m_pctrlYSel->addItem("VInf");						//17
		m_pctrlYSel->addItem("Descent angle atan(Cd/Cl)");	//18
		m_pctrlYSel->addItem("Pitching Moment");			//19
		m_pctrlYSel->addItem("Rolling Moment");				//20
		m_pctrlYSel->addItem("Yawing Moment");				//21
		m_pctrlYSel->addItem("Centre of pressure X-Pos");	//22
		m_pctrlYSel->addItem("Centre of pressure Y-Pos");	//23
		m_pctrlYSel->addItem("Bending moment");				//24
		m_pctrlYSel->addItem("m.g.Vz");						//25
		m_pctrlYSel->addItem("Efficiency");					//26
		m_pctrlYSel->addItem("(XCp-Xcg)/MAC");				//27
		m_pctrlYSel->addItem("Control Variable");			//28
		m_pctrlYSel->addItem("Cy - Lateral force coef.");	//29
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
	XSel = m_pctrlXSel->currentRow();
	YSel = m_pctrlYSel->currentRow();
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

















