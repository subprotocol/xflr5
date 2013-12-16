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
#include "../mainframe.h"
#include "LLTAnalysis.h"
#include "LLTAnalysisDlg.h"
#include <QString>


int LLTAnalysis::s_NLLTStations = 0;
double LLTAnalysis::s_RelaxMax = 0.0;
double LLTAnalysis::s_CvPrec = 0.0;


/**
 *The public constructor
 */
LLTAnalysis::LLTAnalysis(void *pParent)
{
	m_pParent = pParent;

	m_pWing = NULL;
	m_pWPolar = NULL;
	m_poaPolar = NULL;
	ResetVariables();
}

/**
* Initializes the variables to default values
*/
void LLTAnalysis::ResetVariables()
{
	m_bSkip      = false;
	m_bCancel    = false;
	m_bConverged = false;
	m_bWingOut   = false;

	m_IterLim = 0;

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
}


/** 
* Initializes the Reynolds numbers and lift coefficients for the first iteration in LLT
* @todo : useless; change processing order --> set linear solution first, QInf next
 * @param QInf the freestream velocity, in m/s
 * @param Alpha the angle of attack, in degrees
*/
void LLTAnalysis::LLTSetInitialCl(double &QInf, double const Alpha)
{
	Foil *pFoil0 = NULL;
	Foil *pFoil1 = NULL;
	double yob, tau;
	int k;
	bool bOutRe, bError;

	for (k=1; k<s_NLLTStations; k++)
	{
		yob   = cos(k*PI/s_NLLTStations);
		m_pWing->GetFoils(&pFoil0, &pFoil1, yob*m_pWing->m_PlanformSpan/2.0, tau);
		m_Re[k] = m_Chord[k] * QInf /m_pWPolar->m_Viscosity;
        m_Cl[k] = GetCl(pFoil0, pFoil1, m_Re[k], Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
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
void LLTAnalysis::LLTComputeWing(double QInf, double Alpha, QString &ErrorMessage)
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
		ViscCm[m]    = (-c4 *sin(arad) + zpos*cos(arad))* m_PCd[m];
		InducedCm[m] = m_CmAirf[m]- c4  * m_Cl[m]*cos(arad) - zpos* m_Cl[m]*sin(arad);

		eta = Eta(m);
		sigma = Sigma(m);
		Integral0           += eta   * m_Cl[m]  * m_Chord[m];
		Integral1           += sigma * m_Cl[m]  * m_Chord[m];
		Integral2           += eta   * m_Cl[m]  * m_Chord[m] * (m_Offset[m]+m_XCPSpanRel[m]*m_Chord[m]);
        Integral3           += eta   * m_Cl[m]  * m_Chord[m] * (zpos*m_Chord[m]);
//      Integral3           += eta   * m_Cl[m]  * m_Chord[m] * ((m_XCPSpanRel[m]*m_Chord[m]*cos(-m_Twist[m]*PI/180.0)+m_Offset[m]) * sin(-Alpha*PI/180.0) + (zpos*m_Chord[m]+m_XCPSpanRel[m]*m_Chord[m]*sin(-m_Twist[m]*PI/180.0)) * cos(-Alpha*PI/180.0));
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

	LLTSetBending(QInf);
}



/**
 * Calculates the bending moment at span stations, based on the results of the analysis and on the freestrem velocity.
 * @param QInf the freestream velocity, in m/s
 */
void LLTAnalysis::LLTSetBending(double QInf)
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
bool LLTAnalysis::LLTSetLinearSolution(double Alpha)
{
	double* aij = new double[s_NLLTStations*s_NLLTStations];
	double* rhs = new double[s_NLLTStations+1];

	memset(aij, 0, sizeof(aij));
	memset(rhs, 0, sizeof(rhs));

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
		ch = m_pWing->Chord(yob);      //or m_Chord[i], same
		twist = m_pWing->Twist(yob);   //or m_Twist[i], same

		st0 = sin(t0);

		for (j=1; j<s_NLLTStations; j++)
		{
			dj   = (double)j;
			snt0 = sin(dj*t0);

			p = (i-1)*size + (j-1);
			aij[p]  = snt0 + ch*PI/b/2.0* dj*snt0/st0;
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

		m_Cl[i] *= slope*180.0/PI*cs/m_pWing->Chord(yob);
		m_Ai[i]  = -(Alpha-a0+m_pWing->Twist(yob)) + m_Cl[i]/slope;
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
int LLTAnalysis::LLTIterate(double &QInf, double Alpha)
{
	int k ;
	Foil* pFoil0  = NULL;
	Foil* pFoil1  = NULL;
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

		LLTAnalysisDlg *pLLTDlg = (LLTAnalysisDlg*)m_pParent;
		pLLTDlg->UpdateGraph(iter, m_Maxa);
		iter++;
	}
	return iter;
}


/**
 * Initializes the geometric data necessary for the LLT calculation
*/
void LLTAnalysis::LLTInitializeGeom()
{
	double yj, yjm, yjp, dy;
	double y;
	int k;
	m_bWingOut = false;
	m_bConverged = false;

	if(m_pWPolar->m_WPolarType==FIXEDLIFTPOLAR)	m_QInf0 = sqrt(2.*m_pWPolar->m_Mass* 9.81 /m_pWPolar->m_Density/m_pWing->m_PlanformArea);
	else                                        m_QInf0 = 0.0;

	m_pWing->ComputeChords(s_NLLTStations, m_Chord, m_Offset);

	for (k=0; k<=s_NLLTStations; k++)
	{
		y   = cos(k*PI/s_NLLTStations)* m_pWing->m_PlanformSpan/2.0;
		m_Twist[k] = m_pWing->Twist(y);
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





