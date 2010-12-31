/****************************************************************************

    LLTAnalysisDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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

#include <QtGui>
#include <math.h>
#include "LLTAnalysisDlg.h"
#include "Miarex.h"
#include "../MainFrame.h"


void *LLTAnalysisDlg::s_pMainFrame;
void *LLTAnalysisDlg::s_pMiarex;

LLTAnalysisDlg::LLTAnalysisDlg()
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

	m_Iterations =  0;
	m_IterLim    = 20;

	m_bSequence  = false;
	m_bAlpha     = true;

	m_LegendPlace.rx() = 0;
	m_LegendPlace.ry() = 0;

	m_AlphaMin   = 0.0;
	m_AlphaMax   = 1.0;
	m_AlphaDelta = 0.5;


	m_pXFile       = NULL;
	s_pMiarex      = NULL;
	s_pMainFrame   = NULL;
}





bool LLTAnalysisDlg::AlphaLoop()
{
	QString str;

	QMiarex* pMiarex = (QMiarex*)s_pMiarex;
	int i,iter;

	CWing::s_AlphaLLT = m_AlphaMin;

	str = tr("Launching analysis....")+"\n\n";
	UpdateOutput(str);
	str = QString(tr("Max iterations     = %1")+"\n").arg(m_IterLim);
	UpdateOutput(str);
	str = QString(tr("Alpha precision    = %1 deg")+"\n").arg(CWing::s_CvPrec,0,'f',6);
	UpdateOutput(str);
	str = QString(tr("Relaxation factor  = %1")+"\n").arg(CWing::s_RelaxMax,0,'f',1);
	UpdateOutput(str);
	str = QString(tr("Number of stations = %1")+"\n\n").arg(CWing::s_NLLTStations);
	UpdateOutput(str);
	
	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -fabs(m_AlphaDelta);
	int ia  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.001/m_AlphaDelta);

	if(!m_bSequence) ia = 0;

	if(!m_pWing->LLTInitialize(m_pWPolar->m_Weight)) return false;

	m_IterGraph.ResetLimits();
	m_IterGraph.SetXMax((double)m_IterLim);
	m_IterGraph.SetYMinGrid(false, true, QColor(100,100,100), 2, 1, 4);

	for (i=0; i<=ia; i++)
	{
		if(m_bCancel) 
		{
			str = tr("Analysis cancelled on user request....")+"\n";
			UpdateOutput(str);
			break;
		}
		m_IterGraph.SetYMin(0.0);
		m_IterGraph.SetYMax(0.5);
		CCurve *pCurve = m_IterGraph.GetCurve(0);
		pCurve->ResetCurve();
		m_State=0;
		m_pWing->LLTInitCl();//with new angle...
		if(m_pWing->m_bInitCalc) m_pWing->LLTSetLinearSolution();

		str= QString(tr("Calculating Alpha = %1... ")).arg(CWing::s_AlphaLLT,5,'f',2);
		UpdateOutput(str);
		iter = Iterate();

		if (m_bSkip)
		{
			m_bSkip = false;
			m_pWing->m_bInitCalc = true;
		}
		else if (iter==-1 && !m_bCancel)
		{
			str= QString(tr("    ...negative Lift... Aborting")+"\n");
			m_bError = true;
			UpdateOutput(str);
			m_pWing->m_bInitCalc = true;
		}
		else if (iter<m_IterLim && !m_bCancel)
		{
			//converged, 
			str= QString(tr("    ...converged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_pWing->LLTComputeWing();// generates wing results,
			if (m_pWing->m_bWingOut) m_bWarning = true;
			pMiarex->AddWOpp(m_pWing->m_bWingOut);// Adds WOpp point and adds result to polar
/*			if(m_pWing->m_bWingOut)
			{
				str= QString("\n");
				UpdateOutput(str);
			}*/
			m_pWing->m_bInitCalc = false;
		}
		else 
		{
			if (m_pWing->m_bWingOut) m_bWarning = true;
			m_bError = true;
			str= QString(tr("    ...unconverged after %2 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_pWing->m_bInitCalc = true;
		}
		CWing::s_AlphaLLT += m_AlphaDelta;
	}

	return true;

}



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
}



int LLTAnalysisDlg::Iterate()
{
	QString str;
	CCurve *pCurve = m_IterGraph.GetCurve(0);

	int resp=0;
	int   iter = 0;
	bool  bConverged = false;

	while(iter<m_IterLim && !m_bSkip)
	{
		if(m_bCancel) break;
		iter++;
		resp = m_pWing->LLTIterate();
		if(resp == 1) 
		{
			bConverged = true;
			m_State=1;
			pCurve->AddPoint((double)iter, m_pWing->m_Maxa);
			UpdateView();
			return iter;
		}
		else if (resp==-1) 
		{
			bConverged = false;
			break;// Type 2, lift <0
		}
		// else continue iterations

		pCurve->AddPoint((double)iter, m_pWing->m_Maxa);
		UpdateView();
		qApp->processEvents();
	}
	if(m_bSkip)
	{
		if(m_pWPolar->m_Type!=4) str = QString(tr("Alpha = %1, skipped after %2 iterations ")+"\n").arg(CWing::s_AlphaLLT, 6,'f',2).arg(iter);
		else                     str = QString(tr("QInf = %1 skipped after %2 iterations ")+"\n"  ).arg(CWing::s_QInfLLT,  8,'f',2).arg(iter);
		UpdateOutput(str);
	}
	else if (resp<0)
	{ 
		//negative lift
		m_State=2;
		UpdateView();
		m_pWing->m_bInitCalc = true;
		return -1;
	}
	else if(!bConverged && !m_bCancel)
	{
		m_State=2;
		UpdateView();

		m_pWing->m_bInitCalc = true;
		return m_IterLim;
	}
	return iter;
}


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


void LLTAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
	m_bSkip = true;
	m_bExit = true;
	m_bCancel = true;
	if(m_bFinished) done(1);
}


void LLTAnalysisDlg::OnSkipPoint()
{
	m_bSkip = true;
}



void LLTAnalysisDlg::ResetCurves()
{
	CCurve*pCurve;
	pCurve = m_IterGraph.GetCurve(0);
	if(pCurve) pCurve->ResetCurve();
	pCurve = m_IterGraph.GetCurve(1);
	if(pCurve) pCurve->ResetCurve();

}


void LLTAnalysisDlg::SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha)
{
	m_AlphaMin = AlphaMin;
	m_AlphaMax = AlphaMax;
	m_AlphaDelta = DeltaAlpha;
}



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



void LLTAnalysisDlg::StartAnalysis()
{
	//all set to launch the analysis
	if(!m_pWPolar || !m_pWing) return;

	m_pctrlCancel->setText(tr("Cancel"));
	m_bSkip       = false;
	m_bExit       = false;
	m_bCancel     = false;
	m_bWarning    = false;
	m_bError      = false;
	m_bFinished   = false;

	if(m_pWing && m_pWPolar) m_pWing->m_Type=m_pWPolar->m_Type;
	else return;

	m_pctrlTextOutput->clear();

	if (m_pWPolar->m_Type!=4)
	{
		CWing::s_Viscosity = m_pWPolar->m_Viscosity;
		CWing::s_QInfLLT   = m_pWPolar->m_QInf;
		AlphaLoop() ;
	}
	else
	{
		ReLoop();
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


void LLTAnalysisDlg::UpdateView()
{
	m_pGraphWidget->update();
	repaint();
}


void LLTAnalysisDlg::UpdateOutput(QString &strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}


void LLTAnalysisDlg::WriteString(QString &strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}


bool LLTAnalysisDlg::ReLoop()
{
	//Alpha stands for QInf...

	int i,iter;
	QString str;

	QMiarex* pMiarex = (QMiarex*)s_pMiarex;
	CCurve *pCurve = m_IterGraph.GetCurve(0);

	CWing::s_QInfLLT = m_AlphaMin;
	//Alpha has been set at CMiarex::SetWPlr

	str = tr("Launching analysis....")+"\n\n";
	UpdateOutput(str);
	str = QString(tr("Max iterations     = %1")+"\n").arg(m_IterLim);
	UpdateOutput(str);
	str = QString(tr("Alpha precision    = %1 deg")+"\n").arg(CWing::s_CvPrec,0,'f',6);
	UpdateOutput(str);
	str = QString(tr("Relaxation factor  = %1")+"\n").arg(CWing::s_RelaxMax,0,'f',1);
	UpdateOutput(str);
	str = QString(tr("Number of stations = %1")+"\n\n").arg(CWing::s_NLLTStations);
	UpdateOutput(str);
	
	if(m_AlphaMax<m_AlphaMin) m_AlphaDelta = -(double)fabs(m_AlphaDelta);
	int ia  = (int)fabs((m_AlphaMax-m_AlphaMin)*1.001/m_AlphaDelta);

	if(!m_bSequence) ia = 0;

	str = tr("Initializing analysis...")+"\n";
	UpdateOutput(str);

	if(!m_pWing->LLTInitialize(m_pWPolar->m_Weight)) return false;

	m_IterGraph.ResetLimits();
	m_IterGraph.SetXMax((double)m_IterLim);
	m_IterGraph.SetYMinGrid(false, true, QColor(100,100,100), 2, 1, 4);

	for (i=0; i<=ia; i++)
	{
		if(m_bCancel) 
		{
			str = tr("Analysis cancelled on user request....")+"\n";
			UpdateOutput(str);
			break;
		}
		m_IterGraph.SetYMin(0.0);
		m_IterGraph.SetYMax(0.5);
		pCurve->ResetCurve();
		m_State=0;
		if(m_pWing->m_bInitCalc)m_pWing->LLTSetLinearSolution();
		m_pWing->LLTInitCl();//with new angle...
		
		str = QString(tr("Calculating QInf = %1... ")).arg(CWing::s_QInfLLT,6,'f',2);
		UpdateOutput(str);
		iter = Iterate();

		if(iter<0)
		{
			//unconverged
			m_bError = true;
			m_bWarning = true;
			str = QString("\n");
			UpdateOutput(str);
			m_pWing->m_bInitCalc = true;
		}
		else if (m_bSkip)
		{
			m_bSkip = false;
			m_pWing->m_bInitCalc = true;
		}
		else if (iter<m_IterLim  && !m_bCancel)
		{
			//converged, 
			str = QString(tr("    ...converged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_pWing->LLTComputeWing();// generates wing results, 
			if (m_pWing->m_bWingOut) m_bWarning = true;
			pMiarex->AddWOpp(m_pWing->m_bWingOut);// Adds WOpp point and adds result to polar
/*			if(m_pWing->m_bWingOut)
			{
				str = QString("\n");
				UpdateOutput(str);
			}*/
			m_pWing->m_bInitCalc = false;
		}
		else
		{
			if (m_pWing->m_bWingOut) m_bWarning = true;
			m_bError = true;
			str = QString(tr("    ...unconverged after %1 iterations")+"\n").arg(iter);
			UpdateOutput(str);
			m_pWing->m_bInitCalc = true;
		}
		CWing::s_QInfLLT += m_AlphaDelta;

	}
	return true;
}




