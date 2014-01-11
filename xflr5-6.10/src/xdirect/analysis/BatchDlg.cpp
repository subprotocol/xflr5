/****************************************************************************

	BatchDlg Class
        Copyright (C) 2003-2010 Andre Deperrois adeperrois@xflr5.com

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

#include "BatchDlg.h"
#include "../XDirect.h"
#include "ReListDlg.h"
#include "../../misc/Settings.h"
#include "../../xinverse/FoilSelectionDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QtDebug>


bool BatchDlg::s_bCurrentFoil=true;
void * BatchDlg::s_pXDirect;
QPoint BatchDlg::s_Position;



BatchDlg::BatchDlg(QWidget *pParent) : QDialog(pParent)
{
	QString str = tr("Batch foil analysis");
	setWindowTitle(str);

	m_pXFile = NULL;

	m_pXFoilTask = new XFoilTask;
	m_pXFoilTask->m_pParent = this;

	m_FoilList.clear();

	m_PolarType = FIXEDSPEEDPOLAR;
	
	m_Mach  = 0.0;
	m_ReMin = 100000.0;
	m_ReInc =  50000.0;
	m_ReMax = 300000.0;
	m_SpMin = 0.0;
	m_SpMax = 1.0;
	m_SpInc = 0.5;
	m_ClMin = 0.0;
	m_ClMax = 1.0;
	m_ClInc = 0.1;

	m_ACrit  = 9.0;
	m_XTop = 1.0;
	m_XBot = 1.0;

	m_bOutput         = true;
	m_bAlpha          = true;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bInitBL         = false;
	m_bCancel         = false;
	m_bIsRunning      = false;
	m_bErrors         = false;

	m_IterRect.setRect(327,158,620-327,398-158);

	XFoil::s_bCancel = false;

	SetupLayout();

	m_pRmsGraph = m_pctrlGraphOutput->graph();
	m_pRmsGraph->CopySettings(&Settings::s_RefGraph, false);

	m_pRmsGraph->SetXTitle(tr("Iter"));
	m_pRmsGraph->SetYTitle("");//Change from bl newton system solution

	m_pRmsGraph->SetAuto(true);

	m_pRmsGraph->SetXMajGrid(true, QColor(120,120,120),2,1);
	m_pRmsGraph->SetYMajGrid(true, QColor(120,120,120),2,1);

	m_pRmsGraph->SetXMin(0.0);
	m_pRmsGraph->SetXMax(50);
	m_pRmsGraph->SetYMin(-1.0);
	m_pRmsGraph->SetYMax( 1.0);
	m_pRmsGraph->SetType(1);
	m_pRmsGraph->SetMargin(40);



	connect(m_pctrlFoil1, SIGNAL(clicked()), this, SLOT(OnFoilSelectionType()));
	connect(m_pctrlFoil2, SIGNAL(clicked()), this, SLOT(OnFoilSelectionType()));
	connect(m_pctrlFoilList, SIGNAL(clicked()), this, SLOT(OnFoilList()));
	connect(m_pctrlClose, SIGNAL(clicked()), this, SLOT(OnClose()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlSkipOpp, SIGNAL(clicked()), this, SLOT(OnSkipPoint()));
	connect(m_pctrlSkipPolar, SIGNAL(clicked()), this, SLOT(OnSkipPolar()));
	connect(m_rbtype1, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype2, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype3, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbtype4, SIGNAL(clicked()), this, SLOT(OnPolarType()));
	connect(m_rbspec1, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_rbspec2, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_rbRange1, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
	connect(m_rbRange2, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
	connect(m_pctrlEditList, SIGNAL(clicked()), this, SLOT(OnEditReList()));
}


void BatchDlg::SetupLayout()
{
	QGroupBox *pFoilBox = new QGroupBox(tr("Foil Selection"));
	{
		QHBoxLayout *FoilLayout = new QHBoxLayout;
		{
			m_pctrlFoil1 = new QRadioButton(tr("Current foil only"));
			m_pctrlFoil2 = new QRadioButton(tr("Foil list"));
			m_pctrlFoilList = new QPushButton(tr("Foil list"));
			FoilLayout->addWidget(m_pctrlFoil1);
			FoilLayout->addWidget(m_pctrlFoil2);
			FoilLayout->addStretch(1);
			FoilLayout->addWidget(m_pctrlFoilList);
		}
		pFoilBox->setLayout(FoilLayout);
	}

	QGroupBox *pAnalysisTypeGroupBox = new QGroupBox(tr("Analysis Type"));
	{
		QHBoxLayout *AnalysisTypeLayout = new QHBoxLayout;
		{
			m_rbtype1 = new	QRadioButton(tr("Type 1"));
			m_rbtype2 = new QRadioButton(tr("Type 2"));
			m_rbtype3 = new QRadioButton(tr("Type 3"));
			m_rbtype4 = new QRadioButton(tr("Type 4"));
			AnalysisTypeLayout->addWidget(m_rbtype1);
			AnalysisTypeLayout->addWidget(m_rbtype2);
			AnalysisTypeLayout->addWidget(m_rbtype3);
			AnalysisTypeLayout->addWidget(m_rbtype4);
		}
		pAnalysisTypeGroupBox->setLayout(AnalysisTypeLayout);
	}

	QGroupBox *pBatchVarsGroupBox = new QGroupBox(tr("Batch Variables"));
	{
		QGridLayout *BatchVarsLayout = new QGridLayout;
		{
			m_rbRange1 = new QRadioButton(tr("Range"));
			m_rbRange2 = new QRadioButton(tr("Re List"));
			m_pctrlEditList = new QPushButton(tr("Edit List"));
			QLabel *MinVal   = new QLabel(tr("Min"));
			QLabel *MaxVal   = new QLabel(tr("Max"));
			QLabel *DeltaVal = new QLabel(tr("Increment"));
			MinVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			MaxVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			DeltaVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			m_pctrlReType  = new QLabel("Reynolds=");
			m_pctrlMaType  = new QLabel("Mach=");
			m_pctrlReType->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlMaType->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlReMin   = new DoubleEdit(100000,0);
			m_pctrlReMax   = new DoubleEdit(150000,0);
			m_pctrlReDelta = new DoubleEdit(50000,0);
			m_pctrlMach    = new DoubleEdit(0.0, 3);

			QLabel *NCritLabel = new QLabel(tr("NCrit="));
			NCritLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlNCrit   = new DoubleEdit(9.00);

			BatchVarsLayout->addWidget(m_rbRange1, 1, 2);
			BatchVarsLayout->addWidget(m_rbRange2, 1, 3);
			BatchVarsLayout->addWidget(m_pctrlEditList, 1, 4);
			BatchVarsLayout->addWidget(MinVal, 2, 2);
			BatchVarsLayout->addWidget(MaxVal, 2, 3);
			BatchVarsLayout->addWidget(DeltaVal, 2, 4);
			BatchVarsLayout->addWidget(m_pctrlReType, 3, 1);
			BatchVarsLayout->addWidget(m_pctrlReMin, 3, 2);
			BatchVarsLayout->addWidget(m_pctrlReMax, 3, 3);
			BatchVarsLayout->addWidget(m_pctrlReDelta, 3, 4);
			BatchVarsLayout->addWidget(m_pctrlMaType, 4, 1);
			BatchVarsLayout->addWidget(m_pctrlMach, 4, 2);
			BatchVarsLayout->addWidget(NCritLabel, 5, 1);
			BatchVarsLayout->addWidget(m_pctrlNCrit, 5,2);
		}
		pBatchVarsGroupBox->setLayout(BatchVarsLayout);
	}

	QGroupBox *pRangeVarsGroupBox = new QGroupBox(tr("Analysis Range"));
	{
		QGridLayout *RangeVarsLayout = new QGridLayout;
		{
			QLabel *Spec = new QLabel(tr("Specify"));
			m_rbspec1 = new QRadioButton(tr("Alpha"));
			m_rbspec2 = new QRadioButton(tr("Cl"));
			m_pctrlFromZero   = new QCheckBox(tr("From Zero"));
			QLabel *SpecMin   = new QLabel(tr("Min"));
			QLabel *SpecMax   = new QLabel(tr("Max"));
			QLabel *SpecDelta = new QLabel(tr("Increment"));
			SpecMin->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			SpecMax->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			SpecDelta->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			m_pctrlSpecVar    = new QLabel(tr("Spec ="));
			m_pctrlSpecMin    = new DoubleEdit(0.00);
			m_pctrlSpecMax    = new DoubleEdit(1.00);
			m_pctrlSpecDelta  = new DoubleEdit(0.50);
			RangeVarsLayout->addWidget(Spec, 1, 1);
			RangeVarsLayout->addWidget(m_rbspec1, 1, 2);
			RangeVarsLayout->addWidget(m_rbspec2, 1, 3);
			RangeVarsLayout->addWidget(m_pctrlFromZero, 1, 4);
			RangeVarsLayout->addWidget(SpecMin, 2, 2);
			RangeVarsLayout->addWidget(SpecMax, 2, 3);
			RangeVarsLayout->addWidget(SpecDelta, 2, 4);
			RangeVarsLayout->addWidget(m_pctrlSpecVar, 3, 1);
			RangeVarsLayout->addWidget(m_pctrlSpecMin, 3, 2);
			RangeVarsLayout->addWidget(m_pctrlSpecMax, 3, 3);
			RangeVarsLayout->addWidget(m_pctrlSpecDelta, 3, 4);
		}
		pRangeVarsGroupBox->setLayout(RangeVarsLayout);
	}

	QGroupBox *pTransVarsGroupBox = new QGroupBox(tr("Forced transitions"));
	{
		QGridLayout *TransVarsLayout = new QGridLayout;
		{
			TransVarsLayout->setColumnStretch(0,4);
			TransVarsLayout->setColumnStretch(1,1);
			QLabel *TopTransLabel = new QLabel(tr("Top transition location (x/c)"));
			QLabel *BotTransLabel = new QLabel(tr("Bottom transition location (x/c)"));
			TopTransLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			BotTransLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlXTopTr = new DoubleEdit(1.00);
			m_pctrlXBotTr = new DoubleEdit(1.00);
			TransVarsLayout->addWidget(TopTransLabel, 1, 1);
			TransVarsLayout->addWidget(m_pctrlXTopTr, 1, 2);
			TransVarsLayout->addWidget(BotTransLabel, 2, 1);
			TransVarsLayout->addWidget(m_pctrlXBotTr, 2, 2);
		}
		pTransVarsGroupBox->setLayout(TransVarsLayout);
	}

	QHBoxLayout *OptionsLayout = new QHBoxLayout;
	{
		m_pctrlInitBL         = new QCheckBox(tr("Initialize BLs between polars"));
		m_pctrlStoreOpp       = new QCheckBox(tr("Store OpPoints"));
		OptionsLayout->addWidget(m_pctrlInitBL);
		OptionsLayout->addWidget(m_pctrlStoreOpp);
	}

	QHBoxLayout *CommandButtonsLayout = new QHBoxLayout;
	{
		m_pctrlClose     = new QPushButton(tr("Close"));
		m_pctrlAnalyze   = new QPushButton(tr("Analyze"))	;
		m_pctrlSkipOpp   = new QPushButton(tr("Skip Opp"));
		m_pctrlSkipPolar = new QPushButton(tr("Skip Polar"));
		m_pctrlAnalyze->setAutoDefault(true);

		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(m_pctrlAnalyze);
		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(m_pctrlSkipOpp);
		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(m_pctrlSkipPolar);
		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(m_pctrlClose);
		CommandButtonsLayout->addStretch(1);
	}

	QVBoxLayout *LeftSideLayout = new QVBoxLayout;
	{
		LeftSideLayout->addWidget(pFoilBox);
		LeftSideLayout->addWidget(pAnalysisTypeGroupBox);
		LeftSideLayout->addWidget(pBatchVarsGroupBox);
		LeftSideLayout->addWidget(pTransVarsGroupBox);
		LeftSideLayout->addWidget(pRangeVarsGroupBox);
		LeftSideLayout->addSpacing(20);
		LeftSideLayout->addStretch(1);
		LeftSideLayout->addLayout(CommandButtonsLayout);
	}

	QVBoxLayout *RightSideLayout = new QVBoxLayout;
	{
		m_pctrlTextOutput = new QTextEdit;
		m_pctrlTextOutput->setReadOnly(true);
		m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
		m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
		m_pctrlGraphOutput = new GraphWidget;

/*		QFont outFont;
		outFont.setStyleHint(QFont::TypeWriter);
		outFont.setFamily(outFont.defaultFamily());
		outFont.setPointSize(8);*/

		m_pctrlTextOutput->setFont(Settings::s_TableFont);
		QFontMetrics fm(Settings::s_TableFont);
		m_pctrlTextOutput->setMinimumWidth(57*fm.averageCharWidth());

		RightSideLayout->addLayout(OptionsLayout);
		RightSideLayout->addWidget(m_pctrlTextOutput,1);
		RightSideLayout->addWidget(m_pctrlGraphOutput,2);
	}

	QHBoxLayout *mainLayout= new QHBoxLayout;
	{
		mainLayout->setSpacing(10);
		mainLayout->addLayout(LeftSideLayout);
		mainLayout->addLayout(RightSideLayout);
	}
	setLayout(mainLayout);
}



