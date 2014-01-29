/****************************************************************************

	PlaneOpp Class
	Copyright (C) 2006-2014 Andre Deperrois adeperrois@xflr5.com

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

#include "PlaneOpp.h"
#include "Plane.h"
#include "WPolar.h"
#include "../globals.h"
#include "../misc/Units.h"

bool PlaneOpp::s_bStoreOpps=true;
bool PlaneOpp::s_bKeepOutOpps=false;

/**
*The public constructor
*/
PlaneOpp::PlaneOpp(void *pPlanePtr, void *pWPolarPtr, int PanelArraySize)
{
	m_PlaneName   = "";
	m_PlrName     = "";


	m_NStation    = 0;
	m_NPanels     = 0;
	m_Style       = 0;
	m_Width       = 1;

	m_Color = randomColor();

	m_WPolarType     = FIXEDSPEEDPOLAR;
	m_AnalysisMethod = VLMMETHOD;

	m_Weight = 0.0;

	m_bIsVisible  = true;
	m_bShowPoints = false;

	m_bVLM1 = false;
	m_bThinSurface = true;
	m_bTiltedGeom = false;

	m_nControls = 0;

	m_Span = m_MAChord = 0.0; /** @todo set at creation */

	m_bOut        = false;
	m_bVLM1       = true;

	m_Alpha               = 0.0;
	m_Beta                = 0.0;
	m_Bank                = 0.0;
	m_QInf                = 0.0;
	m_Ctrl                = 0.0;


	m_CL=m_CX=m_CY=m_VCD=m_ICD=m_GCm=m_GRm=m_VCm=m_ICm=m_GYm=m_VYm=m_IYm=0.0;

	memset(m_EigenValue, 0, sizeof(m_EigenValue)); //four longitudinal and four lateral modes
	memset(m_EigenVector, 0, sizeof(m_EigenVector));

	CXu = CZu = Cmu = 0.0;
	CXa = CLa = Cma = CXq = CLq = Cmq = CYb = CYp = CYr = Clb = Clp = Clr = Cnb = Cnp = Cnr = 0.0;

	memset(m_ALong, 0, 16*sizeof(double));
	memset(m_ALat, 0, 16*sizeof(double));
	memset(m_BLat, 0, 4*sizeof(double));
	memset(m_BLong, 0, 4*sizeof(double));

	CXe = CYe = CZe = CLe = CMe = CNe = 0;

//	for (int iw=0; iw<MAXWINGS; iw++) m_bWing[iw] = false;
//	m_bWing[0] = true;

	for (int iw=0; iw<MAXWINGS; iw++) m_pPlaneWOpp[iw] = NULL;

	m_dCp = m_dG = m_dSigma = NULL;
	Allocate(PanelArraySize);


	if(pPlanePtr)
	{
		Plane * pPlane = (Plane*)pPlanePtr;
		m_PlaneName = pPlane->planeName();
		m_MAChord   = pPlane->mac();
		m_Span      = pPlane->span();
		m_NStation  = pPlane->m_Wing[0].m_NStation;
	}

	if(pWPolarPtr)
	{
		WPolar *pWPolar = (WPolar*)pWPolarPtr;
		m_PlrName         = pWPolar->polarName();
		m_bVLM1           = pWPolar->bVLM1();
		m_PlrName         = pWPolar->polarName();
		m_bThinSurface    = pWPolar->bThinSurfaces();
		m_bTiltedGeom     = pWPolar->bTilted();
		m_WPolarType      = pWPolar->polarType();
		m_AnalysisMethod  = pWPolar->analysisMethod();
	}	
}


/**
 * Adds a WingOpp to the PlaneOpp and initializes the data
 * @param iw the index of the wing for which a WingOpp is added
 * @param PanelArraySize the number of panels on the wing
 */
void PlaneOpp::AddWingOpp(int iw, int PanelArraySize)
{
//	m_bWing[iw] = true;
	m_pPlaneWOpp[iw] = new WingOpp(PanelArraySize);
}



/**
 * The public destructor
 */
PlaneOpp::~PlaneOpp()
{
	Release();
}



