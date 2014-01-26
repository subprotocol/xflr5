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
 

/**
 *@file
 *
 * This file defines the PanelAnalysisDlg class, which is used to perform VLM and 3D-Panel analysis
 *
 */

#ifndef PANELANALYSISDLG_H
#define PANELANALYSISDLG_H

#include <QDialog>
#include <QProgressBar>
#include <QGLWidget>
#include <QString>
#include <QFile>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include "../../objects/WPolar.h"
#include "../../objects/Plane.h"
#include "../../objects/CVector.h"
#include "PanelAnalysis.h"

#define VLMMAXRHS 100

/**
 *@class PanelAnalysisDlg
 *@brief The class is used to handle the VLM and 3D-panel analysis and to provide the user interface for the analysis
 
 * For convenience, the class is derived from the QDialog class.
 * The dialog interface allows the user to visualize the progress of the analysis, and to interrupt it as necessary.

 */
class PanelAnalysisDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;

public:
	PanelAnalysisDlg(QWidget *pParent, PanelAnalysis *pPanelAnalysis);
	~PanelAnalysisDlg();

	bool InitDialog();

private slots:
	void OnCancelAnalysis();
	void OnProgress();


private:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void SetupLayout();
	void Analyze();

	void UpdateView();
	
private:
	QTextEdit *m_pctrlTextOutput;
	QPushButton *m_pctrlCancel;
	QProgressBar *m_pctrlProgress;

	static QPoint s_Position;        /**< the position on the client area of he dialog's topleft corner */

	bool m_bIsFinished;         /**< true if the analysis is completed */
	bool m_bPointOut;           /** true if an interpolation was outside the min or max Cl */
	bool m_bSequence;           /** true if the calculation is should be performed for a range of aoa */

	double m_Progress;          /**< A measure of the progress of the analysis, used to provide feedback to the user */
	
	QString m_strOut;


public:

	bool m_bTrace;

	PanelAnalysis *m_pPanelAnalysis; /**< a pointer to the one and only instance of the PanelAnalysis class */
};

#endif // PANELANALYSISDLG_H














