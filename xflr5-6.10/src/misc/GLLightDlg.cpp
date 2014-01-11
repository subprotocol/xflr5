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
#include "../threedwidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QShowEvent>

float GLLightDlg::s_Ambient, GLLightDlg::s_Diffuse, GLLightDlg::s_Specular;
float GLLightDlg::s_MatAmbient, GLLightDlg::s_MatDiffuse, GLLightDlg::s_MatSpecular, GLLightDlg::s_MatEmission;
float GLLightDlg::s_Red, GLLightDlg::s_Green, GLLightDlg::s_Blue;
float GLLightDlg::s_XLight, GLLightDlg::s_YLight, GLLightDlg::s_ZLight;
int GLLightDlg::s_iMatShininess;
bool GLLightDlg::s_bCullFaces, GLLightDlg::s_bShade, GLLightDlg::s_bSmooth, GLLightDlg::s_bLocalView, GLLightDlg::s_bDepthTest;
bool GLLightDlg::s_bColorMaterial;
bool GLLightDlg::s_bLight;



GLLightDlg::GLLightDlg(QWidget *pParent) : QDialog(pParent)
{
	m_pGL3dBodyDlg = m_pGL3dWingDlg = NULL;

	m_Size = 5.0;
	SetDefaults(m_Size);

	setWindowTitle(tr("OpenGL Light Options"));
    setModal(false);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    SetupLayout();

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

	QGroupBox *LightIntensityBox = new QGroupBox(tr("Light Intensity"));
	QGridLayout *LightIntensity = new QGridLayout;
	{
		QLabel *lab1 = new QLabel(tr("Diffuse"));
		QLabel *lab2 = new QLabel(tr("Ambient"));
		QLabel *lab3 = new QLabel(tr("Specular"));
		LightIntensity->addWidget(lab1,1,1);
		LightIntensity->addWidget(lab2,2,1);
		LightIntensity->addWidget(lab3,3,1);
		LightIntensity->addWidget(m_pctrlDiffuse,1,2);
		LightIntensity->addWidget(m_pctrlAmbient,2,2);
		LightIntensity->addWidget(m_pctrlSpecular,3,2);
		LightIntensityBox->setLayout(LightIntensity);
	}

	QGroupBox *LightColorBox = new QGroupBox(tr("Light Color"));
	{
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
		LightColorBox->setLayout(LightColor);
	}

	QGroupBox *LightPositionBox = new QGroupBox(tr("Light Position"));
	{
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
		LightPositionBox->setLayout(LightPosition);
	}

	QGroupBox *MaterialDataBox = new QGroupBox(tr("Material"));
	{
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
		MaterialDataBox->setLayout(MaterialData);
	}

	QGroupBox *CheckBoxes = new QGroupBox(tr("Options"));
	{
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
		CheckBoxes->setLayout(CheckLayout);
	}

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		m_pctrlClose = new QPushButton(tr("Close"));
		m_pctrlDefaults = new QPushButton(tr("Reset Defaults"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(m_pctrlDefaults);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(m_pctrlClose);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout *LeftSide = new QVBoxLayout;
	{
		LeftSide->addStretch(1);
		LeftSide->addWidget(LightIntensityBox);
		LeftSide->addStretch(1);
		LeftSide->addWidget(LightColorBox);
		LeftSide->addStretch(1);
		LeftSide->addWidget(LightPositionBox);
		LeftSide->addStretch(1);
	}

	QVBoxLayout *RightSide = new QVBoxLayout;
	{
		RightSide->addStretch(1);
		RightSide->addWidget(MaterialDataBox);
		RightSide->addStretch(1);
		RightSide->addWidget(CheckBoxes);
		RightSide->addStretch(1);
	}

	QHBoxLayout *TopLayout = new QHBoxLayout;
	{
		TopLayout->addLayout(LeftSide);
		TopLayout->addLayout(RightSide);
	}

	QVBoxLayout *MainLayout = new QVBoxLayout;
	{
		m_pctrlLight = new QCheckBox(tr("Light"));
		MainLayout->addWidget(m_pctrlLight);;
		MainLayout->addLayout(TopLayout);
		MainLayout->addLayout(CommandButtons);
	}

	setLayout(MainLayout);
}


void GLLightDlg::Apply()
{
	ReadParams();

	ThreeDWidget *p3dWidget = (ThreeDWidget*)m_p3DWidget;
    p3dWidget->updateGL();
}


void GLLightDlg::OnSlider(int)
{
	Apply();
}


void GLLightDlg::OnChanged()
{
	Apply();
}


void GLLightDlg::OnDefaults()
{
	ThreeDWidget *p3dWidget = (ThreeDWidget*)m_p3DWidget;

	SetDefaults(m_Size);

	SetParams();
	SetEnabled();
    p3dWidget->updateGL();
}



void GLLightDlg::ReadParams(void)
{
	s_bLight = m_pctrlLight->isChecked();

	s_Red     = (float)m_pctrlRed->value()    /100.0f;
	s_Green   = (float)m_pctrlGreen->value()  /100.0f;
	s_Blue    = (float)m_pctrlBlue->value()   /100.0f;

	s_MatAmbient    = (float)m_pctrlMatAmbient->value()    /100.0f;
	s_MatSpecular   = (float)m_pctrlMatSpecular->value()   /100.0f;
	s_MatDiffuse    = (float)m_pctrlMatDiffuse->value()    /100.0f;
	s_MatEmission   = (float)m_pctrlMatEmission->value()   /100.0f;
	s_iMatShininess = m_pctrlMatShininess->value();

	s_bCullFaces     = m_pctrlCullFaces->isChecked();
	s_bColorMaterial = m_pctrlColorMaterial->isChecked();
	s_bSmooth        = m_pctrlSmooth->isChecked();
	s_bShade         = m_pctrlShade->isChecked();
	s_bLocalView     = m_pctrlLocalView->isChecked();
	s_bDepthTest     = m_pctrlDepthTest->isChecked();

	float factor = 10.0f;
	s_XLight  = ((float)m_pctrlXLight->value()-50.0f)/factor;
	s_YLight  = ((float)m_pctrlYLight->value()-50.0f)/factor;
	s_ZLight  = ((float)m_pctrlZLight->value())/factor;

	s_Ambient     = (float)m_pctrlAmbient->value()  / 100.0f;
	s_Diffuse     = (float)m_pctrlDiffuse->value()  / 100.0f;
	s_Specular    = (float)m_pctrlSpecular->value() / 100.0f;
}


void GLLightDlg::SetParams(void)
{
	m_pctrlLight->setChecked(s_bLight);

	m_pctrlAmbient->setValue(   (int)(s_Ambient  *100.0));
	m_pctrlDiffuse->setValue(   (int)(s_Diffuse  *100.0));
	m_pctrlSpecular->setValue(  (int)(s_Specular *100.0));

	float factor = 10.0f;
	m_pctrlXLight->setValue((int)((s_XLight+5.0)*factor));
	m_pctrlYLight->setValue((int)((s_YLight+5.0)*factor));
	m_pctrlZLight->setValue((int)((s_ZLight)*factor));

	m_pctrlRed->setValue(  (int)(s_Red  *100.0));
	m_pctrlGreen->setValue((int)(s_Green*100.0));
	m_pctrlBlue->setValue( (int)(s_Blue *100.0));

	m_pctrlMatAmbient->setValue(   (int)(s_MatAmbient *100.0));
	m_pctrlMatDiffuse->setValue(   (int)(s_MatDiffuse *100.0));
	m_pctrlMatSpecular->setValue(  (int)(s_MatSpecular*100.0));
	m_pctrlMatEmission->setValue(  (int)(s_MatEmission*100.0));
	m_pctrlMatShininess->setValue(s_iMatShininess);

	m_pctrlCullFaces->setChecked(s_bCullFaces);
	m_pctrlSmooth->setChecked(s_bSmooth);
	m_pctrlShade->setChecked(s_bShade);
	m_pctrlLocalView->setChecked(s_bLocalView);
	m_pctrlDepthTest->setChecked(s_bDepthTest);
	m_pctrlColorMaterial->setChecked(s_bColorMaterial);
}



bool GLLightDlg::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GLLight");
	{
	//  we're reading/loading
		s_Diffuse           = pSettings->value("Diffuse",0.26).toDouble();
		s_Ambient           = pSettings->value("Ambient",0.18).toDouble();
		s_Specular          = pSettings->value("Specular",0.05).toDouble();
		s_MatAmbient        = pSettings->value("MatAmbient",-0.51).toDouble();
		s_MatDiffuse        = pSettings->value("MatDiffuse",-0.43).toDouble();
		s_MatSpecular       = pSettings->value("MatSpecular",-0.08).toDouble();
		s_MatEmission       = pSettings->value("MatEmission",-0.04).toDouble();
		s_iMatShininess     = pSettings->value("MatShininess",0).toDouble();
		s_XLight            = pSettings->value("XLight", 0.56).toDouble();
		s_YLight            = pSettings->value("YLight", 0.02).toDouble();
		s_ZLight            = pSettings->value("ZLight", 0.68).toDouble();
		s_Red               = pSettings->value("RedLight",1.0).toDouble();
		s_Green             = pSettings->value("GreenLight",1.0).toDouble();
		s_Blue              = pSettings->value("BlueLight",1.0).toDouble();
		s_bCullFaces        = pSettings->value("CullFaces",false).toBool();
		s_bSmooth           = pSettings->value("Smooth",true).toBool();
		s_bShade            = pSettings->value("Shade",true).toBool();
		s_bLocalView        = pSettings->value("LocalView",true).toBool();
		s_bDepthTest        = pSettings->value("DepthTest",true).toBool();
		s_bColorMaterial    = pSettings->value("ColorMaterial",true).toBool();
		s_bLight            = pSettings->value("bLight", true).toBool();
	}
	pSettings->endGroup();
	return true;
}


void GLLightDlg::SetDefaults(double)
{
	s_Red   = 1.0f;
	s_Green = 1.0f;
	s_Blue  = 1.0f;

	s_Diffuse      = 0.83f;
	s_Ambient      = 0.18f;
	s_Specular     = 0.05f;

	s_MatAmbient   = -0.51f;
	s_MatDiffuse   = -0.43f;
	s_MatSpecular  = -0.08f;
	s_MatEmission  = -0.06f;
	s_iMatShininess = 0;

	s_XLight   =  0.56f * m_Size;//.2
	s_YLight   =  0.02f * m_Size;//1.2
	s_ZLight   =  5.0f * m_Size;//20

	s_bLight = true;
	s_bCullFaces = false;
	s_bShade     = true;
	s_bSmooth    = true;
	s_bLocalView = true;
	s_bDepthTest = true;
	s_bColorMaterial = true;
}



bool GLLightDlg::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("GLLight");
	{
		pSettings->setValue("Ambient",s_Ambient);
		pSettings->setValue("Diffuse",s_Diffuse);
		pSettings->setValue("Specular",s_Specular);
		pSettings->setValue("MatAmbient",s_MatAmbient);
		pSettings->setValue("MatDiffuse",s_MatDiffuse);
		pSettings->setValue("MatSpecular",s_MatSpecular );
		pSettings->setValue("MatEmission",s_MatEmission);
		pSettings->setValue("MatShininess",s_iMatShininess);
		pSettings->setValue("XLight",s_XLight);
		pSettings->setValue("YLight",s_YLight);
		pSettings->setValue("ZLight",s_ZLight);
		pSettings->setValue("RedLight",s_Red);
		pSettings->setValue("GreenLight",s_Green);
		pSettings->setValue("BlueLight",s_Blue);
		pSettings->setValue("CullFaces",s_bCullFaces);
		pSettings->setValue("Smooth",s_bSmooth);
		pSettings->setValue("Shade",s_bShade);
		pSettings->setValue("LocalView",s_bLocalView);
		pSettings->setValue("DepthTest",s_bDepthTest);
		pSettings->setValue("ColorMaterial",s_bColorMaterial);
		pSettings->setValue("bLight", s_bLight);
	}
	pSettings->endGroup();

	return true;
}