void BatchDlg::AlphaLoop()
{
	QString str;
	int iAlpha, nAlpha;
	double alphadeg;
//	QPoint Place(m_pctrlGraphOutput->rect().left()+m_pRmsGraph->GetMargin()*2, m_pctrlGraphOutput->rect().top()+m_pRmsGraph->GetMargin()/2);


	nAlpha = (int)(qAbs((m_SpMax-m_SpMin)*1.000/m_SpInc));//*1.0001 to make sure upper limit is included

	for (iAlpha=0; iAlpha<=nAlpha; iAlpha++)
	{
		if(m_bCancel) break;
		
		alphadeg = m_SpMin + iAlpha*m_SpInc;
		str = QString("Alpha = %1\n").arg(alphadeg,0,'f',2);
		OutputMsg(str);

		m_pCurPolar = CreatePolar(alphadeg, m_Mach, m_ACrit);// Do something
		if(!m_pCurPolar) return;

		m_pXFoilTask->setReRange(m_ReMin, m_ReMax, m_ReInc);
		m_pXFoilTask->InitializeTask(m_pFoil, m_pCurPolar, QXDirect::s_bViscous, m_bInitBL, m_bFromZero);

		m_pXFoilTask->run();

		m_bErrors = m_bErrors || m_pXFoilTask->m_bErrors;

		if(m_bCancel)
		{
			str = tr("Analysis interrupted")+"\n";
			OutputMsg(str);
			break;
		}
	}//end Re loop
}





