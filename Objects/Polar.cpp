/****************************************************************************

    Polar Class
	Copyright (C) 2003 Andre Deperrois XFLR5@yahoo.com

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


// Polar.cpp: implementation of the CPolar class.
//
//////////////////////////////////////////////////////////////////////


#include "Polar.h"
#include "../Globals.h"
#include "../XDirect/XFoil.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolar::CPolar()
{
	pi = 3.141592654;
	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;// = PS_SOLID
	m_Width = 1;
	m_ASpec = 0.0;
	m_Type   = 1;
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


CPolar::~CPolar()
{
}


void CPolar::ExportPolar(QTextStream &out, int FileType)
{
    QString strOut, Header, strong;
	int j;

//	strong ="\n    " + pFrame->m_VersionName + "\n\n";
    out << "\n    QFLR5_v.0001\n\n";
	strong =" Calculated polar for: ";
	strong += m_FoilName + "\n\n";
    out << strong;
	strong = QString(" %1 %2").arg(m_ReType).arg(m_MaType);
	if(m_ReType==1) strong += " Reynolds number fixed       ";
	else if(m_ReType==2) strong += " Reynolds number ~ 1/sqrt(CL)";
	else if(m_ReType==3) strong += " Reynolds number ~ 1/CL      ";
	if(m_MaType==1) strong += "   Mach number fixed         ";
	else if(m_MaType==2) strong += "   Mach number ~ 1/sqrt(CL)  ";
	else if(m_MaType==3) strong += "   Mach number ~ 1/CL        ";
	strong +="\n\n";
    out << strong;
	strong=QString(" xtrf =   %1 (top)        %2 (bottom)\n")
					.arg(m_XTop,0,'f',3).arg(m_XBot,0,'f',3);
    out << strong;

	strong = QString(" Mach = %1     Re = %2 e 6     Ncrit = %3\n\n")
			 .arg(m_Mach,7,'f',3).arg(m_Reynolds/1.e6,9,'f',3).arg(m_ACrit,7,'f',3);
    out << strong;

	if(m_Type != 4)
	{
        if(FileType==1)	Header = "  alpha     CL        CD       CDp       CM    Top Xtr Bot Xtr   Cpmin    Chinge    XCp    \n";
        else            Header = "alpha,CL,CD,CDp,CM,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n";
        out << Header;
		if(FileType==1)
		{
            Header=QString(" ------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
            out << Header;
		}
        for (j=0; j<m_Alpha.size(); j++)
        {
			if(FileType==1)	strong = QString(" %1  %2  %3  %4  %5")
											.arg(m_Alpha[j],7,'f',3)
											.arg(m_Cl[j],7,'f',4)
											.arg(m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			else            strong = QString("%1,%2,%3,%4,%5")
											.arg(m_Alpha[j],7,'f',3)
											.arg(m_Cl[j],7,'f',4)
											.arg(m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);

            out << strong;
            if(m_XTr1[j]<990.0)
			{
				if(FileType==1)	strong=QString("  %1  %2")
											.arg(m_XTr1[j],6,'f',4).arg( m_XTr2[j],6,'f',4);
				else            strong=QString(",%1,%2")
											.arg(m_XTr1[j],6,'f',4).arg( m_XTr2[j],6,'f',4);
                out << strong;
			}
            if(FileType==1)	strong=QString("  %1  %2  %3\n")
											.arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
            else            strong=QString(",%1,%2,%3\n")
											.arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
            out << strong;
        }
	}
	else 
	{
        if(FileType==1) Header=QString("  alpha     Re      CL        CD       CDp       CM    Top Xtr Bot Xtr   Cpmin    Chinge     XCp    \n");
        else            Header=QString("alpha,Re,CL,CD,CDp,CM,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n");
        out << Header;
		if(FileType==1)
		{
            Header=QString(" ------- -------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
            out << Header;
		}
        for (j=0; j<m_Alpha.size(); j++)
		{
			if(FileType==1) strong=QString(" %1 %2  %3  %4  %5  %6")
											.arg(m_Alpha[j],7,'f',3)
											.arg( m_Re[j],8,'f',0)
											.arg( m_Cl[j],7,'f',4)
											.arg( m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			else            strong=QString(" %1,%2,%3,%4,%5,%6")
											.arg(m_Alpha[j],7,'f',3)
											.arg( m_Re[j],8,'f',0)
											.arg( m_Cl[j],7,'f',4)
											.arg( m_Cd[j],8,'f',5)
											.arg(m_Cdp[j],8,'f',5)
											.arg(m_Cm[j],7,'f',4);
			out << strong;
			if(m_XTr1[j]<990.0)
			{
				if(FileType==1) strong=QString("  %1  %2")
											.arg(m_XTr1[j],6,'f',4).arg(m_XTr2[j],6,'f',4);
				else            strong=QString(",%1,%2")
											.arg(m_XTr1[j],6,'f',4).arg(m_XTr2[j],6,'f',4);
                out << strong;
            }
			if(FileType==1)	strong=QString("  %1  %2  %3\n")
											.arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			else            strong=QString(",%1,%2,%3\n")
											.arg(m_Cpmn[j],7,'f',4).arg(m_HMom[j],7,'f',4).arg(m_XCp[j],7,'f',4);
			out << strong;
        }
	}
    out << "\n\n";
	}


void CPolar::ResetPolar()
{
//	Removes all existing OpPoints results from polar
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

void CPolar::AddData(OpPoint *pOpPoint)
{
	//Adds the OpPoint data to the Polar object for further use
	if(!pOpPoint->m_bVisc) return;
	m_ACrit = pOpPoint->ACrit;
	AddPoint(pOpPoint->Alpha, pOpPoint->Cd, pOpPoint->Cdp, pOpPoint->Cl, pOpPoint->Cm,
			 pOpPoint->Xtr1, pOpPoint->Xtr2, pOpPoint->m_TEHMom, pOpPoint->Cpmn, pOpPoint->Reynolds,
			 pOpPoint->m_XCP);
}


void CPolar::AddData(void* ptrXFoil)
{
	XFoil *pXFoil = (XFoil*)ptrXFoil;
	if(!pXFoil->lvisc) return;
	double alpha = pXFoil->alfa*180.0/pi;
	m_ACrit = pXFoil->acrit;

	AddPoint(alpha, pXFoil->cd, pXFoil->cdp, pXFoil->cl, pXFoil->cm, pXFoil->xoctr[1],
			 pXFoil->xoctr[2], pXFoil->hmom, pXFoil->cpmn, pXFoil->reinf1, pXFoil->xcp);
	
}

void CPolar::AddPoint(double Alpha, double Cd, double Cdp, double Cl, double Cm, double Xtr1,
					  double Xtr2, double HMom, double Cpmn, double Reynolds, double XCp)
{
	int i;
	bool bInserted = false;
    int size = (int)m_Alpha.size();
	if(size)
	{
		for ( i=0; i<size; i++)
		{
			if(m_Type != 4)
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

					if(m_Type == 1)	       m_Re[i] =  Reynolds;
                    else if (m_Type == 2)
                    {
						if(Cl>0.0) m_Re[i] =  Reynolds/ sqrt(Cl);
						else m_Re[i] = 0.0;
					}
                    else if (m_Type == 3)
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

                    if(m_Type == 1)	 m_Re.insert(i, Reynolds);
                    else if (m_Type == 2)
                    {
                        if(Cl>0) m_Re.insert(i, Reynolds/sqrt(Cl));
                        else m_Re[i] = 0.0;
                    }

                    else if (m_Type == 3)
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
				//m_Type 4 polar, sort by Reynolds numbers
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

        if(m_Type == 1 || m_Type ==4) m_Re.insert(size, Reynolds);
        else if (m_Type == 2)
        {
            if(Cl>0) m_Re.insert(size, Reynolds/(double) sqrt(Cl));
            else     m_Re.insert(size, 0.0);
        }
        else if (m_Type == 3)
        {
            if(Cl>0.0) m_Re.insert(size, Reynolds/Cl);
            else     m_Re.insert(size, 0.0);
        }
    }
}


void CPolar::Copy(CPolar *pPolar)
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

void CPolar::Serialize(QDataStream &ar, bool bIsStoring)
{
    int i, j, n, l;
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
        ar << m_Type << m_MaType << m_ReType  ;
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
	}
    else
    {
		//read variables
		float Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp;
		int iRe;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 ||ArchiveFormat>1100)
		{
			m_FoilName ="";
			return;
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

		if(m_FoilName =="" || m_PlrName =="" ) {
			m_FoilName ="";
			return;
		}
		ar >> m_Type >> m_MaType >> m_ReType;
		if(m_Type!=1 && m_Type!=2 && m_Type!=3 && m_Type!=4) {
			m_FoilName ="";
			return;
		}
		if(m_MaType!=1 && m_MaType!=2 && m_MaType!=3) {
			m_FoilName ="";
			return;
		}
		if(m_ReType!=1 && m_ReType!=2 && m_ReType!=3) {
			m_FoilName ="";
			return;
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
				m_FoilName ="";
				return;
			}
			if (l) m_bIsVisible =true; else m_bIsVisible = false;
		}

		ar >> l;
        if(l!=0 && l!=1 )
        {
			m_FoilName ="";
			return;
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
			if(m_Type!=4)
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
			if(!bExists)
			{
				AddPoint(Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp);
			}
		}
		if(ArchiveFormat>=1003)
			ar >> m_ACrit >> m_XTop >> m_XBot;
	}
}

void CPolar::Remove(int i)
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

void CPolar::GetAlphaLimits(double &amin, double &amax)
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

void CPolar::GetClLimits(double &Clmin, double &Clmax)
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

double CPolar::GetCm0()
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
	double rr  = m_Cl[k];
	double rr1 = m_Cl[k+1];

    while (m_Cl[k+1]<0.0)
    {
		rr  = m_Cl[k];
		rr1 = m_Cl[k+1];
		k++;
	}
    if(k+1>=m_Cm.size()) return 0.0;
	double Cm0 = m_Cm[k] + (m_Cm[k+1]-m_Cm[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Cm0;

}

double CPolar::GetZeroLiftAngle()
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
	double rr  = m_Cl[k];
	double rr1 = m_Cl[k+1];

    while (m_Cl[k+1]<0.0)
    {
		rr  = m_Cl[k];
		rr1 = m_Cl[k+1];
		k++;
	}
    if(k+1>=m_Alpha.size()) return 0.0;
	double Alpha0 = m_Alpha[k] + (m_Alpha[k+1]-m_Alpha[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Alpha0;

}

void CPolar::GetLinearizedCl(double &Alpha0, double &slope)
{
	// linearize Cl vs. Alpha set of points by least square method
    int n = (int)m_Cl.size();

    if(n<=1)
    {
		Alpha0 = 0.0;
		slope = 2.0*pi*pi/180.0;
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
	if(fn*sum4 == sum2*sum2 || fn*sum1 == sum2 * sum3) {//very improbable...
		Alpha0 = 0.0;
		slope = 2.0*pi*pi/180.0;
		return;
	}

	b1 = (fn*sum1 - sum2 * sum3)/(fn*sum4 - sum2*sum2);
	b2 = (sum3 - b1 * sum2)/fn;

	slope  = b1; //in cl/�
	Alpha0 = -b2/b1;

}

void CPolar::GetBWStyle(QColor &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
//	GetBWColor(color, style, width);
}
