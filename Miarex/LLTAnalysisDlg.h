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

#ifndef LLTANALYSISDLG_H
#define LLTANALYSISDLG_H

#include <QDialog>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPoint>
#include "../Graph/QGraph.h"
#include "../Graph/GraphWidget.h"
#include "../Objects/WPolar.h"
#include "../Objects/Wing.h"

class LLTAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class CWing;

public:
	LLTAnalysisDlg();

	void InitDialog();

private slots:
	void OnCancelAnalysis();
	void OnSkipPoint();


private:
	void accept();
	void reject();

	void ResetCurves();
	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);

	void SetFileHeader();
	void SetupLayout();
	void StartAnalysis();
	void UpdateView();
	void UpdateOutput(QString &strong);
	void WriteString(QString &strong);

	bool AlphaLoop();
	bool ReLoop();
	int Iterate();

	GraphWidget * m_pGraphWidget;
	QTextEdit *m_pctrlTextOutput;

	bool m_bAlpha;
	bool m_bSequence;
	bool m_bSkip, m_bExit, m_bCancel, m_bWarning;

	int m_State;
	int m_Iterations, m_IterLim;

	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_ReMin, m_ReMax, m_ReDelta;

	void *m_pMiarex;
	void *m_pMainFrame;

	QFile *m_pXFile;
	QPoint m_LegendPlace;
	QRect m_ViscRect;

	QGraph m_IterGraph;

	CWing   *m_pWing;
	CWPolar *m_pWPolar;

};

#endif
