/****************************************************************************

    WPolar Class
	Copyright (C) 2005-2014 Andre Deperrois adeperrois@xflr5.com

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


#include "WPolar.h"
#include "../globals.h"
#include "../misc/Settings.h"
#include "../miarex/Objects3D.h"
#include "../misc/Units.h"
#include <math.h>
#include <QMessageBox>


/**
 * The public constructor
 */
WPolar::WPolar()
{
	m_bIsVisible  = true;
	m_bShowPoints = false;

	m_bVLM1         = true;
	m_bThinSurfaces = true;
	m_bWakeRollUp   = false;
	m_bTiltedGeom   = false;
	m_bViscous      = true;
	m_bGround       = false;
	m_bDirichlet    = true;
	m_bIgnoreBodyPanels = false;

	m_NXWakePanels    = 1;
	m_TotalWakeLength = 100.0;
	m_WakePanelFactor =   1.1;

	m_AnalysisMethod = LLTMETHOD;
	m_WPolarType     = FIXEDSPEEDPOLAR;
	m_RefAreaType = 1;
	m_Style  = 0;
	m_Width  = 1;
	m_Color = randomColor();

	m_BankAngle = 0.0;
	m_Beta      = 0.0;
	m_QInf      = 10.0;
	m_Mass      = 1.0;
	m_ASpec     = 0.0;
	m_WArea     = 0.0;
	m_WMAChord  = 0.0;
	m_WSpan     = 0.0;
	m_AMem      = 0.0;
	m_Height    = 0.0;
	m_Density   = 1.225;
	m_Viscosity = 1.5e-5;//m2/s

	m_nControls = 0;
	m_ControlGain.clear();
	m_ControlGain.resize(100);
	memset(m_EigenValue, 0, 2*8*MAXPOLARPOINTS*sizeof(double));
	
	m_bAutoInertia = true;
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz =0.0;
	m_CoG.Set(0.0,0.0,0.0);
}


void WPolar::ReplacePOppDataAt(int pos, PlaneOpp *pPOpp)
{
	if(pos<0 || pos>= m_Alpha.size()) return;

	m_Alpha[pos]      =  pPOpp->m_Alpha;
	m_QInfinite[pos]  =  pPOpp->m_QInf;
	m_CL[pos]         =  pPOpp->m_CL;
	m_CY[pos]         =  pPOpp->m_CY;
	m_ICd[pos]        =  pPOpp->m_ICD;
	m_PCd[pos]        =  pPOpp->m_VCD;
	m_TCd[pos]        =  pPOpp->m_ICD + pPOpp->m_VCD;

	m_GCm[pos]        =  pPOpp->m_GCm;
	m_VCm[pos]        =  pPOpp->m_VCm;
	m_ICm[pos]        =  pPOpp->m_ICm;
	m_GRm[pos]        =  pPOpp->m_GRm;
	m_GYm[pos]        =  pPOpp->m_GYm;
	m_VYm[pos]        =  pPOpp->m_VYm;
	m_IYm[pos]        =  pPOpp->m_IYm;

	m_XCP[pos]        =  pPOpp->m_CP.x;
	m_YCP[pos]        =  pPOpp->m_CP.x;
	m_ZCP[pos]        =  pPOpp->m_CP.z;
	m_MaxBending[pos] =  pPOpp->m_pPlaneWOpp[0]->m_MaxBending;
	m_Ctrl[pos]       =  pPOpp->m_Ctrl;
	m_XNP[pos]        =  pPOpp->m_XNP;

	//store the eigenthings
	for(int l=0; l<8; l++) m_EigenValue[l][pos] = pPOpp->m_EigenValue[l];

	CalculatePoint(pos);
}


void WPolar::InsertPOppDataAt(int pos, PlaneOpp *pPOpp)
{
	if(pos<0 || pos> m_Alpha.size()) return; // if(pos==size), then the data is appended
	
	m_Alpha.insert(pos, pPOpp->m_Alpha);
	m_QInfinite.insert(pos, pPOpp->m_QInf);
	m_CL.insert( pos,  pPOpp->m_CL);
	m_CY.insert( pos,  pPOpp->m_CY);
	m_ICd.insert(pos,  pPOpp->m_ICD);
	m_PCd.insert(pos,  pPOpp->m_VCD);
	m_TCd.insert(pos,  pPOpp->m_ICD + pPOpp->m_VCD);

	m_GCm.insert(pos,  pPOpp->m_GCm);
	m_VCm.insert(pos,  pPOpp->m_VCm);
	m_ICm.insert(pos,  pPOpp->m_ICm);
	m_GRm.insert(pos,  pPOpp->m_GRm);
	m_GYm.insert(pos,  pPOpp->m_GYm);
	m_VYm.insert(pos,  pPOpp->m_VYm);
	m_IYm.insert(pos,  pPOpp->m_IYm);

	m_XCP.insert(pos,  pPOpp->m_CP.x);
	m_YCP.insert(pos,  pPOpp->m_CP.y);
	m_ZCP.insert(pos,  pPOpp->m_CP.z);
	m_MaxBending.insert(pos, pPOpp->m_pPlaneWOpp[0]->m_MaxBending);
	m_Ctrl.insert(pos, pPOpp->m_Ctrl);
	m_XNP.insert(pos,  pPOpp->m_XNP);

	m_ShortPeriodDamping.insert(pos,0.0);
	m_ShortPeriodFrequency.insert(pos,0.0);
	m_PhugoidDamping.insert(pos,0.0);
	m_PhugoidFrequency.insert(pos,0.0);
	m_DutchRollDamping.insert(pos,0.0);
	m_DutchRollFrequency.insert(pos,0.0);
	m_RollDamping.insert(pos,0.0);
	m_SpiralDamping.insert(pos,0.0);

	for(int l=0; l<8; l++)
		for(int j=m_Alpha.size(); j>pos; j--)
		{
			m_EigenValue[l][j] = m_EigenValue[l][j-1];
		}

	//store the eigenthings
	for(int l=0; l<8; l++)	m_EigenValue[l][pos] = pPOpp->m_EigenValue[l];

	//make room for computed values
	m_1Cl.insert(pos,0.0);
	m_ClCd.insert(pos,0.0);
	m_Cl32Cd.insert(pos,0.0);
	m_Vx.insert(pos,0.0);
	m_Vz.insert(pos,0.0);
	m_FZ.insert(pos,0.0);
	m_FY.insert(pos,0.0);
	m_FX.insert(pos,0.0);
	m_Gamma.insert(pos,0.0);
	m_Rm.insert(pos, 0.0);
	m_Pm.insert(pos, 0.0);
	m_Ym.insert(pos, 0.0);
	m_VertPower.insert(pos, 0.0);
	m_Oswald.insert(pos, 0.0);
	m_SM.insert(pos, 0.0);
	
	CalculatePoint(pos);
}



void WPolar::InsertDataAt(int pos, double Alpha, double QInf, double Ctrl, double Cl, double CY, double ICd, double PCd, double GCm, 
								   double ICm, double VCm, double GRm, double GYm, double IYm, double VYm, double XCP, double YCP,
								   double ZCP, double Cb, double XNP)
{
	if(pos<0 || pos>m_Alpha.size()) return;
	
	m_Alpha.insert(pos, Alpha);
	m_CL.insert(pos, Cl);
	m_CY.insert(pos, CY);
	m_ICd.insert(pos, ICd);
	m_PCd.insert(pos, PCd);
	m_TCd.insert(pos, ICd+PCd);

	m_GCm.insert(pos, GCm);
	m_VCm.insert(pos, VCm);
	m_ICm.insert(pos, ICm);
	m_GRm.insert(pos, GRm);
	m_GYm.insert(pos, GYm);
	m_VYm.insert(pos, VYm);
	m_IYm.insert(pos, IYm);

	m_QInfinite.insert(pos, QInf);

	m_XCP.insert(pos, XCP);
	m_YCP.insert(pos, YCP);
	m_ZCP.insert(pos, ZCP);
	m_MaxBending.insert(pos, Cb);
	m_Ctrl.insert(pos, Ctrl);
	m_XNP.insert(pos, XNP);

	m_PhugoidDamping.insert(pos, 0.0);
	m_PhugoidFrequency.insert(pos, 0.0);
	m_ShortPeriodDamping.insert(pos, 0.0);
	m_ShortPeriodFrequency.insert(pos, 0.0);
	m_DutchRollDamping.insert(pos, 0.0);
	m_DutchRollFrequency.insert(pos, 0.0);
	m_RollDamping.insert(pos, 0.0);
	m_SpiralDamping.insert(pos, 0.0);

	m_1Cl.insert(pos, 0.0);
	m_ClCd.insert(pos, 0.0);
	m_Cl32Cd.insert(pos, 0.0);
	m_Vx.insert(pos, 0.0);
	m_Vz.insert(pos, 0.0);
	m_FZ.insert(pos, 0.0);
	m_FY.insert(pos, 0.0);
	m_FX.insert(pos, 0.0);
	m_Gamma.insert(pos, 0.0);
	m_Rm.insert(pos, 0.0);
	m_Pm.insert(pos, 0.0);
	m_Ym.insert(pos, 0.0);
	m_VertPower.insert(pos, 0.0);
	m_Oswald.insert(pos, 0.0);
	m_SM.insert(pos, 0.0);

}



