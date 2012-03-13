/****************************************************************************

    LLTAnalysis Class
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

#include <math.h>
#include "Miarex.h"
#include "../MainFrame.h"
#include "LLTAnalysis.h"
#include "LLTAnalysisDlg.h"
#include <QString>

int LLTAnalysis::s_NLLTStations = 0;
double LLTAnalysis::s_RelaxMax = 0.0;
double LLTAnalysis::s_CvPrec = 0.0;



LLTAnalysis::LLTAnalysis(void *pParent)
{
	m_pParent = pParent;

	m_pWing = NULL;
	m_pWPolar = NULL;
	m_poaPolar = NULL;

	m_bSkip      = false;
	m_bCancel    = false;
	m_bConverged = false;
	m_bWingOut   = false;

	m_IterLim = 0;

	memset(m_Chord, 0, sizeof(m_Chord));
	memset(m_Offset, 0, sizeof(m_Offset));
	memset(m_Twist, 0, sizeof(m_Twist));
	memset(m_SpanPos, 0, sizeof(m_SpanPos));
	memset(m_StripArea, 0, sizeof(m_StripArea));


	memset(m_Re, 0, sizeof(m_Re));
	memset(m_Cl, 0, sizeof(m_Cl));
	memset(m_Ai, 0, sizeof(m_Ai));
	memset(m_ICd, 0, sizeof(m_ICd));
	memset(m_PCd, 0, sizeof(m_PCd));


	memset(m_Cm, 0, sizeof(m_Cm));
	memset(m_CmAirf, 0, sizeof(m_CmAirf));
	memset(m_XCPSpanRel, 0, sizeof(m_XCPSpanRel));
	memset(m_XCPSpanAbs, 0, sizeof(m_XCPSpanAbs));
	memset(m_BendingMoment, 0, sizeof(m_BendingMoment));
	memset(m_XTrTop, 0, sizeof(m_XCPSpanAbs));
	memset(m_XTrBot, 0, sizeof(m_BendingMoment));

	m_LengthUnit.clear();
	m_mtoUnit = 0.0;

	m_QInf0 = 0.0;
	m_Maxa  = 0.0;

	m_CL = 0.0;
	m_InducedDrag = 0.0;
	m_ViscousDrag = 0.0;

	m_VYm = m_IYm = m_GYm = 0.0;
	m_VCm = m_ICm = m_GCm = 0.0;
	m_GRm = 0.0;

	m_XCP = m_YCP = 0.0;
}



void LLTAnalysis::LLTInitCl(double &QInf, double const Alpha)
{
	//Initializes the Reynolds numbers and lift coefficients for the initial iteration in LLT

	CFoil *pFoil0 = NULL;
	CFoil *pFoil1 = NULL;
	double yob, tau;
	int k;
	bool bOutRe, bError;
	for (k=1; k<s_NLLTStations; k++)
	{
		yob   = cos(k*PI/s_NLLTStations);
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
		m_Re[k] = m_Chord[k] * QInf /m_pWPolar->m_Viscosity;
		m_Cl[k] = GetCl(m_poaPolar, pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
	}

	if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)
	{
		double Lift=0.0;// required for Type 2
		for (k=1; k<s_NLLTStations; k++)
		{
			Lift += Eta(k) * m_Cl[k] * m_Chord[k] /m_pWing->m_PlanformSpan;
		}
		if(Lift<=0.0) return;
		QInf  = m_QInf0 / sqrt(Lift);
		for (k=1; k<s_NLLTStations; k++)
		{
			m_Re[k] = m_Chord[k] * QInf /m_pWPolar->m_Viscosity;
		}
	}
}


double LLTAnalysis::Eta(int m)
{
	//Auxiliary calculation of the Eta factor in LLT

	return PI/2.0/(double)m_pWing->m_NStation * sin((double)m*PI/(double)m_pWing->m_NStation) ;
}




void LLTAnalysis::LLTComputeWing(double QInf, double Alpha, QString &ErrorMessage)
{
	CFoil* pFoil0 = NULL;
	CFoil* pFoil1 = NULL;

	int m;
	QString strange;

	double yob, tau, c4, arad, zpos;
	yob = tau = c4 = arad = zpos = 0.0;
	double Integral0           = 0.0;
	double Integral1           = 0.0;
	double Integral2           = 0.0;
	double InducedDrag         = 0.0;
	double ViscousDrag         = 0.0;
	double InducedYawingMoment = 0.0;
	double ViscousYawingMoment = 0.0;
	double PitchingMoment      = 0.0;
	double VCm                 = 0.0;
	double ICm                 = 0.0;
	double eta, sigma;
	double Cm0;
	double ViscCm[MAXSPANSTATIONS+1], InducedCm[MAXSPANSTATIONS+1];

	bool bOutRe, bError;
	bool bPointOutRe, bPointOutAlpha;
	m_bWingOut = false;

	ErrorMessage.clear();

	for (m=1; m<s_NLLTStations; m++)
	{
		bPointOutRe    = false;
		bPointOutAlpha = false;
		yob   = cos((double)m*PI/(double)s_NLLTStations);
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);

		m_Cl[m]     = GetCl(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_PCd[m]    = GetCd(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, m_pWing->m_AR, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_ICd[m]    = -m_Cl[m] * (m_Ai[m]* PI/180.0);

		m_XTrTop[m] = GetXTr(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m] + m_Twist[m], tau, true, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XTrBot[m] = GetXTr(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, false, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_CmAirf[m] = GetCm(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XCPSpanRel[m] = GetXCp(m_poaPolar, pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);

		if(fabs(m_XCPSpanRel[m])<0.000001)
		{
			//plr mesh was generated prior to v3.15, i.e., without XCp calculations
			Cm0 = GetCm0(m_poaPolar, pFoil0, pFoil1, m_Re[m],tau, bOutRe, bError);
			if(m_Cl[m]!=0.0) m_XCPSpanRel[m] = 0.25 - Cm0/m_Cl[m];
			else             m_XCPSpanRel[m] = 0.25;
		}
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		arad = (Alpha+m_Ai[m]+m_Twist[m])*PI/180.0;
//		arad = (s_Alpha-m_Ai[m])*PI/180.0;
		c4   = m_pWing->C4(yob, m_pWPolar->m_CoG.x)/m_Chord[m];
		zpos = m_pWing->ZPos(yob*m_pWing->m_PlanformSpan/2.0)/m_Chord[m];

		m_Cm[m]      = m_CmAirf[m]- c4  * (m_Cl[m]*cos(arad) + m_PCd[m]*sin(arad)) - zpos* (m_Cl[m]*sin(arad) - m_PCd[m]*cos(arad));
		ViscCm[m]    = (-c4 *sin(arad) + zpos*cos(arad))* m_PCd[m];
		InducedCm[m] = m_CmAirf[m]- c4  * m_Cl[m]*cos(arad) - zpos* m_Cl[m]*sin(arad);

		eta = Eta(m);
		sigma = Sigma(m);
		Integral0           += eta   * m_Cl[m]  * m_Chord[m];
		Integral1           += sigma * m_Cl[m]  * m_Chord[m];
		Integral2           += eta   * m_Cl[m]  * m_Chord[m] * (m_Offset[m]+m_XCPSpanRel[m]*m_Chord[m]);
		InducedDrag         += eta   * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousDrag         += eta   * m_PCd[m] * m_Chord[m];
		InducedYawingMoment += sigma * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousYawingMoment += sigma * m_PCd[m] * m_Chord[m];
		PitchingMoment      += eta   * m_Cm[m]      * m_Chord[m] * m_Chord[m];
		VCm                 += eta   * ViscCm[m]    * m_Chord[m] * m_Chord[m];
		ICm                 += eta   * InducedCm[m] * m_Chord[m] * m_Chord[m];

		if(bPointOutAlpha)
		{
			ErrorMessage = QString(QObject::tr("       Span pos = %1 ")).arg(cos(m*PI/s_NLLTStations)*m_pWing->m_PlanformSpan/2.0*m_mtoUnit,9,'f',2);
			ErrorMessage += m_LengthUnit;
			ErrorMessage += ",  Re = ";
			ReynoldsFormat(strange, m_Re[m]);
			ErrorMessage += strange;

			strange = QString(QObject::tr(" ,  A+Ai+Twist = %1 could not be interpolated")+"\n").arg(Alpha+m_Ai[m] + m_Twist[m],6,'f',1);
			ErrorMessage+=strange;

			m_bWingOut = true;
			m_bConverged = false;
		}
		else if(bPointOutRe)
		{
			ErrorMessage = QString(QObject::tr("       Span pos = %1 ")).arg(cos(m*PI/s_NLLTStations)*m_pWing->m_PlanformSpan/2.0*m_mtoUnit,9,'f',2);
			ErrorMessage += m_LengthUnit;
			ErrorMessage += ",  Re = ";
			ReynoldsFormat(strange, m_Re[m]);
			ErrorMessage += strange;

			strange = QString(QObject::tr(" ,  A+Ai+Twist = %1 is outside the flight envelope")+"\n").arg(Alpha+m_Ai[m] + m_Twist[m],6,'f',1);
			ErrorMessage+=strange;

			m_bWingOut = true;
		}
	}

	m_CL            =  Integral0   * m_pWing->m_AR /m_pWing->m_PlanformSpan;
	m_InducedDrag   =  InducedDrag * m_pWing->m_AR /m_pWing->m_PlanformSpan  * PI / 180.0;
	m_ViscousDrag   =  ViscousDrag / m_pWing->m_GChord;

	m_VYm = ViscousYawingMoment /m_pWing->m_GChord;
	m_IYm = InducedYawingMoment /m_pWing->m_PlanformSpan * PI * m_pWing->m_AR /180.0;
	m_GYm = m_VYm + m_IYm;
//	m_GCm = PitchingMoment / m_GChord / m_MAChord;
	m_VCm = VCm / m_pWing->m_GChord / m_pWing->m_MAChord;
	m_ICm = ICm / m_pWing->m_GChord / m_pWing->m_MAChord;
	m_GCm = m_VCm + m_ICm;

	m_GRm = -Integral1   * m_pWing->m_AR /m_pWing->m_PlanformSpan;

	if(m_CL !=0.0)	m_XCP = Integral2 * m_pWing->m_AR /m_pWing->m_PlanformSpan/m_CL;
	else           m_XCP = 0.0;
	if(m_pWing->m_bSymetric) m_YCP = 0.0;
	else                     m_YCP = m_pWing->m_AR/m_CL * Integral1;

	LLTSetBending(QInf);
}




void LLTAnalysis::LLTSetBending(double QInf)
{
	//bending moment

	int j,jj;

	//dynamic pressure, kg/m3
	double q = 0.5*m_pWPolar->m_Density * QInf * QInf;

	double bm;
	double y, yy;

	for (j=1; j<s_NLLTStations; j++)
	{
		y = m_SpanPos[j];
		bm = 0.0;
		if (y>=0)
		{
			for (jj=0; jj<j; jj++)
			{
				yy =  m_SpanPos[jj];
				bm += (yy-y) * m_Cl[jj] * m_StripArea[jj];
			}
		}
		else
		{
			for (jj=j+1; jj<s_NLLTStations; jj++)
			{
				yy =  m_SpanPos[jj];
				bm += (y-yy) * m_Cl[jj] * m_StripArea[jj];
			}
		}
		m_BendingMoment[j] = bm*q;
	}
}



bool LLTAnalysis::LLTSetLinearSolution(double Alpha)
{
	double aij[MAXSPANSTATIONS*MAXSPANSTATIONS];// coefficient matrix
	double rhs[MAXSPANSTATIONS+1];//right hand side

	memset(aij, 0, sizeof(aij));
	memset(rhs, 0, sizeof(rhs));

	CFoil *pFoil0, *pFoil1;
	int i,j,p;
	double fr  = s_NLLTStations;
	double fj, t0, st0, snt0, c, a0, slope, tau, yob;
	double cs = m_pWing->m_TChord[0];

	for (i=1; i<s_NLLTStations; i++)
	{
		c   = m_Chord[i];
		t0  = i * PI/fr;
		st0 = sin(t0);
		for (j=1; j<s_NLLTStations; j++)
		{
			fj = double(j);
			snt0 = sin(fj*t0);
			p = (i-1)*(s_NLLTStations-1)+j-1;
			aij[p]  = snt0 + c*PI/m_pWing->m_PlanformSpan/2.0* fj*snt0/st0;
		}
		yob   = cos(i*PI/s_NLLTStations);
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
		a0 = GetZeroLiftAngle(m_poaPolar, pFoil0, pFoil1, m_Re[i], tau);
		rhs[i] = c/cs * (Alpha-a0+m_Twist[i])/180.0*PI;
	}

	bool bCancel = false;
	if(!Gauss(aij,s_NLLTStations-1, rhs+1,0,&bCancel)) return false;


	for (i=1; i<s_NLLTStations; i++)
	{
		t0  = i * PI/fr;
		m_Cl[i] = 0.0;
		for (j=1; j<s_NLLTStations; j++)
		{
			fj = double(j);
			snt0 = sin(fj*t0);
			m_Cl[i] += rhs[j]* snt0;
		}
		yob   = cos(i*PI/s_NLLTStations);
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
		GetLinearizedPolar(m_poaPolar, pFoil0, pFoil1, m_Re[i], tau, a0, slope);
		a0 = GetZeroLiftAngle(m_poaPolar, pFoil0, pFoil1, m_Re[i], tau);//better approximation ?
		m_Cl[i] *= slope*180.0/PI*cs/m_Chord[i];
		m_Ai[i]  = -(Alpha-a0+m_Twist[i]) + m_Cl[i]/slope;
	}

	return true;
}





double LLTAnalysis::Sigma(int m)
{
	//Auxiliary calculation of the sigma factor in LLT
	return PI/8.0/(double)m_pWing->m_NStation * sin(2.*(double)m*PI/(double)m_pWing->m_NStation) ;
}




double LLTAnalysis::Beta(int m, int k)
{
	//Auxiliary calculation of the Beta factor in LLT
	double b;
	double fk = (double)k;
	double fm = (double)m;
	double fr = (double)m_pWing->m_NStation;

	if (m==k) b = 180.0*fr/8.0/PI/sin(fk*PI/fr);
	else if (IsEven(m+k)) b=0.0;
	else
	{
		double c1 = 180.0/4.0/PI/fr/sin(fk*PI/fr);
		double c2 =   1.0/(1.0-cos((fk+fm)*PI/fr))
					- 1.0/(1.0-cos((fk-fm)*PI/fr));
		b = c1 * c2;
	}
	return b;
}


double LLTAnalysis::AlphaInduced(int k)
{
	// Calculates the induced angle from the lift coefficient and
	// from the Beta factor in LLT

	double ai = 0.0;

	for (int m=1; m<m_pWing->m_NStation; m++)
	{
		ai += Beta(m,k) * m_Cl[m] * m_Chord[m]/m_pWing->m_PlanformSpan;
	}
	return ai;
}





int LLTAnalysis::LLTIterate(double &QInf, double Alpha)
{
	int k ;
	CFoil* pFoil0  = NULL;
	CFoil* pFoil1  = NULL;
	double a, yob, tau, anext;
	bool bOutRe, bError;
	int iter = 0;

	while(iter<m_IterLim && !m_bSkip)
	{
		if(m_bCancel) return -1;
		m_Maxa = 0.0;

		for (k=1; k<s_NLLTStations; k++)
		{
			a        = m_Ai[k];
			anext    = -AlphaInduced(k);
			m_Ai[k]  = a +(anext-a)/s_RelaxMax;
			m_Maxa   = qMax(m_Maxa, fabs(a-anext));
		}

		double Lift=0.0;// required for Type 2
		for (k=1; k<s_NLLTStations; k++)
		{
			yob     = cos(k*PI/s_NLLTStations);
			m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
			m_Cl[k] = GetCl(m_poaPolar, pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
			if (m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)
			{
				Lift += Eta(k) * m_Cl[k] * m_Chord[k];
			}
		}

		if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)
		{
			Lift *= m_pWing->m_AR / m_pWing->m_PlanformSpan;
			if(Lift<=0.0)  return -1;

			QInf  = m_QInf0 / sqrt(Lift);

			for (k=1; k<s_NLLTStations; k++)
			{
				m_Re[k] = m_Chord[k] * QInf /m_pWPolar->m_Viscosity;
				yob     = cos(k*PI/s_NLLTStations);
				m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
				m_Cl[k] = GetCl(m_poaPolar, pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
			}
		}

		if (m_Maxa<s_CvPrec)
		{
			m_bConverged = true;
			break;
		}

		LLTAnalysisDlg *pLLTDlg = (LLTAnalysisDlg*)m_pParent;
		pLLTDlg->UpdateGraph(iter, m_Maxa);
		iter++;
	}
	return iter;
}



void LLTAnalysis::LLTInitialize(double QInf)
{
	//
	// Initializes the LLT calculation
	//
	double y;
	int k;
	m_bWingOut = false;
	m_bConverged = false;

	if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)	m_QInf0 = sqrt(2.*m_pWPolar->m_Mass* 9.81 /m_pWPolar->m_Density/m_pWing->m_PlanformArea);
	else                                    m_QInf0 = 0.0;

	m_bConverged = true;
	m_bWingOut = false;

	m_pWing->ComputeChords(s_NLLTStations, m_Chord, m_Offset, m_Twist);
	for (k=0; k<=s_NLLTStations; k++)
	{
		y   = cos(k*PI/s_NLLTStations)* m_pWing->m_PlanformSpan/2.0;
		m_Twist[k] = m_pWing->Twist(y);
	}

	for (k=0; k<=s_NLLTStations; k++)
	{
		m_SpanPos[k] = m_pWing->m_PlanformSpan/2.0 * cos((double)k*PI/s_NLLTStations);
		m_Re[k] = m_Chord[k] * QInf/m_pWPolar->m_Viscosity;
	}
	double yj, yjm, yjp, dy;
	for (int j=1; j<s_NLLTStations; j++)
	{
		yjp = m_SpanPos[j-1];
		yjm = m_SpanPos[j+1];
		yj  = m_SpanPos[j];

		dy = (yjp-yj)/2.0 + (yj-yjm)/2.0;

		m_StripArea[j] = m_Chord[j]*dy;//m2
	}
}





