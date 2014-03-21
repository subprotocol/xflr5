/****************************************************************************

	GL3dWingDlg Class
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

#ifndef GL3DWINGDLG_H
#define GL3DWINGDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QSlider>
#include <QSettings>
#include <QLabel>
#include <QRadioButton>
#include <QToolButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>

#include "WingDelegate.h"
#include "../../objects/ArcBall.h"
#include "../../threedwidget.h"
#include "../../misc/DoubleEdit.h"
#include "../../misc/ColorButton.h"
#include "../../objects/Wing.h"



class GL3dWingDlg: public QDialog
{
	Q_OBJECT
	friend class MainFrame;
	friend class QMiarex;
	friend class ThreeDWidget;
	friend class Wing;
	friend class GLLightDlg;
	friend class PlaneDlg;
	friend class WingDelegate;

public:
	GL3dWingDlg(QWidget *pParent=NULL);
	~GL3dWingDlg();

private slots:
	void OnAxes();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();

//	void OnSetupLight();
	void OnClipPlane(int pos);
	void OnSurfaces();
	void OnOutline();
	void OnPanels();
	void OnFoilNames();
	void OnShowMasses();

	void OnOK();
	void OnDescriptionChanged();
	void OnCellChanged(QWidget *);
	void OnItemClicked(const QModelIndex &index);
	void OnWingColor();
	void OnSide();
	void OnSymetric();
	void OnInsertBefore();
	void OnInsertAfter();
	void OnDeleteSection();
	void OnResetSection();
	void OnResetMesh();
	void OnScaleWing();
	void OnInertia();
	void OnImportWing();
	void OnExportWing();


private:
	void reject();

	void doubleClickEvent(QPoint point);
	void MouseMoveEvent(QMouseEvent *event);
	void MousePressEvent(QMouseEvent *event) ;
	void MouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

	void zoomEvent(QPoint pos, double zoomFactor);


	bool CheckWing();
	void CreateXPoints(int NXPanels, int XDist, Foil *pFoilA, Foil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap);
	void ComputeGeometry();

	void SetWingData();
	void FillDataTable();
	void FillTableRow(int row);
	void ReadParams();
	void ReadSectionData(int sel);
	void SetCurrentSection(int section);
	void SetScale();
	int VLMGetPanelTotal();
	bool VLMSetAutoMesh(int total=0);

	bool InitDialog(Wing *pWing);
	void Connect();
	void SetupLayout();
	void GLCreateMesh();
	void GLRenderView();
	void GLDrawFoils();
	void GLCreateSectionHighlight();
	void GLInverseMatrix();
	void GLCallViewLists();
	void GLDraw3D();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void SetWingScale();
	void UpdateView();



private:
	static QList <void*> *s_poaFoil;
	static QPoint s_WindowPos;
	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;


	ThreeDWidget *m_pGLWidget;

	QLineEdit *m_pctrlWingName;
	QTextEdit *m_pctrlWingDescription;
	QCheckBox *m_pctrlSymetric;
	QRadioButton *m_pctrlLeftSide, *m_pctrlRightSide;
	ColorButton *m_pctrlWingColor;
	QPushButton *m_pctrlResetMesh;
	QPushButton *m_pctrlInsertBefore, *m_pctrlInsertAfter, *m_pctrlDeleteSection;
	QLabel *m_pctrlWingSpan, *m_pctrlWingArea, *m_pctrlMAC, *m_pctrlGeomChord;
    QLabel *m_pctrlAspectRatio, *m_pctrlTaperRatio, *m_pctrlSweep, *m_pctrlNFlaps;
	QLabel *m_pctrlVLMPanels, *m_pctrl3DPanels;
	QLabel *m_pctrlProjectedArea, *m_pctrlProjectedSpan;

    QLabel *m_pctrlLength1, *m_pctrlLength2, *m_pctrlLength3, *m_pctrlLength4;
	QLabel *m_pctrlAreaUnit1, *m_pctrlAreaUnit2, * m_pctrlVolumeUnit;
	QTableView *m_pctrlWingTable;
	QStandardItemModel *m_pWingModel;
	WingDelegate *m_pWingDelegate;

	QPushButton *OKButton, *CancelButton;
    QCheckBox *m_pctrlAxes, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels, *m_pctrlFoilNames;
	QCheckBox *m_pctrlShowMasses;
	QPushButton *m_pctrlReset, *m_pctrlPickCenter;

	QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;
	QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;

	QAction *m_pScaleWing, *m_pInertia;
	QAction *m_pExportWingAct, *m_pImportWingAct;

	QSlider *m_pctrlClipPlanePos;

	QAction *m_pResetScales;
	

	QMenu *m_pContextMenu;
	QAction *m_pInsertBefore, *m_pInsertAfter, *m_pDeleteSection, *m_pResetSection;

	QWidget *m_pctrlControlsWidget;

	Wing *m_pWing;

	bool m_bAcceptName;
	bool m_bRightSide;
	bool m_bChanged, m_bDescriptionChanged;
	bool m_bTrans;
	bool m_bStored;
	bool m_bEnableName;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bResetglWing;
	bool m_bResetglSectionHighlight;

	static bool s_bOutline;                   /**< true if the surface outlines are to be displayed in the 3D view*/
	static bool s_bSurfaces;                  /**< true if the surfaces are to be displayed in the 3D view*/
	static bool s_bVLMPanels;                 /**< true if the panels are to be displayed in the 3D view*/
	static bool s_bAxes;                      /**< true if the axes are to be displayed in the 3D view*/
	static bool s_bShowMasses;                /**< true if the point masses are to be displayed on the openGL 3D view */
	static bool s_bFoilNames;                 /**< true if the foil names are to be displayed on the openGL 3D view */


	int m_iSection;
	int m_GLList;

	double m_glScaled;//zoom factor for UFO
	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];


	QPoint m_MousePos;
	QPoint m_ptPopUp;
	QPoint m_LastPoint, m_PointDown;


	CVector m_RealPopUp;
	CVector m_UFOOffset;
	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector

	Panel *m_pPanel;
	CVector *m_pNode;

	ArcBall m_ArcBall;

	int  *m_precision;
};

#endif // GL3DWINGDLG_H
