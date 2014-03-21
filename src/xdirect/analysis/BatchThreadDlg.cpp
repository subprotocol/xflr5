/****************************************************************************

	BatchThreadDlg Class
	   Copyright (C) 2003-2014 Andre Deperrois adeperrois@xflr5.com

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

#include "BatchThreadDlg.h"
#include "XFoilAdvancedDlg.h"
#include "ReListDlg.h"
#include "../XDirect.h"
#include "../../misc/Settings.h"
#include "../../xinverse/FoilSelectionDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>
#include <QThread>
#include <QThreadPool>
#include <QtDebug>

bool BatchThreadDlg::s_bCurrentFoil=true;
bool BatchThreadDlg::s_bUpdatePolarView = false;
void * BatchThreadDlg::s_pXDirect;
QPoint BatchThreadDlg::s_Position;


/**
 * The public contructor
 */
BatchThreadDlg::BatchThreadDlg(QWidget *pParent) : QDialog(pParent)
{
	QString str = tr("Multi-threaded batch analysis");
	setWindowTitle(str);

	m_pXFoilTask = NULL;
	m_pXFile = NULL;

	m_PolarType = FIXEDSPEEDPOLAR;

	m_FoilList.clear();

	m_Mach  = 0.0;
	m_ReMin = 100000.0;
	m_ReMax = 300000.0;
	m_ReInc =  50000.0;

	m_ClMin = 0.0;
	m_ClMax = 1.0;
	m_ClInc = 0.1;

	m_ACrit  = 9.0;
	m_XTop = 1.0;
	m_XBot = 1.0;

	m_bAlpha          = true;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bInitBL         = false;
	m_bCancel         = false;

	m_bIsRunning      = false;

	XFoil::s_bCancel = false;
	XFoilTask::s_bSkipOpp = false;
	XFoilTask::s_bSkipPolar = false;

	SetupLayout();
	connect(m_pctrlFoil1, SIGNAL(clicked()), this, SLOT(OnFoilSelectionType()));
	connect(m_pctrlFoil2, SIGNAL(clicked()), this, SLOT(OnFoilSelectionType()));
	connect(m_pctrlFoilList, SIGNAL(clicked()), this, SLOT(OnFoilList()));
	connect(m_pctrlClose, SIGNAL(clicked()), this, SLOT(OnClose()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlAlpha, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_pctrlCl, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_rbRange1, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
//	connect(m_rbRange2, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
	connect(m_pctrlEditList, SIGNAL(clicked()), this, SLOT(OnEditReList()));
	connect(m_pctrlFromZero, SIGNAL(stateChanged(int)), this, SLOT(OnFromZero(int)));
	connect(m_pctrlSpecMin, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecMax, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecDelta, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
}



/**
 * This course of action will lead us to destruction.
 */
BatchThreadDlg::~BatchThreadDlg()
{
//	Trace("Destroying BatchThreadDlg");
	if(m_pXFoilTask) delete [] m_pXFoilTask;
	if(m_pXFile)     delete m_pXFile;

	//clean up the rest of the analysis in case of cancellation
	for(int ia=m_AnalysisPair.count()-1; ia>=0; ia--)
	{
		Analysis *pAnalysis = m_AnalysisPair.last();
		m_AnalysisPair.removeLast();
		delete pAnalysis;
	}
}


/**
 * Sets up the GUI
 */
void BatchThreadDlg::SetupLayout()
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::MinimumExpanding);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QGroupBox *FoilBox = new QGroupBox(tr("Foil Selection"));
	{
		QHBoxLayout *FoilLayout = new QHBoxLayout;
		m_pctrlFoil1 = new QRadioButton(tr("Current foil only"), this);
		m_pctrlFoil2 = new QRadioButton(tr("Foil list"), this);
		m_pctrlFoilList = new QPushButton(tr("Foil list"), this);
		FoilLayout->addWidget(m_pctrlFoil1);
		FoilLayout->addWidget(m_pctrlFoil2);
		FoilLayout->addStretch(1);
		FoilLayout->addWidget(m_pctrlFoilList);
		FoilBox->setLayout(FoilLayout);
	}

	QGroupBox *pBatchVarsGroupBox = new QGroupBox(tr("Batch Variables"));
	{
		QGridLayout *BatchVarsLayout = new QGridLayout;
		{
			m_rbRange1 = new QRadioButton(tr("Range"), this);
			m_rbRange2 = new QRadioButton(tr("Re List"), this);
			m_pctrlEditList = new QPushButton(tr("Edit List"));
			QLabel *MinVal   = new QLabel(tr("Min"));
			QLabel *MaxVal   = new QLabel(tr("Max"));
			QLabel *DeltaVal = new QLabel(tr("Increment"));
			MinVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			MaxVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
			DeltaVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

			m_pctrlReType  = new QLabel("Reynolds=");
			m_pctrlMaType  = new QLabel("Mach=");
			QLabel *NCritLabel = new QLabel(tr("NCrit="));
			m_pctrlReType->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			NCritLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlMaType->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlACrit   = new DoubleEdit(9.00,2, this);

			m_pctrlReMin   = new DoubleEdit(100000,0,this);
			m_pctrlReMax   = new DoubleEdit(150000,0,this);
			m_pctrlReDelta = new DoubleEdit(50000,0, this);
			m_pctrlMach    = new DoubleEdit(0.00, 3, this);

			BatchVarsLayout->addWidget(MinVal, 2, 2);
			BatchVarsLayout->addWidget(MaxVal, 2, 3);
			BatchVarsLayout->addWidget(DeltaVal, 2, 4);
			BatchVarsLayout->addWidget(m_pctrlReType, 3, 1);
			BatchVarsLayout->addWidget(m_pctrlReMin, 3, 2);
			BatchVarsLayout->addWidget(m_pctrlReMax, 3, 3);
			BatchVarsLayout->addWidget(m_pctrlReDelta, 3, 4);
			BatchVarsLayout->addWidget(m_pctrlMaType, 4, 1);
			BatchVarsLayout->addWidget(m_pctrlMach, 4, 2);
			BatchVarsLayout->addWidget(NCritLabel, 5,1);
			BatchVarsLayout->addWidget(m_pctrlACrit, 5, 2);
		}

		QHBoxLayout *RangeSpecLayout = new QHBoxLayout;
		{
			RangeSpecLayout->addWidget(m_rbRange1);
			RangeSpecLayout->addWidget(m_rbRange2);
			RangeSpecLayout->addStretch(1);
			RangeSpecLayout->addWidget(m_pctrlEditList);
		}

		QVBoxLayout *BatchVarsGroupLayout = new QVBoxLayout;
		{
			BatchVarsGroupLayout->addLayout(RangeSpecLayout);
			BatchVarsGroupLayout->addLayout(BatchVarsLayout);
			pBatchVarsGroupBox->setLayout(BatchVarsGroupLayout);
		}
	}

	QGroupBox *pRangeVarsGroupBox = new QGroupBox(tr("Analysis Range"));
	{
		QHBoxLayout *RangeSpecLayout = new QHBoxLayout;
		{
			QLabel *Spec = new QLabel(tr("Specify:"));
			m_pctrlAlpha = new QRadioButton(tr("Alpha"));
			m_pctrlCl = new QRadioButton(tr("Cl"));
			m_pctrlFromZero   = new QCheckBox(tr("From Zero"));
			RangeSpecLayout->addWidget(Spec);
			RangeSpecLayout->addWidget(m_pctrlAlpha);
			RangeSpecLayout->addWidget(m_pctrlCl);
			RangeSpecLayout->addStretch(1);
			RangeSpecLayout->addWidget(m_pctrlFromZero);
		}

		QGridLayout *RangeVarsLayout = new QGridLayout;
		{
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
			RangeVarsLayout->addWidget(SpecMin, 1, 2);
			RangeVarsLayout->addWidget(SpecMax, 1, 3);
			RangeVarsLayout->addWidget(SpecDelta, 1, 4);
			RangeVarsLayout->addWidget(m_pctrlSpecVar, 2, 1);
			RangeVarsLayout->addWidget(m_pctrlSpecMin, 2, 2);
			RangeVarsLayout->addWidget(m_pctrlSpecMax, 2, 3);
			RangeVarsLayout->addWidget(m_pctrlSpecDelta, 2, 4);
		}

		QVBoxLayout *RangeVarsGroupLayout = new QVBoxLayout;
		{
			RangeVarsGroupLayout->addLayout(RangeSpecLayout);
			RangeVarsGroupLayout->addLayout(RangeVarsLayout);
			pRangeVarsGroupBox->setLayout(RangeVarsGroupLayout);
		}
	}

	QGroupBox *pTransVarsGroupBox = new QGroupBox(tr("Forced Transitions"));
	{
		QGridLayout *TransVars = new QGridLayout;
		{
			TransVars->setColumnStretch(0,4);
			TransVars->setColumnStretch(1,1);
			QLabel *TopTransLabel = new QLabel(tr("Top transition location (x/c)"));
			QLabel *BotTransLabel = new QLabel(tr("Bottom transition location (x/c)"));
			TopTransLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			BotTransLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlXTopTr = new DoubleEdit(1.00);
			m_pctrlXBotTr = new DoubleEdit(1.00);

			TransVars->addWidget(TopTransLabel, 2, 1);
			TransVars->addWidget(m_pctrlXTopTr, 2, 2);
			TransVars->addWidget(BotTransLabel, 3, 1);
			TransVars->addWidget(m_pctrlXBotTr, 3, 2);
		}
		pTransVarsGroupBox->setLayout(TransVars);
	}


	QHBoxLayout *OptionsLayout = new QHBoxLayout;
	{
		m_pctrlInitBL          = new QCheckBox(tr("Initialize BLs between polars"));
		m_pctrlUpdatePolarView = new QCheckBox(tr("Update polar view"));
		m_pctrlUpdatePolarView->setToolTip(tr("Update the polar graphs after the completion of each foil/polar pair"));
		OptionsLayout->addWidget(m_pctrlInitBL);
		OptionsLayout->addStretch(1);
		OptionsLayout->addWidget(m_pctrlUpdatePolarView);
	}

	//_*_*_*_*_*_*_**_*_*_**_*_*_*_

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		QPushButton *pAdvancedSettings =  new QPushButton(tr("Advanced Settings"));
		connect(pAdvancedSettings, SIGNAL(clicked()), this, SLOT(OnAdvancedSettings()));

		m_pctrlClose     = new QPushButton(tr("Close"));
		m_pctrlAnalyze   = new QPushButton(tr("Analyze"))	;
		m_pctrlAnalyze->setAutoDefault(true);

		CommandButtons->addStretch(1);
		CommandButtons->addWidget(pAdvancedSettings);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(m_pctrlAnalyze);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(m_pctrlClose);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout *LeftSide = new QVBoxLayout;
	{
		LeftSide->addWidget(FoilBox);
		LeftSide->addWidget(pBatchVarsGroupBox);
		LeftSide->addWidget(pTransVarsGroupBox);
		LeftSide->addWidget(pRangeVarsGroupBox);
		LeftSide->addStretch(1);
		LeftSide->addSpacing(20);
		LeftSide->addLayout(CommandButtons);
	}

	QVBoxLayout *RightSide = new QVBoxLayout;
	{
		m_pctrlTextOutput = new QTextEdit;
		m_pctrlTextOutput->setReadOnly(true);
		m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
		m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
		m_pctrlTextOutput->setSizePolicy(szPolicyMinimum);
		m_pctrlTextOutput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

/*		QFont outFont;
		outFont.setStyleHint(QFont::TypeWriter);
		outFont.setFamily(outFont.defaultFamily());
		outFont.setPointSize(8);*/

		m_pctrlTextOutput->setFont(Settings::s_TableFont);
		QFontMetrics fm(Settings::s_TableFont);
		m_pctrlTextOutput->setMinimumWidth(67*fm.averageCharWidth());

	//	RightSide->addStretch(1);
		RightSide->addLayout(OptionsLayout);
		RightSide->addWidget(m_pctrlTextOutput,1);
	//	RightSide->addStretch(1);
	}

	QHBoxLayout *BoxesLayout = new QHBoxLayout;
	{
		BoxesLayout->setSpacing(10);
		BoxesLayout->addLayout(LeftSide);
		BoxesLayout->addLayout(RightSide);
	}

	setLayout(BoxesLayout);
}


/**
 * Clean-up performed when all the threads are terminated
 */
void BatchThreadDlg::CleanUp()
{
	if(m_pXFile->isOpen())
	{
		QTextStream out(m_pXFile);
		out<<m_pctrlTextOutput->toPlainText();
		m_pXFile->close();
	}
	m_pctrlClose->setEnabled(true);
	m_pctrlAnalyze->setText(tr("Analyze"));
	m_bIsRunning = false;
	m_bCancel    = false;
	XFoil::s_bCancel = false;
	m_pctrlClose->setFocus();


}


/**
 * Creates a polar object for a given set of specified input data
 * @param pFoil a pointer to the Foil object to which the Polar will be attached
 * @param Re  the value of the Reynolds number
 * @param Mach  the value of the Mach number
 * @param NCrit the value of the transition criterion
 * @return a pointer to the Polar object which has been created
 */
Polar * BatchThreadDlg::CreatePolar(Foil *pFoil, double Re, double Mach, double NCrit)
{
	if(!pFoil) return NULL;

	Polar *pNewPolar = new Polar;
	pNewPolar->m_FoilName   = pFoil->m_FoilName;
	pNewPolar->m_bIsVisible = true;

	pNewPolar->m_PolarType = m_PolarType;

	switch (pNewPolar->m_PolarType)
	{
		case 1:
			pNewPolar->m_MaType = 1;
			pNewPolar->m_ReType = 1;
			break;
		case 2:
			pNewPolar->m_MaType = 2;
			pNewPolar->m_ReType = 2;
			break;
		case 3:
			pNewPolar->m_MaType = 1;
			pNewPolar->m_ReType = 3;
			break;
		case 4:
			pNewPolar->m_MaType = 1;
			pNewPolar->m_ReType = 1;
			break;
		default:
			pNewPolar->m_ReType = 1;
			pNewPolar->m_MaType = 1;
			break;
	}
	if(m_PolarType!=FIXEDAOAPOLAR)  pNewPolar->m_Reynolds = Re;
	else                            pNewPolar->m_ASpec    = 0.0;

	pNewPolar->m_Mach  = Mach;
	pNewPolar->m_ACrit = NCrit;
	pNewPolar->m_XTop  = m_XTop;
	pNewPolar->m_XBot  = m_XBot;


	SetPlrName(pNewPolar);
	Polar *pOldPolar = Polar::getPolar(pFoil, pNewPolar->m_PlrName);

	if(pOldPolar)
	{
		delete pNewPolar;
		pNewPolar = pOldPolar;
	}
	else Polar::addPolar(pNewPolar);
	return pNewPolar;
}


/**
 * Overrides the base class keyPressEvent method
 * @param event the keyPressEvent.
 */
void BatchThreadDlg::keyPressEvent(QKeyEvent *event)
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
				m_bCancel = true;
				XFoilTask::s_bCancel = true;
				XFoil::s_bCancel = true;
			}
			else
			{
				OnClose(); // will close the dialog box
			}
			break;
		}
		default:
			event->ignore();
	}
	event->accept();
}


