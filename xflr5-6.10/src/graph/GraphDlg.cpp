/****************************************************************************

	GraphDlg  Classes
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

#include "GraphDlg.h"
#include "../misc/LinePickerDlg.h"
#include "../objects/Polar.h"
#include "../objects/WPolar.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QtDebug>

int GraphDlg::s_ActivePage;


GraphDlg::GraphDlg(QWidget *pParent): QDialog(pParent)
{
	setWindowTitle(tr("Graph Settings"));

	m_pParent = pParent;

	m_iGraphType = 1;
	m_pMemGraph = NULL;
	m_pGraph    = NULL;
	m_NGraph      = 0 ;
	m_bApplied = true;
	m_bVariableChanged = false;

	m_XSel = 0;
	m_YSel = 1;

	m_pTitleFont = m_pLabelFont = NULL;

	SetupLayout();
	Connect();
}


void GraphDlg::Connect()
{
	connect(m_pctrlTitleClr, SIGNAL(clickedTB()),  this, SLOT(OnTitleColor()));
	connect(m_pctrlLabelClr, SIGNAL(clickedTB()),  this, SLOT(OnLabelColor()));

	connect(m_pctrlTitleButton, SIGNAL(clicked()),  this, SLOT(OnTitleFont()));
	connect(m_pctrlLabelButton, SIGNAL(clicked()),  this, SLOT(OnLabelFont()));

	connect(m_pctrlXAuto, SIGNAL(clicked()), this, SLOT(OnAutoX()));
	connect(m_pctrlYAuto, SIGNAL(clicked()), this, SLOT(OnAutoY()));
	connect(m_pctrlYInverted, SIGNAL(clicked()), this, SLOT(OnYInverted()));

	connect(m_pctrlXMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMajGridShow(int)));
	connect(m_pctrlYMajGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMajGridShow(int)));
	connect(m_pctrlXMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnXMinGridShow(int)));
	connect(m_pctrlYMinGridShow, SIGNAL(stateChanged(int)), this, SLOT(OnYMinGridShow(int)));

	connect(m_pctrlAxisStyle, SIGNAL(clickedLB()), this, SLOT(OnAxisStyle()));
	connect(m_pctrlXMajGridStyle, SIGNAL(clickedLB()), this, SLOT(OnXMajGridStyle()));
	connect(m_pctrlYMajGridStyle, SIGNAL(clickedLB()), this, SLOT(OnYMajGridStyle()));
	connect(m_pctrlXMinGridStyle, SIGNAL(clickedLB()), this, SLOT(OnXMinGridStyle()));
	connect(m_pctrlYMinGridStyle, SIGNAL(clickedLB()), this, SLOT(OnYMinGridStyle()));

	connect(m_pctrlAutoXMinUnit, SIGNAL(clicked()), this, SLOT(OnAutoMinGrid()));
	connect(m_pctrlAutoYMinUnit, SIGNAL(clicked()), this, SLOT(OnAutoMinGrid()));

	connect(m_pctrlGraphBorder, SIGNAL(stateChanged(int)), this, SLOT(OnGraphBorder(int)));
	connect(m_pctrlGraphBack, SIGNAL(clicked()), this, SLOT(OnGraphBackColor()));
	connect(m_pctrlBorderStyle, SIGNAL(clicked()), this, SLOT(OnBorderStyle()));

/*	connect(m_pctrlXSel, SIGNAL(itemActivated ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemActivated ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlXSel, SIGNAL(itemClicked ( QListWidgetItem*)), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemClicked ( QListWidgetItem*)), SLOT(OnVariableChanged()));*/
	connect(m_pctrlXSel, SIGNAL(itemSelectionChanged()), SLOT(OnVariableChanged()));
	connect(m_pctrlYSel, SIGNAL(itemSelectionChanged()), SLOT(OnVariableChanged()));

	connect(m_pctrlXSel, SIGNAL(itemDoubleClicked (QListWidgetItem *)), SLOT(OnOK()));
	connect(m_pctrlYSel, SIGNAL(itemDoubleClicked (QListWidgetItem *)), SLOT(OnOK()));

	connect(RestoreButton, SIGNAL(clicked()),this, SLOT(OnRestoreParams()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ApplyButton, SIGNAL(clicked()), this, SLOT(OnApply()));
}




