/****************************************************************************

	ManageCurvesDlg  Classes
	Copyright (C) 2010 Andre Deperrois xflr5@yahoo.com

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

/*
	This dialog is used to manage the curves in the time graphs in Stability Analysis
*/

#include "../MainFrame.h"
#include "Miarex.h"
#include "ManageCurvesDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include "GL3dBodyDlg.h"
#include <QMessageBox>
#include <QtDebug>

ManageCurvesDlg::ManageCurvesDlg()
{
	setWindowTitle("Time Curves");
	for(int i=0; i<10; i++)	m_pGraph[i] = NULL;
	m_NGraph = 0;
	SetupLayout();
}


void ManageCurvesDlg::InitDialog()
{
	if(!m_pGraph) return;
	
	int width = m_pctrlCurveList->width();
	m_pctrlCurveList->setColumnWidth(0,width);;
	//	fill the table with the curve names
	FillCurveTable();
}


void ManageCurvesDlg::FillCurveTable()
{
	m_pctrlCurveList->setRowCount(m_pGraph[0]->GetCurveCount());
	m_pctrlCurveList->clear();
	QString CurveTitle;
	CCurve *pCurve;
	QTableWidgetItem *pNewItem;
	for(int i=0; i<m_pGraph[0]->GetCurveCount(); i++)
	{
		pCurve = m_pGraph[0]->GetCurve(i);
		if(!pCurve) break;
		pCurve->GetTitle(CurveTitle);
		pNewItem = new QTableWidgetItem(CurveTitle);
		m_pctrlCurveList->setItem(i, 0, pNewItem);	
	}
}


void ManageCurvesDlg::OnOK()
{
	if(!m_pGraph) return;
	QTableWidgetItem *pItem;
	CCurve *pCurve;
	for(int n=0; n<m_NGraph; n++)
	{
		for(int i=0; i<m_pGraph[n]->GetCurveCount(); i++)
		{
			pCurve = m_pGraph[n]->GetCurve(i);
			if(!pCurve) return;
			pItem = m_pctrlCurveList->item(i,0);
			pCurve->SetTitle(pItem->text());
		}	
	}
	accept();
}


void ManageCurvesDlg::OnDelete()
{
	if(!m_pGraph) return;
	QString strong;
	int row = m_pctrlCurveList->currentRow();
	if(row==0)
	{
		strong = tr("The first curve cannot be deleted");
		QMessageBox::warning(this,tr("Warning"),strong);	
		return;
	}
	
	strong = tr("Are you sure you wan to delete the curve ")+ m_pctrlCurveList->item(row,0)->text();
	if (QMessageBox::Yes != QMessageBox::question(this, tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
qDebug() << "Deleting";
	for(int i=0; i<m_NGraph; i++)
	{
		if(m_pGraph[i]) m_pGraph[i]->DeleteCurve(row);
	}
	FillCurveTable();
}


void ManageCurvesDlg::SetupLayout()
{
	m_pctrlDelete      = new QPushButton(tr("Delete"));
	QPushButton *CloseButton   = new QPushButton(tr("Close"));
	QVBoxLayout *ButtonsLayout = new QVBoxLayout;

	QVBoxLayout *LeftLayout = new QVBoxLayout;
	m_pctrlCurveList    = new QTableWidget;
	m_pctrlCurveList->setColumnCount(1);
	m_pctrlCurveList->setRowCount(1);
	m_pctrlCurveList->setSelectionBehavior(QAbstractItemView::SelectRows);
	LeftLayout->addWidget(m_pctrlCurveList);

	ButtonsLayout->addWidget(m_pctrlDelete);

	ButtonsLayout->addStretch(1);
	ButtonsLayout->addWidget(CloseButton);
	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addLayout(LeftLayout);
	MainLayout->addLayout(ButtonsLayout);
	setLayout(MainLayout);

	connect(m_pctrlDelete,     SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(CloseButton,       SIGNAL(clicked()), this, SLOT(OnOK()));
}

