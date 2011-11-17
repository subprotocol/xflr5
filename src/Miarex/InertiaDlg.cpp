/****************************************************************************

	InertiaDlg Class
	Copyright (C) 2009-2010 Andre Deperrois XFLR5@yahoo.com

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

//performs an automatic evaluation of the object's CoG an inertia properties

#include "../MainFrame.h"
#include "../Globals.h"
#include "InertiaDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QTextStream>
#include <QFileDialog>



void *InertiaDlg::s_pMainFrame;

InertiaDlg::InertiaDlg()
{
	setWindowTitle(tr("Inertia Properties"));

	m_pPlane = NULL;
	m_pWing = NULL;
	m_pBody = NULL;

	m_PtRef.Set(0.0, 0.0, 0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_NMass = 0;

	m_VolumeMass = 0.0;

	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));

	for(int i=0; i< MAXMASSES; i++)
	{
		m_MassTag[i].clear();
	}

	m_bChanged = false;

	m_pInsertMassRow  = new QAction(tr("Insert Before"), this);
	m_pDeleteMassRow = new QAction(tr("Delete"), this);

	m_pContextMenu = new QMenu(tr("Point Mass"),this);
	m_pContextMenu->addAction(m_pInsertMassRow);
	m_pContextMenu->addAction(m_pDeleteMassRow);


	SetupLayout();
}


void InertiaDlg::ComputeBodyAxisInertia()
{
	if(m_pPlane)     m_pPlane->ComputeBodyAxisInertia();
	else if(m_pWing) m_pWing->ComputeBodyAxisInertia();
        else if(m_pBody) m_pBody->ComputeBodyAxisInertia();
}


void InertiaDlg::ComputeInertia()
{
	//
	// Computes the inertia in the frame of reference with origin at the CoG
	//
	// assumes that the data has been read
	//

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i, iw;
	double TotalMass, TotalIxx, TotalIyy, TotalIzz, TotalIxz;
	double Unit = pMainFrame->m_mtoUnit * pMainFrame->m_mtoUnit * pMainFrame->m_kgtoUnit;
	CVector TotalCoG, MassPos;

	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeCoG.Set(0.0, 0.0, 0.0);
	m_PtRef.Set(0.0, 0.0, 0.0);

	CWing *pWing[MAXWINGS];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;

	if(m_pPlane)
	{
		pWing[0] =  m_pPlane->Wing();
		if(m_pPlane->BiPlane()) pWing[1] = m_pPlane->Wing2();
		if(m_pPlane->Stab())    pWing[2] = m_pPlane->Stab();
		if(m_pPlane->Fin())     pWing[3] = m_pPlane->Fin();
	}
	else if(m_pWing)
	{
		pWing[0] = m_pWing;
	}

	// First evaluate the object's volume inertia, i.e. without point masses,
	// in the frame of reference with origin at the object's self CoG
	if(m_pWing)
	{
		m_pWing->m_VolumeMass = m_VolumeMass;
		if(m_pWing->m_VolumeMass>PRECISION) m_pWing->ComputeVolumeInertia(m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}
	else if(m_pBody)
	{
		m_pBody->m_VolumeMass = m_VolumeMass;
		if(m_pBody->m_VolumeMass>PRECISION) m_pBody->ComputeVolumeInertia(m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}
	else if(m_pPlane)
	{
		m_pPlane->ComputeVolumeInertia(m_VolumeMass, m_VolumeCoG, m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz);
	}

	// and display the results
	m_pctrlXCoG->SetValue(m_VolumeCoG.x*pMainFrame->m_mtoUnit);
	m_pctrlYCoG->SetValue(m_VolumeCoG.y*pMainFrame->m_mtoUnit);
	m_pctrlZCoG->SetValue(m_VolumeCoG.z*pMainFrame->m_mtoUnit);

	m_pctrlCoGIxx->SetValue(m_CoGIxx*Unit);
	m_pctrlCoGIyy->SetValue(m_CoGIyy*Unit);
	m_pctrlCoGIzz->SetValue(m_CoGIzz*Unit);
	m_pctrlCoGIxz->SetValue(m_CoGIxz*Unit);

	// take into account all point masses to calculate the total CoG and total mass
	TotalCoG.Set(m_VolumeMass*m_VolumeCoG.x, m_VolumeMass*m_VolumeCoG.y, m_VolumeMass*m_VolumeCoG.z);
	TotalMass = m_VolumeMass;
	TotalIxx = TotalIyy = TotalIzz = TotalIxz = 0.0;

	for(i=0; i<m_NMass; i++)
	{
		TotalMass += m_MassValue[i];
		TotalCoG  += m_MassPosition[i] * m_MassValue[i];
	}

	if(m_pPlane)
	{
		for(iw=0; iw<MAXWINGS; iw++)
		{
			if(pWing[iw])
			{
				for(i=0; i<pWing[iw]->m_NMass; i++)
				{
					TotalMass +=  pWing[iw]->m_MassValue[i];
					TotalCoG  += (pWing[iw]->m_MassPosition[i]+m_pPlane->WingLE(iw)) * pWing[iw]->m_MassValue[i];
				}
			}
		}
	}

	if(m_pPlane && m_pPlane->Body())
	{
		for(i=0; i<m_pPlane->Body()->m_NMass; i++)
		{
			TotalMass +=  m_pPlane->Body()->m_MassValue[i];
			TotalCoG  += (m_pPlane->Body()->m_MassPosition[i]+m_pPlane->BodyPos()) * m_pPlane->Body()->m_MassValue[i];
		}
	}

	if(TotalMass>PRECISION) TotalCoG *= 1.0/TotalMass;
	else                    TotalCoG.Set(0.0,0.0,0.0);

	//Total inertia in CoG referential
	//Apply Huyghens theorem to convert the object's inertia to the new frame
	CVector LA = TotalCoG - m_VolumeCoG;
	TotalIxx = m_CoGIxx + m_VolumeMass * (LA.y*LA.y+ LA.z*LA.z);
	TotalIyy = m_CoGIyy + m_VolumeMass * (LA.x*LA.x+ LA.z*LA.z);
	TotalIzz = m_CoGIzz + m_VolumeMass * (LA.x*LA.x+ LA.y*LA.y);
	TotalIxz = m_CoGIxz - m_VolumeMass *  LA.x*LA.z;

	//add the inertia contribution of all point masses in the Total CoG frame of reference
	for(i=0; i<m_NMass; i++)
	{
		if(m_MassValue[i]>PRECISION)
		{
			MassPos = TotalCoG - m_MassPosition[i];
			TotalIxx  += m_MassValue[i] * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
			TotalIyy  += m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
			TotalIzz  += m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
			TotalIxz  -= m_MassValue[i] * (MassPos.x*MassPos.z);
		}
	}


	if(m_pPlane || m_pWing)
	{
		for(iw=0; iw<MAXWINGS; iw++)
		{
			if(pWing[iw])
			{
				for(i=0; i<pWing[iw]->m_NMass; i++)
				{
                                    MassPos = TotalCoG - (pWing[iw]->m_MassPosition[i] + (m_pPlane != NULL ? m_pPlane->WingLE(iw) : CVector(0.0, 0.0, 0.0)));
					TotalIxx  += pWing[iw]->m_MassValue[i] * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
					TotalIyy  += pWing[iw]->m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
					TotalIzz  += pWing[iw]->m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
					TotalIxz  -= pWing[iw]->m_MassValue[i] * (MassPos.x*MassPos.z);
				}
			}
		}

		if(m_pPlane && m_pPlane->Body())
		{
			for(i=0; i<m_pPlane->Body()->m_NMass; i++)
			{
				MassPos = TotalCoG - (m_pPlane->BodyPos() + m_pPlane->Body()->m_MassPosition[i]);
				TotalIxx  += m_pPlane->Body()->m_MassValue[i] * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
				TotalIyy  += m_pPlane->Body()->m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
				TotalIzz  += m_pPlane->Body()->m_MassValue[i] * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
				TotalIxz  -= m_pPlane->Body()->m_MassValue[i] * (MassPos.x*MassPos.z);
			}
		}
	}

	//display the results
	m_pctrlTotalMass->SetValue(TotalMass*pMainFrame->m_kgtoUnit);
	
	m_pctrlXTotalCoG->SetValue(TotalCoG.x*pMainFrame->m_mtoUnit);
	m_pctrlYTotalCoG->SetValue(TotalCoG.y*pMainFrame->m_mtoUnit);
	m_pctrlZTotalCoG->SetValue(TotalCoG.z*pMainFrame->m_mtoUnit);

	m_pctrlTotalIxx->SetValue(TotalIxx*Unit);
	m_pctrlTotalIyy->SetValue(TotalIyy*Unit);
	m_pctrlTotalIzz->SetValue(TotalIzz*Unit);
	m_pctrlTotalIxz->SetValue(TotalIxz*Unit);
}



void InertiaDlg::contextMenuEvent(QContextMenuEvent *event)
{
	// Display the context menu
	if(m_pctrlMassView->geometry().contains(event->pos())) m_pContextMenu->exec(event->globalPos());
}


void InertiaDlg::FillMassModel()
{
	//
	// Fill the table with the object's point masses
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QModelIndex index;
	
	m_pMassModel->setRowCount(MAXMASSES);

	for(int i=0; i<MAXMASSES; i++)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		m_pMassModel->setData(index, m_MassValue[i]*pMainFrame->m_kgtoUnit);

		if(m_MassValue[i]>0.0)
		{
			index = m_pMassModel->index(i, 1, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].x*pMainFrame->m_mtoUnit);

			index = m_pMassModel->index(i, 2, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].y*pMainFrame->m_mtoUnit);

			index = m_pMassModel->index(i, 3, QModelIndex());
			m_pMassModel->setData(index, m_MassPosition[i].z*pMainFrame->m_mtoUnit);

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
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strMass, strLength;

	GetWeightUnit(strMass, pMainFrame->m_WeightUnit);
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);

	m_pctrlMassUnit->setText(strMass);
	m_pctrlMassUnit2->setText(strMass);
	m_pctrlLengthUnit->setText(strLength);
	m_pctrlLengthUnit2->setText(strLength);
	strong = strMass+"."+strLength+QString::fromUtf8("²");
	m_pctrlInertiaUnit1->setText(strong);
	m_pctrlInertiaUnit2->setText(strong);

	m_pMassModel->setHeaderData(0, Qt::Horizontal, tr("Mass") +" ("+strMass+")");
	m_pMassModel->setHeaderData(1, Qt::Horizontal, tr("x") +" ("+strLength+")");
	m_pMassModel->setHeaderData(2, Qt::Horizontal, tr("y")+" ("+strLength+")");
	m_pMassModel->setHeaderData(3, Qt::Horizontal, tr("z")+" ("+strLength+")");
	m_pMassModel->setHeaderData(4, Qt::Horizontal, tr("Description"));


	m_pctrlWingInertia->setEnabled(false);
	m_pctrlWing2Inertia->setEnabled(false);
	m_pctrlStabInertia->setEnabled(false);
	m_pctrlFinInertia->setEnabled(false);
	m_pctrlBodyInertia->setEnabled(false);

	int rc = m_pMassModel->rowCount();
	m_pMassModel->removeRows(0, rc);
	m_pMassModel->setRowCount(MAXMASSES);

	if(m_pWing)
	{
		m_VolumeMass = m_pWing->m_VolumeMass;
		m_NMass = m_pWing->m_NMass;
		for(int i=0; i<m_pWing->m_NMass; i++)
		{
			m_MassValue[i] = m_pWing->m_MassValue[i];
			m_MassPosition[i].Copy(m_pWing->m_MassPosition[i]);
			m_MassTag[i]   = m_pWing->m_MassTag[i];
		}
		m_pctrlVolumeMass->SetValue(m_pWing->m_VolumeMass * pMainFrame->m_kgtoUnit); //we only display half a wing, AVL way
		m_pctrlVolumeMassLabel->setText(tr("Wing Mass:"));
		m_pctrlWingInertia->setEnabled(true);
		setWindowTitle(tr("Inertia properties for ")+m_pWing->m_WingName);
	}
	else if (m_pBody)
	{
		m_VolumeMass = m_pBody->m_VolumeMass;
		m_NMass = m_pBody->m_NMass;
		for(int i=0; i<m_pBody->m_NMass; i++)
		{
			m_MassValue[i] = m_pBody->m_MassValue[i];
			m_MassPosition[i].Copy(m_pBody->m_MassPosition[i]);
			m_MassTag[i]   = m_pBody->m_MassTag[i];
		}
		m_pctrlVolumeMass->SetValue(m_pBody->m_VolumeMass * pMainFrame->m_kgtoUnit);
		m_pctrlVolumeMassLabel->setText(tr("Body Mass:"));
		m_pctrlBodyInertia->setEnabled(true);
		setWindowTitle(tr("Inertia properties for ")+m_pBody->m_BodyName);
	}
	else if (m_pPlane)
	{
		m_VolumeMass = m_pPlane->Wing()->m_VolumeMass;
		if(m_pPlane->BiPlane()) m_VolumeMass += m_pPlane->Wing2()->m_VolumeMass;
		if(m_pPlane->Stab())    m_VolumeMass += m_pPlane->Stab()->m_VolumeMass;
		if(m_pPlane->Fin())     m_VolumeMass += m_pPlane->Fin()->m_VolumeMass;
		if(m_pPlane->Body())    m_VolumeMass += m_pPlane->Body()->m_VolumeMass;

		m_NMass = m_pPlane->m_NMass;

		for(int i=0; i<m_pPlane->m_NMass; i++)
		{
			m_MassValue[i] = m_pPlane->m_MassValue[i];
			m_MassPosition[i].Copy(m_pPlane->m_MassPosition[i]);
			m_MassTag[i]   = m_pPlane->m_MassTag[i];
		}

		m_pctrlVolumeMass->SetValue(m_VolumeMass * pMainFrame->m_kgtoUnit);
		m_pctrlVolumeMassLabel->setText(tr("Volume Mass:"));
		m_pctrlVolumeMass->setEnabled(false);
		m_pctrlWingInertia->setEnabled(true);
		if(m_pPlane->BiPlane()) m_pctrlWing2Inertia->setEnabled(true);
		if(m_pPlane->Stab())    m_pctrlStabInertia->setEnabled(true);
		if(m_pPlane->Fin())     m_pctrlFinInertia->setEnabled(true);
		if(m_pPlane->Body())    m_pctrlBodyInertia->setEnabled(true);
		setWindowTitle(tr("Inertia properties for ")+m_pPlane->PlaneName());
	}
	if(m_pPlane) m_pctrlTopStack->setCurrentIndex(1);
	else         m_pctrlTopStack->setCurrentIndex(0);

	FillMassModel();
	ComputeInertia();
	setFocus();
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
	m_bChanged = true;
}


void InertiaDlg::OnExportToAVL()
{
	//
	// Export the mass and inertia data to AVL format
	//
	if (!m_pWing && !m_pBody && !m_pPlane) return;
	QString filter =".mass";

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, strong;
	double CoGIxx, CoGIyy, CoGIzz, CoGIxz;
	CVector CoG;

	CWing *pWing[MAXWINGS];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;

	if(m_pPlane)
	{
		pWing[0] =  m_pPlane->Wing();
		if(m_pPlane->BiPlane()) pWing[1] = m_pPlane->Wing2();
		if(m_pPlane->Stab())    pWing[2] = m_pPlane->Stab();
		if(m_pPlane->Fin())     pWing[3] = m_pPlane->Fin();
	}
	else
	{
		pWing[0] = m_pWing;
	}


	if(m_pPlane)     FileName = m_pPlane->PlaneName();
	else if(m_pWing) FileName = m_pWing->m_WingName;
	else if(m_pBody) FileName = m_pBody->m_BodyName;
	FileName.replace("/", " ");
	FileName += ".mass";
	FileName = QFileDialog::getSaveFileName(this, tr("Export Mass Properties"),pMainFrame->m_LastDirName + "/"+FileName,
	                                        tr("AVL Mass File (*.mass)"), &filter);
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
	if(m_pPlane)      out << "#   "+m_pPlane->PlaneName()+"\n";
	else if(m_pWing)  out << "#   "+m_pWing->m_WingName+"\n";
	else if(m_pBody)  out << "#   "+m_pBody->m_BodyName+"\n";
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
	out << "#     mass          x          y          z        Ixx        Iyy        Izz        Ixy        Ixz        Iyz \n";

	if(m_pWing)
	{
		// in accordance with AVL input format,
		// we need to convert the inertia in the wing's CoG system
		// by applying Huyghen/Steiner's theorem

		strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10! Inertia of both left and right wings"))
						.arg(m_VolumeMass /Munit, 10, 'g', 3)
						.arg(m_VolumeCoG.x/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.y/Lunit, 10, 'g', 3)  //should be zero
						.arg(m_VolumeCoG.z/Lunit, 10, 'g', 3)
						.arg(m_CoGIxx/Iunit,  10, 'g', 3)
						.arg(m_CoGIyy/Iunit,  10, 'g', 3)
						.arg(m_CoGIzz/Iunit,  10, 'g', 3)
						.arg(0.0,  10, 'g', 3).arg(m_CoGIxz/Iunit,  10, 'g', 3).arg(0.0,  10, 'g', 3);
		out << strong+"\n";
	}
	else if (m_pBody)
	{
		strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 ! Body inertia"))
						.arg(m_VolumeMass /Munit, 10, 'g', 3)
						.arg(m_VolumeCoG.x/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.y/Lunit, 10, 'g', 3)
						.arg(m_VolumeCoG.z/Lunit, 10, 'g', 3)
						.arg(m_CoGIxx/Iunit,  10, 'g', 3)
						.arg(m_CoGIyy/Iunit,  10, 'g', 3)
						.arg(m_CoGIzz/Iunit,  10, 'g', 3)
						.arg(0.0,10, 'g', 3)
						.arg(CoGIxz/Iunit,10, 'g', 3)
						.arg(0.0,10, 'g', 3);
		out << strong+"\n";
	}
	else if (m_pPlane)
	{
		// we write out each object contribution individually
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(pWing[iw])
			{
				pWing[iw]->ComputeVolumeInertia(CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
				strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 ! "))
								.arg(pWing[iw]->m_VolumeMass /Munit, 10, 'g', 3)
								.arg(CoG.x/Lunit, 10, 'g', 3)
								.arg(CoG.y/Lunit, 10, 'g', 3)
								.arg(CoG.z/Lunit, 10, 'g', 3)
								.arg(CoGIxx/Iunit,10, 'g', 3)
								.arg(CoGIyy/Iunit,10, 'g', 3)
								.arg(CoGIzz/Iunit,10, 'g', 3)
								.arg(0.0,10, 'g', 3)
								.arg(CoGIxz/Iunit,10, 'g', 3)
								.arg(0.0,10, 'g', 3);
				strong += pWing[iw]->m_WingName;
				out << strong+"\n";
			}
		}

		if(m_pPlane->Body())
		{
                        m_pPlane->Body()->ComputeVolumeInertia(CoG, CoGIxx, CoGIyy, CoGIzz, CoGIxz);
			strong = QString(tr("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 ! Body's inertia"))
                                 .arg(m_pPlane->Body()->m_VolumeMass /Munit, 10, 'g', 3)
                                 .arg(CoG.x/Lunit, 10, 'g', 3)
                                 .arg(CoG.y/Lunit, 10, 'g', 3)
                                 .arg(CoG.z/Lunit, 10, 'g', 3)
                                 .arg(CoGIxx/Iunit,10, 'g', 3)
                                 .arg(CoGIyy/Iunit,10, 'g', 3)
                                 .arg(CoGIzz/Iunit,10, 'g', 3)
                                 .arg(0.0,10, 'g', 3)
                                 .arg(CoGIxz/Iunit,10, 'g', 3)
                                 .arg(0.0,10, 'g', 3);
                                /*		 .arg(m_pPlane->Body()->m_VolumeMass /Munit, 10, 'g', 3)
                                                 .arg(m_pPlane->Body()->m_VolumeCoG.x/Lunit, 10, 'g', 3)
                                                 .arg(m_pPlane->Body()->m_VolumeCoG.y/Lunit, 10, 'g', 3)
                                                 .arg(m_pPlane->Body()->m_VolumeCoG.z/Lunit, 10, 'g', 3)
						 .arg(m_pPlane->Body()->m_CoGIxx/Iunit,10, 'g', 3)
						 .arg(m_pPlane->Body()->m_CoGIyy/Iunit,10, 'g', 3)
						 .arg(m_pPlane->Body()->m_CoGIzz/Iunit,10, 'g', 3)
						 .arg(0.0,10, 'g', 3)
						 .arg(m_pPlane->Body()->m_CoGIxz/Iunit,10, 'g', 3)
                                                 .arg(0.0,10, 'g', 3); */
			out << strong+"\n";
		}
	}

	for (int i=0; i<MAXMASSES; i++)
	{
		if(m_MassValue[i]>0.0)
		{
			strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
				.arg(m_MassValue[i] / Munit,    10, 'g', 3)
				.arg(m_MassPosition[i].x/Lunit, 10, 'g', 3)
				.arg(m_MassPosition[i].y/Lunit, 10, 'g', 3)
				.arg(m_MassPosition[i].z/Lunit, 10, 'g', 3);
			strong += " ! " + m_MassTag[i];
			out << strong+"\n";
		}
	}

	if(m_pPlane)
	{
		// need to write the point masses for the objects
		//Main Wing
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(pWing[iw])
			{
				for (int i=0; i<pWing[iw]->m_NMass; i++)
				{
					if(m_pPlane->Wing()->m_MassValue[i]>0.0)
					{
						strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
							.arg(pWing[iw]->m_MassValue[i] / Munit,    10, 'g', 3)
							.arg((pWing[iw]->m_MassPosition[i].x+m_pPlane->WingLE(iw).x)/Lunit, 10, 'g', 3)
							.arg((pWing[iw]->m_MassPosition[i].y+m_pPlane->WingLE(iw).y)/Lunit, 10, 'g', 3)
							.arg((pWing[iw]->m_MassPosition[i].z+m_pPlane->WingLE(iw).z)/Lunit, 10, 'g', 3);

						strong += " ! " + pWing[iw]->m_MassTag[i];
						out << strong+"\n";
					}
				}
			}
		}

		if(m_pPlane->Body())
		{
			//fin
                        for (int i=0; i<m_pPlane->Body()->m_NMass; i++)
			{
                                if(m_pPlane->Body()->m_MassValue[i]>0.0)
				{
					strong = QString("%1 %2 %3 %4      0.000      0.000      0.000")
						.arg(m_pPlane->Body()->m_MassValue[i] / Munit,    10, 'g', 3)
						.arg(m_pPlane->Body()->m_MassPosition[i].x/Lunit, 10, 'g', 3)
						.arg(m_pPlane->Body()->m_MassPosition[i].y/Lunit, 10, 'g', 3)
						.arg(m_pPlane->Body()->m_MassPosition[i].z/Lunit, 10, 'g', 3);
					strong += " ! " + m_pPlane->Body()->m_MassTag[i];
					out << strong+"\n";
				}
			}
		}
	}

	XFile.close();
}


