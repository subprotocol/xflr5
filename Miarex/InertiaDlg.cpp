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
#include <QTextStream>


InertiaDlg::InertiaDlg()
{
	setWindowTitle(tr("Inertia Properties"));

	m_pWing = NULL;
	m_pBody = NULL;

	m_Ixx  = m_Iyy  = m_Izz  = m_Ixz = 0.0;
	m_PtRef.Set(0.0, 0.0, 0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_NMass = 3;

	m_Mass = 1.0;

	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));

	for(int i=0; i< MAXMASSES; i++)
	{
		m_MassTag[i] = QString("Description %1").arg(i);
	}

	SetupLayout();
}


void InertiaDlg::ComputeInertia()
{
	//assumes that the data has been read
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_Ixx    = m_Iyy    = m_Izz    = m_Ixz    = 0.0;
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_CoG.Set(0.0, 0.0, 0.0);
	m_PtRef.Set(0.0, 0.0, 0.0);
	int i;

	//First evaluate the object's inertia

	if(m_pWing)
	{
		m_pWing->ComputeInertia(m_Mass, m_PtRef,m_CoG,
								m_Ixx, m_Iyy, m_Izz, m_Ixz,
								m_CoGIxx,m_CoGIyy,m_CoGIzz, m_CoGIxz);
	}
	else if(m_pBody)
	{
		m_pBody->ComputeBodyInertia(m_Mass, m_PtRef,m_CoG,
								m_Ixx, m_Iyy, m_Izz, m_Ixz,
								m_CoGIxx,m_CoGIyy,m_CoGIzz, m_CoGIxz);
	}
// check Huyghens / Steiner theorem
//	CVector U;
//	U = m_CoG-m_PtRef;
//	qDebug() << m_Ixx << -m_CoGIxx - m_Mass * (U.y*U.y+U.z*U.z);
//	qDebug() << m_Iyy << -m_CoGIyy - m_Mass * (U.x*U.x+U.z*U.z);
//	qDebug() << m_Izz << -m_CoGIzz - m_Mass * (U.x*U.x+U.y*U.y);
//	qDebug() << m_Ixz << -m_CoGIxz - m_Mass * ( - U.x * U.z);

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

	QString strong;
	strong = QString("%1").arg(m_CoG.x*pMainFrame->m_mtoUnit,10,'f',3);
	m_pctrlXCoG->setText(strong);
	strong = QString("%1").arg(m_CoG.y*pMainFrame->m_mtoUnit,10,'f',3);
	m_pctrlYCoG->setText(strong);
	strong = QString("%1").arg(m_CoG.z*pMainFrame->m_mtoUnit,10,'f',3);
	m_pctrlZCoG->setText(strong);

	m_pctrlCoGIxx->SetValue(m_CoGIxx*Unit);
	m_pctrlCoGIyy->SetValue(m_CoGIyy*Unit);
	m_pctrlCoGIzz->SetValue(m_CoGIzz*Unit);
	m_pctrlCoGIxz->SetValue(m_CoGIxz*Unit);

}


void InertiaDlg::FillMassModel()
{
	QModelIndex index;
	m_pMassModel->setRowCount(MAXMASSES);
	for(int i=0; i<m_NMass; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		m_pMassModel->setData(index, m_MassValue[i]);

		if(m_MassValue[i]>0.0)
		{
			index = m_pMassModel->index(i, 1, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].x);

			index = m_pMassModel->index(i, 2, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].y);

			index = m_pMassModel->index(i, 3, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].z);

			index = m_pMassModel->index(i, 4, QModelIndex());
			m_pMassModel->setData(index, m_MassTag[i]);
		}
		else
		{
			index = m_pMassModel->index(i, 1, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 2, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 3, QModelIndex());
			m_pMassModel->setData(index, 0.0);

			index = m_pMassModel->index(i, 4, QModelIndex());
			m_pMassModel->setData(index, "");
		}
	}
}


void InertiaDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, str1, strong;

	m_pMassModel = new QStandardItemModel;
	m_pMassModel->setRowCount(10);//temporary
	m_pMassModel->setColumnCount(5);

	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pMassModel->setHeaderData(0, Qt::Horizontal, tr("Mass") +" ("+str+")");
	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_pMassModel->setHeaderData(1, Qt::Horizontal, tr("x") +" ("+str+")");
	m_pMassModel->setHeaderData(2, Qt::Horizontal, tr("y")+" ("+str+")");
	m_pMassModel->setHeaderData(3, Qt::Horizontal, tr("z")+" ("+str+")");
	m_pMassModel->setHeaderData(4, Qt::Horizontal, tr("Description"));

	m_pctrlMassView->setModel(m_pMassModel);
	m_pctrlMassView->setColumnWidth(0,90);
	m_pctrlMassView->setColumnWidth(1,70);
	m_pctrlMassView->setColumnWidth(2,70);
	m_pctrlMassView->setColumnWidth(3,70);
	m_pctrlMassView->setColumnWidth(4,150);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlMassView->setItemDelegate(m_pFloatDelegate);
	int  *precision = new int[5];
	precision[0] = 3;
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 3;
	precision[4] = -1;// Not a number, will be detected as such by FloatEditDelegate
	m_pFloatDelegate->SetPrecision(precision);

	connect(m_pFloatDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));


	GetWeightUnit(str, pMainFrame->m_WeightUnit);
	m_pctrlMassUnit->setText(str);
	GetLengthUnit(str1, pMainFrame->m_LengthUnit);
	m_pctrlLengthUnit->setText(str1);
	m_pctrlLengthUnit2->setText(str1);
	strong = str+"."+str1+"2";
	m_pctrlInertiaUnit1->setText(strong);
	m_pctrlInertiaUnit2->setText(strong);

	if(m_pWing)
	{
		m_Mass = m_pWing->m_Mass;
		m_pctrlObjectMass->SetValue(m_pWing->m_Mass * pMainFrame->m_kgtoUnit); //we only display half a wing, AVL way
		m_pctrlObjectMassLabel->setText("Wing Mass:");
	}
	else if (m_pBody)
	{
		m_Mass = m_pBody->m_Mass;
		m_pctrlObjectMass->SetValue(m_pBody->m_Mass * pMainFrame->m_kgtoUnit);
		m_pctrlObjectMassLabel->setText("Body Mass:");
	}

	m_pctrlAuto->setChecked(true);
	OnInputType();
}


void InertiaDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus()) OKButton->setFocus();
			else                      accept();

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

void InertiaDlg::OnCellChanged(QWidget *pWidget)
{
	ReadData();
	ComputeInertia();
}