/** Allocate memory to the arrays */
void PlaneOpp::Allocate(int PanelArraySize)
{
	Release();

	m_NPanels = PanelArraySize;
	m_dCp    = new double[PanelArraySize];
	m_dSigma = new double[PanelArraySize];
	m_dG     = new double[PanelArraySize];
	memset(m_dG,     0, PanelArraySize * sizeof(double));
	memset(m_dSigma, 0, PanelArraySize * sizeof(double));
	memset(m_dCp,    0, PanelArraySize * sizeof(double));
}


/**
 * Releases memory allocated on the heap
 */
void PlaneOpp::Release()
{
	if(m_dCp)    delete [] m_dCp;
	if(m_dSigma) delete [] m_dSigma;
	if(m_dG)     delete [] m_dG;

	m_dCp = NULL;
	m_dSigma = NULL;
	m_dG = NULL;

	for (int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pPlaneWOpp[iw] != NULL) delete m_pPlaneWOpp[iw];
		m_pPlaneWOpp[iw] = NULL;
	}
}



/**
 * Returns a QString object holding the description and value of the WingOpp's parameters
 * @param &WingOppProperties the reference of the QString object to be filled with the description
 * @param bData true if the analysis data should be appended to the string
 */
void PlaneOpp::GetPlaneOppProperties(QString &PlaneOppProperties)
{
	QString strong, strange, lenunit, massunit, speedunit;
	Units::getLengthUnitLabel(lenunit);
	Units::getWeightUnitLabel(massunit);
	Units::getSpeedUnitLabel(speedunit);


	PlaneOppProperties.clear();

	if(m_WPolarType==FIXEDSPEEDPOLAR)     strong += "Type 1 ("+QObject::tr("Fixed speed") +")\n";
	else if(m_WPolarType==FIXEDLIFTPOLAR) strong += "Type 2 ("+QObject::tr("Fixed lift") +")\n";
	else if(m_WPolarType==FIXEDAOAPOLAR)  strong += "Type 4 ("+QObject::tr("Fixed angle of attack") +")\n";
	else if(m_WPolarType==BETAPOLAR)      strong += "Type 5 ("+QObject::tr("Beta range") +")\n";
	else if(m_WPolarType==STABILITYPOLAR) strong += "Type 7 ("+QObject::tr("Stability analysis") +")\n";
	PlaneOppProperties += strong;

//	WOppProperties += QObject::tr("Method")+" = ";
	if(m_AnalysisMethod==LLTMETHOD)                             PlaneOppProperties +=QObject::tr("LLT");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bThinSurface)   PlaneOppProperties +=QObject::tr("3D-Panels");
	else if(m_AnalysisMethod==PANELMETHOD && m_bVLM1)           PlaneOppProperties +=QObject::tr("3D-Panels/VLM1");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bVLM1)          PlaneOppProperties +=QObject::tr("3D-Panels/VLM2");
	PlaneOppProperties +="\n";


	if(m_bTiltedGeom) PlaneOppProperties += QObject::tr("Tilted geometry")+"\n";

	if(m_bOut) PlaneOppProperties += "Point is out of the flight envelope\n";

	strong  = QString(QObject::tr("VInf")+"  =%1 ").arg(m_QInf*Units::mstoUnit(),7,'f',3);
	PlaneOppProperties += "\n"+strong + speedunit+"\n";

	strong  = QString(QObject::tr("Alpha")+" =%1").arg(m_Alpha,7,'f',2);
	PlaneOppProperties += strong +QString::fromUtf8("°")+"\n";

	strong  = QString(QObject::tr("Mass")+"  = %1 ").arg(m_Weight*Units::kgtoUnit(),7,'f',3);
	PlaneOppProperties += strong + massunit + "\n";

	if(qAbs(m_Beta)>PRECISION)
	{
		strong  = QString(QObject::tr("Beta")+"  = %1").arg(m_Beta,7,'f',2);
		PlaneOppProperties += strong +QString::fromUtf8("°")+"\n\n";
	}

	if(m_WPolarType==STABILITYPOLAR)
	{
		strong  = QString(QObject::tr("Control value")+" = %1").arg(m_Ctrl,5,'f',2);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("XNP")+" = %1 ").arg(m_XNP*Units::mtoUnit(),7,'f',3);
		PlaneOppProperties += "\n"+strong +lenunit+"\n";
	}
	strong  = QString(QObject::tr("XCP")+" = %1 ").arg(m_CP.x*Units::mtoUnit(),7,'f',3);
	PlaneOppProperties += strong +lenunit+"\n";
	strong  = QString(QObject::tr("YCP")+" = %1 ").arg(m_CP.y*Units::mtoUnit(),7,'f',3);
	PlaneOppProperties += strong +lenunit+"\n";
	strong  = QString(QObject::tr("ZCP")+" = %1 ").arg(m_CP.z*Units::mtoUnit(),7,'f',3);
	PlaneOppProperties += strong +lenunit+"\n\n";


	strong  = QString(QObject::tr("CL")+"  = %1").arg(m_CL,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CD")+"  = %1").arg(m_ICD+m_VCD,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCD")+" = %1").arg(m_VCD,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICD")+" = %1").arg(m_ICD,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CX")+"  = %1").arg(m_CX,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CY")+"  = %1").arg(m_CY,9,'f',5);
	PlaneOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cl")+"  = %1").arg(m_GRm,9,'f',5);
	PlaneOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cm")+"  = %1").arg(m_GCm,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICm")+" = %1").arg(m_ICm,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCm")+" = %1").arg(m_VCm,9,'f',5);
	PlaneOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cn")+"  = %1").arg(m_GYm,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICn")+" = %1").arg(m_IYm,9,'f',5);
	PlaneOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCn")+" = %1").arg(m_VYm,9,'f',5);
	PlaneOppProperties += strong +"\n";

