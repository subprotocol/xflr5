/****************************************************************************

	MainFrame Class

    Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QStringList>
#include <QStackedWidget>
#include <QToolButton>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QTranslator>
#include <QtGui/QMainWindow>
#include "GLWidget.h"
#include <QList>
#include "TwoDWidget.h"
#include "Params.h"
#include "Objects/Foil.h"
#include "Objects/Polar.h"
#include "Objects/OpPoint.h"
#include "Objects/Plane.h"
#include "Graph/QGraph.h"


class MainFrame : public QMainWindow
{
	friend class TwoDWidget;
	friend class QXDirect;
	friend class QMiarex;
	friend class QAFoil;
	friend class QXInverse;
	friend class Miarex;
	friend class CBody;
	friend class CWing;
	friend class CWPolar;
	friend class CWOpp;
	friend class CPlane;
	friend class BodyGridDlg;
	friend class XFoilAnalysisDlg;
	friend class BatchDlg;
	friend class InterpolateFoilsDlg;
	friend class WingDlg;
	friend class WPolarDlg;
	friend class StabPolarDlg;
	friend class StabViewDlg;
	friend class PlaneDlg;
	friend class PanelAnalysisDlg;
	friend class VLMAnalysisDlg;
	friend class StabAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3DScales;
	friend class ManageBodiesDlg;
	friend class CtrlPolarDlg;
	friend class WingScaleDlg;
	friend class BodyTransDlg;
	friend class GLWidget;
	friend class GL3dWingDlg;
	friend class WAdvancedDlg;
	friend class GraphDlg;
	friend class LLTAnalysisDlg;
	friend class DisplaySettingsDlg;
	friend class ManageFoilsDlg;
	friend class ManageUFOsDlg;
	friend class InertiaDlg;
	friend class ManualInertiaDlg;

	Q_OBJECT

public:
	MainFrame(QWidget *parent = 0, Qt::WFlags flags = 0);

	int LoadXFLR5File(QString PathName);

public slots:
	void OnAFoil();
	void OnXDirect();
	void OnXInverse();
	void OnXInverseMixed();
	void OnMiarex();

private slots:
	void AboutQFLR5();
	void OnCurFoilStyle();
	void OnGuidelines();
	void OnExportCurGraph();
	void OnInsertProject();
	void OnNewProject();
	void OnLanguage();
	void OnLoadFile();
	void OnLogFile();
	void OnRenameCurFoil();
	void OnResetCurGraphScales();
	void OnResetSettings();
	void OnRestoreToolbars();
	void OnSaveOptions();
	bool OnSaveProjectAs();
	void OnSaveViewToImageFile();
	void OnSelChangeFoil(int i);
	void OnSelChangePolar(int i);
	void OnSelChangeOpp(int i);
	void OnSelChangeUFO(int i);
	void OnSelChangeWOpp(int i);
	void OnSelChangeWPolar(int i);
	void OnSaveProject();
	void OnStyle();
	void OnUnits();
	void OnSaveUFOAsProject();
	void openRecentFile();


/*___________________________________________Methods_______________________________*/
private:
	void closeEvent (QCloseEvent * event);
	void contextMenuEvent (QContextMenuEvent * event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	void CreateDockWindows();
	void CreateToolbars();
	void CreateStatusBar();
	void CreateActions();
	void CreateMenus();
	void CreateXDirectActions();
	void CreateXDirectMenus();
	void CreateXDirectToolbar();
	void CreateXInverseActions();
	void CreateXInverseMenus();
	void CreateXInverseToolbar();
	void CreateMiarexActions();
	void CreateMiarexMenus();
	void CreateMiarexToolbar();
	void CreateAFoilActions();
	void CreateAFoilMenus();
	void CreateAFoilToolbar();

	void ClientToGL(QPoint const &point, CVector &real);
	void DeleteProject();
	void DeletePlane(CPlane *pPlane, bool bResultsOnly = false);
	void DeleteWing(CWing *pThisWing, bool bResultsOnly = false);
	void GLToClient(CVector const &real, QPoint &point);
	void RemoveOpPoint(bool bCurrent);
	void SetCurrentFoil(CFoil* pFoil);
	void SaveSettings();
	void SetCentralWidget();
	void SetGraphSettings(Graph *pGraph);
	void SetProjectName(QString PathName);
	void SetMenus();
	void SetSaveState(bool bSave);
	void UpdateFoils();
	void UpdatePolars();
	void UpdateOpps();
	void UpdateUFOs();
	void UpdateWPolars();
	void UpdateWOpps();

	void UpdateView();
	void WritePolars(QDataStream &ar, CFoil *pFoil=NULL, int ProjectFormat=6);

	bool SaveProject(QString PathName="");
	bool LoadSettings();
	bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	bool SerializeProject(QDataStream &ar, bool bIsStoring, int ProjectFormat =6);
	bool SerializeUFOProject(QDataStream &ar, int ProjectFormat=5);
	bool DeleteFoil(CFoil *pFoil, bool bAsk=true);
	bool SelectFoil(CFoil *pFoil);
	bool SelectPolar(CPolar *pPolar);
	bool SelectOpPoint(OpPoint *pOpp);

	void AddFoil(CFoil *pFoil);
	void RenameFoil(CFoil *pFoil);

	CWing *GetWing(QString WingName);
	CFoil* GetFoil(QString strFoilName);
	CFoil* ReadFoilFile(QTextStream &ar);
	CFoil* ReadPolarFile(QDataStream &ar);
	CFoil* SetModFoil(CFoil* pNewFoil, bool bKeepExistingFoil = false);
	CPolar *GetPolar(QString m_FoilName, QString PolarName);
	CPolar* AddPolar(CPolar *pPolar);
	OpPoint *GetOpp(double Alpha);
	QColor GetColor(int type);


	//recent file
	QString ShortenFileName(QString &PathName);
	void AddRecentFile(const QString &PathNAme);
	void updateRecentFileActions();

/*___________________________________________Variables_______________________________*/
public:
	bool m_bMaximized;
	QString m_StyleName;
	QString m_VersionName;

private:
	void *m_pXInverse;
	void *m_pXDirect;
	void *m_pMiarex;
	void *m_pAFoil;

	TwoDWidget *m_p2DWidget;
	QStackedWidget *m_pctrlCentralWidget;
	GLWidget   *m_pGLWidget;

	QDockWidget *m_pctrlXDirectWidget, *m_pctrlMiarexWidget, *m_pctrlAFoilWidget, *m_pctrlXInverseWidget;
	QDockWidget *m_pctrl3DScalesWidget, *m_pctrlStabViewWidget;

	QToolBar *m_pctrlXDirectToolBar, *m_pctrlXInverseToolBar;
	QToolBar *m_pctrlMiarexToolBar;
	QToolBar *m_pctrlAFoilToolBar;

	//Common Menus
	QMenu * MainMenu;
	QMenu *fileMenu, *helpMenu;

	//AFoilMenus
	QMenu *AFoilViewMenu, *AFoilDesignMenu, *AFoilSplineMenu;
	QMenu *AFoilCtxMenu,*AFoilCurrentFoilMenu, *AFoilTableCtxMenu;

	//  XFoilAnalysis Menus
	QMenu * XDirectViewMenu;
	QMenu *FoilMenu, *CurFoilCtxMenu, *CurPolarCtxMenu, *CurGraphCtxMenu, *CurFoilDesignMenu, *CurOppCtxMenu;
	QMenu *currentFoilMenu;
	QMenu *DesignMenu;
	QMenu *OpPointMenu, *CpGraphMenu, *currentOppMenu;
	QMenu *PolarMenu, *currentPolarMenu, *GraphPolarMenu, *CurPolarGraphMenu;
	QMenu *OperFoilCtxMenu, *OperPolarCtxMenu, *CurXFoilResults;

	//XInverse menu
	QMenu *XInverseViewMenu, *InverseFoilMenu, *InverseGraphMenu, *InverseContextMenu;

	//Miarex Menus
	QMenu *MiarexViewMenu;
	QMenu *UFOMenu, *currentUFOMenu, *CurWPlrMenu, *CurWOppMenu, *CurBodyMenu;
	QMenu *MiarexBodyMenu, *MiarexWPlrMenu, *MiarexWOppMenu;
	QMenu *WPlrGraphMenu,*WPlrCurGraphMenu, *WOppGraphMenu, *WOppCurGraphMenu;
	QMenu *WPlrCtxMenu, *WOppCtxMenu, *W3DCtxMenu, *WTimeCtxMenu, *W3DStabCtxMenu;

	//MainFrame actions
	QAction *OnXDirectAct, *OnMiarexAct, *OnAFoilAct, *OnXInverseAct, *OnMixedInverseAct;
	QAction *openAct, *insertAct, *styleAct;
	QAction *saveAct, *saveProjectAsAct,*newProjectAct, *closeProjectAct, *saveOptionsAct;
	QAction *unitsAct;
	QAction *languageAct;
	QAction *exitAct;
	QAction *aboutAct, *guidelinesAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *saveViewToImageFileAct, *resetSettingsAct;

	//AFoil Actions
	QAction *zoomInAct, *ResetXScaleAct, *ResetYScaleAct, *ResetXYScaleAct;
	QAction *zoomYAct, *zoomLessAct, *AFoilGridAct;
	QAction *AFoilDelete, *AFoilRename, *AFoilExport;
	QAction *AFoilSetTEGap, *AFoilSetLERadius, *AFoilSetFlap, *AFoilInterpolateFoils, *AFoilNacaFoils;
	QAction *AFoilDerotateFoil, *AFoilNormalizeFoil, *AFoilRefineLocalFoil, *AFoilRefineGlobalFoil;
	QAction *AFoilEditCoordsFoil, *AFoilScaleFoil;
	QAction *AFoilLECircle, *m_pShowLegend;
	QAction *UndoAFoilAct, *RedoAFoilAct;
	QAction *HideAllFoils, *ShowAllFoils, *ShowCurrentFoil, *HideCurrentFoil;
	QAction *SplinesAct, *SplinedPointsAct, *storeSplineAct, *newSplinesAct, *splineControlsAct, *exportSplinesToFileAct;
	QToolButton *m_pctrlZoomY, *m_pctrlZoomIn;

	//Miarex Actions
	QAction *WPolarAct, *WOppAct, *W3DAct, *CpViewAct, *StabilityAct;//, *RootLocusAct, *ModalViewAct;
	QAction *W3DPrefsAct, *W3DLightAct, *W3DScalesAct;
	QAction *DefineWingAct, *DefinePlaneAct, *EditUFOAct, *SaveUFOAsProject;
	QAction *renameCurUFO, *deleteCurUFO, *duplicateCurUFO;
	QAction *renameCurWPolar, *editCurWPolar,*exportCurWPolar, *resetCurWPolar;
	QAction *deleteCurWPolar, *deleteCurWOpp;
	QAction *twoWingGraphs, *fourWingGraphs;
	QAction *WingGraph1,*WingGraph2,*WingGraph3,*WingGraph4;
	QAction *WPlrGraph1,*WPlrGraph2,*WPlrGraph3,*WPlrGraph4;
	QAction *twoWPlrGraphs, *allWPlrGraphs, *MiarexGraphDlg;

	QAction *ResetWingGraphScale;
	QAction *MiarexPolarFilter;
	QAction *allWPolarGraphsScalesAct, *allWingGraphsScalesAct, *allWPolarGraphsSettings, *allWingGraphsSettings;
	QAction *hideAllWPlrs, *showAllWPlrs;
	QAction *hideUFOWPlrs, *showUFOWPlrs, *deleteUFOWPlrs;
	QAction *hideUFOWOpps, *showUFOWOpps, *deleteUFOWOpps;
	QAction *resetWOppLegend, *resetWPlrLegend;
	QAction *exportCurWOpp, *showCurWOppOnly, *hideAllWOpps, *showAllWOpps, *deleteAllWOpps;
	QAction *showAllWPlrOpps, *hideAllWPlrOpps, * deleteAllWPlrOpps;
	QAction *defineWPolar, *defineCtrlPolar, *defineStabPolar, *advancedSettings;
	QAction *defineBody, *importBody, *exportBodyDef, *exportBodyGeom, *ManageBodies, *EditCurBody;
	QAction *showEllipticCurve, *showXCmRefLocation, *showStabCurve, *showFinCurve, *showWing2Curve;
	QAction *exporttoAVL, *resetWingScale, *scaleWingAct;
	QAction *ManageUFOs;
	QAction *m_pImportWPolar, *m_pUFOInertia, *m_pBodyInertia;
	QToolButton *m_pctrl3dView, *m_pctrlWPolarView, *m_pctrlWOppView, *m_pctrlCpView;
	QToolButton *m_pctrlStabilityButton;//, *m_pctrlRootLocusButton, *m_pctrlModalViewButton;


	//XDirect Actions
	QAction *PolarsAct, *OpPointsAct, *deletePolar, *definePolar, *editCurPolar, *defineBatch, *resetCurPolar;
	QAction *restoreToolbarsAct;
	QAction *exportCurPolar, *hideFoilPolars, *showFoilPolars, *saveFoilPolars,*deleteFoilPolars;
	QAction *showAllPolars, *hideAllPolars, *showCurOppOnly, *showAllOpPoints, *hideAllOpPoints;
	QAction *hideFoilOpps, *showFoilOpps, *deleteFoilOpps;
	QAction *hidePolarOpps, *showPolarOpps, *deletePolarOpps;
	QAction *exportCurOpp, *deleteCurOpp, *resetXFoil;
	QAction *viewXFoilAdvanced, *viewLogFile, *showPanels, *showNeutralLine, *resetFoilScale, *showInviscidCurve;
	QAction *exportCurFoil, *deleteCurFoil, *renameCurFoil, *setCurFoilStyle;
	QAction *DerotateFoil, *NormalizeFoil, *RefineLocalFoil, *RefineGlobalFoil , *EditCoordsFoil, *ScaleFoil;
	QAction *SetTEGap, *SetLERadius, *SetFlap, *InterpolateFoils, *NacaFoils, *DuplicateFoil;
	QAction *XDirectGraphDlg,*exportCurGraphAct, *resetCurGraphScales, *allPolarGraphsSettingsAct, *allPolarGraphsScales;
	QAction *TwoPolarGraphsAct, *AllPolarGraphsAct, *resetGraphLegend;
	QAction *PolarGraphAct[5];
	QAction *XDirectStyleAct;
	QAction *XDirectPolarFilter;
	QAction *defineCpGraphSettings, *exportCpGraphAct, *setQVarGraph, *setCpVarGraph;
	QAction *CurXFoilResExport, * CurXFoilCtPlot, *CurXFoilDbPlot, *CurXFoilDtPlot, *CurXFoilRtLPlot;
	QAction *CurXFoilRtPlot, *CurXFoilNPlot, *CurXFoilCdPlot, *CurXFoilCfPlot, *CurXFoilUePlot, *CurXFoilHPlot;
	QAction *ManageFoilsAct, *RenamePolarAct;
	QAction *m_pImportJavaFoilPolar, *m_pImportXFoilPolar;
	QToolButton *m_pctrlPolarView, *m_pctrlOppView;

	QComboBox *m_pctrlFoil, *m_pctrlPolar, * m_pctrlOpPoint;
	QComboBox *m_pctrlUFO, *m_pctrlWPolar, * m_pctrlWOpp;
	QRadioButton *m_pctrlFullInverse, *m_pctrlMixedInverse;
	QLabel *m_pctrlProjectName;

	//XInverse Actions
	QAction *StoreFoil, *ExtractFoil, *InverseStyles, *InverseResetScale, *InverseInsertCtrlPt, *InverseRemoveCtrlPt;
	QAction *InvQInitial, *InvQSpec, *InvQViscous, *InvQPoints, *InvQReflected;
	QAction *InverseResetGraphScale, *XInverseGraphDlg, *InverseZoomIn, *InverseZoomX, *InverseZoomY;
	QToolButton *m_pctrlInvZoomX, *m_pctrlInvZoomY, *m_pctrlInvZoomIn;

	QStringList m_RecentFiles;


	QList <void *> m_oaFoil;
	QList <void *> m_oaPolar;
	QList <void *> m_oaOpp;
	QList <void *> m_oaPlane;
	QList <void *> m_oaWing;
	QList <void *> m_oaWPolar;
	QList <void *> m_oaWOpp;
	QList <void *> m_oaPOpp;
	QList <void *> m_oaBody;


	int m_iApp;

	bool m_bSaved;
	bool m_bSaveOpps;
	bool m_bSaveWOpps;
	bool m_bSaveSettings;


	QString m_ProjectName, m_FileName, m_LastDirName, m_LanguageFilePath, m_ImageDirName;
	QColor m_crColors[30];

	QGraph m_RefGraph;//Reference setttings
	QColor m_BorderClr;

	int m_ImageFormat;

public:
 	QFont m_TextFont;
	QColor m_TextColor;
	QColor m_BackgroundColor;
	QPoint m_DlgPos;//preferred position for dialog boxes
	void *m_pGL3DScales;
	double m_mtoUnit;
	double m_mstoUnit;
	double m_m2toUnit;
	double m_kgtoUnit;
	double m_NtoUnit;
	double m_NmtoUnit;
	int m_LengthUnit;
	int m_AreaUnit;
	int m_WeightUnit;
	int m_SpeedUnit;
	int m_ForceUnit;
	int m_MomentUnit;

	void *m_pStabView;

};

#endif // MAINFRAME_H
