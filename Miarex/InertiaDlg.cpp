/****************************************************************************

	InertiaDlg Class
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
#include "InertiaDlg.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>



InertiaDlg::InertiaDlg()
{
	setWindowTitle(tr("Inertia Properties"));
	memset(m_MassValue, 0, sizeof(m_MassValue));

	m_pWing = NULL;
	m_pBody = NULL;

	m_Ixx  = m_Iyy  = m_Izz  = m_Ixz = 0.0;
	m_XRef = m_YRef = m_ZRef = 0.0;
	m_NMass = 3;

/*	m_MassValue[0] = 0.1874;
	m_MassValue[1] = 1.3722;
	m_MassValue[2] = 0.1111;
	m_MassPosition[0].Set(0.0, 1.0, -1.736);
	m_MassPosition[1].Set(-0.455454, 5.0, -1.736);
	m_MassPosition[2].Set(-0.1, -3.8, 4.12);*/
	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));
	m_iSelection = 0;

	SetupLayout();
}


void InertiaDlg::ComputeBodyInertia(double &Ixx, double &Iyy, double &Izz, double &Ixz)
{
	// Assume that the mass is distributed homogeneously in the body's skin
	// Homogeneity is questionable, but is a rather handy assumption
	// Mass in the body's skin is reasonable, given that the point masses
	// are added manually

	//evaluate roughly the Body's wetted area
	int i,j,k;
	double ux, rho;
	double dj, dj1;
	CVector Pt, LA, LB, TA,TB, LATB, TALB, N, PLA, PTA, PLB, PTB, Top, Bot;
	double BodyArea = 0.0;
	double SectionArea;
	double xpos, dl;

	if(m_pBody->m_LineType==1)
	{
		// we use the panel division
		//first get the wetted area

		for (i=0; i<m_pBody->m_NStations-1; i++)
		{
			for (k=0; k<m_pBody->m_NSideLines-1; k++)
			{
				//build the four corner points of the strips
				PLA.x =  m_pBody->m_FramePosition[i].x     ;
				PLA.y =  m_pBody->m_Frame[i].m_Point[k].y  ;
				PLA.z =  m_pBody->m_Frame[i].m_Point[k].z  ;

				PLB.x = m_pBody->m_FramePosition[i].x      ;
				PLB.y = m_pBody->m_Frame[i].m_Point[k+1].y ;
				PLB.z = m_pBody->m_Frame[i].m_Point[k+1].z ;

				PTA.x = m_pBody->m_FramePosition[i+1].x    ;
				PTA.y = m_pBody->m_Frame[i+1].m_Point[k].y ;
				PTA.z = m_pBody->m_Frame[i+1].m_Point[k].z ;

				PTB.x = m_pBody->m_FramePosition[i+1].x     ;
				PTB.y = m_pBody->m_Frame[i+1].m_Point[k+1].y;
				PTB.z = m_pBody->m_Frame[i+1].m_Point[k+1].z;

				LATB = PTB - PLA;
				TALB = PLB - PTA;
				N = TALB * LATB;//panel area x2
				BodyArea += N.VAbs() /2.0;
			}
		}

		BodyArea *= 2.0;
		rho = m_pBody->m_Mass/BodyArea;

		//Get section area
		for (i=0; i<m_pBody->m_NStations-1; i++)
		{
			for (j=0; j<m_pBody->m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_pBody->m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_pBody->m_xPanels[i]);
				SectionArea = 0.0;

				PLA.x = PLB.x = (1.0- dj) * m_pBody->m_FramePosition[i].x   +  dj * m_pBody->m_FramePosition[i+1].x;
				PTA.x = PTB.x = (1.0-dj1) * m_pBody->m_FramePosition[i].x   + dj1 * m_pBody->m_FramePosition[i+1].x;

				for (k=0; k<m_pBody->m_NSideLines-1; k++)
				{
					//build the four corner points of the strips
					PLB.y = (1.0- dj) * m_pBody->m_Frame[i].m_Point[k].y   +  dj * m_pBody->m_Frame[i+1].m_Point[k].y;
					PLB.z = (1.0- dj) * m_pBody->m_Frame[i].m_Point[k].z   +  dj * m_pBody->m_Frame[i+1].m_Point[k].z;

					PTB.y = (1.0-dj1) * m_pBody->m_Frame[i].m_Point[k].y   + dj1 * m_pBody->m_Frame[i+1].m_Point[k].y;
					PTB.z = (1.0-dj1) * m_pBody->m_Frame[i].m_Point[k].z   + dj1 * m_pBody->m_Frame[i+1].m_Point[k].z;

					PLA.y = (1.0- dj) * m_pBody->m_Frame[i].m_Point[k+1].y +  dj * m_pBody->m_Frame[i+1].m_Point[k+1].y;
					PLA.z = (1.0- dj) * m_pBody->m_Frame[i].m_Point[k+1].z +  dj * m_pBody->m_Frame[i+1].m_Point[k+1].z;

					PTA.y = (1.0-dj1) * m_pBody->m_Frame[i].m_Point[k+1].y + dj1 * m_pBody->m_Frame[i+1].m_Point[k+1].y;
					PTA.z = (1.0-dj1) * m_pBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pBody->m_Frame[i+1].m_Point[k+1].z;

					LATB = PTB - PLA;
					TALB = PLB - PTA;
					N = TALB * LATB;//panel area x2
					SectionArea += N.VAbs() /2.0;
				}
				SectionArea *= 2.0;// to account for right side;

				// get center point for this section
				Pt.x = (PLA.x + PTA.x)/2.0;
				Pt.y = 0.0;
				Pt.z = ((1.0-dj)  * m_pBody->m_FramePosition[i].z + dj  * m_pBody->m_FramePosition[i+1].z
					   +(1.0-dj1) * m_pBody->m_FramePosition[i].z + dj1 * m_pBody->m_FramePosition[i+1].z)/2.0;
				//Add inertia contribution
				m_Ixx += SectionArea*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) +(Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
				m_Iyy += SectionArea*rho * ( (Pt.x-m_XRef)*(Pt.x-m_XRef) +(Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
				m_Izz += SectionArea*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) +(Pt.x-m_XRef)*(Pt.x-m_XRef) );
				m_Ixz -= SectionArea*rho * ( (Pt.x-m_XRef)*(Pt.z-m_ZRef) );
			}
		}
	}
	else if(m_pBody->m_LineType==2)
	{
		int NSections = 20;//why not ?
		xpos = m_pBody->m_FramePosition[0].x;

		dl = m_pBody->GetLength()/(double)(NSections-1);

		for (j=0; j<NSections-1; j++)
		{
			BodyArea += dl * (m_pBody->GetSectionArcLength(xpos)+ m_pBody->GetSectionArcLength(xpos+dl)) /2.0;
			xpos += dl;
		}

		rho = m_pBody->m_Mass / BodyArea;

		// Next evaluate inertia, assuming each section is a point mass
		xpos = m_pBody->m_FramePosition[0].x;
		for (j=0; j<NSections-1; j++)
		{
			SectionArea = dl * (m_pBody->GetSectionArcLength(xpos)+ m_pBody->GetSectionArcLength(xpos+dl))/2.0;
			Pt.x = xpos + dl/2.0;
			Pt.y = 0.0;
			ux = m_pBody->Getu(Pt.x);
			m_pBody->GetPoint(ux, 0.0, true, Top);
			m_pBody->GetPoint(ux, 1.0, true, Bot);
			Pt.z = (Top.z + Bot.z)/2.0;
			m_Ixx += SectionArea*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) +(Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
			m_Iyy += SectionArea*rho * ( (Pt.x-m_XRef)*(Pt.x-m_XRef) +(Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
			m_Izz += SectionArea*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) +(Pt.x-m_XRef)*(Pt.x-m_XRef) );
			m_Ixz -= SectionArea*rho * ( (Pt.x-m_XRef)*(Pt.z-m_ZRef) );
			xpos += dl;
		}
	}
}