/**
 * Initializes the dialog and the GUI interface
 */
void BatchThreadDlg::InitDialog()
{
	if(!Foil::curFoil()) return;
	blockSignals(true);

	m_pctrlTextOutput->clear();
	m_pctrlTextOutput->setFont(Settings::s_TableFont);

	m_ACrit     = QXDirect::s_refPolar.m_ACrit;
	m_XBot      = QXDirect::s_refPolar.m_XBot;
	m_XTop      = QXDirect::s_refPolar.m_XTop;
	m_Mach      = QXDirect::s_refPolar.m_Mach;

	m_PolarType = FIXEDSPEEDPOLAR; //no choice...

	m_pctrlFoil1->setChecked(s_bCurrentFoil);
	m_pctrlFoil2->setChecked(!s_bCurrentFoil);
	OnFoilSelectionType();
	
	m_pctrlReMin->SetPrecision(0);
	m_pctrlReMax->SetPrecision(0);
	m_pctrlReDelta->SetPrecision(0);

	m_pctrlSpecMin->SetPrecision(1);
	m_pctrlSpecMax->SetPrecision(1);
	m_pctrlSpecDelta->SetPrecision(1);

	if(m_ReMin<=0.0) m_ReMin = qAbs(m_ReInc);
	m_pctrlReMin->SetValue(m_ReMin);
	m_pctrlReMax->SetValue(m_ReMax);
	m_pctrlReDelta->SetValue(m_ReInc);
	m_pctrlSpecMin->SetValue(m_AlphaMin);
	m_pctrlSpecMax->SetValue(m_AlphaMax);
	m_pctrlSpecDelta->SetValue(m_AlphaInc);

	m_pctrlMach->SetValue(m_Mach);
	m_pctrlACrit->SetValue(m_ACrit);
	m_pctrlXTopTr->SetValue(m_XTop);
	m_pctrlXBotTr->SetValue(m_XBot);

	if(m_bAlpha) m_pctrlAlpha->setChecked(true);
	else         m_pctrlCl->setChecked(m_bAlpha);
	OnAcl();


	if(!m_bFromList)  m_rbRange1->setChecked(true);
	else              m_rbRange2->setChecked(true);

	m_pctrlEditList->setEnabled(m_bFromList);
	m_pctrlReMin->setEnabled(!m_bFromList);
	m_pctrlReMax->setEnabled(!m_bFromList);
	m_pctrlReDelta->setEnabled(!m_bFromList);
	m_pctrlMach->setEnabled(!m_bFromList);
	m_pctrlACrit->setEnabled(!m_bFromList);

	if(m_bFromZero)  m_pctrlFromZero->setChecked(true);
	else             m_pctrlFromZero->setChecked(false);

	m_pctrlInitBL->setChecked(true);
	m_pctrlUpdatePolarView->setChecked(s_bUpdatePolarView);

	blockSignals(false);
}


