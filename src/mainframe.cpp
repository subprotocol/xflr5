/****************************************************************************

	MainFrame  Class
    Copyright (C) 2008-2013 Andre Deperrois adeperrois@xflr5.com

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

#include "mainframe.h"
#include "globals.h"
#include "design/AFoil.h"
#include "miarex/Miarex.h"
#include "miarex/InertiaDlg.h"
#include "miarex/BodyTransDlg.h"
#include "miarex/GL3dWingDlg.h"
#include "miarex/GL3dBodyDlg.h"
#include "miarex/GL3DScales.h"
#include "miarex/StabViewDlg.h"
#include "miarex/WPolarDlg.h"
#include "miarex/StabPolarDlg.h"
#include "miarex/PlaneDlg.h"
#include "miarex/StabViewDlg.h"
#include "miarex/ManageUFOsDlg.h"
#include "miarex/UFOTableDelegate.h"
#include "miarex/NURBSDomDoc.h"
#include "misc/AboutQ5.h"
#include "misc/ObjectPropsDlg.h"
#include "misc/LinePickerDlg.h"
#include "misc/DisplaySettingsDlg.h"
#include "misc/SaveOptionsDlg.h"
#include "misc/TranslatorDlg.h"
#include "misc/RenameDlg.h"
#include "misc/UnitsDlg.h"
#include "misc/W3dPrefsDlg.h"
#include "graph/GraphDlg.h"
#include "xdirect/XDirect.h"
#include "xdirect/BatchDlg.h"
#include "xdirect/BatchThreadDlg.h"
#include "xdirect/NacaFoilDlg.h"
#include "xdirect/InterpolateFoilsDlg.h"
#include "xdirect/CAddDlg.h"
#include "xdirect/TwoDPanelDlg.h"
#include "xdirect/FoilCoordDlg.h"
#include "xdirect/FoilGeomDlg.h"
#include "xdirect/TEGapDlg.h"
#include "xdirect/LEDlg.h"
#include "xdirect/XFoilAnalysisDlg.h"
#include "xdirect/FoilPolarDlg.h"
#include "xinverse/XInverse.h"

#include <QMessageBox>
#include <QtCore>
#include <QToolBar>
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>

#ifdef Q_WS_MAC
	#include <CoreFoundation/CoreFoundation.h>
#endif


QPointer<MainFrame> MainFrame::_self = 0L;

QString MainFrame::s_VersionName = QString::fromLatin1("XFLR5 v6.09.06");
QString MainFrame::s_ProjectName = "";
QString MainFrame::s_LastDirName = "";
QString MainFrame::s_LanguageFilePath = "";
QDir MainFrame::s_StylesheetDir;
QDir MainFrame::s_TranslationDir;


Foil * MainFrame::s_pCurFoil=NULL;
QFont MainFrame::s_TextFont;
QColor MainFrame::s_TextColor       = QColor(220,220,220);
QColor MainFrame::s_BackgroundColor = QColor(0, 5, 10);
bool MainFrame::s_bAlphaChannel = true;
bool MainFrame::s_bReverseZoom = false;
bool MainFrame::s_bTrace = false;
bool MainFrame::s_bSaved = true;
QFile *MainFrame::s_pTraceFile = NULL;
enumTextFileType MainFrame::s_ExportFileType;  /**< Defines if the list separator for the output text files should be a space or a comma. */

double MainFrame::s_mtoUnit  = 1.0;
double MainFrame::s_mstoUnit = 1.0;
double MainFrame::s_m2toUnit = 1.0;
double MainFrame::s_kgtoUnit = 1.0;
double MainFrame::s_NtoUnit  = 1.0;
double MainFrame::s_NmtoUnit = 1.0;
int MainFrame::s_LengthUnit = 0;
int MainFrame::s_AreaUnit   = 1;
int MainFrame::s_WeightUnit = 1;
int MainFrame::s_SpeedUnit  = 0;
int MainFrame::s_ForceUnit  = 0;
int MainFrame::s_MomentUnit = 0;

QList <void *> MainFrame::s_oaFoil;
QList <void *> MainFrame::s_oaPolar;
QList <void *> MainFrame::s_oaPlane;
QList <void *> MainFrame::s_oaWing;
QList <void *> MainFrame::s_oaBody;
QLabel *MainFrame::m_pctrlProjectName = NULL;



MainFrame::MainFrame(QWidget * parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	if(s_bTrace)
	{
        QString FileName = QDir::homePath() + "/Trace.log";
		s_pTraceFile = new QFile(FileName);
		if (!s_pTraceFile->open(QIODevice::ReadWrite | QIODevice::Text)) s_bTrace = false;
	}
	QString jpegPluginPath;

	//Jpeg format requires a specific plugin to be loaded dynmically at run time
#ifdef Q_WS_MAC
        // F.Meschia 20101101 -- this is unnecessary, in fact, so I commented it out

        QDir dir(qApp->applicationDirPath());
        dir.cdUp();
        jpegPluginPath = dir.canonicalPath() + "/PlugIns/imageformats/libqjpeg.dylib";

#endif
#ifdef Q_OS_WIN
	QDir dir(qApp->applicationDirPath());
	jpegPluginPath = dir.canonicalPath() + "/imageformats/qjpeg4.dll";
#endif
#ifdef Q_OS_LINUX
	QDir dir(qApp->applicationDirPath());
	jpegPluginPath = dir.canonicalPath() + "/imageformats/libqjpeg.so";
#endif

	if (dir.exists(jpegPluginPath))
	{
/*		QPluginLoader jpegPluginLoad(jpegPluginPath);
		jpegPluginLoad.load();
		if (!jpegPluginLoad.isLoaded())
		{
			QString errorMessage = jpegPluginLoad.errorString() + " : " +jpegPluginPath;
			QMessageBox::information(window(), tr("Info"), errorMessage);
        }*/
	}

	setWindowTitle(s_VersionName);
	setWindowIcon(QIcon(":/images/xflr5_64.png"));


	if(!QGLFormat::hasOpenGL())
	{
		QMessageBox::warning(this, tr("Warning"), tr("Your system does not provide support for OpenGL.\nXFLR5 will not operate correctly."));
	}
	
	UndoAFoilAct = RedoAFoilAct = NULL;

	CreateDockWindows();

	s_TextFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);
	s_TextFont.setFamily(s_TextFont.defaultFamily());
	s_TextFont.setPointSize(10);

	m_RefGraph.SetGraphName("Reference Graph");

	m_ImageFormat = PNG;
	s_ExportFileType = TXT;

	m_bSaveOpps     = false;
	m_bSaveWOpps    = true;
	m_bSaveSettings = true;

	m_GraphExportFilter = "Comma Separated Values (*.csv)";

	s_LastDirName = QDir::homePath();

	QAFoil *pAFoil       = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	QMiarex *pMiarex     = (QMiarex*)m_pMiarex;

#ifdef Q_WS_MAC
		QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"sourceforge.net","xflr5");
#else
		QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif
	QString str;
	int kx=117, ky=57;
	bool bOK;
	settings.beginGroup("MainFrame");
	{
		str = settings.value("LanguageFilePath").toString();
		if(str.length()) s_LanguageFilePath = str;
		kx = settings.value("FrameGeometryx", 117).toInt(&bOK);
		ky = settings.value("FrameGeometryy",  57).toInt(&bOK);
	}
	settings.endGroup();

	XFoilAnalysisDlg::s_Position = QPoint(kx+31, ky+31);
	BatchThreadDlg::s_Position   = QPoint(kx+31, ky+31);
	BatchDlg::s_Position         = QPoint(kx+31, ky+31);
	LLTAnalysisDlg::s_Position   = QPoint(kx+31, ky+31);
	PanelAnalysisDlg::s_Position = QPoint(kx+31, ky+31);

	if(s_LanguageFilePath.length())
	{
		qApp->removeTranslator(&m_Translator);
		if(m_Translator.load(s_LanguageFilePath))
		{
			qApp->installTranslator(&m_Translator);
		}
	}


	if(LoadSettings())
	{
		m_RefGraph.LoadSettings(&settings);

		pAFoil->LoadSettings(&settings);
		pXDirect->LoadSettings(&settings);
		pMiarex->LoadSettings(&settings);
		pXInverse->LoadSettings(&settings);

        GL3DScales::LoadSettings(&settings);
        W3dPrefsDlg::LoadSettings(&settings);
	}

	SetupDataDir();

	SetUnits(s_LengthUnit, s_AreaUnit, s_SpeedUnit, s_WeightUnit, s_ForceUnit, s_MomentUnit,
			 s_mtoUnit, s_m2toUnit, s_mstoUnit, s_kgtoUnit, s_NtoUnit, s_NmtoUnit);

	pXDirect->SetAnalysisParams();
	CreateActions();
	CreateMenus();
	CreateToolbars();
	CreateStatusBar();

	m_ColorList.append(QColor(255,0,0));
	m_ColorList.append(QColor(0,255,0));
	m_ColorList.append(QColor(0,0,255));
	m_ColorList.append(QColor(255,255,0));
	m_ColorList.append(QColor(255,0,255));
	m_ColorList.append(QColor(0,255,255));
	m_ColorList.append(QColor(200,60,60));
	m_ColorList.append(QColor(0,160,0));
	m_ColorList.append(QColor(100,100,240));
	m_ColorList.append(QColor(170,170,0));
	m_ColorList.append(QColor(130,0,130));
	m_ColorList.append(QColor(0,130,130));
	m_ColorList.append(QColor(255,128,128));
	m_ColorList.append(QColor(0,255,128));
	m_ColorList.append(QColor(64,200,255));
	m_ColorList.append(QColor(170,170,100));
	m_ColorList.append(QColor(190,100,190));
	m_ColorList.append(QColor(100,170,170));
	m_ColorList.append(QColor(228,150,70));
	m_ColorList.append(QColor(170,255,170));
	m_ColorList.append(QColor(120,120,255));
	m_ColorList.append(QColor(228,228,128));
	m_ColorList.append(QColor(255,170,255));
	m_ColorList.append(QColor(170,255,255));
	m_ColorList.append(QColor(87,64,30));
	m_ColorList.append(QColor(117,81,26));
	m_ColorList.append(QColor(143,107,50));
	m_ColorList.append(QColor(179,146,93));
	m_ColorList.append(QColor(222,188,133));
	m_ColorList.append(QColor(0,110,41));
	m_ColorList.append(QColor(55,164,44));
	m_ColorList.append(QColor(177,210,143));
	m_ColorList.append(QColor(0,167,169));
	m_ColorList.append(QColor(0,96,102));
	m_ColorList.append(QColor(50,50,50));
	m_ColorList.append(QColor(90,90,90));
	m_ColorList.append(QColor(130,130,130));
	m_ColorList.append(QColor(170,170,170));
	m_ColorList.append(QColor(210,210,210));
	m_ColorList.append(QColor(255,255,255));

	s_bSaved     = true;
	m_bHighlightOpp = m_bHighlightWOpp = false;

	m_iApp = NOAPP;
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->hide();
	m_pctrlMiarexToolBar->hide();
	m_pctrlStabViewWidget->hide();

	SetMenus();

	QString styleSheet;
    if(DisplaySettingsDlg::s_bStyleSheets)
    {
        ReadStyleSheet(DisplaySettingsDlg::s_StyleSheetName, styleSheet);		
    }
}




MainFrame::~MainFrame()
{
	if(s_pTraceFile) s_pTraceFile->close();

    for(int ioa=s_oaFoil.size()-1; ioa>=0; ioa--)
	{
        delete (Foil*)s_oaFoil.at(ioa);
        s_oaFoil.removeAt(ioa);
	}

    for(int ioa=s_oaPolar.size()-1; ioa>=0; ioa--)
	{
        delete (Polar*)s_oaPolar.at(ioa);
        s_oaPolar.removeAt(ioa);
	}

	for(int ioa=m_oaOpp.size()-1; ioa>=0; ioa--)
	{
		delete (OpPoint*)m_oaOpp.at(ioa);
		m_oaOpp.removeAt(ioa);
	}

    for(int ioa=s_oaPlane.size()-1; ioa>=0; ioa--)
	{
        delete (Plane*)s_oaPlane.at(ioa);
        s_oaPlane.removeAt(ioa);
	}

    for(int ioa=s_oaWing.size()-1; ioa>=0; ioa--)
	{
        delete (Wing*)s_oaWing.at(ioa);
        s_oaWing.removeAt(ioa);
	}

	for(int ioa=m_oaWPolar.size()-1; ioa>=0; ioa--)
	{
		delete (WPolar*)m_oaWPolar.at(ioa);
		m_oaWPolar.removeAt(ioa);
	}

	for(int ioa=m_oaWOpp.size()-1; ioa>=0; ioa--)
	{
		delete (WingOpp*)m_oaWOpp.at(ioa);
		m_oaWOpp.removeAt(ioa);
	}

	for(int ioa=m_oaPOpp.size()-1; ioa>=0; ioa--)
	{
		delete (PlaneOpp*)m_oaPOpp.at(ioa);
		m_oaPOpp.removeAt(ioa);
	}

    for(int ioa=s_oaBody.size()-1; ioa>=0; ioa--)
	{
        delete (Body*)s_oaBody.at(ioa);
        s_oaBody.removeAt(ioa);
	}
}


void MainFrame::AboutQt()
{
#ifndef QT_NO_MESSAGEBOX
    QMessageBox::aboutQt(
#ifdef Q_WS_MAC
	this
#else
//            activeWindow()
	this
#endif // Q_WS_MAC
            );
#endif // QT_NO_MESSAGEBOX
}



void MainFrame::AboutQFLR5()
{
	AboutQ5 dlg(this);
	dlg.exec();
}


void MainFrame::AddFoil(Foil *pFoil)
{

	Foil *pOldFoil = foil(pFoil->m_FoilName);
	if(pOldFoil)
	{
		DeleteFoil(pOldFoil, false);
	}

	bool IsInserted = false;

    for (int i=0; i<s_oaFoil.size(); i++)
	{
        pOldFoil = (Foil*)s_oaFoil.at(i);
		if (pFoil->m_FoilName < pOldFoil->m_FoilName)
		{
            s_oaFoil.insert(i, pFoil);
			IsInserted = true;
			break;
		}
	}
    if (!IsInserted) s_oaFoil.append(pFoil);

	pFoil->InitFoil();
	s_pCurFoil = pFoil;
}


Polar* MainFrame::AddPolar(Polar *pPolar)
{
	if(!pPolar) return NULL;
	bool bExists   = false;
	bool bInserted = false;
	bool bFound;
	Polar *pOldPlr;
	Polar *pOld2Plr;
	QString strong;
	int i,j,k,l,p;

    for (i=0; i<s_oaPolar.size(); i++)
	{
    pOldPlr = (Polar*)s_oaPolar.at(i);
	if (pOldPlr->m_PlrName  == pPolar->m_PlrName &&
		pOldPlr->m_FoilName == pPolar->m_FoilName) bExists = true;
	}
	while(!bInserted)
	{
		if(!bExists)
		{
            for (j=0; j<s_oaPolar.size(); j++)
			{
                pOldPlr = (Polar*)s_oaPolar.at(j);

				if (pPolar->m_FoilName.compare(pOldPlr->m_FoilName, Qt::CaseInsensitive)<0)
				{
                    s_oaPolar.insert(j, pPolar);
					bInserted = true;
					break;
				}

				else if (pPolar->m_FoilName == pOldPlr->m_FoilName)
				{
					if(pPolar->m_PolarType < pOldPlr->m_PolarType)
					{
                        s_oaPolar.insert(j, pPolar);
						bInserted = true;
						break;
					}
					else if(pPolar->m_PolarType == pOldPlr->m_PolarType)
					{
						if (pPolar->m_PolarType != FIXEDAOAPOLAR)
						{
							//sort by re Nbr
							if(pPolar->m_Reynolds < pOldPlr->m_Reynolds)
							{
                                s_oaPolar.insert(j, pPolar);
								bInserted = true;
								break;
							}
						}
						else
						{
							//Type 4, sort by Alphas
							if(pPolar->m_ASpec < pOldPlr->m_ASpec)
							{
                                s_oaPolar.insert(j, pPolar);
								bInserted = true;
								break;
							}
						}
					}
				}
			}
			if(!bInserted)
			{
                s_oaPolar.append(pPolar);
				bInserted = true;
			}
			return pPolar;
		}
		else
		{
            for (l=0; l<s_oaPolar.size(); l++)
			{
                pOldPlr = (Polar*)s_oaPolar.at(l);
				if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
					pOldPlr->m_PlrName  == pPolar->m_PlrName)
				{
					p = 2;
					bFound = true;
					while(bFound)
					{
						strong = QString(" (%1)").arg( p);
						strong = pPolar->m_PlrName + strong;
                        for (k=0; k<s_oaPolar.size(); k++)
						{
							bFound = false;
                            pOld2Plr = (Polar*)s_oaPolar.at(k);
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
	if(!s_bSaved)
	{
		int resp = QMessageBox::question(this, tr("Exit"), tr("Save the project before exit ?"),
										 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
										 QMessageBox::Yes);
		if(resp == QMessageBox::Yes)
		{
			if(!SaveProject(m_FileName))
			{
				event->ignore();
				return;
			}
			AddRecentFile(m_FileName);
		}
		else if (resp==QMessageBox::Cancel)
		{
			event->ignore();
			return;
		}
	}
	DeleteProject(true);

	SaveSettings();
	event->accept();//continue closing
}


void MainFrame::contextMenuEvent (QContextMenuEvent * event)
{
	if(m_pctrlCentralWidget->currentIndex()==0) m_p2DWidget->contextMenuEvent(event);
	else                                        m_pGLWidget->contextMenuEvent(event);
}


void MainFrame::CreateActions()
{
	newProjectAct = new QAction(QIcon(":/images/new.png"), tr("New Project"), this);
	newProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
	newProjectAct->setStatusTip(tr("Save and close the current project, create a new project"));
	connect(newProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

	closeProjectAct = new QAction(QIcon(":/images/new.png"), tr("Close the Project"), this);
	closeProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	closeProjectAct->setStatusTip(tr("Save and close the current project"));
	connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(OnNewProject()));

	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	openAct->setStatusTip(tr("Open an existing file"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(OnLoadFile()));

	insertAct = new QAction(tr("&Insert Project..."), this);
	insertAct->setStatusTip(tr("Insert an existing project in the current project"));
	connect(insertAct, SIGNAL(triggered()), this, SLOT(OnInsertProject()));

	OnAFoilAct = new QAction(tr("&Direct Foil Design"), this);
	OnAFoilAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
	OnAFoilAct->setStatusTip(tr("Open Foil Design application"));
	connect(OnAFoilAct, SIGNAL(triggered()), this, SLOT(OnAFoil()));

	OnXInverseAct = new QAction(tr("&XFoil Inverse Design"), this);
	OnXInverseAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
	OnXInverseAct->setStatusTip(tr("Open XFoil inverse analysis application"));
	connect(OnXInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverse()));

	OnMixedInverseAct = new QAction(tr("&XFoil Mixed Inverse Design"), this);
	OnMixedInverseAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
	OnMixedInverseAct->setStatusTip(tr("Open XFoil Mixed Inverse analysis application"));
	connect(OnMixedInverseAct, SIGNAL(triggered()), this, SLOT(OnXInverseMixed()));

	OnXDirectAct = new QAction(tr("&XFoil Direct Analysis"), this);
	OnXDirectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
	OnXDirectAct->setStatusTip(tr("Open XFoil direct analysis application"));
	connect(OnXDirectAct, SIGNAL(triggered()), this, SLOT(OnXDirect()));

	OnMiarexAct = new QAction(tr("&Wing and Plane Design"), this);
	OnMiarexAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6));
	OnMiarexAct->setStatusTip(tr("Open Wing/plane design and analysis application"));
	connect(OnMiarexAct, SIGNAL(triggered()), this, SLOT(OnMiarex()));

	saveAct = new QAction(QIcon(":/images/save.png"), tr("Save"), this);
	saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	saveAct->setStatusTip(tr("Save the project to disk"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(OnSaveProject()));

	saveProjectAsAct = new QAction(tr("Save Project As..."), this);
	saveProjectAsAct->setStatusTip(tr("Save the current project under a new name"));
	connect(saveProjectAsAct, SIGNAL(triggered()), this, SLOT(OnSaveProjectAs()));

	saveOptionsAct = new QAction(tr("Save Options"), this);
	saveOptionsAct->setStatusTip(tr("Define the save options for operating points"));
	connect(saveOptionsAct, SIGNAL(triggered()), this, SLOT(OnSaveOptions()));

	unitsAct = new QAction(tr("Units..."), this);
	unitsAct->setStatusTip(tr("Define the units for this project"));
	connect(unitsAct, SIGNAL(triggered()), this, SLOT(OnUnits()));

	languageAct = new QAction(tr("Language..."), this);
	languageAct->setStatusTip(tr("Define the default language for the application"));
	connect(languageAct, SIGNAL(triggered()), this, SLOT(OnLanguage()));

	restoreToolbarsAct	 = new QAction(tr("Restore toolbars"), this);
	restoreToolbarsAct->setStatusTip(tr("Restores the toolbars to their original state"));
	connect(restoreToolbarsAct, SIGNAL(triggered()), this, SLOT(OnRestoreToolbars()));

	saveViewToImageFileAct = new QAction(tr("Save View to Image File"), this);
	saveViewToImageFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	saveViewToImageFileAct->setStatusTip(tr("Saves the current view to a file on disk"));
	connect(saveViewToImageFileAct, SIGNAL(triggered()), this, SLOT(OnSaveViewToImageFile()));


	resetSettingsAct = new QAction(tr("Reset Default Settings"), this);
	resetSettingsAct->setStatusTip(tr("will revert to default settings at the next session"));
	connect(resetSettingsAct, SIGNAL(triggered()), this, SLOT(OnResetSettings()));


	for (int i = 0; i < MAXRECENTFILES; ++i)
	{
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	styleAct = new QAction(tr("General Display Settings"), this);
	styleAct->setStatusTip(tr("Define the color and font options for all views and graphs"));
	connect(styleAct, SIGNAL(triggered()), this, SLOT(OnStyle()));


	exportCurGraphAct = new QAction(tr("Export Graph"), this);
	exportCurGraphAct->setStatusTip(tr("Export the current graph data to a text file"));
	connect(exportCurGraphAct, SIGNAL(triggered()), this, SLOT(OnExportCurGraph()));

	resetCurGraphScales = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales")+"\t(R)", this);
	resetCurGraphScales->setStatusTip(tr("Restores the graph's x and y scales"));
	connect(resetCurGraphScales, SIGNAL(triggered()), this, SLOT(OnResetCurGraphScales()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	guidelinesAct = new QAction(tr("&Guidelines"), this);
	guidelinesAct->setStatusTip(tr("Show the guidelines for some help"));
	connect(guidelinesAct, SIGNAL(triggered()), this, SLOT(OnGuidelines()));

	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("More information about XFLR5"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(AboutQFLR5()));

	aboutQtAct = new QAction(tr("About Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(AboutQt()));

	ShowPolarProps = new QAction(tr("Properties"), this);
	ShowPolarProps->setStatusTip(tr("Show the properties of the currently selected polar"));
	ShowPolarProps->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Return));
	connect(ShowPolarProps, SIGNAL(triggered()), this, SLOT(OnPolarProps()));

	ShowWOppProps = new QAction(tr("Properties"), this);
	ShowWOppProps->setStatusTip(tr("Show the properties of the currently selected operating point"));
	ShowWOppProps->setShortcut(QKeySequence(Qt::ALT + Qt::SHIFT + Qt::Key_Return));
	connect(ShowWOppProps, SIGNAL(triggered()), this, SLOT(OnWOppProps()));


	CreateAFoilActions();
	CreateXDirectActions();
	CreateXInverseActions();
	CreateMiarexActions();
}


void MainFrame::CreateAFoilActions()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	AFoilGridAct= new QAction(tr("Grid Options"), this);
	AFoilGridAct->setStatusTip(tr("Define the grid settings for the view"));
	connect(AFoilGridAct, SIGNAL(triggered()), pAFoil, SLOT(OnGrid()));
	
	storeSplineAct= new QAction(QIcon(":/images/OnStoreFoil.png"), tr("Store Splines as Foil"), this);
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

	InsertSplinePt = new QAction(tr("Insert Control Point")+"\tShift+Click", this);
	connect(InsertSplinePt, SIGNAL(triggered()), pAFoil, SLOT(OnInsertCtrlPt()));

	RemoveSplinePt = new QAction(tr("Remove Control Point")+"\tCtrl+Click", this);
	connect(RemoveSplinePt, SIGNAL(triggered()), pAFoil, SLOT(OnRemoveCtrlPt()));

	ResetXScaleAct= new QAction(QIcon(":/images/OnResetXScale.png"), tr("Reset X Scale"), this);
	ResetXScaleAct->setStatusTip(tr("Resets the scale to fit the current screen width"));
	connect(ResetXScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetXScale()));

	ResetXYScaleAct= new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset Scales")+"\t(R)", this);
	ResetXYScaleAct->setStatusTip(tr("Resets the x and y scales to screen size"));
	connect(ResetXYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetScales()));


	UndoAFoilAct= new QAction(QIcon(":/images/OnUndo.png"), tr("Undo"), this);
	UndoAFoilAct->setShortcut(Qt::CTRL + Qt::Key_Z);
	UndoAFoilAct->setStatusTip(tr("Cancels the last modification"));
	connect(UndoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnUndo()));

	RedoAFoilAct= new QAction(QIcon(":/images/OnRedo.png"), tr("Redo"), this);
	RedoAFoilAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
	RedoAFoilAct->setStatusTip(tr("Restores the last cancelled modification"));
	connect(RedoAFoilAct, SIGNAL(triggered()), pAFoil, SLOT(OnRedo()));

	ShowAllFoils= new QAction(tr("Show All Foils"), this);
	connect(ShowAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnShowAllFoils()));
	HideAllFoils= new QAction(tr("Hide All Foils"), this);
	connect(HideAllFoils, SIGNAL(triggered()), pAFoil, SLOT(OnHideAllFoils()));


	AFoilDelete = new QAction(tr("Delete..."), this);
	connect(AFoilDelete, SIGNAL(triggered()), pAFoil, SLOT(OnDelete()));

	AFoilRename = new QAction(tr("Rename..."), this);
	AFoilRename->setShortcut(Qt::Key_F2);
	connect(AFoilRename, SIGNAL(triggered()), pAFoil, SLOT(OnRenameFoil()));

	AFoilExport = new QAction(tr("Export..."), this);
	connect(AFoilExport, SIGNAL(triggered()), pAFoil, SLOT(OnExportCurFoil()));

	ShowCurrentFoil= new QAction(tr("Show Current Foil"), this);
	connect(ShowCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnShowCurrentFoil()));

	HideCurrentFoil= new QAction(tr("Hide Current Foil"), this);
	connect(HideCurrentFoil, SIGNAL(triggered()), pAFoil, SLOT(OnHideCurrentFoil()));

	ResetYScaleAct= new QAction(tr("Reset Y Scale"), this);
	connect(ResetYScaleAct, SIGNAL(triggered()), pAFoil, SLOT(OnResetYScale()));

	zoomInAct= new QAction(QIcon(":/images/OnZoomIn.png"), tr("Zoom in"), this);
	zoomInAct->setStatusTip(tr("Zoom the view by drawing a rectangle in the client area"));
	connect(zoomInAct, SIGNAL(triggered()), pAFoil, SLOT(OnZoomIn()));

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

	AFoilRefineGlobalFoil = new QAction(tr("Refine Globally")+"\t(F3)", this);
	connect(AFoilRefineGlobalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilPanels()));

	AFoilRefineLocalFoil = new QAction(tr("Refine Locally")+"\t(Shift+F3)", this);
	connect(AFoilRefineLocalFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilCadd()));

	AFoilEditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(AFoilEditCoordsFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilCoordinates()));

	AFoilScaleFoil = new QAction(tr("Scale camber and thickness")+"\t(F9)", this);
	connect(AFoilScaleFoil, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilFoilGeom()));

	AFoilSetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(AFoilSetTEGap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetTEGap()));

	AFoilSetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(AFoilSetLERadius, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetLERadius()));

	AFoilLECircle = new QAction(tr("Show LE Circle"), this);
	connect(AFoilLECircle, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilLECircle()));

	m_pShowLegend = new QAction(tr("Show Legend"), this);
	m_pShowLegend->setCheckable(true);
	connect(m_pShowLegend, SIGNAL(triggered()), pAFoil, SLOT(OnShowLegend()));

	AFoilSetFlap = new QAction(tr("Set Flap")+"\t(F10)", this);
	connect(AFoilSetFlap, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilSetFlap()));

	AFoilInterpolateFoils = new QAction(tr("Interpolate Foils")+"\t(F11)", this);
	connect(AFoilInterpolateFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilInterpolateFoils()));

	AFoilNacaFoils = new QAction(tr("Naca Foils"), this);
	connect(AFoilNacaFoils, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilNacaFoils()));

	AFoilTableColumns = new QAction(tr("Set Table Columns"), this);
	connect(AFoilTableColumns, SIGNAL(triggered()), pAFoil, SLOT(OnAFoilTableColumns()));

	AFoilTableColumnWidths = new QAction(tr("Reset column widths"), this);
	connect(AFoilTableColumnWidths, SIGNAL(triggered()), pAFoil, SLOT(OnColumnWidths()));

	AFoilLoadImage = new QAction(tr("Load background image")   +"\tCtrl+Shift+I", this);
	connect(AFoilLoadImage, SIGNAL(triggered()), pAFoil, SLOT(OnLoadBackImage()));
	AFoilClearImage = new QAction(tr("Clear background image") +"\tCtrl+Shift+I", this);

	connect(AFoilClearImage, SIGNAL(triggered()), pAFoil, SLOT(OnClearBackImage()));
}