void GraphDlg::FillVariableList()
{
	m_pctrlXSel->clear();
	m_pctrlYSel->clear();
	if(m_iGraphType == 31)
	{
		m_pctrlXSel->addItem(tr("X - Chord"));
		m_pctrlYSel->addItem(tr("Q - Speed"));
	}
	else if(m_iGraphType==51)
	{
		//foil oppoint graph variables
		m_pctrlXSel->addItem(tr("X - chord"));
		m_pctrlYSel->addItem(tr("Cp"));
		m_pctrlYSel->addItem(tr("Q - Speed"));
	}
	else if(m_iGraphType==52)
	{
		//foil polar graph variables
		for(int iVar=0; iVar<15; iVar++)
		{
			m_pctrlXSel->addItem(Polar::variableName(iVar));
			m_pctrlYSel->addItem(Polar::variableName(iVar));
		}
	}
	else if(m_iGraphType==61)
	{
		//wing graph variable
		m_pctrlXSel->addItem(tr("Y - span"));

		m_pctrlYSel->addItem(tr("Induced Angle"));						//0
		m_pctrlYSel->addItem(tr("Total Angle"));						//1
		m_pctrlYSel->addItem(tr("Local lift coef."));					//2
		m_pctrlYSel->addItem(tr("Local Lift C.Cl/M.A.C."));				//3
		m_pctrlYSel->addItem(tr("Airfoil viscous drag coef."));			//4
		m_pctrlYSel->addItem(tr("Induced drag coef."));					//5
		m_pctrlYSel->addItem(tr("Total drag coef."));					//6
		m_pctrlYSel->addItem(tr("Local Drag C.Cd/M.A.C."));              //7
		m_pctrlYSel->addItem(tr("Airfoil Pitching moment coef."));       //8
		m_pctrlYSel->addItem(tr("Total Pitching moment coef."));         //9
		m_pctrlYSel->addItem(tr("Reynolds"));                            //10
		m_pctrlYSel->addItem(tr("Top Transition x-pos%"));               //11
		m_pctrlYSel->addItem(tr("Bottom Transition x-pos%"));            //12
		m_pctrlYSel->addItem(tr("Centre of Pressure x-pos%"));           //13
		m_pctrlYSel->addItem(tr("Bending moment"));                      //14
	}
	else if(m_iGraphType == 62)
	{
		//WingPolar Graph Variables
		for(int iVar=0; iVar<44; iVar++)
		{
			m_pctrlXSel->addItem(WPolar::variableName(iVar));
			m_pctrlYSel->addItem(WPolar::variableName(iVar));
		}
	}
	else if(m_iGraphType == 64)
	{
		m_pctrlXSel->addItem(tr("X - Chord"));
		m_pctrlYSel->addItem(tr("Cp"));
	}

	m_pctrlXSel->adjustSize();
	m_pctrlYSel->adjustSize();
}



void GraphDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
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


void GraphDlg::OnActivePage(int index)
{
	s_ActivePage = index;
	ApplyButton->setEnabled(m_pTabWidget->currentIndex()!=0);
}




void GraphDlg::OnAutoMinGrid()
{
	bool bAuto;
	bAuto = m_pctrlAutoXMinUnit->isChecked();
	m_pGraph->SetAutoXMinUnit(bAuto);
	m_pctrlXMinorUnit->setEnabled(!bAuto);

	bAuto = m_pctrlAutoYMinUnit->isChecked();
	m_pGraph->SetAutoYMinUnit(bAuto);
	m_pctrlYMinorUnit->setEnabled(!bAuto);
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
    LinePickerDlg dlg(this);
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
    LinePickerDlg dlg(this);
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
	BkColor = QColorDialog::getColor(BkColor);
	if(BkColor.isValid()) m_pGraph->SetBkColor(BkColor);

	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());
	SetButtonColors();
	SetApplied(false);	
}


void GraphDlg::OnLabelColor()
{
	QColor color = m_pGraph->GetLabelColor();
	color = QColorDialog::getRgba(color.rgba());

	m_pGraph->SetLabelColor(color);
	m_pctrlLabelClr->setTextColor(color);

	SetApplied(false);
	update();

}



void GraphDlg::OnLabelFont()
{
	bool ok;
	QFont LabelFont("Courier");
	m_pGraph->GetLabelFont(LabelFont);
	QFont font = QFontDialog::getFont(&ok, LabelFont, this);

	if (ok)
	{
		m_pctrlLabelButton->setFont(font);
		m_pctrlLabelButton->setText(font.family()+QString(" %1").arg(font.pointSize()));
		m_pGraph->SetLabelFont(font);
		SetApplied(false);
	}
}


