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
#include "XDirect.h"
#include "ReListDlg.h"
#include "../mainframe.h"
#include "../xinverse/FoilSelectionDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDir>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>



bool BatchDlg::s_bStoreOpp = false;
bool BatchDlg::s_bCurrentFoil=true;
void * BatchDlg::s_pXFoil;
void * BatchDlg::s_pMainFrame;
void * BatchDlg::s_pXDirect;



BatchDlg::BatchDlg(QWidget *pParent) : QDialog(pParent)
{
	QString str = tr("Batch foil analysis");
	setWindowTitle(str);
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

	m_NCrit  = 9.0;
	m_XTopTr = 1.0;
	m_XBotTr = 1.0;
	m_ReList    = NULL;
	m_MachList  = NULL;
	m_NCritList = NULL;


	m_bOutput         = true;
	m_bAlpha          = true;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bInitBL         = false;
	m_bCancel         = false;
	m_bSkipPoint      = false;
	m_bSkipPolar      = false;
	m_bIsRunning      = false;

	m_Iterations = 0;
	m_IterLim = 100;
	m_NRe = 0;


	m_IterRect.setRect(327,158,620-327,398-158);

	m_RmsGraph.SetXTitle(tr("Iter"));
	m_RmsGraph.SetYTitle("");//Change from bl newton system solution

	m_RmsGraph.SetAuto(true);

	m_RmsGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_RmsGraph.SetYMajGrid(true, QColor(120,120,120),2,1);

	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax(50);
	m_RmsGraph.SetYMin(-1.0);
	m_RmsGraph.SetYMax( 1.0);
	m_RmsGraph.SetType(1);
	m_RmsGraph.SetMargin(40);

	XFoil::s_bCancel = false;

	SetupLayout();
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
	connect(m_pctrlFromZero, SIGNAL(stateChanged(int)), this, SLOT(OnFromZero(int)));
	connect(m_pctrlSpecMin, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecMax, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecDelta, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
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
			m_pctrlReMin   = new FloatEdit(100000,0);
			m_pctrlReMax   = new FloatEdit(150000,0);
			m_pctrlReDelta = new FloatEdit(50000,0);
			m_pctrlMach    = new FloatEdit(0.0, 3);

			QLabel *NCritLabel = new QLabel(tr("NCrit="));
			NCritLabel->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
			m_pctrlNCrit   = new FloatEdit(9.00);

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
			m_pctrlSpecMin    = new FloatEdit(0.00);
			m_pctrlSpecMax    = new FloatEdit(1.00);
			m_pctrlSpecDelta  = new FloatEdit(0.50);
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
			m_pctrlXTopTr = new FloatEdit(1.00);
			m_pctrlXBotTr = new FloatEdit(1.00);
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

	//_*_*_*_*_*_*_**_*_*_**_*_*_*_

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
		m_pctrlGraphOutput->m_pGraph = &m_RmsGraph;
	//	m_pctrlGraphOutput->setMinimumHeight(300);
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


void BatchDlg::AddOpPoint()
{
	QXDirect *pXDirect = (QXDirect*)s_pXDirect;
	XFoil *pXFoil = (XFoil*)s_pXFoil;

	if(s_bStoreOpp)	pXDirect->AddOpPoint(m_pCurPolar, true);
	else 
	{
		m_pCurPolar->AddData(pXFoil);
	}
	if(pXDirect->m_bPolarView)
	{
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	m_RmsGraph.ResetYLimits();
}


void BatchDlg::AlphaLoop()
{
	XFoil *pXFoil = (XFoil*)s_pXFoil;
//	QXDirect* pXDirect = (QXDirect*)s_pXDirect;
	QString str, str2;
	QString strong = "";
	int iRe, iAlpha, nAlpha;
	double alphadeg;
	QPoint Place(m_pctrlGraphOutput->rect().left()+m_RmsGraph.GetMargin()*2, m_pctrlGraphOutput->rect().top()+m_RmsGraph.GetMargin()/2);

	nAlpha = (int)(fabs((m_SpMax-m_SpMin)*1.000/m_SpInc));//*1.0001 to make sure upper limit is included

	for (iAlpha=0; iAlpha<=nAlpha; iAlpha++)
	{
		qApp->processEvents();
		if(m_bCancel) break;
		
		alphadeg = m_SpMin + iAlpha*m_SpInc;
		pXFoil->alfa = alphadeg*PI/180.0;
		str = QString("Alpha = %1\n").arg(alphadeg,0,'f',2);
		strong+= str;
		UpdateOutput(str);

		int total = (int)(fabs((m_ReMax-m_ReMin)*1.0001/m_ReInc));
		m_pCurPolar = CreatePolar(alphadeg, pXFoil->minf1, pXFoil->acrit);// Do something
		if(!m_pCurPolar) return;

		pXFoil->InitXFoilAnalysis(m_pCurPolar);

		if (m_bInitBL)
		{
			pXFoil->lblini = false;
			pXFoil->lipan = false;
		}

		m_bSkipPolar = false;

		for (iRe=0; iRe<=total; iRe++)
		{
			ResetCurves();
			if(!m_bCancel && !m_bSkipPolar)
			{
				pXFoil->reinf1 = m_ReMin + iRe *m_ReInc;
				pXFoil->lalfa  = true;
				pXFoil->qinf   = 1.0;
				str = QString("Re=%1   Ma=%2   Nc=%3\n").arg(pXFoil->reinf1,8,'f',0).arg(pXFoil->minf1,5,'f',3).arg(pXFoil->acrit,5,'f',2);
				strong+=str;
				UpdateOutput(str);

				str = QString("Alpha=%1").arg(alphadeg,5,'f',2);
				str += QString::fromUtf8("°");
				str2 = QString(" / Re=%1").arg(pXFoil->reinf1,8,'f',0);
				str += str2;
				m_pctrlGraphOutput->SetTitle(str, Place);

				// here we go !
				if (!pXFoil->specal())
				{
					str = tr("Invalid Analysis Settings\nCpCalc: local speed too large\n Compressibility corrections invalid ");
//					QMessageBox::information(this, tr("Warning"), str);
					UpdateOutput(str);

					m_bCancel = true;
					CleanUp();
					return;
				}

				if (fabs(pXFoil->alfa-pXFoil->awake) > 0.00001) pXFoil->lwake  = false;
				if (fabs(pXFoil->alfa-pXFoil->avisc) > 0.00001) pXFoil->lvconv = false;
				if (fabs(pXFoil->minf-pXFoil->mvisc) > 0.00001) pXFoil->lvconv = false;

				pXFoil->lwake  = false;
				pXFoil->lvconv = false;

				m_Iterations = 0;

				m_bSkipPoint = false;

				while(!Iterate()){}


				if(pXFoil->lvconv)
				{
					AddOpPoint();
				}
				else if(m_bSkipPoint || m_bSkipPolar)
				{
					str = QString(tr("   ...skipped after %1 iterations")+"\n").arg(m_Iterations);
					strong+= str;
					UpdateOutput(str);

				}
				else
				{
					str = QString(tr("   ...unconverged after %1 iterations")+"\n").arg(m_Iterations);
					strong+= str;
					UpdateOutput(str);
				}
			}
			else
			{
				break;
			}
		}// end Alpha or Cl loop
		strong+="\n";
		if(m_bCancel)
		{
			strong+=tr("Analysis interrupted")+"\n";
			break;
		}
	}//end Re loop
//	WriteString(strong);
}



void BatchDlg::Analysis2()
{

	if(m_bAlpha)
	{
		m_AlphaMin = m_SpMin;
		m_AlphaMax = m_SpMax;

		if(m_AlphaMin< m_AlphaMax) m_AlphaInc =  fabs(m_SpInc);
		else                       m_AlphaInc = -fabs(m_SpInc);
	}
	else
	{
		m_ClMin = m_SpMin ;
		m_ClMax = m_SpMax ;
		if(m_ClMin< m_ClMax) m_ClInc =  fabs(m_ClInc);
		else                 m_ClInc = -fabs(m_ClInc);
	}

	if(m_ReMin < m_ReMax)  m_ReInc =  fabs(m_ReInc);
	else                   m_ReInc = -fabs(m_ReInc);
}


void BatchDlg::Analysis3()
{
//	m_bType4 = true;
//	m_bAlpha = false;

	m_AlphaMin = m_SpMin ;
	m_AlphaMax = m_SpMax ;
	if(m_SpMin< m_SpMax) m_AlphaInc =  fabs(m_SpInc);
	else                 m_AlphaInc = -fabs(m_SpInc);


	if(m_ReMin < m_ReMax) m_ReInc =  fabs(m_ReInc);
	else                  m_ReInc = -fabs(m_ReInc);
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
	m_bSkipPoint = false;
	m_bSkipPolar = false;
	XFoil::s_bCancel = false;
	m_pctrlClose->setFocus();
}


Polar *BatchDlg::CreatePolar(double Spec, double Mach, double NCrit)
{
	if(!m_pFoil) return NULL;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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
	pPolar->m_XTop  = m_XTopTr;
	pPolar->m_XBot  = m_XBotTr;

	pPolar->m_Color = pMainFrame->GetColor(1);

	SetPlrName(pPolar);
	Polar *pOldPolar = pMainFrame->GetPolar(m_pFoil->m_FoilName, pPolar->m_PlrName);

	if(pOldPolar)
	{
		delete pPolar;
		pPolar = pOldPolar;
	}
	else pPolar = pMainFrame->AddPolar(pPolar);
	return pPolar;
}


void BatchDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
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
				m_bSkipPoint = true;
				m_bSkipPolar = true;
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
//	CRect WndRect;
//	GetWindowRect(WndRect);
//	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-WndRect.Width()-10,60,0,0,SWP_NOSIZE);

	if(!m_pFoil) return;

//	QXDirect* pXDirect = (QXDirect*)s_pXDirect;

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

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_pctrlReMin->SetPrecision(0);
		m_pctrlReMax->SetPrecision(0);
		m_pctrlReDelta->SetPrecision(0);

		m_pctrlSpecMin->SetPrecision(1);
		m_pctrlSpecMax->SetPrecision(1);
		m_pctrlSpecDelta->SetPrecision(1);
	}
	else
	{
		m_pctrlReMin->SetPrecision(1);
		m_pctrlReMax->SetPrecision(1);
		m_pctrlReDelta->SetPrecision(1);

		m_pctrlSpecMin->SetPrecision(0);
		m_pctrlSpecMax->SetPrecision(0);
		m_pctrlSpecDelta->SetPrecision(0);
	}

	if(m_ReMin<=0.0) m_ReMin = fabs(m_ReInc);
	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_pctrlReMin->SetValue(m_ReMin);
		m_pctrlReMax->SetValue(m_ReMax);
		m_pctrlReDelta->SetValue(m_ReInc);
		m_pctrlSpecMin->SetValue(m_SpMin);
		m_pctrlSpecMax->SetValue(m_SpMax);
		m_pctrlSpecDelta->SetValue(m_SpInc);
	}
	else
	{
		m_pctrlReMin->SetValue(m_SpMin);
		m_pctrlReMax->SetValue(m_SpMax);
		m_pctrlReDelta->SetValue(m_SpInc);
		m_pctrlSpecMin->SetValue(m_ReMin);
		m_pctrlSpecMax->SetValue(m_ReMax);
		m_pctrlSpecDelta->SetValue(m_ReInc);
	}
	m_pctrlMach->SetValue(m_Mach);
	m_pctrlNCrit->SetValue(m_NCrit);
	m_pctrlXTopTr->SetValue(m_XTopTr);
	m_pctrlXBotTr->SetValue(m_XBotTr);

	if(m_bAlpha) m_rbspec1->setChecked(true);
	else         m_rbspec2->setChecked(true);
	OnAcl();

	if(m_PolarType==FIXEDSPEEDPOLAR)       m_rbtype1->setChecked(true);
	else if(m_PolarType==FIXEDLIFTPOLAR)   m_rbtype2->setChecked(true);
	else if(m_PolarType==RUBBERCHORDPOLAR) m_rbtype3->setChecked(true);
	else if(m_PolarType==FIXEDAOAPOLAR)    m_rbtype4->setChecked(true);
	OnPolarType();


	if(!m_bFromList)  m_rbRange1->setChecked(true);
	else              m_rbRange2->setChecked(true);
	OnRange();

	if(m_bFromZero)  m_pctrlFromZero->setChecked(true);
	else             m_pctrlFromZero->setChecked(false);

	m_pctrlInitBL->setChecked(true);
	m_pctrlStoreOpp->setChecked(s_bStoreOpp);

	m_pctrlSkipOpp->setEnabled(false);
	m_pctrlSkipPolar->setEnabled(false);

	m_Iterations = 0;
	ResetCurves();

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pMainFrame) m_RmsGraph.CopySettings(&pMainFrame->m_RefGraph, false);
}



