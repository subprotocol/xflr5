/****************************************************************************

	QAFoil Class
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


#ifndef QAFOIL_H
#define QAFOIL_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QRadioButton>
#include <QSettings>

#include "../params.h"
#include "FoilTableDelegate.h"
#include "LECircleDlg.h"
#include "../misc/FloatEdit.h"
#include "../misc/LineButton.h"
#include "../objects/Foil.h"
#include "../objects/Sf.h"
#include "../xdirect/XFoil.h"


class QAFoil : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class FoilTableDelegate;
	friend class TwoDWidget;
	friend class AFoilGridDlg;
	friend class AFoilTableDlg;
	friend class FlapDlg;
	friend class NacaFoilDlg;
	friend class InterpolateFoilsDlg;
	friend class SplineCtrlsDlg;
	friend class TwoDPanelDlg;
	friend class FoilGeomDlg;
	friend class TEGapDlg;
	friend class LEDlg;
	friend class LECircleDlg;
	friend class FoilCoordDlg;
	friend class CAddDlg;

public:
	QAFoil(QWidget *parent = NULL);
	void SetupLayout();
	void InitDialog();
 

private slots:
	void OnAFoilLECircle();
	void OnExportSplinesToFile();
	void OnSplines();
	void OnSplinedPoints();
	void OnRenameFoil();
	void OnGrid();
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
	void OnFoilTableCtxMenu(const QPoint & position);
	void OnAFoilTableColumns();
	void OnColumnWidths();
	void OnLoadBackImage();
	void OnClearBackImage();
	void OnInsertCtrlPt();
	void OnRemoveCtrlPt();

private:

	void CheckButtons();
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
	void ReleaseZoom();
	void FoilVisibleClicked(const QModelIndex& index);

	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);

	void TakePicture();
	void SetPicture();
	void StorePicture();
	void UpdateView();

	void wheelEvent(QWheelEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	CVector MousetoReal(QPoint &point);


private:

	LineButton *m_pctrlFoilStyle;

	QTableView *m_pctrlFoilTable;
	QStandardItemModel *m_pFoilModel;
	FoilTableDelegate *m_pFoilDelegate;

	void * m_p2DWidget;
	void * m_pMainFrame;

	bool m_bScale;
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


//	CFoil *m_pCurFoil;
	CSF *m_pSF;

	QPoint m_ptOffset;//the foil's leading edge position in screen coordinates
	QPoint m_ViewportTrans; // the translation of the viewport
	QPoint m_ptPopUp;
	QPoint m_PointDown;

	QRect m_rCltRect;
	QRect m_ZoomRect;
	CVector m_MousePos;
	CFoil *m_pBufferFoil;


	QCursor m_hcMove;
	QCursor m_hcCross;

	Picture m_TmpPic;
	Picture m_UndoPic[MAXSTACKPOS];

	int m_CurrentColumn;

	bool m_bIsImageLoaded;
	QPixmap m_BackImage;
};

#endif // QAFOIL_H