/**
 * Adds the data from the instance of the operating point referenced by pPOpp to the polar object.
 * The index used to insert the data is the aoa, or the velocity, or the control parameter, depending on the polar type.
 * If a point with identical index exists, the data is replaced.
 * If not, the data is inserted for this index.
 *
 * @param pPOpp the plane operating point from which the data is to be extracted
 */
void WPolar::AddPlaneOpPoint(PlaneOpp *pPOpp)
{
	bool bInserted = false;
	int i;
	int size = m_Alpha.size();

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_WPolarType<FIXEDAOAPOLAR)
			{
                if (qAbs(pPOpp->m_Alpha-m_Alpha[i]) < 0.001)
				{
					ReplacePOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Alpha < m_Alpha[i])
				{
					InsertPOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
			}
			else if(m_WPolarType==FIXEDAOAPOLAR)
			{
				// type 4, sort by speed
                if (qAbs(pPOpp->m_QInf - m_QInfinite[i]) < 0.001)
				{
					// then erase former result
					ReplacePOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
				else if (pPOpp->m_QInf < m_QInfinite[i])
				{
					// sort by crescending speed
					InsertPOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
			}
			else if(m_WPolarType==STABILITYPOLAR)
			{
				// Control or stability analysis, sort by control value
                if (qAbs(pPOpp->m_Alpha - m_Alpha[i])<0.0001)
				{
					// then erase former result
					ReplacePOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Ctrl < m_Ctrl[i])
				{
					// sort by crescending control values
					InsertPOppDataAt(i, pPOpp);
					bInserted = true;
					break;
				}
			}
		}
	}

	if(!bInserted)
	{
		// data is appended at the end
		int size = m_Alpha.size();
		InsertPOppDataAt(size, pPOpp);
	}
}



/**
 * Calculates aerodynamic values for the i-th point in the array : glide ratio, power factor, forces and moments, power
 * for horizontal flight, efficiency coefficient, mode frequencies and amping factors.
 * @param i the index of the point for which the values are to be calculated
 */
void WPolar::CalculatePoint(int i)
{
	//finish calculations
//	double cl = m_CL[i];
//	double tcd = m_TCd[i];
//	double Qinf =  m_QInfinite[i];

	m_ClCd[i]   =  m_CL[i]/m_TCd[i];

	if(m_CL[i]>0.0) {
		m_1Cl[i]    = (double)(1./sqrt(m_CL[i]));
		m_Cl32Cd[i] = (double)pow(m_CL[i],1.5)/m_TCd[i];
	}
	else {
		m_1Cl[i]    = -1.0;//will not be plotted
		m_Cl32Cd[i] =  -(double)pow(-m_CL[i],1.5)/m_TCd[i];
	}

    if(qAbs(m_CL[i])>0.) m_Gamma[i]  =  atan(m_TCd[i]/m_CL[i]) * 180.0/PI;
	else m_Gamma[i] = 90.0;
	m_Vz[i] = (double)sqrt(2*m_Mass*9.81/m_Density/m_WArea)/m_Cl32Cd[i];
	m_Vx[i] = m_QInfinite[i] * (double)cos(m_Gamma[i]*PI/180.0);

	//dynamic pressure
	double q =  0.5 * m_Density * m_QInfinite[i]*m_QInfinite[i];

	m_FZ[i]  = q * m_CL[i]*m_WArea;
	m_FY[i]  = q * m_CY[i]*m_WArea;
	m_FX[i]  = q * m_TCd[i]*m_WArea;

	m_Rm[i] = q * m_WArea * m_GRm[i] * m_WSpan;// in N.m
	m_Ym[i] = q * m_WArea * m_GYm[i] * m_WSpan;// in N.m
	m_Pm[i] = q * m_WArea * m_GCm[i] * m_WMAChord;// in N.m

	//power for horizontal flight
	m_VertPower[i] = m_Mass * 9.81 * m_Vz[i];

	double AR      = m_WSpan*m_WSpan/m_WArea;

	if(m_ICd[i]==0.0)	m_Oswald[i] = 0.0;
	else				m_Oswald[i] = m_CL[i]*m_CL[i]/PI/m_ICd[i]/AR;

	m_SM[i]        = (m_XCP[i]-m_CoG.x)/m_WMAChord *100.00;

	complex<double> c;
	double OmegaN, Omega1, Dsi, Sigma1;
	double sum, prod;

	c = m_EigenValue[2][i];
	sum  = c.real() * 2.0;                          // is a real number
	prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
    OmegaN = qAbs(c.imag());
	if(OmegaN>PRECISION) Omega1 = sqrt(prod);
	else                 Omega1 = 0.0;
	Sigma1 = sum /2.0;
	if(Omega1 > PRECISION) Dsi = -Sigma1/Omega1;
	else                   Dsi = 0.0;
	m_PhugoidDamping[i] = Dsi;
    m_PhugoidFrequency[i] = qAbs(c.imag()/2.0/PI);

	c = m_EigenValue[0][i];
	sum  = c.real() * 2.0;                          // is a real number
	prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
    OmegaN = qAbs(c.imag());
	if(OmegaN>PRECISION) Omega1 = sqrt(prod);
	else                 Omega1 = 0.0;
	Sigma1 = sum /2.0;
	if(Omega1 > PRECISION) Dsi = -Sigma1/Omega1;
	else                   Dsi = 0.0;
	m_ShortPeriodDamping[i] = Dsi;
    m_ShortPeriodFrequency[i] = qAbs(c.imag()/2.0/PI);

	c = m_EigenValue[5][i];
	sum  = c.real() * 2.0;                          // is a real number
	prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
    OmegaN = qAbs(c.imag());
	if(OmegaN>PRECISION) Omega1 = sqrt(prod);
	else                 Omega1 = 0.0;
	Sigma1 = sum /2.0;
	if(Omega1 > PRECISION) Dsi = -Sigma1/Omega1;
	else                   Dsi = 0.0;
	m_DutchRollDamping[i] = Dsi;
    m_DutchRollFrequency[i] = qAbs(c.imag()/2.0/PI);

	m_RollDamping[i]= m_EigenValue[4][i].real();
	m_SpiralDamping[i]= m_EigenValue[7][i].real();
}


/*	N =  Cn.q.s.b
	L =  Ct.q.s.b
	M =  Cm.q.s.c'
*/

/**
 * Copies the polar's analysis parameters from an existing polar
 * @param pWPolar a pointer to the instance of the reference CWPolar object from which the parameters should be copied
 */
