/****************************************************************************

	DisplaySettingsDlg Class
	Copyright (C) 2008-2008 Andre Deperrois adeperrois@xflr5.com

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
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "ColorButton.h"
#include "../graph/QGraph.h"


class DisplaySettingsDlg : public QDialog
{
	Q_OBJECT

	friend class MainFrame;
public:
	DisplaySettingsDlg(void *pParent=NULL);
	void InitDialog();

private slots:
	void OnStyleChanged(const QString &StyleName);
	void OnBackgroundColor();
	void OnGraphSettings();
	void OnTextColor();
	void OnTextFont();

private:
	void reject();
	void SetupLayout();

	ColorButton *m_pctrlBackColor;
	QPushButton *m_pctrlTextClr;
	QPushButton *m_pctrlTextFont;
	QPushButton *m_pctrlGraphSettings;
	QPushButton *OKButton, *CancelButton;
	QCheckBox *m_pctrlReverseZoom, *m_pctrlAlphaChannel;

	QComboBox *m_pctrlStyles;
	QString m_StyleName;
	QPushButton *OK, *Cancel;

	void *m_pMainFrame;
	QColor m_BackgroundColor;
	QColor m_TextColor;
	QFont m_TextFont;
	QGraph *m_pRefGraph;
	QGraph m_MemGraph;
	bool m_bIsGraphModified;
	bool m_bReverseZoom, m_bAlphaChannel;

};


#endif // DisplaySettingsDlg_H









