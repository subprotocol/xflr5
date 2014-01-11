/****************************************************************************

	QXDirectStyleDlg Class
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

#include "XDirectStyleDlg.h"
#include "XDirect.h"
#include "../misc/LinePickerDlg.h"
#include <QGridLayout>
#include <QLabel>



XDirectStyleDlg::XDirectStyleDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("XDirect Styles"));
	m_pXDirect = NULL;

	SetupLayout();

	OnRestoreDefaults();
}


void XDirectStyleDlg::SetupLayout()
{
	QGridLayout *StyleLayout = new QGridLayout;
	{
		m_pctrlNeutral  = new LineBtn(this);
		m_pctrlBL       = new LineBtn(this);
		m_pctrlPressure = new LineBtn(this);
		QLabel *lab1 = new QLabel(tr("Neutral Line"));
		QLabel *lab2 = new QLabel(tr("Boundary Layer"));
		QLabel *lab3 = new QLabel(tr("Pressure"));
		StyleLayout->addWidget(lab1,1,1);
		StyleLayout->addWidget(lab2,2,1);
		StyleLayout->addWidget(lab3,3,1);
		StyleLayout->addWidget(m_pctrlNeutral,1,2);
		StyleLayout->addWidget(m_pctrlBL,2,2);
		StyleLayout->addWidget(m_pctrlPressure,3,2);
        connect(m_pctrlNeutral,  SIGNAL(clickedLB()),this, SLOT(OnNeutralStyle()));
        connect(m_pctrlBL,       SIGNAL(clickedLB()),this, SLOT(OnBLStyle()));
        connect(m_pctrlPressure, SIGNAL(clickedLB()),this, SLOT(OnPressureStyle()));
	}

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton = new QPushButton(tr("OK"));
		QPushButton *DefaultsButton = new QPushButton(tr("Defaults"));
		QPushButton *CancelButton   = new QPushButton(tr("Cancel"));
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(DefaultsButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
		connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
		connect(DefaultsButton, SIGNAL(clicked()),this, SLOT(OnRestoreDefaults()));
		connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	}

	QVBoxLayout *MainLayout = new QVBoxLayout;
	{
		MainLayout->addLayout(StyleLayout);
		MainLayout->addStretch(1);
		MainLayout->addLayout(CommandButtons);
		MainLayout->addStretch(1);
	}

	setLayout(MainLayout);



}


void XDirectStyleDlg::OnNeutralStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_iNeutralStyle, m_iNeutralWidth, m_crNeutralColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_iNeutralStyle = dlg.GetStyle();
		m_iNeutralWidth = dlg.GetWidth();
		m_crNeutralColor = dlg.GetColor();
		m_pctrlNeutral->SetStyle(m_iNeutralStyle, m_iNeutralWidth, m_crNeutralColor);
	}
	OKButton->setFocus();
}


void XDirectStyleDlg::OnPressureStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_iPressureStyle, m_iPressureWidth, m_crPressureColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_iPressureStyle = dlg.GetStyle();
		m_iPressureWidth = dlg.GetWidth();
		m_crPressureColor = dlg.GetColor();
		m_pctrlPressure->SetStyle(m_iPressureStyle, m_iPressureWidth, m_crPressureColor);
	}
	OKButton->setFocus();
}


void XDirectStyleDlg::OnBLStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_iBLStyle, m_iBLWidth, m_crBLColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_iBLStyle = dlg.GetStyle();
		m_iBLWidth = dlg.GetWidth();
		m_crBLColor = dlg.GetColor();
		m_pctrlBL->SetStyle(m_iBLStyle, m_iBLWidth, m_crBLColor);
	}

	OKButton->setFocus();
}



void XDirectStyleDlg::InitDialog()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	m_iNeutralStyle  = pXDirect->m_iNeutralStyle;
	m_iNeutralWidth  = pXDirect->m_iNeutralWidth;
	m_crNeutralColor = pXDirect->m_crNeutralColor;

	m_iBLStyle  = pXDirect->m_iBLStyle;
	m_iBLWidth  = pXDirect->m_iBLWidth;
	m_crBLColor = pXDirect->m_crBLColor;

	m_iPressureStyle  = pXDirect->m_iPressureStyle;
	m_iPressureWidth  = pXDirect->m_iPressureWidth;
	m_crPressureColor = pXDirect->m_crPressureColor;

	m_pctrlNeutral->SetStyle(m_iNeutralStyle, m_iNeutralWidth, m_crNeutralColor);
	m_pctrlBL->SetStyle(m_iBLStyle, m_iBLWidth, m_crBLColor);
	m_pctrlPressure->SetStyle(m_iPressureStyle, m_iPressureWidth, m_crPressureColor);
}

void XDirectStyleDlg::OnRestoreDefaults()
{
	m_iNeutralStyle = 2;
	m_iNeutralWidth = 1;
	m_crNeutralColor = QColor(200,200,255);
	m_pctrlNeutral->SetStyle(m_iNeutralStyle, m_iNeutralWidth, m_crNeutralColor);

	m_crBLColor = QColor(200,70,70);
	m_iBLStyle = 1;
	m_iBLWidth = 1;
	m_pctrlBL->SetStyle(m_iBLStyle, m_iBLWidth, m_crBLColor);

	m_crPressureColor= QColor(0,255,0);
	m_iPressureStyle = 0;
	m_iPressureWidth = 1;
	m_pctrlPressure->SetStyle(m_iPressureStyle, m_iPressureWidth, m_crPressureColor);
}


void XDirectStyleDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus())
			{
				OKButton->setFocus();
				return;
			}
			else
			{
				accept();
				return;
			}
			event->ignore();
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			break;
		}
	}
}

