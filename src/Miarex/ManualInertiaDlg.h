/****************************************************************************

	ManualInertiaDlg Class
	Copyright (C) 2010 Andre Deperrois XFLR5@yahoo.com

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

#ifndef MANUALINERTIADLG_H
#define MANUALINERTIADLG_H

#include <QDialog>
#include <QPushButton>
#include "../Misc/FloatEdit.h"



class ManualInertiaDlg : public QDialog
{
	Q_OBJECT
	friend class StabPolarDlg;
	friend class MainFrame;	

public:
    ManualInertiaDlg();
	
private:
	void SetupLayout();
	void keyPressEvent(QKeyEvent *event);
	
private slots:
	void OnOK();	
	
private:
	QPushButton *OKButton, *CancelButton;
	
	static void *s_pMainFrame;
	static double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;
	static double m_CoGx, m_CoGz;
	static double m_Mass;

	FloatEdit *m_pctrlMass;
	FloatEdit *m_pctrlCoGx, *m_pctrlCoGz ;
	FloatEdit *m_pctrlCoGIxx,  *m_pctrlCoGIyy, *m_pctrlCoGIzz, *m_pctrlCoGIxz ;
};

#endif // MANUALINERTIADLG_H
