/****************************************************************************

	CtrlPolarDlg Class
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
  
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/WPolar.h"
#include "CtrlPolarDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <math.h>


CtrlPolarDlg::CtrlPolarDlg()
{
	setWindowTitle(tr("Control Polar Definition"));
	m_pMainFrame = NULL;
	m_pMiarex    = NULL;

	m_WPolarName = "WPolar Name";

	m_bAutoName = true;
	m_XCmRef   = 0.0;
	m_WingTilt = 0.0;
	m_StabTilt = 0.0;
	m_pWing  = NULL;
	m_pWing2 = NULL;
	m_pStab  = NULL;
	m_pFin   = NULL;
	m_pctrlControlTable = NULL;
	m_pControlModel   = NULL;
	m_pCtrlDelegate   = NULL;

	memset(m_MinControl, 0, sizeof(m_MinControl));
	memset(m_MaxControl, 0, sizeof(m_MaxControl));
	memset(m_bActiveControl, 0, sizeof(m_bActiveControl));

	m_RefAreaType = 1;
	m_UnitType   = 1;
	m_Density    = 1.225;
	m_Viscosity  = 1.5e-5;

	m_bActiveControl[0] = true;
	m_MinControl[0] =  0.00;//xcmref
	m_MaxControl[0] =  0.100;

	m_bActiveControl[1] = false;
	m_MinControl[1] = 0.0;//wing tilt
	m_MaxControl[1] = 0.0;

	m_bActiveControl[2] = false;
	m_MinControl[2] =   0.0;//stab tilt
	m_MaxControl[2] =   0.0;

	m_bActiveControl[3] = false;
	m_MinControl[3] = 0.0;//flap angle
	m_MaxControl[3] = 0.0;

	m_bActiveControl[4] = false;
	m_MinControl[4] = 0.0;//flap angle
	m_MaxControl[4] = 0.0;

	m_bViscous = true;
	m_QInf     = 10.0;
	m_Weight   = 1.0;
	m_Type     = 5;

	
	m_WPolarName = "T5";
	SetupLayout();
	Connect();
}


void CtrlPolarDlg::Connect()
{
	connect(m_pctrlType5, SIGNAL(clicked()), this, SLOT(OnType()));
	connect(m_pctrlType6, SIGNAL(clicked()), this, SLOT(OnType()));
	connect(m_pctrlUnit1,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	m_pControlModel = new QStandardItemModel;
	m_pControlModel->setRowCount(10);//temporary
	m_pControlModel->setColumnCount(4);

	connect(m_pctrlDensity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWeight, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlQInf, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));

	m_pControlModel->setHeaderData(0, Qt::Horizontal, tr("Control Name"));
	m_pControlModel->setHeaderData(1, Qt::Horizontal, tr("Active (1/0)"));
	m_pControlModel->setHeaderData(2, Qt::Horizontal, tr("Min"));
	m_pControlModel->setHeaderData(3, Qt::Horizontal, tr("Max"));

	m_pctrlControlTable->setModel(m_pControlModel);
	m_pctrlControlTable->setWindowTitle(tr("Controls"));
}




void CtrlPolarDlg::FillControlList()
{
	m_pControlModel->setRowCount(m_nControls);//temporary
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int i;
	GetLengthUnit(str, pMainFrame->m_LengthUnit);

	ind = m_pControlModel->index(0, 0, QModelIndex());
	m_pControlModel->setData(ind, tr("XCmRef (")+str+")");

	ind = m_pControlModel->index(0, 1, QModelIndex());
	if(m_bActiveControl[0])	m_pControlModel->setData(ind, 1);
	else                    m_pControlModel->setData(ind, 0);

	ind = m_pControlModel->index(0, 2, QModelIndex());
	m_pControlModel->setData(ind, m_MinControl[0]*pMainFrame->m_mtoUnit);

	ind = m_pControlModel->index(0, 3, QModelIndex());
	m_pControlModel->setData(ind, m_MaxControl[0]*pMainFrame->m_mtoUnit);

	m_nControls = 1;
	if(m_pPlane)
	{
		ind = m_pControlModel->index(1, 0, QModelIndex());
		m_pControlModel->setData(ind, tr("Wing Tilt (")+QString::fromUtf8("°")+")");

		ind = m_pControlModel->index(1, 1, QModelIndex());
		if(m_bActiveControl[1])	m_pControlModel->setData(ind, 1);
		else                    m_pControlModel->setData(ind, 0);

		ind = m_pControlModel->index(1, 2, QModelIndex());
		m_pControlModel->setData(ind, m_MinControl[1]);

		ind = m_pControlModel->index(1, 3, QModelIndex());
		m_pControlModel->setData(ind, m_MaxControl[1]);

		++m_nControls;

		if(m_pStab)
		{
			ind = m_pControlModel->index(2, 0, QModelIndex());
			m_pControlModel->setData(ind, tr("Elevator Tilt ")+QString::fromUtf8("(°)"));

			ind = m_pControlModel->index(2, 1, QModelIndex());
			if(m_bActiveControl[2])	m_pControlModel->setData(ind, 1);
			else                    m_pControlModel->setData(ind, 0);

			ind = m_pControlModel->index(2, 2, QModelIndex());
			m_pControlModel->setData(ind, m_MinControl[2]);

			ind = m_pControlModel->index(2, 3, QModelIndex());
			m_pControlModel->setData(ind, m_MaxControl[2]);

			++m_nControls;
		}
	}

	for(i=0; i<m_pWing->m_nFlaps; i++)
	{
		ind = m_pControlModel->index(i+m_nControls, 0, QModelIndex());
		strong = QString(tr("Wing Flap angle %1 ")+QString::fromUtf8("(°)")).arg(i+1);
		m_pControlModel->setData(ind, strong);
		ind = m_pControlModel->index(i+m_nControls, 1, QModelIndex());
		if(m_bActiveControl[i+m_nControls]) m_pControlModel->setData(ind, 1);
		else                                m_pControlModel->setData(ind, 0);

		ind = m_pControlModel->index(i+m_nControls, 2, QModelIndex());
		m_pControlModel->setData(ind, m_MinControl[i+m_nControls]);

		ind = m_pControlModel->index(i+m_nControls, 3, QModelIndex());
		m_pControlModel->setData(ind, m_MaxControl[i+m_nControls]);
	}
	m_nControls += m_pWing->m_nFlaps;


	if(m_pStab)
	{
		for(i=0; i<m_pStab->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+m_nControls, 0, QModelIndex());
			strong = QString(tr("Elevator Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

			ind = m_pControlModel->index(i+m_nControls, 1, QModelIndex());
			if(m_bActiveControl[i+m_nControls]) m_pControlModel->setData(ind, 1);
			else                                m_pControlModel->setData(ind, 0);

			ind = m_pControlModel->index(i+m_nControls, 2, QModelIndex());
			m_pControlModel->setData(ind, m_MinControl[i+m_nControls]);

			ind = m_pControlModel->index(i+m_nControls, 3, QModelIndex());
			m_pControlModel->setData(ind, m_MaxControl[i+m_nControls]);
		}
		m_nControls += m_pStab->m_nFlaps;
	}


	bool bViscous=true;
	for(i=3; i<m_nControls; i++)
	{
		if(m_bActiveControl[i]) bViscous = false;
	}
	if(!bViscous)
	{
		m_pctrlViscous->setChecked(false);
		m_pctrlViscous->setEnabled(false);
		m_bViscous = false;
	}
	else
	{
		if(m_bViscous) m_pctrlViscous->setChecked(true);
		m_pctrlViscous->setEnabled(true);
	}
}


void CtrlPolarDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	m_pctrlDensity->SetPrecision(5);
	m_pctrlViscosity->SetPrecision(3);

	QString str;
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);

	m_pctrlSpeedUnit->setText(str);
	m_pctrlQInf->SetValue(m_QInf*pMainFrame->m_mstoUnit);

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pctrlWeightUnit->setText(str);
	m_pctrlWeight->SetValue(m_Weight*pMainFrame->m_kgtoUnit);

	if(m_Type==6) m_pctrlType6->setChecked(true);
	else          m_pctrlType5->setChecked(true);

	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);
	OnUnit();

	if(m_RefAreaType==1) m_pctrlArea1->setChecked(true);
	else                 m_pctrlArea2->setChecked(true);

	m_pctrlUFOName->setText(m_UFOName);
	m_pctrlWPolarName->setText(m_WPolarName);

	m_nControls = 1;//CG position
	m_nControls += m_pWing->m_nFlaps;

	if(m_pPlane) 
	{
		m_nControls++; // Wing Tilt
		if(m_pStab)
		{
			m_nControls++;//stab tilt
			m_nControls += m_pStab->m_nFlaps;
		}
	}

	if(m_pPlane) m_pctrlUFOName->setText(m_pPlane->m_PlaneName);
	else         m_pctrlUFOName->setText(m_pWing->m_WingName);

	m_pctrlAutoName->setChecked(m_bAutoName);

	FillControlList();
	SetWPolarName();
	m_pctrlControlTable->setFocus();
}


void CtrlPolarDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				ReadCtrlData();
				SetWPolarName();
				OKButton->setFocus();
				return;
			}
			else
			{
				OnOK();
				return;
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
		}
		default:
			event->ignore();
	}

}



void CtrlPolarDlg::OnArea()
{
	if(m_pctrlArea1->isChecked())
	{
		m_RefAreaType = 1;
	}
	else if(m_pctrlArea2->isChecked())
	{
		m_RefAreaType = 2;
	}
	SetWPolarName();
}




void CtrlPolarDlg::OnCellChanged(QWidget *pWidget)
{
	ReadCtrlData();
	SetWPolarName();
}


void CtrlPolarDlg::OnEditingFinished()
{
	ReadParams();
	SetWPolarName();
}


void CtrlPolarDlg::OnUnit() 
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m2/s");
	}
	else 
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft2/s");
	}
	SetDensity();
}


void CtrlPolarDlg::OnOK()
{
	QString strong;
	ReadCtrlData();
	ReadParams();

	bool bActive = false;

	for(int i=0; i<m_nControls; i++)
	{
		bActive = bActive || m_bActiveControl[i];
		if(bActive) break;
	}
	if(!bActive)
	{
		strong = tr("No Active Control. Continue ?");
		if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
	}

	CWPolar * pWPolarNew;
	m_WPolarName = m_pctrlWPolarName->text();

	int LineLength = m_WPolarName.length();

	if(!LineLength)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Must enter a name"));
		m_pctrlWPolarName->setFocus();
		return;
	}
	else
	{
		int size = (int)m_poaXPolar->size();
		for (int j=0; j<size; j++)
		{
			pWPolarNew = (CWPolar*)m_poaXPolar->at(j);
			if (pWPolarNew->m_PlrName == m_WPolarName && pWPolarNew->m_UFOName  == m_UFOName)
			{
				QMessageBox::warning(this,tr("Warning"), tr("The polar's name already exists"));
				return;
			}
		}
	}

	if(m_pctrlViscous->isChecked()) m_bViscous = true;
	m_QInf = m_pctrlQInf->GetValue();

	if(m_pctrlType5->isChecked()) m_Type=5;
	else                          m_Type=6;
	accept();
}


void CtrlPolarDlg::OnType()
{
	if(m_pctrlType5->isChecked()) m_Type=5;
	else                          m_Type=6;
	SetWPolarName();
}




void CtrlPolarDlg::OnViscous()
{
	m_bViscous = m_pctrlViscous->isChecked();
	SetWPolarName();
}


void CtrlPolarDlg::ReadCtrlData()
{
	int i, k;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	for(i=0; i<m_nControls; i++)
	{
		k    = m_pControlModel->index(i, 1, QModelIndex()).data().toInt();
		if(k) m_bActiveControl[i] = true;
		else  m_bActiveControl[i] = false;

		m_MinControl[i] = m_pControlModel->index(i, 2, QModelIndex()).data().toDouble();
		m_MaxControl[i] = m_pControlModel->index(i, 3, QModelIndex()).data().toDouble();
	}

	for(i=m_nControls; i<4*MAXCONTROLS; i++) m_bActiveControl[i]=false;

	m_MinControl[0] /= pMainFrame->m_mtoUnit;
	m_MaxControl[0] /= pMainFrame->m_mtoUnit;

}



void CtrlPolarDlg::ReadParams()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_pctrlUnit1->isChecked())
	{
		m_Density   = m_pctrlDensity->GetValue();
		m_Viscosity = m_pctrlViscosity->GetValue();
	}
	else
	{
		m_Density   = m_pctrlDensity->GetValue()   / 0.00194122;
		m_Viscosity = m_pctrlViscosity->GetValue() / 10.7182881;
	}

	m_QInf   = fabs(m_pctrlQInf->GetValue())   / pMainFrame->m_mstoUnit;
	m_Weight = fabs(m_pctrlWeight->GetValue()) / pMainFrame->m_kgtoUnit;

	SetDensity();
}

void CtrlPolarDlg::resizeEvent(QResizeEvent *event)
{
	int w = m_pctrlControlTable->width();
	int w3  = (int)((double)w/3.0);
	int w6 = (int)((double)w/6.0);
	int w4 = (int)((double)w/4.0*.9);

	m_pctrlControlTable->setColumnWidth(0,w3);
	m_pctrlControlTable->setColumnWidth(1,w6);
	m_pctrlControlTable->setColumnWidth(2,w4);
	m_pctrlControlTable->setColumnWidth(3,w4);
}


void CtrlPolarDlg::SetDensity()
{
	int exp, precision;
	if(m_pctrlUnit1->isChecked())
	{
		exp = (int)log(m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(m_Density);
	}
	else
	{
		exp = (int)log(m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_pctrlDensity->SetPrecision(precision);
		m_pctrlDensity->SetValue(m_Density* 0.00194122);
	}
}



void CtrlPolarDlg::SetupLayout()
{
	QVBoxLayout *NameLayout = new QVBoxLayout;
	m_pctrlUFOName = new QLabel(tr("Wing Name"));
	m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
	m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
	NameLayout->addWidget(m_pctrlUFOName);
	NameLayout->addWidget(m_pctrlAutoName);
	NameLayout->addWidget(m_pctrlWPolarName);
	QGroupBox *NameGroup = new QGroupBox(tr("Polar Name"));
	NameGroup->setLayout(NameLayout);

	QVBoxLayout *TypeLayout = new QVBoxLayout;
	m_pctrlType5 = new QRadioButton(tr("Type 5 (Fixed Speed)"));
	m_pctrlType6 = new QRadioButton(tr("Type 6 (Fixed Lift)"));
	m_pctrlViscous = new QCheckBox(tr("Viscous"));
	TypeLayout->addWidget(m_pctrlType5);
	TypeLayout->addWidget(m_pctrlType6);
	TypeLayout->addWidget(m_pctrlViscous);
	QGroupBox *TypeGroup = new QGroupBox(tr("Polar Type"));
	TypeGroup->setLayout(TypeLayout);

	QGridLayout *PlaneLayout = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("Free Stream Speed"));
	QLabel *lab2 = new QLabel(tr("Plane Weight"));
	m_pctrlQInf    = new FloatEdit(10.05);
	m_pctrlWeight  = new FloatEdit(1.234);
	m_pctrlSpeedUnit   = new QLabel("m/s");
	m_pctrlWeightUnit  = new QLabel("kg");
	PlaneLayout->addWidget(lab1,1,1);
	PlaneLayout->addWidget(lab2,2,1);
	PlaneLayout->addWidget(m_pctrlQInf,1,2);
	PlaneLayout->addWidget(m_pctrlWeight,2,2);
	PlaneLayout->addWidget(m_pctrlSpeedUnit ,1,3);
	PlaneLayout->addWidget(m_pctrlWeightUnit ,2,3);
	QGroupBox *PlaneGroup = new QGroupBox(tr("Plane and Flight Data"));
	PlaneGroup->setLayout(PlaneLayout);

	QGridLayout *AeroDataLayout = new QGridLayout;
	QLabel *lab9 = new QLabel(tr("Unit"));
	m_pctrlUnit1 = new QRadioButton(tr("International"));
	m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
	m_pctrlRho           = new QLabel("r=");
	m_pctrlDensity       = new FloatEdit(1.500e-5,3);
	m_pctrlDensityUnit   = new QLabel("kg/m3");
	m_pctrlNu            = new QLabel("n=");
	m_pctrlRho->setFont(QFont("Symbol"));
	m_pctrlNu->setFont(QFont("Symbol"));

	m_pctrlViscosity     = new FloatEdit(1.225,3);
	m_pctrlViscosityUnit = new QLabel("m2/s");
	AeroDataLayout->addWidget(lab9,1,1);
	AeroDataLayout->addWidget(m_pctrlUnit1,1,2);
	AeroDataLayout->addWidget(m_pctrlUnit2,1,3);
	AeroDataLayout->addWidget(m_pctrlRho,2,1);
	AeroDataLayout->addWidget(m_pctrlDensity,2,2);
	AeroDataLayout->addWidget(m_pctrlDensityUnit,2,3);
	AeroDataLayout->addWidget(m_pctrlNu,3,1);
	AeroDataLayout->addWidget(m_pctrlViscosity,3,2);
	AeroDataLayout->addWidget(m_pctrlViscosityUnit,3,3);
	QGroupBox *AeroDataGroup = new QGroupBox(tr("Aerodynamic Data"));
	AeroDataGroup->setLayout(AeroDataLayout);

	QHBoxLayout *AreaOptions = new QHBoxLayout;
	m_pctrlArea1 = new QRadioButton(tr("Wing Planform Area"));
	m_pctrlArea2 = new QRadioButton(tr("Wing Planform Area projected on xy plane"));
	AreaOptions->addWidget(m_pctrlArea1);
	AreaOptions->addWidget(m_pctrlArea2);
	QGroupBox *AreaBox = new QGroupBox(tr("Reference Area for Aero Coefficients"));
	AreaBox->setLayout(AreaOptions);

	QGridLayout *DataLayout = new QGridLayout;
	DataLayout->addWidget(NameGroup,1,1);
	DataLayout->addWidget(TypeGroup, 1,2);
	DataLayout->addWidget(PlaneGroup,2,1);
	DataLayout->addWidget(AeroDataGroup,2,2);
	DataLayout->addWidget(AreaBox,3,1,1,2);

	m_pctrlControlTable = new QTableView(this);
	m_pctrlControlTable->setMinimumWidth(400);
	m_pctrlControlTable->setMinimumHeight(200);
	m_pctrlControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlControlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pCtrlDelegate = new CtrlTableDelegate;
	m_pctrlControlTable->setItemDelegate(m_pCtrlDelegate);
	m_pCtrlDelegate->m_pCtrlModel = m_pControlModel;
	connect(m_pCtrlDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	int  *precision = new int[6];
	precision[0]  = 2;
	precision[1]  = 0;
	precision[2]  = 3;
	precision[3]  = 3;
	precision[4]  = 3;
	precision[5]  = 3;

	m_pCtrlDelegate->m_Precision = precision;

	QVBoxLayout *NoteLayout = new QVBoxLayout;
	QLabel *lab10 = new QLabel(tr("Note 1 : the analysis is necessarily of type VLM1"));
	QLabel *lab11 = new QLabel(tr("Note 2 : if the control for XCmRef is inactive, then the minimal value will be used for the analysis"));
	QLabel *lab12 = new QLabel(tr("Note 3 : the analysis may be of the vicous type only if all the flap controls are inactive"));
	NoteLayout->addWidget(lab10);
	NoteLayout->addWidget(lab11);
	NoteLayout->addWidget(lab12);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setDefault(true);
	CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addLayout(DataLayout);
	MainLayout->addWidget(m_pctrlControlTable);
	MainLayout->addLayout(NoteLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
}




void CtrlPolarDlg::SetWPolarName()
{
	if(!m_bAutoName) return;
	QString str, strong;
	int i, nCtrl;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	if (m_pctrlType5->isChecked())
	{
		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		m_WPolarName = QString("T5-%1 ").arg(m_QInf * pMainFrame->m_mstoUnit,0,'f',1);
		m_WPolarName += str;
	}
	else
	{
		GetWeightUnit(str, pMainFrame->m_WeightUnit);
		m_WPolarName = QString("T6-%1 ").arg(m_Weight*pMainFrame->m_kgtoUnit,0,'f',3);
		m_WPolarName += str;
	}

	if(m_bActiveControl[0])
	{
		GetLengthUnit(str, pMainFrame->m_LengthUnit);
		strong = QString("-CG(%1/%2)")
				   .arg(m_MinControl[0]*pMainFrame->m_mtoUnit,0,'f',2)
				   .arg(m_MaxControl[0]*pMainFrame->m_mtoUnit,0,'f',2);
		m_WPolarName += strong + str ;
	}
	else
	{
		GetLengthUnit(str, pMainFrame->m_LengthUnit);
		strong = QString("-CG(%1)").arg(m_MinControl[0]*pMainFrame->m_mtoUnit,0,'f',2);
		m_WPolarName += strong + str;
	}

	nCtrl = 1;//CG control is always present

	if(m_pPlane)
	{
		if(m_bActiveControl[1] && m_pPlane)
		{
			strong = QString("-W(%1 deg/%2 deg)")
							   .arg(m_MinControl[1],0,'f',1)
							   .arg(m_MaxControl[1],0,'f',1);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	if(m_pPlane && m_pStab)
	{
		if(m_bActiveControl[2])
		{
			strong = QString("-E(%1 deg/%2 deg)").arg(m_MinControl[2],0,'f',1).arg(m_MaxControl[2],0,'f',1);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	for(i=0; i<m_pWing->m_nFlaps; i++)
	{
		if(m_bActiveControl[i+nCtrl])
		{
			strong = QString("-WF%1(%2 deg/%3 deg)")
					 .arg(i+1)
					 .arg(m_MinControl[i+nCtrl],0,'f',1)
					 .arg(m_MaxControl[i+nCtrl],0,'f',1);
			m_WPolarName += strong;
		}
	}
	nCtrl += m_pWing->m_nFlaps;

	if(m_pPlane && m_pStab)
	{
		for(i=0; i<m_pStab->m_nFlaps; i++)
		{
			if(m_bActiveControl[i+nCtrl])
			{
				strong = QString("-EF%1(%2 deg/%3 deg)").arg(i+1).arg(m_MinControl[i+nCtrl]).arg(m_MaxControl[i+nCtrl]);
				m_WPolarName += strong;
			}
		}
	}

	if(!m_bViscous)
	{
		m_WPolarName += "-Inviscid";
	}
	if(m_RefAreaType==2) m_WPolarName += "-proj_area";

	m_pctrlWPolarName->setText(m_WPolarName);
}







