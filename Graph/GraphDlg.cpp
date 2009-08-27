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

int GraphDlg::s_ActivePage;
void *GraphDlg::s_pMainFrame;


GraphDlg::GraphDlg()
{
	setWindowTitle("Graph Settings");

	m_iGraphType = 1;
	m_pMemGraph = NULL;
	m_pGraph    = NULL;
	m_NGraph      = 0 ;
	m_bApplied = true;
	m_bVariableChanged = false;

	m_XSel = 0;
	m_YSel = 1;

	m_pTitleFont = m_pLegendFont = m_pLabelFont = NULL;

	SetupLayout();
	Connect();
}


void GraphDlg::Connect()
{
	connect(m_pctrlTitleClr, SIGNAL(clicked()),  this, SLOT(OnTitleColor()));
	connect(m_pctrlLabelClr, SIGNAL(clicked()),  this, SLOT(OnLabelColor()));
	connect(m_pctrlLegendClr, SIGNAL(clicked()), this, SLOT(OnLegendColor()));

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
	if(m_iGraphType == 31)
	{
		m_pctrlXSel->addItem("X - Chord");
		m_pctrlYSel->addItem("Q - Speed");
	}
	else if(m_iGraphType==51)
	{
		//foil oppoint graph variables
		m_pctrlXSel->addItem("X - chord");
		m_pctrlYSel->addItem("Cp");
		m_pctrlYSel->addItem("Q - Speed");

	}
	else if(m_iGraphType==52)
	{
		//foil polar graph variables
		m_pctrlXSel->addItem("Alpha");
		m_pctrlXSel->addItem("Cl");
		m_pctrlXSel->addItem("Cd");
		m_pctrlXSel->addItem("Cd x 10000");
		m_pctrlXSel->addItem("Cdp");
		m_pctrlXSel->addItem("Cm");
		m_pctrlXSel->addItem("Xtr1");
		m_pctrlXSel->addItem("Xtr2");
		m_pctrlXSel->addItem("HMom");
		m_pctrlXSel->addItem("Cpmin");
		m_pctrlXSel->addItem("Cl/Cd");
		m_pctrlXSel->addItem("|Cl|^(3/2)/Cd");
		m_pctrlXSel->addItem("1/Cl^1/2");
		m_pctrlXSel->addItem("Re");
		m_pctrlXSel->addItem("XCp");

		m_pctrlYSel->addItem("Alpha");
		m_pctrlYSel->addItem("Cl");
		m_pctrlYSel->addItem("Cd");
		m_pctrlYSel->addItem("Cd x 10000");
		m_pctrlYSel->addItem("Cdp");
		m_pctrlYSel->addItem("Cm");
		m_pctrlYSel->addItem("Xtr1");
		m_pctrlYSel->addItem("Xtr2");
		m_pctrlYSel->addItem("HMom");
		m_pctrlYSel->addItem("Cpmin");
		m_pctrlYSel->addItem("Cl/Cd");
		m_pctrlYSel->addItem("|Cl|^(3/2)/Cd");
		m_pctrlYSel->addItem("1/Cl^1/2");
		m_pctrlYSel->addItem("Re");
		m_pctrlYSel->addItem("XCp");
	}
	else if(m_iGraphType==61)
	{
		//wing graph variable
		m_pctrlXSel->addItem("Y - span");

		m_pctrlYSel->addItem("Induced Angle");						//0
		m_pctrlYSel->addItem("Total Angle");						//1
		m_pctrlYSel->addItem("Local lift coef.");					//2
		m_pctrlYSel->addItem("Local Lift C.Cl/M.A.C.");				//3
		m_pctrlYSel->addItem("Airfoil viscous drag coef.");			//4
		m_pctrlYSel->addItem("Induced drag coef.");					//5
		m_pctrlYSel->addItem("Total drag coef.");					//6
		m_pctrlYSel->addItem("Local Drag C.Cd/M.A.C.");				//7
		m_pctrlYSel->addItem("Airfoil Pitching moment coef.");		//8
		m_pctrlYSel->addItem("Geom. Pitching moment coef.");		//9
		m_pctrlYSel->addItem("Total Pitching moment coef.");		//10
		m_pctrlYSel->addItem("Reynolds");							//11
		m_pctrlYSel->addItem("Top Transition x-pos%");				//12
		m_pctrlYSel->addItem("Bottom Transition x-pos%");			//13
		m_pctrlYSel->addItem("Centre of Pressure x-pos%");			//14
		m_pctrlYSel->addItem("Bending moment");						//15
	}
	else if(m_iGraphType == 62)
	{
		//WingPolar Graph Variables
		m_pctrlXSel->addItem("Alpha");						//0
		m_pctrlXSel->addItem("Lift coef.");					//1
		m_pctrlXSel->addItem("Viscous drag coef.");			//2
		m_pctrlXSel->addItem("Induced drag coef.");			//3
		m_pctrlXSel->addItem("Total drag coef.");			//4

		m_pctrlXSel->addItem("Total pitching moment coef.");//5
		m_pctrlXSel->addItem("Total rolling moment coef.");		//6
		m_pctrlXSel->addItem("Total yawing moment coef.");  //7
		m_pctrlXSel->addItem("Viscous yawing moment coef.");//8
		m_pctrlXSel->addItem("Induced yawing moment coef.");//9

		m_pctrlXSel->addItem("Glide ratio Cl/Cd");			//10
		m_pctrlXSel->addItem("Power factor Cl^(3/2)/Cd");	//11
		m_pctrlXSel->addItem("1/Rt(Cl)");					//12
		m_pctrlXSel->addItem("Lift");						//13
		m_pctrlXSel->addItem("Drag");						//14
		m_pctrlXSel->addItem("Vx");							//15
		m_pctrlXSel->addItem("Vz");							//16
		m_pctrlXSel->addItem("VInf");						//17
		m_pctrlXSel->addItem("Descent angle atan(Cd/Cl)");	//18
		m_pctrlXSel->addItem("Pitching Moment");			//19
		m_pctrlXSel->addItem("Rolling Moment");				//20
		m_pctrlXSel->addItem("Yawing Moment");				//21
		m_pctrlXSel->addItem("Centre of pressure X-Pos");	//22
		m_pctrlXSel->addItem("Centre of pressure Y-Pos");	//23
		m_pctrlXSel->addItem("Bending moment");				//24
		m_pctrlXSel->addItem("m.g.Vz");						//25
		m_pctrlXSel->addItem("Efficiency");					//26
		m_pctrlXSel->addItem("(XCp-Xcg)/MAC");				//27
		m_pctrlXSel->addItem("Control Variable");			//28
		m_pctrlXSel->addItem("Cy - Lateral force coef.");	//29

		m_pctrlYSel->addItem("Alpha");						//0
		m_pctrlYSel->addItem("Lift coef.");					//1
		m_pctrlYSel->addItem("Viscous drag coef.");			//2
		m_pctrlYSel->addItem("Induced drag coef.");			//3
		m_pctrlYSel->addItem("Total drag coef.");			//4

		m_pctrlYSel->addItem("Total pitching moment coef.");//5
		m_pctrlYSel->addItem("Total rolling moment coef.");		//6
		m_pctrlYSel->addItem("Total yawing moment coef.");  //7
		m_pctrlYSel->addItem("Viscous yawing moment coef.");//8
		m_pctrlYSel->addItem("Induced yawing moment coef.");//9

		m_pctrlYSel->addItem("Glide ratio Cl/Cd");			//10
		m_pctrlYSel->addItem("Power factor Cl^(3/2)/Cd");	//11
		m_pctrlYSel->addItem("1/Rt(Cl)");					//12
		m_pctrlYSel->addItem("Lift");						//13
		m_pctrlYSel->addItem("Drag");						//14
		m_pctrlYSel->addItem("Vx");							//15
		m_pctrlYSel->addItem("Vz");							//16
		m_pctrlYSel->addItem("VInf");						//17
		m_pctrlYSel->addItem("Descent angle atan(Cd/Cl)");	//18
		m_pctrlYSel->addItem("Pitching Moment");			//19
		m_pctrlYSel->addItem("Rolling Moment");				//20
		m_pctrlYSel->addItem("Yawing Moment");				//21
		m_pctrlYSel->addItem("Centre of pressure X-Pos");	//22
		m_pctrlYSel->addItem("Centre of pressure Y-Pos");	//23
		m_pctrlYSel->addItem("Bending moment");				//24
		m_pctrlYSel->addItem("m.g.Vz");						//25
		m_pctrlYSel->addItem("Efficiency");					//26
		m_pctrlYSel->addItem("(XCp-Xcg)/MAC");				//27
		m_pctrlYSel->addItem("Control Variable");			//28
		m_pctrlYSel->addItem("Cy - Lateral force coef.");	//29
	}
	else if(m_iGraphType == 64)
	{
		m_pctrlXSel->addItem("X - Chord");
		m_pctrlYSel->addItem("Cp");
	}

	m_pctrlXSel->adjustSize();
	m_pctrlYSel->adjustSize();
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


void GraphDlg::OnActivePage(int index)
{
	s_ActivePage = index;
	ApplyButton->setEnabled(m_pTabWidget->currentIndex()!=0);
}


void GraphDlg::OnApply()
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

	double MinUnit;
	if(!m_pctrlAutoXMinUnit->isChecked())
	{
		MinUnit = m_pctrlXMinorUnit->GetValue();
		m_pGraph->SetXMinorUnit(MinUnit);
	}
	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->GetValue();
		m_pGraph->SetYMinorUnit(MinUnit);
	}

	m_pGraph->SetInverted(m_pctrlYInverted->isChecked());

	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pGraph);
	}

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->UpdateView();
	SetApplied(true);
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
	SetButtonColors();
	SetApplied(false);	
}


