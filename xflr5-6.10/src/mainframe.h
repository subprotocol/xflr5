/****************************************************************************

	MainFrame Class

    Copyright (C) 2008-2014 Andre Deperrois adeperrois@xflr5.com

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
#include <QDir>
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
#include "graph/QGraph.h"

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
	friend class Objects3D;
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
	friend class wySettingsDlg;
	friend class ManageFoilsDlg;
	friend class ManagePlanesDlg;
	friend class InertiaDlg;
	friend class TranslatorDlg;
	friend class Settings;

	Q_OBJECT

public:
	MainFrame(QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainFrame();

	enumApp LoadXFLR5File(QString PathName);
	static MainFrame* self();

/*___________________________________________Methods_______________________________*/

public slots:
	void OnAFoil();
	void OnXDirect();
	void OnXInverse();
	void OnXInverseMixed();
	void OnMiarex();

private slots:
	void AboutQt();
	void aboutXFLR5();
	void OnCurFoilStyle();
	void OnExportCurGraph();
	void OnInsertProject();
	void OnNewProject();
	void OnLanguage();
	void OnLoadFile();
	void OnLogFile();
	void OnOpenGLInfo();
	void OnProjectModified();
	void OnResetCurGraphScales();
	void OnResetSettings();
	void OnRestoreToolbars();
	void OnSaveOptions();
	bool OnSaveProjectAs();
	void OnSaveViewToImageFile();
	void OnSelChangeFoil(int sel);
	void OnSelChangePolar(int sel);
	void OnSelChangeOpp(int sel);
	void OnSelChangePlane(int sel);
	void OnSelChangePlaneOpp(int sel);
	void OnSelChangeWPolar(int sel);
	void OnSaveProject();
	void OnStyleSettings();
	void OnUnits();
	void OnManageFoils();
	void OnSavePlaneAsProject();
	void openRecentFile();

protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void closeEvent (QCloseEvent * event);



public:
	void AddRecentFile(const QString &PathNAme);
	void ClientToGL(QPoint const &point, CVector &real);
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
	void DeleteProject(bool bClosing=false);
	void GLToClient(CVector const &real, QPoint &point);
	bool LoadSettings();
	bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	void* ReadFoilFile(QTextStream &ar);
	void ReadPolarFile(QDataStream &ar);
	bool SaveProject(QString PathName="");
	void SaveSettings();
	void SelectFoil(void *pFoilPtr);
	void SelectPolar(void *pPolarPtr);
	void SelectOpPoint(void *pOppPtr);
	void SelectPlane(void *pPlanePtr);
	void SelectWPolar(void *pWPolarPtr);
	void SelectPlaneOpp(void *pPlaneOppPtr);
	bool SerializeProjectWPA(QDataStream &ar, bool bIsStoring);
	bool SerializeProjectXFL(QDataStream &ar, bool bIsStoring);
	bool SerializePlaneProject(QDataStream &ar);
	void SetCentralWidget();
	void SetGraphSettings(QGraph *pGraph);
	void SetProjectName(QString PathName);
	void SetMenus();
	void SetupDataDir();
	QString ShortenFileName(QString &PathName);
	void UpdateFoilListBox();
	void UpdatePolarListBox();
	void UpdateOppListBox();
	void updateRecentFileActions();
	void UpdatePlaneListBox();
	void UpdateView();
	void UpdateWPolarListBox();
	void UpdatePOppListBox();
	void WritePolars(QDataStream &ar, void *pFoilPtr=NULL);

	static void SetSaveState(bool bSave);
	static void ReadStyleSheet(QString styleSheetName, QString &styleSheet);
	static QColor GetColor(int type);

/*___________________________________________Variables_______________________________*/

