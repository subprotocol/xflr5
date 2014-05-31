/****************************************************************************

    LLTAnalysis Class
	Copyright (C) 2011-2014 Andre Deperrois adeperrois@xflr5.com

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

#include <QApplication>
#include <math.h>
#include "LLTAnalysis.h"
#include "LLTAnalysisDlg.h"
#include "../../misc/Units.h"
#include <QString>



QList<void *> *LLTAnalysis::s_poaPolar = NULL;
int LLTAnalysis::s_IterLim = 20;
int LLTAnalysis::s_NLLTStations = 0;
double LLTAnalysis::s_RelaxMax = 0.0;
double LLTAnalysis::s_CvPrec = 0.0;
bool LLTAnalysis::s_bInitCalc = true;

/** The public constructor */
LLTAnalysis::LLTAnalysis()
{
	m_pWing = NULL;
	m_pWPolar = NULL;

	resetVariables();
}

/**
* Initializes the variables to default values
*/
void LLTAnalysis::resetVariables()
{
	m_nPoints = 1;
	m_bSequence = false;
	m_vMin = m_vMax = m_vDelta = 0.0;

	m_bCancel    = false;
	m_bConverged = false;
	m_bWingOut   = false;

	s_IterLim = 0;

	memset(m_Chord,         0, sizeof(m_Chord));
	memset(m_Offset,        0, sizeof(m_Offset));
	memset(m_Twist,         0, sizeof(m_Twist));
	memset(m_SpanPos,       0, sizeof(m_SpanPos));
	memset(m_StripArea,     0, sizeof(m_StripArea));
	memset(m_Re,            0, sizeof(m_Re));
	memset(m_Cl,            0, sizeof(m_Cl));
	memset(m_Ai,            0, sizeof(m_Ai));
	memset(m_ICd,           0, sizeof(m_ICd));
	memset(m_PCd,           0, sizeof(m_PCd));
	memset(m_Cm,            0, sizeof(m_Cm));
	memset(m_CmAirf,        0, sizeof(m_CmAirf));
	memset(m_XCPSpanRel,    0, sizeof(m_XCPSpanRel));
	memset(m_XCPSpanAbs,    0, sizeof(m_XCPSpanAbs));
	memset(m_BendingMoment, 0, sizeof(m_BendingMoment));
	memset(m_XTrTop,        0, sizeof(m_XCPSpanAbs));
	memset(m_XTrBot,        0, sizeof(m_BendingMoment));

	m_LengthUnit.clear();
	m_mtoUnit = 0.0;

	m_QInf0 = 0.0;
	m_Maxa  = 0.0;

	m_CL = 0.0;
	m_CDi = 0.0;
	m_CDv = 0.0;

	m_VYm = m_IYm = m_GYm = 0.0;
	m_VCm = m_ICm = m_GCm = 0.0;
	m_GRm = 0.0;

	m_CP.Set(0.0,0.0,0.0);
	m_pGraph = NULL;
	m_pCurve = NULL;
}


/** 
* Initializes the Reynolds numbers and and the non-linear lift coefficients for the first iteration in LLT
 * @param QInf the freestream velocity, in m/s
 * @param Alpha the angle of attack, in degrees
*/
void LLTAnalysis::setVelocity(double &QInf)
{
	if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)
	{
		double Lift=0.0;// required for Type 2
		for (int k=1; k<s_NLLTStations; k++)
		{
			Lift += Eta(k) * m_Cl[k] * m_Chord[k] /m_pWing->m_PlanformSpan;
		}
		if(Lift<=0.0) return;
		QInf  = m_QInf0 / sqrt(Lift);
	}
	for (int k=1; k<s_NLLTStations; k++)
	{
		m_Re[k] = m_Chord[k] * QInf /m_pWPolar->m_Viscosity;
	}
}


