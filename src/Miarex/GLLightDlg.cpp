/****************************************************************************

	GLLightDlg class
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

#include "GLLightDlg.h"
#include "Miarex.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtDebug>


GLLightDlg::GLLightDlg()
{
	m_pGL3dBodyDlg = m_pGL3dWingDlg = NULL;
	m_bCullFaces     = false;
	m_bShade         = true;
	m_bSmooth        = true;
	m_bLocalView     = true;
	m_bDepthTest     = true;
	m_bColorMaterial = true;

	m_Red   = 1.0f;
	m_Green = 1.0f;
	m_Blue  = 1.0f;

	m_Ambient      = 0.25f;
	m_Diffuse      = 0.58f;
	m_Specular     = 0.05f;

	m_MatAmbient   = -0.51f;
	m_MatDiffuse   = -0.43f;
	m_MatSpecular  = -0.08f;
	m_MatEmission  = -0.04f;
	m_iMatShininess = 0;

	m_XLight   =  0.56f;
	m_YLight   =  0.02f;
	m_ZLight   =  0.68f;
	SetupLayout();
	setWindowTitle(tr("OpenGL Light Options"));

	connect(m_pctrlClose, SIGNAL(clicked()),this, SLOT(accept()));
	connect(m_pctrlDefaults, SIGNAL(clicked()), this, SLOT(OnDefaults()));
	connect(m_pctrlRed, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlGreen, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlBlue, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlAmbient, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlDiffuse, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlSpecular, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlMatAmbient, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlMatDiffuse, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlMatSpecular, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlMatShininess, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlMatEmission, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlXLight, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlYLight, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));
	connect(m_pctrlZLight, SIGNAL(sliderMoved(int)), this, SLOT(OnSlider(int)));

	connect(m_pctrlColorMaterial, SIGNAL(clicked()), this, SLOT(OnChanged()));
	connect(m_pctrlDepthTest, SIGNAL(clicked()), this, SLOT(OnChanged()));
	connect(m_pctrlCullFaces, SIGNAL(clicked()), this, SLOT(OnChanged()));
	connect(m_pctrlShade, SIGNAL(clicked()), this, SLOT(OnChanged()));
	connect(m_pctrlSmooth, SIGNAL(clicked()), this, SLOT(OnChanged()));
	connect(m_pctrlLocalView, SIGNAL(clicked()), this, SLOT(OnChanged()));

	connect(m_pctrlLight, SIGNAL(clicked()), this, SLOT(OnLight()));

}

void GLLightDlg::SetupLayout()
{
	m_pctrlDiffuse      = new QSlider(Qt::Horizontal);
	m_pctrlAmbient      = new QSlider(Qt::Horizontal);
	m_pctrlSpecular     = new QSlider(Qt::Horizontal);
	m_pctrlRed          = new QSlider(Qt::Horizontal);
	m_pctrlGreen        = new QSlider(Qt::Horizontal);
	m_pctrlBlue         = new QSlider(Qt::Horizontal);
	m_pctrlXLight       = new QSlider(Qt::Horizontal);
	m_pctrlYLight       = new QSlider(Qt::Horizontal);
	m_pctrlZLight       = new QSlider(Qt::Horizontal);
	m_pctrlMatDiffuse   = new QSlider(Qt::Horizontal);
	m_pctrlMatAmbient   = new QSlider(Qt::Horizontal);
	m_pctrlMatSpecular  = new QSlider(Qt::Horizontal);
	m_pctrlMatEmission  = new QSlider(Qt::Horizontal);
	m_pctrlMatShininess = new QSlider(Qt::Horizontal);
	m_pctrlDiffuse->setTickPosition(QSlider::TicksBelow);
	m_pctrlAmbient->setTickPosition(QSlider::TicksBelow);
	m_pctrlSpecular->setTickPosition(QSlider::TicksBelow);
	m_pctrlRed->setTickPosition(QSlider::TicksBelow);
	m_pctrlGreen->setTickPosition(QSlider::TicksBelow);
	m_pctrlBlue->setTickPosition(QSlider::TicksBelow);
	m_pctrlXLight->setTickPosition(QSlider::TicksBelow);
	m_pctrlYLight->setTickPosition(QSlider::TicksBelow);
	m_pctrlZLight->setTickPosition(QSlider::TicksBelow);
	m_pctrlMatDiffuse->setTickPosition(QSlider::TicksBelow);
	m_pctrlMatAmbient->setTickPosition(QSlider::TicksBelow);
	m_pctrlMatSpecular->setTickPosition(QSlider::TicksBelow);
	m_pctrlMatEmission->setTickPosition(QSlider::TicksBelow);
	m_pctrlMatShininess->setTickPosition(QSlider::TicksBelow);

	m_pctrlRed->setMinimum(0);
	m_pctrlRed->setMaximum(100);
	m_pctrlRed->setTickInterval(10);
	m_pctrlGreen->setMinimum(0);
	m_pctrlGreen->setMaximum(100);
	m_pctrlGreen->setTickInterval(10);
	m_pctrlBlue->setMinimum(0);
	m_pctrlBlue->setMaximum(100);
	m_pctrlBlue->setTickInterval(10);

	m_pctrlAmbient->setMinimum(0);
	m_pctrlAmbient->setMaximum(100);
	m_pctrlAmbient->setTickInterval(10);
	m_pctrlDiffuse->setMinimum(0);
	m_pctrlDiffuse->setMaximum(100);
	m_pctrlDiffuse->setTickInterval(10);
	m_pctrlSpecular->setMinimum(0);
	m_pctrlSpecular->setMaximum(100);
	m_pctrlSpecular->setTickInterval(10);

	m_pctrlMatAmbient->setMinimum(-100);
	m_pctrlMatAmbient->setMaximum(100);
	m_pctrlMatAmbient->setTickInterval(20);
	m_pctrlMatDiffuse->setMinimum(-100);
	m_pctrlMatDiffuse->setMaximum(100);
	m_pctrlMatDiffuse->setTickInterval(20);
	m_pctrlMatSpecular->setMinimum(-100);
	m_pctrlMatSpecular->setMaximum(100);
	m_pctrlMatSpecular->setTickInterval(20);
	m_pctrlMatEmission->setMinimum(-100);
	m_pctrlMatEmission->setMaximum(100);
	m_pctrlMatEmission->setTickInterval(20);
	m_pctrlMatShininess->setMinimum(0);
	m_pctrlMatShininess->setMaximum(128);
	m_pctrlMatShininess->setTickInterval(16);


	m_pctrlXLight->setMinimum(0);
	m_pctrlXLight->setMaximum(100);
	m_pctrlXLight->setTickInterval(10);
	m_pctrlYLight->setMinimum(0);
	m_pctrlYLight->setMaximum(100);
	m_pctrlYLight->setTickInterval(10);
	m_pctrlZLight->setMinimum(0);
	m_pctrlZLight->setMaximum(100);
	m_pctrlZLight->setTickInterval(10);

	QGridLayout *LightIntensity = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("Diffuse"));
	QLabel *lab2 = new QLabel(tr("Ambient"));
	QLabel *lab3 = new QLabel(tr("Specular"));
	LightIntensity->addWidget(lab1,1,1);
	LightIntensity->addWidget(lab2,2,1);
	LightIntensity->addWidget(lab3,3,1);
	LightIntensity->addWidget(m_pctrlDiffuse,1,2);
	LightIntensity->addWidget(m_pctrlAmbient,2,2);
	LightIntensity->addWidget(m_pctrlSpecular,3,2);
	QGroupBox *LightIntensityBox = new QGroupBox(tr("Light Intensity"));
	LightIntensityBox->setLayout(LightIntensity);

	QGridLayout *LightColor = new QGridLayout;
	QLabel *lab11 = new QLabel(tr("Red"));
	QLabel *lab12 = new QLabel(tr("Green"));
	QLabel *lab13 = new QLabel(tr("Blue"));
	LightColor->addWidget(lab11,1,1);
	LightColor->addWidget(lab12,2,1);
	LightColor->addWidget(lab13,3,1);
	LightColor->addWidget(m_pctrlRed,1,2);
	LightColor->addWidget(m_pctrlGreen,2,2);
	LightColor->addWidget(m_pctrlBlue,3,2);
	QGroupBox *LightColorBox = new QGroupBox(tr("Light Color"));
	LightColorBox->setLayout(LightColor);

	QGridLayout *LightPosition = new QGridLayout;
	QLabel *lab21 = new QLabel(tr("x"));
	QLabel *lab22 = new QLabel(tr("y"));
	QLabel *lab23 = new QLabel(tr("z"));
	LightPosition->addWidget(lab21,1,1);
	LightPosition->addWidget(lab22,2,1);
	LightPosition->addWidget(lab23,3,1);
	LightPosition->addWidget(m_pctrlXLight,1,2);
	LightPosition->addWidget(m_pctrlYLight,2,2);
	LightPosition->addWidget(m_pctrlZLight,3,2);
	QGroupBox *LightPositionBox = new QGroupBox(tr("Light Position"));
	LightPositionBox->setLayout(LightPosition);

	QGridLayout *MaterialData = new QGridLayout;
	QLabel *lab31 = new QLabel(tr("Diffuse"));
	QLabel *lab32 = new QLabel(tr("Ambient"));
	QLabel *lab33 = new QLabel(tr("Specular"));
	QLabel *lab34 = new QLabel(tr("Emissions"));
	QLabel *lab35 = new QLabel(tr("Shininess"));
	MaterialData->addWidget(lab31,1,1);
	MaterialData->addWidget(lab32,2,1);
	MaterialData->addWidget(lab33,3,1);
	MaterialData->addWidget(lab34,4,1);
	MaterialData->addWidget(lab35,5,1);
	MaterialData->addWidget(m_pctrlMatDiffuse,1,2);
	MaterialData->addWidget(m_pctrlMatAmbient,2,2);
	MaterialData->addWidget(m_pctrlMatSpecular,3,2);
	MaterialData->addWidget(m_pctrlMatEmission,4,2);
	MaterialData->addWidget(m_pctrlMatShininess,5,2);
	QGroupBox *MaterialDataBox = new QGroupBox(tr("Material"));
	MaterialDataBox->setLayout(MaterialData);

	QVBoxLayout *CheckLayout = new QVBoxLayout;
	m_pctrlColorMaterial = new QCheckBox(tr("Color Material"));
	m_pctrlCullFaces     = new QCheckBox(tr("Cull Faces"));
	m_pctrlSmooth        = new QCheckBox(tr("Smooth Quads"));
	m_pctrlDepthTest     = new QCheckBox(tr("Depth Test"));
	m_pctrlShade         = new QCheckBox(tr("Smooth Shading"));
	m_pctrlLocalView     = new QCheckBox(tr("Local View"));
	CheckLayout->addWidget(m_pctrlColorMaterial);
	CheckLayout->addWidget(m_pctrlCullFaces);
	CheckLayout->addWidget(m_pctrlSmooth);
	CheckLayout->addWidget(m_pctrlDepthTest);
	CheckLayout->addWidget(m_pctrlShade);
	CheckLayout->addWidget(m_pctrlLocalView);
	QGroupBox *CheckBoxes = new QGroupBox(tr("Options"));
	CheckBoxes->setLayout(CheckLayout);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	m_pctrlClose = new QPushButton(tr("Close"));
	m_pctrlDefaults = new QPushButton(tr("Reset Defaults"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlDefaults);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlClose);
	CommandButtons->addStretch(1);

	QVBoxLayout *LeftSide = new QVBoxLayout;
	LeftSide->addStretch(1);
	LeftSide->addWidget(LightIntensityBox);
	LeftSide->addStretch(1);
	LeftSide->addWidget(LightColorBox);
	LeftSide->addStretch(1);
	LeftSide->addWidget(LightPositionBox);
	LeftSide->addStretch(1);

	QVBoxLayout *RightSide = new QVBoxLayout;
	RightSide->addStretch(1);
	RightSide->addWidget(MaterialDataBox);
	RightSide->addStretch(1);
	RightSide->addWidget(CheckBoxes);
	RightSide->addStretch(1);

	QHBoxLayout *TopLayout = new QHBoxLayout;
	TopLayout->addLayout(LeftSide);
	TopLayout->addLayout(RightSide);

	m_pctrlLight = new QCheckBox(tr("Light"));
	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pctrlLight);;
	MainLayout->addLayout(TopLayout);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
}

void GLLightDlg::Apply()
{
	ReadParams();
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->UpdateView();
}

void GLLightDlg::OnSlider(int pos)
{
	Apply();
}


void GLLightDlg::OnChanged()
{
	Apply();
}



void GLLightDlg::OnDefaults()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	m_bLight = true;
	m_bCullFaces = false;
	m_bShade     = true;
	m_bSmooth    = true;
	m_bLocalView = true;
	m_bDepthTest = true;
	m_bColorMaterial = true;

	m_Red   = 1.0f;
	m_Green = 1.0f;
	m_Blue  = 1.0f;

	m_Ambient      = 0.25f;
	m_Diffuse      = 0.58f;
	m_Specular     = 0.05f;

	m_MatAmbient   = -0.51f;
	m_MatDiffuse   = -0.43f;
	m_MatSpecular  = -0.08f;
	m_MatEmission  = -0.04f;
	m_iMatShininess = 0;

	m_XLight   =  0.56f;
	m_YLight   =  0.02f;
	m_ZLight   =  0.68f;

	SetParams();
	SetEnabled();
	pMiarex->UpdateView();
}



void GLLightDlg::ReadParams(void)
{
	m_bLight = m_pctrlLight->isChecked();

	m_Red     = (float)m_pctrlRed->value()    /100.0f;
	m_Green   = (float)m_pctrlGreen->value()  /100.0f;
	m_Blue    = (float)m_pctrlBlue->value()   /100.0f;

	m_Ambient     = (float)m_pctrlAmbient->value()    / 100.0f;
	m_Diffuse     = (float)m_pctrlDiffuse->value()    / 100.0f;
	m_Specular    = (float)m_pctrlSpecular->value()   / 100.0f;

	m_MatAmbient    = (float)m_pctrlMatAmbient->value()    /100.0f;
	m_MatSpecular   = (float)m_pctrlMatSpecular->value()   /100.0f;
	m_MatDiffuse    = (float)m_pctrlMatDiffuse->value()    /100.0f;
	m_MatEmission   = (float)m_pctrlMatEmission->value()   /100.0f;
	m_iMatShininess = m_pctrlMatShininess->value();

	m_bCullFaces     = m_pctrlCullFaces->isChecked();
	m_bColorMaterial = m_pctrlColorMaterial->isChecked();
	m_bSmooth        = m_pctrlSmooth->isChecked();
	m_bShade         = m_pctrlShade->isChecked();
	m_bLocalView     = m_pctrlLocalView->isChecked();
	m_bDepthTest     = m_pctrlDepthTest->isChecked();

	float factor = 50.0f;
	m_XLight  = ((float)m_pctrlXLight->value()-50.0f)/factor;
	m_YLight  = ((float)m_pctrlYLight->value()-50.0f)/factor;
	m_ZLight  = ((float)m_pctrlZLight->value()-50.0f)/factor;
}


void GLLightDlg::SetParams(void)
{
	m_pctrlLight->setChecked(m_bLight);
	float factor = 50.0f;
	m_pctrlXLight->setValue((int)((m_XLight+1.0)*factor));
	m_pctrlYLight->setValue((int)((m_YLight+1.0)*factor));
	m_pctrlZLight->setValue((int)((m_ZLight+1.0)*factor));

	m_pctrlRed->setValue(  (int)(m_Red*100.0));
	m_pctrlGreen->setValue((int)(m_Green*100.0));
	m_pctrlBlue->setValue( (int)(m_Blue*100.0));

	m_pctrlAmbient->setValue(   (int)(m_Ambient*100.0));
	m_pctrlDiffuse->setValue(   (int)(m_Diffuse*100.0));
	m_pctrlSpecular->setValue(  (int)(m_Specular*100.0));

	m_pctrlMatAmbient->setValue(   (int)(m_MatAmbient*100.0));
	m_pctrlMatDiffuse->setValue(   (int)(m_MatDiffuse*100.0));
	m_pctrlMatSpecular->setValue(  (int)(m_MatSpecular*100.0));
	m_pctrlMatEmission->setValue(  (int)(m_MatEmission*100.0));
	m_pctrlMatShininess->setValue(m_iMatShininess);

	m_pctrlCullFaces->setChecked(m_bCullFaces);
	m_pctrlSmooth->setChecked(m_bSmooth);
	m_pctrlShade->setChecked(m_bShade);
	m_pctrlLocalView->setChecked(m_bLocalView);
	m_pctrlDepthTest->setChecked(m_bDepthTest);
	m_pctrlColorMaterial->setChecked(m_bColorMaterial);
}



bool GLLightDlg::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GLLight");
	{
	//  we're reading/loading
		m_Ambient= pSettings->value("Ambient").toDouble();
		m_Diffuse= pSettings->value("Diffuse").toDouble();
		m_Specular= pSettings->value("Specular").toDouble();
		m_MatAmbient= pSettings->value("MatAmbient").toDouble();
		m_MatDiffuse= pSettings->value("MatDiffuse").toDouble();
		m_MatSpecular  = pSettings->value("MatSpecular").toDouble();
		m_MatEmission= pSettings->value("MatEmission").toDouble();
		m_iMatShininess= pSettings->value("MatShininess").toDouble();
		m_XLight = pSettings->value("XLight").toDouble();
		m_YLight = pSettings->value("YLight").toDouble();
		m_ZLight= pSettings->value("ZLight").toDouble();
		m_Red= pSettings->value("RedLight").toDouble();
		m_Green = pSettings->value("GreenLight").toDouble();
		m_Blue= pSettings->value("BlueLight").toDouble();
		m_bCullFaces = pSettings->value("CullFaces").toBool();
		m_bSmooth = pSettings->value("Smooth").toBool();
		m_bShade = pSettings->value("Shade").toBool();
		m_bLocalView = pSettings->value("LocalView").toBool();
		m_bDepthTest = pSettings->value("DepthTest").toBool();
		m_bColorMaterial = pSettings->value("ColorMaterial").toBool();
	}
	pSettings->endGroup();
	return true;

}


bool GLLightDlg::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GLLight");
	{
		pSettings->setValue("Ambient",m_Ambient);
		pSettings->setValue("Diffuse",m_Diffuse);
		pSettings->setValue("Specular",m_Specular);
		pSettings->setValue("MatAmbient",m_MatAmbient);
		pSettings->setValue("MatDiffuse",m_MatDiffuse);
		pSettings->setValue("MatSpecular",m_MatSpecular );
		pSettings->setValue("MatEmission",m_MatEmission);
		pSettings->setValue("MatShininess",m_iMatShininess);
		pSettings->setValue("XLight",m_XLight);
		pSettings->setValue("YLight",m_YLight);
		pSettings->setValue("ZLight",m_ZLight);
		pSettings->setValue("RedLight",m_Red);
		pSettings->setValue("GreenLight",m_Green);
		pSettings->setValue("BlueLight",m_Blue);
		pSettings->setValue("CullFaces",m_bCullFaces);
		pSettings->setValue("Smooth",m_bSmooth);
		pSettings->setValue("Shade",m_bShade);
		pSettings->setValue("LocalView",m_bLocalView);
		pSettings->setValue("DepthTest",m_bDepthTest);
		pSettings->setValue("ColorMaterial",m_bColorMaterial);
	}
	pSettings->endGroup();

	return true;
}

void GLLightDlg::showEvent(QShowEvent *event)
{
	SetParams();
	SetEnabled();
}


void GLLightDlg::OnLight()
{
	m_bLight = m_pctrlLight->isChecked();
	SetEnabled();
}


void GLLightDlg::SetEnabled()
{
	m_pctrlRed->setEnabled(m_bLight);
	m_pctrlGreen->setEnabled(m_bLight);
	m_pctrlBlue->setEnabled(m_bLight);

	m_pctrlAmbient->setEnabled(m_bLight);
	m_pctrlDiffuse->setEnabled(m_bLight);
	m_pctrlSpecular->setEnabled(m_bLight);

	m_pctrlMatAmbient->setEnabled(m_bLight);
	m_pctrlMatSpecular->setEnabled(m_bLight);
	m_pctrlMatDiffuse->setEnabled(m_bLight);
	m_pctrlMatEmission->setEnabled(m_bLight);
	m_pctrlMatShininess->setEnabled(m_bLight);

	m_pctrlCullFaces->setEnabled(m_bLight);
	m_pctrlColorMaterial->setEnabled(m_bLight);
	m_pctrlSmooth->setEnabled(m_bLight);
	m_pctrlShade->setEnabled(m_bLight);
	m_pctrlLocalView->setEnabled(m_bLight);
	m_pctrlDepthTest->setEnabled(m_bLight);

	m_pctrlXLight->setEnabled(m_bLight);
	m_pctrlYLight->setEnabled(m_bLight);
	m_pctrlZLight->setEnabled(m_bLight);

}