//
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
	AFoilViewMenu->addAction(m_pShowLegend);
	AFoilViewMenu->addAction(AFoilLECircle);
	AFoilViewMenu->addAction(AFoilGridAct);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(AFoilLoadImage);
	AFoilViewMenu->addAction(AFoilClearImage);
	AFoilViewMenu->addSeparator();
	AFoilViewMenu->addAction(saveViewToImageFileAct);

	AFoilDesignMenu = menuBar()->addMenu(tr("F&oil"));
	AFoilDesignMenu->addAction(AFoilRename);
	AFoilDesignMenu->addAction(AFoilDelete);
	AFoilDesignMenu->addAction(AFoilExport);
	AFoilDesignMenu->addAction(DuplicateFoil);
	AFoilDesignMenu->addSeparator();
	AFoilDesignMenu->addAction(HideAllFoils);
	AFoilDesignMenu->addAction(ShowAllFoils);
	AFoilDesignMenu->addSeparator();
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
	AFoilDesignMenu->addAction(ManageFoilsAct);


	AFoilSplineMenu = menuBar()->addMenu(tr("&Splines"));
	AFoilSplineMenu->addAction(InsertSplinePt);
	AFoilSplineMenu->addAction(RemoveSplinePt);
	AFoilSplineMenu->addSeparator();
	AFoilSplineMenu->addAction(UndoAFoilAct);
	AFoilSplineMenu->addAction(RedoAFoilAct);
	AFoilSplineMenu->addSeparator();
	AFoilSplineMenu->addAction(newSplinesAct);
	AFoilSplineMenu->addAction(splineControlsAct);
	AFoilSplineMenu->addAction(storeSplineAct);
	AFoilSplineMenu->addAction(exportSplinesToFileAct);

	//AFoil Context Menu
	AFoilCtxMenu = new QMenu(tr("Context Menu"),this);
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
	AFoilCtxMenu->addAction(m_pShowLegend);
	AFoilCtxMenu->addAction(AFoilLECircle);
	AFoilCtxMenu->addAction(AFoilGridAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(AFoilLoadImage);
	AFoilCtxMenu->addAction(AFoilClearImage);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(saveViewToImageFileAct);
	AFoilCtxMenu->addSeparator();
	AFoilCtxMenu->addAction(AFoilTableColumns);
	AFoilCtxMenu->addAction(AFoilTableColumnWidths);

	//Context menu to be displayed when user right clicks on a foil in the table
	AFoilTableCtxMenu = new QMenu(tr("Foil Actions"),this);
	AFoilTableCtxMenu->addAction(AFoilRename);
	AFoilTableCtxMenu->addAction(AFoilDelete);
	AFoilTableCtxMenu->addAction(AFoilExport);
	AFoilTableCtxMenu->addAction(DuplicateFoil);
	AFoilTableCtxMenu->addSeparator();
	AFoilTableCtxMenu->addAction(AFoilNormalizeFoil);
	AFoilTableCtxMenu->addAction(AFoilDerotateFoil);
	AFoilTableCtxMenu->addAction(AFoilRefineLocalFoil);
	AFoilTableCtxMenu->addAction(AFoilRefineGlobalFoil);
	AFoilTableCtxMenu->addAction(AFoilEditCoordsFoil);
	AFoilTableCtxMenu->addAction(AFoilScaleFoil);
	AFoilTableCtxMenu->addAction(AFoilSetTEGap);
	AFoilTableCtxMenu->addAction(AFoilSetLERadius);
	AFoilTableCtxMenu->addAction(AFoilSetFlap);
	AFoilTableCtxMenu->addSeparator();
	AFoilTableCtxMenu->addAction(AFoilTableColumns);
	AFoilTableCtxMenu->addAction(AFoilTableColumnWidths);
}


void MainFrame::CreateAFoilToolbar()
{
	m_pctrlAFoilToolBar = addToolBar(tr("Foil"));
	m_pctrlAFoilToolBar->addAction(newProjectAct);
	m_pctrlAFoilToolBar->addAction(openAct);
	m_pctrlAFoilToolBar->addAction(saveAct);	
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(zoomInAct);
	m_pctrlAFoilToolBar->addAction(zoomLessAct);
	m_pctrlAFoilToolBar->addAction(ResetXYScaleAct);
	m_pctrlAFoilToolBar->addAction(ResetXScaleAct);
	m_pctrlAFoilToolBar->addAction(zoomYAct);
	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(UndoAFoilAct);
	m_pctrlAFoilToolBar->addAction(RedoAFoilAct);

	m_pctrlAFoilToolBar->addSeparator();
	m_pctrlAFoilToolBar->addAction(storeSplineAct);
}



void MainFrame::CreateDockWindows()
{
	QAFoil::s_pMainFrame           = this;
	QXDirect::s_pMainFrame         = this;
	QXInverse::s_pMainFrame        = this;
	QMiarex::s_pMainFrame          = this;
	ManageUFOsDlg::s_pMainFrame    = this;
	GL3dBodyDlg::s_pMainFrame      = this;
	ThreeDWidget::s_pMainFrame     = this;
	PlaneDlg::s_pMainFrame         = this;
	Body::s_pMainFrame             = this;
	LLTAnalysisDlg::s_pMainFrame   = this;
	XFoilAnalysisDlg::s_pMainFrame = this;
	BatchDlg::s_pMainFrame         = this;
	BatchThreadDlg::s_pMainFrame   = this;
	GraphDlg::s_pMainFrame         = this;

    m_pctrlXDirectWidget = new QDockWidget("XDirect", this);
	m_pctrlXDirectWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXDirectWidget);

	m_pctrlXInverseWidget = new QDockWidget("XInverse", this);
	m_pctrlXInverseWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlXInverseWidget);

	m_pctrlMiarexWidget = new QDockWidget("Miarex", this);
	m_pctrlMiarexWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, m_pctrlMiarexWidget);

	m_pctrlAFoilWidget = new QDockWidget("AFoil", this);
	m_pctrlAFoilWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, m_pctrlAFoilWidget);

	m_p2DWidget = new TwoDWidget(this);
    m_pGLWidget = new ThreeDWidget(this);

	m_pXDirect = new QXDirect(this);
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXDirectWidget->setWidget(pXDirect);
	m_pctrlXDirectWidget->setVisible(false);
	m_pctrlXDirectWidget->setFloating(true);
	m_pctrlXDirectWidget->move(960,60);

	m_pXInverse = new QXInverse(this);
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	pXInverse->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlXInverseWidget->setWidget(pXInverse);
	m_pctrlXInverseWidget->setVisible(false);
	m_pctrlXInverseWidget->setFloating(true);
	m_pctrlXInverseWidget->move(960,60);

    m_pMiarex = new QMiarex;
    QMiarex * pMiarex = (QMiarex*)m_pMiarex;
    pMiarex->setAttribute(Qt::WA_DeleteOnClose, false);
    m_pctrlMiarexWidget->setWidget(pMiarex);
    m_pctrlMiarexWidget->setVisible(false);
    m_pctrlMiarexWidget->setFloating(true);
    m_pctrlMiarexWidget->move(960,60);


	m_pGL3DScales = new GL3DScales(this);
	GL3DScales * pGL3DScales = (GL3DScales*)m_pGL3DScales;
	pGL3DScales->m_pMiarex      = m_pMiarex;
	pGL3DScales->m_pMainFrame    = this;
	pGL3DScales->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrl3DScalesWidget = new QDockWidget(tr("3D Scales"), this);
	m_pctrl3DScalesWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, m_pctrl3DScalesWidget);
	m_pctrl3DScalesWidget->setWidget(pGL3DScales);
	m_pctrl3DScalesWidget->setVisible(false);
	m_pctrl3DScalesWidget->setFloating(true);
	m_pctrl3DScalesWidget->move(60,60);

	StabViewDlg::s_pMiarex = m_pMiarex;
	m_pStabView = new StabViewDlg(this);
	StabViewDlg * pStabView = (StabViewDlg*)m_pStabView;
	pStabView->setAttribute(Qt::WA_DeleteOnClose, false);
	m_pctrlStabViewWidget = new QDockWidget(tr("Stability"), this);
	m_pctrlStabViewWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea, m_pctrlStabViewWidget);
	m_pctrlStabViewWidget->setWidget(pStabView);
	m_pctrlStabViewWidget->setVisible(false);
	m_pctrlStabViewWidget->setFloating(true);
	m_pctrlStabViewWidget->move(60,60);

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

	QMiarex::s_p2dWidget = m_p2DWidget;
	QMiarex::s_p3dWidget = m_pGLWidget;
	pMiarex->m_ArcBall.m_p3dWidget = m_pGLWidget;
    pMiarex->m_poaBody   = &s_oaBody;
    pMiarex->m_poaPlane  = &s_oaPlane;
    pMiarex->m_poaWing   = &s_oaWing;
	pMiarex->m_poaWPolar = &m_oaWPolar;
	pMiarex->m_poaWOpp   = &m_oaWOpp;
	pMiarex->m_poaPOpp   = &m_oaPOpp;


	QXDirect::s_p2DWidget              = m_p2DWidget;
	pXDirect->m_pCpGraph->m_pParent    = m_p2DWidget;
	for(int ig=0; ig<MAXPOLARGRAPHS; ig++) pXDirect->m_PlrGraph[ig].m_pParent = m_p2DWidget;
    pXDirect->m_poaFoil  = &s_oaFoil;
    pXDirect->m_poaPolar = &s_oaPolar;
	pXDirect->m_poaOpp   = &m_oaOpp;

	QAFoil::s_p2DWidget  = m_p2DWidget;
    pAFoil->m_poaFoil    = &s_oaFoil;
	pAFoil->m_pXFoil     = pXDirect->m_pXFoil;

	QXInverse::s_p2DWidget        = m_p2DWidget;
	pXInverse->s_pMainFrame       = this;
	pXInverse->m_pXFoil           = pXDirect->m_pXFoil;
	pXInverse->s_p2DWidget        = m_p2DWidget;
    pXInverse->m_poaFoil          = &s_oaFoil;

    GL3dWingDlg::s_poaFoil = &s_oaFoil;

	GL3dWingDlg::s_pMiarex      = m_pMiarex;
	ThreeDWidget::s_pMiarex     = m_pMiarex;
	PlaneDlg::s_pMiarex         = m_pMiarex;
	Wing::s_pMiarex             = m_pMiarex;
	WPolar::s_pMiarex           = m_pMiarex;
	LLTAnalysisDlg::s_pMiarex   = m_pMiarex;
	PanelAnalysisDlg::s_pMiarex = m_pMiarex;
	ManageUFOsDlg::s_pMiarex    = m_pMiarex;
	StabPolarDlg::s_pMiarex     = m_pMiarex;
	UFOTableDelegate::s_pMiarex = m_pMiarex;
	WPolarDlg::s_pMiarex        = m_pMiarex;


	Plane::SetParents(this, m_pMiarex);


	XFoilAnalysisDlg::s_pXDirect = m_pXDirect;
	NacaFoilDlg::s_pXFoil         = pXDirect->m_pXFoil;
	InterpolateFoilsDlg::s_pXFoil = pXDirect->m_pXFoil;
	CAddDlg::s_pXFoil             = pXDirect->m_pXFoil;
	TwoDPanelDlg::s_pXFoil        = pXDirect->m_pXFoil;
	FoilGeomDlg::s_pXFoil         = pXDirect->m_pXFoil;
	TEGapDlg::s_pXFoil            = pXDirect->m_pXFoil;
	LEDlg::s_pXFoil               = pXDirect->m_pXFoil;
	BatchDlg::s_pXFoil            = pXDirect->m_pXFoil;
	BatchDlg::s_pXDirect          = m_pXDirect;
	BatchThreadDlg::s_pXFoil      = pXDirect->m_pXFoil;
	BatchThreadDlg::s_pXDirect    = m_pXDirect;


	GraphDlg::s_ActivePage = 0;
}



void MainFrame::CreateMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newProjectAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(insertAct);
	fileMenu->addAction(closeProjectAct);
	fileMenu->addSeparator();
	fileMenu->addAction(saveAct);
	fileMenu->addAction(saveProjectAsAct);
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

	optionsMenu = menuBar()->addMenu(tr("Options"));
	optionsMenu->addSeparator();
	optionsMenu->addAction(languageAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(unitsAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(styleAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(saveOptionsAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(restoreToolbarsAct);
	optionsMenu->addSeparator();
	optionsMenu->addAction(resetSettingsAct);

	helpMenu = menuBar()->addMenu(tr("&?"));
	helpMenu->addAction(guidelinesAct);
	helpMenu->addAction(aboutQtAct);
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

	WOppAct = new QAction(QIcon(":/images/OnWOppView.png"), tr("OpPoint View")+"\tF5", this);
	WOppAct->setCheckable(true);
	WOppAct->setStatusTip(tr("Switch to the Operating point view"));
//	WOppAct->setShortcut(Qt::Key_F5);
	connect(WOppAct, SIGNAL(triggered()), pMiarex, SLOT(OnWOpps()));

	WPolarAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar View")+"\tF8", this);
	WPolarAct->setCheckable(true);
	WPolarAct->setStatusTip(tr("Switch to the Polar view"));
//	WPolarAct->setShortcut(Qt::Key_F8);
	connect(WPolarAct, SIGNAL(triggered()), pMiarex, SLOT(OnWPolars()));

	StabTimeAct = new QAction(QIcon(":/images/OnStabView.png"),tr("Time Response Vew")+"\tShift+F8", this);
	StabTimeAct->setCheckable(true);
	StabTimeAct->setStatusTip(tr("Switch to stability analysis post-processing"));
//	StabTimeAct->setShortcut(tr("Shift+F8"));
	connect(StabTimeAct, SIGNAL(triggered()), pMiarex, SLOT(OnTimeView()));

	RootLocusAct = new QAction(QIcon(":/images/OnRootLocus.png"),tr("Root Locus View")+"\tCtrl+F8", this);
	RootLocusAct->setCheckable(true);
	RootLocusAct->setStatusTip(tr("Switch to root locus view"));
	connect(RootLocusAct, SIGNAL(triggered()), pMiarex, SLOT(OnRootLocusView()));

	W3DAct = new QAction(QIcon(":/images/On3DView.png"), tr("3D View")+"\tF4", this);
	W3DAct->setCheckable(true);
	W3DAct->setStatusTip(tr("Switch to the 3D view"));
	connect(W3DAct, SIGNAL(triggered()), pMiarex, SLOT(On3DView()));

	CpViewAct = new QAction(QIcon(":/images/OnCpView.png"), tr("Cp View")+"\tF9", this);
	CpViewAct->setCheckable(true);
	CpViewAct->setStatusTip(tr("Switch to the Cp view"));
	connect(CpViewAct, SIGNAL(triggered()), pMiarex, SLOT(OnCpView()));

	W3DPrefsAct = new QAction(tr("3D Color Preferences"), this);
	W3DPrefsAct->setStatusTip(tr("Define the style and color preferences for the 3D view"));
	connect(W3DPrefsAct, SIGNAL(triggered()), pMiarex, SLOT(On3DPrefs()));

	MiarexPolarFilter = new QAction(tr("Polar Filter"), this);
	MiarexPolarFilter->setStatusTip(tr("Define which type of polars should be shown or hidden"));
	connect(MiarexPolarFilter, SIGNAL(triggered()), pMiarex, SLOT(OnPolarFilter()));

	W3DScalesAct = new QAction(tr("3D Scales"), this);
	W3DScalesAct->setStatusTip(tr("Define the scales for the 3D display of lift, moment, drag, and downwash"));
	W3DScalesAct->setCheckable(true);
	connect(W3DScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnGL3DScale()));

	W3DLightAct = new QAction(tr("3D Light Options"), this);
	W3DLightAct->setStatusTip(tr("Define the light options in 3D view"));
	connect(W3DLightAct, SIGNAL(triggered()), pMiarex, SLOT(OnSetupLight()));

	DefineWingAct = new QAction(tr("Define a New Wing")+"\tF3", this);
	DefineWingAct->setStatusTip(tr("Shows a dialogbox for editing a new wing definition"));
	connect(DefineWingAct, SIGNAL(triggered()), pMiarex, SLOT(OnNewWing()));

	halfWingAct = new QAction(tr("Half Wing"), this);
	halfWingAct->setCheckable(true);
	connect(halfWingAct, SIGNAL(triggered()), pMiarex, SLOT(OnHalfWing()));

	definePlaneAct = new QAction(tr("Define a New Plane")+"\tCtrl+F3", this);
	definePlaneAct->setStatusTip(tr("Shows a dialogbox to create a new plane definition"));
	connect(definePlaneAct, SIGNAL(triggered()), pMiarex, SLOT(OnNewPlane()));

	editUFOAct = new QAction(tr("Edit...")+"\tShift+F3", this);
	editUFOAct->setStatusTip(tr("Shows a dialogbox to edit the currently selected wing or plane"));
	connect(editUFOAct, SIGNAL(triggered()), pMiarex, SLOT(OnEditUFO()));

	renameCurUFO = new QAction(tr("Rename...")+"\tF2", this);
	renameCurUFO->setStatusTip(tr("Rename the currently selected object"));
	connect(renameCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnRenameCurUFO()));


	editCurBodyAct = new QAction(tr("Edit Body..."), this);
	editCurBodyAct->setStatusTip(tr("Edit the body of the currently selected plane"));
	editCurBodyAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_F10));
	connect(editCurBodyAct, SIGNAL(triggered()), pMiarex, SLOT(OnEditCurBody()));

	exporttoAVL = new QAction(tr("Export to AVL..."), this);
	exporttoAVL->setStatusTip(tr("Export the current plane or wing to a text file in the format required by AVL"));
	connect(exporttoAVL, SIGNAL(triggered()), pMiarex, SLOT(OnExporttoAVL()));

	exportCurWOpp = new QAction(tr("Export..."), this);
	exportCurWOpp->setStatusTip(tr("Export the current operating point to a text or csv file"));
	connect(exportCurWOpp, SIGNAL(triggered()), pMiarex, SLOT(OnExportCurWOpp()));

	resetWOppLegend = new QAction(tr("Reset Legend Position"), this);
	resetWOppLegend->setStatusTip(tr("Reset the legend position to its default value"));
	connect(resetWOppLegend, SIGNAL(triggered()), pMiarex, SLOT(OnResetWOppLegend()));

	resetWPlrLegend = new QAction(tr("Reset Legend Position"), this);
	resetWPlrLegend->setStatusTip(tr("Reset the legend position to its default value"));
	connect(resetWPlrLegend, SIGNAL(triggered()), pMiarex, SLOT(OnResetWPlrLegend()));

	resetWingScale = new QAction(tr("Reset Wing Scale"), this);
	resetWingScale ->setStatusTip(tr("Reset the wing scale to its default value"));
	connect(resetWingScale, SIGNAL(triggered()), pMiarex, SLOT(OnResetWingScale()));

	scaleWingAct = new QAction(tr("Scale Wing"), this);
	scaleWingAct->setStatusTip(tr("Scale the dimensions of the currently selected wing"));
	connect(scaleWingAct, SIGNAL(triggered()), pMiarex, SLOT(OnScaleWing()));

	manageUFOsAct = new QAction(tr("Manage objects"), this);
	manageUFOsAct->setStatusTip(tr("Rename or delete the planes and wings stored in the database"));
	manageUFOsAct->setShortcut(Qt::Key_F7);
	connect(manageUFOsAct, SIGNAL(triggered()), pMiarex, SLOT(OnManageUFOs()));

	m_pImportWPolar = new QAction(tr("Import Polar"), this);
	m_pImportWPolar->setStatusTip(tr("Import a polar from a text file"));
	connect(m_pImportWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnImportWPolar()));

	m_pUFOInertia = new QAction(tr("Define Inertia")+"\tF12", this);
	m_pUFOInertia->setStatusTip(tr("Define the inertia for the current plane or wing"));
	connect(m_pUFOInertia, SIGNAL(triggered()), pMiarex, SLOT(OnUFOInertia()));


	showCurWOppOnly = new QAction(tr("Show Current OpPoint Only"), this);
	showCurWOppOnly->setStatusTip(tr("Hide all the curves except for the one corresponding to the currently selected operating point"));
	showCurWOppOnly->setCheckable(true);
	connect(showCurWOppOnly, SIGNAL(triggered()), pMiarex, SLOT(OnCurWOppOnly()));

	showAllWOpps = new QAction(tr("Show All OpPoints"), this);
	showAllWOpps->setStatusTip(tr("Show the graph curves of all operating points"));
	connect(showAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWOpps()));

	hideAllWOpps = new QAction(tr("Hide All OpPoints"), this);
	hideAllWOpps->setStatusTip(tr("Hide the graph curves of all operating points"));
	connect(hideAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWOpps()));

	deleteAllWOpps = new QAction(tr("Delete All OpPoints"), this);
	deleteAllWOpps->setStatusTip(tr("Delete all the operating points of all planes and polars"));
	connect(deleteAllWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteAllWOpps()));

	showAllWPlrOpps = new QAction(tr("Show Associated OpPoints"), this);
	showAllWPlrOpps->setStatusTip(tr("Show the curves of all the operating points of the currently selected polar"));
	connect(showAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWPlrOpps()));
	
	hideAllWPlrOpps = new QAction(tr("Hide Associated OpPoints"), this);
	hideAllWPlrOpps->setStatusTip(tr("Hide the curves of all the operating points of the currently selected polar"));
	connect(hideAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWPlrOpps()));
	
	deleteAllWPlrOpps = new QAction(tr("Delete Associated OpPoints"), this);
	deleteAllWPlrOpps->setStatusTip(tr("Delete all the operating points of the currently selected polar"));
	connect(deleteAllWPlrOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteAllWPlrOpps()));

	showEllipticCurve = new QAction(tr("Show Elliptic Curve"), this);
	showEllipticCurve->setStatusTip(tr("Show the theoretical optimal elliptic lift curve on all graphs for which the selected variable is the local lift"));
	showEllipticCurve->setCheckable(true);
	connect(showEllipticCurve, SIGNAL(triggered()), pMiarex, SLOT(OnShowEllipticCurve()));

	showXCmRefLocation = new QAction(tr("Show XCG location"), this);
	showXCmRefLocation->setStatusTip(tr("Show the position of the center of gravity defined in the analysis"));
	showXCmRefLocation->setCheckable(true);
	connect(showXCmRefLocation, SIGNAL(triggered()), pMiarex, SLOT(OnShowXCmRef()));

	showStabCurve = new QAction(tr("Show Elevator Curve"), this);
	showStabCurve->setStatusTip(tr("Show the graph curves for the elevator"));
	showStabCurve->setCheckable(true);
	connect(showStabCurve, SIGNAL(triggered()), pMiarex, SLOT(OnStabCurve()));

	showFinCurve = new QAction(tr("Show Fin Curve"), this);
	showFinCurve->setStatusTip(tr("Show the graph curves for the fin"));
	showFinCurve->setCheckable(true);
	connect(showFinCurve, SIGNAL(triggered()), pMiarex, SLOT(OnFinCurve()));

	showWing2Curve = new QAction(tr("Show Second Wing Curve"), this);
	showWing2Curve->setStatusTip(tr("Show the graph curves for the second wing"));
	showWing2Curve->setCheckable(true);
	connect(showWing2Curve, SIGNAL(triggered()), pMiarex, SLOT(OnWing2Curve()));

	defineWPolar = new QAction(tr("Define an Analysis")+" \t(F6)", this);
	defineWPolar->setStatusTip(tr("Define an analysis for the current wing or plane"));
	connect(defineWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDefineWPolar()));

	editWPolar = new QAction(tr("Edit...")+" \t(Ctrl+F6)", this);
	editWPolar->setStatusTip(tr("Modify the analysis parameters of this polar"));
	connect(editWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnEditCurWPolar()));


	defineStabPolar = new QAction(tr("Define a Stability Analysis")+"\t(Shift+F6)", this);
	defineStabPolar->setStatusTip(tr("Define a stability analysis for the current wing or plane"));