void InertiaDlg::ComputeInertia()
{
	//assumes that the data has been read
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	double rho, tau, LocalArea, LocalSpan, LocalVolume;
	m_Ixx = m_Iyy = m_Izz = m_Ixz = 0.0;
	CVector Pt;
	int i,j,k;

	//First evaluate the wing's inertia

	if(m_pWing)
	{
		//we are evaluating a wing's inertia
		//the mass density is assumed to be homogeneous
		rho = m_pWing->m_Mass/m_pWing->m_Volume;

		//the local weight is proportional to the chord x foil area
		//the foil's area is interpolated

		for (j=0; j<m_pWing->m_NSurfaces; j++)
		{
			//to make things simple, use Y-Panel mesh spacing for evaluation
			for (k=0; k<m_pWing->m_Surface[j].m_NYPanels; k++)
			{
				m_pWing->m_Surface[j].GetC4(k,Pt, tau);
				LocalArea = m_pWing->m_Surface[j].GetFoilArea(tau);
				LocalSpan = m_pWing->m_Surface[j].GetPanelWidth(k);
				LocalVolume = LocalArea * LocalSpan;
				m_Ixx += LocalVolume*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) + (Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
				m_Iyy += LocalVolume*rho * ( (Pt.x-m_XRef)*(Pt.x-m_XRef) + (Pt.z-m_ZRef)*(Pt.z-m_ZRef) );
				m_Izz += LocalVolume*rho * ( (Pt.y-m_YRef)*(Pt.y-m_YRef) + (Pt.x-m_XRef)*(Pt.x-m_XRef) );
				m_Ixz -= LocalVolume*rho * ( (Pt.x-m_XRef)*(Pt.z-m_ZRef) );
			}
		}
	}
	else if(m_pBody) ComputeBodyInertia(m_Ixx, m_Iyy, m_Izz, m_Ixz);


	//next add the contribution of local weights
	for(i=0; i<MAXMASSES; i++)
	{
		m_Ixx += m_MassValue[i] * ((m_MassPosition[i].y*m_MassPosition[i].y) + (m_MassPosition[i].z*m_MassPosition[i].z));
		m_Iyy += m_MassValue[i] * ((m_MassPosition[i].x*m_MassPosition[i].x) + (m_MassPosition[i].z*m_MassPosition[i].z));
		m_Izz += m_MassValue[i] * ((m_MassPosition[i].x*m_MassPosition[i].x) + (m_MassPosition[i].y*m_MassPosition[i].y));
		m_Ixz -= m_MassValue[i] * ( m_MassPosition[i].x*m_MassPosition[i].z);
	}

	//and display the results
	double Unit = pMainFrame->m_mtoUnit * pMainFrame->m_mtoUnit * pMainFrame->m_kgtoUnit;
	m_pctrlIxx->SetValue(m_Ixx*Unit);
	m_pctrlIyy->SetValue(m_Iyy*Unit);
	m_pctrlIzz->SetValue(m_Izz*Unit);
	m_pctrlIxz->SetValue(m_Ixz*Unit);
}