void InertiaDlg::OnInsertMassRow()
{
	int i, sel;
	sel = m_pctrlMassView->currentIndex().row();
	if(m_NMass>=MAXMASSES) return;
	for (i=MAXMASSES-1; i>sel; i--)
	{
		m_MassTag[i]      = m_MassTag[i-1];
		m_MassValue[i]    = m_MassValue[i-1];
		m_MassPosition[i] = m_MassPosition[i-1];
	}
	m_MassValue[sel]=0.0;
	m_MassTag[sel].clear();
	m_MassPosition[sel].Set(0.0,0.0,0.0);

	FillMassModel();

	QModelIndex index = m_pMassModel->index(sel, 0, QModelIndex());
	m_pctrlMassView->setCurrentIndex(index);
	m_pctrlMassView->openPersistentEditor(index);
}


void InertiaDlg::OnDeleteMassRow()
{
	int i, sel;
	sel = m_pctrlMassView->currentIndex().row();
//	setFocus();
	for (i=sel; i<MAXMASSES-1; i++)
	{
		m_MassValue[i]    = m_MassValue[i+1];
		m_MassTag[i]      = m_MassTag[i+1];
		m_MassPosition[i] = m_MassPosition[i+1];
	}

	m_MassValue[MAXMASSES-1]=0.0;
	m_MassTag[MAXMASSES-1].clear();
	m_MassPosition[MAXMASSES-1].Set(0.0,0.0,0.0);

	m_NMass--;

	FillMassModel();

	QModelIndex index = m_pMassModel->index(sel, 0, QModelIndex());
	m_pctrlMassView->setCurrentIndex(index);
	m_pctrlMassView->openPersistentEditor(index);
}


