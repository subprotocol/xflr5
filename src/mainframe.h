/****************************************************************************

	MainFrame Class

	Copyright (C) 2008-2012 Andre Deperrois adeperrois@xflr5.com

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
 *@file mainframe.h
 *@brief This file contains the description of the MainFrame class associated to the application's main window
 *
*/
#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include <QStringList>
#include <QStackedWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QLabel>
#include <QPointer>
#include <QTranslator>
#include <QMainWindow>
#include <QList>
#include "params.h"
#include "twodwidget.h"
#include "threedwidget.h"
#include "objects/Foil.h"
#include "objects/Polar.h"
#include "objects/OpPoint.h"
#include "graph/QGraph.h"
#include "misc/SaveOptionsDlg.h"
#include "misc/TranslatorDlg.h"
#include "misc/RenameDlg.h"
#include "misc/UnitsDlg.h"


/**
*@class MainFrame
*@brief The class associated to the application's main window.

  The class fills many functions:
  - it creates the child windows and toolbars of the application
  - it manages the loading and saving of settings
  - it stores and manages the arrays of data as member variables
  - it manages the load & save operations of project files 
  
  This class will remain only partially documented.
*/
class MainFrame : public QMainWindow
{
	friend class TwoDWidget;
	friend class ThreeDWidget;
	friend class QXDirect;
	friend class QMiarex;
	friend class QAFoil;
	friend class QXInverse;
	friend class Miarex;
	friend class Body;
	friend class Wing;
	friend class WPolar;
	friend class OpPoint;
	friend class CWOpp;
	friend class Plane;
	friend class BodyGridDlg;
	friend class XFoilAnalysisDlg;
	friend class FoilPolarDlg;
	friend class BatchDlg;
	friend class BatchThreadDlg;
	friend class InterpolateFoilsDlg;
	friend class WingDlg;
	friend class WPolarDlg;
	friend class StabPolarDlg;
	friend class StabViewDlg;
	friend class PlaneDlg;
	friend class PanelAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3DScales;
	friend class ManageBodiesDlg;
	friend class WingScaleDlg;
	friend class BodyTransDlg;
	friend class GL3dWingDlg;
	friend class WAdvancedDlg;
	friend class GraphDlg;
	friend class LLTAnalysisDlg;
	friend class DisplaySettingsDlg;
	friend class ManageFoilsDlg;
	friend class ManageUFOsDlg;
	friend class InertiaDlg;

