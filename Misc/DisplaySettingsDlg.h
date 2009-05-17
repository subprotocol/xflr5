/****************************************************************************

	DisplaySettingsDlg Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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
 


#ifndef DISPLAYSETTINGSDLG_H
#define DISPLAYSETTINGSDLG_H

#include <QDialog>
#include <QtGui>
#include "ColorButton.h"
#include "../Graph/QGraph.h"


class DisplaySettingsDlg : public QDialog
{
	Q_OBJECT

public:
	DisplaySettingsDlg(void *pParent=NULL);
	void SetupLayout();
	void InitDialog();

	ColorButton *m_pctrlBackColor;
	ColorButton *m_pctrlTextColor;
	QPushButton *m_pctrlTextFont;
	QPushButton *m_pctrlGraphSettings;
	QPushButton *OKButton, *CancelButton;

	QComboBox *m_pctrlStyles;
	QString m_StyleName;
	QPushButton *OK, *Cancel;

	void *m_pMainFrame;
	QColor m_BackgroundColor;
	QColor m_GraphBackColor;
	QColor m_TextColor;
	QFont m_TextFont;
	QGraph *m_pRefGraph;
	bool m_bIsGraphModified;

private slots:
	void OnStyleChanged(const QString &StyleName);
	void OnBackgroundColor();
	void OnGraphSettings();
	void OnTextColor();
	void OnTextFont();
};


#endif // DisplaySettingsDlg_H









