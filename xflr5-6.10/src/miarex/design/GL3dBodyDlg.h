/****************************************************************************

	BodyDlg Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com
 
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
 
#ifndef BODYDLG_H
#define BODYDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSlider>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QToolButton>
#include <QSettings>

#include "BodyGridDlg.h"
#include "BodyTableDelegate.h"
#include "../../misc/DoubleEdit.h"
#include "../../misc/LineBtn.h"
#include "../../objects/ArcBall.h"
#include "../../objects/Body.h"
#include "../../threedwidget.h"


class GL3dBodyDlg : public QDialog
{
	Q_OBJECT
	friend class MainFrame;
	friend class QSail7;
	friend class QMiarex;
	friend class ThreeDWidget;
	friend class GLLightDlg;
	friend class BodyScaleDlg;
	friend class ManageBodiesDlg;
	friend class BoatDlg;
	friend class PlaneDlg;

public:
    GL3dBodyDlg(QWidget *pParent=NULL);
    ~GL3dBodyDlg();

private slots:
	void OnAxes();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();
	void OnBodyStyle();
	void OnBodyInertia();
	void OnEdgeWeight();
	void OnExportBodyDef();
	void OnExportBodyGeom();
	void OnImportBodyDef() ;
	void OnTranslateBody();
	void OnGrid();
	void OnClipPlane();
	void OnLight();
	void OnSurfaces();
	void OnOutline();
	void OnPanels();
	void OnLineType();
	void OnNURBSPanels();
	void OnInsert();
	void OnResetScales();
	void OnShowCurFrameOnly();
	void OnRemove();
	void OnFrameCellChanged(QWidget *);
	void OnFrameItemClicked(const QModelIndex &index);
	void OnPointCellChanged(QWidget *);
	void OnPointItemClicked(const QModelIndex &index);
	void OnScaleBody();
	void OnUndo();
	void OnRedo();
	void OnBodyName();
	void OnShowMasses();
	void OnSelChangeXDegree(int sel);
	void OnSelChangeHoopDegree(int sel);
	void OnOK();

private:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
	void reject();

	void zoomEvent(QPoint pos, double zoomFactor);
	void doubleClickEvent(QPoint point);

	void ShowContextMenu(QContextMenuEvent * event);

	void blockSignalling(bool bBlock);

	void FillFrameTableRow(int row);
	void FillFrameDataTable();
	void FillFrameCell(int iItem, int iSubItem);
	void ReadFrameSectionData(int sel);

	void FillPointCell(int iItem, int iSubItem);
	void FillPointTableRow(int row);
	void FillPointDataTable();
	void ReadPointSectionData(int sel);

	void SetFrame(int iFrame);
	void SetFrame(Frame *pFrame);

	void SetupLayout();
	void SetViewControls();
	void SetTableUnits();

	void GLInverseMatrix();
	void GLCreateBody2DBodySection();
	void GLCreateBodyPoints();
	void GLCreateBodyFrames();
	void GLCreateBodyGrid();
	void GLRenderBody();
	void GLCallViewLists();
	void GLDraw3D();
	void GLDrawBodyLegend();
	void GLDrawBodyLineScale();
	void GLDrawBodyFrameScale();
	void GLDrawMasses();
	void PaintBodyLegend(QPainter &painter);
	void SetControls();
	void SetScales();
	void SetRectangles();
	void SetBodyScale();
	void SetBodyLineScale();
	void SetFrameScale();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void UpdateView();

	bool InitDialog(Body *pBody);
	static bool LoadSettings(QSettings *pSettings);
	static bool SaveSettings(QSettings *pSettings);

	void ResizeTables();

private:
	bool SetBody(Body *pBody);

	void ClearStack(int pos=0);
	void SetPicture();
	void TakePicture();

	void Insert(CVector Pt);
	void Remove(CVector Pt);


private:
	ThreeDWidget m_3dWidget;

	static QPoint s_WindowPos;
	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;


	QWidget *m_pctrlControlsWidget;

	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels, *m_pctrlShowMasses;
	QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;
	QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;
	QPushButton *m_pctrlReset, *m_pctrlPickCenter;
	QPushButton *m_pctrlUndo, *m_pctrlRedo;
	QPushButton *m_pctrlOK, *m_pctrlCancel;

	QSlider *m_pctrlClipPlanePos;
	QSlider *m_pctrlEdgeWeight;
	QSlider *m_pctrlPanelBunch;

	QLineEdit *m_pctrlBodyName;
	QTextEdit *m_pctrlBodyDescription;

	QRadioButton *m_pctrlFlatPanels, *m_pctrlBSplines;
	LineBtn *m_pctrlBodyStyle;
	DoubleEdit *m_pctrlNXPanels, *m_pctrlNHoopPanels;
	QComboBox *m_pctrlXDegree, *m_pctrlHoopDegree;
	QPushButton *m_pctrlMenuButton;
	QMenu *BodyMenu;

	QTableView *m_pctrlFrameTable, *m_pctrlPointTable;
	QStandardItemModel *m_pFrameModel, *m_pPointModel;
	BodyTableDelegate *m_pFrameDelegate, *m_pPointDelegate;
	QItemSelectionModel *m_pSelectionModelPoint, *m_pSelectionModelFrame;

	QAction *m_pInsertPoint, *m_pRemovePoint, *m_pScaleBody;
	QAction *m_pShowCurFrameOnly, *m_pResetScales;
	QAction *m_pUndo, *m_pRedo;
	QAction *m_pExportBodyDef, *m_pImportBodyDef, *m_pExportBodyGeom, *m_pTranslateBody, *m_pBodyInertia;// *m_pSetupLight;
	QAction *m_pGrid;


	int *m_pPointPrecision;  /**< the array of digit numbers for each column of the Point table >*/
	int *m_pFramePrecision;  /**< the array of digit numbers for each column of the Frame table >*/


	int m_StackPos;                /**< the current position on the Undo stack */
	QList<Body*> m_UndoStack;      /**< the stack of incremental modifications to the SplineFoil;
									 we can't use the QStack though, because we need to access
									 any point in the case of multiple undo operations */

