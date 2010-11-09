/****************************************************************************

	StabPolarDlg Class
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
  
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/WPolar.h"
#include "Miarex.h"
#include "StabPolarDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <math.h>
#include <QtDebug>

void *StabPolarDlg::s_pMainFrame;
void *StabPolarDlg::s_pMiarex;

StabPolarDlg::StabPolarDlg()
{
	setWindowTitle(tr("Stability Polar Definition"));
	m_WPolarName = "Stability Polar Name";

	m_bAutoName = true;
	m_bAutoInertia = true;
	m_bThinSurfaces = true;

	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;

	m_WingTilt = 0.0;
	m_StabTilt = 0.0;
	m_Mass     = 0.0;

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
	m_UnitType    = 1;

	m_Density    = 1.225;
	m_Viscosity  = 1.5e-5;

	m_SideSlip  = 0.0;
	m_BankAngle = 0.0;

	m_nControls = 0;
	
	m_bViscous = true;
	m_Type     = 7;
	
	m_WPolarName = "T7";
	SetupLayout();
	Connect();
}


void StabPolarDlg::Connect()
{
	connect(m_pctrlUnit1,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlUnit2,   SIGNAL(clicked()), this, SLOT(OnUnit()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlArea1, SIGNAL(clicked()),this, SLOT(OnArea()));
	connect(m_pctrlArea2, SIGNAL(clicked()),this, SLOT(OnArea()));

	connect(m_pctrlWingMethod2, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));
	connect(m_pctrlWingMethod3, SIGNAL(toggled(bool)), this, SLOT(OnMethod()));

	connect(m_pctrlDensity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlViscosity, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlBeta, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlPhi, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlWPolarName, SIGNAL(editingFinished()), this, SLOT(OnWPolarName()));
	connect(m_pctrlAutoName, SIGNAL(clicked()), this, SLOT(OnAutoName()));

	connect(OKButton,     SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	m_pControlModel = new QStandardItemModel;
	m_pControlModel->setRowCount(10);//temporary
	m_pControlModel->setColumnCount(4);

	m_pControlModel->setHeaderData(0, Qt::Horizontal, tr("Control Name"));
	m_pControlModel->setHeaderData(1, Qt::Horizontal, tr("Active (1/0)"));
	m_pControlModel->setHeaderData(2, Qt::Horizontal, tr("Min"));
	m_pControlModel->setHeaderData(3, Qt::Horizontal, tr("Max"));

	m_pctrlControlTable->setModel(m_pControlModel);
	m_pctrlControlTable->setWindowTitle(tr("Controls"));
}



void StabPolarDlg::FillUFOInertia()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_pPlane)
	{
		m_Mass = m_pPlane->m_TotalMass;
		m_CoG = m_pPlane->m_CoG;
		m_CoGIxx = m_pPlane->m_CoGIxx;
		m_CoGIyy = m_pPlane->m_CoGIyy;
		m_CoGIzz = m_pPlane->m_CoGIzz;
		m_CoGIxz = m_pPlane->m_CoGIxz;
	}
	else if(m_pWing)
	{
		m_Mass = m_pWing->m_TotalMass;
		m_CoG = m_pWing->m_CoG;
		m_CoGIxx = m_pWing->m_CoGIxx;
		m_CoGIyy = m_pWing->m_CoGIyy;
		m_CoGIzz = m_pWing->m_CoGIzz;
		m_CoGIxz = m_pWing->m_CoGIxz;
	}

	m_pctrlMass->SetValue(m_Mass*pMainFrame->m_kgtoUnit);
	m_pctrlCoGx->SetValue(m_CoG.x*pMainFrame->m_mtoUnit);
	m_pctrlCoGz->SetValue(m_CoG.z*pMainFrame->m_mtoUnit);
	m_pctrlIxx->SetValue(m_CoGIxx*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlIyy->SetValue(m_CoGIyy*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlIzz->SetValue(m_CoGIzz*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
	m_pctrlIxz->SetValue(m_CoGIxz*pMainFrame->m_kgtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit);
}



void StabPolarDlg::FillControlList()
{
	m_pControlModel->setRowCount(m_nControls);//temporary
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	GetLengthUnit(str, pMainFrame->m_LengthUnit);

	m_nControls = 0;
	if(m_pPlane)
	{
		ind = m_pControlModel->index(m_nControls, 0, QModelIndex());
		m_pControlModel->setData(ind, tr("Wing Tilt (")+QString::fromUtf8("°")+")");

		ind = m_pControlModel->index(m_nControls, 1, QModelIndex());
		if(m_bActiveControl[0])	m_pControlModel->setData(ind, 1);
		else                    m_pControlModel->setData(ind, 0);

		ind = m_pControlModel->index(m_nControls, 2, QModelIndex());
		m_pControlModel->setData(ind, m_MinControl[0]);

		ind = m_pControlModel->index(m_nControls, 3, QModelIndex());
		m_pControlModel->setData(ind, m_MaxControl[0]);

		++m_nControls;

		if(m_pStab)
		{
			ind = m_pControlModel->index(m_nControls, 0, QModelIndex());
			m_pControlModel->setData(ind, tr("Elevator Tilt ")+QString::fromUtf8("(°)"));

			ind = m_pControlModel->index(m_nControls, 1, QModelIndex());
			if(m_bActiveControl[1])	m_pControlModel->setData(ind, 1);
			else                    m_pControlModel->setData(ind, 0);

			ind = m_pControlModel->index(m_nControls, 2, QModelIndex());
			m_pControlModel->setData(ind, m_MinControl[1]);

			ind = m_pControlModel->index(m_nControls, 3, QModelIndex());
			m_pControlModel->setData(ind, m_MaxControl[1]);

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
	if(m_pFin)
	{
		for(i=0; i<m_pFin->m_nFlaps; i++)
		{
			ind = m_pControlModel->index(i+m_nControls, 0, QModelIndex());
			strong = QString(tr("Fin Flap %1 ")+QString::fromUtf8("(°)")).arg(i+1);
			m_pControlModel->setData(ind, strong);

			ind = m_pControlModel->index(i+m_nControls, 1, QModelIndex());
			if(m_bActiveControl[i+m_nControls]) m_pControlModel->setData(ind, 1);
			else                                m_pControlModel->setData(ind, 0);

			ind = m_pControlModel->index(i+m_nControls, 2, QModelIndex());
			m_pControlModel->setData(ind, m_MinControl[i+m_nControls]);

			ind = m_pControlModel->index(i+m_nControls, 3, QModelIndex());
			m_pControlModel->setData(ind, m_MaxControl[i+m_nControls]);
		}
		m_nControls += m_pFin->m_nFlaps;
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


void StabPolarDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strLen, strMass, strInertia;
	GetLengthUnit(strLen, pMainFrame->m_LengthUnit);
	GetWeightUnit(strMass, pMainFrame->m_WeightUnit);
	strInertia = strMass+"."+strLen+QString::fromUtf8("²");
	m_pctrlLab299->setText(strMass);
	m_pctrlLab300->setText(strLen);
	m_pctrlLab301->setText(strLen);
	m_pctrlLab302->setText(strInertia);
	m_pctrlLab303->setText(strInertia);
	m_pctrlLab304->setText(strInertia);
	m_pctrlLab305->setText(strInertia);

	m_pctrlDensity->SetPrecision(5);
	m_pctrlViscosity->SetPrecision(3);

	QString str;
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);

	if(m_UnitType==1) m_pctrlUnit1->setChecked(true);
	else              m_pctrlUnit2->setChecked(true);
	OnUnit();

	if(m_RefAreaType==1) m_pctrlArea1->setChecked(true);
	else                 m_pctrlArea2->setChecked(true);

	m_pctrlUFOName->setText(m_UFOName);
	m_pctrlWPolarName->setText(m_WPolarName);

	if(m_pPlane)
	{
		m_pctrlAnalysisControls->setCurrentIndex(1);
	}
	else m_pctrlAnalysisControls->setCurrentIndex(0);

	m_nControls = 0;
	m_nControls += m_pWing->m_nFlaps;

	if(m_pPlane) 
	{
		m_nControls++; // Wing Tilt
		if(m_pStab)
		{
			m_nControls++;//stab tilt
			m_nControls += m_pStab->m_nFlaps;
		}
		if(m_pFin) m_nControls+=m_pFin->m_nFlaps;
	}

	if(m_pPlane) m_pctrlUFOName->setText(m_pPlane->m_PlaneName);
	else         m_pctrlUFOName->setText(m_pWing->m_WingName);

	m_pctrlAutoName->setChecked(m_bAutoName);

	m_pctrlBeta->SetValue(m_SideSlip);
	m_pctrlPhi->SetValue(m_BankAngle);

	m_pctrlWingMethod2->setChecked(m_bThinSurfaces);
	m_pctrlWingMethod3->setChecked(!m_bThinSurfaces);
	if(m_pPlane) m_pctrlPanelMethod->setChecked(true);

	FillControlList();
	FillUFOInertia();
	SetWPolarName();
	m_pctrlControlTable->setFocus();
}


void StabPolarDlg::keyPressEvent(QKeyEvent *event)
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



void StabPolarDlg::OnArea()
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


void StabPolarDlg::OnAutoName()
{
	m_bAutoName = m_pctrlAutoName->isChecked();
	if(m_bAutoName) SetWPolarName();
}


void StabPolarDlg::OnCellChanged(QWidget *pWidget)
{
	ReadCtrlData();
	SetWPolarName();
}




void StabPolarDlg::OnEditingFinished()
{
	ReadParams();
	SetWPolarName();
}



void StabPolarDlg::OnUnit() 
{
	if(m_pctrlUnit1->isChecked())
	{
		m_UnitType   = 1;
		m_pctrlViscosity->SetValue(m_Viscosity);
		m_pctrlDensityUnit->setText("kg/m3");
		m_pctrlViscosityUnit->setText("m"+QString::fromUtf8("²")+"/s");
	}
	else 
	{
		m_UnitType   = 2;
		m_pctrlViscosity->SetValue(m_Viscosity* 10.7182881);
		m_pctrlDensityUnit->setText("slugs/ft3");
		m_pctrlViscosityUnit->setText("ft"+QString::fromUtf8("²")+"/s");
	}
	SetDensity();
}


void StabPolarDlg::OnOK()
{
	int i;

	ReadCtrlData();
	ReadParams();

	bool bActive = false;

	for(i=0; i<m_nControls; i++)
	{
		bActive = bActive || m_bActiveControl[i];
		if(bActive) break;
	}
/*	if(!bActive)
	{
		strong = tr("No Active Control. Continue ?");
		if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
	}*/

	for(i=m_nControls; i<4*MAXCONTROLS; i++) m_bActiveControl[i] = false;

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
//	m_QInf = m_pctrlQInf->GetValue();

	accept();
}



