/****************************************************************************

	SplineCtrlsDlg
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
#include <QtDebug>
#include "SplineCtrlsDlg.h"
#include <QGridLayout>
#include <QLabel>


SplineCtrlsDlg::SplineCtrlsDlg()
{
	setWindowTitle(tr("Spline Parameters"));
	m_pSF = NULL;
	m_pPF = NULL;
	SetuLayout();
}


void SplineCtrlsDlg::InitDialog()
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
//		m_pctrlOutText->SetWindowText("Output density:");
	}
}



void SplineCtrlsDlg::SetuLayout()
{
	QGridLayout *Layout = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("Spline degree"));
	QLabel *lab2 = new QLabel(tr("Output"));
	QLabel *lab3 = new QLabel(tr("Upper Surface"));
	QLabel *lab4 = new QLabel(tr("Lower Surface"));
	m_pctrlDegIntrados = new QComboBox;
	m_pctrlDegExtrados = new QComboBox;
	m_pctrlOutExtrados = new FloatEdit;
	m_pctrlOutIntrados = new FloatEdit;
	m_pctrlOutIntrados->SetPrecision(0);
	m_pctrlOutExtrados->SetPrecision(0);


	Layout->addWidget(lab1, 2,1);
	Layout->addWidget(lab2, 3,1);
	Layout->addWidget(lab3, 1,2);
	Layout->addWidget(lab4, 1,3);
	Layout->addWidget(m_pctrlDegExtrados, 2,2);
	Layout->addWidget(m_pctrlDegIntrados, 2,3);
	Layout->addWidget(m_pctrlOutExtrados, 3,2);
	Layout->addWidget(m_pctrlOutIntrados, 3,3);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton        = new QPushButton(tr("OK"));
	CancelButton    = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(Layout);
	MainLayout->addStretch(1);
	MainLayout->addSpacing(20);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);


	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}