/**
 * The user has switched between aoa and lift coeficient.
 * Initializes the interface with the corresponding values.
 */
void BatchThreadDlg::OnAcl()
{
	if(m_PolarType==FIXEDAOAPOLAR) return;
	m_bAlpha = m_pctrlAlpha->isChecked();
	if(m_bAlpha)
	{
		m_pctrlSpecVar->setText(tr("Alpha"));
		m_pctrlSpecMin->SetValue(m_AlphaMin);
		m_pctrlSpecMax->SetValue(m_AlphaMax);
		m_pctrlSpecDelta->SetValue(m_AlphaInc);
		m_pctrlFromZero->setEnabled(true);
	}
	else
	{
		m_pctrlSpecVar->setText(tr("CL"));
		m_pctrlSpecMin->SetValue(m_ClMin);
		m_pctrlSpecMax->SetValue(m_ClMax);
		m_pctrlSpecDelta->SetValue(m_ClInc);
		m_bFromZero = false;
		m_pctrlFromZero->setChecked(false);
		m_pctrlFromZero->setEnabled(false);
	}
}

/**
 * The user has changed the range of Re values to analyze
 */
void BatchThreadDlg::OnSpecChanged()
{
	ReadParams();
}


/**
 * If an analysis is running, cancels the XFoilTask and returns.
 * It not, launches the analysis.
 */
