/****************************************************************************

	TwoDPanelDlg Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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

#include "TwoDPanelDlg.h"
#include "../Params.h"
#include "XFoil.h"
#include "XDirect.h"
#include "../Design/AFoil.h"

void *TwoDPanelDlg::s_pXFoil;

TwoDPanelDlg::TwoDPanelDlg()
{
	SetupLayout();
	m_pXDirect    = NULL;
	m_pBufferFoil = NULL;
	m_bApplied    = false;
}

void TwoDPanelDlg::SetupLayout()
{
	setWindowTitle("Global Panel Refinement");
	QGridLayout *InputData = new QGridLayout;
	QLabel *l1 = new QLabel("Number of Panels");
	QLabel *l2 = new QLabel("Panel Bunching Parameter");
	QLabel *l3 = new QLabel("TE/LE Panel Density Ratio");
	QLabel *l4 = new QLabel("Refined area/LE Panel Density Ratio");
	QLabel *l5 = new QLabel("Top Side Refined Area x/c limits");
	QLabel *l6 = new QLabel("Bottom Side Refined Area x/c limits");

	InputData->addWidget(l1,1,1);
	InputData->addWidget(l2,2,1);
	InputData->addWidget(l3,3,1);
	InputData->addWidget(l4,4,1);
	InputData->addWidget(l5,5,1);
	InputData->addWidget(l6,6,1);


	m_pctrlNPanels = new QLineEdit("100");
	m_pctrlNPanels->setAlignment(Qt::AlignRight);
	QValidator *PanelValid = new QIntValidator(0, 1000000, this);
	m_pctrlNPanels->setValidator(PanelValid);
	m_pctrlCVpar  = new FloatEdit;
	m_pctrlCTErat = new FloatEdit;
	m_pctrlCTRrat = new FloatEdit;
	m_pctrlXsRef1 = new FloatEdit;
	m_pctrlXsRef2 = new FloatEdit;
	m_pctrlXpRef1 = new FloatEdit;
	m_pctrlXpRef2 = new FloatEdit;

	InputData->addWidget(m_pctrlNPanels, 1, 2);
	InputData->addWidget(m_pctrlCVpar,         2, 2);
	InputData->addWidget(m_pctrlCTErat,        3, 2);
	InputData->addWidget(m_pctrlCTRrat,        4, 2);
	InputData->addWidget(m_pctrlXsRef1,        5, 2);
	InputData->addWidget(m_pctrlXsRef2,        5, 3);
	InputData->addWidget(m_pctrlXpRef1,        6, 2);
	InputData->addWidget(m_pctrlXpRef2,        6, 3);

	connect(m_pctrlNPanels, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlCVpar, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlCTErat, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlCTRrat, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXsRef1, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXsRef2, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXpRef1, SIGNAL(editingFinished()), this, SLOT(OnChanged()));
	connect(m_pctrlXpRef2, SIGNAL(editingFinished()), this, SLOT(OnChanged()));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton      = new QPushButton(tr("OK"));
	CancelButton  = new QPushButton(tr("Cancel"));
	ApplyButton   = new QPushButton("Apply");
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(ApplyButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addLayout(InputData);
	mainLayout->addStretch(1);
	mainLayout->addLayout(CommandButtons);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

	connect(ApplyButton, SIGNAL(clicked()),this, SLOT(OnApply()));
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));


	setMinimumHeight(250);
}

void TwoDPanelDlg::InitDialog()
{

	XFoil *pXFoil =(XFoil*)s_pXFoil;
	CFoil *pMemFoil = (CFoil *)m_pMemFoil;
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

	QString strong = QString("%1").arg(npan);
	m_pctrlNPanels->setText(strong);
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
	m_bApplied = false;
}


void TwoDPanelDlg::OnApply()
{
	if(m_bApplied) return;

	//reset everything and retry
	XFoil *pXFoil = (XFoil*)s_pXFoil;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QAFoil *pAFoil = (QAFoil*)m_pAFoil;

	CFoil *pMemFoil = (CFoil*)m_pMemFoil;
	CFoil *pBufferFoil = (CFoil*)m_pBufferFoil;

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
		QMessageBox::information(this, "Warning", "Unrecognized foil format");
		return;
	}

	ReadParams();

	pXFoil->pangen();

	if(pXFoil->n>IQX)
	{
		QMessageBox::information(this, "Warning", "Panel number cannot exceed 350");
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
	if(pXDirect)    pXDirect->UpdateView();
	else if(pAFoil) pAFoil->UpdateView();
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

	pXFoil->npan   = m_pctrlNPanels->text().toInt();
	pXFoil->cvpar  = m_pctrlCVpar->GetValue();
	pXFoil->cterat = m_pctrlCTErat->GetValue();
	pXFoil->ctrrat = m_pctrlCTRrat->GetValue();
	pXFoil->xsref1 = m_pctrlXsRef1->GetValue();
	pXFoil->xsref2 = m_pctrlXsRef2->GetValue();
	pXFoil->xpref1 = m_pctrlXpRef1->GetValue();
	pXFoil->xpref2 = m_pctrlXpRef2->GetValue();

}











