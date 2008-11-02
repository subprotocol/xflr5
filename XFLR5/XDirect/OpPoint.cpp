/****************************************************************************

    OpPoint Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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


// OpPoint.cpp: implementation of the OpPoint class.
// The Class associated to OpPoint objects
// resulting from XFoil calculations
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "OpPoint.h"
//#include "Foil.h"
#include "XDirect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OpPoint::OpPoint()
{
	m_pXDirect = NULL;
	m_bVisc     = false;//not a  viscous point a priori
	m_bDispSurf = false;// no boundary layer surface either
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
	memset(x,  0, sizeof(y));
	memset(y,  0, sizeof(x));
//	memset(s,  0, sizeof(s));
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
	m_Color = RGB(255,0,100);

}

OpPoint::~OpPoint()
{

}


// This Object is  used to store the data issued from an XFoil Calculation
// an array of OperatingPoints is a CPolar


bool OpPoint::SerializeOpp(CArchive &ar, int ArchiveFormat)
{

	int a, b, k, Format;
	float f,g;

	if(ar.IsStoring()){
		ar << 100003;
		//100003 : suppressed archiving of s coordinate
		//100002 : first numbered archiveformat
		//write variables
		ar << m_strFoilName << m_strPlrName;
		ar << (float)Reynolds << (float)Mach << (float)Alpha;
		ar << n << nd1 << nd2 << nd3;
		if(m_bVisc)     a=1; else a=0;
		if(m_bDispSurf) b=1; else b=0;

		ar << a << b;
		ar << (float)Cl << (float)Cm << (float)Cd << (float)Cdp;
		ar << (float)Xtr1 << (float)Xtr2 << (float)ACrit << (float)m_TEHMom << (float)Cpmn;
		for (k=0; k<n; k++)	ar << (float)Cpv[k] << (float)Cpi[k];
//		for (k=0; k<n; k++)	    ar << (float)s[k] << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<n; k++)	    ar << (float)Qv[k] << (float)Qi[k];
		for (k=0; k<=nd1; k++)  ar << (float)xd1[k] << (float)yd1[k];
		for (k=0; k<nd2; k++)	ar << (float)xd2[k] << (float)yd2[k];
		for (k=0; k<nd3; k++)	ar << (float)xd3[k] << (float)yd3[k];
		ar<<m_Style<<m_Width<<m_Color;
		if(m_bIsVisible)     ar<<1; else ar<<0;
		if(m_bShowPoints)    ar<<1; else ar<<0;
	}
	else {
		try{
			if(ArchiveFormat>=100002) ar>>Format;
			else Format = 0;
			//read variables
			ar >> m_strFoilName;
			if(m_strFoilName==""){
				return false;
			}
			ar >> m_strPlrName;
			
			ar >> f; Reynolds =f;
			ar >> f; Mach = f;
			ar >> f; Alpha = f;
			ar >> n >> nd1 >> nd2 >> nd3;
			ar >> a >> b;
			if(a) m_bVisc = true; else m_bVisc = false;
			if(a!=0 && a!=1){
				m_strFoilName = "";
				return false;
			}
			if(b) m_bDispSurf = true; else m_bDispSurf = false;
			if(b!=0 && b!=1){
				m_strFoilName = "";
				return false;
			}
			
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
				for (k=0; k<n; k++){
					if(Format<=100002)	ar >> f; //s[k]  = f;
					ar >> f; Qv[k] = f;
					ar >> f; Qi[k] = f;
				}
//			}
			for (k=0; k<=nd1; k++)  {
				ar >> f >> g;
				xd1[k] = f;
				yd1[k] = g;
			}
			for (k=0; k<nd2; k++)  {
				ar >> f >> g;
				xd2[k] = f;
				yd2[k] = g;
			}
			for (k=0; k<nd3; k++)  {
				ar >> f >> g;
				xd3[k] = f;
				yd3[k] = g;
			}
			if(ArchiveFormat>=100002) {
				ar>>m_Style>>m_Width>>m_Color;
				ar >> a ;
				if(a!=0 && a!=1){
					m_strFoilName = "";
					return false;
				}
				if(a) m_bIsVisible = true; else m_bIsVisible = false;

				ar >> a ;
				if(a!=0 && a!=1){
					m_strFoilName = "";
					return false;
				}
				if(a) m_bShowPoints = true; else m_bShowPoints = false;
			}

		}
		catch (CArchiveException *ex){
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error importing OpPoint : ");
			str += szCause;
			ex->Delete();
			m_strFoilName = "";
			return false;
		}
	}
	return true;
}



void OpPoint::Export(CString FileName, CString Version)
{
	CStdioFile XFile;
	CFileException fe;

	try{
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);//erase and write

		if(!bOpen){
			fe.m_strFileName = FileName;
			throw &fe;
		}
		else{
			CString strOut;
			CString strong;
			XFile.WriteString(Version);
			XFile.WriteString("\n");
			strong = m_strFoilName + "\n";
			XFile.WriteString(strong);
			strong = m_strPlrName + "\n";
			XFile.WriteString(strong);
			strong.Format("Alpha = %5.1f,  Re = %5.1f,  Ma= %6.4f,  ACrit=%4.1f \n\n", Alpha, Reynolds, Mach, ACrit);
			XFile.WriteString(strong);

			strong.Format("   x        Cpi      Cpv        Qi        Qv\n");
			XFile.WriteString(strong);

			for (int k=0; k<n; k++){
				strong.Format("%7.4f  %7.3f   %7.3f   %7.3f   %7.3f\n",
					x[k], Cpi[k], Cpv[k], Qi[k], Qv[k]);
				
				XFile.WriteString(strong);
			}

			XFile.WriteString("\n\n");
			XFile.Close();
		}
	}
	catch (CFileException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error writing OpPoint : ");
		str += szCause;
		AfxMessageBox(str);
//			ex->Delete();
	}
}