void InertiaDlg::OnExportToAVL()
{
	if (!m_pWing && !m_pBody) return;
	QString filter =".mass";

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, DestFileName, OutString, strong;
	QFile DestFile;

	if(m_pWing) FileName = m_pWing->m_WingName;
	else        FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");
	FileName += ".mass";
	FileName = QFileDialog::getSaveFileName(this, "Export Mass Properties",pMainFrame->m_LastDirName + "/"+FileName,
	                                        "AVL Mass File (*.mass)", &filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	pos = FileName.lastIndexOf(".");
	if(pos<0) FileName += ".mass";

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;
	QTextStream out(&XFile);
	out.setCodec("UTF-8");

	double Lunit = 1./pMainFrame->m_mtoUnit;
	double Munit = 1./pMainFrame->m_kgtoUnit;
	double Iunit = Munit * Lunit * Lunit;

	out << "#-------------------------------------------------\n";
	out << "#\n";
	if(m_pWing)      out << "#   "+m_pWing->m_WingName+"\n";
	else if(m_pBody) out << "#   "+m_pBody->m_BodyName+"\n";
	out << "#\n";
	out << "#  Dimensional unit and parameter data.\n";
	out << "#  Mass & Inertia breakdown.\n";
	out << "#-------------------------------------------------\n";
	out << "#  Names and scalings for units to be used for trim and eigenmode calculations.\n";
	out << "#  The Lunit and Munit values scale the mass, xyz, and inertia table data below.\n";
	out << "#  Lunit value will also scale all lengths and areas in the AVL input file.\n";
	out << "#\n";
	strong = QString("Lunit = %1 m").arg(Lunit,10,'f',4);
	out << strong+"\n";
	strong = QString("Munit = %1 kg").arg(Munit,10,'f',4);
	out << strong+"\n";
	out << "Tunit = 1.0 s\n";
	out << "#-------------------------\n";
	out << "#  Gravity and density to be used as default values in trim setup (saves runtime typing).\n";
	out << "#  Must be in the unit names given above (i.e. m,kg,s).\n";
	out << "g   = 9.81\n";
	out << "rho = 1.225\n";
	out << "#-------------------------\n";
	out << "#  Mass & Inertia breakdown.\n";
	out << "#  x y z  is location of item's own CG.\n";
	out << "#  Ixx... are item's inertias about item's own CG.\n";
	out << "#\n";
	out << "#  x,y,z system here must be exactly the same one used in the .avl input file\n";
	out << "#     (same orientation, same origin location, same length units)\n";
	out << "#\n";
	out << "#   mass       x         y         z        Ixx       Iyy       Izz      [  Ixy   Ixz   Iyz ]\n";

	if(m_pWing)
	{
		// in accordance with AVL input format,
		// we need to convert the inertia in the wing's CoG system
		// by applying Huyghen/Steiner's theorem

		strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Inertia of both left and right wings")
		                .arg(m_Mass /Munit,  8, 'f', 3)
                                .arg(m_CoG.x/Lunit, 8, 'f', 3) 
                                .arg(m_CoG.y/Lunit, 8, 'f', 3)  //should be zero 
                                .arg(m_CoG.z/Lunit, 8, 'f', 3)  
				.arg((m_Ixx + m_Mass * (m_CoG.y*m_CoG.y + m_CoG.z*m_CoG.z))/Iunit,  8, 'f', 3)
				.arg((m_Iyy + m_Mass * (m_CoG.x*m_CoG.x + m_CoG.z*m_CoG.z))/Iunit,  8, 'f', 3)
				.arg((m_Izz + m_Mass * (m_CoG.x*m_CoG.x + m_CoG.y*m_CoG.y))/Iunit,  8, 'f', 3);
		out << strong+"\n";
	}
	else if (m_pBody)
	{
		strong = QString("%1  %2  %3  %4  %5  %6  %7 ! Body inertia")
                                .arg(m_Mass /Munit, 8, 'f', 3)
                                .arg(m_CoG.x/Lunit, 8, 'f', 3)
                                .arg(m_CoG.y/Lunit, 8, 'f', 3)
                                .arg(m_CoG.z/Lunit, 8, 'f', 3)
				.arg((m_Ixx + m_Mass * (m_CoG.y*m_CoG.y + m_CoG.z*m_CoG.z))/Iunit,  8, 'f', 3)
				.arg((m_Iyy + m_Mass * (m_CoG.x*m_CoG.x + m_CoG.z*m_CoG.z))/Iunit,  8, 'f', 3)
				.arg((m_Izz + m_Mass * (m_CoG.x*m_CoG.x + m_CoG.y*m_CoG.y))/Iunit,  8, 'f', 3);
		out << strong+"\n";
	}
	for (int i=0; i<m_NMass; i++)
	{
		strong = QString("%1  %2  %3  %4     0.000     0.000     0.000")
			.arg(m_MassValue[i] / Munit,      8, 'f', 3)
			.arg(m_MassPosition[i].x/Lunit, 8, 'f', 3)
			.arg(m_MassPosition[i].y/Lunit, 8, 'f', 3)
			.arg(m_MassPosition[i].z/Lunit, 8, 'f', 3);
		strong += "! " + m_MassTag[i];
		out << strong+"\n";
	}

	XFile.close();
}



void InertiaDlg::OnInputType()
{
	bool bAuto = m_pctrlAuto->isChecked();
	m_pctrlCoGIxx->setEnabled(!bAuto);
	m_pctrlCoGIyy->setEnabled(!bAuto);
	m_pctrlCoGIzz->setEnabled(!bAuto);
	m_pctrlCoGIxz->setEnabled(!bAuto);

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
	if(m_pWing)      m_pWing->m_Mass = m_Mass;
	else if(m_pBody) m_pBody->m_Mass = m_Mass;
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

		index = m_pMassModel->index(i, 4, QModelIndex());
		m_MassTag[i] = index.data().toString();
	}
	m_NMass = i;

	m_Mass = m_pctrlObjectMass->GetValue() / pMainFrame->m_kgtoUnit;
}



