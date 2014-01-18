/****************************************************************************

	AFoil Class
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


/** @file
 *
 * This file implements the QAFoil class used as the interface for direct Foil design.
 *
*/

#ifndef QAFOIL_H
#define QAFOIL_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QRadioButton>
#include <QSettings>
#include <QList>

#include "../params.h"
#include "FoilTableDelegate.h"
#include "../objects/Foil.h"
#include "../objects/SplineFoil.h"



/**
 * @brief the QAFoil class used as the interface for direct Foil design
 */
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
	~QAFoil();

	void SetupLayout();
	void InitDialog();

signals:
	void projectModified();

private slots:
	void OnAFoilLECircle();
	void OnExportSplinesToFile();
	void OnRenameFoil();
	void OnGrid();
	void OnFoilStyle();
	void OnDeleteCurFoil();
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
	void OnFoilTableCtxMenu(const QPoint &);
	void OnAFoilTableColumns();
	void OnResetColumnWidths();
	void OnLoadBackImage();
	void OnClearBackImage();
	void OnInsertCtrlPt();
	void OnRemoveCtrlPt();

private:
	Foil* AddNewFoil(Foil *pFoil);
	void SetControls();
	void ClearStack(int pos=0);
	void DrawScale(QPainter &painter, double scalex);
	void DrawXGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawYGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawXMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void DrawYMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect);
	void FillFoilTable();
	void FillTableRow(int row);
	void ShowFoil(Foil* pFoil, bool bShow=true);
	void SetAFoilParams();
	void SelectFoil(Foil* pFoil = NULL);
	void PaintGrids(QPainter &painter);
	void PaintSplines(QPainter &painter);
	void PaintFoils(QPainter &painter);
	void PaintLegend(QPainter &painter);
	void PaintView(QPainter &painter);
	void SetScale();
	void SetScale(QRect CltRect);
	void SetTableFont();
	void ReleaseZoom();

	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);

	void TakePicture();
	void SetPicture();

	void UpdateView();

	void doubleClickEvent(QPoint pos);
	void zoomEvent(QPoint pos, double zoomFactor);

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	CVector MousetoReal(QPoint &point);


private:

	QTableView *m_pctrlFoilTable;
	QStandardItemModel *m_pFoilModel;
	FoilTableDelegate *m_pFoilDelegate;

	static void *s_pMainFrame;  /**< a static pointer to the instance of the application's MainFrame object */
	static void *s_p2DWidget;   /**< a static pointer to the instance of the application's central widget used for 2D drawings */

	int *m_precision;           /**< the array of digit numbers for each column of the Foil table >*/

	bool m_bScale;              /**< true if the scale should be displayed */
	bool m_bZoomPlus;           /**< true if the user is in the process of zooming in by drawing a rectangle */
	bool m_bZoomYOnly;          /**< true if only the y-axis should be scaled */
	bool m_bNeutralLine;        /**< true if the neutral line should be displayed */
	bool m_bTrans;              /**< true if the view is being dragged by the user */
	bool m_bLECircle;           /**< true if the leading edge circle should be displayed */
	bool m_bStored;             /**< true if the current Picture has been stored on the Undo stack */
	bool m_bShowLegend;         /**< true is the legend should be shown */
	bool m_bXDown;              /**< true if the 'X' key is pressed */
	bool m_bYDown;              /**< true if the 'Y' key is pressed */
	bool m_bZDown;              /**< true if the 'Z' key is pressed */
	bool m_bXGrid;              /**< true if the X main grid (vertical lines) should be displayed */
	bool m_bYGrid;              /**< true if the Y main grid (horizontal lines) should be displayed */
	bool m_bXMinGrid;           /**< true if the X minor grid (vertical lines) should be displayed */
	bool m_bYMinGrid;           /**< true if the Y minor grid (horizontal lines) should be displayed */

	int m_XGridStyle;           /**< the style of the main X-grid */
	int m_YGridStyle;           /**< the style of the main Y-grid */
	int m_XGridWidth;           /**< the width of the main X-grid */
	int m_YGridWidth;           /**< the width of the main Y-grid */
	int m_XMinStyle;            /**< the style of the minor X-grid */
	int m_YMinStyle;            /**< the style of the minor Y-grid */
	int m_XMinWidth;            /**< the width of the minor X-grid */
	int m_YMinWidth;            /**< the width of the minor Y-grid */
	int m_NeutralStyle;         /**< the style of the neutral line y=0 */
	int m_NeutralWidth;         /**< the width of the neutral line y=0 */

	double m_XGridUnit;         /**< the unit of the main X-grid */
	double m_YGridUnit;         /**< the unit of the main Y-grid */
	double m_XMinUnit;          /**< the unit of the minor X-grid */
	double m_YMinUnit;          /**< the unit of the minor Y-grid */

	QColor m_XGridColor;        /**< the color of the main X-grid */
	QColor m_YGridColor;        /**< the color of the main Y-grid */
	QColor m_XMinColor;         /**< the color of the minor X-grid */
	QColor m_YMinColor;         /**< the color of the minor Y-grid */
	QColor m_NeutralColor;


	double m_LERad;             /**< the radius of the leading edge circle to draw */
	double m_fScale;            /**< the current scale of the display */
	double m_fScaleY;           /**< the ratio between the  y and x scales */
	double m_fRefScale;         /**< the reference scale of the display */

	QList<void *> *m_poaFoil;   /**< a pointer to the array of Foil objects */
	void *m_pXFoil;             /**< a void pointer to the XFoil object */

	SplineFoil *m_pSF;          /**< a pointer to the SplineFoil object */

	QPoint m_ptOffset;          /**< the foil's leading edge position in screen coordinates */
	QPoint m_ViewportTrans;     /**< the translation of the viewport */
	QPoint m_PointDown;         /**< the screen point where the last left-click occured */

	QRect m_rCltRect;           /**< the screen client rectangle */
	QRect m_ZoomRect;           /**< the user-defined rectangle for zooming in */

	CVector m_MousePos;         /**< the mouse position */

	Foil *m_pBufferFoil;        /**< a pointer to the active buffer foil */


	QCursor m_hcMove;           /**< the cursor to display when moving the viewport */
	QCursor m_hcCross;          /**< the cursor to display in the client area, when not dragging or zooming */


	int m_StackPos;                   /**< the current position on the Undo stack */
	QList<SplineFoil> m_UndoStack;    /**< the stack of incremental modifications to the SplineFoil;
										 we can't use the QStack though, because we need to access
										 any point in the case of multiple undo operations */

	bool m_bIsImageLoaded;      /**< true if a backgruond image is loaded */
	QPixmap m_BackImage;        /**< the QPixmap object with the background image */
};

#endif // QAFOIL_H



