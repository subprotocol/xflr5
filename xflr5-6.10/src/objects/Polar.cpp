/****************************************************************************

    Polar Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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


#include "Polar.h"
#include "../misc/Settings.h"
#include "../globals.h"
#include "../xdirect/analysis/XFoil.h"

QList <void *> Polar::s_oaPolar;
Polar* Polar::s_pCurPolar = NULL;


/**
*The public constructor.
*/
Polar::Polar()
{
	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;// = PS_SOLID
	m_Width = 1;
	m_ASpec = 0.0;
	m_PolarType = FIXEDSPEEDPOLAR;
	m_ReType = 1;
	m_MaType = 1;
	m_Reynolds = 100000.0;
	m_Mach     = 0.0;
	m_ACrit    = 9.0;
	m_XTop     = 1.0;
	m_XBot     = 1.0;
	m_FoilName = "";
	m_PlrName = "";
	m_Color= randomColor();
}



/**
 * Exports the data of the polar to a text file
 * @param out the instance of output QtextStream
 * @param FileType TXT if the data is separated by spaces, CSV for a comma separator
 * @param bDataOnly true if the analysis parameters should not be output
 */
void Polar::ExportPolar(QTextStream &out, enumTextFileType FileType, bool bDataOnly)
{
	QString Header, strong;
	int j;

	if(!bDataOnly)
	{
		strong = VERSIONNAME;
		strong += "\n\n";
		out << strong;
		strong =(" Calculated polar for: ");
		strong += m_FoilName + "\n\n";
		out << strong;

		strong = QString(" %1 %2").arg(m_ReType).arg(m_MaType);

		if(m_ReType==1)      strong += (" Reynolds number fixed       ");
		else if(m_ReType==2) strong += (" Reynolds number ~ 1/sqrt(CL)");
		else if(m_ReType==3) strong += (" Reynolds number ~ 1/CL      ");

		if(m_MaType==1)      strong += ("   Mach number fixed         ");
		else if(m_MaType==2) strong += ("   Mach number ~ 1/sqrt(CL)  ");
		else if(m_MaType==3) strong += ("   Mach number ~ 1/CL        ");
		strong +="\n\n";
		out << strong;

		strong=QString((" xtrf =   %1 (top)        %2 (bottom)\n"))
						.arg(m_XTop,0,'f',3).arg(m_XBot,0,'f',3);
		out << strong;

		strong = QString(" Mach = %1     Re = %2 e 6     Ncrit = %3\n\n")
				 .arg(m_Mach,7,'f',3).arg(m_Reynolds/1.e6,9,'f',3).arg(m_ACrit,7,'f',3);
		out << strong;
	}

	if(m_PolarType!=FIXEDAOAPOLAR)
	{
		if(FileType==TXT) Header = ("  alpha     CL        CD       CDp       Cm    Top Xtr Bot Xtr   Cpmin    Chinge    XCp    \n");
		else              Header = ("alpha,CL,CD,CDp,Cm,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n");
		out << Header;
		if(FileType==TXT)
		{
			Header=QString(" ------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
			out << Header;
		}
		for (j=0; j<m_Alpha.size(); j++)
		{
			if(FileType==TXT) strong = QString(" %1  %2  %3  %4  %5")
											.arg(m_Alpha[j],7,'f',3)
											.arg(m_Cl[j],7,'f',4)
											.arg(m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			else              strong = QString("%1,%2,%3,%4,%5")
											.arg(m_Alpha[j],7,'f',3)
											.arg(m_Cl[j],7,'f',4)
											.arg(m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);

			out << strong;
			if(m_XTr1[j]<990.0)
			{
				if(FileType==TXT) strong=QString("  %1  %2").arg(m_XTr1[j],6,'f',4).arg( m_XTr2[j],6,'f',4);
				else              strong=QString(",%1,%2").arg(m_XTr1[j],6,'f',4).arg( m_XTr2[j],6,'f',4);
				out << strong;
			}
			if(FileType==TXT) strong=QString("  %1  %2  %3\n").arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			else              strong=QString(",%1,%2,%3\n").arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			out << strong;
			}
	}
	else 
	{
		if(FileType==TXT) Header=QString(("  alpha     Re      CL        CD       CDp       Cm    Top Xtr Bot Xtr   Cpmin    Chinge     XCp    \n"));
		else              Header=QString(("alpha,Re,CL,CD,CDp,Cm,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n"));
		out << Header;
		if(FileType==TXT)
		{
			Header=QString(" ------- -------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
			out << Header;
		}
		for (j=0; j<m_Alpha.size(); j++)
		{
			if(FileType==TXT) strong=QString(" %1 %2  %3  %4  %5  %6")
											.arg(m_Alpha[j],7,'f',3)
											.arg( m_Re[j],8,'f',0)
											.arg( m_Cl[j],7,'f',4)
											.arg( m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			else              strong=QString(" %1,%2,%3,%4,%5,%6")
											.arg(m_Alpha[j],7,'f',3)
											.arg( m_Re[j],8,'f',0)
											.arg( m_Cl[j],7,'f',4)
											.arg( m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			out << strong;
			if(m_XTr1[j]<990.0)
			{
				if(FileType==TXT) strong=QString("  %1  %2").arg(m_XTr1[j],6,'f',4).arg(m_XTr2[j],6,'f',4);
				else              strong=QString(",%1,%2").arg(m_XTr1[j],6,'f',4).arg(m_XTr2[j],6,'f',4);
				out << strong;
			}
			if(FileType==TXT) strong=QString("  %1  %2  %3\n").arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			else              strong=QString(",%1,%2,%3\n").arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			out << strong;
        }
	}
	out << "\n\n";
}


/**
 * Remove all data from the polar object
 */
void Polar::ResetPolar()
{
    m_Alpha.clear();
    m_Cl.clear();
    m_Cd.clear();
    m_Cdp.clear();
    m_Cm.clear();
    m_XTr1.clear();
    m_XTr2.clear();
    m_HMom.clear();
    m_Cpmn.clear();
    m_ClCd.clear();
    m_RtCl.clear();
    m_Cl32Cd.clear();
    m_Re.clear();
    m_XCp.clear();
}



/**
 * Adds the data from the instance of the operating point referenced by pOpPoint to the polar object.
 * The index used to insert the data is the aoa for type 1, 2 and 3 polars, and the freestream velocity for type 4 polars.
 * If a point with identical index exists, the data is replaced.
 * If not, the data is inserted for this index.
 *
 * @param *pPOpPoint a pointer to the foil's operating point from which the data is to be extracted
 */
void Polar::AddOpPointData(OpPoint *pOpPoint)
{
	if(!pOpPoint->m_bViscResults) return;

	bool bInserted = false;
	int i;
	int size = m_Alpha.size();

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_PolarType<FIXEDAOAPOLAR)
			{
				if (qAbs(pOpPoint->Alpha-m_Alpha[i]) < 0.001)
				{
					ReplaceOppDataAt(i, pOpPoint);
					bInserted = true;
					break;
				}
				else if (pOpPoint->Alpha < m_Alpha[i])
				{
					InsertOppDataAt(i, pOpPoint);
					bInserted = true;
					break;
				}
			}
			else if(m_PolarType==FIXEDAOAPOLAR)
			{
				// type 4, sort by speed
				if (qAbs(pOpPoint->Reynolds - m_Re[i]) < 0.1)
				{
					// then erase former result
					ReplaceOppDataAt(i, pOpPoint);
					bInserted = true;
					break;
				}
				else if (pOpPoint->Reynolds < m_Re[i])
				{
					// sort by crescending speed
					InsertOppDataAt(i, pOpPoint);
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
		InsertOppDataAt(size, pOpPoint);
	}
}


void Polar::ReplaceOppDataAt(int pos, OpPoint *pOpp)
{
	if(pos<0 || pos>= m_Alpha.size()) return;

	m_Alpha[pos] =  pOpp->Alpha;
	m_Cd[pos]    =  pOpp->Cd;
	m_Cdp[pos]   =  pOpp->Cdp;
	m_Cl[pos]    =  pOpp->Cl;
	m_Cm[pos]    =  pOpp->Cm;
	m_XTr1[pos]  =  pOpp->Xtr1;
	m_XTr2[pos]  =  pOpp->Xtr2;
	m_HMom[pos]  =  pOpp->m_TEHMom;
	m_Cpmn[pos]  =  pOpp->Cpmn;
	m_ClCd[pos]  =  pOpp->Cl/pOpp->Cd;
	m_XCp[pos]   =  pOpp->m_XCP;

	if(pOpp->Cl>0.0) m_RtCl[pos] = 1.0/sqrt(pOpp->Cl);
	else             m_RtCl[pos] = 0.0;
	if (pOpp->Cl>=0.0) m_Cl32Cd[pos] =  pow( pOpp->Cl, 1.5)/ pOpp->Cd;
	else               m_Cl32Cd[pos] = -pow(-pOpp->Cl, 1.5)/ pOpp->Cd;

	if(m_PolarType==FIXEDSPEEDPOLAR)  m_Re[pos] =  pOpp->Reynolds;
	else if (m_PolarType==FIXEDLIFTPOLAR)
	{
		if(pOpp->Cl>0.0) m_Re[pos] =  pOpp->Reynolds/ sqrt(pOpp->Cl);
		else             m_Re[pos] = 0.0;
	}
	else if (m_PolarType==RUBBERCHORDPOLAR)
	{
		if(pOpp->Cl>0.0) m_Re[pos] =  pOpp->Reynolds/(pOpp->Cl);
		else             m_Re[pos] = 0.0;
	}

}


void Polar::InsertOppDataAt(int i, OpPoint *pOpp)
{
	m_Alpha.insert(i, pOpp->Alpha);
	m_Cd.insert(i, pOpp->Cd);
	m_Cdp.insert(i, pOpp->Cdp);
	m_Cl.insert(i, pOpp->Cl);
	m_Cm.insert(i, pOpp->Cm);
	m_XTr1.insert(i, pOpp->Xtr1);
	m_XTr2.insert(i, pOpp->Xtr2);
	m_HMom.insert(i, pOpp->m_TEHMom);
	m_Cpmn.insert(i, pOpp->Cpmn);
	m_ClCd.insert(i, pOpp->Cl/pOpp->Cd);
	m_XCp.insert(i, pOpp->m_XCP);

	if(pOpp->Cl>0.0) m_RtCl.insert(i, 1.0/sqrt(pOpp->Cl));
	else             m_RtCl.insert(i, 0.0);

	if (pOpp->Cl>=0.0) m_Cl32Cd.insert(i,pow( pOpp->Cl, 1.5) / pOpp->Cd);
	else               m_Cl32Cd.insert(i,-pow(-pOpp->Cl, 1.5)/ pOpp->Cd);

	if(m_PolarType==FIXEDSPEEDPOLAR)	 m_Re.insert(i, pOpp->Reynolds);
	else if (m_PolarType==FIXEDLIFTPOLAR)
	{
		if(pOpp->Cl>0) m_Re.insert(i, pOpp->Reynolds/sqrt(pOpp->Cl));
		else           m_Re[i] = 0.0;
	}
	else if (m_PolarType==RUBBERCHORDPOLAR)
	{
		if(pOpp->Cl>0.0) m_Re.insert(i, pOpp->Reynolds/pOpp->Cl);
		else             m_Re.insert(i, 0.0);
	}
	else if (m_PolarType==FIXEDAOAPOLAR)
	{
		m_Re.insert(i, pOpp->Reynolds);
	}
}



/**
 * Adds the data from the instance of the XFoil object to the data arrays
 * The index used to insert the data is the aoa for type 1, 2 and 3 polars, and the freestream velocity for type 4 polars.
 * If a point with identical index exists, the data is replaced.
 * If not, the data is inserted for this index.
 *
 * @param *ptrXFoil a pointer to the instance of the XFoil class where the calculation has been performed.
 */
void Polar::AddXFoilData(void* ptrXFoil)
{
	XFoil *pXFoil = (XFoil*)ptrXFoil;

	OpPoint *pOpp = new OpPoint;
	pOpp->m_bViscResults = pXFoil->lvisc;
	pOpp->Alpha    = pXFoil->alfa*180.0/PI;
	pOpp->Cd       = pXFoil->cd;
	pOpp->Cdp      = pXFoil->cdp;
	pOpp->Cl       = pXFoil->cl;
	pOpp->Cm       = pXFoil->cm;
	pOpp->Xtr1     = pXFoil->xoctr[1];
	pOpp->Xtr2     = pXFoil->xoctr[2];
	pOpp->m_TEHMom = pXFoil->hmom;
	pOpp->Cpmn     = pXFoil->cpmn;
	pOpp->Reynolds = pXFoil->reinf1;
	pOpp->m_XCP    = pXFoil->xcp;

	AddOpPointData(pOpp);
	delete pOpp;
}



/**
 * Adds the parameter data to the data arrays
 * The index used to insert the data is the aoa for type 1, 2 and 3 polars, and the freestream velocity for type 4 polars.
 * If a point with identical index exists, the data is replaced.
 * If not, the data is inserted for this index.
 *
 */
void Polar::AddPoint(double Alpha, double Cd, double Cdp, double Cl, double Cm, double Xtr1,
					 double Xtr2, double HMom, double Cpmn, double Reynolds, double XCp)
{
	OpPoint *pOpp = new OpPoint;
	pOpp->m_bViscResults = true;
	pOpp->Alpha    = Alpha;
	pOpp->Cd       = Cd;
	pOpp->Cdp      = Cdp;
	pOpp->Cl       = Cl;
	pOpp->Cm       = Cm;
	pOpp->Xtr1     = Xtr1;
	pOpp->Xtr2     = Xtr2;
	pOpp->m_TEHMom = HMom;
	pOpp->Cpmn     = Cpmn;
	pOpp->Reynolds = Reynolds;
	pOpp->m_XCP    = XCp;

	AddOpPointData(pOpp);
	delete pOpp;

}


/**
 * Copies the polar's data from an existing polar
 * @param pPolar a pointer to the instance of the reference Polar object from which the data should be copied
 */
void Polar::CopyPolar(Polar *pPolar)
{
	CopySpecification(pPolar);

	int size  = m_Alpha.size();
	for(int i=size-1; i>=0; i--)
		Remove(i);

	size  = pPolar->m_Alpha.size();
	for(int i=0; i<size; i++)
	{
		m_Alpha.insert(i,  pPolar->m_Alpha[i]);
		m_Cd.insert(i,     pPolar->m_Cd[i]);
		m_Cdp.insert(i,    pPolar->m_Cdp[i]);
		m_Cl.insert(i,     pPolar-> m_Cl[i]);
		m_Cm.insert(i,     pPolar->m_Cm[i]);
		m_XTr1.insert(i,   pPolar->m_XTr1[i]);
		m_XTr2.insert(i,   pPolar->m_XTr2[i]);
		m_HMom.insert(i,   pPolar->m_HMom[i]);
		m_Cpmn.insert(i,   pPolar->m_Cpmn[i]);
		m_ClCd.insert(i,   pPolar->m_ClCd[i]);
		m_RtCl.insert(i,   pPolar->m_RtCl[i]);
		m_Cl32Cd.insert(i, pPolar->m_Cl32Cd[i]);
		m_Re.insert(i,     pPolar->m_Re[i]);
		m_XCp.insert(i,    pPolar->m_XCp[i]);
	}
}



/**
 * Copies the polar's data from an existing polar
 * @param pPolar a pointer to the instance of the reference Polar object from which the data should be copied
 */
void Polar::CopySpecification(Polar *pPolar)
{
	m_PolarType = pPolar->polarType();
	m_ReType    = pPolar->m_ReType;
	m_MaType    = pPolar->m_MaType;
	m_Reynolds  = pPolar->m_Reynolds;
	m_ASpec     = pPolar->m_ASpec;
	m_Mach      = pPolar->m_Mach;
	m_ACrit     = pPolar->m_ACrit;
	m_XTop      = pPolar->m_XTop;
	m_XBot      = pPolar->m_XBot;
}


/**
 * Loads or saves the data of this polar to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Polar::Serialize(QDataStream &ar, bool bIsStoring)
{
	int i, j, n, l, k;
	int ArchiveFormat;// identifies the format of the file
	float f;

	if(bIsStoring)
    {
		//write variables
        n = (int)m_Alpha.size();
		
        ar << 1004; // identifies the format of the file
		// 1004 : added XCp
		// 1003 : re-instated NCrit, XtopTr and XBotTr with polar
		WriteCString(ar, m_FoilName);
		WriteCString(ar, m_PlrName);

		if(m_PolarType==FIXEDSPEEDPOLAR)       ar<<1;
		else if(m_PolarType==FIXEDLIFTPOLAR)   ar<<2;
		else if(m_PolarType==RUBBERCHORDPOLAR) ar<<3;
		else if(m_PolarType==FIXEDAOAPOLAR)    ar<<4;
		else                                   ar<<1;

		ar << m_MaType << m_ReType  ;
        ar << (int)m_Reynolds << (float)m_Mach ;
        ar << (float)m_ASpec;
        ar << n << (float)m_ACrit;
        ar << (float)m_XTop << (float)m_XBot;
		WriteCOLORREF(ar,m_Color);
		ar << m_Style << m_Width;
		if (m_bIsVisible)  ar<<1; else ar<<0;
		if (m_bShowPoints) ar<<1; else ar<<0;
		
        for (i=0; i< m_Alpha.size(); i++){
            ar << (float)m_Alpha[i] << (float)m_Cd[i] ;
            ar << (float)m_Cdp[i] << (float)m_Cl[i] << (float)m_Cm[i];
            ar << (float)m_XTr1[i] << (float)m_XTr2[i];
            ar << (float)m_HMom[i] << (float)m_Cpmn[i];
            ar << (float)m_Re[i];
            ar << (float)m_XCp[i];
		}

        ar << m_ACrit << m_XTop << m_XBot;

		return true;
	}
    else
    {
		//read variables
		float Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp;
		int iRe;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 ||ArchiveFormat>1100)
		{
			return false;
		}

		ReadCString(ar, m_FoilName);
		ReadCString(ar, m_PlrName);

		if(m_FoilName =="" || m_PlrName =="" )
		{
			return false;
		}

		ar >>k;
		if(k==1)      m_PolarType = FIXEDSPEEDPOLAR;
		else if(k==2) m_PolarType = FIXEDLIFTPOLAR;
		else if(k==3) m_PolarType = RUBBERCHORDPOLAR;
		else if(k==4) m_PolarType = FIXEDAOAPOLAR;
		else          m_PolarType = FIXEDSPEEDPOLAR;


		ar >> m_MaType >> m_ReType;

		if(m_MaType!=1 && m_MaType!=2 && m_MaType!=3)
		{
			return false;
		}
		if(m_ReType!=1 && m_ReType!=2 && m_ReType!=3)
		{
			return false;
		}

		ar >> iRe;
		m_Reynolds = (double) iRe;
		ar >> f; m_Mach =f;

		ar >> f; m_ASpec =f;

		ar >> n;
		ar >> f; m_ACrit =f;
		ar >> f; m_XTop =f;
		ar >> f; m_XBot =f;

		ReadCOLORREF(ar, m_Color);

		ar >> m_Style >> m_Width;

		if(ArchiveFormat>=1002)
		{
			ar >> l;
            if(l!=0 && l!=1 )
            {
				return false;
			}
			if (l) m_bIsVisible =true; else m_bIsVisible = false;
		}

		ar >> l;
		if(l!=0 && l!=1 )
		{
			return false;
		}
		if (l) m_bShowPoints =true; else m_bShowPoints = false;
			
		bool bExists;
		for (i=0; i< n; i++)
		{
			ar >> Alpha >> Cd >> Cdp >> Cl >> Cm;
			ar >> XTr1 >> XTr2;
			ar >> HMom >> Cpmn;

			if(ArchiveFormat >=4) ar >> Re;
			else                  Re = (float)m_Reynolds;

			if(ArchiveFormat>=1004) ar>> XCp;
			else                    XCp = 0.0;

			bExists = false;
			if(m_PolarType!=FIXEDAOAPOLAR)
			{
                for (j=0; j<m_Alpha.size(); j++)
				{
                    if(qAbs(Alpha-m_Alpha[j])<0.001)
					{
						bExists = true;
						break;
					}
				}
			}
			else
			{
                for (j=0; j<m_Re.size(); j++)
				{
                    if(qAbs(Re-m_Re[j])<0.1)
					{
						bExists = true;
						break;
					}
				}
			}
			if(!bExists)
			{
				AddPoint(Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp);
			}
		}
		if(ArchiveFormat>=1003)
			ar >> m_ACrit >> m_XTop >> m_XBot;
	}
	return true;
}



/**
 * Loads or saves the data of this polar to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Polar::SerializePolarXFL(QDataStream &ar, bool bIsStoring)
{
	double dble;
	int i, k, n;
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{
		ar << 100004; // first format for XFL file

		ar << m_FoilName;
		ar << m_PlrName;

		ar << m_Style << m_Width;
		ar << m_Color;
		ar << m_bIsVisible << m_bShowPoints;


		if(m_PolarType==FIXEDSPEEDPOLAR)       ar<<1;
		else if(m_PolarType==FIXEDLIFTPOLAR)   ar<<2;
		else if(m_PolarType==RUBBERCHORDPOLAR) ar<<3;
		else if(m_PolarType==FIXEDAOAPOLAR)    ar<<4;
		else                                   ar<<1;

		ar << m_MaType << m_ReType;
		ar << m_Reynolds << m_Mach;
		ar << m_ASpec;
		ar << m_XTop << m_XBot;
		ar << m_ACrit;

		ar << m_Alpha.size();
		for (i=0; i< m_Alpha.size(); i++)
		{
			ar << (float)m_Alpha[i] << (float)m_Cd[i] ;
			ar << (float)m_Cdp[i] << (float)m_Cl[i] << (float)m_Cm[i];
			ar << (float)m_XTr1[i] << (float)m_XTr2[i];
			ar << (float)m_HMom[i] << (float)m_Cpmn[i];
			ar << (float)m_Re[i];
			ar << (float)m_XCp[i];
		}

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<20; i++) ar << i;
		for (int i=0; i<50; i++) ar << (double)i;

		return true;
	}
	else
	{
		//read variables
		float Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp;

		ar >> ArchiveFormat;
		if (ArchiveFormat <100000 || ArchiveFormat>110000) return false;

		ar >> m_FoilName;
		ar >> m_PlrName;

		ar >> m_Style >> m_Width;
		ar >> m_Color;
		ar >> m_bIsVisible >> m_bShowPoints;


		ar >> n;
		if(n==1)      m_PolarType=FIXEDSPEEDPOLAR;
		else if(n==2) m_PolarType=FIXEDLIFTPOLAR;
		else if(n==3) m_PolarType=RUBBERCHORDPOLAR;
		else if(n==4) m_PolarType=FIXEDAOAPOLAR;

		ar >> m_MaType >> m_ReType;
		ar >> m_Reynolds >> m_Mach;
		ar >> m_ASpec;
		ar >> m_XTop >> m_XBot;
		ar >> m_ACrit;

		ar >> n;

		for (i=0; i< n; i++)
		{
			ar >> Alpha >> Cd >> Cdp >> Cl >> Cm >> XTr1 >> XTr2 >> HMom >> Cpmn >> Re >> XCp;
			AddPoint(Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp);
		}

		// space allocation
		for (int i=0; i<20; i++) ar >> k;
		for (int i=0; i<50; i++) ar >> dble;
	}
	return true;
}



/**
 * Removes the data for the point at a given index of the data arrays
 * @param i the index of the point to be removed
 **/
void Polar::Remove(int i)
{
    m_Alpha.removeAt(i);
    m_Cl.removeAt(i);
    m_Cd.removeAt(i);
    m_Cdp.removeAt(i);
    m_Cm.removeAt(i);
    m_XTr1.removeAt(i);
    m_XTr2.removeAt(i);
    m_HMom.removeAt(i);
    m_Cpmn.removeAt(i);
    m_ClCd.removeAt(i);
    m_RtCl.removeAt(i);
    m_Cl32Cd.removeAt(i);
    m_Re.removeAt(i);
    m_XCp.removeAt(i);
}


/**
* Returns the minimum and maximum angles of attack stored in the polar.
* Since the data is sorted by crescending aoa, this is a matter of returning the first and last values of the array.
*@param &amin the miminum aoa
*@param &amax the maximum aoa
*/
void Polar::GetAlphaLimits(double &amin, double &amax)
{
    if(!m_Alpha.size()){
		amin = 0.0;
		amax = 0.0;
	}
    else
    {
        amin = m_Alpha[0];
        amax = m_Alpha[m_Alpha.size()-1];
	}
}


/**
* Returns the minimum and maximum lift coefficients stored in the polar.
*@param &Clmin the miminum lift coefficient
*@param &Clmax the maximum lift coefficient
*/
void Polar::GetClLimits(double &Clmin, double &Clmax)
{
    if(!m_Cl.size())
    {
		Clmin = 0.0;
		Clmax = 0.0;
	}
    else
    {
		Clmin = 10000.0;
		Clmax =-10000.0;
		double Cl;
        for (int i=0;i<m_Cl.size(); i++)
        {
            Cl = m_Cl[i];
			if(Clmin>Cl) Clmin = Cl;
			if(Clmax<Cl) Clmax = Cl;
		}
	}
}

/**
* Returns the moment coefficient at zero-lift.
* Cm0 is interpolated between the two points in the array such that Cl[i]<0 and Cl[i+1]>0.
* If no such pair is found, the method returns 0.
*@return Cm0
*/
double Polar::GetCm0()
{
    int i;
	double Clmin =  1000.0;
	double Clmax = -1000.0;
    for (i=0; i<m_Cl.size(); i++)
    {
		Clmin = qMin(Clmin, m_Cl[i]);
		Clmax = qMax(Clmax, m_Cl[i]);
	}
	if(!(Clmin<0.0) || !(Clmax>0.0)) return 0.0;
	int k=0;
//	double rr  = m_Cl[k];
//	double rr1 = m_Cl[k+1];

    while (m_Cl[k+1]<0.0)
    {
//		rr  = m_Cl[k];
//		rr1 = m_Cl[k+1];
		k++;
	}
    if(k+1>=m_Cm.size()) return 0.0;
	double Cm0 = m_Cm[k] + (m_Cm[k+1]-m_Cm[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Cm0;

}


/**
* Returns the value of the aoa such that Cl=0.
* The zero lift angle is interpolated between the two points in the array such that Cl[i]<0 and Cl[i+1]>0.
* If no such pair is found, the method returns 0.
*@return Cm0
*/
double Polar::GetZeroLiftAngle()
{
	double Clmin =  1000.0;
	double Clmax = -1000.0;
    for (int i=0; i<m_Cl.size(); i++)
    {
		Clmin = qMin(Clmin, m_Cl[i]);
		Clmax = qMax(Clmax, m_Cl[i]);
	}
	if(!(Clmin<0.0) || !(Clmax>0.0)) return 0.0;
	int k=0;
//	double rr  = m_Cl[k];
//	double rr1 = m_Cl[k+1];

    while (m_Cl[k+1]<0.0)
    {
//		rr  = m_Cl[k];
//		rr1 = m_Cl[k+1];
		k++;
	}
    if(k+1>=m_Alpha.size()) return 0.0;
	double Alpha0 = m_Alpha[k] + (m_Alpha[k+1]-m_Alpha[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Alpha0;

}


/**
* Linearizes Cl vs. Alpha set of points by least square method
* @param Alpha0 the zero-lift angle, i.e.such that Cl = 0, in degrees
* @param slope the slope of the curve Cl=f(aoa), in units 1/°
*/
void Polar::GetLinearizedCl(double &Alpha0, double &slope)
{
    int n = (int)m_Cl.size();

    if(n<=1)
    {
		Alpha0 = 0.0;
		slope = 2.0*PI*PI/180.0;
		return;
	}

	double fn = (double)n;
	double sum1 = 0.0;
	double sum2 = 0.0;
	double sum3 = 0.0;
	double sum4 = 0.0;
	double b1, b2;

	for (int k=0; k<n; k++)
	{
		sum1 += m_Cl[k] * m_Alpha[k];
		sum2 += m_Alpha[k];
		sum3 += m_Cl[k];
		sum4 += m_Alpha[k] * m_Alpha[k];
	}
	if(fn*sum4 == sum2*sum2 || fn*sum1 == sum2 * sum3) 
	{
		//very improbable...
		Alpha0 = 0.0;
		slope = 2.0*PI*PI/180.0;
		return;
	}

	b1 = (fn*sum1 - sum2 * sum3)/(fn*sum4 - sum2*sum2);
	b2 = (sum3 - b1 * sum2)/fn;

	slope  = b1; //in cl/°
	Alpha0 = -b2/b1;
}


/**
 * Returns a QString object holding the description and value of the polar's parameters
 * @param &PolarProperties the reference of the QString object to be filled with the description
 * @param bData true if the analysis data should be appended to the string
 */
void Polar::GetPolarProperties(QString &PolarProperties, bool bData)
{
	QString strong;
	PolarProperties = m_PlrName +"\n\n";

//	PolarProperties += QObject::tr("Parent foil")+" = "+ m_FoilName+"\n";

//	strong = QString(QObject::tr("Analysis Type")+" = %1\n").arg(m_PolarType);
	PolarProperties.clear();

	strong = QString(QObject::tr("Type")+" = %1").arg(m_PolarType);
	if(m_PolarType==FIXEDSPEEDPOLAR)      strong += " ("+QObject::tr("Fixed speed") +")\n";
	else if(m_PolarType==FIXEDLIFTPOLAR) strong += " ("+QObject::tr("Fixed lift") +")\n";
	else if(m_PolarType==FIXEDAOAPOLAR) strong += " ("+QObject::tr("Fixed angle of attack") +")\n";
	PolarProperties += strong;

	if(m_PolarType==FIXEDSPEEDPOLAR)
	{
		strong = QString(QObject::tr("Reynolds number")+" = %L1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %L1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==FIXEDLIFTPOLAR)
	{
		strong = QString("Re.sqrt(Cl) = %L1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString("Ma.sqrt(Cl) = %L1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==RUBBERCHORDPOLAR)
	{
		strong = QString(QObject::tr("Re.Cl")+" = %L1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %L1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==FIXEDAOAPOLAR)
	{
		strong = QString(QObject::tr("Alpha")+" = %L1"+QString::fromUtf8("°")+"\n").arg(m_ASpec,7,'f',2);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %L1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}


	strong = QString(QObject::tr("NCrit") + " = %L1\n").arg(m_ACrit,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Forced top trans.   ") + " = %L1\n").arg(m_XTop,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Forced bottom trans.") + " = %L1\n").arg(m_XBot,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Number of data points") +" = %L1").arg(m_Alpha	.size());
	PolarProperties += "\n" +strong;

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	ExportPolar(out, Settings::s_ExportFileType, true);
	PolarProperties += "\n"+strong;
}



/**
* Returns the name of the variable for a given index
*@param iVar the index of the variable
*@param &Name the reference of the QString object to be filled with the variable's name.
*/
QString Polar::variableName(int iVar)
{
	switch (iVar)
	{
		case 0:
			return "Alpha";
			break;
		case 1:
			return "Cl";
			break;
		case 2:
			return "Cd";
			break;
		case 3:
			return "Cd x 10000";
			break;
		case 4:
			return "Cdp";
			break;
		case 5:
			return "Cm";
			break;
		case 6:
			return "Xtr top";
			break;
		case 7:
			return "Xtr bot";
			break;
		case 8:
			return "HMom";
			break;
		case 9:
			return "Cpmin";
			break;
		case 10:
			return "Cl/Cd";
			break;
		case 11:
			return "|Cl|^(3/2)/Cd";
			break;
		case 12:
			return "1/Rt(Cl)";
			break;
		case 13:
			return "Re";
			break;
		case 14:
			return "XCp";
			break;
		default:
			return "Alpha";
			break;
	}
	return "";
}



void Polar::deletePolar(Polar *pPolar)
{
	Polar* pOldPolar;

	if(!pPolar) return;
	if(pPolar == curPolar()) setCurPolar(NULL);

	for (int iPolar=0; iPolar<s_oaPolar.size(); iPolar++)
	{
		pOldPolar =(Polar*)s_oaPolar.at(iPolar);
		if (pPolar == pOldPolar)
		{
			s_oaPolar.removeAt(iPolar);
			delete pOldPolar;
			break;
		}
	}
}






/**
 * Inserts a polar in the array, using the foil name, the polar type, the Re number and the a.o.a. as sorting keys.
 * If a Polar with identical foilname and polar name exists, deletes the old and replaces it.
 * Note : sorting of Type 1 polars is necessary for the interpolation process in Plane analysis.
 * @param pPolar a pointer to the Polar object to insert in the array;
 */
void Polar::addPolar(Polar *pPolar)
{
	if(!pPolar) return;

	bool bExists   = false;
	bool bInserted = false;
	Polar *pOldPlr = NULL;
	int ip,j;

	for (ip=0; ip<s_oaPolar.size(); ip++)
	{
		pOldPlr = (Polar*)s_oaPolar.at(ip);
		if (pOldPlr->polarName()  == pPolar->polarName() &&	pOldPlr->foilName() == pPolar->foilName())
		{
			bExists = true;
			s_oaPolar.removeAt(ip);
			delete pOldPlr;
			s_oaPolar.insert(ip, pPolar);
			break;
		}
	}

	if(!bExists)
	{
		for (j=0; j<s_oaPolar.size(); j++)
		{
			pOldPlr = (Polar*)s_oaPolar.at(j);

			//first index is the parent foil name
			if (pPolar->foilName().compare(pOldPlr->foilName(), Qt::CaseInsensitive)<0)
			{
				s_oaPolar.insert(j, pPolar);
				bInserted = true;
				break;
			}
			else if (pPolar->foilName() == pOldPlr->foilName())
			{
				//second index is the polar type
				if(pPolar->polarType() < pOldPlr->polarType())
				{
					s_oaPolar.insert(j, pPolar);
					bInserted = true;
					break;
				}
				else if(pPolar->polarType() == pOldPlr->polarType())
				{
					if (pPolar->polarType() != FIXEDAOAPOLAR)
					{
						//sort by re Nbr
						if(pPolar->m_Reynolds < pOldPlr->m_Reynolds)
						{
							s_oaPolar.insert(j, pPolar);
							bInserted = true;
							break;
						}
					}
					else
					{
						//Type 4, sort by Alphas
						if(pPolar->m_ASpec < pOldPlr->m_ASpec)
						{
							s_oaPolar.insert(j, pPolar);
							bInserted = true;
							break;
						}
					}
				}
			}
		}
		if(!bInserted)
		{
			s_oaPolar.append(pPolar);
			bInserted = true;
		}
	}
}



void Polar::setPolarType(enumPolarType type)
{
	m_PolarType =type;
	switch (m_PolarType)
	{
		case FIXEDSPEEDPOLAR:
			m_MaType = 1;
			m_ReType = 1;
			break;
		case FIXEDLIFTPOLAR:
			m_MaType = 2;
			m_ReType = 2;
			break;
		case RUBBERCHORDPOLAR:
			m_MaType = 1;
			m_ReType = 3;
			break;
		case FIXEDAOAPOLAR:
			m_MaType = 1;
			m_ReType = 1;
			break;
		default:
			m_ReType = 1;
			m_MaType = 1;
			break;
	}
}


Polar *Polar::getPolar(QString m_FoilName, QString PolarName)
{
	if (!PolarName.length())
	{
		return NULL;
	}

	Polar *pPolar;
	for (int i=0; i<s_oaPolar.size(); i++)
	{
		pPolar = (Polar*) s_oaPolar.at(i);
		if (pPolar->foilName() == m_FoilName &&  pPolar->polarName() == PolarName)
		{
			return pPolar;
		}
	}
	return NULL;
}




Polar *Polar::getPolar(void *pFoilPtr, QString PolarName)
{
	if (!PolarName.length()) return NULL;

	Foil *pFoil = (Foil*)pFoilPtr;

	Polar *pPolar;
	for (int i=0; i<s_oaPolar.size(); i++)
	{
		pPolar = (Polar*) s_oaPolar.at(i);
		if (pPolar->foilName() == pFoil->foilName() &&  pPolar->polarName() == PolarName)
		{
			return pPolar;
		}
	}
	return NULL;
}



void Polar::setAutoPolarName()
{
	m_PlrName = getAutoPolarName(m_PolarType, m_Reynolds, m_Mach, m_ACrit, m_ASpec);
}



/**
 * Static polar name creation
 * @param polarType
 * @param Re
 * @param Mach
 * @param NCrit
 * @param ASpec
 * @return
 */
QString Polar::getAutoPolarName(enumPolarType polarType, double Re, double Mach, double NCrit, double ASpec)
{
	QString polarName;
	Re = Re/1.e6;
	switch(polarType)
	{
		case FIXEDSPEEDPOLAR:
		{
			polarName = QString("T1_Re%1_M%2").arg(Re,5,'f',3).arg(Mach,4,'f',2);
			break;
		}
		case FIXEDLIFTPOLAR:
		{
			polarName = QString("T2_Re%1_M%2").arg(Re,5,'f',3).arg(Mach,4,'f',2);
			break;
		}
		case RUBBERCHORDPOLAR:
		{
			polarName = QString("T3_Re%1_M%2").arg(Re,5,'f',3).arg(Mach,4,'f',2);
			break;
		}
		case(FIXEDAOAPOLAR):
		{
			polarName = QString("T4_Al%1_M%2").arg(ASpec,5,'f',2).arg(Mach,4,'f',2);
			break;
		}
		default:
		{
			polarName = QString("T1_Re%1_M%2").arg(Re,5,'f',3).arg(Mach,4,'f',2);
			break;
		}
	}


	QString str = QString("_N%1").arg(NCrit,3,'f',1);
	polarName += str;

	return polarName;
}












