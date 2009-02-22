/****************************************************************************

	CpScaleDlg Class
	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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


#include <QGridLayout>
#include "CpScaleDlg.h"
#include <QLabel>
#include <QtDebug>


CpScaleDlg::CpScaleDlg(void *pParent)
{
	m_LegendMin = 0.0;
	m_LegendMax = 1.0;
	m_bAutoCpScale = true;

	SetupLayout();

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(m_pctrlAutoCpScale, SIGNAL(clicked()), this, SLOT(OnAutoCpScale()));

}

void CpScaleDlg::SetupLayout()
{
	QGridLayout *ScaleLayout = new QGridLayout;
	m_pctrlAutoCpScale = new QCheckBox("Auto Scales");
	m_pctrlLegendMin = new FloatEdit("61.234");
	m_pctrlLegendMax = new FloatEdit("3.5555");
	m_pctrlLegendMin->SetPrecision(2);
	m_pctrlLegendMax->SetPrecision(2);
	QLabel *lab0 = new QLabel("Min");
	QLabel *lab1 = new QLabel("Max");
	ScaleLayout->addWidget(m_pctrlAutoCpScale,1,2);
	ScaleLayout->addWidget(lab0,2,1);
	ScaleLayout->addWidget(m_pctrlLegendMin,2,2);
	ScaleLayout->addWidget(lab1,3,1);
	ScaleLayout->addWidget(m_pctrlLegendMax,3,2);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton     = new QPushButton(tr("Done"));
	CommandButtons->addWidget(OKButton);



	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(ScaleLayout);

	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void CpScaleDlg::InitDialog()
{
	m_pctrlAutoCpScale->setChecked(m_bAutoCpScale);
qDebug() << m_LegendMin << m_LegendMax;
	m_pctrlLegendMin->SetValue(m_LegendMin);
	m_pctrlLegendMax->SetValue(m_LegendMax);
	m_pctrlLegendMin->setEnabled(!m_bAutoCpScale);
	m_pctrlLegendMax->setEnabled(!m_bAutoCpScale);
}

void CpScaleDlg::OnAutoCpScale()
{
	m_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
	m_pctrlLegendMin->setEnabled(!m_bAutoCpScale);
	m_pctrlLegendMax->setEnabled(!m_bAutoCpScale);
}

void CpScaleDlg::OnOK()
{
	m_LegendMax = m_pctrlLegendMax->GetValue();
	m_LegendMin = m_pctrlLegendMin->GetValue();
	m_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
	accept();
}