bool BatchDlg::Iterate()
{
	QString str;
	XFoil *pXFoil = (XFoil*)s_pXFoil;
	QXDirect* pXDirect = (QXDirect*)s_pXDirect;

	if(!pXFoil->viscal())
	{
		pXFoil->lvconv = false;//point is unconverged
		str =tr("CpCalc: local speed too large\n Compressibility corrections invalid");
//		QMessageBox::information(this, tr("Warning"), str);
		UpdateOutput(str);
		WriteString(str);
		m_bCancel = true;
		CleanUp();
		return true;
	}


	while(m_Iterations<m_IterLim && !pXFoil->lvconv && !m_bCancel && !m_bSkipPoint && !m_bSkipPolar)
	{
		qApp->processEvents();
		if(pXFoil->ViscousIter())
		{
			m_Iterations++;
			OutputIter(m_Iterations);
		}
		else
		{
			m_Iterations = m_IterLim;
		}
	}

	OutputIter(0);
	if(m_bCancel) return true;

	if(m_bSkipPoint || m_bSkipPolar)
	{
		pXFoil->lblini = false;
		pXFoil->lipan = false;
		return true;
	}

	if(!pXFoil->ViscalEnd())
	{
		pXFoil->lvconv = false;//point is unconverged
		str =tr("CpCalc: local speed too large\n Compressibility corrections invalid");
//		QMessageBox::information(this, tr("Warning"), str);
		m_bCancel = true;
		UpdateOutput(str);
		WriteString(str);
		CleanUp();
		pXFoil->lblini = false;
		pXFoil->lipan  = false;
		return true;// to exit loop
	}
//	m_bCalc = true;

	if(m_Iterations>=m_IterLim && !pXFoil->lvconv)
	{
		if(pXDirect->m_bAutoInitBL)
		{
			pXFoil->lblini = false;
			pXFoil->lipan = false;
		}
		pXFoil->fcpmin();// Is it of any use ?
		return true;
	}
	if(!pXFoil->lvconv)
	{
		pXFoil->fcpmin();// Is it of any use ?
		return false;
	}
	else
	{
		//converged at last
		pXFoil->fcpmin();// Is it of any use ?
		return true;
	}
	return false;
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


void BatchDlg::OnSpecChanged()
{
	ReadParams();
	if(m_bAlpha)
	{
		m_AlphaMin = m_SpMin;
		m_AlphaMax = m_SpMax;
		m_AlphaInc = m_SpInc;
	}
	else
	{
		m_ClMin = m_SpMin;
		m_ClMax = m_SpMax;
		m_ClInc = m_SpInc;
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

		m_pctrlReMin->SetValue(m_SpMin);
		m_pctrlReMax->SetValue(m_SpMax);
		m_pctrlReDelta->SetValue(m_SpInc);

		m_pctrlSpecMin->SetValue(m_ReMin);
		m_pctrlSpecMax->SetValue(m_ReMax);
		m_pctrlSpecDelta->SetValue(m_ReInc);
	}
}



void BatchDlg::OutputIter(int iter)
{
	XFoil *pXFoil = (XFoil*)s_pXFoil;
	if(iter)
	{
		m_RmsGraph.GetCurve(0)->AppendPoint(iter, pXFoil->rmsbl);
		m_RmsGraph.GetCurve(1)->AppendPoint(iter, pXFoil->rmxbl);
		UpdateGraph();
	}
}


void BatchDlg::OnAnalyze()
{
	if(m_bIsRunning)
	{
		XFoil::s_bCancel = true;
		m_bCancel = true;
		return;
	}
	m_bCancel    = false;
	m_bIsRunning = true;

	XFoil *pXFoil = (XFoil*)s_pXFoil;

	m_pctrlTextOutput->setText("");

	m_pctrlClose->setEnabled(false);

	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	pXFoil->pXFile = m_pXFile;


	ReadParams();
	SetFileHeader();
	m_bInitBL = m_pctrlInitBL->isChecked();
	s_bStoreOpp = m_pctrlStoreOpp->isChecked();

	if(m_PolarType!=FIXEDAOAPOLAR) Analysis2();
	else                           Analysis3();

	pXFoil->lvisc = true;
	pXFoil->m_bTrace = false;

	m_pctrlAnalyze->setText(tr("Cancel"));
	m_pctrlSkipOpp->setEnabled(true);
	m_pctrlSkipPolar->setEnabled(true);

	m_pctrlAnalyze->setFocus();
	StartAnalysis();
}



void BatchDlg::OnClose()
{
	m_bCancel = true;
	ReadParams();
	done(1);
}



void BatchDlg::OnEditReList()
{
    ReListDlg dlg(this);

	for (int i=0; i<m_NRe; i++)
	{
		dlg.m_ReList[i]    = m_ReList[i];
		dlg.m_MachList[i]  = m_MachList[i];
		dlg.m_NCritList[i] = m_NCritList[i];
	}
	dlg.m_NRe = m_NRe;
	dlg.InitDialog();

	if(QDialog::Accepted == dlg.exec())
	{
		for (int i=0; i<dlg.m_NRe; i++)
		{
			m_ReList[i]    = dlg.m_ReList[i];
			m_MachList[i]  = dlg.m_MachList[i];
			m_NCritList[i] = dlg.m_NCritList[i];
		}
		m_NRe = dlg.m_NRe;
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


void BatchDlg::OnFromZero(int state)
{
	state = 0;
	if(m_pctrlFromZero->isChecked()) m_bFromZero = true;
	else                             m_bFromZero = false;
}

void BatchDlg::OnInitBL(int state)
{
	state = 0;
	if (m_pctrlInitBL->isChecked()) m_bInitBL = true;
	else                            m_bInitBL = false;
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
	m_bSkipPoint = true;
}


void BatchDlg::OnSkipPolar()
{
	m_bSkipPolar = true;
}



void BatchDlg::ReadParams()
{
	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		m_ReInc = m_pctrlReDelta->Value();
		m_ReMax = m_pctrlReMax->Value();
		m_ReMin = m_pctrlReMin->Value();

		m_SpInc = m_pctrlSpecDelta->Value();
		m_SpMax = m_pctrlSpecMax->Value();
		m_SpMin = m_pctrlSpecMin->Value();
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

	if(m_ReMin<=0.0) m_ReMin = fabs(m_ReInc);
	if(m_ReMax<=0.0) m_ReMax = fabs(m_ReMax);
	m_SpInc = fabs(m_SpInc);

	m_Mach     = m_pctrlMach->Value();
	if(m_Mach<=0.0) m_Mach = 0.0;
	m_NCrit    = m_pctrlNCrit->Value();
	m_XTopTr   = m_pctrlXTopTr->Value();
	m_XBotTr   = m_pctrlXBotTr->Value();
	
	s_bStoreOpp = m_pctrlStoreOpp->isChecked();
}


void BatchDlg::ReLoop()
{
	XFoil *pXFoil = (XFoil*)s_pXFoil;
	QString str;
	QString strong = "";

	double alphadeg;
	int ia, iRe, nRe, series, total, MaxSeries;
	double SpMin, SpMax, SpInc;

	QPoint Place(m_pctrlGraphOutput->rect().left()+20, m_pctrlGraphOutput->rect().top()+m_RmsGraph.GetMargin()/2);

	if(!m_bFromList) nRe = (int)fabs((m_ReMax-m_ReMin)/m_ReInc);
	else             nRe = m_NRe-1;

	for (iRe=0; iRe<=nRe; iRe++)
	{
		if(!m_bFromList)
		{
			pXFoil->reinf1 = m_ReMin + iRe *m_ReInc;
			pXFoil->minf1  = m_Mach;
			pXFoil->acrit  = m_NCrit;
		}
		else
		{
			pXFoil->reinf1 = m_ReList[iRe];
			pXFoil->minf1  = m_MachList[iRe];
			pXFoil->acrit  = m_NCritList[iRe];
		}
		str = QString("Re=%1   Ma=%2   Nc=%3\n").arg(pXFoil->reinf1,8,'f',0).arg(pXFoil->minf1,5,'f',3).arg(pXFoil->acrit,5,'f',2);
		strong+= str;
		UpdateOutput(str);

		if (m_bFromZero && m_SpMin*m_SpMax<0)
		{
			MaxSeries = 2;
			SpMin = 0.0;
			SpMax = m_SpMax;
		}
		else
		{
			MaxSeries = 1;
			SpMin = m_SpMin;
			SpMax = m_SpMax;
		}
		SpInc = fabs(m_SpInc);

		if(SpMin > SpMax) SpInc = -fabs(SpInc);

		m_bSkipPolar = false;

		for (series=0; series<MaxSeries;series++)
		{
			qApp->processEvents();
			if(m_bCancel) break;

			total = (int)fabs((SpMax*1.0001-SpMin)/SpInc);//*1.0001 to make sure upper limit is included

			m_pCurPolar = CreatePolar(pXFoil->reinf1, pXFoil->minf1, pXFoil->acrit);
			if(!m_pCurPolar) return;

			pXFoil->InitXFoilAnalysis(m_pCurPolar);

			if (m_bInitBL)
			{
				pXFoil->lblini = false;
				pXFoil->lipan = false;
			}

			for (ia=0; ia<=total; ia++)
			{
				ResetCurves();
				if(!m_bCancel && !m_bSkipPolar)
				{
					if(m_bAlpha)
					{
						alphadeg = SpMin+ia*SpInc;
						pXFoil->alfa = alphadeg * PI/180.0;
						pXFoil->lalfa = true;
						pXFoil->qinf = 1.0;
						str = QString("Alpha = %1").arg(alphadeg,9,'f',3);
						strong+=str;
						UpdateOutput(str);

						str = m_pFoil->m_FoilName+" / ";
						str += QString("Re=%1  /  Alpha=%2").arg(pXFoil->reinf1,8,'f',0).arg(alphadeg,5,'f',2);
						str += QString::fromUtf8("°");
						m_pctrlGraphOutput->SetTitle(str, Place);


						// here we go !
						if (!pXFoil->specal())
						{
							str = tr("Invalid Analysis Settings\nCpCalc: local speed too large\n Compressibility corrections invalid ");
							UpdateOutput(str);
							WriteString(str);
							m_bCancel = true;
							CleanUp();
							return;
						}
					}
					else
					{
						pXFoil->lalfa = false;
						pXFoil->alfa = 0.0;
						pXFoil->qinf = 1.0;
						pXFoil->clspec = SpMin+ia*SpInc;
						str = QString(tr("Cl = %1")).arg(pXFoil->clspec,9,'f',3);
						strong+=str;
						UpdateOutput(str);
						if(!pXFoil->speccl())
						{
							str = tr("Invalid Analysis Settings\nCpCalc: local speed too large\n Compressibility corrections invalid ");
							UpdateOutput(str);
							WriteString(str);
							m_bCancel = true;
							CleanUp();
							return;
						}
					}

					pXFoil->lwake = false;
					pXFoil->lvconv = false;

					m_Iterations = 0;

					m_bSkipPoint = false;

					while(!Iterate()){}

					if(pXFoil->lvconv)
					{
						str = QString(tr("   ...converged after %1 iterations\n")).arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);
						AddOpPoint();
//						m_pCurPolar->AddData(pXFoil);
					}
					else if(m_bSkipPoint || m_bSkipPolar)
					{
						str = QString(tr("   ...skipped after %1 iterations\n")).arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);

					}
					else
					{
						str = QString(tr("   ...unconverged after %1 iterations\n")).arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);
					}
				}
				else
				{
					break;
				}
			}// end Alpha or Cl loop
			SpMin = 0;
			SpMax = m_SpMin;
			SpInc = -SpInc;
		}
		strong+="\n";
		if(m_bCancel) break;

	}//end Re loop

	WriteString(strong);
}


