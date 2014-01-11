/****************************************************************************

	AFoilGridDlg Class
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

#include "AFoil.h"
#include "AFoilGridDlg.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "../misc/LinePickerDlg.h"

AFoilGridDlg::AFoilGridDlg(QWidget *pParent): QDialog(pParent)
{
	setWindowTitle(tr("Grid Options"));
	m_pAFoil = pParent;

	m_bScale = false;

	m_bXGrid     = false;
	m_XUnit  = 0.05;
	m_XStyle = 1;
	m_XWidth = 1;
	m_XColor = QColor(150,150,150);

	m_bYGrid     = false;
	m_YUnit  = 0.05;
	m_YStyle = 1;
	m_YWidth = 1;
	m_YColor = QColor(150,150,150);

	m_bXMinGrid  = false;
	m_XMinUnit = 0.01;
	m_XMinStyle  = 2;
	m_XMinWidth  = 1;
	m_XMinColor  = QColor(70,70,70);

	m_bYMinGrid  = false;
	m_YMinUnit = 0.01;
	m_YMinStyle  = 2;
	m_YMinWidth  = 1;
	m_YMinColor  = QColor(70,70,70);

	m_NeutralStyle = 3;
	m_NeutralWidth = 1;
	m_NeutralColor = QColor(70,70,70);

	SetupLayout();

	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlScale, SIGNAL(clicked()), this, SLOT(OnScale()));
	connect(m_pctrlNeutralShow, SIGNAL(clicked(bool)), this, SLOT(OnNeutralShow(bool)));
	connect(m_pctrlXMajShow, SIGNAL(clicked(bool)), this, SLOT(OnXMajShow(bool)));
	connect(m_pctrlYMajShow, SIGNAL(clicked(bool)), this, SLOT(OnYMajShow(bool)));
	connect(m_pctrlXMinShow, SIGNAL(clicked(bool)), this, SLOT(OnXMinShow(bool)));
	connect(m_pctrlYMinShow, SIGNAL(clicked(bool)), this, SLOT(OnYMinShow(bool)));
    connect(m_pctrlNeutralStyle, SIGNAL(clickedLB()), this, SLOT(OnNeutralStyle()));
    connect(m_pctrlXMajStyle, SIGNAL(clickedLB()), this, SLOT(OnXMajStyle()));
    connect(m_pctrlYMajStyle, SIGNAL(clickedLB()), this, SLOT(OnYMajStyle()));
    connect(m_pctrlXMinStyle, SIGNAL(clickedLB()), this, SLOT(OnXMinStyle()));
    connect(m_pctrlYMinStyle, SIGNAL(clickedLB()), this, SLOT(OnYMinStyle()));

}

void AFoilGridDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			done(0);
			break;
		}
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OnApply();
				OKButton->setFocus();
//				m_bApplied  = true;
			}
			else
			{
				QDialog::accept();
			}
			break;
		}
		default:
			event->ignore();
	}
}


void AFoilGridDlg::InitDialog()
{
	m_pctrlNeutralStyle->SetStyle(m_NeutralStyle, m_NeutralWidth, m_NeutralColor);

	m_pctrlXMajStyle->SetStyle(m_XStyle, m_XWidth, m_XColor);
	m_pctrlXMinStyle->SetStyle(m_XMinStyle, m_XMinWidth, m_XMinColor);
	m_pctrlYMajStyle->SetStyle(m_YStyle, m_YWidth, m_YColor);
	m_pctrlYMinStyle->SetStyle(m_YMinStyle, m_YMinWidth, m_YMinColor);

	m_pctrlNeutralStyle->setEnabled(m_bNeutralLine);
	m_pctrlXMajStyle->setEnabled(m_bXGrid);
	m_pctrlYMajStyle->setEnabled(m_bYGrid);
	m_pctrlXMinStyle->setEnabled(m_bXMinGrid);
	m_pctrlYMinStyle->setEnabled(m_bYMinGrid);
	m_pctrlXUnit->setEnabled(m_bXGrid);
	m_pctrlYUnit->setEnabled(m_bYGrid);
	m_pctrlXMinUnit->setEnabled(m_bXMinGrid);
	m_pctrlYMinUnit->setEnabled(m_bYMinGrid);

	m_pctrlScale->setChecked(m_bScale);
	m_pctrlNeutralShow->setChecked(m_bNeutralLine);
	m_pctrlXMajShow->setChecked(m_bXGrid);
	m_pctrlYMajShow->setChecked(m_bYGrid);
	m_pctrlXMinShow->setChecked(m_bXMinGrid);
	m_pctrlYMinShow->setChecked(m_bYMinGrid);

	m_pctrlXUnit->SetValue(m_XUnit);
	m_pctrlYUnit->SetValue(m_YUnit);
	m_pctrlXMinUnit->SetValue(m_XMinUnit);
	m_pctrlYMinUnit->SetValue(m_YMinUnit);
}


void AFoilGridDlg::SetupLayout()
{
	QGridLayout *GridData = new QGridLayout;
	{
		m_pctrlNeutralShow = new QCheckBox(tr("Neutral Line"));
		m_pctrlScale       = new QCheckBox(tr("X-Scale"));
		m_pctrlXMajShow = new QCheckBox(tr("X Major Grid"));
		m_pctrlYMajShow = new QCheckBox(tr("Y Major Grid"));
		m_pctrlXMinShow = new QCheckBox(tr("X Minor Grid"));
		m_pctrlYMinShow = new QCheckBox(tr("Y Minor Grid"));

		m_pctrlNeutralStyle = new LineBtn(this);
		m_pctrlXMajStyle = new LineBtn(this);
		m_pctrlYMajStyle = new LineBtn(this);
		m_pctrlXMinStyle = new LineBtn(this);
		m_pctrlYMinStyle = new LineBtn(this);

		m_pctrlXUnit = new DoubleEdit;
		m_pctrlYUnit = new DoubleEdit;
		m_pctrlXMinUnit = new DoubleEdit;
		m_pctrlYMinUnit = new DoubleEdit;
		m_pctrlXUnit->SetPrecision(3);
		m_pctrlYUnit->SetPrecision(3);
		m_pctrlXMinUnit->SetPrecision(3);
		m_pctrlYMinUnit->SetPrecision(3);

		GridData->addWidget(m_pctrlNeutralShow,1,1);
		GridData->addWidget(m_pctrlXMajShow,2,1);
		GridData->addWidget(m_pctrlYMajShow,3,1);
		GridData->addWidget(m_pctrlXMinShow,4,1);
		GridData->addWidget(m_pctrlYMinShow,5,1);

		GridData->addWidget(m_pctrlNeutralStyle,1,2);
		GridData->addWidget(m_pctrlXMajStyle,2,2);
		GridData->addWidget(m_pctrlYMajStyle,3,2);
		GridData->addWidget(m_pctrlXMinStyle,4,2);
		GridData->addWidget(m_pctrlYMinStyle,5,2);

		GridData->addWidget(m_pctrlScale,1,3);
		GridData->addWidget(m_pctrlXUnit,2,3);
		GridData->addWidget(m_pctrlYUnit,3,3);
		GridData->addWidget(m_pctrlXMinUnit,4,3);
		GridData->addWidget(m_pctrlYMinUnit,5,3);
	}

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		OKButton      = new QPushButton(tr("Accept"));
		CancelButton  = new QPushButton(tr("Cancel"));
		ApplyButton  = new QPushButton(tr("Apply"));

		CommandButtons->addStretch(1);
		CommandButtons->addWidget(ApplyButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(OKButton);
		CommandButtons->addStretch(1);
		CommandButtons->addWidget(CancelButton);
		CommandButtons->addStretch(1);
	}

	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		mainLayout->addLayout(GridData);
		mainLayout->addLayout(CommandButtons);
	}
	setLayout(mainLayout);
}


void AFoilGridDlg::OnNeutralStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_NeutralStyle,m_NeutralWidth,m_NeutralColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_NeutralStyle = dlg.GetStyle();
		m_NeutralWidth = dlg.GetWidth();
		m_NeutralColor = dlg.GetColor();
		m_pctrlNeutralStyle->SetStyle(dlg.GetStyle());
		m_pctrlNeutralStyle->SetWidth(dlg.GetWidth());
		m_pctrlNeutralStyle->SetColor(dlg.GetColor());
	}
}


void AFoilGridDlg::OnXMajStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_XStyle,m_XWidth,m_XColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_XStyle = dlg.GetStyle();
		m_XWidth = dlg.GetWidth();
		m_XColor = dlg.GetColor();
		m_pctrlXMajStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMajStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMajStyle->SetColor(dlg.GetColor());
	}
}

void AFoilGridDlg::OnXMinStyle()
{
    LinePickerDlg dlg(this);
	dlg.InitDialog(m_XMinStyle,m_XMinWidth,m_XMinColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_XMinStyle = dlg.GetStyle();
		m_XMinWidth = dlg.GetWidth();
		m_XMinColor = dlg.GetColor();
		m_pctrlXMinStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMinStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMinStyle->SetColor(dlg.GetColor());
	}

}

void AFoilGridDlg::OnYMajStyle()
{
    LinePickerDlg dlg(this);

	dlg.InitDialog(m_YStyle,m_YWidth,m_YColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_YStyle = dlg.GetStyle();
		m_YWidth = dlg.GetWidth();
		m_YColor = dlg.GetColor();
		m_pctrlYMajStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMajStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMajStyle->SetColor(dlg.GetColor());
	}
}

void AFoilGridDlg::OnYMinStyle()
{
    LinePickerDlg dlg(this);

	dlg.InitDialog(m_YMinStyle,m_YMinWidth,m_YMinColor);

	if(QDialog::Accepted==dlg.exec())
	{
		m_YMinStyle = dlg.GetStyle();
		m_YMinWidth = dlg.GetWidth();
		m_YMinColor = dlg.GetColor();
		m_pctrlYMinStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMinStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMinStyle->SetColor(dlg.GetColor());
	}
}

void AFoilGridDlg::OnNeutralShow(bool bShow)
{
	m_bNeutralLine = bShow;
	m_pctrlNeutralStyle->setEnabled(m_bNeutralLine);
}

void AFoilGridDlg::OnScale()
{
	m_bScale = m_pctrlScale->isChecked();
}


void AFoilGridDlg::OnXMajShow(bool bShow)
{
	m_bXGrid = bShow;
	m_pctrlXMajStyle->setEnabled(m_bXGrid);
	m_pctrlXUnit->setEnabled(m_bXGrid);
}





void AFoilGridDlg::OnYMajShow(bool bShow)
{
	m_bYGrid = bShow;
	m_pctrlYMajStyle->setEnabled(m_bYGrid);
	m_pctrlYUnit->setEnabled(m_bYGrid);
}


void AFoilGridDlg::OnXMinShow(bool bShow)
{
	m_bXMinGrid = bShow;
	m_pctrlXMinStyle->setEnabled(m_bXMinGrid);
	m_pctrlXMinUnit->setEnabled(m_bXMinGrid);
}


void AFoilGridDlg::OnYMinShow(bool bShow)
{
	m_bYMinGrid = bShow;
	m_pctrlYMinStyle->setEnabled(m_bYMinGrid);
	m_pctrlYMinUnit->setEnabled(m_bYMinGrid);
}

void AFoilGridDlg::OnOK()
{
	m_XUnit = m_pctrlXUnit->Value();
	m_YUnit = m_pctrlYUnit->Value();
	m_XMinUnit = m_pctrlXMinUnit->Value();
	m_YMinUnit = m_pctrlYMinUnit->Value();
	accept();
}

void AFoilGridDlg::OnApply()
{
	if(!m_pAFoil) return;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	m_XUnit = m_pctrlXUnit->Value();
	m_YUnit = m_pctrlYUnit->Value();
	m_XMinUnit = m_pctrlXMinUnit->Value();
	m_YMinUnit = m_pctrlYMinUnit->Value();

	pAFoil->m_bScale       = m_bScale;

	pAFoil->m_bNeutralLine = m_bNeutralLine;
	pAFoil->m_NeutralStyle = m_NeutralStyle;
	pAFoil->m_NeutralWidth = m_NeutralWidth;
	pAFoil->m_NeutralColor = m_NeutralColor;

	pAFoil->m_bXGrid     = m_bXGrid;
	pAFoil->m_bXMinGrid  = m_bXMinGrid;
	pAFoil->m_XGridStyle = m_XStyle;
	pAFoil->m_XGridWidth = m_XWidth;
	pAFoil->m_XGridColor = m_XColor;
	pAFoil->m_XGridUnit  = m_XUnit;
	pAFoil->m_XMinStyle  = m_XMinStyle;
	pAFoil->m_XMinWidth  = m_XMinWidth;
	pAFoil->m_XMinColor  = m_XMinColor;
	pAFoil->m_XMinUnit   = m_XMinUnit;

	pAFoil->m_bYGrid     = m_bYGrid;
	pAFoil->m_bYMinGrid  = m_bYMinGrid;
	pAFoil->m_YGridStyle = m_YStyle;
	pAFoil->m_YGridWidth = m_YWidth;
	pAFoil->m_YGridColor = m_YColor;
	pAFoil->m_YGridUnit  = m_YUnit;
	pAFoil->m_YMinStyle  = m_YMinStyle;
	pAFoil->m_YMinWidth  = m_YMinWidth;
	pAFoil->m_YMinColor  = m_YMinColor;
	pAFoil->m_YMinUnit   = m_YMinUnit;

	pAFoil->UpdateView();
}