void InertiaDlg::FillMassModel()
{
	QModelIndex index;
	m_pMassModel->setRowCount(MAXMASSES);
	for(int i=0; i<m_NMass; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		m_pMassModel->setData(index, m_MassValue[i]);

		index = m_pMassModel->index(i, 1, QModelIndex());
		m_pMassModel->setData(index, m_MassPosition[i].x);

		index = m_pMassModel->index(i, 2, QModelIndex());
		m_pMassModel->setData(index, m_MassPosition[i].y);

		index = m_pMassModel->index(i, 3, QModelIndex());
		m_pMassModel->setData(index, m_MassPosition[i].z);
	}
}


void InertiaDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, str1, strong;

	m_pMassModel = new QStandardItemModel;
	m_pMassModel->setRowCount(10);//temporary
	m_pMassModel->setColumnCount(4);

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pMassModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Mass") +" ("+str+")");
	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pMassModel->setHeaderData(1, Qt::Horizontal, QObject::tr("x") +" ("+str+")");
	m_pMassModel->setHeaderData(2, Qt::Horizontal, QObject::tr("y")+" ("+str+")");
	m_pMassModel->setHeaderData(3, Qt::Horizontal, QObject::tr("z")+" ("+str+")");

	m_pctrlMassView->setModel(m_pMassModel);
	m_pctrlMassView->setColumnWidth(0,90);
	m_pctrlMassView->setColumnWidth(1,70);
	m_pctrlMassView->setColumnWidth(2,70);
	m_pctrlMassView->setColumnWidth(3,70);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlMassView->setItemDelegate(m_pFloatDelegate);
	int  *precision = new int[3];
	precision[0] = 3;
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 3;
	m_pFloatDelegate->SetPrecision(precision);

	connect(m_pFloatDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));


	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pctrlMassUnit->setText(str);
	GetLengthUnit(str1, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit->setText(str1);
	strong = str+"."+str1+"2";
	m_pctrlInertiaUnit1->setText(strong);
	m_pctrlInertiaUnit2->setText(strong);
	m_pctrlInertiaUnit3->setText(strong);
	m_pctrlInertiaUnit4->setText(strong);

	if(m_pWing)
	{
		m_pctrlObjectMass->SetValue(m_pWing->m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlObjectMassLabel->setText("Wing Mass:");
	}
	else if (m_pBody)
	{
		m_pctrlObjectMass->SetValue(m_pBody->m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlObjectMassLabel->setText("Body Mass:");
	}

	m_pctrlAuto->setChecked(true);
	OnInputType();
	FillMassModel();
	ComputeInertia();
}


void InertiaDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                        accept();

			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}
}


