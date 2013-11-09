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
#include "../mainframe.h"
#include "../globals.h"
#include "../xdirect/XFoil.h"
#include <QtDebug>



void *Polar::s_pMainFrame;
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
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!bDataOnly)
	{
		strong =pMainFrame->m_VersionName + "\n\n";
		out << strong;
		strong =(" Calculated polar for: ");
		strong += m_FoilName + "\n\n";
		out << strong;
		strong = QString(" %1 %2").arg(m_ReType).arg(m_MaType);
		if(m_ReType==1) strong += (" Reynolds number fixed       ");
		else if(m_ReType==2) strong += (" Reynolds number ~ 1/sqrt(CL)");
		else if(m_ReType==3) strong += (" Reynolds number ~ 1/CL      ");
		if(m_MaType==1) strong += ("   Mach number fixed         ");
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
 void Polar::AddData(OpPoint *pOpPoint)
{
	if(!pOpPoint->m_bViscResults) return;
	m_ACrit = pOpPoint->ACrit;
	AddPoint(pOpPoint->Alpha, pOpPoint->Cd, pOpPoint->Cdp, pOpPoint->Cl, pOpPoint->Cm,
			 pOpPoint->Xtr1, pOpPoint->Xtr2, pOpPoint->m_TEHMom, pOpPoint->Cpmn, pOpPoint->Reynolds,
			 pOpPoint->m_XCP);
}


/**
 * Adds the data from the instance of the XFoil object to the data arrays
 * The index used to insert the data is the aoa for type 1, 2 and 3 polars, and the freestream velocity for type 4 polars.
 * If a point with identical index exists, the data is replaced.
 * If not, the data is inserted for this index.
 *
 * @param *ptrXFoil a pointer to the instance of the XFoil class where the calculation has been performed.
 */
void Polar::AddData(void* ptrXFoil)
{
	XFoil *pXFoil = (XFoil*)ptrXFoil;

	if(!pXFoil->lvisc) return;
	double alpha = pXFoil->alfa*180.0/PI;
	m_ACrit = pXFoil->acrit;

	AddPoint(alpha, pXFoil->cd, pXFoil->cdp, pXFoil->cl, pXFoil->cm, pXFoil->xoctr[1],
			 pXFoil->xoctr[2], pXFoil->hmom, pXFoil->cpmn, pXFoil->reinf1, pXFoil->xcp);
	
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
	int i;
	bool bInserted = false;
	int size = (int)m_Alpha.size();
	if(size)
	{
		for ( i=0; i<size; i++)
		{
			if(m_PolarType!=FIXEDAOAPOLAR)
			{
                if (fabs(Alpha - m_Alpha[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i] =  Alpha;
					m_Cd[i]    =  Cd;
					m_Cdp[i]   =  Cdp;
					m_Cl[i]    =  Cl;
					m_Cm[i]    =  Cm;
					m_XTr1[i]  =  Xtr1;
					m_XTr2[i]  =  Xtr2;
					m_HMom[i]  =  HMom;
					m_Cpmn[i]  =  Cpmn;
					m_ClCd[i]  =  Cl/Cd;
					m_XCp[i]   =  XCp;

					if(Cl>0.0)	 m_RtCl[i] = 1.0/sqrt(Cl);
					else         m_RtCl[i] = 0.0;
                    if (Cl>=0.0) m_Cl32Cd[i] = pow(Cl, 1.5)/ Cd;
                    else         m_Cl32Cd[i] = -pow(-Cl, 1.5)/ Cd;

					if(m_PolarType==FIXEDSPEEDPOLAR)	       m_Re[i] =  Reynolds;
					else if (m_PolarType==FIXEDLIFTPOLAR)
                    {
						if(Cl>0.0) m_Re[i] =  Reynolds/ sqrt(Cl);
						else m_Re[i] = 0.0;
					}
					else if (m_PolarType==RUBBERCHORDPOLAR)
                    {
						if(Cl>0.0) m_Re[i] =  Reynolds/(Cl);
						else m_Re[i] = 0.0;
					}


					bInserted = true;
					break;
				}
                else if (Alpha < m_Alpha[i])
                {
                    // sort by crescending alphas
                    m_Alpha.insert(i, Alpha);
                    m_Cd.insert(i, Cd);
                    m_Cdp.insert(i, Cdp);
                    m_Cl.insert(i, Cl);
                    m_Cm.insert(i, Cm);
                    m_XTr1.insert(i, Xtr1);
                    m_XTr2.insert(i, Xtr2);
                    m_HMom.insert(i, HMom);
                    m_Cpmn.insert(i, Cpmn);
                    m_ClCd.insert(i, Cl/Cd);
                    m_XCp.insert(i, XCp);

                    if(Cl>0.0)	 m_RtCl.insert(i, 1.0/sqrt(Cl));
                    else         m_RtCl.insert(i, 0.0);

                    if (Cl>=0.0) m_Cl32Cd.insert(i,pow(Cl, 1.5)/ Cd);
                    else         m_Cl32Cd.insert(i,-pow(-Cl, 1.5)/ Cd);

					if(m_PolarType==FIXEDSPEEDPOLAR)	 m_Re.insert(i, Reynolds);
					else if (m_PolarType==FIXEDLIFTPOLAR)
                    {
                        if(Cl>0) m_Re.insert(i, Reynolds/sqrt(Cl));
                        else m_Re[i] = 0.0;
                    }
					else if (m_PolarType==RUBBERCHORDPOLAR)
                    {
                        if(Cl>0.0) m_Re.insert(i, Reynolds/Cl);
                        else       m_Re.insert(i, 0.0);
                    }

                    bInserted = true;
                    break;
                }
            }
			else
			{
				//m_PolarType 4 polar, sort by Reynolds numbers
                if (fabs(Reynolds - m_Re[i]) < 0.1)
				{
					// then erase former result
					m_Alpha[i] =  Alpha;
					m_Cd[i]    =  Cd;
					m_Cdp[i]   =  Cdp;
					m_Cl[i]    =  Cl;
					m_Cm[i]    =  Cm;
					m_XTr1[i]  =  Xtr1;
					m_XTr2[i]  =  Xtr2;
					m_HMom[i]  =  HMom;
					m_Cpmn[i]  =  Cpmn;
					m_ClCd[i]  =  Cl/Cd;
					m_XCp[i]   =  XCp;

					if(Cl>0.0)	 m_RtCl[i] = 1.0/sqrt(Cl);
					else 	     m_RtCl[i] = 0.0;
                    if (Cl>=0.0) m_Cl32Cd[i] = pow(Cl, 1.5)/ Cd;
                    else         m_Cl32Cd[i] = -pow(-Cl, 1.5)/ Cd;
					m_Re[i] =  Reynolds;

					bInserted = true;
					break;
				}
                else if (Reynolds < m_Re[i]){// sort by crescending Reynolds numbers
                    m_Alpha.insert(i, Alpha);
                    m_Cd.insert(i, Cd);
                    m_Cdp.insert(i, Cdp);
                    m_Cl.insert(i, Cl);
                    m_Cm.insert(i, Cm);
                    m_XTr1.insert(i, Xtr1);
                    m_XTr2.insert(i, Xtr2);
                    m_HMom.insert(i, HMom);
                    m_Cpmn.insert(i, Cpmn);
                    m_ClCd.insert(i, Cl/Cd);
                    m_XCp.insert(i, XCp);

                    if(Cl>0.0)   m_RtCl.insert(i, 1.0/(double)sqrt(Cl));
                    else         m_RtCl.insert(i, 0.0);
                    if (Cl>=0.0) m_Cl32Cd.insert(i, pow(Cl, 1.5)/ Cd);
                    else         m_Cl32Cd.insert(i,-pow(-Cl, 1.5)/ Cd);

                    m_Re.insert(i, Reynolds);

                    bInserted = true;
                    break;
                }
            }
		}
	}
    if(!bInserted)
    {
        // data is appended at the end
        m_Alpha.insert(size, Alpha);
        m_Cd.insert(size, Cd);
        m_Cdp.insert(size, Cdp);
        m_Cl.insert(size, Cl);
        m_Cm.insert(size, Cm);
        m_XTr1.insert(size, Xtr1);
        m_XTr2.insert(size, Xtr2);
        m_HMom.insert(size, HMom);
        m_Cpmn.insert(size, Cpmn);
        m_ClCd.insert(size, Cl/Cd);
        m_XCp.insert(size, XCp);

        if(Cl>0.0)   m_RtCl.insert(size, 1.0/(double)sqrt(Cl));
        else         m_RtCl.insert(size, 0.0);
        if (Cl>=0.0) m_Cl32Cd.insert(size,(double)pow(Cl, 1.5)/ Cd);
        else         m_Cl32Cd.insert(size,-(double)pow(-Cl, 1.5)/ Cd);

		if(m_PolarType==FIXEDSPEEDPOLAR || m_PolarType==FIXEDAOAPOLAR) m_Re.insert(size, Reynolds);
		else if (m_PolarType==FIXEDLIFTPOLAR)
        {
            if(Cl>0) m_Re.insert(size, Reynolds/(double) sqrt(Cl));
            else     m_Re.insert(size, 0.0);
        }
		else if (m_PolarType==RUBBERCHORDPOLAR)
        {
            if(Cl>0.0) m_Re.insert(size, Reynolds/Cl);
            else     m_Re.insert(size, 0.0);
        }
    }
}


/**
 * Copies the polar's data from an existing polar
 * @param pPolar a pointer to the instance of the reference Polar object from which the data should be copied
 */
void Polar::Copy(Polar *pPolar)
{
	int i;
	int size  = (int)m_Alpha.size();
	for(i=size-1; i>=0; i--)
		Remove(i);

	size  = (int)pPolar->m_Alpha.size();
	for(i=0; i<size; i++)
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
//	qint32 colorref;
//	int r,g,b;

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

/*		ar >> qi;

		m_PlrName="";
		for(j=0; j<qi;j++)
		{
			m_PlrName += " ";
			ar >>ch;
			m_PlrName[j] = char(ch);
		}*/

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
//		b = (int)(colorref/256/256);
//		colorref -= b*256*256;
//		g = (int)(colorref/256);
//		r = colorref - g*256;
//		m_Color = QColor(r,g,b,255);

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
                    if(fabs(Alpha-m_Alpha[j])<0.001)
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
                    if(fabs(Re-m_Re[j])<0.1)
					{
						bExists = true;
						break;
					}
				}
			}
//if(bTrace) qDebug() << i<<Alpha<<Cd<< Cdp<< Cl<<Cm<< XTr1<< XTr2<< HMom<< Cpmn<< Re<< XCp;
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
* @param Alpha0 the aoa such that Cl = 0, in degrees
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

	for (int k=0; k<n; k++){
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
*@deprecated
*Returns an equivalent grey tone for the style of this polar.
*Used for B&W printing
*/
void Polar::GetBWStyle(QColor &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
//	GetBWColor(color, style, width);
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
		strong = QString(QObject::tr("Reynolds number")+" = %1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==FIXEDLIFTPOLAR)
	{
		strong = QString("Re.sqrt(Cl) = %1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString("Ma.sqrt(Cl) = %1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==RUBBERCHORDPOLAR)
	{
		strong = QString(QObject::tr("Re.Cl")+" = %1\n").arg(m_Reynolds,0,'f',0);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}
	else if(m_PolarType==FIXEDAOAPOLAR)
	{
		strong = QString(QObject::tr("Alpha")+" = %1"+QString::fromUtf8("°")+"\n").arg(m_ASpec,7,'f',2);
		PolarProperties += strong;
		strong = QString(QObject::tr("Mach number") + " = %1\n").arg(m_Mach,5,'f',2);
		PolarProperties += strong;
	}


	strong = QString(QObject::tr("NCrit") + " = %1\n").arg(m_ACrit,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Forced top trans.") + " = %1\n").arg(m_XTop,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Forced bottom trans.") + " = %1\n").arg(m_XBot,6,'f',2);
	PolarProperties += strong;

	strong = QString(QObject::tr("Number of data points") +" = %1").arg(m_Alpha	.size());
	PolarProperties += "\n" +strong;

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ExportPolar(out, pMainFrame->m_ExportFileType, true);
	PolarProperties += "\n"+strong;
}



/**
* Returns the name of the variable for a given index
*@param iVar the index of the variable
*@param &Name the reference of the QString object to be filled with the variable's name.
*/
void Polar::GetPlrVariableName(int iVar, QString &Name)
{
	switch (iVar)
	{
		case 0:
			Name = QObject::tr("Alpha");
			break;
		case 1:
			Name = QObject::tr("Cl");
			break;
		case 2:
			Name = QObject::tr("Cd");
			break;
		case 3:
			Name = QObject::tr("Cd x 10000");
			break;
		case 4:
			Name = QObject::tr("Cdp");
			break;
		case 5:
			Name = QObject::tr("Cm");
			break;
		case 6:
			Name = QObject::tr("Xtr1");
			break;
		case 7:
			Name = QObject::tr("Xtr2");
			break;
		case 8:
			Name = QObject::tr("HMom");
			break;
		case 9:
			Name = QObject::tr("Cpmin");
			break;
		case 10:
			Name = QObject::tr("Cl/Cd");
			break;
		case 11:
			Name = QObject::tr("|Cl|^(3/2)/Cd");
			break;
		case 12:
			Name = QObject::tr("1/Rt(Cl)");
			break;
		case 13:
			Name = QObject::tr("Re");
			break;
		case 14:
			Name = QObject::tr("XCp");
			break;
		default:
			Name = QObject::tr("Alpha");
			break;
	}
}
