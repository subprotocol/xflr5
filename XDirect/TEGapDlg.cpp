/****************************************************************************

	TEGapDlg Class
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

#include <QtGui>
#include "TEGapDlg.h"
#include "XFoil.h"
#include "XDirect.h"


TEGapDlg::TEGapDlg(void *pParent)
{
	m_Gap   = 0.0;
	m_Blend = 0.8;
	m_pXDirect = pParent;
	m_bModified = false;
	m_bApplied  = true;

	SetupLayout();

	connect(m_pctrlGap, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlBlend, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

void TEGapDlg::SetupLayout()
{
	QHBoxLayout *GapValue = new QHBoxLayout;
	QLabel *lab1 = new QLabel("T.E. Gap Value");
	lab1->setAlignment(Qt::AlignRight);
	lab1->setMinimumWidth(150);
	QLabel *lab2 = new QLabel("% chord");
	m_pctrlGap = new FloatEdit;
	m_pctrlGap->setMaximumWidth(70);
	GapValue->addWidget(lab1);
	GapValue->addWidget(m_pctrlGap);
	GapValue->addWidget(lab2);

	QHBoxLayout *BlendValue = new QHBoxLayout;
	QLabel *lab3 = new QLabel("Blending Distance from L.E.");
	lab3->setAlignment(Qt::AlignRight);
	lab3->setMinimumWidth(150);
	QLabel *lab4 = new QLabel("% chord");
	m_pctrlBlend = new FloatEdit;
	m_pctrlBlend->setMaximumWidth(70);
	BlendValue->addWidget(lab3);
	BlendValue->addWidget(m_pctrlBlend);
	BlendValue->addWidget(lab4);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton      = new QPushButton(tr("OK"));
	CancelButton  = new QPushButton(tr("Cancel"));
	ApplyButton  = new QPushButton("Apply");
	OKButton->setMaximumWidth(70);
	CancelButton->setMaximumWidth(70);
	ApplyButton->setMaximumWidth(70);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(GapValue);
	MainLayout->addStretch(1);
	MainLayout->addLayout(BlendValue);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}

void TEGapDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			done(0);
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
		QDialog::keyPressEvent(event);
	}

	QDialog::keyPressEvent(event);
}

void TEGapDlg::InitDialog()
{
	m_pctrlGap->SetMin(  0.0);
	m_pctrlGap->SetMax(100.0);

	m_pctrlBlend->SetMin(  0.0);
	m_pctrlBlend->SetMax(100.0);

	m_pctrlGap->SetValue(m_pMemFoil->m_Gap*100.0);
	m_pctrlBlend->SetValue(m_Blend*100.0);

}

void TEGapDlg::OnChanged()
{
	m_bApplied = false;
}

void TEGapDlg::OnOK()
{
	if(!m_bApplied)	OnApply();
	if(!m_bModified) done(0);
	else done(1);
}

void TEGapDlg::OnApply()
{
	if(m_bApplied) return;
	//reset everything and retry
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	XFoil *pXFoil = (XFoil*)m_pXFoil;
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
		QMessageBox::information(window(), "QFLR5", "Unrecognized foil format");
		return;
	}

	m_pctrlGap->GetValue(m_Gap);
	m_pctrlBlend->GetValue(m_Blend);

	pXFoil->tgap(m_Gap/100.0,m_Blend/100.0);
	if(pXFoil->n>IQX)
	{
		QMessageBox::information(window(), "QFLR5", "Panel number cannot exceed 300");
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
	pXDirect->UpdateView();

}
