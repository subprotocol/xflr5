/****************************************************************************

	BodyTransDlg Class
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


#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "BodyTransDlg.h"
#include "../../misc/Units.h"
#include "../../globals.h"


BodyTransDlg::BodyTransDlg(QWidget *pParent): QDialog(pParent)
{
	setWindowTitle(tr("Body Translation"));
	m_XTrans = m_YTrans = m_ZTrans = 0.0;
	m_bFrameOnly = false;
	m_FrameID = 1;

	SetupLayout();
}


void BodyTransDlg::InitDialog()
{
	m_pctrlXTransFactor->SetValue(m_XTrans);
	m_pctrlYTransFactor->SetValue(m_YTrans);
	m_pctrlZTransFactor->SetValue(m_ZTrans);

	m_pctrlYTransFactor->setEnabled(false);

	m_pctrlFrameOnly->setChecked(m_bFrameOnly);
	m_pctrlFrameID->SetValue(m_FrameID+1);
	m_pctrlFrameID->setEnabled(m_bFrameOnly);

	QString length;
	Units::getLengthUnitLabel(length);
	m_pctrlLength1->setText(length);
	m_pctrlLength2->setText(length);
	m_pctrlLength3->setText(length);
}


void BodyTransDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
				return;
			}
			else
			{
				OnOK();
				return;
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			break;
		}
		default:
			event->ignore();
	}
}


void BodyTransDlg::OnOK()
{
	m_bFrameOnly = m_pctrlFrameOnly->isChecked();
	m_FrameID    = m_pctrlFrameID->Value()-1;
	m_XTrans     = m_pctrlXTransFactor->Value() / Units::mtoUnit();
	m_YTrans     = m_pctrlYTransFactor->Value() / Units::mtoUnit();
	m_ZTrans     = m_pctrlZTransFactor->Value() / Units::mtoUnit();
	accept();
}



void BodyTransDlg::OnFrameOnly()
{
	m_bFrameOnly = m_pctrlFrameOnly->isChecked();
	m_pctrlFrameID->setEnabled(m_bFrameOnly);
}



void BodyTransDlg::SetupLayout()
{
	QHBoxLayout *FrameID = new QHBoxLayout;
	m_pctrlFrameOnly = new QCheckBox(tr("Frame Only"));
	m_pctrlFrameID = new DoubleEdit(0.0,0);
	FrameID->addWidget(m_pctrlFrameOnly);
	FrameID->addWidget(m_pctrlFrameID);

	QGridLayout *TransLayout = new QGridLayout;
	QLabel * XTrans = new QLabel(tr("X Translation"));
	QLabel * YTrans = new QLabel(tr("Y Translation"));
	QLabel * ZTrans = new QLabel(tr("Z Translation"));
	m_pctrlXTransFactor = new DoubleEdit(0.0,3);
	m_pctrlYTransFactor = new DoubleEdit(0.0,3);
	m_pctrlZTransFactor = new DoubleEdit(0.0,3);
	m_pctrlLength1 = new QLabel("m");
	m_pctrlLength2 = new QLabel("m");
	m_pctrlLength3 = new QLabel("m");
	TransLayout->addWidget(XTrans,1,1);
	TransLayout->addWidget(YTrans,2,1);
	TransLayout->addWidget(ZTrans,3,1);
	TransLayout->addWidget(m_pctrlXTransFactor,1,2);
	TransLayout->addWidget(m_pctrlYTransFactor,2,2);
	TransLayout->addWidget(m_pctrlZTransFactor,3,2);
	TransLayout->addWidget(m_pctrlLength1,1,3);
	TransLayout->addWidget(m_pctrlLength2,2,3);
	TransLayout->addWidget(m_pctrlLength3,3,3);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton       = new QPushButton(tr("OK"));
	CancelButton   = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton,   SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(FrameID);
	MainLayout->addLayout(TransLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);

	connect(m_pctrlFrameOnly, SIGNAL(clicked()), SLOT(OnFrameOnly()));

}

