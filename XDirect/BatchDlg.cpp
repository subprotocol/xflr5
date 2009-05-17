/****************************************************************************

	BatchDlg Class
        Copyright (C) 2003-2009 Andre Deperrois xflr5@yahoo.com

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
#include "../MainFrame.h"


BatchDlg::BatchDlg(void *pParent)
{
	m_ReInc =  50000.0;
	m_ReMax = 300000.0;
	m_ReMin = 100000.0;
	m_Mach  = 0.0;
	m_SpInc = 0.5;
	m_SpMax = 1.0;
	m_SpMin = 0.0;

	m_IterRect.setRect(327,158,620-327,398-158);

	m_RmsGraph.SetXTitle("Iter");
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

	m_pXDirect        = pParent;
	m_bAlpha          = true;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bCancel         = false;
	m_bInitBL         = false;
	m_bSkipPoint      = false;
	m_bSkipPolar      = false;
	m_bIsRunning      = false;

	m_pXFoil       = NULL;
	m_Iterations = 0;
	pi = 3.141592654;

	SetLayout();
	connect(m_pctrlClose, SIGNAL(clicked()), this, SLOT(OnClose()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlSkipOpp, SIGNAL(clicked()), this, SLOT(OnSkipPoint()));
	connect(m_pctrlSkipPolar, SIGNAL(clicked()), this, SLOT(OnSkipPolar()));
	connect(m_rbtype1, SIGNAL(toggled(bool)), this, SLOT(OnType1()));
	connect(m_rbtype2, SIGNAL(toggled(bool)), this, SLOT(OnType1()));
	connect(m_rbtype3, SIGNAL(toggled(bool)), this, SLOT(OnType1()));
	connect(m_rbtype4, SIGNAL(toggled(bool)), this, SLOT(OnType1()));
	connect(m_rbspec1, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_rbspec2, SIGNAL(toggled(bool)), this, SLOT(OnAcl()));
	connect(m_rbRange1, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
	connect(m_rbRange2, SIGNAL(toggled(bool)), this, SLOT(OnRange()));
	connect(m_pctrlEditList, SIGNAL(clicked()), this, SLOT(OnEditReList()));
	connect(m_pctrlFromZero, SIGNAL(stateChanged(int)), this, SLOT(OnFromZero(int)));
	connect(m_pctrlSpecMin, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecMax, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));
	connect(m_pctrlSpecDelta, SIGNAL(editingFinished()), this, SLOT(OnSpecChanged()));}


void BatchDlg::SetLayout()
{
	QHBoxLayout *AnalysisType = new QHBoxLayout;
	m_rbtype1 = new	QRadioButton("Type 1");
	m_rbtype2 = new QRadioButton("Type 2");
	m_rbtype3 = new QRadioButton("Type 3");
	m_rbtype4 = new QRadioButton("Type 4");
	AnalysisType->addWidget(m_rbtype1);
	AnalysisType->addWidget(m_rbtype2);
	AnalysisType->addWidget(m_rbtype3);
	AnalysisType->addWidget(m_rbtype4);
//	AnalysisType->setMaximumHeight(50);
	QGroupBox *AnalysisTypeGroup = new QGroupBox("Analysis Type");
	AnalysisTypeGroup->setLayout(AnalysisType);
//	AnalysisTypeGroup->setMaximumHeight(70);

	QGridLayout *BatchVars = new QGridLayout;
	m_rbRange1 = new QRadioButton("Range");
	m_rbRange2 = new QRadioButton("Re List");
	m_pctrlEditList = new QPushButton("Edit List");
	QLabel *MinVal   = new QLabel("Min");
	QLabel *MaxVal   = new QLabel("Max");
	QLabel *DeltaVal = new QLabel("Increment");
	MinVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	MaxVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	DeltaVal->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	m_pctrlReType  = new QLabel("Reynolds=");
	m_pctrlMaType  = new QLabel("Mach=");
	m_pctrlReMin   = new FloatEdit(100000,0);
	m_pctrlReMax   = new FloatEdit(150000,0);
	m_pctrlReDelta = new FloatEdit(50000,0);
	m_pctrlMach    = new FloatEdit(0.00);

	BatchVars->addWidget(m_rbRange1, 1, 2);
	BatchVars->addWidget(m_rbRange2, 1, 3);
	BatchVars->addWidget(m_pctrlEditList, 1, 4);
	BatchVars->addWidget(MinVal, 2, 2);
	BatchVars->addWidget(MaxVal, 2, 3);
	BatchVars->addWidget(DeltaVal, 2, 4);
	BatchVars->addWidget(m_pctrlReType, 3, 1);
	BatchVars->addWidget(m_pctrlReMin, 3, 2);
	BatchVars->addWidget(m_pctrlReMax, 3, 3);
	BatchVars->addWidget(m_pctrlReDelta, 3, 4);
	BatchVars->addWidget(m_pctrlMaType, 4, 1);
	BatchVars->addWidget(m_pctrlMach, 4, 2);
	QGroupBox *BatchVarsGroup = new QGroupBox("Batch Variables");
	BatchVarsGroup->setLayout(BatchVars);


	QGridLayout *RangeVars = new QGridLayout;
	QLabel *Spec = new QLabel("Specify");
	m_rbspec1 = new QRadioButton("Alpha");
	m_rbspec2 = new QRadioButton("Cl");
	m_pctrlFromZero   = new QCheckBox("From Zero");
	QLabel *SpecMin   = new QLabel("Min");
	QLabel *SpecMax   = new QLabel("Max");
	QLabel *SpecDelta = new QLabel("Increment");
	SpecMin->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	SpecMax->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	SpecDelta->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	m_pctrlSpecVar    = new QLabel("Spec =");
	m_pctrlSpecMin    = new FloatEdit(0.00);
	m_pctrlSpecMax    = new FloatEdit(1.00);
	m_pctrlSpecDelta  = new FloatEdit(0.50);
	RangeVars->addWidget(Spec, 1, 1);
	RangeVars->addWidget(m_rbspec1, 1, 2);
	RangeVars->addWidget(m_rbspec2, 1, 3);
	RangeVars->addWidget(m_pctrlFromZero, 1, 4);
	RangeVars->addWidget(SpecMin, 2, 2);
	RangeVars->addWidget(SpecMax, 2, 3);
	RangeVars->addWidget(SpecDelta, 2, 4);
	RangeVars->addWidget(m_pctrlSpecVar, 3, 1);
	RangeVars->addWidget(m_pctrlSpecMin, 3, 2);
	RangeVars->addWidget(m_pctrlSpecMax, 3, 3);
	RangeVars->addWidget(m_pctrlSpecDelta, 3, 4);
	QGroupBox *RangeVarsGroup = new QGroupBox("Analysis Range");
	RangeVarsGroup->setLayout(RangeVars);


	QGridLayout *TransVars = new QGridLayout;
	QLabel *FreeTransMethod = new QLabel("Free transitions (e^n)");
	QLabel *NCritLabel = new QLabel("NCrit=");
	QLabel *ForcedTransMethod = new QLabel("Forced transitions");
	QLabel *TopTransLabel = new QLabel("Trip Location (top)");
	QLabel *BotTransLabel = new QLabel("Trip Location (bottom)");
	m_pctrlNCrit   = new FloatEdit(9.00);
	m_pctrlXTopTr = new FloatEdit(1.00);
	m_pctrlXBotTr = new FloatEdit(1.00);
	TransVars->addWidget(FreeTransMethod, 1, 1);
	TransVars->addWidget(NCritLabel, 1, 2);
	TransVars->addWidget(m_pctrlNCrit, 1, 3);
	TransVars->addWidget(ForcedTransMethod, 2, 1);
	TransVars->addWidget(TopTransLabel, 2, 2);
	TransVars->addWidget(m_pctrlXTopTr, 2, 3);
	TransVars->addWidget(BotTransLabel, 3, 2);
	TransVars->addWidget(m_pctrlXBotTr, 3, 3);
	QGroupBox *TransVarsGroup = new QGroupBox("Transitions Data");
	TransVarsGroup->setLayout(TransVars);

	m_pctrlInitBL         = new QCheckBox("Initialize BLs between polars");

	//_*_*_*_*_*_*_**_*_*_**_*_*_*_
	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
	m_pctrlGraphOutput = new GraphWidget;
	m_pctrlGraphOutput->m_pGraph = &m_RmsGraph;
//	m_pctrlGraphOutput->setMinimumHeight(300);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	m_pctrlClose     = new QPushButton("Close");
	m_pctrlAnalyze   = new QPushButton(tr("Analyze"))	;
	m_pctrlSkipOpp   = new QPushButton("Skip Opp");
	m_pctrlSkipPolar = new QPushButton("Skip Polar");
	m_pctrlAnalyze->setAutoDefault(true);

	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlAnalyze);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlSkipOpp);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlSkipPolar);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlClose);
	CommandButtons->addStretch(1);

	QVBoxLayout *LeftSide = new QVBoxLayout;
	LeftSide->addWidget(AnalysisTypeGroup);
	LeftSide->addWidget(BatchVarsGroup);
	LeftSide->addWidget(RangeVarsGroup);
	LeftSide->addWidget(TransVarsGroup);

	QVBoxLayout *RightSide = new QVBoxLayout;
	RightSide->addWidget(m_pctrlInitBL);
	RightSide->addWidget(m_pctrlTextOutput,1);
	RightSide->addWidget(m_pctrlGraphOutput,2);
	RightSide->addLayout(CommandButtons);


	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->setSpacing(10);
	mainLayout->addLayout(LeftSide);
	mainLayout->addLayout(RightSide);
	setLayout(mainLayout);

}

void BatchDlg::AlphaLoop()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
//	QXDirect* pXDirect = (QXDirect*)m_pXDirect;
	QString str;
	QString strong = "";
	QString StrTmp;
	int iRe, iAlpha, nAlpha;
	double alphadeg;
	nAlpha = (int)(fabs((m_SpMax-m_SpMin)*1.000/m_SpInc));//*1.0001 to make sure upper limit is included

	for (iAlpha=0; iAlpha<=nAlpha; iAlpha++)
	{
		qApp->processEvents();
		if(m_bCancel) break;
		
		alphadeg = m_SpMin + iAlpha*m_SpInc;
		pXFoil->alfa = alphadeg*3.141592654/180.0;
		str = QString("Alpha = %1\r\n").arg(alphadeg,0,'f',2);
		strong+= str;
		UpdateOutput(str);

		int total = (int)(fabs((m_ReMax-m_ReMin)*1.0001/m_ReInc));
		CreatePolar(alphadeg, pXFoil->minf1, pXFoil->acrit);// Do something

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

				// here we go !
				if (!pXFoil->specal())
				{
					str ="Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
					QMessageBox::information(this, "QFLR5", str);
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
					str =QString("   ...converged after %1 iterations\r\n").arg(m_Iterations);
					strong+= str;
					UpdateOutput(str);
					m_pCurPolar->AddData(pXFoil);
				}
				else if(m_bSkipPoint || m_bSkipPolar)
				{
					str = QString("   ...skipped after %1 iterations\r\n").arg(m_Iterations);
					strong+= str;
					UpdateOutput(str);

				}
				else
				{
					str = QString("   ...unconverged after %1 iterations\r\n").arg(m_Iterations);
					strong+= str;
					UpdateOutput(str);
				}
			}
			else
			{
				break;
			}
		}// end Alpha or Cl loop
		strong+="\r\n";
		if(m_bCancel)
		{
			strong+="Analysis interrupted\r\n";
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

		if(m_AlphaMin< m_AlphaMax) m_AlphaInc =  abs(m_SpInc);
		else                       m_AlphaInc = -abs(m_SpInc);
	}
	else
	{
		m_ClMin = m_SpMin ;
		m_ClMax = m_SpMax ;
		if(m_ClMin< m_ClMax) m_ClInc =  abs(m_ClInc);
		else                 m_ClInc = -abs(m_ClInc);
	}

	if(m_ReMin < m_ReMax)  m_ReInc =  abs(m_ReInc);
	else                   m_ReInc = -abs(m_ReInc);
	InitXFoil2();
}


void BatchDlg::Analysis3()
{
//	m_bType4 = true;
//	m_bAlpha = false;

	m_AlphaMin = m_SpMin ;
	m_AlphaMax = m_SpMax ;
	if(m_SpMin< m_SpMax) m_AlphaInc =  abs(m_SpInc);
	else                 m_AlphaInc = -abs(m_SpInc);


	if(m_ReMin < m_ReMax) m_ReInc =  abs(m_ReInc);
	else                  m_ReInc = -abs(m_ReInc);

	InitXFoil2();
}


void BatchDlg::CleanUp()
{
	ResetCurves();
	if(m_pXFile->isOpen())	m_pXFile->close();
	m_pctrlClose->setEnabled(true);
	m_pctrlSkipOpp->setEnabled(false);
	m_pctrlSkipPolar->setEnabled(false);
	m_pctrlAnalyze->setText("Analyze");
	m_bIsRunning = false;
	m_bCancel    = false;
	m_bSkipPoint = false;
	m_bSkipPolar = false;
	m_pctrlClose->setFocus();
}


void BatchDlg::CreatePolar(double Spec, double Mach, double NCrit)
{
//	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	if(!m_pCurFoil) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_pCurPolar = new CPolar;
	m_pCurPolar->m_FoilName   = m_FoilName;
	m_pCurPolar->m_bIsVisible = true;
	m_pCurPolar->m_Type = m_Type;
	switch (m_pCurPolar->m_Type)
	{
		case 1:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 1;
			break;
		case 2:
			m_pCurPolar->m_MaType = 2;
			m_pCurPolar->m_ReType = 2;
			break;
		case 3:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 3;
			break;
		case 4:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 1;
			break;
		default:
			m_pCurPolar->m_ReType = 1;
			m_pCurPolar->m_MaType = 1;
			break;
	}
	if(m_Type !=4)
	{
		m_pCurPolar->m_Reynolds = Spec;
	}
	else
	{
		m_pCurPolar->m_ASpec = Spec;
	}
	m_pCurPolar->m_Mach  = Mach;
	m_pCurPolar->m_ACrit = NCrit;
	m_pCurPolar->m_XTop  = m_XTopTr;
	m_pCurPolar->m_XBot  = m_XBotTr;

	m_pCurPolar->m_Color = pMainFrame->GetColor(1);

	SetPlrName();
	CPolar *pPolar = pMainFrame->GetPolar(m_pCurFoil->m_FoilName, m_pCurPolar->m_PlrName);

	if(pPolar)
	{
		delete m_pCurPolar;
		m_pCurPolar = pPolar;
	}
	else m_pCurPolar = pMainFrame->AddPolar(m_pCurPolar);
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

	QXDirect* pXDirect = (QXDirect*)m_pXDirect;

	m_FoilName = pXDirect->m_pCurFoil->m_FoilName;
	QString str = "Batch analysis for "+ m_FoilName;
	setWindowTitle(str);

	m_pctrlMach->SetPrecision(2);

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

	if(m_Type!=4)
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
	if(m_Type!=4)
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

	if(m_Type==1)      m_rbtype1->setChecked(true);
	else if(m_Type==2) m_rbtype2->setChecked(true);
	else if(m_Type==3) m_rbtype3->setChecked(true);
	else if(m_Type==4) m_rbtype4->setChecked(true);
	OnType1();


	if(!m_bFromList)  m_rbRange1->setChecked(true);
	else              m_rbRange2->setChecked(true);
	OnRange();

	if(m_bFromZero)  m_pctrlFromZero->setChecked(true);
	else             m_pctrlFromZero->setChecked(false);

	m_pctrlInitBL->setChecked(true);
	m_pctrlSkipOpp->setEnabled(false);
	m_pctrlSkipPolar->setEnabled(false);

//	m_TitleFont.CreateFontIndirect(&LgFt);
//	m_TitleFont.CreatePointFont(85, "Courier New");

	m_Iterations = 0;
	ResetCurves();
}

/*

	m_pXFoil->lblini = false;
	m_pXFoil->lipan = false;

	m_pXFoil->reinf1 = m_pCurPolar->m_Reynolds;
	if (m_pCurPolar->m_Type == 4) m_pXFoil->alfa = m_pCurPolar->m_ASpec*pi/180.0;
	m_pXFoil->lalfa = true;
	m_pXFoil->qinf  = 1.0;

}
*/
bool BatchDlg::InitXFoil2()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->Initialize();
	pXFoil->pXFile = m_pXFile;

	pXFoil->reinf1 = m_ReMin;
	pXFoil->minf1  = m_Mach;

	switch (m_Type)
	{
		case 1:
			pXFoil->retyp  = 1;
			pXFoil->matyp  = 1;
			break;
		case 2:
			pXFoil->retyp  = 2;
			pXFoil->matyp  = 2;
			break;
		case 3:
			pXFoil->retyp  = 3;
			pXFoil->matyp  = 1;
			break;
		case 4:
			pXFoil->retyp  = 1;
			pXFoil->matyp  = 1;
			break;
		default:
			pXFoil->retyp  = 1;
			pXFoil->matyp  = 1;
			break;
	}

	pXFoil->lalfa = true;
	pXFoil->qinf = 1.0;

	pXFoil->acrit      = m_NCrit;
	pXFoil->xstrip[1]  = m_XTopTr;
	pXFoil->xstrip[2]  = m_XBotTr;

	if (m_Mach !=0.0)
	{
		if(!pXFoil->SetMach())
		{
			QString str;
			str = "Invalid Analysis Settings\nCPCALC: local speed too large \n Compressibility corrections invalid ";
			QMessageBox::information(window(), "QFLR5", str);
		}
	}

	for(int i =0; i<m_pCurFoil->n; i++)
	{
		pXFoil->xb[i+1] = m_pCurFoil->x[i];
		pXFoil->yb[i+1] = m_pCurFoil->y[i];
	}
	pXFoil->nb     = m_pCurFoil->n;
	pXFoil->lbflap = false;
	pXFoil->ddef   = 0.0;
	pXFoil->xbf    = 1.0;
	pXFoil->ybf    = 0.0;