void BatchDlg::CleanUp()
{
	ResetCurves();
	if(m_pXFile->isOpen())	m_pXFile->close();
	m_pctrlClose->setEnabled(true);
	m_pctrlSkipOpp->setEnabled(false);
	m_pctrlSkipPolar->setEnabled(false);
	m_pctrlAnalyze->setText(tr("Analyze"));
	m_bIsRunning = false;
	m_bCancel    = false;
	XFoil::s_bCancel = false;
	m_pctrlClose->setFocus();
}


Polar *BatchDlg::CreatePolar(double Spec, double Mach, double NCrit)
{
	if(!m_pFoil) return NULL;

	Polar *pPolar = new Polar;
	pPolar->m_FoilName   = m_pFoil->m_FoilName;
	pPolar->m_bIsVisible = true;

	pPolar->m_PolarType = m_PolarType;

	switch (pPolar->m_PolarType)
	{
		case FIXEDSPEEDPOLAR:
			pPolar->m_MaType = 1;
			pPolar->m_ReType = 1;
			break;
		case FIXEDLIFTPOLAR:
			pPolar->m_MaType = 2;
			pPolar->m_ReType = 2;
			break;
		case RUBBERCHORDPOLAR:
			pPolar->m_MaType = 1;
			pPolar->m_ReType = 3;
			break;
		case FIXEDAOAPOLAR:
			pPolar->m_MaType = 1;
			pPolar->m_ReType = 1;
			break;
		default:
			pPolar->m_ReType = 1;
			pPolar->m_MaType = 1;
			break;
	}

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		pPolar->m_Reynolds = Spec;
	}
	else
	{
		pPolar->m_ASpec = Spec;
	}
	pPolar->m_Mach  = Mach;
	pPolar->m_ACrit = NCrit;
	pPolar->m_XTop  = m_XTop;
	pPolar->m_XBot  = m_XBot;

	SetPlrName(pPolar);
	Polar *pOldPolar = Polar::getPolar(m_pFoil, pPolar->m_PlrName);

	if(pOldPolar)
	{
		delete pPolar;
		pPolar = pOldPolar;
	}
	else Polar::addPolar(pPolar);
	return pPolar;
}


void BatchDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(m_pctrlClose->hasFocus())	     done(1);
			else if(m_pctrlAnalyze->hasFocus())  OnAnalyze();
			else                                 m_pctrlAnalyze->setFocus();
			break;
		}
		case Qt::Key_Escape:
		{
			if(m_bIsRunning)
			{
				m_bCancel    = true;
				XFoil::s_bCancel = true;
			}
			else
			{
				reject();
				//close the dialog box
			}
			break;
		}
		default:
			event->ignore();
	}
}


void BatchDlg::InitDialog()
{
	if(!m_pFoil) return;

	m_ACrit     = QXDirect::s_refPolar.m_ACrit;
	m_XBot      = QXDirect::s_refPolar.m_XBot;
	m_XTop      = QXDirect::s_refPolar.m_XTop;
	m_Mach      = QXDirect::s_refPolar.m_Mach;
	m_PolarType = FIXEDSPEEDPOLAR;


	m_pctrlFoil1->setChecked(s_bCurrentFoil);
	m_pctrlFoil2->setChecked(!s_bCurrentFoil);
	m_pctrlFoilList->setEnabled(!s_bCurrentFoil);
	
	if(m_bAlpha)
	{
		m_SpMin     = m_AlphaMin;
		m_SpMax     = m_AlphaMax;
		m_SpInc     = m_AlphaInc;
	}
	else
	{
		m_SpMin     = m_ClMin;
		m_SpMax     = m_ClMax;
		m_SpInc     = m_ClInc;
	}

	if(m_ReMin<=0.0) m_ReMin = qAbs(m_ReInc);

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_pctrlReMin->SetPrecision(0);
		m_pctrlReMax->SetPrecision(0);
		m_pctrlReDelta->SetPrecision(0);

		m_pctrlSpecMin->SetPrecision(1);
		m_pctrlSpecMax->SetPrecision(1);
		m_pctrlSpecDelta->SetPrecision(1);

		m_pctrlReMin->SetValue(m_ReMin);
		m_pctrlReMax->SetValue(m_ReMax);
		m_pctrlReDelta->SetValue(m_ReInc);
		m_pctrlSpecMin->SetValue(m_SpMin);
		m_pctrlSpecMax->SetValue(m_SpMax);
		m_pctrlSpecDelta->SetValue(m_SpInc);
		m_rbRange2->setEnabled(true);
	}
	else
	{
		m_pctrlReMin->SetPrecision(1);
		m_pctrlReMax->SetPrecision(1);
		m_pctrlReDelta->SetPrecision(1);

		m_pctrlSpecMin->SetPrecision(0);
		m_pctrlSpecMax->SetPrecision(0);
		m_pctrlSpecDelta->SetPrecision(0);
		m_pctrlReMin->SetValue(m_SpMin);
		m_pctrlReMax->SetValue(m_SpMax);
		m_pctrlReDelta->SetValue(m_SpInc);
		m_pctrlSpecMin->SetValue(m_ReMin);
		m_pctrlSpecMax->SetValue(m_ReMax);
		m_pctrlSpecDelta->SetValue(m_ReInc);
		m_rbRange2->setEnabled(false);
		m_bFromList = false;
	}


	m_pctrlMach->SetValue(m_Mach);
	m_pctrlNCrit->SetValue(m_ACrit);
	m_pctrlXTopTr->SetValue(m_XTop);
	m_pctrlXBotTr->SetValue(m_XBot);

	if(m_bAlpha) m_rbspec1->setChecked(true);
	else         m_rbspec2->setChecked(true);
	OnAcl();

	if(m_PolarType==FIXEDSPEEDPOLAR)       m_rbtype1->setChecked(true);
	else if(m_PolarType==FIXEDLIFTPOLAR)   m_rbtype2->setChecked(true);
	else if(m_PolarType==RUBBERCHORDPOLAR) m_rbtype3->setChecked(true);
	else if(m_PolarType==FIXEDAOAPOLAR)    m_rbtype4->setChecked(true);
	OnPolarType();


	if(!m_bFromList)  m_rbRange1->setChecked(!m_bFromList);
	else              m_rbRange2->setChecked(m_bFromList);
	OnRange();

	if(m_bFromZero)  m_pctrlFromZero->setChecked(true);
	else             m_pctrlFromZero->setChecked(false);

	m_pctrlInitBL->setChecked(true);
	m_pctrlStoreOpp->setChecked(OpPoint::s_bStoreOpp);

	m_pctrlSkipOpp->setEnabled(false);
	m_pctrlSkipPolar->setEnabled(false);

	ResetCurves();
}