void InertiaDlg::resizeEvent(QResizeEvent *event)
{
	double w = (double)m_pctrlMassView->width()*.97;
	int w6  = (int)(w/6.);

	m_pctrlMassView->setColumnWidth(0, w6);
	m_pctrlMassView->setColumnWidth(1, w6);
	m_pctrlMassView->setColumnWidth(2, w6);
	m_pctrlMassView->setColumnWidth(3, w6);
	m_pctrlMassView->setColumnWidth(4, w-4*w6);
}


void InertiaDlg::SetupLayout()
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QString strange = tr("This is a calculation form for the inertia tensor. Refer to the Guidelines for explanations.") +"\r\n";
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


	QGroupBox *ObjectMassBox = new QGroupBox(tr("Object Mass"));
	QVBoxLayout *ObjectMassLayout = new QVBoxLayout;
	QHBoxLayout *WingMassLayout = new QHBoxLayout;
	m_pctrlObjectMassLabel  = new QLabel(tr("Wing Mass"));
	m_pctrlMassUnit = new QLabel("kg");
	m_pctrlObjectMass = new FloatEdit(1.00,3);
	WingMassLayout->addWidget(m_pctrlObjectMassLabel);
	WingMassLayout->addWidget(m_pctrlObjectMass);
	WingMassLayout->addWidget(m_pctrlMassUnit);
	WingMassLayout->addStretch(1);
	QGridLayout *CoGLayout = new QGridLayout;
	QLabel *CoGLabel = new QLabel(tr("Center of gravity"));
	QLabel *XCoGLab = new QLabel("X=");
	QLabel *YCoGLab = new QLabel("Y=");
	QLabel *ZCoGLab = new QLabel("Z=");
	m_pctrlXCoG = new QLabel("0.00");
	m_pctrlYCoG = new QLabel("0.00");
	m_pctrlZCoG = new QLabel("0.00");
	m_pctrlLengthUnit2 = new QLabel("m");
	XCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	YCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	ZCoGLab->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	m_pctrlXCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlYCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	m_pctrlZCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	CoGLayout->addWidget(XCoGLab,1,2);
	CoGLayout->addWidget(YCoGLab,1,3);
	CoGLayout->addWidget(ZCoGLab,1,4);
	CoGLayout->addWidget(CoGLabel,2,1);
	CoGLayout->addWidget(m_pctrlXCoG,2,2);
	CoGLayout->addWidget(m_pctrlYCoG,2,3);
	CoGLayout->addWidget(m_pctrlZCoG,2,4);
	CoGLayout->addWidget(m_pctrlLengthUnit2,2,5);

	ObjectMassLayout->addLayout(WingMassLayout);
	ObjectMassLayout->addLayout(CoGLayout);
	ObjectMassBox->setLayout(ObjectMassLayout);

	QGroupBox *PointMassBox = new QGroupBox(tr("Additional Point Masses:"));
	QVBoxLayout *MassLayout = new QVBoxLayout;
	m_pctrlMassView = new QTableView(this);
	m_pctrlMassView->setMinimumHeight(200);
	m_pctrlMassView->setMinimumWidth(350);
	m_pctrlMassView->setSelectionBehavior(QAbstractItemView::SelectRows);
	MassLayout->addWidget(m_pctrlMassView);
	PointMassBox->setLayout(MassLayout);

	m_pctrlIxx = new FloatEdit(1.0,2);
	m_pctrlIyy = new FloatEdit(1.2,2);
	m_pctrlIzz = new FloatEdit(-1.5,2);
	m_pctrlIxz = new FloatEdit(4.2,2);
	m_pctrlCoGIxx = new FloatEdit(1.0,2);
	m_pctrlCoGIyy = new FloatEdit(1.2,2);
	m_pctrlCoGIzz = new FloatEdit(-1.5,2);
	m_pctrlCoGIxz = new FloatEdit(4.2,2);

	QLabel *LabIxx = new QLabel("Ixx");
	QLabel *LabIyy = new QLabel("Iyy");
	QLabel *LabIzz = new QLabel("Izz");
	QLabel *LabIxz = new QLabel("Ixz");
	LabIxx->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIyy->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIzz->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	LabIxz->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	QLabel *LabInertiaObject = new QLabel("CoG Frame");
	QLabel *LabInertiaTotal = new QLabel("Reference Frame");
	LabInertiaObject->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	LabInertiaTotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pctrlInertiaUnit1 = new QLabel("kg.m2");
	m_pctrlInertiaUnit2 = new QLabel("kg.m2");


	QGroupBox *ResultsBox = new QGroupBox(tr("Inertias : Object only, without point masses"));
	QGridLayout *GridLayout = new QGridLayout;
	GridLayout->addWidget(LabIxx,1,2);
	GridLayout->addWidget(LabIyy,1,3);
	GridLayout->addWidget(LabIzz,1,4);
	GridLayout->addWidget(LabIxz,1,5);
	GridLayout->addWidget(m_pctrlCoGIxx,2,2);
	GridLayout->addWidget(m_pctrlCoGIyy,2,3);
	GridLayout->addWidget(m_pctrlCoGIzz,2,4);
	GridLayout->addWidget(m_pctrlCoGIxz,2,5);
	GridLayout->addWidget(m_pctrlIxx,3,2);
	GridLayout->addWidget(m_pctrlIyy,3,3);
	GridLayout->addWidget(m_pctrlIzz,3,4);
	GridLayout->addWidget(m_pctrlIxz,3,5);
	GridLayout->addWidget(LabInertiaObject,2,1);
	GridLayout->addWidget(LabInertiaTotal,3,1);
	GridLayout->addWidget(m_pctrlInertiaUnit1,2,6);
	GridLayout->addWidget(m_pctrlInertiaUnit2,3,6);