void InertiaDlg::CleanEmptyRows()
{
	QModelIndex index;
	bool bOK;
	double val;

	for (int i=m_pMassModel->rowCount()-1; i>=0; i--)
	{
		index = m_pMassModel->index(i, 0, QModelIndex());
		val = index.data().toDouble(&bOK);
		if(!bOK || fabs(val)<PRECISION)
		{
			m_pMassModel->removeRow(i);
		}
	}
}


void InertiaDlg::OnOK()
{
	int i,j;
	ReadData();

	if(m_pWing)
	{
		j=0;
		m_pWing->m_VolumeMass = m_VolumeMass;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>PRECISION)
			{
				m_pWing->m_MassValue[j] = m_MassValue[i];
				m_pWing->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pWing->m_MassTag[j]   = m_MassTag[i];
				j++;
			}			
		}
		m_pWing->m_NMass = j;
	}
	else if(m_pBody)
	{
		j=0;
		m_pBody->m_VolumeMass = m_VolumeMass;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>PRECISION)
			{
				m_pBody->m_MassValue[j] = m_MassValue[i];
				m_pBody->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pBody->m_MassTag[j]   = m_MassTag[i];
				j++;
			}
		}
		m_pBody->m_NMass = j;
	}
	else if(m_pPlane)
	{
		j=0;
		for(i=0; i< MAXMASSES; i++)
		{
			if(m_MassValue[i]>PRECISION)
			{
				m_pPlane->m_MassValue[j] = m_MassValue[i];
				m_pPlane->m_MassPosition[j].Copy(m_MassPosition[i]);
				m_pPlane->m_MassTag[j]   = m_MassTag[i];
				j++;
			}
		}
		m_pPlane->m_NMass = j;
	}

	ComputeBodyAxisInertia();

	accept();
}


void InertiaDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QModelIndex index;
	bool bOK;
	double val;
	int i;
	m_NMass=0;

	for (i=0; i<MAXMASSES; i++)
	{
		m_MassValue[i] = 0.0;
		m_MassTag[i].clear();
		m_MassPosition[i].Set(0.0,0.0,0.0);

		index = m_pMassModel->index(i, 0, QModelIndex());
		val = index.data().toDouble(&bOK);

		if(bOK && fabs(val)>PRECISION)
		{
			m_MassValue[m_NMass]=val/pMainFrame->m_kgtoUnit;

			index = m_pMassModel->index(i, 1, QModelIndex());
			val = index.data().toDouble(&bOK);
			if(bOK) m_MassPosition[m_NMass].x=val/pMainFrame->m_mtoUnit;

			index = m_pMassModel->index(i, 2, QModelIndex());
			val = index.data().toDouble(&bOK);
			if(bOK) m_MassPosition[m_NMass].y=val/pMainFrame->m_mtoUnit;

			index = m_pMassModel->index(i, 3, QModelIndex());
			val = index.data().toDouble(&bOK);
			if(bOK) m_MassPosition[m_NMass].z=val/pMainFrame->m_mtoUnit;

			index = m_pMassModel->index(i, 4, QModelIndex());
			m_MassTag[m_NMass] = index.data().toString();

			m_NMass++;
		}
	}
	m_VolumeMass = m_pctrlVolumeMass->Value() / pMainFrame->m_kgtoUnit;
}



