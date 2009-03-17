/****************************************************************************

	ManageBodiesDlg  Classes
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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

#include "../MainFrame.h"
#include "Miarex.h"
#include "ManageBodiesDlg.h"
#include "../Objects/Plane.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include "GL3dBodyDlg.h"


ManageBodiesDlg::ManageBodiesDlg()
{
	m_pGL3dBodyDlg = NULL;
	m_pMiarex = NULL;
	m_pMainFrame = NULL;
	m_pBody = NULL;
	setWindowTitle("Body Dlg");
	SetupLayout();
}


void ManageBodiesDlg::SetupLayout()
{
	m_pctrlNameList = new QListWidget;
	m_pctrlEdit = new QPushButton("Edit");
	m_pctrlRename = new QPushButton("Rename");
	m_pctrlDelete = new QPushButton("Delete");
	m_pctrlDuplicate = new QPushButton("Duplicate");
	QPushButton * CloseButton = new QPushButton("Close");
	QVBoxLayout *ButtonsLayout = new QVBoxLayout;
	ButtonsLayout->addWidget(m_pctrlEdit);
	ButtonsLayout->addWidget(m_pctrlRename);
	ButtonsLayout->addWidget(m_pctrlDelete);
	ButtonsLayout->addWidget(m_pctrlDuplicate);
	ButtonsLayout->addStretch(1);
	ButtonsLayout->addWidget(CloseButton);
	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addLayout(ButtonsLayout);
	setLayout(MainLayout);

	connect(m_pctrlNameList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(OnNameList(QListWidgetItem *)));
	connect(m_pctrlNew, SIGNAL(clicked()), this, SLOT(OnNew()));
	connect(m_pctrlEdit, SIGNAL(clicked()), this, SLOT(OnEdit()));
	connect(m_pctrlRename, SIGNAL(clicked()), this, SLOT(OnRename()));
	connect(m_pctrlDelete, SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(m_pctrlDuplicate, SIGNAL(clicked()), this, SLOT(OnDuplicate()));
	connect(CloseButton, SIGNAL(clicked()), this, SLOT(accept()));
}


void ManageBodiesDlg::OnNameList(QListWidgetItem *pItem)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	QListWidgetItem *pOldItem ;

	CBody *pBody = pMiarex->GetBody(pItem->text());
	if(pBody)
	{
		for(int i=0; i<m_pctrlNameList->count(); i++)
		{
			pOldItem =  m_pctrlNameList->item(i);
			if(pOldItem->text() == pBody->m_BodyName)
			{
				m_pBody = pBody;
				break;
			}
		}
	}
}



void ManageBodiesDlg::OnDelete()
{
	if(!m_pBody) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	int i;
	QString strong;
	CPlane *pOldPlane;
	bool bIsInUse = false;

	for (i=0; i<m_poaPlane->count();i++)
	{
		pOldPlane = (CPlane*)m_poaPlane->at(i);
		if(pOldPlane->m_bBody && pOldPlane->m_pBody == m_pBody)
		{
			bIsInUse = true;
			break;
		}
	}

	if(bIsInUse)
	{
		strong = "The body " +  m_pBody->m_BodyName +" is in use by a plane.\n Delete Anyhow?\n";
		if (QMessageBox::Yes != QMessageBox::question(window(), "QFLR5", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
	}
	else
	{
		strong = "Are you sure you want to delete the body :\n" +  m_pBody->m_BodyName +"?\n";
		if (QMessageBox::Yes != QMessageBox::question(window(), "QFLR5", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
	}

	if(bIsInUse)
	{
		for (i=0; i<m_poaPlane->count();i++)
		{
			pOldPlane = (CPlane*)m_poaPlane->at(i);
			if(pOldPlane->m_pBody == m_pBody)
			{
				pOldPlane->m_bBody = false;
				pOldPlane->m_pBody = NULL;
				pMainFrame->DeletePlane(pOldPlane, true);
			}
			pMiarex->m_bResetglGeom = true;
		}
	}

	pMiarex->DeleteBody(m_pBody);
	m_pBody = NULL;

	UpdateBodyList();
	SetBody();
}


void ManageBodiesDlg::OnDuplicate()
{
	if(!m_pBody) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	CBody *pBody = new CBody;
	pBody->Duplicate(m_pBody);
	pBody->m_BodyName = m_pBody->m_BodyName;
	pMiarex->SetModBody(pBody);
	UpdateBodyList();
}


void ManageBodiesDlg::OnEdit()
{
	if(!m_pBody) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	GL3dBodyDlg *pGL3dBodyDlg = (GL3dBodyDlg*)m_pGL3dBodyDlg;
	CBody memBody;

	memBody.Duplicate(m_pBody);
	pGL3dBodyDlg->SetBody(m_pBody);
	pGL3dBodyDlg->m_bEnableName = false;

	if(pGL3dBodyDlg->exec() == QDialog::Accepted)
	{
//		SetModBody(pBody);
		pMainFrame->SetSaveState(false);
	}
	else m_pBody->Duplicate(&memBody);
}




void ManageBodiesDlg::OnNew()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	GL3dBodyDlg *pGL3dBodyDlg = (GL3dBodyDlg*)m_pGL3dBodyDlg;
	CBody *pBody = new CBody;

	pGL3dBodyDlg->SetBody(pBody);
	if(pGL3dBodyDlg->exec() == QDialog::Accepted)
	{
		pMiarex->AddBody(pBody);
		pMainFrame->SetSaveState(false);
		UpdateBodyList();
	}
	else delete pBody;
}


void ManageBodiesDlg::OnRename()
{
	if(!m_pBody) return;
	MainFrame *pFrame = (MainFrame*)m_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	pMiarex->SetModBody(m_pBody);
	UpdateBodyList();
	SetBody();
}



void ManageBodiesDlg::InitDialog()
{
	int i=-1;

	UpdateBodyList();
	SetBody();
}


void ManageBodiesDlg::SetBody(CBody *pBody)
{
	int i;
	QListWidgetItem *pItem ;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	if(!pBody) pBody = m_pBody;
	if(pBody)
	{
		for (i=0; i<m_pctrlNameList->count(); i++)
		{
			pItem =  m_pctrlNameList->item(i);
			if(pItem->text() == m_pBody->m_BodyName) break;
		}
		if(i>=0) m_pctrlNameList->setCurrentRow(i);
	}
	else
	{
		if(m_pctrlNameList->count())
		{
			m_pctrlNameList->setCurrentRow(0);
			pItem =  m_pctrlNameList->item(0);
			m_pBody = pMiarex->GetBody(pItem->text());
		}
		else m_pBody = NULL;
	}
}



void ManageBodiesDlg::UpdateBodyList()
{

	CBody * pBody;
	m_pctrlNameList->clear();
	for (int i=0; i<m_poaBody->size(); i++)
	{
		pBody = (CBody*)m_poaBody->at(i);
		m_pctrlNameList->addItem(pBody->m_BodyName);
	}

}










