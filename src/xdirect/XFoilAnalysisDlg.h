/****************************************************************************

    XFoilAnalysisDlg Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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

#ifndef XFOILANALYSISDLG_H
#define XFOILANALYSISDLG_H

#include <QDialog>
#include <QEventLoop>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include <QPoint>
#include "../graph/QGraph.h"
#include "../graph/GraphWidget.h"
#include "XFoil.h"

class XFoilAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class MainFrame;
	friend class QXDirect;

public:
	XFoilAnalysisDlg();

	void InitDialog();

private slots:
	void OnCancelAnalysis();
	void OnSkipPoint();

signals:
	void SkipPoint();
	void CancelAnalysis();

private:
	void AddOpPoint();
//	void paintEvent ( QPaintEvent * event );
//	void processEvents(QEventLoop::ProcessEventsFlags flags);
	void accept();
	void reject();

	void ResetCurves();
	void SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha);
	void SetCl(double ClMin, double ClMax, double DeltaCl);
	void SetRe(double ReMin, double ReMax, double DeltaRe);
	void SetFileHeader();
	void SetupLayout();
	void StartAnalysis();
	void UpdateView();
	void UpdateOutput(QString &strong);
	void WriteString(QString &strong);

	bool AlphaLoop();
	bool Iterate();
	bool ReLoop();

	GraphWidget * m_pGraphWidget;
	QTextEdit *m_pctrlTextOutput;
	QPushButton* m_pctrlCancel, *m_pctrlSkip;

	bool m_bAlpha;
	bool m_bType4;
	bool m_bSequence;
	bool m_bSkip, m_bExit, m_bFinished, m_bAutoInitBL;

	int m_Iterations, m_IterLim;

	double m_AlphaMin, m_AlphaMax, m_DeltaAlpha;
	double m_ClMin, m_ClMax, m_DeltaCl;
	double m_ReMin, m_ReMax, m_DeltaRe;

	static void *s_pXDirect;
	static void *s_pMainFrame;

	QTimer *EventTimer;
	QFile *m_pXFile;
	QPoint m_LegendPlace;
	QRect m_ViscRect;
	QString m_FoilName;
	QGraph m_RmsGraph;

	XFoil* m_pXFoil;
	

};

#endif
