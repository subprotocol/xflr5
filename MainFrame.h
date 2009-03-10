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
#include <QtGui/QMainWindow>
#include "GLWidget.h"
#include <QList>
#include "TwoDWidget.h"
#include "Params.h"
#include "Objects/Foil.h"
#include "Objects/Polar.h"
#include "Objects/OpPoint.h"
#include "Objects/Plane.h"

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
	friend class BatchDlg;
	friend class InterpolateFoilsDlg;
	friend class WingDlg;
	friend class WPolarDlg;
	friend class PlaneDlg;
	friend class PanelAnalysisDlg;
	friend class VLMAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3dViewDlg;
	friend class GL3DScales;

    Q_OBJECT

public:
	MainFrame(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainFrame();


private slots:
	void AboutQFLR5();
	void OnCurFoilStyle();
	void OnDeleteCurPolar();
	void OnGraphSettings();
	void OnExportCurGraph();
	void OnNewProject();
	void OnLoadFile();
	void OnLogFile();
	void OnRenameCurFoil();
	void OnResetCurGraphScales();
	void OnRestoreToolbars();
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
	void OnAFoil();
	void OnXDirect();
	void OnXInverse();
	void OnMiarex();
	void OnSaveUFOAsProject();
	void openRecentFile();
/*___________________________________________Methods_______________________________*/
private:
	void closeEvent (QCloseEvent * event);
	void contextMenuEvent (QContextMenuEvent * event) ;
//	void keyPressEvent(QKeyEvent *event);
	void CreateDockWindows();
	void CreateToolbars();
	void CreateStatusBar();
	void CreateActions();
	void CreateMenus();
	void CreateXDirectActions();
	void CreateXDirectMenus();
	void CreateXDirectToolbar();
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
	void LoadSettings();
	void RemoveOpPoint(bool bCurrent);
	void SaveSettings();
	bool SaveProject(QString PathName="");
	void SetCentralWidget();
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
	void WritePolars(QDataStream &ar, CFoil *pFoil=NULL);

	int LoadXFLR5File(QString PathName);
	bool LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	bool SerializeProject(QDataStream &ar, bool bIsStoring);
	bool SerializeUFOProject(QDataStream &ar, bool bIsStoring);
	bool DeleteFoil(CFoil *pFoil, bool bAsk=true);
	bool SelectFoil(CFoil *pFoil);
	bool SelectPolar(CPolar *pPolar);
	bool SelectOpPoint(OpPoint *pOpp);

	void AddFoil(CFoil *pFoil);
	CWing *GetWing(QString WingName);
	CFoil* GetFoil(QString strFoilName);
	CFoil* ReadFoilFile(QTextStream &ar);
	CFoil* ReadPolarFile(QDataStream &ar);
	CFoil* SetModFoil(CFoil* pNewFoil, bool bKeepExistingFoil = false);
	CPolar *GetPolar(QString strPolarName);
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
	void *m_pGL3DScales;
	TwoDWidget *m_p2DWidget;
	QStackedWidget *m_pctrlCentralWidget;
	GLWidget   *m_pGLWidget;

	QDockWidget *m_pctrlXDirectWidget, *m_pctrlMiarexWidget, *m_pctrlAFoilWidget, *m_pctrlXInverseWidget, *m_pctrl3DScalesWidget;

	QToolBar *m_pctrlXDirectToolBar;
	QToolBar *m_pctrlMiarexToolBar;
	QToolBar *m_pctrlAFoilToolBar;

	//Common Menus
	QMenu * MainMenu;
	QMenu *fileMenu, *helpMenu;

	//AFoilMenus
	QMenu *AFoilViewMenu, *AFoilDesignMenu;

	//  XFoilAnalysis Menus
	QMenu * XDirectViewMenu;
	QMenu *FoilMenu, *CurFoilCtxMenu, *CurPolarCtxMenu, *CurGraphCtxMenu, *CurFoilDesignMenu;
	QMenu *currentFoilMenu;
	QMenu *DesignMenu;
	QMenu *OpPointMenu;
	QMenu *PolarMenu, *currentPolarMenu, *GraphPolarMenu, *CurPolarGraphMenu;
	QMenu *OperFoilCtxMenu, *OperPolarCtxMenu, *CurXFoilResults;

	//Miarex Menus
	QMenu * MiarexViewMenu;
	QMenu *UFOMenu, *currentUFOMenu, *CurWPlrMenu, *CurWOppMenu;
	QMenu *MiarexBodyMenu, *MiarexWPlrMenu, *MiarexWOppMenu;
	QMenu *WPlrGraphMenu, *WOppGraphMenu, *WPlrCurGraphMenu;
	QMenu *WPlrCtxMenu, *WOppCtxMenu, *W3DCtxMenu;

	//MainFrame actions
	QAction *OnXDirectAct, *OnMiarexAct, *OnAFoilAct, *OnXInverseAct;
	QAction *openAct, *styleAct;
	QAction *saveAct, *saveProjectAsAct,*newProjectAct;
	QAction *unitsAct;
	QAction *exitAct;
	QAction *aboutAct, *aboutQtAct;

	//AFoil Actions
	QAction *storeSplineAct, *zoomInAct, *zoomOutAct, *zoomYAct, *zoomLessAct, *AFoilGridAct;
	QAction *AFoilSetTEGap, *AFoilSetLERadius, *AFoilSetFlap, *AFoilInterpolateFoils, *AFoilNacaFoils;
	QAction *AFoilDerotateFoil, *AFoilNormalizeFoil, *AFoilRefineLocalFoil, *AFoilRefineGlobalFoil;
	QAction *AFoilEditCoordsFoil, *AFoilScaleFoil;

	//Miarex Actions
	QAction *WPolarAct, *WOppAct, *W3DAct, *CpViewAct;
	QAction *DefineWingAct, *DefinePlaneAct, *EditUFOAct, *SaveUFOAsProject;
	QAction *renameCurUFO, *renameCurWPolar;
	QAction *deleteCurUFO, *deleteCurWPolar, *deleteCurWOpp;
	QAction *twoWingGraphs, *fourWingGraphs;
	QAction *WingGraph1,*WingGraph2,*WingGraph3,*WingGraph4;
	QAction *WPlrGraph1,*WPlrGraph2,*WPlrGraph3,*WPlrGraph4;
	QAction *twoWPlrGraphs, *allWPlrGraphs, *WGraphVariable;
	QAction *hideAllWPlrs, *showAllWPlrs, *hideUFOWPlrs, *showUFOWPlrs;
	QAction *resetWOppLegend, *resetWPlrLegend;
	QAction *exportCurWOpp, *showCurWOppOnly, *hideAllWOpps, *showAllWOpps, *deleteAllWOpps, *deleteAllWPlrOpps;
	QAction *defineWPolar, *advancedSettings;
	QAction *defineBody, *editCurBody, *importBody, *exportBody;

	//XDirect Actions
	QAction *PolarsAct, *OpPointsAct, *deletePolar, *definePolar, *defineBatch;
	QAction *restoreToolbarsAct;
	QAction *exportCurPolar, *hideFoilPolars, *showFoilPolars, *saveFoilPolars,*deleteFoilPolars;
	QAction *showAllPolars, *hideAllPolars, *showCurOppOnly, *showAllOpPoints, *hideAllOpPoints;
	QAction *exportCurOpp, *deleteCurOpp, *resetXFoil;
	QAction *viewXFoilAdvanced, *viewLogFile, *showPanels, *showNeutralLine, *resetFoilScale, *showInviscidCurve;
	QAction *exportCurFoil, *deleteCurFoil, *renameCurFoil, *setCurFoilStyle;
	QAction *DerotateFoil, *NormalizeFoil, *RefineLocalFoil, *RefineGlobalFoil , *EditCoordsFoil, *ScaleFoil;
	QAction *SetTEGap, *SetLERadius, *SetFlap, *InterpolateFoils, *NacaFoils;
	QAction *GraphDlgAction,*exportCurGraphAct, *resetCurGraphScales, *allPolarGraphsSettingsAct, *allPolarGraphsScales, *curPolarGraphVariableAct;
	QAction *TwoPolarGraphsAct, *AllPolarGraphsAct;
	QAction *PolarGraphAct[5];
	QAction *recentFileActs[MAXRECENTFILES];
	QAction *separatorAct;
	QAction *saveViewToImageFileAct;
	QAction *defineCpGraphSettings, *resetCpGraphScales, *exportCpGraphAct, *setQVarGraph, *setCpVarGraph;
	QAction *CurXFoilResExport, * CurXFoilCtPlot, *CurXFoilDbPlot, *CurXFoilDtPlot, *CurXFoilRtLPlot;
	QAction *CurXFoilRtPlot, *CurXFoilNPlot, *CurXFoilCdPlot, *CurXFoilCfPlot, *CurXFoilUePlot, *CurXFoilHPlot;

	QComboBox * m_pctrlFoil, * m_pctrlPolar, * m_pctrlOpPoint;
	QComboBox * m_pctrlUFO, * m_pctrlWPolar, * m_pctrlWOpp;
	QLabel *m_pctrlProjectName;

	QStringList m_RecentFiles;

	CFoil *m_pCurFoil;

	QList <void *> m_oaFoil;
	QList <void *> m_oaPolar;
	QList <void *> m_oaOpp;
	QList <void *> m_oaPlane;
	QList <void *> m_oaWing;
	QList <void *> m_oaWPolar;
	QList <void *> m_oaWOpp;
	QList <void *> m_oaPOpp;
	QList <void *> m_oaBody;

	CRectangle m_GLViewRect; // The OpenGl Viewport

	int m_iApp;

	bool m_bSaved;
	bool m_bSaveOpps;
	bool m_bSaveWOpps;

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

	QString m_ProjectName, m_FileName, m_LastDirName;
	QColor m_crColors[30];

	QColor m_BackgroundColor;
	QColor m_GraphBackColor;
	QColor m_TextColor;
	QColor m_BorderClr;
	QFont m_TextFont;

	int m_ImageFormat;
};

#endif // MAINFRAME_H
