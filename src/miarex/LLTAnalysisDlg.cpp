/****************************************************************************

    LLTAnalysisDlg Class
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

#include <math.h>
#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "LLTAnalysisDlg.h"
#include "Miarex.h"
#include "../mainframe.h"


void *LLTAnalysisDlg::s_pMainFrame;
void *LLTAnalysisDlg::s_pMiarex;


/**
*The public constructor
*/
LLTAnalysisDlg::LLTAnalysisDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("LLT Analysis"));
	SetupLayout();

	m_IterGraph.SetXTitle(tr("Iterations"));
	m_IterGraph.SetYTitle("");

	m_IterGraph.SetAuto(true);

	m_IterGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_IterGraph.SetYMajGrid(true, QColor(120,120,120),2,1);

	m_IterGraph.SetXMin(0.0);
	m_IterGraph.SetXMax(50);
	m_IterGraph.SetYMin(0.0);
	m_IterGraph.SetYMax(1.0);
	m_IterGraph.SetType(1);

	m_IterGraph.SetXTitle(tr("abs"));
	m_IterGraph.SetYTitle("|Da|");

	m_IterGraph.SetMargin(10);

	m_bSkip       = false;
	m_bExit       = false;
	m_bCancel     = false;
	m_bWarning    = false;
	m_bError      = false;
	m_bFinished   = false;
	m_bInitCalc   = true;
	m_bSequence   = false;

	m_Iterations =  0;
	m_IterLim    = 20;

	m_LegendPlace.rx() = 0;
	m_LegendPlace.ry() = 0;

	m_AlphaMin = m_AlphaMax = m_AlphaDelta = 0.;
	m_ReMin = m_ReMax = m_ReDelta = 0.0;

	m_pXFile       = NULL;

	m_LLT.m_pParent = this;
}