void StabPolarDlg::OnViscous()
{
	m_bViscous = m_pctrlViscous->isChecked();
	SetWPolarName();
}





void StabPolarDlg::OnWPolarName()
{
	m_pctrlAutoName->setChecked(false);
	m_bAutoName = false;
}


void StabPolarDlg::ReadCtrlData()
{
	int i, k;

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	for(i=0; i<m_nControls; i++)
	{
		k    = m_pControlModel->index(i, 1, QModelIndex()).data().toInt();
		if(k) m_bActiveControl[i] = true;
		else  m_bActiveControl[i] = false;

		m_MinControl[i] = m_pControlModel->index(i, 2, QModelIndex()).data().toDouble();
		m_MaxControl[i] = m_pControlModel->index(i, 3, QModelIndex()).data().toDouble();
	}

	for(i=m_nControls; i<4*MAXCONTROLS; i++) m_bActiveControl[i]=false;
}


void StabPolarDlg::ReadParams()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

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

	m_SideSlip  = m_pctrlBeta->GetValue();
	m_BankAngle = m_pctrlPhi->GetValue();
	SetDensity();

	m_Mass  = m_pctrlMass->GetValue() / pMainFrame->m_kgtoUnit;
	m_CoG.x = m_pctrlCoGx->GetValue() / pMainFrame->m_mtoUnit;
	m_CoG.z = m_pctrlCoGz->GetValue() / pMainFrame->m_mtoUnit;
	m_CoGIxx = m_pctrlIxx->GetValue()  / pMainFrame->m_kgtoUnit / pMainFrame->m_mtoUnit / pMainFrame->m_mtoUnit;
	m_CoGIyy = m_pctrlIyy->GetValue()  / pMainFrame->m_kgtoUnit / pMainFrame->m_mtoUnit / pMainFrame->m_mtoUnit;
	m_CoGIzz = m_pctrlIzz->GetValue()  / pMainFrame->m_kgtoUnit / pMainFrame->m_mtoUnit / pMainFrame->m_mtoUnit;
	m_CoGIxz = m_pctrlIxz->GetValue()  / pMainFrame->m_kgtoUnit / pMainFrame->m_mtoUnit / pMainFrame->m_mtoUnit;
}



