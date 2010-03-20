/****************************************************************************

	QAFoil Class
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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


#ifndef QAFOIL_H
#define QAFOIL_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QRadioButton>
#include <QSettings>

#include "FoilTableDelegate.h"
#include "../Misc/FloatEdit.h"
#include "../Misc/LineButton.h"
#include "../Objects/Foil.h"
#include "../Objects/Sf.h"
#include "../Objects/Pf.h"
#include "../XDirect/XFoil.h"


struct Picture
{
	public:
		double xExt[SPLINECONTROLSIZE];
		double yExt[SPLINECONTROLSIZE];
		double xInt[SPLINECONTROLSIZE];
		double yInt[SPLINECONTROLSIZE];
		CVector ExtRearPt, IntRearPt;
		int m_iExt, m_iInt;
};

class QAFoil : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class FoilTableDelegate;
	friend class TwoDWidget;
	friend class AFoilGridDlg;
	friend class FlapDlg;
	friend class NacaFoilDlg;
	friend class InterpolateFoilsDlg;
	friend class TwoDPanelDlg;
	friend class FoilGeomDlg;
	friend class TEGapDlg;
	friend class LEDlg;
	friend class LECircleDlg;
	friend class FoilCoordDlg;
	friend class CAddDlg;

public:
	QAFoil(QWidget *parent = NULL);
	virtual ~QAFoil();
	void SetupLayout();
	void InitDialog();
 

private slots:
	void OnAFoilLECircle();
	void OnExportSplinesToFile();
	void OnSplineType();
	void OnRenameFoil();
	void OnGrid();
	void OnCenterLine();
	void OnPoints();
	void OnVisible();
	void OnFoilStyle();
	void OnDelete();
	void OnDuplicate();
	void OnExportCurFoil();
	void OnFoilClicked(const QModelIndex& index);
	void OnShowAllFoils();
	void OnHideAllFoils();
	void OnHideCurrentFoil();
	void OnShowCurrentFoil();
	void OnShowLegend();
	void OnStoreSplines();
	void OnZoomIn();
	void OnZoomLess();
	void OnZoomYOnly();
	void OnResetXScale();
	void OnResetYScale();
	void OnResetScales();
	void OnUndo();
	void OnRedo();
	void OnSplineControls();
	void OnNewSplines();
	void OnAFoilSetFlap();
	void OnAFoilDerotateFoil();
	void OnAFoilNormalizeFoil();
	void OnAFoilCadd();
	void OnAFoilPanels();
	void OnAFoilFoilCoordinates();
	void OnAFoilFoilGeom();
	void OnAFoilSetTEGap();
	void OnAFoilSetLERadius();
	void OnAFoilInterpolateFoils();
	void OnAFoilNacaFoils();

private:
	void CheckButtons();
	void CheckFoilParams(CFoil* pFoil = NULL);
	void DrawScale(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawXGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawYGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawXMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawYMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void FillFoilTable();
	void FillTableRow(int row);
	void ShowFoil(CFoil* pFoil, bool bShow=true);
	void SetParams();
	void SelectFoil(CFoil* pFoil = NULL);
	void PaintGrids(QPainter &painter);
	void PaintSplines(QPainter &painter);
	void PaintFoils(QPainter &painter);
	void PaintLegend(QPainter &painter);
	void PaintView(QPainter &painter);
	void SetScale();
	void SetScale(QRect CltRect);
	void PFNew();
	void SFNew();
	void ReleaseZoom();


	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);

	void TakePicture();
	void SetPicture();
	void StorePicture();
	void UpdateView();

	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);

	CVector MousetoReal(QPoint &point);


private:
	QRadioButton *m_pctrlSF, *m_pctrlPF;
	LineButton *m_pctrlFoilStyle;
	QCheckBox *m_pctrlVisible;
	QCheckBox *m_pctrlCenterLine;
	QCheckBox *m_pctrlFoilPoints;
	QPushButton *m_pctrlRename, *m_pctrlDelete, *m_pctrlDuplicate, *m_pctrlExport;
	QTableView *m_pctrlFoilTable;
	QStandardItemModel *m_pFoilModel;
	FoilTableDelegate *m_pFoilDelegate;

	void * m_p2DWidget;
	void * m_pMainFrame;

	bool m_bScale;
	bool m_bSF;//else m_bPF
	bool m_bZoomPlus;
	bool m_bZoomYOnly;
	bool m_bNeutralLine;
	bool m_bTrans;
	bool m_bLECircle;
	bool m_bStored;
	bool m_bShowLegend;
	bool m_bXDown, m_bYDown, m_bZDown;

	bool m_bXGrid,m_bYGrid;
	bool m_bXMinGrid, m_bYMinGrid;
	int m_XGridStyle, m_YGridStyle;
	int m_XGridWidth, m_YGridWidth;
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	int m_NeutralStyle, m_NeutralWidth;
	double m_XGridUnit, m_YGridUnit;
	double m_XMinUnit, m_YMinUnit;
	QColor m_XGridColor,m_YGridColor;
	QColor m_XMinColor,m_YMinColor;
	QColor m_NeutralColor;


	int m_StackPos, m_StackSize;// undo : stack position and stack size

	double m_LERad;
	double m_fScale;
	double m_fScaleY;//ratio between y and x scales;
	double m_fRefScale;

	QList<void *> *m_poaFoil;
	XFoil *m_pXFoil;


	CFoil *m_pCurFoil;
	CSF *m_pSF;
	CPF *m_pPF;

	QPoint m_ptOffset;
	QPoint m_ptPopUp;
	QPoint m_PointDown;

	QRect m_rCltRect;
	QRect m_ZoomRect;
	CVector m_MousePos;
	CFoil *m_pBufferFoil;


	QCursor m_hcArrow;
	QCursor m_hcMove;
	QCursor m_hcCross;

	Picture m_TmpPic;
	Picture m_UndoPic[50];
};

#endif // QAFOIL_H



