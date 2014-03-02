/****************************************************************************

	DisplaySettingsDlg Class
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

#include "Settings.h"
#include "../mainframe.h"
#include "../miarex/Miarex.h"
#include "../xdirect/XDirect.h"
#include "../xinverse/XInverse.h"
#include "../graph/GraphDlg.h"
#include <QApplication>
#include <QGroupBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QStyleFactory>
#include <QDir>
#include <QtDebug>

bool Settings::s_bStyleSheets = true;
QString Settings::s_StyleName;
QString Settings::s_StyleSheetName;

QFont Settings::s_TextFont;
QFont Settings::s_TableFont;
QColor Settings::s_BackgroundColor = QColor(3, 9, 9);
QColor Settings::s_TextColor=QColor(221,221,221);
bool Settings::s_bReverseZoom = false;
bool Settings::s_bAlphaChannel = true;
QGraph Settings::s_RefGraph;
enumTextFileType Settings::s_ExportFileType;  /**< Defines if the list separator for the output text files should be a space or a comma. */
QString Settings::s_LastDirName = QDir::homePath();


Settings::Settings(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("General Display Settings"));

	s_RefGraph.SetGraphName("Reference_Graph");

	m_pMainFrame = pParent;
	m_bIsGraphModified = false;


#ifdef Q_WS_MAC
	m_StyleSheetDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_OS_WIN
	m_StyleSheetDir.setPath(qApp->applicationDirPath());
#endif
#ifdef Q_OS_LINUX
	m_StyleSheetDir.setPath("/usr/share/xflr5");
#endif

	SetupLayout();

	connect(m_pctrlStyles, SIGNAL(activated(const QString &)),this, SLOT(OnStyleChanged(const QString &)));

	connect(m_pctrlBackColor, SIGNAL(clicked()),this, SLOT(OnBackgroundColor()));
	connect(m_pctrlGraphSettings, SIGNAL(clicked()),this, SLOT(OnGraphSettings()));
	connect(m_pctrlTextClr, SIGNAL(clickedTB()),this, SLOT(OnTextColor()));
	connect(m_pctrlTextFont, SIGNAL(clicked()),this, SLOT(OnTextFont()));
	connect(m_pctrlTableFont, SIGNAL(clicked()),this, SLOT(OnTableFont()));

	connect(m_pctrlReverseZoom, SIGNAL(clicked()), this, SLOT(OnReverseZoom()));
	connect(m_pctrlAlphaChannel, SIGNAL(clicked()), this, SLOT(OnAlphaChannel()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
}


void Settings::SetupLayout()
{
	m_pctrlStyles = new QComboBox;

	QRegExp regExp("Q(.*)Style");
	QString defaultStyle = QApplication::style()->metaObject()->className();
	if (defaultStyle == QLatin1String("QMacStyle"))
		defaultStyle = QLatin1String("Macintosh (Aqua)");
	else if (defaultStyle == QLatin1String("OxygenStyle"))
		defaultStyle = QLatin1String("Oxygen");
	else if (regExp.exactMatch(defaultStyle))
		defaultStyle = regExp.cap(1);

	m_pctrlStyles->addItems(QStyleFactory::keys());
	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(defaultStyle));

	// add custom style sheets
	QString fileName = "*.qss";
	QStringList filesList = MainFrame::s_StylesheetDir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);
	for(int is=0; is<filesList.count(); is++)
	{
		QString styleSheetName = filesList.at(is);
		int len = styleSheetName.length();
		styleSheetName = styleSheetName.left(len-4);
		m_pctrlStyles->addItem(styleSheetName);
	}


	QGroupBox *GraphBox = new QGroupBox(tr("Graph Settings"));
	{
		QHBoxLayout *GraphLayout = new QHBoxLayout;
		{
			m_pctrlGraphSettings  = new QPushButton(tr("All Graph Settings"));
			m_pctrlGraphSettings->setMinimumWidth(120);
			GraphLayout->addWidget(m_pctrlGraphSettings);
		}
		GraphBox->setLayout(GraphLayout);
	}


	QGroupBox *BackBox = new QGroupBox(tr("Background Color"));
	{
		QHBoxLayout *BackLayout = new QHBoxLayout;
		{
			m_pctrlBackColor      = new ColorButton(this);
			m_pctrlBackColor->setMinimumWidth(120);
			BackLayout->addWidget(m_pctrlBackColor);
		}
		BackBox->setLayout(BackLayout);
	}

	QGroupBox *pFontBox = new QGroupBox(tr("Fonts"));
	{
		QGridLayout *pMainFontLayout = new QGridLayout;
		{
			QLabel *labMain = new QLabel(tr("Main display font"));
			m_pctrlTextFont = new QPushButton;
			m_pctrlTextClr  = new TextClrBtn(this);

			pMainFontLayout->addWidget(labMain,1,1);
			pMainFontLayout->addWidget(m_pctrlTextFont,1,2);
			pMainFontLayout->addWidget(m_pctrlTextClr,1,3);

			QLabel *labTable = new QLabel(tr("Table font"));
			m_pctrlTableFont = new QPushButton;

			pMainFontLayout->addWidget(labTable,2,1);
			pMainFontLayout->addWidget(m_pctrlTableFont,2,2);
		}
		pFontBox->setLayout(pMainFontLayout);
	}


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("Close"));
		OKButton->setAutoDefault(false);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		m_pctrlReverseZoom = new QCheckBox(tr("Reverse zoom direction using mouse wheel"));
		m_pctrlAlphaChannel = new QCheckBox(tr("Enable 3D transparency"));
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(m_pctrlStyles);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(BackBox);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(pFontBox);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(GraphBox);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(m_pctrlReverseZoom);
		pMainLayout->addStretch(1);
		pMainLayout->addWidget(m_pctrlAlphaChannel);
		pMainLayout->addSpacing(20);
		pMainLayout->addStretch(1);
		pMainLayout->addLayout(CommandButtons);
		pMainLayout->addStretch(1);
	}

	setLayout(pMainLayout);
}