void GraphDlg::OnLabelColor()
{
	QColor color = m_pGraph->GetLabelColor();
	m_pGraph->SetLabelColor(QColorDialog::getRgba(color.rgba()));

	QPalette palette = m_pctrlLabelClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());
		palette.setColor(QPalette::ButtonText, m_pGraph->GetLabelColor());
		m_pctrlLabelClr->setPalette(palette);
//		m_pctrlLabelClr->setAutoFillBackground(true);
	}

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
	QColor color = m_pGraph->GetLegendColor();
	m_pGraph->SetLegendColor(QColorDialog::getRgba(color.rgba()));

	QPalette palette = m_pctrlLegendClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());
		palette.setColor(QPalette::ButtonText, m_pGraph->GetLegendColor());
		m_pctrlLegendClr->setPalette(palette);
//		m_pctrlLegendClr->setAutoFillBackground(true);
	}

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

	double MinUnit;
	if(!m_pctrlAutoXMinUnit->isChecked())
	{
		MinUnit = m_pctrlXMinorUnit->GetValue();
		m_pGraph->SetXMinorUnit(MinUnit);
		m_pGraph->SetAutoXMinUnit(false);
	}
	else
		m_pGraph->SetAutoXMinUnit(true);

	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->GetValue();
		m_pGraph->SetYMinorUnit(MinUnit);
		m_pGraph->SetAutoYMinUnit(false);
	}
	else
		m_pGraph->SetAutoYMinUnit(true);

	for(int i=0; i<m_NGraph; i++)
	{
		m_GraphArray[i]->CopySettings(m_pGraph);
	}

	m_XSel = m_pctrlXSel->currentRow();
	m_YSel = m_pctrlYSel->currentRow();
	m_pGraph->SetVariables(m_pctrlXSel->currentRow(), m_pctrlYSel->currentRow());

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

	QPalette palette = m_pctrlTitleClr->palette();
	QColor listColor = palette.color(QPalette::Button);
	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());
		palette.setColor(QPalette::ButtonText, m_pGraph->GetTitleColor());
		m_pctrlTitleClr->setPalette(palette);
