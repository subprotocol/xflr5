/****************************************************************************

	XFoilTask Class
	   Copyright (C) 2011 Andre Deperrois adeperrois@xflr5.com

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


#include "BatchThreadDlg.h"
#include "XFoilTask.h"
#include <QThread>
#include <QCoreApplication>
#include <QApplication>
#include <QTimerEvent>



bool XFoilTask::s_bAutoInitBL = false;
bool XFoilTask::s_bCancel = true;
void *XFoilTask::s_pBatchThreadDlg;

XFoilTask::XFoilTask()
{
	m_Id = -1;
	m_pFoil  = NULL;
	m_pPolar = NULL;
	m_bIsFinished = true;
	setAutoDelete(false);
}


void XFoilTask::run()
{
	//asssumes that XFoil has been initialized with foil and polar
	QObject *pBatch = (QObject*)s_pBatchThreadDlg;
	if(s_bCancel || !m_pPolar || !m_pFoil)
	{
		m_bIsFinished = true;
		return;
	}

	if(m_pPolar->m_PolarType!=FIXEDAOAPOLAR) AlphaSequence();
	else
	{
		m_bIsFinished = true;
		return;
	}

	QTimerEvent *event = new QTimerEvent(m_Id);
	QApplication::postEvent(pBatch, event);
	//will be truly finished whent this message has been received by the batch analysis engine
}


bool XFoilTask::Init(CFoil *pFoil, CPolar *pPolar)
{
	m_bIsFinished = false;
	XFoilInstance.lvisc=true;

	m_pFoil = pFoil;
	m_pPolar = pPolar;

	if(!XFoilInstance.InitXFoilGeometry(m_pFoil))  return false;
//	if(!XFoilInstance.InitXFoilAnalysis(m_pPolar)) return false;

	return true;
}


bool XFoilTask::AlphaSequence()
{
	BatchThreadDlg *pBatch = (BatchThreadDlg*)s_pBatchThreadDlg;
	QString str;
	QString strong = "";

	double alphadeg;
	int ia, series, total, MaxSeries;
	double SpMin, SpMax, SpInc;

//	str = QString("Re=%1   Ma=%2   Nc=%3\n").arg(XFoilInstance.reinf1,8,'f',0).arg(XFoilInstance.minf1,5,'f',3).arg(XFoilInstance.acrit,5,'f',2);
//	strong+= str;
//	UpdateOutput(str);
	MaxSeries = 1;
	if(pBatch->m_bAlpha)
	{
		SpMin = pBatch->m_AlphaMin;
		SpMax = pBatch->m_AlphaMax;
		SpInc = fabs(pBatch->m_AlphaInc);
		if (pBatch->m_bFromZero && SpMin*SpMax<0)
		{
			MaxSeries = 2;
			SpMin = 0.0;
			SpMax = SpMax;
		}
	}
	else
	{
		SpMin = pBatch->m_ClMin;
		SpMax = pBatch->m_ClMax;
		SpInc = fabs(pBatch->m_ClInc);
	}

	if(SpMin > SpMax) SpInc = -fabs(SpInc);

	for (series=0; series<MaxSeries;series++)
	{
		qApp->processEvents();
		if(s_bCancel) break;

		total = (int)fabs((SpMax*1.0001-SpMin)/SpInc);//*1.0001 to make sure upper limit is included

		XFoilInstance.InitXFoilAnalysis(m_pPolar);

		XFoilInstance.lvisc = true;

		if(pBatch->m_bInitBL)
		{
			XFoilInstance.lblini = false;
			XFoilInstance.lipan = false;
		}

		for (ia=0; ia<=total; ia++)
		{
			if(s_bCancel) break;

			if(pBatch->m_bAlpha)
			{
				alphadeg = SpMin+ia*SpInc;
				XFoilInstance.alfa = alphadeg * PI/180.0;
				XFoilInstance.lalfa = true;
				XFoilInstance.qinf = 1.0;
//				str = QString("Alpha = %1").arg(alphadeg,9,'f',3);
//				strong+=str;
//				UpdateOutput(str);


				// here we go !
				if (!XFoilInstance.specal())
				{
					str = QObject::tr("Invalid Analysis Settings\nCpCalc: local speed too large\n Compressibility corrections invalid ");
//					UpdateOutput(str);
					return false;
				}
			}
			else
			{
				XFoilInstance.lalfa = false;
				XFoilInstance.alfa = 0.0;
				XFoilInstance.qinf = 1.0;
				XFoilInstance.clspec = SpMin+ia*SpInc;
				str = QString(QObject::tr("Cl = %1")).arg(XFoilInstance.clspec,9,'f',3);
				strong+=str;
//				UpdateOutput(str);
				if(!XFoilInstance.speccl())
				{
//					str = QObject::tr("Invalid Analysis Settings\nCpCalc: local speed too large\n Compressibility corrections invalid ");
//					UpdateOutput(str);
					return false;
				}
			}

			XFoilInstance.lwake = false;
			XFoilInstance.lvconv = false;

			m_Iterations = 0;

			while(!Iterate()){}
			if(XFoilInstance.lvconv)
			{
				str = QString(QObject::tr("   ...converged after %1 iterations\n")).arg(m_Iterations);
				strong+= str;
//						UpdateOutput(str);
				m_pPolar->AddData(&XFoilInstance);
			}
			else
			{
				str = QString(QObject::tr("   ...unconverged after %1 iterations\n")).arg(m_Iterations);
				strong+= str;
//						UpdateOutput(str);
			}

		}// end Alpha or Cl loop
		SpMin = 0.0;
		SpMax = pBatch->m_AlphaMin;
		SpInc = -SpInc;
	}
//		strong+="\n";
	return true;
}


bool XFoilTask::Iterate()
{
	BatchThreadDlg *pBatch = (BatchThreadDlg*)s_pBatchThreadDlg;
	QString str;

	if(!XFoilInstance.viscal())
	{
		XFoilInstance.lvconv = false;//point is unconverged
//		str =QObject::tr("CpCalc: local speed too large\n Compressibility corrections invalid");
		return false;
	}


	while(m_Iterations<pBatch->m_IterLim && !XFoilInstance.lvconv && !s_bCancel)
	{
		if(XFoilInstance.ViscousIter())  m_Iterations++;
		else                             m_Iterations = pBatch->m_IterLim;
	}

	if(s_bCancel)  return true;// to exit loop

	if(!XFoilInstance.ViscalEnd())
	{
		XFoilInstance.lvconv = false;//point is unconverged

		XFoilInstance.lblini = false;
		XFoilInstance.lipan  = false;
		return true;// to exit loop
	}

	if(m_Iterations>=pBatch->m_IterLim && !XFoilInstance.lvconv)
	{
		if(s_bAutoInitBL)
		{
			XFoilInstance.lblini = false;
			XFoilInstance.lipan = false;
		}
		XFoilInstance.fcpmin();// Is it of any use ?
		return true;
	}
	if(!XFoilInstance.lvconv)
	{
		XFoilInstance.fcpmin();// Is it of any use ?
		return false;
	}
	else
	{
		//converged at last
		XFoilInstance.fcpmin();// Is it of any use ?
		return true;
	}
	return false;
}


