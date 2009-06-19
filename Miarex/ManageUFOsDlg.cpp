/****************************************************************************

	ManageUFOsDlg Class
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
  

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include "Miarex.h"
#include "../MainFrame.h"
#include "ManageUFOsDlg.h"


ManageUFOsDlg::ManageUFOsDlg()
{
	setWindowTitle(tr("UFO Management"));

	m_pMainFrame = NULL;
	m_pMiarex    = NULL;
	m_pWing      = NULL;
	m_pPlane     = NULL;

	SetupLayout();

	connect(m_pctrlDelete, SIGNAL(clicked()),this, SLOT(OnDelete()));
	connect(m_pctrlRename, SIGNAL(clicked()),this, SLOT(OnRename()));

	connect(m_pctrlUFOTable, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnDoubleClickTable(const QModelIndex &)));

	connect(CloseButton, SIGNAL(clicked()),this, SLOT(accept()));
}



void ManageUFOsDlg::InitDialog(QString &UFOName)
{
	FillUFOTable();
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	QString strong;

	if(m_pUFOModel->rowCount())
	{
		if(UFOName.length())
		{
			QModelIndex ind;
			for(int i=0; i< m_pUFOModel->rowCount(); i++)
			{
				ind = m_pUFOModel->index(i, 0, QModelIndex());
				strong = ind.model()->data(ind, Qt::EditRole).toString();

				if(strong == UFOName)
				{
					m_pctrlUFOTable->selectRow(i);
					break;
				}
			}
		}
		else
		{
			m_pctrlUFOTable->selectRow(0);
			QStandardItem *pItem = m_pUFOModel->item(0,0);
			UFOName = pItem->text();
		}
		m_pWing = pMiarex->GetWing(UFOName);
		if(m_pWing) m_pPlane = NULL;
		else        m_pPlane = pMiarex->GetPlane(UFOName);
	}
	else
	{
		m_pWing  = NULL;
		m_pPlane = NULL;
	}
}


void ManageUFOsDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!CloseButton->hasFocus()) CloseButton->setFocus();
			else                         accept();

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


void ManageUFOsDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	m_pctrlDelete     = new QPushButton(tr("Delete"));
	m_pctrlRename     = new QPushButton(tr("Rename"));

	CloseButton     = new QPushButton(tr("Close"));

	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlRename);
	CommandButtons->addWidget(m_pctrlDelete);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(CloseButton);
	CommandButtons->addStretch(1);

	m_pctrlUFOTable   = new QTableView(this);

	m_pctrlUFOTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlUFOTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlUFOTable->setSizePolicy(szPolicyExpanding);
	m_pctrlUFOTable->setMinimumWidth(800);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addWidget(m_pctrlUFOTable);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);

	connect(m_pctrlUFOTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnUFOClicked(const QModelIndex&)));
	connect(m_pctrlUFOTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnUFOClicked(const QModelIndex&)));

	m_pUFOModel = new QStandardItemModel;
	m_pUFOModel->setRowCount(10);//temporary
	m_pUFOModel->setColumnCount(6);

	m_pUFOModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
	m_pUFOModel->setHeaderData(1, Qt::Horizontal, tr("Wing Span"));
	m_pUFOModel->setHeaderData(2, Qt::Horizontal, tr("AR"));
	m_pUFOModel->setHeaderData(3, Qt::Horizontal, tr("TR"));
	QString str = tr("Rt-Tip Sweep") +QString::fromUtf8("Rt-Tip Sweep(°)");
	m_pUFOModel->setHeaderData(4, Qt::Horizontal, str);
	m_pUFOModel->setHeaderData(5, Qt::Horizontal, tr("Tail Volume"));

	m_pctrlUFOTable->setModel(m_pUFOModel);
	m_pctrlUFOTable->setWindowTitle(tr("UFOs"));

	m_pUFODelegate = new UFOTableDelegate;

	m_pctrlUFOTable->setItemDelegate(m_pUFODelegate);
	m_pUFODelegate->m_pUFOModel = m_pUFOModel;

	int  *precision = new int[12];
	precision[0]  = 2;
	precision[1]  = 3;
	precision[2]  = 2;
	precision[3]  = 2;
	precision[4]  = 3;
	precision[5]  = 3;
	precision[6]  = 0;

	m_pUFODelegate->m_Precision = precision;
}


void ManageUFOsDlg::FillUFOTable()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int i,n;
	m_pUFOModel->setRowCount(pMainFrame->m_oaWing.size() + pMainFrame->m_oaPlane.size());

	for(i=0; i<pMainFrame->m_oaWing.size(); i++)
	{
		FillWingRow(i);
	}
	n = pMainFrame->m_oaWing.size();
	for(i=0; i<pMainFrame->m_oaPlane.size(); i++)
	{
		FillPlaneRow(i,n);
	}
}


void ManageUFOsDlg::FillWingRow(int row)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, strong;
	QModelIndex ind;
	CWing *pWing = (CWing*)pMainFrame->m_oaWing.at(row);

	ind = m_pUFOModel->index(row, 0, QModelIndex());
	m_pUFOModel->setData(ind,pWing->m_WingName);

	ind = m_pUFOModel->index(row, 1, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_Span);

	ind = m_pUFOModel->index(row, 2, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_AR);

	ind = m_pUFOModel->index(row, 3, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_TR);

	ind = m_pUFOModel->index(row, 4, QModelIndex());
	m_pUFOModel->setData(ind,pWing->GetAverageSweep());

	ind = m_pUFOModel->index(row, 5, QModelIndex());
	m_pUFOModel->setData(ind,0);
}


void ManageUFOsDlg::FillPlaneRow(int row, int n)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString str, strong;
	QModelIndex ind;

	CPlane *pPlane = (CPlane*)pMainFrame->m_oaPlane.at(row);
	CWing *pWing = &pPlane->m_Wing;

	ind = m_pUFOModel->index(row+n, 0, QModelIndex());
	m_pUFOModel->setData(ind,pPlane->m_PlaneName);

	ind = m_pUFOModel->index(row+n, 1, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_Span);

	ind = m_pUFOModel->index(row+n, 2, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_AR);

	ind = m_pUFOModel->index(row+n, 3, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_TR);

	ind = m_pUFOModel->index(row+n, 4, QModelIndex());
	m_pUFOModel->setData(ind,pWing->GetAverageSweep());

	ind = m_pUFOModel->index(row+n, 5, QModelIndex());
	m_pUFOModel->setData(ind,pPlane->m_TailVolume);
}


void ManageUFOsDlg::OnRename()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	if(m_pPlane)      pMiarex->RenameUFO(m_pPlane->m_PlaneName);
	else if (m_pWing) pMiarex->RenameUFO(m_pWing->m_WingName);

	FillUFOTable();
	pMainFrame->SetSaveState(false);
}


void ManageUFOsDlg::OnDelete()
{
	if(!m_pWing && !m_pPlane) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	QString strong;
	if(m_pPlane) strong = tr("Are you sure you want to delete the plane :\n") +  m_pPlane->m_PlaneName +"?\n";
	else 	     strong = tr("Are you sure you want to delete the wing :\n") +   m_pWing->m_WingName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
												  QMessageBox::Cancel)) return;

	if(m_pPlane) pMainFrame->DeletePlane(m_pPlane);
	else         pMainFrame->DeleteWing(m_pWing, false);

	QModelIndex index = m_pctrlUFOTable->currentIndex();
	int sel = qMax(index.row()-1,0);

	FillUFOTable();

	if(m_pUFOModel->rowCount()>0)
	{
		m_pctrlUFOTable->selectRow(sel);

		QStandardItem *pItem = m_pUFOModel->item(sel,0);
		QString UFOName = pItem->text();

		m_pWing = pMiarex->GetWing(UFOName);
		if(m_pWing) m_pPlane = NULL; //not necessary...
		else        m_pPlane = pMiarex->GetPlane(UFOName);
	}
	else
	{
		m_pWing = NULL;
		m_pPlane = NULL;
	}
}


void ManageUFOsDlg::OnDoubleClickTable(const QModelIndex &index)
{
	if(index.row()>=0) accept();
}



void ManageUFOsDlg::OnUFOClicked(const QModelIndex& index)
{
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	QStandardItem *pItem = m_pUFOModel->item(index.row(),0);
	QString UFOName = pItem->text();

	m_pWing = pMiarex->GetWing(UFOName);
	if(m_pWing) m_pPlane = NULL; //not necessary...
	else        m_pPlane = pMiarex->GetPlane(UFOName);
}


void ManageUFOsDlg::resizeEvent(QResizeEvent *event)
{
	int w = m_pctrlUFOTable->width();
	int w8 = (int)((double)w/10.0);
//	int w10 = (int)((double)w/10.0);

	m_pctrlUFOTable->setColumnWidth(1,w8);
	m_pctrlUFOTable->setColumnWidth(2,w8);
	m_pctrlUFOTable->setColumnWidth(3,w8);
	m_pctrlUFOTable->setColumnWidth(4,w8);
	m_pctrlUFOTable->setColumnWidth(5,w8);
	
	m_pctrlUFOTable->setColumnWidth(0,w-5*w8-40);
}