//	defineStabPolar->setShortcut(tr("Ctrl+F6"));
	connect(defineStabPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDefineStabPolar()));

	MiarexGraphDlg = new QAction(tr("Define Graph Settings")+"\t(G)", this);
	MiarexGraphDlg->setStatusTip(tr("Define the settings for the selected graph"));
	connect(MiarexGraphDlg, SIGNAL(triggered()), pMiarex, SLOT(OnGraphSettings()));

	twoGraphs = new QAction(tr("Two Graphs")+"\t(T)", this);
	twoGraphs->setStatusTip(tr("Display the first two graphs"));
	twoGraphs->setCheckable(true);
	connect(twoGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnTwoGraphs()));
	
	fourGraphs = new QAction(tr("All Graphs")+"\t(A)", this);
	fourGraphs->setStatusTip(tr("Display all four graphs"));
	fourGraphs->setCheckable(true);
	connect(fourGraphs, SIGNAL(triggered()), pMiarex, SLOT(OnFourGraphs()));

	Graph1 = new QAction(tr("Graph 1")+"\t(1)", this);
	Graph1->setStatusTip(tr("Display only the first graph"));
	Graph1->setCheckable(true);
	connect(Graph1, SIGNAL(triggered()), pMiarex, SLOT(OnSingleGraph1()));
	
	Graph2 = new QAction(tr("Graph 2")+"\t(2)", this);
	Graph2->setStatusTip(tr("Display only the second graph"));
	Graph2->setCheckable(true);
	connect(Graph2, SIGNAL(triggered()), pMiarex, SLOT(OnSingleGraph2()));
	
	Graph3 = new QAction(tr("Graph 3")+"\t(3)", this);
	Graph3->setStatusTip(tr("Display only the third graph"));
	Graph3->setCheckable(true);
	connect(Graph3, SIGNAL(triggered()), pMiarex, SLOT(OnSingleGraph3()));

	Graph4 = new QAction(tr("Graph 4")+"\t(4)", this);
	Graph4->setStatusTip(tr("Display only the fourth graph"));
	Graph4->setCheckable(true);
	connect(Graph4, SIGNAL(triggered()), pMiarex, SLOT(OnSingleGraph4()));

	highlightWOppAct	 = new QAction(tr("Highlight Current OpPoint")+"\t(Ctrl+H)", this);
	highlightWOppAct->setCheckable(true);
	highlightWOppAct->setStatusTip(tr("Highlights on the polar curve the currently selected operating point"));
	connect(highlightWOppAct, SIGNAL(triggered()), pMiarex, SLOT(OnHighlightWOpp()));

	ResetWingGraphScale = new QAction(QIcon(":/images/OnResetGraphScale.png"), tr("Reset Graph Scales")+"\t(R)", this);
	ResetWingGraphScale->setStatusTip(tr("Reset the scale of the current operating point graph"));
	connect(ResetWingGraphScale, SIGNAL(triggered()), pMiarex, SLOT(OnResetWingGraphScale()));

	allWingGraphsScalesAct = new QAction(tr("Reset All Graph Scales"), this);
	allWingGraphsScalesAct->setStatusTip(tr("Reset the scales of all four operating point graphs"));
	connect(allWingGraphsScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnAllWingGraphScales()));

	allWPolarGraphsScalesAct = new QAction(tr("Reset All Graph Scales"), this);
	allWPolarGraphsScalesAct->setStatusTip(tr("Reset the scales of all four polar graphs"));
	connect(allWPolarGraphsScalesAct, SIGNAL(triggered()), pMiarex, SLOT(OnAllWPolarGraphScales()));

	allWingGraphsSettings = new QAction(tr("All Graph Settings"), this);
	allWingGraphsSettings->setStatusTip(tr("Define the settings of all four operating point graphs"));
	connect(allWingGraphsSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAllWingGraphSettings()));

	allWPolarGraphsSettings = new QAction(tr("All Graph Settings"), this);
	allWPolarGraphsSettings->setStatusTip(tr("Define the settings of all four polar graphs"));
	connect(allWPolarGraphsSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAllWPolarGraphSettings()));

	hideUFOWPlrs = new QAction(tr("Hide Associated Polars"), this);
	hideUFOWPlrs->setStatusTip(tr("Hide all the polar curves associated to the currently selected wing or plane"));
	connect(hideUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnHideUFOWPolars()));
	
	showUFOWPlrsOnly = new QAction(tr("Show Only Associated Polars"), this);
	connect(showUFOWPlrsOnly, SIGNAL(triggered()), pMiarex, SLOT(OnShowUFOWPolarsOnly()));

	showUFOWPlrs = new QAction(tr("Show Associated Polars"), this);
	showUFOWPlrs->setStatusTip(tr("Show all the polar curves associated to the currently selected wing or plane"));
	connect(showUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnShowUFOWPolars()));

	deleteUFOWPlrs = new QAction(tr("Delete Associated Polars"), this);
	deleteUFOWPlrs->setStatusTip(tr("Delete all the polars associated to the currently selected wing or plane"));
	connect(deleteUFOWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteUFOWPolars()));

	hideAllWPlrs = new QAction(tr("Hide All Polars"), this);
	hideAllWPlrs->setStatusTip(tr("Hide all the polar curves of all wings and planes"));
	connect(hideAllWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnHideAllWPolars()));
	
	showAllWPlrs = new QAction(tr("Show All Polars"), this);
	showAllWPlrs->setStatusTip(tr("Show all the polar curves of all wings and planes"));
	connect(showAllWPlrs, SIGNAL(triggered()), pMiarex, SLOT(OnShowAllWPolars()));

	hideUFOWOpps = new QAction(tr("Hide Associated OpPoints"), this);
	hideUFOWOpps->setStatusTip(tr("Hide all the operating point curves of the currently selected wing or plane"));
	connect(hideUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnHideUFOWOpps()));
	
	showUFOWOpps = new QAction(tr("Show Associated OpPoints"), this);
	showUFOWOpps->setStatusTip(tr("Show all the operating point curves of the currently selected wing or plane"));
	connect(showUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnShowUFOWOpps()));
	
	deleteUFOWOpps = new QAction(tr("Delete Associated OpPoints"), this);
	deleteUFOWOpps->setStatusTip(tr("Delete all the operating points of the currently selected wing or plane"));
	connect(deleteUFOWOpps, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteUFOWOpps()));

	deleteCurUFO = new QAction(tr("Delete..."), this);
	deleteCurUFO->setStatusTip(tr("Delete the currently selected wing or plane"));
	connect(deleteCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurUFO()));

	duplicateCurUFO = new QAction(tr("Duplicate..."), this);
	duplicateCurUFO->setStatusTip(tr("Duplicate the currently selected wing or plane"));
	connect(duplicateCurUFO, SIGNAL(triggered()), pMiarex, SLOT(OnDuplicateCurUFO()));

	SaveUFOAsProject = new QAction(tr("Save as Project..."), this);
	SaveUFOAsProject->setStatusTip(tr("Save the currently selected wing or plane as a new separate project"));
	connect(SaveUFOAsProject, SIGNAL(triggered()), this, SLOT(OnSaveUFOAsProject()));

	renameCurWPolar = new QAction(tr("Rename..."), this);
	renameCurWPolar->setStatusTip(tr("Rename the currently selected polar"));
	connect(renameCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnRenameCurWPolar()));

	exportCurWPolar = new QAction(tr("Export ..."), this);
	exportCurWPolar->setStatusTip(tr("Export the currently selected polar to a text or csv file"));
	connect(exportCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnExportCurWPolar()));

	resetCurWPolar = new QAction(tr("Reset ..."), this);
	resetCurWPolar->setStatusTip(tr("Delete all the points of the currently selected polar, but keep the analysis settings"));
	connect(resetCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnResetCurWPolar()));

	deleteCurWPolar = new QAction(tr("Delete ..."), this);
	deleteCurWPolar->setStatusTip(tr("Delete the currently selected polar"));
	connect(deleteCurWPolar, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurWPolar()));

	deleteCurWOpp = new QAction(tr("Delete..."), this);
	deleteCurWOpp->setStatusTip(tr("Delete the currently selected operating point"));
	connect(deleteCurWOpp, SIGNAL(triggered()), pMiarex, SLOT(OnDeleteCurWOpp()));

	advancedSettings = new QAction(tr("Advanced Settings..."), this);
	advancedSettings->setStatusTip(tr("Define the settings for LLT, VLM and Panel analysis"));
	connect(advancedSettings, SIGNAL(triggered()), pMiarex, SLOT(OnAdvancedSettings()));
}


void MainFrame::CreateMiarexMenus()
{
	//MainMenu for Miarex Application
	MiarexViewMenu = menuBar()->addMenu(tr("&View"));
	{
		MiarexViewMenu->addAction(WOppAct);
		MiarexViewMenu->addAction(WPolarAct);
		MiarexViewMenu->addAction(W3DAct);
		MiarexViewMenu->addAction(CpViewAct);
		MiarexViewMenu->addAction(StabTimeAct);
		MiarexViewMenu->addAction(RootLocusAct);
		MiarexViewMenu->addSeparator();
		MiarexViewMenu->addAction(W3DPrefsAct);
		MiarexViewMenu->addAction(W3DLightAct);
		MiarexViewMenu->addAction(W3DScalesAct);
		MiarexViewMenu->addSeparator();
		MiarexViewMenu->addAction(saveViewToImageFileAct);
	}


	UFOMenu = menuBar()->addMenu(tr("&Wing-Plane"));
	{
		UFOMenu->addAction(DefineWingAct);
		UFOMenu->addAction(definePlaneAct);
		UFOMenu->addAction(manageUFOsAct);
		currentUFOMenu = UFOMenu->addMenu(tr("Current UFO"));
		{
			currentUFOMenu->addAction(editUFOAct);
			currentUFOMenu->addAction(renameCurUFO);
			currentUFOMenu->addAction(duplicateCurUFO);
			currentUFOMenu->addAction(deleteCurUFO);
			currentUFOMenu->addAction(SaveUFOAsProject);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(editCurBodyAct);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(scaleWingAct);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(m_pUFOInertia);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(exporttoAVL);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(showUFOWPlrsOnly);
			currentUFOMenu->addAction(showUFOWPlrs);
			currentUFOMenu->addAction(hideUFOWPlrs);
			currentUFOMenu->addAction(deleteUFOWPlrs);
			currentUFOMenu->addSeparator();
			currentUFOMenu->addAction(hideUFOWOpps);
			currentUFOMenu->addAction(showUFOWOpps);
			currentUFOMenu->addAction(deleteUFOWOpps);
		}
	}

	MiarexWPlrMenu = menuBar()->addMenu(tr("&Polars"));
	{
		curWPlrMenu = MiarexWPlrMenu->addMenu(tr("Current Polar"));
		{
			curWPlrMenu->addAction(ShowPolarProps);
			curWPlrMenu->addAction(editWPolar);
			curWPlrMenu->addAction(renameCurWPolar);
			curWPlrMenu->addAction(deleteCurWPolar);
			curWPlrMenu->addAction(exportCurWPolar);
			curWPlrMenu->addAction(resetCurWPolar);
			curWPlrMenu->addSeparator();
			curWPlrMenu->addAction(showAllWPlrOpps);
			curWPlrMenu->addAction(hideAllWPlrOpps);
			curWPlrMenu->addAction(deleteAllWPlrOpps);
		}

		MiarexWPlrMenu->addSeparator();
		MiarexWPlrMenu->addAction(m_pImportWPolar);
		MiarexWPlrMenu->addSeparator();
		MiarexWPlrMenu->addAction(MiarexPolarFilter);
		MiarexWPlrMenu->addSeparator();
		MiarexWPlrMenu->addAction(hideAllWPlrs);
		MiarexWPlrMenu->addAction(showAllWPlrs);
		MiarexWPlrMenu->addSeparator();

		WPlrGraphMenu = MiarexWPlrMenu->addMenu(tr("Graphs"));
		{
			WPlrGraphMenu->addAction(Graph1);
			WPlrGraphMenu->addAction(Graph2);
			WPlrGraphMenu->addAction(Graph3);
			WPlrGraphMenu->addAction(Graph4);
			WPlrGraphMenu->addSeparator();
			WPlrGraphMenu->addAction(twoGraphs);
			WPlrGraphMenu->addAction(fourGraphs);
			WPlrGraphMenu->addSeparator();
			WPlrGraphMenu->addAction(allWPolarGraphsSettings);
			WPlrGraphMenu->addAction(allWPolarGraphsScalesAct);
			WPlrGraphMenu->addAction(resetWPlrLegend);
			WPlrGraphMenu->addSeparator();
			WPlrGraphMenu->addAction(highlightWOppAct);
		}
	}

	MiarexWOppMenu = menuBar()->addMenu(tr("&OpPoint"));
	{
		curWOppMenu = MiarexWOppMenu->addMenu(tr("Current OpPoint"));
		{
			curWOppMenu->addAction(ShowWOppProps);
			curWOppMenu->addAction(exportCurWOpp);
			curWOppMenu->addAction(deleteCurWOpp);
		}
		MiarexWOppMenu->addSeparator();
		MiarexWOppMenu->addAction(showCurWOppOnly);
		MiarexWOppMenu->addAction(showAllWOpps);
		MiarexWOppMenu->addAction(hideAllWOpps);
		MiarexWOppMenu->addAction(deleteAllWOpps);
		MiarexWOppMenu->addSeparator();
		MiarexWOppMenu->addAction(halfWingAct);
		MiarexWOppMenu->addAction(showEllipticCurve);
		MiarexWOppMenu->addAction(showXCmRefLocation);
		MiarexWOppMenu->addAction(showWing2Curve);
		MiarexWOppMenu->addAction(showStabCurve);
		MiarexWOppMenu->addAction(showFinCurve);
		MiarexWOppMenu->addSeparator();

		WOppGraphMenu = MiarexWOppMenu->addMenu(tr("Graphs"));
		{
			WOppGraphMenu->addAction(Graph1);
			WOppGraphMenu->addAction(Graph2);
			WOppGraphMenu->addAction(Graph3);
			WOppGraphMenu->addAction(Graph4);
			WOppGraphMenu->addSeparator();
			WOppGraphMenu->addAction(twoGraphs);
			WOppGraphMenu->addAction(fourGraphs);
			WOppGraphMenu->addSeparator();
			WOppGraphMenu->addAction(allWingGraphsSettings);
			WOppGraphMenu->addAction(allWingGraphsScalesAct);
			WOppGraphMenu->addAction(resetWOppLegend);
		}
	}

	//Miarex Analysis Menu
	MiarexAnalysisMenu  = menuBar()->addMenu(tr("&Analysis"));
	{
		MiarexAnalysisMenu->addAction(defineWPolar);
		MiarexAnalysisMenu->addAction(defineStabPolar);
		MiarexAnalysisMenu->addSeparator();
		MiarexAnalysisMenu->addAction(viewLogFile);
		MiarexAnalysisMenu->addAction(advancedSettings);
	}


	//WOpp View Context Menu
	WOppCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		WOppCtxMenu->addMenu(currentUFOMenu);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addMenu(curWPlrMenu);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addMenu(curWOppMenu);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addAction(showCurWOppOnly);
		WOppCtxMenu->addAction(showAllWOpps);
		WOppCtxMenu->addAction(hideAllWOpps);
		WOppCtxMenu->addAction(deleteAllWOpps);
		WOppCtxMenu->addSeparator();
		WOppCurGraphMenu = WOppCtxMenu->addMenu(tr("Current Graph"));
		{
			WOppCurGraphMenu->addAction(MiarexGraphDlg);
			WOppCurGraphMenu->addAction(exportCurGraphAct);
		}
		WOppCtxMenu->addMenu(WOppCurGraphMenu);
		WOppCtxMenu->addMenu(WOppGraphMenu);
		WOppCtxMenu->addAction(ResetWingGraphScale);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addAction(halfWingAct);
		WOppCtxMenu->addAction(showEllipticCurve);
		WOppCtxMenu->addAction(showXCmRefLocation);
		WOppCtxMenu->addAction(showWing2Curve);
		WOppCtxMenu->addAction(showStabCurve);
		WOppCtxMenu->addAction(showFinCurve);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addAction(resetWingScale);
		WOppCtxMenu->addSeparator();
		WOppCtxMenu->addAction(viewLogFile);
		WOppCtxMenu->addAction(saveViewToImageFileAct);
	}

	//WOpp View Context Menu
	WCpCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		WCpCtxMenu->addMenu(currentUFOMenu);
		WCpCtxMenu->addSeparator();
		WCpCtxMenu->addMenu(curWPlrMenu);
		WCpCtxMenu->addSeparator();
		WCpCtxMenu->addMenu(curWOppMenu);
		WCpCtxMenu->addSeparator();
		WCpCtxMenu->addMenu(WOppCurGraphMenu);
		WCpCtxMenu->addAction(ResetWingGraphScale);
		WCpCtxMenu->addSeparator();
		WCpCtxMenu->addAction(showWing2Curve);
		WCpCtxMenu->addAction(showStabCurve);
		WCpCtxMenu->addAction(showFinCurve);
		WCpCtxMenu->addSeparator();
		WCpCtxMenu->addAction(viewLogFile);
		WCpCtxMenu->addAction(saveViewToImageFileAct);
	}

	//WTime View Context Menu
	WTimeCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		WTimeCtxMenu->addMenu(currentUFOMenu);
		WTimeCtxMenu->addSeparator();
		WTimeCtxMenu->addMenu(curWPlrMenu);
		WTimeCtxMenu->addSeparator();
		WTimeCtxMenu->addMenu(curWOppMenu);
		WTimeCtxMenu->addSeparator();
		WTimeCtxMenu->addAction(showCurWOppOnly);
		WTimeCtxMenu->addAction(showAllWOpps);
		WTimeCtxMenu->addAction(hideAllWOpps);
		WTimeCtxMenu->addAction(deleteAllWOpps);
		WTimeCtxMenu->addSeparator();
		WTimeCtxMenu->addMenu(WOppCurGraphMenu);
		WTimeCtxMenu->addMenu(WOppGraphMenu);
		WTimeCtxMenu->addSeparator();
		WTimeCtxMenu->addAction(viewLogFile);
		WTimeCtxMenu->addAction(saveViewToImageFileAct);
	}

	//Polar View Context Menu
	WPlrCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		WPlrCtxMenu->addMenu(currentUFOMenu);
		WPlrCtxMenu->addSeparator();
		WPlrCtxMenu->addMenu(curWPlrMenu);
		WPlrCtxMenu->addSeparator();
		WPlrCtxMenu->addMenu(WPlrGraphMenu);
		WPlrCurGraphMenu = WPlrCtxMenu->addMenu(tr("Current Graph"));
		{
			WPlrCurGraphMenu->addAction(MiarexGraphDlg);
			WPlrCurGraphMenu->addAction(exportCurGraphAct);
		}
		WPlrCtxMenu->addAction(resetCurGraphScales);
		WPlrCtxMenu->addSeparator();
		WPlrCtxMenu->addAction(hideAllWPlrs);
		WPlrCtxMenu->addAction(showAllWPlrs);
		WPlrCtxMenu->addSeparator();
		WPlrCtxMenu->addAction(viewLogFile);
		WPlrCtxMenu->addAction(saveViewToImageFileAct);
	}

	//W3D View Context Menu
	W3DCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		W3DCtxMenu->addMenu(currentUFOMenu);
		W3DCtxMenu->addSeparator();
		W3DCtxMenu->addMenu(curWPlrMenu);
		W3DCtxMenu->addSeparator();
		W3DCtxMenu->addMenu(curWOppMenu);
		W3DCtxMenu->addSeparator();
		W3DCtxMenu->addAction(deleteAllWOpps);
		W3DCtxMenu->addSeparator();
		W3DCtxMenu->addAction(W3DScalesAct);
		W3DCtxMenu->addAction(W3DLightAct);
		W3DCtxMenu->addSeparator();
		W3DCtxMenu->addAction(viewLogFile);
		W3DCtxMenu->addAction(saveViewToImageFileAct);
	}

	//W3D Stab View Context Menu
	W3DStabCtxMenu = new QMenu(tr("Context Menu"),this);
	{
		W3DStabCtxMenu->addMenu(currentUFOMenu);
		W3DStabCtxMenu->addSeparator();
		W3DStabCtxMenu->addMenu(curWPlrMenu);
		W3DStabCtxMenu->addSeparator();
		W3DStabCtxMenu->addMenu(curWOppMenu);
		W3DStabCtxMenu->addSeparator();
		W3DStabCtxMenu->addAction(W3DLightAct);
		W3DStabCtxMenu->addSeparator();
		W3DStabCtxMenu->addAction(viewLogFile);
		W3DStabCtxMenu->addAction(saveViewToImageFileAct);
	}
}