/**
* Launches a type 1 or 2 analysis.
* Loops over the range of specified aoa. 
* For each successful aoa, stores the data in the WPolar and Operating Point objects.
*/
bool LLTAnalysisDlg::AlphaLoop()
{
	QString str;

	QMiarex* pMiarex = (QMiarex*)s_pMiarex;
	int i,iter;

//	CWing::s_AlphaLLT = m_AlphaMin;

	str = tr("Launching analysis....")+"\n\n";
	UpdateOutput(str);
	str = QString(tr("Max iterations     = %1")+"\n").arg(m_IterLim);
	UpdateOutput(str);
	str = QString(tr("Alpha precision    = %1 deg")+"\n").arg(LLTAnalysis::s_CvPrec,0,'f',6);
	UpdateOutput(str);
	str = QString(tr("Relaxation factor  = %1")+"\n").arg(LLTAnalysis::s_RelaxMax,0,'f',1);
	UpdateOutput(str);
	str = QString(tr("Number of stations = %1")+"\n\n").arg(LLTAnalysis::s_NLLTStations);
	UpdateOutput(str);
	
	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -fabs(m_AlphaDelta);
	int ia  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.001/m_AlphaDelta);

	if(!m_bSequence) ia = 0;

	m_LLT.LLTInitialize(m_pWPolar->m_QInf);

	m_IterGraph.ResetLimits();
	m_IterGraph.SetXMax((double)m_IterLim);
	m_IterGraph.SetYMinGrid(false, true, QColor(100,100,100), 2, 1, 4);

	double Alpha;

	for (i=0; i<=ia; i++)
	{
		Alpha = m_AlphaMin +(double)i * m_AlphaDelta;
		if(m_bCancel) 
		{
			str = tr("Analysis cancelled on user request....")+"\n";
			UpdateOutput(str);
			break;
		}
		m_IterGraph.SetYMin(0.0);
		m_IterGraph.SetYMax(0.5);
		Curve *pCurve = m_IterGraph.GetCurve(0);
		pCurve->ResetCurve();

		m_LLT.LLTInitCl(m_pWPolar->m_QInf, Alpha);
		if(m_bInitCalc) m_LLT.LLTSetLinearSolution(Alpha);

		str= QString(tr("Calculating Alpha = %1... ")).arg(Alpha,5,'f',2);
		UpdateOutput(str);

		iter = m_LLT.LLTIterate(m_pWPolar->m_QInf, Alpha);

		if (m_bSkip)
		{
			if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR) str = QString(tr("Alpha = %1, skipped after %2 iterations ")+"\n").arg(Alpha, 6,'f',2).arg(iter);
			else                                       str = QString(tr("QInf = %1 skipped after %2 iterations ")+"\n"  ).arg(m_pWPolar->m_QInf,  8,'f',2).arg(iter);
			UpdateOutput(str);
			m_bSkip = false;
			m_bInitCalc = true;
		}
		else if (iter==-1 && !m_bCancel)
		{
			str= QString(tr("    ...negative Lift... Aborting")+"\n");
			m_bError = true;
			m_bInitCalc = true;
			UpdateOutput(str);
		}
		else if (iter<m_IterLim && !m_bCancel)
		{
			//converged, 
			str= QString(tr("    ...converged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_LLT.LLTComputeWing(m_pWPolar->m_QInf, Alpha, str);// generates wing results,
			UpdateOutput(str);
			if (m_LLT.m_bWingOut) m_bWarning = true;
			pMiarex->AddWOpp(m_pWPolar->m_QInf, Alpha, m_LLT.m_bWingOut);// Adds WOpp point and adds result to polar
			if(pMiarex->m_iView==WPOLARVIEW)
			{
				pMiarex->CreateWPolarCurves();
				pMiarex->UpdateView();
			}
			m_bInitCalc = false;
		}
		else 
		{
			if (m_LLT.m_bWingOut) m_bWarning = true;
			m_bError = true;
			str= QString(tr("    ...unconverged after %2 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_bInitCalc = true;
		}
		qApp->processEvents();
	}

	return true;

}

/**
* Launches a type 4 analysis.
* Loops over the range of specified velocities. 
* For each successful aoa, stores the data in the WPolar and Operating Point objects.
*/
bool LLTAnalysisDlg::QInfLoop()
{
	//Alpha stands for QInf...
	int i,iter;
	QString str;

	QMiarex* pMiarex = (QMiarex*)s_pMiarex;
	Curve *pCurve = m_IterGraph.GetCurve(0);

	//Alpha has been set in CMiarex::SetWPlr

	str = tr("Launching analysis....")+"\n\n";
	UpdateOutput(str);
	str = QString(tr("Max iterations     = %1")+"\n").arg(m_IterLim);
	UpdateOutput(str);
	str = QString(tr("Alpha precision    = %1 deg")+"\n").arg(LLTAnalysis::s_CvPrec,0,'f',6);
	UpdateOutput(str);
	str = QString(tr("Relaxation factor  = %1")+"\n").arg(LLTAnalysis::s_RelaxMax,0,'f',1);
	UpdateOutput(str);
	str = QString(tr("Number of stations = %1")+"\n\n").arg(LLTAnalysis::s_NLLTStations);
	UpdateOutput(str);
	
	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -(double)fabs(m_AlphaDelta);
	int ia  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.001/m_AlphaDelta);

	if(!m_bSequence) ia = 0;

	str = tr("Initializing analysis...")+"\n";
	UpdateOutput(str);

	m_LLT.LLTInitialize(m_AlphaMin);
	//	CWing::s_QInfLLT = m_AlphaMin;

	m_IterGraph.ResetLimits();
	m_IterGraph.SetXMax((double)m_IterLim);
	m_IterGraph.SetYMinGrid(false, true, QColor(100,100,100), 2, 1, 4);

	double QInf;
	for (i=0; i<=ia; i++)
	{
		QInf = m_AlphaMin + (double)i * m_AlphaDelta;
		if(m_bCancel) 
		{
			str = tr("Analysis cancelled on user request....")+"\n";
			UpdateOutput(str);
			break;
		}
		m_IterGraph.SetYMin(0.0);
		m_IterGraph.SetYMax(0.5);
		pCurve->ResetCurve();

		if(m_bInitCalc) m_LLT.LLTSetLinearSolution(m_pWPolar->m_ASpec);
		 m_LLT.LLTInitCl(QInf, m_pWPolar->m_ASpec);
		
		str = QString(tr("Calculating QInf = %1... ")).arg(QInf,6,'f',2);
		UpdateOutput(str);
		iter = m_LLT.LLTIterate(QInf, m_pWPolar->m_ASpec);

		if(iter<0)
		{
			//unconverged
			m_bError = true;
			m_bWarning = true;
			str = QString("\n");
			UpdateOutput(str);
			m_bInitCalc = true;
		}
		else if (m_bSkip)
		{
			if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR) str = QString(tr("Alpha = %1, skipped after %2 iterations ")+"\n").arg(m_pWPolar->m_ASpec, 6,'f',2).arg(iter);
			else                                       str = QString(tr("QInf = %1 skipped after %2 iterations ")+"\n"  ).arg(QInf,  8,'f',2).arg(iter);
			UpdateOutput(str);
			m_bSkip = false;
			m_bInitCalc = true;
		}
		else if (iter<m_IterLim  && !m_bCancel)
		{
			//converged, 
			str = QString(tr("    ...converged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_LLT.LLTComputeWing(QInf, m_pWPolar->m_ASpec,str);// generates wing results,
			UpdateOutput(str);
			if (m_LLT.m_bWingOut) m_bWarning = true;
			pMiarex->AddWOpp(QInf, m_pWPolar->m_ASpec, m_LLT.m_bWingOut);// Adds WOpp point and adds result to polar
			if(pMiarex->m_iView==WPOLARVIEW)
			{
				pMiarex->CreateWPolarCurves();
				pMiarex->UpdateView();
			}
/*			if(m_LLT.m_bWingOut)
			{
				str = QString("\n");
				UpdateOutput(str);
			}*/
			m_bInitCalc = false;
		}
		else
		{
			if (m_LLT.m_bWingOut) m_bWarning = true;
			m_bError = true;
			str = QString(tr("    ...unconverged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_bInitCalc = true;
		}
		qApp->processEvents();
	}
	return true;
}



/**
*Initializes the dialog and its associated data.
*/
void LLTAnalysisDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	SetFileHeader();

	m_Iterations = 0;

	QString str;
	m_IterGraph.AddCurve();
	m_IterGraph.AddCurve();
	str = "|Da|";
	m_IterGraph.GetCurve(0)->SetTitle(str);

	m_IterGraph.SetAutoX(true);
	m_IterGraph.SetXMin(0.0);
	m_IterGraph.SetXMax((double)m_IterLim);
	m_IterGraph.SetX0(0.0);
	m_IterGraph.SetXUnit((int)(m_IterLim/10.0));

	m_IterGraph.SetAutoY(true);
	m_IterGraph.SetY0(0.0);
	m_IterGraph.SetYMin(0.0);
	m_IterGraph.SetYMax(1.0);

	m_IterGraph.SetMargin(40);
	if(pMainFrame) m_IterGraph.CopySettings(&pMainFrame->m_RefGraph,false);

	m_LLT.m_IterLim = m_IterLim;
}



/** Overrides and handles the keyPressEvent sent by Qt */
void LLTAnalysisDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			OnCancelAnalysis();
			event->accept();
			return;
		}
		default:
			event->ignore();
	}
}

