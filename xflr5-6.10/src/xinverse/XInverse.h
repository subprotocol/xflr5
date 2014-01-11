/****************************************************************************

	QXInverse Class
	Copyright (C) 2009-2013 Andre Deperrois adeperrois@xflr5.com

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
 * @file
 * This implements the QXInverse class which provides the interface for inverse design of Foil objects
 */

#ifndef QXINVERSE_H
#define QXINVERSE_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSettings>
#include "../misc/DoubleEdit.h"
#include "../misc/IntEdit.h"
#include "../misc/MinTextEdit.h"
#include "../graph/QGraph.h"
#include "../objects/Foil.h"
#include "../objects/Spline.h"

/**
 * @brief This class implements the interface for the inverse Foil design.
 *
 *Note: this interface was written without a good understanding of the XFoil methodology, which is a potential source of errors.
 */
class QXInverse : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class TwoDWidget;
	friend class InverseOptionsDlg;
	friend class FoilSelectionDlg;
	friend class Settings;

public:
	QXInverse(QWidget *parent = NULL);
	~QXInverse();

	void SetupLayout();
	void InitDialog();
 
signals:
	void projectModified();

private slots:
	void OnCpxx();
	void OnInverseApp();
	void OnMarkSegment();
	void OnInverseStyles();
	void OnTangentSpline();
	void OnShowSpline();
	void OnNewSpline();
	void OnApplySpline();
	void OnSpecal();
	void OnQReset();
	void OnFilter();
	void OnSmooth();
	void OnSymm() ;
	void OnExecute();
	void OnStoreFoil();
	void OnExtractFoil();
	void OnResetFoilScale();
	void OnInsertCtrlPt();
	void OnRemoveCtrlPt();
	void OnQInitial();
	void OnQSpec();
	void OnQViscous();
	void OnQPoints();
	void OnQReflected();
	void OnPertubate();
	void OnGraphSettings();
	void OnSpecInv() ;
	void OnZoomIn();
	void OnZoomX();
	void OnZoomY();

private:
	void UpdateView();

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	void doubleClickEvent(QPoint pos);
	void zoomEvent(QPoint pos, double zoomFactor);

	void CheckActions();
	void DrawGrid(QPainter &painter, double scale);
	void PaintView(QPainter &painter);
	void PaintGraph(QPainter &painter);
	void PaintFoil(QPainter &painter);
	void SetScale(QRect CltRect);
	void ResetQ();
	void ResetScale();
	void ResetMixedQ();
	void ReleaseZoom();
	void Smooth(int Pos1 = -1, int Pos2 = -1);
	void Clear();
	void Connect();
	void CreateMCurve();
	void CreateQCurve();
	void CancelMark();
	void CancelSpline();
	void CancelSmooth();
	void SetFoil();
	void SetTAngle(double a);
	void SetTGap(double tr, double ti);

	void ExecMDES();
	void LoadSettings(QSettings *pSettings);
	void SaveSettings(QSettings *pSettings);

	bool ExecQDES();
	bool SetParams();
	bool InitXFoil(Foil * pFoil);


	double qincom(double qc, double qinf, double tklam);

