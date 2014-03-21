/****************************************************************************

    XFoilAnalysisDlg Class
	Copyright (C) 2008-2014 Andre Deperrois adeperrois@xflr5.com

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

#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QHBoxLayout>
#include <QtDebug>


#include "XFoilAnalysisDlg.h"
#include "../XDirect.h"
#include "../../misc/Settings.h"

void *XFoilAnalysisDlg::s_pXDirect;
QPoint XFoilAnalysisDlg::s_Position;


XFoilAnalysisDlg::XFoilAnalysisDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("XFoil Analysis"));
	SetupLayout();

	m_pXFoilTask = new XFoilTask;
	m_pXFoilTask->m_pParent = this;

	m_pXFile       = NULL;

	m_pRmsGraph = m_pGraphWidget->graph();

	m_pRmsGraph->SetXTitle(tr("Iter"));
	m_pRmsGraph->SetYTitle("");//Change from BL newton system solution


	m_pRmsGraph->SetXMajGrid(true, QColor(120,120,120),2,1);
	m_pRmsGraph->SetYMajGrid(true, QColor(120,120,120),2,1);

	m_pRmsGraph->SetXMin(0.0);
	m_pRmsGraph->SetXMax(50);
	m_pRmsGraph->SetYMin(0.0);
	m_pRmsGraph->SetYMax(1.0);
	m_pRmsGraph->SetType(1);

	m_pRmsGraph->SetXTitle("abs");
	m_pRmsGraph->SetYTitle("rms");

	m_pRmsGraph->SetAuto(true);
	m_pRmsGraph->SetMargin(40);
	m_pRmsGraph->CopySettings(&Settings::s_RefGraph, false);

	m_bErrors     = false;
	m_bAlpha      = true;

	m_AlphaMin   = 0.0;
	m_AlphaMax   = 1.0;
	m_AlphaDelta = 0.5;
	m_ClMin      = 0.0;
	m_ClMax      = 1.0;
	m_ClDelta    = 0.1;
	m_ReMin      =  10000.0;
	m_ReMax      = 100000.0;
	m_ReDelta    =  10000.0;

}

XFoilAnalysisDlg::~XFoilAnalysisDlg()
{
//	Trace("Destroying XFoilAnalysisDlg");
	if(m_pXFoilTask) delete m_pXFoilTask;
	if(m_pXFile) delete m_pXFile;
}

void XFoilAnalysisDlg::SetupLayout()
{
	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pGraphWidget = new GraphWidget;
	m_pGraphWidget->setMinimumHeight(350);
	m_pGraphWidget->setMinimumWidth(600);
	m_pGraphWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	{
		m_pctrlSkip   = new QPushButton(tr("Skip"));
		m_pctrlCancel = new QPushButton(tr("Cancel"));

		connect(m_pctrlSkip,   SIGNAL(clicked()), this, SLOT(OnSkipPoint()));
		connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));
		buttonsLayout->addStretch(1);
		buttonsLayout->addWidget(m_pctrlSkip);
		buttonsLayout->addStretch(1);
		buttonsLayout->addWidget(m_pctrlCancel);
		buttonsLayout->addStretch(1);
	}

	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		mainLayout->addWidget(m_pctrlTextOutput);
		mainLayout->addWidget(m_pGraphWidget);
		mainLayout->addLayout(buttonsLayout);
		setLayout(mainLayout);
	}
}



void XFoilAnalysisDlg::showEvent(QShowEvent *event)
{
	move(s_Position);
	event->accept();
}


void XFoilAnalysisDlg::hideEvent(QHideEvent *event)
{
	s_Position = pos();
	event->accept();
}



void XFoilAnalysisDlg::InitDialog()
{	
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	m_pXFoilTask->m_OutStream.setDevice(m_pXFile);

	if(m_bAlpha) m_pXFoilTask->setSequence(true,  m_AlphaMin, m_AlphaMax, m_AlphaDelta);
	else         m_pXFoilTask->setSequence(false, m_ClMin, m_ClMax, m_ClDelta);

	m_pXFoilTask->setReRange(m_ReMin, m_ReMax, m_ReDelta);
	m_pXFoilTask->InitializeTask(Foil::curFoil(), Polar::curPolar(),
						    QXDirect::s_bViscous, QXDirect::s_bInitBL, false);


	SetFileHeader();

	QString str;
	m_pRmsGraph->DeleteCurves();
	Curve *pCurve0 = m_pRmsGraph->AddCurve();
	Curve *pCurve1 = m_pRmsGraph->AddCurve();

	str = "rms";
	pCurve0->SetTitle(str);
	str = "max";
	pCurve1->SetTitle(str);
	pCurve1->SetStyle(0);

	m_pRmsGraph->SetXMin(0.0);
	m_pRmsGraph->SetXMax((double)XFoilTask::s_IterLim);
	m_pRmsGraph->SetX0(0.0);
	m_pRmsGraph->SetXUnit((int)(XFoilTask::s_IterLim/5.0));

	m_pRmsGraph->SetY0(0.0);
	m_pRmsGraph->SetYMin(0.0);
	m_pRmsGraph->SetYMax(1.0);

	m_pXFoilTask->setGraphPointers(m_pRmsGraph, pCurve0, pCurve1);

	m_pctrlTextOutput->clear();
}



void XFoilAnalysisDlg::OnCancelAnalysis()
{
	XFoil::s_bCancel= true;
	XFoilTask::s_bCancel = true;

	if(m_pXFoilTask->m_bIsFinished) reject();
}



void XFoilAnalysisDlg::reject()
{
	if(!m_pXFoilTask->m_bIsFinished)
	{
		XFoil::s_bCancel= true;
		XFoilTask::s_bCancel = true;
		return;
	}

	XFoilTask::s_bCancel = true;
	XFoil::s_bCancel = true;
	if(m_pXFile)
	{
		m_pXFoilTask->m_OutStream.flush();
		m_pXFile->close();
	}
	QDialog::reject();
}


void XFoilAnalysisDlg::accept()
{
	XFoilTask::s_bCancel = true;
	XFoil::s_bCancel = true;
	if(m_pXFile)
	{
		m_pXFoilTask->m_OutStream.flush();
		m_pXFile->close();
	}

	QDialog::accept();
}

void XFoilAnalysisDlg::OnSkipPoint()
{
	XFoilTask::s_bSkipOpp= true;
}


void XFoilAnalysisDlg::ResetCurves()
{
	Curve*pCurve;
	pCurve = m_pRmsGraph->GetCurve(0);
	if(pCurve) pCurve->clear();
	pCurve = m_pRmsGraph->GetCurve(1);
	if(pCurve) pCurve->clear();
}


void XFoilAnalysisDlg::SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta)
{
	m_AlphaMin = AlphaMin;
	m_AlphaMax = AlphaMax;
	m_AlphaDelta = AlphaDelta;
}


void XFoilAnalysisDlg::SetCl(double ClMin, double ClMax, double DeltaCl)
{
	m_ClMin = ClMin;
	m_ClMax = ClMax;
	m_ClDelta = DeltaCl;
}


void XFoilAnalysisDlg::SetRe(double ReMin, double ReMax, double DeltaRe)
{
	m_ReMin = ReMin;
	m_ReMax = ReMax;
	m_ReDelta = DeltaRe;
}


void XFoilAnalysisDlg::SetFileHeader()
{
	if(!m_pXFile) return;
	QTextStream out(m_pXFile);

	out << "\n";
	out << VERSIONNAME;
	out << "\n";
	out << Foil::curFoil()->foilName();
	out << "\n";
	if(Polar::curPolar())
	{
		out << Polar::curPolar()->polarName();
		out << "\n";
	}

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";

}


void XFoilAnalysisDlg::Analyze()
{
	m_pctrlCancel->setText(tr("Cancel"));
	m_pctrlSkip->setEnabled(true);

	//all set to launch the analysis

	//create a timer to update the output at regular intervals
	QTimer *pTimer = new QTimer;
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnProgress()));
	pTimer->setInterval(QXDirect::s_TimeUpdateInterval);
	pTimer->start();

	//Launch the task

	m_pXFoilTask->run();

	pTimer->stop();
	delete pTimer;

	OnProgress();
	m_pXFoilTask->m_OutStream.flush();

	m_bErrors = m_pXFoilTask->m_bErrors;
	if(m_bErrors)
	{
		m_pctrlTextOutput->insertPlainText(tr(" ...some points are unconverged"));
		m_pctrlTextOutput->ensureCursorVisible();
	}

	m_pctrlCancel->setText(tr("Close"));
	m_pctrlSkip->setEnabled(false);
	update();

}



void XFoilAnalysisDlg::OnProgress()
{
	if(m_pXFoilTask->m_OutMessage.length())
	{
		m_pctrlTextOutput->insertPlainText(m_pXFoilTask->m_OutMessage);
//		m_pctrlTextOutput->textCursor().movePosition(QTextCursor::End);
		m_pctrlTextOutput->ensureCursorVisible();
	}
	m_pXFoilTask->m_OutMessage.clear();
//	m_pGraphWidget->update();
//	update();

	repaint();

	QXDirect *pXDirect = (QXDirect*)s_pXDirect;
	pXDirect->CreatePolarCurves();
	pXDirect->UpdateView();
}