void BatchThreadDlg::OnAnalyze()
{
	if(m_bIsRunning)
	{
		m_bCancel = true;
		XFoilTask::s_bCancel = true;
		XFoil::s_bCancel = true;
		return;
	}

	m_bCancel    = false;
	m_bIsRunning = true;

	m_pctrlClose->setEnabled(false);

	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	ReadParams();

	SetFileHeader();
	m_bInitBL = m_pctrlInitBL->isChecked();

	m_pctrlAnalyze->setFocus();
	StartAnalysis();
}


/**
 * The user has requested to quit the dialog box
 */
void BatchThreadDlg::OnClose()
{
	if(m_bIsRunning) return;

	s_bUpdatePolarView = m_pctrlUpdatePolarView->isChecked();
	m_bCancel = true;
	XFoilTask::s_bCancel = true;
	QThreadPool::globalInstance()->waitForDone();
	ReadParams();

	QXDirect::s_refPolar.m_ACrit    = m_ACrit;
	QXDirect::s_refPolar.m_XBot     = m_XBot;
	QXDirect::s_refPolar.m_XTop     = m_XTop;
	QXDirect::s_refPolar.m_Mach     = m_Mach;

	accept();
}



/**
 * The user has requested an edition of the Re list
*/
void BatchThreadDlg::OnEditReList()
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


