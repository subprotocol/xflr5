/****************************************************************************

	PanelAnalysisDlg Class
	Copyright (C) 2009-2014 Andre Deperrois adeperrois@xflr5.com

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

#include <QtDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTimer>
#include <QDir>
#include <QKeyEvent>
#include <math.h>

#include "PanelAnalysisDlg.h"
#include "../../misc/Settings.h"
#include "../../globals.h"
#include "../../objects/CVector.h"
#include "../../misc/Units.h"
#include "../Objects3D.h"

QPoint PanelAnalysisDlg::s_Position;


/**
* The public constructor
*/
PanelAnalysisDlg::PanelAnalysisDlg(QWidget *pParent, PanelAnalysis *pPanelAnalysis) : QDialog(pParent)
{
	setWindowTitle(tr("3D Panel Analysis"));
	SetupLayout();
	m_pPanelAnalysis = pPanelAnalysis;
}


/**
 * The public destructor.
 *
 */
PanelAnalysisDlg::~PanelAnalysisDlg()
{
}


/**
*Initializes the dialog and the analysis
*/
bool PanelAnalysisDlg::InitDialog()
{
	m_pctrlTextOutput->setFont(Settings::s_TableFont);
	m_Progress = 0.0;
	m_pctrlProgress->setValue(m_Progress);

	m_pctrlTextOutput->clear();

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);

	return true;
}


/** Overrides the keyPressEvent sent by Qt */
void PanelAnalysisDlg::keyPressEvent(QKeyEvent *event)
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


/** The user has requested to cancel the on-going analysis*/
void PanelAnalysisDlg::OnCancelAnalysis()
{
	PanelAnalysis::s_bCancel = true;
	if(m_bIsFinished) { PanelAnalysis::s_bCancel = false; done(1); }
}



/**Updates the progress of the analysis in the slider widget */
void PanelAnalysisDlg::OnProgress()
{
	m_pctrlProgress->setValue((int)PanelAnalysis::s_Progress);
	if(m_pPanelAnalysis->m_OutMessage.length())
	{
		m_pctrlTextOutput->insertPlainText(m_pPanelAnalysis->m_OutMessage);
		m_pctrlTextOutput->textCursor().movePosition(QTextCursor::End);
		m_pctrlTextOutput->ensureCursorVisible();
	}
	m_pPanelAnalysis->m_OutMessage.clear();
}



/**  Sets up the GUI */
void PanelAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/2);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit(this);
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
//	m_pctrlTextOutput->setFont(Settings::m_TableFont);

	m_pctrlProgress = new QProgressBar(this);
	m_pctrlProgress->setOrientation(Qt::Horizontal);
	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);
	m_pctrlProgress->setValue(0);

	m_pctrlCancel = new QPushButton(tr("Cancel"),this);
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *pButtonLayout = new QHBoxLayout;
	{
		pButtonLayout->addStretch(1);
		pButtonLayout->addWidget(m_pctrlCancel);
		pButtonLayout->addStretch(1);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addWidget(m_pctrlTextOutput);
		pMainLayout->addWidget(m_pctrlProgress);
		pMainLayout->addLayout(pButtonLayout);
	}
	setLayout(pMainLayout);
}





/**
* Starts the panel or VLM analysis
*
* Method applied from v6.00 onwards:
* 
* First case :
*    If the analysis is for a wing and not a plane, the full 3D panel method is applied
*    and the wing is modelled as a thick surface
*    The method is strictly the one described in NASA TN 4023
*    The boundary condition is of the Dirichlet type, which has proved more convincing than the Neumann BC for full 3D panel methods
*
* Second case :
*    If the analysis is for a plane, the full 3D method is not applicable since the 
*    junctions between wing and body, or between fin and elevator, cannot be adequately 
*    represented as closed surfaces. This would require a 3D CAD programe. 
*    Therefore, in this case, the wings are modelled as thin surfaces.
*    Trial tests using the method of NASA TN4023 have not been conclusive. With a uniform doublet
*    distribution and a boundary condition applied at the panel's centroid, the results 
*    are less convincing than with VLM.
*    Therefore in this case, the VLM1 method is applied to the thin surfaces, and the 3D-panel method
*    is applied to the body.
*    Last consideration : since the potential of a straight vortex line requires a lot of computations, 
*    the Neumann type BC is applied to the body panels, rather than the Dirichlet type BC
*/
void PanelAnalysisDlg::Analyze()
{
	if(!m_pPanelAnalysis) return;

	qApp->processEvents();

	QString strong;
	m_pctrlCancel->setText(tr("Cancel"));
	m_bIsFinished = false;

	m_pctrlProgress->setRange(0, PanelAnalysis::s_TotalTime);

	QString len;
	Units::getLengthUnitLabel(len);

	QTimer *pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnProgress()));
	pTimer->setInterval(250);
	pTimer->start();

	qApp->processEvents();

	m_pPanelAnalysis->initializeAnalysis();
	m_pPanelAnalysis->loop();

	if(PlaneOpp::s_bStoreOpps)
	{
		for(int iPOpp=0; iPOpp<m_pPanelAnalysis->m_PlaneOppList.size(); iPOpp++)
		{
			//add the data to the polar object
			PlaneOpp *pPOpp = m_pPanelAnalysis->m_PlaneOppList.at(iPOpp);
			if(PlaneOpp::s_bKeepOutOpps || !pPOpp->isOut())
				Objects3D::InsertPOpp(pPOpp);
		}
	}
	pTimer->stop();


	m_bIsFinished = true;

	if (!PanelAnalysis::s_bCancel && !PanelAnalysis::s_bWarning)
		strong = "\n"+tr("Panel Analysis completed successfully")+"\n";
	else if (PanelAnalysis::s_bWarning)
		strong = "\n"+tr("Panel Analysis completed ... Errors encountered")+"\n";

	m_pPanelAnalysis->traceLog(strong);
	OnProgress();

	QString FileName = QDir::tempPath() + "/XFLR5.log";
	QFile *pXFile = new QFile(FileName);
	if(pXFile->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream outstream(pXFile);
		outstream << "\n";
		outstream << VERSIONNAME;
		outstream << "\n";
		QDateTime dt = QDateTime::currentDateTime();
		QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
		outstream << str<<"\n\n";

		outstream << m_pctrlTextOutput->toPlainText();
		outstream.flush();
		pXFile->close();
		delete pXFile;
	}

	m_pctrlCancel->setText(tr("Close"));
}


void PanelAnalysisDlg::showEvent(QShowEvent *event)
{
    move(s_Position);
	event->accept();
}

void PanelAnalysisDlg::hideEvent(QHideEvent *event)
{
    s_Position = pos();
	event->accept();
}