void GraphDlg::OnOK()
{
	ApplyChanges();

	m_XSel = m_pctrlXSel->currentRow();
	m_YSel = m_pctrlYSel->currentRow();
	m_pGraph->SetVariables(m_pctrlXSel->currentRow(), m_pctrlYSel->currentRow());

	accept();
}


void GraphDlg::ApplyChanges()
{
	m_pGraph->SetAutoX(m_pctrlXAuto->isChecked());
	m_pGraph->SetXMin(m_pctrlXMin->Value());
	m_pGraph->SetXMax(m_pctrlXMax->Value());
	m_pGraph->SetX0(m_pctrlXOrigin->Value());
	m_pGraph->SetXUnit(m_pctrlXUnit->Value());

	m_pGraph->SetAutoY(m_pctrlYAuto->isChecked());
	m_pGraph->SetYMin(m_pctrlYMin->Value());
	m_pGraph->SetYMax(m_pctrlYMax->Value());
	m_pGraph->SetY0(m_pctrlYOrigin->Value());
	m_pGraph->SetYUnit(m_pctrlYUnit->Value());

	double MinUnit;
	if(!m_pctrlAutoXMinUnit->isChecked())
	{
		MinUnit = m_pctrlXMinorUnit->Value();
		m_pGraph->SetXMinorUnit(MinUnit);
		m_pGraph->SetAutoXMinUnit(false);
	}
	else
		m_pGraph->SetAutoXMinUnit(true);

	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->Value();
		m_pGraph->SetYMinorUnit(MinUnit);
		m_pGraph->SetAutoYMinUnit(false);
	}
	else
		m_pGraph->SetAutoYMinUnit(true);

	m_pGraph->SetMargin(m_pctrlMargin->Value());

	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pGraph);
	}

}


void GraphDlg::OnApply()
{
	ApplyChanges();

	m_pGraph->SetInverted(m_pctrlYInverted->isChecked());

	if(m_pParent) m_pParent->update();
	SetApplied(true);
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

	if(m_pParent) m_pParent->update();
}


void GraphDlg::OnTitleColor()
{
	QColor color = m_pGraph->GetTitleColor();
	color = QColorDialog::getRgba(color.rgba());

	m_pGraph->SetTitleColor(color);
	m_pctrlTitleClr->setTextColor(color);

	SetApplied(false);
	update();
}


void GraphDlg::OnTitleFont()
{
	bool bOk;
	QFont TitleFont("Arial");
	m_pGraph->GetTitleFont(TitleFont);

	QFont font = QFontDialog::getFont(&bOk, TitleFont, this);

	if (bOk)
	{
		m_pctrlTitleButton->setFont(font);
		m_pctrlTitleButton->setText(font.family()+QString(" %1").arg(font.pointSize()));

		m_pGraph->SetTitleFont(font);
		SetApplied(false);
	}
}


void GraphDlg::OnVariableChanged()
{
	m_bVariableChanged = true;
}


void GraphDlg::OnMargin()
{
	m_pGraph->SetMargin(m_pctrlMargin->Value());
}

void GraphDlg::OnXMajGridStyle()
{
    LinePickerDlg dlg(this);
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
    LinePickerDlg dlg(this);
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
	m_pctrlAutoXMinUnit->setEnabled(bShow);
	m_pctrlXMinorUnit->setEnabled(bShow && !m_pGraph->GetAutoXMin());

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
    LinePickerDlg dlg(this);
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
	m_pctrlAutoYMinUnit->setEnabled(bShow);
	m_pctrlYMinorUnit->setEnabled(bShow && !m_pGraph->GetAutoYMin());

	SetApplied(false);
}


void GraphDlg::OnYMinGridStyle()
{
    LinePickerDlg dlg(this);
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
//	ApplyButton->setEnabled(!bApplied);
}


void GraphDlg::SetButtonColors()
{
	m_pctrlTitleClr->setTextColor(m_pGraph->GetTitleColor());
	m_pctrlTitleClr->setBackgroundColor(m_pGraph->GetBackColor());

	m_pctrlLabelClr->setTextColor(m_pGraph->GetLabelColor());
	m_pctrlLabelClr->setBackgroundColor(m_pGraph->GetBackColor());
}