/**
 * The user has requested an edition of the list of Foil objects to analyze
 */
void BatchThreadDlg::OnFoilList()
{
	FoilSelectionDlg dlg(this);
//	dlg.SetSelectionMode(true);
	dlg.m_poaFoil = &Foil::s_oaFoil;

	dlg.m_FoilList.clear();
	dlg.m_FoilList.append(m_FoilList);
	dlg.InitDialog();

	m_FoilList.clear();

	if(QDialog::Accepted == dlg.exec())
	{
		m_FoilList.append(dlg.m_FoilList);
	}
	outputFoilList();
}


/**
 *The user has changed between single Foil and list of Foil objects to analyze
 */
void BatchThreadDlg::OnFoilSelectionType()
{
	s_bCurrentFoil = m_pctrlFoil1->isChecked();
	m_pctrlFoilList->setEnabled(!s_bCurrentFoil);

	if(s_bCurrentFoil)
	{
		m_FoilList.clear();
		m_FoilList.append(Foil::curFoil()->foilName());
	}

	outputFoilList();
}

/**
 * The user has clicked the "start from zero" checkbox
 * @param state the new state of the checkbox
 */
void BatchThreadDlg::OnFromZero(int )
{
	if(m_pctrlFromZero->isChecked()) m_bFromZero = true;
	else                             m_bFromZero = false;
}