void GLLightDlg::showEvent(QShowEvent *event)
{
	SetParams();
	SetEnabled();
	Apply();
	event->accept();
}


void GLLightDlg::OnLight()
{
	s_bLight = m_pctrlLight->isChecked();
	SetEnabled();
	Apply();
}


void GLLightDlg::SetEnabled()
{
	m_pctrlRed->setEnabled(s_bLight);
	m_pctrlGreen->setEnabled(s_bLight);
	m_pctrlBlue->setEnabled(s_bLight);

	m_pctrlAmbient->setEnabled(s_bLight);
	m_pctrlDiffuse->setEnabled(s_bLight);
	m_pctrlSpecular->setEnabled(s_bLight);

	m_pctrlMatAmbient->setEnabled(s_bLight);
	m_pctrlMatSpecular->setEnabled(s_bLight);
	m_pctrlMatDiffuse->setEnabled(s_bLight);
	m_pctrlMatEmission->setEnabled(s_bLight);
	m_pctrlMatShininess->setEnabled(s_bLight);

	m_pctrlCullFaces->setEnabled(s_bLight);
	m_pctrlColorMaterial->setEnabled(s_bLight);
	m_pctrlSmooth->setEnabled(s_bLight);
	m_pctrlShade->setEnabled(s_bLight);
	m_pctrlLocalView->setEnabled(s_bLight);
	m_pctrlDepthTest->setEnabled(s_bLight);

	m_pctrlXLight->setEnabled(s_bLight);
	m_pctrlYLight->setEnabled(s_bLight);
	m_pctrlZLight->setEnabled(s_bLight);

}









