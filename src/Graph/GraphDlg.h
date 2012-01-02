/****************************************************************************

	GraphDlg  Classes
	Copyright (C) 2008 Andre Deperrois xflr5@yahoo.com

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

#ifndef GRAPHDLG_H
#define GRAPHDLG_H

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>

#include "../Graph/Graph.h"
#include "../Misc/FloatEdit.h"
#include "../Misc/ColorButton.h"
#include "../Misc/LineButton.h"

class GraphDlg : public QDialog
{
	Q_OBJECT
	friend class Graph;


public:
	GraphDlg();
	void SetParams();

private slots:
	void OnOK();
	void OnApply();
	void OnTitleFont();
	void OnLabelFont();
	void OnTitleColor();
	void OnLabelColor();
	void OnRestoreParams();
	void OnAutoX();
	void OnAutoY();
	void OnAutoMinGrid();
	void OnAxisStyle();
	void OnXMajGridStyle();
	void OnXMinGridStyle();
	void OnYMajGridStyle();
	void OnYMinGridStyle();
	void OnXMajGridShow(int state);
	void OnYMajGridShow(int state);
	void OnXMinGridShow(int state);
	void OnYMinGridShow(int state);
	void OnGraphBorder(int state);
	void OnGraphBackColor();
	void OnBorderStyle();
	void OnYInverted();
	void OnActivePage(int index);
	void OnVariableChanged();

private:
	void keyPressEvent(QKeyEvent *event);
	void SetupLayout();
	void SetButtonColors();
	void SetApplied(bool bApplied);
	void Connect();

	void FillVariableList();

	QTabWidget *m_pTabWidget;
	QWidget *ScalePage, *FontPage, *GridPage;
	QWidget *VariablePage;

	QListWidget *m_pctrlXSel, *m_pctrlYSel;

	QPushButton *OKButton,*CancelButton, *RestoreButton, *ApplyButton;
	QPushButton *m_pctrlTitleButton, *m_pctrlLabelButton;
	QPushButton*m_pctrlTitleClr, *m_pctrlLabelClr;
	QLabel *m_pctrlTitleLabel, *m_pctrlLabelLabel;

	QCheckBox *m_pctrlXAuto, *m_pctrlYAuto, *m_pctrlYInverted;
	FloatEdit *m_pctrlXMin, *m_pctrlXMax, *m_pctrlXOrigin,*m_pctrlXUnit;
	FloatEdit *m_pctrlYMin, *m_pctrlYMax, *m_pctrlYOrigin,*m_pctrlYUnit;

	QCheckBox *m_pctrlXMajGridShow, *m_pctrlYMajGridShow, *m_pctrlXMinGridShow, *m_pctrlYMinGridShow;
	QCheckBox *m_pctrlAutoXMinUnit, *m_pctrlAutoYMinUnit;
	LineButton *m_pctrlAxisStyle, *m_pctrlXMajGridStyle, *m_pctrlYMajGridStyle, *m_pctrlXMinGridStyle, *m_pctrlYMinGridStyle;
	FloatEdit *m_pctrlXMinorUnit, *m_pctrlYMinorUnit;

	QCheckBox *m_pctrlGraphBorder;
	ColorButton *m_pctrlGraphBack;
	LineButton *m_pctrlBorderStyle;

	QFont *m_pTitleFont, *m_pLabelFont;

	bool m_bApplied;

public:
	Graph *m_pGraph, *m_pMemGraph;
	Graph *m_GraphArray[30];
	int m_NGraph;
	int m_XSel, m_YSel;
	int m_iGraphType;
	bool m_bVariableChanged;

	static int s_ActivePage;
	static void *s_pMainFrame;
};

#endif // GRAPHDLG_H