/** The user has requested the cancellation of the analysis*/
void LLTAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
	m_bSkip = true;
	m_bExit = true;
	m_bCancel = true;
	if(m_bFinished) done(1);
}

/** The user has requested that the calculation for the current aoa be skipped. 
*The analysis will continue for the next aoa if anay.
*/
void LLTAnalysisDlg::OnSkipPoint()
{
	m_bSkip = true;
}


/**
*Clears the content of the graph's curve prior to the start of the calculation of the next operating point.
*/
void LLTAnalysisDlg::ResetCurves()
{
	Curve*pCurve;
	pCurve = m_IterGraph.GetCurve(0);
	if(pCurve) pCurve->ResetCurve();
	pCurve = m_IterGraph.GetCurve(1);
	if(pCurve) pCurve->ResetCurve();

}

/**
* Copies the value of the input parameters to the member variables
*/
void LLTAnalysisDlg::SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha)
{
	m_AlphaMin = AlphaMin;
	m_AlphaMax = AlphaMax;
	m_AlphaDelta = DeltaAlpha;
}


/**
* Initializes the header of the log file
*/
void LLTAnalysisDlg::SetFileHeader()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);
	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	out << m_pWing->m_WingName;
	out << "\n";
	if(pMiarex && pMiarex->m_pCurWPolar)
	{
//		out << pMiarex->m_pCurWPolar->m_WPlrName;
//		out << "\n";
	}

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";
}