void StabPolarDlg::resizeEvent(QResizeEvent *event)
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


void StabPolarDlg::SetDensity()
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



void StabPolarDlg::SetupLayout()
{
	QVBoxLayout *NameLayout = new QVBoxLayout;
	QHBoxLayout *MiscLayout = new QHBoxLayout;
	m_pctrlUFOName = new QLabel(tr("Wing Name"));
	MiscLayout->addWidget(m_pctrlUFOName);
	MiscLayout->addStretch(2);
	m_pctrlAutoName = new QCheckBox(tr("Auto Analysis Name"));
	m_pctrlWPolarName = new QLineEdit(tr("Polar Name"));
	NameLayout->addLayout(MiscLayout);
	QHBoxLayout *AutoNameLayout = new QHBoxLayout;
	AutoNameLayout->addWidget(m_pctrlAutoName);
	AutoNameLayout->addWidget(m_pctrlWPolarName);
	NameLayout->addLayout(AutoNameLayout);
	QGroupBox *NameGroup = new QGroupBox(tr("Polar Name"));
	NameGroup->setLayout(NameLayout);

	QVBoxLayout *PlaneFlightLayout = new QVBoxLayout;
	QGridLayout *PlaneLayout = new QGridLayout;
	QLabel *lab2 = new QLabel(tr("b ="));
	QLabel *lab3 = new QLabel(tr("f ="));
	lab2->setFont(QFont("Symbol"));
	lab3->setFont(QFont("Symbol"));
	lab2->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	lab3->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
	QLabel *lab4 = new QLabel(QString::fromUtf8("°"));
	QLabel *lab5 = new QLabel(QString::fromUtf8("°"));
	m_pctrlBeta  = new FloatEdit(0.818,2);
	m_pctrlPhi   = new FloatEdit(0.414,2);
	
	PlaneLayout->addWidget(lab2,1,1);
	PlaneLayout->addWidget(m_pctrlBeta,1,2);
	PlaneLayout->addWidget(lab4 ,1,3);
	PlaneLayout->addWidget(lab3,2,1);
	PlaneLayout->addWidget(m_pctrlPhi,2,2);
	PlaneLayout->addWidget(lab5,2,3);
	
	m_pctrlViscous = new QCheckBox(tr("Viscous Analysis"));
//	PlaneLayout->addWidget(m_pctrlViscous,5,1,1,3);
	PlaneFlightLayout->addLayout(PlaneLayout);
	PlaneFlightLayout->addWidget(m_pctrlViscous);
	QLabel *lab11 = new QLabel(tr("Note : the analysis may be of the viscous type\nonly if all the flap controls are inactive"));
	PlaneFlightLayout->addWidget(lab11);
	PlaneFlightLayout->addStretch(1);


	QGroupBox *PlaneGroup = new QGroupBox(tr("Plane and Flight Data"));
	PlaneGroup->setLayout(PlaneFlightLayout);

	QGridLayout *AeroDataLayout = new QGridLayout;
	QLabel *lab9 = new QLabel(tr("Unit"));
	m_pctrlUnit1 = new QRadioButton(tr("International"));
	m_pctrlUnit2 = new QRadioButton(tr("Imperial"));
	m_pctrlRho           = new QLabel("r =");
	m_pctrlDensity       = new FloatEdit(1.500e-5,3);
	m_pctrlDensityUnit   = new QLabel("kg/m3");
	m_pctrlNu            = new QLabel("n =");
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


	QVBoxLayout *AreaOptions = new QVBoxLayout;
	m_pctrlArea1 = new QRadioButton(tr("Wing Planform Area"));
	m_pctrlArea2 = new QRadioButton(tr("Wing Planform Area projected on xy plane"));
	AreaOptions->addWidget(m_pctrlArea1);
	AreaOptions->addWidget(m_pctrlArea2);
	QGroupBox *AreaBox = new QGroupBox(tr("Reference Area for Aero Coefficients"));
	AreaBox->setLayout(AreaOptions);


	QVBoxLayout *LeftSideLayout = new QVBoxLayout;
	LeftSideLayout->addWidget(PlaneGroup);
	LeftSideLayout->addWidget(AeroDataGroup);
	LeftSideLayout->addWidget(AreaBox);


	QGroupBox *InertiaBox = new QGroupBox("Inertia");
	{
		QVBoxLayout *InertiaLayout = new QVBoxLayout;
		QLabel *InertiaNote = new QLabel;
		InertiaNote->setText(tr("Note : The following estimation is based on the\nmass properties defined for the wing or plane.\nModify the estimation as required."));
	//	m_pctrlEstimation = new QPushButton(tr("Estimate Inertia"));

		QGridLayout *InertiaData = new QGridLayout;
		QLabel *Lab099 = new QLabel("Mass=");
		QLabel *Lab100 = new QLabel("CoG.x=");
		QLabel *Lab101 = new QLabel("CoG.z=");
		QLabel *Lab102 = new QLabel("Ixx=");
		QLabel *Lab103 = new QLabel("Iyy=");
		QLabel *Lab104 = new QLabel("Izz=");
		QLabel *Lab105 = new QLabel("Ixz=");
		Lab099->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab100->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab101->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab102->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab103->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab104->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		Lab105->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		m_pctrlLab299 = new QLabel;
		m_pctrlLab300 = new QLabel;
		m_pctrlLab301 = new QLabel;
		m_pctrlLab302 = new QLabel;
		m_pctrlLab303 = new QLabel;
		m_pctrlLab304 = new QLabel;
		m_pctrlLab305 = new QLabel;

		m_pctrlMass = new FloatEdit(0.0,3);
		m_pctrlCoGx = new FloatEdit(0.0);
		m_pctrlCoGz = new FloatEdit(0.0);
		m_pctrlIxx  = new FloatEdit(0.0);
		m_pctrlIyy  = new FloatEdit(0.0);
		m_pctrlIzz  = new FloatEdit(0.0);
		m_pctrlIxz  = new FloatEdit(0.0);
		InertiaData->addWidget(Lab099,1,1);
		InertiaData->addWidget(Lab100,2,1);
		InertiaData->addWidget(Lab101,3,1);
		InertiaData->addWidget(Lab102,4,1);
		InertiaData->addWidget(Lab103,5,1);
		InertiaData->addWidget(Lab104,6,1);
		InertiaData->addWidget(Lab105,7,1);
		InertiaData->addWidget(m_pctrlMass,1,2);
		InertiaData->addWidget(m_pctrlCoGx,2,2);
		InertiaData->addWidget(m_pctrlCoGz,3,2);
		InertiaData->addWidget(m_pctrlIxx, 4,2);
		InertiaData->addWidget(m_pctrlIyy, 5,2);
		InertiaData->addWidget(m_pctrlIzz, 6,2);
		InertiaData->addWidget(m_pctrlIxz, 7,2);
		InertiaData->addWidget(m_pctrlLab299,1,3);
		InertiaData->addWidget(m_pctrlLab300,2,3);
		InertiaData->addWidget(m_pctrlLab301,3,3);
		InertiaData->addWidget(m_pctrlLab302,4,3);
		InertiaData->addWidget(m_pctrlLab303,5,3);
		InertiaData->addWidget(m_pctrlLab304,6,3);
		InertiaData->addWidget(m_pctrlLab305,7,3);

		InertiaLayout->addWidget(InertiaNote);
	//	InertiaLayout->addWidget(m_pctrlEstimation);
		InertiaLayout->addLayout(InertiaData);
		InertiaLayout->addStretch(1);

		InertiaBox->setLayout(InertiaLayout);
	}

	//Analysis method
	m_pctrlAnalysisControls = new QStackedWidget;
	{
		QVBoxLayout *WingMethodLayout = new QVBoxLayout;
		m_pctrlWingMethod2 = new QRadioButton(tr("VLM"));
		m_pctrlWingMethod3 = new QRadioButton(tr("3D Panels"));
		WingMethodLayout->addWidget(m_pctrlWingMethod2);
		WingMethodLayout->addWidget(m_pctrlWingMethod3);
		QGroupBox *WingMethodBox = new QGroupBox("Wing analysis methods");
		WingMethodBox->setLayout(WingMethodLayout);

		m_pctrlPanelMethod = new QRadioButton(tr("Mix 3D Panels/VLM"));
		QHBoxLayout *PlaneMethodLayout = new QHBoxLayout;
		PlaneMethodLayout->addWidget(m_pctrlPanelMethod);
		QGroupBox *PlaneMethodBox = new QGroupBox("Plane analysis methods");
		PlaneMethodBox->setLayout(PlaneMethodLayout);

		m_pctrlAnalysisControls->addWidget(WingMethodBox);
		m_pctrlAnalysisControls->addWidget(PlaneMethodBox);
	}

	QVBoxLayout *RightSideLayout = new QVBoxLayout;
	RightSideLayout->addWidget(InertiaBox);
	RightSideLayout->addWidget(m_pctrlAnalysisControls);


	QHBoxLayout *DataLayout = new QHBoxLayout;
	DataLayout->addLayout(LeftSideLayout);
	DataLayout->addLayout(RightSideLayout);

	m_pctrlControlTable = new QTableView(this);
	m_pctrlControlTable->setMinimumWidth(400);
	m_pctrlControlTable->setMinimumHeight(150);
	m_pctrlControlTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlControlTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlControlTable->horizontalHeader()->setStretchLastSection(true);
	
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
	MainLayout->addWidget(NameGroup);
	MainLayout->addLayout(DataLayout);
	MainLayout->addWidget(m_pctrlControlTable);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
}




