/****************************************************************************

	W3dPrefsDlg Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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


#ifndef W3DPREFSDLG_H
#define W3DPREFSDLG_H

#include <QDialog>
#include <QCheckBox>
#include "../Misc/LineButton.h"
#include "../Misc/ColorButton.h"



class W3dPrefsDlg : public QDialog
{
	Q_OBJECT

	friend class QMiarex;

public:
    W3dPrefsDlg();
	void InitDialog();

private slots:
	void On3DAxis();
	void OnWingColor();
	void OnTopTrans();
	void OnBotTrans();
	void OnXCP();
	void OnMoments();
	void OnIDrag();
	void OnVDrag();
	void OnDownwash();
	void OnWakePanels();
	void OnStreamLines();
	void OnVLMMesh();
	void OnShowWake();
	void OnMasses();

private:
	void SetupLayout();

	void *m_pMiarex;

	LineButton *m_pctrlAxis, *m_pctrlOutline, *m_pctrlVLMMesh, *m_pctrlTopTrans, *m_pctrlBotTrans;
	LineButton *m_pctrlLift, *m_pctrlMoments, *m_pctrlInducedDrag, *m_pctrlViscousDrag, *m_pctrlDownwash;
	LineButton *m_pctrlStreamLines, *m_pctrlWakePanels;

	ColorButton *m_pctrlMassColor;

	QCheckBox *m_pctrlShowWake;

	int m_3DAxisStyle, m_3DAxisWidth;
	QColor m_3DAxisColor;
	int m_VLMStyle, m_VLMWidth;
	QColor m_VLMColor;
	int m_OutlineStyle, m_OutlineWidth;
	QColor m_OutlineColor;
	int m_TopStyle, m_TopWidth;
	QColor m_TopColor;
	int m_BotStyle, m_BotWidth;
	QColor m_BotColor;
	int m_XCPStyle, m_XCPWidth;
	QColor m_XCPColor;
	int m_MomentStyle, m_MomentWidth;
	QColor m_MomentColor;
	int m_IDragStyle, m_IDragWidth;
	QColor m_IDragColor;
	int m_VDragStyle, m_VDragWidth;
	QColor m_VDragColor;
	int m_DownwashStyle, m_DownwashWidth;
	QColor m_DownwashColor;
	int m_StreamLinesStyle, m_StreamLinesWidth;
	QColor m_StreamLinesColor;
	int m_WakeStyle, m_WakeWidth;
	QColor m_WakeColor;

	QColor m_MassColor;

	bool m_bWakePanels;

};

#endif // W3DPREFSDLG_H