private:

	MinTextEdit *m_pctrlOutput;
	QLabel *m_pctrlSpecif;
	QRadioButton *m_pctrlSpecAlpha, *m_pctrlSpecCl;
	QPushButton *m_pctrlExec, *m_pctrlFilter, *m_pctrlPert, *m_pctrlApplySpline, *m_pctrlNewSpline, *m_pctrlResetQSpec, *m_pctrlSmooth;
	QCheckBox *m_pctrlShowSpline, *m_pctrlTangentSpline, *m_pctrlSymm;
	DoubleEdit *m_pctrlSpec, *m_pctrlFilterParam, *m_pctrlTGapy, *m_pctrlTGapx, *m_pctrlTAngle;

	QLineEdit *m_pctrlMAlphaSpec, *m_pctrlMClSpec;
	MinTextEdit *m_pctrlMOutput;
	IntEdit *m_pctrlIter;
	QPushButton *m_pctrlMark;
	QCheckBox *m_pctrlCpxx;

	QCheckBox *m_pctrlMShowSpline, *m_pctrlMTangentSpline;
	QPushButton *m_pctrlMExec, *m_pctrlMNewSpline, *m_pctrlMApplySpline, *m_pctrlMSmooth, *m_pctrlMResetQSpec;

	QWidget *m_pctrlMInvWidget,*m_pctrlFInvWidget;
	QStackedWidget *m_pctrlStackedInv;


	static void *s_pMainFrame;  /**< a static pointer to the instance of the application's MainFrame object */
	static void *s_p2DWidget;   /**< a static pointer to the instance of the application's central widget used for 2D drawings */

	void *m_pXFoil;             /**< a void pointer to the unique instance of the XFoil object */

	QList<void *> *m_poaFoil;   /**< a pointer to the array of Foil objects */

	Foil* m_pRefFoil;           /**< a pointer to the reference foil geometry used for inverse design */
	Foil* m_pModFoil;           /**< a pointer to the resulting Foil modified by inverse design operations */

	Spline m_Spline;            /**< the spline oject to modify the velocity curve */

	bool m_bXPressed;           /**< true if the 'X' key is pressed */
	bool m_bYPressed;           /**< true if the 'Y' key is pressed */
	bool m_bLoaded;             /**< true if a Foil has been loaded from the database and copied to the reference Foil */
	bool m_bTrans;              /**< true if the Foil representation is in the process of being translated */
	bool m_bTransGraph;         /**< true if the curves in the graph are in the process of being translated */
	bool m_bRefFoil;            /**< true if the reference Foil should be displayed */
	bool m_bModFoil;            /**< true if the modified Foil should be displayed */
	bool m_bGetPos;             /**< true if the program is waiting for the user to click on a point curve */
	bool m_bMark;               /**< true if the user is in the process of marking a curve segment for modifiction */
	bool m_bMarked;             /**< true if a curve segment has been marked for modifiction */
	bool m_bSpline;             /**< true if the user is in the process of selecting points segments for the spline */
	bool m_bSplined;            /**< true if the velocity curve has been modified by application of the spline */
	bool m_bSmooth;             /**< true if the user is in the process of smoothing the curve */
	bool m_bZoomPlus;           /**< true if the user is in the process of zooming in by drawing a rectangle */
	bool m_bZoomXOnly;          /**< true if only the x-axis should be scaled */
	bool m_bZoomYOnly;          /**< true if only the y-axis should be scaled */
	bool m_bFullInverse;        /**< true if the full inverse method is selected, false if mixed-inverse */
	bool m_bReflected;          /**< true if the reflected curve should be displayed */

	bool m_bShowPoints;         /**< true if the curve points are visible in the graph */
	bool m_bTangentSpline;      /**< true if the spline should be tangent to the velocity curve at its end points */


	int m_Mk1;                 /** the index of the first marked point on the graph */
	int m_Mk2;                 /** the index of the second marked point on the graph */

	double m_fRefScale;        /**< the default scale for the display of the Foil, for the current window size */
	double m_fScale;           /**< the current scale for the Foil display */

	int m_ReflectedStyle;      /**< the index of the reflected curve's style */
	int m_ReflectedWidth;      /**< the reflected curve's width */
	QColor m_ReflectedClr;     /**< the reflected curve's color */

	QGraph m_QGraph;           /**< the velocity QGraph object */
	QGraph *m_pCurGraph;       /**< = &m_QGraph if the mouse hivers over the graph, false otherwise */
	Curve* m_pQCurve;          /**< a pointer to the inviscid velocity reference Curve */
	Curve* m_pQVCurve;         /**< a pointer to the viscous velocity reference Curve */
	Curve* m_pMCurve;          /**< a pointer to the modified specification curve */
	Curve* m_pReflectedCurve;  /**< a pointer to the reflected curve */

	QRect m_rCltRect;          /**< the view's client area, in pixels */
	QRect m_rGraphRect;        /**< the graph's client are, in pixels */
	QRect m_ZoomRect;          /**< the rectangle drawn by the user to define the area to zoom in */

	QPoint m_PointDown;        /**< the screen point where the left mouse button was last pressed */
	QPoint m_ptPopUp;          /**< the screen point where the right mouse button was last pressed */
	QPoint m_ptOffset;         /**< the offset position for the display of the Foil */

// temporary allocations
	int m_tmpPos, m_Pos1, m_Pos2, m_nPos, m_SplineLeftPos, m_SplineRightPos;
	double xd, yd;
	double xu, yu;

	QPoint tanpt;
	QPoint P0, P1, P2;

};

#endif // QXINVERSE_H



