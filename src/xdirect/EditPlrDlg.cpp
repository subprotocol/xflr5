/****************************************************************************

	EditPlrDlg Class
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

#include "EditPlrDlg.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>

#include "../xdirect/XDirect.h"
#include "../miarex/Miarex.h"



QPoint EditPlrDlg::s_WindowPos;
QSize  EditPlrDlg::s_WindowSize;
bool EditPlrDlg::s_bWindowMaximized;



EditPlrDlg::EditPlrDlg()
{
	setWindowTitle(tr("Polar Points Edition"));
	m_pXDirect    = NULL;
	m_pPolar      = NULL;

	SetupLayout();
}



void EditPlrDlg::InitDialog()
{
	m_pPointModel = new QStandardItemModel;
	m_pPointModel->setRowCount(10);//temporary
	m_pPointModel->setColumnCount(14);

	m_pPointModel->setHeaderData(0, Qt::Horizontal, "Alpha");
	m_pPointModel->setHeaderData(1, Qt::Horizontal, "Cl");
	m_pPointModel->setHeaderData(2, Qt::Horizontal, "Cd");
	m_pPointModel->setHeaderData(3, Qt::Horizontal, "Cm");
	m_pPointModel->setHeaderData(4, Qt::Horizontal, "XTr_top");
	m_pPointModel->setHeaderData(5, Qt::Horizontal, "XTr_bot");
	m_pPointModel->setHeaderData(6, Qt::Horizontal, "Cl/Cd");
	m_pPointModel->setHeaderData(7, Qt::Horizontal, "Cl^3/2/Cd");
	m_pPointModel->setHeaderData(8, Qt::Horizontal, "sqrt(Cl)");
	m_pPointModel->setHeaderData(9, Qt::Horizontal, "XCp");
	m_pPointModel->setHeaderData(10, Qt::Horizontal, "HMom");
	m_pPointModel->setHeaderData(11, Qt::Horizontal, "Cdp");
	m_pPointModel->setHeaderData(12, Qt::Horizontal, "Cpmn");
	m_pPointModel->setHeaderData(13, Qt::Horizontal, "Re");

	m_pctrlPointTable->setModel(m_pPointModel);


	QHeaderView *HorizontalHeader = m_pctrlPointTable->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlPointTable->setItemDelegate(m_pFloatDelegate);

	int  *precision = new int[14];
	precision[0] = 3;
	precision[1] = 3;
	precision[2] = 3;
	precision[3] = 3;
	precision[4] = 3;
	precision[5] = 3;
	precision[6] = 3;
	precision[7] = 3;
	precision[8] = 3;
	precision[9] = 3;
	precision[10] = 3;
	precision[11] = 3;
	precision[12] = 3;
	precision[13] = 0;

	m_pFloatDelegate->SetPrecision(precision);

	FillTable();
}


void EditPlrDlg::FillTable()
{
	m_pPointModel->setColumnCount(14);
	m_pPointModel->setRowCount(m_pPolar->m_Alpha.size());
	QModelIndex index;
	for (int i=0; i<m_pPolar->m_Alpha.size(); i++)
	{
		index = m_pPointModel->index(i, 0, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Alpha.at(i));

		index = m_pPointModel->index(i, 1, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cl.at(i));

		index = m_pPointModel->index(i, 2, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cd.at(i));

		index = m_pPointModel->index(i, 3, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cm.at(i));

		index = m_pPointModel->index(i, 4, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_XTr1.at(i));

		index = m_pPointModel->index(i, 5, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_XTr2.at(i));

		index = m_pPointModel->index(i, 6, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_ClCd.at(i));

		index = m_pPointModel->index(i, 7, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cl32Cd.at(i));

		index = m_pPointModel->index(i, 8, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_RtCl.at(i));

		index = m_pPointModel->index(i, 9, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_XCp.at(i));

		index = m_pPointModel->index(i, 10, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_HMom.at(i));

		index = m_pPointModel->index(i, 11, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cdp.at(i));

		index = m_pPointModel->index(i, 12, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Cpmn.at(i));

		index = m_pPointModel->index(i, 13, QModelIndex());
		m_pPointModel->setData(index, m_pPolar->m_Re.at(i));
	}
	m_pctrlPointTable->resizeRowsToContents();
//	m_pctrlPointTable->resizeColumnsToContents();
}


void EditPlrDlg::closeEvent(QCloseEvent*event)
{
	s_WindowPos = pos();
	s_WindowSize = size();
	s_bWindowMaximized = isMaximized();
}


void EditPlrDlg::resizeEvent(QResizeEvent*event)
{
	int n = m_pPointModel->columnCount();
	int w = m_pctrlPointTable->width();
	int w14 = (int)((double)(w-25)/(double)n);

	for(int i=0; i<m_pPointModel->columnCount(); i++)
		m_pctrlPointTable->setColumnWidth(i,w14);

}

void EditPlrDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
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
			QDialog::reject();
			return;
		}
		default:
			event->ignore();
	}
}


void EditPlrDlg::OnDeletePoint()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;

	QModelIndex index = m_pctrlPointTable->currentIndex();

	if(pXDirect)
	{
		m_pPolar->Remove(index.row());
		FillTable();
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}

	if(index.row()>=m_pPointModel->rowCount()-1)
	{
		index = m_pPointModel->index(m_pPointModel->rowCount()-1,0);
	}
	if(m_pPointModel->rowCount()) m_pctrlPointTable->setCurrentIndex(index);
}



void EditPlrDlg::OnDeleteAllPoints()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;

	if(pXDirect)
	{
		m_pPolar->ResetPolar();
		FillTable();
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
}


void EditPlrDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	m_pctrlDeleteAllPoints = new QPushButton(tr("Delete All Points"));
	m_pctrlDeletePoint	   = new QPushButton(tr("Delete Point"));
	OKButton               = new QPushButton(tr("OK"));
	CancelButton           = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlDeleteAllPoints);
	CommandButtons->addWidget(m_pctrlDeletePoint);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pctrlPointTable = new QTableView(this);

	m_pctrlPointTable->setMinimumHeight(500);
	m_pctrlPointTable->setMinimumWidth(500);
	m_pctrlPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlPointTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addWidget(m_pctrlPointTable);
	MainLayout->addLayout(CommandButtons);


	setLayout(MainLayout);

	connect(m_pctrlDeletePoint, SIGNAL(clicked()),this, SLOT(OnDeletePoint()));
	connect(m_pctrlDeleteAllPoints, SIGNAL(clicked()),this, SLOT(OnDeleteAllPoints()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