void BatchDlg::OnAcl()
{
	if(m_PolarType==FIXEDAOAPOLAR) return;
	if(m_rbspec1->isChecked())
	{
		m_pctrlSpecVar->setText(tr("Alpha ="));
		m_pctrlSpecMin->SetValue(m_AlphaMin);
		m_pctrlSpecMax->SetValue(m_AlphaMax);
		m_pctrlSpecDelta->SetValue(m_AlphaInc);
		m_bAlpha = true;
		m_pctrlFromZero->setEnabled(true);
	}
	else
	{
		m_pctrlSpecVar->setText(tr("Cl ="));
		m_pctrlSpecMin->SetValue(m_ClMin);
		m_pctrlSpecMax->SetValue(m_ClMax);
		m_pctrlSpecDelta->SetValue(m_ClInc);
		m_bAlpha = false;
		m_pctrlFromZero->setEnabled(false);
	}
}



void BatchDlg::OnPolarType()
{
	if(m_rbtype1->isChecked())
	{
		m_pctrlReType->setText(tr("Reynolds ="));
		m_pctrlMaType->setText(tr("Mach ="));
		m_pctrlEditList->setEnabled(true);
		m_PolarType = FIXEDSPEEDPOLAR;
	}
	else if(m_rbtype2->isChecked())
	{
		m_pctrlReType->setText(tr("Re.sqrt(Cl) ="));
		m_pctrlMaType->setText(tr("Ma.sqrt(Cl) ="));
		m_pctrlEditList->setEnabled(true);
		m_PolarType = FIXEDLIFTPOLAR;
	}
	else if(m_rbtype3->isChecked())
	{
		m_pctrlReType->setText(tr("Re.Cl ="));
		m_pctrlMaType->setText(tr("Mach ="));
		m_pctrlEditList->setEnabled(true);
		m_PolarType = RUBBERCHORDPOLAR;
	}
	else if(m_rbtype4->isChecked())
	{
		m_pctrlReType->setText(tr("Alpha ="));
		m_pctrlMaType->setText(tr("Mach ="));
		m_pctrlEditList->setEnabled(false);
		m_rbspec1->setChecked(true);
		m_PolarType = FIXEDAOAPOLAR;
	}

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_pctrlReMin->SetPrecision(0);
		m_pctrlReMax->SetPrecision(0);
		m_pctrlReDelta->SetPrecision(0);
		m_pctrlSpecMin->SetPrecision(2);
		m_pctrlSpecMax->SetPrecision(2);
		m_pctrlSpecDelta->SetPrecision(2);
		m_rbspec1->setEnabled(true);
		m_rbspec2->setEnabled(true);
		m_rbRange2->setEnabled(true);
		OnAcl();

		m_pctrlReMin->SetValue(m_ReMin);
		m_pctrlReMax->SetValue(m_ReMax);
		m_pctrlReDelta->SetValue(m_ReInc);

		m_pctrlSpecMin->SetValue(m_SpMin);
		m_pctrlSpecMax->SetValue(m_SpMax);
		m_pctrlSpecDelta->SetValue(m_SpInc);
	}
	else
	{
		m_pctrlReMin->SetPrecision(2);
		m_pctrlReMax->SetPrecision(2);
		m_pctrlReDelta->SetPrecision(2);
		m_pctrlSpecMin->SetPrecision(0);
		m_pctrlSpecMax->SetPrecision(0);
		m_pctrlSpecDelta->SetPrecision(0);
		m_pctrlSpecVar->setText(tr("Reynolds ="));
		m_rbspec1->setEnabled(false);
		m_rbspec2->setEnabled(false);

		m_rbRange1->setChecked(true);
		m_rbRange2->setEnabled(false);
		m_bFromList = false;

		m_pctrlReMin->SetValue(m_SpMin);
		m_pctrlReMax->SetValue(m_SpMax);
		m_pctrlReDelta->SetValue(m_SpInc);

		m_pctrlSpecMin->SetValue(m_ReMin);
		m_pctrlSpecMax->SetValue(m_ReMax);
		m_pctrlSpecDelta->SetValue(m_ReInc);
	}
}