//		m_pctrlTitleClr->setAutoFillBackground(true);
	}

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


void GraphDlg::OnVariableChanged()
{
	m_bVariableChanged = true;
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
	m_pctrlAutoYMinUnit->setEnabled(bShow);
	m_pctrlYMinorUnit->setEnabled(bShow && !m_pGraph->GetAutoYMin());

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
//	ApplyButton->setEnabled(!bApplied);
}


void GraphDlg::SetButtonColors()
{
	QPalette palette = m_pctrlTitleClr->palette();
	QColor listColor = palette.color(QPalette::Button);

	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());

		palette.setColor(QPalette::ButtonText, m_pGraph->GetTitleColor());
		m_pctrlTitleClr->setPalette(palette);

		palette.setColor(QPalette::ButtonText, m_pGraph->GetLegendColor());
		m_pctrlLegendClr->setPalette(palette);

		palette.setColor(QPalette::ButtonText, m_pGraph->GetLabelColor());
		m_pctrlLabelClr->setPalette(palette);
	}
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

	SetButtonColors();

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
	QDesktopWidget desktop;
	QRect r = desktop.geometry();


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

	m_pTabWidget = new QTabWidget(this);


	ScalePage    = new QWidget(this);
	GridPage     = new QWidget(this);
	FontPage     = new QWidget(this);
	VariablePage = new QWidget(this);

	//________Variable Page______________________

	QHBoxLayout *AxisNames = new QHBoxLayout;
	QLabel *YAxis = new QLabel("YAxis");
	QLabel *vs = new QLabel("vs.");
	QLabel *XAxis = new QLabel("XAxis");
	AxisNames->addStretch(1);
	AxisNames->addWidget(YAxis);
	AxisNames->addStretch(1);
	AxisNames->addWidget(vs);
	AxisNames->addStretch(1);
	AxisNames->addWidget(XAxis);
	AxisNames->addStretch(1);

	QHBoxLayout *VariableBox = new QHBoxLayout;
	m_pctrlXSel = new QListWidget;
	m_pctrlYSel = new QListWidget;
	m_pctrlXSel->setMinimumWidth(30);
	m_pctrlYSel->setMinimumWidth(30);
	m_pctrlXSel->setMinimumHeight(200);
	m_pctrlYSel->setMinimumHeight(200);
	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlYSel);
	VariableBox->addStretch(1);
	VariableBox->addWidget(m_pctrlXSel);
	VariableBox->addStretch(1);

	QVBoxLayout *VariableLayout = new QVBoxLayout(this);
	VariableLayout->addLayout(AxisNames);
	VariableLayout->addLayout(VariableBox);
	VariableLayout->addStretch(1);

	VariablePage->setLayout(VariableLayout);

	//________Font Page______________________
	QGridLayout *FontButtons = new QGridLayout;

	QLabel *lab1  = new QLabel("Title");
	QLabel *lab2  = new QLabel("Label");
	QLabel *lab3 = new QLabel("Legend");
	QLabel *lab402  = new QLabel("Font");
	QLabel *lab403  = new QLabel("Color");
	lab1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	lab2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	lab3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	lab402->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	lab403->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
	FontButtons->addWidget(lab402,1,2);
	FontButtons->addWidget(lab403,1,3);
	FontButtons->addWidget(lab1,2,1);
	FontButtons->addWidget(lab2,3,1);
	FontButtons->addWidget(lab3,4,1);

	m_pctrlTitleButton  = new QPushButton("Set Title Font");
	m_pctrlLabelButton  = new QPushButton("Set Label Font");
	m_pctrlLegendButton = new QPushButton("Set Legend Font");
	FontButtons->addWidget(m_pctrlTitleButton,2,2);
	FontButtons->addWidget(m_pctrlLabelButton,3,2);
	FontButtons->addWidget(m_pctrlLegendButton,4,2);

	m_pctrlTitleClr  = new QPushButton("Title Color");
	m_pctrlLabelClr  = new QPushButton("Label Color");
	m_pctrlLegendClr = new QPushButton("Legend Color");