void Settings::InitDialog()
{
	m_MemGraph.CopySettings(&s_RefGraph);

	m_pctrlBackColor->SetColor(s_BackgroundColor);

	m_pctrlTextClr->setTextColor(s_TextColor);
	m_pctrlTextClr->setBackgroundColor(s_BackgroundColor);

	QString textFontName = s_TextFont.family() + QString(" %1").arg(s_TextFont.pointSize());
	m_pctrlTextFont->setText(textFontName);
	m_pctrlTextFont->setFont(s_TextFont);
	m_pctrlTextClr->setFont(s_TextFont);
	m_pctrlTextClr->setText(QObject::tr("Text color"));

	QString tableFontName = s_TableFont.family() + QString(" %1").arg(s_TableFont.pointSize());
	m_pctrlTableFont->setText(tableFontName);
	m_pctrlTableFont->setFont(s_TableFont);

	if(m_pctrlStyles->findText(s_StyleName)>=0)
		m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(s_StyleName));
	else if(m_pctrlStyles->findText(s_StyleSheetName)>=0)
		m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(s_StyleSheetName));

	m_pctrlReverseZoom->setChecked(s_bReverseZoom);
	m_pctrlAlphaChannel->setChecked(s_bAlphaChannel);
}



void Settings::OnStyleChanged(const QString &StyleName)
{
	//test for style sheet
	QString fileName = "*.qss";
	QStringList filesList = MainFrame::s_StylesheetDir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

	for(int is=0; is<filesList.count(); is++)
	{
		QString styleSheetName = filesList.at(is);
		int len = styleSheetName.length();
		styleSheetName = styleSheetName.left(len-4);
		if(styleSheetName.compare(StyleName)==0)
		{
			s_bStyleSheets = true;
			s_StyleSheetName = styleSheetName;
			s_StyleName.clear();
			QString styleSheet;
			MainFrame::ReadStyleSheet(styleSheetName, styleSheet);
			return;
		}
	}

	s_bStyleSheets = false;
	s_StyleSheetName.clear();
	qApp->setStyleSheet(styleSheet());
	qApp->setStyle(StyleName);
	s_StyleName = StyleName;
}