// end "load"

	if(pXFoil->Preprocess())
	{
		pXFoil->abcopy();
	}
	else
	{
			//what do I do now ?
	}

	return true;
}



bool BatchDlg::Iterate()
{
	QString str;
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	QXDirect* pXDirect = (QXDirect*)m_pXDirect;

	if(!pXFoil->viscal())
	{
		pXFoil->lvconv = false;//point is unconverged
		str ="CpCalc: local speed too large \r\n Compressibility corrections invalid";
		QMessageBox::information(this, "QFLR5", str);
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
			OutputIter(m_Iterations, pXFoil->reinf1, pXFoil->alfa);
		}
		else
		{
			m_Iterations = m_IterLim;
		}
	}

	OutputIter(0, 0.0, 0.0);
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
		str ="CpCalc: local speed too large \r\n Compressibility corrections invalid";
		QMessageBox::information(this, "QFLR5", str);
		m_bCancel = true;
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
	if(m_Type==4) return;
	if(m_rbspec1->isChecked())
	{
		m_pctrlSpecVar->setText("Alpha =");
		m_pctrlSpecMax->SetValue(1.0);
		m_pctrlSpecDelta->SetValue(0.5);
		m_bAlpha = true;
		m_pctrlFromZero->setEnabled(true);
	}
	else
	{
		m_pctrlSpecVar->setText("Cl =");
		m_pctrlSpecMax->SetValue(0.5);
		m_pctrlSpecDelta->SetValue(0.1);
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

void BatchDlg::OnType1()
{

	if(m_rbtype1->isChecked())
	{
		m_pctrlReType->setText("Reynolds =");
		m_pctrlMaType->setText("Mach =");
		m_pctrlEditList->setEnabled(true);
		m_Type = 1;
	}
	else if(m_rbtype2->isChecked())
	{
		m_pctrlReType->setText("Re.sqrt(Cl) =");
		m_pctrlMaType->setText("Ma.sqrt(Cl) =");
		m_pctrlEditList->setEnabled(true);
		m_Type = 2;
	}
	else if(m_rbtype3->isChecked())
	{
		m_pctrlReType->setText("Re.Cl =");
		m_pctrlMaType->setText("Mach =");
		m_pctrlEditList->setEnabled(true);
		m_Type = 3;
	}
	else if(m_rbtype4->isChecked())
	{
		m_pctrlReType->setText("Alpha =");
		m_pctrlMaType->setText("Mach =");
		m_pctrlEditList->setEnabled(false);
		m_rbspec1->setChecked(true);
		m_Type = 4;
	}

	if(m_Type !=4)
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
		m_pctrlSpecVar->setText("&Reynolds =");
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

void BatchDlg::OutputIter(int iter, double Re, double Alpha)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	if(iter)
	{
		m_RmsGraph.GetCurve(0)->AddPoint(iter, pXFoil->rmsbl);
		m_RmsGraph.GetCurve(1)->AddPoint(iter, pXFoil->rmxbl);
		UpdateGraph(Re, Alpha);
	}
}


void BatchDlg::OnAnalyze()
{
	if(m_bIsRunning)
	{
		m_bCancel = true;
		return;
	}
	m_bCancel    = false;
	m_bIsRunning = true;

	XFoil *pXFoil = (XFoil*)m_pXFoil;

	m_pctrlTextOutput->setText("");

	m_pctrlClose->setEnabled(false);
	QString str;

	QString FileName = QDir::tempPath() + "/QFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	pXFoil->pXFile = m_pXFile;


	ReadParams();
	SetFileHeader();
	if (m_pctrlInitBL->isChecked()) m_bInitBL = true;
	else                           m_bInitBL = false;

	if(m_Type!=4) Analysis2();
	else          Analysis3();

	pXFoil->lvisc = true;
	pXFoil->m_bTrace = false;

	m_pctrlAnalyze->setText("Cancel");
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
	ReListDlg dlg;

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
	if(m_Type !=4)
	{
		m_ReInc = m_pctrlReDelta->GetValue();
		m_ReMax = m_pctrlReMax->GetValue();
		m_ReMin = m_pctrlReMin->GetValue();

		m_SpInc = m_pctrlSpecDelta->GetValue();
		m_SpMax = m_pctrlSpecMax->GetValue();
		m_SpMin = m_pctrlSpecMin->GetValue();
	}
	else
	{
		m_SpInc = m_pctrlReDelta->GetValue();
		m_SpMax = m_pctrlReMax->GetValue();
		m_SpMin = m_pctrlReMin->GetValue();

		m_ReInc = m_pctrlSpecDelta->GetValue();
		m_ReMax = m_pctrlSpecMax->GetValue();
		m_ReMin = m_pctrlSpecMin->GetValue();
	}

	if(m_ReMin<=0.0) m_ReMin = fabs(m_ReInc);

	m_Mach     = m_pctrlMach->GetValue();
	if(m_Mach<=0.0) m_Mach = 0.0;
	m_NCrit    = m_pctrlNCrit->GetValue();
	m_XTopTr   = m_pctrlXTopTr->GetValue();
	m_XBotTr   = m_pctrlXBotTr->GetValue();
}


void BatchDlg::ReLoop()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	QXDirect* pXDirect = (QXDirect*)m_pXDirect;
	QString str;
	QString strong = "";
	QString StrTmp;
	double alphadeg;
	int ia, iRe, nRe, series, total, MaxSeries;
	double SpMin, SpMax, SpInc;

	if(!m_bFromList) nRe = (int)fabs((m_ReMax-m_ReMin)/m_ReInc);
	else             nRe = m_NRe-1;

	for (iRe=0; iRe<=nRe; iRe++)
	{
		if(!m_bFromList) pXFoil->reinf1 = m_ReMin + iRe *m_ReInc;
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
			SpInc = m_SpInc;
		}
		else
		{
			MaxSeries = 1;
			SpMin = m_SpMin;
			SpMax = m_SpMax;
			SpInc = m_SpInc;
		}

		m_bSkipPolar = false;

		for (series=0; series<MaxSeries;series++)
		{
			qApp->processEvents();
			if(m_bCancel) break;

			total = int((SpMax*1.0001-SpMin)/SpInc);//*1.0001 to make sure upper limit is included

			CreatePolar(pXFoil->reinf1, pXFoil->minf1, pXFoil->acrit);
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
						pXFoil->alfa = alphadeg * pi/180.0;
						pXFoil->lalfa = true;
						pXFoil->qinf = 1.0;
						str = QString("Alpha = %1").arg(alphadeg,9,'f',3);
						strong+=str;
						UpdateOutput(str);

						// here we go !
						if (!pXFoil->specal())
						{
							str ="Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
							QMessageBox::information(this, "QFLR5", str);
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
						str = QString("Cl = %1").arg(pXFoil->clspec,9,'f',3);
						strong+=str;
						UpdateOutput(str);
						if(!pXFoil->speccl())
						{
							str ="Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
							QMessageBox::information(this, "QFLR5", str);
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
						str = QString("   ...converged after %1 iterations\n").arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);
						m_pCurPolar->AddData(pXFoil);
					}
					else if(m_bSkipPoint || m_bSkipPolar)
					{
						str = QString("   ...skipped after %1 iterations\r\n").arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);

					}
					else
					{
						str = QString("   ...unconverged after %1 iterations\r\n").arg(m_Iterations);
						strong+= str;
						UpdateOutput(str);
					}
					if(pXDirect->m_bPolar)
					{
						pXDirect->CreatePolarCurves();
						pXDirect->UpdateView();
					}
				}
				else
				{
					break;
				}
			}// end Alpha or Cl loop
			SpMin = 0;
			SpMax = m_SpMin;
			SpInc = -m_SpInc;
		}