void MainFrame::CreateMiarexToolbar()
{
	m_pctrlUFO    = new QComboBox();
	m_pctrlWPolar = new QComboBox;
	m_pctrlWOpp   = new QComboBox;
	m_pctrlUFO->setMinimumWidth(150);
	m_pctrlWPolar->setMinimumWidth(150);
	m_pctrlWOpp->setMinimumWidth(80);
	m_pctrlUFO->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlWPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlWOpp->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlUFO->setMaxVisibleItems(23);
	m_pctrlWPolar->setMaxVisibleItems(23);
	m_pctrlWOpp->setMaxVisibleItems(23);

	m_pctrlMiarexToolBar = addToolBar(tr("UFO"));
	m_pctrlMiarexToolBar->addAction(newProjectAct);
	m_pctrlMiarexToolBar->addAction(openAct);
	m_pctrlMiarexToolBar->addAction(saveAct);
	m_pctrlMiarexToolBar->addSeparator();
	m_pctrlMiarexToolBar->addAction(WOppAct);
	m_pctrlMiarexToolBar->addAction(WPolarAct);
	m_pctrlMiarexToolBar->addAction(W3DAct);
	m_pctrlMiarexToolBar->addAction(CpViewAct);
	m_pctrlMiarexToolBar->addAction(RootLocusAct);
	m_pctrlMiarexToolBar->addAction(StabTimeAct);

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
	m_pctrlFoil    = new QComboBox;
	m_pctrlPolar   = new QComboBox;
	m_pctrlOpPoint = new QComboBox;
	m_pctrlFoil->setMaxVisibleItems(23);
	m_pctrlPolar->setMaxVisibleItems(23);
	m_pctrlOpPoint->setMaxVisibleItems(23);
	m_pctrlFoil->setMinimumWidth(150);
	m_pctrlPolar->setMinimumWidth(150);
	m_pctrlOpPoint->setMinimumWidth(80);
	m_pctrlFoil->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlPolar->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_pctrlOpPoint->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	m_pctrlXDirectToolBar = addToolBar(tr("Foil"));
	m_pctrlXDirectToolBar->addAction(newProjectAct);
	m_pctrlXDirectToolBar->addAction(openAct);
	m_pctrlXDirectToolBar->addAction(saveAct);
	m_pctrlXDirectToolBar->addSeparator();
	m_pctrlXDirectToolBar->addAction(OpPointsAct);
	m_pctrlXDirectToolBar->addAction(PolarsAct);
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

	OpPointsAct = new QAction(QIcon(":/images/OnCpView.png"), tr("OpPoint view")+"\tF5", this);
	OpPointsAct->setCheckable(true);
	OpPointsAct->setStatusTip(tr("Show Operating point view"));
	connect(OpPointsAct, SIGNAL(triggered()), pXDirect, SLOT(OnOpPoints()));

	PolarsAct = new QAction(QIcon(":/images/OnPolarView.png"), tr("Polar view")+"\tF8", this);
	PolarsAct->setCheckable(true);
	PolarsAct->setStatusTip(tr("Show Polar view"));
	connect(PolarsAct, SIGNAL(triggered()), pXDirect, SLOT(OnPolars()));

	defineCpGraphSettings = new QAction(tr("Define Cp Graph Settings")+"\t(G)", this);
	connect(defineCpGraphSettings, SIGNAL(triggered()), pXDirect, SLOT(OnCpGraphSettings()));

	XDirectPolarFilter = new QAction(tr("Polar Filter"), this);
	connect(XDirectPolarFilter, SIGNAL(triggered()), pXDirect, SLOT(OnPolarFilter()));

	allPolarGraphsSettingsAct = new QAction(tr("All Polar Graph Settings"), this);
	allPolarGraphsSettingsAct->setStatusTip("Modifies the setting for all polar graphs simultaneously");
	connect(allPolarGraphsSettingsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphsSetting()));

	allPolarGraphsScales = new QAction(tr("Reset All Polar Graph Scales"), this);
	connect(allPolarGraphsScales, SIGNAL(triggered()), pXDirect, SLOT(OnResetAllPolarGraphsScales()));

	XDirectGraphDlg = new QAction(tr("Define Graph Settings")+"\t(G)", this);
	connect(XDirectGraphDlg, SIGNAL(triggered()), pXDirect, SLOT(OnGraphSettings()));

	resetGraphLegend = new QAction(tr("Reset Legend Position"), this);
	connect(resetGraphLegend, SIGNAL(triggered()), pXDirect, SLOT(OnResetGraphLegend()));

	TwoPolarGraphsAct = new QAction(tr("Two Polar Graphs")+"\t(T)", this);
	TwoPolarGraphsAct->setCheckable(true);
	connect(TwoPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnCouplePolarGraphs()));

	AllPolarGraphsAct = new QAction(tr("All Polar Graphs")+"\t(A)", this);
	AllPolarGraphsAct->setCheckable(true);
	connect(AllPolarGraphsAct, SIGNAL(triggered()), pXDirect, SLOT(OnAllPolarGraphs()));

	for (int i = 0; i < 5; ++i)
	{
		PolarGraphAct[i] = new QAction(this);
		PolarGraphAct[i]->setData(i);
		PolarGraphAct[i]->setCheckable(true);
		connect(PolarGraphAct[i], SIGNAL(triggered()), pXDirect, SLOT(OnSinglePolarGraph()));
	}
	PolarGraphAct[0]->setText(tr("Cl vs. Cd") +"\t(1)");
	PolarGraphAct[1]->setText(tr("Cl vs.Alpha")+ "\t(2)");
	PolarGraphAct[2]->setText(tr("Cl vs. Xtr.")+ "\t(3)");
	PolarGraphAct[3]->setText(tr("Cm vs.Alpha")+ "\t(4)");
	PolarGraphAct[4]->setText(tr("Glide ratio vs. alpha")+ "\t(5)");

	highlightOppAct	 = new QAction(tr("Highlight Current OpPoint")+"\t(Ctrl+H)", this);
	highlightOppAct->setCheckable(true);
	highlightOppAct->setStatusTip(tr("Highlights on the polar curve the currently selected operating point"));
	connect(highlightOppAct, SIGNAL(triggered()), pXDirect, SLOT(OnHighlightOpp()));


	deleteCurFoil = new QAction(tr("Delete..."), this);
	connect(deleteCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteCurFoil()));

	renameCurFoil = new QAction(tr("Rename...")+"\tF2", this);
	connect(renameCurFoil, SIGNAL(triggered()), this, SLOT(OnRenameCurFoil()));

	exportCurFoil = new QAction(tr("Export..."), this);
	connect(exportCurFoil, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurFoil()));

	setCurFoilStyle = new QAction(tr("Set Style..."), this);
	connect(setCurFoilStyle, SIGNAL(triggered()), this, SLOT(OnCurFoilStyle()));

	deleteFoilPolars = new QAction(tr("Delete associated polars"), this);
	deleteFoilPolars->setStatusTip(tr("Delete all the polars associated to this foil"));
	connect(deleteFoilPolars, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilPolars()));

	showFoilPolarsOnly = new QAction(tr("Show only associated polars"), this);
	connect(showFoilPolarsOnly, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilPolarsOnly()));

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

	exportPolarOpps = new QAction(tr("Export associated OpPoints"), this);
	connect(exportPolarOpps, SIGNAL(triggered()), pXDirect, SLOT(OnExportPolarOpps()));

	hideFoilOpps = new QAction(tr("Hide associated OpPoints"), this);
	connect(hideFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnHideFoilOpps()));

	showFoilOpps = new QAction(tr("Show associated OpPoints"), this);
	connect(showFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnShowFoilOpps()));

	deleteFoilOpps = new QAction(tr("Delete associated OpPoints"), this);
	connect(deleteFoilOpps, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteFoilOpps()));

	definePolar = new QAction(tr("Define an Analysis")+"\tF6", this);
	definePolar->setStatusTip(tr("Defines a single analysis/polar"));
	connect(definePolar, SIGNAL(triggered()), pXDirect, SLOT(OnDefinePolar()));

	defineBatch = new QAction(tr("Batch Analysis")+"\tShift+F6", this);
	defineBatch->setStatusTip(tr("Launches a batch of analysis calculation for a specified range or list of Reynolds numbers"));
	connect(defineBatch, SIGNAL(triggered()), pXDirect, SLOT(OnBatchAnalysis()));

	MultiThreadedBatchAct = new QAction(tr("Multi-threaded Batch Analysis")+"\tCtrl+F6", this);
	MultiThreadedBatchAct->setStatusTip(tr("Launches a batch of analysis calculation using all available computer CPU cores"));
	connect(MultiThreadedBatchAct, SIGNAL(triggered()), pXDirect, SLOT(OnMultiThreadedBatchAnalysis()));

	deletePolar = new QAction(tr("Delete"), this);
	deletePolar->setStatusTip(tr("Deletes the currently selected polar"));
	connect(deletePolar, SIGNAL(triggered()), pXDirect, SLOT(OnDeleteCurPolar()));

	resetCurPolar = new QAction(tr("Reset"), this);
	resetCurPolar->setStatusTip(tr("Deletes the contents of the currently selected polar"));
	connect(resetCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnResetCurPolar()));

	editCurPolar = new QAction(tr("Edit"), this);
	editCurPolar->setStatusTip(tr("Remove the unconverged or erroneaous points of the currently selected polar"));
	connect(editCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnEditCurPolar()));

	exportCurPolar = new QAction(tr("Export"), this);
	connect(exportCurPolar, SIGNAL(triggered()), pXDirect, SLOT(OnExportCurPolar()));

	exportAllPolars = new QAction(tr("Export all polars"), this);
	connect(exportAllPolars, SIGNAL(triggered()), pXDirect, SLOT(OnExportAllPolars()));

	XDirectStyleAct = new QAction(tr("Define Styles"), this);
	XDirectStyleAct->setStatusTip(tr("Define the style for the boundary layer and the pressure arrows"));
	connect(XDirectStyleAct, SIGNAL(triggered()), pXDirect, SLOT(OnXDirectStyle()));

	ManageFoilsAct = new QAction(tr("Manage Foils"), this);
	ManageFoilsAct->setShortcut(Qt::Key_F7);
	connect(ManageFoilsAct, SIGNAL(triggered()), pXDirect, SLOT(OnManageFoils()));

	RenamePolarAct = new QAction(tr("Rename"), this);
	connect(RenamePolarAct, SIGNAL(triggered()), pXDirect, SLOT(OnRenamePolar()));

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
 
	viewLogFile = new QAction(tr("View Log File")+"\t(L)", this);
	connect(viewLogFile, SIGNAL(triggered()), this, SLOT(OnLogFile()));

	DerotateFoil = new QAction(tr("De-rotate the Foil"), this);
	connect(DerotateFoil, SIGNAL(triggered()), pXDirect, SLOT(OnDerotateFoil()));

	NormalizeFoil = new QAction(tr("Normalize the Foil"), this);
	connect(NormalizeFoil, SIGNAL(triggered()), pXDirect, SLOT(OnNormalizeFoil()));

	RefineLocalFoil = new QAction(tr("Refine Locally")+"\t(Shift+F3)", this);
	connect(RefineLocalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnCadd()));

	RefineGlobalFoil = new QAction(tr("Refine Globally")+"\t(F3)", this);
	connect(RefineGlobalFoil, SIGNAL(triggered()), pXDirect, SLOT(OnRefinePanelsGlobally()));

	EditCoordsFoil = new QAction(tr("Edit Foil Coordinates"), this);
	connect(EditCoordsFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilCoordinates()));

	ScaleFoil = new QAction(tr("Scale camber and thickness")+"\t(F9)", this);
	connect(ScaleFoil, SIGNAL(triggered()), pXDirect, SLOT(OnFoilGeom()));

	SetTEGap = new QAction(tr("Set T.E. Gap"), this);
	connect(SetTEGap, SIGNAL(triggered()), pXDirect, SLOT(OnSetTEGap()));

	SetLERadius = new QAction(tr("Set L.E. Radius"), this);
	connect(SetLERadius, SIGNAL(triggered()), pXDirect, SLOT(OnSetLERadius()));

	SetFlap = new QAction(tr("Set Flap")+"\t(F10)", this);
	connect(SetFlap, SIGNAL(triggered()), pXDirect, SLOT(OnSetFlap()));

	InterpolateFoils = new QAction(tr("Interpolate Foils")+"\t(F11)", this);
	connect(InterpolateFoils, SIGNAL(triggered()), pXDirect, SLOT(OnInterpolateFoils()));

	NacaFoils = new QAction(tr("Naca Foils"), this);
	connect(NacaFoils, SIGNAL(triggered()), pXDirect, SLOT(OnNacaFoils()));

	DuplicateFoil = new QAction(tr("Duplicate"), this);
	connect(DuplicateFoil, SIGNAL(triggered()), this, SLOT(OnDuplicateFoil()));

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

//	m_pImportJavaFoilPolar = new QAction(tr("Import JavaFoil Polar"), this);
//	connect(m_pImportJavaFoilPolar, SIGNAL(triggered()), pXDirect, SLOT(OnImportJavaFoilPolar()));

	m_pImportXFoilPolar = new QAction(tr("Import XFoil Polar"), this);
	connect(m_pImportXFoilPolar, SIGNAL(triggered()), pXDirect, SLOT(OnImportXFoilPolar()));
}


void MainFrame::CreateXDirectMenus()
{
	//MainMenu for XDirect Application
	XDirectViewMenu = menuBar()->addMenu(tr("&View"));
	XDirectViewMenu->addAction(OpPointsAct);
	XDirectViewMenu->addAction(PolarsAct);
	XDirectViewMenu->addSeparator();
	XDirectViewMenu->addAction(saveViewToImageFileAct);

	FoilMenu = menuBar()->addMenu(tr("&Foil"));
	FoilMenu->addAction(ManageFoilsAct);
	FoilMenu->addSeparator();
	currentFoilMenu = FoilMenu->addMenu(tr("Current Foil"));
	currentFoilMenu->addAction(setCurFoilStyle);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(exportCurFoil);
	currentFoilMenu->addAction(renameCurFoil);
	currentFoilMenu->addAction(deleteCurFoil);
	currentFoilMenu->addAction(DuplicateFoil);
	currentFoilMenu->addSeparator();
	currentFoilMenu->addAction(showFoilPolarsOnly);
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
	DesignMenu->addAction(RefineGlobalFoil);
	DesignMenu->addAction(RefineLocalFoil);
	DesignMenu->addAction(EditCoordsFoil);
	DesignMenu->addAction(ScaleFoil);
	DesignMenu->addAction(SetTEGap);
	DesignMenu->addAction(SetLERadius);
	DesignMenu->addAction(SetFlap);
	DesignMenu->addSeparator();
	DesignMenu->addAction(InterpolateFoils);
	DesignMenu->addAction(NacaFoils);

	XFoilAnalysisMenu = menuBar()->addMenu(tr("Analysis"));
	XFoilAnalysisMenu->addAction(definePolar);
	XFoilAnalysisMenu->addAction(defineBatch);
	XFoilAnalysisMenu->addAction(MultiThreadedBatchAct);
	XFoilAnalysisMenu->addSeparator();
	XFoilAnalysisMenu->addAction(resetXFoil);
	XFoilAnalysisMenu->addAction(viewXFoilAdvanced);
	XFoilAnalysisMenu->addAction(viewLogFile);

	PolarMenu = menuBar()->addMenu(tr("&Polars"));
	currentPolarMenu = PolarMenu->addMenu(tr("Current Polar"));
	currentPolarMenu->addAction(ShowPolarProps);
	currentPolarMenu->addAction(editCurPolar);
	currentPolarMenu->addAction(resetCurPolar);
	currentPolarMenu->addAction(deletePolar);
	currentPolarMenu->addAction(RenamePolarAct);
	currentPolarMenu->addAction(exportCurPolar);
	currentPolarMenu->addSeparator();
	currentPolarMenu->addAction(showPolarOpps);
	currentPolarMenu->addAction(hidePolarOpps);
	currentPolarMenu->addAction(deletePolarOpps);
	currentPolarMenu->addAction(exportPolarOpps);
	PolarMenu->addSeparator();
	PolarMenu->addAction(m_pImportXFoilPolar);
//	PolarMenu->addAction(m_pImportJavaFoilPolar);
	PolarMenu->addSeparator();
	PolarMenu->addAction(exportAllPolars);
	PolarMenu->addSeparator();
	PolarMenu->addAction(XDirectPolarFilter);
	PolarMenu->addSeparator();
	PolarMenu->addAction(showAllPolars);
	PolarMenu->addAction(hideAllPolars);
	PolarMenu->addSeparator();
	GraphPolarMenu = PolarMenu->addMenu(tr("Polar Graphs"));
	GraphPolarMenu->addAction(allPolarGraphsSettingsAct);
	GraphPolarMenu->addAction(allPolarGraphsScales);
	GraphPolarMenu->addAction(resetGraphLegend);
	GraphPolarMenu->addSeparator();
	GraphPolarMenu->addAction(AllPolarGraphsAct);
	GraphPolarMenu->addAction(TwoPolarGraphsAct);
	GraphPolarMenu->addSeparator();
	for(int i=0; i<5; i++)
		GraphPolarMenu->addAction(PolarGraphAct[i]);
	GraphPolarMenu->addSeparator();
	GraphPolarMenu->addAction(highlightOppAct);


	OpPointMenu = menuBar()->addMenu(tr("Operating Points"));
	currentOppMenu = OpPointMenu->addMenu(tr("Current OpPoint"));
	currentOppMenu->addAction(ShowWOppProps);
	currentOppMenu->addAction(exportCurOpp);
	currentOppMenu->addAction(deleteCurOpp);
	CpGraphMenu = OpPointMenu->addMenu(tr("Cp Graph"));
	CpGraphMenu->addAction(setCpVarGraph);
	CpGraphMenu->addAction(setQVarGraph);
	CpGraphMenu->addSeparator();
	CpGraphMenu->addAction(showInviscidCurve);
	CpGraphMenu->addSeparator();
	CurXFoilResults = CpGraphMenu->addMenu(tr("Current XFoil Results"));
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
	CpGraphMenu->addAction(resetCurGraphScales);
	CpGraphMenu->addAction(defineCpGraphSettings);
	CpGraphMenu->addAction(exportCurGraphAct);
	OpPointMenu->addSeparator();
	OpPointMenu->addAction(showCurOppOnly);
	OpPointMenu->addAction(hideAllOpPoints);
	OpPointMenu->addAction(showAllOpPoints);

	//XDirect foil Context Menu
	OperFoilCtxMenu = new QMenu(tr("Context Menu"),this);
	OperFoilCtxMenu->addMenu(currentFoilMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addMenu(currentPolarMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addMenu(DesignMenu);
	OperFoilCtxMenu->addSeparator();//_______________
	CurOppCtxMenu = OperFoilCtxMenu->addMenu(tr("Current OpPoint"));
	CurOppCtxMenu->addAction(ShowWOppProps);
	CurOppCtxMenu->addAction(exportCurOpp);
	CurOppCtxMenu->addAction(deleteCurOpp);

	OperFoilCtxMenu->addSeparator();//_______________
//	CurGraphCtxMenu = OperFoilCtxMenu->addMenu(tr("Cp graph"));
	OperFoilCtxMenu->addMenu(CpGraphMenu);

	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(definePolar);
	OperFoilCtxMenu->addAction(defineBatch);
	OperFoilCtxMenu->addAction(MultiThreadedBatchAct);
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
	OperFoilCtxMenu->addSeparator();//_______________
	OperFoilCtxMenu->addAction(saveViewToImageFileAct);
	//End XDirect foil Context Menu


	//XDirect polar Context Menu
	OperPolarCtxMenu = new QMenu(tr("Context Menu"),this);
	OperPolarCtxMenu->addMenu(currentFoilMenu);
	OperPolarCtxMenu->addMenu(currentPolarMenu);
	OperPolarCtxMenu->addSeparator();//_______________
	CurGraphCtxMenu = OperPolarCtxMenu->addMenu(tr("Current Graph"));
	CurGraphCtxMenu->addAction(resetCurGraphScales);
	CurGraphCtxMenu->addAction(XDirectGraphDlg);
	CurGraphCtxMenu->addAction(exportCurGraphAct);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addMenu(GraphPolarMenu);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(definePolar);
	OperPolarCtxMenu->addAction(defineBatch);
	OperPolarCtxMenu->addAction(MultiThreadedBatchAct);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(showAllPolars);
	OperPolarCtxMenu->addAction(hideAllPolars);
	OperPolarCtxMenu->addAction(showAllOpPoints);
	OperPolarCtxMenu->addAction(hideAllOpPoints);
	OperPolarCtxMenu->addSeparator();//_______________
	OperPolarCtxMenu->addAction(saveViewToImageFileAct);

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

	InverseResetScale = new QAction(QIcon(":/images/OnResetFoilScale.png"), tr("Reset foil scale")+"\t(R)", this);
	InverseResetScale->setStatusTip(tr("Resets the scale to fit the screen size"));
	connect(InverseResetScale, SIGNAL(triggered()), pXInverse, SLOT(OnResetFoilScale()));

	InverseInsertCtrlPt = new QAction(tr("Insert Control Point")+"\tShift+Click", this);
	connect(InverseInsertCtrlPt, SIGNAL(triggered()), pXInverse, SLOT(OnInsertCtrlPt()));

	InverseRemoveCtrlPt = new QAction(tr("Remove Control Point")+"\tCtrl+Click", this);
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
	XInverseGraphDlg = new QAction(tr("Define Graph Settings")+"\t(G)", this);
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
	InverseContextMenu = new QMenu(tr("Context Menu"),this);
	InverseContextMenu->addAction(InverseStyles);
	InverseContextMenu->addAction(InverseResetScale);
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
	m_pctrlFullInverse  = new QRadioButton(tr("Full Inverse"));
	m_pctrlMixedInverse = new QRadioButton(tr("Mixed Inverse"));
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;
	connect(m_pctrlFullInverse,  SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));
	connect(m_pctrlMixedInverse, SIGNAL(clicked()), pXInverse, SLOT(OnInverseApp()));

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
	m_pctrlXInverseToolBar->addAction(InverseZoomIn);
	m_pctrlXInverseToolBar->addAction(InverseZoomX);
	m_pctrlXInverseToolBar->addAction(InverseZoomY);
	m_pctrlXInverseToolBar->addAction(resetCurGraphScales);
	m_pctrlXInverseToolBar->addAction(InverseResetScale);
}



Foil* MainFrame::DeleteFoil(Foil *pFoil, bool bAsk)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	if(!pFoil || !pFoil->m_FoilName.length()) return NULL;
	QString strong;
	Foil *pOldFoil, *pNextFoil=NULL;
	OpPoint * pOpPoint;
	Polar* pPolar;
	int j;

	if(bAsk)
	{
		strong = tr("Are you sure you want to delete")  +"\n"+ pFoil->m_FoilName +"\n";
		strong+= tr("and all associated OpPoints and Polars ?");

		int resp = QMessageBox::question(this,tr("Question"), strong,  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if(resp != QMessageBox::Yes) return NULL;
	}

	for (j= m_oaOpp.size()-1; j>=0; j--)
	{
		pOpPoint = (OpPoint*)m_oaOpp[j];
		if(pOpPoint->m_strFoilName == pFoil->m_FoilName)
		{
			m_oaOpp.removeAt(j);
			delete pOpPoint;
		}
	}
    for (j= s_oaPolar.size()-1;j>=0; j--)
	{
        pPolar = (Polar*)s_oaPolar.at(j);
		if(pPolar->m_FoilName == pFoil->m_FoilName)
		{
            s_oaPolar.removeAt(j);
			delete pPolar;
		}
	}
    for (j=0; j< s_oaFoil.size(); j++)
	{
        pOldFoil = (Foil*)s_oaFoil.at(j);
		if (pOldFoil == pFoil)
		{
            if(j>0)                    pNextFoil = (Foil*)s_oaFoil.at(j-1);
            else if(s_oaFoil.size()>1) pNextFoil = (Foil*)s_oaFoil.at(1);
			else                       pNextFoil = NULL;
            s_oaFoil.removeAt(j);
			delete pOldFoil;
			if(s_pCurFoil == pOldFoil) s_pCurFoil = NULL;
			break;
		}
	}
	pXDirect->m_pCurOpp = NULL;
	pXDirect->m_pCurPolar = NULL;
	s_pCurFoil = NULL;
	pXDirect->SetControls();
	SetSaveState(false);

	return pNextFoil;
}



void MainFrame::DeletePlane(void *pPlanePtr, bool bResultsOnly)
{
	Plane *pPlane = (Plane*)pPlanePtr;
	if(!pPlane || !pPlane->PlaneName().length()) return ;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	WPolar* pWPolar;
	PlaneOpp * pPOpp;
	Plane *pOldPlane;
	SetSaveState(false);
	int i;
	//first remove all POpps associated to the plane
	for (i=m_oaPOpp.size()-1; i>=0; i--)
	{
		pPOpp = (PlaneOpp*)m_oaPOpp.at(i);
		if(pPOpp->m_PlaneName == pPlane->PlaneName())
		{
			m_oaPOpp.removeAt(i);
			delete pPOpp;
		}
	}

	pMiarex->m_pCurPOpp = NULL;
	pMiarex->m_pCurWOpp = NULL;
	//next remove all PPolars associated to the plane
	for (i=m_oaWPolar.size()-1; i>=0; i--)
	{
		pWPolar = (WPolar*)m_oaWPolar.at(i);
		if (pWPolar->m_UFOName == pPlane->PlaneName())
		{
			if(!bResultsOnly)
			{
				m_oaWPolar.removeAt(i);
				if(pWPolar == pMiarex->m_pCurWPolar)
				{
					pMiarex->m_pCurWPolar = NULL;
				}
				delete pWPolar;
			}
			else
			{
				pWPolar->ResetWPlr();
				//results only... means that the areas and spans have been edited... update polar
				if( pWPolar->m_RefAreaType==PLANFORMAREA)
				{
					pWPolar->m_WArea     = pMiarex->m_pCurWing->m_PlanformArea;
					pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_PlanformSpan;
				}
				else
				{
					pWPolar->m_WArea     = pMiarex->m_pCurWing->m_ProjectedArea;
					pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_ProjectedSpan;
				}
				pWPolar->m_WMAChord  = pMiarex->m_pCurWing->m_MAChord;
			}
		}
	}

	if(bResultsOnly) return;


    for (i=s_oaPlane.size()-1; i>=0; i--)
	{
        pOldPlane = (Plane*)s_oaPlane.at(i);
		if (pOldPlane == pPlane)
		{
            s_oaPlane.removeAt(i);
			delete pPlane;
			if(pPlane == pMiarex->m_pCurPlane)
			{
				pMiarex->m_pCurPlane = NULL;
				pMiarex->m_pCurWing  = NULL;
				for(int iw=0; iw<MAXWINGS; iw++) pMiarex->m_pWingList[iw] = NULL;
			}
			break;
		}
	}
}


void MainFrame::DeleteProject(bool bClosing)
{
	// clear everything
	int i;
	void *pObj;

    for (i=s_oaPlane.size()-1; i>=0; i--)
	{
        pObj = s_oaPlane.at(i);
        s_oaPlane.removeAt(i);
		delete (Plane*)pObj;
	}

	for (i=m_oaPOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaPOpp.at(i);
		m_oaPOpp.removeAt(i);
		delete (PlaneOpp*)pObj;
	}
    for (i=s_oaWing.size()-1; i>=0; i--)
	{
        pObj = s_oaWing.at(i);
        s_oaWing.removeAt(i);
		delete (Wing*)pObj;
	}

	for (i=m_oaWPolar.size()-1; i>=0; i--)
	{
		pObj = m_oaWPolar.at(i);
		m_oaWPolar.removeAt(i);
		delete (WPolar*)pObj;
	}

	for (i=m_oaWOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaWOpp.at(i);
		m_oaWOpp.removeAt(i);
		delete (WingOpp*)pObj;
	}

    for (i=s_oaFoil.size()-1; i>=0; i--)
	{
        pObj = s_oaFoil.at(i);
        s_oaFoil.removeAt(i);
		delete (Foil*)pObj;
	}
    for (i=s_oaPolar.size()-1; i>=0; i--)
	{
        pObj = s_oaPolar.at(i);
        s_oaPolar.removeAt(i);
		delete (Polar*)pObj;
	}
	for (i=m_oaOpp.size()-1; i>=0; i--)
	{
		pObj = m_oaOpp.at(i);
		m_oaOpp.removeAt(i);
		delete (OpPoint*)pObj;
	}
    for (i=s_oaBody.size()-1; i>=0; i--)
	{
        pObj = s_oaBody.at(i);
        s_oaBody.removeAt(i);
		delete (Body*)pObj;
	}

	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_pCurPlane  = NULL;
	pMiarex->m_pCurPOpp   = NULL;
	pMiarex->m_pCurWing   = NULL;
	pMiarex->m_pCurWPolar = NULL;
	pMiarex->m_pCurWOpp   = NULL;
	pMiarex->m_bStream = false;

	if(!bClosing)
	{
		UpdateUFOs();
		pMiarex->SetUFO();
		if(pMiarex->m_iView==WPOLARVIEW)    pMiarex->CreateWPolarCurves();
		else if(pMiarex->m_iView==WOPPVIEW) pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==WCPVIEW)  pMiarex->CreateCpCurves();
		if(m_iApp==MIAREX) pMiarex->SetControls();

		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->m_pXFoil->m_FoilName = "";
		s_pCurFoil  = NULL;
		pXDirect->m_pCurPolar = NULL;
		pXDirect->m_pCurOpp   = NULL;
		pXDirect->SetFoil();

		UpdateFoils();
		if(pXDirect->m_bPolarView) pXDirect->CreatePolarCurves();
		else                       pXDirect->CreateOppCurves();

		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->FillFoilTable();
		pAFoil->SelectFoil();


		QXInverse *pXInverse =(QXInverse*)m_pXInverse;
		pXInverse->Clear();

		SetProjectName("");
		SetSaveState(true);
	}
}




