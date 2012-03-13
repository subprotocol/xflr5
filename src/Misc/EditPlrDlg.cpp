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

#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"



EditPlrDlg::EditPlrDlg()
{
	setWindowTitle(tr("Polar Points Edition"));
	m_pMiarex     = NULL;
	m_pXDirect    = NULL;
	m_pPolar      = NULL;
	m_pWPolar     = NULL;

	SetupLayout();
}



void EditPlrDlg::InitDialog()
{
	FillTable();
}


void EditPlrDlg::FillTable()
{
	QString strong;

	m_pctrlAlphaList->clear();
	if(m_pXDirect && m_pPolar)
	{
		if(m_pPolar->m_PolarType!=FIXEDAOAPOLAR)
		{
			for (int i=0; i<m_pPolar->m_Alpha.size(); i++)
			{
				strong = QString("%1").arg(m_pPolar->m_Alpha.at(i),8,'f',3);
				m_pctrlAlphaList->addItem(strong);
			}
		}
		else
		{
			for (int i=0; i<m_pPolar->m_Alpha.size(); i++)
			{
				strong = QString("%1").arg(m_pPolar->m_Re.at(i),8,'f',0);
				m_pctrlAlphaList->addItem(strong);
			}
		}
	}

	else if(m_pMiarex && m_pWPolar)
	{
		if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR)
		{
			for (int i=0; i<m_pWPolar->m_Alpha.size(); i++)
			{
				strong = QString("%1").arg(m_pWPolar->m_Alpha.at(i),8,'f',3);
				m_pctrlAlphaList->addItem(strong);
			}
		}
		else
		{
			for (int i=0; i<m_pWPolar->m_Alpha.size(); i++)
			{
				strong = QString("%1").arg(m_pWPolar->m_QInfinite.at(i),12,'f',3);
				m_pctrlAlphaList->addItem(strong);
			}
		}
	}
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
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	int index = m_pctrlAlphaList->currentRow();
	if(pXDirect)
	{
		m_pPolar->Remove(index);
		FillTable();
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	else if(pMiarex)
	{
		m_pWPolar->Remove(index);
		FillTable();
		pMiarex->CreateWPolarCurves();
		pMiarex->UpdateView();
	}
	if(index>=m_pctrlAlphaList->count()-1)
	{
		index = m_pctrlAlphaList->count()-1;
	}
	if (m_pctrlAlphaList->count()) m_pctrlAlphaList->setCurrentRow(index);
}



void EditPlrDlg::OnDeleteAllPoints()
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex *pMiarex = (QMiarex*)m_pMiarex;

	if(pXDirect)
	{
		m_pPolar->ResetPolar();
		FillTable();
		pXDirect->CreatePolarCurves();
		pXDirect->UpdateView();
	}
	else if(pMiarex)
	{
		m_pWPolar->ResetWPlr();
		FillTable();
		pMiarex->CreateWPolarCurves();
		pMiarex->UpdateView();
	}
}


void EditPlrDlg::SetupLayout()
{
	QVBoxLayout *CommandButtons = new QVBoxLayout;
	m_pctrlDeleteAllPoints= new QPushButton(tr("Delete All Points"));
	m_pctrlDeletePoint	= new QPushButton(tr("Delete Point"));
	OKButton            = new QPushButton(tr("OK"));
	CancelButton        = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(m_pctrlDeleteAllPoints);
	CommandButtons->addWidget(m_pctrlDeletePoint);
	CommandButtons->addStretch(2);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	m_pctrlAlphaList = new QListWidget;

	QHBoxLayout * MainLayout = new QHBoxLayout(this);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlAlphaList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);

	connect(m_pctrlDeletePoint, SIGNAL(clicked()),this, SLOT(OnDeletePoint()));
	connect(m_pctrlDeleteAllPoints, SIGNAL(clicked()),this, SLOT(OnDeleteAllPoints()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