void WPolar::DuplicateSpec(WPolar *pWPolar)
{
	m_PlaneName   = pWPolar->m_PlaneName;
	m_WPlrName   = pWPolar->m_WPlrName;

	m_WPolarType  = pWPolar->m_WPolarType;

	m_QInf      = pWPolar->m_QInf;
	m_ASpec     = pWPolar->m_ASpec;
	m_AMem      = pWPolar->m_AMem;
	m_Beta      = pWPolar->m_Beta;

	m_Style  = pWPolar->m_Style;
	m_Width  = pWPolar->m_Width;
	m_Color  = pWPolar->m_Color;


	// general aerodynamic data - specific to a polar
	m_Viscosity   = pWPolar->m_Viscosity;
	m_Density     = pWPolar->m_Density ;
	m_Height      = pWPolar->m_Height;//for ground effect
	m_BankAngle   = pWPolar->m_BankAngle;


	m_NXWakePanels      = pWPolar->m_NXWakePanels;
	m_TotalWakeLength   = pWPolar->m_TotalWakeLength;
	m_WakePanelFactor   = pWPolar->m_WakePanelFactor;

	m_bGround         = pWPolar->m_bGround;
	m_bDirichlet      = pWPolar->m_bDirichlet;
	m_bIsVisible      = pWPolar->m_bIsVisible;
	m_bShowPoints     = pWPolar->m_bShowPoints;
	m_bTiltedGeom     = pWPolar->m_bTiltedGeom;
	m_bViscous        = pWPolar->m_bViscous;
	m_bIgnoreBodyPanels = pWPolar->m_bIgnoreBodyPanels;
	m_bVLM1           = pWPolar->m_bVLM1;
	m_bWakeRollUp     = pWPolar->m_bWakeRollUp;
	m_AnalysisMethod  = pWPolar->m_AnalysisMethod;
	m_bThinSurfaces   = pWPolar->m_bThinSurfaces;
	m_bVLM1           = pWPolar->m_bVLM1;


	m_nControls       = pWPolar->m_nControls;
	m_ControlGain.clear();
	for(int icg=0; icg<pWPolar->m_ControlGain.size(); icg++)
		m_ControlGain.append(pWPolar->m_ControlGain.at(icg));



	m_RefAreaType = pWPolar->m_RefAreaType;
	m_WArea       = pWPolar->m_WArea;//for lift and drag calculations
	m_WMAChord    = pWPolar->m_WMAChord;// for moment calculations
	m_WSpan       = pWPolar->m_WSpan;//for moment calculations

	//Inertia properties
	m_Mass = pWPolar->m_Mass;
	m_bAutoInertia = pWPolar->m_bAutoInertia;
	m_bThinSurfaces = pWPolar->m_bThinSurfaces;
	m_CoGIxx = pWPolar->m_CoGIxx;
	m_CoGIyy = pWPolar->m_CoGIyy;
	m_CoGIzz = pWPolar->m_CoGIzz;
	m_CoGIxz = pWPolar->m_CoGIxz;

	m_CoG = pWPolar->m_CoG;

}





/**
 * Exports the data of the polar to a text file
 * @param out the instance of output QtextStream
 * @param FileType TXT if the data is separated by spaces, CSV for a comma separator
 * @param bDataOnly true if the analysis parameters should not be output
 */
void WPolar::Export(QTextStream &out, enumTextFileType FileType, bool bDataOnly)
{
	int j;
	QString Header, strong, str;

	if (FileType==TXT)
	{
		if(!bDataOnly)
		{
			strong = VERSIONNAME;
			strong += "\n\n";
			out << strong;

			strong ="Wing name :        "+ m_PlaneName + "\n";
			out << strong;

			strong ="Wing polar name :  "+ m_WPlrName + "\n";
			out << strong;

			Units::getSpeedUnitLabel(str);
			str +="\n\n";

			if(m_WPolarType==FIXEDSPEEDPOLAR)
			{
				strong = QString("Freestream speed : %1 ").arg(m_QInf*Units::mstoUnit(),7,'f',3);
				strong +=str + "\n";
			}
			else if(m_WPolarType==FIXEDAOAPOLAR)
			{
				strong = QString("Alpha = %1").arg(m_ASpec) + QString::fromUtf8("°") + "\n";
			}
			else strong = "\n";

			out << strong;
		}

		Header = "   alpha      CL          CDi        CDv        CD         CY         Cl         Cm         Cn        Cni       QInf        XCP\n";
		out << Header;
		for (j=0; j<m_Alpha.size(); j++)
		{
			strong = QString(" %1  %2  %3  %4  %5  %6  %7  %8  %9  %10  %11  %12\n")
					 .arg(m_Alpha[j],8,'f',3)
					 .arg(m_CL[j], 9,'f',6)
					 .arg(m_ICd[j],9,'f',6)
					 .arg(m_PCd[j],9,'f',6)
					 .arg(m_TCd[j],9,'f',6)
					 .arg(m_CY[j] ,9,'f',6)
					 .arg(m_GRm[j],9,'f',6)
					 .arg(m_GCm[j],9,'f',6)
					 .arg(m_GYm[j],9,'f',6)
					 .arg(m_IYm[j],9,'f',6)
					 .arg(m_QInfinite[j],8,'f',4)
					 .arg(m_XCP[j],9,'f',4);

			out << strong;
		}
	}
	else if (FileType==CSV)
	{
		if(!bDataOnly)
		{
			strong = VERSIONNAME;
			strong += "\n\n";
			out << strong;

			strong ="Wing name :, "+ m_PlaneName + "\n";
			out << strong;

			strong ="Wing polar name :, "+ m_WPlrName + "\n";
			out << strong;

			Units::getSpeedUnitLabel(str);
			str +="\n\n";
			strong = QString("Freestream speed :, %1 ").arg(m_QInf*Units::mstoUnit(),3,'f',1);
			strong +=str;
			out << strong;
		}

		Header = "alpha, CL, CDi, CDv, CD, CY, Cl, Cm, Cn, Cni, QInf, XCP\n";
		out << Header;
		for (j=0; j<m_Alpha.size(); j++)
		{
//			strong.Format(" %8.3f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %8.4f,  %9.4f\n",
			strong = QString(" %1,  %2,  %3,  %4,  %5,  %6,  %7,  %8,  %9,  %10,  %11,  %12\n")
					 .arg(m_Alpha[j],8,'f',3)
					 .arg(m_CL[j], 9,'f',6)
					 .arg(m_ICd[j],9,'f',6)
					 .arg(m_PCd[j],9,'f',6)
					 .arg(m_TCd[j],9,'f',6)
					 .arg(m_CY[j] ,9,'f',6)
					 .arg(m_GRm[j],9,'f',6)
					 .arg(m_GCm[j],9,'f',6)
					 .arg(m_GYm[j],9,'f',6)
					 .arg(m_IYm[j],9,'f',6)
					 .arg(m_QInfinite[j],8,'f',4)
					 .arg(m_XCP[j],9,'f',4);

			out << strong;

		}
	}
	out << "\n\n";
}



/**
 * Returns a pointer to the QList array of data for the variable referenced by iVar
 * @param iVar the index of the variable
 * @return a void pointer to the array of data
 */
void * WPolar::GetWPlrVariable(int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;
	switch (iVar)
	{
		case 0:
			pVar = &m_Alpha;
			break;
		case 1:
			pVar = &m_CL;
			break;
		case 2:
			pVar = &m_TCd;
			break;
		case 3:
			pVar = &m_PCd;
			break;
		case 4:
			pVar = &m_ICd;
			break;
		case 5:
			pVar = &m_CY;
			break;
		case 6:
			pVar = &m_GCm;
			break;
		case 7:
			pVar = &m_VCm;
			break;
		case 8:
			pVar = &m_ICm;
			break;
		case 9:
			pVar = &m_GRm;
			break;
		case 10:
			pVar = &m_GYm;
			break;
		case 11:
			pVar = &m_VYm;
			break;
		case 12:
			pVar = &m_IYm;
			break;
		case 13:
			pVar = &m_ClCd;
			break;
		case 14:
			pVar = &m_Cl32Cd;
			break;
		case 15:
			pVar = &m_1Cl;
			break;
		case 16:
			pVar = &m_FX;
			break;
		case 17:
			pVar = &m_FY;
			break;
		case 18:
			pVar = &m_FZ;
			break;
		case 19:
			pVar = &m_Vx;
			break;
		case 20:
			pVar = &m_Vz;
			break;
		case 21:
			pVar = &m_QInfinite;
			break;
		case 22:
			pVar = &m_Gamma;
			break;
		case 23:
			pVar = &m_Rm;
			break;
		case 24:
			pVar = &m_Pm;
			break;
		case 25:
			pVar = &m_Ym;
			break;
		case 26:
			pVar = &m_XCP;
			break;
		case 27:
			pVar = &m_YCP;
			break;
		case 28:
			pVar = &m_ZCP;
			break;
		case 29:
			pVar = &m_MaxBending;
			break;
		case 30:
			pVar = &m_VertPower;
			break;
		case 31:
			pVar = &m_Oswald;
			break;
		case 32:
			pVar = &m_SM;
			break;
		case 33:
			pVar = &m_Ctrl;
			break;
		case 34:
			pVar = &m_XNP;
			break;
		case 35:
			pVar = &m_PhugoidFrequency;
			break;
		case 36:
			pVar = &m_PhugoidDamping;
			break;
		case 37:
			pVar = &m_ShortPeriodFrequency;
			break;
		case 38:
			pVar = &m_ShortPeriodDamping;
			break;
		case 39:
			pVar = &m_DutchRollFrequency;
			break;
		case 40:
			pVar = &m_DutchRollDamping;
			break;
		case 41:
			pVar = &m_RollDamping;
			break;
		case 42:
			pVar = &m_SpiralDamping;
			break;
		default:
			pVar = &m_Alpha;
			break;
	}
	return pVar;
}