	Q_OBJECT

public:
	MainFrame(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainFrame();

	enumApp LoadXFLR5File(QString PathName);
	static MainFrame* self();

public slots:
	void OnAFoil();
	void OnXDirect();
	void OnXInverse();
	void OnXInverseMixed();
	void OnMiarex();

private slots:
    void AboutQt();
	void AboutQFLR5();
	void OnCurFoilStyle();
	void OnDuplicateFoil();
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
	void OnPolarProps();
	void OnWOppProps();
	void openRecentFile();


/*___________________________________________Methods_______________________________*/
public:

	static Foil * curFoil(){return s_pCurFoil;}

	void AddFoil(Foil *pFoil);
	Polar* AddPolar(Polar *pPolar);
	void AddRecentFile(const QString &PathNAme);
	void ClientToGL(QPoint const &point, CVector &real);
	void closeEvent (QCloseEvent * event);
	void contextMenuEvent (QContextMenuEvent * event) ;
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
	Foil *DeleteFoil(Foil *pFoil, bool bAsk=true);
	void DeleteProject();
	void DeletePlane(void *pPlanePtr, bool bResultsOnly = false);
	void DeleteWing(void *pWingPtr, bool bResultsOnly = false);
	QColor GetColor(int type);
	OpPoint *GetOpp(double Alpha);
	Polar *GetPolar(QString m_FoilName, QString PolarName);
	void GLToClient(CVector const &real, QPoint &point);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	bool LoadSettings();
	bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	Foil* ReadFoilFile(QTextStream &ar);
	Foil* ReadPolarFile(QDataStream &ar);
	static void ReadStyleSheet(QString styleSheetName, QString &styleSheet);
	void RemoveOpPoint(bool bCurrent);
	void RenameFoil(Foil *pFoil);
	bool SaveProject(QString PathName="");
	void SaveSettings();
	void SelectOpPoint(OpPoint *pOpp);
	void SelectWOpp(double x);
	bool SerializeProject(QDataStream &ar, bool bIsStoring);
	bool SerializeUFOProject(QDataStream &ar);
	void SetCentralWidget();
	void SetCurrentFoil(Foil* pFoil);
	void SetGraphSettings(Graph *pGraph);
	void SetProjectName(QString PathName);
	void SetMenus();
	Foil* SetModFoil(Foil* pNewFoil, bool bKeepExistingFoil = false);
	QString ShortenFileName(QString &PathName);
	void UpdateFoils();
	void UpdatePolars();
	void UpdateOpps();
	void updateRecentFileActions();
	void UpdateUFOs();
	void UpdateView();
	void UpdateWPolars();
	void UpdateWOpps();
	void WritePolars(QDataStream &ar, Foil *pFoil=NULL);

	static QString versionName(){return s_VersionName;}
	static void SetSaveState(bool bSave);
	static Foil* foil(QString strFoilName);

/*___________________________________________Variables_______________________________*/

private:
	//the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
//	DisplaySettingsDlg *m_pDisplaySettingsDlg;
	SaveOptionsDlg *m_pSaveOptionsDlg;
	TranslatorDlg *m_pTranslatorDlg;
    RenameDlg *m_pRenameDlg;
    UnitsDlg *m_pUnitsDlg;

	void *m_pAFoil;     /**< A void pointer to the instance of the QAFoil application. The pointer will be cast to the QAFoil type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pMiarex;    /**< A void pointer to the instance of the QMiarex application. The pointer will be cast to the QMiarex type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pXInverse;  /**< A void pointer to the instance of the QXInverse application. The pointer will be cast to the QXInverse type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pXDirect;   /**< A void pointer to the instance of the QXDirect application. The pointer will be cast to the QXDirect type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pStabView;  /** < A void pointer to the instance of the StabViewDlg window. */
	
	static QPointer<MainFrame> _self;  /**< @todo need to discuss this one with Francesco */

	QStackedWidget *m_pctrlCentralWidget;  /** The stacked widget which is loaded at the center of the display area. The stack holds one TwoDWidget and one ThreeDWidget and sxwitches between the two depending on the user's request. */
	TwoDWidget *m_p2DWidget;        /** A pointer to the instance of the TwoDWidget which is used to perform 2d drawings */
	ThreeDWidget   *m_pGLWidget;  /** A pointer to the instance of the ThreeDWidget which is used to perform all 3D drawings */

	QDockWidget *m_pctrlXDirectWidget, *m_pctrlMiarexWidget, *m_pctrlAFoilWidget, *m_pctrlXInverseWidget;
	QDockWidget *m_pctrl3DScalesWidget, *m_pctrlStabViewWidget;

	QToolBar *m_pctrlXDirectToolBar;   /**< The tool bar container which holds the instance of the QXDirect application  */
	QToolBar *m_pctrlXInverseToolBar;
	QToolBar *m_pctrlMiarexToolBar;
	QToolBar *m_pctrlAFoilToolBar;

	//Common Menus
	QMenu * MainMenu;
	QMenu *fileMenu, *optionsMenu, *helpMenu;

	//AFoilMenus
	QMenu *AFoilViewMenu, *AFoilDesignMenu, *AFoilSplineMenu;
	QMenu *AFoilCtxMenu,*AFoilCurrentFoilMenu, *AFoilTableCtxMenu;

	//  XFoilAnalysis Menus
	QMenu * XDirectViewMenu;
	QMenu *FoilMenu, *CurGraphCtxMenu, *CurOppCtxMenu;
	QMenu *currentFoilMenu;
	QMenu *DesignMenu;
	QMenu *XFoilAnalysisMenu;
	QMenu *OpPointMenu, *CpGraphMenu, *currentOppMenu;
	QMenu *PolarMenu, *currentPolarMenu, *GraphPolarMenu, *CurPolarGraphMenu;
	QMenu *OperFoilCtxMenu, *OperPolarCtxMenu, *CurXFoilResults;

	//XInverse menu
	QMenu *XInverseViewMenu, *InverseFoilMenu, *InverseGraphMenu, *InverseContextMenu;

	//Miarex Menus
	QMenu *MiarexViewMenu;
	QMenu *MiarexAnalysisMenu;
	QMenu *UFOMenu, *currentUFOMenu, *CurWPlrMenu, *CurWOppMenu, *CurBodyMenu;
	QMenu *MiarexBodyMenu, *MiarexWPlrMenu, *MiarexWOppMenu;
	QMenu *WPlrGraphMenu,*WPlrCurGraphMenu, *WOppGraphMenu, *WOppCurGraphMenu;
	QMenu *WPlrCtxMenu, *WOppCtxMenu, *W3DCtxMenu, *WCpCtxMenu, *WTimeCtxMenu, *W3DStabCtxMenu;

	//MainFrame actions
	QAction *OnXDirectAct, *OnMiarexAct, *OnAFoilAct, *OnXInverseAct, *OnMixedInverseAct;
	QAction *openAct, *insertAct, *styleAct;
	QAction *saveAct, *saveProjectAsAct,*newProjectAct, *closeProjectAct, *saveOptionsAct;
	QAction *unitsAct;
	QAction *languageAct;
	QAction *exitAct;
	QAction *aboutAct, *guidelinesAct, *aboutQtAct;
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
	QAction *storeSplineAct, *newSplinesAct, *splineControlsAct, *exportSplinesToFileAct;
	QAction *InsertSplinePt, *RemoveSplinePt;
	QAction *AFoilTableColumns, *AFoilTableColumnWidths;
	QAction *AFoilLoadImage, *AFoilClearImage;

	//Miarex Actions
	QAction *WPolarAct, *WOppAct, *W3DAct, *CpViewAct, *StabTimeAct, *RootLocusAct;
	QAction *W3DPrefsAct, *W3DLightAct, *W3DScalesAct;
	QAction *DefineWingAct, *DefinePlaneAct, *EditUFOAct, *SaveUFOAsProject;
	QAction *renameCurUFO, *deleteCurUFO, *duplicateCurUFO;
	QAction *renameCurWPolar, *editWPolar,*exportCurWPolar, *resetCurWPolar;
	QAction *ShowPolarProps, *ShowWOppProps;
	QAction *deleteCurWPolar, *deleteCurWOpp;
	QAction *twoGraphs, *fourGraphs;
	QAction *Graph1,*Graph2,*Graph3,*Graph4;
	QAction *MiarexGraphDlg;
	QAction *highlightWOppAct;

	QAction *ResetWingGraphScale;
	QAction *MiarexPolarFilter;
	QAction *allWPolarGraphsScalesAct, *allWingGraphsScalesAct, *allWPolarGraphsSettings, *allWingGraphsSettings;
	QAction *hideAllWPlrs, *showAllWPlrs;
	QAction *hideUFOWPlrs, *showUFOWPlrs, *showUFOWPlrsOnly, *deleteUFOWPlrs;
	QAction *hideUFOWOpps, *showUFOWOpps, *deleteUFOWOpps;
	QAction *resetWOppLegend, *resetWPlrLegend;
	QAction *exportCurWOpp, *showCurWOppOnly, *hideAllWOpps, *showAllWOpps, *deleteAllWOpps;
	QAction *showAllWPlrOpps, *hideAllWPlrOpps, * deleteAllWPlrOpps;
	QAction *defineWPolar, *defineStabPolar, *advancedSettings;
	QAction *defineBody, *importBody, *exportBodyDef, *exportBodyGeom, *ManageBodies, *EditCurBody;
	QAction *HalfWingAct;
	QAction *showEllipticCurve, *showXCmRefLocation, *showStabCurve, *showFinCurve, *showWing2Curve;
	QAction *exporttoAVL, *resetWingScale, *scaleWingAct;
	QAction *ManageUFOs;
	QAction *m_pImportWPolar, *m_pUFOInertia;
//	QToolButton *m_pctrl3dView, *m_pctrlWPolarView, *m_pctrlWOppView, *m_pctrlCpView;
//	QToolButton *m_pctrlStabilityButton, *m_pctrlRootLocusButton;//, *m_pctrlModalViewButton;


	//XDirect Actions
	QAction *PolarsAct, *OpPointsAct, *deletePolar, *definePolar, *editCurPolar, *defineBatch, *resetCurPolar;
	QAction *MultiThreadedBatchAct;
	QAction *restoreToolbarsAct;
	QAction *exportCurPolar, *exportAllPolars, *hideFoilPolars, *showFoilPolars, *showFoilPolarsOnly, *saveFoilPolars,*deleteFoilPolars;
	QAction *showAllPolars, *hideAllPolars, *showCurOppOnly, *showAllOpPoints, *hideAllOpPoints, *exportPolarOpps;
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
	QAction *highlightOppAct;

	QComboBox *m_pctrlFoil, *m_pctrlPolar, * m_pctrlOpPoint;
	QComboBox *m_pctrlUFO, *m_pctrlWPolar, * m_pctrlWOpp;
	QRadioButton *m_pctrlFullInverse, *m_pctrlMixedInverse;
	static QLabel *m_pctrlProjectName;

	//XInverse Actions
	QAction *StoreFoil, *ExtractFoil, *InverseStyles, *InverseResetScale, *InverseInsertCtrlPt, *InverseRemoveCtrlPt;
	QAction *InvQInitial, *InvQSpec, *InvQViscous, *InvQPoints, *InvQReflected;
	QAction *InverseResetGraphScale, *XInverseGraphDlg, *InverseZoomIn, *InverseZoomX, *InverseZoomY;

	QStringList m_RecentFiles;


	static QList <void *> m_oaFoil;    /**< The array of void pointers to the Foil objects. */
	QList <void *> m_oaPolar;   /**< The array of void pointers to the foil Polar objects. */
	QList <void *> m_oaOpp;     /**< The array of void pointers to the foil operating point objects. */
	QList <void *> m_oaPlane;   /**< The array of void pointers to the Plane objects. */
	QList <void *> m_oaWing;    /**< The array of void pointers to the Wing objects. */
	QList <void *> m_oaWPolar;  /**< The array of void pointers to the WPolar objects. */
	QList <void *> m_oaWOpp;    /**< The array of void pointers to the WingOpp objects. */
	QList <void *> m_oaPOpp;    /**< The array of void pointers to the PlaneOpp objects. */
	QList <void *> m_oaBody;    /**< The array of void pointers to the Body objects. */


	enumApp m_iApp;                 /**< The identification number of the active app. */

	static bool s_bSaved;       /**< true if the project has not been modified since the last save operation. */
	bool m_bSaveOpps;           /**< true if foil operating points should be serialized in the project file */
	bool m_bSaveWOpps;          /**< true if wing operating points should be serialized in the project file */
	bool m_bSaveSettings;       /**< true if user-defined settings should be saved on exit. */
	bool m_bHighlightOpp;       /**< true if the active OpPoint should be highlighted on the polar curve. */
	bool m_bHighlightWOpp;      /**< true if the active WingOpp should be highlighted on the polar curve. */


	QString m_LanguageFilePath;
	QString m_ExportLastDirName, m_ImageDirName;
	QString m_FileName;         /**< The absolute path to the file of the current project. */

	QList <QColor> m_ColorList;

	QGraph m_RefGraph;//Reference setttings
	QString m_GraphExportFilter;

	enumImageFormat m_ImageFormat;   /**< The index of the type of image file which should be used. */

public:
	static Foil *s_pCurFoil;

	static bool s_bTrace;
	static bool s_bReverseZoom;        /**< true if the rolling forward zooms in rather than out. */
	static QFile *s_pTraceFile;
	static QString s_ProjectName;      /**< The Project's name. */
	static QString s_LastDirName;

	static QFont s_TextFont;
	static QColor s_TextColor;
	static QColor s_BackgroundColor;
	static bool s_bAlphaChannel;  /**< true if transparency is enabled for 3D displays. */
	static QString s_VersionName;
	static enumTextFileType s_ExportFileType;  /**< Defines if the list separator for the output text files should be a space or a comma. */

	static double s_mtoUnit;    /**< Conversion factor from meters to the user selected length unit. */
	static double s_mstoUnit;   /**< Conversion factor from m/s to the user selected speed unit. */
	static double s_m2toUnit;   /**< Conversion factor from square meters to the user selected area unit. */
	static double s_kgtoUnit;   /**< Conversion factor from kg to the user selected mass unit. */
	static double s_NtoUnit;    /**< Conversion factor from Newtons to the user selected force unit. */
	static double s_NmtoUnit;   /**< Conversion factor from N.m to the user selected unit for moments. */
	static int s_LengthUnit;    /**< The index of the user selected unit in the array of length units. @todo use an enumeration instead. */
	static int s_AreaUnit;      /**< The index of the user selected unit in the array of area units. */
	static int s_WeightUnit;    /**< The index of the user selected unit in the array of mass units. */
	static int s_SpeedUnit;     /**< The index of the user selected unit in the array of speed units. */
	static int s_ForceUnit;     /**< The index of the user selected unit in the array of force units. */
	static int s_MomentUnit;    /**< The index of the user selected unit in the array of moment units. */

	void *m_pGL3DScales;

};

#endif // MAINFRAME_H
 
