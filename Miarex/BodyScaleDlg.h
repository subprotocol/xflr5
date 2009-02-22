/****************************************************************************

	BodyScaleDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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


#ifndef BODYSCALEDLG_H
#define BODYSCALEDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include "../Misc/FloatEdit.h"


class BodyScaleDlg : public QDialog
{
	Q_OBJECT

	friend class GL3dBodyDlg;
	friend class CBody;
	friend class MainFrame;

public:
	BodyScaleDlg(void *pParent = NULL);

private slots:
	void OnOK();
	void OnRadio();
	void OnEditingFinished();

private:
	void SetupLayout();
	void InitDialog();
	void EnableControls();

private:
	static void *s_pMainFrame;

	QRadioButton *m_pctrlBody, *m_pctrlFrame;
	FloatEdit *m_pctrlXScaleFactor;
	FloatEdit *m_pctrlYScaleFactor;
	FloatEdit *m_pctrlZScaleFactor;
	FloatEdit *m_pctrlFrameID;


private:
	void *m_pGL3dBodyDlg;
	double m_XFactor, m_YFactor, m_ZFactor;
	bool m_bFrameOnly;
	int m_FrameID;
};

#endif // BODYGRIDDLG_H