void GraphDlg::SetParams()
{
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

	SetButtonColors();

	QFont font;
	m_pGraph->GetLabelFont(font);
	m_pctrlLabelButton->setText(font.family()+QString(" %1").arg(font.pointSize()));
	m_pctrlLabelButton->setFont(font);

	m_pGraph->GetTitleFont(font);
	m_pctrlTitleButton->setText(font.family()+QString(" %1").arg(font.pointSize()));
	m_pctrlTitleButton->setFont(font);

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
	m_pctrlAutoXMinUnit->setChecked(bAuto);
	m_pctrlAutoXMinUnit->setEnabled(bState);
	m_pctrlXMinorUnit->setEnabled(!bAuto && bState);
	
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
	m_pctrlAutoYMinUnit->setChecked(bAuto);
	m_pctrlAutoYMinUnit->setEnabled(bState);
	m_pctrlYMinorUnit->setEnabled(!bAuto && bState);

	m_pctrlAxisStyle->SetColor(m_pGraph->GetAxisColor());
	m_pctrlAxisStyle->SetStyle(m_pGraph->GetAxisStyle());
	m_pctrlAxisStyle->SetWidth(m_pGraph->GetAxisWidth());

	m_pctrlGraphBorder->setChecked(m_pGraph->GetBorder());
	m_pctrlBorderStyle->SetColor(m_pGraph->GetBorderColor());
	m_pctrlBorderStyle->SetStyle(m_pGraph->GetBorderStyle());
	m_pctrlBorderStyle->SetWidth(m_pGraph->GetBorderWidth());

	m_pctrlGraphBack->SetColor(m_pGraph->GetBackColor());

	m_pctrlMargin->SetValue(m_pGraph->GetMargin());

	m_pctrlYInverted->setChecked(m_pGraph->GetInverted());

	FillVariableList();

	m_pctrlXSel->setCurrentRow(m_pGraph->GetXVariable());
	m_pctrlYSel->setCurrentRow(m_pGraph->GetYVariable());
	m_bVariableChanged = false;

	SetApplied(true);

	ApplyButton->setEnabled(m_pTabWidget->currentIndex()!=0);
}



