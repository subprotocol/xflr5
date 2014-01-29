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
 


#ifndef XSETTINGS_H
#define XSETTINGS_H

#include <QDir>
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "ColorButton.h"
#include "TextClrBtn.h"
#include "../graph/QGraph.h"


class Settings : public QDialog
{
	Q_OBJECT

	friend class MainFrame;
public:
	Settings(QWidget *pParent);
	void InitDialog();
	static void LoadSettings(QSettings *settings);
	static void SaveSettings(QSettings *settings);

private slots:
	void OnStyleChanged(const QString &StyleName);
	void OnBackgroundColor();
	void OnGraphSettings();
	void OnTextColor();
	void OnTextFont();
	void OnTableFont();
	void OnReverseZoom();
	void OnAlphaChannel();

private:
	void reject();
	void SetupLayout();

	ColorButton *m_pctrlBackColor;
	TextClrBtn *m_pctrlTextClr;
	QPushButton *m_pctrlTextFont, *m_pctrlTableFont;
	QPushButton *m_pctrlGraphSettings;
	QPushButton *OKButton;
	QCheckBox *m_pctrlReverseZoom, *m_pctrlAlphaChannel;

	QComboBox *m_pctrlStyles;
	QPushButton *OK, *Cancel;

	void *m_pMainFrame;
	QDir m_StyleSheetDir;
	QGraph m_MemGraph;
	bool m_bIsGraphModified;

public:
	//settings variables used throughout the program
	static QString s_StyleName, s_StyleSheetName;
	static QFont s_TextFont, s_TableFont;
	static QColor s_BackgroundColor;
	static QColor s_TextColor;
	static bool s_bStyleSheets;
	static bool s_bReverseZoom, s_bAlphaChannel;
	static enumTextFileType s_ExportFileType;  /**< Defines if the list separator for the output text files should be a space or a comma. */
	static QGraph s_RefGraph;//Reference setttings
	static QString s_LastDirName;
};


#endif // XSettings_H


