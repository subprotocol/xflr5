/****************************************************************************

	LEDlg Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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

#include "LEDlg.h"
#include "XFoil.h"
#include "XDirect.h"
#include "../design/AFoil.h"
#include <QMessageBox>
#include <QHBoxLayout>


void *LEDlg::s_pXFoil;

LEDlg::LEDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Leading Edge"));
	m_LErfac    = 1.0;
	m_Blend     = 0.1;
	m_pAFoil    = NULL;
	m_pXDirect  = NULL;
	m_bModified = false;
	m_bApplied  = true;

	SetupLayout();

	connect(m_pctrlLE, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlBlend, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

void LEDlg::SetupLayout()
{
	QHBoxLayout *LEValue = new QHBoxLayout;
	QLabel *lab1 = new QLabel(tr("Approximate new/old ratio for L.E. radius"));
	lab1->setMinimumWidth(200);
	lab1->setAlignment(Qt::AlignRight);
	QLabel *lab2 = new QLabel(tr("ratio"));
	lab2->setMinimumWidth(80);
	m_pctrlLE = new DoubleEdit;
	LEValue->addWidget(lab1);
	LEValue->addWidget(m_pctrlLE);
	LEValue->addWidget(lab2);

	QHBoxLayout *BlendValue = new QHBoxLayout;
	QLabel *lab3 = new QLabel(tr("Blending Distance from L.E."));
	lab3->setMinimumWidth(200);
	lab3->setAlignment(Qt::AlignRight);
	QLabel *lab4 = new QLabel(tr("% chord"));
	lab4->setMinimumWidth(80);
	m_pctrlBlend = new DoubleEdit;
	BlendValue->addWidget(lab3);
	BlendValue->addWidget(m_pctrlBlend);
	BlendValue->addWidget(lab4);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton     = new QPushButton(tr("OK"));
	CancelButton = new QPushButton(tr("Cancel"));
	ApplyButton  = new QPushButton(tr("Apply"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(LEValue);
	MainLayout->addStretch(1);
	MainLayout->addLayout(BlendValue);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void LEDlg::keyPressEvent(QKeyEvent *event)
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
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OnApply();
				OKButton->setFocus();
				m_bApplied  = true;
			}
			else
			{
				QDialog::accept();
			}
			break;
		}
		default:
			event->ignore();
			break;
	}
}


void LEDlg::InitDialog()
{
	m_pctrlLE->SetMin(  0.0);
	m_pctrlLE->SetMax(100.0);

	m_pctrlBlend->SetMin(  0.001);
	m_pctrlBlend->SetMax(100.0);

	m_pctrlLE->SetValue(m_LErfac);
	m_pctrlBlend->SetValue(m_Blend*100.0);
}


void LEDlg::OnChanged()
{
	m_bApplied = false;
}


void LEDlg::OnApply()
{
	if(m_bApplied) return;

	//reset everything and retry
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	XFoil *pXFoil = (XFoil*)s_pXFoil;

	int i, j;

	for (i=0; i< m_pMemFoil->nb; i++)
	{
		pXFoil->xb[i+1] = m_pMemFoil->xb[i] ;
		pXFoil->yb[i+1] = m_pMemFoil->yb[i];
	}
	pXFoil->nb = m_pMemFoil->nb;

	pXFoil->lflap = false;
	pXFoil->lbflap = false;

	if(pXFoil->Preprocess())
	{
		pXFoil->CheckAngles();
/*		for (int k=0; k<pXFoil->n;k++)
		{
			m_pMemFoil->nx[k] = pXFoil->nx[k+1];
			m_pMemFoil->ny[k] = pXFoil->ny[k+1];
		}
		m_pMemFoil->n = pXFoil->n;*/
	}
	else
	{
		QMessageBox::information(window(), tr("Warning"), tr("Unrecognized foil format"));
		return;
	}

	m_LErfac = m_pctrlLE->Value();
	m_Blend = m_pctrlBlend->Value()/100.0;

	pXFoil->lerad(m_LErfac,m_Blend);

	if(pXFoil->n>IQX)
	{
		QMessageBox::information(window(), tr("Warning"), tr("Panel number cannot exceed 300"));
		//reset everything and retry
		for (i=0; i< m_pMemFoil->nb; i++)
		{
			pXFoil->x[i+1] = m_pMemFoil->xb[i] ;
			pXFoil->y[i+1] = m_pMemFoil->yb[i];
		}
		pXFoil->n = m_pMemFoil->nb;
	}
	else
	{
		for (j=0; j< pXFoil->n; j++)
		{
			m_pBufferFoil->xb[j] = pXFoil->xb[j+1];
			m_pBufferFoil->yb[j] = pXFoil->yb[j+1];
		}
		m_pBufferFoil->nb = pXFoil->nb;
//		pXFoil->SetFoilFlap(m_pBufferFoil);
		m_pBufferFoil->InitFoil();
		m_pBufferFoil->SetFlap();
	}
	m_bApplied = true;
	m_bModified = true;

	if(pXDirect) pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();

}

void LEDlg::OnOK()
{
	if(!m_bApplied)	OnApply();
	if(!m_bModified) done(0);
	else done(1);
}
