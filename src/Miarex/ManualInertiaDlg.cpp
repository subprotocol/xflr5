/****************************************************************************

	ManualInertiaDlg Class
	Copyright (C) 2010 Andre Deperrois XFLR5@yahoo.com

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

//allows the user to input the CoG and inertia properties manually
#include "../MainFrame.h"
#include "../Globals.h"
#include "ManualInertiaDlg.h"
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QtDebug>


void *ManualInertiaDlg::s_pMainFrame = NULL;

double ManualInertiaDlg::m_Mass = 0.0;
double ManualInertiaDlg::m_CoGIxx = 0.0;
double ManualInertiaDlg::m_CoGIyy = 0.0;
double ManualInertiaDlg::m_CoGIzz = 0.0;
double ManualInertiaDlg::m_CoGIxz = 0.0;
double ManualInertiaDlg::m_CoGx = 0.0;
double ManualInertiaDlg::m_CoGz = 0.0;

ManualInertiaDlg::ManualInertiaDlg()
{
	setWindowTitle(tr("Inertia Input"));
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	SetupLayout();
	m_pctrlMass->SetValue(m_Mass*pMainFrame->m_kgtoUnit);
	m_pctrlCoGx->SetValue(m_CoGx*pMainFrame->m_mtoUnit);
	m_pctrlCoGz->SetValue(m_CoGz*pMainFrame->m_mtoUnit);
	m_pctrlCoGIxx->SetValue(m_CoGIxx*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlCoGIyy->SetValue(m_CoGIyy*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlCoGIzz->SetValue(m_CoGIzz*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlCoGIxz->SetValue(m_CoGIxz*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
}


void ManualInertiaDlg::SetupLayout()
{
	QHBoxLayout *MassLayout = new QHBoxLayout;
	QLabel *LabMass = new QLabel(tr("Mass ="));
	QLabel *LabMassUnit = new QLabel("kg");
	m_pctrlMass = new FloatEdit(0,3);
	MassLayout->addWidget(LabMass);	
	MassLayout->addWidget(m_pctrlMass);	
	MassLayout->addWidget(LabMassUnit);	
	
	QGridLayout *CoGPosLayout = new QGridLayout;
	m_pctrlCoGx = new FloatEdit(0.0,3);
	m_pctrlCoGz = new FloatEdit(0.0,3);
	QLabel *Labx = new QLabel("x=");
	QLabel *Labz = new QLabel("z=");
	QLabel *LabLength1 = new QLabel("m");
	QLabel *LabLength3 = new QLabel("m");
	CoGPosLayout->addWidget(Labx,1,1);
	CoGPosLayout->addWidget(Labz,3,1);
	CoGPosLayout->addWidget(m_pctrlCoGx,1,2);
	CoGPosLayout->addWidget(m_pctrlCoGz,3,2);
	CoGPosLayout->addWidget(LabLength1,1,3);
	CoGPosLayout->addWidget(LabLength3,3,3);
	QGroupBox *CoGPosBox = new QGroupBox(tr("Center of Gravity position"));
	CoGPosBox->setLayout(CoGPosLayout);
	
	QGridLayout *InertiaLayout = new QGridLayout;
	QLabel *LabIxx = new QLabel("Ixx=");
	QLabel *LabIyy = new QLabel("Iyy=");
	QLabel *LabIzz = new QLabel("Izz=");
	QLabel *LabIxz = new QLabel("Ixz=");
	QLabel *LabInertia1 = new QLabel("kg.m2");
	QLabel *LabInertia2 = new QLabel("kg.m2");
	QLabel *LabInertia3 = new QLabel("kg.m2");
	QLabel *LabInertia4 = new QLabel("kg.m2");
	m_pctrlCoGIxx = new FloatEdit(0.0,3);
	m_pctrlCoGIyy = new FloatEdit(0.0,3);
	m_pctrlCoGIzz = new FloatEdit(0.0,3);
	m_pctrlCoGIxz = new FloatEdit(0.0,3);
	InertiaLayout->addWidget(LabIxx,1,1);
	InertiaLayout->addWidget(LabIyy,2,1);
	InertiaLayout->addWidget(LabIzz,3,1);
	InertiaLayout->addWidget(LabIxz,4,1);
	InertiaLayout->addWidget(m_pctrlCoGIxx,1,2);
	InertiaLayout->addWidget(m_pctrlCoGIyy,2,2);
	InertiaLayout->addWidget(m_pctrlCoGIzz,3,2);
	InertiaLayout->addWidget(m_pctrlCoGIxz,4,2);
	InertiaLayout->addWidget(LabInertia1,1,3);
	InertiaLayout->addWidget(LabInertia2,2,3);
	InertiaLayout->addWidget(LabInertia3,3,3);
	InertiaLayout->addWidget(LabInertia4,4,3);
	QGroupBox *InertiaBox = new QGroupBox(tr("Inertia w.r.t. CoG"));
	InertiaBox->setLayout(InertiaLayout);
	
	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton     = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(MassLayout);
	MainLayout->addWidget(CoGPosBox);
	MainLayout->addWidget(InertiaBox);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString length, mass, inertia;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	LabLength1->setText(length);
	LabLength3->setText(length);
	
	GetWeightUnit(mass, pMainFrame->m_WeightUnit);
	LabMassUnit->setText(mass);
	inertia = mass+"."+length+"2";
	LabInertia1->setText(inertia);
	LabInertia2->setText(inertia);	
	LabInertia3->setText(inertia);	
	LabInertia4->setText(inertia);	
}


void ManualInertiaDlg::OnOK()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_Mass = m_pctrlMass->GetValue()/pMainFrame->m_kgtoUnit;
	m_CoGx = m_pctrlCoGx->GetValue()/pMainFrame->m_mtoUnit;
	m_CoGz = m_pctrlCoGz->GetValue()/pMainFrame->m_mtoUnit;
	m_CoGIxx = m_pctrlCoGIxx->GetValue()/pMainFrame->m_kgtoUnit/pMainFrame->m_mtoUnit/pMainFrame->m_mtoUnit;
	m_CoGIyy = m_pctrlCoGIyy->GetValue()/pMainFrame->m_kgtoUnit/pMainFrame->m_mtoUnit/pMainFrame->m_mtoUnit;
	m_CoGIzz = m_pctrlCoGIzz->GetValue()/pMainFrame->m_kgtoUnit/pMainFrame->m_mtoUnit/pMainFrame->m_mtoUnit;
	m_CoGIxz = m_pctrlCoGIxz->GetValue()/pMainFrame->m_kgtoUnit/pMainFrame->m_mtoUnit/pMainFrame->m_mtoUnit;
	accept();
}


void ManualInertiaDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                      accept();

			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}
}
