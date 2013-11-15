/****************************************************************************

    OpPoint Class
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


#include "OpPoint.h"
#include "../globals.h"
#include "../mainframe.h"

void *OpPoint::s_pMainFrame;


/**
 * The public constructor
 */
OpPoint::OpPoint()
{
	m_bViscResults     = false;//not a  viscous point a priori
	m_bBL = false;// no boundary layer surface either
	m_bTEFlap   = false;
	m_bLEFlap   = false;

	Xtr1   = 0.0;
	Xtr2   = 0.0;
	XForce = 0.0;
	YForce = 0.0;
	Cpmn   = 0.0;
	m_XCP  = 0.0;
	m_LEHMom   = 0.0; m_TEHMom = 0.0;

	memset(Qi,  0, sizeof(Qi));
	memset(Qv,  0, sizeof(Qv));
	memset(Cpi,  0, sizeof(Cpi));
	memset(Cpv,  0, sizeof(Cpv));
//	memset(x,  0, sizeof(x));
//	memset(y,  0, sizeof(y));

	memset(xd1,  0, sizeof(xd1));
	memset(xd2,  0, sizeof(xd2));
	memset(xd3,  0, sizeof(xd3));
	memset(yd1,  0, sizeof(yd1));
	memset(yd2,  0, sizeof(yd2));
	memset(yd3,  0, sizeof(yd3));
	nd1 = 0;
	nd2 = 0;
	nd3 = 0;
//	Format = 1;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;
	m_Width = 1;
    m_Color = QColor(255,0,100,127);

}




/**
 * Loads or saves the data of this OpPoint to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool OpPoint::Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	int a, b, k, Format;
	float f,gg;

    if(bIsStoring)
    {
		ar << 100003;
		//100003 : suppressed archiving of s coordinate
		//100002 : first numbered archiveformat
		//write variables
		WriteCString(ar, m_strFoilName);
		WriteCString(ar, m_strPlrName);
		ar << (float)Reynolds << (float)Mach << (float)Alpha;
		ar << n << nd1 << nd2 << nd3;
		if(m_bViscResults)     a=1; else a=0;
		if(m_bBL) b=1; else b=0;

		ar << a << b;
		ar << (float)Cl << (float)Cm << (float)Cd << (float)Cdp;
		ar << (float)Xtr1 << (float)Xtr2 << (float)ACrit << (float)m_TEHMom << (float)Cpmn;
		for (k=0; k<n; k++)	ar << (float)Cpv[k] << (float)Cpi[k];
//		for (k=0; k<n; k++)	    ar << (float)s[k] << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<n; k++)	    ar << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<=nd1; k++)  ar << (float)xd1[k] << (float)yd1[k];
		for (k=0; k<nd2; k++)	ar << (float)xd2[k] << (float)yd2[k];
		for (k=0; k<nd3; k++)	ar << (float)xd3[k] << (float)yd3[k];
		ar<<m_Style<<m_Width;
		WriteCOLORREF(ar,m_Color);
		if(m_bIsVisible)     ar<<1; else ar<<0;
		if(m_bShowPoints)    ar<<1; else ar<<0;
	}
	else
	{

        if(ArchiveFormat>=100002) ar>>Format;
        else Format = 0;
        //read variables
		ReadCString(ar, m_strFoilName);
		ReadCString(ar, m_strPlrName);

        ar >> f; Reynolds =f;
        ar >> f; Mach = f;
        ar >> f; Alpha = f;
        ar >> n >> nd1 >> nd2 >> nd3;
        ar >> a >> b;
        if(a) m_bViscResults = true; else m_bViscResults = false;
		if(a!=0 && a!=1) return false;

        if(b) m_bBL = true; else m_bBL = false;
		if(b!=0 && b!=1) return false;

        ar >> f; Cl = f;
        ar >> f; Cm = f;
        ar >> f; Cd = f;
        ar >> f; Cdp = f;
        ar >> f; Xtr1 = f;
        ar >> f; Xtr2 = f;
        ar >> f; ACrit =f;
        ar >> f; m_TEHMom = f;
        ar >> f; Cpmn = f;
        for (k=0; k<n; k++)	{
            ar >> f; Cpv[k] = f;
            ar >> f; Cpi[k] = f;
        }

//			if (Format ==2) {
		for (k=0; k<n; k++)
		{
			if(Format<=100002)	ar >> f; //s[k]  = f;
			ar >> f; Qv[k] = f;
			ar >> f; Qi[k] = f;
		}
//			}
		for (k=0; k<=nd1; k++)
		{
			ar >> f >> gg;
            xd1[k] = f;
			yd1[k] = gg;
        }
		for (k=0; k<nd2; k++)
		{
			ar >> f >> gg;
            xd2[k] = f;
			yd2[k] = gg;
        }
		for (k=0; k<nd3; k++)
		{
			ar >> f >> gg;
            xd3[k] = f;
			yd3[k] = gg;
        }
		if(ArchiveFormat>=100002)
		{
			ar>>m_Style>>m_Width;
			ReadCOLORREF(ar, m_Color);

			ar >> a ;
			if(a!=0 && a!=1) return false;
            if(a) m_bIsVisible = true; else m_bIsVisible = false;

            ar >> a ;
			if(a!=0 && a!=1) return false;

            if(a) m_bShowPoints = true; else m_bShowPoints = false;
        }
	}
	return true;
}


/**
 * Exports the data of the polar to a text file
 * @param out the instance of output QtextStream
 * @param Version the version name of the program
 * @param FileType TXT if the data is separated by spaces, CSV for a comma separator
 * @param bDataOnly true if the analysis parameters should not be output
 */

