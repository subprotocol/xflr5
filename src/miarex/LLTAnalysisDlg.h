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


/**
 *@file
 *
 * This file contains the LLTAnalysisDlg class, which is used to perform LLT analysis
 *
 */

#ifndef LLTANALYSISDLG_H
#define LLTANALYSISDLG_H

#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPoint>
#include "LLTAnalysis.h"
#include "../graph/QGraph.h"
#include "../graph/GraphWidget.h"
#include "../objects/WPolar.h"
#include "../objects/Wing.h"


/**
 *@class LLTAnalysisDlg
 *@brief The class is used to launch the LLT and to manage the progress of the analysis.
 
 It successively :
  - creates a single instance of the LLTAnalysis object, 
  - initializes the data, 
  - launches the analysis
  - displays the progress,
  - stores the results in the OpPoint and Polar objects
  - updates the display in the Miarex view.

 The LLTAnalysis class performs the calculation of a signle OpPoint. The loop over a sequence of aoa, Cl, or Re values
 are performed in the LLAnalysisDlg Class. 
*/
class LLTAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class MainFrame;
	friend class QMiarex;
	friend class Wing;

public:
    LLTAnalysisDlg(QWidget *pParent);

	void InitDialog();
	void UpdateGraph(int x, double y);

private slots:
	void OnCancelAnalysis();
	void OnSkipPoint();


private:
	bool AlphaLoop();
	void keyPressEvent(QKeyEvent *event);
	bool QInfLoop();
	void ResetCurves();
	void SetAlpha(double AlphaMin, double AlphaMax, double AlphaDelta);
	void SetFileHeader();
	void SetupLayout();
	void StartAnalysis();
	void UpdateView();
	void UpdateOutput(QString &strong);
	void WriteString(QString &strong);


	static void *s_pMiarex;     /**< A static pointer to the unique instance of the QMiarex class. */
	static void *s_pMainFrame;  /**< A static pointer to the unique instance of the MainFrame class. */

	double m_AlphaMin;          /**< The starting aoa for the analysis of type 1 & 2 polars */
	double m_AlphaMax;          /**< The ending aoa for the analysis of type 1 & 2 polars */
	double m_AlphaDelta;        /**< The aoa increment for the analysis of type 1 & 2 polars */
	bool m_bCancel;             /**< true if the user has cancelled the analysis */
	bool m_bError;              /**< true if the analysis couldn't converge within the max number of iterations */
	bool m_bExit;               /**< true if the analysis should be interrupted - either skipped or cancelled */
	bool m_bFinished;           /**< true if the analysis is completed, false if it is running */
	bool m_bInitCalc;           /**< true if the data should be initialized prior to the launch of the analysis. 
	                                 If false, the iterations will start with the data from the last calculated point. */
	bool m_bSequence;           /**< true if the analysis should be performed for a range of input values, false if only a single OpPoint calculation is requested */
	bool m_bSkip;               /**< true if the user has requested to skip the on-going OpPoint calculation. The analysis will move on to the calculation of the next OpPoint in sequence */
	bool m_bWarning;            /**< true if one the OpPoints could not be properly interpolated */
	int m_Iterations;           /**< The number of iterations already performed for the calculation of the current OpPoint */
	QGraph m_IterGraph;         /**< The instance of the QGraph object where the progress of the iterations are displayed */
	int m_IterLim;              /**< The maximum number of operations allowed for the calculation of an OpPoint */
	QPoint m_LegendPlace;       /**< The position where the legend should be diplayed in the output graph */
	LLTAnalysis m_LLT;          /**< The unique instance of the LLTAnalysis class, called to perform each OpPOint calculation */
	Wing   *m_pWing;            /**< A pointer to the instance of the Wing object for which the analysis is performed */
	WPolar *m_pWPolar;          /**< A pointer to the instance of the wing WPolar object for which the analysis is performed */
	QFile *m_pXFile;            /**< A pointer to the instance of the output log file */
	double m_ReDelta;           /**< The increment value of the velocity for the analysis of type 4 polars */
	double m_ReMax;             /**< The ending value of the velocity for the analysis of type 4 polars */
	double m_ReMin;             /**< The starting value of the velocity for the analysis of type 4 polars */
	QRect m_ViscRect;           /**< The rectangle in the client area where the graph is displayed */



	//GUI widget variables
	QPushButton *m_pctrlCancel, *m_pctrlSkip;
	GraphWidget * m_pGraphWidget;
	QTextEdit *m_pctrlTextOutput;
};

#endif
