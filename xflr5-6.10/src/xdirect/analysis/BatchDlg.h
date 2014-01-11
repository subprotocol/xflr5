/****************************************************************************

	BatchDlg Class
	Copyright (C) 2003-2008 Andre Deperrois adeperrois@xflr5.com

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

#ifndef BATCHDLG_H
#define BATCHDLG_H

#include <QDialog>
#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QTextEdit>
#include "../../graph/QGraph.h"
#include "../../graph/GraphWidget.h"
#include "../../objects/Foil.h"
#include "../../objects/Polar.h"
#include "../../misc/DoubleEdit.h"
#include "XFoilTask.h"


class BatchDlg : public QDialog
{
	Q_OBJECT
	friend class QXDirect;
	friend class MainFrame;
	friend class QReListDlg;
public:
	BatchDlg(QWidget *pParent=NULL);

private:
	void keyPressEvent(QKeyEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	void AlphaLoop();
	void CleanUp();
	Polar* CreatePolar(double Spec, double Mach, double NCrit);
	void SetupLayout();
	void InitDialog();
	void ReadParams();
	void ReLoop();
	void ResetCurves();
	void SetFileHeader();
	void SetPlrName(Polar *pPolar);
	void Analyze();
	void UpdateGraph();
	void OutputMsg(QString &msg);


private slots:
	void OnAnalyze();
	void OnRange();
	void OnAcl();
	void OnEditReList();
	void OnPolarType();
	void OnClose();
	void OnSkipPoint();
	void OnSkipPolar();
	void OnFoilList();
	void OnFoilSelectionType();
	void OnProgress();

private:
    static QPoint s_Position;   /**< the position on the client area of the dialog's topleft corner */

	QRadioButton * m_pctrlFoil1, * m_pctrlFoil2;
	QPushButton *m_pctrlFoilList;
	QRadioButton *m_rbtype1, *m_rbtype2, *m_rbtype3, *m_rbtype4;
	QRadioButton *m_rbRange1, *m_rbRange2;
	QRadioButton *m_rbspec1, *m_rbspec2;
	QPushButton *m_pctrlEditList;
	DoubleEdit *m_pctrlReMin, *m_pctrlReMax, *m_pctrlReDelta, *m_pctrlMach;
	DoubleEdit *m_pctrlSpecMin, *m_pctrlSpecMax, *m_pctrlSpecDelta;
	DoubleEdit *m_pctrlNCrit, *m_pctrlXTopTr, *m_pctrlXBotTr;
	QLabel *m_pctrlSpecVar;
	QLabel *m_pctrlMaType, *m_pctrlReType;
	QCheckBox *m_pctrlInitBL, *m_pctrlFromZero, *m_pctrlStoreOpp;
	QPushButton *m_pctrlSkipOpp, *m_pctrlSkipPolar;
	QPushButton *m_pctrlClose, *m_pctrlAnalyze;
	QTextEdit *m_pctrlTextOutput;
	GraphWidget *m_pctrlGraphOutput;

	static void* s_pXDirect;
	static bool s_bCurrentFoil;
	enumPolarType m_PolarType;

	double m_Mach;

	double m_ReMin, m_ReMax, m_ReInc;
	double m_SpMin, m_SpMax, m_SpInc;

	double m_AlphaMin, m_AlphaMax, m_AlphaInc;
	double m_ClMin, m_ClMax, m_ClInc;

	double m_ACrit;
	double m_XTop;
	double m_XBot;

	bool m_bOutput;
	bool m_bAlpha;
	bool m_bFromList;
	bool m_bFromZero;
	bool m_bInitBL;
	bool m_bCancel;
	bool m_bIsRunning;
	bool m_bErrors;

	QFile *m_pXFile;

	Foil *m_pFoil;
	Polar *m_pCurPolar;

	QFont m_RFont;
	QFont m_TitleFont;

	QStringList m_FoilList;
	QString m_PlrName;

	QGraph *m_pRmsGraph;
	QRect m_IterRect;
	Curve* m_pIterCurve;

	XFoilTask *m_pXFoilTask;

};

#endif // BATCHDLG_H