private:

	void *m_pAFoil;     /**< A void pointer to the instance of the QAFoil application. The pointer will be cast to the QAFoil type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pMiarex;    /**< A void pointer to the instance of the QMiarex application. The pointer will be cast to the QMiarex type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pXInverse;  /**< A void pointer to the instance of the QXInverse application. The pointer will be cast to the QXInverse type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pXDirect;   /**< A void pointer to the instance of the QXDirect application. The pointer will be cast to the QXDirect type at runtime. This is necessary to prevent loop includes of header files. */
	void *m_pStabView;  /** < A void pointer to the instance of the StabViewDlg window. */
	
	static QPointer<MainFrame> _self; /**< necessary for MacOS >*/

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
	QMenu *PlaneMenu, *currentPlaneMenu, *curWPlrMenu, *curWOppMenu;
	QMenu *MiarexWPlrMenu, *MiarexWOppMenu;
	QMenu *WPlrGraphMenu,*WPlrCurGraphMenu, *WOppGraphMenu, *WOppCurGraphMenu;
	QMenu *WPlrCtxMenu, *WOppCtxMenu, *W3DCtxMenu, *WCpCtxMenu, *WTimeCtxMenu, *W3DStabCtxMenu;

	//MainFrame actions
	QAction *OnXDirectAct, *OnMiarexAct, *OnAFoilAct, *OnXInverseAct, *OnMixedInverseAct;
	QAction *openAct, *insertAct, *styleAct;
	QAction *saveAct, *saveProjectAsAct,*newProjectAct, *closeProjectAct, *saveOptionsAct;
	QAction *unitsAct;
	QAction *languageAct;
	QAction *exitAct;
	QAction *aboutAct, *aboutQtAct, *openGLAct;
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *saveViewToImageFileAct, *resetSettingsAct;

	//AFoil Actions
	QAction *zoomInAct, *ResetXScaleAct, *ResetYScaleAct, *ResetXYScaleAct;
	QAction *zoomYAct, *zoomLessAct, *AFoilGridAct;
	QAction *AFoilDelete, *AFoilRename, *AFoilExport, *pAFoilDuplicateFoil;
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
	QAction *definePlaneAct, *editPlaneAct, *editWingAct, *editBodyAct, *savePlaneAsProjectAct;
	QAction *renameCurPlaneAct, *deleteCurPlane, *duplicateCurPlane;
	QAction *renameCurWPolar, *editWPolar, *editWPolarPts, *exportCurWPolar, *resetCurWPolar;
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
	QAction *hidePlaneWPlrs, *showPlaneWPlrs, *showPlaneWPlrsOnly, *deletePlaneWPlrs;
	QAction *hidePlaneWOpps, *showPlaneWOpps, *deletePlaneWOpps;
	QAction *resetWOppLegend, *resetWPlrLegend;
	QAction *exportCurWOpp, *showCurWOppOnly, *hideAllWOpps, *showAllWOpps, *deleteAllWOpps;
	QAction *showAllWPlrOpps, *hideAllWPlrOpps, * deleteAllWPlrOpps;
	QAction *defineWPolar, *defineStabPolar, *advancedSettings;
	QAction *halfWingAct;
	QAction *showEllipticCurve, *showXCmRefLocation, *showStabCurve, *showFinCurve, *showWing2Curve;
	QAction *exporttoAVL, *resetWingScale, *scaleWingAct;
	QAction *managePlanesAct;
	QAction *m_pImportWPolar, *m_pPlaneInertia;

	//XDirect Actions
	QAction *PolarsAct, *OpPointsAct, *deletePolar, *definePolar, *editCurPolar, *defineBatch, *resetCurPolar;
	QAction *MultiThreadedBatchAct;
	QAction *restoreToolbarsAct;
	QAction *exportCurPolar, *exportAllPolars, *hideFoilPolars, *showFoilPolars, *showFoilPolarsOnly, *saveFoilPolars,*deleteFoilPolars;
	QAction *showAllPolars, *hideAllPolars, *showCurOppOnly, *showAllOpPoints, *hideAllOpPoints, *exportPolarOpps;
	QAction *hideFoilOpps, *showFoilOpps, *deleteFoilOpps;
	QAction *hidePolarOpps, *showPolarOpps, *deletePolarOpps;
	QAction *exportCurOpp, *deleteCurOpp, *getOppProps, *getPolarProps;
	QAction *viewXFoilAdvanced, *viewLogFile, *showPanels, *showNeutralLine, *resetFoilScale, *showInviscidCurve;
	QAction *exportCurFoil, *deleteCurFoil, *renameCurFoil, *setCurFoilStyle;
	QAction *DerotateFoil, *NormalizeFoil, *RefineLocalFoil, *RefineGlobalFoil , *EditCoordsFoil, *ScaleFoil;
	QAction *SetTEGap, *SetLERadius, *SetFlap, *InterpolateFoils, *NacaFoils, *pDirectDuplicateCurFoil;
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
	QComboBox *m_pctrlPlane, *m_pctrlPlanePolar, * m_pctrlPlaneOpp;
	QRadioButton *m_pctrlFullInverse, *m_pctrlMixedInverse;
	static QLabel *m_pctrlProjectName;

	//XInverse Actions
	QAction *StoreFoil, *ExtractFoil, *InverseStyles, *InverseResetScale, *InverseInsertCtrlPt, *InverseRemoveCtrlPt;
	QAction *InvQInitial, *InvQSpec, *InvQViscous, *InvQPoints, *InvQReflected;
	QAction *InverseResetGraphScale, *XInverseGraphDlg, *InverseZoomIn, *InverseZoomX, *InverseZoomY;

	QStringList m_RecentFiles;




	enumApp m_iApp;                 /**< The identification number of the active app. */

	static bool s_bSaved;       /**< true if the project has not been modified since the last save operation. */
	bool m_bSaveOpps;           /**< true if foil operating points should be serialized in the project file */
	bool m_bSaveWOpps;          /**< true if wing operating points should be serialized in the project file */
	bool m_bSaveSettings;       /**< true if user-defined settings should be saved on exit. */
	bool m_bHighlightOpp;       /**< true if the active OpPoint should be highlighted on the polar curve. */
	bool m_bHighlightWOpp;      /**< true if the active WingOpp should be highlighted on the polar curve. */


	static QDir s_StylesheetDir;
	static QDir s_TranslationDir;
	static QString s_LanguageFilePath;

	QString m_ExportLastDirName, m_ImageDirName;
	QString m_FileName;         /**< The absolute path to the file of the current project. */

	static QList <QColor> s_ColorList;

	QString m_GraphExportFilter;

	enumImageFormat m_ImageFormat;   /**< The index of the type of image file which should be used. */

public:
	static bool s_bTrace;
	static QFile *s_pTraceFile;
	static QString s_ProjectName;      /**< The Project's name. */

	void *m_pGL3DScales;

	QTranslator m_Translator;  /**< the translator object; due to a Qt bug, need to load twice: once from the main function, once from the mainframe */

};

#endif // MAINFRAME_H
 
