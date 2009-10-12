/****************************************************************************

	GL3dBodyDlg Class
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
 
#ifndef GL3DBODYDLG_H
#define GL3DBODYDLG_H

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
#include "../GLWidget.h"
#include "ArcBall.h"
#include "BodyGridDlg.h"
#include "BodyTableDelegate.h"
#include "../Misc/FloatEdit.h"
#include "../Misc/LineButton.h"
#include "../Objects/Body.h"


class GL3dBodyDlg : public QDialog
{
	Q_OBJECT
	friend class MainFrame;
	friend class QMiarex;
	friend class GLWidget;
	friend class WingDlg;
	friend class GLLightDlg;
	friend class BodyScaleDlg;
	friend class ManageBodiesDlg;
	friend class PlaneDlg;

public:
	GL3dBodyDlg(void *pParent = NULL);


private slots:
	void OnAxes();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();
	void OnBodyStyle();
	void OnExportBodyDef();
	void OnExportBodyGeom();
	void OnImportBodyDef() ;
	void OnTranslateBody();
	void OnGrid();
	void OnInertia();
	void OnClipPlane(int pos);
	void OnLight();
	void OnSurfaces();
	void OnOutline();
	void OnPanels();
	void OnLineType();
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
	void OnSelChangeXDegree(int sel);
	void OnSelChangeHoopDegree(int sel);
	void OnOK();

private:
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
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

	void InitDialog();
	void SetFrame(int iFrame);
	void SetupLayout();
	void ClientToGL(QPoint const &point, CVector &real);
	void GLToClient(CVector const &real, QPoint &point);
	void GLCreateCtrlPts();
	void GLInverseMatrix();
	void GLCreateBody2DBodySection();
	void GLCreateBodyBezier(CBody *pBody);
	void GLCreateBody3DSplines(CBody *pBody);
	void GLCreateBody3DFlatPanels(CBody *pBody);
	void GLCreateBodyMesh(CBody *pBody);
	void GLCreateBodyPoints();
	void GLCreateBodyFrames();
	void GLCreateBodyGrid();
	void GLCreateBodyOverlay();
	void GLRenderBody();
	void GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes);
	void GLSetupLight();
	void GLDrawAxes();
	void GLCallViewLists();
	void GLDraw3D();
	void GLDrawBodyLegend();
	void GLDrawBodyLineScale();
	void GLDrawBodyFrameScale();
	void SetBodyScale();
	void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void SetBody(CBody *pBody);
	void UpdateView();

	bool LoadSettings(QDataStream &ar);
	bool SaveSettings(QDataStream &ar);

	void TakePicture();
	void SetPicture();
	void StorePicture();


private:
	static void *s_pMiarex;
	static void *s_pMainFrame;
	static void *s_pGLLightDlg;

	GLWidget *m_pglWidget;

	QWidget *m_pctrlControlsWidget;

	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels;
	QPushButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso, *m_pctrlReset, *m_pctrlPickCenter, *m_pctrlGLLight;
	QPushButton *m_pctrlUndo, *m_pctrlRedo;
	QPushButton *m_pctrlOK, *m_pctrlCancel;

	QSlider *m_pctrlClipPlanePos;

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
	QAction *m_pInertia;

	CBody m_TmpPic;
	CBody m_UndoPic[20];
	int m_StackPos, m_StackSize;// undo : current stack position and current stack size
	bool m_bStored;
	bool m_bResetFrame;

	bool m_bChanged;

	QPoint m_MousePos;
	QPoint m_ptPopUp;
	CVector m_RealPopUp;

	CFrame *m_pFrame;
	CBody *m_pBody;
	CPanel *m_pPanel;
	CVector *m_pNode;

	BodyGridDlg m_BodyGridDlg;

	static QList<void*> *s_poaBody;

	ArcBall m_ArcBall;
	QPoint m_LastPoint, m_PointDown;

	bool m_bEnableName;
	bool m_bTrans;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bglLight;
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels;
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
	int m_NXPoints, m_NHoopPoints;
	int m_Precision[10];

	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];
	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; //temporary points to save calculation times for body NURBS surfaces
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)];

	double pi;
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

#endif // GL3DBODYDLG_H
