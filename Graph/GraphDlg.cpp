/****************************************************************************

	GraphDlg  Classes
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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

#include "GraphDlg.h"
#include "../Misc/LinePickerDlg.h"
#include "../MainFrame.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>

void *GraphDlg::s_pMainFrame;


GraphDlg::GraphDlg()
{
	setWindowTitle("Graph Settings");

	m_pMemGraph = NULL;
	m_pGraph    = NULL;
	m_NGraph      = 0 ;
	m_bApplied = true;

	m_pTitleFont = m_pLegendFont = m_pLabelFont = NULL;

	SetupLayout();
	Connect();
}


void GraphDlg::Connect()
{
	connect(m_pctrlTitleColor, SIGNAL(clicked()),  this, SLOT(OnTitleColor()));
	connect(m_pctrlLabelColor, SIGNAL(clicked()),  this, SLOT(OnLabelColor()));
	connect(m_pctrlLegendColor, SIGNAL(clicked()), this, SLOT(OnLegendColor()));

	connect(m_pctrlTitleButton, SIGNAL(clicked()),  this, SLOT(OnTitleFont()));
	connect(m_pctrlLabelButton, SIGNAL(clicked()),  this, SLOT(OnLabelFont()));
	connect(m_pctrlLegendButton, SIGNAL(clicked()), this, SLOT(OnLegendFont()));

	connect(m_pctrlXAuto, SIGNAL(clicked()), this, SLOT(OnAutoX()));
	connect(m_pctrlYAuto, SIGNAL(clicked()), this, SLOT(OnAutoY()));
	connect(m_pctrlYInverted, SIGNAL(clicked()), this, SLOT(OnYInverted()));

	connect(m_pctrlXMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMajGridShow(int)));
	connect(m_pctrlYMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMajGridShow(int)));
	connect(m_pctrlXMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMinGridShow(int)));
	connect(m_pctrlYMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMinGridShow(int)));

	connect(m_pctrlAxisStyle, SIGNAL(clicked()), this, SLOT(OnAxisStyle()));
	connect(m_pctrlXMajGridStyle, SIGNAL(clicked()), this, SLOT(OnXMajGridStyle()));
	connect(m_pctrlYMajGridStyle, SIGNAL(clicked()), this, SLOT(OnYMajGridStyle()));
	connect(m_pctrlXMinGridStyle, SIGNAL(clicked()), this, SLOT(OnXMinGridStyle()));
	connect(m_pctrlYMinGridStyle, SIGNAL(clicked()), this, SLOT(OnYMinGridStyle()));

	connect(m_pctrlGraphBorder, SIGNAL(stateChanged(int)), this, SLOT(OnGraphBorder(int)));
	connect(m_pctrlGraphBack, SIGNAL(clicked()), this, SLOT(OnGraphBackColor()));
	connect(m_pctrlBorderStyle, SIGNAL(clicked()), this, SLOT(OnBorderStyle()));

	connect(RestoreButton, SIGNAL(clicked()),this, SLOT(OnRestoreParams()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ApplyButton, SIGNAL(clicked()), this, SLOT(OnApply()));

}

void GraphDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	// Generate the foil instead
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else
			{
				QDialog::accept();
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			break;
		}
		default:
			event->ignore();
	}
}


void GraphDlg::OnApply()
{
//	if(!m_pMemGraph) return;
//	m_pMemGraph->CopySettings(m_pGraph);
	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pGraph);
	}

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->UpdateView();
	SetApplied(true);
}


void GraphDlg::OnAutoX()
{
	bool bAuto = m_pctrlXAuto->checkState() == Qt::Checked;
	m_pctrlXMin->setEnabled(!bAuto);
	m_pctrlXMax->setEnabled(!bAuto);
	m_pctrlXUnit->setEnabled(!bAuto);
	m_pctrlXOrigin->setEnabled(!bAuto);
	SetApplied(false);
}


void GraphDlg::OnAutoY()
{
	bool bAuto = m_pctrlYAuto->checkState() == Qt::Checked;
	m_pctrlYMin->setEnabled(!bAuto);
	m_pctrlYMax->setEnabled(!bAuto);
	m_pctrlYUnit->setEnabled(!bAuto);
	m_pctrlYOrigin->setEnabled(!bAuto);
	SetApplied(false);
}

void GraphDlg::OnAxisStyle()
{
	LinePickerDlg dlg;
	dlg.InitDialog(m_pGraph->GetAxisStyle(), m_pGraph->GetAxisWidth(), m_pGraph->GetAxisColor());

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetAxisData(dlg.GetStyle(), dlg.GetWidth(), dlg.GetColor());
		m_pctrlAxisStyle->SetStyle(dlg.GetStyle());
		m_pctrlAxisStyle->SetWidth(dlg.GetWidth());
		m_pctrlAxisStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnBorderStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	s = m_pGraph->GetBorderStyle();
	w = m_pGraph->GetBorderWidth();
	color = m_pGraph->GetBorderColor();
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetBorderColor(dlg.GetColor());
		m_pGraph->SetBorderStyle(dlg.GetStyle());
		m_pGraph->SetBorderWidth(dlg.GetWidth());
		m_pctrlBorderStyle->SetStyle(dlg.GetStyle());
		m_pctrlBorderStyle->SetWidth(dlg.GetWidth());
		m_pctrlBorderStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnGraphBorder(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetBorder(bShow);
	SetApplied(false);
}

void GraphDlg::OnGraphBackColor()
{
	QColor BkColor = m_pGraph->GetBackColor();

	bool bOK;
	QRgb rgb = BkColor.rgba();
	rgb = QColorDialog::getRgba(rgb, &bOK);
	BkColor = QColor::fromRgba(rgb);

	m_pGraph->SetBkColor(BkColor);
	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());
	SetApplied(false);
}


void GraphDlg::OnLabelColor()
{
	QColor color = m_pGraph->GetLabelColor();
	m_pGraph->SetLabelColor(QColorDialog::getRgba(color.rgba()));
	m_pctrlLabelColor->SetColor(m_pGraph->GetLabelColor());
	SetApplied(false);
}



void GraphDlg::OnLabelFont()
{
	bool ok;
	QFont LabelFont("Courier");
	m_pGraph->GetLabelLogFont(&LabelFont);
	QFont font = QFontDialog::getFont(&ok, LabelFont, this);

	if (ok)
	{
	   m_pctrlLabelButton->setFont(font);
	   m_pctrlLabelButton->setText(font.family());
	   m_pGraph->SetLabelLogFont(&font);
		SetApplied(false);
	}
}



void GraphDlg::OnLegendColor()
{
	QColor color = m_pGraph->GetLabelColor();
	m_pGraph->SetLegendColor(QColorDialog::getRgba(color.rgba()));
	m_pctrlLegendColor->SetColor(m_pGraph->GetLegendColor());
	SetApplied(false);
}



void GraphDlg::OnLegendFont()
{
	bool ok;
	QFont LegendFont("Comic Sans MS");
	m_pGraph->GetLegendLogFont(&LegendFont);

	QFont font = QFontDialog::getFont(&ok, LegendFont, this);

	if (ok)
	{
	   m_pctrlLegendButton->setFont(font);
	   m_pctrlLegendButton->setText(font.family());
	   m_pGraph->SetLegendLogFont(&font);
		SetApplied(false);
	}
}




void GraphDlg::OnOK()
{
	m_pGraph->SetAutoX(m_pctrlXAuto->isChecked());
	m_pGraph->SetXMin(m_pctrlXMin->GetValue());
	m_pGraph->SetXMax(m_pctrlXMax->GetValue());
	m_pGraph->SetX0(m_pctrlXOrigin->GetValue());
	m_pGraph->SetXUnit(m_pctrlXUnit->GetValue());

	m_pGraph->SetAutoY(m_pctrlYAuto->isChecked());
	m_pGraph->SetYMin(m_pctrlYMin->GetValue());
	m_pGraph->SetYMax(m_pctrlYMax->GetValue());
	m_pGraph->SetY0(m_pctrlYOrigin->GetValue());
	m_pGraph->SetYUnit(m_pctrlYUnit->GetValue());

	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pGraph);
	}
	accept();
}

void GraphDlg::OnRestoreParams()
{
	m_pGraph->CopySettings(m_pMemGraph);

	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pMemGraph);
	}

	SetParams();
	SetApplied(true);

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->UpdateView();
}


void GraphDlg::OnTitleColor()
{
	QColor color = m_pGraph->GetTitleColor();
	m_pGraph->SetTitleColor(QColorDialog::getRgba(color.rgba()));
	m_pctrlTitleColor->SetColor(m_pGraph->GetTitleColor());
	SetApplied(false);
}


void GraphDlg::OnTitleFont()
{
	bool ok;
	QFont TitleFont("Arial");
	m_pGraph->GetTitleLogFont(&TitleFont);

	QFont font = QFontDialog::getFont(&ok, TitleFont, this);

	if (ok)
	{
	   m_pctrlTitleButton->setFont(font);
	   m_pctrlTitleButton->setText(font.family());
	   m_pGraph->SetTitleLogFont(&font);
		SetApplied(false);
	}
}



void GraphDlg::OnXMajGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow;
	m_pGraph->GetXMajGrid(bShow,color,s,w);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetXMajGrid(bShow, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth());
		m_pctrlXMajGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMajGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMajGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}

void GraphDlg::OnXMinGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow, bAuto;
	double unit;
	m_pGraph->GetXMinGrid(bShow, bAuto,color,s,w,unit);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetXMinGrid(bShow, bAuto, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth(),unit);
		m_pctrlXMinGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlXMinGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlXMinGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}

}


void GraphDlg::OnXMajGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetXMajGrid(bShow);
	m_pctrlXMajGridStyle->setEnabled(bShow);
	SetApplied(false);
}


void GraphDlg::OnXMinGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetXMinGrid(bShow);
	m_pctrlXMinGridStyle->setEnabled(bShow);
	SetApplied(false);
}


void GraphDlg::OnYInverted()
{
	m_pGraph->SetInverted(m_pctrlYInverted->checkState() == Qt::Checked);
	SetApplied(false);
}


void GraphDlg::OnYMajGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetYMajGrid(bShow);
	m_pctrlYMajGridStyle->setEnabled(bShow);
	SetApplied(false);
}

void GraphDlg::OnYMajGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow;
	m_pGraph->GetYMajGrid(bShow,color,s,w);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetYMajGrid(bShow, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth());
		m_pctrlYMajGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMajGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMajGridStyle->SetColor(dlg.GetColor());
	}
}


void GraphDlg::OnYMinGridShow(int state)
{
	bool bShow = (state==Qt::Checked);
	m_pGraph->SetYMinGrid(bShow);
	m_pctrlYMinGridStyle->setEnabled(bShow);
	SetApplied(false);
}


void GraphDlg::OnYMinGridStyle()
{
	LinePickerDlg dlg;
	int s,w;
	QColor color;
	bool bShow, bAuto;
	double unit;
	m_pGraph->GetYMinGrid(bShow, bAuto,color,s,w,unit);
	dlg.InitDialog(s,w,color);

	if(QDialog::Accepted==dlg.exec())
	{
		m_pGraph->SetYMinGrid(bShow, bAuto, dlg.GetColor(), dlg.GetStyle(), dlg.GetWidth(),unit);
		m_pctrlYMinGridStyle->SetStyle(dlg.GetStyle());
		m_pctrlYMinGridStyle->SetWidth(dlg.GetWidth());
		m_pctrlYMinGridStyle->SetColor(dlg.GetColor());
		SetApplied(false);
	}
}


void GraphDlg::SetApplied(bool bApplied)
{
	m_bApplied = bApplied;
	ApplyButton->setEnabled(!bApplied);
}

void GraphDlg::SetParams()
{
	QString strong;

	m_pctrlXAuto->setChecked(m_pGraph->GetAutoX());
	m_pctrlYAuto->setChecked(m_pGraph->GetAutoY());

	m_pctrlXMin->SetValue(m_pGraph->GetXMin());
	m_pctrlXMax->SetValue(m_pGraph->GetXMax());
	m_pctrlXOrigin->SetValue(m_pGraph->GetX0());
	m_pctrlXUnit->SetValue(m_pGraph->GetXUnit());
	m_pctrlYMin->SetValue(m_pGraph->GetYMin());
	m_pctrlYMax->SetValue(m_pGraph->GetYMax());
	m_pctrlYOrigin->SetValue(m_pGraph->GetY0());
	m_pctrlYUnit->SetValue(m_pGraph->GetYUnit());

	OnAutoX();
	OnAutoY();

	m_pctrlTitleColor->SetColor(m_pGraph->GetTitleColor());
	m_pctrlLabelColor->SetColor(m_pGraph->GetLabelColor());
	m_pctrlLegendColor->SetColor(m_pGraph->GetLegendColor());

	QFont font;
	m_pGraph->GetLabelLogFont(&font);
	m_pctrlLabelButton->setText(font.family());
	m_pctrlLabelButton->setFont(font);

	m_pGraph->GetTitleLogFont(&font);
	m_pctrlTitleButton->setText(font.family());
	m_pctrlTitleButton->setFont(font);

	m_pGraph->GetLegendLogFont(&font);
	m_pctrlLegendButton->setText(font.family());
	m_pctrlLegendButton->setFont(font);


	bool bState, bAuto;
	QColor color;
	int style, width;
	double unit;

	m_pGraph->GetXMajGrid(bState, color, style, width);
	m_pctrlXMajGridShow->setChecked(bState);
	m_pctrlXMajGridStyle->SetColor(color);
	m_pctrlXMajGridStyle->SetStyle(style);
	m_pctrlXMajGridStyle->SetWidth(width);
	m_pctrlXMajGridStyle->setEnabled(bState);

	m_pGraph->GetXMinGrid(bState, bAuto,color, style, width, unit);
	m_pctrlXMinGridShow->setChecked(bState);
	m_pctrlXMinGridStyle->SetColor(color);
	m_pctrlXMinGridStyle->SetStyle(style);
	m_pctrlXMinGridStyle->SetWidth(width);
	m_pctrlXMinGridStyle->setEnabled(bState);
	m_pctrlXMinorUnit->SetValue(unit);

	m_pGraph->GetYMajGrid(bState, color, style, width);
	m_pctrlYMajGridShow->setChecked(bState);
	m_pctrlYMajGridStyle->SetColor(color);
	m_pctrlYMajGridStyle->SetStyle(style);
	m_pctrlYMajGridStyle->SetWidth(width);
	m_pctrlYMajGridStyle->setEnabled(bState);

	m_pGraph->GetYMinGrid(bState, bAuto,color, style, width, unit);
	m_pctrlYMinGridShow->setChecked(bState);
	m_pctrlYMinGridStyle->SetColor(color);
	m_pctrlYMinGridStyle->SetStyle(style);
	m_pctrlYMinGridStyle->SetWidth(width);
	m_pctrlYMinGridStyle->setEnabled(bState);
	m_pctrlYMinorUnit->SetValue(unit);

	m_pctrlAxisStyle->SetColor(m_pGraph->GetAxisColor());
	m_pctrlAxisStyle->SetStyle(m_pGraph->GetAxisStyle());
	m_pctrlAxisStyle->SetWidth(m_pGraph->GetAxisWidth());

	m_pctrlGraphBorder->setChecked(m_pGraph->GetBorder());
	m_pctrlBorderStyle->SetColor(m_pGraph->GetBorderColor());
	m_pctrlBorderStyle->SetStyle(m_pGraph->GetBorderStyle());
	m_pctrlBorderStyle->SetWidth(m_pGraph->GetBorderWidth());

	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());

	m_pctrlYInverted->setChecked(m_pGraph->GetInverted());

	SetApplied(true);
}



void GraphDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/2);
//	move(r.width()/3, r.height()/6);


	QHBoxLayout *CommandButtons = new QHBoxLayout;
	RestoreButton = new QPushButton("Restore");
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(true);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	ApplyButton = new QPushButton(tr("Apply"));
	ApplyButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(RestoreButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QTabWidget *pTabWidget = new QTabWidget(this);
//	pTabWidget->setMinimumWidth(300);
//	pTabWidget->setMinimumHeight(300);

	ScalePage = new QWidget(this);
	GridPage = new QWidget(this);
	FontPage = new QWidget(this);
	BackPage = new QWidget(this);

	//________Font Page______________________
	QGridLayout *FontButtons = new QGridLayout;

	QLabel *lab1  = new QLabel("Title font");
	QLabel *lab2  = new QLabel("Label font");
	QLabel *lab3 = new QLabel("Legend font");
	FontButtons->addWidget(lab1,1,1);
	FontButtons->addWidget(lab2,2,1);
	FontButtons->addWidget(lab3,3,1);

	m_pctrlTitleButton  = new QPushButton("Title Font");
	m_pctrlLabelButton  = new QPushButton("Label Font");
	m_pctrlLegendButton = new QPushButton("Legend Font");
	FontButtons->addWidget(m_pctrlTitleButton,1,2);
	FontButtons->addWidget(m_pctrlLabelButton,2,2);
	FontButtons->addWidget(m_pctrlLegendButton,3,2);

	m_pctrlTitleColor  = new ColorButton;
	m_pctrlLabelColor  = new ColorButton;
	m_pctrlLegendColor = new ColorButton;
	FontButtons->addWidget(m_pctrlTitleColor,1,3);
	FontButtons->addWidget(m_pctrlLabelColor,2,3);
	FontButtons->addWidget(m_pctrlLegendColor,3,3);

	FontPage->setLayout(FontButtons);
	//________End Font Page______________________
	//________Scale Page______________________
	QHBoxLayout *HScaleBox = new QHBoxLayout;

	QGridLayout *ScaleData = new QGridLayout;

	QLabel *MinLabel = new QLabel("Min");
	QLabel *MaxLabel = new QLabel("Max");
	QLabel *OriginLabel = new QLabel("Origin");
	QLabel *UnitLabel = new QLabel("Unit");
	MinLabel->setAlignment(Qt::AlignRight);
	MaxLabel->setAlignment(Qt::AlignRight);
	OriginLabel->setAlignment(Qt::AlignRight);
	UnitLabel->setAlignment(Qt::AlignRight);
//	ScaleData->addStretch(1);
	ScaleData->addWidget(MinLabel,3,1);
	ScaleData->addWidget(MaxLabel,4,1);
	ScaleData->addWidget(OriginLabel,5,1);
	ScaleData->addWidget(UnitLabel,6,1);

	m_pctrlXAuto    = new QCheckBox("Auto Scale");
	m_pctrlXMin     = new FloatEdit;
	m_pctrlXMax     = new FloatEdit;
	m_pctrlXOrigin  = new FloatEdit;
	m_pctrlXUnit    = new FloatEdit;

	ScaleData->addWidget(m_pctrlXAuto,2,2);
	ScaleData->addWidget(m_pctrlXMin,3,2);
	ScaleData->addWidget(m_pctrlXMax,4,2);
	ScaleData->addWidget(m_pctrlXOrigin,5,2);
	ScaleData->addWidget(m_pctrlXUnit,6,2);

	m_pctrlYInverted = new QCheckBox("Inverted Axis");
	m_pctrlYAuto     = new QCheckBox("Auto Scale");
	m_pctrlYMin      = new FloatEdit;
	m_pctrlYMax      = new FloatEdit;
	m_pctrlYOrigin   = new FloatEdit;
	m_pctrlYUnit     = new FloatEdit;

	ScaleData->addWidget(m_pctrlYInverted,1,3);
	ScaleData->addWidget(m_pctrlYAuto,2,3);
	ScaleData->addWidget(m_pctrlYMin,3,3);
	ScaleData->addWidget(m_pctrlYMax,4,3);
	ScaleData->addWidget(m_pctrlYOrigin,5,3);
	ScaleData->addWidget(m_pctrlYUnit,6,3);


//	HScaleBox->addLayout(Labels);
//	HScaleBox->addWidget(XDataGroup);
//	HScaleBox->addWidget(YDataGroup);

	ScalePage->setLayout(ScaleData);
	//________End Scale Page______________________
	//________Axis Page______________________
	QGridLayout *AxisData = new QGridLayout;
	QLabel *AxisStyleLabel = new QLabel("Axis Style");
	AxisStyleLabel->setMinimumWidth(100);
	m_pctrlXMajGridShow = new QCheckBox("X Major Grid");
	m_pctrlYMajGridShow = new QCheckBox("Y Major Grid");
	m_pctrlXMinGridShow = new QCheckBox("X Minor Grid");
	m_pctrlYMinGridShow = new QCheckBox("Y Minor Grid");

	m_pctrlAxisStyle = new LineButton;
	m_pctrlAxisStyle->setMinimumWidth(100);
	m_pctrlXMajGridStyle = new LineButton;
	m_pctrlYMajGridStyle = new LineButton;
	m_pctrlXMinGridStyle = new LineButton;
	m_pctrlYMinGridStyle = new LineButton;

	m_pctrlXMinorUnit = new FloatEdit;
	m_pctrlYMinorUnit = new FloatEdit;

	AxisData->addWidget(AxisStyleLabel,1,1);
	AxisData->addWidget(m_pctrlXMajGridShow,2,1);
	AxisData->addWidget(m_pctrlYMajGridShow,3,1);
	AxisData->addWidget(m_pctrlXMinGridShow,4,1);
	AxisData->addWidget(m_pctrlYMinGridShow,5,1);

	AxisData->addWidget(m_pctrlAxisStyle,1,2);
	AxisData->addWidget(m_pctrlXMajGridStyle,2,2);
	AxisData->addWidget(m_pctrlYMajGridStyle,3,2);
	AxisData->addWidget(m_pctrlXMinGridStyle,4,2);
	AxisData->addWidget(m_pctrlYMinGridStyle,5,2);

	AxisData->addWidget(m_pctrlXMinorUnit,4,3);
	AxisData->addWidget(m_pctrlYMinorUnit,5,3);

	GridPage->setLayout(AxisData);
	//________End Axis Page______________________
	//________Back Page______________________
	QGridLayout *BackData = new QGridLayout;
	QLabel *GraphBackLabel = new QLabel("Graph Background");
	GraphBackLabel->setMinimumWidth(100);
	m_pctrlGraphBorder = new QCheckBox("Graph Border");

	m_pctrlGraphBack = new ColorButton;
	m_pctrlGraphBack->setMinimumWidth(100);
	m_pctrlBorderStyle = new LineButton;
	m_pctrlBorderStyle->setMinimumWidth(100);

	BackData->addWidget(GraphBackLabel,1,1);
	BackData->addWidget(m_pctrlGraphBorder,2,1);

	BackData->addWidget(m_pctrlGraphBack,1,2);
	BackData->addWidget(m_pctrlBorderStyle,2,2);

	BackPage->setLayout(BackData);
	//________End Back Page______________________
	pTabWidget->addTab(ScalePage, "Scales");
	pTabWidget->addTab(GridPage, "Axis and Grids");
	pTabWidget->addTab(FontPage, "Fonts");
	pTabWidget->addTab(BackPage, "Background");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(pTabWidget);
	mainLayout->addLayout(CommandButtons);
	setLayout(mainLayout);
}