void InertiaDlg::resizeEvent(QResizeEvent *event)
{
/*	double w = (double)m_pctrlMassView->width()*.97;
	int w6  = (int)(w/6.);

	m_pctrlMassView->setColumnWidth(0, w6);
	m_pctrlMassView->setColumnWidth(1, w6);
	m_pctrlMassView->setColumnWidth(2, w6);
	m_pctrlMassView->setColumnWidth(3, w6);
	m_pctrlMassView->setColumnWidth(4, w-4*w6);*/
}


void InertiaDlg::SetupLayout()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strMass, strLength;
	GetWeightUnit(strMass, pMainFrame->m_WeightUnit);
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QVBoxLayout *MessageLayout = new QVBoxLayout;
	QLabel *Label1 = new QLabel(tr("This is a calculation form for a rough order of magnitude for the inertia tensor."));
	QLabel *Label2 = new QLabel(tr("Refer to the Guidelines for explanations."));
	MessageLayout->addWidget(Label1);
	MessageLayout->addWidget(Label2);

	//___________Volume Mass, Center of gravity, and inertias__________
	m_pctrlTopStack = new QStackedWidget;

	QGroupBox *ObjectMassBox = new QGroupBox(tr("Object Mass - Volume only, excluding point masses"));
	{
		QHBoxLayout *ObjectMassLayout = new QHBoxLayout;
		m_pctrlVolumeMassLabel  = new QLabel(tr("Wing Mass="));
		m_pctrlMassUnit   = new QLabel("kg");
		m_pctrlVolumeMass = new FloatEdit(1.00,3);

		QGroupBox *CoGBox = new QGroupBox(tr("Center of gravity"));
		{
			QGridLayout *CoGLayout = new QGridLayout;
			QLabel *CoGLabel = new QLabel(tr("Center of gravity"));
			CoGLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			QLabel *XCoGLab = new QLabel("X_CoG=");
			QLabel *YCoGLab = new QLabel("Y_CoG=");
			QLabel *ZCoGLab = new QLabel("Z_CoG=");
			m_pctrlXCoG = new FloatEdit(0.00,3);
			m_pctrlYCoG = new FloatEdit(0.00,3);
			m_pctrlZCoG = new FloatEdit(0.00,3);
			m_pctrlXCoG->setEnabled(false);
			m_pctrlYCoG->setEnabled(false);
			m_pctrlZCoG->setEnabled(false);
			m_pctrlLengthUnit = new QLabel("m");
			m_pctrlVolumeMassLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			XCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			YCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			ZCoGLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlXCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlYCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlZCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			CoGLayout->addWidget(m_pctrlVolumeMassLabel,1,1);
			CoGLayout->addWidget(m_pctrlVolumeMass,1,2);
			CoGLayout->addWidget(m_pctrlMassUnit,1,3);
			CoGLayout->addWidget(XCoGLab,2,1);
			CoGLayout->addWidget(YCoGLab,3,1);
			CoGLayout->addWidget(ZCoGLab,4,1);
		//	CoGLayout->addWidget(CoGLabel,2,1);
			CoGLayout->addWidget(m_pctrlXCoG,2,2);
			CoGLayout->addWidget(m_pctrlYCoG,3,2);
			CoGLayout->addWidget(m_pctrlZCoG,4,2);
			CoGLayout->addWidget(m_pctrlLengthUnit,2,3);
			CoGLayout->setColumnStretch(1,1);
			CoGLayout->setColumnStretch(2,2);
			CoGLayout->setColumnStretch(3,1);
			CoGBox->setLayout(CoGLayout);
		}

		QGroupBox *ResultsBox = new QGroupBox(tr("Inertia in CoG Frame"));
		{
			QGridLayout *ResultsLayout = new QGridLayout;
			m_pctrlCoGIxx = new FloatEdit(1.0,3);
			m_pctrlCoGIyy = new FloatEdit(1.2,3);
			m_pctrlCoGIzz = new FloatEdit(-1.5,3);
			m_pctrlCoGIxz = new FloatEdit(4.2,3);
			m_pctrlCoGIxx->setEnabled(false);
			m_pctrlCoGIyy->setEnabled(false);
			m_pctrlCoGIzz->setEnabled(false);
			m_pctrlCoGIxz->setEnabled(false);
			QLabel *LabIxx = new QLabel("Ixx=");
			QLabel *LabIyy = new QLabel("Iyy=");
			QLabel *LabIzz = new QLabel("Izz=");
			QLabel *LabIxz = new QLabel("Ixz=");
			LabIxx->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabIyy->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabIzz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabIxz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			QLabel *LabInertiaObject = new QLabel(tr("Inertia in CoG Frame"));
			LabInertiaObject->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlInertiaUnit1 = new QLabel("kg.m2");
			ResultsLayout->addWidget(LabIxx,1,1);
			ResultsLayout->addWidget(LabIyy,2,1);
			ResultsLayout->addWidget(LabIzz,3,1);
			ResultsLayout->addWidget(LabIxz,4,1);
			ResultsLayout->addWidget(m_pctrlCoGIxx,1,2);
			ResultsLayout->addWidget(m_pctrlCoGIyy,2,2);
			ResultsLayout->addWidget(m_pctrlCoGIzz,3,2);
			ResultsLayout->addWidget(m_pctrlCoGIxz,4,2);
		//	ResultsLayout->addWidget(LabInertiaObject,2,1);
			ResultsLayout->addWidget(m_pctrlInertiaUnit1,1,3);
			ResultsLayout->setColumnStretch(1,1);
			ResultsLayout->setColumnStretch(2,2);
			ResultsLayout->setColumnStretch(3,1);
			ResultsBox->setLayout(ResultsLayout);
		}

		ObjectMassLayout->addWidget(CoGBox);
		ObjectMassLayout->addWidget(ResultsBox);
		ObjectMassBox->setLayout(ObjectMassLayout);
	}

	QGroupBox *ObjectSelectionBox = new QGroupBox(tr("Component inertias"));
	{
		QVBoxLayout *ObjectLayout = new QVBoxLayout;
		m_pctrlWingInertia  = new QPushButton(tr("Main Wing"));
		m_pctrlWing2Inertia = new QPushButton(tr("Second Wing"));
		m_pctrlStabInertia  = new QPushButton(tr("Elevator"));
		m_pctrlFinInertia   = new QPushButton(tr("Fin"));
		m_pctrlBodyInertia   = new QPushButton(tr("Body"));
		QHBoxLayout *WingInertiasLayout = new QHBoxLayout;
		WingInertiasLayout->addStretch(1);
		WingInertiasLayout->addWidget(m_pctrlWingInertia);
		WingInertiasLayout->addStretch(2);
		WingInertiasLayout->addWidget(m_pctrlWing2Inertia);
		WingInertiasLayout->addStretch(3);
		QHBoxLayout *TailInertiasLayout = new QHBoxLayout;
		TailInertiasLayout->addStretch(3);
		TailInertiasLayout->addWidget(m_pctrlStabInertia);
		TailInertiasLayout->addStretch(2);
		TailInertiasLayout->addWidget(m_pctrlFinInertia);
		TailInertiasLayout->addStretch(1);
		QHBoxLayout *BodyInertiaLayout = new QHBoxLayout;
		BodyInertiaLayout->addStretch(1);
		BodyInertiaLayout->addWidget(m_pctrlBodyInertia);
		BodyInertiaLayout->addStretch(2);

		ObjectLayout->addLayout(WingInertiasLayout);
		ObjectLayout->addStretch(1);
		ObjectLayout->addLayout(TailInertiasLayout);
		ObjectLayout->addStretch(1);
		ObjectLayout->addLayout(BodyInertiaLayout);
		ObjectLayout->addStretch(1);
		ObjectSelectionBox->setLayout(ObjectLayout);
	}

	m_pctrlTopStack->addWidget(ObjectMassBox);
	m_pctrlTopStack->addWidget(ObjectSelectionBox);

	//___________________Point Masses__________________________