void BatchDlg::OnAnalyze()
{
	if(m_bIsRunning)
	{
		XFoil::s_bCancel = true;
		XFoilTask::s_bCancel = true;
		m_bCancel = true;
		return;
	}
	m_bCancel    = false;
	m_bIsRunning = true;
	m_bErrors    = false;

	//read data specification
	ReadParams();

	//set output file
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;
	SetFileHeader();

	//initialize XFoil task
	m_pXFoilTask->m_OutStream.setDevice(m_pXFile);

	if(m_bAlpha) m_pXFoilTask->setSequence(true,  m_AlphaMin, m_AlphaMax, m_AlphaInc);
	else         m_pXFoilTask->setSequence(false, m_ClMin, m_ClMax, m_ClInc);

	m_pXFoilTask->setReRange(m_ReMin, m_ReMax, m_ReInc);
	m_pXFoilTask->InitializeTask(Foil::curFoil(), Polar::curPolar(),
						    QXDirect::s_bViscous, m_bInitBL, m_bFromZero);


	//prepare button state for analysis
	m_pctrlAnalyze->setText(tr("Cancel"));
	m_pctrlSkipOpp->setEnabled(true);
	m_pctrlSkipPolar->setEnabled(true);
	m_pctrlTextOutput->clear();
	m_pctrlClose->setEnabled(false);
	m_pctrlAnalyze->setFocus();

	Analyze();
}



void BatchDlg::OnClose()
{
	m_bCancel = true;
	XFoil::s_bCancel= true;
	XFoilTask::s_bCancel = true;

	ReadParams();

	QXDirect::s_refPolar.m_ACrit    = m_ACrit;
	QXDirect::s_refPolar.m_XBot     = m_XBot;
	QXDirect::s_refPolar.m_XTop     = m_XTop;
	QXDirect::s_refPolar.m_Mach     = m_Mach;

	done(1);
}



void BatchDlg::OnEditReList()
{
	ReListDlg dlg(this);
	dlg.InitDialog(QXDirect::s_ReList,QXDirect::s_MachList, QXDirect::s_NCritList);

	if(QDialog::Accepted == dlg.exec())
	{
		QXDirect::s_ReList.clear();
		QXDirect::s_MachList.clear();
		QXDirect::s_NCritList.clear();

		QXDirect::s_ReList.append(dlg.m_ReList);
		QXDirect::s_MachList.append(dlg.m_MachList);
		QXDirect::s_NCritList.append(dlg.m_NCritList);
	}
}



void BatchDlg::OnFoilList()
{
	QXDirect  *pXDirect   = (QXDirect*)s_pXDirect;
    FoilSelectionDlg dlg(this);
//	dlg.SetSelectionMode(true);
	dlg.m_poaFoil = pXDirect->m_poaFoil;
	if(m_pFoil) dlg.m_FoilName = m_pFoil->m_FoilName;
	dlg.m_FoilList.clear();
	for(int i=0; i<m_FoilList.size(); i++)
	{
		dlg.m_FoilList.append(m_FoilList.at(i));
	}
	dlg.InitDialog();

	m_FoilList.clear();
	if(QDialog::Accepted == dlg.exec())
	{
		for(int i=0; i<dlg.m_FoilList.count();i++)
		{
			m_FoilList.append(dlg.m_FoilList.at(i));
		}
	}
}


