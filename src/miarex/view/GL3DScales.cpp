/****************************************************************************

	GL3DScales Class
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

#include "GL3DScales.h"
#include "../Miarex.h"
#include "../../misc/Units.h"
#include "../../globals.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QDockWidget>

void *GL3DScales::s_pMiarex;

int GL3DScales::s_pos = 1;
int GL3DScales::s_NX = 30;
double GL3DScales::s_DeltaL = 0.01;
double GL3DScales::s_XFactor = 1.10;
double GL3DScales::s_XOffset = 0.0;
double GL3DScales::s_ZOffset = 0.0;


GL3DScales::GL3DScales(QWidget *)
{
	setWindowTitle(tr("3D Scales Settings"));

	m_pParent = NULL;
	s_pos       = 1;
	s_NX        =   30;
	s_XFactor   = 1.10;
	s_DeltaL    =  0.01;
	s_XOffset   =  0.0;
	s_ZOffset   =  0.0;

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

	QGroupBox *pCpScaleBox = new QGroupBox(tr("Cp Scale"));
	{
		QGridLayout *pCpScaleLayout = new QGridLayout;
		{
			m_pctrlAutoCpScale = new QCheckBox(tr("Auto Scales"));
			m_pctrlLegendMin = new DoubleEdit(-1.0);
			m_pctrlLegendMax = new DoubleEdit(1.0);
			m_pctrlLegendMin->SetPrecision(2);
			m_pctrlLegendMax->SetPrecision(2);
			QLabel *lab0 = new QLabel(tr("Min"));
			QLabel *lab1 = new QLabel(tr("Max"));
			lab0->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
			lab1->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
			pCpScaleLayout->addWidget(m_pctrlAutoCpScale,1,2);
			pCpScaleLayout->addWidget(lab1,2,1);
			pCpScaleLayout->addWidget(m_pctrlLegendMax,2,2);
			pCpScaleLayout->addWidget(lab0,3,1);
			pCpScaleLayout->addWidget(m_pctrlLegendMin,3,2);
		}
		pCpScaleBox->setLayout(pCpScaleLayout);
	}

//_______________________3D Scales

	QGroupBox *pScaleBox = new QGroupBox(tr("Vector Scales"));
	{
		QHBoxLayout *pScaleLayout = new QHBoxLayout;
		{
			QVBoxLayout *pSliderLayout = new QVBoxLayout;
			{
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
				pSliderLayout->addWidget(m_pctrlLiftScaleSlider);
				pSliderLayout->addWidget(m_pctrlDragScaleSlider);
				pSliderLayout->addWidget(m_pctrlVelocityScaleSlider);
			}

			QVBoxLayout *pLabelLayout = new QVBoxLayout;
			{
				QLabel *lab2 = new QLabel(tr("Lift "));
				QLabel *lab3 = new QLabel(tr("Drag "));
				QLabel *lab4 = new QLabel(tr("Velocity "));
				pLabelLayout->addWidget(lab2);
				pLabelLayout->addWidget(lab3);
				pLabelLayout->addWidget(lab4);
			}

			pScaleLayout->addLayout(pLabelLayout);
			pScaleLayout->addLayout(pSliderLayout);
		}
		pScaleBox->setLayout(pScaleLayout);
	}

//__________________________________	Streamlines

	QGroupBox *pLengthBox = new QGroupBox(tr("Streamline length"));
	{
		m_pctrlNXPoint = new DoubleEdit(33,0);
		m_pctrlNXPoint->SetPrecision(0);
		m_pctrlDeltaL = new DoubleEdit(12.34,2);
		m_pctrlXFactor       = new DoubleEdit(1.23,2);
		m_pctrlLengthUnit1 = new QLabel("miles");
		QLabel *lab5 = new QLabel(tr("X-axis points"));
		QLabel *lab6 = new QLabel(tr("1st segment"));
		QLabel *lab7 = new QLabel(tr("X factor"));
		lab5->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab6->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		lab7->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
		QGridLayout *pLengthLayout = new QGridLayout;
		{
			pLengthLayout->addWidget(lab5, 1, 1);
			pLengthLayout->addWidget(m_pctrlNXPoint , 1, 2);
			pLengthLayout->addWidget(lab6, 2, 1);
			pLengthLayout->addWidget(m_pctrlDeltaL, 2, 2);
			pLengthLayout->addWidget(m_pctrlLengthUnit1, 2, 3);
			pLengthLayout->addWidget(lab7, 3, 1);
			pLengthLayout->addWidget(m_pctrlXFactor, 3, 2);
		}
		pLengthBox->setLayout(pLengthLayout);
	}

	QGroupBox *pStartBox = new QGroupBox(tr("Start Streamline at"));
	{
		QVBoxLayout *pStartLayout = new QVBoxLayout;
		{
			m_pctrlXOffset       = new DoubleEdit(4.56,3);
			m_pctrlZOffset       = new DoubleEdit(7.89,3);
			m_pctrlLengthUnit2 = new QLabel("km");
			m_pctrlLengthUnit3 = new QLabel("m");
			m_pctrlLE = new QRadioButton(tr("L.E."));
			m_pctrlTE = new QRadioButton(tr("T.E."));
			m_pctrlLine = new QRadioButton(tr("Y-Line"));
			QLabel *lab8 = new QLabel(tr("X-Offset"));
			QLabel *lab9 = new QLabel(tr("Z-Offset"));
			lab8->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
			lab9->setAlignment(Qt::AlignVCenter |Qt::AlignRight);
			QHBoxLayout *pLineLayout = new QHBoxLayout;
			{
				pLineLayout->addWidget(m_pctrlLE);
				pLineLayout->addWidget(m_pctrlTE);
				pLineLayout->addWidget(m_pctrlLine);
			}
			QGridLayout *pOffsetLayout = new QGridLayout;
			{
				pOffsetLayout->addWidget(lab8,1,1);
				pOffsetLayout->addWidget(m_pctrlXOffset,1,2);
				pOffsetLayout->addWidget(m_pctrlLengthUnit2,1,3);
				pOffsetLayout->addWidget(lab9,2,1);
				pOffsetLayout->addWidget(m_pctrlZOffset,2,2);
				pOffsetLayout->addWidget(m_pctrlLengthUnit3,2,3);
			}
			pStartLayout->addLayout(pLineLayout);
			pStartLayout->addLayout(pOffsetLayout);
		}
		pStartBox->setLayout(pStartLayout);
	}

	QGroupBox *pStreamBox = new QGroupBox(tr("Streamlines"));
	{
		ApplyButton = new QPushButton(tr("Apply"));
		QVBoxLayout *StreamLayout = new QVBoxLayout;
		StreamLayout->addWidget(pLengthBox);
		StreamLayout->addWidget(pStartBox);
		StreamLayout->addStretch(1);
		StreamLayout->addWidget(ApplyButton);
		StreamLayout->addStretch(1);
		pStreamBox->setLayout(StreamLayout);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pScaleBox);
		pMainLayout->addSpacing(15);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pCpScaleBox);
		pMainLayout->addSpacing(15);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pStreamBox);
		pMainLayout->addStretch(1);
	}

	setLayout(pMainLayout);
}


void GL3DScales::InitDialog()
{
	QString str;

	Units::getLengthUnitLabel(str);
	m_pctrlLengthUnit1->setText(str);
	m_pctrlLengthUnit2->setText(str);
	m_pctrlLengthUnit3->setText(str);

	m_pctrlAutoCpScale->setChecked(QMiarex::s_bAutoCpScale);
	m_pctrlLegendMin->SetValue(QMiarex::s_LegendMin);
	m_pctrlLegendMax->SetValue(QMiarex::s_LegendMax);
	m_pctrlLegendMin->setEnabled(!QMiarex::s_bAutoCpScale);
	m_pctrlLegendMax->setEnabled(!QMiarex::s_bAutoCpScale);

	double pos;
	pos = -QMiarex::s_LiftScale*QMiarex::s_LiftScale + QMiarex::s_LiftScale*sqrt(QMiarex::s_LiftScale*QMiarex::s_LiftScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlLiftScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -QMiarex::s_DragScale*QMiarex::s_DragScale + QMiarex::s_DragScale*sqrt(QMiarex::s_DragScale*QMiarex::s_DragScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlDragScaleSlider->setSliderPosition((int)(pos*100.0));

	pos = -QMiarex::s_VelocityScale*QMiarex::s_VelocityScale + QMiarex::s_VelocityScale*sqrt(QMiarex::s_VelocityScale*QMiarex::s_VelocityScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_pctrlVelocityScaleSlider->setSliderPosition((int)(pos*100.0));

	if(s_pos==0)	    m_pctrlLE->setChecked(true);
	else if(s_pos==1)	m_pctrlTE->setChecked(true);
	else if(s_pos==2)	m_pctrlLine->setChecked(true);

	m_pctrlDeltaL->SetValue(s_DeltaL* Units::mtoUnit());
	m_pctrlXOffset->SetValue(s_XOffset* Units::mtoUnit());
	m_pctrlZOffset->SetValue(s_ZOffset* Units::mtoUnit());
	m_pctrlXFactor->SetValue(s_XFactor);
	m_pctrlNXPoint->SetValue(s_NX);
}


void GL3DScales::OnCpScale()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
    QMiarex::s_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
    QMiarex::s_LegendMax = m_pctrlLegendMax->Value();
    QMiarex::s_LegendMin = m_pctrlLegendMin->Value();
    m_pctrlLegendMin->setEnabled(!QMiarex::s_bAutoCpScale);
    m_pctrlLegendMax->setEnabled(!QMiarex::s_bAutoCpScale);

	pMiarex->m_bResetglPanelCp = true;
	pMiarex->m_bResetglLegend = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnApply()
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	QMiarex::s_LegendMax = m_pctrlLegendMax->Value();
	QMiarex::s_LegendMin = m_pctrlLegendMin->Value();
	QMiarex::s_bAutoCpScale = m_pctrlAutoCpScale->isChecked();
	ReadStreamParams();
	pMiarex->m_bResetglStream = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnLiftScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->s_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglLift = true;
	pMiarex->m_bResetglPanelForce = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnDragScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->s_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglDrag = true;
	pMiarex->UpdateView();
}


void GL3DScales::OnVelocityScale(int pos)
{
	QMiarex * pMiarex = (QMiarex*)s_pMiarex;
	pMiarex->s_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
	pMiarex->m_bResetglDownwash = true;
	pMiarex->UpdateView();
}


void GL3DScales::showEvent(QShowEvent *event)
{
	InitDialog();
	event->accept();
}


void GL3DScales::ReadStreamParams()
{
	s_NX = m_pctrlNXPoint->Value();
	s_XOffset = m_pctrlXOffset->Value() / Units::mtoUnit();
	s_ZOffset = m_pctrlZOffset->Value() / Units::mtoUnit();
	s_DeltaL  = m_pctrlDeltaL->Value()  / Units::mtoUnit();
	s_XFactor = m_pctrlXFactor->Value();

	if(m_pctrlLE->isChecked())	        s_pos=0;
	else if(m_pctrlTE->isChecked())     s_pos=1;
	else if(m_pctrlLine->isChecked())   s_pos=2;
}



bool GL3DScales::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GL3DScales");
	{
		QMiarex::s_bAutoCpScale = pSettings->value("AutoCpScale").toBool();
		QMiarex::s_LegendMin    = pSettings->value("LegendMin").toDouble();
		QMiarex::s_LegendMax    = pSettings->value("LegendMax").toDouble();
		s_pos     = pSettings->value("Position").toInt();
		s_NX      = pSettings->value("NX").toInt();
		s_DeltaL  = pSettings->value("DeltaL").toDouble();
		s_XFactor = pSettings->value("XFactor").toDouble();
		s_XOffset = pSettings->value("XOffset").toDouble();
		s_ZOffset = pSettings->value("ZOffset").toDouble();
	}
	pSettings->endGroup();

	return true;
}


bool GL3DScales::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GL3DScales");
	{
		pSettings->setValue("AutoCpScale", QMiarex::s_bAutoCpScale);
		pSettings->setValue("LegendMin", QMiarex::s_LegendMin);
		pSettings->setValue("LegendMax", QMiarex::s_LegendMax);
		pSettings->setValue("Position", s_pos);
		pSettings->setValue("NX", s_NX);
		pSettings->setValue("DeltaL", s_DeltaL);
		pSettings->setValue("XFactor", s_XFactor);
		pSettings->setValue("XOffset", s_XOffset);
		pSettings->setValue("ZOffset", s_ZOffset);
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
			if(m_pParent) m_pParent->setVisible(false);
			return;
		}
		default:
			event->ignore();
	}
}