void GraphDlg::SetupLayout()
{
	QFontMetrics fm(font());

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	{
		RestoreButton = new QPushButton(tr("Restore"));
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
	}

	m_pTabWidget = new QTabWidget(this);
	m_pScalePage    = new QWidget(this);
	m_pGridPage     = new QWidget(this);
	m_pFontPage     = new QWidget(this);
	m_pVariablePage = new QWidget(this);

	//________Variable Page______________________
	QVBoxLayout *pVariablePageLayout = new QVBoxLayout(this);
	{
		QHBoxLayout *pAxisNamesLayout = new QHBoxLayout;
		{
			QLabel *YAxis = new QLabel(tr("YAxis"));
			QLabel *vs = new QLabel(tr("vs."));
			QLabel *XAxis = new QLabel(tr("XAxis"));
			pAxisNamesLayout->addStretch(1);
			pAxisNamesLayout->addWidget(YAxis);
			pAxisNamesLayout->addStretch(1);
			pAxisNamesLayout->addWidget(vs);
			pAxisNamesLayout->addStretch(1);
			pAxisNamesLayout->addWidget(XAxis);
			pAxisNamesLayout->addStretch(1);
		}

		QHBoxLayout *pVariableBoxLayout = new QHBoxLayout;
		{
			m_pctrlXSel = new QListWidget;
			m_pctrlYSel = new QListWidget;
			pVariableBoxLayout->addWidget(m_pctrlYSel);
			pVariableBoxLayout->addWidget(m_pctrlXSel);
		}

		pVariablePageLayout->addLayout(pAxisNamesLayout);
		pVariablePageLayout->addLayout(pVariableBoxLayout);
	}
	m_pVariablePage->setLayout(pVariablePageLayout);

	//________Font Page______________________

	QVBoxLayout *pFontPageLayout = new QVBoxLayout;
	{
		QGroupBox *pFontBox = new QGroupBox(tr("Fonts"));
		{
			QGridLayout *pFontButtonsLayout = new QGridLayout;
			{
				QLabel *lab1  = new QLabel(tr("Title"));
				QLabel *lab2  = new QLabel(tr("Label"));
				QLabel *lab402  = new QLabel(tr("Font"));
				QLabel *lab403  = new QLabel(tr("Color"));
				lab1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
				lab2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
				lab402->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				lab403->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				pFontButtonsLayout->addWidget(lab402,1,2);
				pFontButtonsLayout->addWidget(lab403,1,3);
				pFontButtonsLayout->addWidget(lab1,2,1);
				pFontButtonsLayout->addWidget(lab2,3,1);

				m_pctrlTitleButton  = new QPushButton();
				m_pctrlLabelButton  = new QPushButton();

				pFontButtonsLayout->addWidget(m_pctrlTitleButton,2,2);
				pFontButtonsLayout->addWidget(m_pctrlLabelButton,3,2);

				m_pctrlTitleClr  = new TextClrBtn(this);
				m_pctrlTitleClr->setText(tr("Title Color"));
				m_pctrlLabelClr  = new TextClrBtn(this);
				m_pctrlLabelClr->setText(tr("Label Color"));

				pFontButtonsLayout->addWidget(m_pctrlTitleClr,2,3);
				pFontButtonsLayout->addWidget(m_pctrlLabelClr,3,3);
			}
			pFontBox->setLayout(pFontButtonsLayout);
		}

		QGroupBox *pBackBox = new QGroupBox(tr("BackGround"));
		{
			QGridLayout *pBackDataLayout = new QGridLayout;
			{
				QLabel *GraphBackLabel = new QLabel(tr("Graph Background"));
				GraphBackLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
				m_pctrlGraphBorder = new QCheckBox(tr("Graph Border"));

				m_pctrlGraphBack = new ColorButton;
				m_pctrlGraphBack->setMinimumWidth(100);
				m_pctrlBorderStyle = new LineBtn(this);
				m_pctrlBorderStyle->setMinimumWidth(100);

				pBackDataLayout->addWidget(GraphBackLabel,1,1);
				pBackDataLayout->addWidget(m_pctrlGraphBorder,2,1,1,1,Qt::AlignRight | Qt::AlignVCenter);

				pBackDataLayout->addWidget(m_pctrlGraphBack,1,2);
				pBackDataLayout->addWidget(m_pctrlBorderStyle,2,2);

				pBackDataLayout->setColumnStretch(0,1);
			}
			pBackBox->setLayout(pBackDataLayout);
		}
		QGroupBox *pPaddingBox = new QGroupBox(tr("Padding"));
		{
			QHBoxLayout *pPaddingLayout = new QHBoxLayout;
			{
				QLabel *pMarginLabel = new QLabel(tr("Margin"));
				QLabel *pMarginUnit = new QLabel(tr("pixels"));
				m_pctrlMargin = new IntEdit(31, this);
				pPaddingLayout->addStretch(1);
				pPaddingLayout->addWidget(pMarginLabel);
				pPaddingLayout->addWidget(m_pctrlMargin);
				pPaddingLayout->addWidget(pMarginUnit);
			}
			pPaddingBox->setLayout(pPaddingLayout);
		}

		pFontPageLayout->addWidget(pFontBox);
		pFontPageLayout->addStretch(1);
		pFontPageLayout->addWidget(pBackBox);
		pFontPageLayout->addStretch(1);
		pFontPageLayout->addWidget(pPaddingBox);
		pFontPageLayout->addStretch(1);
	}
	m_pFontPage->setLayout(pFontPageLayout);
	//________End Font Page______________________

	//________Scale Page______________________

	QGridLayout *pScalePageLayout = new QGridLayout;
	{
		QLabel *XAxis2 = new QLabel(tr("X Axis"));
		QLabel *YAxis2 = new QLabel(tr("Y Axis"));
		XAxis2->setAlignment(Qt::AlignCenter);
		YAxis2->setAlignment(Qt::AlignCenter);

		QLabel *MinLabel = new QLabel(tr("Min"));
		QLabel *MaxLabel = new QLabel(tr("Max"));
		QLabel *OriginLabel = new QLabel(tr("Origin"));
		QLabel *UnitLabel = new QLabel(tr("Unit"));
		MinLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		MaxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		OriginLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		UnitLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	//	ScaleData->addStretch(1);
		pScalePageLayout->addWidget(MinLabel,5,1);
		pScalePageLayout->addWidget(MaxLabel,6,1);
		pScalePageLayout->addWidget(OriginLabel,7,1);
		pScalePageLayout->addWidget(UnitLabel,8,1);

		m_pctrlXAuto    = new QCheckBox(tr("Auto Scale"));
		m_pctrlXMin     = new DoubleEdit;
		m_pctrlXMax     = new DoubleEdit;
		m_pctrlXOrigin  = new DoubleEdit;
		m_pctrlXUnit    = new DoubleEdit;

		pScalePageLayout->addWidget(XAxis2,2,2);
		pScalePageLayout->addWidget(m_pctrlXAuto,4,2);
		pScalePageLayout->addWidget(m_pctrlXMin,5,2);
		pScalePageLayout->addWidget(m_pctrlXMax,6,2);
		pScalePageLayout->addWidget(m_pctrlXOrigin,7,2);
		pScalePageLayout->addWidget(m_pctrlXUnit,8,2);

		m_pctrlYInverted = new QCheckBox(tr("Inverted Axis"));
		m_pctrlYAuto     = new QCheckBox(tr("Auto Scale"));
		m_pctrlYMin      = new DoubleEdit;
		m_pctrlYMax      = new DoubleEdit;
		m_pctrlYOrigin   = new DoubleEdit;
		m_pctrlYUnit     = new DoubleEdit;

		pScalePageLayout->addWidget(YAxis2,2,3);
		pScalePageLayout->addWidget(m_pctrlYInverted,3,3);
		pScalePageLayout->addWidget(m_pctrlYAuto,4,3);
		pScalePageLayout->addWidget(m_pctrlYMin,5,3);
		pScalePageLayout->addWidget(m_pctrlYMax,6,3);
		pScalePageLayout->addWidget(m_pctrlYOrigin,7,3);
		pScalePageLayout->addWidget(m_pctrlYUnit,8,3);
		pScalePageLayout->setRowStretch(9,1);
	}
	m_pScalePage->setLayout(pScalePageLayout);
	//________End Scale Page______________________

	//________Axis Page______________________
	QGridLayout *pAxisDataLayout = new QGridLayout;
	{
		pAxisDataLayout->setRowStretch(0,1);
		QLabel *AxisStyleLabel = new QLabel(tr("Axis Style"));

		m_pctrlXMajGridShow = new QCheckBox(tr("X Major Grid"));
		m_pctrlYMajGridShow = new QCheckBox(tr("Y Major Grid"));
		m_pctrlXMinGridShow = new QCheckBox(tr("X Minor Grid"));
		m_pctrlYMinGridShow = new QCheckBox(tr("Y Minor Grid"));
		m_pctrlAutoXMinUnit = new QCheckBox(tr("Auto Unit"));
		m_pctrlAutoYMinUnit = new QCheckBox(tr("Auto Unit"));

		m_pctrlAxisStyle = new LineBtn(this);

		m_pctrlXMajGridStyle = new LineBtn(this);
		m_pctrlYMajGridStyle = new LineBtn(this);
		m_pctrlXMinGridStyle = new LineBtn(this);
		m_pctrlYMinGridStyle = new LineBtn(this);

		m_pctrlXMinorUnit = new DoubleEdit;
		m_pctrlYMinorUnit = new DoubleEdit;

		pAxisDataLayout->addWidget(AxisStyleLabel,1,1);
		pAxisDataLayout->addWidget(m_pctrlXMajGridShow,2,1);
		pAxisDataLayout->addWidget(m_pctrlYMajGridShow,3,1);
		pAxisDataLayout->addWidget(m_pctrlXMinGridShow,4,1);
		pAxisDataLayout->addWidget(m_pctrlYMinGridShow,5,1);

		pAxisDataLayout->addWidget(m_pctrlAxisStyle,1,2);
		pAxisDataLayout->addWidget(m_pctrlXMajGridStyle,2,2);
		pAxisDataLayout->addWidget(m_pctrlYMajGridStyle,3,2);
		pAxisDataLayout->addWidget(m_pctrlXMinGridStyle,4,2);
		pAxisDataLayout->addWidget(m_pctrlYMinGridStyle,5,2);

		pAxisDataLayout->addWidget(m_pctrlAutoXMinUnit,4,3);
		pAxisDataLayout->addWidget(m_pctrlAutoYMinUnit,5,3);
		pAxisDataLayout->addWidget(m_pctrlXMinorUnit,4,4);
		pAxisDataLayout->addWidget(m_pctrlYMinorUnit,5,4);
	}
	m_pGridPage->setLayout(pAxisDataLayout);
	//________End Axis Page______________________


	m_pTabWidget->addTab(m_pVariablePage, tr("Variables"));
	m_pTabWidget->addTab(m_pScalePage, tr("Scales"));
	m_pTabWidget->addTab(m_pGridPage, tr("Axis and Grids"));
	m_pTabWidget->addTab(m_pFontPage, tr("Fonts and BackGround"));

	m_pTabWidget->setCurrentIndex(s_ActivePage);
	connect(m_pTabWidget, SIGNAL(currentChanged (int)), this, SLOT(OnActivePage(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		mainLayout->addWidget(m_pTabWidget);
		mainLayout->addLayout(CommandButtons);
	}
	setLayout(mainLayout);
}




