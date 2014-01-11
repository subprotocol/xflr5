/****************************************************************************

	InterpolateFoilsDlg Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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

#include <QGroupBox>
#include <QVBoxLayout>
#include "InterpolateFoilsDlg.h"
#include "../analysis/XFoil.h"


void *InterpolateFoilsDlg::s_pXFoil;

#define SLIDERSCALE 10000

InterpolateFoilsDlg::InterpolateFoilsDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Interpolate Foils"));

	m_pParent = pParent;
	m_pBufferFoil = NULL;
	m_poaFoil = NULL;


	SetupLayout();

	connect(m_pctrlFoil1,  SIGNAL(activated(int)),    this, SLOT(OnSelChangeFoil1(int)));
	connect(m_pctrlFoil2,  SIGNAL(activated(int)),    this, SLOT(OnSelChangeFoil2(int)));
	connect(m_pctrlFrac,   SIGNAL(editingFinished()), this, SLOT(OnFrac()));
	connect(m_pctrlSlider, SIGNAL(sliderMoved(int)),  this, SLOT(OnVScroll(int)));
}


void InterpolateFoilsDlg::SetupLayout()
{
	QVBoxLayout *LeftSide = new QVBoxLayout;
	{
		m_pctrlFoil1 = new QComboBox;
		m_pctrlFoil1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		m_pctrlFoil2 = new QComboBox;
		m_pctrlCamb1 = new QLabel(tr("Camb1"));
		m_pctrlCamb2 = new QLabel(tr("Camb2"));
		m_pctrlThick1 = new QLabel(tr("Thick1"));
		m_pctrlThick2 = new QLabel(tr("Thick2"));
		m_pctrlCamb1->setMinimumWidth(250);
		m_pctrlCamb2->setMinimumWidth(250);
		m_pctrlThick1->setMinimumWidth(250);
		m_pctrlThick2->setMinimumWidth(250);
		LeftSide->addWidget(m_pctrlFoil1);
		LeftSide->addWidget(m_pctrlCamb1);
		LeftSide->addWidget(m_pctrlThick1);
		LeftSide->addStretch(1);
		LeftSide->addWidget(m_pctrlFoil2);
		LeftSide->addWidget(m_pctrlCamb2);
		LeftSide->addWidget(m_pctrlThick2);
	}


	m_pctrlSlider = new QSlider;
	m_pctrlSlider->setMinimumHeight(300);
	m_pctrlSlider->setMinimum(0);
	m_pctrlSlider->setMaximum(SLIDERSCALE);
	m_pctrlSlider->setTickInterval(SLIDERSCALE/10);
	m_pctrlSlider->setTickPosition(QSlider::TicksLeft);


	QVBoxLayout *Foil3Layout = new QVBoxLayout;
	{
		m_pctrlFrac = new DoubleEdit;
		m_pctrlCamb3 = new QLabel(tr("Camb3"));
		m_pctrlCamb3->setMinimumWidth(250);
		m_pctrlThick3 = new QLabel(tr("Thick3"));
		m_pctrlThick3->setMinimumWidth(250);
		m_pctrlFrac->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		m_pctrlCamb3->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		m_pctrlThick3->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		Foil3Layout->addStretch();
		Foil3Layout->addWidget(m_pctrlFrac);
		Foil3Layout->addWidget(m_pctrlCamb3);
		Foil3Layout->addWidget(m_pctrlThick3);
		Foil3Layout->addStretch();
	}

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		CancelButton = new QPushButton(tr("Cancel"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
		connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
		connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	}


	QHBoxLayout *pColumnLayout = new QHBoxLayout;
	{
		pColumnLayout->addLayout(LeftSide);
		pColumnLayout->addStretch(1);
		pColumnLayout->addWidget(m_pctrlSlider);
		pColumnLayout->addLayout(Foil3Layout);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addLayout(pColumnLayout);
		pMainLayout->addLayout(CommandButtons);
	}

	setLayout(pMainLayout);
	setMinimumWidth(400);
	setMinimumHeight(400);
}


void InterpolateFoilsDlg::InitDialog()
{
	int i;
	Foil* pFoil;
	m_pctrlFoil1->clear();
	m_pctrlFoil2->clear();
	for (i=0; i<m_poaFoil->size(); i++)
	{
		pFoil = (Foil*)m_poaFoil->at(i);
		if(pFoil)
		{
			m_pctrlFoil1->addItem(pFoil->m_FoilName);
			m_pctrlFoil2->addItem(pFoil->m_FoilName);
		}
	}
	m_pctrlFoil1->setCurrentIndex(0);
	m_pctrlFoil2->setCurrentIndex(1);

	m_Frac = 0.0;
	m_pctrlFrac->SetValue(100);
	m_pctrlSlider->setSliderPosition(SLIDERSCALE);

	OnSelChangeFoil1(0);
	OnSelChangeFoil2(1);
}


void InterpolateFoilsDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	// Generate the foil instead
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				Update();
				OKButton->setFocus();
			}
			else if (OKButton->hasFocus())
			{
				OnOK();
			}
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


void InterpolateFoilsDlg::OnSelChangeFoil1(int)
{
	QString strong  = m_pctrlFoil1->currentText();

	Foil* pFoil = Foil::foil(strong);

	if(pFoil)
	{
		QString str;
		str = QString(tr("Camb.=%1")).arg(pFoil->m_fCamber*100,5,'f',2);
		str += "%";
		strong = QString(tr(" at x=%1")).arg(pFoil->m_fXCamber*100,5,'f',1);
		strong += "%";
		str+=strong;
		m_pctrlCamb1->setText(str);

		str = QString(tr("Thick.=%1")).arg(pFoil->m_fThickness*100,5,'f',2);
		str += "%";
		strong = QString(tr(" at x=%1")).arg(pFoil->m_fXThickness*100,5,'f',1);
		strong += "%";
		str+=strong;
		m_pctrlThick1->setText(str);

		m_NewFoilName = pFoil->m_FoilName;
	}
	Update();
}


void InterpolateFoilsDlg::OnSelChangeFoil2(int)
{
	QString strong  = m_pctrlFoil2->currentText();

	Foil* pFoil = Foil::foil(strong);

	if(pFoil)
	{
		QString str;
		str = QString(tr("Camb.=%1")).arg(pFoil->m_fCamber*100,5,'f',2);
		str += "%";
		strong = QString(tr(" at x=%1")).arg(pFoil->m_fXCamber*100,5,'f',1);
		strong += "%";
		str+=strong;
		m_pctrlCamb2->setText(str);

		str = QString(tr("Thick.=%1")).arg(pFoil->m_fThickness*100,5,'f',2);
		str += "%";
		strong = QString(tr(" at x=%1")).arg(pFoil->m_fXThickness*100,5,'f',1);
		strong += "%";
		str+=strong;
		m_pctrlThick2->setText(str);
	}
	Update();
}


void InterpolateFoilsDlg::Update()
{
	XFoil *pXFoil = (XFoil*)s_pXFoil;
	QString strong;

	strong = m_pctrlFoil1->currentText();
	Foil* pFoil1 = Foil::foil(strong);

	strong = m_pctrlFoil2->currentText();
	Foil* pFoil2 = Foil::foil(strong);

	if(!pFoil1 || !pFoil2) return;

	pXFoil->Interpolate(pFoil1->x, pFoil1->y, pFoil1->n,
						pFoil2->x, pFoil2->y, pFoil2->n,
						m_Frac/100.0);

	for (int j=0; j< pFoil1->n; j++)
	{
		m_pBufferFoil->x[j]  = pXFoil->xb[j+1];
		m_pBufferFoil->y[j]  = pXFoil->yb[j+1];
		m_pBufferFoil->xb[j] = pXFoil->xb[j+1];
		m_pBufferFoil->yb[j] = pXFoil->yb[j+1];
	}
	m_pBufferFoil->n  = pFoil1->n;
	m_pBufferFoil->nb = pFoil1->n;

	m_pBufferFoil->InitFoil();

	QString str;
	str = QString(tr("Camb.=%1")).arg(m_pBufferFoil->m_fCamber*100,5,'f',2);
	str += "%";
	strong = QString(tr(" at x=%1")).arg(m_pBufferFoil->m_fXCamber*100,5,'f',1);
	strong += "%";
	str+=strong;
	m_pctrlCamb3->setText(str);

	str = QString(tr("Thick.=%1")).arg(m_pBufferFoil->m_fThickness*100,5,'f',2);
	str += "%";
	strong = QString(tr(" at x=%1")).arg(m_pBufferFoil->m_fXThickness*100,5,'f',1);
	strong += "%";
	str+=strong;
	m_pctrlThick3->setText(str);

	m_pParent->update();
}


void InterpolateFoilsDlg::OnFrac()
{
	m_Frac = m_pctrlFrac->Value();
	m_pctrlSlider->setSliderPosition((int)(m_Frac/100.0*SLIDERSCALE));
	m_Frac = 100.0 - m_Frac;

	Update();
}


void InterpolateFoilsDlg::OnOK()
{
	m_pBufferFoil->m_FoilName = m_NewFoilName;

	QDialog::accept();
}


void InterpolateFoilsDlg::OnVScroll(int val)
{
	val = m_pctrlSlider->sliderPosition();
	m_Frac = (SLIDERSCALE - (double)val)/SLIDERSCALE*100.0;
	m_pctrlFrac->SetValue(100.0-m_Frac);
	Update();
}










