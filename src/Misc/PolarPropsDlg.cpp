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
#include <QHeaderView>

#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"
#include "../MainFrame.h"
#include "../Globals.h"

void * PolarPropsDlg::s_pMainFrame;

PolarPropsDlg::PolarPropsDlg()
{
	setWindowTitle(tr("Polar Properties"));
	m_pMiarex     = NULL;
	m_pXDirect    = NULL;
	m_pPolar      = NULL;
	m_pWPolar     = NULL;

	SetupLayout();
}


void PolarPropsDlg::SetupLayout()
{
	QLabel *lab = new QLabel("");
	m_pctrlPolarDescription = new QLabel;
	QVBoxLayout *PropertiesLayout = new QVBoxLayout;
	PropertiesLayout->addWidget(lab);
	PropertiesLayout->addWidget(m_pctrlPolarDescription);

	QPushButton *OKButton            = new QPushButton(tr("OK"));
	QHBoxLayout *CommandButtons = new QHBoxLayout;
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));

	QVBoxLayout * MainLayout = new QVBoxLayout(this);
	MainLayout->addLayout(PropertiesLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void PolarPropsDlg::InitDialog()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString strong, strange;

	if(m_pXDirect && m_pPolar)
	{
		strange = m_pPolar->m_PlrName +"\n\n";

		strange += tr("Parent foil")+" = "+ m_pPolar->m_FoilName+"\n";

		strong = QString(tr("Analysis Type")+" = %1\n").arg(m_pPolar->m_Type);
		strange += strong;

		if(m_pPolar->m_Type==1)
		{
			strong = QString(tr("Reynolds number")+" = %1\n").arg(m_pPolar->m_Reynolds,0,'f',0);
			strange += strong;
			strong = QString(tr("Mach number") + " = %1\n").arg(m_pPolar->m_Mach,5,'f',2);
			strange += strong;
		}
		else if(m_pPolar->m_Type==2)
		{
			strong = QString(tr("Re.sqrt(Cl)")+" = %1\n").arg(m_pPolar->m_Reynolds,0,'f',0);
			strange += strong;
			strong = QString(tr("Ma.sqrt(Cl)") + " = %1\n").arg(m_pPolar->m_Mach,5,'f',2);
			strange += strong;
		}
		else if(m_pPolar->m_Type==3)
		{
			strong = QString(tr("Re.Cl")+" = %1\n").arg(m_pPolar->m_Reynolds,0,'f',0);
			strange += strong;
			strong = QString(tr("Mach number") + " = %1\n").arg(m_pPolar->m_Mach,5,'f',2);
			strange += strong;
		}
		else if(m_pPolar->m_Type==4)
		{
			strong = QString(tr("Alpha")+" = %1"+QString::fromUtf8("°")+"\n").arg(m_pPolar->m_ASpec,7,'f',2);
			strange += strong;
			strong = QString(tr("Mach number") + " = %1\n").arg(m_pPolar->m_Mach,5,'f',2);
			strange += strong;
		}


		strong = QString(tr("NCrit") + " = %1\n").arg(m_pPolar->m_ACrit,6,'f',2);
		strange += strong;

		strong = QString(tr("Forced top transition position") + " = %1\n").arg(m_pPolar->m_XTop,6,'f',2);
		strange += strong;

		strong = QString(tr("Forced bottom transition position") + " = %1\n").arg(m_pPolar->m_XBot,6,'f',2);
		strange += strong;

		strong = QString(tr("Number of analysis points") +" = %1").arg(m_pPolar->m_Alpha	.size());
		strange += strong;
	}
	else if(m_pMiarex && m_pWPolar)
	{
		QString lenunit, massunit, speedunit;
		GetLengthUnit(lenunit, pMainFrame->m_LengthUnit);
		GetWeightUnit(massunit, pMainFrame->m_WeightUnit);
		GetSpeedUnit(speedunit, pMainFrame->m_SpeedUnit);

		strange = m_pWPolar->m_PlrName +"\n\n";

		strange += tr("Parent object") +" = "+ m_pWPolar->m_UFOName+"\n";

		strong = QString(tr("Analysis Type")+" = %1").arg(m_pWPolar->m_Type);
		if(m_pWPolar->m_Type==1)      strong += " ("+tr("Fixed speed") +")\n";
		else if(m_pWPolar->m_Type==2) strong += " ("+tr("Fixed lift") +")\n";
		else if(m_pWPolar->m_Type==4) strong += " ("+tr("Fixed angle of attack") +")\n";
		else if(m_pWPolar->m_Type==7) strong += " ("+tr("Stability analysis") +")\n";
		strange += strong;

		if(m_pWPolar->m_Type==1)
		{
			strong  = QString(tr("VInf =")+"%1").arg(m_pWPolar->m_QInf,10,'g',2);
			strange += strong + speedunit+"\n";
		}
		else if(m_pWPolar->m_Type==4)
		{
			strong  = QString(tr("Alpha =")+"%1").arg(m_pWPolar->m_ASpec,7,'f',2);
			strange += strong +QString::fromUtf8("°")+"\n";
		}

		strong  = QString(tr("Mass")+" = %1").arg(m_pWPolar->m_Weight*pMainFrame->m_kgtoUnit,12,'g',4);
		strange += strong + massunit + "\n";

		strong  = QString(tr("CoG.x")+" = %1 ").arg(m_pWPolar->m_CoG.x*pMainFrame->m_mtoUnit,12,'g',4);
		strange += strong + lenunit + "\n";

		strong  = QString(tr("CoG.z")+" = %1 ").arg(m_pWPolar->m_CoG.z*pMainFrame->m_mtoUnit,12,'g',4);
		strange += strong + lenunit + "\n";

		strong  = QString(tr("Beta")+" = %1").arg(m_pWPolar->m_Beta,7,'f',2);
		strange += strong +QString::fromUtf8("°")+"\n";

		strange += tr("Analysis method")+" = ";
		if(m_pWPolar->m_AnalysisMethod==1) strange +="LLT\n";
		else if(m_pWPolar->m_AnalysisMethod==2 && m_pWPolar->m_bVLM1)  strange +="VLM1\n";
		else if(m_pWPolar->m_AnalysisMethod==2 && !m_pWPolar->m_bVLM1) strange +="VLM2\n";
		else if(m_pWPolar->m_AnalysisMethod==3) strange +="3D-Panels\n";
		else if(m_pWPolar->m_AnalysisMethod==4) strange +="VLM1\n";
		else                                    strange +="\n";

		if(m_pWPolar->m_bViscous) strange += tr("Viscous analysis")+"\n";
		else                      strange += tr("Inviscid analysis")+"\n";

		strange += tr("Reference Area = ");
		if(m_pWPolar->m_RefAreaType==1) strange += tr("Planform area")+"\n";
		else                            strange += tr("Projected area")+"\n";

		if(m_pWPolar->m_bTiltedGeom) strange += tr("Tilted geometry")+"\n";

		if(m_pWPolar->m_bGround)
		{
			strong = QString(tr("Ground height")+" = %1").arg(m_pWPolar->m_Height*pMainFrame->m_mtoUnit)+lenunit+"\n";
			strange += strong;
		}

		strong  = QString(tr("Density =")+"%1 kg/m3\n").arg(m_pWPolar->m_Density,12,'g',4);
		strange += strong;

		strong  = QString(tr("Viscosity =")+"%1 m2/s\n").arg(m_pWPolar->m_Viscosity,12,'g',4);
		strange += strong;

		strong = QString(tr("Number of analysis points") +" = %1").arg(m_pWPolar->m_Alpha.size());
		strange += strong;
	}
	m_pctrlPolarDescription->setText(strange);
}
