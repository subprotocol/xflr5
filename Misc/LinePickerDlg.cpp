/****************************************************************************

	LinePicker Class
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


#include <QtGui>
#include "LinePickerDlg.h"
#include "../Globals.h"

LinePickerDlg::LinePickerDlg(void *pParent)
{
	m_pParent = pParent;
	m_Style  = 0;
	m_Width  = 1;
	m_Color  = QColor(0,255,0);
	SetupLayout();

	m_pStyleDelegate = new LineDelegate(this);//will intercept painting operations
	m_pWidthDelegate = new LineDelegate(this);//will intercept painting operations

	m_pctrlStyle->setItemDelegate(m_pStyleDelegate);
	m_pctrlWidth->setItemDelegate(m_pWidthDelegate);

	connect(m_pctrlColor, SIGNAL(clicked()), this, SLOT(OnColor()));
	connect(m_pctrlStyle, SIGNAL(activated(int)), this, SLOT(OnStyle(int)));
	connect(m_pctrlWidth, SIGNAL(activated(int)), this, SLOT(OnWidth(int)));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void LinePickerDlg::InitDialog(int style, int width, QColor color)
{
	m_Color = color;
	m_Width = width;
	m_Style = style;
	QString str;
	for (int i=0; i<5; i++)
	{
		str = QString("%1").arg(i);
		m_pctrlWidth->addItem(str);
	}
	m_pctrlStyle->addItem("solid");//string doesn't matter
	m_pctrlStyle->addItem("dash");
	m_pctrlStyle->addItem("dot");
	m_pctrlStyle->addItem("dashdot");
	m_pctrlStyle->addItem("dashdotdot");

	FillBoxes();
}


void LinePickerDlg::SetupLayout()
{
	QGridLayout *StyleLayout = new QGridLayout;
	QLabel *lab1 = new QLabel("Style");
	QLabel *lab2 = new QLabel("Width");
	QLabel *lab3 = new QLabel("Color");
	lab1->setAlignment(Qt::AlignRight);
	lab2->setAlignment(Qt::AlignRight);
	lab3->setAlignment(Qt::AlignRight);
	lab1->setMinimumWidth(60);
	lab2->setMinimumWidth(60);
	lab3->setMinimumWidth(60);
	m_pctrlColor = new LineButton;
	m_pctrlStyle = new LineCbBox;
	m_pctrlWidth = new LineCbBox;
	m_pctrlStyle->setMinimumWidth(140);
	m_pctrlWidth->setMinimumWidth(140);
	m_pctrlColor->setMinimumWidth(140);

	StyleLayout->addWidget(lab1,1,1);
	StyleLayout->addWidget(lab2,2,1);
	StyleLayout->addWidget(lab3,3,1);
	StyleLayout->addWidget(m_pctrlStyle,1,2);
	StyleLayout->addWidget(m_pctrlWidth,2,2);
	StyleLayout->addWidget(m_pctrlColor,3,2);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(StyleLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setMinimumHeight(170);
	setLayout(MainLayout);
}



void LinePickerDlg::OnWidth(int val)
{
	m_Width = val+1;
	FillBoxes();
	repaint();
}


void LinePickerDlg::OnStyle(int val)
{
	m_Style = val;
	FillBoxes();
	repaint();
}


void LinePickerDlg::FillBoxes()
{
	m_pctrlStyle->SetLine(m_Style, m_Width, m_Color);
	m_pctrlWidth->SetLine(m_Style, m_Width, m_Color);

	m_pctrlColor->SetStyle(m_Style);
	m_pctrlColor->SetWidth(m_Width);

	m_pctrlColor->SetColor(m_Color);
	m_pStyleDelegate->SetLineColor(m_Color);
	m_pWidthDelegate->SetLineColor(m_Color);

	int LineWidth[5];
	for (int i=0; i<5;i++) LineWidth[i] = m_Width;
	m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles


	int LineStyle[5];
	for (int i=0; i<5;i++) LineStyle[i] = m_Style;
	m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
}


void LinePickerDlg::OnColor()
{
	bool bOK = true;

	QRgb rgb = m_Color.rgba();
	rgb = QColorDialog::getRgba(rgb, &bOK);
	m_Color = QColor::fromRgba(rgb);

	FillBoxes();
	repaint();
}



int & LinePickerDlg::GetWidth()
{
	return m_Width;
}


int & LinePickerDlg::GetStyle()
{
	return m_Style;
}


QColor & LinePickerDlg::GetColor()
{
	return m_Color;
}









