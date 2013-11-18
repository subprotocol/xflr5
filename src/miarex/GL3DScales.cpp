/****************************************************************************

	GL3DScales Class
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

#include "Miarex.h"
#include "../mainframe.h"
#include "../globals.h"
#include "GL3DScales.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QDockWidget>


GL3DScales::GL3DScales(QWidget *parent)
{
	setWindowTitle(tr("3D Scales Settings"));
	m_pMainFrame = NULL;
	m_pMiarex = NULL;

	m_pos       = 1;
	m_NX        =   30;
	m_XFactor   = 1.10;
	m_DeltaL    =  0.01;
	m_XOffset   =  0.0;
	m_ZOffset   =  0.0;

	SetupLayout();

	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));

	connect(m_pctrlAutoCpScale, SIGNAL(clicked()), this, SLOT(OnCpScale()));
	connect(m_pctrlLegendMin, SIGNAL(editingFinished()), this, SLOT(OnCpScale()));
	connect(m_pctrlLegendMax, SIGNAL(editingFinished()), this, SLOT(OnCpScale()));

	connect(m_pctrlLiftScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnLiftScale(int)));
	connect(m_pctrlDragScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnDragScale(int)));
	connect(m_pctrlVelocityScaleSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnVelocityScale(int)));
}


void GL3DScales::SetupLayout()
{
	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	setSizePolicy(szPolicyMinimum);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QGroupBox *CpScaleBox = new QGroupBox(tr("Cp Scale"));
	{
		QGridLayout *CpScaleLayout = new QGridLayout;
		m_pctrlAutoCpScale = new QCheckBox(tr("Auto Scales"));
		m_pctrlLegendMin = new FloatEdit(61.234);
		m_pctrlLegendMax = new FloatEdit(3.5555);
		m_pctrlLegendMin->SetPrecision(2);
		m_pctrlLegendMax->SetPrecision(2);
		QLabel *lab0 = new QLabel(tr("Min"));
		QLabel *lab1 = new QLabel(tr("Max"));
		lab0->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab1->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		CpScaleLayout->addWidget(m_pctrlAutoCpScale,1,2);
		CpScaleLayout->addWidget(lab1,2,1);
		CpScaleLayout->addWidget(m_pctrlLegendMax,2,2);
		CpScaleLayout->addWidget(lab0,3,1);
		CpScaleLayout->addWidget(m_pctrlLegendMin,3,2);
		CpScaleBox->setLayout(CpScaleLayout);
	}

//_______________________3D Scales

	QGroupBox *ScaleBox = new QGroupBox(tr("Vector Scales"));
	{
		QVBoxLayout *SliderLayout = new QVBoxLayout;
		m_pctrlLiftScaleSlider  = new QSlider(Qt::Horizontal);
		m_pctrlLiftScaleSlider->setMinimum(0);
		m_pctrlLiftScaleSlider->setMaximum(100);
		m_pctrlLiftScaleSlider->setSliderPosition(50);
		m_pctrlLiftScaleSlider->setTickInterval(5);
		m_pctrlLiftScaleSlider->setTickPosition(QSlider::TicksBelow);
		m_pctrlDragScaleSlider = new QSlider(Qt::Horizontal);
		m_pctrlDragScaleSlider->setMinimum(0);
		m_pctrlDragScaleSlider->setMaximum(100);
		m_pctrlDragScaleSlider->setSliderPosition(50);
		m_pctrlDragScaleSlider->setTickInterval(5);
		m_pctrlDragScaleSlider->setTickPosition(QSlider::TicksBelow);
		m_pctrlVelocityScaleSlider  = new QSlider(Qt::Horizontal);
		m_pctrlVelocityScaleSlider->setMinimum(0);
		m_pctrlVelocityScaleSlider->setMaximum(100);
		m_pctrlVelocityScaleSlider->setSliderPosition(50);
		m_pctrlVelocityScaleSlider->setTickInterval(5);
		m_pctrlVelocityScaleSlider->setTickPosition(QSlider::TicksBelow);
		SliderLayout->addWidget(m_pctrlLiftScaleSlider);
		SliderLayout->addWidget(m_pctrlDragScaleSlider);
		SliderLayout->addWidget(m_pctrlVelocityScaleSlider);

		QVBoxLayout *LabelLayout = new QVBoxLayout;
		QLabel *lab2 = new QLabel(tr("Lift "));
		QLabel *lab3 = new QLabel(tr("Drag "));
		QLabel *lab4 = new QLabel(tr("Velocity "));
		LabelLayout->addWidget(lab2);
		LabelLayout->addWidget(lab3);
		LabelLayout->addWidget(lab4);

		QHBoxLayout *ScaleLayout = new QHBoxLayout;
		ScaleLayout->addLayout(LabelLayout);
		ScaleLayout->addLayout(SliderLayout);
		ScaleBox->setLayout(ScaleLayout);
	}

//__________________________________	Streamlines

	QGroupBox *LengthBox = new QGroupBox(tr("Streamline length"));
	{
		m_pctrlNXPoint = new FloatEdit(33,0);
		m_pctrlNXPoint->SetPrecision(0);
		m_pctrlDeltaL = new FloatEdit(12.34,2);
		m_pctrlXFactor       = new FloatEdit(1.23,2);
		m_pctrlLengthUnit1 = new QLabel("miles");
		QLabel *lab5 = new QLabel(tr("X-axis points"));
		QLabel *lab6 = new QLabel(tr("1st segment"));
		QLabel *lab7 = new QLabel(tr("X factor"));
		lab5->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab6->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab7->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		QGridLayout *LengthLayout = new QGridLayout;
		LengthLayout->addWidget(lab5, 1, 1);
		LengthLayout->addWidget(m_pctrlNXPoint , 1, 2);
		LengthLayout->addWidget(lab6, 2, 1);
		LengthLayout->addWidget(m_pctrlDeltaL, 2, 2);
		LengthLayout->addWidget(m_pctrlLengthUnit1, 2, 3);
		LengthLayout->addWidget(lab7, 3, 1);
		LengthLayout->addWidget(m_pctrlXFactor, 3, 2);
		LengthBox->setLayout(LengthLayout);
	}

	QGroupBox *StartBox = new QGroupBox(tr("Start Streamline at"));
	{
		m_pctrlXOffset       = new FloatEdit(4.56,3);
		m_pctrlZOffset       = new FloatEdit(7.89,3);
		m_pctrlLengthUnit2 = new QLabel("km");
		m_pctrlLengthUnit3 = new QLabel("m");
		m_pctrlLE = new QRadioButton(tr("L.E."));
		m_pctrlTE = new QRadioButton(tr("T.E."));
		m_pctrlLine = new QRadioButton(tr("Y-Line"));
		QLabel *lab8 = new QLabel(tr("X-Offset"));
		QLabel *lab9 = new QLabel(tr("Z-Offset"));
		lab8->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab9->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		QHBoxLayout *LineLayout = new QHBoxLayout;
		LineLayout->addWidget(m_pctrlLE);
		LineLayout->addWidget(m_pctrlTE);
		LineLayout->addWidget(m_pctrlLine);
		QGridLayout *OffsetLayout = new QGridLayout;
		OffsetLayout->addWidget(lab8,1,1);
		OffsetLayout->addWidget(m_pctrlXOffset,1,2);
		OffsetLayout->addWidget(m_pctrlLengthUnit2,1,3);
		OffsetLayout->addWidget(lab9,2,1);
		OffsetLayout->addWidget(m_pctrlZOffset,2,2);
		OffsetLayout->addWidget(m_pctrlLengthUnit3,2,3);
		QVBoxLayout *StartLayout = new QVBoxLayout;
		StartLayout->addLayout(LineLayout);
		StartLayout->addLayout(OffsetLayout);
		StartBox->setLayout(StartLayout);
	}

	QGroupBox *StreamBox = new QGroupBox(tr("Streamlines"));
	{
		ApplyButton = new QPushButton(tr("Apply"));
		QVBoxLayout *StreamLayout = new QVBoxLayout;
		StreamLayout->addWidget(LengthBox);
		StreamLayout->addWidget(StartBox);
		StreamLayout->addStretch(1);
		StreamLayout->addWidget(ApplyButton);
		StreamLayout->addStretch(1);
		StreamBox->setLayout(StreamLayout);
	}


	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addWidget(ScaleBox);
	MainLayout->addSpacing(15);
	MainLayout->addStretch(1);
	MainLayout->addWidget(CpScaleBox);
	MainLayout->addSpacing(15);
	MainLayout->addStretch(1);
	MainLayout->addWidget(StreamBox);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void GL3DScales::InitDialog()
{
	if(!m_pMainFrame) return;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	QString str;

	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit2->setText(str);
	m_pctrlLengthUnit3->setText(str);

	m_pctrlAutoCpScale->setChecked(pMiarex->m_bAutoCpScale);
	m_pctrlLegendMin->SetValue(pMiarex->m_LegendMin);
	m_pctrlLegendMax->SetValue(pMiarex->m_LegendMax);
	m_pctrlLegendMin->setEnabled(!pMiarex->m_bAutoCpScale);
	m_pctrlLegendMax->setEnabled(!pMiarex->m_bAutoCpScale);

	double pos;
	pos = -pMiarex->m_LiftScale*pMiarex->m_LiftScale + pMiarex->m_LiftScale*sqrt(pMiarex->m_LiftScale*pMiarex->m_LiftScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlLiftScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -pMiarex->m_DragScale*pMiarex->m_DragScale + pMiarex->m_DragScale*sqrt(pMiarex->m_DragScale*pMiarex->m_DragScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlDragScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -pMiarex->m_VelocityScale*pMiarex->m_VelocityScale + pMiarex->m_VelocityScale*sqrt(pMiarex->m_VelocityScale*pMiarex->m_VelocityScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlVelocityScaleSlider->setSliderPosition((int)(pos*100.0));

	if(m_pos==0)	    m_pctrlLE->setChecked(true);
	else if(m_pos==1)	m_pctrlTE->setChecked(true);
	else if(m_pos==2)	m_pctrlLine->setChecked(true);

	m_pctrlDeltaL->SetValue(m_DeltaL* pMainFrame->m_mtoUnit);
	m_pctrlXOffset->SetValue(m_XOffset* pMainFrame->m_mtoUnit);
	m_pctrlZOffset->SetValue(m_ZOffset* pMainFrame->m_mtoUnit);
	m_pctrlXFactor->SetValue(m_XFactor);
	m_pctrlNXPoint->SetValue(m_NX);
}


void GL3DScales::OnCpScale()
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
	pMiarex->m_LegendMax = m_pctrlLegendMax->Value();
	pMiarex->m_LegendMin = m_pctrlLegendMin->Value();
	m_pctrlLegendMin->setEnabled(!pMiarex->m_bAutoCpScale);
	m_pctrlLegendMax->setEnabled(!pMiarex->m_bAutoCpScale);

	pMiarex->m_bResetglPanelCp = true;
	pMiarex->m_bResetglLegend = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnApply()
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_LegendMax = m_pctrlLegendMax->Value();
	pMiarex->m_LegendMin = m_pctrlLegendMin->Value();
	pMiarex->m_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
	ReadStreamParams();
	pMiarex->m_bResetglStream = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnLiftScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglLift = true;
	pMiarex->m_bResetglPanelForce = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnDragScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglDrag = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnVelocityScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglDownwash = true;
	pMiarex->UpdateView();
}


void GL3DScales::showEvent(QShowEvent *event)
{
	InitDialog();
}


void GL3DScales::ReadStreamParams()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_NX = m_pctrlNXPoint->Value();
	m_XOffset = m_pctrlXOffset->Value() / pMainFrame->m_mtoUnit;
	m_ZOffset = m_pctrlZOffset->Value() / pMainFrame->m_mtoUnit;
	m_DeltaL  = m_pctrlDeltaL->Value()  / pMainFrame->m_mtoUnit;
	m_XFactor = m_pctrlXFactor->Value();

	if(m_pctrlLE->isChecked())	        m_pos=0;
	else if(m_pctrlTE->isChecked())     m_pos=1;
	else if(m_pctrlLine->isChecked())   m_pos=2;
}



bool GL3DScales::LoadSettings(QSettings *pSettings)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pSettings->beginGroup("GL3DScales");
	{
		pMiarex->m_bAutoCpScale = pSettings->value("AutoCpScale").toBool();
		pMiarex->m_LegendMin    = pSettings->value("LegendMin").toDouble();
		pMiarex->m_LegendMax    = pSettings->value("LegendMax").toDouble();
		m_pos     = pSettings->value("Position").toInt();
		m_NX      = pSettings->value("NX").toInt();
		m_DeltaL  = pSettings->value("DeltaL").toDouble();
		m_XFactor = pSettings->value("XFactor").toDouble();
		m_XOffset = pSettings->value("XOffset").toDouble();
		m_ZOffset = pSettings->value("ZOffset").toDouble();
	}
	pSettings->endGroup();

	return true;
}


bool GL3DScales::SaveSettings(QSettings *pSettings)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pSettings->beginGroup("GL3DScales");
	{
		pSettings->setValue("AutoCpScale", pMiarex->m_bAutoCpScale);
		pSettings->setValue("LegendMin", pMiarex->m_LegendMin);
		pSettings->setValue("LegendMax", pMiarex->m_LegendMax);
		pSettings->setValue("Position", m_pos);
		pSettings->setValue("NX", m_NX);
		pSettings->setValue("DeltaL", m_DeltaL);
		pSettings->setValue("XFactor", m_XFactor);
		pSettings->setValue("XOffset", m_XOffset);
		pSettings->setValue("ZOffset", m_ZOffset);
	}
	pSettings->endGroup();
	return true;
}



void GL3DScales::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
			pMainFrame->m_pctrl3DScalesWidget->hide();
			return;
		}
		default:
			event->ignore();
	}
}