void BatchDlg::ResetCurves()
{
	m_RmsGraph.DeleteCurves();
	m_RmsGraph.AddCurve();
	m_RmsGraph.AddCurve();
	QString str = "rms";
	m_RmsGraph.GetCurve(0)->SetTitle(str);
	str = "max";
	m_RmsGraph.GetCurve(1)->SetTitle(str);
	m_RmsGraph.GetCurve(1)->SetStyle(0);
	m_RmsGraph.SetAutoX(false);
	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax((double)m_IterLim);
	m_RmsGraph.SetXUnit((int)(m_IterLim/10.0));
	m_RmsGraph.SetYMin(-1.0);
	m_RmsGraph.SetYMax( 1.0);
	m_RmsGraph.SetX0(0.0);
	m_RmsGraph.SetY0(0.0);
}


void BatchDlg::SetFileHeader()
{
	QXDirect *pXDirect = (QXDirect*)s_pXDirect;

	QTextStream out(m_pXFile);

	out << "\n";
	out << MainFrame::versionName();
	out << "\n";
	out << m_pFoil->m_FoilName;
	out << "\n";
	if(pXDirect && pXDirect->m_pCurPolar)
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



void BatchDlg::StartAnalysis()
{
	QXDirect *pXDirect = (QXDirect*)s_pXDirect;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	Foil *pFoil;
	QString strong;

	m_pctrlAnalyze->setText(tr("Cancel"));
	if(s_bCurrentFoil)
	{
		if(m_PolarType!=FIXEDAOAPOLAR) ReLoop();
		else                           AlphaLoop();
	}
	else
	{
		XFoil *pXFoil = (XFoil*)s_pXFoil;
		for(int i=0; i<m_FoilList.count();i++)
		{
			pFoil = MainFrame::foil(m_FoilList.at(i));
			m_pFoil = pFoil;
			pXFoil->InitXFoilGeometry(pFoil);
			pXFoil->lvisc=true;

			strong = tr("Analyzing ")+pFoil->m_FoilName+("\n");
			UpdateOutput(strong);
			if(m_PolarType!=FIXEDAOAPOLAR) ReLoop();
			else                           AlphaLoop();
			strong = "\n\n";
			UpdateOutput(strong);
		}
	}

	if(pXDirect->m_bPolarView)
	{
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}

	strong = tr("Analysis completed");
	UpdateOutput(strong);

	CleanUp();
}


void BatchDlg::UpdateGraph()
{
	m_pctrlGraphOutput->update();
}



void BatchDlg::UpdateOutput(QString &str)
{
	m_pctrlTextOutput->insertPlainText(str);
	m_pctrlTextOutput->ensureCursorVisible();
}




void BatchDlg::WriteString(QString &strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}
















