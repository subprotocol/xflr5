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
#include "../Misc/ModDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include "GL3dBodyDlg.h"
#include <QMessageBox>


ManageBodiesDlg::ManageBodiesDlg()
{
	m_pGL3dBodyDlg = NULL;
	m_pMiarex = NULL;
	m_pMainFrame = NULL;
	m_pBody = NULL;
	setWindowTitle(tr("Body Management"));
	SetupLayout();
}




void ManageBodiesDlg::InitDialog()
{
	UpdateBodyList();
	SetBody();
}


bool ManageBodiesDlg::IsInUse(CBody *pBody)
{
	int i;
	CPlane *pOldPlane;
	for (i=0; i<m_poaPlane->count();i++)
	{
		pOldPlane = (CPlane*)m_poaPlane->at(i);
		if(pOldPlane->m_bBody && pOldPlane->m_pBody == m_pBody)
		{
			return true;
		}
	}
	return false;
}


void ManageBodiesDlg::OnDelete()
{
	if(!m_pBody) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	QString strong;
	int i;
	CPlane *pOldPlane;

	if(IsInUse(m_pBody))
	{
		strong = tr("The body ") +  m_pBody->m_BodyName + tr(" is in use by a plane.\n Delete Anyhow?\n");
		if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
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
	else
	{
		strong = tr("Are you sure you want to delete the body :\n") +  m_pBody->m_BodyName +"?\n";
		if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;
	}

	pMiarex->DeleteBody(m_pBody);
	m_pBody = NULL;

	UpdateBodyList();
	SetBody();
}



void ManageBodiesDlg::OnDoubleClickTable(const QModelIndex &index)
{
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	if(index.row()>=0)
	{
		QListWidgetItem* pItem =  m_pctrlNameList->item(index.row());

		CBody *pBody = pMiarex->GetBody(pItem->text());
		if(pBody)
		{
			SetBody(pBody);
			OnEdit();
		}
	}
}


void ManageBodiesDlg::OnDescriptionChanged()
{
	if(m_pBody) m_pBody->m_BodyDescription = m_pctrlDescription->toPlainText();
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
}


void ManageBodiesDlg::OnDuplicate()
{
	if(!m_pBody) return;


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
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;
	GL3dBodyDlg *pGL3dBodyDlg = (GL3dBodyDlg*)m_pGL3dBodyDlg;

	bool bUsed = false;
	int i;
	CPlane *pPlane;
	CWPolar *pWPolar;
	for (i=0; i< pMiarex->m_poaPlane->size(); i++)
	{
		pPlane = (CPlane*)m_poaPlane->at(i);
		if(pPlane->m_bBody && pPlane->m_pBody==m_pBody)
		{
			// Does this plane have results
			for(int j=0; j<pMiarex->m_poaWPolar->size(); j++)
			{
				pWPolar = (CWPolar*)pMiarex->m_poaWPolar->at(j);
				if(pWPolar->m_UFOName==pPlane->m_PlaneName && pWPolar->m_Alpha.size())
				{
					bUsed = true;
					break;
				}
			}
			if(bUsed) break;
		}
	}

	CBody memBody;
	memBody.Duplicate(m_pBody);
	pGL3dBodyDlg->SetBody(m_pBody);
	pGL3dBodyDlg->m_bEnableName = false;
	pGL3dBodyDlg->InitDialog();

	if(pGL3dBodyDlg->exec() == QDialog::Accepted)
	{
		if(bUsed)
		{
			ModDlg dlg;
			dlg.m_Question = tr("The modification will erase all results for the planes using this body.\nContinue ?");
			dlg.InitDialog();
			int Ans = dlg.exec();
			if (Ans == QDialog::Rejected)
			{
				//restore geometry
				m_pBody->Duplicate(&memBody);
				return;
			}
			else if(Ans==20)
			{
				CBody* pNewBody= new CBody();
				pNewBody->Duplicate(m_pBody);
				m_pBody->Duplicate(&memBody);
				if(!pMiarex->SetModBody(pNewBody))
				{
					delete pNewBody;
				}

				return;
			}
			else
			{
				//delete all results associated to planes using this body
				for (i=0; i<pMiarex->m_poaPlane->count();i++)
				{
					pPlane = (CPlane*)pMiarex->m_poaPlane->at(i);
					if(pPlane->m_pBody == m_pBody)
					{
						pMainFrame->DeletePlane(pPlane, true);
					}
				}
			}
		}

		if(m_pBody == pMiarex->m_pCurBody)
		{
			pMiarex->SetUFO();

			if(pMiarex->m_iView==WPOLARVIEW)    pMiarex->CreateWPolarCurves();
			else if(pMiarex->m_iView==WOPPVIEW)	pMiarex->CreateWOppCurves();
			else if(pMiarex->m_iView==WCPVIEW)	pMiarex->CreateCpCurves();

			pMiarex->m_bResetglBody     = true;
			pMiarex->m_bResetglBodyMesh = true;
			pMiarex->m_bResetglGeom     = true;
			pMiarex->m_bResetglMesh     = true;
			pMainFrame->UpdateWOpps();
			pMainFrame->SetSaveState(false);
			pMiarex->m_bIs2DScaleSet = false;
			pMiarex->SetScale();
			pMiarex->UpdateView();
		}
	}
	else m_pBody->Duplicate(&memBody);
}




void ManageBodiesDlg::OnExportDefinition()
{
	if(!m_pBody) return;
	m_pBody->ExportDefinition();
}


void ManageBodiesDlg::OnExportGeometry()
{
	if(!m_pBody) return;
	GL3dBodyDlg *pGL3dBodyDlg = (GL3dBodyDlg*)m_pGL3dBodyDlg;
	m_pBody->ExportGeometry(pGL3dBodyDlg->m_NXPoints, pGL3dBodyDlg->m_NHoopPoints);
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
				m_pctrlDescription->setPlainText(m_pBody->m_BodyDescription);
				break;
			}
		}
	}
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
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

	pMiarex->SetModBody(m_pBody);
	UpdateBodyList();
	SetBody();
}