//	m_pctrlTitleClr->setAutoFillBackground(true);
//	m_pctrlLegendClr->setAutoFillBackground(true);
//	m_pctrlLabelClr->setAutoFillBackground(true);

	FontButtons->addWidget(m_pctrlTitleClr,2,3);
	FontButtons->addWidget(m_pctrlLabelClr,3,3);
	FontButtons->addWidget(m_pctrlLegendClr,4,3);

	QGroupBox *FontBox = new QGroupBox("Fonts");
	FontBox->setLayout(FontButtons);


	QGridLayout *BackData = new QGridLayout;
	QLabel *GraphBackLabel = new QLabel("Graph Background");
	GraphBackLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlGraphBorder = new QCheckBox("Graph Border");

	m_pctrlGraphBack = new ColorButton;
	m_pctrlGraphBack->setMinimumWidth(100);
	m_pctrlBorderStyle = new LineButton;
	m_pctrlBorderStyle->setMinimumWidth(100);

	BackData->addWidget(GraphBackLabel,1,1);
	BackData->addWidget(m_pctrlGraphBorder,2,1,1,1,Qt::AlignRight | Qt::AlignVCenter);

	BackData->addWidget(m_pctrlGraphBack,1,2);
	BackData->addWidget(m_pctrlBorderStyle,2,2);

	QGroupBox *BackBox = new QGroupBox("BackGround");
	BackBox->setLayout(BackData);


	QVBoxLayout *FontLayout = new QVBoxLayout;
	FontLayout->addWidget(FontBox);
	FontLayout->addStretch(1);
	FontLayout->addWidget(BackBox);
	FontLayout->addStretch(1);
	FontPage->setLayout(FontLayout);
	//________End Font Page______________________

	//________Scale Page______________________

	QGridLayout *ScaleData = new QGridLayout;

	QLabel *XAxis2 = new QLabel("X Axis");
	QLabel *YAxis2 = new QLabel("Y Axis");
	XAxis2->setAlignment(Qt::AlignCenter);
	YAxis2->setAlignment(Qt::AlignCenter);

	QLabel *MinLabel = new QLabel("Min");
	QLabel *MaxLabel = new QLabel("Max");
	QLabel *OriginLabel = new QLabel("Origin");
	QLabel *UnitLabel = new QLabel("Unit");
	MinLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	MaxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	OriginLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	UnitLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//	ScaleData->addStretch(1);
	ScaleData->addWidget(MinLabel,4,1);
	ScaleData->addWidget(MaxLabel,5,1);
	ScaleData->addWidget(OriginLabel,6,1);
	ScaleData->addWidget(UnitLabel,7,1);

	m_pctrlXAuto    = new QCheckBox("Auto Scale");
	m_pctrlXMin     = new FloatEdit;
	m_pctrlXMax     = new FloatEdit;
	m_pctrlXOrigin  = new FloatEdit;
	m_pctrlXUnit    = new FloatEdit;

	ScaleData->addWidget(XAxis2,1,2);
	ScaleData->addWidget(m_pctrlXAuto,3,2);
	ScaleData->addWidget(m_pctrlXMin,4,2);
	ScaleData->addWidget(m_pctrlXMax,5,2);
	ScaleData->addWidget(m_pctrlXOrigin,6,2);
	ScaleData->addWidget(m_pctrlXUnit,7,2);

	m_pctrlYInverted = new QCheckBox("Inverted Axis");
	m_pctrlYAuto     = new QCheckBox("Auto Scale");
	m_pctrlYMin      = new FloatEdit;
	m_pctrlYMax      = new FloatEdit;
	m_pctrlYOrigin   = new FloatEdit;
	m_pctrlYUnit     = new FloatEdit;

	ScaleData->addWidget(YAxis2,1,3);
	ScaleData->addWidget(m_pctrlYInverted,2,3);
	ScaleData->addWidget(m_pctrlYAuto,3,3);
	ScaleData->addWidget(m_pctrlYMin,4,3);
	ScaleData->addWidget(m_pctrlYMax,5,3);
	ScaleData->addWidget(m_pctrlYOrigin,6,3);
	ScaleData->addWidget(m_pctrlYUnit,7,3);

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
	m_pctrlAutoXMinUnit = new QCheckBox("Auto Unit");
	m_pctrlAutoYMinUnit = new QCheckBox("Auto Unit");

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

	AxisData->addWidget(m_pctrlAutoXMinUnit,4,3);
	AxisData->addWidget(m_pctrlAutoYMinUnit,5,3);
	AxisData->addWidget(m_pctrlXMinorUnit,4,4);
	AxisData->addWidget(m_pctrlYMinorUnit,5,4);

	GridPage->setLayout(AxisData);
	//________End Axis Page______________________


	m_pTabWidget->addTab(VariablePage, "Variables");
	m_pTabWidget->addTab(ScalePage, "Scales");
	m_pTabWidget->addTab(GridPage, "Axis and Grids");
	m_pTabWidget->addTab(FontPage, "Fonts and BackGround");

	m_pTabWidget->setCurrentIndex(s_ActivePage);
	connect(m_pTabWidget, SIGNAL(currentChanged (int)), this, SLOT(OnActivePage(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pTabWidget);
	mainLayout->addLayout(CommandButtons);
	setLayout(mainLayout);
}