void MainFrame::DeleteWing(void *pWingPtr, bool bResultsOnly)
{
	int i;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	if(!pWingPtr) return;
	Wing *pThisWing = (Wing*)pWingPtr;

	SetSaveState(false);

	//first remove all WOpps associated to the wing
	WingOpp * pWOpp;
	for (i=m_oaWOpp.size()-1; i>=0; i--)
	{
		pWOpp = (WingOpp*)m_oaWOpp.at(i);
		if(pWOpp->m_WingName == pThisWing->m_WingName)
		{
			m_oaWOpp.removeAt(i);
			if(pWOpp == pMiarex->m_pCurWOpp) pMiarex->m_pCurWOpp = NULL;
			delete pWOpp;
		}
		pMiarex->m_pCurWOpp = NULL;
	}

	//next remove all WPolars associated to the wing
	WPolar* pWPolar;
	for (i=m_oaWPolar.size()-1; i>=0; i--)
	{
		pWPolar = (WPolar*)m_oaWPolar.at(i);
		if (pWPolar->m_UFOName == pThisWing->m_WingName)
		{
			if(!bResultsOnly)
			{
				m_oaWPolar.removeAt(i);
				if(pWPolar == pMiarex->m_pCurWPolar)
				{
					pMiarex->m_pCurWPolar = NULL;
				}
				delete pWPolar;
			}
			else
			{
				pWPolar->ResetWPlr();
				//results only... means that the geometry have been edited... update polar
				if( pWPolar->m_RefAreaType==PLANFORMAREA)
				{
					pWPolar->m_WArea     = pMiarex->m_pCurWing->m_PlanformArea;
					pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_PlanformSpan;
				}
				else
				{
					pWPolar->m_WArea     = pMiarex->m_pCurWing->m_ProjectedArea;
					pWPolar->m_WSpan     = pMiarex->m_pCurWing->m_ProjectedSpan;
				}
				pWPolar->m_WMAChord  = pMiarex->m_pCurWing->m_MAChord;
			}
		}
	}

	if(bResultsOnly) return;// that's enough

	// ... Find the wing in the object array and remove it...
	Wing* pWing;
    for (i=s_oaWing.size()-1; i>=0; i--)
	{
        pWing = (Wing*)s_oaWing.at(i);
		if (pWing == pThisWing)
		{
            s_oaWing.removeAt(i);
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
			Foil *pFoil;
			for (j=0; j<m_ColorList.size(); j++)
			{
                for (i=0; i<s_oaFoil.size(); i++)
				{
                    pFoil = (Foil*)s_oaFoil.at(i);
					bFound = false;
					if(pFoil->m_FoilColor == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_ColorList.at(j);
			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}
		case 1:
		{
			Polar *pPolar;
			for (j=0; j<m_ColorList.size(); j++)
			{
                for (i=0; i<s_oaPolar.size(); i++)
				{
                    pPolar = (Polar*)s_oaPolar.at(i);
					bFound = false;
					if(pPolar->m_Color == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound)
					return m_ColorList.at(j);
			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}
		case 2:
		{
			OpPoint *pOpPoint;
			for (j=0; j<m_ColorList.size(); j++){
				for (i=0; i<m_oaOpp.size(); i++)
				{
					pOpPoint = (OpPoint*)m_oaOpp.at(i);
					bFound = false;
					if(pOpPoint->m_Color == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_ColorList.at(j);
			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}
		case 4:
		{
			WPolar *pWPolar;
			for (j=0; j<m_ColorList.size(); j++)
			{
				for (i=0; i<m_oaWPolar.size(); i++)
				{
					pWPolar = (WPolar*)m_oaWPolar.at(i);
					bFound = false;
					if(pWPolar->m_Color == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound)
					return m_ColorList.at(j);

			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}
		case 5:
		{
			WingOpp *pWOpp;
			for (j=0; j<m_ColorList.size(); j++)
			{
				for (i=0; i<m_oaWOpp.size(); i++)
				{
					pWOpp = (WingOpp*)m_oaWOpp.at(i);
					bFound = false;
					if(pWOpp->m_Color == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_ColorList.at(j);
			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}
		case 6:
		{
			PlaneOpp *pPOpp;
			for (j=0; j<m_ColorList.size(); j++)
			{
				for (i=0; i<m_oaPOpp.size(); i++)
				{
					pPOpp = (PlaneOpp*)m_oaPOpp.at(i);
					bFound = false;
					if(pPOpp->m_Color == m_ColorList.at(j))
					{
						bFound = true;
						break;
					}
				}
				if(!bFound)
					return m_ColorList.at(j);
			}
			return QColor((int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0),
						  (int)((double)qrand()/(double)RAND_MAX*255.0));
		}

		default:
		{
			return QColor(255,0,0);
		}
	}
	return color;
}



Foil* MainFrame::foil(QString strFoilName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strFoilName.length()) return NULL;
	Foil* pFoil;
    for (int i=0; i<s_oaFoil.size(); i++)
	{
        pFoil = (Foil*)s_oaFoil.at(i);
		if (pFoil->m_FoilName == strFoilName)
		{
			return pFoil;
		}
	}

	return NULL;
}

Polar *MainFrame::GetPolar(QString m_FoilName, QString PolarName)
{
	if (!PolarName.length())
	{
		//try to get the first from the Combobox
		if(!m_pctrlPolar->count()) return NULL;        //nothing more to try
		PolarName = m_pctrlPolar->itemText(0); //... and carry on
	}

	Polar *pPolar;
    for (int i=0; i<s_oaPolar.size(); i++)
	{
        pPolar = (Polar*) s_oaPolar.at(i);
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
	Polar *pCurPolar = pXDirect->m_pCurPolar;
	if(!pCurPolar) return NULL;

	for (int i=0; i<m_oaOpp.size(); i++)
	{
		if(!pCurPolar) return NULL;
		pOpPoint = (OpPoint*)m_oaOpp.at(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == s_pCurFoil->m_FoilName)
		{
			if (pOpPoint->m_strPlrName == pCurPolar->m_PlrName)
			{
				if(pCurPolar->m_PolarType != FIXEDAOAPOLAR)
				{
					if(qAbs(pOpPoint->Alpha - Alpha) <0.001)
					{
						return pOpPoint;
					}
				}
				else
				{
					if(qAbs(pOpPoint->Reynolds - Alpha) <0.1)
					{
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}


void MainFrame::keyPressEvent(QKeyEvent *event)
{
	if(m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyPressEvent(event);
	}
	else if(m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyPressEvent(event);
	}
	else if(m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyPressEvent(event);
	}
	else if(m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyPressEvent(event);
	}
	else
	{
		switch (event->key())
		{
			case Qt::Key_F1:
			{
				OnMiarex();

				break;
			}			case Qt::Key_L:
			{
				OnLogFile();
				break;
			}
			case Qt::Key_Control:
			{
				QMiarex* pMiarex = (QMiarex*)m_pMiarex;
				pMiarex->m_bArcball = true;
				UpdateView();
				break;
			}	

			default:
				event->ignore();
		}
	}
}


void MainFrame::keyReleaseEvent(QKeyEvent *event)
{
	if(m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyReleaseEvent(event);
	}
	else if(m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;

		if (event->key()==Qt::Key_Control)
		{
			pMiarex->m_bArcball = false;
			UpdateView();
		}
		else pMiarex->keyReleaseEvent(event);
	}
	else if(m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyReleaseEvent(event);
	}
	else if(m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyReleaseEvent(event);
	}
}


bool MainFrame::LoadPolarFileV3(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	Foil *pFoil;
	Polar *pPolar = NULL;
	Polar *pOldPlr;
	OpPoint *pOpp, *pOldOpp;

	//first read all available foils
	int i,l,n;
	ar>>n;

	for (i=0;i<n; i++)
	{
		pFoil = new Foil();

		if (!pFoil->Serialize(ar, bIsStoring))
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
		pPolar = new Polar();

		pPolar->m_Color = GetColor(1);

		if (!pPolar->Serialize(ar, bIsStoring))
		{
			delete pPolar;
			return false;
		}
        for (l=0; l<s_oaPolar.size(); l++)
		{
            pOldPlr = (Polar*)s_oaPolar[l];
			if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
				pOldPlr->m_PlrName  == pPolar->m_PlrName)
			{
				//just overwrite...
                s_oaPolar.removeAt(l);
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

		pOpp->m_Color = m_ColorList[m_oaOpp.size()%24];
		if(ArchiveFormat>=100002)
		{
			if (!pOpp->Serialize(ar, bIsStoring, 100002))
			{
				delete pOpp;
				return false;
			}
			else
			{
				pFoil = foil(pOpp->m_strFoilName);
				if(pFoil)
				{
//					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
//					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
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
				pFoil = foil(pOpp->m_strFoilName);
				if(pFoil)
				{
//					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
//					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
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
					qAbs(pOldOpp->Alpha-pOpp->Alpha)<0.001)
				{
					//just overwrite...
					m_oaOpp.removeAt(l);
					delete pOldOpp;
					//... and continue to add
				}
			}
		}
		((QXDirect*)m_pXDirect)->InsertOpPoint(pOpp);
	}

	return true;
}


bool MainFrame::LoadSettings()
{
	QPoint pt;
	bool bFloat;
	int SettingsFormat;
	QSize size;
	

#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"sourceforge.net","xflr5");
#else
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif

	settings.beginGroup("MainFrame");
	{
		SettingsFormat = settings.value("SettingsFormat").toInt();
		if(SettingsFormat != SETTINGSFORMAT) return false;


		DisplaySettingsDlg::s_StyleName = settings.value("StyleName","").toString();

		s_LanguageFilePath = settings.value("LanguageFilePath").toString();

		m_GraphExportFilter = settings.value("GraphExportFilter",".csv").toString();

		bFloat  = settings.value("Miarex_Float").toBool();
		pt.rx() = settings.value("Miarex_x").toInt();
		pt.ry() = settings.value("Miarex_y").toInt();
		size    = settings.value("MiarexSize").toSize();
		m_pctrlMiarexWidget->setFloating(bFloat);
		if(bFloat) m_pctrlMiarexWidget->move(pt);
		m_pctrlMiarexWidget->resize(size);

		bFloat  = settings.value("XDirect_Float").toBool();
		pt.rx() = settings.value("XDirect_x").toInt();
		pt.ry() = settings.value("XDirect_y").toInt();
		size    = settings.value("XDirectSize").toSize();
		m_pctrlXDirectWidget->setFloating(bFloat);
		if(bFloat) m_pctrlXDirectWidget->move(pt);
		m_pctrlXDirectWidget->resize(size);

		bFloat  = settings.value("AFoil_Float").toBool();
		pt.rx() = settings.value("AFoil_x").toInt();
		pt.ry() = settings.value("AFoil_y").toInt();
		size    = settings.value("AFoilSize").toSize();
		m_pctrlAFoilWidget->setFloating(bFloat);
		if(bFloat) m_pctrlAFoilWidget->move(pt);
		m_pctrlAFoilWidget->resize(size);

		bFloat  = settings.value("XInverse_Float").toBool();
		pt.rx() = settings.value("XInverse_x").toInt();
		pt.ry() = settings.value("XInverse_y").toInt();
		size    = settings.value("XInverseSize").toSize();
		m_pctrlXInverseWidget->setFloating(bFloat);
		if(bFloat) m_pctrlXInverseWidget->move(pt);
		m_pctrlXInverseWidget->resize(size);

		bFloat  = settings.value("StabView_Float").toBool();
		pt.rx() = settings.value("StabView_x").toInt();
		pt.ry() = settings.value("StabView_y").toInt();
		size    = settings.value("StabSize").toSize();
		m_pctrlStabViewWidget->setFloating(bFloat);
		if(bFloat) m_pctrlStabViewWidget->move(pt);
		m_pctrlStabViewWidget->resize(size);

		s_LastDirName = settings.value("LastDirName").toString();
		m_ImageDirName = settings.value("ImageDirName").toString();
		m_ExportLastDirName = settings.value("ExportLastDirName").toString();
		s_LengthUnit  = settings.value("LengthUnit").toInt();
		s_AreaUnit    = settings.value("AreaUnit").toInt();
		s_WeightUnit  = settings.value("WeightUnit").toInt();
		s_SpeedUnit   = settings.value("SpeedUnit").toInt();
		s_ForceUnit   = settings.value("ForceUnit").toInt();
		s_MomentUnit  = settings.value("MomentUnit").toInt();
		s_BackgroundColor.setRed(settings.value("BackgroundColorRed").toInt());
		s_BackgroundColor.setGreen(settings.value("BackgroundColorGreen").toInt());
		s_BackgroundColor.setBlue(settings.value("BackgroundColorBlue").toInt());
		s_TextColor.setRed(settings.value("TextColorRed").toInt());
		s_TextColor.setGreen(settings.value("TextColorGreen").toInt());
		s_TextColor.setBlue(settings.value("TextColorBlue").toInt());

		s_TextFont = QFont(settings.value("TextFontFamily", "Courier").toString());
		s_TextFont.setPointSize(settings.value("TextFontPointSize", 10).toInt());
		s_TextFont.setItalic(settings.value("TextFontItalic", false).toBool());
		s_TextFont.setBold(settings.value("TextFontBold", false).toBool());
		s_TextFont.setStyleStrategy(QFont::OpenGLCompatible);

		switch(settings.value("ImageFormat").toInt())
		{
			case 0:
				m_ImageFormat = PNG;
				break;
			case 1:
				m_ImageFormat = JPEG;
				break;
			case 2:
				m_ImageFormat = BMP;
				break;
			default:
				m_ImageFormat = PNG;
				break;
		}

		m_bSaveOpps   = settings.value("SaveOpps").toBool();
		m_bSaveWOpps  = settings.value("SaveWOpps").toBool();

//		a = settings.value("RecentFileSize").toInt();
		QString RecentF,strange;
		m_RecentFiles.clear();
		int n=0;
		do
		{
			RecentF = QString("RecentFile_%1").arg(n);
			strange = settings.value(RecentF).toString();
			if(strange.length())
			{
				m_RecentFiles.append(strange);
				n++;
			}
			else break;
		}while(n<MAXRECENTFILES);

		s_bReverseZoom  = settings.value("ReverseZoom", false).toBool();
		s_bAlphaChannel = settings.value("AlphaChannel", true).toBool();

		DisplaySettingsDlg::s_bStyleSheets  = settings.value("ShowStyleSheets", false).toBool();
		DisplaySettingsDlg::s_StyleSheetName = settings.value("StyleSheetName", "").toString();
	}

	return true;
}


MainFrame* MainFrame::self() {
    if (!_self) {
        _self = new MainFrame(0L, 0L);
    }
    return _self;
}


enumApp MainFrame::LoadXFLR5File(QString PathName)
{
	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = tr("Could not read the file\n")+PathName;
		QMessageBox::information(window(), tr("Info"), strange);
		return NOAPP;
	}
	QXDirect * pXDirect = (QXDirect*)m_pXDirect;
	QString end;
	//QFileInfo fileinfo(XFile);
	end = PathName.right(4);
	end = end.toLower();
	//QString dir = fileinfo.canonicalPath();

    PathName.replace(QDir::separator(), "/"); // Qt sometimes uses the windows \ separator

    int pos = PathName.lastIndexOf("/");
	if(pos>0) s_LastDirName = PathName.left(pos);
	if(end==".plr")
	{
		QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
		ar.setVersion(QDataStream::Qt_4_5);
#endif
		ar.setByteOrder(QDataStream::LittleEndian);
		Foil *pFoil = ReadPolarFile(ar);
		pXDirect->m_bPolarView = true;
		pXDirect->m_pCurPolar = NULL;
		pXDirect->m_pCurOpp   = NULL;
		s_pCurFoil = pXDirect->SetFoil(pFoil);
		pXDirect->SetPolar();

		XFile.close();

		AddRecentFile(PathName);
		SetSaveState(false);
		pXDirect->SetControls();
		return XFOILANALYSIS;
	}
	else
	{
		if(end==".dat")
		{
			QTextStream ar(&XFile);
			Foil *pFoil = ReadFoilFile(ar);
			XFile.close();

			if(pFoil)
			{
				AddFoil(pFoil);
				s_pCurFoil  = pFoil;
				pXDirect->m_pCurPolar = NULL;
				pXDirect->m_pCurOpp   = NULL;
				s_pCurFoil = pXDirect->SetFoil(pFoil);
				pXDirect->SetPolar();
				QAFoil *pAFoil= (QAFoil*)m_pAFoil;
				pAFoil->SelectFoil(pFoil);
				XFile.close();
				SetSaveState(false);
				AddRecentFile(PathName);
				pXDirect->SetControls();
				return XFOILANALYSIS;
			}
		}
		else
		{
			if(end==".wpa")
			{
				if(!s_bSaved)
				{
					QString strong = tr("Save the current project ?");
					int resp =  QMessageBox::question(this ,tr("Save"), strong,  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
					if(resp==QMessageBox::Cancel)
					{
						XFile.close();
						return NOAPP;
					}
					else if (resp==QMessageBox::Yes)
					{
						if(!SaveProject(m_FileName))
						{
							XFile.close();
							return NOAPP;
						}
					}
				}

				QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				DeleteProject();

				QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
				ar.setVersion(QDataStream::Qt_4_5);
#endif
				ar.setByteOrder(QDataStream::LittleEndian);
				if(SerializeProject(ar, false))
				{
					s_pCurFoil = pXDirect->SetFoil();
					UpdateFoils();
					UpdateView();
					QApplication::restoreOverrideCursor();
				}
				else
				{
					QApplication::restoreOverrideCursor();
					QMessageBox::warning(this,tr("Warning"), tr("Error reading the file")+"\n"+tr("Saved the valid part"));
//					DeleteProject();
				}

				AddRecentFile(PathName);
				SetSaveState(true);
				SetProjectName(PathName);

				XFile.close();


				pXDirect->SetControls();

                if(s_oaPlane.size() || s_oaWing.size()) return MIAREX;
				else                                    return XFOILANALYSIS;
			}
		}
	}
	XFile.close();

	return NOAPP;
}



void MainFrame::OnAFoil()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->StopAnimate();

	m_iApp = DIRECTDESIGN;
	m_pctrlMiarexToolBar->hide();
	m_pctrlStabViewWidget->hide();
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
	if(!s_pCurFoil) return;

    LinePickerDlg dlg(this);
	dlg.InitDialog(s_pCurFoil->m_nFoilStyle, s_pCurFoil->m_nFoilWidth, s_pCurFoil->m_FoilColor);

	if(QDialog::Accepted==dlg.exec())
	{
		s_pCurFoil->m_FoilColor  = dlg.GetColor();
		s_pCurFoil->m_nFoilStyle = dlg.GetStyle();
		s_pCurFoil->m_nFoilWidth = dlg.GetWidth();
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->m_BufferFoil.m_FoilColor  = s_pCurFoil->m_FoilColor;
		pXDirect->m_BufferFoil.m_nFoilStyle = s_pCurFoil->m_nFoilStyle;
		pXDirect->m_BufferFoil.m_nFoilWidth = s_pCurFoil->m_nFoilWidth;
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
		default:
			pGraph = NULL;
	}
	if(!pGraph) return;

	QString FileName;

	pGraph->GetGraphName(FileName);
	FileName = QFileDialog::getSaveFileName(this, tr("Export Graph"), m_ExportLastDirName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&m_GraphExportFilter);
	if(!FileName.length()) return;

	int pos;
	enumTextFileType type;
	pos = FileName.lastIndexOf("/");
	if(pos>0) m_ExportLastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
	pos = FileName.indexOf(".csv");
	if(pos>0) type=CSV; else type=TXT;

	pGraph->ExportToFile(XFile, type);
}


void MainFrame::OnGuidelines()
{

#ifdef Q_WS_MAC

     CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
     CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                            kCFURLPOSIXPathStyle);
     QString bundlePath(CFStringGetCStringPtr(macPath,
                                            CFStringGetSystemEncoding()));
     QFileInfo fileInfo(bundlePath);
     QDir dir = fileInfo.dir();
     QString FileName = dir.absoluteFilePath("Guidelines.pdf");
     CFRelease(appUrlRef);
     CFRelease(macPath);


#endif
#ifdef Q_OS_WIN
	QDir dir(qApp->applicationDirPath());
	QString FileName = dir.canonicalPath() + "/Guidelines.pdf" ;
#endif
#ifdef Q_OS_LINUX
	QDir dir("/usr/share/xflr5");
	QString FileName = dir.canonicalPath() + "/Guidelines.pdf" ;
#endif
	QDesktopServices::openUrl(QUrl::fromLocalFile(FileName));
}



void MainFrame::OnInsertProject()
{
	QString PathName;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	PathName = QFileDialog::getOpenFileName(this, tr("Open File"),
											s_LastDirName,
											tr("Project file (*.wpa)"));
	if(!PathName.length())		return;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) s_LastDirName = PathName.left(pos);


	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = tr("Could not read the file\n") + PathName;
		QMessageBox::information(window(), tr("Warning"), strange);
		return;
	}

	QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
	ar.setVersion(QDataStream::Qt_4_5);
#endif
	ar.setByteOrder(QDataStream::LittleEndian);

	SerializeProject(ar, false);

	SetSaveState(false);

	if(m_iApp == MIAREX)
	{
		UpdateUFOs();
		pMiarex->SetUFO();

		if(pMiarex->m_iView==WPOLARVIEW)    pMiarex->CreateWPolarCurves();
		else if(pMiarex->m_iView==WOPPVIEW) pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==WCPVIEW)  pMiarex->CreateCpCurves();
	}
	else if(m_iApp == XFOILANALYSIS)
	{
		if(pXDirect->m_bPolarView) pXDirect->CreatePolarCurves();
		else		         pXDirect->CreateOppCurves();
		UpdateFoils();
	}
	else if(m_iApp == DIRECTDESIGN)
	{
		pAFoil->FillFoilTable();
		pAFoil->SelectFoil();
	}
	UpdateView();
}



void MainFrame::OnLanguage()
{
    TranslatorDlg tDlg(this);
    tDlg.InitDialog();
    if(tDlg.exec()==QDialog::Accepted)
	{
	}
}




void MainFrame::OnLoadFile()
{
	QStringList PathNames;
	QString PathName;
	enumApp App  = NOAPP;
	bool warn_non_airfoil_multiload = false;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	PathNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
												s_LastDirName,
												tr("XFLR5 file (*.dat *.plr *.wpa)"));
	if(!PathNames.size()) return;
	if(PathNames.size() > 1)
	{
		for (int i=0; i<PathNames.size(); i++)
		{
			PathName = PathNames.at(i);
			if (PathName.endsWith(".dat"))
			{
				App = LoadXFLR5File(PathName);
			} else {
				warn_non_airfoil_multiload = true;
			}
		}
		if (warn_non_airfoil_multiload) {
			QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Multiple file loading only available for airfoil files.\nNon *.dat files will be ignored."));
		}
    }
    else
    {
		PathName = PathNames.at(0);
		if(!PathName.length()) return;


        PathName.replace(QDir::separator(), "/"); // Qt sometimes uses the windows \ separator

		int pos = PathName.lastIndexOf("/");
		if(pos>0) s_LastDirName = PathName.left(pos);

		App = LoadXFLR5File(PathName);
	}
	if(m_iApp==NOAPP)
	{
		m_iApp = App;
		QString strange = PathName.right(4);
		strange = strange.toLower();
		if(strange == ".dat" || strange==".plr") OnXDirect();
        else if(strange==".wpa" && (s_oaWing.size() || s_oaPlane.size())) OnMiarex();
		else OnXDirect();
	}

	if(App==0)
	{
	}
	else if(m_iApp==XFOILANALYSIS)
	{
        if(s_oaPolar.size())
		{
			if(pXDirect->m_bPolarView) pXDirect->CreatePolarCurves();
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
		pMiarex->SetControls();
		UpdateView();
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		QAFoil *pAFoil = (QAFoil*)m_pAFoil;
		pAFoil->SetParams();
		pAFoil->SelectFoil(s_pCurFoil);
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
	QString FileName = QDir::tempPath() + "/XFLR5.log";
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

//	pMiarex->SetControls();
	pMiarex->SetUFO();
	pMiarex->m_bArcball = false;
	UpdateUFOs();

	SetMenus();
	SetCentralWidget();
//	pMiarex->SetControls();
	UpdateView();
}


void MainFrame::OnNewProject()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;
	if(!s_bSaved)
	{
		int resp = QMessageBox::question(this, tr("Question"), tr("Save the current project ?"),
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
				DeleteProject();
				statusBar()->showMessage(tr("The project ") + s_ProjectName + tr(" has been saved"));
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
			if(!pXDirect->m_bPolarView) pGraph = pXDirect->m_pCpGraph;
			else pGraph = pXDirect->m_pCurGraph;
			break;
		}
		case INVERSEDESIGN:
		{
			QXInverse *pXInverse = (QXInverse*)m_pXInverse;
			pGraph = &pXInverse->m_QGraph;
			pXInverse->ReleaseZoom();
			break;
		}
		default:
		pGraph = NULL;
	}

	if(!pGraph) return;

	pGraph->SetAuto(true);
	UpdateView();
}



void MainFrame::OnRenameCurFoil()
{
	if(!s_pCurFoil) return;
	RenameFoil(s_pCurFoil);
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->SetFoil(s_pCurFoil);
	UpdateFoils();
	UpdateView();
}


void MainFrame::OnResetSettings()
{
	int resp = QMessageBox::question(this, tr("Default Settings"), tr("Are you sure you want to reset the default settings ?"),
									 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
	if(resp == QMessageBox::Yes)
	{
		QMessageBox::warning(this,tr("Default Settings"), tr("The settings will be reset at the next session"));
#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"sourceforge.net","xflr5");
#else
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif
		settings.clear();
		s_LastDirName = QDir::homePath();
		// do not save on exit
		m_bSaveSettings = false;
	}
}


void MainFrame::OnRestoreToolbars()
{
	if(m_iApp==XFOILANALYSIS)
	{
		m_pctrlXInverseToolBar->hide();
		m_pctrlAFoilToolBar->hide();
		m_pctrlMiarexToolBar->hide();
		m_pctrlStabViewWidget->hide();
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
		m_pctrlStabViewWidget->hide();

		m_pctrlAFoilToolBar->show();
		m_pctrlAFoilWidget->show();
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		m_pctrlAFoilToolBar->hide();
		m_pctrlMiarexToolBar->hide();
		m_pctrlXDirectToolBar->hide();
		m_pctrl3DScalesWidget->hide();
		m_pctrlStabViewWidget->hide();

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
		QMiarex *pMiarex = (QMiarex*)m_pMiarex;
		if(pMiarex->m_iView==WSTABVIEW) m_pctrlStabViewWidget->show();
		else                            m_pctrlStabViewWidget->hide();
	}
}


void MainFrame::OnSaveOptions()
{
    SaveOptionsDlg soDlg(this);
    soDlg.InitDialog(m_bSaveOpps, m_bSaveWOpps);
    if(soDlg.exec()==QDialog::Accepted)
	{
        m_bSaveOpps  = soDlg.m_bOpps;
        m_bSaveWOpps = soDlg.m_bWOpps;
	}
}


void MainFrame::OnSaveProject()
{
	if (!s_ProjectName.length() || s_ProjectName=="*")
	{
		OnSaveProjectAs();
		return;
	}
	if(SaveProject(m_FileName))
	{
		SetSaveState(true);
		statusBar()->showMessage(tr("The project ") + s_ProjectName + tr(" has been saved"));
	}
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;
	pMiarex->UpdateView();
}



bool MainFrame::OnSaveProjectAs()
{
	if(SaveProject())
	{
		SetProjectName(m_FileName);
		AddRecentFile(m_FileName);
		statusBar()->showMessage(tr("The project ") + s_ProjectName + tr(" has been saved"));
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
	if(pMiarex->m_pCurPlane)     strong = pMiarex->m_pCurPlane->PlaneName();
	else if(pMiarex->m_pCurWing) strong = pMiarex->m_pCurWing->m_WingName;
	else
	{
		QMessageBox::warning(this, tr("Warning"), tr("Nothing to save"));
		return ;
	}

	QString PathName;
	QString Filter = ".wpa";
	QString FileName = strong;

	PathName = QFileDialog::getSaveFileName(this, tr("Save the Project File"),
											s_LastDirName+"/"+FileName,
											tr("XFLR5 Project File (*.wpa)"), &Filter);
	if(!PathName.length()) return;//nothing more to do
	int pos = PathName.indexOf(".wpa", Qt::CaseInsensitive);
	if(pos<0) PathName += ".wpa";
    PathName.replace(QDir::separator(), "/"); // Qt sometimes uses the windows \ separator
    pos = PathName.lastIndexOf("/");
	if(pos>0) s_LastDirName = PathName.left(pos);


	QFile fp(PathName);

	if (!fp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
		return;
	}

	QDataStream ar(&fp);
#if QT_VERSION >= 0x040500
	ar.setVersion(QDataStream::Qt_4_5);
#endif
	ar.setByteOrder(QDataStream::LittleEndian);
	SerializeUFOProject(ar);
	fp.close();

}

void MainFrame::OnSaveViewToImageFile()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->m_bArcball = false;

	QSize sz(m_p2DWidget->geometry().width(), m_p2DWidget->geometry().height());
	QImage img(sz, QImage::Format_RGB32);
	QPainter painter(&img);
	QString FileName, Filter;

	switch(m_ImageFormat)
	{
		case PNG :
		{
			Filter = "Portable Network Graphics (*.png)";
			break;
		}
		case JPEG :
		{
			Filter = "JPEG (*.jpg)";
			break;
		}
		case BMP :
		{
			Filter = "Windows Bitmap (*.bmp)";
			break;
		}
	}

	FileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
											m_ImageDirName,
											"Portable Network Graphics (*.png);;JPEG (*.jpg);;Windows Bitmap (*.bmp)",
											&Filter);

	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) m_ImageDirName = FileName.left(pos);

	if(Filter == "Portable Network Graphics (*.png)")
	{
		if(FileName.right(4)!=".png") FileName+= ".png";
		m_ImageFormat = PNG;
	}
	else if(Filter == "JPEG (*.jpg)")
	{
		if(FileName.right(4)!=".jpg") FileName+= ".jpg";
		m_ImageFormat = JPEG;
	}
	else if(Filter == "Windows Bitmap (*.bmp)")
	{
		if(FileName.right(4)!=".bmp") FileName+= ".bmp";
		m_ImageFormat = BMP;
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

			if(pMiarex->m_iView==W3DVIEW)
			{
				pMiarex->UpdateView();
				pMiarex->SnapClient(FileName);
				return;
			}
			else pMiarex->PaintView(painter);
			break;
		}
		default:
			break;
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
//	int sel = m_pctrlUFO->currentIndex();
	if (i >=0) strong = m_pctrlUFO->itemText(i);
	pMiarex->SetUFO(strong);

	m_iApp = MIAREX;
//	UpdateWPolars();
//	UpdateWOpps();
//	pMiarex->SetWPlr(false);
	pMiarex->m_bIs2DScaleSet = false;
	pMiarex->Set2DScale();
	pMiarex->SetControls();
	pMiarex->UpdateView();
}


void MainFrame::OnSelChangeWPolar(int i)
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	QString strong;
//	int sel = m_pctrlWPolar->currentIndex();
	if (i>=0) strong = m_pctrlWPolar->itemText(i);
	m_iApp = MIAREX;
	pMiarex->SetWPolar(false, strong);
//	pMiarex->SetWingOpp(true);
	pMiarex->SetControls();
	pMiarex->UpdateView();
}


void MainFrame::OnSelChangeWOpp(int i)
{
	QString strong;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	pMiarex->StopAnimate();

	// Gets the new selected WOpp name and notifies Miarex
	if(!m_pctrlWOpp->count())
	{
		pMiarex->m_pCurWOpp = NULL;
		if (pMiarex->m_iView==WOPPVIEW)    pMiarex->CreateWOppCurves();
		else if(pMiarex->m_iView==WCPVIEW) pMiarex->CreateCpCurves();
		pMiarex->UpdateView();
		return;
	}

	if (i>=0) strong = m_pctrlWOpp->itemText(i);
	else
	{
		m_pctrlWOpp->setCurrentIndex(0);
        strong = m_pctrlWOpp->itemText(0).trimmed();
	}
	if(strong.length())
	{
		bool bOK;
		double x = locale().toDouble(strong, &bOK);
		if(bOK)
		{
			m_iApp = MIAREX;
			pMiarex->SetWingOpp(false, x);
			pMiarex->UpdateView();
		}
		else
		{
			QMessageBox::warning(window(), tr("Warning"), tr("Unidentified Operating Point"));
			pMiarex->m_pCurWOpp = NULL;
			pMiarex->SetWingOpp(true);
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

	s_pCurFoil = foil(strong);
	pXDirect->SetFoil(s_pCurFoil);
	pXDirect->SetPolar();
	m_iApp = XFOILANALYSIS;
	UpdatePolars();
	pXDirect->SetControls();
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
	pXDirect->SetControls();
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

	DisplaySettingsDlg DSdlg(this);

	DSdlg.m_BackgroundColor = s_BackgroundColor;
	DSdlg.m_TextColor       = s_TextColor;
	DSdlg.m_TextFont        = s_TextFont;
	DSdlg.m_pRefGraph       = &m_RefGraph;
	DSdlg.m_bReverseZoom    = s_bReverseZoom;
	DSdlg.m_bAlphaChannel   = s_bAlphaChannel;
	DSdlg.InitDialog();

	if(DSdlg.exec() ==QDialog::Accepted)
	{
		s_BackgroundColor = DSdlg.m_BackgroundColor;
		s_TextColor       = DSdlg.m_TextColor;
		s_TextFont        = DSdlg.m_TextFont;
		s_bReverseZoom    = DSdlg.m_pctrlReverseZoom->isChecked();
		s_bAlphaChannel   = DSdlg.m_pctrlAlphaChannel->isChecked();
		pMiarex->m_bResetglGeom = true;
		pMiarex->m_bResetglBody = true;
		pMiarex->m_bResetglLegend = true;

		if(DSdlg.m_bIsGraphModified)
		{
			SetGraphSettings(&m_RefGraph);
		}
	}
//    m_DlgPos = m_DisplaySettingsDlg->pos();

	pXDirect->m_pCpGraph->SetInverted(true);
	pMiarex->m_CpGraph.SetInverted(true);
	UpdateView();
}


void MainFrame::OnUnits()
{
    UnitsDlg uDlg(this);
    uDlg.m_Length = s_LengthUnit;
    uDlg.m_Area   = s_AreaUnit;
    uDlg.m_Weight = s_WeightUnit;
    uDlg.m_Speed  = s_SpeedUnit;
    uDlg.m_Force  = s_ForceUnit;
    uDlg.m_Moment = s_MomentUnit;
    uDlg.InitDialog();

    if(uDlg.exec()==QDialog::Accepted)
	{
        s_LengthUnit = uDlg.m_Length;
        s_AreaUnit   = uDlg.m_Area;
        s_WeightUnit = uDlg.m_Weight;
        s_SpeedUnit  = uDlg.m_Speed;
        s_ForceUnit  = uDlg.m_Force;
        s_MomentUnit = uDlg.m_Moment;

		SetUnits(s_LengthUnit, s_AreaUnit, s_SpeedUnit, s_WeightUnit, s_ForceUnit, s_MomentUnit,
		s_mtoUnit, s_m2toUnit, s_mstoUnit, s_kgtoUnit, s_NtoUnit, s_NmtoUnit);

		SetSaveState(false);

		if(m_iApp==MIAREX)
		{
			QMiarex *pMiarex= (QMiarex*)m_pMiarex;
			pMiarex->UpdateUnits();
		}
	}
//    m_DlgPos = m_UnitsDlg->pos();
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
	m_pctrlStabViewWidget->hide();
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
	pXDirect->SetControls();
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
	m_pctrlStabViewWidget->hide();
	m_pctrlAFoilToolBar->hide();
	m_pctrlXDirectToolBar->hide();
	m_pctrlXInverseToolBar->show();

	m_pctrlAFoilWidget->hide();
	m_pctrlMiarexWidget->hide();
	m_pctrlXDirectWidget->hide();
	m_pctrlXInverseWidget->show();

	SetCentralWidget();
//	pXInverse->m_bFullInverse = true;
	SetMenus();
	pXInverse->SetParams();
	pXInverse->UpdateView();
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
	m_pctrlStabViewWidget->hide();
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
	pXInverse->UpdateView();
}


void MainFrame::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action) return;

	QXDirect *pXDirect = (QXDirect*) m_pXDirect;
	QMiarex *pMiarex = (QMiarex*) m_pMiarex;
	
	enumApp App = LoadXFLR5File(action->data().toString());
	if(m_iApp==NOAPP) m_iApp = App;

	if(App==NOAPP)
	{
		m_iApp = App;
		QString FileName = action->data().toString();
		m_RecentFiles.removeAll(FileName);
		updateRecentFileActions();
	}

	else if(m_iApp==XFOILANALYSIS)
	{
        if(s_oaPolar.size())
		{
			if(pXDirect->m_bPolarView) pXDirect->CreatePolarCurves();
            else                       pXDirect->CreateOppCurves();
		}
		OnXDirect();
		UpdateFoils();
		UpdateView();

	}
	else if(m_iApp==MIAREX)
	{
		pMiarex->m_bIs2DScaleSet = false;
		pMiarex->Set2DScale();
		OnMiarex();
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


Foil* MainFrame::ReadFoilFile(QTextStream &in)
{
	QString Strong;
	QString tempStr;
	QString FoilName;

	Foil* pFoil = NULL;
	int pos, res, i, ip;
	pos = 0;
	double x,y, z,area;
	bool bRead;


	pFoil = new Foil();
	if(!pFoil)	return NULL;

	while(tempStr.length()==0 && !in.atEnd())
	{
		FoilName = Strong;
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);
		// ignore everything after # (including #)
		if(pos>0)Strong.truncate(pos);
		tempStr = Strong;
		tempStr.remove(" ");
	}

	if(!in.atEnd())
	{
		// FoilName contains the last comment
		ReadValues(Strong, res,x,y,z);
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
	do
	{
		Strong = in.readLine();
		pos = Strong.indexOf("#",0);
		// ignore everything after # (including #)
		if(pos>0)Strong.truncate(pos);
		tempStr = Strong;
		tempStr.remove(" ");
		if (!Strong.isNull() && bRead && tempStr.length())
		{
			ReadValues(Strong, res, x,y,z);
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
	}while (bRead && !Strong.isNull());

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


/**
 *Reads a Foil and its related Polar objects from a binary stream associated to a .plr file.
 * @param ar the binary stream
 * @return the pointer to the Foil object which has been created, or NULL if failure.
 */
Foil * MainFrame::ReadPolarFile(QDataStream &ar)
{
	Foil* pFoil = NULL;
	Polar *pPolar = NULL;
	Polar * pOldPolar;
	int i, n, l;

	ar >> n;

	if(n<100000)
	{
		//old format
		QMessageBox::warning(window(), tr("Warning"), tr("Obsolete format, cannot read"));
		return NULL;
	}
	else if (n >=100000)
	{
		//new format XFLR5 v1.99+
		//first read all available foils
		ar>>n;
		for (i=0;i<n; i++)
		{
			pFoil = new Foil();
			if (!pFoil->Serialize(ar, false))
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
			pPolar = new Polar();

			pPolar->m_Color = QColor(56,128,188,127);
			;
			if (!pPolar->Serialize(ar, false))
			{
				delete pPolar;
				return pFoil;
			}
            for (l=0; l<s_oaPolar.size(); l++)
			{
                pOldPolar = (Polar*)s_oaPolar[l];
				if (pOldPolar->m_FoilName == pPolar->m_FoilName &&
					pOldPolar->m_PlrName  == pPolar->m_PlrName)
				{
					//just overwrite...
                    s_oaPolar.removeAt(l);
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
			if (pOpPoint->m_strFoilName == s_pCurFoil->m_FoilName &&
				pOpPoint->m_strPlrName == pXDirect->m_pCurPolar->m_PlrName)
			{
				m_oaOpp.removeAt(i);
				delete pOpPoint;
			}
		}
	}

	pXDirect->m_pCurOpp = NULL;
}




void MainFrame::RenameFoil(Foil *pFoil)
{
	if(!pFoil) return;

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QString strong;
	bool bNotFound = true;
	int i,k,l;
	Foil*pOldFoil;
	Polar * pPolar;
	OpPoint * pOpPoint;

	QString OldName = pFoil->m_FoilName;

	while(bNotFound)
	{
		QStringList NameList;
        for(k=0; k<s_oaFoil.size(); k++)
		{
            pOldFoil = (Foil*)s_oaFoil.at(k);
			NameList.append(pOldFoil->m_FoilName);
		}

        RenameDlg renDlg(this);
        renDlg.m_pstrArray = & NameList;
        renDlg.m_strQuestion = tr("Enter the foil's new name");
        renDlg.m_strName = OldName;
		bool bExists = false;
        renDlg.InitDialog();
        int resp = renDlg.exec();

        strong = renDlg.m_strName;

		if(QDialog::Accepted == resp)
		{
			if (strong==OldName)
			{
				// do nothing
				bNotFound = false;
			}
			else
			{
                for (l=0; l<s_oaFoil.size(); l++)
				{
                    pOldFoil = (Foil*)s_oaFoil.at(l);
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
					pFoil->m_FoilName = strong;
					bool bInserted = false;
                    for(i=0;i<s_oaFoil.size();i++)
					{
                        pOldFoil = (Foil*)s_oaFoil.at(i);
						if(pOldFoil == pFoil)
						{
                            s_oaFoil.removeAt(i);
							//and re-insert
                            for(l=0;l<s_oaFoil.size();l++)
							{
                                pOldFoil = (Foil*)s_oaFoil.at(l);
								if(pFoil->m_FoilName.compare(pOldFoil->m_FoilName, Qt::CaseInsensitive)<0)
								{
									//then insert before
                                    s_oaFoil.insert(l, pFoil);
									bInserted = true;
									break;
								}
							}
                            if(!bInserted)	s_oaFoil.append(pFoil);
							break;
						}
					}
                    for (i=0; i<s_oaPolar.size(); i++)
					{
                        pPolar = (Polar*)s_oaPolar.at(i);
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
                for (l=s_oaFoil.size()-1;l>=0; l--)
				{
                    pOldFoil = (Foil*)s_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong)
					{
						if(s_pCurFoil == pOldFoil)           s_pCurFoil = NULL;
						if(s_pCurFoil == pOldFoil) s_pCurFoil = NULL;
                        s_oaFoil.removeAt(l);
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
				Polar * pPolar;
                for (l=s_oaPolar.size()-1;l>=0;l--)
				{
                    pPolar = (Polar*) s_oaPolar.at(l);
					if (pPolar->m_FoilName == strong)
					{
						if(pXDirect->m_pCurPolar == pPolar) pXDirect->m_pCurPolar = NULL;
                        s_oaPolar.removeAt(l);
						delete pPolar;
					}
				}
				// finally add to array
				pFoil->m_FoilName = strong;
                for (i=0; i<s_oaPolar.size(); i++)
				{
                    pPolar = (Polar*)s_oaPolar.at(i);
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
}



bool MainFrame::SaveProject(QString PathName)
{
	QString Filter = "XFLR5 v6 Project File (*.wpa)";
	QString FileName = s_ProjectName;

	if(!PathName.length())
	{
		if(FileName.right(1)=="*") 	FileName = FileName.left(FileName.length()-1);

        PathName = QFileDialog::getSaveFileName(this, tr("Save the Project File"),
												s_LastDirName+"/"+FileName,
                                                tr("XFLR5 v6 Project File (*.wpa)"),
												&Filter);

		if(!PathName.length()) return false;//nothing more to do

        int pos = PathName.indexOf(".wpa", Qt::CaseInsensitive);
		if(pos<0) PathName += ".wpa";

        PathName.replace(QDir::separator(), "/"); // Qt sometimes uses the windows \ separator

        pos = PathName.lastIndexOf("/");
		if(pos>0) s_LastDirName = PathName.left(pos);
	}

	QFile fp(PathName);

	if (!fp.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(window(), tr("Warning"), tr("Could not open the file for writing"));
		return false;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QDataStream ar(&fp);
#if QT_VERSION >= 0x040500
	ar.setVersion(QDataStream::Qt_4_5);
#endif
	ar.setByteOrder(QDataStream::LittleEndian);

	SerializeProject(ar,true);
	m_FileName = PathName;
	fp.close();

	SaveSettings();
	QApplication::restoreOverrideCursor();
	return true;
}


void MainFrame::SaveSettings()
{
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;

	if(!m_bSaveSettings) return;
#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"sourceforge.net","xflr5");
#else
	QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif
	settings.beginGroup("MainFrame");
	{
		settings.setValue("SettingsFormat", SETTINGSFORMAT);
		settings.setValue("FrameGeometryx", frameGeometry().x());
		settings.setValue("FrameGeometryy", frameGeometry().y());
		settings.setValue("SizeWidth", size().width());
		settings.setValue("SizeHeight", size().height());
		settings.setValue("SizeMaximized", isMaximized());
		settings.setValue("StyleName", DisplaySettingsDlg::s_StyleName);
		settings.setValue("GraphExportFilter", m_GraphExportFilter);
		settings.setValue("Miarex_Float", m_pctrlMiarexWidget->isFloating());
		settings.setValue("XDirect_Float", m_pctrlXDirectWidget->isFloating());
		settings.setValue("AFoil_Float", m_pctrlAFoilWidget->isFloating());
		settings.setValue("XInverse_Float", m_pctrlXInverseWidget->isFloating());
		settings.setValue("StabView_Float", m_pctrlStabViewWidget->isFloating());
		settings.setValue("Miarex_x", m_pctrlMiarexWidget->frameGeometry().x());
		settings.setValue("Miarex_y", m_pctrlMiarexWidget->frameGeometry().y());
		settings.setValue("XDirect_x", m_pctrlXDirectWidget->frameGeometry().x());
		settings.setValue("XDirect_y", m_pctrlXDirectWidget->frameGeometry().y());
		settings.setValue("AFoil_x", m_pctrlAFoilWidget->frameGeometry().x());
		settings.setValue("AFoil_y", m_pctrlAFoilWidget->frameGeometry().y());
		settings.setValue("XInverse_x", m_pctrlXInverseWidget->frameGeometry().x());
		settings.setValue("XInverse_y", m_pctrlXInverseWidget->frameGeometry().y());
		settings.setValue("StabView_x", m_pctrlStabViewWidget->frameGeometry().x());
		settings.setValue("StabView_y", m_pctrlStabViewWidget->frameGeometry().y());
		settings.setValue("XDirectSize", m_pctrlXDirectWidget->size());
		settings.setValue("AFoilSize", m_pctrlAFoilWidget->size());
		settings.setValue("XInverseSize", m_pctrlXInverseWidget->size());
		settings.setValue("MiarexSize", m_pctrlMiarexWidget->size());
		settings.setValue("StabSize", m_pctrlStabViewWidget->size());
		settings.setValue("LastDirName", s_LastDirName);
		settings.setValue("ImageDirName", m_ImageDirName);
		settings.setValue("ExportLastDirName", m_ExportLastDirName);
		settings.setValue("LengthUnit", s_LengthUnit);
		settings.setValue("AreaUnit", s_AreaUnit);
		settings.setValue("WeightUnit", s_WeightUnit);
		settings.setValue("SpeedUnit", s_SpeedUnit);
		settings.setValue("ForceUnit", s_ForceUnit);
		settings.setValue("MomentUnit", s_MomentUnit);
		settings.setValue("BackgroundColorRed", s_BackgroundColor.red());
		settings.setValue("BackgroundColorGreen", s_BackgroundColor.green());
		settings.setValue("BackgroundColorBlue", s_BackgroundColor.blue());
		settings.setValue("TextColorRed", s_TextColor.red());
		settings.setValue("TextColorGreen", s_TextColor.green());
		settings.setValue("TextColorBlue", s_TextColor.blue());
		settings.setValue("TextFontFamily", s_TextFont.family());
		settings.setValue("TextFontPointSize", s_TextFont.pointSize());
		settings.setValue("TextFontItalic", s_TextFont.italic());
		settings.setValue("TextFontBold", s_TextFont.bold());

		settings.setValue("LanguageFilePath", s_LanguageFilePath);
		settings.setValue("ImageFormat", m_ImageFormat);
		settings.setValue("SaveOpps", m_bSaveOpps);
		settings.setValue("SaveWOpps", m_bSaveWOpps);
		settings.setValue("RecentFileSize", m_RecentFiles.size());
		settings.setValue("ShowStyleSheets", DisplaySettingsDlg::s_bStyleSheets);
		settings.setValue("ReverseZoom", s_bReverseZoom);
		settings.setValue("AlphaChannel", s_bAlphaChannel);
		settings.setValue("StyleSheetName", DisplaySettingsDlg::s_StyleSheetName);


		QString RecentF;
		for(int i=0; i<m_RecentFiles.size() && i<MAXRECENTFILES; i++)
		{
			RecentF = QString("RecentFile_%1").arg(i);
			if(m_RecentFiles[i].length()) settings.setValue(RecentF, m_RecentFiles.at(i));
			else                          settings.setValue(RecentF, "");
		}
		for(int i=m_RecentFiles.size(); i<MAXRECENTFILES; i++)
		{
			RecentF = QString("RecentFile_%1").arg(i);
			settings.setValue(RecentF, "");
		}
	}
	settings.endGroup();


	pAFoil->SaveSettings(&settings);
	pXDirect->SaveSettings(&settings);
	pMiarex->SaveSettings(&settings);
	pXInverse->SaveSettings(&settings);
    GL3DScales::SaveSettings(&settings);
    W3dPrefsDlg::SaveSettings(&settings);
	m_RefGraph.SaveSettings(&settings);
}


void MainFrame::SetCentralWidget()
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	if(m_iApp!=MIAREX)
	{
		m_pctrlCentralWidget->setCurrentIndex(0);
	}
	else if(m_iApp==MIAREX)
	{
		if(pMiarex->m_iView==WOPPVIEW || pMiarex->m_iView==WPOLARVIEW || pMiarex->m_iView==WCPVIEW)
			m_pctrlCentralWidget->setCurrentIndex(0);
		else if(pMiarex->m_iView==W3DVIEW)
			m_pctrlCentralWidget->setCurrentIndex(1);
		else if(pMiarex->m_iView==WSTABVIEW)
		{
			if(pMiarex->m_iStabilityView==STABTIMEVIEW ||
			   pMiarex->m_iStabilityView==STABPOLARVIEW  )   m_pctrlCentralWidget->setCurrentIndex(0);
			else                                             m_pctrlCentralWidget->setCurrentIndex(1);
		}

/*		if(m_pctrlCentralWidget->currentIndex()==0) pMiarex->Set2DScale();
		else                                        pMiarex->Set3DScale();*/
	}
}



void MainFrame::SelectOpPoint(OpPoint *pOpp)
{
	//Selects pOpp in the combobox and returns true
	//On error, selects the first and returns false
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	Polar *pCurPlr    = pXDirect->m_pCurPolar;
	if(!pOpp || !pCurPlr) return;

	QString strange;
	double val;
	bool bOK;

	for(int i=0; i<m_pctrlOpPoint->count(); i++)
	{
		strange = m_pctrlOpPoint->itemText(i);
		val = locale().toDouble(strange, &bOK);

		if(pCurPlr->m_PolarType != FIXEDAOAPOLAR)
		{
			if(bOK && qAbs(val-pOpp->Alpha)<0.001)
			{
				m_pctrlOpPoint->setCurrentIndex(i);
				break;
			}
		}
		else
		{
			if(bOK && qAbs(val-pOpp->Reynolds)<1.0)
			{
				m_pctrlOpPoint->setCurrentIndex(i);
				break;
			}
		}
	}
}


void MainFrame::SelectWOpp(WingOpp *pWingOpp)
{
    if(pWingOpp)
    {
        if(pWingOpp->m_WPolarType<FIXEDAOAPOLAR)        SelectWOpp(pWingOpp->m_Alpha);
        else if(pWingOpp->m_WPolarType==FIXEDAOAPOLAR)  SelectWOpp(pWingOpp->m_QInf);
        else if(pWingOpp->m_WPolarType==STABILITYPOLAR) SelectWOpp(pWingOpp->m_Ctrl);
    }
    else SelectWOpp();
}


void MainFrame::SelectWOpp(PlaneOpp *pPlaneOpp)
{
    if(pPlaneOpp)
    {
        if(pPlaneOpp->m_WPolarType<FIXEDAOAPOLAR)        SelectWOpp(pPlaneOpp->m_Alpha);
        else if(pPlaneOpp->m_WPolarType==FIXEDAOAPOLAR)  SelectWOpp(pPlaneOpp->m_QInf);
        else if(pPlaneOpp->m_WPolarType==STABILITYPOLAR) SelectWOpp(pPlaneOpp->m_Ctrl);
    }
    else SelectWOpp();
}


void MainFrame::SelectWOpp(double x)
{
	//Selects a pOpp in the combobox
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	WPolar *pCurWPlr    = pMiarex->m_pCurWPolar;
	double val;
	bool bOK;
	QString strange;

	m_pctrlWOpp->blockSignals(true);
//	x = (double)qRound(x*100.0)/100.0;

	for(int i=0; i<m_pctrlWOpp->count(); i++)
	{
		strange = m_pctrlWOpp->itemText(i);
		val = locale().toDouble(strange, &bOK);

		if(pCurWPlr->m_WPolarType<FIXEDAOAPOLAR)
		{
			if(bOK && qAbs(val-x)<0.001)
			{
				m_pctrlWOpp->setCurrentIndex(i);
				break;
			}
		}
		else if(pCurWPlr->m_WPolarType==FIXEDAOAPOLAR)
		{
			if(bOK && qAbs(val-x)<1.0)
			{
				m_pctrlWOpp->setCurrentIndex(i);
				break;
			}
		}
		else if(pCurWPlr->m_WPolarType==STABILITYPOLAR)
		{
			if(bOK && qAbs(val-x)<.001)
			{
				m_pctrlWOpp->setCurrentIndex(i);
				break;
			}
		}
	}
	m_pctrlWOpp->blockSignals(false);
}



bool MainFrame::SerializeUFOProject(QDataStream &ar)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	if(!pMiarex->m_pCurWing)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Nothing to save"));
		return false;
	}
	QString UFOName;
	//saves only the UFO with its associated plrs, foils and other results
	Plane *pPlane   = pMiarex->m_pCurPlane;
	Wing *pWing     = pMiarex->m_pCurWing;
	Wing *pStab     = pMiarex->m_pWingList[2];
	Wing *pFin      = pMiarex->m_pWingList[3];
	WPolar *pWPolar = NULL;
//	CWOpp *pWOpp     = NULL;
	Foil *pFoil  = NULL;
	Polar * pPolar  = NULL;

	if(pPlane) UFOName = pPlane->PlaneName();
	else       UFOName = pWing->m_WingName;

	int i,j;

	// storing code
	ar << 100014;
	// 100014 : removed Point Foil
	// 100013 : Added CoG serialization
	// 100012 : Added sideslip
	// 100011 : Added Body serialization
	// 100010 : Converted to I.S. units
	// 100009 : added serialization of opps in numbered format
	// 100008 : Added m_WPolarDlg.m_bTiltedGeom, m_WPolarDlg.m_bWakeRollUp
	// 100006 : version 2.99Beta001 format
	// 100005 : version 2.00 format
	//Archive format introduced in Beta V22
	// Write units //added in Beta v16
	// Save analysis settings // added in Beta v22
	ar << s_LengthUnit;
	ar << s_AreaUnit;
	ar << s_WeightUnit;
	ar << s_SpeedUnit;
	ar << s_ForceUnit;
	ar << s_MomentUnit;

	if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
	else if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
	else if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
	else if(WPolarDlg::s_WPolar.m_WPolarType==STABILITYPOLAR)  ar<<7;
	else ar << 0;

	ar << (float)WPolarDlg::s_WPolar.m_Mass;
	ar << (float)WPolarDlg::s_WPolar.m_QInf;
	ar << (float)WPolarDlg::s_WPolar.m_CoG.x;
	ar << (float)WPolarDlg::s_WPolar.m_CoG.y;
	ar << (float)WPolarDlg::s_WPolar.m_CoG.z;
	ar << (float)WPolarDlg::s_WPolar.m_Density;
	ar << (float)WPolarDlg::s_WPolar.m_Viscosity;
	ar << (float)WPolarDlg::s_WPolar.m_ASpec;
	ar << (float)WPolarDlg::s_WPolar.m_Beta;

	if(WPolarDlg::s_WPolar.m_AnalysisMethod==LLTMETHOD)        ar << 1;
	else if(WPolarDlg::s_WPolar.m_AnalysisMethod==VLMMETHOD)   ar << 2;
	else if(WPolarDlg::s_WPolar.m_AnalysisMethod==PANELMETHOD) ar << 3;


	if (pMiarex->m_bVLM1)  ar << 1;
	else				 ar << 0;
//		if (WPolarDlg::m_bMiddle) ar << 1; else ar << 0;
	ar <<1;
	if (WPolarDlg::s_WPolar.m_bTiltedGeom) ar << 1;
	else                                               ar << 0;
	if (WPolarDlg::s_WPolar.m_bWakeRollUp) ar << 1;
	else                                               ar << 0;

	// Store the wing, if any
	if(!pPlane)
	{
		ar << 1;//just one wing
		pWing->SerializeWing(ar, true);
	}
	else ar <<0;

	// now store all the WPolars associated to the UFO
	//start by listing the number
	int size = 0;
	for (i=0; i<m_oaWPolar.size();i++)
	{
		pWPolar = (WPolar*)m_oaWPolar.at(i);
		if(pWPolar->m_UFOName == UFOName) size++;
	}

	ar << size;
	for (i=0; i<m_oaWPolar.size();i++)
	{
		pWPolar = (WPolar*)m_oaWPolar.at(i);
		if(pWPolar->m_UFOName == UFOName) pWPolar->SerializeWPlr(ar, true);
	}

	ar << 0;//no WOpps

	// then the foils,  polars and Opps
	//list the foils to be saved
	QStringList FoilList;
	bool bFound = false;
	if(pWing)
	{
		for (i=0; i<pWing->NWingSection(); i++)
		{
			pFoil = foil(pWing->RightFoil(i));
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
			for (i=0; i<pWing->NWingSection(); i++)
			{
				pFoil = foil(pWing->LeftFoil(i));
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
		for (i=0; i<pStab->NWingSection(); i++)
		{
			pFoil = foil(pStab->RightFoil(i));
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
			for (i=0; i<pStab->NWingSection(); i++)
			{
				pFoil = foil(pStab->LeftFoil(i));
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
		for (i=0; i<pFin->NWingSection(); i++)
		{
			pFoil = foil(pFin->RightFoil(i));
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
			for (i=0; i<pFin->NWingSection(); i++)
			{
				pFoil = foil(pFin->LeftFoil(i));
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
	ar << FoilList.size();
	for (i=0; i<FoilList.size(); i++)
	{
		pFoil = foil(FoilList[i]);
		pFoil->Serialize(ar, true);
	}
	int n = 0;
	for (i=0; i<FoilList.size(); i++)
	{
		pFoil = foil(FoilList[i]);
		//count polars associated to the foil
        for (int i=0; i<s_oaPolar.size();i++)
		{
            pPolar = (Polar*)s_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName)
				n++;
		}
	}
	//then write polars
	ar << n;
	for (i=0; i<FoilList.size(); i++)
	{
		pFoil = foil(FoilList[i]);
        for (j=0; j<s_oaPolar.size();j++)
		{
            pPolar = (Polar*)s_oaPolar.at(j);
			if (pPolar->m_FoilName == pFoil->m_FoilName)
			{
				pPolar->Serialize(ar, true);
			}
		}
	}

	ar<<0;//no Opps

	// next the bodies

	if(pPlane && pPlane->body())
	{
		ar << 1;
		pPlane->body()->SerializeBody(ar, true);
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
//	pAFoil->m_pPF->Serialize(ar, true);

	return true;
}


bool MainFrame::SerializeProject(QDataStream &ar, bool bIsStoring)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	Wing *pWing     = NULL;
	WPolar *pWPolar = NULL;
	WingOpp *pWOpp     = NULL;
	PlaneOpp *pPOpp     = NULL;
	Plane *pPlane   = NULL;
	Body *pBody     = NULL;
	Polar *pPolar   = NULL;
	Foil *pFoil     = NULL;

	QString str;
	int i, n, j, k;
	float f;

	if (bIsStoring)
	{
		// storing code
		ar << 100014;
		// 100014 : Removed Point Foil
		// 100013 : Added CoG serialization
		// 100012 : Added sideslip
		// 100011 : Added Body serialization
		// 100010 : Converted to I.S. units
		// 100009 : added serialization of opps in numbered format
		// 100008 : Added m_WPolarDlg.m_bTiltedGeom, m_WPolarDlg.m_bWakeRollUp
		// 100006 : version 2.99Beta001 format
		// 100005 : version 2.00 format
		//Archive format introduced in Beta V22
		// Write units //added in Beta v16
		// Save analysis settings // added in Beta v22
		ar << s_LengthUnit;
		ar << s_AreaUnit;
		ar << s_WeightUnit;
		ar << s_SpeedUnit;
		ar << s_ForceUnit;
		ar << s_MomentUnit;

		if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
		else if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
		else if(WPolarDlg::s_WPolar.m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
		else if(WPolarDlg::s_WPolar.m_WPolarType==STABILITYPOLAR)  ar<<7;
		else ar << 0;

		ar << (float)WPolarDlg::s_WPolar.m_Mass;
		ar << (float)WPolarDlg::s_WPolar.m_QInf;
		ar << (float)WPolarDlg::s_WPolar.m_CoG.x;
		ar << (float)WPolarDlg::s_WPolar.m_CoG.y;
		ar << (float)WPolarDlg::s_WPolar.m_CoG.z;
		
		ar << (float)WPolarDlg::s_WPolar.m_Density;
		ar << (float)WPolarDlg::s_WPolar.m_Viscosity;
		ar << (float)WPolarDlg::s_WPolar.m_ASpec;
		ar << (float)WPolarDlg::s_WPolar.m_Beta;

		if(WPolarDlg::s_WPolar.m_AnalysisMethod==LLTMETHOD)        ar << 1;
		else if(WPolarDlg::s_WPolar.m_AnalysisMethod==VLMMETHOD)   ar << 2;
		else if(WPolarDlg::s_WPolar.m_AnalysisMethod==PANELMETHOD) ar << 3;

		if (pMiarex->m_bVLM1)   ar << 1;
		else                    ar << 0;

		ar <<123;
		if (WPolarDlg::s_WPolar.m_bTiltedGeom) ar << 1;
		else                                               ar << 0;
		if (WPolarDlg::s_WPolar.m_bWakeRollUp) ar << 1;
		else                                               ar << 0;

        ar << s_oaWing.size() ;//number of wings
		// Store the wings
        for (i=0; i<s_oaWing.size();i++)
		{
            pWing = (Wing*)s_oaWing.at(i);
			pWing->SerializeWing(ar, true);
		}
		// now store all the WPolars
		ar << m_oaWPolar.size();
		for (i=0; i<m_oaWPolar.size();i++)
		{
			pWPolar = (WPolar*)m_oaWPolar.at(i);
			pWPolar->SerializeWPlr(ar, bIsStoring);
		}
		// next store all the WOpps
		if(m_bSaveWOpps)
		{
			ar << m_oaWOpp.size();
			for (i=0; i<m_oaWOpp.size();i++)
			{
				pWOpp = (WingOpp*)m_oaWOpp.at(i);
				pWOpp->SerializeWingOpp(ar, bIsStoring);
			}
		}
		else ar << 0;

		// then the foils,  polars and Opps
		WritePolars(ar, NULL);

        /** @deprecated body array is deprecated; kept to insure compatibility with versions prior to v6.09.06 */
        ar << s_oaBody.size();
        for (i=0; i<s_oaBody.size();i++)
		{
            pBody = (Body*)s_oaBody.at(i);
			pBody->SerializeBody(ar, bIsStoring);
        }

		// last write the planes...
        ar << s_oaPlane.size();
        for (i=0; i<s_oaPlane.size();i++)
		{
            pPlane = (Plane*)s_oaPlane.at(i);
			pPlane->SerializePlane(ar, bIsStoring);
		}

		if(m_bSaveWOpps)
		{
			// not forgetting their POpps
			ar << m_oaPOpp.size();
			for (i=0; i<m_oaPOpp.size();i++)
			{
				pPOpp = (PlaneOpp*)m_oaPOpp.at(i);
				pPOpp->SerializePOpp(ar, bIsStoring);
			}
		}
		else ar << 0;

		pAFoil->m_pSF->Serialize(ar, bIsStoring);
//		pAFoil->m_pPF->Serialize(ar, bIsStoring);

		QApplication::restoreOverrideCursor();
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
			ar >> s_LengthUnit;
			ar >> s_AreaUnit;
			ar >> s_WeightUnit;
			ar >> s_SpeedUnit;
			ar >> s_ForceUnit;

			if(ArchiveFormat>=100005)
			{
				ar >> s_MomentUnit;
			}

			SetUnits(s_LengthUnit, s_AreaUnit, s_SpeedUnit, s_WeightUnit, s_ForceUnit, s_MomentUnit,
					 s_mtoUnit, s_m2toUnit, s_mstoUnit, s_kgtoUnit, s_NtoUnit, s_NmtoUnit);

			if(ArchiveFormat>=100004)
			{
				ar >>k;
				if(k==1)      WPolarDlg::s_WPolar.m_WPolarType = FIXEDSPEEDPOLAR;
				else if(k==2) WPolarDlg::s_WPolar.m_WPolarType = FIXEDLIFTPOLAR;
				else if(k==4) WPolarDlg::s_WPolar.m_WPolarType = FIXEDAOAPOLAR;
				else if(k==7) WPolarDlg::s_WPolar.m_WPolarType = STABILITYPOLAR;

				ar >> f; WPolarDlg::s_WPolar.m_Mass=f;
				ar >> f; WPolarDlg::s_WPolar.m_QInf=f;
				if(ArchiveFormat>=100013)
				{
					ar >> f; WPolarDlg::s_WPolar.m_CoG.x=f;
					ar >> f; WPolarDlg::s_WPolar.m_CoG.y=f;
					ar >> f; WPolarDlg::s_WPolar.m_CoG.z=f;
				}
				else
				{
					ar >> f; WPolarDlg::s_WPolar.m_CoG.x=f;
					WPolarDlg::s_WPolar.m_CoG.y=0;
					WPolarDlg::s_WPolar.m_CoG.z=0;
				}
				if(ArchiveFormat<100010) WPolarDlg::s_WPolar.m_CoG.x=f/1000.0;
				ar >> f; WPolarDlg::s_WPolar.m_Density   = f;
				ar >> f; WPolarDlg::s_WPolar.m_Viscosity = f;
				ar >> f; WPolarDlg::s_WPolar.m_ASpec     = f;
				if(ArchiveFormat>=100012)
				{
					ar >>f; WPolarDlg::s_WPolar.m_Beta=f;
				}

				ar >> k;
				if(k==1)      WPolarDlg::s_WPolar.m_AnalysisMethod=LLTMETHOD;
				else if(k==2) WPolarDlg::s_WPolar.m_AnalysisMethod=VLMMETHOD;
				else if(k==3) WPolarDlg::s_WPolar.m_AnalysisMethod=PANELMETHOD;
			}
			if(ArchiveFormat>=100006)
			{
				ar >> k;
				if (k) pMiarex->m_bVLM1 = true;
				else   pMiarex->m_bVLM1 = false;

				ar >> k;
//					if (k) WPolarDlg::m_bMiddle = true;
//					else   WPolarDlg::m_bMiddle = false;
			}

			if(ArchiveFormat>=100008)
			{
				ar >> k;
				if (k) WPolarDlg::s_WPolar.m_bTiltedGeom = true;
				else   WPolarDlg::s_WPolar.m_bTiltedGeom = false;

				ar >> k;
				if (k) WPolarDlg::s_WPolar.m_bWakeRollUp = true;
				else   WPolarDlg::s_WPolar.m_bWakeRollUp = false;
			}
			// and read n again
			ar >> n;
		}
		// WINGS FIRST
		for (i=0;i<n; i++)
		{
			pWing = new Wing;

			if (!pWing->SerializeWing(ar, bIsStoring))
			{
					if(pWing) delete pWing;
					QApplication::restoreOverrideCursor();
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
			pWPolar = new WPolar;
			bWPolarOK = pWPolar->SerializeWPlr(ar, bIsStoring);
			//force compatibilty
			if(pWPolar->m_AnalysisMethod==PANELMETHOD && pWPolar->m_WPolarType==STABILITYPOLAR) pWPolar->m_bThinSurfaces = true;

			if (!bWPolarOK)
			{
				if(pWPolar) delete pWPolar;
				QApplication::restoreOverrideCursor();
				return false;
			}
			if(!pWPolar->m_AnalysisMethod==LLTMETHOD && ArchiveFormat <100003)	pWPolar->ResetWPlr();//former VLM version was flawed
//			if(pWPolar->m_WPolarType==STABILITYPOLAR)	pWPolar->m_bThinSurfaces = true;

			if(pWPolar->m_PolarFormat!=1020 || pWPolar->m_WPolarType!=STABILITYPOLAR)//v601 stability polars are obsolete
				pWPolar = pMiarex->AddWPolar(pWPolar);
		}

		//THEN WOPPS
		ar >> n;// number of WOpps to load
		bool bWOppOK;
		for (i=0;i<n; i++)
		{
			pWOpp = new WingOpp();
			if (ArchiveFormat<=100001)
			{
				QApplication::restoreOverrideCursor();
				return false;
			}
			else
			{
				bWOppOK = pWOpp->SerializeWingOpp(ar, bIsStoring);
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
				QApplication::restoreOverrideCursor();
				return false;
			}

			if(pWOpp->m_AnalysisMethod==LLTMETHOD || ArchiveFormat >=100006)//former VLM version was flawed
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
				QApplication::restoreOverrideCursor();
				return false;
			}
		}
		else
		{
			if(ArchiveFormat>=100006)
			{
				if(!LoadPolarFileV3(ar, bIsStoring))
				{
					QApplication::restoreOverrideCursor();
					return false;
				}
			}
			else
			{
				QApplication::restoreOverrideCursor();
				return false;
			}
		}
		if(n==100000)
		{
            for (j=0; j<s_oaPolar.size(); j++)
			{
                pPolar = (Polar*)s_oaPolar.at(j);
                for (k=0; k<s_oaFoil.size(); k++)
				{
                    pFoil = (Foil*)s_oaFoil.at(k);
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
				pBody = new Body();

				if (pBody->SerializeBody(ar, bIsStoring))
				{
					s_oaBody.append(pBody);

				}
				else
				{
					if(pBody) delete pBody;
					QApplication::restoreOverrideCursor();
					return false;
				}
			}
		}
		if(ArchiveFormat>=100006)
		{ //read the planes
			ar >> n;
			for (i=0; i<n;i++)
			{
				pPlane = new Plane();
				if(pPlane)
				{
					if(pPlane->SerializePlane(ar, bIsStoring))
					{
						pMiarex->AddPlane(pPlane);
					}
					else
					{
						if(pPlane) delete pPlane;
						QApplication::restoreOverrideCursor();
						return false;
					}
				}
			}

			// attach the body pointers to the Plane objects
			for (int ib=0; ib<s_oaBody.size(); ib++)
			{
				Body *pBody = (Body*)s_oaBody.at(ib);
				pMiarex->AddBody(pBody);
			}


			//and their pPolars
			if(ArchiveFormat <100007)
			{
				ar >> n;// number of WPolars to load
				for (i=0;i<n; i++)
				{
					pWPolar = new WPolar();

					if (!pWPolar->SerializeWPlr(ar, bIsStoring))
					{
						if(pWPolar) delete pWPolar;
						QApplication::restoreOverrideCursor();
						return false;
					}
					if(!pWPolar->m_AnalysisMethod==LLTMETHOD && ArchiveFormat <100003)
						pWPolar->ResetWPlr();
					pMiarex->AddWPolar(pWPolar);
				}
			}

			ar >> n;// number of PlaneOpps to load
			for (i=0;i<n; i++)
			{
				pPOpp = new PlaneOpp();

				if (!pPOpp->SerializePOpp(ar, bIsStoring))
				{
					if(pPOpp) delete pPOpp;
					QApplication::restoreOverrideCursor();
					return false;
				}
				pMiarex->AddPOpp(false, NULL, NULL, NULL, pPOpp);
			}
		}
		pMiarex->m_pCurPOpp = NULL;

		pAFoil->m_pSF->Serialize(ar, bIsStoring);

        for (i=0; i<s_oaWing.size();i++)
		{
            pWing = (Wing*)s_oaWing[i];
			pWing->ComputeGeometry();
		}

		if(m_iApp==MIAREX) pMiarex->SetUFO();

		pMiarex->UpdateUnits();

		QApplication::restoreOverrideCursor();
		return true;
	}
}



void MainFrame::SetCurrentFoil(Foil* pFoil)
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil   *pAFoil = (QAFoil*)m_pAFoil;
	pXDirect->SetFoil(pFoil);
	pAFoil->SelectFoil(pFoil);
	s_pCurFoil = pFoil;
}




void MainFrame::SetMenus()
{
	if(m_iApp==NOAPP)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(optionsMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==XFOILANALYSIS)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(XDirectViewMenu);
		menuBar()->addMenu(FoilMenu);
		menuBar()->addMenu(DesignMenu);
		menuBar()->addMenu(XFoilAnalysisMenu);
		menuBar()->addMenu(PolarMenu);
		menuBar()->addMenu(OpPointMenu);
		menuBar()->addMenu(optionsMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==INVERSEDESIGN)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(XInverseViewMenu);
		menuBar()->addMenu(InverseFoilMenu);
		menuBar()->addMenu(optionsMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp==DIRECTDESIGN)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(AFoilViewMenu);
		menuBar()->addMenu(AFoilDesignMenu);
		menuBar()->addMenu(AFoilSplineMenu);
		menuBar()->addMenu(optionsMenu);
		menuBar()->addMenu(helpMenu);
	}
	else if(m_iApp== MIAREX)
	{
		menuBar()->clear();
		menuBar()->addMenu(fileMenu);
		menuBar()->addMenu(MiarexViewMenu);
		menuBar()->addMenu(UFOMenu);
		menuBar()->addMenu(MiarexWPlrMenu);
		menuBar()->addMenu(MiarexWOppMenu);
		menuBar()->addMenu(MiarexAnalysisMenu);
		menuBar()->addMenu(optionsMenu);
		menuBar()->addMenu(helpMenu);
	}
}



Foil* MainFrame::SetModFoil(Foil* pNewFoil, bool bKeepExistingFoil)
{
	// Adds the buffer foil to the ObArray,
	// gives it a proper name, FoilName or another,
	// selects it ,
	// and initializes XFoil, comboboxes and everything.

	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	int j,k,l;
	bool bExists = false;
	bool bNotFound = true;
	pNewFoil->m_bSaved = false;
	Foil*pOldFoil;

	if(!pNewFoil->m_FoilName.length())
	{
		bExists = true;
	}
	else
	{
        for (j=0; j<s_oaFoil.size(); j++)
		{
            pOldFoil = (Foil*)s_oaFoil.at(j);
			if (pOldFoil->m_FoilName == pNewFoil->m_FoilName)
			{
				bExists = true;
				if(bKeepExistingFoil)
				{
						delete pNewFoil;
						pNewFoil = NULL;
						s_pCurFoil = NULL;
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
			Foil *pFoil;
			QStringList NameList;
            for(k=0; k<s_oaFoil.size(); k++)
			{
                pFoil = (Foil*)s_oaFoil.at(k);
				NameList.append(pFoil->m_FoilName);
			}

            RenameDlg renDlg(this);
            renDlg.m_pstrArray = & NameList;
            renDlg.m_strQuestion = tr("A foil of that name already exists\nPlease enter a new name");
            renDlg.m_strName = pNewFoil->m_FoilName;
            renDlg.InitDialog();

			bool exists = false;
			QString strong;
            int resp = renDlg.exec();
            strong = renDlg.m_strName;

			if(QDialog::Accepted == resp)
			{
                strong = renDlg.m_strName;
                for (l=0; l<s_oaFoil.size(); l++)
				{
                    pOldFoil = (Foil*)s_oaFoil.at(l);
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
                for (l=s_oaFoil.size()-1;l>=0; l--)
				{
                    pOldFoil = (Foil*)s_oaFoil.at(l);
					if(pOldFoil->m_FoilName == strong)
					{
						pNewFoil->m_FoilColor  = pOldFoil->m_FoilColor;
						pNewFoil->m_nFoilStyle = pOldFoil->m_nFoilStyle;
						pNewFoil->m_nFoilWidth = pOldFoil->m_nFoilWidth;
						pNewFoil->m_bPoints    = pOldFoil->m_bPoints;
                        s_oaFoil.removeAt(l);
						delete pOldFoil;
						if(s_pCurFoil == pOldFoil) s_pCurFoil = NULL;
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
				Polar * pPolar;
                for (l=0; l <s_oaPolar.size();l++)
				{
                    pPolar = (Polar*) s_oaPolar.at(l);
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
				s_pCurFoil = NULL;
				return NULL;// foil not added
			}
		}
	}
	else
	{
		AddFoil(pNewFoil);
		SetSaveState(false);
	}

	pAFoil->SelectFoil(pNewFoil);
	pXDirect->SetFoil(pNewFoil);
	s_pCurFoil = pNewFoil;

	return pNewFoil;// foil added
}


void MainFrame::SetProjectName(QString PathName)
{
	m_FileName = PathName;
	int pos = PathName.lastIndexOf("/");
	if (pos>0) s_ProjectName = PathName.right(PathName.length()-pos-1);
	else s_ProjectName = PathName;
	if(s_ProjectName.length()>4)
	{
		s_ProjectName = s_ProjectName.left(s_ProjectName.length()-4);
		m_pctrlProjectName->setText(s_ProjectName);
	}
}


void MainFrame::SetSaveState(bool bSave)
{
	s_bSaved = bSave;

	int len = s_ProjectName.length();
	if(s_ProjectName.right(1)=="*") s_ProjectName = s_ProjectName.left(len-1);
	if (!bSave)
	{
		s_ProjectName += "*";
	}
	m_pctrlProjectName->setText(s_ProjectName);
}




void MainFrame::SetGraphSettings(Graph *pGraph)
{
	QXDirect *pXDirect   = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex     = (QMiarex*)m_pMiarex;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;

	pXDirect->m_pCpGraph->CopySettings(pGraph, false);
	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
	{
		pXDirect->m_PlrGraph[ig].CopySettings(pGraph, false);
	}

	pXInverse->m_QGraph.CopySettings(pGraph, false);

	pMiarex->m_LongRLGraph.CopySettings(pGraph, false);
	pMiarex->m_LatRLGraph.CopySettings(pGraph, false);
	pMiarex->m_CpGraph.CopySettings(pGraph, false);
	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		pMiarex->m_WPlrGraph[ig].CopySettings(pGraph, false);
		pMiarex->m_WingGraph[ig].CopySettings(pGraph, false);
		pMiarex->m_TimeGraph[ig].CopySettings(pGraph, false);
	}
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
	Wing *pWing, *pCurWing;
	Plane *pPlane, *pCurPlane;
	pCurWing  = pMiarex->m_pCurWing;
	pCurPlane = pMiarex->m_pCurPlane;

	QStringList UFONames;
    for (i=0; i<s_oaWing.size(); i++)
	{
        pWing = (Wing*)s_oaWing[i];
		UFONames.append(pWing->m_WingName);
	}
    for (i=0; i<s_oaPlane.size(); i++)
	{
        pPlane = (Plane*)s_oaPlane[i];
		UFONames.append(pPlane->PlaneName());
	}
	UFONames.sort();
	m_pctrlUFO->addItems(UFONames);

	if (m_pctrlUFO->count())
	{
		m_pctrlUFO->setEnabled(true);
		//select the current Wing, if any...
		if(pCurPlane)
		{
			int pos = m_pctrlUFO->findText(pCurPlane->PlaneName());
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
	WPolar *pWPolar;
	QString UFOName;
	int i;

	m_pctrlWPolar->clear();

	Plane  *pCurPlane = pMiarex->m_pCurPlane;
	Wing   *pCurWing  = pMiarex->m_pCurWing;
	WPolar *pCurWPlr  = pMiarex->m_pCurWPolar;

	if(pCurPlane)      UFOName = pCurPlane->PlaneName();
	else if(pCurWing)  UFOName = pCurWing->m_WingName;
	else               UFOName = "";

	if(!UFOName.length())
	{
		pMiarex->m_pCurWPolar = NULL;
		m_pctrlWPolar->setEnabled(false);
		return;
	}

	int size = 0;
	//count the number of WPolars associated to the current Wing
	for(i=0; i<m_oaWPolar.size(); i++)
	{
		pWPolar = (WPolar*)m_oaWPolar[i];
		if(pWPolar->m_UFOName == UFOName)
		{
			size++;
		}
	}
	if(size)
	{
		// if any
		m_pctrlWPolar->blockSignals(true);
		m_pctrlWPolar->setEnabled(true);
		for (i=0; i<m_oaWPolar.size(); i++)
		{
			pWPolar = (WPolar*)m_oaWPolar[i];
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
			}
		}		
		//... else select the first
		else
		{
			m_pctrlWPolar->setCurrentIndex(0);
		}
		m_pctrlWPolar->blockSignals(false);
	}
	else
	{
		// otherwise disable control
		m_pctrlWPolar->setEnabled(false);
		pMiarex->m_pCurWPolar = NULL;
		pMiarex->m_pCurWOpp = NULL;
	}
}

/**
 * Fills the combobox with the WOpp parameters associated to Miarex' current WPLr,
 * then selects the current WingOpp or PlaneOpp if any, else selects the first, if any,
 * else disables the combobox.
 */
void MainFrame::UpdateWOpps()
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	WingOpp *pWOpp;
	PlaneOpp *pPOpp;
	int i;
	QString str;
	m_pctrlWOpp->clear();

	Plane  *pCurPlane   = pMiarex->m_pCurPlane;
	Wing   *pCurWing    = pMiarex->m_pCurWing;
	WPolar *pCurWPlr    = pMiarex->m_pCurWPolar;

	if (!pCurWing || !pCurWing->m_WingName.length() || !pCurWPlr || !pCurWPlr->m_PlrName.length())
	{
		m_pctrlWOpp->setEnabled(false);
		pMiarex->m_pCurWOpp = NULL;
		return;
	}

	if(pCurPlane)
	{
		int size = 0;
		//count the number of POpps associated to the current Wing & WPolar
		for (i=0; i<m_oaPOpp.size(); i++)
		{
			pPOpp = (PlaneOpp*)m_oaPOpp[i];
			if (pPOpp->m_PlaneName==pCurPlane->PlaneName() && pPOpp->m_PlrName==pCurWPlr->m_PlrName)
			{
				size++;
			}
		}
		if (size)
		{
			// if any
			m_pctrlWOpp->setEnabled(true);

			for (int i=0; i<m_oaPOpp.size(); i++)
			{
				pPOpp = (PlaneOpp*)m_oaPOpp[i];
				if (pPOpp->m_PlaneName == pCurPlane->PlaneName() && pPOpp->m_PlrName == pCurWPlr->m_PlrName)
				{
                    if(pCurWPlr->m_WPolarType<FIXEDAOAPOLAR)        str = QString("%L1").arg(pPOpp->m_Alpha,8,'f',3);
                    else if(pCurWPlr->m_WPolarType==FIXEDAOAPOLAR)  str = QString("%L1").arg(pPOpp->m_QInf,8,'f',3);
                    else if(pCurWPlr->m_WPolarType==STABILITYPOLAR) str = QString("%L1").arg(pPOpp->m_Ctrl,8,'f',3);
					m_pctrlWOpp->addItem(str);
                }
			}

            if(pMiarex->m_pCurPOpp) SelectWOpp(pMiarex->m_pCurPOpp);
            else                    m_pctrlWOpp->setCurrentIndex(0);
		}
		else
		{
			// otherwise disable control
			m_pctrlWOpp->setEnabled(false);
			pMiarex->m_pCurPOpp = NULL;
		}
	}
	else
	{
		int size = 0;
		//count the number of WOpps associated to the current Wing & WPolar
		for (i=0; i<m_oaWOpp.size(); i++)
		{
			pWOpp = (WingOpp*)m_oaWOpp[i];
			if (pWOpp->m_WingName == pCurWing->m_WingName && pWOpp->m_PlrName  == pCurWPlr->m_PlrName)
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
				pWOpp = (WingOpp*)m_oaWOpp[i];
				if (pWOpp->m_WingName == pCurWing->m_WingName && pWOpp->m_PlrName == pCurWPlr->m_PlrName)
				{

                    if(pCurWPlr->m_WPolarType<FIXEDAOAPOLAR)         str = QString("%L1").arg(pWOpp->m_Alpha,8,'f',3);
                    else  if(pCurWPlr->m_WPolarType==FIXEDAOAPOLAR)  str = QString("%L1").arg(pWOpp->m_QInf,8,'f',3);
                    else  if(pCurWPlr->m_WPolarType==STABILITYPOLAR) str = QString("%L1").arg(pWOpp->m_Ctrl,8,'f',3);

					m_pctrlWOpp->addItem(str);
				}
			}

            if(pMiarex->m_pCurWOpp) SelectWOpp(pMiarex->m_pCurWOpp);
            else                    m_pctrlWOpp->setCurrentIndex(0);
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


/**
 * Fills the combobox with the Foil names,
 * then selects the current Foil if any, else selects the first, if any,
 * else disables the combobox
 */
void MainFrame::UpdateFoils()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, pos;
	QString strong;
	m_pctrlFoil->clear();

	Foil *pFoil;

    for (i=0; i<s_oaFoil.size(); i++)
	{
        pFoil = (Foil*)s_oaFoil[i];
		m_pctrlFoil->addItem(pFoil->m_FoilName);
	}

	if (m_pctrlFoil->count())
	{
		m_pctrlFoil->setEnabled(true);
		//select the current foil, if any...
		if (s_pCurFoil)
		{
			pos = m_pctrlFoil->findText(s_pCurFoil->m_FoilName);
			if (pos>=0) m_pctrlFoil->setCurrentIndex(pos);
			else
			{
				// if error, select the first...
				m_pctrlFoil->setCurrentIndex(0);
				strong = m_pctrlFoil->itemText(0);
				//...and set it
//				g_ppCurFoil = GetFoil(strong);
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

			pXDirect->SetFoil(foil(strong));
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


/**
 * Fills the combobox with polar names associated to the current foil,
 * then selects XDirect current polar if any, else selects the first, if any,
 * else disables the combobox,
 */
void MainFrame::UpdatePolars()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, size, pos;
	Polar *pPolar;
	QString strong;
	m_pctrlPolar->clear();

	s_pCurFoil = s_pCurFoil;

	if(!s_pCurFoil || !s_pCurFoil->m_FoilName.length())
	{
		m_pctrlPolar->setEnabled(false);
		m_pctrlOpPoint->clear();
		m_pctrlOpPoint->setEnabled(false);
		return;
	}

	size = 0;
	//count the number of polars associated to the current foil
    for (i=0; i<s_oaPolar.size(); i++)
	{
        pPolar = (Polar*)s_oaPolar[i];
		if(pPolar->m_FoilName == s_pCurFoil->m_FoilName)
		{
			size++;
		}
	}
	if (size)
	{

		// if any
		m_pctrlPolar->setEnabled(true);

        for (i=0; i<s_oaPolar.size(); i++)
		{
            pPolar = (Polar*)s_oaPolar[i];
			if(pPolar->m_FoilName == s_pCurFoil->m_FoilName)
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
			}
		}
		else
		{
			m_pctrlPolar->setCurrentIndex(0);
			strong = m_pctrlPolar->itemText(0);
		}
	}
	else
	{
		// otherwise disable control
		m_pctrlPolar->setEnabled(false);
	}

	UpdateOpps();
}

/**
 * Fills the combobox with the OpPoint values associated to the current foil,
 * then selects the current OpPoint if any, else selects the first, if any,
 * else disables the combobox.
 */
void MainFrame::UpdateOpps()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	int i, pos;
	OpPoint *pOpp;
	QString strong, str;
	m_pctrlOpPoint->clear();


	Polar *pCurPlr    = pXDirect->m_pCurPolar;

	if (!s_pCurFoil || !s_pCurFoil->m_FoilName.length() || !pCurPlr  || !pCurPlr->m_PlrName.length())
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
		if (pOpp->m_strFoilName == s_pCurFoil->m_FoilName && pOpp->m_strPlrName  == pCurPlr->m_PlrName)
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
			if (pOpp->m_strFoilName == s_pCurFoil->m_FoilName && pOpp->m_strPlrName  == pCurPlr->m_PlrName)
			{
				if (pCurPlr->m_PolarType !=FIXEDAOAPOLAR)
				{
//					if(qAbs(pOpp->Alpha)<0.0001) pOpp->Alpha = 0.0001;
                    str = QString("%L1").arg(pOpp->Alpha,8,'f',3);
					m_pctrlOpPoint->addItem(str);
				}
				else
				{
                    str = QString("%L1").arg(pOpp->Reynolds,8,'f',0);
					m_pctrlOpPoint->addItem(str);
				}
			}
		}
		if (pXDirect->m_pCurOpp && pXDirect->m_pCurOpp->m_strFoilName==s_pCurFoil->m_FoilName)
		{
			//select it
			if (pCurPlr->m_PolarType !=FIXEDAOAPOLAR)
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
			textline = str.toLatin1();
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
		default:
			break;
	}
}


void MainFrame::WritePolars(QDataStream &ar, Foil *pFoil)
{
	int i;
	if(!pFoil)
	{
		ar << 100003;
		//100003 : added foil comment
		//100002 : means we are serializings opps in the new numbered format
		//100001 : transferred NCrit, XTopTr, XBotTr to polar file
		//first write foils
        ar << s_oaFoil.size();

        for (i=0; i<s_oaFoil.size(); i++)
		{
            pFoil = (Foil*)s_oaFoil.at(i);
			pFoil->Serialize(ar, true);
		}

		//then write polars
        ar << s_oaPolar.size();
		Polar * pPolar ;
        for (i=0; i<s_oaPolar.size();i++)
		{
            pPolar = (Polar*)s_oaPolar.at(i);
			pPolar->Serialize(ar, true);
		}
	}
	else
	{
		ar << 100003;
		//100003 : added foil comment
		//100002 : means we are serializings opps in the new numbered format
		//100001 : transferred NCrit, XTopTr, XBotTr to polar file
		//first write foil
		ar << 1;//only one this time
		pFoil->Serialize(ar,true);
		//count polars associated to the foil
		Polar * pPolar ;
		int n=0;
        for (i=0; i<s_oaPolar.size();i++)
		{
            pPolar = (Polar*)s_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName) n++;
		}
		//then write polars
		ar << n;
        for (i=0; i<s_oaPolar.size();i++)
		{
            pPolar = (Polar*)s_oaPolar.at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName) pPolar->Serialize(ar, true);
		}		
	}

	if(m_bSaveOpps)
	{
		ar << m_oaOpp.size();
		OpPoint * pOpp ;
		for (i=0; i<m_oaOpp.size();i++)
		{
			pOpp = (OpPoint*)m_oaOpp.at(i);
			pOpp->Serialize(ar,true,100002);
		}
	}
	else ar << 0;

}


void MainFrame::OnPolarProps()
{
	if(m_iApp==XFOILANALYSIS)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->OnPolarProps();
	}
	else if(m_iApp==MIAREX)
	{
		QMiarex *pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->OnWPolarProps();
	}
}



void MainFrame::OnWOppProps()
{
	if(m_iApp==XFOILANALYSIS)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->OnOpPointProps();
	}
	else if(m_iApp==MIAREX)
	{
		QMiarex *pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->OnWOppProps();
	}
}



void MainFrame::OnDuplicateFoil()
{
	if(!s_pCurFoil) return;
	Foil *pNewFoil = new Foil();
	pNewFoil->CopyFoil(s_pCurFoil);
	pNewFoil->InitFoil();

	if(SetModFoil(pNewFoil))
	{
		if(m_iApp==XFOILANALYSIS)
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->m_BufferFoil.CopyFoil(pNewFoil);
			pXDirect->SetFoil(pNewFoil);
		}
		else if(m_iApp==DIRECTDESIGN)
		{
			QAFoil *pAFoil= (QAFoil*)m_pAFoil;
			//then duplicate the buffer foil and add it
			Foil *pNewFoil = new Foil();
			pNewFoil->CopyFoil(pAFoil->m_pBufferFoil);
			pAFoil->FillFoilTable();
			pAFoil->SelectFoil(pNewFoil);
		}

		UpdateFoils();
		UpdateView();
		SetSaveState(false);
	}
}
 

void MainFrame::SetupDataDir()
{
#ifdef Q_WS_MAC
	s_TranslationDir.setPath(qApp->applicationDirPath()+"/translations/");
	s_StylesheetDir.setPath(qApp->applicationDirPath()+"/qss/");
#endif
#ifdef Q_OS_WIN
    s_TranslationDir.setPath(qApp->applicationDirPath()+"/translations");
    s_StylesheetDir.setPath(qApp->applicationDirPath()+"/qss");
#endif
#ifdef Q_OS_LINUX
	s_TranslationDir.setPath("/usr/share/xflr5/translations");
	s_StylesheetDir.setPath("/usr/share/xflr5/qss");
#endif

}


void MainFrame::ReadStyleSheet(QString styleSheetName, QString &styleSheet)
{
	QFile file(s_StylesheetDir.canonicalPath()+"/"+styleSheetName+".qss");

    if(file.open(QFile::ReadOnly))
	{
		styleSheet = QLatin1String(file.readAll());
		qApp->setStyleSheet(styleSheet);
	}
}






