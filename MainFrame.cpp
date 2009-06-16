/****************************************************************************

    MainFrame  Class
    Copyright (C) 2008 Andre Deperrois xflr5@yahoo.com

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

//#include <QDesktopWidget>
#include <QtGui>
#include "MainFrame.h"
#include "Globals.h"
#include "Design/AFoil.h"
#include "Miarex/Miarex.h"
#include "Miarex/GL3dWingDlg.h"
#include "Miarex/GL3dBodyDlg.h"
#include "Miarex/GL3DScales.h"
#include "Miarex/PlaneDlg.h"
#include "Misc/AboutQ5.h" 
#include "Misc/DisplaySettingsDlg.h"
#include "Misc/RenameDlg.h" 
#include "Misc/LinePickerDlg.h"
#include "Misc/UnitsDlg.h"
#include "Misc/SaveOptionsDlg.h"
#include "Graph/GraphDlg.h"
#include "XDirect/XDirect.h"
#include "XDirect/NacaFoilDlg.h"
#include "XDirect/InterpolateFoilsDlg.h"
#include "XDirect/CAddDlg.h"
#include "XDirect/TwoDPanelDlg.h"
#include "XDirect/FoilCoordDlg.h"
#include "XDirect/FoilGeomDlg.h"
#include "XDirect/TEGapDlg.h"
#include "XDirect/LEDlg.h"
#include "XInverse/XInverse.h"


MainFrame::MainFrame(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
	setWindowTitle("QFLR5");
	m_VersionName = "QFLR5 v5.00";

	m_SettingsFormat = 100551;

	m_bMaximized = true;
	m_LengthUnit  = 0;
	m_AreaUnit    = 1;
	m_WeightUnit  = 1;
	m_SpeedUnit   = 0;
	m_ForceUnit   = 0;
	m_MomentUnit  = 0;
	CreateDockWindows();

	m_ProjectName = "";

	m_BorderClr       = QColor(200,200,200);
	m_BackgroundColor = QColor(0, 10, 10);
	m_TextColor       = QColor(220,220,220);

	m_TextFont.setStyleHint(QFont::TypeWriter);
	m_TextFont.setFamily(m_TextFont.defaultFamily());

	m_ImageFormat = 2;

	m_mtoUnit   = 1.0;
	m_m2toUnit  = 1.0;
	m_kgtoUnit  = 1.0;
	m_mstoUnit  = 1.0;
	m_NtoUnit   = 1.0;
	m_NmtoUnit  = 1.0;

	m_bSaveOpps  = false;
	m_bSaveWOpps = true;

	m_StyleName = "Cleanlooks";

	LoadSettings();

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->SetAnalysisParams();
	CreateActions();
	CreateMenus();
	CreateToolbars();
	CreateStatusBar();

// second line
	m_crColors[0] = QColor(255,0,0),
	m_crColors[1] = QColor(0,255,0),
	m_crColors[2] = QColor(0,0,255),
	m_crColors[3] = QColor(255,255,0),
	m_crColors[4] = QColor(255,0,255),
	m_crColors[5] = QColor(0,255,255),

// third line
	m_crColors[6]  = QColor(200,60,60),
	m_crColors[7]  = QColor(0,160,0),
	m_crColors[8]  = QColor(100,100,240),
	m_crColors[9]  = QColor(170,170,0),
	m_crColors[10] = QColor(130,0,130),
	m_crColors[11] = QColor(0,130,130),

// fourth line
	m_crColors[12] = QColor(255,128,128),
	m_crColors[13] = QColor(0,255,128),
	m_crColors[14] = QColor(64,200,255),
	m_crColors[15] = QColor(170,170,100),
	m_crColors[16] = QColor(190,100,190),
	m_crColors[17] = QColor(100,170,170),

// fifth line
	m_crColors[18] = QColor(228,150,70),
	m_crColors[19] = QColor(170,255,170),
	m_crColors[20] = QColor(120,120,255),
	m_crColors[21] = QColor(228,228,128),
	m_crColors[22] = QColor(255,170,255),
	m_crColors[23] = QColor(170,255,255),

// first line
	m_crColors[24] = QColor(50,50,50),
	m_crColors[25] = QColor(90,90,90),
	m_crColors[26] = QColor(130,130,130),
	m_crColors[27] = QColor(170,170,170),
	m_crColors[28] = QColor(210,210,210),
	m_crColors[29] = QColor(255,255,255),

	m_pCurFoil = NULL;

	m_bSaved     = true;

	m_iApp = 0;
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlMiarexToolBar->hide();
	SetMenus();
}


MainFrame::~MainFrame()
{
}


void MainFrame::AboutQFLR5()
{
	AboutQ5 dlg;
	dlg.exec();
}


void MainFrame::AddFoil(CFoil *pFoil)
{

	CFoil *pOldFoil = GetFoil(pFoil->m_FoilName);
	if(pOldFoil)
	{
		DeleteFoil(pOldFoil, false);
	}

	bool IsInserted = false;

	for (int i=0; i<m_oaFoil.size(); i++)
	{
		pOldFoil = (CFoil*)m_oaFoil.at(i);
		if (pFoil->m_FoilName < pOldFoil->m_FoilName)
		{
			m_oaFoil.insert(i, pFoil);
			IsInserted = true;
			break;
		}
	}
	if (!IsInserted) m_oaFoil.append(pFoil);

	pFoil->InitFoil();
	m_pCurFoil = pFoil;

}


CPolar* MainFrame::AddPolar(CPolar *pPolar)
{
    if(!pPolar) return NULL;
    bool bExists   = false;
    bool bInserted = false;
    bool bFound;
    CPolar *pOldPlr;
    CPolar *pOld2Plr;
    QString strong;
    int i,j,k,l,p;

    for (i=0; i<m_oaPolar.size(); i++)
    {
	pOldPlr = (CPolar*)m_oaPolar.at(i);
	if (pOldPlr->m_PlrName  == pPolar->m_PlrName &&
		pOldPlr->m_FoilName == pPolar->m_FoilName) bExists = true;
    }
    while(!bInserted)
    {
		if(!bExists)
		{
			for (j=0; j<m_oaPolar.size(); j++)
			{
				pOldPlr = (CPolar*)m_oaPolar.at(j);

				if (pPolar->m_FoilName.compare(pOldPlr->m_FoilName, Qt::CaseInsensitive)<0)
				{
					m_oaPolar.insert(j, pPolar);
					bInserted = true;
					break;
				}

				else if (pPolar->m_FoilName == pOldPlr->m_FoilName)
				{
					if(pPolar->m_Type < pOldPlr->m_Type)
					{
						m_oaPolar.insert(j, pPolar);
						bInserted = true;
						break;
					}
					else if(pPolar->m_Type == pOldPlr->m_Type)
					{
						if (pPolar->m_Type !=4)
						{
							//sort by re Nbr
							if(pPolar->m_Reynolds < pOldPlr->m_Reynolds)
							{
								m_oaPolar.insert(j, pPolar);
								bInserted = true;
								break;
							}
						}
						else
						{
							//Type 4, sort by Alphas
							if(pPolar->m_ASpec < pOldPlr->m_ASpec)
							{
								m_oaPolar.insert(j, pPolar);
								bInserted = true;
								break;
							}
						}
					}
				}
			}
			if(!bInserted)
			{
				m_oaPolar.append(pPolar);
				bInserted = true;
			}
			return pPolar;
		}
		else
		{
			for (l=0; l<m_oaPolar.size(); l++)
			{
				pOldPlr = (CPolar*)m_oaPolar.at(l);
				if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
					pOldPlr->m_PlrName  == pPolar->m_PlrName)
				{
					p = 2;
					bFound = true;
					while(bFound)
					{
						strong = QString(" (%1)").arg( p);
						strong = pPolar->m_PlrName + strong;
						for (k=0; k<m_oaPolar.size(); k++)
						{
							bFound = false;
							pOld2Plr = (CPolar*)m_oaPolar.at(k);
							if (pOld2Plr->m_FoilName == pPolar->m_FoilName &&
								pOld2Plr->m_PlrName  == strong)
							{
								p++;
								bFound = true;
								break;
							}
						}
					}
					pPolar->m_PlrName = strong;
				}
			}
			bExists = false;
		}
    }
    return NULL;
}



void MainFrame::AddRecentFile(const QString &PathName)
{

	m_RecentFiles.removeAll(PathName);
	m_RecentFiles.prepend(PathName);
	while (m_RecentFiles.size() > MAXRECENTFILES)
		m_RecentFiles.removeLast();

	updateRecentFileActions();
}


void MainFrame::closeEvent (QCloseEvent * event)
{
//	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
//	pMiarex->m_GL3dView.hide();
//	pMiarex->m_GL3dView.close();

	if(!m_bSaved)
	{
		int resp = QMessageBox::question(this, "Exit", tr("Save the project before exit ?"),
										 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
										 QMessageBox::Yes);
		if(resp == QMessageBox::Yes)
		{
			if(!SaveProject(m_FileName))
			{
				event->ignore();
				return;
			}
		}
		else if (resp==QMessageBox::Cancel)
		{
			event->ignore();
			return;
		}
	}

	DeleteProject();
	SaveSettings();
	event->accept();//continue closing
}


void MainFrame::contextMenuEvent (QContextMenuEvent * event)
{
	QPoint CltPt = event->pos();
	QPoint ScreenPt = event->pos();
	ScreenPt.rx() += geometry().x();
	ScreenPt.ry() += geometry().y();
	switch(m_iApp)
	{
		case MIAREX:
		{
			W3DScalesAct->setChecked(m_pctrl3DScalesWidget->isVisible());
			QMiarex *pMiarex = (QMiarex*)m_pMiarex;
			pMiarex->m_pCurGraph = pMiarex->GetGraph(CltPt);
			if(pMiarex->m_iView==1)       WOppCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==2) WPlrCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==3) W3DCtxMenu->exec(ScreenPt);
			break;
		}
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->m_pCurGraph = pXDirect->GetGraph(CltPt);
			if(pXDirect->m_bPolar) 	OperPolarCtxMenu->exec(ScreenPt);
			else                    OperFoilCtxMenu->exec(ScreenPt);
			break;
		}
		case DIRECTDESIGN:
		{
//			QAFoil *pAFoil = (QAFoil*)m_pAFoil;
			AFoilCtxMenu->exec(ScreenPt);
			break;
		}
		case INVERSEDESIGN:
		{
//			QXInverse *pXInverse = (QXInverse*)m_pXInverse;
			InverseContextMenu->exec(ScreenPt);
			break;
		}
	}
}


void MainFrame::CreateActions()
{
	newProjectAct = new QAction(QIcon(":/images/new.png"), tr("New Project"), this);
	newProjectAct->setShortcut(tr("Ctrl+N"));
	newProjectAct->setStatusTip(tr("Save and close the current project, create a new project"));
	connect(newProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

	closeProjectAct = new QAction(QIcon(":/images/new.png"), tr("Close the Project"), this);
	closeProjectAct->setShortcut(tr("Ctrl+W"));
	closeProjectAct->setStatusTip(tr("Save and close the current project"));
	connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(OnLoadFile()));

	insertAct = new QAction(tr("&Insert Project..."), this);
	insertAct->setStatusTip(tr("Insert an existing project in the current project"));
	connect(insertAct, SIGNAL(triggered()), this, SLOT(OnInsertProject()));

	OnAFoilAct = new QAction(tr("&Direct Foil Design"), this);
	OnAFoilAct->setShortcut(tr("Ctrl+1"));
	OnAFoilAct->setStatusTip(tr("Open Foil Design application"));
	connect(OnAFoilAct, SIGNAL(triggered()), this, SLOT(OnAFoil()));

	OnXInverseAct = new QAction(tr("&XFoil Inverse Design"), this);
	OnXInverseAct->setShortcut(tr("Ctrl+3"));
	OnXInverseAct->setStatusTip(tr("Open XFoil inverse analysis application"));
	connect(OnXInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverse()));

	OnMixedInverseAct = new QAction(tr("&XFoil Mixed Inverse Design"), this);
	OnMixedInverseAct->setShortcut(tr("Ctrl+4"));
	OnMixedInverseAct->setStatusTip(tr("Open XFoil Mixed Inverse analysis application"));
	connect(OnMixedInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverseMixed()));

	OnXDirectAct = new QAction(tr("&XFoil Direct Analysis"), this);
	OnXDirectAct->setShortcut(tr("Ctrl+5"));
	OnXDirectAct->setStatusTip(tr("Open XFoil direct analysis application"));
	connect(OnXDirectAct, SIGNAL(triggered()), this, SLOT(OnXDirect()));

	OnMiarexAct = new QAction(tr("&Wing and Plane Design"), this);
	OnMiarexAct->setShortcut(tr("Ctrl+6"));
	OnMiarexAct->setStatusTip(tr("Open Wing/plane design and analysis application"));
	connect(OnMiarexAct, SIGNAL(triggered()), this, SLOT(OnMiarex()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
 	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Save the project to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(OnSaveProject()));

	saveProjectAsAct = new QAction(tr("Save Project As..."), this);
	saveProjectAsAct->setStatusTip(tr("Save the current project under a new name"));
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(OnSaveProjectAs()));

	saveOptionsAct = new QAction(tr("Save Options"), this);
	connect(saveOptionsAct, SIGNAL(triggered()), this, SLOT(OnSaveOptions()));

	unitsAct = new QAction(tr("Define units..."), this);
	unitsAct->setStatusTip(tr("Define the units for this project"));
	connect(unitsAct, SIGNAL(triggered()), this, SLOT(OnUnits()));

	restoreToolbarsAct	 = new QAction("Restore toolbars", this);
	restoreToolbarsAct->setStatusTip(tr("Restores the toolbars to their original state"));
	connect(restoreToolbarsAct, SIGNAL(triggered()), this, SLOT(OnRestoreToolbars()));

	saveViewToImageFileAct = new QAction("Save View to Image File", this);
	saveViewToImageFileAct->setShortcut(tr("Ctrl+I"));
	saveViewToImageFileAct->setStatusTip(tr("Saves the current view to a file on disk"));
	connect(saveViewToImageFileAct, SIGNAL(triggered()), this, SLOT(OnSaveViewToImageFile()));


	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	styleAct = new QAction("General Display Settings", this);
	connect(styleAct, SIGNAL(triggered()), this, SLOT(OnStyle()));


	exportCurGraphAct = new QAction(tr("Export Graph"), this);
	exportCurGraphAct->setStatusTip(tr("Export the current graph data to a text file"));
	connect(exportCurGraphAct, SIGNAL(triggered()), this, SLOT(OnExportCurGraph()));

	resetCurGraphScales = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales"), this);
	resetCurGraphScales->setStatusTip(tr("Restores the graph's x and y scales"));
	connect(resetCurGraphScales, SIGNAL(triggered()), this, SLOT(OnResetCurGraphScales()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	guidelinesAct = new QAction(tr("&Guidelines"), this);
	guidelinesAct->setStatusTip(tr("Show the guidelines for some help"));
	connect(guidelinesAct, SIGNAL(triggered()), this, SLOT(OnGuidelines()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("QFLR5"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(AboutQFLR5()));


	CreateAFoilActions();
	CreateXDirectActions();
	CreateXInverseActions();
	CreateMiarexActions();
}


void MainFrame::CreateAFoilActions()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	AFoilGridAct= new QAction("Grid Options", this);
	AFoilGridAct->setStatusTip(tr("Define the grid settings for the view"));
	connect(AFoilGridAct, SIGNAL(triggered()), pAFoil, SLOT(OnGrid()));

	storeSplineAct= new QAction(QIcon(":/images/OnStoreFoil.png"), tr("Store Splines"), this);
	storeSplineAct->setStatusTip(tr("Store the current splines in the foil database"));
	connect(storeSplineAct, SIGNAL(triggered()), pAFoil, SLOT(OnStoreSplines()));

	splineControlsAct= new QAction(tr("Splines Params"), this);
	splineControlsAct->setStatusTip(tr("Define parameters for the splines : degree, number of out points"));
	connect(splineControlsAct, SIGNAL(triggered()), pAFoil, SLOT(OnSplineControls()));

	exportSplinesToFileAct= new QAction(tr("Export Splines To File"), this);
	exportSplinesToFileAct->setStatusTip(tr("Define parameters for the splines : degree, number of out points"));
	connect(exportSplinesToFileAct, SIGNAL(triggered()), pAFoil, SLOT(OnExportSplinesToFile()));

	newSplinesAct= new QAction(tr("New Splines"), this);
	newSplinesAct->setStatusTip(tr("Reset the splines"));
	connect(newSplinesAct, SIGNAL(triggered()), pAFoil, SLOT(OnNewSplines()));

	zoomInAct= new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
	zoomInAct->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));
	connect(zoomInAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomIn()));

	ResetXScaleAct= new QAction(QIcon(":/images/OnResetXScale.png"), tr("Reset X Scale"), this);
	ResetXScaleAct->setStatusTip(tr("Resets the scale to fit the current screen width"));
	connect(ResetXScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetXScale()));

	UndoAFoilAct= new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
	UndoAFoilAct->setStatusTip(tr("Cancels the last modifiction made to the splines"));
	connect(UndoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnUndo()));

	RedoAFoilAct= new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
	RedoAFoilAct->setStatusTip(tr("Restores the last cancelled modifiction made to the splines"));
	connect(RedoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnRedo()));

	ShowAllFoils= new QAction(tr("Show All Foils"), this);
	connect(ShowAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnShowAllFoils()));
	HideAllFoils= new QAction(tr("Hide All Foils"), this);
	connect(HideAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnHideAllFoils()));


	AFoilDelete = new QAction(tr("Delete..."), this);
	connect(AFoilDelete, SIGNAL(triggered()), pAFoil, SLOT(OnDelete()));

	AFoilRename = new QAction(tr("Rename..."), this);
	connect(AFoilRename, SIGNAL(triggered()), this, SLOT(OnRenameCurFoil()));

	AFoilExport = new QAction(tr("Export..."), this);
	connect(AFoilExport, SIGNAL(triggered()), pAFoil, SLOT(OnExportCurFoil()));

	ShowCurrentFoil= new QAction(tr("Show Current Foil"), this);
	connect(ShowCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnShowCurrentFoil()));

	HideCurrentFoil= new QAction(tr("Hide Current Foil"), this);
	connect(HideCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnHideCurrentFoil()));

	ResetYScaleAct= new QAction(tr("Reset Y Scale"), this);
	connect(ResetYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetYScale()));

	ResetXYScaleAct= new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset Scales"), this);
	ResetXYScaleAct->setStatusTip(tr("Resets the x and y scales to screen size"));
	connect(ResetXYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetScales()));

	zoomLessAct= new QAction(QIcon(":/images/OnZoomLess.png"), tr("Zoom Less"), this);
	zoomLessAct->setStatusTip(tr("Zoom Less"));
	connect(zoomLessAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomLess()));

	zoomYAct= new QAction(QIcon(":/images/OnZoomYScale.png"), tr("Zoom Y Scale Only"), this);
	zoomYAct->setStatusTip(tr("Zoom Y scale Only"));
	connect(zoomYAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomYOnly()));

	AFoilDerotateFoil = new QAction(tr("De-rotate the Foil"), this);
	connect(AFoilDerotateFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilDerotateFoil()));

	AFoilNormalizeFoil = new QAction(tr("Normalize the Foil"), this);
	connect(AFoilNormalizeFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilNormalizeFoil()));

	AFoilRefineLocalFoil = new QAction(tr("Refine Locally"), this);
	connect(AFoilRefineLocalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilCadd()));

	AFoilRefineGlobalFoil = new QAction(tr("Refine Globally"), this);
	connect(AFoilRefineGlobalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilPanels()));

	AFoilEditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(AFoilEditCoordsFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilCoordinates()));

	AFoilScaleFoil = new QAction(tr("Scale camber and thickness"), this);
	connect(AFoilScaleFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilGeom()));

	AFoilSetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(AFoilSetTEGap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetTEGap()));

	AFoilSetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(AFoilSetLERadius, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetLERadius()));

	AFoilLECircle = new QAction(tr("Show LE Circle"), this);
	connect(AFoilLECircle, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilLECircle()));

	AFoilSetFlap = new QAction(tr("Set Flap"), this);
	connect(AFoilSetFlap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetFlap()));

	AFoilInterpolateFoils = new QAction(tr("Interpolate Foils"), this);
	connect(AFoilInterpolateFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilInterpolateFoils()));

	AFoilNacaFoils = new QAction(tr("Naca Foils"), this);
	connect(AFoilNacaFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilNacaFoils()));

}

void MainFrame::CreateAFoilMenus()
{
	AFoilViewMenu = menuBar()->addMenu(tr("&View"));
	AFoilViewMenu->addAction(ShowCurrentFoil);
	AFoilViewMenu->addAction(HideCurrentFoil);
	AFoilViewMenu->addAction(ShowAllFoils);
	AFoilViewMenu->addAction(HideAllFoils);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(zoomInAct);
	AFoilViewMenu->addAction(zoomLessAct);
	AFoilViewMenu->addAction(ResetXScaleAct);
	AFoilViewMenu->addAction(ResetYScaleAct);
	AFoilViewMenu->addAction(ResetXYScaleAct);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(AFoilLECircle);
	AFoilViewMenu->addAction(AFoilGridAct);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(restoreToolbarsAct);
	AFoilViewMenu->addAction(styleAct);
	AFoilViewMenu->addAction(saveViewToImageFileAct);

	AFoilDesignMenu = menuBar()->addMenu(tr("&Foil Design"));
	AFoilDesignMenu->addAction(AFoilNormalizeFoil);
	AFoilDesignMenu->addAction(AFoilDerotateFoil);
	AFoilDesignMenu->addAction(AFoilRefineLocalFoil);
	AFoilDesignMenu->addAction(AFoilRefineGlobalFoil);
	AFoilDesignMenu->addAction(AFoilEditCoordsFoil);
	AFoilDesignMenu->addAction(AFoilScaleFoil);
	AFoilDesignMenu->addAction(AFoilSetTEGap);
	AFoilDesignMenu->addAction(AFoilSetLERadius);
	AFoilDesignMenu->addAction(AFoilSetFlap);
	AFoilDesignMenu->addSeparator();
	AFoilDesignMenu->addAction(AFoilInterpolateFoils);
	AFoilDesignMenu->addAction(AFoilNacaFoils);

	AFoilSplineMenu = menuBar()->addMenu(tr("&Splines"));
	AFoilSplineMenu->addAction(UndoAFoilAct);
	AFoilSplineMenu->addAction(RedoAFoilAct);
	AFoilSplineMenu->addSeparator();
	AFoilSplineMenu->addAction(newSplinesAct);
	AFoilSplineMenu->addAction(splineControlsAct);
	AFoilSplineMenu->addAction(storeSplineAct);
	AFoilSplineMenu->addAction(exportSplinesToFileAct);

	//AFoil Context Menu
	AFoilCtxMenu = new QMenu("Context Menu",this);
	AFoilCurrentFoilMenu = AFoilCtxMenu->addMenu(tr("Current Foil"));
	AFoilCurrentFoilMenu->addAction(AFoilRename);
	AFoilCurrentFoilMenu->addAction(AFoilDelete);
	AFoilCurrentFoilMenu->addAction(AFoilExport);
	AFoilCurrentFoilMenu->addAction(ShowCurrentFoil);
	AFoilCurrentFoilMenu->addAction(HideCurrentFoil);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addMenu(AFoilDesignMenu);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addMenu(AFoilSplineMenu);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(ShowAllFoils);
	AFoilCtxMenu->addAction(HideAllFoils);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(ResetXScaleAct);
	AFoilCtxMenu->addAction(ResetYScaleAct);
	AFoilCtxMenu->addAction(ResetXYScaleAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(AFoilLECircle);
	AFoilCtxMenu->addAction(AFoilGridAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(saveViewToImageFileAct);
}


void MainFrame::CreateAFoilToolbar()
{
	m_pctrlZoomY = new QToolButton;
	m_pctrlZoomY->setDefaultAction(zoomYAct);
	m_pctrlZoomY->setCheckable(true);

	m_pctrlZoomIn = new QToolButton;
	m_pctrlZoomIn->setDefaultAction(zoomInAct);
	m_pctrlZoomIn->setCheckable(true);

	m_pctrlAFoilToolBar = addToolBar(tr("Foil"));
	m_pctrlAFoilToolBar->addAction(newProjectAct);
	m_pctrlAFoilToolBar->addAction(openAct);
	m_pctrlAFoilToolBar->addAction(saveAct);
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addWidget(m_pctrlZoomIn);
	m_pctrlAFoilToolBar->addAction(zoomLessAct);
	m_pctrlAFoilToolBar->addAction(ResetXYScaleAct);
	m_pctrlAFoilToolBar->addAction(ResetXScaleAct);
	m_pctrlAFoilToolBar->addWidget(m_pctrlZoomY);
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(UndoAFoilAct);
	m_pctrlAFoilToolBar->addAction(RedoAFoilAct);

	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(storeSplineAct);
}



void MainFrame::CreateDockWindows()
{
	m_pctrlXDirectWidget = new QDockWidget(tr("XDirect"), this);
	m_pctrlXDirectWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXDirectWidget);

	m_pctrlXInverseWidget = new QDockWidget(tr("XInverse"), this);
	m_pctrlXInverseWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXInverseWidget);

	m_pctrlMiarexWidget = new QDockWidget(tr("Miarex"), this);
	m_pctrlMiarexWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlMiarexWidget);

	m_pctrlAFoilWidget = new QDockWidget(tr("AFoil"), this);
	m_pctrlAFoilWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, m_pctrlAFoilWidget);

	m_p2DWidget = new TwoDWidget(this);
	m_pGLWidget = new GLWidget(this);

	m_pXDirect = new QXDirect(this);
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXDirectWidget->setWidget(pXDirect);
	m_pctrlXDirectWidget->setVisible(false);

	m_pXInverse = new QXInverse(this);
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	pXInverse->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXInverseWidget->setWidget(pXInverse);
	m_pctrlXInverseWidget->setVisible(false);

	m_pMiarex = new QMiarex(this);
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlMiarexWidget->setWidget(pMiarex);
	m_pctrlMiarexWidget->setVisible(false);

	m_pGL3DScales = new GL3DScales(this);
	GL3DScales * pGL3DScales = (GL3DScales*)m_pGL3DScales;
	pGL3DScales->setAttribute(Qt::WA_DeleteOnClose, false);

	m_pctrl3DScalesWidget = new QDockWidget(tr("3D Scales"), this);
	m_pctrl3DScalesWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, m_pctrl3DScalesWidget);
	m_pctrl3DScalesWidget->setWidget(pGL3DScales);
	m_pctrl3DScalesWidget->setVisible(false);

	m_pctrlCentralWidget = new QStackedWidget;
	m_pctrlCentralWidget->addWidget(m_p2DWidget);
	m_pctrlCentralWidget->addWidget(m_pGLWidget);

	setCentralWidget(m_pctrlCentralWidget);

	m_pAFoil  = new QAFoil(this);
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlAFoilWidget->setWidget(pAFoil);
	m_pctrlAFoilWidget->setVisible(false);

	m_p2DWidget->m_pXDirect   = pXDirect;
	m_p2DWidget->m_pXInverse  = pXInverse;
	m_p2DWidget->m_pMiarex    = pMiarex;
	m_p2DWidget->m_pAFoil     = pAFoil;
	m_p2DWidget->m_pMainFrame = this;

	QSizePolicy sizepol;
	sizepol.setHorizontalPolicy(QSizePolicy::Expanding);
	sizepol.setVerticalPolicy(QSizePolicy::Expanding);
	m_p2DWidget->setSizePolicy(sizepol);

	pMiarex->m_pMainFrame    = this;
	pMiarex->m_p2DWidget = m_p2DWidget;
	pMiarex->m_pGLWidget = m_pGLWidget;
	pMiarex->m_poaBody   = &m_oaBody;
	pMiarex->m_poaPlane  = &m_oaPlane;
	pMiarex->m_poaWing   = &m_oaWing;
	pMiarex->m_poaWPolar = &m_oaWPolar;
	pMiarex->m_poaWOpp   = &m_oaWOpp;
	pMiarex->m_poaPOpp   = &m_oaPOpp;
	pMiarex->m_poaFoil   = &m_oaFoil;
	pMiarex->m_poaPolar  = &m_oaPolar;

	pXDirect->m_pMainFrame             = this;
	pXDirect->m_p2DWidget              = m_p2DWidget;
	pXDirect->m_pCpGraph->m_pParent    = m_p2DWidget;
	pXDirect->m_pPolarGraph->m_pParent = m_p2DWidget;
	pXDirect->m_pTrGraph->m_pParent    = m_p2DWidget;
	pXDirect->m_pCzGraph->m_pParent    = m_p2DWidget;
	pXDirect->m_pCmGraph->m_pParent    = m_p2DWidget;
	pXDirect->m_pUserGraph->m_pParent  = m_p2DWidget;
	pXDirect->m_poaFoil  = &m_oaFoil;
	pXDirect->m_poaPolar = &m_oaPolar;
	pXDirect->m_poaOpp   = &m_oaOpp;

	pAFoil->m_pMainFrame = this;
	pAFoil->m_p2DWidget  = m_p2DWidget;
	pAFoil->m_poaFoil    = &m_oaFoil;
	pAFoil->m_pXFoil     = pXDirect->m_pXFoil;

	pXInverse->m_pMainFrame       = this;
	pXInverse->m_pXFoil           = pXDirect->m_pXFoil;
	pXInverse->m_p2DWidget        = m_p2DWidget;
	pXInverse->m_poaFoil          = &m_oaFoil;

	GL3dBodyDlg::s_pMainFrame = this;
	GL3dBodyDlg::s_pMiarex    = m_pMiarex;
	GL3dBodyDlg::s_poaBody    = &m_oaBody;

	GL3dWingDlg::s_pMainFrame = this;
	GL3dWingDlg::s_pMiarex    = m_pMiarex;
	GL3dWingDlg::s_poaFoil     = &m_oaFoil;

	GLWidget::s_pMainFrame = this;
	GLWidget::s_pMiarex = m_pMiarex;

	PlaneDlg::s_pMainFrame = this;
	PlaneDlg::s_pMiarex    = m_pMiarex;
	PlaneDlg::s_poaBody    = &m_oaBody;
	PlaneDlg::s_poaWing    = &m_oaWing;

	CWing::s_pMainFrame    = this;
	CWing::s_pMiarex       = m_pMiarex;

	pGL3DScales->m_pMainFrame    = this;
	pGL3DScales->m_pMiarex       = m_pMiarex;

	CBody::s_pMainFrame    = this;

	VLMAnalysisDlg::s_pMainFrame = this;
	VLMAnalysisDlg::s_pMiarex    = m_pMiarex;
	PanelAnalysisDlg::s_pMainFrame = this;
	PanelAnalysisDlg::s_pMiarex    = m_pMiarex;

	CPlane::s_pMainFrame   = this;
	CPlane::s_pMiarex      = m_pMiarex;

	BodyGridDlg::s_pMainFrame = this;
	NacaFoilDlg::s_pXFoil         = pXDirect->m_pXFoil;
	InterpolateFoilsDlg::s_pXFoil = pXDirect->m_pXFoil;
	CAddDlg::s_pXFoil             = pXDirect->m_pXFoil;
	TwoDPanelDlg::s_pXFoil        = pXDirect->m_pXFoil;
	FoilGeomDlg::s_pXFoil         = pXDirect->m_pXFoil;
	TEGapDlg::s_pXFoil            = pXDirect->m_pXFoil;
	LEDlg::s_pXFoil               = pXDirect->m_pXFoil;

	GraphDlg::s_pMainFrame = this;
	GraphDlg::s_ActivePage = 0;
}



void MainFrame::CreateMenus()
{
// Create common File, View and Help menus
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(insertAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveProjectAsAct);
	fileMenu->addAction(saveOptionsAct);
	fileMenu->addSeparator();
	fileMenu->addAction(OnAFoilAct);
	fileMenu->addAction(OnXInverseAct);
	fileMenu->addAction(OnXDirectAct);
	fileMenu->addAction(OnMiarexAct);
	separatorAct = fileMenu->addSeparator();
	for (int i = 0; i < MAXRECENTFILES; ++i)
		fileMenu->addAction(recentFileActs[i]);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	updateRecentFileActions();

	helpMenu = menuBar()->addMenu(tr("&?"));
	helpMenu->addAction(guidelinesAct);
	helpMenu->addAction(aboutAct);

	//Create Application-Specific Menus
	CreateXDirectMenus();
	CreateXInverseMenus();
	CreateMiarexMenus();
	CreateAFoilMenus();
}



void MainFrame::CreateMiarexActions()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	WOppAct = new QAction(QIcon(":/images/OnWOppView.png"), tr("OpPoint view"), this);
	WOppAct->setCheckable(true);
	WOppAct->setStatusTip(tr("Show Operating point view"));
	WOppAct->setShortcut(tr("F5"));
	connect(WOppAct, SIGNAL(triggered()), pMiarex, SLOT(OnWOpps()));

	WPolarAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar view"), this);
	WPolarAct->setCheckable(true);
	WPolarAct->setStatusTip(tr("Show Polar view"));
	WPolarAct->setShortcut(tr("F8"));
	connect(WPolarAct, SIGNAL(triggered()), pMiarex, SLOT(OnWPolars()));

	W3DAct = new QAction(QIcon(":/images/On3DView.png"), tr("3D view"), this);
	W3DAct->setCheckable(true);
	W3DAct->setStatusTip(tr("Show 3D view"));
	W3DAct->setShortcut(tr("F4"));
	connect(W3DAct, SIGNAL(triggered()), pMiarex, SLOT(On3DView()));

	CpViewAct = new QAction(QIcon(":/images/OnCpView.png"), tr("Cp view"), this);
	CpViewAct->setCheckable(true);
	CpViewAct->setStatusTip(tr("Show Cp view"));
	CpViewAct->setShortcut(tr("F9"));
	connect(CpViewAct, SIGNAL(triggered()), pMiarex, SLOT(OnCpView()));

	W3DPrefsAct = new QAction(tr("3D Color Preferences"), this);
	connect(W3DPrefsAct, SIGNAL(triggered()), pMiarex, SLOT(On3DPrefs()));

	W3DScalesAct = new QAction(tr("3D Scales"), this);
	W3DScalesAct->setCheckable(true);
	connect(W3DScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnGL3DScale()));

	W3DLightAct = new QAction(tr("3D Light Options"), this);
	connect(W3DLightAct, SIGNAL(triggered()), pMiarex, SLOT(OnSetupLight()));

	DefineWingAct = new QAction(tr("Define a New Wing"), this);
	DefineWingAct->setStatusTip(tr("Shows a dialogbox for editing a new wing definition"));
	DefineWingAct->setShortcut(tr("F3"));
	connect(DefineWingAct, SIGNAL(triggered()), pMiarex, SLOT(OnNewWing()));

	DefinePlaneAct = new QAction(tr("Define a New Plane"), this);
	DefinePlaneAct->setStatusTip(tr("Shows a dialogbox to create a new plane definition"));
	DefinePlaneAct->setShortcut(tr("Ctrl+F3"));
	connect(DefinePlaneAct, SIGNAL(triggered()), pMiarex, SLOT(OnNewPlane()));

	EditUFOAct = new QAction(tr("Edit..."), this);
	EditUFOAct->setStatusTip(tr("Shows a dialogbox to edit the currently selected wing or plane"));
	EditUFOAct->setShortcut(tr("Shift+F3"));
	connect(EditUFOAct, SIGNAL(triggered()), pMiarex, SLOT(OnEditUFO()));

	defineBody = new QAction(tr("Define a New Body"), this);
	defineBody->setStatusTip(tr("Shows a dialogbox for editing a new body definition"));
	defineBody->setShortcut(tr("F10"));
	connect(defineBody, SIGNAL(triggered()), pMiarex, SLOT(OnNewBody()));

	EditCurBody = new QAction(tr("Edit Current"), this);
	EditCurBody->setShortcut(tr("Shift+F10"));
	connect(EditCurBody, SIGNAL(triggered()), pMiarex, SLOT(OnEditCurBody()));

	exportBodyDef = new QAction(tr("Export Body Definition"), this);
	exportBodyDef->setStatusTip(tr("Export a body definition to a text file"));
	connect(exportBodyDef, SIGNAL(triggered()), pMiarex, SLOT(OnExportBodyDef()));

	exportBodyGeom = new QAction(tr("Export Body Geometry"), this);
	exportBodyGeom->setStatusTip(tr("Export a body geometry at different cross sections to a text file"));
	connect(exportBodyGeom, SIGNAL(triggered()), pMiarex, SLOT(OnExportBodyGeom()));

	importBody = new QAction(tr("Import Body"), this);
	importBody->setStatusTip(tr("Import a body definition from a text file"));
	connect(importBody, SIGNAL(triggered()), pMiarex, SLOT(OnImportBody()));

	ManageBodies = new QAction(tr("Manage Bodies"), this);
	ManageBodies->setStatusTip(tr("Manage the body list : Rename, Duplicate, Delete"));
	connect(ManageBodies, SIGNAL(triggered()), pMiarex, SLOT(OnManageBodies()));


	exporttoAVL = new QAction(tr("Export to AVL..."), this);
	connect(exporttoAVL, SIGNAL(triggered()), pMiarex, SLOT(OnExporttoAVL()));

	exportCurWOpp = new QAction(tr("Export..."), this);
	connect(exportCurWOpp, SIGNAL(triggered()), pMiarex, SLOT(OnExportCurWOpp()));

	resetWOppLegend = new QAction(tr("Reset Legend Position"), this);
	connect(resetWOppLegend, SIGNAL(triggered()), pMiarex, SLOT(OnResetWOppLegend()));

	resetWPlrLegend = new QAction(tr("Reset Legend Position"), this);
	connect(resetWPlrLegend, SIGNAL(triggered()), pMiarex, SLOT(OnResetWPlrLegend()));

	resetWingScale = new QAction(tr("Reset Wing Scale"), this);
	connect(resetWingScale, SIGNAL(triggered()), pMiarex, SLOT(OnResetWingScale()));

	scaleWingAct = new QAction(tr("Scale Wing"), this);
	connect(scaleWingAct, SIGNAL(triggered()), pMiarex, SLOT(OnScaleWing()));

	showCurWOppOnly = new QAction(tr("Show Current OpPoint Only"), this);
	showCurWOppOnly->setCheckable(true);
	connect(showCurWOppOnly, SIGNAL(triggered()), pMiarex, SLOT(OnCurWOppOnly()));
	showAllWOpps = new QAction(tr("Show All OpPoints"), this);
	connect(showAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWOpps()));
	hideAllWOpps = new QAction(tr("Hide All OpPoints"), this);
	connect(hideAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWOpps()));
	deleteAllWOpps = new QAction(tr("Delete All OpPoints"), this);
	connect(deleteAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteAllWOpps()));

	showAllWPlrOpps = new QAction(tr("Show Associated OpPoints"), this);
	connect(showAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWPlrOpps()));
	hideAllWPlrOpps = new QAction(tr("Hide Associated OpPoints"), this);
	connect(hideAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWPlrOpps()));
	deleteAllWPlrOpps = new QAction(tr("Delete Associated OpPoints"), this);
	connect(deleteAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteAllWPlrOpps()));

	showEllipticCurve = new QAction(tr("Show Elliptic Curve"), this);
	showEllipticCurve->setCheckable(true);
	connect(showEllipticCurve, SIGNAL(triggered()), pMiarex, SLOT(OnShowEllipticCurve()));

	showXCmRefLocation = new QAction(tr("Show XCmRef location"), this);
	showXCmRefLocation->setCheckable(true);
	connect(showXCmRefLocation, SIGNAL(triggered()), pMiarex, SLOT(OnShowXCmRef()));

	showStabCurve = new QAction(tr("Show Stab Curve"), this);
	showStabCurve->setCheckable(true);
	connect(showStabCurve, SIGNAL(triggered()), pMiarex, SLOT(OnStabCurve()));

	showFinCurve = new QAction(tr("Show Fin Curve"), this);
	showFinCurve->setCheckable(true);
	connect(showFinCurve, SIGNAL(triggered()), pMiarex, SLOT(OnFinCurve()));

	showWing2Curve = new QAction(tr("Show Second Wing Curve"), this);
	showWing2Curve->setCheckable(true);
	connect(showWing2Curve, SIGNAL(triggered()), pMiarex, SLOT(OnWing2Curve()));

	defineWPolar = new QAction(tr("Define an Analysis"), this);
	defineWPolar->setShortcut(tr("F6"));
	connect(defineWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDefineWPolar()));

	defineCtrlPolar = new QAction(tr("Define a Control Analysis"), this);
	defineCtrlPolar->setShortcut(tr("Ctrl+F6"));
	connect(defineCtrlPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDefineCtrlPolar()));

	MiarexGraphDlg = new QAction(tr("Define Graph Settings\t(G)"), this);
	connect(MiarexGraphDlg, SIGNAL(triggered()), pMiarex, SLOT(OnGraphSettings()));

	twoWingGraphs = new QAction(tr("Two OpPoint Graphs\t(T)"), this);
	twoWingGraphs->setCheckable(true);
	connect(twoWingGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnTwoWingGraphs()));
	fourWingGraphs = new QAction(tr("All OpPoint Graphs\t(A)"), this);
	fourWingGraphs->setCheckable(true);
	connect(fourWingGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnFourWingGraphs()));
	WingGraph1 = new QAction(tr("Wing Graph 1\t(1)"), this);
	WingGraph1->setCheckable(true);
	connect(WingGraph1, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWingGraph1()));
	WingGraph2 = new QAction(tr("Wing Graph 2\t(2)"), this);
	WingGraph2->setCheckable(true);
	connect(WingGraph2, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWingGraph2()));
	WingGraph3 = new QAction(tr("Wing Graph 3\t(3)"), this);
	WingGraph3->setCheckable(true);
	connect(WingGraph3, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWingGraph3()));
	WingGraph4 = new QAction(tr("Wing Graph 4\t(4)"), this);
	WingGraph4->setCheckable(true);
	connect(WingGraph4, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWingGraph4()));

	twoWPlrGraphs = new QAction(tr("Two Polar Graphs\t(T)"), this);
	twoWPlrGraphs->setCheckable(true);
	connect(twoWPlrGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnTwoWPlrGraphs()));
	allWPlrGraphs = new QAction(tr("All Polar Graphs\t(A)"), this);
	allWPlrGraphs->setCheckable(true);
	connect(allWPlrGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnFourWPlrGraphs()));
	WPlrGraph1 = new QAction(tr("Polar Graph 1\t(1)"), this);
	WPlrGraph1->setCheckable(true);
	connect(WPlrGraph1, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWPlrGraph1()));
	WPlrGraph2 = new QAction(tr("Polar Graph 2\t(2)"), this);
	WPlrGraph2->setCheckable(true);
	connect(WPlrGraph2, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWPlrGraph2()));
	WPlrGraph3 = new QAction(tr("Polar Graph 3\t(3)"), this);
	WPlrGraph3->setCheckable(true);
	connect(WPlrGraph3, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWPlrGraph3()));
	WPlrGraph4 = new QAction(tr("Polar Graph 4\t(4)"), this);
	WPlrGraph4->setCheckable(true);
	connect(WPlrGraph4, SIGNAL(triggered()), pMiarex, SLOT(OnSingleWPlrGraph4()));

	ResetWingGraphScale = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales"), this);
	connect(ResetWingGraphScale, SIGNAL(triggered()), pMiarex, SLOT(OnResetWingGraphScale()));

	allWingGraphsScalesAct = new QAction(tr("Reset All Graph Scales"), this);
	connect(allWingGraphsScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnAllWingGraphScales()));

	allWPolarGraphsScalesAct = new QAction(tr("Reset All Graph Scales"), this);
	connect(allWPolarGraphsScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnAllWPolarGraphScales()));

	allWingGraphsSettings = new QAction(tr("All Graph Settings"), this);
	connect(allWingGraphsSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAllWingGraphSettings()));

	allWPolarGraphsSettings = new QAction(tr("All Graph Settings"), this);
	connect(allWPolarGraphsSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAllWPolarGraphSettings()));

	hideUFOWPlrs = new QAction(tr("Hide Associated Polars"), this);
	connect(hideUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnHideUFOWPolars()));
	showUFOWPlrs = new QAction(tr("Show Associated Polars"), this);
	connect(showUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnShowUFOWPolars()));
	deleteUFOWPlrs = new QAction(tr("Delete Associated Polars"), this);
	connect(deleteUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteUFOWPolars()));

	hideAllWPlrs = new QAction(tr("Hide All Polars"), this);
	connect(hideAllWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWPolars()));
	showAllWPlrs = new QAction(tr("Show All Polars"), this);
	connect(showAllWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWPolars()));

	hideUFOWOpps = new QAction(tr("Hide Associated OpPoints"), this);
	connect(hideUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideUFOWOpps()));
	showUFOWOpps = new QAction(tr("Show Associated OpPoints"), this);
	connect(showUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowUFOWOpps()));
	deleteUFOWOpps = new QAction(tr("Delete Associated OpPoints"), this);
	connect(deleteUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteUFOWOpps()));

	renameCurUFO = new QAction(tr("Rename..."), this);
	renameCurUFO->setShortcut(tr("F2"));
	connect(renameCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnRenameCurUFO()));

	deleteCurUFO = new QAction(tr("Delete..."), this);
	connect(deleteCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurUFO()));

	duplicateCurUFO = new QAction(tr("Duplicate..."), this);
	connect(duplicateCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnDuplicateCurUFO()));

	SaveUFOAsProject = new QAction(tr("Save as Project..."), this);
	connect(SaveUFOAsProject, SIGNAL(triggered()), this, SLOT(OnSaveUFOAsProject()));

	renameCurWPolar = new QAction(tr("Rename..."), this);
	connect(renameCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnRenameCurWPolar()));
	editCurWPolar = new QAction(tr("Edit ..."), this);
	connect(editCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnEditCurWPolar()));
	exportCurWPolar = new QAction(tr("Export ..."), this);
	connect(exportCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnExportCurWPolar()));
	resetCurWPolar = new QAction(tr("Reset ..."), this);
	connect(resetCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnResetCurWPolar()));
	deleteCurWPolar = new QAction(tr("Delete ..."), this);
	connect(deleteCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurWPolar()));

	deleteCurWOpp = new QAction(tr("Delete..."), this);
	connect(deleteCurWOpp, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurWOpp()));

	advancedSettings = new QAction(tr("Advanced Settings..."), this);
	connect(advancedSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAdvancedSettings()));
}


void MainFrame::CreateMiarexMenus()
{
	//MainMenu for Miarex Application
	MiarexViewMenu = menuBar()->addMenu(tr("&View"));
	MiarexViewMenu->addAction(WOppAct);
	MiarexViewMenu->addAction(WPolarAct);
	MiarexViewMenu->addAction(W3DAct);
	MiarexViewMenu->addAction(CpViewAct);
	MiarexViewMenu->addSeparator();
	MiarexViewMenu->addAction(unitsAct);
	MiarexViewMenu->addAction(W3DPrefsAct);
	MiarexViewMenu->addAction(W3DScalesAct);
	MiarexViewMenu->addAction(W3DLightAct);
	MiarexViewMenu->addSeparator();
	MiarexViewMenu->addAction(restoreToolbarsAct);
	MiarexViewMenu->addAction(styleAct);
	MiarexViewMenu->addAction(saveViewToImageFileAct);


	UFOMenu = menuBar()->addMenu(tr("&Wing-Plane"));
	UFOMenu->addAction(DefineWingAct);
	UFOMenu->addAction(DefinePlaneAct);
	currentUFOMenu = UFOMenu->addMenu("Current UFO");
	currentUFOMenu->addAction(EditUFOAct);
	currentUFOMenu->addAction(renameCurUFO);
	currentUFOMenu->addAction(duplicateCurUFO);
	currentUFOMenu->addAction(deleteCurUFO);
	currentUFOMenu->addAction(SaveUFOAsProject);
	currentUFOMenu->addSeparator();
	currentUFOMenu->addAction(scaleWingAct);
	currentUFOMenu->addSeparator();
	currentUFOMenu->addAction(exporttoAVL);
	currentUFOMenu->addSeparator();
	currentUFOMenu->addAction(hideUFOWPlrs);
	currentUFOMenu->addAction(showUFOWPlrs);
	currentUFOMenu->addAction(deleteUFOWPlrs);
	currentUFOMenu->addSeparator();
	currentUFOMenu->addAction(hideUFOWOpps);
	currentUFOMenu->addAction(showUFOWOpps);
	currentUFOMenu->addAction(deleteUFOWOpps);
	currentUFOMenu->addSeparator();
	currentUFOMenu->addAction(resetWingScale);

	MiarexBodyMenu = menuBar()->addMenu("Body");
	MiarexBodyMenu->addAction(defineBody);
	MiarexBodyMenu->addAction(importBody);
	CurBodyMenu = MiarexBodyMenu->addMenu("Current Body");
	CurBodyMenu->addAction(EditCurBody);
	CurBodyMenu->addAction(exportBodyDef);
	CurBodyMenu->addAction(exportBodyGeom);
	MiarexBodyMenu->addSeparator();
	MiarexBodyMenu->addAction(ManageBodies);

	MiarexWPlrMenu = menuBar()->addMenu(tr("&Polars"));
	MiarexWPlrMenu->addAction(defineWPolar);
	MiarexWPlrMenu->addAction(defineCtrlPolar);
	CurWPlrMenu = MiarexWPlrMenu->addMenu("Current Polar");
	CurWPlrMenu->addAction(editCurWPolar);
	CurWPlrMenu->addAction(renameCurWPolar);
	CurWPlrMenu->addAction(deleteCurWPolar);
	CurWPlrMenu->addAction(exportCurWPolar);
	CurWPlrMenu->addAction(resetCurWPolar);
	CurWPlrMenu->addSeparator();
	CurWPlrMenu->addAction(showAllWPlrOpps);
	CurWPlrMenu->addAction(hideAllWPlrOpps);
	CurWPlrMenu->addAction(deleteAllWPlrOpps);
	MiarexWPlrMenu->addSeparator();
	MiarexWPlrMenu->addAction(hideAllWPlrs);
	MiarexWPlrMenu->addAction(showAllWPlrs);
	MiarexWPlrMenu->addSeparator();
	WPlrGraphMenu = MiarexWPlrMenu->addMenu("Graphs");
	WPlrGraphMenu->addAction(WPlrGraph1);
	WPlrGraphMenu->addAction(WPlrGraph2);
	WPlrGraphMenu->addAction(WPlrGraph3);
	WPlrGraphMenu->addAction(WPlrGraph4);
	WPlrGraphMenu->addSeparator();
	WPlrGraphMenu->addAction(twoWPlrGraphs);
	WPlrGraphMenu->addAction(allWPlrGraphs);
	WPlrGraphMenu->addSeparator();
	WPlrGraphMenu->addAction(allWPolarGraphsSettings);
	WPlrGraphMenu->addAction(allWPolarGraphsScalesAct);
	WPlrGraphMenu->addAction(resetWPlrLegend);


	MiarexWOppMenu = menuBar()->addMenu(tr("&OpPoint"));
	CurWOppMenu = MiarexWOppMenu->addMenu("Current OpPoint");
	CurWOppMenu->addAction(exportCurWOpp);
	CurWOppMenu->addAction(deleteCurWOpp);
	MiarexWOppMenu->addSeparator();
	MiarexWOppMenu->addAction(showCurWOppOnly);
	MiarexWOppMenu->addAction(showAllWOpps);
	MiarexWOppMenu->addAction(hideAllWOpps);
	MiarexWOppMenu->addAction(deleteAllWOpps);
	MiarexWOppMenu->addSeparator();
	MiarexWOppMenu->addAction(showEllipticCurve);
	MiarexWOppMenu->addAction(showXCmRefLocation);
	MiarexWOppMenu->addAction(showWing2Curve);
	MiarexWOppMenu->addAction(showStabCurve);
	MiarexWOppMenu->addAction(showFinCurve);
	MiarexWOppMenu->addSeparator();
	WOppCurGraphMenu = MiarexWOppMenu->addMenu("Current Graph");
	WOppCurGraphMenu->addAction(MiarexGraphDlg);
	WOppCurGraphMenu->addAction(exportCurGraphAct);

	WOppGraphMenu = MiarexWOppMenu->addMenu("Graphs");
	WOppGraphMenu->addAction(WingGraph1);
	WOppGraphMenu->addAction(WingGraph2);
	WOppGraphMenu->addAction(WingGraph3);
	WOppGraphMenu->addAction(WingGraph4);
	WOppGraphMenu->addSeparator();
	WOppGraphMenu->addAction(twoWingGraphs);
	WOppGraphMenu->addAction(fourWingGraphs);
	WOppGraphMenu->addSeparator();
	WOppGraphMenu->addAction(allWingGraphsSettings);
	WOppGraphMenu->addAction(allWingGraphsScalesAct);
	WOppGraphMenu->addAction(resetWOppLegend);

	MiarexWOppMenu->addSeparator();
	MiarexWOppMenu->addAction(advancedSettings);
	MiarexWOppMenu->addAction(viewLogFile);

	//WOpp View Context Menu
	WOppCtxMenu = new QMenu("Context Menu",this);
	WOppCtxMenu->addMenu(currentUFOMenu);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addMenu(CurBodyMenu);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addMenu(CurWPlrMenu);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addMenu(CurWOppMenu);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addAction(showCurWOppOnly);
	WOppCtxMenu->addAction(showAllWOpps);
	WOppCtxMenu->addAction(hideAllWOpps);
	WOppCtxMenu->addAction(deleteAllWOpps);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addMenu(WOppCurGraphMenu);
	WOppCtxMenu->addMenu(WOppGraphMenu);
	WOppCtxMenu->addAction(ResetWingGraphScale);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addAction(showEllipticCurve);
	WOppCtxMenu->addAction(showXCmRefLocation);
	WOppCtxMenu->addAction(showWing2Curve);
	WOppCtxMenu->addAction(showStabCurve);
	WOppCtxMenu->addAction(showFinCurve);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addAction(resetWingScale);
	WOppCtxMenu->addSeparator();
	WOppCtxMenu->addAction(advancedSettings);
	WOppCtxMenu->addAction(viewLogFile);
	WOppCtxMenu->addAction(saveViewToImageFileAct);


	//Polar View Context Menu
	WPlrCtxMenu = new QMenu("Context Menu",this);
	WPlrCtxMenu->addMenu(currentUFOMenu);
	WPlrCtxMenu->addSeparator();
	WPlrCtxMenu->addMenu(CurBodyMenu);
	WPlrCtxMenu->addSeparator();
	WPlrCtxMenu->addMenu(CurWPlrMenu);
	WPlrCtxMenu->addSeparator();
	WPlrCtxMenu->addMenu(WPlrGraphMenu);
	WPlrCurGraphMenu = WPlrCtxMenu->addMenu("Current Graph");
	WPlrCurGraphMenu->addAction(MiarexGraphDlg);
	WPlrCurGraphMenu->addAction(exportCurGraphAct);
	WPlrCtxMenu->addAction(resetCurGraphScales);
	WPlrCtxMenu->addSeparator();
	WPlrCtxMenu->addAction(hideAllWPlrs);
	WPlrCtxMenu->addAction(showAllWPlrs);
	WPlrCtxMenu->addSeparator();
	WPlrCtxMenu->addAction(saveViewToImageFileAct);


	//W3D View Context Menu
	W3DCtxMenu = new QMenu("Context Menu",this);
	W3DCtxMenu->addMenu(currentUFOMenu);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addMenu(CurBodyMenu);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addMenu(CurWPlrMenu);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addMenu(CurWOppMenu);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addAction(showCurWOppOnly);
	W3DCtxMenu->addAction(showAllWOpps);
	W3DCtxMenu->addAction(hideAllWOpps);
	W3DCtxMenu->addAction(deleteAllWOpps);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addAction(W3DScalesAct);
	W3DCtxMenu->addAction(W3DLightAct);
	W3DCtxMenu->addSeparator();
	W3DCtxMenu->addAction(saveViewToImageFileAct);
}



void MainFrame::CreateMiarexToolbar()
{
	m_pctrlUFO    = new QComboBox();
	m_pctrlWPolar = new QComboBox;
	m_pctrlWOpp   = new QComboBox;

	m_pctrl3dView = new QToolButton;
	m_pctrl3dView->setDefaultAction(W3DAct);
	m_pctrl3dView->setCheckable(true);
	m_pctrlWPolarView = new QToolButton;
	m_pctrlWPolarView->setDefaultAction(WPolarAct);
	m_pctrlWPolarView->setCheckable(true);
	m_pctrlWOppView = new QToolButton;
	m_pctrlWOppView->setDefaultAction(WOppAct);
	m_pctrlWOppView->setCheckable(true);
	m_pctrlCpView = new QToolButton;
	m_pctrlCpView->setDefaultAction(CpViewAct);
	m_pctrlCpView->setCheckable(true);

	m_pctrlUFO->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlWPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlWOpp->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlUFO->setMinimumWidth(200);
	m_pctrlWPolar->setMinimumWidth(200);
	m_pctrlWOpp->setMinimumWidth(30);
	m_pctrlUFO->setInsertPolicy(QComboBox::InsertAlphabetically);
	m_pctrlWPolar->setInsertPolicy(QComboBox::InsertAlphabetically);
	m_pctrlWOpp->setInsertPolicy(QComboBox::InsertAlphabetically);

	m_pctrlMiarexToolBar = addToolBar(tr("UFO"));
	m_pctrlMiarexToolBar->addAction(newProjectAct);
	m_pctrlMiarexToolBar->addAction(openAct);
	m_pctrlMiarexToolBar->addAction(saveAct);
	m_pctrlMiarexToolBar->addSeparator();
	m_pctrlMiarexToolBar->addWidget(m_pctrlWOppView);
	m_pctrlMiarexToolBar->addWidget(m_pctrlWPolarView);
	m_pctrlMiarexToolBar->addWidget(m_pctrl3dView);
	m_pctrlMiarexToolBar->addWidget(m_pctrlCpView);
	m_pctrlMiarexToolBar->addSeparator();
	m_pctrlMiarexToolBar->addWidget(m_pctrlUFO);
	m_pctrlMiarexToolBar->addWidget(m_pctrlWPolar);
	m_pctrlMiarexToolBar->addWidget(m_pctrlWOpp);

	connect(m_pctrlUFO,    SIGNAL(activated(int)), this, SLOT(OnSelChangeUFO(int)));
	connect(m_pctrlWPolar, SIGNAL(activated(int)), this, SLOT(OnSelChangeWPolar(int)));
	connect(m_pctrlWOpp,   SIGNAL(activated(int)), this, SLOT(OnSelChangeWOpp(int)));
}


void MainFrame::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
	m_pctrlProjectName = new QLabel(" ");
	m_pctrlProjectName->setMinimumWidth(200);
	statusBar()->addPermanentWidget(m_pctrlProjectName);
}


void MainFrame::CreateToolbars()
{
	CreateXDirectToolbar();
	CreateXInverseToolbar();
	CreateMiarexToolbar();
	CreateAFoilToolbar();
}


void MainFrame::CreateXDirectToolbar()
{
	m_pctrlFoil    = new QComboBox();
	m_pctrlPolar   = new QComboBox;
	m_pctrlOpPoint = new QComboBox;

	m_pctrlPolarView = new QToolButton;
	m_pctrlPolarView->setDefaultAction(PolarsAct);
	m_pctrlPolarView->setCheckable(true);
	m_pctrlOppView = new QToolButton;
	m_pctrlOppView->setDefaultAction(OpPointsAct);
	m_pctrlOppView->setCheckable(true);

	m_pctrlFoil->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlOpPoint->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlFoil->setMinimumWidth(200);
	m_pctrlPolar->setMinimumWidth(200);
	m_pctrlOpPoint->setMinimumWidth(30);

	m_pctrlXDirectToolBar = addToolBar(tr("Foil"));
	m_pctrlXDirectToolBar->addAction(newProjectAct);
	m_pctrlXDirectToolBar->addAction(openAct);
	m_pctrlXDirectToolBar->addAction(saveAct);
	m_pctrlXDirectToolBar->addSeparator();
	m_pctrlXDirectToolBar->addWidget(m_pctrlOppView);
	m_pctrlXDirectToolBar->addWidget(m_pctrlPolarView);
	m_pctrlXDirectToolBar->addSeparator();
	m_pctrlXDirectToolBar->addWidget(m_pctrlFoil);
	m_pctrlXDirectToolBar->addWidget(m_pctrlPolar);
	m_pctrlXDirectToolBar->addWidget(m_pctrlOpPoint);

	connect(m_pctrlFoil,    SIGNAL(activated(int)), this, SLOT(OnSelChangeFoil(int)));
	connect(m_pctrlPolar,   SIGNAL(activated(int)), this, SLOT(OnSelChangePolar(int)));
	connect(m_pctrlOpPoint, SIGNAL(activated(int)), this, SLOT(OnSelChangeOpp(int)));

}



void MainFrame::CreateXDirectActions()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;

	OpPointsAct = new QAction(QIcon(":/images/OnCpView.png"), tr("OpPoint view"), this);
	OpPointsAct->setCheckable(true);
	OpPointsAct->setStatusTip(tr("Show Operating point view"));
	connect(OpPointsAct, SIGNAL(triggered()), pXDirect, SLOT(OnOpPoints()));

	PolarsAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar view"), this);
	PolarsAct->setCheckable(true);
	PolarsAct->setStatusTip(tr("Show Polar view"));
	connect(PolarsAct, SIGNAL(triggered()), pXDirect, SLOT(OnPolars()));

	defineCpGraphSettings = new QAction(tr("Define Cp Graph Settings\t(G)"), this);
	connect(defineCpGraphSettings, SIGNAL(triggered()), pXDirect, SLOT(OnCpGraphSettings()));

	resetCpGraphScales = new QAction(tr("Reset Cp Graph Scales"), this);
	connect(resetCpGraphScales, SIGNAL(triggered()), pXDirect, SLOT(OnResetCpGraphScales()));

	allPolarGraphsSettingsAct = new QAction(tr("All Polar Graph Settings"), this);
	allPolarGraphsSettingsAct->setStatusTip("Modifies the setting for all polar graphs simultaneously");
	connect(allPolarGraphsSettingsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphsSetting()));

	allPolarGraphsScales = new QAction(tr("Reset All Polar Graph Scales"), this);
	connect(allPolarGraphsScales, SIGNAL(triggered()), pXDirect, SLOT(OnResetAllPolarGraphsScales()));

	XDirectGraphDlg = new QAction(tr("Define Graph Settings\t(G)"), this);
	connect(XDirectGraphDlg, SIGNAL(triggered()), pXDirect, SLOT(OnGraphSettings()));

	resetGraphLegend = new QAction(tr("Reset Legend Position"), this);
	connect(resetGraphLegend, SIGNAL(triggered()), pXDirect, SLOT(OnResetGraphLegend()));

	TwoPolarGraphsAct = new QAction(tr("Two Polar Graphs\t(T)"), this);
	TwoPolarGraphsAct->setCheckable(true);
	connect(TwoPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnCouplePolarGraphs()));

	AllPolarGraphsAct = new QAction(tr("All Polar Graphs\t(A)"), this);
	AllPolarGraphsAct->setCheckable(true);
	connect(AllPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphs()));

	for (int i = 0; i < 5; ++i)
	{
		PolarGraphAct[i] = new QAction(this);
		PolarGraphAct[i]->setData(i);
		PolarGraphAct[i]->setCheckable(true);
		connect(PolarGraphAct[i], SIGNAL(triggered()), pXDirect, SLOT(OnSinglePolarGraph()));
	}
	PolarGraphAct[0]->setText("Cl vs. Cd \t(1)");
	PolarGraphAct[1]->setText("Cl vs.Alpha \t(2)");
	PolarGraphAct[2]->setText("Cl vs. Xtr. \t(3)");
	PolarGraphAct[3]->setText("Cm vs.Alpha \t(4)");
	PolarGraphAct[4]->setText("Glide ratio vs. alpha \t(5)");


	deleteCurFoil = new QAction(tr("Delete..."), this);
	connect(deleteCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteCurFoil()));

	renameCurFoil = new QAction(tr("Rename..."), this);
	renameCurFoil->setShortcut(tr("F2"));
	connect(renameCurFoil, SIGNAL(triggered()), this, SLOT(OnRenameCurFoil()));

	exportCurFoil = new QAction(tr("Export..."), this);
	connect(exportCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurFoil()));

	setCurFoilStyle = new QAction(tr("Set Style..."), this);
	connect(setCurFoilStyle, SIGNAL(triggered()), this, SLOT(OnCurFoilStyle()));

	deleteFoilPolars = new QAction(tr("Delete associated polars"), this);
	deleteFoilPolars->setStatusTip(tr("Delete all the polars associated to this foil"));
	connect(deleteFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilPolars()));

	showFoilPolars = new QAction(tr("Show associated polars"), this);
	connect(showFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilPolars()));

	hideFoilPolars = new QAction(tr("Hide associated polars"), this);
	connect(hideFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnHideFoilPolars()));

	saveFoilPolars = new QAction(tr("Save associated polars"), this);
	connect(saveFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnSavePolars()));

	hidePolarOpps = new QAction(tr("Hide associated OpPoints"), this);
	connect(hidePolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnHidePolarOpps()));

	showPolarOpps = new QAction(tr("Show associated OpPoints"), this);
	connect(showPolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnShowPolarOpps()));

	deletePolarOpps = new QAction(tr("Delete associated OpPoints"), this);
	connect(deletePolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnDeletePolarOpps()));

	hideFoilOpps = new QAction(tr("Hide associated OpPoints"), this);
	connect(hideFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnHideFoilOpps()));

	showFoilOpps = new QAction(tr("Show associated OpPoints"), this);
	connect(showFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilOpps()));

	deleteFoilOpps = new QAction(tr("Delete associated OpPoints"), this);
	connect(deleteFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilOpps()));

	definePolar = new QAction(tr("Define an Analysis"), this);
	definePolar->setShortcut(tr("F6"));
	definePolar->setStatusTip(tr("Defines a single analysis/polar"));
	connect(definePolar, SIGNAL(triggered()), pXDirect, SLOT(OnSingleAnalysis()));

	defineBatch = new QAction(tr("Batch Analysis"), this);
	defineBatch->setShortcut(tr("Shift+F6"));
	defineBatch->setStatusTip(tr("Launches a batch of analysis calculation for a specified range or list of Reynolds numbers"));
	connect(defineBatch, SIGNAL(triggered()), pXDirect, SLOT(OnBatchAnalysis()));

	deletePolar = new QAction(tr("Delete the polar"), this);
	deletePolar->setStatusTip(tr("Deletes the currently selected polar"));
	connect(deletePolar, SIGNAL(triggered()), this, SLOT(OnDeleteCurPolar()));

	editCurPolar = new QAction(tr("Edit the polar"), this);
	editCurPolar->setStatusTip(tr("Remove the unconverged or erroneaous points of the currently selected polar"));
	connect(editCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnEditCurPolar()));

	exportCurPolar = new QAction(tr("Export the Polar"), this);
	connect(exportCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurPolar()));

	XDirectStyleAct = new QAction(tr("Define Styles"), this);
	XDirectStyleAct->setStatusTip(tr("Define the style for the boundary layer and the pressure arrows"));
	connect(XDirectStyleAct, SIGNAL(triggered()), pXDirect, SLOT(OnXDirectStyle()));

	showPanels = new QAction(tr("Show Panels"), this);
	showPanels->setCheckable(true);
	showPanels->setStatusTip(tr("Show the foil's panels"));
	connect(showPanels, SIGNAL(triggered()), pXDirect, SLOT(OnShowPanels()));

	resetFoilScale = new QAction(tr("Reset Foil Scale"), this);
	resetFoilScale->setStatusTip(tr("Resets the foil's scale to original size"));
	connect(resetFoilScale, SIGNAL(triggered()), pXDirect, SLOT(OnResetFoilScale()));

	showInviscidCurve = new QAction(tr("Show Inviscid Curve"), this);
	showInviscidCurve->setCheckable(true);
	showInviscidCurve->setStatusTip(tr("Display the Opp's inviscid curve"));
	connect(showInviscidCurve, SIGNAL(triggered()), pXDirect, SLOT(OnCpi()));

	showNeutralLine = new QAction(tr("Neutral Line"), this);
	showNeutralLine->setCheckable(true);
	connect(showNeutralLine, SIGNAL(triggered()), pXDirect, SLOT(OnShowNeutralLine()));

	showAllPolars = new QAction(tr("Show All Polars"), this);
	connect(showAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnShowAllPolars()));

	hideAllPolars = new QAction(tr("Hide All Polars"), this);
	connect(hideAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnHideAllPolars()));

	showCurOppOnly = new QAction(tr("Show Current Opp Only"), this);
	showCurOppOnly->setCheckable(true);
	connect(showCurOppOnly, SIGNAL(triggered()), pXDirect, SLOT(OnCurOppOnly()));

	showAllOpPoints = new QAction(tr("Show All Opps"), this);
	connect(showAllOpPoints, SIGNAL(triggered()), pXDirect, SLOT(OnShowAllOpps()));

	hideAllOpPoints = new QAction(tr("Hide All Opps"), this);
	connect(hideAllOpPoints, SIGNAL(triggered()), pXDirect, SLOT(OnHideAllOpps()));

	exportCurOpp = new QAction(tr("Export"), this);
	connect(exportCurOpp, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurOpp()));

	deleteCurOpp = new QAction(tr("Delete"), this);
	connect(deleteCurOpp, SIGNAL(triggered()), pXDirect, SLOT(OnDelCurOpp()));

	resetXFoil = new QAction(tr("Reset XFoil"), this);
	connect(resetXFoil, SIGNAL(triggered()), pXDirect, SLOT(OnResetXFoil()));

	viewXFoilAdvanced = new QAction(tr("XFoil Advanced Settings"), this);
	defineBatch->setStatusTip(tr("Tip : you don't want to use that option..."));
	connect(viewXFoilAdvanced, SIGNAL(triggered()), pXDirect, SLOT(OnXFoilAdvanced()));

	viewLogFile = new QAction(tr("View Log File"), this);
	connect(viewLogFile, SIGNAL(triggered()), this, SLOT(OnLogFile()));

	DerotateFoil = new QAction(tr("De-rotate the Foil"), this);
	connect(DerotateFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDerotateFoil()));

	NormalizeFoil = new QAction(tr("Normalize the Foil"), this);
	connect(NormalizeFoil, SIGNAL(triggered()), pXDirect, SLOT(OnNormalizeFoil()));

	RefineLocalFoil = new QAction(tr("Refine Locally"), this);
	connect(RefineLocalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnCadd()));

	RefineGlobalFoil = new QAction(tr("Refine Globally"), this);
	connect(RefineGlobalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnPanels()));

	EditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(EditCoordsFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilCoordinates()));

	ScaleFoil = new QAction(tr("Scale camber and thickness"), this);
	connect(ScaleFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilGeom()));

	SetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(SetTEGap, SIGNAL(triggered()), pXDirect, SLOT(OnSetTEGap()));

	SetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(SetLERadius, SIGNAL(triggered()), pXDirect, SLOT(OnSetLERadius()));

	SetFlap = new QAction(tr("Set Flap"), this);
	connect(SetFlap, SIGNAL(triggered()), pXDirect, SLOT(OnSetFlap()));

	InterpolateFoils = new QAction(tr("Interpolate Foils"), this);
	connect(InterpolateFoils, SIGNAL(triggered()), pXDirect, SLOT(OnInterpolateFoils()));

	NacaFoils = new QAction(tr("Naca Foils"), this);
	connect(NacaFoils, SIGNAL(triggered()), pXDirect, SLOT(OnNacaFoils()));

	DuplicateFoil = new QAction(tr("Duplicate"), this);
	connect(DuplicateFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDuplicateFoil()));

	setCpVarGraph = new QAction(tr("Cp Variable"), this);
	setCpVarGraph->setCheckable(true);
	setCpVarGraph->setStatusTip(tr("Sets Cp vs. chord graph"));
	connect(setCpVarGraph, SIGNAL(triggered()), pXDirect, SLOT(OnCpGraph()));

	setQVarGraph = new QAction(tr("Q Variable"), this);
	setQVarGraph->setCheckable(true);
	setQVarGraph->setStatusTip(tr("Sets Speed vs. chord graph"));
	connect(setQVarGraph, SIGNAL(triggered()), pXDirect, SLOT(OnQGraph()));

	CurXFoilResExport = new QAction(tr("Export Cur. XFoil Results"), this);
	CurXFoilResExport->setStatusTip(tr("Sets Speed vs. chord graph"));
	connect(CurXFoilResExport, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurXFoilResults()));

	CurXFoilCtPlot = new QAction(tr("Max. Shear Coefficient"), this);
	CurXFoilCtPlot->setCheckable(true);
	connect(CurXFoilCtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCtPlot()));

	CurXFoilDbPlot = new QAction(tr("Bottom Side D* and Theta"), this);
	CurXFoilDbPlot->setCheckable(true);
	connect(CurXFoilDbPlot, SIGNAL(triggered()), pXDirect, SLOT(OnDbPlot()));

	CurXFoilDtPlot = new QAction(tr("Top Side D* and Theta"), this);
	CurXFoilDtPlot->setCheckable(true);
	connect(CurXFoilDtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnDtPlot()));

	CurXFoilRtLPlot = new QAction(tr("Log(Re_Theta)"), this);
	CurXFoilRtLPlot->setCheckable(true);
	connect(CurXFoilRtLPlot, SIGNAL(triggered()), pXDirect, SLOT(OnRtLPlot()));

	CurXFoilRtPlot = new QAction(tr("Re_Theta"), this);
	CurXFoilRtPlot->setCheckable(true);
	connect(CurXFoilRtPlot, SIGNAL(triggered()), pXDirect, SLOT(OnRtPlot()));

	CurXFoilNPlot = new QAction(tr("Amplification Ratio"), this);
	CurXFoilNPlot->setCheckable(true);
	connect(CurXFoilNPlot, SIGNAL(triggered()), pXDirect, SLOT(OnNPlot()));

	CurXFoilCdPlot = new QAction(tr("Dissipation Coefficient"), this);
	CurXFoilCdPlot->setCheckable(true);
	connect(CurXFoilCdPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCdPlot()));

	CurXFoilCfPlot = new QAction(tr("Skin Friction Coefficient"), this);
	CurXFoilCfPlot->setCheckable(true);
	connect(CurXFoilCfPlot, SIGNAL(triggered()), pXDirect, SLOT(OnCfPlot()));

	CurXFoilUePlot = new QAction(tr("Edge Velocity"), this);
	CurXFoilUePlot->setCheckable(true);
	connect(CurXFoilUePlot, SIGNAL(triggered()), pXDirect, SLOT(OnUePlot()));

	CurXFoilHPlot = new QAction(tr("Kinematic Shape Parameter"), this);
	CurXFoilHPlot->setCheckable(true);
	connect(CurXFoilHPlot, SIGNAL(triggered()), pXDirect, SLOT(OnHPlot()));
}


void MainFrame::CreateXDirectMenus()
{
	//MainMenu for XDirect Application
	XDirectViewMenu = menuBar()->addMenu(tr("&View"));
	XDirectViewMenu->addAction(OpPointsAct);
	XDirectViewMenu->addAction(PolarsAct);
	XDirectViewMenu->addSeparator();
	XDirectViewMenu->addAction(restoreToolbarsAct);
	XDirectViewMenu->addAction(styleAct);
	XDirectViewMenu->addAction(saveViewToImageFileAct);

	FoilMenu = menuBar()->addMenu(tr("&Foil"));
	currentFoilMenu = FoilMenu->addMenu("Current Foil");
	currentFoilMenu->addAction(setCurFoilStyle);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(DuplicateFoil);
	currentFoilMenu->addAction(exportCurFoil);
	currentFoilMenu->addAction(renameCurFoil);
	currentFoilMenu->addAction(deleteCurFoil);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(showFoilPolars);
	currentFoilMenu->addAction(hideFoilPolars);
	currentFoilMenu->addAction(deleteFoilPolars);
	currentFoilMenu->addAction(saveFoilPolars);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(showFoilOpps);
	currentFoilMenu->addAction(hideFoilOpps);
	currentFoilMenu->addAction(deleteFoilOpps);
	FoilMenu->addSeparator();
	FoilMenu->addAction(resetFoilScale);
	FoilMenu->addAction(showPanels);
	FoilMenu->addAction(showNeutralLine);
	FoilMenu->addAction(XDirectStyleAct);

	DesignMenu = menuBar()->addMenu(tr("&Design"));
	DesignMenu->addAction(NormalizeFoil);
	DesignMenu->addAction(DerotateFoil);
	DesignMenu->addAction(RefineLocalFoil);
	DesignMenu->addAction(RefineGlobalFoil);
	DesignMenu->addAction(EditCoordsFoil);
	DesignMenu->addAction(ScaleFoil);
	DesignMenu->addAction(SetTEGap);
	DesignMenu->addAction(SetLERadius);
	DesignMenu->addAction(SetFlap);
	DesignMenu->addSeparator();
	DesignMenu->addAction(InterpolateFoils);
	DesignMenu->addAction(NacaFoils);

	PolarMenu = menuBar()->addMenu(tr("&Polars"));
	PolarMenu->addAction(definePolar);
	PolarMenu->addAction(defineBatch);
	PolarMenu->addSeparator();
	currentPolarMenu = PolarMenu->addMenu("Current Polar");
	currentPolarMenu->addAction(exportCurPolar);
	currentPolarMenu->addAction(deletePolar);
	currentPolarMenu->addAction(editCurPolar);
	currentPolarMenu->addSeparator();
	currentPolarMenu->addAction(showPolarOpps);
	currentPolarMenu->addAction(hidePolarOpps);
	currentPolarMenu->addAction(deletePolarOpps);
	PolarMenu->addSeparator();
	PolarMenu->addAction(showAllPolars);
	PolarMenu->addAction(hideAllPolars);
	PolarMenu->addSeparator();
	GraphPolarMenu = PolarMenu->addMenu("Polar Graphs");
	GraphPolarMenu->addAction(allPolarGraphsSettingsAct);
	GraphPolarMenu->addAction(allPolarGraphsScales);
	GraphPolarMenu->addAction(resetGraphLegend);
	GraphPolarMenu->addSeparator();
	GraphPolarMenu->addAction(AllPolarGraphsAct);
	GraphPolarMenu->addAction(TwoPolarGraphsAct);
	GraphPolarMenu->addSeparator();
	for(int i=0; i<5; i++)
		GraphPolarMenu->addAction(PolarGraphAct[i]);
	PolarMenu->addSeparator();

	OpPointMenu = menuBar()->addMenu(tr("Operating Points"));
	currentOppMenu = OpPointMenu->addMenu(tr("Current OpPoint"));
	currentOppMenu->addAction(exportCurOpp);
	currentOppMenu->addAction(deleteCurOpp);
	CpGraphMenu = OpPointMenu->addMenu("Graph");
	CpGraphMenu->addAction(setCpVarGraph);
	CpGraphMenu->addAction(setQVarGraph);
	CpGraphMenu->addSeparator();
	CpGraphMenu->addAction(showInviscidCurve);
	CpGraphMenu->addSeparator();
	CurXFoilResults = CpGraphMenu->addMenu("Current XFoil Results");
	CurXFoilResults->addAction(CurXFoilResExport);
	CurXFoilResults->addSeparator();
	CurXFoilResults->addAction(CurXFoilCtPlot);
	CurXFoilResults->addAction(CurXFoilDbPlot);
	CurXFoilResults->addAction(CurXFoilDtPlot);
	CurXFoilResults->addAction(CurXFoilRtLPlot);
	CurXFoilResults->addAction(CurXFoilRtPlot);
	CurXFoilResults->addAction(CurXFoilNPlot);
	CurXFoilResults->addAction(CurXFoilCdPlot);
	CurXFoilResults->addAction(CurXFoilCfPlot);
	CurXFoilResults->addAction(CurXFoilUePlot);
	CurXFoilResults->addAction(CurXFoilHPlot);
	CpGraphMenu->addSeparator();
	CpGraphMenu->addAction(resetCpGraphScales);
	CpGraphMenu->addAction(defineCpGraphSettings);
	CpGraphMenu->addAction(exportCurGraphAct);
	OpPointMenu->addSeparator();
	OpPointMenu->addAction(showCurOppOnly);
	OpPointMenu->addAction(hideAllOpPoints);
	OpPointMenu->addAction(showAllOpPoints);
	OpPointMenu->addSeparator();
	OpPointMenu->addAction(resetXFoil);
	OpPointMenu->addAction(viewXFoilAdvanced);
	OpPointMenu->addAction(viewLogFile);


	//XDirect foil Context Menu
	OperFoilCtxMenu = new QMenu("Context Menu",this);
	CurFoilCtxMenu = OperFoilCtxMenu->addMenu("Current Foil");
	CurFoilCtxMenu->addAction(setCurFoilStyle);
	CurFoilCtxMenu->addSeparator();
	CurFoilCtxMenu->addAction(DuplicateFoil);
	CurFoilCtxMenu->addAction(renameCurFoil);
	CurFoilCtxMenu->addAction(deleteCurFoil);
	CurFoilCtxMenu->addAction(exportCurFoil);
	CurFoilCtxMenu->addSeparator();
	CurFoilDesignMenu = CurFoilCtxMenu->addMenu("Design Operations");
	CurFoilDesignMenu->addAction(NormalizeFoil);
	CurFoilDesignMenu->addAction(DerotateFoil);
	CurFoilDesignMenu->addAction(RefineLocalFoil);
	CurFoilDesignMenu->addAction(RefineGlobalFoil);
	CurFoilDesignMenu->addAction(EditCoordsFoil);
	CurFoilDesignMenu->addAction(ScaleFoil);
	CurFoilDesignMenu->addAction(SetTEGap);
	CurFoilDesignMenu->addAction(SetLERadius);
	CurFoilDesignMenu->addAction(SetFlap);
	CurFoilDesignMenu->addSeparator();
	CurFoilDesignMenu->addAction(InterpolateFoils);
	CurFoilDesignMenu->addAction(NacaFoils);
	CurFoilCtxMenu->addSeparator();//_______________
	CurFoilCtxMenu->addAction(deleteFoilPolars);
	CurFoilCtxMenu->addAction(showFoilPolars);
	CurFoilCtxMenu->addAction(hideFoilPolars);
	CurFoilCtxMenu->addAction(saveFoilPolars);
	CurFoilCtxMenu->addSeparator();
	CurFoilCtxMenu->addAction(showFoilOpps);
	CurFoilCtxMenu->addAction(hideFoilOpps);
	CurFoilCtxMenu->addAction(deleteFoilOpps);


	OperFoilCtxMenu->addSeparator();//_______________
	CurPolarCtxMenu = OperFoilCtxMenu->addMenu("Current Polar");
	CurPolarCtxMenu->addAction(deletePolar);
	CurPolarCtxMenu->addAction(exportCurPolar);
	CurPolarCtxMenu->addAction(editCurPolar);
	CurPolarCtxMenu->addSeparator();
	CurPolarCtxMenu->addAction(showPolarOpps);
	CurPolarCtxMenu->addAction(hidePolarOpps);
	CurPolarCtxMenu->addAction(deletePolarOpps);

	OperFoilCtxMenu->addSeparator();//_______________
	CurOppCtxMenu = OperFoilCtxMenu->addMenu("Current OpPoint");
	CurOppCtxMenu->addAction(exportCurOpp);
	CurOppCtxMenu->addAction(deleteCurOpp);

	OperFoilCtxMenu->addSeparator();//_______________
//	CurGraphCtxMenu = OperFoilCtxMenu->addMenu("Cp graph");
	OperFoilCtxMenu->addMenu(CpGraphMenu);

	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(definePolar);
	OperFoilCtxMenu->addAction(defineBatch);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(showAllPolars);
	OperFoilCtxMenu->addAction(hideAllPolars);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(showCurOppOnly);
	OperFoilCtxMenu->addAction(showAllOpPoints);
	OperFoilCtxMenu->addAction(hideAllOpPoints);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(resetFoilScale);
	OperFoilCtxMenu->addAction(showPanels);
	OperFoilCtxMenu->addAction(showNeutralLine);
	OperFoilCtxMenu->addAction(XDirectStyleAct);
	//End XDirect foil Context Menu


	//XDirect polar Context Menu
	OperPolarCtxMenu = new QMenu("Context Menu",this);
	CurFoilCtxMenu = OperPolarCtxMenu->addMenu("Current Foil");
	CurFoilCtxMenu->addAction(renameCurFoil);
	CurFoilCtxMenu->addAction(deleteCurFoil);
	CurFoilCtxMenu->addAction(exportCurFoil);
	CurFoilCtxMenu->addSeparator();//_______________
	CurFoilCtxMenu->addAction(showFoilPolars);
	CurFoilCtxMenu->addAction(hideFoilPolars);
	CurFoilCtxMenu->addAction(deleteFoilPolars);
	CurFoilCtxMenu->addAction(saveFoilPolars);
	CurFoilCtxMenu->addSeparator();
	OperPolarCtxMenu->addSeparator();//_______________
	CurPolarCtxMenu = OperPolarCtxMenu->addMenu("Current Polar");
	CurPolarCtxMenu->addAction(deletePolar);
	CurPolarCtxMenu->addAction(exportCurPolar);
	CurPolarCtxMenu->addAction(editCurPolar);
	OperPolarCtxMenu->addSeparator();//_______________
	CurGraphCtxMenu = OperPolarCtxMenu->addMenu("Current Graph");
	CurGraphCtxMenu->addAction(resetCurGraphScales);
	CurGraphCtxMenu->addAction(XDirectGraphDlg);
	CurGraphCtxMenu->addAction(exportCurGraphAct);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addMenu(GraphPolarMenu);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(definePolar);
	OperPolarCtxMenu->addAction(defineBatch);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(showAllPolars);
	OperPolarCtxMenu->addAction(hideAllPolars);
	OperPolarCtxMenu->addAction(showAllOpPoints);
	OperPolarCtxMenu->addAction(hideAllOpPoints);

	//End XDirect polar Context Menu
}


void MainFrame::CreateXInverseActions()
{
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;

	StoreFoil = new QAction(QIcon(":/images/OnStoreFoil.png"), tr("Store Foil"), this);
	StoreFoil->setStatusTip(tr("Store Foil in database"));
	connect(StoreFoil, SIGNAL(triggered()), pXInverse, SLOT(OnStoreFoil()));

	ExtractFoil = new QAction(QIcon(":/images/OnExtractFoil.png"),tr("Extract Foil"), this);
	ExtractFoil->setStatusTip(tr("Extract a Foil from the database for modification"));
	connect(ExtractFoil, SIGNAL(triggered()), pXInverse, SLOT(OnExtractFoil()));

	InverseStyles = new QAction(tr("Define Styles"), this);
	InverseStyles->setStatusTip(tr("Define the styles for this view"));
	connect(InverseStyles, SIGNAL(triggered()), pXInverse, SLOT(OnInverseStyles()));

	InverseResetScale = new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset foil scale"), this);
	InverseResetScale->setStatusTip(tr("Resets the scale to fit the screen size"));
	connect(InverseResetScale, SIGNAL(triggered()), pXInverse, SLOT(OnResetFoilScale()));

	InverseInsertCtrlPt = new QAction(tr("Insert Control Point"), this);
	connect(InverseInsertCtrlPt, SIGNAL(triggered()), pXInverse, SLOT(OnInsertCtrlPt()));

	InverseRemoveCtrlPt = new QAction(tr("Remove Control Point"), this);
	connect(InverseRemoveCtrlPt, SIGNAL(triggered()), pXInverse, SLOT(OnRemoveCtrlPt()));

	InvQInitial = new QAction(tr("Show Q-Initial"), this);
	InvQInitial->setCheckable(true);
	connect(InvQInitial, SIGNAL(triggered()), pXInverse, SLOT(OnQInitial()));

	InvQSpec = new QAction(tr("Show Q-Spec"), this);
	InvQSpec->setCheckable(true);
	connect(InvQSpec, SIGNAL(triggered()), pXInverse, SLOT(OnQSpec()));

	InvQViscous = new QAction(tr("Show Q-Viscous"), this);
	InvQViscous->setCheckable(true);
	connect(InvQViscous, SIGNAL(triggered()), pXInverse, SLOT(OnQViscous()));

	InvQPoints = new QAction(tr("Show Points"), this);
	InvQPoints->setCheckable(true);
	connect(InvQPoints, SIGNAL(triggered()), pXInverse, SLOT(OnQPoints()));

	InvQReflected = new QAction(tr("Show Reflected"), this);
	InvQReflected->setCheckable(true);

	connect(InvQReflected, SIGNAL(triggered()), pXInverse, SLOT(OnQReflected()));
	XInverseGraphDlg = new QAction(tr("Define Graph Settings\t(G)"), this);
	connect(XInverseGraphDlg, SIGNAL(triggered()), pXInverse, SLOT(OnGraphSettings()));

	InverseZoomIn = new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
	InverseZoomIn->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));
	connect(InverseZoomIn, SIGNAL(triggered()), pXInverse, SLOT(OnZoomIn()));

	InverseZoomX = new QAction(QIcon(":/images/OnZoomGraphX.png"), tr("Zoom X Scale"), this);
	InverseZoomX->setStatusTip(tr("Zoom X Scale Only"));
	connect(InverseZoomX, SIGNAL(triggered()), pXInverse, SLOT(OnZoomX()));

	InverseZoomY = new QAction(QIcon(":/images/OnZoomGraphY.png"), tr("Zoom Y Scale"), this);
	InverseZoomY->setStatusTip(tr("Zoom Y Scale Only"));
	connect(InverseZoomY, SIGNAL(triggered()), pXInverse, SLOT(OnZoomY()));
}


void MainFrame::CreateXInverseMenus()
{
	//MainMenu for XInverse Application
	XInverseViewMenu = menuBar()->addMenu(tr("&View"));
	XInverseViewMenu->addAction(InverseStyles);
	XInverseViewMenu->addAction(XInverseGraphDlg);
	XInverseViewMenu->addSeparator();
	XInverseViewMenu->addAction(restoreToolbarsAct);
	XInverseViewMenu->addAction(styleAct);
	XInverseViewMenu->addAction(saveViewToImageFileAct);

	InverseGraphMenu = menuBar()->addMenu(tr("&Graph"));
	InverseGraphMenu->addAction(XInverseGraphDlg);
	InverseGraphMenu->addAction(resetCurGraphScales);
	InverseGraphMenu->addAction(exportCurGraphAct);

	InverseFoilMenu = menuBar()->addMenu(tr("&Foil"));
	InverseFoilMenu->addAction(StoreFoil);
	InverseFoilMenu->addAction(ExtractFoil);
	InverseFoilMenu->addAction(InverseResetScale);
	InverseFoilMenu->addSeparator();
	InverseFoilMenu->addAction(InvQInitial);
	InverseFoilMenu->addAction(InvQSpec);
	InverseFoilMenu->addAction(InvQViscous);
	InverseFoilMenu->addAction(InvQPoints);
	InverseFoilMenu->addAction(InvQReflected);

	//Context Menu for XInverse Application
	InverseContextMenu = new QMenu("Context Menu",this);
	InverseContextMenu->addAction(InverseStyles);
	InverseContextMenu->addAction(resetCurGraphScales);
	InverseContextMenu->addAction(XInverseGraphDlg);
	InverseContextMenu->addSeparator();
	InverseContextMenu->addAction(InverseInsertCtrlPt);
	InverseContextMenu->addAction(InverseRemoveCtrlPt);
	InverseContextMenu->addSeparator();
	InverseContextMenu->addAction(InvQInitial);
	InverseContextMenu->addAction(InvQSpec);
	InverseContextMenu->addAction(InvQViscous);
	InverseContextMenu->addAction(InvQPoints);
	InverseContextMenu->addAction(InvQReflected);

}



void MainFrame::CreateXInverseToolbar()
{
	m_pctrlFullInverse  = new QRadioButton("Full Inverse");
	m_pctrlMixedInverse = new QRadioButton("Mixed Inverse");
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	connect(m_pctrlFullInverse, SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));
	connect(m_pctrlMixedInverse, SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));

	m_pctrlInvZoomX = new QToolButton;
	m_pctrlInvZoomX->setDefaultAction(InverseZoomX);
	m_pctrlInvZoomX->setCheckable(true);

	m_pctrlInvZoomY = new QToolButton;
	m_pctrlInvZoomY->setDefaultAction(InverseZoomY);
	m_pctrlInvZoomY->setCheckable(true);

	m_pctrlInvZoomIn = new QToolButton;
	m_pctrlInvZoomIn->setDefaultAction(InverseZoomIn);
	m_pctrlInvZoomIn->setCheckable(true);

	m_pctrlXInverseToolBar = addToolBar(tr("XInverse"));
	m_pctrlXInverseToolBar->addAction(newProjectAct);
	m_pctrlXInverseToolBar->addAction(openAct);
	m_pctrlXInverseToolBar->addAction(saveAct);
	m_pctrlXInverseToolBar->addSeparator();
	m_pctrlXInverseToolBar->addWidget(m_pctrlFullInverse);
	m_pctrlXInverseToolBar->addWidget(m_pctrlMixedInverse);
	m_pctrlXInverseToolBar->addSeparator();
	m_pctrlXInverseToolBar->addAction(ExtractFoil);
	m_pctrlXInverseToolBar->addAction(StoreFoil);
	m_pctrlXInverseToolBar->addSeparator();
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomIn);
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomX);
	m_pctrlXInverseToolBar->addWidget(m_pctrlInvZoomY);
	m_pctrlXInverseToolBar->addAction(resetCurGraphScales);
	m_pctrlXInverseToolBar->addAction(InverseResetScale);
}



void MainFrame::OnDeleteCurPolar()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	if(!pXDirect->m_pCurPolar) return;
	OpPoint *pOpPoint;
	int l;
	QString str;

	str = "Are you sure you want to delete the polar :\n  "+pXDirect->m_pCurPolar->m_PlrName;
	str += "\n and all the associated OpPoints ?";

	if (QMessageBox::Yes == QMessageBox::question(window(), "Question", str,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		// start by removing all OpPoints
		for (l=m_oaOpp.size()-1; l>=0; l--)
		{
			pOpPoint = (OpPoint*)m_oaOpp.at(l);
			if (pOpPoint->m_strPlrName == pXDirect->m_pCurPolar->m_PlrName &&
				pOpPoint->m_strFoilName == pXDirect->m_pCurFoil->m_FoilName)
			{
				m_oaOpp.removeAt(l);
				delete pOpPoint;
			}
		}
		// then remove CPolar and update views
		for (l=m_oaPolar.size()-1; l>=0; l--)
		{
			if(pXDirect->m_pCurPolar == m_oaPolar.at(l))
			{
				m_oaPolar.removeAt(l);
				delete pXDirect->m_pCurPolar;
			}
		}
		pXDirect->m_pCurOpp   = NULL;
		pXDirect->m_pCurPolar = NULL;
	}

	UpdatePolars();
	pXDirect->SetPolar();

	SetSaveState(false);
	UpdateView();
}

bool MainFrame::DeleteFoil(CFoil *pFoil, bool bAsk)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	if(!pFoil || !pFoil->m_FoilName.length()) return false;
    QString strong;
    CFoil *pOldFoil;
    OpPoint * pOpPoint;
    CPolar* pPolar;
    int j;
    bool bDelete = true;

    if(bAsk)
    {
		strong = "Are you sure you want to delete \n" + pFoil->m_FoilName ;
		strong+= "\nand all associated OpPoints and Polars ?";

		int resp=QMessageBox::question(this,"Question", strong,  QMessageBox::Yes | QMessageBox::Default, QMessageBox::No);
		if(resp==QMessageBox::Yes)			bDelete = true;
		else return false;
    }

    if (bDelete)
    {
		for (j= m_oaOpp.size()-1; j>=0; j--)
		{
			pOpPoint = (OpPoint*)m_oaOpp[j];
			if(pOpPoint->m_strFoilName == pFoil->m_FoilName)
			{
				m_oaOpp.removeAt(j);
				delete pOpPoint;
			}
		}
		for (j= (int)m_oaPolar.size()-1;j>=0; j--)
		{
			pPolar = (CPolar*)m_oaPolar.at(j);
			if(pPolar->m_FoilName == pFoil->m_FoilName)
			{
				m_oaPolar.removeAt(j);
				delete pPolar;
			}
		}
		for (j=0; j< m_oaFoil.size(); j++)
		{
			pOldFoil = (CFoil*)m_oaFoil.at(j);
			if (pOldFoil == pFoil)
			{
				m_oaFoil.removeAt(j);
				delete pOldFoil;
				if(m_pCurFoil == pOldFoil)           m_pCurFoil = NULL;
				if(pXDirect->m_pCurFoil == pOldFoil) pXDirect->m_pCurFoil = NULL;
				break;
			}
		}
		pXDirect->m_pCurOpp = NULL;
		pXDirect->m_pCurPolar = NULL;
		pXDirect->m_pCurFoil = NULL;
		pXDirect->CheckButtons();
		SetSaveState(false);
    }
    return true;

}

void MainFrame::DeletePlane(CPlane *pPlane, bool bResultsOnly)
{
	if(!pPlane || !pPlane->m_PlaneName.length()) return ;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CPlane *pOldPlane;
	SetSaveState(false);
	int i;
	//first remove all POpps associated to the plane
	for (i=(int)m_oaPOpp.size()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*)m_oaPOpp.at(i);
		if(pPOpp->m_PlaneName == pPlane->m_PlaneName)
		{
			m_oaPOpp.removeAt(i);
			delete pPOpp;
		}
	}

	pMiarex->m_pCurPOpp = NULL;
	pMiarex->m_pCurWOpp = NULL;
	//next remove all PPolars associated to the plane
	for (i=(int)m_oaWPolar.size()-1; i>=0; i--)
	{
		pWPolar = (CWPolar*)m_oaWPolar.at(i);
		if (pWPolar->m_UFOName == pPlane->m_PlaneName)
		{
			if(!bResultsOnly)
			{
				m_oaWPolar.removeAt(i);
				if(pWPolar == pMiarex->m_pCurWPolar)
				{
					pMiarex->m_pCurWPolar = NULL;
	//				m_WOperDlgBar.EnableAnalysis(false);
				}
				delete pWPolar;
			}
			else
			{
				pWPolar->ResetWPlr();
				pWPolar->m_WArea  = pMiarex->m_pCurWing->m_Area;
				pWPolar->m_WMAChord  = pMiarex->m_pCurWing->m_MAChord;
				pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_Span;
			}
		}
	}

	if(bResultsOnly) return;


	for (i=(int)m_oaPlane.size()-1; i>=0; i--)
	{
		pOldPlane = (CPlane*)m_oaPlane.at(i);
		if (pOldPlane == pPlane)
		{
			m_oaPlane.removeAt(i);
			delete pPlane;
			if(pPlane == pMiarex->m_pCurPlane)
			{
				pMiarex->m_pCurPlane = NULL;
				pMiarex->m_pCurWing  = NULL;
				pMiarex->m_pCurStab  = NULL;
				pMiarex->m_pCurFin   = NULL;
				pMiarex->m_pCurBody  = NULL;
			}
			break;
		}
	}
}


void MainFrame::DeleteProject()
{
	// clear everything
	int i;
	void *pObj;
	for (i=m_oaPlane.size()-1; i>=0; i--)
	{
		pObj = m_oaPlane.at(i);
		m_oaPlane.removeAt(i);
		delete (CPlane*)pObj;
	}

	for (i=m_oaPOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaPOpp.at(i);
		m_oaPOpp.removeAt(i);
		delete (CPOpp*)pObj;
	}
	for (i=m_oaWing.size()-1; i>=0; i--)
	{
		pObj = m_oaWing.at(i);
		m_oaWing.removeAt(i);
		delete (CWing*)pObj;
	}

	for (i=m_oaWPolar.size()-1; i>=0; i--)
	{
		pObj = m_oaWPolar.at(i);
		m_oaWPolar.removeAt(i);
		delete (CWPolar*)pObj;
	}

	for (i=m_oaWOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaWOpp.at(i);
		m_oaWOpp.removeAt(i);
		delete (CWOpp*)pObj;
	}

	for (i=m_oaFoil.size()-1; i>=0; i--)
	{
		pObj = m_oaFoil.at(i);
		m_oaFoil.removeAt(i);
		delete (CFoil*)pObj;
	}
	for (i=m_oaPolar.size()-1; i>=0; i--)
	{
		pObj = m_oaPolar.at(i);
		m_oaPolar.removeAt(i);
		delete (CPolar*)pObj;
	}
	for (i=m_oaOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaOpp.at(i);
		m_oaOpp.removeAt(i);
		delete (OpPoint*)pObj;
	}
	for (i=m_oaBody.size()-1; i>=0; i--)
	{
		pObj = m_oaBody.at(i);
		m_oaBody.removeAt(i);
		delete (CBody*)pObj;
	}

	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_pCurPlane  = NULL;
	pMiarex->m_pCurPOpp   = NULL;
	pMiarex->m_pCurWing   = NULL;
	pMiarex->m_pCurWPolar = NULL;
	pMiarex->m_pCurWOpp   = NULL;
	pMiarex->m_pCurBody   = NULL;
	pMiarex->m_pCurFrame  = NULL;

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->m_pXFoil->m_FoilName = "";
	pXDirect->m_pCurFoil  = NULL;
	pXDirect->m_pCurPolar = NULL;
	pXDirect->m_pCurOpp   = NULL;
	pXDirect->SetFoil();

	if(m_iApp == MIAREX)
	{
		UpdateUFOs();
		pMiarex->SetUFO();
		pMiarex->CreateWPolarCurves();
		pMiarex->CreateWOppCurves();
		pMiarex->CreateCpCurves();
		pMiarex->CheckButtons();
//		pMiarex->SetBody();
	}
	else if (m_iApp==XFOILANALYSIS)
	{
		UpdateFoils();
		if(pXDirect->m_bPolar) pXDirect->CreatePolarCurves();
		else                   pXDirect->CreateOppCurves();
	}
	else if (m_iApp==DIRECTDESIGN)
	{
		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->FillFoilTable();
		pAFoil->SelectFoil();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		QXInverse *pXInverse =(QXInverse*)m_pXInverse;
		pXInverse->Clear();
	}
	SetProjectName("");
	SetSaveState(true);
}




void MainFrame::DeleteWing(CWing *pThisWing, bool bResultsOnly)
{
	if(!pThisWing)
	{
		return;
	}
	SetSaveState(false);
	int i;

	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	//first remove all WOpps associated to the wing
	CWOpp * pWOpp;
	for (i=(int)m_oaWOpp.size()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*)m_oaWOpp.at(i);
		if(pWOpp->m_WingName == pThisWing->m_WingName)
		{
			m_oaWOpp.removeAt(i);
			if(pWOpp == pMiarex->m_pCurWOpp) pMiarex->m_pCurWOpp = NULL;
			delete pWOpp;
		}
		pMiarex->m_pCurWOpp = NULL;
	}

	//next remove all WPolars associated to the wing
	CWPolar* pWPolar;
	for (i=m_oaWPolar.size()-1; i>=0; i--)
	{
		pWPolar = (CWPolar*)m_oaWPolar.at(i);
		if (pWPolar->m_UFOName == pThisWing->m_WingName)
		{
			if(!bResultsOnly)
			{
				m_oaWPolar.removeAt(i);
				if(pWPolar == pMiarex->m_pCurWPolar)
				{
//					m_WOperDlgBar.EnableAnalysis(false);
					pMiarex->m_pCurWPolar = NULL;
				}
				delete pWPolar;
			}
			else
			{
				pWPolar->ResetWPlr();
				pWPolar->m_WArea     = pMiarex->m_pCurWing->m_Area;
				pWPolar->m_WMAChord  = pMiarex->m_pCurWing->m_MAChord;
				pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_Span;
			}
		}
	}

	if(bResultsOnly) return;// that's enough

	// ... Find the wing in the object array and remove it...
	CWing* pWing;
	for (i=m_oaWing.size()-1; i>=0; i--)
	{
		pWing = (CWing*)m_oaWing.at(i);
		if (pWing == pThisWing)
		{
			m_oaWing.removeAt(i);
			delete pWing;
			if(pWing == pMiarex->m_pCurWing)	pMiarex->m_pCurWing = NULL;
			break;
		}
	}
}


QColor MainFrame::GetColor(int type)
{
	//type
	// 0=Foil
	// 1=Polar
	// 2=Opp
	// 3=Wing (unused)
	// 4=WPolar
	// 5=WOpp
	// 6=POpp
	QColor color = QColor(255,0,0);
	int i,j;
	bool bFound = false;
	switch (type)
	{
		case 0:
		{
			CFoil *pFoil;
			for (j=0; j<24; j++)
			{
				for (i=0; i<m_oaFoil.size(); i++)
				{
					pFoil = (CFoil*)m_oaFoil.at(i);
					bFound = false;
					if(pFoil->m_FoilColor == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaFoil.size()%24];
			break;
		}
		case 1:
		{
			CPolar *pPolar;
			for (j=0; j<24; j++)
			{
				for (i=0; i<m_oaPolar.size(); i++)
				{
					pPolar = (CPolar*)m_oaPolar.at(i);
					bFound = false;
					if(pPolar->m_Color == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaPolar.size()%24];
			break;
		}
		case 2:
		{
			OpPoint *pOpPoint;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaOpp.size(); i++)
				{
					pOpPoint = (OpPoint*)m_oaOpp.at(i);
					bFound = false;
					if(pOpPoint->m_Color == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaOpp.size()%24];
			break;
		}
		case 4:
		{
			CWPolar *pWPolar;
			for (j=0; j<24; j++)
			{
				for (i=0; i<m_oaWPolar.size(); i++)
				{
					pWPolar = (CWPolar*)m_oaWPolar.at(i);
					bFound = false;
					if(pWPolar->m_Color == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound)
					return m_crColors[j];

			}
			return m_crColors[m_oaWPolar.size()%24];
			break;
		}
		case 5:
		{
			CWOpp *pWOpp;
			for (j=0; j<24; j++)
			{
				for (i=0; i<m_oaWOpp.size(); i++)
				{
					pWOpp = (CWOpp*)m_oaWOpp.at(i);
					bFound = false;
					if(pWOpp->m_Color == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaWOpp.size()%24];
			break;
		}
		case 6:
		{
			CPOpp *pPOpp;
			for (j=0; j<24; j++)
			{
				for (i=0; i<m_oaPOpp.size(); i++)
				{
					pPOpp = (CPOpp*)m_oaPOpp.at(i);
					bFound = false;
					if(pPOpp->m_Color == m_crColors[j])
					{
						bFound = true;
						break;
					}
				}
				if(!bFound)
					return m_crColors[j];
			}
			return m_crColors[m_oaPOpp.size()%24];
			break;
		}

		default:
		{
			return QColor(255,0,0);
		}
	}
	return color;
}


CWing* MainFrame::GetWing(QString WingName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!WingName.length()) return NULL;
	CWing* pWing;
	for (int i=0; i<m_oaWing.size(); i++)
	{
		pWing = (CWing*)m_oaWing.at(i);
		if (pWing->m_WingName == WingName)
		{
			return pWing;
		}
	}

	return NULL;
}

CFoil* MainFrame::GetFoil(QString strFoilName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strFoilName.length()) return NULL;
	CFoil* pFoil;
	for (int i=0; i<m_oaFoil.size(); i++)
	{
		pFoil = (CFoil*)m_oaFoil.at(i);
		if (pFoil->m_FoilName == strFoilName)
		{
			return pFoil;
		}
	}

	return NULL;
}

CPolar *MainFrame::GetPolar(QString m_FoilName, QString PolarName)
{
	if (!PolarName.length())
	{
		//try to get the first from the Combobox
		if(!m_pctrlPolar->count()) return NULL;        //nothing more to try
		PolarName = m_pctrlPolar->itemText(0); //... and carry on
	}

    CPolar *pPolar;
    for (int i=0; i<m_oaPolar.size(); i++)
    {
		pPolar = (CPolar*) m_oaPolar.at(i);
		if (pPolar->m_FoilName == m_FoilName &&  pPolar->m_PlrName == PolarName)
		{
			return pPolar;
		}
	}
	return NULL;
}

OpPoint *MainFrame::GetOpp(double Alpha)
{
    OpPoint* pOpPoint;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	CPolar *pCurPolar = pXDirect->m_pCurPolar;
    if(!pCurPolar) return NULL;

    for (int i=0; i<m_oaOpp.size(); i++)
    {
		if(!pCurPolar) return NULL;
		pOpPoint = (OpPoint*)m_oaOpp.at(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName)
		{
			if (pOpPoint->m_strPlrName == pCurPolar->m_PlrName)
			{
				if(pCurPolar->m_Type !=4)
				{
					if(fabs(pOpPoint->Alpha - Alpha) <0.01)
					{
						return pOpPoint;
					}
				}
				else
				{
					if(fabs(pOpPoint->Reynolds - Alpha) <0.1)
					{
						return pOpPoint;
					}
				}
			}
		}
    }
    return NULL;// shouldn't ever get here, fortunately
}



bool MainFrame::LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	CFoil *pFoil;
	CPolar *pPolar = NULL;
	CPolar *pOldPlr;
	OpPoint *pOpp, *pOldOpp;
	QString strong;
	QXDirect *pXDirect =(QXDirect*)m_pXDirect;


	//first read all available foils
	int i,l,n;
	ar>>n;
	for (i=0;i<n; i++)
	{
		pFoil = new CFoil();

		pFoil->Serialize(ar, bIsStoring);
		if (!pFoil->m_FoilName.length())
		{
			delete pFoil;
			return false;
		}

		AddFoil(pFoil);
	}
	//next read all available polars

	ar>>n;
	for (i=0;i<n; i++)
	{
		pPolar = new CPolar();

		pPolar->m_Color = GetColor(1);
		pPolar->Serialize(ar, bIsStoring);
		if (!pPolar->m_FoilName.length())
		{
			delete pPolar;
			return false;
		}
		for (l=0; l<m_oaPolar.size(); l++)
		{
			pOldPlr = (CPolar*)m_oaPolar[l];
			if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
				pOldPlr->m_PlrName  == pPolar->m_PlrName)
			{
				//just overwrite...
				m_oaPolar.removeAt(l);
				delete pOldPlr;
				//... and continue to add
			}
		}
		pPolar = AddPolar(pPolar);
	}

	//Last read all available operating points

	ar>>n;
	for (i=0;i<n; i++)
	{
		pOpp = new OpPoint();
		if(!pOpp)
		{
			delete pOpp;
			return false;
		}

		pOpp->m_Color = m_crColors[m_oaOpp.size()%24];
		if(ArchiveFormat>=100002)
		{
			if (!pOpp->Serialize(ar, bIsStoring, 100002))
			{
				delete pOpp;
				return false;
			}
			else
			{
				pFoil = GetFoil(pOpp->m_strFoilName);
				if(pFoil)
				{
					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
				}
				else
				{
					delete pOpp;
				}
			}
		}
		else
		{
			if (!pOpp->Serialize(ar, bIsStoring))
			{
				delete pOpp;
				return false;
			}
			else
			{
				pFoil = GetFoil(pOpp->m_strFoilName);
				if(pFoil)
				{
					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
				}
				else
				{
					delete pOpp;
				}
			}
		}
		if(pOpp)
		{
			for (int l=m_oaOpp.size()-1;l>=0; l--)
			{
				pOldOpp = (OpPoint*)m_oaOpp.at(l);
				if (pOldOpp->m_strFoilName == pOpp->m_strFoilName &&
					pOldOpp->m_strPlrName  == pOpp->m_strPlrName &&
					fabs(pOldOpp->Alpha-pOpp->Alpha)<0.001)
				{
					//just overwrite...
					m_oaOpp.removeAt(l);
					delete pOldOpp;
					//... and continue to add
				}
			}
		}
		pXDirect->InsertOpPoint(pOpp);
	}

	return true;
}


void MainFrame::LoadSettings()
{
	QAFoil *pAFoil       = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	QMiarex *pMiarex     = (QMiarex*)m_pMiarex;

	QString FileName   = QDir::tempPath() + "/QFLR5.set";
	QFile *pXFile = new QFile(FileName);

	if (!pXFile->open(QIODevice::ReadOnly)) return;

	bool bFloat;
	int k;
	int a,b,c,d;
	QString strange;

	QDataStream ar(pXFile);
	ar >> k;//format
	if(k != m_SettingsFormat)
	{
		pXFile->close();
		return;
	}
	ar >> a >> b >> c >> d;
	QPoint pt(a,b);
	QSize sz(c,d);

	ar >> m_bMaximized;
	ar >> m_StyleName;

	ar >> bFloat >> pt.rx()>> pt.ry();
	m_pctrlMiarexWidget->setFloating(bFloat);
	if(bFloat) m_pctrlMiarexWidget->move(pt);

	ar >> bFloat >> pt.rx()>> pt.ry();
	m_pctrlXDirectWidget->setFloating(bFloat);
	if(bFloat) m_pctrlXDirectWidget->move(pt);

	ar >> bFloat >> pt.rx()>> pt.ry() >> sz.rwidth() >> sz.rheight();
	m_pctrlAFoilWidget->setFloating(bFloat);
	m_pctrlAFoilWidget->resize(sz);
	if(bFloat) m_pctrlAFoilWidget->move(pt);

	ar >> bFloat >> pt.rx()>> pt.ry();
	m_pctrlXInverseWidget->setFloating(bFloat);
	if(bFloat) m_pctrlXInverseWidget->move(pt);


	ar >> m_LastDirName;
	ar >> m_LengthUnit >> m_AreaUnit >> m_WeightUnit >> m_SpeedUnit >> m_ForceUnit >> m_MomentUnit;
	SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
			 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);
	ar >> m_BackgroundColor  >> m_TextColor;
	ar >> m_TextFont;
	ar >> m_ImageFormat;
	ar >> m_bSaveOpps >> m_bSaveWOpps;

	ar >> k;
	if(k<0 || k> MAXRECENTFILES)
	{
		pXFile->close();
		return;
	}
	m_RecentFiles.clear();
	for(int i=0; i<k; i++)
	{
		ar >> strange;
		m_RecentFiles.append(strange);
	}
	pAFoil->LoadSettings(ar);
	pXDirect->LoadSettings(ar);
	pMiarex->LoadSettings(ar);
	pXInverse->LoadSettings(ar);

	GL3DScales *p3DScales = (GL3DScales *)m_pGL3DScales;
	p3DScales->LoadSettings(ar);

	m_RefGraph.Serialize(ar, false);

	ar >> a>>b>>c>>d; //sanity check

	pXFile->close();

}


int MainFrame::LoadXFLR5File(QString PathName)
{
	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = "Could not read the file\n"+PathName;
		QMessageBox::information(window(), "Info", strange);
		return 0;
	}

	QXDirect * pXDirect = (QXDirect*)m_pXDirect;
	QString end;
	end = PathName.right(4);
	end = end.toLower();

	if(end==".plr")
	{
		QDataStream ar(&XFile);
		ar.setByteOrder(QDataStream::LittleEndian);
		CFoil *pFoil = ReadPolarFile(ar);
		pXDirect->m_bPolar = true;
		pXDirect->m_pCurPolar = NULL;
		pXDirect->m_pCurOpp   = NULL;
		m_pCurFoil = pXDirect->SetFoil(pFoil);
		pXDirect->SetPolar();

		XFile.close();

		AddRecentFile(PathName);
		SetSaveState(false);
		pXDirect->CheckButtons();
		return XFOILANALYSIS;
	}
	else
	{
		if(end==".dat")
		{
			QTextStream ar(&XFile);
			CFoil *pFoil = ReadFoilFile(ar);
			XFile.close();

			if(pFoil)
			{
				AddFoil(pFoil);
				pXDirect->m_pCurFoil  = pFoil;
				pXDirect->m_pCurPolar = NULL;
				pXDirect->m_pCurOpp   = NULL;
				m_pCurFoil = pXDirect->SetFoil(pFoil);
				pXDirect->SetPolar();
				QAFoil *pAFoil= (QAFoil*)m_pAFoil;
				pAFoil->SetFoil(pFoil);
				pAFoil->SelectFoil(pFoil);
				XFile.close();
				SetSaveState(false);
				AddRecentFile(PathName);
				pXDirect->CheckButtons();
				return XFOILANALYSIS;
			}
		}
		else
		{
			if(end==".wpa")
			{
				if(!m_bSaved)
				{
					QString strong = "Save the current project ?";
					int resp =  QMessageBox::question(0,"Save", strong,  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
					if(resp==QMessageBox::Cancel)
					{
						XFile.close();
						return 0;
					}
					else if (resp==QMessageBox::Yes)
					{
						if(!SaveProject(m_FileName))
						{
							XFile.close();
							return 0;
						}
					}
				}

				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

				DeleteProject();

				QDataStream ar(&XFile);
				ar.setByteOrder(QDataStream::LittleEndian);

				if(SerializeProject(ar, false))
				{
					m_pCurFoil = pXDirect->SetFoil();
					UpdateFoils();
					UpdateView();
				}

				AddRecentFile(PathName);
				SetSaveState(true);
				SetProjectName(PathName);

				XFile.close();
				QApplication::restoreOverrideCursor();

				pXDirect->CheckButtons();

				if(m_oaPlane.size() || m_oaWing.size()) return MIAREX;
				else                                    return XFOILANALYSIS;
			}
		}
	}
	XFile.close();

	return 0;
}



void MainFrame::OnAFoil()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->StopAnimate();

	m_iApp = DIRECTDESIGN;
	m_pctrlMiarexToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlAFoilToolBar->show();

	m_pctrlMiarexWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->hide();
	m_pctrlAFoilWidget->show();

	SetCentralWidget();
	SetMenus();
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->SetParams();
}


void MainFrame::OnCurFoilStyle()
{
	if(!m_pCurFoil) return;

	LinePickerDlg dlg;
	dlg.InitDialog(m_pCurFoil->m_nFoilStyle, m_pCurFoil->m_nFoilWidth, m_pCurFoil->m_FoilColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pCurFoil->m_FoilColor  = dlg.GetColor();
		m_pCurFoil->m_nFoilStyle = dlg.GetStyle();
		m_pCurFoil->m_nFoilWidth = dlg.GetWidth();
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->m_BufferFoil.m_FoilColor  = m_pCurFoil->m_FoilColor;
		pXDirect->m_BufferFoil.m_nFoilStyle = m_pCurFoil->m_nFoilStyle;
		pXDirect->m_BufferFoil.m_nFoilWidth = m_pCurFoil->m_nFoilWidth;
		SetSaveState(false);
	}
	UpdateView();
}


void MainFrame::OnExportCurGraph()
{
	QGraph *pGraph = NULL;

	switch(m_iApp)
	{
		case MIAREX:
		{
			QMiarex *pMiarex = (QMiarex*)m_pMiarex;
			pGraph = pMiarex->m_pCurGraph;
			break;
		}
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pGraph = pXDirect->m_pCurGraph;
			break;
		}
	}
	if(!pGraph) return;

	QFile DestFile;
	QString FileName;
	QString SelectedFilter;
	QFileDialog::Options options;
//	options |= QFileDialog::DontUseNativeDialog;

	pGraph->GetGraphName(FileName);
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, "Export Graph", m_LastDirName,
											"Text File (*.txt);;Comma Separated Values (*.csv)",
											&SelectedFilter, options);

	if(!FileName.length()) return;

	int pos, type;
	pos = FileName.lastIndexOf("/");
	if(pos>0) m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
	pos = FileName.indexOf(".csv");
	if(pos>0) type=2; else type=1;

	pGraph->ExportToFile(XFile, type);

}


void MainFrame::OnGuidelines()
{
	QString FileName = qApp->applicationDirPath() + "/Guidelines.pdf" ;
	QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
}


void MainFrame::OnInsertProject()
{
	QString PathName;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	PathName = QFileDialog::getOpenFileName(this, "Open File",
											m_LastDirName,
											"Project file (*.wpa)");
	if(!PathName.length())		return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) m_LastDirName = PathName.left(pos);


	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = "Could not read the file\n"+PathName;
		QMessageBox::information(window(), "Warning", strange);
		return;
	}
	QDataStream ar(&XFile);
	ar.setByteOrder(QDataStream::LittleEndian);

	SerializeProject(ar, false);

	SetSaveState(false);

	if(m_iApp == MIAREX)
	{
		UpdateUFOs();
		pMiarex->SetUFO();

		if(pMiarex->m_iView==2)      pMiarex->CreateWPolarCurves();
		else if(pMiarex->m_iView==1) pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==4) pMiarex->CreateCpCurves();
	}
	else if(m_iApp == XFOILANALYSIS)
	{
		if(pXDirect->m_bPolar) pXDirect->CreatePolarCurves();
		else		         pXDirect->CreateOppCurves();
		UpdateFoils();
	}
	else if(m_iApp == DIRECTDESIGN)
	{
		pAFoil->SetFoil();
	}
	UpdateView();
}


void MainFrame::OnLoadFile()
{
	QString PathName;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	PathName = QFileDialog::getOpenFileName(this, "Open File",
											m_LastDirName, 
											"XFLR5 file (*.dat *.plr *.wpa)");
	if(!PathName.length())		return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) m_LastDirName = PathName.left(pos);

	int App = LoadXFLR5File(PathName);

	if(m_iApp==0)
	{
		m_iApp = App;
		QString strange = PathName.right(4);
		strange = strange.toLower();
		if(strange == ".dat" || strange==".plr") OnXDirect();
		else if(strange==".wpa" && (m_oaWing.size() || m_oaPlane.size())) OnMiarex();
		else OnXDirect();
	}

	if(App==0)
	{
	}
	else if(m_iApp==XFOILANALYSIS)
	{
		if(m_oaPolar.size())
		{
			if(pXDirect->m_bPolar) pXDirect->CreatePolarCurves();
			else                   pXDirect->CreateOppCurves();
		}
		UpdateFoils();
		UpdateView();
	}
	else if(m_iApp==MIAREX)
	{
		UpdateUFOs();
		pMiarex->SetUFO();
		pMiarex->m_bArcball = false;
		pMiarex->m_bIs2DScaleSet = false;
		pMiarex->Set2DScale();
		pMiarex->CheckButtons();
		UpdateView();
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->SetParams();
		pAFoil->SelectFoil(pAFoil->m_pCurFoil);
		UpdateView();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		OnXInverse();
		UpdateView();
	}
}


void MainFrame::OnLogFile()
{
	QString FileName = QDir::tempPath() + "/QFLR5.log";
	// 20090605 Francesco Meschia
	QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
}


void MainFrame::OnMiarex()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->StopAnimate();

	QMiarex *pMiarex = (QMiarex*)m_pMiarex;	m_iApp = MIAREX;

	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlAFoilToolBar->hide();
	m_pctrlMiarexToolBar->show();

	m_pctrlXDirectWidget->hide();
	m_pctrlAFoilWidget->hide();
	m_pctrlXInverseWidget->hide();
	m_pctrlMiarexWidget->show();

	pMiarex->SetControls();
	pMiarex->SetUFO();
	pMiarex->m_bArcball = false;
	UpdateUFOs();
	SetMenus();
	SetCentralWidget();
	pMiarex->CheckButtons();
	UpdateView();
}


void MainFrame::OnNewProject()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;
	if(!m_bSaved)
	{
		int resp = QMessageBox::question(window(), "Question", "Save the current project ?",
										  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		if (QMessageBox::Cancel == resp)
		{
			return;
		}
		else if (QMessageBox::Yes == resp)
		{
			if(SaveProject(m_FileName))
			{
				SetSaveState(true);
				statusBar()->showMessage("The project " + m_ProjectName + " has been saved");
			}
			else return; //save failed, don't close
		}
		else if (QMessageBox::No == resp)
		{
			DeleteProject();
		}
	}
	else
	{
		DeleteProject();
	}

	UpdateView();
}


void MainFrame::OnResetCurGraphScales()
{
	QGraph *pGraph = NULL;
	switch(m_iApp)
	{
		case MIAREX:
		{
			QMiarex *pMiarex = (QMiarex*)m_pMiarex;
			pGraph = pMiarex->m_pCurGraph;
			break;
		}
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pGraph = pXDirect->m_pCurGraph;
			break;
		}
		case INVERSEDESIGN:
		{
			QXInverse *pXInverse = (QXInverse*)m_pXInverse;
			pGraph = &pXInverse->m_QGraph;
			pXInverse->ReleaseZoom();
			break;
		}
	}

	if(!pGraph) return;

	pGraph->SetAuto(true);
	UpdateView();
}


void MainFrame::OnRenameCurFoil()
{
	if(!m_pCurFoil) return;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;

	QString strong;
	bool bNotFound = true;
	int i,k,l;
	CFoil*pOldFoil;
	CPolar * pPolar;
	OpPoint * pOpPoint;

	QString OldName = m_pCurFoil->m_FoilName;

	while(bNotFound)
	{
		QStringList NameList;
		for(k=0; k<m_oaFoil.size(); k++)
		{
			pOldFoil = (CFoil*)m_oaFoil.at(k);
			NameList.append(pOldFoil->m_FoilName);
		}
		RenameDlg dlg(this);
		dlg.m_pstrArray = & NameList;
		dlg.m_strQuestion = "Enter the foil's new name";
		dlg.m_strName = OldName;
		bool bExists = false;
		dlg.InitDialog();
		int resp = dlg.exec();
		strong = dlg.m_strName;

		if(QDialog::Accepted == resp)
		{
			if (strong==OldName)
			{
				// do nothing
				bNotFound = false;
			}
			else
			{
				for (l=0; l<m_oaFoil.size(); l++)
				{
					pOldFoil = (CFoil*)m_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong)
					{
						bExists = true;
						break;
					}
				}
				if (!bExists)
				{
					bNotFound = false;// at last (users !...)
					// so rename the foil and associated polars and opps
					m_pCurFoil->m_FoilName = strong;
					bool bInserted = false;
					for(i=0;i<m_oaFoil.size();i++)
					{
						pOldFoil = (CFoil*)m_oaFoil.at(i);
						if(pOldFoil == m_pCurFoil)
						{
							m_oaFoil.removeAt(i);
							//and re-insert
							for(l=0;l<m_oaFoil.size();l++)
							{
								pOldFoil = (CFoil*)m_oaFoil.at(l);
								if(m_pCurFoil->m_FoilName.compare(pOldFoil->m_FoilName, Qt::CaseInsensitive)<0)
								{
									//then insert before
									m_oaFoil.insert(l, m_pCurFoil);
									bInserted = true;
									break;
								}
							}
							if(!bInserted)	m_oaFoil.append(m_pCurFoil);
							break;
						}
					}
					for (i=0; i<m_oaPolar.size(); i++)
					{
						pPolar = (CPolar*)m_oaPolar.at(i);
						if(pPolar->m_FoilName == OldName)
						{
							pPolar->m_FoilName = strong;
						}
					}
					for (i=0; i<m_oaOpp.size(); i++)
					{
						pOpPoint = (OpPoint*)m_oaOpp.at(i);
						if(pOpPoint->m_strFoilName == OldName)
						{
							pOpPoint->m_strFoilName = strong;
						}
					}
					SetSaveState(false);
				}
			}
		}
		else if(resp==10)
		{
			// user wants to overwrite an existing airfoil
			if (strong==OldName)
			{
				// do nothing
				bNotFound = false;
			}
			else
			{
				//So delete any foil with that name
				for (l=(int)m_oaFoil.size()-1;l>=0; l--)
				{
					pOldFoil = (CFoil*)m_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong)
					{
						if(m_pCurFoil == pOldFoil)           m_pCurFoil = NULL;
						if(pXDirect->m_pCurFoil == pOldFoil) pXDirect->m_pCurFoil = NULL;
						m_oaFoil.removeAt(l);
						delete pOldFoil;
					}
				}
				// delete all associated OpPoints
				OpPoint * pOpPoint;
				for (l=m_oaOpp.size()-1;l>=0;l--)
				{
					pOpPoint = (OpPoint*)m_oaOpp[l];
					if (pOpPoint->m_strFoilName == strong)
					{
						if(pOpPoint == pXDirect->m_pCurOpp) pXDirect->m_pCurOpp = NULL;
						m_oaOpp.removeAt(l);
						delete pOpPoint;
					}
				}
				// delete all Polar results for that airfoil
				CPolar * pPolar;
				for (l=m_oaPolar.size()-1;l>=0;l--)
				{
					pPolar = (CPolar*) m_oaPolar.at(l);
					if (pPolar->m_FoilName == strong)
					{
						if(pXDirect->m_pCurPolar == pPolar) pXDirect->m_pCurPolar = NULL;
						m_oaPolar.removeAt(l);
						delete pPolar;
					}
				}
				// finally add to array
				m_pCurFoil->m_FoilName = strong;
				for (i=0; i<m_oaPolar.size(); i++)
				{
					pPolar = (CPolar*)m_oaPolar.at(i);
					if(pPolar->m_FoilName == OldName)
					{
						pPolar->m_FoilName = strong;
					}
				}
				for (i=0; i<m_oaOpp.size(); i++)
				{
					pOpPoint = (OpPoint*)m_oaOpp.at(i);
					if(pOpPoint->m_strFoilName == OldName)
					{
						pOpPoint->m_strFoilName = strong;
					}
				}
	//			UpdateFoils();
				SetSaveState(false);
				bNotFound = false;//exit loop
			}
		}
		else
		{
			// Cancel so exit
			bNotFound = false;//exit loop
		}
	}

	pXDirect->SetFoil(m_pCurFoil);
	UpdateFoils();
	UpdateView();
}

void MainFrame::OnRestoreToolbars()
{
	if(m_iApp==XFOILANALYSIS)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
		m_pctrlMiarexToolBar->hide();
		m_pctrl3DScalesWidget->hide();

		m_pctrlAFoilWidget->hide();
		m_pctrlXInverseWidget->hide();
		m_pctrlMiarexWidget->hide();

		m_pctrlXDirectToolBar->show();
		m_pctrlXDirectWidget->show();
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlMiarexToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		m_pctrl3DScalesWidget->hide();

		m_pctrlXDirectWidget->hide();
		m_pctrlXInverseWidget->hide();
		m_pctrlMiarexWidget->hide();

		m_pctrlAFoilToolBar->show();
		m_pctrlAFoilWidget->show();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		m_pctrlAFoilToolBar->hide();
		m_pctrlMiarexToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		m_pctrl3DScalesWidget->hide();

		m_pctrlAFoilWidget->hide();
		m_pctrlXDirectWidget->hide();
		m_pctrlMiarexWidget->hide();

		m_pctrlXInverseToolBar->show();
		m_pctrlXInverseWidget->show();
	}
	else if(m_iApp==MIAREX)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		m_pctrl3DScalesWidget->hide();

		m_pctrlAFoilWidget->hide();
		m_pctrlXDirectWidget->hide();
		m_pctrlXInverseWidget->hide();
		m_pctrlMiarexWidget->show();
		m_pctrlMiarexToolBar->show();
	}

}
void MainFrame::OnSaveOptions()
{
	SaveOptionsDlg dlg;
	dlg.InitDialog(m_bSaveOpps, m_bSaveWOpps);
	if(dlg.exec()==QDialog::Accepted)
	{
		m_bSaveOpps  = dlg.m_bOpps;
		m_bSaveWOpps = dlg.m_bWOpps;
	}
}


void MainFrame::OnSaveProject()
{
	if (!m_ProjectName.length() || m_ProjectName=="*")
	{
		OnSaveProjectAs();
		return;
	}
	if(SaveProject(m_FileName))
	{
		SetSaveState(true);
		statusBar()->showMessage("The project " + m_ProjectName + " has been saved");
	}
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;
}



bool MainFrame::OnSaveProjectAs()
{
	if(SaveProject())
	{
		SetProjectName(m_FileName);
		AddRecentFile(m_FileName);
		statusBar()->showMessage("The project " + m_ProjectName + " has been saved");
		SetSaveState(true);
	}
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;
	return true;
}


void MainFrame::OnSaveUFOAsProject()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	QString strong;
	if(pMiarex->m_pCurPlane)     strong = pMiarex->m_pCurPlane->m_PlaneName;
	else if(pMiarex->m_pCurWing) strong = pMiarex->m_pCurWing->m_WingName;
	else
	{
		QMessageBox::warning(this, "Warning", "Nothing to save");
		return ;
	}

	QString PathName;
	QString Filter = ".wpa";
	QString FileName = strong;

	FileName.replace("/", " ");
	PathName = QFileDialog::getSaveFileName(this, "Save the Project File",
											m_LastDirName+"/"+FileName,
											"XFLR5 Project File (*.wpa)", &Filter);
	if(!PathName.length()) return;//nothing more to do
	int pos = PathName.indexOf(".wpa", Qt::CaseInsensitive);
	if(pos<0) PathName += ".wpa";
	pos = PathName.lastIndexOf("/");
	if(pos>0) m_LastDirName = PathName.left(pos);


	QFile fp(PathName);

	if (!fp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
		return;
	}

	QDataStream ar(&fp);
	ar.setByteOrder(QDataStream::LittleEndian);
	SerializeUFOProject(ar);
	m_FileName = PathName;
	fp.close();

}

void MainFrame::OnSaveViewToImageFile()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	QSize sz(m_p2DWidget->geometry().width(), m_p2DWidget->geometry().height());
	QImage img(sz, QImage::Format_RGB32);
	QPainter painter(&img);
	QString FileName;
	QString Filter;
	switch(m_ImageFormat)
	{
		case 0 :
		{
			Filter = "Windows Bitmap (*.bmp)";
			break;
		}
		case 1 :
		{
			Filter = "JPEG (*.jpg)";
			break;
		}
		case 2 :
		{
			Filter = "Portable Network Graphics (*.png)";
			break;
		}

	}
	FileName = QFileDialog::getSaveFileName(this, "Save Image",
											m_LastDirName,
											"Windows Bitmap (*.bmp);;JPEG (*.jpg);;Portable Network Graphics (*.png)",
											&Filter);
	if(!FileName.length()) return;

	if(Filter == "Windows Bitmap (*.bmp)")
	{
		if(FileName.right(4)!=".bmp") FileName+= ".bmp";
		m_ImageFormat = 0;
	}
	else if(Filter == "JPEG (*.jpg)")
	{
		if(FileName.right(4)!=".jpg") FileName+= ".jpg";
		m_ImageFormat = 1;
	}
	else if(Filter == "Portable Network Graphics (*.png)")
	{
		if(FileName.right(4)!=".png") FileName+= ".png";
		m_ImageFormat = 2;
	}


	switch(m_iApp)
	{
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->PaintView(painter);
			break;
		}
		case DIRECTDESIGN:
		{
			QAFoil *pAFoil = (QAFoil*)m_pAFoil;
			pAFoil->PaintView(painter);
			break;
		}
		case INVERSEDESIGN:
		{
			QXInverse *pXInverse = (QXInverse*)m_pXInverse;
			pXInverse->PaintView(painter);
			break;
		}
		case MIAREX:
		{
			QMiarex *pMiarex = (QMiarex*)m_pMiarex;
			pMiarex->m_bArcball = false;

			if(pMiarex->m_iView==3)
			{
				pMiarex->UpdateView();
				pMiarex->SnapClient(FileName, m_ImageFormat);
				return;
			}
			else pMiarex->PaintView(painter);
			break;
		}
	}
	img.save(FileName);
}


void MainFrame::OnSelChangeUFO(int i)
{
	// Gets the new selected wing name and notifies Miarex
	// then updates WPolar combobox
	// and selects either the current WPolar
	// or the first one in the list, if any
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();
	QString strong;
	int sel = m_pctrlUFO->currentIndex();
	if (sel >=0) strong = m_pctrlUFO->itemText(sel);
	pMiarex->SetUFO(strong);

	m_iApp = MIAREX;
	UpdateWPolars();
	UpdateWOpps();
	pMiarex->SetWPlr(false);
	pMiarex->m_bIs2DScaleSet = false;
	pMiarex->Set2DScale();
	pMiarex->CheckButtons();
	pMiarex->UpdateView();
}


void MainFrame::OnSelChangeWPolar(int i)
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QString strong;
	int sel = m_pctrlWPolar->currentIndex();
	if (sel >=0) strong = m_pctrlWPolar->itemText(sel);
	m_iApp = MIAREX;
	pMiarex->SetWPlr(false, strong);
	pMiarex->SetWOpp(true);
	pMiarex->CheckButtons();
	pMiarex->UpdateView();
}


void MainFrame::OnSelChangeWOpp(int i)
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	// Gets the new selected WOpp name and notifies Miarex
	if(!m_pctrlWOpp->count())
	{
		pMiarex->m_pCurWOpp = NULL;
		if (pMiarex->m_iView==1)     pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==4) pMiarex->CreateCpCurves();
		pMiarex->UpdateView();
		return;
	}

	QString strong;

	if (i >=0) strong = m_pctrlWOpp->itemText(i);
	else
	{
		m_pctrlWOpp->setCurrentIndex(0);
		strong = m_pctrlWOpp->itemText(0);
	}

	if(strong.length())
	{
		bool bOK;
		double Alpha = strong.toDouble(&bOK);
		if(bOK)
		{
			m_iApp = MIAREX;
			pMiarex->SetWOpp(false, Alpha);
			pMiarex->UpdateView();
		}
		else
		{
			QMessageBox::warning(window(), tr("Warning"), tr("Unidentified Operating Point"));
			pMiarex->m_pCurWOpp = NULL;
			pMiarex->SetWOpp(true);
		}
	}
}


void MainFrame::OnSelChangeFoil(int i)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	i=0;
	// Gets the new selected foil name and notifies XDirect
	// sets a polar, if any
	// then updates the polar combobox

	pXDirect->m_bAnimate = false;

	QString strong;
	int sel = m_pctrlFoil->currentIndex();
	if (sel >=0) strong = m_pctrlFoil->itemText(sel);

	m_pCurFoil = GetFoil(strong);
	pXDirect->SetFoil(m_pCurFoil);
	pXDirect->SetPolar();
	m_iApp = XFOILANALYSIS;
	UpdatePolars();
	pXDirect->CheckButtons();
	UpdateView();
}


void MainFrame::OnSelChangePolar(int i)
{
	i=0;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->m_bAnimate = false;

	// Gets the new selected polar name and notifies XDirect
	QString strong;
	int sel = m_pctrlPolar->currentIndex();
	if (sel >= 0) strong = m_pctrlPolar->itemText(sel);
	m_iApp = XFOILANALYSIS;
	pXDirect->SetPolar(strong);
	UpdateOpps();
	pXDirect->CheckButtons();
	UpdateView();
}


void MainFrame::OnSelChangeOpp(int i)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->m_bAnimate = false;

	// Gets the new selected Opp name and notifies XDirect
	QString strong;

	if (i>=0) strong = m_pctrlOpPoint->itemText(i);
	m_iApp = XFOILANALYSIS;

	double Alpha;
 	bool bOK;
	Alpha = strong.toFloat(&bOK);
	if(bOK)
	{
		pXDirect->SetOpp(Alpha);
	}
	else
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Unidentified Operating Point"));
		pXDirect->m_pCurOpp = NULL;
		pXDirect->SetOpp();
	}
	UpdateView();
}



void MainFrame::OnStyle()
{
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex     = (QMiarex*)m_pMiarex;
//	QXInverse *pXInverse = (QXInverse*)m_pXInverse;

	DisplaySettingsDlg dlg(this);
	dlg.m_BackgroundColor = m_BackgroundColor;
	dlg.m_TextColor       = m_TextColor;
	dlg.m_TextFont        = m_TextFont;
	dlg.m_pRefGraph       = &m_RefGraph;
	dlg.m_StyleName       = m_StyleName;

	dlg.InitDialog();

	if(dlg.exec() ==QDialog::Accepted)
	{
		m_BackgroundColor = dlg.m_BackgroundColor;
		m_TextColor       = dlg.m_TextColor;
		m_TextFont        = dlg.m_TextFont;
		m_StyleName       = dlg.m_StyleName;

		pMiarex->m_bResetglLegend = true;

		if(dlg.m_bIsGraphModified)
		{
			SetGraphSettings(&m_RefGraph);
		}
	}

	pXDirect->m_pCpGraph->SetInverted(true);
	UpdateView();
}


void MainFrame::OnUnits()
{
	UnitsDlg dlg;
	dlg.m_Length = m_LengthUnit;
	dlg.m_Area   = m_AreaUnit;
	dlg.m_Weight = m_WeightUnit;
	dlg.m_Speed  = m_SpeedUnit;
	dlg.m_Force  = m_ForceUnit;
	dlg.m_Moment = m_MomentUnit;
	dlg.InitDialog();

	if(dlg.exec()==QDialog::Accepted)
	{
		m_LengthUnit = dlg.m_Length;
		m_AreaUnit   = dlg.m_Area;
		m_WeightUnit = dlg.m_Weight;
		m_SpeedUnit  = dlg.m_Speed;
		m_ForceUnit  = dlg.m_Force;
		m_MomentUnit = dlg.m_Moment;

		SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
				 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

		SetSaveState(false);

		if(m_iApp==MIAREX)
		{
			QMiarex *pMiarex= (QMiarex*)m_pMiarex;
			pMiarex->UpdateUnits();
		}
	}
}


void MainFrame::OnXDirect()
{
	UpdateFoils();
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->SetFoilScale();
	m_iApp = XFOILANALYSIS;

	m_pctrlMiarexToolBar->hide();
	m_pctrlAFoilToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlXDirectToolBar->show();

	m_pctrlAFoilWidget->hide();
	m_pctrlMiarexWidget->hide();
	m_pctrlXInverseWidget->hide();
	m_pctrlXDirectWidget->show();

	SetCentralWidget();
	SetMenus();
	pXDirect->SetPolarLegendPos();
	pXDirect->CheckButtons();
	pXDirect->UpdateView();
}


void MainFrame::OnXInverse()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->StopAnimate();
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
//	pXInverse->SetScale();
	m_iApp = INVERSEDESIGN;

	m_pctrlMiarexToolBar->hide();
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->show();

	m_pctrlAFoilWidget->hide();
	m_pctrlMiarexWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->show();

	SetCentralWidget();
	pXInverse->m_bFullInverse = true;
	SetMenus();
	pXInverse->SetParams();
}


void MainFrame::OnXInverseMixed()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->StopAnimate();
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
//	pXInverse->SetScale();
	m_iApp = INVERSEDESIGN;
	m_pctrlMiarexToolBar->hide();
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->show();

	m_pctrlAFoilWidget->hide();
	m_pctrlMiarexWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->show();
	pXInverse->m_bFullInverse = false;
	SetCentralWidget();
	SetMenus();
	pXInverse->SetParams();
}

void MainFrame::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action) return;

	QXDirect *pXDirect = (QXDirect*) m_pXDirect;
	QMiarex *pMiarex = (QMiarex*) m_pMiarex;

	int App = LoadXFLR5File(action->data().toString());

	if(m_iApp==0) m_iApp = App;

	if(App==0)
	{
		m_iApp = App;
		QString FileName = action->data().toString();
		m_RecentFiles.removeAll(FileName);
		updateRecentFileActions();
	}

	else if(m_iApp==XFOILANALYSIS)
	{
		if(m_oaPolar.size())
		{
			if(pXDirect->m_bPolar) pXDirect->CreatePolarCurves();
			else                   pXDirect->CreateOppCurves();
		}
		OnXDirect();
		UpdateFoils();
		UpdateView();

	}
	else if(m_iApp==MIAREX)
	{
//		UpdateUFOs();
//		pMiarex->SetUFO();
		pMiarex->m_bIs2DScaleSet = false;
		pMiarex->Set2DScale();
		OnMiarex();
		UpdateView();
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->SetParams();
		UpdateView();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		OnXInverse();
		UpdateView();
	}
}


CFoil* MainFrame::ReadFoilFile(QTextStream &in)
{
	QString Strong, StrTemp;
	QString FoilName;
	CFoil* pFoil = NULL;
	int pos, res, line, i, ip;
	pos = line = 0;
	double x,y, area;

	bool bRead;

	QByteArray textline;
	const char *text;

	pFoil = new CFoil();
	if(!pFoil)	return NULL;

	while(pos>=0 && !in.atEnd())
	{
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);
	}

	if(!in.atEnd())
	{
		textline = Strong.toAscii();
		text = textline.constData();
		res = sscanf(text, "%lf%lf", &x,&y);
		if(res==2)
		{
			//there isn't a name on the first line
			FoilName = "New Foil";
			{
				pFoil->xb[0] = x;
				pFoil->yb[0] = y;
				pFoil->nb=1;
			}
		}
		else FoilName = Strong;
		// remove fore and aft spaces
		FoilName = FoilName.trimmed();
	}

	bRead = true;

	while (bRead)
	{
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);

		if (bRead && pos<0)
		{

			textline = Strong.toAscii();
			text = textline.constData();
			res = sscanf(text, "%lf%lf", &x,&y);
			if(res==2)
			{
				pFoil->xb[pFoil->nb] = x;
				pFoil->yb[pFoil->nb] = y;
				pFoil->nb++;
				if(pFoil->nb>IQX)
				{
					delete pFoil;
					return NULL;
				}
			}
			else bRead = false;
		}
	}

	pFoil->m_FoilName = FoilName;

// Check if the foil was written clockwise or counter-clockwise

	area = 0.0;
	for (i=0; i<pFoil->nb; i++)
	{
		if(i==pFoil->nb-1)	ip = 0;
		else				ip = i+1;
		area +=  0.5*(pFoil->yb[i]+pFoil->yb[ip])*(pFoil->xb[i]-pFoil->xb[ip]);
	}

	if(area < 0.0)
	{
		//reverse the points order
		double xtmp, ytmp;
		for (int i=0; i<pFoil->nb/2; i++)
		{
			xtmp         = pFoil->xb[i];
			ytmp         = pFoil->yb[i];
			pFoil->xb[i] = pFoil->xb[pFoil->nb-i-1];
			pFoil->yb[i] = pFoil->yb[pFoil->nb-i-1];
			pFoil->xb[pFoil->nb-i-1] = xtmp;
			pFoil->yb[pFoil->nb-i-1] = ytmp;
		}
	}

	memcpy(pFoil->x, pFoil->xb, sizeof(pFoil->xb));
	memcpy(pFoil->y, pFoil->yb, sizeof(pFoil->yb));
	pFoil->n=pFoil->nb;

	pFoil->m_FoilColor = GetColor(0);
	pFoil->InitFoil();

	return pFoil;
}


CFoil * MainFrame::ReadPolarFile(QDataStream &ar)
{
//	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QString FoilName;
	CFoil* pFoil = NULL;
	CPolar *pPolar = NULL;
	CPolar * pOldPolar;
	int i, n, l;

	ar >> n;

	if(n<100000)
	{
		//old format
		QMessageBox::warning(window(), "Warning", "Obsolete format, cannot read");
		return NULL;
	}
	else if (n >=100000)
	{
		//new format XFLR5 v1.99+
		//first read all available foils
		ar>>n;
		for (i=0;i<n; i++)
		{
			pFoil = new CFoil();
			pFoil->Serialize(ar, false);
			if (!pFoil->m_FoilName.length())
			{
				delete pFoil;
				return NULL;
			}
			AddFoil(pFoil);
		}

		//next read all available polars

		ar>>n;
		for (i=0;i<n; i++)
		{
			pPolar = new CPolar();

			pPolar->m_Color = QColor(56,128,188,127);
			pPolar->Serialize(ar, false);
			if (!pPolar->m_FoilName.length())
			{
				delete pPolar;
				return pFoil;
			}
			for (l=0; l<m_oaPolar.size(); l++)
			{
				pOldPolar = (CPolar*)m_oaPolar[l];
				if (pOldPolar->m_FoilName == pPolar->m_FoilName &&
					pOldPolar->m_PlrName  == pPolar->m_PlrName)
				{
					//just overwrite...
					m_oaPolar.removeAt(l);
					delete pOldPolar;
					//... and continue to add
				}
			}
			pPolar = AddPolar(pPolar);

		}
	}
	return pFoil;
}



void MainFrame::RemoveOpPoint(bool bCurrent)
{
	int i;
	QXDirect *pXDirect =(QXDirect*)m_pXDirect;

	if(bCurrent)
	{
		// we remove only the current OpPoint
		OpPoint* pOpPoint = pXDirect->m_pCurOpp;
		OpPoint* opt;
		for (i=0; i<m_oaOpp.size(); i++)
		{
			opt =(OpPoint*)m_oaOpp.at(i);
			if (pOpPoint == opt)
			{
				m_oaOpp.removeAt(i);
				delete pOpPoint;
				break;
			}
		}
	}
	else
	{
		// kill'em all
		OpPoint* pOpPoint;
		for (i=m_oaOpp.size()-1; i>=0;i--)
		{
			pOpPoint =(OpPoint*)m_oaOpp.at(i);
			if (pOpPoint->m_strFoilName == pXDirect->m_pCurFoil->m_FoilName &&
				pOpPoint->m_strPlrName == pXDirect->m_pCurPolar->m_PlrName)
			{
				m_oaOpp.removeAt(i);
				delete pOpPoint;
			}
		}
	}

	pXDirect->m_pCurOpp = NULL;
}


bool MainFrame::SaveProject(QString PathName)
{
	if(!PathName.length())
	{
		QString Filter = ".wpa";
		QString FileName = m_ProjectName;
		if(FileName.right(1)=="*") 	FileName = FileName.left(FileName.length()-1);
		FileName.replace("/", " ");

		PathName = QFileDialog::getSaveFileName(this, "Save the Project File",
												m_LastDirName+"/"+FileName,
												"XFLR5 Project File (*.wpa)", &Filter);
		if(!PathName.length()) return false;//nothing more to do
		int pos = PathName.indexOf(".wpa", Qt::CaseInsensitive);
		if(pos<0) PathName += ".wpa";
		pos = PathName.lastIndexOf("/");
		if(pos>0) m_LastDirName = PathName.left(pos);
	}

	QString strong;
	QFile fp(PathName);

	if (!fp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
		return false;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QDataStream ar(&fp);
	ar.setByteOrder(QDataStream::LittleEndian);
	SerializeProject(ar,true);
	m_FileName = PathName;
	fp.close();

	QApplication::restoreOverrideCursor();
	return true;
}


void MainFrame::SaveSettings()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	QString FileName = QDir::tempPath() + "/QFLR5.set";
	QFile *pXFile = new QFile(FileName);

	if (!pXFile->open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("could not write saved settings"));
		return;
	}

	QDataStream ar(pXFile);

	ar << m_SettingsFormat;
	ar << frameGeometry().x();
	ar << frameGeometry().y();
	ar << frameGeometry().width();
	ar << frameGeometry().height();
	ar << isMaximized();
	ar << m_StyleName;

	ar <<  m_pctrlMiarexWidget->isFloating() << m_pctrlMiarexWidget->frameGeometry().x() << m_pctrlMiarexWidget->frameGeometry().y();
	ar <<  m_pctrlXDirectWidget->isFloating() << m_pctrlXDirectWidget->frameGeometry().x() << m_pctrlXDirectWidget->frameGeometry().y();
	ar <<  m_pctrlAFoilWidget->isFloating() << m_pctrlAFoilWidget->frameGeometry().x() << m_pctrlAFoilWidget->frameGeometry().y();
	ar <<  m_pctrlAFoilWidget->frameGeometry().width() <<  m_pctrlAFoilWidget->frameGeometry().height();
	ar <<  m_pctrlXInverseWidget->isFloating() << m_pctrlXInverseWidget->frameGeometry().x() << m_pctrlXInverseWidget->frameGeometry().y();


	ar << m_LastDirName;

	ar << m_LengthUnit << m_AreaUnit << m_WeightUnit << m_SpeedUnit << m_ForceUnit << m_MomentUnit;
	ar << m_BackgroundColor <<  m_TextColor;
	ar << m_TextFont ;
	ar << m_ImageFormat;
	ar << m_bSaveOpps << m_bSaveWOpps;

	ar << m_RecentFiles.size();
	for(int i=0; i<m_RecentFiles.size(); i++)
		ar << m_RecentFiles.at(i);

	pAFoil->SaveSettings(ar);
	pXDirect->SaveSettings(ar);
	pMiarex->SaveSettings(ar);
	pXInverse->SaveSettings(ar);

	GL3DScales *p3DScales = (GL3DScales *)m_pGL3DScales;
	p3DScales->SaveSettings(ar);

	m_RefGraph.Serialize(ar, true);
	ar << 1 <<10 <<101 << 123456;
	pXFile->close();
}


void MainFrame::SetCentralWidget()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	if(m_iApp!=MIAREX || (m_iApp==MIAREX && pMiarex->m_iView!=3))
	{
		m_pctrlCentralWidget->setCurrentIndex(0);
	}
	else if(m_iApp==MIAREX && pMiarex->m_iView==3)
	{
		m_pctrlCentralWidget->setCurrentIndex(1);
	}
}


bool MainFrame::SelectFoil(CFoil *pFoil)
{
    //Selects pFoil in the combobox and returns true
    //On error, selects the first and returns false
    return true;
    if(!pFoil) return false;
    int pos = m_pctrlFoil->findText(pFoil->m_FoilName);
    if(pos>=0)
    {
	    m_pctrlFoil->setCurrentIndex(pos);
	    return true;
    }
    return false;
}

bool MainFrame::SelectPolar(CPolar *pPolar)
{
    //Selects pPolar in the combobox and returns true
    //On error, selects the first and returns false
	if(!m_pCurFoil || !pPolar) return false;

    CPolar *pOldPolar;
    for(int i=0; i<m_pctrlPolar->count(); i++)
    {
		pOldPolar = GetPolar(m_pCurFoil->m_FoilName, m_pctrlPolar->itemText(i));
		if(pOldPolar && pPolar==pOldPolar)
		{
			//TODO : check if this activates the Selchange signal
			m_pctrlPolar->setCurrentIndex(i);
			return true;
		}
    }
    return false;
}

bool MainFrame::SelectOpPoint(OpPoint *pOpp)
{
    //Selects pOpp in the combobox and returns true
    //On error, selects the first and returns false
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	CPolar *pCurPlr    = pXDirect->m_pCurPolar;
    if(!pOpp || !pCurPlr) return false;
    QString strong;
    double alpha, Re;

    for(int i=0; i<m_pctrlOpPoint->count(); i++)
    {
	    if(pCurPlr->m_Type !=4)
	    {
		    alpha = m_pctrlOpPoint->itemText(i).toDouble();
		    if(fabs(alpha-pOpp->Alpha)<0.01)
		    {
			    m_pctrlOpPoint->setCurrentIndex(i);
			    return true;
		    }
	    }
	    else
	    {
		    Re = m_pctrlOpPoint->itemText(i).toDouble();
		    if(fabs(Re-pOpp->Reynolds)<1.0)
		    {
			    m_pctrlOpPoint->setCurrentIndex(i);
			    return true;
		    }
	    }
    }
    return false;
}

bool MainFrame::SerializeUFOProject(QDataStream &ar)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	if(!pMiarex->m_pCurWing)
	{
		QMessageBox::warning(this, "Warning", "Nothing to save");
		return false;
	}
	QString UFOName;
	//saves only the UFO with its associated plrs, foils and other results
	CPlane *pPlane   = pMiarex->m_pCurPlane;
	CWing *pWing     = pMiarex->m_pCurWing;
	CWing *pStab     = pMiarex->m_pCurStab;
	CWing *pFin      = pMiarex->m_pCurFin;
	CWPolar *pWPolar = NULL;
//	CWOpp *pWOpp     = NULL;
	CFoil *pFoil  = NULL;
	CPolar * pPolar  = NULL;

	if(pPlane) UFOName = pPlane->m_PlaneName;
	else       UFOName = pWing->m_WingName;

	int i,j;
	QString strong;

	// storing code
	ar << 100011;
	// 100011 : Added Body serialization
	// 100010 : Converted to I.S. units
	// 100009 : added serialization of opps in numbered format
	// 100008 : Added m_WngAnalysis.m_bTiltedGeom, m_WngAnalysis.m_bWakeRollUp
	// 100006 : version 2.99Beta001 format
	// 100005 : version 2.00 format
	//Archive format introduced in Beta V22
	// Write units //added in Beta v16
	// Save analysis settings // added in Beta v22
	ar << m_LengthUnit;
	ar << m_AreaUnit;
	ar << m_WeightUnit;
	ar << m_SpeedUnit;
	ar << m_ForceUnit;
	ar << m_MomentUnit;
	ar << pMiarex->m_WngAnalysis.m_Type;
	ar << (float)pMiarex->m_WngAnalysis.m_Weight;
	ar << (float)pMiarex->m_WngAnalysis.m_QInf;
	ar << (float)pMiarex->m_WngAnalysis.m_XCmRef;
	ar << (float)pMiarex->m_WngAnalysis.m_Density;
	ar << (float)pMiarex->m_WngAnalysis.m_Viscosity;

//	pMiarex->m_CtrlDlg.m_QInf      = pMiarex->m_WngAnalysis.m_QInf;
//	pMiarex->m_CtrlDlg.m_Weight    = pMiarex->m_WngAnalysis.m_Weight;
//	pMiarex->m_CtrlDlg.m_Viscosity = pMiarex->m_WngAnalysis.m_Viscosity;
//	pMiarex->m_CtrlDlg.m_Density   = pMiarex->m_WngAnalysis.m_Density;

	ar << (float)pMiarex->m_WngAnalysis.m_Alpha;
	ar << pMiarex->m_WngAnalysis.m_AnalysisType;

	if (pMiarex->m_WngAnalysis.m_bVLM1)   ar << 1;
	else								  ar << 0;
//		if (pMiarex->m_WngAnalysis.m_bMiddle) ar << 1; else ar << 0;
	ar <<1;
	if (pMiarex->m_WngAnalysis.m_bTiltedGeom) ar << 1;
	else									  ar << 0;
	if (pMiarex->m_WngAnalysis.m_bWakeRollUp) ar << 1;
	else									  ar << 0;

	// Store the wing, if any
	if(!pPlane)
	{
		ar << 1;//just one wing
		pWing->SerializeWing(ar,true);
	}
	else ar <<0;

	// now store all the WPolars associated to the UFO
	//start by listing the number
	int size = 0;
	for (i=0; i<m_oaWPolar.size();i++)
	{
		pWPolar = (CWPolar*)m_oaWPolar.at(i);
		if(pWPolar->m_UFOName == UFOName) size++;
	}

	ar << size;
	for (i=0; i<m_oaWPolar.size();i++)
	{
		pWPolar = (CWPolar*)m_oaWPolar.at(i);
		pWPolar->m_pParent = this;
		if(pWPolar->m_UFOName == UFOName) pWPolar->SerializeWPlr(ar, true);
	}

	ar << 0;//no WOpps

	// then the foils,  polars and Opps
	//list the foils to be saved
	QStringList FoilList;
	bool bFound = false;
	if(pWing)
	{
		for (i=0; i<=pWing->m_NPanel; i++)
		{
			pFoil = GetFoil(pWing->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.size();j++)
			{
				if(pFoil->m_FoilName == FoilList.at(j))
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.append(pFoil->m_FoilName);
		}
		if(!pWing->m_bSymetric)
		{
			for (i=0; i<=pWing->m_NPanel; i++)
			{
				pFoil = GetFoil(pWing->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.size();j++)
				{
					if(pFoil->m_FoilName == FoilList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.append(pFoil->m_FoilName);
			}
		}
	}
	if(pStab)
	{
		for (i=0; i<=pStab->m_NPanel; i++)
		{
			pFoil = GetFoil(pStab->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.size();j++)
			{
				if(pFoil->m_FoilName == FoilList.at(j))
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.append(pFoil->m_FoilName);
		}
		if(!pStab->m_bSymetric)
		{
			for (i=0; i<=pStab->m_NPanel; i++)
			{
				pFoil = GetFoil(pStab->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.size();j++)
				{
					if(pFoil->m_FoilName == FoilList.at(j)){
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.append(pFoil->m_FoilName);
			}
		}
	}
	if(pFin)
	{
		for (i=0; i<=pFin->m_NPanel; i++)
		{
			pFoil = GetFoil(pFin->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.size();j++)
			{
				if(pFoil->m_FoilName == FoilList.at(j))
				{
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.append(pFoil->m_FoilName);
		}
		if(!pFin->m_bSymetric)
		{
			for (i=0; i<=pFin->m_NPanel; i++)
			{
				pFoil = GetFoil(pFin->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.size();j++)
				{
					if(pFoil->m_FoilName == FoilList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.append(pFoil->m_FoilName);
			}
		}
	}

	//write the number of foils
	ar << 100001;//unused
	ar << (int)FoilList.size();
	for (i=0; i<(int)FoilList.size(); i++)
	{
		pFoil = GetFoil(FoilList[i]);
		pFoil->Serialize(ar, true);
	}
	int n = 0;
	for (i=0; i<FoilList.size(); i++)
	{
		pFoil = GetFoil(FoilList[i]);
		//count polars associated to the foil
		for (int i=0; i<m_oaPolar.size();i++)
		{
			pPolar = (CPolar*)m_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName)
				n++;
		}
	}
	//then write polars
	ar << n;
	for (i=0; i<FoilList.size(); i++)
	{
		pFoil = GetFoil(FoilList[i]);
		for (j=0; j<m_oaPolar.size();j++)
		{
			pPolar = (CPolar*)m_oaPolar.at(j);
			if (pPolar->m_FoilName == pFoil->m_FoilName)
			{
				pPolar->Serialize(ar, true);
			}
		}
	}

	ar<<0;//no Opps

	// next the bodies

	if(pPlane && pPlane->m_bBody && pPlane->m_pBody)
	{
		ar << 1;
		pPlane->m_pBody->SerializeBody(ar, true);
	}
	else ar <<0; //no plane

	// last write the plane...

	if(pPlane)
	{
		ar <<1;
		pPlane->SerializePlane(ar, true);
	}
	else ar <<0;

	ar << 0;//no POpps

	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	pAFoil->m_pSF->Serialize(ar, true);
	pAFoil->m_pPF->Serialize(ar, true);

	return true;
}


bool MainFrame::SerializeProject(QDataStream &ar, bool bIsStoring)
{
//	CWaitCursor Wait;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	CWing *pWing     = NULL;
	CWPolar *pWPolar = NULL;
	CWOpp *pWOpp     = NULL;
	CPOpp *pPOpp     = NULL;
	CPlane *pPlane   = NULL;
	CBody *pBody     = NULL;
	CPolar *pPolar   = NULL;
	CFoil *pFoil     = NULL;


	QString str, strong;
	int i, n, j, k;
	float f;

	if (bIsStoring)
	{
		// storing code
		ar << 100012;
		// 100012 : Added sideslip
		// 100011 : Added Body serialization
		// 100010 : Converted to I.S. units
		// 100009 : added serialization of opps in numbered format
		// 100008 : Added m_WngAnalysis.m_bTiltedGeom, m_WngAnalysis.m_bWakeRollUp
		// 100006 : version 2.99Beta001 format
		// 100005 : version 2.00 format
		//Archive format introduced in Beta V22
		// Write units //added in Beta v16
		// Save analysis settings // added in Beta v22
		ar << m_LengthUnit;
		ar << m_AreaUnit;
		ar << m_WeightUnit;
		ar << m_SpeedUnit;
		ar << m_ForceUnit;
		ar << m_MomentUnit;

		ar << pMiarex->m_WngAnalysis.m_Type;
		ar << (float)pMiarex->m_WngAnalysis.m_Weight;
		ar << (float)pMiarex->m_WngAnalysis.m_QInf;
		ar << (float)pMiarex->m_WngAnalysis.m_XCmRef;
		ar << (float)pMiarex->m_WngAnalysis.m_Density;
		ar << (float)pMiarex->m_WngAnalysis.m_Viscosity;
		ar << (float)pMiarex->m_WngAnalysis.m_Alpha;
		ar << (float)pMiarex->m_WngAnalysis.m_Beta;
		ar << pMiarex->m_WngAnalysis.m_AnalysisType;

		if (pMiarex->m_WngAnalysis.m_bVLM1)   ar << 1;
		else								ar << 0;
//		if (pMiarex->m_WngAnalysis.m_bMiddle) ar << 1; else ar << 0;
		ar <<1;
		if (pMiarex->m_WngAnalysis.m_bTiltedGeom) ar << 1;
		else									ar << 0;
		if (pMiarex->m_WngAnalysis.m_bWakeRollUp) ar << 1;
		else									ar << 0;

		ar << (int)m_oaWing.size() ;//number of wings
		// Store the wings
		for (i=0; i<m_oaWing.size();i++)
		{
			pWing = (CWing*)m_oaWing.at(i);
			pWing->SerializeWing(ar, true);
		}

		// now store all the WPolars
		ar << (int)m_oaWPolar.size();
		for (i=0; i<m_oaWPolar.size();i++)
		{
			pWPolar = (CWPolar*)m_oaWPolar.at(i);
			pWPolar->m_pParent = this;
			pWPolar->SerializeWPlr(ar, bIsStoring);
		}

		// next store all the WOpps
		if(m_bSaveWOpps)
		{
			ar << (int)m_oaWOpp.size();
			for (i=0; i<m_oaWOpp.size();i++)
			{
				pWOpp = (CWOpp*)m_oaWOpp.at(i);
				pWOpp->SerializeWOpp(ar, bIsStoring);
			}
		}
		else ar << 0;

		// then the foils,  polars and Opps
                WritePolars(ar);

		// next the bodies
		ar << (int)m_oaBody.size();
		for (i=0; i<m_oaBody.size();i++)
		{
			pBody = (CBody*)m_oaBody.at(i);
			pBody->SerializeBody(ar, bIsStoring);
		}

		// last write the planes...
		ar << (int)m_oaPlane.size();
		for (i=0; i<m_oaPlane.size();i++)
		{
			pPlane = (CPlane*)m_oaPlane.at(i);
			pPlane->SerializePlane(ar, bIsStoring);
		}

		if(m_bSaveWOpps)
		{
			// not forgetting their POpps
			ar << (int)m_oaPOpp.size();
			for (i=0; i<m_oaPOpp.size();i++)
			{
				pPOpp = (CPOpp*)m_oaPOpp.at(i);
				pPOpp->SerializePOpp(ar, bIsStoring);
			}
		}
		else ar << 0;

		pAFoil->m_pSF->Serialize(ar, bIsStoring);
		pAFoil->m_pPF->Serialize(ar, bIsStoring);

		return true;
	}
	else
	{
		// LOADING CODE

		int ArchiveFormat;
		ar >> n;

		if(n<100000)
		{
		// then n is the number of wings to load
		// up to v1.99beta15
			ArchiveFormat = 100000;
		}
		else
		{
		// then n is the ArchiveFormat number
			ArchiveFormat = n;
			ar >> m_LengthUnit;
			ar >> m_AreaUnit;
			ar >> m_WeightUnit;
			ar >> m_SpeedUnit;
			ar >> m_ForceUnit;
			if(ArchiveFormat>=100005)
			{
				ar >> m_MomentUnit;
			}

			SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
					 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

			if(ArchiveFormat>=100004)
			{
				ar >> pMiarex->m_WngAnalysis.m_Type;
				ar >> f; pMiarex->m_WngAnalysis.m_Weight=f;
				ar >> f; pMiarex->m_WngAnalysis.m_QInf=f;
				ar >> f; pMiarex->m_WngAnalysis.m_XCmRef=f;
				if(ArchiveFormat<100010) pMiarex->m_WngAnalysis.m_XCmRef=f/1000.0;
				ar >> f; pMiarex->m_WngAnalysis.m_Density=f;
				ar >> f; pMiarex->m_WngAnalysis.m_Viscosity=f;
				ar >> f; pMiarex->m_WngAnalysis.m_Alpha=f;
				if(ArchiveFormat>=100012)
				{
					ar >>f; pMiarex->m_WngAnalysis.m_Beta=f;
				}

				ar >> pMiarex->m_WngAnalysis.m_AnalysisType;
			}
			if(ArchiveFormat>=100006)
			{
				ar >> k;
				if (k) pMiarex->m_WngAnalysis.m_bVLM1 = true;
				else   pMiarex->m_WngAnalysis.m_bVLM1 = false;

				ar >> k;
//					if (k) pMiarex->m_WngAnalysis.m_bMiddle = true;
//					else   pMiarex->m_WngAnalysis.m_bMiddle = false;
			}

			if(ArchiveFormat>=100008)
			{
				ar >> k;
				if (k) pMiarex->m_WngAnalysis.m_bTiltedGeom = true;
				else   pMiarex->m_WngAnalysis.m_bTiltedGeom = false;

				ar >> k;
				if (k) pMiarex->m_WngAnalysis.m_bWakeRollUp = true;
				else   pMiarex->m_WngAnalysis.m_bWakeRollUp = false;
			}
			// and read n again
			ar >> n;
		}

		// WINGS FIRST
		for (i=0;i<n; i++)
		{
			pWing = new CWing;

			if (!pWing->SerializeWing(ar, bIsStoring))
			{
					if(pWing) delete pWing;
					return false;
			}
			if(pWing)
			{
				pWing = pMiarex->AddWing(pWing);
			}
			else
			{
				if(pWing) delete pWing;
			}
		}


		//THEN WPOLARS
		ar >> n;// number of WPolars to load
		bool bWPolarOK;
		for (i=0;i<n; i++)
		{
			pWPolar = new CWPolar;
			bWPolarOK = pWPolar->SerializeWPlr(ar, bIsStoring);

			if (!bWPolarOK)
			{
				if(pWPolar) delete pWPolar;
				return false;
			}
			if(!pWPolar->m_AnalysisType==1 && ArchiveFormat <100003)	pWPolar->ResetWPlr();//former VLM version was flawed

			pWPolar = pMiarex->AddWPolar(pWPolar);
		}

		//THEN WOPPS
		ar >> n;// number of WOpps to load
		bool bWOppOK;
		for (i=0;i<n; i++)
		{
			pWOpp = new CWOpp();
			if (ArchiveFormat<=100001)
			{
				return false;
			}
			else
			{
				bWOppOK = pWOpp->SerializeWOpp(ar, bIsStoring);
				if(pWOpp && bWOppOK)
				{
					pWing = pMiarex->GetWing(pWOpp->m_WingName);
					if(pWing) pWOpp->m_MAChord = pWing->m_MAChord;
					else      bWOppOK = false;
				}
			}
			if (!bWOppOK)
			{
				if(pWOpp) delete pWOpp;
				return false;
			}

			if(pWOpp->m_AnalysisType==1 || ArchiveFormat >=100006)//former VLM version was flawed
				pMiarex->InsertWOpp(pWOpp);

			else delete pWOpp;
		}

		ar >> n;
		//=100000 ... unused

		//THEN FOILS, POLARS and OPPS
		if(ArchiveFormat>=100009)
		{
			if(!LoadPolarFileV3(ar, bIsStoring,100002))
			{
				return false;
			}
		}
		else
		{
			if(ArchiveFormat>=100006)
			{
				if(!LoadPolarFileV3(ar, bIsStoring))
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if(n==100000)
		{
			for (j=0; j<m_oaPolar.size(); j++)
			{
				pPolar = (CPolar*)m_oaPolar.at(j);
				for (k=0; k<m_oaFoil.size(); k++)
				{
					pFoil = (CFoil*)m_oaFoil.at(k);
					if(pFoil->m_FoilName==pPolar->m_FoilName)
					{
						pPolar->m_ACrit    = 9.0;
						pPolar->m_XTop     = 1.0;
						pPolar->m_XBot     = 1.0;
						str = QString("_N%1").arg(9.0,4,'f',1);
						pPolar->m_PlrName += str;
						break;
					}
				}
			}
		}

		if(ArchiveFormat>=100011)
		{
			ar >> n;// number of Bodies to load
			for (i=0;i<n; i++)
			{
				pBody = new CBody();

				if (!pBody->SerializeBody(ar, bIsStoring))
				{
					if(pPOpp) delete pPOpp;
				}
				pMiarex->AddBody(pBody);
			}
		}

		if(ArchiveFormat>=100006)
		{ //read the planes
			ar >> n;
			// last read the planes
			for (i=0; i<n;i++)
			{
				pPlane = new CPlane();
				if(pPlane)
				{
					if(pPlane->SerializePlane(ar, bIsStoring))
					{
						pMiarex->AddPlane(pPlane);
					}
					else
					{
						if(pPlane) delete pPlane;
						return false;
					}
				}
			}
			//and their pPolars

			if(ArchiveFormat <100007)
			{
				ar >> n;// number of PPolars to load
				for (i=0;i<n; i++)
				{
					pWPolar = new CWPolar();

					if (!pWPolar->SerializeWPlr(ar, bIsStoring))
					{

						if(pWPolar) delete pWPolar;
						return false;
					}
					if(!pWPolar->m_AnalysisType==1 && ArchiveFormat <100003)
						pWPolar->ResetWPlr();
					pMiarex->AddWPolar(pWPolar);
				}
			}

			ar >> n;// number of POpps to load
			for (i=0;i<n; i++)
			{
				pPOpp = new CPOpp();

				if (!pPOpp->SerializePOpp(ar, bIsStoring))
				{
					if(pPOpp) delete pPOpp;
					return false;
				}
				pMiarex->AddPOpp(false, NULL, NULL, NULL, pPOpp);

			}
		}

		pMiarex->m_pCurPOpp = NULL;

		pAFoil->m_pSF->Serialize(ar, bIsStoring);
		pAFoil->m_pPF->Serialize(ar, bIsStoring);

		for (i=0; i<m_oaWing.size();i++)
		{
			pWing = (CWing*)m_oaWing[i];
			pWing->ComputeGeometry();
		}

		if(m_iApp==MIAREX) pMiarex->SetUFO();

		return true;
	}
}


void MainFrame::SetCurrentFoil(CFoil* pFoil)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil   *pAFoil = (QAFoil*)m_pAFoil;
	pXDirect->SetFoil(pFoil);
	pAFoil->SetFoil(pFoil);
	m_pCurFoil = pFoil;
}



void MainFrame::SetMenus()
{
	if(m_iApp==0)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==XFOILANALYSIS)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(XDirectViewMenu);
		menuBar()->addMenu(FoilMenu);
		menuBar()->addMenu(DesignMenu);
		menuBar()->addMenu(PolarMenu);
		menuBar()->addMenu(OpPointMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(XInverseViewMenu);
		menuBar()->addMenu(InverseFoilMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(AFoilViewMenu);
		menuBar()->addMenu(AFoilDesignMenu);
		menuBar()->addMenu(AFoilSplineMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp== MIAREX)
	{

		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(MiarexViewMenu);
		menuBar()->addMenu(UFOMenu);
		menuBar()->addMenu(MiarexBodyMenu);
		menuBar()->addMenu(MiarexWPlrMenu);
		menuBar()->addMenu(MiarexWOppMenu);
		menuBar()->addMenu(helpMenu);
	}
}



CFoil* MainFrame::SetModFoil(CFoil* pNewFoil, bool bKeepExistingFoil)
{
	// Adds the buffer foil to the ObArray,
	// gives it a proper name, FoilName or another,
	// selects it ,
	// and initializes XFoil, comboboxes and everything.

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int j,k,l;
	bool bExists = false;
	bool bNotFound = true;
	pNewFoil->m_bSaved = false;
	CFoil*pOldFoil;

	if(!pNewFoil->m_FoilName.length())
	{
		bExists = true;
	}
	else
	{
		for (j=0; j<m_oaFoil.size(); j++)
		{
			pOldFoil = (CFoil*)m_oaFoil.at(j);
			if (pOldFoil->m_FoilName == pNewFoil->m_FoilName)
			{
				bExists = true;
				if(bKeepExistingFoil)
				{
						delete pNewFoil;
						pNewFoil = NULL;
						m_pCurFoil = NULL;
						return pOldFoil;
				}
				break;
			}
		}
	}
	if(bExists)
	{
		while(bNotFound)
		{
			CFoil *pFoil;
			QStringList NameList;
			for(k=0; k<m_oaFoil.size(); k++)
			{
				pFoil = (CFoil*)m_oaFoil.at(k);
				NameList.append(pFoil->m_FoilName);
			}
			RenameDlg dlg(this);
			dlg.m_pstrArray = & NameList;
			dlg.m_strQuestion = "A foil of that name already exists\r\nPlease enter a new name";
			dlg.m_strName = pNewFoil->m_FoilName;
			dlg.InitDialog();

			bool exists = false;
			QString strong;
			int resp = dlg.exec();
			strong = dlg.m_strName;

			if(QDialog::Accepted == resp)
			{
				strong = dlg.m_strName;
				for (l=0; l<m_oaFoil.size(); l++)
				{
					pOldFoil = (CFoil*)m_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong) exists = true;
				}
				if (!exists)
				{
					bNotFound = false;// at last (users !...)
					// so create and add foil to array
					pNewFoil->m_FoilName = strong;

					AddFoil(pNewFoil);
					SetSaveState(false);
				}
			}
			else if(resp==10)
			{
				// the user wants to overwrite existing airfoil
				// So delete any foil with that name
				for (l=m_oaFoil.size()-1;l>=0; l--)
				{
					pOldFoil = (CFoil*)m_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong)
					{
						pNewFoil->m_FoilColor  = pOldFoil->m_FoilColor;
						pNewFoil->m_nFoilStyle = pOldFoil->m_nFoilStyle;
						pNewFoil->m_nFoilWidth = pOldFoil->m_nFoilWidth;
						pNewFoil->m_bPoints    = pOldFoil->m_bPoints;
						m_oaFoil.removeAt(l);
						delete pOldFoil;
						if(m_pCurFoil == pOldFoil)           m_pCurFoil = NULL;
						if(pXDirect->m_pCurFoil == pOldFoil) pXDirect->m_pCurFoil = NULL;
					}
				}
				// delete all associated OpPoints
				OpPoint * pOpPoint;
				for (l=m_oaOpp.size()-1;l>=0;l--)
				{
					pOpPoint = (OpPoint*)m_oaOpp[l];
					if (pOpPoint->m_strFoilName == strong)
					{
						m_oaOpp.removeAt(l);
						if(pOpPoint == pXDirect->m_pCurOpp) pXDirect->m_pCurOpp = NULL;
						delete pOpPoint;
					}
				}
				// delete all Polar results for that airfoil, but keep polar for analysis
				CPolar * pPolar;
				for (l=0; l <m_oaPolar.size();l++)
				{
					pPolar = (CPolar*) m_oaPolar.at(l);
					if (pPolar->m_FoilName == strong)
					{
						pPolar->ResetPolar();
					}
				}
				// finally add to array
				pNewFoil->m_FoilName = strong;
				AddFoil(pNewFoil);
				SetSaveState(false);
				bNotFound = false;//exit loop
			}
			else
			{
				// Cancel so exit
				delete pNewFoil;
				pNewFoil = NULL;
				m_pCurFoil = NULL;
				return NULL;// foil not added
			}
		}
	}
	else
	{
		AddFoil(pNewFoil);
		SetSaveState(false);
	}
	if(m_iApp == XFOILANALYSIS) pXDirect->SetFoil(pNewFoil);
	m_pCurFoil = pNewFoil;
	return pNewFoil;// foil added
}


void MainFrame::SetProjectName(QString PathName)
{
	m_FileName = PathName;
	int pos = PathName.lastIndexOf("/");
	if (pos>0) m_ProjectName = PathName.right(PathName.length()-pos-1);
	else m_ProjectName = PathName;
	if(m_ProjectName.length()>4)
	{
		m_ProjectName = m_ProjectName.left(m_ProjectName.length()-4);
		m_pctrlProjectName->setText(m_ProjectName);
	}
}


void MainFrame::SetSaveState(bool bSave)
{
	m_bSaved = bSave;

	int len = m_ProjectName.length();
	if(m_ProjectName.right(1)=="*") m_ProjectName = m_ProjectName.left(len-1);
	if (!bSave)
	{
		m_ProjectName += "*";
	}
	m_pctrlProjectName->setText(m_ProjectName);
}






void MainFrame::SetGraphSettings(Graph *pGraph)
{
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex     = (QMiarex*)m_pMiarex;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;


	pXDirect->m_pPolarGraph->CopySettings(pGraph, false);
	pXDirect->m_pCpGraph->CopySettings(pGraph, false);
	pXDirect->m_pCmGraph->CopySettings(pGraph, false);
	pXDirect->m_pCzGraph->CopySettings(pGraph, false);
	pXDirect->m_pTrGraph->CopySettings(pGraph, false);
	pXDirect->m_pUserGraph->CopySettings(pGraph, false);

	pXInverse->m_QGraph.CopySettings(pGraph, false);

	pMiarex->m_WingGraph1.CopySettings(pGraph, false);
	pMiarex->m_WingGraph2.CopySettings(pGraph, false);
	pMiarex->m_WingGraph3.CopySettings(pGraph, false);
	pMiarex->m_WingGraph4.CopySettings(pGraph, false);
	pMiarex->m_WPlrGraph1.CopySettings(pGraph, false);
	pMiarex->m_WPlrGraph2.CopySettings(pGraph, false);
	pMiarex->m_WPlrGraph3.CopySettings(pGraph, false);
	pMiarex->m_WPlrGraph4.CopySettings(pGraph, false);
	pMiarex->m_CpGraph.CopySettings(pGraph, false);
}



QString MainFrame::ShortenFileName(QString &PathName)
{
	QString strong, strange;
	if(PathName.length()>60)
	{
		int pos = PathName.lastIndexOf('/');
		if (pos>0)
		{
			strong = '/'+PathName.right(PathName.length()-pos-1);
			strange = PathName.left(60-strong.length()-6);
			pos = strange.lastIndexOf('/');
			if(pos>0) strange = strange.left(pos)+"/...  ";
			strong = strange+strong;
		}
		else
		{
			strong = PathName.left(40);
		}
	}
	else strong = PathName;
	return strong;
}


void MainFrame::UpdateUFOs()
{
	// fills combobox with wing and plane names 
	// then selects the current wing or plane, if any
	// else selects the first, if any
	// else disables the combobox
	int i;
	QString strong;
	m_pctrlUFO->clear();
	QMiarex *pMiarex= (QMiarex*)m_pMiarex;
	CWing *pWing, *pCurWing;
	CPlane *pPlane, *pCurPlane;
	pCurWing  = pMiarex->m_pCurWing;
	pCurPlane = pMiarex->m_pCurPlane;


	QStringList UFONames;
	for (i=0; i<m_oaWing.size(); i++)
	{
		pWing = (CWing*)m_oaWing[i];
		UFONames.append(pWing->m_WingName);
	}
	for (i=0; i<m_oaPlane.size(); i++)
	{
		pPlane = (CPlane*)m_oaPlane[i];
		UFONames.append(pPlane->m_PlaneName);
	}
	UFONames.sort();
	m_pctrlUFO->addItems(UFONames);

	if (m_pctrlUFO->count())
	{
 		m_pctrlUFO->setEnabled(true);
		//select the current Wing, if any...
		if(pCurPlane)
		{
			int pos = m_pctrlUFO->findText(pCurPlane->m_PlaneName);
			if(pos>=0)
			{
				m_pctrlUFO->setCurrentIndex(pos);
			}
			else 
			{
				// if error, select the first
				m_pctrlUFO->setCurrentIndex(0);
				strong = m_pctrlUFO->itemText(0);
			}		
		}
		else if(pCurWing)
		{
			int pos = m_pctrlUFO->findText(pCurWing->m_WingName);
			if(pos>=0) 
			{
				m_pctrlUFO->setCurrentIndex(pos);
			}
			else
			{
				// if error, select the first
				m_pctrlUFO->setCurrentIndex(0);
				strong = m_pctrlUFO->itemText(0);
			}
		}
		//... else select the first
		else 
		{
			m_pctrlUFO->setCurrentIndex(0);
			strong = m_pctrlUFO->itemText(0);
		}
	}
 	else
	{
		m_pctrlUFO->clear();
 		m_pctrlUFO->setEnabled(false);

		pMiarex->m_pCurPlane  = NULL;
		pMiarex->m_pCurWing   = NULL;
		pMiarex->m_pCurWPolar = NULL;
		pMiarex->m_pCurWOpp   = NULL;
		pMiarex->m_pCurPOpp   = NULL;
	}

	UpdateWPolars();
}

void MainFrame::UpdateWPolars()
{
//	fills the combobox with WPolar names associated to Miarex's current wing
//	then selects Miarex current WPolar if any, else selects the first, if any
//	else disables the combobox
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	CWPolar *pWPolar;
	QString strong, UFOName;
	int i;

	m_pctrlWPolar->clear();

	CPlane  *pCurPlane = pMiarex->m_pCurPlane;
	CWing   *pCurWing  = pMiarex->m_pCurWing;
	CWPolar *pCurWPlr  = pMiarex->m_pCurWPolar;

	if(pCurPlane)      UFOName = pCurPlane->m_PlaneName;
	else if(pCurWing)  UFOName = pCurWing->m_WingName;
	else               UFOName = "";

	if(!UFOName.length())
	{
		pMiarex->m_pCurWPolar = NULL;
//		pMiarex->SetWPlr();
		m_pctrlWPolar->setEnabled(false);
		UpdateWOpps();
		return;
	}

	int size = 0;
	//count the number of WPolars associated to the current Wing
	for (i=0; i<m_oaWPolar.size(); i++)
	{
		pWPolar = (CWPolar*)m_oaWPolar[i];
		if(pWPolar->m_UFOName == UFOName)
		{
			size++;
		}
	}
	if (size)
	{
		// if any
		m_pctrlWPolar->setEnabled(true);
		for (i=0; i<m_oaWPolar.size(); i++)
		{
			pWPolar = (CWPolar*)m_oaWPolar[i];
			if(pWPolar->m_UFOName == UFOName)
			{
				m_pctrlWPolar->addItem(pWPolar->m_PlrName);
			}
		}
		//select the current Wing Polar, if any...
		if(pCurWPlr)
		{
			int pos = m_pctrlWPolar->findText(pCurWPlr->m_PlrName);
			if(pos>=0) m_pctrlWPolar->setCurrentIndex(pos);
			else
			{
				// if error, select the first
				m_pctrlWPolar->setCurrentIndex(0);
//				m_pctrlWPolar->GetLBText(0, strong);
//				m_pMiarex->SetWPlr(false, strong);
			}
		}
		//... else select the first
		else
		{
			m_pctrlWPolar->setCurrentIndex(0);
//			m_pctrlWPolar->GetLBText(0, strong);
//			m_pMiarex->SetWPlr(false, strong);
		}

	}
	else
	{
		// otherwise disable control
		m_pctrlWPolar->setEnabled(false);
		pMiarex->m_pCurWPolar = NULL;
		pMiarex->m_pCurWOpp = NULL;
//		m_pMiarex->SetWPlr();
	}
}

void MainFrame::UpdateWOpps()
{
	// fills combobox with WOpp names associated to Miarex' current WPLr
	// then selects Miarex current WOpp if any, else selects the first, if any
	// else disables the combobox
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	CWOpp *pWOpp;
	CPOpp *pPOpp;
	int i;
	QString strong, str;
	m_pctrlWOpp->clear();

	CPlane  *pCurPlane   = pMiarex->m_pCurPlane;
	CWing   *pCurWing    = pMiarex->m_pCurWing;
	CWPolar *pCurWPlr    = pMiarex->m_pCurWPolar;

	if (!pCurWing || !pCurWing->m_WingName.length() ||	!pCurWPlr || !pCurWPlr->m_PlrName.length())
	{
		m_pctrlWOpp->setEnabled(false);
		pMiarex->m_pCurWOpp = NULL;
//		pMiarex->SetWOpp(true);
		return;
	}

	if(pCurPlane)
	{
		int size = 0;
		//count the number of POpps associated to the current Wing & WPolar
		for (i=0; i<m_oaPOpp.size(); i++)
		{
			pPOpp = (CPOpp*)m_oaPOpp[i];
			if (pPOpp->m_PlaneName == pCurPlane->m_PlaneName &&	pPOpp->m_PlrName   == pCurWPlr->m_PlrName)
			{
				size++;
			}
		}
		if (size){// if any
			m_pctrlWOpp->setEnabled(true);
			for (int i=0; i<m_oaPOpp.size(); i++)
			{
				pPOpp = (CPOpp*)m_oaPOpp[i];
				if (pPOpp->m_PlaneName == pCurPlane->m_PlaneName &&
					pPOpp->m_PlrName == pCurWPlr->m_PlrName)
				{
					if(pCurWPlr->m_Type != 4) str = QString("%1").arg(pPOpp->m_Alpha,8,'f',2);
					else                      str = QString("%1").arg(pPOpp->m_QInf,8,'f',2);

					m_pctrlWOpp->addItem(str);
				}
			}
//			Miarex.SetPOpp(true);
			if(pMiarex->m_pCurPOpp)
			{
					if(pCurWPlr->m_Type != 4) str = QString("%1").arg(pPOpp->m_Alpha,8,'f',2);
					else                      str = QString("%1").arg(pPOpp->m_QInf,8,'f',2);

				int pos = m_pctrlWOpp->findText(str);
				if(pos >=0)
				{
					m_pctrlWOpp->setCurrentIndex(pos);
				}
				else {
					m_pctrlWOpp->setCurrentIndex(0);
				}
			}
			else{
				m_pctrlWOpp->setCurrentIndex(0);
			}

		}
		else
		{
			// otherwise disable control
			m_pctrlWOpp->setEnabled(false);
			pMiarex->m_pCurPOpp = NULL;
//			Miarex.SetPOpp(true);
		}
	}
	else
	{
		int size = 0;
		//count the number of WOpps associated to the current Wing & WPolar
		for (i=0; i<m_oaWOpp.size(); i++)
		{
			pWOpp = (CWOpp*)m_oaWOpp[i];
			if (pWOpp->m_WingName == pCurWing->m_WingName &&
				pWOpp->m_PlrName  == pCurWPlr->m_PlrName)
			{
				size++;
			}
		}
		if (size)
		{
			// if any
			m_pctrlWOpp->setEnabled(true);
			for (int i=0; i<m_oaWOpp.size(); i++)
			{
				pWOpp = (CWOpp*)m_oaWOpp[i];
				if (pWOpp->m_WingName == pCurWing->m_WingName &&
					pWOpp->m_PlrName == pCurWPlr->m_PlrName)
				{

					if(pCurWPlr->m_Type != 4) str = QString("%1").arg(pWOpp->m_Alpha,8,'f',2);
					else                      str = QString("%1").arg(pWOpp->m_QInf,8,'f',2);

					m_pctrlWOpp->addItem(str);
				}
			}
//			Miarex.SetWOpp(true);
			if(pMiarex->m_pCurWOpp)
			{
				if(pCurWPlr->m_Type != 4) str = QString("%1").arg(pWOpp->m_Alpha,8,'f',2);
				else                      str = QString("%1").arg(pWOpp->m_QInf,8,'f',2);

				int pos = m_pctrlWOpp->findText(str);
				if(pos >=0)
				{
					m_pctrlWOpp->setCurrentIndex(pos);
				}
				else
				{
					m_pctrlWOpp->setCurrentIndex(0);
				}
			}
			else
			{
				m_pctrlWOpp->setCurrentIndex(0);
			}

		}
		else
		{
			// otherwise disable control
			m_pctrlWOpp->setEnabled(false);
			pMiarex->m_pCurWOpp = NULL;
//			Miarex.SetWOpp(true);
		}
	}
}



void MainFrame::UpdateFoils()
{
	// fills combobox with foil names
	// then selects XDirect current foil if any, else selects the first, if any
	// else disables the combobox
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, pos;
	QString strong;
	m_pctrlFoil->clear();

	CFoil *pFoil;
	m_pCurFoil = pXDirect->m_pCurFoil;

	for (i=0; i<m_oaFoil.size(); i++)
	{
		pFoil = (CFoil*)m_oaFoil[i];
		m_pctrlFoil->addItem(pFoil->m_FoilName);
	}

	if (m_pctrlFoil->count())
	{
		m_pctrlFoil->setEnabled(true);
		//select the current foil, if any...
		if (m_pCurFoil)
		{
			pos = m_pctrlFoil->findText(m_pCurFoil->m_FoilName);
			if (pos>=0) m_pctrlFoil->setCurrentIndex(pos);
			else
			{
				// if error, select the first...
				m_pctrlFoil->setCurrentIndex(0);
				strong = m_pctrlFoil->itemText(0);
				//...and set it
//				pXDirect->m_pCurFoil = GetFoil(strong);
//				pXDirect->SetFoil(strong);
			}
		}
		//... else select the first
		else
		{
			// if error, select the first...
			m_pctrlFoil->setCurrentIndex(0);
			strong = m_pctrlFoil->itemText(0);
			//...and set it

			pXDirect->SetFoil(GetFoil(strong));
		}
	}
	else
	{
		m_pctrlFoil->clear();
		m_pctrlFoil->setEnabled(false);
		pXDirect->SetFoil();
	}
	UpdatePolars();
}

void MainFrame::UpdatePolars()
{
    // fills combobox with polar names associated to XDirect' current foil
    // then selects XDirect current polar if any, else selects the first, if any
    // else disables the combobox
    // sets the polar in XDirect in all cases
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, size, pos;
    CPolar *pPolar;
    QString strong;
    m_pctrlPolar->clear();

	m_pCurFoil = pXDirect->m_pCurFoil;

    if(!m_pCurFoil || !m_pCurFoil->m_FoilName.length())
    {
		m_pctrlPolar->setEnabled(false);
		m_pctrlOpPoint->clear();
		m_pctrlOpPoint->setEnabled(false);
		return;
	}

    size = 0;
    //count the number of polars associated to the current foil
    for (i=0; i<m_oaPolar.size(); i++)
    {
	pPolar = (CPolar*)m_oaPolar[i];
	if(pPolar->m_FoilName == m_pCurFoil->m_FoilName)
	{
	    size++;
	}
    }
    if (size)
    {

		// if any
		m_pctrlPolar->setEnabled(true);

		for (i=0; i<m_oaPolar.size(); i++)
		{
			pPolar = (CPolar*)m_oaPolar[i];
			if(pPolar->m_FoilName == m_pCurFoil->m_FoilName)
			{
				m_pctrlPolar->addItem(pPolar->m_PlrName);
			}
		}
		if(pXDirect->m_pCurPolar)
		{
			pos = m_pctrlPolar->findText(pXDirect->m_pCurPolar->m_PlrName);
			if(pos>=0) m_pctrlPolar->setCurrentIndex(pos);
			else
			{
				m_pctrlPolar->setCurrentIndex(0);
				strong = m_pctrlPolar->itemText(0);
		//		pXDirect->SetPolar(strong);
			}
		}
		else
		{
			m_pctrlPolar->setCurrentIndex(0);
			strong = m_pctrlPolar->itemText(0);
	//			pXDirect->SetPolar(strong);
		}
    }
    else
    {
		// otherwise disable control
		m_pctrlPolar->setEnabled(false);
	//		pXDirect->SetPolar();
    }

    UpdateOpps();
}

void MainFrame::UpdateOpps()
{
    // fills the combobox with the Opp names associated to XDirect's current foil
    // then selects XDirect current opp if any, else selects the first, if any
    // else disables the combobox

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, pos;
    OpPoint *pOpp;
    QString strong, str;
    m_pctrlOpPoint->clear();

//	m_pCurFoil = pXDirect->m_pCurFoil;
	CPolar *pCurPlr    = pXDirect->m_pCurPolar;

    if (!m_pCurFoil || !m_pCurFoil->m_FoilName.length() || !pCurPlr  || !pCurPlr->m_PlrName.length())
    {
		m_pctrlOpPoint->clear();
		m_pctrlOpPoint->setEnabled(false);
		return;
    }

    int size = 0;
    //count the number of Opps associated to the current foil & polar
    for (i=0; i<m_oaOpp.size(); i++)
    {
		pOpp = (OpPoint*)m_oaOpp[i];
		if (pOpp->m_strFoilName == m_pCurFoil->m_FoilName && pOpp->m_strPlrName  == pCurPlr->m_PlrName)
		{
			size++;
		}
    }

    if (size)
    {
		// if any
		m_pctrlOpPoint->setEnabled(true);
		for (i=0; i<m_oaOpp.size(); i++)
		{
			pOpp = (OpPoint*)m_oaOpp[i];
			if (pOpp->m_strFoilName == m_pCurFoil->m_FoilName && pOpp->m_strPlrName  == pCurPlr->m_PlrName)
			{
				if (pCurPlr->m_Type !=4)
				{
					if(fabs(pOpp->Alpha)<0.0001) pOpp->Alpha = 0.0001;
					str = QString("%1").arg(pOpp->Alpha,8,'f',2);
					m_pctrlOpPoint->addItem(str);
				}
				else
				{
					str = QString("%1").arg(pOpp->Reynolds,8,'f',0);
					m_pctrlOpPoint->addItem(str);
				}
			}
		}
		if (pXDirect->m_pCurOpp && pXDirect->m_pCurOpp->m_strFoilName==pXDirect->m_pCurFoil->m_FoilName)
		{
			//select it
			if (pCurPlr->m_Type !=4)
			{
				str = QString("%8.2f").arg(pXDirect->m_pCurOpp->Alpha);
			}
			else
			{
				str = QString("%8.0f").arg(pXDirect->m_pCurOpp->Reynolds);
			}
			pos = m_pctrlOpPoint->findText(str);

			if(pos>=0) m_pctrlOpPoint->setCurrentIndex(pos);
		}
		else
		{
			//select the first
			m_pctrlOpPoint->setCurrentIndex(0);
			str = m_pctrlOpPoint->itemText(0);
			QByteArray textline;
			const char *text;
			double x;
			textline = str.toAscii();
			text = textline.constData();
			int res = sscanf(text, "%lf", &x);

			if(res==1) pXDirect->SetOpp(x);
			else       pXDirect->SetOpp();

			if(!pXDirect->m_pCurOpp)
			{
				m_pctrlOpPoint->setEnabled(false);
				pXDirect->m_pCurOpp = NULL;
			}
		}
		if (m_pctrlOpPoint->count()) m_pctrlOpPoint->setEnabled(true);
		else                         m_pctrlOpPoint->setEnabled(false);
    }
    else
    {
		// otherwise disable control
		m_pctrlOpPoint->setEnabled(false);
		pXDirect->m_pCurOpp = NULL;
		m_pctrlOpPoint->clear();
    }
}



void MainFrame::updateRecentFileActions()
{
	int numRecentFiles = qMin(m_RecentFiles.size(), MAXRECENTFILES);

	QString text;
	for (int i = 0; i < numRecentFiles; ++i)
	{
		text = tr("&%1 %2").arg(i + 1).arg(ShortenFileName(m_RecentFiles[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(m_RecentFiles[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MAXRECENTFILES; ++j)
		recentFileActs[j]->setVisible(false);

	separatorAct->setVisible(numRecentFiles > 0);
}



void MainFrame::UpdateView()
{
	switch(m_iApp)
	{
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->UpdateView();
			break;
		}
		case DIRECTDESIGN:
		{
			QAFoil *pAFoil= (QAFoil*)m_pAFoil;
			pAFoil->UpdateView();
			break;
		}
		case MIAREX:
		{
			QMiarex *pMiarex= (QMiarex*)m_pMiarex;
			pMiarex->UpdateView();
			break;
		}
		case INVERSEDESIGN:
		{
			QXInverse *pXInverse = (QXInverse*)m_pXInverse;
			pXInverse->UpdateView();
			break;
		}
	}
}


void MainFrame::WritePolars(QDataStream &ar, CFoil *pFoil)
{
	int i;
	if(!pFoil)
	{
		ar << 100002;
		//100001 : transferred NCrit, XTopTr, XBotTr to polar file
		//100002 : means we are serializings opps in the new numbered format
		//first write foils
		ar << (int)m_oaFoil.size();

		for (i=0; i<m_oaFoil.size(); i++)
		{
			pFoil = (CFoil*)m_oaFoil.at(i);
			pFoil->Serialize(ar, true);
		}

		//then write polars
		ar << (int)m_oaPolar.size();
		CPolar * pPolar ;
		for (i=0; i<m_oaPolar.size();i++)
		{
			pPolar = (CPolar*)m_oaPolar.at(i);
			pPolar->Serialize(ar, true);
		}
		if(m_bSaveOpps)
		{
			ar << (int)m_oaOpp.size();
			OpPoint * pOpp ;
			for (i=0; i<m_oaOpp.size();i++)
			{
				pOpp = (OpPoint*)m_oaOpp.at(i);
				pOpp->Serialize(ar,true,100002);
			}
		}
		else ar << 0;
	}
	else
	{
		ar << 100002;
		//100001 : transferred NCrit, XTopTr, XBotTr to polar file
		//100002 : means we are serializings opps in the new numbered format
		//first write foil
		ar << 1;//only one this time
		pFoil->Serialize(ar,true);
		//count polars associated to the foil
		CPolar * pPolar ;
		int n=0;
		for (i=0; i<m_oaPolar.size();i++)
		{
			pPolar = (CPolar*)m_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName) n++;
		}
		//then write polars
		ar << n;
		for (i=0; i<m_oaPolar.size();i++)
		{
			pPolar = (CPolar*)m_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName) pPolar->Serialize(ar, true);
		}
	}
}