/**
 * Returns the name of the variable referenced by iVar
 * @param iVar the index of the variable
 * @param Name the name of the variable as a QString object
 */
QString WPolar::variableName(int iVar)
{
	QString StrLength;
	QString StrSpeed;
	QString StrMoment;
	Units::getLengthUnitLabel(StrLength);
	Units::getSpeedUnitLabel(StrSpeed);
	Units::getMomentUnitLabel(StrMoment);

	switch (iVar)
	{
		case 0:
			return "Alpha";
			break;
		case 1:
			return "CL";
			break;
		case 2:
			return "CD";
			break;
		case 3:
			return "CD_viscous";
			break;
		case 4:
			return "CD_induced";
			break;
		case 5:
			return "CY";
			break;
		case 6:
			return "Cm";// Total Pitching moment coef.
			break;
		case 7:
			return "Cm_viscous";// Viscous Pitching moment coef.
			break;
		case 8:
			return "Cm_induced";// Induced Pitching moment coef.
			break;
		case 9:
			return "Cl";// Total Rolling moment coef.
			break;
		case 10:
			return "Cn";// Total Yawing moment coef.
			break;
		case 11:
			return "Cn_viscous";// Profile yawing moment
			break;
		case 12:
			return "Cn_induced";// Induced yawing moment
			break;
		case 13:
			return "CL/CD";
			break;
		case 14:
			return "CL^(3/2)/CD";
			break;
		case 15:
			return "1/Rt(CL)";
			break;
		case 16:
			if(Units::forceUnitIndex()==0) return "Fx (N)";
			else                           return "Fx (lbf)";
			break;
		case 17:
			if(Units::forceUnitIndex()==0) return "Fy (N)";
			else                           return "Fy (lbf)";
			break;
		case 18:
			if(Units::forceUnitIndex()==0) return "Fz (N)";
			else                           return "Fz (lbf)";
			break;
		case 19:
			return "Vx ("+StrSpeed+")";
			break;
		case 20:
			return "Vz ("+StrSpeed+")";
			break;
		case 21:
			return "V ("+StrSpeed+")";
			break;
		case 22:
			return "Gamma";
			break;
		case 23:
			return "L ("+ StrMoment+")";
			break;
		case 24:
			return "M ("+ StrMoment+")";
			break;
		case 25:
			return "N ("+ StrMoment+")";
			break;
		case 26:
			return "CPx ("+ StrLength+")";
			break;
		case 27:
			return "CPy ("+ StrLength+")";
			break;
		case 28:
			return "CPz ("+ StrLength+")";
			break;
		case 29:
			return "BM ("+ StrMoment+")";
			break;
		case 30:
			return "m.g.Vz (W)";
			break;
		case 31:
			return "Efficiency";
			break;
		case 32:
			return "(XCp-XCG)/MAC(%)";
			break;
		case 33:
			return "ctrl";
			break;
		case 34:
			return "XNP ("+ StrLength+")";
			break;
		case 35:
			return "Phugoid Freq. (Hz)";
			break;
		case 36:
			return "Phugoid Damping";
			break;
		case 37:
			return "Short Period Freq. (Hz)";
			break;
		case 38:
			return "Short Period Damping";
			break;
		case 39:
			return "Dutch Roll Freq. (Hz)";
			break;
		case 40:
			return "Dutch Roll Damping";
			break;
		case 41:
			return "Roll Damping";
			break;
		case 42:
			return "Spiral Damping";
			break;
		default:
			return "";
	}
	return "";
}



/**
 * Removes the data for the point with aoa alpha
 * @param alpha the aoa of the point to be deleted
 **/
void WPolar::Remove(double alpha)
{
	for(int ia=0;ia<m_Alpha.size(); ia++)
	{
        if(qAbs(m_Alpha.at(ia)-alpha)<PRECISION)
		{
			Remove(ia);
			break;
		}
	}
}


/**
 * Removes the data at index i of the data arrays
 * @param i the index at which the data is to be deleted
 **/
void WPolar::Remove(int i)
{
	int size = m_Alpha.size();
	m_Alpha.removeAt(i);
	m_CL.removeAt(i);
	m_CY.removeAt(i);
	m_ICd.removeAt(i);
	m_PCd.removeAt(i);
	m_TCd.removeAt(i);

	m_GCm.removeAt(i);
	m_VCm.removeAt(i);
	m_ICm.removeAt(i);
	m_GRm.removeAt(i);
	m_GYm.removeAt(i);
	m_VYm.removeAt(i);
	m_IYm.removeAt(i);

	m_XCP.removeAt(i);
	m_YCP.removeAt(i);
	m_ZCP.removeAt(i);
	m_MaxBending.removeAt(i);
	m_VertPower.removeAt(i);

	m_Oswald.removeAt(i);
	m_SM.removeAt(i);
	m_Ctrl.removeAt(i);
	m_XNP.removeAt(i);
	m_ShortPeriodDamping.removeAt(i);
	m_ShortPeriodFrequency.removeAt(i);
	m_PhugoidFrequency.removeAt(i);
	m_PhugoidDamping.removeAt(i);

	m_DutchRollDamping.removeAt(i);
	m_DutchRollFrequency.removeAt(i);
	m_RollDamping.removeAt(i);
	m_SpiralDamping.removeAt(i);

	m_ClCd.removeAt(i);
	m_1Cl.removeAt(i);
	m_Cl32Cd.removeAt(i);

	m_QInfinite.removeAt(i);
	m_Gamma.removeAt(i);
	m_FZ.removeAt(i);
	m_FY.removeAt(i);
	m_FX.removeAt(i);
	m_Vx.removeAt(i);

	m_Vz.removeAt(i);
	m_Pm.removeAt(i);
	m_Ym.removeAt(i);
	m_Rm.removeAt(i);

	for(int j=i; j<size; j++)
	{
		for(int l=0; l<8; l++)
			m_EigenValue[l][j] = m_EigenValue[l][j+1];
	}
}


/**
 *Clears the content of the data arrays
*/
void WPolar::ClearData()
{
	int size = m_Alpha.size();
	m_Alpha.clear();
	m_CL.clear();
	m_CY.clear();
	m_ICd.clear();
	m_PCd.clear();
	m_TCd.clear();

	m_GCm.clear();
	m_VCm.clear();
	m_ICm.clear();
	m_GRm.clear();
	m_GYm.clear();
	m_VYm.clear();
	m_IYm.clear();

	m_XCP.clear();
	m_YCP.clear();
	m_ZCP.clear();
	m_MaxBending.clear();
	m_VertPower.clear();

	m_Oswald.clear();
	m_SM.clear();
	m_Ctrl.clear();
	m_XNP.clear();
	m_ShortPeriodDamping.clear();
	m_ShortPeriodFrequency.clear();
	m_PhugoidDamping.clear();
	m_PhugoidFrequency.clear();

	m_DutchRollDamping.clear();
	m_DutchRollFrequency.clear();
	m_RollDamping.clear();
	m_SpiralDamping.clear();

	m_ClCd.clear();
	m_1Cl.clear();
	m_Cl32Cd.clear();

	m_QInfinite.clear();
	m_Gamma.clear();
	m_FZ.clear();
	m_FY.clear();
	m_FX.clear();
	m_Vx.clear();

	m_Vz.clear();
	m_Pm.clear();
	m_Ym.clear();
	m_Rm.clear();

	for(int l=0; l<8; l++)
		for(int j=0; j<size; j++)
			m_EigenValue[l][j] = 0.0;
}