void BatchDlg::OnFoilSelectionType()
{
	s_bCurrentFoil = m_pctrlFoil1->isChecked();
	m_pctrlFoilList->setEnabled(!s_bCurrentFoil);
}




void BatchDlg::OnRange()
{
	if(m_rbRange1->isChecked())
		m_bFromList = false;
	else
		m_bFromList = true;

	m_pctrlEditList->setEnabled(m_bFromList);
	m_pctrlReMin->setEnabled(!m_bFromList);
	m_pctrlReMax->setEnabled(!m_bFromList);
	m_pctrlReDelta->setEnabled(!m_bFromList);
	m_pctrlMach->setEnabled(!m_bFromList);
	m_pctrlNCrit->setEnabled(!m_bFromList);
//	m_pctrlXBotTr->setEnabled(!m_bFromList);
//	m_pctrlXTopTr->setEnabled(!m_bFromList);
}


void BatchDlg::OnSkipPoint()
{
	XFoilTask::s_bSkipOpp = true;
}


void BatchDlg::OnSkipPolar()
{
	XFoilTask::s_bSkipOpp   = true;
	XFoilTask::s_bSkipPolar = true;
}



void BatchDlg::ReadParams()
{
	m_bAlpha = m_rbspec1->isChecked();

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_ReInc = m_pctrlReDelta->Value();
		m_ReMax = m_pctrlReMax->Value();
		m_ReMin = m_pctrlReMin->Value();

		if(m_bAlpha)
		{
			m_AlphaInc = m_pctrlSpecDelta->Value();
			m_AlphaMax = m_pctrlSpecMax->Value();
			m_AlphaMin = m_pctrlSpecMin->Value();
			if(m_AlphaMin< m_AlphaMax) m_AlphaInc =  qAbs(m_SpInc);
			else                       m_AlphaInc = -qAbs(m_SpInc);
		}
		else
		{
			m_ClInc = m_pctrlSpecDelta->Value();
			m_ClMax = m_pctrlSpecMax->Value();
			m_ClMin = m_pctrlSpecMin->Value();
			if(m_ClMin< m_ClMax) m_ClInc =  qAbs(m_ClInc);
			else                 m_ClInc = -qAbs(m_ClInc);
		}
	}
	else
	{
		m_SpInc = m_pctrlReDelta->Value();
		m_SpMax = m_pctrlReMax->Value();
		m_SpMin = m_pctrlReMin->Value();

		m_ReInc = m_pctrlSpecDelta->Value();
		m_ReMax = m_pctrlSpecMax->Value();
		m_ReMin = m_pctrlSpecMin->Value();
	}

	if(m_ReMin<=0.0) m_ReMin = qAbs(m_ReInc);
	if(m_ReMax<=0.0) m_ReMax = qAbs(m_ReMax);
	m_SpInc = qAbs(m_SpInc);


	m_Mach     = m_pctrlMach->Value();
	if(m_Mach<=0.0) m_Mach = 0.0;
	m_ACrit  = m_pctrlNCrit->Value();
	m_XTop   = m_pctrlXTopTr->Value();
	m_XBot   = m_pctrlXBotTr->Value();
	
	OpPoint::s_bStoreOpp = m_pctrlStoreOpp->isChecked();
	m_bInitBL = m_pctrlInitBL->isChecked();
	m_bFromZero = m_pctrlFromZero->isChecked();
}




/**
 * For Type 1, 2 and 3 Polar objects
 * Loops through all the specified Relist, and for each element of the list:
 *	- creates a Polar object
 *   - initializes the XFoilTaks object
 *   - launches the XFoilTask whcih will loop through the specified aoa or Cl range
 */
void BatchDlg::ReLoop()
{
	QString str;

	int iRe, nRe;
	double Reynolds =0, Mach = 0, NCrit = 9.0;

	if(!m_bFromList) nRe = (int)qAbs((m_ReMax-m_ReMin)/m_ReInc);
	else             nRe = QXDirect::s_ReList.count()-1;

	for (iRe=0; iRe<=nRe; iRe++)
	{
		if(!m_bFromList)
		{
			Reynolds = m_ReMin + iRe *m_ReInc;
			Mach  = m_Mach;
			NCrit  = m_ACrit;
		}
		else
		{
			Reynolds = QXDirect::s_ReList[iRe];
			Mach     = QXDirect::s_MachList[iRe];
			NCrit    = QXDirect::s_NCritList[iRe];
		}
		str = QString("Re=%1   Ma=%2   Nc=%3\n").arg(Reynolds,8,'f',0).arg(Mach,5,'f',3).arg(NCrit,5,'f',2);
		OutputMsg(str);

		m_pCurPolar = CreatePolar(Reynolds, Mach, NCrit);
		if(!m_pCurPolar) return;

		m_pXFoilTask->InitializeTask(m_pFoil, m_pCurPolar, QXDirect::s_bViscous, m_bInitBL, m_bFromZero);
		m_pXFoilTask->run();

		m_bErrors = m_bErrors || m_pXFoilTask->m_bErrors;
		str = "\n";
		OutputMsg(str);
		if(m_bCancel) break;

	}//end Re loop
}


