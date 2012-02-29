/****************************************************************************

	SplineCtrlsDlg
	Copyright (C) 2009-2010 Andre Deperrois adeperrois@xflr5.com

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

#include "SplineCtrlsDlg.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>


SplineCtrlsDlg::SplineCtrlsDlg()
{
	setWindowTitle(tr("Spline Parameters"));
	m_bSymetric = false;
	m_pSF = NULL;
	m_pPF = NULL;
	SetuLayout();
}


void SplineCtrlsDlg::InitDialog()
{
	int i;

	QString str;
	m_pctrlDegExtrados->clear();
	m_pctrlDegIntrados->clear();
	for (i=2; i<6; i++)
	{
		str = QString("%1").arg(i);
		m_pctrlDegExtrados->addItem(str);
		m_pctrlDegIntrados->addItem(str);
	}
	m_pctrlDegExtrados->setEnabled(true);
	m_pctrlDegIntrados->setEnabled(true);

	if(m_bSF)
	{
		m_pctrlDegExtrados->setCurrentIndex(m_pSF->m_Extrados.m_iDegree-2);
		m_pctrlDegIntrados->setCurrentIndex(m_pSF->m_Intrados.m_iDegree-2);
		m_pctrlOutExtrados->SetValue(m_pSF->m_Extrados.m_iRes);
		m_pctrlOutIntrados->SetValue(m_pSF->m_Intrados.m_iRes);
	}
	else
	{
		m_pctrlDegExtrados->setEnabled(false);
		m_pctrlDegIntrados->setEnabled(false);
		m_pctrlOutExtrados->SetMin(3);
		m_pctrlOutExtrados->SetMax(30);
		m_pctrlOutIntrados->SetMin(3);
		m_pctrlOutIntrados->SetMax(30);

		m_pctrlOutExtrados->SetValue(m_pPF->m_Extrados.m_Freq);
		m_pctrlOutIntrados->SetValue(m_pPF->m_Intrados.m_Freq);
//		m_pctrlOutText->SetWindowText("Output density:");
	}

	//upper point list
	m_pUpperListModel = new QStandardItemModel;
	m_pUpperListModel->setRowCount(10);//temporary
	m_pUpperListModel->setColumnCount(3);

	m_pUpperListModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Point"));
	m_pUpperListModel->setHeaderData(1, Qt::Horizontal, QObject::tr("x"));
	m_pUpperListModel->setHeaderData(2, Qt::Horizontal, QObject::tr("y"));

	m_pctrlUpperList->setModel(m_pUpperListModel);

	QHeaderView *HorizontalHeader = m_pctrlUpperList->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pUpperFloatDelegate = new FloatEditDelegate;
	m_pctrlUpperList->setItemDelegate(m_pUpperFloatDelegate);

	//Lower point list
	m_pLowerListModel = new QStandardItemModel;
	m_pLowerListModel->setRowCount(10);//temporary
	m_pLowerListModel->setColumnCount(3);

	m_pLowerListModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Point"));
	m_pLowerListModel->setHeaderData(1, Qt::Horizontal, QObject::tr("x"));
	m_pLowerListModel->setHeaderData(2, Qt::Horizontal, QObject::tr("y"));

	m_pctrlLowerList->setModel(m_pLowerListModel);

	HorizontalHeader = m_pctrlLowerList->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pLowerFloatDelegate = new FloatEditDelegate;
	m_pctrlLowerList->setItemDelegate(m_pLowerFloatDelegate);

	int  *precision = new int[3];
	precision[0] = 0;
	precision[1] = 5;
	precision[2] = 5;
	m_pUpperFloatDelegate->SetPrecision(precision);
	m_pLowerFloatDelegate->SetPrecision(precision);

	connect(m_pUpperFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnUpperCellChanged(QWidget *)));
	connect(m_pLowerFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnLowerCellChanged(QWidget *)));

	if(m_pSF) m_SF.Copy(m_pSF);
	if(m_pPF) m_PF.Copy(m_pPF);

	m_pctrlSymetric->setChecked(m_bSymetric);

	FillPointLists();
}


void SplineCtrlsDlg::showEvent(QShowEvent *event)
{
	int w = m_pctrlUpperList->width();
	m_pctrlUpperList->setColumnWidth(0,(int)(w/3)-20);
	m_pctrlUpperList->setColumnWidth(1,(int)(w/3)-20);
	m_pctrlUpperList->setColumnWidth(2,(int)(w/3)-20);
	w = m_pctrlLowerList->width();
	m_pctrlLowerList->setColumnWidth(0,(int)(w/3)-20);
	m_pctrlLowerList->setColumnWidth(1,(int)(w/3)-20);
	m_pctrlLowerList->setColumnWidth(2,(int)(w/3)-20);
}



void SplineCtrlsDlg::SetuLayout()
{
	QGroupBox *UpperSideBox = new QGroupBox(tr("Upper side"));
	QGroupBox *LowerSideBox = new QGroupBox(tr("Lower side"));

	QGridLayout *UpperLayout = new QGridLayout;
	QLabel *labupper1 = new QLabel(tr("Spline degree"));
	QLabel *labupper2 = new QLabel(tr("Output"));
	m_pctrlDegExtrados = new QComboBox;
	m_pctrlOutExtrados = new FloatEdit;
	m_pctrlOutExtrados->SetPrecision(0);
	UpperLayout->addWidget(labupper1, 1,1);
	UpperLayout->addWidget(labupper2, 2,1);
	UpperLayout->addWidget(m_pctrlDegExtrados, 1,2);
	UpperLayout->addWidget(m_pctrlOutExtrados, 2,2);

	QGridLayout *LowerLayout = new QGridLayout;
	QLabel *lablower1 = new QLabel(tr("Spline degree"));
	QLabel *lablower2 = new QLabel(tr("Output"));
	m_pctrlDegIntrados = new QComboBox;
	m_pctrlOutIntrados = new FloatEdit;
	m_pctrlOutIntrados->SetPrecision(0);
	LowerLayout->addWidget(lablower1, 1,1);
	LowerLayout->addWidget(lablower2, 2,1);
	LowerLayout->addWidget(m_pctrlDegIntrados, 1,2);
	LowerLayout->addWidget(m_pctrlOutIntrados, 2,2);


	m_pctrlUpperList = new QTableView;
	m_pctrlLowerList = new QTableView;
	m_pctrlUpperList->setWindowTitle(QObject::tr("Upper side points"));
	m_pctrlLowerList->setWindowTitle(QObject::tr("Lower side points"));
	m_pctrlUpperList->setMinimumHeight(200);
	m_pctrlUpperList->setMinimumWidth(250);
	m_pctrlUpperList->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlLowerList->setMinimumHeight(200);
	m_pctrlLowerList->setMinimumWidth(250);
	m_pctrlLowerList->setSelectionBehavior(QAbstractItemView::SelectRows);

	QVBoxLayout *UpperSideLayout = new QVBoxLayout;
	UpperSideLayout->addLayout(UpperLayout);
	UpperSideLayout->addStretch(1);
	UpperSideLayout->addWidget(m_pctrlUpperList);
	UpperSideBox->setLayout(UpperSideLayout);

	QVBoxLayout *LowerSideLayout = new QVBoxLayout;
	LowerSideLayout->addLayout(LowerLayout);
	LowerSideLayout->addStretch(1);
	LowerSideLayout->addWidget(m_pctrlLowerList);
	LowerSideBox->setLayout(LowerSideLayout);

	QHBoxLayout *SideLayout = new QHBoxLayout;
	SideLayout->addWidget(UpperSideBox);
	SideLayout->addWidget(LowerSideBox);

	m_pctrlSymetric = new QCheckBox(tr("Symetric foil"));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton        = new QPushButton(tr("OK"));
	CancelButton    = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);


	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(SideLayout);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlSymetric);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);


	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pctrlSymetric, SIGNAL(clicked()), this, SLOT(OnSymetric()));
	connect(m_pctrlDegExtrados, SIGNAL(activated(int)), this, SLOT(OnSelChangeDegree()));
	connect(m_pctrlDegIntrados, SIGNAL(activated(int)), this, SLOT(OnSelChangeDegree()));
	connect(m_pctrlOutExtrados, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlOutIntrados, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}



void SplineCtrlsDlg::OnUpperCellChanged(QWidget *FloatEdit)
{
	if(m_bSymetric)
	{
		ReadData();
		if(m_bSF)
		{
			m_SF.m_Intrados.CopySymetric(&m_SF.m_Extrados);
		}
		else
		{
			m_PF.m_Intrados.CopySymetric(&m_PF.m_Intrados);
		}
		FillPointLists();
	}
}

void SplineCtrlsDlg::OnLowerCellChanged(QWidget *FloatEdit)
{

}

void SplineCtrlsDlg::FillPointLists()
{
	if(m_bSF)
	{
		m_pUpperListModel->setRowCount(m_SF.m_Extrados.m_iCtrlPoints);
		for (int i=0; i<m_SF.m_Extrados.m_iCtrlPoints; i++)
		{
			QModelIndex index = m_pUpperListModel->index(i, 0, QModelIndex());
			m_pUpperListModel->setData(index, i+1);

			QModelIndex Xindex =m_pUpperListModel->index(i, 1, QModelIndex());
			m_pUpperListModel->setData(Xindex, m_SF.m_Extrados.m_Input[i].x);

			QModelIndex Zindex =m_pUpperListModel->index(i, 2, QModelIndex());
			m_pUpperListModel->setData(Zindex, m_SF.m_Extrados.m_Input[i].y);
		}

		m_pLowerListModel->setRowCount(m_SF.m_Intrados.m_iCtrlPoints);
		for (int i=0; i<m_SF.m_Intrados.m_iCtrlPoints; i++)
		{
			QModelIndex index = m_pLowerListModel->index(i, 0, QModelIndex());
			m_pLowerListModel->setData(index, i+1);

			QModelIndex Xindex =m_pLowerListModel->index(i, 1, QModelIndex());
			m_pLowerListModel->setData(Xindex, m_SF.m_Intrados.m_Input[i].x);

			QModelIndex Zindex =m_pLowerListModel->index(i, 2, QModelIndex());
			m_pLowerListModel->setData(Zindex, m_SF.m_Intrados.m_Input[i].y);
		}
	}
	else if(m_pPF)
	{
		m_pUpperListModel->setRowCount(m_PF.m_Extrados.m_iPoints);
		for (int i=0; i<m_PF.m_Extrados.m_iPoints; i++)
		{
			QModelIndex index = m_pUpperListModel->index(i, 0, QModelIndex());
			m_pUpperListModel->setData(index, i+1);

			QModelIndex Xindex =m_pUpperListModel->index(i, 1, QModelIndex());
			m_pUpperListModel->setData(Xindex, m_PF.m_Extrados.m_ctrlPoint[i].x);

			QModelIndex Zindex =m_pUpperListModel->index(i, 2, QModelIndex());
			m_pUpperListModel->setData(Zindex,m_PF.m_Extrados.m_ctrlPoint[i].y);
		}

		m_pLowerListModel->setRowCount(m_PF.m_Intrados.m_iPoints);
		for (int i=0; i<m_PF.m_Intrados.m_iPoints; i++)
		{
			QModelIndex index = m_pLowerListModel->index(i, 0, QModelIndex());
			m_pLowerListModel->setData(index, i+1);

			QModelIndex Xindex =m_pLowerListModel->index(i, 1, QModelIndex());
			m_pLowerListModel->setData(Xindex, m_PF.m_Intrados.m_ctrlPoint[i].x);

			QModelIndex Zindex =m_pLowerListModel->index(i, 2, QModelIndex());
			m_pLowerListModel->setData(Zindex, m_PF.m_Intrados.m_ctrlPoint[i].y);
		}
	}
}


void SplineCtrlsDlg::ReadData()
{
	if(m_bSF)
	{
		for (int i=0; i<m_SF.m_Extrados.m_iCtrlPoints; i++)
		{
			QModelIndex index = m_pUpperListModel->index(i, 1, QModelIndex());
			m_SF.m_Extrados.m_Input[i].x = index.data().toDouble();

			index = m_pUpperListModel->index(i, 2, QModelIndex());
			m_SF.m_Extrados.m_Input[i].y = index.data().toDouble();
		}
		for (int i=0; i<m_SF.m_Intrados.m_iCtrlPoints; i++)
		{
			QModelIndex index = m_pLowerListModel->index(i, 1, QModelIndex());
			m_SF.m_Intrados.m_Input[i].x = index.data().toDouble();

			index = m_pLowerListModel->index(i, 2, QModelIndex());
			m_SF.m_Intrados.m_Input[i].y = index.data().toDouble();
		}

		m_SF.m_Extrados.m_iDegree = m_pctrlDegExtrados->currentIndex()+2;
		m_SF.m_Intrados.m_iDegree = m_pctrlDegIntrados->currentIndex()+2;

		m_SF.m_Extrados.m_iRes = m_pctrlOutExtrados->Value();
		m_SF.m_Intrados.m_iRes = m_pctrlOutExtrados->Value();
		m_SF.m_bSymetric = m_pctrlSymetric->isChecked();
	}
	else
	{
		for (int i=0; i<m_PF.m_Extrados.m_iPoints; i++)
		{
			QModelIndex index = m_pUpperListModel->index(i, 1, QModelIndex());
			m_PF.m_Extrados.m_ctrlPoint[i].x = index.data().toDouble();

			index = m_pUpperListModel->index(i, 2, QModelIndex());
			m_PF.m_Extrados.m_ctrlPoint[i].y = index.data().toDouble();
		}
		for (int i=0; i<m_PF.m_Intrados.m_iPoints; i++)
		{
			QModelIndex index = m_pLowerListModel->index(i, 1, QModelIndex());
			m_PF.m_Intrados.m_ctrlPoint[i].x = index.data().toDouble();

			index = m_pLowerListModel->index(i, 2, QModelIndex());
			m_PF.m_Intrados.m_ctrlPoint[i].y = index.data().toDouble();
		}

		m_PF.m_Extrados.m_Freq = m_pctrlOutExtrados->Value();
		m_PF.m_Intrados.m_Freq = m_pctrlOutIntrados->Value();
		m_PF.m_bSymetric = m_pctrlSymetric->isChecked();
	}
}


void SplineCtrlsDlg::OnOK()
{
	ReadData();
	accept();
}

void SplineCtrlsDlg::OnSymetric()
{
	m_bSymetric = m_pctrlSymetric->isChecked();
	if(m_bSymetric)
	{
		ReadData();
		if(m_bSF)
		{
			m_SF.m_Intrados.CopySymetric(&m_SF.m_Extrados);
			m_pctrlDegIntrados->setCurrentIndex(m_SF.m_Intrados.m_iDegree-2);
			m_pctrlOutIntrados->SetValue(m_SF.m_Intrados.m_iRes);
		}
		else
		{
			m_PF.m_Intrados.CopySymetric(&m_PF.m_Extrados);
			m_pctrlOutIntrados->SetValue(m_PF.m_Intrados.m_Freq);
		}
	}
	m_pctrlLowerList->setEnabled(!m_bSymetric);
	m_pctrlDegIntrados->setEnabled(!m_bSymetric);
	m_pctrlOutIntrados->setEnabled(!m_bSymetric);

	FillPointLists();
}


void SplineCtrlsDlg::OnSelChangeDegree()
{
	int i;
	i = m_pctrlDegExtrados->currentIndex();
	if(m_bSymetric) m_pctrlDegIntrados->setCurrentIndex(i);
}



void SplineCtrlsDlg::OnEditingFinished()
{
	int i;
	i = m_pctrlOutExtrados->Value();
	if(m_bSymetric) m_pctrlOutIntrados->SetValue(i);
}