/**
* Initializes the interface of the dialog box
*/
void LLTAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()*2/3);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setFontFamily("Courier");
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pGraphWidget = new GraphWidget;
//	m_pGraphWidget->setMinimumHeight(r.height()/4);
//	m_pGraphWidget->setMinimumWidth(r.width()/4);
	m_pGraphWidget->m_pGraph = &m_IterGraph;

//	QHBoxLayout *GraphLayout = new QHBoxLayout;
//	GraphLayout->addWidget(m_pGraphWidget,1);

	m_pctrlSkip   = new QPushButton(tr("Skip"));
	m_pctrlCancel = new QPushButton(tr("Cancel"));

	connect(m_pctrlSkip,   SIGNAL(clicked()), this, SLOT(OnSkipPoint()));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(m_pctrlSkip);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(m_pctrlCancel);
	buttonsLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addWidget(m_pGraphWidget,2);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
}


/**
* Launches the analysis.
* At this stage, the dialogbox has been setup and initialized.
* The LLTAnalysis object has been created and the input data has been loaded.
* Depending on the type of polar, the method launches either a loop over aoa or velocity values.
*/
void LLTAnalysisDlg::StartAnalysis()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	//all set to launch the analysis
	if(!m_pWPolar || !m_pWing) return;

	m_pctrlCancel->setText(tr("Cancel"));
	m_bSkip       = false;
	m_bExit       = false;
	m_bCancel     = false;
	m_bWarning    = false;
	m_bError      = false;
	m_bFinished   = false;

	m_pctrlTextOutput->clear();

	m_LLT.m_poaPolar = &pMainFrame->m_oaPolar;
	m_LLT.m_pWing = m_pWing;
	m_LLT.m_pWPolar = m_pWPolar;
	m_LLT.LLTInitialize(m_pWPolar->m_QInf);


	if (m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR)
	{
		AlphaLoop() ;
	}
	else
	{
		QInfLoop();
	}

	m_bFinished = true;
	QString strange = "\n_________\n"+tr("Analysis completed");
	if(m_bWarning)      strange += tr(" ...some points are outside the flight envelope");
	else if(m_bError)	strange += tr(" ...some points are unconverged");

	strange+= "\n";

	UpdateOutput(strange);
	m_pctrlCancel->setText(tr("Close"));
	m_pctrlSkip->setEnabled(false);

	m_bSkip   = false;
	m_bExit   = false;
}

/**
* Updates the graph widget. Called after each iteration of the LLTAnalysis. 
* Time consuming, but it's necessary to provide the user with visual feedback on the progress of the analysis
*/
void LLTAnalysisDlg::UpdateView()
{
	m_pGraphWidget->update();
	repaint();
}

/**
* Updates the text output in the dialog box and the log file.
*@param strong the text message to append to the output widget and to the log file.
*/
void LLTAnalysisDlg::UpdateOutput(QString &strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}


/**
* Appends a string to the log file
*@param strong the text message to append to the log file.
*/
void LLTAnalysisDlg::WriteString(QString &strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}


/** 
* Appends a point to the curve in the graph widget 
* @param x the x-value of the point to be appended
* @param x the y-value of the point to be appended
*/
void LLTAnalysisDlg::UpdateGraph(int x, double y)
{
	Curve *pCurve = m_IterGraph.GetCurve(0);
	pCurve->AppendPoint((double)x,y);
	UpdateView();
}


