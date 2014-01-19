/****************************************************************************

	PertDlg class
	Copyright (C) 2004-2014 Andre Deperrois adeperrois@xflr5.com

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include "PertDlg.h"
#include "../misc/Settings.h"




PertDlg::PertDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Pertubation Dialog"));
	m_pXInverse = pParent;
	memset(m_cnr,   0, sizeof(m_cnr));
	memset(m_cni,   0, sizeof(m_cni));
	memset(m_backr, 0, sizeof(m_backr));
	memset(m_backi, 0, sizeof(m_backi));

	SetupLayout();

	connect(RestoreButton, SIGNAL(clicked()),this, SLOT(OnRestore()));
	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

PertDlg::~PertDlg()
{
	delete [] m_precision;
}


void PertDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	RestoreButton	= new QPushButton(tr("Restore"));
	ApplyButton	= new QPushButton(tr("Apply"));
	OKButton        = new QPushButton(tr("OK"));
	CancelButton    = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(RestoreButton);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pctrlCnTable = new QTableView(this);
	m_pctrlCnTable->setFont(Settings::s_TableFont);
	m_pctrlCnTable->setWindowTitle(tr("Cn List"));
	m_pctrlCnTable->setMinimumHeight(500);
	m_pctrlCnTable->setMinimumWidth(350);

	m_pCnModel = new QStandardItemModel;
	m_pCnModel->setColumnCount(3);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addWidget(m_pctrlCnTable);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);
}



void PertDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else if(CancelButton->hasFocus())
			{
				reject();
				return;
			}
			else
			{
				accept();
				return;

			}
			break;
		}
		default:
			break;
	}
}

void PertDlg::OnApply()
{
	ReadData();
	OKButton->setFocus();

}


void PertDlg::OnOK()
{
	ReadData();
	accept();
}


void PertDlg::ReadData()
{
	int pos;
	bool bOK;
	QString strong;
	QStandardItem *pItem;

	for (pos=0; pos<m_pCnModel->rowCount(); pos++)
	{
		pItem = m_pCnModel->item(pos,1);
		strong =pItem->text();
		m_cnr[pos]=strong.toDouble(&bOK);

		pItem = m_pCnModel->item(pos,2);
		strong =pItem->text();
		m_cni[pos]=strong.toDouble(&bOK);
	}
}


void PertDlg::InitDialog() 
{
	memcpy(m_backr, m_cnr, sizeof(m_cnr));
	memcpy(m_backi, m_cni, sizeof(m_cni));

	m_pCnModel = new QStandardItemModel(this);
	m_pCnModel->setRowCount(10);//temporary
	m_pCnModel->setColumnCount(3);

	m_pCnModel->setHeaderData(0, Qt::Horizontal, QObject::tr("i"));
	m_pCnModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Cn"));
	m_pCnModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Ci"));

	m_pctrlCnTable->setModel(m_pCnModel);
	m_pctrlCnTable->setColumnWidth(0,30);
	m_pctrlCnTable->setColumnWidth(1,100);
	m_pctrlCnTable->setColumnWidth(2,100);

	m_pFloatDelegate = new FloatEditDelegate(this);
	m_pctrlCnTable->setItemDelegate(m_pFloatDelegate);

	m_precision = new int[3];
	m_precision[0] = 0;
	m_precision[1] = 5;
	m_precision[2] = 5;
	m_pFloatDelegate->SetPrecision(m_precision);

	connect(m_pFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	FillCnModel();

	m_pctrlCnTable->setFocus();
}


void PertDlg::OnCellChanged(QWidget *)
{
	QModelIndex index = m_pctrlCnTable->currentIndex();
	int pos = index.row();
	if(pos<0)  return;
	
	bool bOK;
	QString strong;
	QStandardItem *pItem;
	pItem = m_pCnModel->item(pos,1);
	strong =pItem->text();
	m_cnr[pos]=strong.toDouble(&bOK);

	pItem = m_pCnModel->item(pos,2);
	strong =pItem->text();
	m_cni[pos]=strong.toDouble(&bOK);

	FillCnModel();
}


void PertDlg::FillCnModel() 
{
	m_pCnModel->setRowCount(m_nc);

	for (int i=0; i<m_nc; i++)
	{
		QModelIndex Xindex = m_pCnModel->index(i, 0, QModelIndex());
		m_pCnModel->setData(Xindex, i);

		QModelIndex Yindex =m_pCnModel->index(i, 1, QModelIndex());
		m_pCnModel->setData(Yindex, m_cnr[i]);

		QModelIndex Zindex =m_pCnModel->index(i, 2, QModelIndex());
		m_pCnModel->setData(Zindex, m_cni[i]);
	}
}


void PertDlg::OnRestore() 
{
	for (int i=0; i<m_nc; i++)
	{
		m_cnr[i]   = m_backr[i];
		m_cni[i]   = m_backi[i];
//		m_backi[i] = m_cni[i];
	}

	FillCnModel();
}

















