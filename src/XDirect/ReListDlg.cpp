/****************************************************************************

	ReListDlg Class
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
#include <QHeaderView>
#include <QStringList>
#include "XDirect.h"
#include "ReListDlg.h"
#include "../Misc/FloatEdit.h"

ReListDlg::ReListDlg()
{
	setWindowTitle(tr("Reynolds Number List"));
	memset(m_ReList, 0, sizeof(m_ReList));
	memset(m_MachList, 0, sizeof(m_MachList));
	memset(m_NCritList, 0, sizeof(m_NCritList));
	m_NRe = 0;
	m_iSelection = 0;

	SetupLayout();

	connect(m_pctrlDelete, SIGNAL(clicked()),this, SLOT(OnDelete()));
	connect(m_pctrlInsert, SIGNAL(clicked()),this, SLOT(OnInsert()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}



void ReListDlg::InitDialog()
{
	m_pReModel = new QStandardItemModel;
	m_pReModel->setRowCount(10);//temporary
	m_pReModel->setColumnCount(3);

	m_pReModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Re"));
	m_pReModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Mach"));
	m_pReModel->setHeaderData(2, Qt::Horizontal, QObject::tr("NCrit"));

	m_pctrlReView->setModel(m_pReModel);
	
	int w = m_pctrlReView->width();
	m_pctrlReView->setColumnWidth(0,(int)(w/3));
	m_pctrlReView->setColumnWidth(1,(int)(w/3));
	m_pctrlReView->setColumnWidth(2,(int)(w/3));
	QHeaderView *HorizontalHeader = m_pctrlReView->horizontalHeader();
	HorizontalHeader->setStretchLastSection(true);

	m_pFloatDelegate = new FloatEditDelegate;
	m_pctrlReView->setItemDelegate(m_pFloatDelegate);

	int  *precision = new int[3];
	precision[0] = 0;//no digits for Re Number
	precision[1] = 2;//two digits for Mach and NCrit
	precision[2] = 2;//two digits for Mach and NCrit
	m_pFloatDelegate->SetPrecision(precision);

	connect(m_pFloatDelegate, SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	FillReModel();
}


void ReListDlg::keyPressEvent(QKeyEvent *event)
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


void ReListDlg::OnCellChanged(QWidget *FloatEdit)
{
//sort the data in case the Re Number was changed
	SortData();

}

void ReListDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	m_pctrlInsert	= new QPushButton(tr("Insert"));
	m_pctrlDelete	= new QPushButton(tr("Delete"));

	OKButton        = new QPushButton(tr("OK"));
	CancelButton    = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlInsert);
	CommandButtons->addWidget(m_pctrlDelete);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pctrlReView = new QTableView(this);

	m_pctrlReView->setMinimumHeight(500);
	m_pctrlReView->setMinimumWidth(250);
	m_pctrlReView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlReView->setEditTriggers(QAbstractItemView::CurrentChanged |
								   QAbstractItemView::DoubleClicked |
								   QAbstractItemView::SelectedClicked |
								   QAbstractItemView::EditKeyPressed);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addWidget(m_pctrlReView);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);
}


void ReListDlg::FillReModel()
{
	m_pReModel->setColumnCount(3);
	m_pReModel->setRowCount(m_NRe);

	for (int i=0; i<m_NRe; i++)
	{
		QModelIndex Xindex = m_pReModel->index(i, 0, QModelIndex());
		m_pReModel->setData(Xindex, m_ReList[i]);

		QModelIndex Yindex =m_pReModel->index(i, 1, QModelIndex());
		m_pReModel->setData(Yindex, m_MachList[i]);

		QModelIndex Zindex =m_pReModel->index(i, 2, QModelIndex());
		m_pReModel->setData(Zindex, m_NCritList[i]);
	}
	m_pctrlReView->resizeRowsToContents();
//	m_pctrlReView->resizeColumnsToContents();
	m_pctrlReView->setWindowTitle(QObject::tr("Re List"));
}


void ReListDlg::OnDelete()
{
	int i, sel;
	QModelIndex index = m_pctrlReView->currentIndex();
	sel = index.row();

	if(sel<0) return;

	for (i=sel;i<m_NRe-1; i++)
	{
		m_ReList[i]    = m_ReList[i+1];
		m_MachList[i]  = m_MachList[i+1];
		m_NCritList[i] = m_NCritList[i+1];
	}

	m_NRe --;

	FillReModel();
}


void ReListDlg::OnInsert()
{
	int i, sel;
	sel = m_pctrlReView->currentIndex().row();
//	if(sel<0) return;

	for (i=m_NRe; i>sel; i--)
	{
		m_ReList[i]    = m_ReList[i-1];
		m_MachList[i]  = m_MachList[i-1];
		m_NCritList[i] = m_NCritList[i-1];
	}

        if(sel>0)        m_ReList[sel]    = (m_ReList[sel-1]+m_ReList[sel+1]) /2.0;
        else if(sel==0)  m_ReList[sel]    = m_ReList[sel+1]                   /2.0;
        else             m_ReList[0]      = 100000.0;

        if(sel>=0)
        {
            m_MachList[sel]  = m_MachList[sel+1];
            m_NCritList[sel] = m_NCritList[sel+1];
        }
        else
        {
            sel = 0;
            m_MachList[sel]  = 0.0;
            m_NCritList[sel] = 0.0;
        }

	m_NRe ++;

	FillReModel();

        QModelIndex index = m_pReModel->index(sel, 0, QModelIndex());
        m_pctrlReView->setCurrentIndex(index);
        m_pctrlReView->openPersistentEditor(index);
}


void ReListDlg::OnOK()
{
	for (int i=0; i<m_NRe; i++)
	{
		QModelIndex index = m_pReModel->index(i, 0, QModelIndex());
		m_ReList[i] = index.data().toDouble();

		index = m_pReModel->index(i, 1, QModelIndex());
		m_MachList[i] = index.data().toDouble();

		index = m_pReModel->index(i, 2, QModelIndex());
		m_NCritList[i] = index.data().toDouble();

	}
	done(1);
}



void ReListDlg::SortData()
{
	int i;
	double ReList[30];
	double MachList[30];
	double NCritList[30];
	// unfortunately, the signal doesn't tell us which cell was changed
	// so we sort it all... great
	// hopefully, only one at most is out of order : the latest one which was changed
	//first read the data
	for (i=0; i<m_NRe; i++)
	{
		ReList[i]    = m_pReModel->index(i, 0, QModelIndex()).data().toDouble();
		MachList[i]  = m_pReModel->index(i, 1, QModelIndex()).data().toDouble();
		NCritList[i] = m_pReModel->index(i, 2, QModelIndex()).data().toDouble();
	}

	bool bNotSorted = true;
	double tmp;
	while(bNotSorted)
	{
		bNotSorted = false;
		for(i=1; i<m_NRe; i++)
		{
			if(ReList[i]<ReList[i-1])
			{
				//swap the two rows
				tmp = ReList[i-1];    ReList[i-1]    = ReList[i];       ReList[i]    = tmp;
				tmp = MachList[i-1];  MachList[i-1]  = MachList[i];     MachList[i]  = tmp;
				tmp = NCritList[i-1]; NCritList[i-1] = NCritList[i];    NCritList[i] = tmp;
				bNotSorted = true;//try again...
			}
		}
	}


	//paste back the sorted data
	memcpy(m_ReList,    ReList, 30*sizeof(double));
	memcpy(m_MachList,  MachList, 30*sizeof(double));
	memcpy(m_NCritList, NCritList, 30*sizeof(double));

	//and fill back the model
	FillReModel();
}