/*	if(m_nFlaps>0)
	{
		WingOppProperties += "\n";
		for(int ip=0; ip<m_nFlaps; ip++)
		{
			strong = QString("Flap Moment[%1] = %2").arg(ip+1).arg(m_FlapMoment[ip]*UnitsDlg::NmtoUnit(), 7,'g',3);
			UnitsDlg::GetMomentUnit(strange, UnitsDlg::momentUnit());
			WingOppProperties += strong + strange +"\n";
		}
	}*/


	if(m_WPolarType==STABILITYPOLAR)
	{
		PlaneOppProperties += "\n\n";
		PlaneOppProperties += QObject::tr("Non-dimensional Stability Derivatives:")+"\n";
		strong  = QString(QObject::tr("CXu")+"  = %1").arg(CXu,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLu")+"  = %1").arg(-CZu,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cmu")+"  = %1").arg(Cmu,9,'f',5);
		PlaneOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CXa")+"  = %1").arg(CXa,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLa")+"  = %1").arg(CLa,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cma")+"  = %1").arg(Cma,9,'f',5);
		PlaneOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CXq")+"  = %1").arg(CXq,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLq")+"  = %1").arg(CLq,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cmq")+"  = %1").arg(Cmq,9,'f',5);
		PlaneOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYb")+"  = %1").arg(CYb,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clb")+"  = %1").arg(Clb,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnb")+"  = %1").arg(Cnb,9,'f',5);
		PlaneOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYp")+"  = %1").arg(CYp,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clp")+"  = %1").arg(Clp,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnp")+"  = %1").arg(Cnp,9,'f',5);
		PlaneOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYr")+"  = %1").arg(CYr,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clr")+"  = %1").arg(Clr,9,'f',5);
		PlaneOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnr")+"  = %1").arg(Cnr,9,'f',5);
		PlaneOppProperties += strong +"\n\n";