//	QGroupBox *PointMassBox = new QGroupBox(tr("Additional Point Masses"));
	QLabel *PointMasses = new QLabel(tr("Additional Point Masses"));
//	QVBoxLayout *MassLayout = new QVBoxLayout;
	m_pctrlMassView = new QTableView(this);
	m_pctrlMassView->setMinimumHeight(150);
//	m_pctrlMassView->setMinimumWidth(350);
//	m_pctrlMassView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlMassView->horizontalHeader()->setStretchLastSection(true);
//	MassLayout->addWidget(m_pctrlMassView);
//	PointMassBox->setLayout(MassLayout);
	m_pctrlMassView->setEditTriggers(
							   QAbstractItemView::CurrentChanged |
//							   QAbstractItemView::DoubleClicked |
//							   QAbstractItemView::EditKeyPressed |
							   QAbstractItemView::AnyKeyPressed);

//	m_pctrlMassView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	m_pMassModel = new QStandardItemModel;
	m_pMassModel->setRowCount(10);//temporary
	m_pMassModel->setColumnCount(5);

	m_pctrlMassView->setModel(m_pMassModel);
	m_pctrlMassView->setColumnWidth(0,70);
	m_pctrlMassView->setColumnWidth(1,55);
	m_pctrlMassView->setColumnWidth(2,55);
	m_pctrlMassView->setColumnWidth(3,55);
	m_pctrlMassView->setColumnWidth(4,70);

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

	//________________Total Mass, Center of gravity, and inertias__________
	QGroupBox *TotalMassBox = new QGroupBox(tr("Total Mass = Volume + point masses"));
	{
		QHBoxLayout *TotalMassLayout = new QHBoxLayout;
		m_pctrlTotalMassLabel   = new QLabel(tr("Total Mass="));
		m_pctrlMassUnit2        = new QLabel("kg");
		m_pctrlTotalMass        = new FloatEdit(1.00,3);
		m_pctrlTotalMass->setEnabled(false);

		QGroupBox *TotalCoGBox = new QGroupBox(tr("Center of gravity"));
		{
			QGridLayout *TotalCoGLayout = new QGridLayout;
			QLabel *TotalLabel = new QLabel(tr("Center of gravity"));
			TotalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			QLabel *XTotalLab = new QLabel("X_CoG=");
			QLabel *YTotalLab = new QLabel("Y_CoG=");
			QLabel *ZTotalLab = new QLabel("Z_CoG=");
			m_pctrlXTotalCoG = new FloatEdit(0.00,3);
			m_pctrlYTotalCoG = new FloatEdit(0.00,3);
			m_pctrlZTotalCoG = new FloatEdit(0.00,3);
			m_pctrlXTotalCoG->setEnabled(false);
			m_pctrlYTotalCoG->setEnabled(false);
			m_pctrlZTotalCoG->setEnabled(false);
			m_pctrlLengthUnit2 = new QLabel("m");
			m_pctrlTotalMassLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			XTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			YTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			ZTotalLab->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlXTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlYTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			m_pctrlZTotalCoG->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
			TotalCoGLayout->addWidget(m_pctrlTotalMassLabel,1,1);
			TotalCoGLayout->addWidget(m_pctrlTotalMass,1,2);
			TotalCoGLayout->addWidget(m_pctrlMassUnit2,1,3);
			TotalCoGLayout->addWidget(XTotalLab,2,1);
			TotalCoGLayout->addWidget(YTotalLab,3,1);
			TotalCoGLayout->addWidget(ZTotalLab,4,1);
		//	TotalCoGLayout->addWidget(TotalLabel,2,1);
			TotalCoGLayout->addWidget(m_pctrlXTotalCoG,2,2);
			TotalCoGLayout->addWidget(m_pctrlYTotalCoG,3,2);
			TotalCoGLayout->addWidget(m_pctrlZTotalCoG,4,2);
			TotalCoGLayout->addWidget(m_pctrlLengthUnit2,2,3);
			TotalCoGLayout->setColumnStretch(1,1);
			TotalCoGLayout->setColumnStretch(2,2);
			TotalCoGLayout->setColumnStretch(3,1);
			TotalCoGBox->setLayout(TotalCoGLayout);
		}

		QGroupBox *TotalInertiaBox = new QGroupBox(tr("Inertia in CoG Frame"));
		{
			QGridLayout *TotalInertiaLayout = new QGridLayout;
			m_pctrlTotalIxx = new FloatEdit(1.0,3);
			m_pctrlTotalIyy = new FloatEdit(1.2,3);
			m_pctrlTotalIzz = new FloatEdit(-1.5,3);
			m_pctrlTotalIxz = new FloatEdit(4.2,3);
			m_pctrlTotalIxx->setEnabled(false);
			m_pctrlTotalIyy->setEnabled(false);
			m_pctrlTotalIzz->setEnabled(false);
			m_pctrlTotalIxz->setEnabled(false);
			QLabel *LabTotIxx = new QLabel("Ixx=");
			QLabel *LabTotIyy = new QLabel("Iyy=");
			QLabel *LabTotIzz = new QLabel("Izz=");
			QLabel *LabTotIxz = new QLabel("Ixz=");
			LabTotIxx->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabTotIyy->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabTotIzz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			LabTotIxz->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			QLabel *LabInertiaTotal = new QLabel(tr("Inertia in CoG Frame"));
			LabInertiaTotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
			m_pctrlInertiaUnit2 = new QLabel("kg.m2");
			TotalInertiaLayout->addWidget(LabTotIxx,1,1);
			TotalInertiaLayout->addWidget(LabTotIyy,2,1);
			TotalInertiaLayout->addWidget(LabTotIzz,3,1);
			TotalInertiaLayout->addWidget(LabTotIxz,4,1);
			TotalInertiaLayout->addWidget(m_pctrlTotalIxx,1,2);
			TotalInertiaLayout->addWidget(m_pctrlTotalIyy,2,2);
			TotalInertiaLayout->addWidget(m_pctrlTotalIzz,3,2);
			TotalInertiaLayout->addWidget(m_pctrlTotalIxz,4,2);
		//	TotalInertiaLayout->addWidget(LabInertiaObject,2,1);
			TotalInertiaLayout->addWidget(m_pctrlInertiaUnit2,1,3);
			TotalInertiaLayout->setColumnStretch(1,1);
			TotalInertiaLayout->setColumnStretch(2,2);
			TotalInertiaLayout->setColumnStretch(3,1);
			TotalInertiaBox->setLayout(TotalInertiaLayout);
		}

		TotalMassLayout->addWidget(TotalCoGBox);
		TotalMassLayout->addWidget(TotalInertiaBox);
		TotalMassBox->setLayout(TotalMassLayout);
	}
	//__________________Control buttons___________________

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
	MainLayout->addStretch(1);
	MainLayout->addLayout(MessageLayout);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlTopStack);
	MainLayout->addStretch(1);
