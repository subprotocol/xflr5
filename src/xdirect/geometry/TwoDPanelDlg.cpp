/****************************************************************************

	TwoDPanelDlg Class
	Copyright (C) 2008-2008 Andre Deperrois adeperrois@xflr5.com

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

#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>

#include "TwoDPanelDlg.h"
#include "../../params.h"
#include "../../objects/Foil.h"
#include "../analysis/XFoil.h"

void *TwoDPanelDlg::s_pXFoil;


TwoDPanelDlg::TwoDPanelDlg(QWidget *pParent) : QDialog(pParent)
{
	SetupLayout();

	m_pParent = pParent;

	m_pBufferFoil = NULL;
	m_bApplied    = false;
}



void TwoDPanelDlg::SetupLayout()
{
	setWindowTitle(tr("Global Panel Refinement"));
	QGridLayout *pInputDataLayout = new QGridLayout;
	{
		QLabel *l1 = new QLabel(tr("Number of Panels"));
		QLabel *l2 = new QLabel(tr("Panel Bunching Parameter"));
		QLabel *l3 = new QLabel(tr("TE/LE Panel Density Ratio"));
		QLabel *l4 = new QLabel(tr("Refined area/LE Panel Density Ratio"));
		QLabel *l5 = new QLabel(tr("Top Side Refined Area x/c limits"));
		QLabel *l6 = new QLabel(tr("Bottom Side Refined Area x/c limits"));

		pInputDataLayout->addWidget(l1,1,1);
		pInputDataLayout->addWidget(l2,2,1);
		pInputDataLayout->addWidget(l3,3,1);
		pInputDataLayout->addWidget(l4,4,1);
		pInputDataLayout->addWidget(l5,5,1);
		pInputDataLayout->addWidget(l6,6,1);


		m_pctrlNPanels = new IntEdit(100, this);
		m_pctrlNPanels->SetMax(IQX);

		m_pctrlCVpar  = new DoubleEdit;
		m_pctrlCTErat = new DoubleEdit;
		m_pctrlCTRrat = new DoubleEdit;
		m_pctrlXsRef1 = new DoubleEdit;
		m_pctrlXsRef2 = new DoubleEdit;
		m_pctrlXpRef1 = new DoubleEdit;
		m_pctrlXpRef2 = new DoubleEdit;

		pInputDataLayout->addWidget(m_pctrlNPanels, 1, 2);
		pInputDataLayout->addWidget(m_pctrlCVpar,   2, 2);
		pInputDataLayout->addWidget(m_pctrlCTErat,  3, 2);
		pInputDataLayout->addWidget(m_pctrlCTRrat,  4, 2);
		pInputDataLayout->addWidget(m_pctrlXsRef1,  5, 2);
		pInputDataLayout->addWidget(m_pctrlXsRef2,  5, 3);
		pInputDataLayout->addWidget(m_pctrlXpRef1,  6, 2);
		pInputDataLayout->addWidget(m_pctrlXpRef2,  6, 3);

		connect(m_pctrlNPanels, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlCVpar,   SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlCTErat,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlCTRrat,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlXsRef1,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlXsRef2,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlXpRef1,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
		connect(m_pctrlXpRef2,  SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	}

	QHBoxLayout *pCommandButtonsLayout = new QHBoxLayout;
	{
		OKButton      = new QPushButton(tr("OK"));
		CancelButton  = new QPushButton(tr("Cancel"));
		ApplyButton   = new QPushButton(tr("Apply"));
		pCommandButtonsLayout->addStretch(1);
		pCommandButtonsLayout->addWidget(ApplyButton);
		pCommandButtonsLayout->addStretch(1);
		pCommandButtonsLayout->addWidget(OKButton);
		pCommandButtonsLayout->addStretch(1);
		pCommandButtonsLayout->addWidget(CancelButton);
		pCommandButtonsLayout->addStretch(1);
	}

	QVBoxLayout *pmainLayout = new QVBoxLayout;
	{
		pmainLayout->addStretch(1);
		pmainLayout->addLayout(pInputDataLayout);
		pmainLayout->addStretch(1);
		pmainLayout->addLayout(pCommandButtonsLayout);
		pmainLayout->addStretch(1);
		setLayout(pmainLayout);
	}

	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setMinimumHeight(250);
}


void TwoDPanelDlg::InitDialog()
{
	XFoil *pXFoil =(XFoil*)s_pXFoil;
	Foil *pMemFoil = (Foil *)m_pMemFoil;
	//memorize initial values
//	npan   = pXFoil->n;
	npan   = pMemFoil->nb;
	cvpar  = pXFoil->cvpar;
	cterat = pXFoil->cterat;
	ctrrat = pXFoil->ctrrat;
	xsref1 = pXFoil->xsref1;
	xsref2 = pXFoil->xsref2;
	xpref1 = pXFoil->xpref1;
	xpref2 = pXFoil->xpref2;

	m_pctrlNPanels->SetValue(npan);
	m_pctrlCVpar->SetValue(cvpar);
	m_pctrlCTErat->SetValue(cterat);
	m_pctrlCTRrat->SetValue(ctrrat);
	m_pctrlXsRef1->SetValue(xsref1);
	m_pctrlXsRef2->SetValue(xsref2);
	m_pctrlXpRef1->SetValue(xpref1);
	m_pctrlXpRef2->SetValue(xpref2);
	m_pctrlNPanels->setFocus();
}


void TwoDPanelDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			done(0);
			return;
		}
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OnApply();
				OKButton->setFocus();
				m_bApplied  = true;
			}
			else
			{
				event->ignore();
			}
			break;
		}
		default:
			event->ignore();
			break;
	}
}



void TwoDPanelDlg::OnChanged()
{
	m_bApplied  = false;
	OnApply();
}



void TwoDPanelDlg::OnApply()
{
	if(m_bApplied) return;

	//reset everything and retry
	XFoil *pXFoil = (XFoil*)s_pXFoil;

	Foil *pMemFoil = (Foil*)m_pMemFoil;
	Foil *pBufferFoil = (Foil*)m_pBufferFoil;

	for (int i=0; i< pMemFoil->nb; i++)
	{
		pXFoil->xb[i+1] = pMemFoil->xb[i] ;
		pXFoil->yb[i+1] = pMemFoil->yb[i];
	}
	pXFoil->nb = pMemFoil->nb;

	pXFoil->lflap = false;
	pXFoil->lbflap = false;

	if(pXFoil->Preprocess())
	{
		pXFoil->CheckAngles();
	}
	else
	{
		QMessageBox::information(this, tr("Warning"), tr("Unrecognized foil format"));
		return;
	}

	ReadParams();

	pXFoil->pangen();

	if(pXFoil->n>IQX)
	{
		QString strange = QString(tr("The total number of panels cannot exceed %1")).arg(IQX);
		QMessageBox::information(this, tr("Warning"), strange);
		//reset everything and retry
		for (int i=0; i< pMemFoil->nb; i++)
		{
			pXFoil->x[i+1] = pMemFoil->xb[i] ;
			pXFoil->y[i+1] = pMemFoil->yb[i];
		}
		pXFoil->n = pMemFoil->nb;
	}
	else
	{
		for (int j=0; j< pXFoil->n; j++)
		{
			pBufferFoil->xb[j] = pXFoil->x[j+1];
			pBufferFoil->yb[j] = pXFoil->y[j+1];
		}
		pBufferFoil->nb = pXFoil->n;
//		pXFoil->SetFoilFlap(pBufferFoil);
		pBufferFoil->InitFoil();
		pBufferFoil->SetFlap();
	}
	m_bApplied = true;
	m_bModified = true;

	m_pParent->update();
}


void TwoDPanelDlg::OnOK()
{
	if(!m_bModified)
	{
		done(0);
	}
	else
	{
		OnApply();
		done(1);
	}
}


void TwoDPanelDlg::ReadParams()
{
	XFoil *pXFoil = (XFoil*)s_pXFoil;

	pXFoil->npan   = m_pctrlNPanels->Value();
	pXFoil->cvpar  = m_pctrlCVpar->Value();
	pXFoil->cterat = m_pctrlCTErat->Value();
	pXFoil->ctrrat = m_pctrlCTRrat->Value();
	pXFoil->xsref1 = m_pctrlXsRef1->Value();
	pXFoil->xsref2 = m_pctrlXsRef2->Value();
	pXFoil->xpref1 = m_pctrlXpRef1->Value();
	pXFoil->xpref2 = m_pctrlXpRef2->Value();
}