/**
 * Loads or saves the data of this polar to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool WPolar::SerializeWPlrWPA(QDataStream &ar, bool bIsStoring)
{
	int n;
	float f,r0,r1,r2,r3,i0,i1,i2,i3;
	int i, j, k;

    m_PolarFormat = 1024;
    // 1024 : added ignore body flag
	// 1023 : added ZCP position
	// 1022 : added XNP position and provision for 50 more variables
	// 1021 : XFLR5 v6.02 - deleted autoinertia for older format polars
	// 1020 : QFLR6 v0.00 - added inertia tensor values
	// 1019 : QFLR6 v0.00 - added eigenvalues
	// 1018 : QFLR5 v0.04 - replaced m_XcmRef by m_Cog
	// 1017 : QFLR5 v0.03 - added viscous and induced pitching moments
	// 1016 : added lateral force coefficient
	// 1015 : added lateral force coefficient
	// 1014 : added control results
	// 1013 : added control variables
	// 1012 : redefined the moment coefficients
	// 1011 : added wake roll-up parameters
	// 1010 : added ground effect variables langth changed length unit to m
	// 1009 : added viscous flag
	// 1008 : added Tilted Geometry flag
	// 1007 : added NXWakePanels
	// 1006 : added Wake Roll Up flag
	// 1005 : added VLM method types
	// 1004 : with corrected PCd calculation
	// 1003 : added bending moment V18
	// 1002 : added XCmRef
	// 1001 : v0.00

	if(bIsStoring)
	{
		// not storing to .wpa format anymore
		return true;
	}
	else
	{
		//read variables
		ar >> m_PolarFormat;
		if (m_PolarFormat <=1000 || m_PolarFormat>1100)
		{
			m_WPlrName ="";
			return false;
		}
		ReadCString(ar, m_PlaneName);
		ReadCString(ar, m_WPlrName);

		ar>> f;
		m_WArea = f;
		if (m_WArea<0) return false;

		ar>> f;
		m_WMAChord = f;
		if (m_WMAChord<0) return false;

		ar>> f;
		m_WSpan = f;
		if (m_WSpan<0) return false;

		ar >> m_Style  >> m_Width;
		if (m_Style<0 || m_Style> 10) return false;

		if (m_Width<0 || m_Width> 10) return false;

		ReadCOLORREF(ar, m_Color);

		ar>>k;
		if(k==1)      m_AnalysisMethod=LLTMETHOD;
		else if(k==2) m_AnalysisMethod=VLMMETHOD;
		else if(k==3) m_AnalysisMethod=PANELMETHOD;
		else if(k==4) m_AnalysisMethod=VLMMETHOD;

		if(m_AnalysisMethod==VLMMETHOD)
		{
			m_AnalysisMethod=PANELMETHOD;
			m_bThinSurfaces = true;
		}

		if(m_PolarFormat>=1005)
		{
			ar >> n;
			if (n!=0 && n!=1) return false;

			if(n) m_bVLM1 =true; else m_bVLM1 = false;
			ar >> n;
			if (n!=0 && n!=1) return false;

			if(n) m_bThinSurfaces =true; else m_bThinSurfaces = false;
		}
		if(m_PolarFormat>=1008)
		{
			ar >> n;
			if (n!=0 && n!=1) return false;
			if(n) m_bTiltedGeom =true; else m_bTiltedGeom = false;
		}

		if(m_PolarFormat>=1006)
		{
			ar >> n;
			if (n!=0 && n!=1) return false;
			if(n) m_bDirichlet = false; else m_bDirichlet = true;
		}
		if(m_PolarFormat>=1009)
		{
			ar >> n;
			if (n!=0 && n!=1) return false;
			if(n) m_bViscous =true; else m_bViscous = false;
		}
/*		if(m_PolarFormat>=1024)
		{
            ar >> n;
            if (n!=0 && n!=1) return false;
            if(n) m_bIgnoreBody =true; else m_bIgnoreBody = false;
        }*/

		if(m_PolarFormat>=1010)
		{
			ar >> n;
			if (n!=0 && n!=1) return false;

			if(n) m_bGround =true; else m_bGround = false;
			ar >> f; m_Height = f;
		}

		if(m_PolarFormat>=1007)
		{
			ar >> m_NXWakePanels;
			if (m_NXWakePanels<0 || m_NXWakePanels>1000) return false;
		}
		if(m_PolarFormat>=1011)
		{
			ar >> f; 			m_TotalWakeLength = f;
			ar >> f; 			m_WakePanelFactor = f;
		}

		ar >> n;
		if (n!=0 && n!=1) return false;
		else {
			if(n) m_bIsVisible =true; else m_bIsVisible = false;
		}
		ar >> n;
		if (n!=0 && n!=1) return false;
		else {
			if(n) m_bShowPoints =true; else m_bShowPoints = false;
		}

		ar >>k;
		if(k==1)      m_WPolarType = FIXEDSPEEDPOLAR;
		else if(k==2) m_WPolarType = FIXEDLIFTPOLAR;
		else if(k==4) m_WPolarType = FIXEDAOAPOLAR;
		else if(k==6) m_WPolarType = STABILITYPOLAR; // former control polars
		else if(k==7) m_WPolarType = STABILITYPOLAR;
		else return false;


		ar >> f;	m_QInf = f;
		ar >> f;	m_Mass = f;
		ar >> f;	m_ASpec = f;
		if(m_PolarFormat>=1015)
		{
			ar >> f;
			m_Beta = f;
		}
		else m_Beta = 0.0;
		if(m_PolarFormat<1018 && m_PolarFormat>=1002)
		{
			ar >> f;			m_CoG.x = f;
		}
		else if(m_PolarFormat>=1018)
		{
			ar >> f;			m_CoG.x = f;
			ar >> f;			m_CoG.y = f;
			ar >> f;			m_CoG.z = f;
		}
//		if(m_PolarFormat>=1002) ar >> f; m_XCmRef = f;
		ar >> f;	m_Density=f;
		ar >> f;	m_Viscosity=f;

		if(m_PolarFormat>=1016) ar >> m_RefAreaType;
		else                    m_RefAreaType = 1;

		ar >> n;
		if (n<0 || n> 100000) return false;

		if(m_PolarFormat<1010)
		{
			m_WArea    /=100.0;
			m_WMAChord /=1000.0;
			m_WSpan    /=1000.0;
			m_CoG.x   /=1000.0;
		}
		float Alpha,  Cl, CY, ICd, PCd, GCm, GRm, GYm, VCm, ICm, VYm, IYm, QInfinite, XCP, YCP, ZCP, Ctrl, Cb, XNP;
		f = Alpha =  Cl = CY = ICd = PCd = GCm = GRm = GYm = VCm = ICm = VYm = IYm = QInfinite = XCP = YCP = ZCP = Ctrl = Cb =0.0;