void Settings::OnBackgroundColor()
{
	QColor Color = QColorDialog::getColor(s_BackgroundColor);
	if(Color.isValid()) s_BackgroundColor = Color;

	m_pctrlBackColor->SetColor(s_BackgroundColor);

	m_pctrlTextClr->setBackgroundColor(s_BackgroundColor);
}


void Settings::reject()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect *pXDirect   = (QXDirect*)pMainFrame->m_pXDirect;
	QMiarex *pMiarex     = (QMiarex*)pMainFrame->m_pMiarex;
	QXInverse *pXInverse = (QXInverse*)pMainFrame->m_pXInverse;

	pXDirect->m_CpGraph.CopySettings(&m_MemGraph);
	pXDirect->m_CpGraph.SetInverted(true);

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		pXDirect->m_PlrGraph[ig].CopySettings(&m_MemGraph);

	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		pMiarex->m_WingGraph[ig].CopySettings(&m_MemGraph);
		pMiarex->m_TimeGraph[ig].CopySettings(&m_MemGraph);
		pMiarex->m_WPlrGraph[ig].CopySettings(&m_MemGraph);
	}

	pXInverse->m_QGraph.CopySettings(&m_MemGraph);
	pXInverse->m_QGraph.SetInverted(true);

	QDialog::reject();
}



void Settings::OnGraphSettings()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect *pXDirect   = (QXDirect*)pMainFrame->m_pXDirect;
	QMiarex *pMiarex     = (QMiarex*)pMainFrame->m_pMiarex;
	QXInverse *pXInverse = (QXInverse*)pMainFrame->m_pXInverse;

	GraphDlg dlg(pMainFrame);

	dlg.m_GraphArray[0] = &pXDirect->m_CpGraph;
	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		dlg.m_GraphArray[ig+1] = pXDirect->m_PlrGraph+ig;

	dlg.m_GraphArray[6] = pMiarex->m_WingGraph;
	dlg.m_GraphArray[7] = pMiarex->m_WingGraph+1;
	dlg.m_GraphArray[8] = pMiarex->m_WingGraph+2;
	dlg.m_GraphArray[9] = pMiarex->m_WingGraph+3;
	dlg.m_GraphArray[10] = pMiarex->m_WPlrGraph;
	dlg.m_GraphArray[11] = pMiarex->m_WPlrGraph+1;
	dlg.m_GraphArray[12] = pMiarex->m_WPlrGraph+2;
	dlg.m_GraphArray[13] = pMiarex->m_WPlrGraph+3;
	dlg.m_GraphArray[14] = pMiarex->m_TimeGraph;
	dlg.m_GraphArray[15] = pMiarex->m_TimeGraph+1;
	dlg.m_GraphArray[16] = pMiarex->m_TimeGraph+2;
	dlg.m_GraphArray[17] = pMiarex->m_TimeGraph+3;
	dlg.m_GraphArray[18] = &pMiarex->m_LongRLGraph;
	dlg.m_GraphArray[19] = &pMiarex->m_LatRLGraph;

	dlg.m_GraphArray[20] = &pXInverse->m_QGraph;

	dlg.m_NGraph = 21;

	QGraph graph;
	graph.CopySettings(&s_RefGraph);

	/** @todo mem is same as ref */
	dlg.m_pMemGraph = &s_RefGraph;
	dlg.m_pGraph = &graph;

	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
		s_RefGraph.CopySettings(dlg.m_pGraph);
		m_bIsGraphModified = true;

		pXDirect->m_CpGraph.SetInverted(true);
		pXInverse->m_QGraph.SetInverted(true);
	}
}


void Settings::OnTextColor()
{
	QColor Color = QColorDialog::getColor(s_TextColor);
	if(Color.isValid()) s_TextColor = Color;
	m_pctrlTextClr->setTextColor(s_TextColor);
}



void Settings::OnTextFont()
{
	bool bOK;
	QFont TextFont;
	TextFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);

#ifdef Q_WS_MAC
	//20090604 Mac OS Native font dialog does not work well under QT 4.5.1
	//QFont font = QFontDialog::getFont(&ok, m_TextFont, this);
		//20110324 Works again under QT 4.6, though it loses focus if mouse is moved outside of it (QT bug?)
		//QFont font = QFontDialog::getFont(&bOK, m_TextFont, this, "",QFontDialog::DontUseNativeDialog);
       TextFont = QFontDialog::getFont(&bOK, s_TextFont, this);