/*		if(m_nControls>0)
		{
			// (only one)
			WingOppProperties += QObject::tr("Non-dimensional Control Derivatives:")+"\n";
			strong  = QString(QObject::tr("CXd")+"  = %1").arg(CXe,9,'f',5);
			WingOppProperties += strong +"\n";
			strong  = QString(QObject::tr("CYd")+"  = %1").arg(CYe,9,'f',5);
			WingOppProperties += strong +"\n";
			strong  = QString(QObject::tr("CZd")+"  = %1").arg(CZe,9,'f',5);
			WingOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cld")+"  = %1").arg(CLe,9,'f',5);
			WingOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cmd")+"  = %1").arg(CMe,9,'f',5);
			WingOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cnd")+"  = %1").arg(CNe,9,'f',5);
			WingOppProperties += strong +"\n";
		}*/

		complex<double> c, angle;
		double OmegaN, Omega1, Dsi, Sigma1, sum, prod, u0, mac, span;
		u0   = m_QInf;
		mac  = m_MAChord;
		span = m_Span;


		PlaneOppProperties += "\nLongitudinal modes:\n";
		for(int im=0; im<4; im++)
		{
			c = m_EigenValue[im];
			sum  = c.real() * 2.0;                         // is a real number
			prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
			OmegaN = qAbs(c.imag());
			if(OmegaN>PRECISION)	Omega1 = sqrt(prod);
			else                    Omega1 = 0.0;
			Sigma1 = sum /2.0;
			if(Omega1>PRECISION) Dsi = -Sigma1/Omega1;
			else                 Dsi = 0.0;

			if(c.imag()>=0.0) strange = QString("  Eigenvalue    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("  Eigenvalue    = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Undamped Natural Frequency = %1 Hz").arg(OmegaN/2.0/PI, 8,'f',3);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Damped Natural Frequency   = %1 Hz").arg(Omega1/2.0/PI, 8,'f',3);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Damping Ratio              = %1 ").arg(Dsi, 8,'f',3);
			PlaneOppProperties += strange +"\n";


			PlaneOppProperties += "  Normalized Eigenvector:\n";
			angle = m_EigenVector[im][3];
			c = m_EigenVector[im][0]/u0;
			if(c.imag()>=0.0) strange = QString("    u/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    u/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][1]/u0;
			if(c.imag()>=0.0) strange = QString("    w/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    w/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][2]/(2.0*u0/mac);
			if(c.imag()>=0.0) strange = QString("    q/(2.u0.MAC)  = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    q/(2.u0.MAC)  = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][3]/angle;
			if(c.imag()>=0.0) strange = QString("    theta(rad)    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    theta(rad)    = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n\n";
		}

		PlaneOppProperties += "\nLateral modes:\n";
		for(int im=4; im<8; im++)
		{
			c = m_EigenValue[im];
			sum  = c.real() * 2.0;                         // is a real number
			prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
			OmegaN = qAbs(c.imag());
			if(OmegaN>PRECISION)	Omega1 = sqrt(prod);
			else                    Omega1 = 0.0;
			Sigma1 = sum /2.0;
			if(Omega1>PRECISION) Dsi = -Sigma1/Omega1;
			else                 Dsi = 0.0;

			if(c.imag()>=0.0) strange = QString("  Eigenvalue    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("  Eigenvalue    = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Undamped Natural Frequency = %1 Hz").arg(OmegaN/2.0/PI, 8,'f',3);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Damped Natural Frequency   = %1 Hz").arg(Omega1/2.0/PI, 8,'f',3);
			PlaneOppProperties += strange +"\n";

			strange = QString("  Damping Ratio              = %1 ").arg(Dsi, 8,'f',3);
			PlaneOppProperties += strange +"\n";


			PlaneOppProperties += "  Normalized Eigenvector:\n";

			angle = m_EigenVector[im][3];

			c = m_EigenVector[im][0]/u0;
			if(c.imag()>=0.0) strange = QString("    v/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    v/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][1]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("    p/(2.u0.Span) = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    p/(2.u0.Span) = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][2]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("    r/(2.u0.Span) = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    r/(2.u0.Span) = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n";

			c = m_EigenVector[im][3]/angle;
			if(c.imag()>=0.0) strange = QString("    phi(rad)      = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    phi(rad)      = %1-%2i").arg(c.real(),10,'f',5).arg(qAbs(c.imag()),10,'f',5);
			PlaneOppProperties += strange +"\n\n";
		}

	}
}





/**
 * Loads or saves the data of this operating point to a binary file.
 * This method serializes the data associated to the plane, then calls the serialization methods
 * of the wings.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool PlaneOpp::SerializePOppWPA(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int a, k;
	float f, g, h;

	if(bIsStoring)
	{
		// not storing to .wpa format anymore
	}
	else
	{
		//1009 : restored the serialization of Cp, Gamma, Sigma arrays...
		//1008 : removed the serialization of Cp, Gamma, Sigma arrays
		//1007 : added Sideslip Beta
		//1006 : added Panel's source strengths Sigma
		//1005 : added second wing results for a biplane
		//1004 : converted units to SI
		//1003 : added vortices strengths
		//1002 : added Cp, suppressed Cl and other parameters
		//1001 : suppressed Ai, Cd, SpanPos,
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
		//read variables
		ReadCString(ar, m_PlaneName);
		ReadCString(ar, m_PlrName);

		//always a main wing
		if(m_pPlaneWOpp[0]!=NULL) delete m_pPlaneWOpp[0];
		m_pPlaneWOpp[0] = new WingOpp();

		if(ArchiveFormat>=1005)
		{
			ar >> a;
			if (a!=0 && a!=1) return false;
			if(a)
			{
				if(m_pPlaneWOpp[1]!=NULL) delete m_pPlaneWOpp[1];
				m_pPlaneWOpp[1] = new WingOpp();
			}
		}

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a)
		{
			if(m_pPlaneWOpp[2]!=NULL) delete m_pPlaneWOpp[2];
			m_pPlaneWOpp[2] = new WingOpp();
		}

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a)
		{
			if(m_pPlaneWOpp[3]!=NULL) delete m_pPlaneWOpp[3];
			m_pPlaneWOpp[3] = new WingOpp();
		}


		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a) m_bIsVisible = true; else m_bIsVisible = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bShowPoints = true; else m_bShowPoints = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bOut = true; else m_bOut = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bVLM1 = true; else m_bVLM1 = false;

		ar >> a;
		if (a!=0 && a!=1) return false;
//		if(a) m_bMiddle = true; else m_bMiddle = false;

		ar >> m_Style >> m_Width;
		ReadCOLORREF(ar, m_Color);

		ar >>k;
		if(k==1)      m_WPolarType = FIXEDSPEEDPOLAR;
		else if(k==2) m_WPolarType = FIXEDLIFTPOLAR;
		else if(k==4) m_WPolarType = FIXEDAOAPOLAR;
		else if(k==5) m_WPolarType = BETAPOLAR;
		else if(k==7) m_WPolarType = STABILITYPOLAR;
		else return false;


		ar >> m_NStation;
		ar >> f;		m_Alpha = f;
		ar >> f;        m_QInf  = f;
		ar >> f;//        m_Weight = f;

		if(ArchiveFormat>=1007)
		{
			ar>>f; m_Beta = f;
		}
		if(ArchiveFormat<1002)
		{
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
		}
		if(ArchiveFormat<1001)
		{
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
		}
		if(ArchiveFormat>=1002 )
		{
			ar >>m_NPanels;

			if(ArchiveFormat<=1007)
			{
				for (k=0; k<=m_NPanels; k++)
				{
					ar >> f;
	//				m_Cp[k] = f;
				}
			}
		}
		if(ArchiveFormat>=1003 && ArchiveFormat<=1007)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
//				if(ArchiveFormat<1004)	m_G[k] = f/1000.0;
//				else 					m_G[k] = f;
			}
		}

		if(ArchiveFormat>=1006 && ArchiveFormat<=1007)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
//				m_Sigma[k] = f;
			}
		}

		if(ArchiveFormat>=1009)
		{
			if(m_dG!=NULL)     delete [] m_dG;
			if(m_dSigma!=NULL) delete [] m_dSigma;
			if(m_dCp!=NULL)    delete [] m_dCp;

			m_dG     = new double[m_NPanels];
			m_dSigma = new double[m_NPanels];
			m_dCp    = new double[m_NPanels];

			for (k=0; k<m_NPanels; k++)
			{
				ar >> f >> g >> h;
				m_dCp[k]    = (double)f;
				m_dSigma[k] = (double)g;
				m_dG[k]     = (double)h;
			}
		}

		ar >> k; //VLMType

		if (!m_pPlaneWOpp[0]->SerializeWingOppWPA(ar, bIsStoring))
		{
			return false;
		}


		if(ArchiveFormat>=1005)
		{
			if(m_pPlaneWOpp[1])
			{
				if (!m_pPlaneWOpp[1]->SerializeWingOppWPA(ar, bIsStoring))
				{
					return false;
				}
			}
		}
		if(m_pPlaneWOpp[2])
		{
			if (!m_pPlaneWOpp[2]->SerializeWingOppWPA(ar, bIsStoring))
			{
				return false;
			}

		}
		if(m_pPlaneWOpp[3])
		{
			if (!m_pPlaneWOpp[3]->SerializeWingOppWPA(ar, bIsStoring))
			{
				return false;
			}
		}


		m_CL = m_pPlaneWOpp[0]->m_CL;
		m_CX = m_pPlaneWOpp[0]->m_CX;
		m_CY = m_pPlaneWOpp[0]->m_CY;

		m_ICD = m_pPlaneWOpp[0]->m_ICD;
		m_VCD = m_pPlaneWOpp[0]->m_VCD;

		m_ICm = m_pPlaneWOpp[0]->m_ICm;
		m_VCm = m_pPlaneWOpp[0]->m_VCm;
		m_GCm = m_pPlaneWOpp[0]->m_GCm;

		m_GRm = m_pPlaneWOpp[0]->m_GRm;

		m_IYm = m_pPlaneWOpp[0]->m_IYm;
		m_VYm = m_pPlaneWOpp[0]->m_VYm;
		m_GYm = m_pPlaneWOpp[0]->m_GYm;

		m_CP.Copy(m_pPlaneWOpp[0]->m_CP);

		m_Ctrl = m_pPlaneWOpp[0]->m_oldCtrl;

		memcpy(m_EigenValue,  m_pPlaneWOpp[0]->m_oldEigenValue,  16*sizeof(double));
		memcpy(m_EigenVector, m_pPlaneWOpp[0]->m_oldEigenVector, 64*sizeof(double));

		int pos = 0;
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pPlaneWOpp[iw])
			{
				m_pPlaneWOpp[iw]->m_dCp    = m_dCp    + pos;
				m_pPlaneWOpp[iw]->m_dG     = m_dG     + pos;
				m_pPlaneWOpp[iw]->m_dSigma = m_dSigma + pos;
				pos +=m_pPlaneWOpp[iw]->m_NVLMPanels;
			}
		}


		if(ArchiveFormat>=1020)
		{
			// Non dimensional stability derivatives
			ar>>f;   CXa= f;
			ar>>f;   CXq= f;
			ar>>f;   CXu= f;
			ar>>f;   CZu= f;
			ar>>f;   Cmu= f;
		}
		if(ArchiveFormat>=1017)
		{
			// Non dimensional stability derivatives
			ar>>f;   CLa= f;
			ar>>f;   CLq= f;
			ar>>f;   Cma= f;
			ar>>f;   Cmq= f;
			ar>>f;   CYb= f;
			ar>>f;   CYp= f;
			ar>>f;   CYr= f;
			ar>>f;   Clb= f;
			ar>>f;   Clp= f;
			ar>>f;   Clr= f;
			ar>>f;   Cnb= f;
			ar>>f;   Cnp= f;
			ar>>f;   Cnr= f;
		}

		int n;
		float f0, f1,f2,f3;
		if(ArchiveFormat>=1018)
		{
			ar >> m_nControls;

			if(ArchiveFormat<1022) n = m_nControls;
			else                   n =1;
			for(k=0; k<n; k++)
			{
				ar >>f;   if(k==0) CXe=f;
				ar >>f;   if(k==0) CYe=f;
				ar >>f;   if(k==0) CZe=f;
				ar >>f;   if(k==0) CLe=f;
				ar >>f;   if(k==0) CMe=f;
				ar >>f;   if(k==0) CNe=f;

				ar >>f0>>f1>>f2>>f3;
				if(k==0) m_BLat[0]= f0; m_BLat[1]= f1; m_BLat[2] = f2; m_BLat[3] = f3;
				ar >>f0>>f1>>f2>>f3;
				m_BLong[0]=f0; m_BLong[1]=f1; m_BLong[2]= f2; m_BLong[3]= f3;
			}

			for(k=0; k<4; k++)
			{
				ar >>f0>>f1>>f2>>f3;
				m_ALong[k][0]= f0; m_ALong[k][1]= f1; m_ALong[k][2]= f2; m_ALong[k][3] = f3;
				ar >>f0>>f1>>f2>>f3;
				m_ALat[k][0] = f0; m_ALat[k][1] = f1; m_ALat[k][2] = f2; m_ALat[k][3] = f3;
			}
		}

		if(ArchiveFormat>=1019)
		{
			ar>>f;
			m_XNP = f;
		}
		else m_XNP = 0.0;
	}
	return true;
}


/**
 * Loads or saves the data of this operating point to a binary file.
 * This method serializes the data associated to the plane, then calls the serialization methods
 * of the wings.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool PlaneOpp::SerializePOppXFL(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int k, n;
	float f0, f1, f2;
	double dble, dbl1, dbl2;

	if(bIsStoring)
	{
		ar << 200001;
		//200001 : first go at the new format

		//write variables
		ar << m_PlaneName;
		ar << m_PlrName;

		ar << m_Style << m_Width;
		ar << m_Color;
		ar << m_bIsVisible << m_bShowPoints;

		ar << m_bOut;
		ar << m_bVLM1;

		ar << m_bThinSurface << m_bTiltedGeom;

		if(m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
		else if(m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
		else if(m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
		else if(m_WPolarType==BETAPOLAR)       ar<<5;
		else if(m_WPolarType==STABILITYPOLAR)  ar<<7;
		else ar << 1;

		if(m_AnalysisMethod==LLTMETHOD)         ar<<1;
		else if(m_AnalysisMethod==VLMMETHOD)    ar<<2;
		else if(m_AnalysisMethod==PANELMETHOD)  ar<<3;
		else ar << 2;

		ar << m_NPanels;
		ar << m_NStation;
		ar << m_Alpha << m_QInf;
		ar << m_Beta;
		ar << m_Ctrl;

		ar << m_Weight;

		if(m_AnalysisMethod!=LLTMETHOD)
		{
			for (k=0; k<m_NPanels; k++) ar<<(float)m_dCp[k]<<(float)m_dSigma[k]<<(float)m_dG[k];
		}

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pPlaneWOpp[iw])  ar << 1; else ar<<0;

			if(m_pPlaneWOpp[iw])
			{
				m_pPlaneWOpp[iw]->SerializeWingOppXFL(ar, bIsStoring);
			}
		}

		ar << m_CL << m_CX << m_CY;
		ar << m_VCD << m_ICD << m_VCm << m_ICm;
		ar << m_GRm;
		ar << m_GYm << m_VYm << m_IYm;
		ar << m_CP.x << m_CP.y << m_CP.z;

		ar << CXa << CXq << CXu << CZu <<Cmu;
		ar << CLa << CLq << Cma << Cmq;
		ar << CYb << CYp << CYr << Clb << Clp << Clr << Cnb << Cnp << Cnr;

		ar << m_nControls;
		ar<<CXe<<CYe<<CZe;
		ar<<CLe<<CMe<<CNe;

		ar << m_BLat[0] << m_BLat[1] << m_BLat[2] << m_BLat[3];
		ar << m_BLong[0]<< m_BLong[1]<< m_BLong[2]<< m_BLong[3];

		for(k=0; k<4; k++)
		{
			ar << m_ALong[k][0]<< m_ALong[k][1]<< m_ALong[k][2]<< m_ALong[k][3];
			ar << m_ALat[k][0] << m_ALat[k][1] << m_ALat[k][2] << m_ALat[k][3];
		}

		ar << m_XNP;

		for(int kv=0; kv<8;kv++)
		{
			ar << m_EigenValue[kv].real() << m_EigenValue[kv].imag();
			for(int lv=0; lv<4; lv++)
			{
				ar << m_EigenVector[kv][lv].real() << m_EigenVector[kv][lv].imag();
			}
		}

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<20; i++) ar << i;
		for (int i=0; i<50; i++) ar << (double)i;
	}
	else
	{
		ar >> ArchiveFormat;

		ar >> m_PlaneName;
		ar >> m_PlrName;

		ar >> m_Style >> m_Width;
		ar >> m_Color;
		ar >> m_bIsVisible >> m_bShowPoints;

		ar >> m_bOut;
		ar >> m_bVLM1;

		ar >> m_bThinSurface >> m_bTiltedGeom;

		ar >> n;
		if(n==1)      m_WPolarType=FIXEDSPEEDPOLAR;
		else if(n==2) m_WPolarType=FIXEDLIFTPOLAR;
		else if(n==4) m_WPolarType=FIXEDAOAPOLAR;
		else if(n==5) m_WPolarType=BETAPOLAR;
		else if(n==7) m_WPolarType=STABILITYPOLAR;

		ar >> n;
		if(n==1)      m_AnalysisMethod=LLTMETHOD;
		else if(n==2) m_AnalysisMethod=VLMMETHOD;
		else if(n==3) m_AnalysisMethod=PANELMETHOD;

		ar >> m_NPanels;
		ar >> m_NStation;
		ar >> m_Alpha >> m_QInf;
		ar >> m_Beta;
		ar >> m_Ctrl;

		ar >> m_Weight;

		if(m_dG!=NULL)     delete [] m_dG;
		if(m_dSigma!=NULL) delete [] m_dSigma;
		if(m_dCp!=NULL)    delete [] m_dCp;

		m_dG     = new double[m_NPanels];
		m_dSigma = new double[m_NPanels];
		m_dCp    = new double[m_NPanels];

		if(m_AnalysisMethod!=LLTMETHOD)
		{
			for (k=0; k<m_NPanels; k++)
			{
				ar >> f0 >> f1 >> f2;
				m_dCp[k]    = (double)f0;
				m_dSigma[k] = (double)f1;
				m_dG[k]     = (double)f2;
			}
		}

		int pos = 0;
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			ar >> n;
			if(n) m_pPlaneWOpp[iw] = new WingOpp();
			else  m_pPlaneWOpp[iw] = NULL;

			if(m_pPlaneWOpp[iw])
			{
				m_pPlaneWOpp[iw]->SerializeWingOppXFL(ar, bIsStoring);

				m_pPlaneWOpp[iw]->m_dCp    = m_dCp    + pos;
				m_pPlaneWOpp[iw]->m_dG     = m_dG     + pos;
				m_pPlaneWOpp[iw]->m_dSigma = m_dSigma + pos;
				pos +=m_pPlaneWOpp[iw]->m_NVLMPanels;
			}
		}


		ar >> m_CL >> m_CX >> m_CY;
		ar >> m_VCD >> m_ICD >> m_VCm >> m_ICm;
		ar >> m_GRm;
		ar >> m_GYm >> m_VYm >> m_IYm;
		ar >> m_CP.x >> m_CP.y >> m_CP.z;

		m_GCm = m_ICm + m_VCm;
		m_GYm = m_IYm + m_VYm;


		ar >> CXa >> CXq >> CXu >> CZu >> Cmu;
		ar >> CLa >> CLq >> Cma >> Cmq;
		ar >> CYb >> CYp >> CYr >> Clb >> Clp >> Clr >> Cnb >> Cnp >> Cnr;

		ar >> m_nControls;
		ar >> CXe >> CYe >> CZe;
		ar >> CLe >> CMe >> CNe;
		ar >> m_BLat[0] >> m_BLat[1] >> m_BLat[2] >> m_BLat[3];
		ar >> m_BLong[0]>> m_BLong[1]>> m_BLong[2]>> m_BLong[3];

		for(k=0; k<4; k++)
		{
			ar >> m_ALong[k][0]>> m_ALong[k][1]>> m_ALong[k][2]>> m_ALong[k][3];
			ar >> m_ALat[k][0] >> m_ALat[k][1] >> m_ALat[k][2] >> m_ALat[k][3];
		}

		ar >> m_XNP;

		for(int kv=0; kv<8;kv++)
		{
			ar >> dbl1 >> dbl2;
			m_EigenValue[kv] = complex<double>(dbl1, dbl2);

			for(int lv=0; lv<4; lv++)
			{
				ar >> dbl1 >> dbl2;
				m_EigenVector[kv][lv] = complex<double>(dbl1, dbl2);
			}
		}

		// space allocation
		for (int i=0; i<20; i++) ar >> k;
		for (int i=0; i<50; i++) ar >> dble;
	}
	return true;
}




QString PlaneOpp::title()
{
	QString str1, str2, str3, str4, str5, str6;
	str1 = QString("V=%1").arg(m_QInf*Units::mstoUnit(),5,'f',2);
	Units::getSpeedUnitLabel(str2);
	str3 = QString("_a=%1").arg(m_Alpha,5,'f',2);

	if(qAbs(m_Beta)>0.0)             str4 = QString("_b=%1").arg(m_Beta,5,'f',2);
	else                             str4 ="";
	if(m_WPolarType==STABILITYPOLAR) str5 = QString("_Ctrl=%1").arg(m_Ctrl,5,'f',2);
	else                             str5 ="";


	if(m_AnalysisMethod==LLTMETHOD)  str6="_LLT";
	else if(m_bThinSurface)
	{
		if(m_bVLM1) str6="_VLM1";
		else        str6="_VLM2";
	}
	else if(m_AnalysisMethod==PANELMETHOD)
	{
		str6="_Panel";
		if(m_bThinSurface) str6 += "_Thin";
	}

	if(m_bTiltedGeom) str6 += "_TG";

	if(m_bOut) str6+=" (Out)";

	return str1+str2+str3+str4+str5+str6;
}