void BatchDlg::ResetCurves()
{
	m_pRmsGraph->DeleteCurves();
	Curve *pCurve0 = m_pRmsGraph->AddCurve();
	Curve *pCurve1 = m_pRmsGraph->AddCurve();
	pCurve0->SetTitle("rms");
	pCurve1->SetTitle("max");
	pCurve1->SetStyle(0);
	m_pRmsGraph->SetAutoX(false);
	m_pRmsGraph->SetXMin(0.0);
	m_pRmsGraph->SetXMax((double)XFoilTask::s_IterLim);
	m_pRmsGraph->SetXUnit((int)(XFoilTask::s_IterLim/10.0));
	m_pRmsGraph->SetYMin(-1.0);
	m_pRmsGraph->SetYMax( 1.0);
	m_pRmsGraph->SetX0(0.0);
	m_pRmsGraph->SetY0(0.0);

	m_pXFoilTask->setGraphPointers(m_pRmsGraph, pCurve0, pCurve1);

}


void BatchDlg::SetFileHeader()
{
	if(!m_pXFile) return;
	QTextStream out(m_pXFile);

	out << "\n";
	out << VERSIONNAME;
	out << "\n";
	out << m_pFoil->foilName();
	out << "\n";
	if(Polar::curPolar())
	{
//	out << pXDirect->m_pCurPolar->m_PlrName;
//	out << "\n";
	}

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";

}



void BatchDlg::SetPlrName(Polar *pPolar)
{
	if(pPolar->m_PolarType!=FIXEDAOAPOLAR)
	{
		double R = pPolar->m_Reynolds/1000000.;
		pPolar->m_PlrName = QString("T%1_Re%2_M%3")
								 .arg(pPolar->m_PolarType+1)
								 .arg(R,0,'f',3)
								 .arg( pPolar->m_Mach,0,'f',2);
	}
	else
	{
		pPolar->m_PlrName = QString("T4_Al%2_M%3")
								 .arg(pPolar->m_ASpec,5,'f',2)
								 .arg(pPolar->m_Mach,0,'f',2);
	}
	QString str;
	str = QString("_N%1").arg(pPolar->m_ACrit,0,'f',1);
	pPolar->m_PlrName += str;
}



void BatchDlg::Analyze()
{
	QXDirect *pXDirect = (QXDirect*)s_pXDirect;
	QString strong;

	m_pctrlAnalyze->setText(tr("Cancel"));

	//create a timer to update the output at regular intervals
	QTimer *pTimer = new QTimer;
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnProgress()));
	pTimer->setInterval(100);
	pTimer->start();

	if(s_bCurrentFoil)
	{
		if(m_PolarType!=FIXEDAOAPOLAR) ReLoop();
		else                           AlphaLoop();
	}
	else
	{

		for(int i=0; i<m_FoilList.count();i++)
		{
			m_pFoil = Foil::foil(m_FoilList.at(i));

			strong = tr("Analyzing ")+m_pFoil->m_FoilName+("\n");
			OutputMsg(strong);
			if(m_PolarType!=FIXEDAOAPOLAR) ReLoop();
			else                           AlphaLoop();
			strong = "\n\n";
			OutputMsg(strong);
		}
	}

	if(pXDirect->m_bPolarView)
	{
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}

	pTimer->stop();
	delete pTimer;

	OnProgress();

	if(m_bErrors)
	{
		strong = tr(" ...some points are unconverged") + "\n";
		OutputMsg(strong);
	}

	strong = tr("Analysis completed")+"\n";
	OutputMsg(strong);

	m_pXFoilTask->m_OutStream.flush();

	OnProgress();

	CleanUp();

}



void BatchDlg::OnProgress()
{
	m_pctrlGraphOutput->update();

	if(m_pXFoilTask->m_OutMessage.length())
	{
		m_pctrlTextOutput->insertPlainText(m_pXFoilTask->m_OutMessage);
//		m_pctrlTextOutput->textCursor().movePosition(QTextCursor::End);
		m_pctrlTextOutput->ensureCursorVisible();
	}
	m_pXFoilTask->m_OutMessage.clear();
}


void BatchDlg::OutputMsg(QString &msg)
{
	if(!msg.length()) return;

	m_pXFoilTask->traceLog(msg);

}


void BatchDlg::showEvent(QShowEvent *event)
{
	move(s_Position);
	event->accept();
}

void BatchDlg::hideEvent(QHideEvent *event)
{
    s_Position = pos();
	event->accept();
}














