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
#include "../mainframe.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>


int GraphDlg::s_ActivePage;
void *GraphDlg::s_pMainFrame;


GraphDlg::GraphDlg(QWidget *pParent): QDialog(pParent)
{
	setWindowTitle(tr("Graph Settings"));

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
	connect(m_pctrlTitleClr, SIGNAL(clicked()),  this, SLOT(OnTitleColor()));
	connect(m_pctrlLabelClr, SIGNAL(clicked()),  this, SLOT(OnLabelColor()));

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
		m_pctrlXSel->addItem(tr("Alpha"));
		m_pctrlXSel->addItem(tr("Cl"));
		m_pctrlXSel->addItem(tr("Cd"));
		m_pctrlXSel->addItem(tr("Cd x 10000"));
		m_pctrlXSel->addItem(tr("Cdp"));
		m_pctrlXSel->addItem(tr("Cm"));
		m_pctrlXSel->addItem(tr("Xtr1"));
		m_pctrlXSel->addItem(tr("Xtr2"));
		m_pctrlXSel->addItem(tr("HMom"));
		m_pctrlXSel->addItem(tr("Cpmin"));
		m_pctrlXSel->addItem(tr("Cl/Cd"));
		m_pctrlXSel->addItem(tr("|Cl|^(3/2)/Cd"));
		m_pctrlXSel->addItem(tr("1/Cl^1/2"));
		m_pctrlXSel->addItem(tr("Re"));
		m_pctrlXSel->addItem(tr("XCp"));

		m_pctrlYSel->addItem(tr("Alpha"));
		m_pctrlYSel->addItem(tr("Cl"));
		m_pctrlYSel->addItem(tr("Cd"));
		m_pctrlYSel->addItem(tr("Cd x 10000"));
		m_pctrlYSel->addItem(tr("Cdp"));
		m_pctrlYSel->addItem(tr("Cm"));
		m_pctrlYSel->addItem(tr("Xtr1"));
		m_pctrlYSel->addItem(tr("Xtr2"));
		m_pctrlYSel->addItem(tr("HMom"));
		m_pctrlYSel->addItem(tr("Cpmin"));
		m_pctrlYSel->addItem(tr("Cl/Cd"));
		m_pctrlYSel->addItem(tr("|Cl|^(3/2)/Cd"));
		m_pctrlYSel->addItem(tr("1/Cl^1/2"));
		m_pctrlYSel->addItem(tr("Re"));
		m_pctrlYSel->addItem(tr("XCp"));
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
		m_pctrlYSel->addItem(tr("Local Drag C.Cd/M.A.C."));				//7
		m_pctrlYSel->addItem(tr("Airfoil Pitching moment coef."));		//8
		m_pctrlYSel->addItem(tr("Total Pitching moment coef."));		//9
		m_pctrlYSel->addItem(tr("Reynolds"));							//10
		m_pctrlYSel->addItem(tr("Top Transition x-pos%"));				//11
		m_pctrlYSel->addItem(tr("Bottom Transition x-pos%"));			//12
		m_pctrlYSel->addItem(tr("Centre of Pressure x-pos%"));			//13
		m_pctrlYSel->addItem(tr("Bending moment"));						//14
	}
	else if(m_iGraphType == 62)
	{
		//WingPolar Graph Variables
		m_pctrlXSel->addItem(tr("Alpha"));						//0
		m_pctrlXSel->addItem(tr("Lift coef."));					//1
		m_pctrlXSel->addItem(tr("Viscous drag coef."));			//2
		m_pctrlXSel->addItem(tr("Induced drag coef."));			//3
		m_pctrlXSel->addItem(tr("Total drag coef."));			//4

		m_pctrlXSel->addItem(tr("Total pitching moment coef."));//5
		m_pctrlXSel->addItem(tr("Viscous pitching moment coef."));//6
		m_pctrlXSel->addItem(tr("Induced pitching moment coef."));//7
		m_pctrlXSel->addItem(tr("Total rolling moment coef."));	//8
		m_pctrlXSel->addItem(tr("Total yawing moment coef."));  //9
		m_pctrlXSel->addItem(tr("Viscous yawing moment coef."));//10
		m_pctrlXSel->addItem(tr("Induced yawing moment coef."));//11

		m_pctrlXSel->addItem(tr("Glide ratio Cl/Cd"));			//12
		m_pctrlXSel->addItem(tr("Power factor Cl^(3/2)/Cd"));	//13
		m_pctrlXSel->addItem(tr("1/Rt(Cl)"));					//14
		m_pctrlXSel->addItem(tr("FX (Drag)"));					//15
		m_pctrlXSel->addItem(tr("FY (Side force)"));			//16
		m_pctrlXSel->addItem(tr("FZ (Lift)"));					//17
		m_pctrlXSel->addItem(tr("Vx"));							//18
		m_pctrlXSel->addItem(tr("Vz"));							//19
		m_pctrlXSel->addItem(tr("VInf"));						//20
		m_pctrlXSel->addItem(tr("Descent angle atan(Cd/Cl)"));	//21
		m_pctrlXSel->addItem(tr("Pitching Moment"));			//22
		m_pctrlXSel->addItem(tr("Rolling Moment"));				//23
		m_pctrlXSel->addItem(tr("Yawing Moment"));				//24
        m_pctrlXSel->addItem(tr("Centre of pressure X-Pos"));	//25
		m_pctrlXSel->addItem(tr("Centre of pressure Y-Pos"));	//26
        m_pctrlXSel->addItem(tr("Centre of pressure Z-Pos"));	//27
        m_pctrlXSel->addItem(tr("Bending moment"));				//28
        m_pctrlXSel->addItem(tr("m.g.Vz"));						//29
        m_pctrlXSel->addItem(tr("Efficiency"));					//30
        m_pctrlXSel->addItem(tr("(XCp-Xcg)/MAC"));				//31
        m_pctrlXSel->addItem(tr("Control Variable"));			//32
        m_pctrlXSel->addItem(tr("Cy - Lateral force coef."));	//33
        m_pctrlXSel->addItem(tr("Neutral Point x-position"));	//34
        m_pctrlXSel->addItem(tr("Phugoid Frequency"));	        //35
        m_pctrlXSel->addItem(tr("Phugoid Damping"));	        //36
        m_pctrlXSel->addItem(tr("Short Period Frequency"));	    //37
        m_pctrlXSel->addItem(tr("Short Period Damping"));	    //38
        m_pctrlXSel->addItem(tr("Dutch Roll Frequency"));	    //39
        m_pctrlXSel->addItem(tr("Dutch Roll Damping"));         //40
        m_pctrlXSel->addItem(tr("Roll Damping"));	            //41
        m_pctrlXSel->addItem(tr("Spiral Damping"));	            //42


		//Y variable

		m_pctrlYSel->addItem(tr("Alpha"));						//0
		m_pctrlYSel->addItem(tr("Lift coef."));					//1
		m_pctrlYSel->addItem(tr("Viscous drag coef."));			//2
		m_pctrlYSel->addItem(tr("Induced drag coef."));			//3
		m_pctrlYSel->addItem(tr("Total drag coef."));			//4

		m_pctrlYSel->addItem(tr("Total pitching moment coef."));//5
		m_pctrlYSel->addItem(tr("Viscous pitching moment coef."));//6
		m_pctrlYSel->addItem(tr("Induced pitching moment coef."));//7
		m_pctrlYSel->addItem(tr("Total rolling moment coef."));	//8
		m_pctrlYSel->addItem(tr("Total yawing moment coef."));  //9
		m_pctrlYSel->addItem(tr("Viscous yawing moment coef."));//10
		m_pctrlYSel->addItem(tr("Induced yawing moment coef."));//11

		m_pctrlYSel->addItem(tr("Glide ratio Cl/Cd"));			//12
		m_pctrlYSel->addItem(tr("Power factor Cl^(3/2)/Cd"));	//13
		m_pctrlYSel->addItem(tr("1/Rt(Cl)"));					//14
		m_pctrlYSel->addItem(tr("FX (Drag)"));					//15
		m_pctrlYSel->addItem(tr("FY (Side force)"));			//16
		m_pctrlYSel->addItem(tr("FZ (Lift)"));					//17
		m_pctrlYSel->addItem(tr("Vx"));							//18
		m_pctrlYSel->addItem(tr("Vz"));							//19
		m_pctrlYSel->addItem(tr("VInf"));						//20
		m_pctrlYSel->addItem(tr("Descent angle atan(Cd/Cl)"));	//21
		m_pctrlYSel->addItem(tr("Pitching Moment"));			//22
		m_pctrlYSel->addItem(tr("Rolling Moment"));				//23
		m_pctrlYSel->addItem(tr("Yawing Moment"));				//24
		m_pctrlYSel->addItem(tr("Centre of pressure X-Pos"));	//25
		m_pctrlYSel->addItem(tr("Centre of pressure Y-Pos"));	//26
        m_pctrlYSel->addItem(tr("Centre of pressure Z-Pos"));	//27
        m_pctrlYSel->addItem(tr("Bending moment"));				//28
        m_pctrlYSel->addItem(tr("m.g.Vz"));						//29
        m_pctrlYSel->addItem(tr("Efficiency"));					//30
        m_pctrlYSel->addItem(tr("(XCp-Xcg)/MAC"));				//31
        m_pctrlYSel->addItem(tr("Control Variable"));			//32
        m_pctrlYSel->addItem(tr("Cy - Lateral force coef."));	//33
        m_pctrlYSel->addItem(tr("Neutral Point x-position"));	//34
        m_pctrlYSel->addItem(tr("Phugoid Frequency"));	        //35
        m_pctrlYSel->addItem(tr("Phugoid Damping"));	        //36
        m_pctrlYSel->addItem(tr("Short Period Frequency"));	    //37
        m_pctrlYSel->addItem(tr("Short Period Damping"));	    //38
        m_pctrlYSel->addItem(tr("Dutch Roll Frequency"));	    //39
        m_pctrlYSel->addItem(tr("Dutch Roll Damping"));         //40
        m_pctrlYSel->addItem(tr("Roll Damping"));	            //41
        m_pctrlYSel->addItem(tr("Spiral Damping"));	            //42

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
	}
	if(!m_pctrlAutoYMinUnit->isChecked())
	{
		MinUnit = m_pctrlYMinorUnit->Value();
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




void GraphDlg::OnOK()
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
	QPalette palette = m_pctrlTitleClr->palette();
	QColor listColor = palette.color(QPalette::Button);

	if(listColor.isValid())
	{
//		palette.setColor(QPalette::Background, m_pGraph->GetBackColor());
		palette.setColor(QPalette::Button, m_pGraph->GetBackColor());

		palette.setColor(QPalette::ButtonText, m_pGraph->GetTitleColor());
		m_pctrlTitleClr->setPalette(palette);

		palette.setColor(QPalette::ButtonText, m_pGraph->GetLabelColor());
		m_pctrlLabelClr->setPalette(palette);
	}
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
	m_pGraph->GetLabelLogFont(&font);
	m_pctrlLabelButton->setText(font.family());
	m_pctrlLabelButton->setFont(font);

	m_pGraph->GetTitleLogFont(&font);
	m_pctrlTitleButton->setText(font.family());
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
	ScalePage    = new QWidget(this);
	GridPage     = new QWidget(this);
	FontPage     = new QWidget(this);
	VariablePage = new QWidget(this);

	//________Variable Page______________________
	QVBoxLayout *VariablePageLayout = new QVBoxLayout(this);
	{
		QHBoxLayout *AxisNamesLayout = new QHBoxLayout;
		{
			QLabel *YAxis = new QLabel(tr("YAxis"));
			QLabel *vs = new QLabel(tr("vs."));
			QLabel *XAxis = new QLabel(tr("XAxis"));
			AxisNamesLayout->addStretch(1);
			AxisNamesLayout->addWidget(YAxis);
			AxisNamesLayout->addStretch(1);
			AxisNamesLayout->addWidget(vs);
			AxisNamesLayout->addStretch(1);
			AxisNamesLayout->addWidget(XAxis);
			AxisNamesLayout->addStretch(1);
		}

		QHBoxLayout *VariableBoxLayout = new QHBoxLayout;
		{
			m_pctrlXSel = new QListWidget;
			m_pctrlYSel = new QListWidget;
			VariableBoxLayout->addWidget(m_pctrlYSel);
			VariableBoxLayout->addWidget(m_pctrlXSel);
		}

		VariablePageLayout->addLayout(AxisNamesLayout);
		VariablePageLayout->addLayout(VariableBoxLayout);
	}
	VariablePage->setLayout(VariablePageLayout);

	//________Font Page______________________

	QVBoxLayout *FontPageLayout = new QVBoxLayout;
	{

		QGroupBox *FontBox = new QGroupBox(tr("Fonts"));
		{
			QGridLayout *FontButtonsLayout = new QGridLayout;
			{
				QLabel *lab1  = new QLabel(tr("Title"));
				QLabel *lab2  = new QLabel(tr("Label"));
				QLabel *lab402  = new QLabel(tr("Font"));
				QLabel *lab403  = new QLabel(tr("Color"));
				lab1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
				lab2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
				lab402->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				lab403->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
				FontButtonsLayout->addWidget(lab402,1,2);
				FontButtonsLayout->addWidget(lab403,1,3);
				FontButtonsLayout->addWidget(lab1,2,1);
				FontButtonsLayout->addWidget(lab2,3,1);

				m_pctrlTitleButton  = new QPushButton(tr("Set Title Font"));
				m_pctrlLabelButton  = new QPushButton(tr("Set Label Font"));
				FontButtonsLayout->addWidget(m_pctrlTitleButton,2,2);
				FontButtonsLayout->addWidget(m_pctrlLabelButton,3,2);

				m_pctrlTitleClr  = new QPushButton(tr("Title Color"));
				m_pctrlLabelClr  = new QPushButton(tr("Label Color"));

				FontButtonsLayout->addWidget(m_pctrlTitleClr,2,3);
				FontButtonsLayout->addWidget(m_pctrlLabelClr,3,3);
			}
			FontBox->setLayout(FontButtonsLayout);
		}

		QGroupBox *BackBox = new QGroupBox(tr("BackGround"));
		{
			QGridLayout *BackDataLayout = new QGridLayout;
			{
				QLabel *GraphBackLabel = new QLabel(tr("Graph Background"));
				GraphBackLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
				m_pctrlGraphBorder = new QCheckBox(tr("Graph Border"));

				m_pctrlGraphBack = new ColorButton;
				m_pctrlGraphBack->setMinimumWidth(100);
				m_pctrlBorderStyle = new LineBtn(this);
				m_pctrlBorderStyle->setMinimumWidth(100);

				BackDataLayout->addWidget(GraphBackLabel,1,1);
				BackDataLayout->addWidget(m_pctrlGraphBorder,2,1,1,1,Qt::AlignRight | Qt::AlignVCenter);

				BackDataLayout->addWidget(m_pctrlGraphBack,1,2);
				BackDataLayout->addWidget(m_pctrlBorderStyle,2,2);

				BackDataLayout->setColumnStretch(0,1);
			}
			BackBox->setLayout(BackDataLayout);
		}

		FontPageLayout->addWidget(FontBox);
		FontPageLayout->addStretch(1);
		FontPageLayout->addWidget(BackBox);
		FontPageLayout->addStretch(1);
	}
	FontPage->setLayout(FontPageLayout);
	//________End Font Page______________________

	//________Scale Page______________________

	QGridLayout *ScalePageLayout = new QGridLayout;
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
		ScalePageLayout->addWidget(MinLabel,4,1);
		ScalePageLayout->addWidget(MaxLabel,5,1);
		ScalePageLayout->addWidget(OriginLabel,6,1);
		ScalePageLayout->addWidget(UnitLabel,7,1);

		m_pctrlXAuto    = new QCheckBox(tr("Auto Scale"));
		m_pctrlXMin     = new DoubleEdit;
		m_pctrlXMax     = new DoubleEdit;
		m_pctrlXOrigin  = new DoubleEdit;
		m_pctrlXUnit    = new DoubleEdit;

		ScalePageLayout->addWidget(XAxis2,1,2);
		ScalePageLayout->addWidget(m_pctrlXAuto,3,2);
		ScalePageLayout->addWidget(m_pctrlXMin,4,2);
		ScalePageLayout->addWidget(m_pctrlXMax,5,2);
		ScalePageLayout->addWidget(m_pctrlXOrigin,6,2);
		ScalePageLayout->addWidget(m_pctrlXUnit,7,2);

		m_pctrlYInverted = new QCheckBox(tr("Inverted Axis"));
		m_pctrlYAuto     = new QCheckBox(tr("Auto Scale"));
		m_pctrlYMin      = new DoubleEdit;
		m_pctrlYMax      = new DoubleEdit;
		m_pctrlYOrigin   = new DoubleEdit;
		m_pctrlYUnit     = new DoubleEdit;

		ScalePageLayout->addWidget(YAxis2,1,3);
		ScalePageLayout->addWidget(m_pctrlYInverted,2,3);
		ScalePageLayout->addWidget(m_pctrlYAuto,3,3);
		ScalePageLayout->addWidget(m_pctrlYMin,4,3);
		ScalePageLayout->addWidget(m_pctrlYMax,5,3);
		ScalePageLayout->addWidget(m_pctrlYOrigin,6,3);
		ScalePageLayout->addWidget(m_pctrlYUnit,7,3);
	}
	ScalePage->setLayout(ScalePageLayout);
	//________End Scale Page______________________

	//________Axis Page______________________
	QGridLayout *AxisDataLayout = new QGridLayout;
	{
		AxisDataLayout->setRowStretch(0,1);
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

		AxisDataLayout->addWidget(AxisStyleLabel,1,1);
		AxisDataLayout->addWidget(m_pctrlXMajGridShow,2,1);
		AxisDataLayout->addWidget(m_pctrlYMajGridShow,3,1);
		AxisDataLayout->addWidget(m_pctrlXMinGridShow,4,1);
		AxisDataLayout->addWidget(m_pctrlYMinGridShow,5,1);

		AxisDataLayout->addWidget(m_pctrlAxisStyle,1,2);
		AxisDataLayout->addWidget(m_pctrlXMajGridStyle,2,2);
		AxisDataLayout->addWidget(m_pctrlYMajGridStyle,3,2);
		AxisDataLayout->addWidget(m_pctrlXMinGridStyle,4,2);
		AxisDataLayout->addWidget(m_pctrlYMinGridStyle,5,2);

		AxisDataLayout->addWidget(m_pctrlAutoXMinUnit,4,3);
		AxisDataLayout->addWidget(m_pctrlAutoYMinUnit,5,3);
		AxisDataLayout->addWidget(m_pctrlXMinorUnit,4,4);
		AxisDataLayout->addWidget(m_pctrlYMinorUnit,5,4);
	}
	GridPage->setLayout(AxisDataLayout);
	//________End Axis Page______________________


	m_pTabWidget->addTab(VariablePage, tr("Variables"));
	m_pTabWidget->addTab(ScalePage, tr("Scales"));
	m_pTabWidget->addTab(GridPage, tr("Axis and Grids"));
	m_pTabWidget->addTab(FontPage, tr("Fonts and BackGround"));

	m_pTabWidget->setCurrentIndex(s_ActivePage);
	connect(m_pTabWidget, SIGNAL(currentChanged (int)), this, SLOT(OnActivePage(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		mainLayout->addWidget(m_pTabWidget);
		mainLayout->addLayout(CommandButtons);
	}
	setLayout(mainLayout);
}