//		bool bExists;
		for (i=0; i< n; i++)
		{
			ar >> Alpha >> Cl;
			if(m_PolarFormat>=1015) ar>>CY;
			ar >> ICd >> PCd;
			ar >> GCm;
			if(m_PolarFormat>=1017) ar >> VCm >> ICm;
			ar >> GRm >> GYm >> f >> VYm >> IYm;
			if(m_PolarFormat<1012) GCm = GRm = GYm = VCm = VYm = IYm = 0.0;
			ar >> QInfinite >> XCP >> YCP;
			if (m_PolarFormat>=1023) ar >> ZCP;

			if(m_PolarFormat<1010)
			{
				XCP   /=1000.0;
				YCP   /=1000.0;
			}

			if (m_PolarFormat>=1003)
				ar >> Cb;
			else
				Cb = 0.0;

			if (m_PolarFormat>=1014) ar >> Ctrl;
			else                     Ctrl = 0.0;

			if (m_PolarFormat>=1022) ar >> XNP;
			else                     XNP = 0.0;

			if(m_WPolarType!=FIXEDAOAPOLAR)
			{
				for (j=0; j<m_Alpha.size(); j++)
				{
                    if(qAbs(Alpha-m_Alpha[j])<0.001)
					{
						break;
					}
				}
			}
			else
			{
				for (j=0; j<m_Alpha.size(); j++)
				{
                    if(qAbs(QInfinite-m_QInfinite[j])<0.001)
					{
						break;
					}
				}
			}

			m_Alpha.append(Alpha);
			m_CL.append(Cl);
			m_CY.append(CY);
			m_ICd.append(ICd);
			m_PCd.append(PCd);
			m_TCd.append(ICd+PCd);

			m_GCm.append(GCm);
			m_VCm.append(VCm);
			m_ICm.append(ICm);
			m_GRm.append(GRm);
			m_GYm.append(GYm);
			m_VYm.append(VYm);
			m_IYm.append(IYm);

			m_QInfinite.append(QInfinite);

			m_XCP.append(XCP);
			m_YCP.append(YCP);
			m_ZCP.append(ZCP);
			m_MaxBending.append(Cb);
			m_Ctrl.append(Ctrl);
			m_XNP.append(XNP);

			m_PhugoidDamping.append(0.0);
			m_PhugoidFrequency.append(0.0);
			m_ShortPeriodDamping.append(0.0);
			m_ShortPeriodFrequency.append(0.0);
			m_DutchRollDamping.append(0.0);
			m_DutchRollFrequency.append(0.0);
			m_RollDamping.append(0.0);
			m_SpiralDamping.append(0.0);


			m_1Cl.append(0.0);
			m_ClCd.append(0.0);
			m_Cl32Cd.append(0.0);
			m_Vx.append(0.0);
			m_Vz.append(0.0);
			m_FZ.append(0.0);
			m_FY.append(0.0);
			m_FX.append(0.0);
			m_Gamma.append(0.0);
			m_Rm.append(0.0);
			m_Pm.append(0.0);
			m_Ym.append(0.0);
			m_VertPower.append(0.0);
			m_Oswald.append(0.0);
			m_SM.append(0.0);
			
	 /** @todo replace with InsertDataAt(i, ...); */

		}
		if(m_PolarFormat>1012)
		{
			ar >> m_nControls;
			if(abs(m_nControls)>1000) m_nControls = 0;
			m_ControlGain.clear();
			for(i=0; i<m_nControls; i++)
			{
				ar >> f; //m_MinControl[i] = f;
				ar >> f; m_ControlGain.append(f);
			}
			for(i=0; i<m_nControls; i++)
			{
				ar >> n;
				if (n!=0 && n!=1) return false;
				else {
//					if(n) m_bActiveControl[i] =true; else m_bActiveControl[i] = false;
				}
			}
		}
		if(m_PolarFormat>=1019)
		{
			n = m_Alpha.size();

			for(i=0; i<n; i++)
			{
				ar>>r0>>r1>>r2>>r3;
				ar>>i0>>i1>>i2>>i3;

				m_EigenValue[0][i] = complex<double>(r0,i0);
				m_EigenValue[1][i] = complex<double>(r1,i1);
				m_EigenValue[2][i] = complex<double>(r2,i2);
				m_EigenValue[3][i] = complex<double>(r3,i3);
				ar>>r0>>r1>>r2>>r3;
				ar>>i0>>i1>>i2>>i3;
				m_EigenValue[4][i] = complex<double>(r0,i0);
				m_EigenValue[5][i] = complex<double>(r1,i1);
				m_EigenValue[6][i] = complex<double>(r2,i2);
				m_EigenValue[7][i] = complex<double>(r3,i3);
				CalculatePoint(i);
			}
		}
		if(m_PolarFormat>=1020)
		{
			ar >> n;
			if(n && m_PolarFormat>1020) m_bAutoInertia =true; else m_bAutoInertia = false;
			ar>>r0>>r1>>r2>>r3;
			m_CoGIxx = r0;
			m_CoGIyy = r1;
			m_CoGIzz = r2;
			m_CoGIxz = r3;
		}
		else
		{
			m_bAutoInertia = false;
			m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
		}

		if(m_PolarFormat>=1022)
		{
			//float provision
			for(int i=0; i<20; i++) ar>>f;

			//int provision
            ar >> n;
            if (m_PolarFormat >= 1024) {
                if (n!=0 && n!=1) return false;
				if(n) m_bIgnoreBodyPanels = true; else m_bIgnoreBodyPanels = false;
			} else m_bIgnoreBodyPanels = false;

			for(int i=1; i<20; i++) ar>>n;
		}
	}

	return true;
}


/**
 * Returns a QString object holding the description and value of the polar's parameters
 * @param &PolarProperties the reference of the QString object to be filled with the description
 * @param bData true if the analysis data should be appended to the string
 */
