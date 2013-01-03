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
#include "../objects/ArcBall.h"
#include "BodyGridDlg.h"
#include "BodyTableDelegate.h"
#include "../misc/FloatEdit.h"
#include "../misc/LineButton.h"
#include "../objects/Body.h"
#include "../threedwidget.h"


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
    GL3dBodyDlg(QWidget *pParent);
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
	void OnFrameCellChanged(QWidget *pWidget);
	void OnFrameItemClicked(const QModelIndex &index);
	void OnPointCellChanged(QWidget *pWidget);
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
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *evendyt);
	void ShowContextMenu(QContextMenuEvent * event);
	void showEvent(QShowEvent *event);
	void reject();

	void FillFrameTableRow(int row);
	void FillFrameDataTable();
	void FillFrameCell(int iItem, int iSubItem);
	void ReadFrameSectionData(int sel);

	void FillPointCell(int iItem, int iSubItem);
	void FillPointTableRow(int row);
	void FillPointDataTable();
	void ReadPointSectionData(int sel);

	void SetFrame(int iFrame);
	void SetFrame(CFrame *pFrame);

	void SetupLayout();
	void SetViewControls();

	void GLInverseMatrix();
	void GLCreateBody2DBodySection();
	void GLCreateBodyPoints();
	void GLCreateBodyFrames();
	void GLCreateBodyGrid();
	void GLCreateBodyOverlay();
	void GLRenderBody();
	void GLCallViewLists();
	void GLDraw3D();
	void GLDrawBodyLegend();
	void GLDrawBodyLineScale();
	void GLDrawBodyFrameScale();
	void SetControls();
	void SetScales();
	void SetRectangles();
	void SetBodyScale();
	void SetBodyLineScale();
	void SetFrameScale();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void UpdateView();

	bool InitDialog(CBody *pBody);
	bool LoadSettings(QSettings *pSettings);
	bool SaveSettings(QSettings *pSettings);

	void ResizeTables();

private:
	bool SetBody(CBody *pBody);
	void SetPicture();
	void StorePicture();
	void TakePicture();

	void Insert(CVector Pt);
	void Remove(CVector Pt);


private:
	static void *s_pMainFrame;
	static void *s_pGLLightDlg;
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
	LineButton *m_pctrlBodyStyle;
	FloatEdit *m_pctrlNXPanels, *m_pctrlNHoopPanels;
	QComboBox *m_pctrlXDegree, *m_pctrlHoopDegree;
	QPushButton *m_pctrlMenuButton;
	QMenu *BodyMenu;

	QTableView *m_pctrlFrameTable, *m_pctrlPointTable;
	QStandardItemModel *m_pFrameModel, *m_pPointModel;
	BodyTableDelegate *m_pFrameDelegate, *m_pPointDelegate;

	QAction *m_pInsertPoint, *m_pRemovePoint, *m_pScaleBody;
	QAction *m_pShowCurFrameOnly, *m_pResetScales;
	QAction *m_pUndo, *m_pRedo;
	QAction *m_pExportBodyDef, *m_pImportBodyDef, *m_pExportBodyGeom, *m_pTranslateBody;// *m_pSetupLight;
	QAction *m_pGrid;

	CBody m_TmpPic;
	CBody m_UndoPic[20];
	int m_StackPos, m_StackSize;// undo : current stack position and current stack size
	bool m_bStored;
	bool m_bResetFrame;

	bool m_bChanged;
	bool m_bShowMasses;

	QPoint m_MousePos;
	QPoint m_ptPopUp;
	CVector m_RealPopUp;

	CFrame *m_pFrame;
	CBody *m_pBody;
	CPanel *m_pPanel;
	CVector *m_pNode;

    BodyGridDlg *m_BodyGridDlg;

	static QList<void*> *s_poaBody;

	ArcBall m_ArcBall;
	QPoint m_LastPoint, m_PointDown;

	static bool s_bSurfaces, s_bOutline, s_bAxes, s_bVLMPanels;

	bool m_bEnableName;
	bool m_bTrans;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bglLight;
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bBodyOverlay;		//true if a foil should be overlayed on the body cross-section
	bool m_bResetglBody;
	bool m_bResetglBodyMesh;
	bool m_bResetglBody2D;
	bool m_bResetglBodyOverlay;
	bool m_bResetglBodyPoints;
	bool m_bIs3DScaleSet;		// true if the 3D scale has been set, false if needs to be reset
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bCurFrameOnly;

	int m_GLList;
//	int m_iView;
	static int s_NHoopPoints;			//hoop resolution for NURBS bodies
	static int s_NXPoints;				//longitudinal resolution for NURBS Bodies
	int m_Precision[10];

	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];

	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view
	double m_glScaled;//zoom factor for UFO
	double m_BodyScale, m_FrameScale, m_BodyRefScale, m_FrameRefScale;			// scale for 3D display
	double m_GLScale;	// the OpenGl scale for the view frustrum with respect to the windows device context
						// this is not the scale to display the model in the OpenGL view
	double m_LetterWidth;

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