void ManageBodiesDlg::SetBody(CBody *pBody)
{
	int i;
	QListWidgetItem *pItem ;
	QMiarex * pMiarex = (QMiarex*)m_pMiarex;

//	if(!pBody) pBody = m_pBody;
	if(pBody)
	{
		m_pBody = pBody;
		for (i=0; i<m_pctrlNameList->count(); i++)
		{
			pItem =  m_pctrlNameList->item(i);
			if(pItem->text() == pBody->m_BodyName) break;
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
	if(m_pBody) m_pctrlDescription->setText(m_pBody->m_BodyDescription);
	else        m_pctrlDescription->clear();
}



void ManageBodiesDlg::SetupLayout()
{
	m_pctrlNew         = new QPushButton(tr("New"));
	m_pctrlEdit        = new QPushButton(tr("Edit"));
	m_pctrlRename      = new QPushButton(tr("Rename"));
	m_pctrlDelete      = new QPushButton(tr("Delete"));
	m_pctrlDuplicate   = new QPushButton(tr("Duplicate"));
	m_pctrlExportDef   = new QPushButton(tr("Export Definition"));
	m_pctrlExportGeom  = new QPushButton(tr("Export Geometry"));
	QPushButton *CloseButton   = new QPushButton(tr("Close"));
	QVBoxLayout *ButtonsLayout = new QVBoxLayout;

	QVBoxLayout *LeftLayout = new QVBoxLayout;
	m_pctrlNameList    = new QListWidget;
	m_pctrlDescription = new QTextEdit;
	QLabel *Description = new QLabel(tr("Description:"));
	LeftLayout->addWidget(m_pctrlNameList);
	LeftLayout->addWidget(Description);
	LeftLayout->addWidget(m_pctrlDescription);
	LeftLayout->setStretchFactor(m_pctrlNameList, 5);
	LeftLayout->setStretchFactor(m_pctrlDescription, 1);

	ButtonsLayout->addWidget(m_pctrlNew);
	ButtonsLayout->addWidget(m_pctrlEdit);
	ButtonsLayout->addWidget(m_pctrlRename);
	ButtonsLayout->addWidget(m_pctrlDelete);
	ButtonsLayout->addWidget(m_pctrlDuplicate);
	ButtonsLayout->addWidget(m_pctrlExportDef);
	ButtonsLayout->addWidget(m_pctrlExportGeom);
	ButtonsLayout->addStretch(1);
	ButtonsLayout->addWidget(CloseButton);
	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addLayout(LeftLayout);
	MainLayout->addLayout(ButtonsLayout);
	setLayout(MainLayout);

	connect(m_pctrlNameList,   SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(OnNameList(QListWidgetItem *)));
	connect(m_pctrlNameList,   SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnDoubleClickTable(const QModelIndex &)));
	connect(m_pctrlNew,        SIGNAL(clicked()), this, SLOT(OnNew()));
	connect(m_pctrlEdit,       SIGNAL(clicked()), this, SLOT(OnEdit()));
	connect(m_pctrlRename,     SIGNAL(clicked()), this, SLOT(OnRename()));
	connect(m_pctrlDelete,     SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(m_pctrlDuplicate,  SIGNAL(clicked()), this, SLOT(OnDuplicate()));
	connect(m_pctrlExportDef,  SIGNAL(clicked()), this, SLOT(OnExportDefinition()));
	connect(m_pctrlExportGeom, SIGNAL(clicked()), this, SLOT(OnExportGeometry()));
	connect(m_pctrlDescription, SIGNAL(textChanged()), this, SLOT(OnDescriptionChanged()));
	connect(CloseButton,       SIGNAL(clicked()), this, SLOT(accept()));
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










