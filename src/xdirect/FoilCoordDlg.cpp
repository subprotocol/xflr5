/****************************************************************************

	FoilCoordDlg Class
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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QHeaderView>

#include "FoilCoordDlg.h"
#include "XDirect.h"
#include "../design/AFoil.h"



FoilCoordDlg::FoilCoordDlg()
{
	setWindowTitle(tr("Foil Coordinates"));
	m_pAFoil      = NULL;
	m_pXDirect    = NULL;
	m_pBufferFoil = NULL;
	m_pMemFoil    = NULL;
	SetupLayout();

	m_bApplied  = true;
	m_bModified = false;
}


void FoilCoordDlg::FillList()
{
	m_pCoordModel->setRowCount(m_pBufferFoil->n);
	m_pCoordModel->setColumnCount(2);
	for (int i=0; i<m_pMemFoil->n; i++)
	{
		QModelIndex Xindex = m_pCoordModel->index(i, 0, QModelIndex());
		m_pCoordModel->setData(Xindex, m_pBufferFoil->x[i]);

		QModelIndex Yindex =m_pCoordModel->index(i, 1, QModelIndex());
		m_pCoordModel->setData(Yindex, m_pBufferFoil->y[i]);
	}
}


void FoilCoordDlg::InitDialog()
{
	if(!m_pMemFoil || !m_pBufferFoil) return;

	int w = m_pctrlCoordView->width();
	m_pctrlCoordView->setColumnWidth(0,(int)(w/2));
	m_pctrlCoordView->setColumnWidth(1,(int)(w/2));
	QHeaderView *HorizontalHeader = m_pctrlCoordView->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pCoordModel = new QStandardItemModel;
	m_pCoordModel->setRowCount(10);//temporary
	m_pCoordModel->setColumnCount(2);
	m_pCoordModel->setHeaderData(0, Qt::Horizontal, QObject::tr("X"));
	m_pCoordModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Y"));

	m_pctrlCoordView->setModel(m_pCoordModel);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlCoordView->setItemDelegate(m_pFloatDelegate);

	int  *precision = new int[2];
	precision[0] = 5;//five digits for x and y coordinates
	precision[1] = 5;
	m_pFloatDelegate->SetPrecision(precision);

//void QAbstractItemDelegate::closeEditor ( QWidget * editor, QAbstractItemDelegate::EndEditHint hint = NoHint )
	connect(m_pFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	QItemSelectionModel *selectionModel = new QItemSelectionModel(m_pCoordModel);
	m_pctrlCoordView->setSelectionModel(selectionModel);
	connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));


//void QAbstractItemView::activated ( const QModelIndex & index )   [signal]
//void itemChanged ( QStandardItem * item )
	connect(m_pctrlApply, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(m_pctrlDeletePoint, SIGNAL(clicked()),this, SLOT(OnDeletePoint()));
	connect(m_pctrlInsertPoint, SIGNAL(clicked()),this, SLOT(OnInsertPoint()));
	connect(m_pctrlRestore, SIGNAL(clicked()),this, SLOT(OnRestore()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	FillList();
}



void FoilCoordDlg::resizeEvent(QResizeEvent *event)
{
	int w2 = (int)((double)m_pctrlCoordView->width()*.7/2);

	m_pctrlCoordView->setColumnWidth(0,w2);
	m_pctrlCoordView->setColumnWidth(1,w2);
}


void FoilCoordDlg::keyPressEvent(QKeyEvent *event)
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
		default:
			event->ignore();
	}
}


void FoilCoordDlg::OnApply()
{
	if(m_bApplied) return;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	m_bApplied = true;
	m_bModified = true;
	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
}

void FoilCoordDlg::OnDeletePoint()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	int i, sel;
	QModelIndex index = m_pctrlCoordView->currentIndex();
	sel = index.row();

	if(sel<0) return;

	for (i=sel;i<m_pBufferFoil->nb-1; i++)
	{
		m_pBufferFoil->xb[i] = m_pBufferFoil->xb[i+1];
		m_pBufferFoil->yb[i] = m_pBufferFoil->yb[i+1];
	}
	for (i=sel;i<m_pBufferFoil->n-1; i++)
	{
		m_pBufferFoil->x[i] = m_pBufferFoil->x[i+1];
		m_pBufferFoil->y[i] = m_pBufferFoil->y[i+1];
	}
	m_pBufferFoil->nb--;
	m_pBufferFoil->n--;

	FillList();
	SetSelection(sel);
	m_bModified = true;
	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
}

void FoilCoordDlg::OnInsertPoint()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	int i, sel;
	sel = m_pctrlCoordView->currentIndex().row();


	if(sel<=0) return;

	for (i=m_pBufferFoil->nb; i>sel; i--)
	{
		m_pBufferFoil->xb[i] = m_pBufferFoil->xb[i-1];
		m_pBufferFoil->yb[i] = m_pBufferFoil->yb[i-1];
	}
	m_pBufferFoil->xb[sel] = (m_pBufferFoil->xb[sel-1] + m_pBufferFoil->xb[sel+1])/2.0;
	m_pBufferFoil->yb[sel] = (m_pBufferFoil->yb[sel-1] + m_pBufferFoil->yb[sel+1])/2.0 ;

	for (i=m_pBufferFoil->n; i>sel; i--)
	{
		m_pBufferFoil->x[i] = m_pBufferFoil->x[i-1];
		m_pBufferFoil->y[i] = m_pBufferFoil->y[i-1];
	}
	m_pBufferFoil->x[sel] = (m_pBufferFoil->x[sel-1] + m_pBufferFoil->x[sel+1])/2.;
	m_pBufferFoil->y[sel] = (m_pBufferFoil->y[sel-1] + m_pBufferFoil->y[sel+1])/2.;

	m_pBufferFoil->nb++;
	m_pBufferFoil->n++;

	FillList();
	SetSelection(sel);

	m_bModified = true;
	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
}




void FoilCoordDlg::OnCellChanged(QWidget *FloatEdit)
{
	double X,Y;

	int  sel = m_pctrlCoordView->currentIndex().row();

	QModelIndex Xindex = m_pCoordModel->index(sel, 0);
	X = Xindex.data().toDouble();
	m_pBufferFoil->x[sel]  = X;
	m_pBufferFoil->xb[sel] = X;

	QModelIndex Yindex = m_pCoordModel->index(sel, 1);
	Y = Yindex.data().toDouble();
	m_pBufferFoil->y[sel]  = Y;
	m_pBufferFoil->yb[sel] = Y;

	m_bApplied = false;

	OnApply();
}


void FoilCoordDlg::OnItemClicked(QModelIndex index)
{
	int sel = m_pctrlCoordView->currentIndex().row();
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	if(m_pBufferFoil)	m_pBufferFoil->m_iHighLight = sel;

	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
}



void FoilCoordDlg::OnRestore()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	int i;

	for (i=0;i<m_pMemFoil->nb; i++)
	{
		m_pBufferFoil->xb[i] = m_pMemFoil->xb[i];
		m_pBufferFoil->yb[i] = m_pMemFoil->yb[i];
	}
	m_pBufferFoil->nb = m_pMemFoil->n;
	for (i=0;i<m_pMemFoil->n; i++)
	{
		m_pBufferFoil->x[i]  = m_pMemFoil->x[i];
		m_pBufferFoil->y[i]  = m_pMemFoil->y[i];
	}
	m_pBufferFoil->n = m_pMemFoil->n;


	FillList();
	m_bApplied = true;
	m_bModified = false;

	SetSelection(0);
	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
}

void FoilCoordDlg::ReadSectionData(int sel, double &X, double &Y)
{
	QModelIndex XIndex =m_pCoordModel->index(sel, 0, QModelIndex());
	X = XIndex.data().toDouble();
	QModelIndex YIndex =m_pCoordModel->index(sel, 0, QModelIndex());
	Y = YIndex.data().toDouble();

}

void FoilCoordDlg::SetSelection(int sel)
{
	if(sel>=0)
	{
		m_pctrlCoordView->selectRow(sel);
	}
}


void FoilCoordDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	m_pctrlInsertPoint	= new QPushButton(tr("Insert Point"));
	m_pctrlDeletePoint	= new QPushButton(tr("Delete Point"));
	m_pctrlRestore      = new QPushButton(tr("Restore"));
	m_pctrlApply        = new QPushButton(tr("Apply"));
	OKButton            = new QPushButton(tr("OK"));
	CancelButton        = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlInsertPoint);
	CommandButtons->addWidget(m_pctrlDeletePoint);
	CommandButtons->addWidget(m_pctrlRestore);
	CommandButtons->addWidget(m_pctrlApply);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pctrlCoordView = new QTableView(this);
	m_pctrlCoordView->setMinimumHeight(500);
	m_pctrlCoordView->setMinimumWidth(150);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addWidget(m_pctrlCoordView);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);
}


void FoilCoordDlg::showEvent(QShowEvent *event)
{
	setWindowModality(Qt::NonModal);

//	Qt::WindowFlags flags = windowFlags();
//	flags = Qt::Dialog | Qt::WindowStaysOnTopHint;
//	setWindowFlags(flags);
}
