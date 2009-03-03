/****************************************************************************

	SplineCtrlDlg Class
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


#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include "SplineCtrlDlg.h"


SplineCtrlDlg::SplineCtrlDlg(void *pParent)
{
	m_pAFoil = pParent;
	m_pSF = NULL;
	m_pPF = NULL;
	SetupLayout();
}


void SplineCtrlDlg::SetupLayout()
{
	QGridLayout *CtrlLayout = new QGridLayout;
	m_pctrlOutExtrados = new FloatEdit("12");
	m_pctrlOutIntrados = new FloatEdit("22");
	m_pctrlOutExtrados->SetPrecision(0);
	m_pctrlOutIntrados->SetPrecision(0);
	m_pctrlDegExtrados = new QComboBox;
	m_pctrlDegIntrados = new QComboBox;
	QLabel *lab1 = new QLabel("Lower Surface");
	QLabel *lab2 = new QLabel("Upper Surface");
	QLabel *lab3 = new QLabel("Degree");
	QLabel *lab4 = new QLabel("Points");
	CtrlLayout->addWidget(lab1,1,2);
	CtrlLayout->addWidget(lab2,1,3);
	CtrlLayout->addWidget(lab3,2,1);
	CtrlLayout->addWidget(lab4,3,1);
	CtrlLayout->addWidget(m_pctrlDegIntrados, 2,2);
	CtrlLayout->addWidget(m_pctrlDegExtrados, 2,3);
	CtrlLayout->addWidget(m_pctrlOutIntrados, 3,2);
	CtrlLayout->addWidget(m_pctrlOutExtrados, 3,3);


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton        = new QPushButton("OK");
	QPushButton *CancelButton    = new QPushButton("Cancel");
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(CtrlLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);


	setLayout(MainLayout);

	connect(OKButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}


void SplineCtrlDlg::InitDialog()
{
	int i;

	QString str;
	m_pctrlDegExtrados->clear();
	m_pctrlDegIntrados->clear();
	for (i=2; i<6; i++)
	{
		str = QString("%1").arg(i);
		m_pctrlDegExtrados->addItem(str);
		m_pctrlDegIntrados->addItem(str);
	}
	m_pctrlDegExtrados->setEnabled(true);
	m_pctrlDegIntrados->setEnabled(true);

	if(m_bSF)
	{
		m_pctrlDegExtrados->setCurrentIndex(m_pSF->m_Extrados.m_iDegree-2);
		m_pctrlDegIntrados->setCurrentIndex(m_pSF->m_Intrados.m_iDegree-2);
		m_pctrlOutExtrados->SetValue(m_pSF->m_Extrados.m_iRes);
		m_pctrlOutIntrados->SetValue(m_pSF->m_Intrados.m_iRes);
//		m_pctrlSpinIn.SetRange(10,140);
//		m_pctrlSpinOut.SetRange(10,140);
	}
	else
	{
		m_pctrlDegExtrados->setEnabled(false);
		m_pctrlDegIntrados->setEnabled(false);
		m_pctrlOutExtrados->SetMin(3);
		m_pctrlOutExtrados->SetMax(30);
		m_pctrlOutIntrados->SetMin(3);
		m_pctrlOutIntrados->SetMax(30);

		m_pctrlOutExtrados->SetValue(m_pPF->m_Extrados.m_Freq);
		m_pctrlOutIntrados->SetValue(m_pPF->m_Intrados.m_Freq);
//		m_pctrlOutText->setText("Output density:");
//		m_pctrlSpinIn.SetRange(3,30);
//		m_pctrlSpinOut.SetRange(3,30);
	}

	m_pctrlDegExtrados->setFocus();
}



















