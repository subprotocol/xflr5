/****************************************************************************

	Miarex    Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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


#ifndef QMIAREX_H
#define QMIAREX_H

#include "Params.h"
#include <QWidget>
#include <QPixmap>
#include <QList>
#include "../Misc/FloatEdit.h"
#include "../Objects/Body.h"
#include "../Objects/Wing.h"
#include "../Objects/Plane.h"
#include "../Objects/WPolar.h"
#include "../Objects/WOpp.h"
#include "../Objects/POpp.h"
#include "../Objects/Polar.h"
#include "../Objects/Polar.h"
#include "../Objects/Foil.h"
#include "../Objects/OpPoint.h"
#include "../Graph/QGraph.h"



struct TempStruct
{
	public:

		int m_Type, m_AnalysisType;
		double m_Weight, m_QInf, m_XCmRef, m_Density, m_Viscosity, m_Alpha, m_Beta;
		bool m_bVLM1, m_bTiltedGeom, m_bWakeRollUp;
};


class QMiarex : public QWidget
{
	friend class MainFrame;
	friend class GLWidget;

	Q_OBJECT

public:

	QMiarex(QWidget *parent = NULL);

	CWPolar* AddWPolar(CWPolar* pWPolar);
	CWPolar* GetWPolar(QString WPolarName);
	CWing* AddWing(CWing *pWing);
	CWOpp* GetWOpp(double Alpha);
	CPOpp* GetPOpp(double Alpha);
	CBody* AddBody(CBody *pBody);

	CPlane * AddPlane(CPlane *pPlane);
	CPlane * GetPlane(QString PlaneName);
	CWing * GetWing(QString WingName);
	CBody *GetBody(QString BodyName);
	CWOpp* InsertWOpp(CWOpp *pNewPoint);
	void AddWOpp(bool bPointOut, double *Gamma = NULL, double *Sigma = NULL, double *Cp = NULL);
	void AddPOpp(bool bPointOut, double *Cp, double *Gamma = NULL, double *Sigma=NULL, CPOpp *pPOpp = NULL);


//____________________Variables______________________________________
//
	TempStruct m_WngAnalysis;
	CPanel m_Panel[VLMMATSIZE];		// the panel array for the currently loaded UFO
	CPanel m_WakePanel[VLMMATSIZE];	// the reference current wake panel array
	CPanel m_MemPanel[VLMMATSIZE];		// used if the analysis should be performed on the tilted geometry
	CPanel m_RefWakePanel[VLMMATSIZE]; 	// the reference wake panel array if wake needs to be reset
	CPanel *m_pPanel[VLMMATSIZE];		// an array to the re-ordered VLM panels for a calculation

	CVector m_Node[2*VLMMATSIZE];		// the node array for the currently loaded UFO
	CVector m_MemNode[2*VLMMATSIZE];	// used if the analysis should be performed on the tilted geometry
	CVector m_WakeNode[2*VLMMATSIZE];	// the reference current wake node array
	CVector m_RefWakeNode[2*VLMMATSIZE]; 	// the reference wake node array if wake needs to be reset
	CVector m_TempWakeNode[2*VLMMATSIZE];	// the temporary wake node array during relaxation calc

	double m_aij[VLMMATSIZE*VLMMATSIZE];    // coefficient matrix
	double m_aijRef[VLMMATSIZE*VLMMATSIZE]; // coefficient matrix
	double m_RHS[VLMMATSIZE*100];			// RHS vector
	double m_RHSRef[VLMMATSIZE*100];		// RHS vector

	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; //temporary points to save calculation times for body NURBS surfaces
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)];

	CSurface *m_pSurface[2*MAXPANELS];	// An array with the pointers to the diferrent wing's surfaces

//	CWPolarAnalysis m_WngAnalysis;		// the dialog box for the polar definition
//	CControlAnalysis m_CtrlDlg;
//	CBodyGridDlg  m_BodyGridDlg;

	void* m_pMainFrame ;			// a pointer to the frame class

//	CVLMDlg m_VLMDlg;			// the dialog class which manages the VLM calculations
//	C3DPanelDlg m_PanelDlg;			// the dialog class which manages the 3D panel calculations
//	CLLTDlg m_LLTDlg;			// the dialog class which manages the LLT calculations
//	CFlowLinesDlg m_FlowLinesDlg;		// the dialog class for streamline options
//	CArcBall m_ArcBall;
//	CUFOListDlg m_UFOdlg;			// the dialog class for UFO management

	QList<void *> *m_poaFoil;			// a pointer to the foil array
	QList<void *> *m_poaPolar;			// a pointer to the foil polar array
	QList<void *> *m_poaWing;			// a pointer to the wing array
	QList<void *> *m_poaPlane;			// a pointer to the plane array
	QList<void *> *m_poaWPolar;			// a pointer to the UFO polar array
	QList<void *> *m_poaWOpp;			// a pointer to the UFO OpPoint array
	QList<void *> *m_poaPOpp;			// a pointer to the Plane OpPoint array
	QList<void *> *m_poaBody;			// a pointer to the Body array

//	bool m_bIsPrinting;			// the view is being printed
	bool m_bTrans;				// the view is being dragged
	bool m_bDragPoint;				// a point is being dragged
	bool m_bType1, m_bType2, m_bType4, m_bType5, m_bType6;	// polar types to be displayed
	bool m_bShowElliptic;			// true if the elliptic loading should be displayed in the local lift graph
	bool m_bStoreWOpp;			// true if the WOpp should be stored after a calculation
	bool m_bKeepOutOpps;			// true if points out of hte polar mesh should be kept
	bool m_bCurWOppOnly;			// true if only the current WOpp is to be displayed
	bool m_bCurFrameOnly;			// true if only the currently selected body frame is to be displayed
	bool m_bHalfWing;			// true if only a half-wing should be displayed in the OpPoint view
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
	bool m_bAnimate;			// true if there is an animation going on,
	bool m_bAnimatePlus;		// true if the animation is going in aoa crescending order
	bool m_bTransGraph;			// true if a graph is being dragged
	bool m_bShowWing2;			// true if the biplane's second wing OpPoint results should be displayed
	bool m_bShowStab;			// true if the stabilisator (elevator) OpPoint results should be displayed
	bool m_bShowFin;			// true if the fin OpPoint results should be displayed
	bool m_bIs2DScaleSet;		// true if the 2D scale has been set, false if needs to be reset
	bool m_bIs3DScaleSet;		// true if the 3D scale has been set, false if needs to be reset
	bool m_bAutoScales;			// true if the scale is to be reset after each UFO selection
	bool m_bLogFile;			// true if the log file warning is turned on
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bResetWake;
	bool m_bVLMFinished;		// true if the VLM calculation is finished
	bool m_bSetNewWake;			// true if the wake needs to be reset
	bool m_bDirichlet;			// true if Dirichlet BC are applied in 3D panel analysis, false if Neumann
	bool m_bTrefftz;			// true if the induced drag should be calculated in the Trefftz plane, false if calculated by summation over panels
	bool m_bWakePanels;
	bool m_bShowCpScale;		//true if the Cp Scale in Miarex is to be displayed
	bool m_bAutoCpScale;		//true if the Cp scale should be set automatically
	bool m_b3DCp, m_b3DDownwash; 	// defines whether the corresponfing data should be displayed
	bool m_bXTop, m_bXBot, m_bXCP, m_bXCmRef; 	// defines whether the corresponfing data should be displayed
	bool m_bMoments;							// defines whether the corresponfing data should be displayed
	bool m_bICd, m_bVCd, m_bStream, m_bSpeeds;  	// defines whether the corresponfing data should be displayed
	bool m_bVortices;				// defines whether the corresponfing data should be displayed
	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels;
	bool m_bglLight;
	bool m_bArcball;			//true if the arcball is to be displayed
	bool m_bCrossPoint;			//true if the control point on the arcball is to be displayed
	bool m_bPickCenter;			//true if the user is in the process of picking a new center for OpenGL display
	bool m_bBodyOverlay;		//true if a foil should be overlayed on the body cross-section

	int m_NSurfaces;
	int m_nNodes;				// the current number of nodes for the currently loaded UFO
	int m_MatSize;			// the matrix size
	int m_iView;				//1=opp, 2=plr, 3=3D
	int m_GLList;				// number of current Open Gl Lists
	int m_iWingView;			// defines how many graphs will be displayed in WOpp view
	int m_iWPlrView;			// defines how many graphs will be displayed in WPolar view
	int m_XW1, m_YW1, m_XW2, m_YW2, m_XW3, m_YW3, m_XW4, m_YW4; 	// the WPolar graph variables
	int m_WOppVar1,m_WOppVar2, m_WOppVar3, m_WOppVar4; 		// the WOpp graph variables
	int m_Iter ;				// the number of iterations for LLT
	int m_NStation ;			// the number of stations for LLT
	int m_posAnimate;			// the current animation aoa index
	int m_nWakeNodes;			// Size of the node array if there is a wake
	int m_WakeSize;				// Size of the Matrix if there is a wake
	int m_NWakeColumn;			// number of wake columns
	int m_MaxWakeIter;			// wake roll-up iteration limit
	int m_WakeInterNodes;		// number of intermediate nodes between wake panels
	int m_InducedDragPoint;		// 0 if dwonwash is at panel's centroid, 1 if averaged over panel length //used in CWing::VLMTrefftz

	int m_NHoopPoints;			//hoop resolution for NURBS bodies
	int m_NXPoints;				//longitudinal resolution for NURBS Bodies

	double m_ClipPlanePos;
	double m_LegendMin, m_LegendMax;
	double m_CurSpanPos;		//Span position for Cp Grpah
	double m_CoreSize;			// core size for VLM vortices
	double m_MinPanelSize;			// wing minimum panel size ; panels of less length are ignored
	double pi;				// ???
	double m_WingScale;			// scale for 2D display
	double m_BodyScale;			// scale for 2D display
	double m_FrameScale;			// scale for 2D display
	double m_BodyRefScale;			// scale for 2D display
	double m_FrameRefScale;			// scale for 2D display
	double m_LastWOpp;			// last WOPP selected, try to set the same if it exists, for the new polar
	double MatIn[4][4], MatOut[4][4];

	double m_GLScale;	// the OpenGl scale for the view frustrum with respect to the windows device context
						// this is not the scale to display the model in the OpenGL view

	QFile* m_pXFile;			// a pointer to the output .log file

	QPoint m_LastPoint;			// last mouse position point
	QPoint m_PointDown;			//last place where the user clicked
	QPoint m_ptOffset;			// client offset position for wing display
	QPoint m_WPlrLegendOffset;		// client offset position for wing polar legend
	QPoint m_WingLegendOffset;		// client offset position for WOPP polar legend
	QPoint m_ptPopUp;
	CVector m_RealPopUp;

	CVector m_UFOOffset;
	CVector m_BodyOffset;
	CVector m_FrameOffset;

	CVector m_BodyScalingCenter, m_BodyScaledOffset;
	CVector m_FrameScalingCenter, m_FrameScaledOffset;

	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view

	double m_glScaled;//zoom factor for UFO

	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector

	QColor m_CpColor;
	int m_CpStyle, m_CpWidth;
	bool m_bShowCp, m_bShowCpPoints;

	QRect m_rCltRect, m_rDrawRect; // the client and drawing rectangles
	QRect m_rSingleRect;
	QRect m_BodyLineRect;
	QRect m_FrameRect;
	QRect m_BodyRect;

	QCursor m_hcArrow;
	QCursor m_hcCross;
	QCursor m_hcMove;

	QGraph m_WingGraph1;			// the WOpp graphs
	QGraph m_WingGraph2;
	QGraph m_WingGraph3;
	QGraph m_WingGraph4;
	QGraph m_WPlrGraph1;			// the WPolar graphs
	QGraph m_WPlrGraph2;
	QGraph m_WPlrGraph3;
	QGraph m_WPlrGraph4;
	QGraph m_CpGraph;			// cross flow Cp Graph in 3D panel analysis
	QGraph* m_pCurGraph;			// currently active graph
	QGraph* m_pCurWPlrGraph;			// currently active WPolar graph
	QGraph* m_pCurWingGraph;			// currently active WOpp graph

	CWing *m_pCurWing;			// the currently selected wing
	CWing * m_pCurWing2;			// the currently selected Plane's 2nd wing, if any
	CWing * m_pCurStab;			// the currently selected Plane's elevator
	CWing * m_pCurFin;			// the currently selected Plane's fin
	CWPolar * m_pCurWPolar;			// the currently selected WPolar
	CWOpp * m_pCurWOpp;			// the currently selected Wing Operating Point
	CPlane * m_pCurPlane;			// the currently selected Plane
	CPOpp * m_pCurPOpp;			// the currently selected Plane Operating Point
	CFrame *m_pCurFrame;
	CBody *m_pCurBody;

	QColor m_WingColor, m_StabColor, m_FinColor;
};

#endif // QMIAREX_H
