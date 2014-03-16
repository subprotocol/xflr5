/****************************************************************************

	WingScaleDlg Class
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

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

#include "WingScaleDlg.h"
#include "../../globals.h"
#include "../../misc/Units.h"


WingScaleDlg::WingScaleDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Scale Wing Dlg"));
	m_bSweep = m_bSpan = m_bChord = m_bTwist = false;

	m_NewSweep = m_NewChord = m_NewTwist = m_NewSpan = 1.0;
	m_RefSweep = m_RefChord = m_RefTwist = m_RefSpan = 1.0;

	m_RefArea = m_RefAR = 1.0;
	m_NewArea = m_NewAR = 1.0;

	SetupLayout();
}


void WingScaleDlg::SetupLayout()
{
	QGridLayout *pScaleLayout = new QGridLayout;
	{
		m_pctrlSpan  = new QCheckBox(tr("Span scaling"));
		m_pctrlChord = new QCheckBox(tr("Chord scaling"));
		m_pctrlSweep = new QCheckBox(tr("Sweep scaling"));
		m_pctrlTwist = new QCheckBox(tr("Twist scaling"));
		m_pctrlScaleArea = new QCheckBox(tr("Area scaling"));
		m_pctrlScaleAR   = new QCheckBox(tr("Aspect ratio scaling"));

		m_pctrlNewSpan  = new DoubleEdit(0,3);
		m_pctrlNewChord = new DoubleEdit(0,3);
		m_pctrlNewTwist = new DoubleEdit(0,3);
		m_pctrlNewSweep = new DoubleEdit(0,3);
		m_pctrlNewArea  = new DoubleEdit(0,3);
		m_pctrlNewAR    = new DoubleEdit(0,3);

		m_pctrlRefSpan  = new QLabel("0.000");
		m_pctrlRefChord = new QLabel("0.000");
		m_pctrlRefSweep = new QLabel("0.000");
		m_pctrlRefTwist = new QLabel("0.000");
		m_pctrlRefArea = new QLabel("0.000");
		m_pctrlRefAR   = new QLabel("0.000");

		m_pctrlRefSpan->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlRefChord->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlRefSweep->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlRefTwist->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlRefArea->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlRefAR->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		m_pctrlSpanRatio  = new QLabel("1.000");
		m_pctrlChordRatio = new QLabel("1.000");
		m_pctrlSweepRatio = new QLabel("1.000");
		m_pctrlTwistRatio = new QLabel("1.000");
		m_pctrlAreaRatio = new QLabel("1.000");
		m_pctrlARRatio = new QLabel("1.000");
		m_pctrlSpanRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlChordRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlSweepRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlTwistRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlAreaRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		m_pctrlARRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

		QLabel *lab11 = new QLabel(tr("Reference"));
		QLabel *lab12 = new QLabel(tr("New"));
		QLabel *lab13 = new QLabel(tr("Ratio"));
		lab11->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		lab12->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		lab13->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
		pScaleLayout->addWidget(lab11, 1,2);
		pScaleLayout->addWidget(lab12, 1,3);
		pScaleLayout->addWidget(lab13, 1,5);

		pScaleLayout->addWidget(m_pctrlSpan,      2,1);
		pScaleLayout->addWidget(m_pctrlRefSpan,   2,2);
		pScaleLayout->addWidget(m_pctrlNewSpan,   2,3);
		pScaleLayout->addWidget(m_pctrlSpanRatio, 2,5);

		pScaleLayout->addWidget(m_pctrlChord,     3,1);
		pScaleLayout->addWidget(m_pctrlRefChord,  3,2);
		pScaleLayout->addWidget(m_pctrlNewChord,  3,3);
		pScaleLayout->addWidget(m_pctrlChordRatio,3,5);

		pScaleLayout->addWidget(m_pctrlSweep,     4,1);
		pScaleLayout->addWidget(m_pctrlRefSweep,  4,2);
		pScaleLayout->addWidget(m_pctrlNewSweep,  4,3);
		pScaleLayout->addWidget(m_pctrlSweepRatio,4,5);

		pScaleLayout->addWidget(m_pctrlTwist,     5,1);
		pScaleLayout->addWidget(m_pctrlRefTwist,  5,2);
		pScaleLayout->addWidget(m_pctrlNewTwist,  5,3);
		pScaleLayout->addWidget(m_pctrlTwistRatio,5,5);

		pScaleLayout->addWidget(m_pctrlScaleArea, 6,1);
		pScaleLayout->addWidget(m_pctrlRefArea,   6,2);
		pScaleLayout->addWidget(m_pctrlNewArea,   6,3);
		pScaleLayout->addWidget(m_pctrlAreaRatio, 6,5);

		pScaleLayout->addWidget(m_pctrlScaleAR, 7,1);
		pScaleLayout->addWidget(m_pctrlRefAR,   7,2);
		pScaleLayout->addWidget(m_pctrlNewAR,   7,3);
		pScaleLayout->addWidget(m_pctrlARRatio, 7,5);

		QString unitLabel;
		Units::getLengthUnitLabel(unitLabel);
		QLabel *pLengthUnit1 = new QLabel(unitLabel);
		QLabel *pLengthUnit2 = new QLabel(unitLabel);

		QLabel *pAngleUnit1 = new QLabel(QString::fromUtf8("°"));
		QLabel *pAngleUnit2 = new QLabel(QString::fromUtf8("°"));

		Units::getAreaUnitLabel(unitLabel);
		QLabel *pAreaUnit = new QLabel(unitLabel);
		pScaleLayout->addWidget(pLengthUnit1, 2, 4);
		pScaleLayout->addWidget(pLengthUnit2, 3, 4);
		pScaleLayout->addWidget(pAngleUnit1,  4, 4);
		pScaleLayout->addWidget(pAngleUnit2,  5, 4);
		pScaleLayout->addWidget(pAreaUnit,    6, 4);
	}

	QHBoxLayout *pCommandButtons = new QHBoxLayout;
	{
		QPushButton *OKButton = new QPushButton(tr("OK"));
		QPushButton *CancelButton = new QPushButton(tr("Cancel"));
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(OKButton);
		pCommandButtons->addStretch(1);
		pCommandButtons->addWidget(CancelButton);
		pCommandButtons->addStretch(1);

		connect(OKButton, SIGNAL(clicked()), this, SLOT(OnOK()));
		connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	}

	QHBoxLayout *pStretchLayout = new QHBoxLayout;
	{
		pStretchLayout->addStretch(1);
		pStretchLayout->addLayout(pScaleLayout);
		pStretchLayout->addStretch(1);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pStretchLayout);
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(pCommandButtons);
		pMainLayout->addStretch(1);
	}

	setLayout(pMainLayout);

	connect(m_pctrlSpan,  SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlChord, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlSweep, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlTwist, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlScaleArea, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlScaleAR, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));

	connect(m_pctrlNewSpan,  SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewChord, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewSweep, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewTwist, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewArea, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewAR, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}


void WingScaleDlg::InitDialog(double const &RefSpan, double const &RefChord, double const &RefSweep, double const &RefTwist,
						double const &RefArea, double const &RefAR)
{
	m_RefSpan  = RefSpan;
	m_RefChord = RefChord;
	m_RefSweep = RefSweep;
	m_RefTwist = RefTwist; 
	m_RefArea  = RefArea;
	m_RefAR    = RefAR;

	m_NewSpan  = RefSpan;
	m_NewChord = RefChord;
	m_NewSweep = RefSweep;
	m_NewTwist = RefTwist; 
	m_NewArea  = RefArea;
	m_NewAR    = RefAR;

	m_pctrlSpan->setChecked(m_bSpan);//(false)
	m_pctrlChord->setChecked(m_bChord);
	m_pctrlTwist->setChecked(m_bTwist);
	m_pctrlSweep->setChecked(m_bSweep);
	m_pctrlScaleArea->setChecked(m_bArea);
	m_pctrlScaleAR->setChecked(m_bAR);


	QString strong;

	strong = QString("%1").arg(m_RefSpan * Units::mtoUnit(),8,'f',3);
	m_pctrlRefSpan->setText(strong);

	strong = QString("%1").arg(m_RefChord * Units::mtoUnit(),8,'f',3);
	m_pctrlRefChord->setText(strong);

	strong = QString("%1").arg(m_RefSweep,8,'f',2);
	strong += QString::fromUtf8("°");
	m_pctrlRefSweep->setText(strong);

	strong = QString("%1").arg(m_RefTwist,8,'f',2);
	m_pctrlRefTwist->setText(strong);

	strong = QString ("%1").arg(m_RefArea *Units::m2toUnit(), 8,'f',3);
	m_pctrlRefArea->setText(strong);

	strong = QString ("%1").arg(m_RefAR , 8,'f',3);
	m_pctrlRefAR->setText(strong);

	m_pctrlNewSpan->SetValue(m_NewSpan * Units::mtoUnit());
	m_pctrlNewChord->SetValue(m_NewChord * Units::mtoUnit());
	m_pctrlNewSweep->SetValue(m_NewSweep);
	m_pctrlNewTwist->SetValue(m_NewTwist);
	m_pctrlNewArea->SetValue(m_NewArea);
	m_pctrlNewAR->SetValue(m_NewAR);

	SetResults();
	EnableControls();
}



void WingScaleDlg::OnClickedCheckBox()
{
	ReadData();
	EnableControls();
}


void WingScaleDlg::OnOK()
{
	ReadData();
	accept();
}

void WingScaleDlg::OnEditingFinished()
{
	ReadData();
	SetResults();
}



void WingScaleDlg::EnableControls()
{
	m_pctrlNewSpan->setEnabled(m_bSpan);
	m_pctrlNewChord->setEnabled(m_bChord);
	m_pctrlNewSweep->setEnabled(m_bSweep);
	m_pctrlNewTwist->setEnabled(m_bTwist);
	m_pctrlNewArea->setEnabled(m_bArea);
	m_pctrlNewAR->setEnabled(m_bAR);
}


void WingScaleDlg::ReadData()
{
	m_bSpan  = m_pctrlSpan->isChecked();
	m_bChord = m_pctrlChord->isChecked();
	m_bSweep = m_pctrlSweep->isChecked();
	m_bTwist = m_pctrlTwist->isChecked();
	m_bArea  = m_pctrlScaleArea->isChecked();
	m_bAR    = m_pctrlScaleAR->isChecked();

	m_NewSpan  = m_pctrlNewSpan->Value()  / Units::mtoUnit();
	m_NewChord = m_pctrlNewChord->Value() / Units::mtoUnit();
	m_NewSweep = m_pctrlNewSweep->Value();
	m_NewTwist = m_pctrlNewTwist->Value();
	m_NewArea  = m_pctrlNewArea->Value() /Units::m2toUnit();
	m_NewAR    = m_pctrlNewAR->Value();
}


void WingScaleDlg::SetResults()
{
	QString strong;

	if(m_RefSpan>0.0)  strong = QString("%1").arg(m_NewSpan/m_RefSpan, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlSpanRatio->setText(strong);

	if(m_RefChord>0.0) strong = QString("%1").arg(m_NewChord/m_RefChord, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlChordRatio->setText(strong);

	if(m_RefSweep>0.0) strong = QString("%1").arg(m_NewSweep/m_RefSweep, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlSweepRatio->setText(strong);

	if(m_RefTwist>0.0) strong = QString("%1").arg(m_NewTwist/m_RefTwist, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlTwistRatio->setText(strong);

	if(m_RefArea>0.0)  strong = QString("%1").arg(m_NewArea/m_RefArea, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlAreaRatio->setText(strong);

	if(m_RefAR>0.0)    strong = QString("%1").arg(m_NewAR/m_RefAR, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlARRatio->setText(strong);

}