//		strong+="\r\n";
		if(m_bCancel) break;

	}//end Re loop

//	WriteString(strong);
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
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << "QFLR5 v001";
	out << "\n";
	out << m_FoilName;
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



void BatchDlg::SetPlrName()
{
	if(m_Type !=4)
	{
		double R = m_pCurPolar->m_Reynolds/1000000.;
		m_pCurPolar->m_PlrName = QString("T%1_Re%2_M%3")
								 .arg(m_pCurPolar->m_Type)
								 .arg(R,0,'f',2)
								 .arg( m_pCurPolar->m_Mach,0,'f',2);
	}
	else
	{
		m_pCurPolar->m_PlrName = QString("T%1_Al%2_M%3")
								 .arg(m_pCurPolar->m_Type)
								 .arg(m_pCurPolar->m_ASpec,5,'f',2)
								 .arg(m_pCurPolar->m_Mach,0,'f',2);
	}
	QString str;
	str = QString("_N%1").arg(m_pCurPolar->m_ACrit,0,'f',1);
	m_pCurPolar->m_PlrName += str;
}



void BatchDlg::StartAnalysis()
{
	m_pctrlAnalyze->setText("Cancel");
	if(m_Type !=4) 	ReLoop();
	else            AlphaLoop();

	QString strong = "Analysis completed";
	UpdateOutput(strong);

	CleanUp();
}


void BatchDlg::UpdateGraph(double Re, double Alpha)
{
	m_pctrlGraphOutput->update();
//	repaint();

	if(Re>0.0)
	{
		QString strong = QString("Re=%1  /  Alpha=%2").arg(Re,8,'f',0).arg(Alpha*180.0/pi,5,'f',2);
//		QPoint Place(int((BltRect.left+BltRect.right)/2), BltRect.top+10);
//		dcMem.TextOut(Place.x, Place.y, strong);
//		dcMem.SelectObject(pOldFont);
	}
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
