//	GridLayout->setColumnStretch(2,1);
//	GridLayout->setColumnStretch(5,1);
	GridLayout->setColumnStretch(1,2);
	GridLayout->setColumnStretch(2,1);
	GridLayout->setColumnStretch(3,1);
	GridLayout->setColumnStretch(4,1);
	GridLayout->setColumnStretch(5,1);
	GridLayout->setColumnStretch(6,2);
	ResultsBox->setLayout(GridLayout);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *ExportAVLButton = new QPushButton(tr("Export to AVL"));
	OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ExportAVLButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

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
	MainLayout->addWidget(ObjectMassBox);
	MainLayout->addWidget(ResultsBox);
	MainLayout->addStretch(1);
	MainLayout->addWidget(PointMassBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
	connect(m_pctrlAuto, SIGNAL(clicked()), this , SLOT(OnInputType()));
	connect(m_pctrlMan, SIGNAL(clicked()), this , SLOT(OnInputType()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ExportAVLButton, SIGNAL(clicked()), this, SLOT(OnExportToAVL()));
	connect(m_pctrlObjectMass, SIGNAL(editingFinished()), SLOT(OnCellChanged()));
}


void InertiaDlg::showEvent(QShowEvent *event)
{
	resizeEvent(NULL);
	FillMassModel();
	ComputeInertia();
}