/**
 * The multiplier for the lift, drag and pitching-moment coefficients in the case of asymmetrical distributions.
 * cf. NACA TN-1269 for details
 * @param m the index of the span station
 * @return the value of the eta factor
*/
double LLTAnalysis::Eta(int m)
{
	return PI/2.0/(double)m_pWing->m_NStation * sin((double)m*PI/(double)m_pWing->m_NStation) ;
}


/**
 * The multiplier for the yawing and rolling moment coefficients in the case of asymmetrical distributions.
 * cf. NACA TN-1269 for details
 * @param m 
 * @return the value of the sigma factor
*/
double LLTAnalysis::Sigma(int m)
{
	return PI/8.0/(double)m_pWing->m_NStation * sin(2.*(double)m*PI/(double)m_pWing->m_NStation) ;
}



/**
 * The multiplier for the induced angle of attack in the case of asymmetrical distributions.
 * cf. NACA TN-1269 for details
 * @param m 
 * @param k
 * @return the value of the beta factor
*/
double LLTAnalysis::Beta(int m, int k)
{
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


/**
 * Once the solution has converged, computes the wing's aerodynamic coefficients and stores them in the member variables.
 * @param QInf the freestream velocity, in m/s
 * @param Alpha the angle of attack, in degrees
 * @param ErrorMessage a reference to the output string which is filled with the error messages
 */
void LLTAnalysis::ComputeWing(double QInf, double Alpha, QString &ErrorMessage)
{
	Foil* pFoil0 = NULL;
	Foil* pFoil1 = NULL;

	int m;
	QString strange;
	double yob, tau, c4, arad, zpos;
	
	yob = tau = c4 = arad = zpos = 0.0;
	double Integral0           = 0.0;
	double Integral1           = 0.0;
	double Integral2           = 0.0;
	double Integral3           = 0.0;
	double InducedDrag         = 0.0;
	double ViscousDrag         = 0.0;
	double InducedYawingMoment = 0.0;
	double ViscousYawingMoment = 0.0;
	double PitchingMoment      = 0.0;
	double VCm                 = 0.0;
	double ICm                 = 0.0;
	double eta, sigma;
	double Cm0;
	double ViscCm, InducedCm;

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

		m_Cl[m]     = GetCl(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_PCd[m]    = GetCd(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, m_pWing->m_AR, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_ICd[m]    = -m_Cl[m] * (m_Ai[m]* PI/180.0);

		m_XTrTop[m] = GetXTr(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m] + m_Twist[m], tau, true, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XTrBot[m] = GetXTr(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, false, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_CmAirf[m] = GetCm(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XCPSpanRel[m] = GetXCp(pFoil0, pFoil1, m_Re[m], Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);

		if(qAbs(m_XCPSpanRel[m])<0.000001)
		{
			//plr mesh was generated prior to v3.15, i.e., without XCp calculations
            Cm0 = GetCm0(pFoil0, pFoil1, m_Re[m],tau, bOutRe, bError);
			if(m_Cl[m]!=0.0) m_XCPSpanRel[m] = 0.25 - Cm0/m_Cl[m];
			else             m_XCPSpanRel[m] = 0.25;
		}
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		arad = (Alpha+m_Ai[m]+m_Twist[m])*PI/180.0;
//		arad = (s_Alpha-m_Ai[m])*PI/180.0;
		c4   = m_pWing->C4(yob, m_pWPolar->m_CoG.x)/m_Chord[m];
		zpos = m_pWing->ZPosition(yob*m_pWing->m_PlanformSpan/2.0)/m_Chord[m];

		m_Cm[m]      = m_CmAirf[m]- c4  * (m_Cl[m]*cos(arad) + m_PCd[m]*sin(arad)) - zpos* (m_Cl[m]*sin(arad) - m_PCd[m]*cos(arad));
		ViscCm       = (-c4 *sin(arad) + zpos*cos(arad))* m_PCd[m];
		InducedCm    = m_CmAirf[m]- c4  * m_Cl[m]*cos(arad) - zpos* m_Cl[m]*sin(arad);

		eta = Eta(m);
		sigma = Sigma(m);
		Integral0           += eta   * m_Cl[m]  * m_Chord[m];
		Integral1           += sigma * m_Cl[m]  * m_Chord[m];
		Integral2           += eta   * m_Cl[m]  * m_Chord[m] * (m_Offset[m]+m_XCPSpanRel[m]*m_Chord[m]);
		Integral3           += eta   * m_Cl[m]  * m_Chord[m] * (zpos*m_Chord[m]);
//		Integral3           += eta   * m_Cl[m]  * m_Chord[m] * ((m_XCPSpanRel[m]*m_Chord[m]*cos(-m_Twist[m]*PI/180.0)+m_Offset[m]) * sin(-Alpha*PI/180.0) + (zpos*m_Chord[m]+m_XCPSpanRel[m]*m_Chord[m]*sin(-m_Twist[m]*PI/180.0)) * cos(-Alpha*PI/180.0));
		InducedDrag         += eta   * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousDrag         += eta   * m_PCd[m] * m_Chord[m];
		InducedYawingMoment += sigma * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousYawingMoment += sigma * m_PCd[m] * m_Chord[m];
		PitchingMoment      += eta   * m_Cm[m]      * m_Chord[m] * m_Chord[m];
		VCm                 += eta   * ViscCm    * m_Chord[m] * m_Chord[m];
		ICm                 += eta   * InducedCm * m_Chord[m] * m_Chord[m];

		if(bPointOutAlpha)
		{
			ErrorMessage = QString("       Span pos = %1 ").arg(cos(m*PI/s_NLLTStations)*m_pWing->m_PlanformSpan/2.0*m_mtoUnit,9,'f',2);
			ErrorMessage += m_LengthUnit;
			ErrorMessage += ",  Re = ";
			ReynoldsFormat(strange, m_Re[m]);
			ErrorMessage += strange;

			strange = QString(" ,  A+Ai+Twist = %1 could not be interpolated\n").arg(Alpha+m_Ai[m] + m_Twist[m],6,'f',1);
			ErrorMessage+=strange;

			m_bWingOut = true;
			m_bConverged = false;
		}
		else if(bPointOutRe)
		{
			ErrorMessage = QString("       Span pos = %1 ").arg(cos(m*PI/s_NLLTStations)*m_pWing->m_PlanformSpan/2.0*m_mtoUnit,9,'f',2);
			ErrorMessage += m_LengthUnit;
			ErrorMessage += ",  Re = ";
			ReynoldsFormat(strange, m_Re[m]);
			ErrorMessage += strange;

			strange = QString(" ,  A+Ai+Twist = %1 is outside the flight envelope\n").arg(Alpha+m_Ai[m] + m_Twist[m],6,'f',1);
			ErrorMessage+=strange;

			m_bWingOut = true;
		}
	}

	m_CL    =  Integral0   * m_pWing->m_AR /m_pWing->m_PlanformSpan;
	m_CDi   =  InducedDrag * m_pWing->m_AR /m_pWing->m_PlanformSpan  * PI / 180.0;
	m_CDv   =  ViscousDrag / m_pWing->m_GChord;

	m_VYm = ViscousYawingMoment /m_pWing->m_GChord;
	m_IYm = InducedYawingMoment /m_pWing->m_PlanformSpan * PI * m_pWing->m_AR /180.0;
	m_GYm = m_VYm + m_IYm;
//	m_GCm = PitchingMoment / m_GChord / m_MAChord;
	m_VCm = VCm / m_pWing->m_GChord / m_pWing->m_MAChord;
	m_ICm = ICm / m_pWing->m_GChord / m_pWing->m_MAChord;
	m_GCm = m_VCm + m_ICm;

	m_GRm = -Integral1   * m_pWing->m_AR /m_pWing->m_PlanformSpan;

	if(m_CL !=0.0)
	{
		m_CP.x = Integral2 * m_pWing->m_AR /m_pWing->m_PlanformSpan/m_CL;
//        m_ZCP = Integral3 * m_pWing->m_AR /m_pWing->m_PlanformSpan/m_CL;
		m_CP.z=0.0;//the ZCP position may make physical sense in 3D panel analysis, but not in LLT

	}
	else
	{
		m_CP.Set(0.0,0.0,0.0);
    }
	if(m_pWing->m_bSymetric) m_CP.y = 0.0;
	else                     m_CP.y = m_pWing->m_AR/m_CL * Integral1;

	setBending(QInf);
}



/**
 * Calculates the bending moment at span stations, based on the results of the analysis and on the freestrem velocity.
 * @param QInf the freestream velocity, in m/s
 */
void LLTAnalysis::setBending(double QInf)
{
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



/**
 * Calculates the linear solution to the Lifting line problem, for the given wing geometry and angle of attack.
 * This is the starting point for the non-linear iterations.
 * A simplifying assumtion is that the lift slope is Cl = 2.pi (alpha-alpha0+wahshout) at all positions.
 * Numerical experiments have shown however that the non-linear LLT converges in roughly the same amount of iterations
 * whatever the initial state, even if random or asymetric.
 * @param Alpha the angle of attack, in degrees
 * @return true if a linear solution has been set, false otherwise. Should always be true, unless the user has defined some crazy wing configuration. Who knows what a user can do ?
 */
bool LLTAnalysis::setLinearSolution(double Alpha)
{
	double* aij = new double[s_NLLTStations*s_NLLTStations];
	double* rhs = new double[s_NLLTStations+1];

	memset(aij, 0, s_NLLTStations*s_NLLTStations*sizeof(double));
	memset(rhs, 0, (s_NLLTStations+1)*sizeof(double));

	Foil *pFoil0, *pFoil1;
	int i,j,p;
	int size = s_NLLTStations-1;
	double dn  = (double)s_NLLTStations;
	double di, dj, t0, st0, snt0, ch, a0, slope, tau, yob, twist;
	double cs = m_pWing->RootChord();
	double b  = m_pWing->m_PlanformSpan;

	for (i=1; i<s_NLLTStations; i++)
	{
		di  = (double)i;
		t0  = di * PI/dn;
		yob = cos(t0);
		ch = m_pWing->getChord(yob);      //or m_Chord[i], same
		twist = m_pWing->getTwist(yob);   //or m_Twist[i], same

		st0 = sin(t0);

		for (j=1; j<s_NLLTStations; j++)
		{
			dj   = (double)j;
			snt0 = sin(dj*t0);

			p = (i-1)*size + (j-1);
			aij[p] = snt0 + ch*PI/b/2.0* dj*snt0/st0;
		}

		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*b/2.0, tau);
		a0 = GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau);
		rhs[i] = ch/cs * (Alpha-a0+twist)/180.0*PI;
	}

	bool bCancel = false;
	if(!Gauss(aij, s_NLLTStations-1, rhs+1, 1, &bCancel))
	{
		delete [] aij;
		delete [] rhs;
		return false;
	}

	for (i=1; i<s_NLLTStations; i++)
	{
		di  = (double)i;
		t0  = di * PI/dn;
		yob = cos(t0);

		m_Cl[i] = 0.0;
		for (j=1; j<s_NLLTStations; j++)
		{
			dj = double(j);
			snt0 = sin(dj*t0);
			m_Cl[i] += rhs[j]* snt0;
		}
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*b/2.0, tau);
		GetLinearizedPolar(pFoil0, pFoil1, m_Re[i], tau, a0, slope);
		a0 = GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau); //better approximation ?

		m_Cl[i] *= slope*180.0/PI*cs/m_pWing->getChord(yob);
		m_Ai[i]  = -(Alpha-a0+m_pWing->getTwist(yob)) + m_Cl[i]/slope;
	}

	delete [] aij;
	delete [] rhs;
	return true;
}


/** 
 * Calculates the induced angle from the lift coefficient and from the Beta factor
 * @param k
 * @return the induced angle, in degrees
 */
double LLTAnalysis::AlphaInduced(int k)
{
	double ai = 0.0;

	for (int m=1; m<m_pWing->m_NStation; m++)
	{
		ai += Beta(m,k) * m_Cl[m] * m_Chord[m]/m_pWing->m_PlanformSpan;
	}
	return ai;
}




/** 
 * Performs the iterations of the non-linear LLT analysis.
 * @param QInf the freestream velocity, in m/s
 * @param Alpha the angle of attack, in degrees
 * @return the number of iterations performed, or -1 if the analysis has been user-cancelled
*/
int LLTAnalysis::iterate(double &QInf, double Alpha)
{
	int k ;
	Foil* pFoil0  = NULL;
	Foil* pFoil1  = NULL;
	double a, yob, tau, anext;
	bool bOutRe, bError;
	int iter = 0;

	while(iter<s_IterLim)
	{
		if(m_bCancel) return -1;
		m_Maxa = 0.0;

		for (k=1; k<s_NLLTStations; k++)
		{
			a        = m_Ai[k];
			anext    = -AlphaInduced(k);
			m_Ai[k]  = a +(anext-a)/s_RelaxMax;
			m_Maxa   = qMax(m_Maxa, qAbs(a-anext));
		}

		double Lift=0.0;// required for Type 2
		for (k=1; k<s_NLLTStations; k++)
		{
			yob     = cos(k*PI/s_NLLTStations);
			m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
			m_Cl[k] = GetCl( pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
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
				m_Cl[k] = GetCl(pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
			}
		}

		if (m_Maxa<s_CvPrec)
		{
			m_bConverged = true;
			break;
		}

		if(m_pCurve) m_pCurve->AppendPoint(iter, m_Maxa);
		iter++;
	}
	return iter;
}


/**
 * Initializes the geometric data necessary for the LLT calculation
*/
void LLTAnalysis::initializeGeom()
{	
	double yj, yjm, yjp, dy;
	int k;
	m_bWingOut = false;
	m_bConverged = false;

	if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)	m_QInf0 = sqrt(2.*m_pWPolar->m_Mass* 9.81 /m_pWPolar->m_Density/m_pWing->m_PlanformArea);
	else                                        m_QInf0 = 0.0;

	m_pWing->ComputeChords(s_NLLTStations, m_Chord, m_Offset, m_Twist);

	for (k=0; k<=s_NLLTStations; k++)
	{
//		y   = cos(k*PI/s_NLLTStations)* m_pWing->m_PlanformSpan/2.0;
		m_SpanPos[k] = m_pWing->m_PlanformSpan/2.0 * cos((double)k*PI/s_NLLTStations);
	}

	for (int j=1; j<s_NLLTStations; j++)
	{
		yjp = m_SpanPos[j-1];
		yjm = m_SpanPos[j+1];
		yj  = m_SpanPos[j];

		dy = (yjp-yj)/2.0 + (yj-yjm)/2.0;

		m_StripArea[j] = m_Chord[j]*dy;//m2
	}
}




bool LLTAnalysis::loop()
{
	if (m_pWPolar->polarType()!=FIXEDAOAPOLAR)
	{
		return alphaLoop();
	}
	else
	{
		return QInfLoop();
	}
	return false;
}


/**
* Launches a type 1 or 2 analysis.
* Loops over the range of specified aoa.
* For each successful aoa, stores the data in the WPolar and Operating Point objects.
*/
bool LLTAnalysis::alphaLoop()
{
	QString str;

	int i,iter;
	double Alpha,yob, tau;
	Foil *pFoil0, *pFoil1;
	bool bOutRe, bError;

	for (i=0; i<=m_nPoints; i++)
	{
		Alpha = m_vMin +(double)i * m_vDelta;
		if(m_bCancel)
		{
			str = "Analysis cancelled on user request....\n";
			traceLog(str);
			break;
		}

		if(m_pGraph)
		{
			str  = QString("Alpha=%1").arg(Alpha, 7,'f', 2);
			str += QString::fromUtf8("°");
			m_pCurve = m_pGraph->AddCurve();
			m_pCurve->SetTitle(str);
		}


		if(s_bInitCalc) setLinearSolution(Alpha);
		setVelocity(m_pWPolar->m_QInfSpec);

		//initialize first iteration
		for (int k=1; k<s_NLLTStations; k++)
		{
			yob   = cos(k*PI/s_NLLTStations);
			m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
			m_Cl[k] = GetCl(pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
		}


		str= QString("Calculating Alpha = %1... ").arg(Alpha,5,'f',2);
		traceLog(str);

		iter = iterate(m_pWPolar->m_QInfSpec, Alpha);

		if (iter==-1 && !m_bCancel)
		{
			str= QString("    ...negative Lift... Aborting\n");
			m_bError = true;
			s_bInitCalc = true;
			traceLog(str);
		}
		else if (iter<s_IterLim && !m_bCancel)
		{
			//converged,
			str= QString("    ...converged after %1 iterations\n").arg(iter);
			traceLog(str);
			ComputeWing(m_pWPolar->m_QInfSpec, Alpha, str);// generates wing results,
			traceLog(str);
			if (m_bWingOut) m_bWarning = true;
			PlaneOpp *pPOpp = createPlaneOpp(m_pWPolar->m_QInfSpec, Alpha, m_bWingOut);// Adds WOpp point and adds result to polar
			m_PlaneOppList.append(pPOpp);
			s_bInitCalc = false;

		}
		else
		{
			if (m_bWingOut) m_bWarning = true;
			m_bError = true;
			str= QString("    ...unconverged after %2 iterations\n").arg(iter);
			traceLog(str);
			s_bInitCalc = true;
		}
		qApp->processEvents();
	}

	return true;
}



/**
* Launches a type 4 analysis.
* Loops over the range of specified velocities.
* For each successful aoa, stores the data in the WPolar and Operating Point objects.
*/
bool LLTAnalysis::QInfLoop()
{
	//Alpha stands for QInf...
	int i,iter;
	QString str;
	double QInf, Alpha,yob, tau;
	Foil *pFoil0, *pFoil1;
	bool bOutRe, bError;

	str = "Initializing analysis...\n";
	traceLog(str);

	QInf = 0.0;
	Alpha = m_pWPolar->m_AlphaSpec;

	for (i=0; i<=m_nPoints; i++)
	{
		QInf = m_vMin + (double)i * m_vDelta;
		if(m_bCancel)
		{
			str = "Analysis cancelled on user request....\n";
			traceLog(str);
			break;
		}

		if(m_pGraph)
		{
			str = QString("QInf=%1 m/s").arg(QInf, 7,'f', 2);
			m_pCurve = m_pGraph->AddCurve();
			m_pCurve->SetTitle(str);
		}

		if(s_bInitCalc) setLinearSolution(m_pWPolar->m_AlphaSpec);
		setVelocity(QInf);

		//initialize first iteration
		for (int k=1; k<s_NLLTStations; k++)
		{
			yob   = cos(k*PI/s_NLLTStations);
			m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
			m_Cl[k] = GetCl(pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
		}

		str = QString("Calculating QInf = %1... ").arg(QInf,6,'f',2);
		traceLog(str);
		iter = iterate(QInf, m_pWPolar->m_AlphaSpec);

		if(iter<0)
		{
			//unconverged
			m_bError = true;
			m_bWarning = true;
			str = QString("\n");
			traceLog(str);
			s_bInitCalc = true;
		}
		else if (iter<s_IterLim  && !m_bCancel)
		{
			//converged,
			str = QString("    ...converged after %1 iterations\n").arg(iter);
			traceLog(str);
			ComputeWing(QInf, m_pWPolar->m_AlphaSpec,str);// generates wing results,
			traceLog(str);
			if (m_bWingOut) m_bWarning = true;
			PlaneOpp *pPOpp = createPlaneOpp(QInf, m_pWPolar->m_AlphaSpec, m_bWingOut);// Adds WOpp point and adds result to polar
			m_PlaneOppList.append(pPOpp);

/*			if(m_bWingOut)
			{
				str = QString("\n");
				traceLog(str);
			}*/
			s_bInitCalc = false;
		}
		else
		{
			if (m_bWingOut) m_bWarning = true;
			m_bError = true;
			str = QString("    ...unconverged after %1 iterations\n").arg(iter);
			traceLog(str);
			s_bInitCalc = true;
		}
		qApp->processEvents();
	}
	return true;
}



/**
* Copies the value of the input parameters to the member variables
*/
void LLTAnalysis::setLLTRange(double AlphaMin, double AlphaMax, double DeltaAlpha, bool bSequence)
{
	m_vMin      = AlphaMin;
	m_vMax      = AlphaMax;
	m_vDelta    = DeltaAlpha;
	m_bSequence = bSequence;

	if(m_vMax<m_vMin) m_vDelta = -(double)qAbs(m_vDelta);
	m_nPoints  = (int)qAbs((m_vMax-m_vMin)*1.001/m_vDelta);
	if(!m_bSequence) m_nPoints = 0;

}



void LLTAnalysis::setLLTData(Plane *pPlane, WPolar *pWPolar)
{
	m_pPlane   = pPlane;
	m_pWing    = pPlane->wing();
	m_pWPolar  = pWPolar;
}


void LLTAnalysis::initializeAnalysis()
{
	m_bWarning = m_bError = false;
	m_PlaneOppList.clear();
	initializeGeom();

	QString strUnitLabel, strange;
	Units::getAreaUnitLabel(strUnitLabel);
	strange =QString("Ref. area  = %1 ").arg(m_pWPolar->referenceArea()*Units::m2toUnit(),9,'f',3)+strUnitLabel;
	traceLog(strange+"\n");
	Units::getLengthUnitLabel(strUnitLabel);
	strange =QString("Ref. span  = %1 ").arg(m_pWPolar->referenceSpanLength()*Units::mtoUnit(),9,'f',3)+strUnitLabel;
	traceLog(strange+"\n");
	Units::getLengthUnitLabel(strUnitLabel);
	strange =QString("Ref. chord = %1 ").arg(m_pWPolar->referenceChordLength()*Units::mtoUnit(),9,'f',3)+strUnitLabel;
	traceLog(strange+"\n"+"\n");
}



/** Sends the analysis messages to the specified text output stream */
void LLTAnalysis::traceLog(QString str)
{
	m_OutMessage += str;
	qApp->processEvents();
}



/**
* Creates the plane's operating point,
* fills it with the input resulting from the  analysis, and returns a pointer to the PlaneOpp object which has been created.

*@param bPointOut: if true, part of the oppoint viscous properties could not be interpolated
*@param Cp: the array of Cp value for each panel
*@param Gamma: the array of circulation or doublet strengths Gamma for each panel
*@param Sigma: the array of source strengths for a panel analysis
*@return a pointer to the PlaneOpp object which has been created
*/
PlaneOpp* LLTAnalysis::createPlaneOpp(double QInf, double Alpha, bool bWingOut)
{
	int l;

	PlaneOpp *pNewPOpp = new PlaneOpp(m_pPlane, m_pWPolar, 0);

	if(pNewPOpp == NULL)
	{
		traceLog("Not enough memory to store the OpPoint\n");
		return NULL;
	}
	else
	{
		pNewPOpp->m_Alpha               = Alpha;
		pNewPOpp->m_QInf                = QInf;
		pNewPOpp->m_NPanels             = m_pWing->m_MatSize;
		pNewPOpp->m_bOut                = m_bWingOut;
		pNewPOpp->m_CL                  = m_CL;
		pNewPOpp->m_ICD                 = m_CDi;

		pNewPOpp->m_GCm                 = m_GCm;
		pNewPOpp->m_VCm                 = m_VCm;
		pNewPOpp->m_ICm                 = m_ICm;
		pNewPOpp->m_GRm                 = m_GRm;
		pNewPOpp->m_GYm                 = m_GYm;
		pNewPOpp->m_VYm                 = m_VYm;
		pNewPOpp->m_IYm                 = m_IYm;

		pNewPOpp->m_CP                  = m_CP;
		pNewPOpp->m_VCD                 = m_CDv;

		pNewPOpp->m_bOut                = bWingOut;


		pNewPOpp->AddWingOpp(0, 0);
		pNewPOpp->m_pPlaneWOpp[0]->createWOpp(m_pWing, m_pWPolar);

		WingOpp *pNewPoint = pNewPOpp->m_pPlaneWOpp[0];
		{
			pNewPoint->m_Alpha          = Alpha;
			pNewPoint->m_QInf           = QInf;

			pNewPoint->m_bOut           = m_bWingOut;
			pNewPoint->m_CL             = m_CL;
			pNewPoint->m_ICD            = m_CDi;

			pNewPoint->m_GCm            = m_GCm;
			pNewPoint->m_VCm            = m_VCm;
			pNewPoint->m_ICm            = m_ICm;
			pNewPoint->m_GRm            = m_GRm;
			pNewPoint->m_GYm            = m_GYm;
			pNewPoint->m_VYm            = m_VYm;
			pNewPoint->m_IYm            = m_IYm;

			pNewPoint->m_CP             = m_CP;
			pNewPoint->m_VCD            = m_CDv;

			int nStation = LLTAnalysis::s_NLLTStations;
			pNewPoint->m_NStation = nStation;

			double Cb =0.0;
			for (l=1; l<nStation; l++)
			{
				pNewPoint->m_SpanPos[l]       = -m_SpanPos[l];
				pNewPoint->m_StripArea[l]     =  m_StripArea[l];
				pNewPoint->m_Ai[l]            =  m_Ai[nStation-l];
				pNewPoint->m_Cl[l]            =  m_Cl[nStation-l];
				pNewPoint->m_PCd[l]           =  m_PCd[nStation-l];
				pNewPoint->m_ICd[l]           =  m_ICd[nStation-l];
				pNewPoint->m_Cm[l]            =  m_Cm[nStation-l];
				pNewPoint->m_CmAirf[l]        =  m_CmAirf[nStation-l];
				pNewPoint->m_XCPSpanRel[l]    =  m_XCPSpanRel[nStation-l];
				pNewPoint->m_XCPSpanAbs[l]    =  m_XCPSpanAbs[nStation-l];
				pNewPoint->m_Re[l]            =  m_Re[nStation-l];
				pNewPoint->m_Chord[l]         =  m_Chord[nStation-l];
				pNewPoint->m_Twist[l]         =  m_Twist[nStation-l];
				pNewPoint->m_XTrTop[l]        =  m_XTrTop[nStation-l];
				pNewPoint->m_XTrBot[l]        =  m_XTrBot[nStation-l];
				pNewPoint->m_BendingMoment[l] =  m_BendingMoment[nStation-l];
				if(qAbs(m_BendingMoment[l])>qAbs(Cb))	Cb = m_BendingMoment[l];
			}

			pNewPoint->m_MaxBending = (float)Cb;
		}
	}

	//add the data to the polar object
	if(PlaneOpp::s_bKeepOutOpps || !pNewPOpp->m_bOut)
		m_pWPolar->AddPlaneOpPoint(pNewPOpp);

	return pNewPOpp;
}