//	MainLayout->addWidget(PointMassBox);
	MainLayout->addWidget(PointMasses);
	MainLayout->addWidget(m_pctrlMassView);
	MainLayout->addStretch(1);
	MainLayout->addWidget(TotalMassBox);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
	connect(m_pctrlWingInertia,  SIGNAL(clicked()), this, SLOT(OnWingInertia()));
	connect(m_pctrlWing2Inertia, SIGNAL(clicked()), this, SLOT(OnWing2Inertia()));
	connect(m_pctrlStabInertia,  SIGNAL(clicked()), this, SLOT(OnStabInertia()));
	connect(m_pctrlFinInertia,   SIGNAL(clicked()), this, SLOT(OnFinInertia()));
	connect(m_pctrlBodyInertia,  SIGNAL(clicked()), this, SLOT(OnBodyInertia()));
	connect(OKButton,            SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton,        SIGNAL(clicked()), this, SLOT(reject()));
	connect(ExportAVLButton,     SIGNAL(clicked()), this, SLOT(OnExportToAVL()));
	connect(m_pctrlVolumeMass,   SIGNAL(editingFinished()), SLOT(OnCellChanged()));
	connect(m_pInsertMassRow,   SIGNAL(triggered()), SLOT(OnInsertMassRow()));
	connect(m_pDeleteMassRow,   SIGNAL(triggered()), SLOT(OnDeleteMassRow()));
}


