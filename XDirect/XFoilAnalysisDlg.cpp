/****************************************************************************

    XFoilAnalysisDlg Class
	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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

#include "XFoilAnalysisDlg.h"
#include "XDirect.h"
#include "../MainFrame.h"

XFoilAnalysisDlg::XFoilAnalysisDlg()
{

	setWindowTitle(tr("XFoil Analysis"));
	SetupLayout();

	m_RmsGraph.SetXTitle("Iter");
	m_RmsGraph.SetYTitle("");//Change from bl newton system solution

	m_RmsGraph.SetAuto(true);

	m_RmsGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_RmsGraph.SetYMajGrid(true, QColor(120,120,120),2,1);

	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax(50);
	m_RmsGraph.SetYMin(0.0);
	m_RmsGraph.SetYMax(1.0);
	m_RmsGraph.SetType(1);

	m_RmsGraph.SetXTitle("abs");
	m_RmsGraph.SetYTitle("rms");

	m_RmsGraph.SetMargin(.1);

	m_bSkip       = false;
	m_bExit       = false;
	m_bCalc       = false;
	m_bAutoInitBL = true;

	m_pXFile = NULL;


	m_Iterations =  0;
	m_IterLim    = 20;
	m_bType4     = false;
	m_bSequence  = false;
	m_bAlpha     = true;

	m_LegendPlace.rx() = 0;
	m_LegendPlace.ry() = 0;

	m_AlphaMin   = 0.0;
	m_AlphaMax   = 1.0;
	m_DeltaAlpha = 0.5;
	m_ClMin      = 0.0;
	m_ClMax      = 1.0;
	m_DeltaCl    = 0.1;
	m_ReMin      =  10000.0;
	m_ReMax      = 100000.0;
	m_DeltaRe    =  10000.0;

	m_pXFoil       = NULL;
	m_pXFile       = NULL;
	m_pXDirect     = NULL;
	m_pMainFrame   = NULL;

}
void XFoilAnalysisDlg::SetupLayout()
{

	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pGraphWidget = new GraphWidget;
	m_pGraphWidget->setMinimumHeight(350);
	m_pGraphWidget->setMinimumWidth(100);
	m_pGraphWidget->m_pGraph = &m_RmsGraph;

	QHBoxLayout *GraphLayout = new QHBoxLayout;
	GraphLayout->addWidget(m_pGraphWidget);

	QPushButton *skipButton   = new QPushButton(tr("Skip"));
	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	QPushButton *closeButton  = new QPushButton(tr("Close"));

	connect(skipButton,   SIGNAL(clicked()), this, SLOT(OnSkipPoint()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));
	connect(closeButton,  SIGNAL(clicked()), this, SLOT(close()));


	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(skipButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(cancelButton);
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	buttonsLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addLayout(GraphLayout);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
}

void XFoilAnalysisDlg::reject()
{
	m_bSkip = true;
	m_bExit = true;
	m_pXFile->close();

	QDialog::reject();
}

void XFoilAnalysisDlg::accept()
{
	m_bSkip = true;
	m_bExit = true;
	m_pXFile->close();

	QDialog::accept();
}

void XFoilAnalysisDlg::AddOpPoint()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->AddOpPoint();
	if(pXDirect->m_bPolar)
	{
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	m_RmsGraph.ResetYLimits();
}


bool XFoilAnalysisDlg::AlphaLoop()
{
	QString str;
	double alfa;
	double pi = 3.141592654;

	int ia, total;

	if(m_AlphaMax< m_AlphaMin) m_DeltaAlpha = -fabs(m_DeltaAlpha);
	if(m_ClMax< m_ClMin)       m_DeltaCl = -fabs(m_DeltaCl);

	if      ( m_bAlpha && fabs(m_DeltaAlpha)<1.e-3) total = 0;
	else if (!m_bAlpha && fabs(m_DeltaCl)<1.e-4)    total = 0;
	else
	{
		if(m_bAlpha) total=int((m_AlphaMax-m_AlphaMin)*1.0001/m_DeltaAlpha);//*1.0001 to make sure upper limit is included
		else         total=int((m_ClMax-m_ClMin)*1.0001/m_DeltaCl);//*1.0001 to make sure upper limit is included
	}

	total = abs(total);

	if(!m_bSequence) total = 0;

	QString strange;

	for (ia=0; ia<=total; ia++)
	{
		if(!m_bExit)
		{
			if(m_bAlpha)
			{
				alfa = m_AlphaMin+ia*m_DeltaAlpha;
				m_pXFoil->alfa = alfa*pi/180.0;
				m_pXFoil->lalfa = true;
				m_pXFoil->qinf = 1.0;
				str = QString("\r\n\r\nAlpha = %1\r\n").arg(alfa,5,'f',2);
				WriteString(str);
				strange = QString("Alfa = %1 ........ ").arg(m_pXFoil->alfa*180.0/pi,7,'f',2);
				UpdateOutput(strange);

				// here we go !
				if (!m_pXFoil->specal())
				{
					str = "Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
					WriteString(str);
					m_bExit = true;
					return FALSE;
				}
			}
			else
			{
				m_pXFoil->lalfa = false;
				m_pXFoil->alfa = 0.0;
				m_pXFoil->qinf = 1.0;
				m_pXFoil->clspec = m_ClMin+ia*m_DeltaCl;
				str = QString("\r\n\r\nCl = %1\r\n").arg(m_pXFoil->clspec, 10,'f',3);
				WriteString(str);

				strange = QString("Cl = %1 ........ ").arg(m_pXFoil->clspec,7,'f',2);
				UpdateOutput(strange);

				if(!m_pXFoil->speccl())
				{
					str = "Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
					WriteString(str);
					m_bExit = true;
					return false;
				}
			}
			if ((double)abs(m_pXFoil->alfa-m_pXFoil->awake) > 0.00001)
				m_pXFoil->lwake  = false;
			if ((double)abs(m_pXFoil->alfa-m_pXFoil->avisc) > 0.00001)
				m_pXFoil->lvconv = false;
			if ((double)abs(m_pXFoil->minf-m_pXFoil->mvisc) > 0.00001)
				m_pXFoil->lvconv = false;

			m_pXFoil->lwake = false;
			m_pXFoil->lvconv = false;

			m_bSkip = false;

			while(!Iterate()){}

			qApp->processEvents();
			ResetCurves();
			m_Iterations = 0;
			AddOpPoint();// only if converged ???
		}
		else
		{
			break;
		}
	}
	return true;
}



void XFoilAnalysisDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName = QDir::tempPath() + "/QFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	m_pXFoil->pXFile = m_pXFile;

	SetFileHeader();

	m_Iterations = 0;

	QString str;
	m_RmsGraph.AddCurve();
	m_RmsGraph.AddCurve();
	str = "rms";
	m_RmsGraph.GetCurve(0)->SetTitle(str);
	str = "max";
	m_RmsGraph.GetCurve(1)->SetTitle(str);
        m_RmsGraph.GetCurve(1)->SetStyle(0);

	m_RmsGraph.SetAutoX(true);
	m_RmsGraph.SetXMin(0.0);
	m_RmsGraph.SetXMax((double)m_IterLim);
	m_RmsGraph.SetX0(0.0);
	m_RmsGraph.SetXUnit((int)(m_IterLim/10.0));

	m_RmsGraph.SetAutoY(true);
	m_RmsGraph.SetY0(0.0);
	m_RmsGraph.SetYMin(0.0);
	m_RmsGraph.SetYMax(1.0);

	m_RmsGraph.SetMargin(40);

	if(pMainFrame) m_RmsGraph.CopySettings(&pMainFrame->m_RefGraph, false);
}



bool XFoilAnalysisDlg::Iterate()
{
	QString str, strange;
	CCurve *pCurve0 = m_RmsGraph.GetCurve(0);
	CCurve *pCurve1 = m_RmsGraph.GetCurve(1);

	str="   Initializing viscous analysis ...\r\n";
	WriteString(str);

	if(!m_pXFoil->viscal())
	{
		m_pXFoil->lvconv = false;//point is unconverged
		str = "CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n";
		WriteString(str);
		m_bExit = true;
		return true;// to exit loop
		}

	str="   Solving BL system ...\r\n";
	WriteString(str);


	while(m_Iterations<m_IterLim  && !m_pXFoil->lvconv && !m_bSkip)
	{
		qApp->processEvents();
		str= QString("   Iteration %1 ... \r\n").arg(m_Iterations);

		WriteString(str);

		if(m_pXFoil->ViscousIter())
		{
			if(pCurve0) pCurve0->AddPoint((double)m_Iterations, m_pXFoil->rmsbl);
			if(pCurve1) pCurve1->AddPoint((double)m_Iterations, m_pXFoil->rmxbl);
			m_Iterations++;
		}
		else
		{
			m_Iterations = m_IterLim;
		}
		UpdateView();
	}

	if(m_bSkip)
	{
		m_pXFoil->lblini = false;
		m_pXFoil->lipan = false;
		return true;
	}

	if(!m_pXFoil->ViscalEnd())
	{
		m_pXFoil->lvconv = false;//point is unconverged
		str = "CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n";
		WriteString(str);
		m_bExit = true;
		m_pXFoil->lblini = false;
		m_pXFoil->lipan = false;
		return true;// to exit loop
	}
	m_bCalc = true;

	if(m_Iterations>=m_IterLim && !m_pXFoil->lvconv)
	{
		strange = QString("unconverged after %1 iterations\n").arg(m_Iterations);
		UpdateOutput(strange);
		str = "--------- Unconverged -----------\r\n";
		WriteString(str);
		UpdateView();

		m_pXFoil->fcpmin();// Is it of any use ?
		if(m_bAutoInitBL)
		{
			m_pXFoil->lblini = false;
			m_pXFoil->lipan = false;
		}
		return true;
	}
	if(!m_pXFoil->lvconv)
	{
		m_pXFoil->fcpmin();// Is it of any use ?
		return false;
	}
	else
	{
		//converged at last
		strange = QString("converged after %1 iterations\n").arg(m_Iterations);
		UpdateOutput(strange);
		m_pXFoil->fcpmin();
		return true;
	}
}

void XFoilAnalysisDlg::OnCancelAnalysis()
{
	m_bSkip = true;
	m_bExit = true;
}


void XFoilAnalysisDlg::OnSkipPoint()
{
	m_bSkip = true;
}


bool XFoilAnalysisDlg::ReLoop()
{
//	double alphadeg;
//	QMutexLocker locker(&mutex);
	QString str;
	int ia;
	double Re;

	if(m_ReMax< m_ReMin) m_DeltaRe = -fabs(m_DeltaRe);

	int total=int((m_ReMax*1.0001-m_ReMin)/m_DeltaRe);//*1.0001 to make sure upper limit is included

	total = abs(total);
	QString strange;

	for (ia=0; ia<=total; ia++)
	{
		if(!m_bExit)
		{
			Re = m_ReMin+ia*m_DeltaRe;
			strange =QString("Re = %1 ........ ").arg(Re,0,'f',0);
			UpdateOutput(strange);
			m_pXFoil->reinf1 = Re;
			m_pXFoil->lalfa = true;
			m_pXFoil->qinf = 1.0;
			str = QString("\r\n\r\nRe = %1\r\n").arg(Re,8,'f',0);
			WriteString(str);

			// here we go !
			if (!m_pXFoil->specal())
			{
				QString str;
				str = "Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ";
				WriteString(str);
				m_bExit = true;
				return FALSE;
			}

			if (fabs(m_pXFoil->alfa-m_pXFoil->awake) > 0.00001)
				m_pXFoil->lwake  = false;
			if (fabs(m_pXFoil->alfa-m_pXFoil->avisc) > 0.00001)
				m_pXFoil->lvconv = false;
			if (fabs(m_pXFoil->minf-m_pXFoil->mvisc) > 0.00001)
				m_pXFoil->lvconv = false;

			m_pXFoil->lwake = false;
			m_pXFoil->lvconv = false;

			m_bSkip = false;

			while(!Iterate()){}

			qApp->processEvents();

			ResetCurves();
			m_Iterations = 0;

			AddOpPoint();// only if converged ???
		}
		else
		{
			break;
		}
	}
	return TRUE;
}




void XFoilAnalysisDlg::ResetCurves()
{
	CCurve*pCurve;
	pCurve = m_RmsGraph.GetCurve(0);
	if(pCurve) pCurve->ResetCurve();
	pCurve = m_RmsGraph.GetCurve(1);
	if(pCurve) pCurve->ResetCurve();

}

void XFoilAnalysisDlg::SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha)
{
	m_AlphaMin = AlphaMin;
	m_AlphaMax = AlphaMax;
	m_DeltaAlpha = DeltaAlpha;
}

void XFoilAnalysisDlg::SetCl(double ClMin, double ClMax, double DeltaCl)
{
	m_ClMin = ClMin;
	m_ClMax = ClMax;
	m_DeltaCl = DeltaCl;
}

void XFoilAnalysisDlg::SetRe(double ReMin, double ReMax, double DeltaRe)
{
	m_ReMin = ReMin;
	m_ReMax = ReMax;
	m_DeltaRe = DeltaRe;
}


void XFoilAnalysisDlg::SetFileHeader()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
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

//	m_pXFile->close();
}


void XFoilAnalysisDlg::StartAnalysis()
{
	//all set to launch the analysis
	if (!m_bType4)
	{
		AlphaLoop() ;
	}
	else
	{
		ReLoop();
	}

	m_bSkip   = false;
	m_bExit   = false;
}


void XFoilAnalysisDlg::UpdateView()
{
	m_pGraphWidget->update();
	repaint();
}


void XFoilAnalysisDlg::UpdateOutput(QString &strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
}
void XFoilAnalysisDlg::WriteString(QString &strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}