void OpPoint::ExportOpp(QTextStream &out, QString Version, enumTextFileType FileType, bool bDataOnly)
{
	int k;
	QString strong;

	if(!bDataOnly)
	{
		out << Version+"\n";

		strong = m_strFoilName + "\n";
		out<< strong;
		strong = m_strPlrName + "\n";
		out<< strong;
		if(FileType==TXT) strong=QString("Alpha = %1,  Re = %2,  Ma = %3,  ACrit =%4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		else              strong=QString("Alpha =, %1,  Re =, %2,  Ma =, %3,  ACrit =, %4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		out<< strong;
	}

	if(FileType==TXT) out << "   x        Cpi      Cpv        Qi        Qv\n";
	else            out << "x,Cpi,Cpv,Qi,Qv\n";

	for (k=0; k<n; k++)
	{
		if(FileType==CSV) strong=QString("%1  %2   %3   %4   %5\n")
									   .arg(MainFrame::curFoil()->x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		else              strong=QString("%1,%2,%3,%4,%5\n")
									   .arg(MainFrame::curFoil()->x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		out<< strong;
	}

	out << "\n\n";
}


/**
 * Returns a QString object holding the description and value of the OpPoint's parameters
 * @param &OpPointProperties the reference of the QString object to be filled with the description
 * @param bData true if the analysis data should be appended to the string
 */
void OpPoint::GetOppProperties(QString &OpPointProperties, bool bData)
{
	QString strong;
	OpPointProperties.clear();

	strong  = QString(QObject::tr("Re")+"    = %1 ").arg(Reynolds,7,'f',0);
	OpPointProperties += strong +"\n";

	strong  = QString(QObject::tr("Alpha")+" = %1").arg(Alpha,6,'f',2);
	OpPointProperties += strong +QString::fromUtf8("°")+"\n";

	strong  = QString(QObject::tr("Mach")+"  = %1 ").arg(Mach,7,'f',3);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("NCrit")+" = %1 ").arg(ACrit,5,'f',1);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CL")+"    = %1 ").arg(Cl,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CD")+"    = %1 ").arg(Cd,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cm")+"    = %1 ").arg(Cm,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cdp")+"   = %1 ").arg(Cdp,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cpmn")+"  = %1 ").arg(Cpmn,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("XCP")+"   = %1 ").arg(m_XCP,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Top Transition")+" = %1 ").arg(Xtr1,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Bot Transition")+" = %1 ").arg(Xtr2,9,'f',5);
	OpPointProperties += strong + "\n";

	if(m_bTEFlap)
	{
		strong  = QString(QObject::tr("T.E. Flap moment")+" = %1 ").arg(m_TEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}
	if(m_bLEFlap)
	{
		strong  = QString(QObject::tr("L.E. Flap moment")+" = %1 ").arg(m_LEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ExportOpp(out, pMainFrame->m_VersionName, pMainFrame->m_ExportFileType, true);
	OpPointProperties += "\n"+strong;
}



