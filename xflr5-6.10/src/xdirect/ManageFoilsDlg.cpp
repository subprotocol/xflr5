/****************************************************************************

	ManageFoilsDlg Class
	Copyright (C) 2009-2014 Andre Deperrois adeperrois@xflr5.com

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
#include <QHeaderView>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStringList>
#include "../misc/Settings.h"
#include "../misc/RenameDlg.h"
#include "ManageFoilsDlg.h"



ManageFoilsDlg::ManageFoilsDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Foil Management"));

	m_bChanged = false;
	m_iSelection = 0;
	m_pFoil = NULL;
	m_precision = NULL;

	SetupLayout();

	connect(m_pctrlDelete, SIGNAL(clicked()),this, SLOT(OnDelete()));
	connect(m_pctrlRename, SIGNAL(clicked()),this, SLOT(OnRename()));
	connect(m_pctrlExport, SIGNAL(clicked()),this, SLOT(OnExport()));
	connect(m_pctrlFoilTable, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnDoubleClickTable(const QModelIndex &)));

	connect(CloseButton, SIGNAL(clicked()),this, SLOT(accept()));
}



void ManageFoilsDlg::InitDialog(QString FoilName)
{
	FillFoilTable();
	QString strong;

	if(m_pFoilModel->rowCount())
	{
		if(FoilName.length())
		{
			QModelIndex ind;
			for(int i=0; i< m_pFoilModel->rowCount(); i++)
			{
				ind = m_pFoilModel->index(i, 0, QModelIndex());
				strong = ind.model()->data(ind, Qt::EditRole).toString();

				if(strong == FoilName)
				{
					m_pctrlFoilTable->selectRow(i);
					break;
				}
			}
		}
		else
		{
			m_pctrlFoilTable->selectRow(0);
			QStandardItem *pItem = m_pFoilModel->item(0,0);
			FoilName = pItem->text();
		}
		m_pFoil = Foil::foil(FoilName);
	}
	else
	{
		m_pFoil = NULL;
	}
}


void ManageFoilsDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
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

void ManageFoilsDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	{
		m_pctrlDelete     = new QPushButton(tr("Delete"));
		m_pctrlRename     = new QPushButton(tr("Rename"));
		m_pctrlExport     = new QPushButton(tr("Export Foil"));

		CloseButton     = new QPushButton(tr("Close"));

		CommandButtons->addStretch(1);
		CommandButtons->addWidget(m_pctrlDelete);
		CommandButtons->addWidget(m_pctrlRename);
		CommandButtons->addWidget(m_pctrlExport);
		CommandButtons->addStretch(2);
		CommandButtons->addWidget(CloseButton);
		CommandButtons->addStretch(1);
	}

	m_pctrlFoilTable   = new QTableView(this);
	m_pctrlFoilTable->setFont(Settings::s_TableFont);
	m_pctrlFoilTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlFoilTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlFoilTable->horizontalHeader()->setStretchLastSection(true);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlFoilTable->setSizePolicy(szPolicyExpanding);
	m_pctrlFoilTable->setMinimumWidth(800);

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	{
		MainLayout->addWidget(m_pctrlFoilTable);
		MainLayout->addLayout(CommandButtons);
	}
	setLayout(MainLayout);


	connect(m_pctrlFoilTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));


	m_pFoilModel = new QStandardItemModel(this);
	m_pFoilModel->setRowCount(10);//temporary
	m_pFoilModel->setColumnCount(12);

	m_pFoilModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
	m_pFoilModel->setHeaderData(1, Qt::Horizontal, tr("Thickness (%)"));
	m_pFoilModel->setHeaderData(2, Qt::Horizontal, tr("at (%)"));
	m_pFoilModel->setHeaderData(3, Qt::Horizontal, tr("Camber (%)"));
	m_pFoilModel->setHeaderData(4, Qt::Horizontal, tr("at (%)"));
	m_pFoilModel->setHeaderData(5, Qt::Horizontal, tr("Points"));
	m_pFoilModel->setHeaderData(6, Qt::Horizontal, tr("TE Flap (")+QString::fromUtf8("°")+")");
	m_pFoilModel->setHeaderData(7, Qt::Horizontal, tr("TE XHinge"));
	m_pFoilModel->setHeaderData(8, Qt::Horizontal, tr("TE YHinge"));
	m_pFoilModel->setHeaderData(9, Qt::Horizontal, tr("LE Flap (")+QString::fromUtf8("°")+")");
	m_pFoilModel->setHeaderData(10, Qt::Horizontal, tr("LE XHinge"));
	m_pFoilModel->setHeaderData(11, Qt::Horizontal, tr("LE YHinge"));

	m_pctrlFoilTable->setModel(m_pFoilModel);
	m_pctrlFoilTable->setWindowTitle(tr("Foils"));

	m_pFoilDelegate = new FoilTableDelegate(this);
	m_pFoilDelegate->m_pManageFoils = this;
	m_pctrlFoilTable->setItemDelegate(m_pFoilDelegate);
	m_pFoilDelegate->m_pFoilModel = m_pFoilModel;

	m_precision = new int[12];
	m_precision[0]  = 2;
	m_precision[1]  = 2;
	m_precision[2]  = 2;
	m_precision[3]  = 2;
	m_precision[4]  = 2;
	m_precision[5]  = 0;
	m_precision[6]  = 2;
	m_precision[7]  = 2;
	m_precision[8]  = 2;
	m_precision[9]  = 2;
	m_precision[10] = 2;
	m_precision[11] = 2;

	m_pFoilDelegate->m_Precision = m_precision;
}


ManageFoilsDlg::~ManageFoilsDlg()
{
	if(m_precision) delete [] m_precision;
}

void ManageFoilsDlg::FillFoilTable()
{
	int i;
	m_pFoilModel->setRowCount(Foil::s_oaFoil.size());

	for(i=0; i<Foil::s_oaFoil.size(); i++)
	{
		FillTableRow(i);
	}
}


void ManageFoilsDlg::FillTableRow(int row)
{
	QModelIndex ind;
	Foil *pFoil = (Foil*)Foil::s_oaFoil.at(row);

	ind = m_pFoilModel->index(row, 0, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->m_FoilName);

	if(pFoil->m_FoilDescription.length()) m_pFoilModel->setData(ind, pFoil->m_FoilDescription, Qt::ToolTipRole);

	ind = m_pFoilModel->index(row, 1, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fThickness);

	ind = m_pFoilModel->index(row, 2, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fXThickness);

	ind = m_pFoilModel->index(row, 3, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fCamber);

	ind = m_pFoilModel->index(row, 4, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->m_fXCamber);

	ind = m_pFoilModel->index(row, 5, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->n);


	if(pFoil && pFoil->m_bTEFlap)
	{
		ind = m_pFoilModel->index(row, 6, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEFlapAngle);

		ind = m_pFoilModel->index(row, 7, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEXHinge/100.0);

		ind = m_pFoilModel->index(row, 8, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEYHinge/100.0);

	}
	if(pFoil && pFoil->m_bLEFlap)
	{
		ind = m_pFoilModel->index(row, 9, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEFlapAngle);

		ind = m_pFoilModel->index(row, 10, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEXHinge/100.0);

		ind = m_pFoilModel->index(row, 11, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEYHinge/100.0);
	}
}


void ManageFoilsDlg::OnRename()
{
	if (m_pFoil) 
	{
		QStringList NameList;
		for(int k=0; k<Foil::s_oaFoil.size(); k++)
		{
			Foil *pOldFoil= (Foil*)Foil::s_oaFoil.at(k);
			NameList.append(pOldFoil->m_FoilName);
		}

		RenameDlg renDlg(this);
		renDlg.InitDialog(&NameList, m_pFoil->foilName(), tr("Enter the foil's new name"));

		if(renDlg.exec()!=QDialog::Rejected)
		{
			m_pFoil->renameThisFoil(renDlg.newName());
			FillFoilTable();
			m_bChanged = true;
		}
	}
}


void ManageFoilsDlg::OnExport()
{
	if(!m_pFoil)	return;

	QString FileName;

	FileName = m_pFoil->m_FoilName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Export Foil"),
											Settings::s_LastDirName+"/"+FileName+".dat",
											tr("Foil File (*.dat)"));

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	m_pFoil->ExportFoil(out);
	XFile.close();
}


void ManageFoilsDlg::OnDelete()
{
	if(!m_pFoil) return;

	Foil::deleteFoil(m_pFoil);

	QModelIndex index = m_pctrlFoilTable->currentIndex();
	int sel = qMax(index.row()-1,0);

	FillFoilTable();

	if(m_pFoilModel->rowCount()>0)
	{
		m_pctrlFoilTable->selectRow(sel);

		QStandardItem *pItem = m_pFoilModel->item(sel,0);
		QString FoilName = pItem->text();

		m_pFoil = Foil::foil(FoilName);
	}
	else m_pFoil = NULL;

	m_bChanged = true;
}



void ManageFoilsDlg::OnDoubleClickTable(const QModelIndex &index)
{
	if(index.row()>=0) accept();
}


void ManageFoilsDlg::OnFoilClicked(const QModelIndex& index)
{
	if(index.row()>=Foil::s_oaFoil.size()) return;

	QStandardItem *pItem = m_pFoilModel->item(index.row(),0);
	QString FoilName =pItem->text();

	m_pFoil= Foil::foil(FoilName);
}


void ManageFoilsDlg::resizeEvent(QResizeEvent *event)
{
	int w = m_pctrlFoilTable->width();
	int w12 = (int)((double)w/13.0);
	int w14 = (int)((double)w/15.0);

	m_pctrlFoilTable->setColumnWidth(1,w12);
	m_pctrlFoilTable->setColumnWidth(2,w12);
	m_pctrlFoilTable->setColumnWidth(3,w12);
	m_pctrlFoilTable->setColumnWidth(4,w12);
	m_pctrlFoilTable->setColumnWidth(5,w14);//points

	m_pctrlFoilTable->setColumnWidth(6,w14);//TE Flap
	m_pctrlFoilTable->setColumnWidth(7,w12);//TE XHinge
	m_pctrlFoilTable->setColumnWidth(8,w12);//TE YHinge

	m_pctrlFoilTable->setColumnWidth(9, w14);//LE Flap
	m_pctrlFoilTable->setColumnWidth(10,w12);//LE XHinge
	m_pctrlFoilTable->setColumnWidth(11,w12);//LE YHinge

	m_pctrlFoilTable->setColumnWidth(0,w-8*w12-3*w14-40);
	event->accept();
}