#else
	TextFont = QFontDialog::getFont(&bOK, s_TextFont, this);
#endif

	if (bOK)
	{
		s_TextFont = TextFont;
		m_pctrlTextFont->setText(s_TextFont.family());
		m_pctrlTextFont->setFont(s_TextFont);
		m_pctrlTextClr->setFont(s_TextFont);
	}
}



void Settings::OnTableFont()
{
	bool bOK;
	QFont TableFont;
//	TableFont.setStyleHint(QFont::TypeWriter, QFont::OpenGLCompatible);

#ifdef Q_WS_MAC
	//20090604 Mac OS Native font dialog does not work well under QT 4.5.1
	//QFont font = QFontDialog::getFont(&ok, m_TextFont, this);
		//20110324 Works again under QT 4.6, though it loses focus if mouse is moved outside of it (QT bug?)
		//QFont font = QFontDialog::getFont(&bOK, m_TextFont, this, "",QFontDialog::DontUseNativeDialog);
       TableFont = QFontDialog::getFont(&bOK, s_TableFont, this);
#else
	TableFont = QFontDialog::getFont(&bOK, s_TableFont, this);
#endif

	if (bOK)
	{
		s_TableFont = TableFont;
		m_pctrlTableFont->setText(s_TableFont.family());
		m_pctrlTableFont->setFont(s_TableFont);
	}
}



void Settings::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("global_settings");
	{
		pSettings->setValue("LastDirName", s_LastDirName);

		pSettings->setValue("BackgroundColor", s_BackgroundColor);
		pSettings->setValue("TextColor", s_TextColor);

		pSettings->setValue("TextFontFamily", s_TextFont.family());
		pSettings->setValue("TextFontPointSize", s_TextFont.pointSize());
		pSettings->setValue("TextFontItalic", s_TextFont.italic());
		pSettings->setValue("TextFontBold", s_TextFont.bold());

		pSettings->setValue("TableFontFamily", s_TableFont.family());
		pSettings->setValue("TableFontPointSize", s_TableFont.pointSize());
		pSettings->setValue("TableFontItalic", s_TableFont.italic());
		pSettings->setValue("TableFontBold", s_TableFont.bold());

		pSettings->setValue("ReverseZoom", s_bReverseZoom);
		pSettings->setValue("AlphaChannel", s_bAlphaChannel);
		s_RefGraph.SaveSettings(pSettings);
	}
	pSettings->endGroup();

}


void Settings::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("global_settings");
	{
		s_LastDirName = pSettings->value("LastDirName").toString();

		s_BackgroundColor = pSettings->value("BackgroundColor", QColor(5,11,13)).value<QColor>();

		s_TextColor = pSettings->value("TextColor", QColor(237,237,237)).value<QColor>();

		s_TextFont = QFont(pSettings->value("TextFontFamily", "Courier").toString());
		s_TextFont.setPointSize(pSettings->value("TextFontPointSize", 10).toInt());
		s_TextFont.setItalic(pSettings->value("TextFontItalic", false).toBool());
		s_TextFont.setBold(pSettings->value("TextFontBold", false).toBool());
		s_TextFont.setStyleStrategy(QFont::OpenGLCompatible);

		s_TableFont = QFont(pSettings->value("TableFontFamily", "Courier").toString());
		s_TableFont.setPointSize(pSettings->value("TableFontPointSize", 10).toInt());
		s_TableFont.setItalic(pSettings->value("TableFontItalic", false).toBool());
		s_TableFont.setBold(pSettings->value("TableFontBold", false).toBool());

		s_bReverseZoom  = pSettings->value("ReverseZoom", false).toBool();
		s_bAlphaChannel = pSettings->value("AlphaChannel", true).toBool();


		s_RefGraph.LoadSettings(pSettings);
	}
	pSettings->endGroup();

}



void Settings::OnReverseZoom()
{
	s_bReverseZoom = m_pctrlReverseZoom->isChecked();
}


void Settings::OnAlphaChannel()
{
	s_bAlphaChannel = m_pctrlAlphaChannel->isChecked();
}
