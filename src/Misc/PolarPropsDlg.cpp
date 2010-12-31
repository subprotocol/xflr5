/****************************************************************************

	PolarPropsDlg Class
	Copyright (C) 2010 Andre Deperrois XFLR5@yahoo.com

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

#include "PolarPropsDlg.h"
#include "PolarPropsDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QtDebug>

#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"
#include "../MainFrame.h"
#include "../Globals.h"

void * PolarPropsDlg::s_pMainFrame;

PolarPropsDlg::PolarPropsDlg()
{
	setWindowTitle(tr("Object Properties"));
	m_pMiarex     = NULL;
	m_pXDirect    = NULL;
	m_pPolar      = NULL;
	m_pWPolar     = NULL;
	m_pWOpp       = NULL;
	m_pOpp        = NULL;

	SetupLayout();
}


void PolarPropsDlg::SetupLayout()
{
	setMinimumHeight(400);
	setMinimumWidth(450);

	m_pctrlDescription = new QTextEdit;
	m_pctrlDescription->setFontFamily("Courier");
	m_pctrlDescription->setReadOnly(true);
	m_pctrlDescription->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlDescription->setWordWrapMode(QTextOption::NoWrap);

//	QVBoxLayout *PropertiesLayout = new QVBoxLayout;
//	PropertiesLayout->addWidget(lab);
//	PropertiesLayout->addWidget(m_pctrlDescription);

	QPushButton *OKButton = new QPushButton(tr("OK"));
	QHBoxLayout *CommandButtons = new QHBoxLayout;
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addWidget(m_pctrlDescription);
//	MainLayout->addStretch(1);
	MainLayout->addSpacing(20);
	MainLayout->addLayout(CommandButtons);
//	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void PolarPropsDlg::InitDialog()
{
	QString strange;

	if(m_pXDirect)
	{
		if(m_pPolar)     m_pPolar->GetPolarProperties(strange);
		else if(m_pOpp)  m_pOpp->GetOppProperties(strange);
	}
	else if(m_pMiarex)
	{
		if(m_pWPolar)    m_pWPolar->GetPolarProperties(strange);
		else if(m_pWOpp) m_pWOpp->GetWOppProperties(strange);
	}

	m_pctrlDescription->setText(strange);
}