void InertiaDlg::showEvent(QShowEvent *event)
{
	resizeEvent(NULL);
}


void InertiaDlg::OnWingInertia()
{
	InertiaDlg dlg;
	if(!m_pPlane->Wing()) return;
	dlg.m_pWing  = m_pPlane->Wing();
	dlg.m_pPlane = NULL;
	dlg.m_pBody  = NULL;
	dlg.InitDialog();
	dlg.move(pos().x()+25, pos().y()+25);
	dlg.exec();
	ComputeInertia();
}


void InertiaDlg::OnWing2Inertia()
{
	if(!m_pPlane->BiPlane()) return;
	InertiaDlg dlg;
	dlg.m_pWing  = m_pPlane->Wing2();
	dlg.m_pPlane = NULL;
	dlg.m_pBody  = NULL;
	dlg.InitDialog();
	dlg.move(pos().x()+25, pos().y()+25);
	dlg.exec();
	ComputeInertia();
}


void InertiaDlg::OnStabInertia()
{
	if(!m_pPlane->Stab()) return;
	InertiaDlg dlg;
	dlg.m_pWing  = m_pPlane->Stab();
	dlg.m_pPlane = NULL;
	dlg.m_pBody  = NULL;
	dlg.InitDialog();
	dlg.move(pos().x()+25, pos().y()+25);
	dlg.exec();
	ComputeInertia();
}


void InertiaDlg::OnFinInertia()
{
	if(!m_pPlane->Fin()) return;
	InertiaDlg dlg;
	dlg.m_pWing  = m_pPlane->Fin();
	dlg.m_pPlane = NULL;
	dlg.m_pBody  = NULL;
	dlg.InitDialog();
	dlg.move(pos().x()+25, pos().y()+25);
	dlg.exec();
	ComputeInertia();
}


void InertiaDlg::OnBodyInertia()
{
	if(!m_pPlane->Body()) return;
	InertiaDlg dlg;
	dlg.m_pBody  = m_pPlane->Body();
	dlg.m_pPlane = NULL;
	dlg.m_pWing  = NULL;
	dlg.InitDialog();
	dlg.move(pos().x()+25, pos().y()+25);
	dlg.exec();
	ComputeInertia();
}