void InertiaDlg::OnInputType()
{
	bool bAuto = m_pctrlAuto->isChecked();
	m_pctrlIxx->setEnabled(!bAuto);
	m_pctrlIyy->setEnabled(!bAuto);
	m_pctrlIzz->setEnabled(!bAuto);
	m_pctrlIxz->setEnabled(!bAuto);

	m_pctrlObjectMass->setEnabled(bAuto);
	m_pctrlMassView->setEnabled(bAuto);
}


void InertiaDlg::OnOK()
{
	ReadData();
	accept();
}


void InertiaDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QModelIndex index ;
	bool bOK;
	double val;
	int i;
	for (i=0; i<MAXMASSES; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassValue[i]=val/pMainFrame->m_kgtoUnit; else break;

		index = m_pMassModel->index(i, 1, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].x=val/pMainFrame->m_mtoUnit; else break;

		index = m_pMassModel->index(i, 2, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].y=val/pMainFrame->m_mtoUnit; else break;

		index = m_pMassModel->index(i, 3, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(bOK) m_MassPosition[i].z=val/pMainFrame->m_mtoUnit; else break;
	}
	m_NMass = i;
	if(m_pWing)      m_pWing->m_Mass = m_pctrlObjectMass->GetValue() / pMainFrame->m_kgtoUnit;
	else if(m_pBody) m_pBody->m_Mass = m_pctrlObjectMass->GetValue() / pMainFrame->m_kgtoUnit;
}


