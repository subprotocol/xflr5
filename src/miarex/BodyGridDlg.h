/****************************************************************************

	BodyGridDlg Class
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


#ifndef BODYGRIDDLG_H
#define BODYGRIDDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "../misc/FloatEdit.h"
#include "../misc/LineButton.h"

class BodyGridDlg : public QDialog
{
	Q_OBJECT

	friend class GL3dBodyDlg;
	friend class CBody;
	friend class MainFrame;

public:
	BodyGridDlg();

private slots:
	void OnOK();
	void OnGrid();
	void OnGrid2();
	void OnMinGrid();
	void OnMinGrid2();
	void OnLineStyle();
	void OnLine2Style();
	void OnMinLineStyle();
	void OnMinLine2Style();

private:
	void SetupLayout();
	void InitDialog();
	void EnableControls();

private:
	static void *s_pMainFrame;

	QCheckBox *m_pctrlScales;

	QCheckBox *m_pctrlGrid, *m_pctrlMinGrid, *m_pctrlGrid2, *m_pctrlMinGrid2;
	LineButton *m_pctrlLine, *m_pctrlMinLine, *m_pctrlLine2, *m_pctrlMinLine2;
	FloatEdit *m_pctrlUnit, *m_pctrlMinUnit, *m_pctrlUnit2, *m_pctrlMinUnit2;
	QLabel *m_pctrlLength1, *m_pctrlLength2, *m_pctrlLength3, *m_pctrlLength4;

	bool m_bScale;

	bool m_bGrid;
	int m_Style, m_Width;
	QColor m_Color;
	double m_Unit;
	bool m_bMinGrid;
	int m_MinStyle, m_MinWidth;
	QColor m_MinColor;
	double m_MinorUnit;

	bool m_bGrid2;
	int m_Style2, m_Width2;
	QColor m_Color2;
	double m_Unit2;
	bool m_bMinGrid2;
	int m_MinStyle2, m_MinWidth2;
	QColor m_MinColor2;
	double m_MinorUnit2;
};

#endif // BODYGRIDDLG_H
