/****************************************************************************

	ManageUFOsDlg Class
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
  
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QMessageBox>
#include "Miarex.h"
#include "../Globals.h"
#include "../MainFrame.h"
#include "ManageUFOsDlg.h"


void *ManageUFOsDlg::s_pMainFrame;
void *ManageUFOsDlg::s_pMiarex;


ManageUFOsDlg::ManageUFOsDlg()
{
	setWindowTitle(tr("Object Management"));

	m_pWing      = NULL;
	m_pPlane     = NULL;

	SetupLayout();

	connect(m_pctrlDelete, SIGNAL(clicked()),this, SLOT(OnDelete()));
	connect(m_pctrlRename, SIGNAL(clicked()),this, SLOT(OnRename()));
	connect(m_pctrlUFOTable, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnDoubleClickTable(const QModelIndex &)));
	connect(m_pctrlDescription, SIGNAL(textChanged()), this, SLOT(OnDescriptionChanged()));
	connect(CloseButton, SIGNAL(clicked()),this, SLOT(accept()));
}



void ManageUFOsDlg::InitDialog(QString &UFOName)
{
	FillUFOTable();
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	QString strong;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strArea, strLength;
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);
	GetAreaUnit(strArea, pMainFrame->m_AreaUnit);
	m_pUFOModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
	m_pUFOModel->setHeaderData(1, Qt::Horizontal, tr("Span")+" ("+strLength+")");
	m_pUFOModel->setHeaderData(2, Qt::Horizontal, tr("Area")+" ("+strArea+")");
	m_pUFOModel->setHeaderData(3, Qt::Horizontal, tr("M.A.C.")+" ("+strLength+")");
	m_pUFOModel->setHeaderData(4, Qt::Horizontal, tr("AR"));
	m_pUFOModel->setHeaderData(5, Qt::Horizontal, tr("TR"));
	QString str = tr("Rt-Tip Sweep") +QString::fromUtf8(" (°)");
	m_pUFOModel->setHeaderData(6, Qt::Horizontal, str);
	m_pUFOModel->setHeaderData(7, Qt::Horizontal, tr("Tail Volume"));


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
			if(pItem) UFOName = pItem->text();
			else      UFOName.clear();
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

	m_pctrlUFOTable = new QTableView(this);
	m_pctrlUFOTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlUFOTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlUFOTable->horizontalHeader()->setStretchLastSection(true);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlUFOTable->setSizePolicy(szPolicyExpanding);
	m_pctrlUFOTable->setMinimumWidth(800);

	m_pctrlDescription = new QTextEdit;
//	m_pctrlDescription->setEnabled(false);
	QLabel *Description = new QLabel(tr("Description:"));

	QVBoxLayout *LeftLayout = new QVBoxLayout;
	LeftLayout->addWidget(m_pctrlUFOTable);
	LeftLayout->addWidget(Description);
	LeftLayout->addWidget(m_pctrlDescription);
	LeftLayout->setStretchFactor(m_pctrlUFOTable, 5);
	LeftLayout->setStretchFactor(m_pctrlDescription, 1);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addLayout(LeftLayout);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);

	m_pUFOModel = new QStandardItemModel;
	m_pUFOModel->setRowCount(10);//temporary
	m_pUFOModel->setColumnCount(8);

	m_pctrlUFOTable->setModel(m_pUFOModel);
	m_pctrlUFOTable->setWindowTitle(tr("UFOs"));

	QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pUFOModel);
	m_pctrlUFOTable->setSelectionModel(selectionModel);
	connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnUFOClicked(QModelIndex)));

	m_pUFODelegate = new UFOTableDelegate;

	m_pctrlUFOTable->setItemDelegate(m_pUFODelegate);
	m_pUFODelegate->m_pUFOModel = m_pUFOModel;

	int  *precision = new int[12];
	precision[0]  = 2;
	precision[1]  = 3;
	precision[2]  = 3;
	precision[3]  = 3;
	precision[4]  = 2;
	precision[5]  = 2;
	precision[6]  = 1;
	precision[7]  = 2;
	precision[8]  = 3;

	m_pUFODelegate->m_Precision = precision;
}


void ManageUFOsDlg::FillUFOTable()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(row>=pMainFrame->m_oaWing.size()) return;

	QModelIndex ind;
	CWing *pWing = (CWing*)pMainFrame->m_oaWing.at(row);
	if(!pWing) return;

	ind = m_pUFOModel->index(row, 0, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_WingName);
	if(pWing->m_WingDescription.length()) m_pUFOModel->setData(ind, pWing->m_WingDescription, Qt::ToolTipRole);

	ind = m_pUFOModel->index(row, 1, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_PlanformSpan*pMainFrame->m_mtoUnit);

	ind = m_pUFOModel->index(row, 2, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_PlanformArea*pMainFrame->m_m2toUnit);

	ind = m_pUFOModel->index(row, 3, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_MAChord*pMainFrame->m_mtoUnit);

	ind = m_pUFOModel->index(row, 4, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_AR);

	ind = m_pUFOModel->index(row, 5, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_TR);

	ind = m_pUFOModel->index(row, 6, QModelIndex());
	m_pUFOModel->setData(ind,pWing->AverageSweep());

	ind = m_pUFOModel->index(row, 7, QModelIndex());
	m_pUFOModel->setData(ind,0.0);
}


void ManageUFOsDlg::FillPlaneRow(int row, int n)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QModelIndex ind;

	if(row>=pMainFrame->m_oaPlane.size()) return;

	CPlane *pPlane = (CPlane*)pMainFrame->m_oaPlane.at(row);
	if(!pPlane) return;
	CWing *pWing = pPlane->Wing();

	ind = m_pUFOModel->index(row+n, 0, QModelIndex());
	m_pUFOModel->setData(ind,pPlane->PlaneName());
	if(pPlane->PlaneDescription().length()) m_pUFOModel->setData(ind, pPlane->PlaneDescription(), Qt::ToolTipRole);

	ind = m_pUFOModel->index(row+n, 1, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_PlanformSpan*pMainFrame->m_mtoUnit);

	ind = m_pUFOModel->index(row+n, 2, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_PlanformArea*pMainFrame->m_m2toUnit);

	ind = m_pUFOModel->index(row+n, 3, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_MAChord*pMainFrame->m_mtoUnit);

	ind = m_pUFOModel->index(row+n, 4, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_AR);

	ind = m_pUFOModel->index(row+n, 5, QModelIndex());
	m_pUFOModel->setData(ind, pWing->m_TR);

	ind = m_pUFOModel->index(row+n, 6, QModelIndex());
	m_pUFOModel->setData(ind,pWing->AverageSweep());

	ind = m_pUFOModel->index(row+n, 7, QModelIndex());
	m_pUFOModel->setData(ind,pPlane->TailVolume());
}


void ManageUFOsDlg::OnRename()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	if(m_pPlane)      pMiarex->RenameUFO(m_pPlane->PlaneName());
	else if (m_pWing) pMiarex->RenameUFO(m_pWing->m_WingName);

	FillUFOTable();
	pMainFrame->SetSaveState(false);
}


void ManageUFOsDlg::OnDelete()
{
	if(!m_pWing && !m_pPlane) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	QString strong;
	if(m_pPlane) strong = tr("Are you sure you want to delete the plane :\n") +  m_pPlane->PlaneName() +"?\n";
	else 	     strong = tr("Are you sure you want to delete the wing :\n") +   m_pWing->m_WingName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
										 QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
										 QMessageBox::Yes)) return;

	if(m_pPlane) pMainFrame->DeletePlane(m_pPlane);
	else         pMainFrame->DeleteWing(m_pWing, false);

	QModelIndex index = m_pctrlUFOTable->currentIndex();
	int sel = qMax(index.row()-1,0);

	FillUFOTable();

	if(m_pUFOModel->rowCount()>0)
	{
		m_pctrlUFOTable->selectRow(sel);
		QString UFOName;

		QStandardItem *pItem = m_pUFOModel->item(sel,0);
		if(pItem) UFOName = pItem->text();
		else      UFOName.clear();

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


void ManageUFOsDlg::OnDescriptionChanged()
{
	if(m_pPlane)
	{
		m_pPlane->rPlaneDescription() = m_pctrlDescription->toPlainText();
	}
	else if(m_pWing)
	{
		m_pWing->m_WingDescription = m_pctrlDescription->toPlainText();
	}
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
}


void ManageUFOsDlg::OnUFOClicked(QModelIndex index)
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	QStandardItem *pItem = m_pUFOModel->item(index.row(),0);
	QString UFOName;
	if(pItem) UFOName = pItem->text();
	else      UFOName.clear();

	m_pWing = pMiarex->GetWing(UFOName);
	if(m_pWing)
	{
		m_pPlane = NULL; //not necessary...
		m_pctrlDescription->setText(m_pWing->m_WingDescription);
	}
	else
	{
		m_pPlane = pMiarex->GetPlane(UFOName);
		if(m_pPlane) m_pctrlDescription->setText(m_pPlane->PlaneDescription());
	}
}


void ManageUFOsDlg::resizeEvent(QResizeEvent *event)
{
	int w = m_pctrlUFOTable->width();
	int w8 = (int)((double)w/12.0);
//	int w10 = (int)((double)w/10.0);

	m_pctrlUFOTable->setColumnWidth(1,w8);
	m_pctrlUFOTable->setColumnWidth(3,w8);
	m_pctrlUFOTable->setColumnWidth(2,w8);
	m_pctrlUFOTable->setColumnWidth(4,w8);
	m_pctrlUFOTable->setColumnWidth(5,w8);
	m_pctrlUFOTable->setColumnWidth(6,w8);
	m_pctrlUFOTable->setColumnWidth(7,w8);
	
	m_pctrlUFOTable->setColumnWidth(0,w-7*w8-40);
}