/**
 * The user has clicked the checkbox which specifies the initialization of the boundary layer
 **/
void BatchThreadDlg::OnInitBL(int)
{
	if (m_pctrlInitBL->isChecked()) m_bInitBL = true;
	else                            m_bInitBL = false;
}

/**
 * The user has clicked the checkbox specifying which range of Re should be analyzed
 */
void BatchThreadDlg::OnRange()
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
	m_pctrlACrit->setEnabled(!m_bFromList);
}


/**
 * Reads the value of the input parameters from the widgets and maps the data
 */
void BatchThreadDlg::ReadParams()
{
	m_bAlpha = m_pctrlAlpha->isChecked();

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_ReInc = m_pctrlReDelta->Value();
		m_ReMax = m_pctrlReMax->Value();
		m_ReMin = m_pctrlReMin->Value();

		if(m_bAlpha)
		{
			m_AlphaInc = qAbs(m_pctrlSpecDelta->Value());
			m_AlphaMax = m_pctrlSpecMax->Value();
			m_AlphaMin = m_pctrlSpecMin->Value();
		}
		else
		{
			m_ClInc = qAbs(m_pctrlSpecDelta->Value());
			m_ClMin = m_pctrlSpecMin->Value();
			m_ClMax = m_pctrlSpecMax->Value();
		}
	}

	if(m_ReMin<=0.0) m_ReMin = qAbs(m_ReInc);
	if(m_ReMax<=0.0) m_ReMax = qAbs(m_ReMax);

	m_Mach     = m_pctrlMach->Value();
	if(m_Mach<=0.0) m_Mach = 0.0;
	m_ACrit    = m_pctrlACrit->Value();
	m_XTop   = m_pctrlXTopTr->Value();
	m_XBot   = m_pctrlXBotTr->Value();
}


/**
 * Initializes the header of the log file
 */
void BatchThreadDlg::SetFileHeader()
{
	if(!m_pXFile) return;
	QTextStream out(m_pXFile);

	out << "\n";
	out << VERSIONNAME;
	out << "\n";
	if(s_bCurrentFoil)
	{
		out << Foil::curFoil()->m_FoilName;
		out << "\n";
	}


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";

}


/**
 * Creates the polar name for the input Polar
 * @param pNewPolar a pointer to the Polar object to name
 */
void BatchThreadDlg::SetPlrName(Polar *pNewPolar)
{
	if(!pNewPolar) return;
	pNewPolar->setAutoPolarName();
}


/**
 * Adds a text message to the ouput widget
 * @param str the message to output
 */
void BatchThreadDlg::UpdateOutput(QString &str)
{
	m_pctrlTextOutput->insertPlainText(str);
	m_pctrlTextOutput->ensureCursorVisible();
}



/**
 * Adds a text message to the log file
 * @param str the message to output
 */