//	bool m_bStored;
	bool m_bResetFrame;

	bool m_bChanged;

	QPoint m_MousePos;
	QPoint m_ptPopUp;
	CVector m_RealPopUp;

	Frame *m_pFrame;
	Body *m_pBody;

	BodyGridDlg *m_pBodyGridDlg;


	ArcBall m_ArcBall;
	QPoint m_LastPoint, m_PointDown;

	//make static to keep settings across multiple calls
	static bool s_bglLight;
	static bool s_bOutline;                   /**< true if the surface outlines are to be displayed in the 3D view*/
	static bool s_bSurfaces;                  /**< true if the surfaces are to be displayed in the 3D view*/
	static bool s_bVLMPanels;                 /**< true if the panels are to be displayed in the 3D view*/
	static bool s_bAxes;                      /**< true if the axes are to be displayed in the 3D view*/
	static bool s_bShowMasses;                /**< true if the point masses are to be displayed on the openGL 3D view */


	bool m_bEnableName;
	bool m_bTrans;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bResetglBody;
	bool m_bResetglBodyMesh;
	bool m_bResetglBody2D;
	bool m_bResetglBodyPoints;
	bool m_bIs3DScaleSet;		// true if the 3D scale has been set, false if needs to be reset
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bCurFrameOnly;

	int m_GLList;
	static int s_NHoopPoints;			//hoop resolution for NURBS bodies
	static int s_NXPoints;				//longitudinal resolution for NURBS Bodies

	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];

	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view
	double m_glScaled;//zoom factor for UFO
	double m_BodyScale, m_FrameScale, m_BodyRefScale, m_FrameRefScale;			// scale for 3D display

	CVector m_UFOOffset;
	CVector m_BodyOffset;
	CVector m_FrameOffset;

	CVector m_BodyScalingCenter, m_BodyScaledOffset;
	CVector m_FrameScalingCenter, m_FrameScaledOffset;


	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector
	QRect m_BodyLineRect;
	QRect m_FrameRect;
	QRect m_BodyRect;
	QRect m_rCltRect;

};

#endif // BodyDlg_H