void WPolar::GetPolarProperties(QString &PolarProperties, bool bData)
{
	QString strong, lenunit, massunit, speedunit;
	Units::getLengthUnitLabel(lenunit);
	Units::getWeightUnitLabel(massunit);
	Units::getSpeedUnitLabel(speedunit);

	QString inertiaunit = massunit+"."+lenunit+QString::fromUtf8("²");

	Plane *pPlane = Objects3D::getPlane(m_PlaneName);

	PolarProperties.clear();

	if(m_WPolarType==FIXEDSPEEDPOLAR)     strong = "Type 1: "+QObject::tr("Fixed speed") +"\n";
	else if(m_WPolarType==FIXEDLIFTPOLAR) strong = "Type 2: "+QObject::tr("Fixed lift") +"\n";
	else if(m_WPolarType==FIXEDAOAPOLAR)  strong = "Type 4: "+QObject::tr("Fixed angle of attack") +"\n";
	else if(m_WPolarType==STABILITYPOLAR) strong = "Type 7: "+QObject::tr("Stability analysis") +"\n";
	PolarProperties += strong;

	if(m_WPolarType==FIXEDSPEEDPOLAR)
	{
		strong  = QString(QObject::tr("VInf =")+"%1 ").arg(m_QInf*Units::mstoUnit(),10,'g',2);
		PolarProperties += strong + speedunit+"\n";
	}
	else if(m_WPolarType==FIXEDAOAPOLAR)
	{
		strong  = QString(QObject::tr("Alpha =")+"%1").arg(m_ASpec,7,'f',2);
		PolarProperties += strong +QString::fromUtf8("°")+"\n";
	}

    if(qAbs(m_Beta)>PRECISION)
	{
		strong  = QString(QObject::tr("Beta")+" = %1").arg(m_Beta,7,'f',2);
		PolarProperties += strong +QString::fromUtf8("°")+"\n";
	}

//	PolarProperties += QObject::tr("Method")+" = ";
	if(m_AnalysisMethod==LLTMETHOD)                              PolarProperties +=QObject::tr("LLT");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bThinSurfaces)   PolarProperties +=QObject::tr("3D-Panels");
	else if(m_AnalysisMethod==PANELMETHOD && m_bVLM1)            PolarProperties +=QObject::tr("3D-Panels/VLM1");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bVLM1)           PolarProperties +=QObject::tr("3D-Panels/VLM2");
	PolarProperties +="\n";


	//Control data
	if(m_WPolarType==STABILITYPOLAR)
	{
		int j;
		int iCtrl = 0;

		strong = "AVL type controls\n";
		PolarProperties +=strong;


		if(!pPlane->isWing())
		{
			if(qAbs(m_ControlGain[iCtrl])>PRECISION)
			{
				strong = QString(QString::fromUtf8("Wing Tilt: gain=%1°/unit\n")).arg(m_ControlGain[iCtrl],0,'f',2);
				PolarProperties +=strong;
			}
			iCtrl=1;
			if(pPlane->stab())
			{
				if(qAbs(m_ControlGain[iCtrl])>PRECISION)
				{
					strong = QString(QString::fromUtf8("Elev. Tilt: gain=%1°/unit\n")).arg(m_ControlGain[iCtrl],0,'f',2);
					PolarProperties +=strong;
				}
				iCtrl=2;
			}
		}

		Wing *pStab, *pFin, *pWing;
		pStab = pFin = pWing = NULL;
		if(pPlane)
		{
			pWing = pPlane->wing();
			pStab = pPlane->stab();
			pFin  = pPlane->fin();
		}

		// flap controls
		//wing first
		int nFlap = 0;
		if(pWing)
		{
			for (j=0; j<pWing->m_Surface.size(); j++)
			{
				if(pWing->m_Surface.at(j)->m_bTEFlap)
				{
                    if(qAbs(m_ControlGain[iCtrl])>PRECISION)
					{
						strong = QString(QString::fromUtf8("Wing Flap %1: g=%2°/unit\n"))
										.arg(nFlap+1)
										.arg(m_ControlGain[iCtrl],5,'f',2);
						PolarProperties +=strong;
					}
					nFlap++;
					iCtrl++;
				}
			}
		}

		//elevator next
		nFlap = 0;
		if(pStab)
		{
			for (j=0; j<pStab->m_Surface.size(); j++)
			{
				if(pStab->m_Surface.at(j)->m_bTEFlap)
				{
                    if(qAbs(m_ControlGain[iCtrl])>PRECISION)
					{
						strong = QString(QString::fromUtf8("Elev. Flap %1: gain=%2°/unit\n"))
										.arg(nFlap+1)
										.arg(m_ControlGain[iCtrl],5,'f',2);
						PolarProperties +=strong;
					}
					nFlap++;
					iCtrl++;
				}
			}
		}

		nFlap = 0;
		if(pFin)
		{
			for (j=0; j<pFin->m_Surface.size(); j++)
			{
				if(pFin->m_Surface.at(j)->m_bTEFlap)
				{
                    if(qAbs(m_ControlGain[iCtrl])>PRECISION)
					{
						strong = QString(QString::fromUtf8("Fin Flap %1: gain=%2°/unit\n"))
										.arg(nFlap+1)
										.arg(m_ControlGain[iCtrl],5,'f',2);
						PolarProperties +=strong;
					}
					nFlap++;
					iCtrl++;
				}
			}
		}
	}


	if(m_bAutoInertia)
	{
		PolarProperties += "Using plane inertia\n";
	}

	strong  = QString(QObject::tr("Mass")+" = %1 ").arg(m_Mass*Units::kgtoUnit(),10,'f',3);
	PolarProperties += strong + massunit + "\n";

	strong  = QString(QObject::tr("CoG.x")+" = %1 ").arg(m_CoG.x*Units::mtoUnit(),10,'g',4);
	PolarProperties += strong + lenunit + "\n";

	strong  = QString(QObject::tr("CoG.z")+" = %1 ").arg(m_CoG.z*Units::mtoUnit(),10,'g',4);
	PolarProperties += strong + lenunit + "\n";

	if(m_WPolarType==STABILITYPOLAR)
	{
		strong  = QString("Ixx = %1 ").arg(m_CoGIxx*Units::mtoUnit()*Units::mtoUnit()*Units::kgtoUnit(),10,'g',4);
		PolarProperties += strong + inertiaunit + "\n";
		strong  = QString("Iyy = %1 ").arg(m_CoGIyy*Units::mtoUnit()*Units::mtoUnit()*Units::kgtoUnit(),10,'g',4);
		PolarProperties += strong + inertiaunit + "\n";
		strong  = QString("Izz = %1 ").arg(m_CoGIzz*Units::mtoUnit()*Units::mtoUnit()*Units::kgtoUnit(),10,'g',4);
		PolarProperties += strong + inertiaunit + "\n";
		strong  = QString("Ixz = %1 ").arg(m_CoGIxz*Units::mtoUnit()*Units::mtoUnit()*Units::kgtoUnit(),10,'g',4);
		PolarProperties += strong + inertiaunit + "\n";
	}

	if(m_AnalysisMethod !=LLTMETHOD)
	{
		if(m_bDirichlet)  strong  = QObject::tr("B.C. = Dirichlet");
		else              strong  = QObject::tr("B.C. = Neumann");
		PolarProperties += strong +"\n";
	}

	PolarProperties += QObject::tr("Analysis type")+" = ";
	if(m_bViscous) PolarProperties += QObject::tr("Viscous")+"\n";
	else           PolarProperties += QObject::tr("Inviscid")+"\n";

	if(pPlane && pPlane->body())
	{
		PolarProperties += QObject::tr("Body option")+" = ";
		if(m_bIgnoreBodyPanels) PolarProperties += QObject::tr("Body Panels Ignored");
		//    else              PolarProperties += QObject::tr("Body Included")+"\n";
		PolarProperties += "\n";
	}

	PolarProperties += QObject::tr("Ref. Area = ");
	if(m_RefAreaType==1) PolarProperties += QObject::tr("Planform area")+"\n";
	else                 PolarProperties += QObject::tr("Projected area")+"\n";

	if(m_bTiltedGeom) PolarProperties += QObject::tr("Tilted geometry")+"\n";

	if(m_bGround)
	{
		strong = QString(QObject::tr("Ground height")+" = %1").arg(m_Height*Units::mtoUnit())+lenunit+"\n";
		PolarProperties += strong;
	}

	strong  = QString(QObject::tr("Density =")+"%1 kg/m3\n").arg(m_Density,12,'g',4);
	PolarProperties += strong;

	strong  = QString(QObject::tr("Viscosity =")+"%1").arg(m_Viscosity,12,'g',4);
	strong +=  "m"+QString::fromUtf8("²")+"/s\n";
	PolarProperties += strong;

	strong = QString(QObject::tr("Data points") +" = %1\n").arg(m_Alpha.size());
	PolarProperties += "\n"+strong;

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	Export(out, Settings::s_ExportFileType, true);
	PolarProperties += "\n"+strong;
}



/**
 * Maps the inertia data from the parameter object to the polar's variables
 * @param ptr a void pointer to the reference wing or plane instance
 * @param bPlane true if the reference object is a plane, false if it is a wing
 */
void WPolar::RetrieveInertia(void *ptr, bool bPlane)
{
	Plane *pPlane = NULL;
	Wing *pWing = NULL;
	if(bPlane)
	{
		pPlane = (Plane*)ptr;
		m_Mass = pPlane->TotalMass();
		m_CoG = pPlane->CoG();
		m_CoGIxx = pPlane->m_CoGIxx;
		m_CoGIyy = pPlane->m_CoGIyy;
		m_CoGIzz = pPlane->m_CoGIzz;
		m_CoGIxz = pPlane->m_CoGIxz;
	}
	else
	{
		pWing  = (Wing*)ptr;
		m_Mass = pWing->TotalMass();
		m_CoG = pWing->m_CoG;
		m_CoGIxx = pWing->m_CoGIxx;
		m_CoGIyy = pWing->m_CoGIyy;
		m_CoGIzz = pWing->m_CoGIzz;
		m_CoGIxz = pWing->m_CoGIxz;
	}
	ClearData();
}



