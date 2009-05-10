/****************************************************************************

	WingDlg Class
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

#ifndef WINGDLG_H
#define WINGDLG_H

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QTableView>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include "../Objects/Wing.h"
#include "../Misc/ColorButton.h"
#include "../Params.h"
#include "WingDelegate.h"


class WingDlg : public QDialog
{
	Q_OBJECT

	friend class CWing;
	friend class QMiarex;
	friend class MainFrame;
	friend class PlaneDlg;
	friend class WingDelegate;

public:
    WingDlg();
	bool InitDialog(CWing *pWing);

private slots:
	void OnCancel();
	void OnDelete();
	void OnOK();
	void OnCellChanged(QWidget *pWidget);
	void OnItemActivated(const QModelIndex &index);
	void OnItemClicked(const QModelIndex &index);
	void OnWingColor();
	void OnSide();
	void OnSymetric();
	void OnInsertBefore();
	void OnInsertAfter();
	void OnDeleteSection();
	void OnResetMesh();

private:
	void showEvent(QShowEvent *event);
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event );
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void reject();

	int IsFoil(QPoint pt);
	bool CheckWing();
	void CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap);
	void DrawDihedral(QPainter &painter, QPoint O);
	void DrawVLMMesh(QPainter &painter, QPoint O);
	void DrawWing(QPainter &painter, QPoint O);
	void DrawFoils(QPainter &painter, QPoint O);
	void ComputeGeometry();
	void SetupLayout();
	void SetWingData();
	void FillDataTable();
	void FillTableRow(int row);
	void ReadParams();
	void ReadSectionData(int sel);
	void SetCurrentSection(int section);
	void SetScale();
	int VLMGetPanelTotal();
	bool VLMSetAutoMesh(int total=0);

	double GetOffset(double yob);
	double GetZPos(double y);
	double GetChord(double yob);


	QLineEdit *m_pctrlWingName;
	QCheckBox *m_pctrlSymetric;
	QRadioButton *m_pctrlLeftSide, *m_pctrlRightSide;
	ColorButton *m_pctrlWingColor;
	QPushButton *m_pctrlResetMesh;
	QLabel *m_pctrlWingSpan, *m_pctrlWingArea, *m_pctrlWingVolume, *m_pctrlMAC, *m_pctrlGeomChord;
	QLabel *m_pctrlMACSpanPos, *m_pctrlAspectRatio, *m_pctrlTaperRatio, *m_pctrlSweep, *m_pctrlNFlaps;
	QLabel *m_pctrlVLMPanels, *m_pctrl3DPanels;
	QLabel *m_pctrlProjectedArea, *m_pctrlProjectedSpan;

	QLabel *m_pctrlLength1, *m_pctrlLength2, *m_pctrlLength3, *m_pctrlLength4, *m_pctrlLength5;
	QLabel *m_pctrlAreaUnit1, *m_pctrlAreaUnit2, * m_pctrlVolumeUnit;
	QTableView *m_pctrlWingTable;
	QStandardItemModel *m_pWingModel;
	WingDelegate *m_pWingDelegate;

	QPushButton *OKButton, *CancelButton;

	QMenu *m_pContextMenu;
	QAction *m_pInsertBefore, *m_pInsertAfter, *m_pDeleteSection;

	static void *s_pMainFrame;
	static void *s_pMiarex;
	static QList <void *> *s_poaFoil;

	CWing *m_pWing;

	QPoint m_PointDown, m_ptOffset;
	QRect m_DrawRect;

	bool m_bAcceptName;
	bool m_bRightSide;
	bool m_bChanged;
	bool m_bTrans;
	int m_iSection;

	double pi;
	double m_yMAC;
	double m_fWingScale;
};

#endif // WINGDLG_H