void BatchThreadDlg::WriteString(QString &strong)
{
	if(!m_pXFile || !m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}


/**
 * Starts the multithreaded analysis.
 * First, creates a pool list of all (Foil, pairs) to analyze.
 * Then, starts the threads which will pick the pairs from the pool and remove them once the analayis is finished.
 */
void BatchThreadDlg::StartAnalysis()
{
	Foil *pFoil;
	Polar *pPolar;
	QString strong;
	int iRe, nRe;

	if(s_bCurrentFoil)
	{
		m_FoilList.clear();
		m_FoilList.append(Foil::curFoil()->foilName());
	}

	if(!m_FoilList.count())
	{
		strong ="No foil defined for analysis\n\n";
		UpdateOutput(strong);
		CleanUp();
		return;
	}

	m_pctrlAnalyze->setText(tr("Cancel"));
	strong ="Launching multi-threaded batch analysis\n\n";
	UpdateOutput(strong);

	if(!m_bFromList) nRe = (int)qAbs((m_ReMax-m_ReMin)/m_ReInc)+1;
	else             nRe = QXDirect::s_ReList.count();

//	QThreadPool::globalInstance()->setExpiryTimeout(60000);//ms

	//build an array of all analysis pairs to run
	m_nAnalysis = 0;
	Analysis *pAnalysis=NULL;
	for(int i=0; i<m_FoilList.count(); i++)
	{
		pFoil = Foil::foil(m_FoilList.at(i));
		if(pFoil)
		{
			for (iRe=0; iRe<nRe; iRe++)
			{
				pAnalysis = new Analysis;
				m_AnalysisPair.append(pAnalysis);
				pAnalysis->pFoil = pFoil;

				if(!m_bFromList) pPolar = CreatePolar(pFoil, m_ReMin + iRe *m_ReInc, m_Mach, m_ACrit);
				else             pPolar = CreatePolar(pFoil, QXDirect::s_ReList[iRe], QXDirect::s_MachList[iRe], QXDirect::s_NCritList[iRe]);
				pAnalysis->pPolar=pPolar;

				m_nAnalysis++;
			}
		}
	}
	strong = QString("Found %1 foil/polar pairs to analyze\n").arg(m_nAnalysis);
	UpdateOutput(strong);

	//Start as many threads as the system will support
	m_nThreads = min(QThread::idealThreadCount(), nRe);
	m_pXFoilTask = new XFoilTask[m_nThreads];
	for(int ixfl=0; ixfl<m_nThreads; ixfl++) m_pXFoilTask[ixfl].m_pParent = this;


	XFoilTask::s_bCancel = false;

	strong = QString("Using %1 threads\n\n").arg(m_nThreads);
	UpdateOutput(strong);

	for(int it=0; it<m_nThreads; it++)	StartThread();
}


/**
 * A timer event used out of purpose as a convenient way to receive a notification
 * of the analysis pair which has ended its analysis and to display the output message.
*/
void BatchThreadDlg::timerEvent(QTimerEvent *event)
{
	qApp->processEvents();

	if(m_pXFoilTask)
	{
		QString str = "   ...Finished "+m_pXFoilTask[event->timerId()].m_pFoil->m_FoilName+" / "
								 +m_pXFoilTask[event->timerId()].m_pPolar->m_PlrName+"\n";

		UpdateOutput(str);
		m_pXFoilTask[event->timerId()].m_bIsFinished = true;
	}

	//time to launch another analysis, if any is left
	QString strong;
	QXDirect *pXDirect = (QXDirect*)s_pXDirect;
	if(m_pctrlUpdatePolarView->isChecked())
	{
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}

	if(m_AnalysisPair.size()<=0 || m_bCancel)
	{
		//nothing left to launch... just wait and enjoy the show
		bool bAllFinished = true;
		for (int it=0; it<m_nThreads; it++)
		{
			if(m_pXFoilTask && !m_pXFoilTask[it].m_bIsFinished)
			{
				bAllFinished = false;
				break;
			}
		}

		if(bAllFinished)
		{
			QThreadPool::globalInstance()->waitForDone();

			if(m_pXFoilTask)
			{
				delete [] m_pXFoilTask;
				m_pXFoilTask = NULL;

				if(m_bCancel) strong = "\n_____Analysis cancelled_____\n";
				else          strong = "\n_____Analysis completed_____\n";
				UpdateOutput(strong);

				CleanUp();
				if(pXDirect->m_bPolarView)
				{
					pXDirect->CreatePolarCurves();
					pXDirect->UpdateView();
				}
			}
		}
	}
	else if(m_bIsRunning)
	{
		//need to check if we are still running in case a timer event arrives after a cancellation for instance.
		StartThread(); // analyze a new pair
	}
}


/**
 * Starts an individual thread
 */
void BatchThreadDlg::StartThread()
{
	Analysis *pAnalysis;
	QString strong;
	//  browse through the array until we find an available thread
	while(QThreadPool::globalInstance()->activeThreadCount()<m_nThreads && m_AnalysisPair.count())
	{
		//re-use a runnable object, don't recreate one, to save memory allocation time
		for (int it=0; it<m_nThreads; it++)
		{
			if(m_pXFoilTask && m_pXFoilTask[it].m_bIsFinished)
			{
				m_pXFoilTask[it].m_bIsFinished = false;
				m_pXFoilTask[it].m_Id = it;

				//take the last analysis in the array
				pAnalysis = (Analysis*)m_AnalysisPair.at(m_AnalysisPair.size()-1);

				//initiate the task
				if(m_bAlpha) m_pXFoilTask[it].setSequence(true,  m_AlphaMin, m_AlphaMax, m_AlphaInc);
				else         m_pXFoilTask[it].setSequence(false, m_ClMin, m_ClMax, m_ClInc);


				m_pXFoilTask[it].InitializeTask(pAnalysis->pFoil, pAnalysis->pPolar, true, m_bInitBL, m_bFromZero);

				//launch it
				strong = "Starting "+pAnalysis->pFoil->m_FoilName+" / "+pAnalysis->pPolar->m_PlrName+"\n";
				UpdateOutput(strong);
				QThreadPool::globalInstance()->start(m_pXFoilTask+it);

				//remove it from the array of pairs to analyze
				pAnalysis = m_AnalysisPair.last();
				m_AnalysisPair.removeLast();
				delete pAnalysis;
				break;
			}
		}
	}
}

/**
 * Outputs the list of the Foil names selected for analysis to the output text window.
 */
void BatchThreadDlg::outputFoilList()
{
	m_pctrlTextOutput->append("Foils to analyze:");
	for(int i=0; i<m_FoilList.count();i++)
	{
		m_pctrlTextOutput->append("   "+m_FoilList.at(i));
	}
	m_pctrlTextOutput->append("\n");
}


/**
 * Outputs the list of the Re values selected for analysis to the output text window.
 */
void BatchThreadDlg::outputReList()
{
	m_pctrlTextOutput->append("Reynolds numbers to analyze:");
	if(m_bFromList)
	{
		for(int i=0; i<QXDirect::s_ReList.count(); i++)
		{
			QString strong = QString("   Re = %L1  /  Mach = %L2  /  NCrit = %L3")
								   .arg(QXDirect::s_ReList.at(i), 10,'f',0)
								   .arg(QXDirect::s_MachList.at(i), 5,'f',3)
								   .arg(QXDirect::s_NCritList.at(i), 5, 'f', 2);
			m_pctrlTextOutput->append(strong);
		}
	}
	else
	{
		for(double Re=m_ReMin; Re<m_ReMax; Re+=m_ReInc)
		{
			QString strong = QString("   Re = %L1  /  Mach = %L2  /  NCrit = %L3")
								   .arg(Re, 10,'f',0)
								   .arg(m_Mach, 5,'f',3)
								   .arg(m_ACrit, 5, 'f', 2);
			m_pctrlTextOutput->append(strong);
		}
	}
	m_pctrlTextOutput->append("");
}

/**
 * Overrides the base class showEvent method. Moves the window to its former location.
 * @param event the showEvent.
 */
void BatchThreadDlg::showEvent(QShowEvent *event)
{
    move(s_Position);
	event->accept();
}

/**
 * Overrides the base class hideEvent method. Stores the window's current position.
 * @param event the hideEvent.
 */
void BatchThreadDlg::hideEvent(QHideEvent *event)
{
    s_Position = pos();
	event->accept();
}


void BatchThreadDlg::OnAdvancedSettings()
{
	XFoilAdvancedDlg xfaDlg(this);
	xfaDlg.m_IterLimit   = XFoilTask::s_IterLim;
	xfaDlg.m_bAutoInitBL     = XFoilTask::s_bAutoInitBL;
	xfaDlg.m_VAccel      = XFoil::vaccel;
	xfaDlg.m_bFullReport = XFoil::s_bFullReport;
	xfaDlg.InitDialog();

	if (QDialog::Accepted == xfaDlg.exec())
	{
		XFoil::vaccel             = xfaDlg.m_VAccel;
		XFoil::s_bFullReport      = xfaDlg.m_bFullReport;
		XFoilTask::s_bAutoInitBL  = xfaDlg.m_bAutoInitBL;
		XFoilTask::s_IterLim      = xfaDlg.m_IterLimit;
	}
}