/**
 * Loads or saves the data of this polar to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool WPolar::SerializeWPlrXFL(QDataStream &ar, bool bIsStoring)
{
	int i, k, n;
	double dble;
	double r0, r1, r2, r3, r4, r5, r6, r7;
	double i0, i1, i2, i3, i4, i5, i6, i7;

	m_PolarFormat = 200011;
	// 200011 : v0.00

	if(bIsStoring)
	{
		//output the variables to the stream
		ar << m_PolarFormat;

		ar << m_PlaneName;
		ar << m_WPlrName;

		ar << m_WArea << m_WMAChord << m_WSpan ;
		ar << m_Style << m_Width;
		ar << m_Color;
		ar << m_bIsVisible << m_bShowPoints;

		if(m_AnalysisMethod==LLTMETHOD)        ar<<1;
		else if(m_AnalysisMethod==VLMMETHOD)   ar<<2;
		else if(m_AnalysisMethod==PANELMETHOD) ar<<3;
		else                                   ar<<0;

		if(m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
		else if(m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
		else if(m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
		else if(m_WPolarType==STABILITYPOLAR)  ar<<7;
		else ar << 0;

		ar << m_bVLM1;
		ar << m_bThinSurfaces;
		ar << m_bTiltedGeom;
		ar << m_bDirichlet;
		ar << m_bViscous;
		ar << m_bIgnoreBodyPanels;

		ar << m_bGround;
		ar << m_Height;

		ar << m_Density << m_Viscosity;

		ar << m_RefAreaType;

		ar << m_bAutoInertia; ;
		ar << m_Mass;
		ar << m_CoG.x  << m_CoG.y  << m_CoG.z;
		ar << m_CoGIxx << m_CoGIyy << m_CoGIzz << m_CoGIxz;

		ar << m_nControls;
		for(i=0; i<m_nControls; i++)
		{
			ar << m_ControlGain[i];
		}

		ar << m_NXWakePanels << m_TotalWakeLength << m_WakePanelFactor;

		ar << m_QInf;
		ar << m_ASpec;
		ar << m_Beta;

		// Last store the array data
		ar <<m_Alpha.size();
		for (i=0; i< m_Alpha.size(); i++)
		{
			ar << m_Alpha[i] << m_QInfinite[i]<< m_Ctrl[i];
			ar << m_CL[i] << m_CY[i] << m_ICd[i] << m_PCd[i] ;
			ar << m_GCm[i] << m_ICm[i] << m_VCm[i];
			ar << m_GRm[i];
			ar << m_GYm[i] << m_IYm[i] << m_VYm[i];
			ar << m_XCP[i] << m_YCP[i] << m_ZCP[i];
			ar << m_MaxBending[i];
			ar << m_XNP[i];

			ar <<m_EigenValue[0][i].real() <<m_EigenValue[1][i].real() <<m_EigenValue[2][i].real() <<m_EigenValue[3][i].real();
			ar <<m_EigenValue[0][i].imag() <<m_EigenValue[1][i].imag() <<m_EigenValue[2][i].imag() <<m_EigenValue[3][i].imag();
			ar <<m_EigenValue[4][i].real() <<m_EigenValue[5][i].real() <<m_EigenValue[6][i].real() <<m_EigenValue[7][i].real();
			ar <<m_EigenValue[4][i].imag() <<m_EigenValue[5][i].imag() <<m_EigenValue[6][i].imag() <<m_EigenValue[7][i].imag();
		}

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<20; i++) ar << i;
		for (int i=0; i<50; i++) ar << (double)i;

		return true;
	}
	else
	{
		//input the variables from the stream
		ar >> m_PolarFormat;

		ar >> m_PlaneName;
		ar >> m_WPlrName;

		ar >> m_WArea >> m_WMAChord >> m_WSpan ;
		ar >> m_Style >> m_Width;
		ar >> m_Color;
		ar >> m_bIsVisible >> m_bShowPoints;


		ar >> n;
		if(n==1)      m_AnalysisMethod=LLTMETHOD;
		else if(n==2) m_AnalysisMethod=VLMMETHOD;
		else if(n==3) m_AnalysisMethod=PANELMETHOD;

		ar >> n;
		if(n==1)      m_WPolarType=FIXEDSPEEDPOLAR;
		else if(n==2) m_WPolarType=FIXEDLIFTPOLAR;
		else if(n==4) m_WPolarType=FIXEDAOAPOLAR;
		else if(n==7) m_WPolarType=STABILITYPOLAR;

		ar >> m_bVLM1;
		ar >> m_bThinSurfaces;
		ar >> m_bTiltedGeom;
		ar >> m_bDirichlet;
		ar >> m_bViscous;
		ar >> m_bIgnoreBodyPanels;

		ar >> m_bGround;
		ar >> m_Height;

		ar >> m_Density >> m_Viscosity;

		ar >> m_RefAreaType;

		ar >> m_bAutoInertia; ;
		ar >> m_Mass;
		ar >> m_CoG.x  >> m_CoG.y  >> m_CoG.z;
		ar >> m_CoGIxx >> m_CoGIyy >> m_CoGIzz >> m_CoGIxz;

		ar >> m_nControls;
		m_ControlGain.clear();
		for(i=0; i<m_nControls; i++)
		{
			ar >> dble;
			m_ControlGain.append(dble);
		}

		ar >> m_NXWakePanels >> m_TotalWakeLength >> m_WakePanelFactor;

		ar >> m_QInf;
		ar >> m_ASpec;
		ar >> m_Beta;

		// Last store the array data
		// assumes the arrays have been cleared previously
		double d[20];
		ClearData();
		ar >> n;
		for (i=0; i<n; i++)
		{
			for(int j=0; j<19; j++)
			{
				ar >> d[j];
			}

			InsertDataAt(i, d[0],  d[1],  d[2],  d[3],  d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13],
							d[14], d[15], d[16], d[17], d[18]);

			ar >> r0 >> r1 >>r2 >> r3;
			ar >> i0 >> i1 >>i2 >> i3;
			ar >> r4 >> r5 >>r6 >> r7;
			ar >> i4 >> i5 >>i6 >> i7;
			m_EigenValue[0][i] = complex<double>(r0, i0);
			m_EigenValue[1][i] = complex<double>(r1, i1);
			m_EigenValue[2][i] = complex<double>(r2, i2);
			m_EigenValue[3][i] = complex<double>(r3, i3);
			m_EigenValue[4][i] = complex<double>(r4, i4);
			m_EigenValue[5][i] = complex<double>(r5, i5);
			m_EigenValue[6][i] = complex<double>(r6, i6);
			m_EigenValue[7][i] = complex<double>(r7, i7);

			CalculatePoint(i);
		}
	}

	// space allocation
	for (int i=0; i<20; i++) ar >> k;
	for (int i=0; i<50; i++) ar >> dble;

	return true;
}

void WPolar::Copy(WPolar *pWPolar)
{
	int i;
	m_bIsVisible      = pWPolar->m_bIsVisible;
	m_bShowPoints     = pWPolar->m_bShowPoints;
	m_bTiltedGeom     = pWPolar->m_bTiltedGeom;
	m_bViscous        = pWPolar->m_bViscous;
	m_bVLM1           = pWPolar->m_bVLM1;
	m_bWakeRollUp     = pWPolar->m_bWakeRollUp;
	m_AnalysisMethod  = pWPolar->m_AnalysisMethod;
	m_WPolarType      = pWPolar->m_WPolarType;
	m_bThinSurfaces   = pWPolar->m_bThinSurfaces;
	m_nControls       = pWPolar->m_nControls;

	ClearData();

	for(i=0; i<pWPolar->m_Alpha.size(); i++)
	{
		m_Alpha.append(     pWPolar->m_Alpha[i]);
		m_CL.append(        pWPolar-> m_CL[i]);
		m_CY.append(        pWPolar-> m_CY[i]);
		m_ICd.append(       pWPolar-> m_ICd[i]);
		m_PCd.append(       pWPolar-> m_PCd[i]);
		m_TCd.append(       pWPolar-> m_TCd[i]);

		m_GCm.append(       pWPolar-> m_GCm[i]);
		m_VCm.append(       pWPolar-> m_VCm[i]);
		m_ICm.append(       pWPolar-> m_ICm[i]);
		m_GRm.append(       pWPolar-> m_GRm[i]);
		m_GYm.append(       pWPolar-> m_GYm[i]);
		m_VYm.append(       pWPolar-> m_VYm[i]);
		m_IYm.append(       pWPolar-> m_IYm[i]);

		m_QInfinite.append( pWPolar->m_QInfinite[i]);
		m_XCP.append(       pWPolar-> m_XCP[i]);
		m_YCP.append(       pWPolar-> m_YCP[i]);
		m_ZCP.append(       pWPolar-> m_YCP[i]);

		m_MaxBending.append(pWPolar-> m_MaxBending[i]);
		m_Ctrl.append(      pWPolar-> m_Ctrl[i]);
		m_XNP.append(       pWPolar-> m_XNP[i]);

		m_PhugoidDamping.append(      pWPolar->m_PhugoidDamping[i]);
		m_PhugoidFrequency.append(    pWPolar->m_PhugoidFrequency[i]);
		m_ShortPeriodDamping.append(  pWPolar->m_ShortPeriodDamping[i]);
		m_ShortPeriodFrequency.append(pWPolar->m_ShortPeriodFrequency[i]);
		m_DutchRollDamping.append(    pWPolar->m_DutchRollDamping[i]);
		m_DutchRollFrequency.append(  pWPolar->m_DutchRollFrequency[i]);
		m_RollDamping.append(         pWPolar->m_RollDamping[i]);
		m_SpiralDamping.append(       pWPolar->m_SpiralDamping[i]);

		m_ClCd.append(      pWPolar-> m_ClCd[i]);
		m_1Cl.append(       pWPolar-> m_1Cl[i]);
		m_Cl32Cd.append(    pWPolar-> m_Cl32Cd[i]);

		m_Vx.append(        pWPolar-> m_Vx[i]);
		m_Vz.append(        pWPolar-> m_Vz[i]);

		m_FX.append(        pWPolar-> m_FX[i]);
		m_FY.append(        pWPolar-> m_FY[i]);
		m_FZ.append(        pWPolar-> m_FY[i]);

		m_Gamma.append(     pWPolar-> m_Gamma[i]);
		m_Pm.append(        pWPolar-> m_Pm[i]);
		m_Ym.append(        pWPolar-> m_Ym[i]);
		m_Rm.append(        pWPolar-> m_Rm[i]);
		m_VertPower.append( pWPolar-> m_VertPower[i]);

		m_Oswald.append(    pWPolar-> m_Oswald[i]);
		m_SM.append(        pWPolar-> m_SM[i]);

		for(int l=0; l<8; l++)
			m_EigenValue[l][i] = pWPolar->m_EigenValue[l][i];
	}
}













