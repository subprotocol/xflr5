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
#include "../graph/QGraph.h"
#include "../graph/GraphWidget.h"
#include "../objects/Foil.h"
#include "../objects/Polar.h"
#include "../misc/FloatEdit.h"

class BatchDlg : public QDialog
{
	Q_OBJECT
	friend class QXDirect;
	friend class MainFrame;
	friend class QReListDlg;
public:
    BatchDlg(QWidget *pParent);

	void SetupLayout();
	void InitDialog();

private:
	void keyPressEvent(QKeyEvent *event);

	void AddOpPoint();
	void AlphaLoop();
	void Analysis2();
	void Analysis3();
	void CleanUp();
	CPolar* CreatePolar(double Spec, double Mach, double NCrit);
	bool Iterate();
	void OutputIter(int iter);
	void ReadParams();
	void ReLoop();
	void ResetCurves();
	void SetFileHeader();
	void SetPlrName(CPolar *pPolar);
	void StartAnalysis();
	void UpdateGraph();
	void UpdateOutput(QString &str);
	void WriteString(QString &strong);


private slots:
	void OnSpecChanged();
	void OnAnalyze();
	void OnRange();
	void OnAcl();
	void OnEditReList();
	void OnPolarType();
	void OnClose();
	void OnFromZero(int state);
	void OnSkipPoint();
	void OnSkipPolar();
	void OnInitBL(int state);
	void OnFoilList();
	void OnFoilSelectionType();

protected:
	QRadioButton * m_pctrlFoil1, * m_pctrlFoil2;
	QPushButton *m_pctrlFoilList;
	QRadioButton *m_rbtype1, *m_rbtype2, *m_rbtype3, *m_rbtype4, *m_rbRange1, *m_rbRange2;
	QRadioButton *m_rbspec1, *m_rbspec2;
	QPushButton *m_pctrlEditList;
	FloatEdit *m_pctrlReMin, *m_pctrlReMax, *m_pctrlReDelta, *m_pctrlMach;
	FloatEdit *m_pctrlSpecMin, *m_pctrlSpecMax, *m_pctrlSpecDelta;
	FloatEdit *m_pctrlNCrit, *m_pctrlXTopTr, *m_pctrlXBotTr;
	QLabel *m_pctrlSpecVar;
	QLabel *m_pctrlMaType, *m_pctrlReType;
	QCheckBox *m_pctrlInitBL, *m_pctrlFromZero, *m_pctrlStoreOpp;
	QPushButton *m_pctrlSkipOpp, *m_pctrlSkipPolar;
	QPushButton *m_pctrlClose, *m_pctrlAnalyze;
	QTextEdit *m_pctrlTextOutput;
	GraphWidget *m_pctrlGraphOutput;

	static bool s_bStoreOpp;
	static void* s_pXFoil;
	static void* s_pXDirect;
	static void * s_pMainFrame;
	static bool s_bCurrentFoil;

	double	m_Mach;

	double	m_ReMin, m_ReMax, m_ReInc;
	double	m_SpMin, m_SpMax, m_SpInc;

	double m_AlphaMin, m_AlphaMax, m_AlphaInc;
	double m_ClMin, m_ClMax, m_ClInc;

	bool m_bOutput;
	bool m_bAlpha;
	bool m_bFromList;
	bool m_bFromZero;
	bool m_bInitBL;
	bool m_bCancel, m_bSkipPoint, m_bSkipPolar;
	bool m_bIsRunning;

	enumPolarType m_PolarType;

	int m_Iterations;
	int m_IterLim;
	double m_NCrit;
	double m_XTopTr;
	double m_XBotTr;
	double *m_ReList;
	double *m_MachList;
	double *m_NCritList;

	int m_NRe;

	QFile *m_pXFile;

	CFoil *m_pFoil;
	CPolar *m_pCurPolar;

	QFont m_RFont;
	QFont m_TitleFont;

	QStringList m_FoilList;
	QString m_PlrName;

	QGraph m_RmsGraph;
	QRect m_IterRect;
	CCurve* m_pIterCurve;


};

#endif // BATCHDLG_H