void StabPolarDlg::SetWPolarName()
{
	if(!m_bAutoName) return;
	QString str, strong;
	int i, nCtrl;
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	m_WPolarName = "T7";

	if(m_pPlane || !m_bThinSurfaces) m_WPolarName += "-Panel";
	if(m_bThinSurfaces)
	{
		if(pMiarex->m_bVLM1) m_WPolarName += "-VLM1";
		else		         m_WPolarName += "-VLM2";
	}

	nCtrl = 0;

	if(m_pPlane)
	{
		if(m_bActiveControl[0] && m_pPlane)
		{
			strong = QString(QString::fromUtf8("-Wing(%2°/%3°)"))
							   .arg(m_MinControl[0],0,'f',1)
							   .arg(m_MaxControl[0],0,'f',1);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	if(m_pPlane && m_pStab)
	{
		if(m_bActiveControl[1])
		{
			strong = QString(QString::fromUtf8("-Elev(%2°/%3°)")).arg(m_MinControl[1],0,'f',1).arg(m_MaxControl[1],0,'f',1);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	for(i=0; i<m_pWing->m_nFlaps; i++)
	{
		if(m_bActiveControl[i+nCtrl])
		{
			strong = QString(QString::fromUtf8("-WF%1(%2°/%3°)"))
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
				strong = QString(QString::fromUtf8("-EF%1(%2°/%3°)"))
						 .arg(i+1).arg(m_MinControl[i+nCtrl]).arg(m_MaxControl[i+nCtrl]);
				m_WPolarName += strong;
			}
		}
	}
	
	if(m_pPlane && m_pFin)
	{
		for(i=0; i<m_pFin->m_nFlaps; i++)
		{
			if(m_bActiveControl[i+nCtrl])
			{
				strong = QString(QString::fromUtf8("-FF%1(%2°/%3°)"))
						 .arg(i+1).arg(m_MinControl[i+nCtrl]).arg(m_MaxControl[i+nCtrl]);
				m_WPolarName += strong;
			}
		}
	}

	if(fabs(m_SideSlip) > .001)
	{
		strong = QString(QString::fromUtf8("-b%1°")).arg(m_SideSlip,0,'f',1);
		m_WPolarName += strong;
	}
	
	if(fabs(m_BankAngle) > .001)
	{
		strong = QString(QString::fromUtf8("-B%1°")).arg(m_BankAngle,0,'f',1);
		m_WPolarName += strong;
	}

	if(!m_bViscous)
	{
		m_WPolarName += "-Inviscid";
	}
	if(m_RefAreaType==2) m_WPolarName += "-proj_area";
	
	
	m_pctrlWPolarName->setText(m_WPolarName);
}



void StabPolarDlg::OnMethod()
{
	if (m_pctrlWingMethod2->isChecked())
	{
		m_bThinSurfaces = true;
	}
	else if (m_pctrlWingMethod3->isChecked())
	{
		m_bThinSurfaces = false;
	}

//	EnableControls();
	SetWPolarName();
}





