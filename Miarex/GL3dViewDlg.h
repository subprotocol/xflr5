/****************************************************************************

	GL3dViewDlg Class
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
 
#ifndef GL3DVIEWDLG_H
#define GL3DVIEWDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include "GLWidget.h"
#include "../Objects/Plane.h"
#include "../Objects/WPolar.h"
#include "../Objects/WOpp.h"
#include "ArcBall.h"


class GL3dViewDlg : public QDialog
{
	Q_OBJECT
	friend class MainFrame;
	friend class QMiarex;
	friend class GLWidget;
	friend class WingDlg;
	friend class GLLightDlg;

public:
	GL3dViewDlg(void *pParent = NULL);
	~GL3dViewDlg();

private slots:
	void OnAxes(int state);
	void On3DCp(bool bChecked);
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();
	void OnSetupLight();
	void OnClipPlane(int pos);
	void OnLight(int state);
	void OnSurfaces(int state);
	void OnOutline(int state);
	void OnPanels(int state);
	void OnVortices(int state);
	void OnLift(int state);
	void OnIDrag(int state);
	void OnVDrag(int state);
	void OnDownwash(int state);
	void OnMoment(int state);

	void OnLiftScale(int pos);
	void OnDragScale(int pos);
	void OnVelocityScale(int pos);
	void OnCpScale();

private:
	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);

	void SetupLayout();
	void ClientToGL(QPoint const &point, CVector &real);
	void GLToClient(CVector const &real, QPoint &point);
	void GLCreateGeom(CWing *pWing, int List, QColor wingcolor);
	void GLCreateMesh();
	void GLCreateCtrlPts();
	void GLCreateVortices();
	void GLCreateLiftForce();
	void GLCreateMoments();
	void GLCreateCp();
	void GLCreateCpLegend();
	void GLCreateWOppLegend();
	void GLCreateDownwash(CWing *pWing, CWOpp *pWOpp, int List, int surf0);
	void GLCreateLiftStrip(CWing *pWing, CWOpp *pWOpp, int List);
	void GLCreateDrag(CWing *pWing, CWOpp *pWOpp, int List);
	void GLInverseMatrix();
	void GLRenderView();
	void GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes);
	void GLSetupLight();
	void GLDrawAxes();
	void GLCallViewLists();
	void GLDraw3D();
	void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);
	void Set3DScale();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void SetPointers(CWing *pWing=NULL, CWing *pWing2=NULL, CWing *pStab=NULL, CWing*pFin=NULL, CBody *pBody=NULL,
					 CPlane *pPlane=NULL, CWPolar *pWPolar=NULL, CWOpp* pWOpp=NULL, CPOpp *pPOpp=NULL);
	void UpdateView();
	void SetWOppPtr(CWOpp* pWOpp);
	void SetPOppPtr(CPOpp* pPOpp);
	void SetWPlrPtr(CWPolar* pWPolar);


private:
	static void *s_pMiarex;
	static void *s_pMainFrame;
	static void *s_pGLLightDlg;

	GLWidget *m_pglWidget;
	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels, *m_pctrlVortices;
	QPushButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso, *m_pctrlReset, *m_pctrlPickCenter, *m_pctrlGLLight, *m_pctrlCpScales;
	QSlider *m_pctrlClipPlanePos;

	QCheckBox *m_pctrlLift, *m_pctrlMoment, *m_pctrlIDrag, *m_pctrlVDrag;
	QCheckBox *m_pctrlTrans, *m_pctrlDownwash, *m_pctrlCp,*m_pctrlSurfVel, *m_pctrlAnimate, *m_pctrlStream;
	QSlider *m_pctrlAnimateSpeed;
	QSlider *m_pctrlLiftScaleSlider, *m_pctrlDragScaleSlider, *m_pctrlVelocityScaleSlider;
	QCursor m_hcArrow;
	QCursor m_hcCross;
	QCursor m_hcMove;

	CWing *m_pWing;//pointer to Miarex's current wing
	CWing *m_pWing2;
	CWing *m_pStab;
	CWing *m_pFin;
	CFrame *m_pFrame;
	CPlane *m_pPlane;
	CBody *m_pBody;
	CWPolar *m_pWPolar;
	CWOpp *m_pWOpp;
	CPOpp *m_pPOpp;
	CPanel *m_pPanel;
	CVector *m_pNode;

	ArcBall m_ArcBall;
	QPoint m_LastPoint, m_PointDown;

	bool m_bTrans;
	bool m_bDragPoint;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bglLight;
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_b3DCp, m_bDownwash; 	// defines whether the corresponding data should be displayed
	bool m_bMoments;							// defines whether the corresponfing data should be displayed
	bool m_bICd, m_bVCd, m_bStream, m_bSpeeds;  	// defines whether the corresponfing data should be displayed
	bool m_bVortices;				// defines whether the corresponfing data should be displayed
	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels;
	bool m_bBodyOverlay;		//true if a foil should be overlayed on the body cross-section
	bool m_bXTop, m_bXBot, m_bXCP; 	// defines whether the corresponfing data should be displayed

	bool m_bResetglGeom;			// true if the geometry OpenGL list needs to be refreshed
	bool m_bResetglMesh;			// true if the mesh OpenGL list needs to be refreshed
	bool m_bResetglWake;			// true if the wake OpenGL list needs to be refreshed
	bool m_bResetglOpp, m_bResetglLift, m_bResetglDrag, m_bResetglDownwash;			// true if the OpenGL lists need to be refreshed
	bool m_bResetglStream;			// true if the streamlines OpenGL list needs to be refreshed
	bool m_bResetglLegend;          //needs to be reset is window has been resized
	bool m_bResetglBody;
	bool m_bResetglBodyMesh;
	bool m_bResetglBody2D;
	bool m_bResetglBodyOverlay;
	bool m_bResetglBodyPoints;
	bool m_bResetglFlow;			// true if the crossflow OpenGL list needs to be refreshed
	bool m_bWakePanels;
	bool m_bShowCpScale;		//true if the Cp Scale in Miarex is to be displayed
	bool m_bAutoCpScale;		//true if the Cp scale should be set automatically
	bool m_bIs3DScaleSet;		// true if the 3D scale has been set, false if needs to be reset
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bAutoScales;

	int m_GLList;
	int m_iView;

	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];
	double m_LegendMin, m_LegendMax;

	double pi;
	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view
	double m_glScaled;//zoom factor for UFO
	double m_LiftScale, m_DragScale, m_VelocityScale;
	double m_GLScale;	// the OpenGl scale for the view frustrum with respect to the windows device context
						// this is not the scale to display the model in the OpenGL view

	CVector m_UFOOffset;


	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector
};

#endif // GL3DViewDlg_H