void InertiaDlg::SetupLayout()
{
	QString strange = tr("This is a calculation form for the inertia tensor. Refer to the Guidelines for explanations.") +"\r\n";
	strange += tr("The inertia tensor is calculated w.r.t. the origin. The moment for another origin is calculated using the transportation theorem.") +"\r\n";
	strange += tr("The results are used in stability calculations.");
	QTextEdit *Message = new QTextEdit(strange);
	Message->setReadOnly(true);

	QGroupBox *InputType = new QGroupBox(tr("Input Type for Inertias"));
	QHBoxLayout *InputLayout = new QHBoxLayout;
	m_pctrlAuto = new QRadioButton(tr("XFLR5 ROM estimate"));
	m_pctrlMan  = new QRadioButton(tr("User Input"));
	InputLayout->addWidget(m_pctrlAuto);
	InputLayout->addWidget(m_pctrlMan);
	InputType->setLayout(InputLayout);

	m_pctrlMan->setEnabled(false);

	QGroupBox *RefBox = new QGroupBox(tr("Reference Point"));
	m_pctrlXRef = new FloatEdit(0.0);
	m_pctrlYRef = new FloatEdit(0.0);
	m_pctrlZRef = new FloatEdit(0.0);
	QLabel *XLab = new QLabel("X=");
	QLabel *YLab = new QLabel("Y=");
	QLabel *ZLab = new QLabel("Z=");
	XLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	YLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	ZLab->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
	m_pctrlLengthUnit = new QLabel("m");
	QGridLayout *RefLayout = new QGridLayout;
	RefLayout->addWidget(XLab,1,1);
	RefLayout->addWidget(YLab,1,2);
	RefLayout->addWidget(ZLab,1,3);
	RefLayout->addWidget(m_pctrlXRef,2,1);
	RefLayout->addWidget(m_pctrlYRef,2,2);
	RefLayout->addWidget(m_pctrlZRef,2,3);
	RefLayout->addWidget(m_pctrlLengthUnit,2,4);
	RefBox->setLayout(RefLayout);

	QGroupBox *MassBox = new QGroupBox(tr("Masses"));
	QLabel *TableLabel = new QLabel(tr("Additional Point Masses:"));
	m_pctrlMassView = new QTableView(this);
//	m_pctrlMassView->setMinimumHeight(500);
	m_pctrlMassView->setMinimumWidth(350);
	m_pctrlMassView->setSelectionBehavior(QAbstractItemView::SelectRows);
	QHBoxLayout *WingMassLayout = new QHBoxLayout;
	m_pctrlObjectMassLabel  = new QLabel(tr("Wing Mass"));
	m_pctrlMassUnit = new QLabel("kg");
	m_pctrlObjectMass = new FloatEdit(1.00);
	WingMassLayout->addWidget(m_pctrlObjectMassLabel);
	WingMassLayout->addWidget(m_pctrlObjectMass);
	WingMassLayout->addWidget(m_pctrlMassUnit);
	WingMassLayout->addStretch(1);
	QVBoxLayout *MassLayout = new QVBoxLayout;
	MassLayout->addLayout(WingMassLayout);
	MassLayout->addWidget(TableLabel);
	MassLayout->addWidget(m_pctrlMassView);
	MassBox->setLayout(MassLayout);

	m_pctrlIxx = new FloatEdit(1.0,3);
	m_pctrlIyy = new FloatEdit(1.2,3);
	m_pctrlIzz = new FloatEdit(-1.5,3);
	m_pctrlIxz = new FloatEdit(4.2,3);

	QLabel *LabIxx = new QLabel("Ixx=");
	QLabel *LabIyy = new QLabel("Iyy=");
	QLabel *LabIzz = new QLabel("Izz=");
	QLabel *LabIxz = new QLabel("Ixz=");
	LabIxx->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIyy->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIzz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabIxz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pctrlInertiaUnit1 = new QLabel("kg.m2");
	m_pctrlInertiaUnit2 = new QLabel("kg.m2");
	m_pctrlInertiaUnit3 = new QLabel("kg.m2");
	m_pctrlInertiaUnit4 = new QLabel("kg.m2");

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QGroupBox *ResultsBox = new QGroupBox(tr("Inertias"));
	QGridLayout *GridLayout = new QGridLayout;
	GridLayout->addWidget(LabIxx,1,1);
	GridLayout->addWidget(LabIyy,2,1);
	GridLayout->addWidget(LabIzz,1,4);
	GridLayout->addWidget(LabIxz,2,4);
	GridLayout->addWidget(m_pctrlIxx,1,2);
	GridLayout->addWidget(m_pctrlIyy,2,2);
	GridLayout->addWidget(m_pctrlIzz,1,5);
	GridLayout->addWidget(m_pctrlIxz,2,5);
	GridLayout->addWidget(m_pctrlInertiaUnit1,1,3);
	GridLayout->addWidget(m_pctrlInertiaUnit2,2,3);
	GridLayout->addWidget(m_pctrlInertiaUnit3,1,6);
	GridLayout->addWidget(m_pctrlInertiaUnit4,2,6);
//	GridLayout->setColumnStretch(2,1);
//	GridLayout->setColumnStretch(5,1);
	GridLayout->setColumnStretch(1,5);
	GridLayout->setColumnStretch(4,5);
	GridLayout->setColumnStretch(3,1);
	GridLayout->setColumnStretch(6,1);
	ResultsBox->setLayout(GridLayout);

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
//	MainLayout->addWidget(FirstLine);
	//MainLayout->addWidget(SecondLine);
	//MainLayout->addWidget(ThirdLine);
	MainLayout->addStretch(1);
	MainLayout->addWidget(Message);
	MainLayout->addWidget(InputType);
	MainLayout->addStretch(1);
//	MainLayout->addWidget(RefBox);
//	MainLayout->addStretch(1);
	MainLayout->addWidget(MassBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(ResultsBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
	connect(m_pctrlAuto, SIGNAL(clicked()), this , SLOT(OnInputType()));
	connect(m_pctrlMan, SIGNAL(clicked()), this , SLOT(OnInputType()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pctrlObjectMass, SIGNAL(editingFinished()), SLOT(OnCellChanged()));
}


void InertiaDlg::OnCellChanged(QWidget *pWidget)
{
	ReadData();
	ComputeInertia();
}
