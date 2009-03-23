/****************************************************************************

    WPolar Class
    Copyright (C) 2005-2008 André Deperrois XFLR5@yahoo.com

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


// WPolar.cpp: implementation of the CWPolar class.
// The class fo wing polars
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "WPolar.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWPolar::CWPolar(CWnd* pParent)
{
	m_pParent     = pParent;// is MainFrame
	m_bIsVisible  = true;
	m_bShowPoints = false;

	m_bVLM1         = true;
	m_bThinSurfaces = true;
	m_bWakeRollUp   = false;
	m_bTiltedGeom   = false;
	m_bViscous      = true;
	m_bPolar        = true;
	m_bGround       = false;

	m_NXWakePanels = 1;
	m_TotalWakeLength = 1.0;
	m_WakePanelFactor =1.1;

	m_RefAreaType = 1;

	m_AnalysisType = 0;
	m_Type   = 1;
	m_Style  = 0;
	m_Width  = 1;
	m_Color  = 255;//red

	m_Beta      = 0.0;
	m_QInf      = 10.0;
	m_Weight    = 1.0;
	m_XCmRef    = 0.0;
	m_ASpec     = 0.0;
	m_WArea     = 0.0;
	m_WMAChord  = 0.0;
	m_WSpan     = 0.0;
	m_AMem      = 0.0;
	m_Height    = 0.0;
	m_Density   = 1.225;
	m_Viscosity = 1.5e-5;//m²/s

	m_nControls = 0;
	memset(m_MinControl, 0, sizeof(m_MinControl));
	memset(m_MaxControl, 0, sizeof(m_MaxControl));
	memset(m_bActiveControl, 0, sizeof(m_bActiveControl));

	pi = 3.141592654;
}

void CWPolar::Copy(CWPolar *pWPolar)
{
	int i;
	m_bIsVisible    = pWPolar->m_bIsVisible;
	m_bPolar        = pWPolar->m_bPolar;
	m_bShowPoints   = pWPolar->m_bShowPoints;
	m_bTiltedGeom   = pWPolar->m_bTiltedGeom;
	m_bViscous      = pWPolar->m_bViscous;
	m_bVLM1         = pWPolar->m_bVLM1;
	m_bWakeRollUp   = pWPolar->m_bWakeRollUp;
	m_AnalysisType  = pWPolar->m_AnalysisType;
	m_bThinSurfaces = pWPolar->m_bThinSurfaces;
	m_nControls     = pWPolar->m_nControls;

	int size  = (int)m_Alpha.GetSize();
	for(i=size-1; i>=0; i--)
		Remove(i);
	
	size  = (int)pWPolar->m_Alpha.GetSize();
	for(i=0; i<size; i++)
	{
		m_Alpha.InsertAt(i,      pWPolar->m_Alpha[i],       1);
		m_Cl.InsertAt(i,         pWPolar-> m_Cl[i],         1);
		m_CY.InsertAt(i,         pWPolar-> m_CY[i],         1);
		m_ICd.InsertAt(i,        pWPolar-> m_ICd[i],        1);
		m_PCd.InsertAt(i,        pWPolar-> m_PCd[i],        1);
		m_TCd.InsertAt(i,        pWPolar-> m_TCd[i],        1);

		m_GCm.InsertAt(i,        pWPolar-> m_GCm[i],        1);
		m_GRm.InsertAt(i,        pWPolar-> m_GRm[i],        1);
		m_GYm.InsertAt(i,        pWPolar-> m_GYm[i],        1);
		m_VYm.InsertAt(i,        pWPolar-> m_VYm[i],        1);
		m_IYm.InsertAt(i,        pWPolar-> m_IYm[i],        1);

		m_ClCd.InsertAt(i,       pWPolar-> m_ClCd[i],       1);
		m_1Cl.InsertAt(i,        pWPolar-> m_1Cl[i],        1);
		m_Cl32Cd.InsertAt(i,     pWPolar-> m_Cl32Cd[i],     1);
		m_QInfinite.InsertAt(i,  pWPolar-> m_QInfinite[i],  1);

		m_Gamma.InsertAt(i,      pWPolar-> m_Gamma[i],      1);
		m_XCP.InsertAt(i,        pWPolar-> m_XCP[i],        1);
		m_YCP.InsertAt(i,        pWPolar-> m_YCP[i],        1);
		m_MaxBending.InsertAt(i, pWPolar-> m_MaxBending[i], 1);
		m_VertPower.InsertAt(i,  pWPolar-> m_VertPower[i],  1);

		m_Oswald.InsertAt(i,     pWPolar-> m_Oswald[i],     1);
		m_SM.InsertAt(i,         pWPolar-> m_SM[i],         1);
		m_Ctrl.InsertAt(i,       pWPolar-> m_Ctrl[i],       1);
		m_L.InsertAt(i,          pWPolar-> m_L[i],          1);
		m_D.InsertAt(i,          pWPolar-> m_D[i],          1);
		m_Vx.InsertAt(i,         pWPolar-> m_Vx[i],         1);

		m_Vz.InsertAt(i,         pWPolar-> m_Vz[i],         1);
		m_Pm.InsertAt(i,         pWPolar-> m_Pm[i],         1);
		m_Ym.InsertAt(i,         pWPolar-> m_Ym[i],         1);
		m_Rm.InsertAt(i,         pWPolar-> m_Rm[i],         1);
	}
}


CWPolar::~CWPolar()
{
	ResetWPlr();
}


void CWPolar::Export(CString FileName, int FileType)
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	int j;
	CStdioFile XFile;
	CFileException fe;
	BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);//erase and write

	try
	{
		if (!bOpen)
		{
//			CFileException *pfe = new CFileException(CFileException::invalidFile);
//			pfe->m_strFileName = FileName;
			throw &fe;
		}
		if (FileType==1)
		{
			CString strOut;
			CString Header, strong;

			strong=  pFrame->m_VersionName +"\n";
			XFile.WriteString(strong);

			SYSTEMTIME tm;
			GetLocalTime(&tm);
			CString str;
			str.Format("%02d/%02d/%d  at  %02d:%02d:%02d \n", tm.wMonth, tm.wDay, tm.wYear,tm.wHour, tm.wMinute, tm.wSecond);
			XFile.WriteString(str);


			strong ="Wing name :        "+ m_UFOName + "\n";
			XFile.WriteString(strong);

			strong ="Wing polar name :  "+ m_PlrName + "\n";
			XFile.WriteString(strong);

			GetSpeedUnit(str, pFrame->m_SpeedUnit);
			str +="\n\n";
			strong.Format("Freestream speed : %3.1f ",m_QInf*pFrame->m_mstoUnit);
			strong +=str;
			XFile.WriteString(strong);

			Header.Format("   alpha      CL           ICd        PCd        TCd        CY        GCm         GRm        GYm       IYm       QInf        XCP\n");
			XFile.WriteString(Header);
	//		Header.Format(" _________  ________   ________  _________  _________  _________  _________  _________  _________  _________  _________  _________\n");
	//		XFile.WriteString(Header); 
			for (j=0; j<m_Alpha.GetSize(); j++)
			{
				strong.Format(" %8.3f  %9.6f    %9.6f  %9.6f  %9.6f  %9.6f  %9.6f  %9.6f  %9.6f  %9.6f  %8.4f    %9.4f\n",
					m_Alpha[j],	m_Cl[j], m_ICd[j], m_PCd[j], m_TCd[j], m_CY[j], m_GCm[j], m_GRm[j], m_GYm[j], m_IYm[j], m_QInfinite[j], m_XCP[j]);
				
				XFile.WriteString(strong);
			
			}
		}	
		else if (FileType==2)
		{
			CString strOut;
			CString Header, strong;

			strong=  pFrame->m_VersionName +"\n";
			XFile.WriteString(strong);

			SYSTEMTIME tm;
			GetLocalTime(&tm);
			CString str;
			str.Format("%02d/%02d/%d  at  %02d:%02d:%02d \n", tm.wMonth, tm.wDay, tm.wYear,tm.wHour, tm.wMinute, tm.wSecond);
			XFile.WriteString(str);


			strong ="Wing name :, "+ m_UFOName + "\n";
			XFile.WriteString(strong);

			strong ="Wing polar name :, "+ m_PlrName + "\n";
			XFile.WriteString(strong);

			GetSpeedUnit(str, pFrame->m_SpeedUnit);
			str +="\n\n";
			strong.Format("Freestream speed :, %3.1f ",m_QInf*pFrame->m_mstoUnit);
			strong +=str;
			XFile.WriteString(strong);

			Header.Format("alpha, CL, ICd, PCd, TCd, CY, GCm, GRm,GYm, IYm, QInf, XCP\n");
			XFile.WriteString(Header);
	//		Header.Format(" _________  ________   ________  _________  _________  _________  _________  _________  _________  _________  _________  _________\n");
	//		XFile.WriteString(Header); 
			for (j=0; j<m_Alpha.GetSize(); j++)
			{
				strong.Format(" %8.3f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %8.4f,  %9.4f\n",
					m_Alpha[j],	m_Cl[j], m_ICd[j], m_PCd[j], m_TCd[j], m_CY[j], m_GCm[j], m_GRm[j], m_GYm[j], m_IYm[j], m_QInfinite[j], m_XCP[j]);
				
				XFile.WriteString(strong);
			
			}
		}	
		XFile.WriteString("\n\n");
		XFile.Close();
		return ;
		

	}
	catch (CFileException *ex)
	{
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error exporting wing polar : ");
		str += szCause;
		AfxMessageBox(str);
		return ;
	}

}

void CWPolar::ResetWPlr()
{
	m_Alpha.RemoveAll();
	m_Cl.RemoveAll();
	m_CY.RemoveAll();
	m_ICd.RemoveAll();
	m_PCd.RemoveAll();
	m_TCd.RemoveAll();

	m_GCm.RemoveAll();
	m_GRm.RemoveAll();
	m_GYm.RemoveAll();
	m_VYm.RemoveAll();
	m_IYm.RemoveAll();

	m_XCP.RemoveAll();
	m_YCP.RemoveAll();
	m_MaxBending.RemoveAll();
	m_VertPower.RemoveAll();

	m_Oswald.RemoveAll();
	m_SM.RemoveAll();
	m_Ctrl.RemoveAll();
	m_ClCd.RemoveAll();
	m_1Cl.RemoveAll();
	m_Cl32Cd.RemoveAll();

	m_QInfinite.RemoveAll();
	m_Gamma.RemoveAll();
	m_L.RemoveAll();
	m_D.RemoveAll();
	m_Vx.RemoveAll();

	m_Vz.RemoveAll();
	m_Pm.RemoveAll();
	m_Ym.RemoveAll();
	m_Rm.RemoveAll();
}


bool CWPolar::SerializeWPlr(CArchive &ar)
{
	int n;
	float f;
	int i, ArchiveFormat;// identifies the format of the file
	//Call Base class function
	if(ar.IsStoring())
	{
		//write variables
		
		ar << 1016; // identifies the format of the file
					// 1016 : added reference area type 
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
		ar << m_UFOName << m_PlrName;
		ar << (float)m_WArea << (float)m_WMAChord << (float)m_WSpan ;
		ar << m_Style  << m_Width << m_Color;
		ar << m_AnalysisType;
		if (m_bVLM1)         ar << 1; else ar << 0;
		if (m_bThinSurfaces) ar << 1; else ar << 0;
		if (m_bTiltedGeom)   ar << 1; else ar << 0;
		if (m_bWakeRollUp)   ar << 1; else ar << 0;
		if (m_bViscous)      ar << 1; else ar << 0;
		if (m_bGround)       ar << 1; else ar << 0;
		ar << (float)m_Height;

		ar << m_NXWakePanels << (float)m_TotalWakeLength << (float)m_WakePanelFactor;

		if (m_bIsVisible)  ar << 1; else ar << 0;
		if (m_bShowPoints) ar << 1; else ar << 0;
		ar << m_Type;
		ar << (float)m_QInf;
		ar << (float)m_Weight;
		ar << (float)m_ASpec ;
		ar << (float)m_Beta ;
		ar << (float)m_XCmRef;
		ar <<(float)m_Density << (float)m_Viscosity;
	
		ar << m_RefAreaType;

		ar <<(int)m_Alpha.GetSize();
		for (i=0; i< (int)m_Alpha.GetSize(); i++)
		{
			ar << (float)m_Alpha[i] << (float)m_Cl[i] << (float)m_CY[i] << (float)m_ICd[i] << (float)m_PCd[i] ;

			ar << (float)m_GCm[i] << (float)m_GRm[i] << (float)m_GYm[i] << 0.0f << (float)m_VYm[i] << (float)m_IYm[i];

			ar << (float)m_QInfinite[i];

			ar << (float)m_XCP[i] << (float)m_YCP[i] << (float)m_MaxBending[i];

			ar << (float)m_Ctrl[i];
		}

		ar << m_nControls;
		for(i=0; i<m_nControls; i++)
		{
			ar << (float)m_MinControl[i] << (float)m_MaxControl[i];
		}
		for(i=0; i<m_nControls; i++)
		{
			if(m_bActiveControl[i])	ar<<1 ; else ar <<0;
		}

		return true;
	}
	else 
	{
		//read variables
		ar >> ArchiveFormat;
		if (ArchiveFormat <=1000 || ArchiveFormat>1100) {
			m_PlrName ="";
			return false;
		}
		ar >> m_UFOName >> m_PlrName;
		if (m_UFOName.GetLength()==0){
			m_PlrName ="";
			return false;
		}
		if (m_PlrName.GetLength()==0){
			return false;
		}

		ar>> f;
		m_WArea = f;
		if (m_WArea<0){
			m_PlrName ="";
			return false;
		}

		ar>> f;
		m_WMAChord = f;
		if (m_WMAChord<0){
			m_PlrName ="";
			return false;
		}

		ar>> f;
		m_WSpan = f;
		if (m_WSpan<0){
			m_PlrName ="";
			return false;
		}

		ar >> m_Style  >> m_Width >> m_Color;
		if (m_Style<0 || m_Style> 10){
			m_PlrName ="";
			return false;
		}
		if (m_Width<0 || m_Width> 10){
			m_PlrName ="";
			return false;
		}
		if (m_Color<0 || m_Color> RGB(255,255,255)){
			m_PlrName ="";
			return false;
		}
		ar >> m_AnalysisType; 
		if (m_AnalysisType<0 || m_AnalysisType>10){
			m_PlrName ="";
			return false;
		}
		if(m_AnalysisType==0) m_AnalysisType=2;

		if(ArchiveFormat>=1005){
			ar >> n; 
			if (n!=0 && n!=1){
				m_PlrName ="";
				return false;
			}
			if(n) m_bVLM1 =true; else m_bVLM1 = false;
			ar >> n; 
			if (n!=0 && n!=1){
				m_PlrName ="";
				return false;
			}
			if(n) m_bThinSurfaces =true; else m_bThinSurfaces = false;
		}
		if(ArchiveFormat>=1008){
			ar >> n; 
			if (n!=0 && n!=1){
				m_PlrName ="";
				return false;
			}
			if(n) m_bTiltedGeom =true; else m_bTiltedGeom = false;
		}

		if(ArchiveFormat>=1006)
		{
			ar >> n; 
			if (n!=0 && n!=1){
				m_PlrName ="";
				return false;
			}
			if(n) m_bWakeRollUp =true; else m_bWakeRollUp = false;
		}
		if(ArchiveFormat>=1009){
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bViscous =true; else m_bViscous = false;
		}


		if(ArchiveFormat>=1010)
		{
			ar >> n; 
			if (n!=0 && n!=1){
				m_PlrName ="";
				return false;
			}
			if(n) m_bGround =true; else m_bGround = false;
			ar >> f; m_Height = f;
		}

		if(ArchiveFormat>=1007){
			ar >> m_NXWakePanels; 
			if (m_NXWakePanels<0 || m_NXWakePanels>1000){
				m_PlrName ="";
				return false;
			}
		}

		if(ArchiveFormat>=1011)
		{
			ar >> f; 			m_TotalWakeLength  = f;
			ar >> f; 			m_WakePanelFactor = f;
		}

		ar >> n; 
		if (n!=0 && n!=1)
		{
			m_PlrName ="";
			return false;
		}
		else {
			if(n) m_bIsVisible =true; else m_bIsVisible = false;
		}
		ar >> n; 
		if (n!=0 && n!=1)
		{
			m_PlrName ="";
			return false;
		}
		else {
			if(n) m_bShowPoints =true; else m_bShowPoints = false;
		}

		ar >> n;
		if (n<1 || n>10)
		{
			m_PlrName ="";
			return false;
		}
		m_Type = n;

		ar >> f;	m_QInf = f;
		ar >> f;	m_Weight = f;
		ar >> f;	m_ASpec = f;
		if(ArchiveFormat>=1015) ar >> f;	m_Beta = f;

		if(ArchiveFormat>=1002) ar >> f; m_XCmRef = f;

		ar >> f;	m_Density=f;
		ar >> f;	m_Viscosity=f;

		if(ArchiveFormat>=1016) ar >> m_RefAreaType;
		else                    m_RefAreaType=1;

		ar >> n;
		if (n<0 || n> 100000)
		{
			m_PlrName ="";
			return false;
		}
		if(ArchiveFormat<1010)
		{
			m_WArea    /=100.0;
			m_WMAChord /=1000.0;
			m_WSpan    /=1000.0;
			m_XCmRef   /=1000.0;
		}
	
		float Alpha,  Cl, CY=0.0, ICd, PCd, GCm, GRm, GYm, VCm, VYm, IYm, QInfinite, XCP, YCP, Ctrl;
		float Cb = 0.0;
		bool bExists;
		for (i=0; i< n; i++)
		{
			try
			{
				ar >> Alpha >> Cl;
				if(ArchiveFormat>=1015) ar>>CY;
				ar >> ICd >> PCd;
				ar >> GCm >> GRm >> GYm >> VCm >> VYm >> IYm;
		        if(ArchiveFormat<1012) GCm = GRm = GYm = VCm = VYm = IYm = 0.0; 
				ar >> QInfinite >> XCP >> YCP;
				if(ArchiveFormat<1010)
				{
					XCP   /=1000.0;
					YCP   /=1000.0;
				}

				if (ArchiveFormat>=1003) 
					ar >> Cb;
				else 
					Cb = 0.0;

				if (ArchiveFormat>=1014) ar >> Ctrl;
				else					 Ctrl = 0.0;

			}
			catch (CArchiveException *ex)
			{
				ex->Delete();
				m_PlrName ="";
				return false;
			}
			bExists = false;
			if(m_Type!=4)
			{
				for (int j=0; j<m_Alpha.GetSize(); j++)
				{
					if(abs(Alpha-m_Alpha[j])<0.001)
					{
						bExists = true;
						break;
					}
				}
			}
			else 
			{
				for (int j=0; j<m_Alpha.GetSize(); j++)
				{
					if(abs(QInfinite-m_QInfinite[j])<0.001)
					{
						bExists = true;
						break;
					}
				}
			}
			if(!bExists)
			{
				m_Alpha.Add(Alpha);
				m_Cl.Add(Cl);
				m_CY.Add(CY);
				m_ICd.Add(ICd);
				m_PCd.Add(PCd);
				m_TCd.Add(ICd+PCd);

				m_GCm.Add(GCm);
				m_GRm.Add(GRm);
				m_GYm.Add(GYm);
				m_VYm.Add(VYm);
				m_IYm.Add(IYm);

				m_QInfinite.Add(QInfinite);

				m_XCP.Add(XCP);
				m_YCP.Add(YCP);
				m_MaxBending.Add(Cb);
				m_Ctrl.Add(Ctrl);

				m_1Cl.Add(0.0);
				m_ClCd.Add(0.0);
				m_Cl32Cd.Add(0.0);
				m_Vx.Add(0.0);
				m_Vz.Add(0.0);
				m_L.Add(0.0);
				m_D.Add(0.0);
				m_Gamma.Add(0.0);
				m_Rm.Add(0.0);
				m_Pm.Add(0.0);
				m_Ym.Add(0.0);
				m_VertPower.Add(0.0);
				m_Oswald.Add(0.0);
				m_SM.Add(0.0);

				CalculatePoint((int)m_Alpha.GetSize()-1);
			}
		}
		if(ArchiveFormat>1012)
		{
			ar >> m_nControls;
			if(abs(m_nControls)>1000) m_nControls = 0;
			for(i=0; i<m_nControls; i++)
			{
				ar >> f; m_MinControl[i] = f;
				ar >> f; m_MaxControl[i] = f;
			}
			for(i=0; i<m_nControls; i++)
			{
				ar >> n; 
				if (n!=0 && n!=1){
					m_PlrName ="";
					return false;
				}
				else {
					if(n) m_bActiveControl[i] =true; else m_bActiveControl[i] = false;
				}
			}
		}
	}

	return true;
}


void CWPolar::CalculatePoint(int i)
{
	//finish calculations
//	double cl = m_Cl[i];
//	double tcd = m_TCd[i];
//	double Qinf =  m_QInfinite[i];

	m_ClCd[i]   =  m_Cl[i]/m_TCd[i];
				
	if(m_Cl[i]>0.0) {
		m_1Cl[i]    = (double)(1./sqrt(m_Cl[i]));
		m_Cl32Cd[i] = (double)pow(m_Cl[i],1.5)/m_TCd[i];
	}
	else {
		m_1Cl[i]    = -1.0;//will not be plotted
		m_Cl32Cd[i] =  -(double)pow(-m_Cl[i],1.5)/m_TCd[i];
	}

	if(abs(m_Cl[i])>0.) m_Gamma[i]  =  atan(m_TCd[i]/m_Cl[i]) * 180.0/pi;
	else m_Gamma[i] = 90.0;
	m_Vz[i] = (double)sqrt(2*m_Weight*9.81/m_Density/m_WArea)/m_Cl32Cd[i];
	m_Vx[i] = m_QInfinite[i] * (double)cos(m_Gamma[i]*pi/180.0);

	//dynamic pressure
	double q =  0.5 * m_Density * m_QInfinite[i]*m_QInfinite[i];

	m_L[i]  = q * m_Cl[i]*m_WArea;
	m_D[i]  = q * m_TCd[i]*m_WArea;

	m_Rm[i] = q * m_WArea * m_GRm[i] * m_WSpan;// in N.m
	m_Ym[i] = q * m_WArea * m_GYm[i] * m_WSpan;// in N.m
	m_Pm[i] = q * m_WArea * m_GCm[i] * m_WMAChord;// in N.m

	//power for horizontal flight
	m_VertPower[i] = m_Weight * 9.81 * m_Vz[i];

	double AR      = m_WSpan*m_WSpan/m_WArea;

	if(m_ICd[i]==0.0)	m_Oswald[i] = 0.0;
	else				m_Oswald[i] = m_Cl[i]*m_Cl[i]/pi/m_ICd[i]/AR;

	m_SM[i]        = (m_XCP[i]-m_XCmRef)/m_WMAChord *100.00;
}


/*	N =  Cn.q.s.b
	L =  Ct.q.s.b
	R =  Cm.q.s.c'
*/

void CWPolar::Remove(int i)
{
	m_Alpha.RemoveAt(i);
	m_Cl.RemoveAt(i);
	m_CY.RemoveAt(i);
	m_ICd.RemoveAt(i);
	m_PCd.RemoveAt(i);
	m_TCd.RemoveAt(i);

	m_GCm.RemoveAt(i);
	m_GRm.RemoveAt(i);
	m_GYm.RemoveAt(i);
	m_VYm.RemoveAt(i);
	m_IYm.RemoveAt(i);

	m_XCP.RemoveAt(i);
	m_YCP.RemoveAt(i);
	m_MaxBending.RemoveAt(i);
	m_VertPower.RemoveAt(i);

	m_Oswald.RemoveAt(i);
	m_SM.RemoveAt(i);
	m_Ctrl.RemoveAt(i);
	m_ClCd.RemoveAt(i);
	m_1Cl.RemoveAt(i);
	m_Cl32Cd.RemoveAt(i);

	m_QInfinite.RemoveAt(i);
	m_Gamma.RemoveAt(i);
	m_L.RemoveAt(i);
	m_D.RemoveAt(i);
	m_Vx.RemoveAt(i);

	m_Vz.RemoveAt(i);
	m_Pm.RemoveAt(i);
	m_Ym.RemoveAt(i);
	m_Rm.RemoveAt(i);
}

void CWPolar::GetBWStyle(COLORREF &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
	GetBWColor(color, style, width);
}

void CWPolar::AddPoint(CWOpp *pWOpp)
{
	bool bInserted = false;
	int i;
	int size = (int)m_Alpha.GetSize();

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_Type <4)
			{
				if (abs(pWOpp->m_Alpha - m_Alpha[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  =  pWOpp->m_QInf;
					m_XCP[i]        =  pWOpp->m_XCP;
					m_YCP[i]        =  pWOpp->m_YCP;
					m_MaxBending[i] =  pWOpp->m_MaxBending;
					m_Ctrl[i]       =  pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pWOpp->m_Alpha < m_Alpha[i])
				{
					// sort by crescending alphas
					m_Alpha.InsertAt(i, pWOpp->m_Alpha, 1);
					m_Cl.InsertAt(i,  pWOpp->m_CL);
					m_CY.InsertAt(i,  pWOpp->m_CY);
					m_ICd.InsertAt(i, pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i, pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i, pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i, pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i, pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i, pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i, pWOpp->m_IYm, 1);

					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_XCP.InsertAt(i,  pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,  pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i, pWOpp->m_Ctrl);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if (m_Type==4)
			{
				// type 4, sort by speed
				if (abs(pWOpp->m_QInf - m_QInfinite[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;	
					m_Ctrl[i]       = pWOpp->m_Ctrl;

					bInserted = true;
					break;
				}
				else if (pWOpp->m_QInf < m_QInfinite[i])
				{
					// sort by crescending alphas
					m_Alpha.InsertAt(i, pWOpp->m_Alpha, 1);
					m_Cl.InsertAt(i, pWOpp->m_CL);
					m_CY.InsertAt(i, pWOpp->m_CY);
					m_ICd.InsertAt(i, pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i, pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i, pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i, pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i, pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i, pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i, pWOpp->m_IYm, 1);

					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_XCP.InsertAt(i,  pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,  pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i, pWOpp->m_Ctrl);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);
					
					bInserted = true;
					break;
				}
			}
			else if (m_Type==5 || m_Type==6)
			{
				// type 5 or 6, sort by crescending ctrl value
				if (abs(pWOpp->m_Ctrl - m_Ctrl[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;	
					m_Ctrl[i]       = pWOpp->m_Ctrl;

					bInserted = true;
					break;
				}
				else if (pWOpp->m_Ctrl < m_Ctrl[i])
				{
					// sort by crescending control values
					m_Alpha.InsertAt(i, pWOpp->m_Alpha, 1);
					m_Cl.InsertAt(i, pWOpp->m_CL);
					m_CY.InsertAt(i, pWOpp->m_CY);
					m_ICd.InsertAt(i, pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i, pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i, pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i, pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i, pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i, pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i, pWOpp->m_IYm, 1);

					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_XCP.InsertAt(i,  pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,  pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i, pWOpp->m_Ctrl);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);
					
					bInserted = true;
					break;
				}
			}
		}
	}
	if(bInserted) CalculatePoint(i);
	else
	{
		// data is appended at the end
		m_Alpha.Add(pWOpp->m_Alpha);
		m_Cl.Add(pWOpp->m_CL);
		m_CY.Add(pWOpp->m_CY);
		m_ICd.Add(pWOpp->m_InducedDrag);
		m_PCd.Add(pWOpp->m_ViscousDrag);
		m_TCd.Add(pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

		m_GCm.Add(pWOpp->m_GCm);
		m_GRm.Add(pWOpp->m_GRm);
		m_GYm.Add(pWOpp->m_GYm);
		m_VYm.Add(pWOpp->m_VYm);
		m_IYm.Add(pWOpp->m_IYm);

		m_QInfinite.Add(pWOpp->m_QInf);
		m_XCP.Add(pWOpp->m_XCP);
		m_YCP.Add(pWOpp->m_YCP);
		m_MaxBending.Add(pWOpp->m_MaxBending);
		m_Ctrl.Add(pWOpp->m_Ctrl);

		m_1Cl.Add(0.0);//make room for computed values
		m_ClCd.Add(0.0);
		m_Cl32Cd.Add(0.0);
		m_Vx.Add(0.0);
		m_Vz.Add(0.0);
		m_L.Add(0.0);
		m_D.Add(0.0);
		m_Gamma.Add(0.0);
		m_Rm.Add(0.0);
		m_Pm.Add(0.0);
		m_Ym.Add(0.0);
		m_VertPower.Add(0.0);
		m_Oswald.Add(0.0);
		m_SM.Add(0.0);

		CalculatePoint(size);
	}
}

void CWPolar::AddPoint(CPOpp *pPOpp)
{
	bool bInserted = false;
	int i;
	int size = (int)m_Alpha.GetSize();
	CWOpp *pWOpp = &pPOpp->m_WingWOpp;

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_Type <4)
			{
				if (abs(pPOpp->m_Alpha - m_Alpha[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_QInfinite[i]  =  pWOpp->m_QInf;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_XCP[i]        =  pWOpp->m_XCP;
					m_YCP[i]        =  pWOpp->m_YCP;
					m_MaxBending[i] =  pWOpp->m_MaxBending;	
					m_Ctrl[i]       =  pWOpp->m_Ctrl; 
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Alpha < m_Alpha[i])
				{
					// sort by crescending alphas
					m_Alpha.InsertAt(i, pWOpp->m_Alpha, 1);
					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_Cl.InsertAt(i,   pWOpp->m_CL);
					m_CY.InsertAt(i,   pWOpp->m_CY);
					m_ICd.InsertAt(i,  pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i,  pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i,  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i,  pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i,  pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i,  pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i,  pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i,  pWOpp->m_IYm, 1);

					m_XCP.InsertAt(i,  pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,  pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i, pWOpp->m_Ctrl);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if(m_Type==4)
			{
				// type 4, sort by speed
				if (abs(pPOpp->m_QInf - m_QInfinite[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      = pWOpp->m_Alpha;
					m_Cl[i]         = pWOpp->m_CL;
					m_CY[i]         = pWOpp->m_CY;
					m_ICd[i]        = pWOpp->m_InducedDrag;
					m_PCd[i]        = pWOpp->m_ViscousDrag;
					m_TCd[i]        = pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        = pWOpp->m_GCm;
					m_GRm[i]        = pWOpp->m_GRm;
					m_GYm[i]        = pWOpp->m_GYm;
					m_VYm[i]        = pWOpp->m_VYm;
					m_IYm[i]        = pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;	
					m_Ctrl[i]       = pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pPOpp->m_QInf < m_QInfinite[i])
				{
					// sort by crescending speed
					m_Alpha.InsertAt(i,     pWOpp->m_Alpha, 1);
					m_Cl.InsertAt(i,        pWOpp->m_CL);
					m_CY.InsertAt(i,        pWOpp->m_CY);
					m_ICd.InsertAt(i,       pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i,       pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i,       pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i,       pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i,       pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i,       pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i,       pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i,       pWOpp->m_IYm, 1);

					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_XCP.InsertAt(i,       pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,       pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i,       pWOpp->m_Ctrl, 1);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if(m_Type==5 || m_Type==6)
			{
				// type 5 or 6, sort by control value
				if (abs(pPOpp->m_Ctrl - m_Ctrl[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      = pWOpp->m_Alpha;
					m_Cl[i]         = pWOpp->m_CL;
					m_CY[i]         = pWOpp->m_CY;
					m_ICd[i]        = pWOpp->m_InducedDrag;
					m_PCd[i]        = pWOpp->m_ViscousDrag;
					m_TCd[i]        = pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        = pWOpp->m_GCm;
					m_GRm[i]        = pWOpp->m_GRm;
					m_GYm[i]        = pWOpp->m_GYm;
					m_VYm[i]        = pWOpp->m_VYm;
					m_IYm[i]        = pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;	
					m_Ctrl[i]       = pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Ctrl < m_Ctrl[i])
				{
					// sort by crescending control values
					m_Alpha.InsertAt(i,     pWOpp->m_Alpha, 1);
					m_Cl.InsertAt(i,        pWOpp->m_CL);
					m_CY.InsertAt(i,        pWOpp->m_CY);
					m_ICd.InsertAt(i,       pWOpp->m_InducedDrag, 1);
					m_PCd.InsertAt(i,       pWOpp->m_ViscousDrag, 1);
					m_TCd.InsertAt(i,       pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag, 1);

					m_GCm.InsertAt(i,       pWOpp->m_GCm, 1);
					m_GRm.InsertAt(i,       pWOpp->m_GRm, 1);
					m_GYm.InsertAt(i,       pWOpp->m_GYm, 1);
					m_VYm.InsertAt(i,       pWOpp->m_VYm, 1);
					m_IYm.InsertAt(i,       pWOpp->m_IYm, 1);

					m_QInfinite.InsertAt(i, pWOpp->m_QInf, 1);
					m_XCP.InsertAt(i,       pWOpp->m_XCP, 1);
					m_YCP.InsertAt(i,       pWOpp->m_YCP, 1);
					m_MaxBending.InsertAt(i, pWOpp->m_MaxBending);
					m_Ctrl.InsertAt(i,       pWOpp->m_Ctrl, 1);

					m_1Cl.InsertAt(i,0.0);//make room for computed values
					m_ClCd.InsertAt(i,0.0);
					m_Cl32Cd.InsertAt(i,0.0);
					m_Vx.InsertAt(i,0.0);
					m_Vz.InsertAt(i,0.0);
					m_L.InsertAt(i,0.0);
					m_D.InsertAt(i,0.0);
					m_Gamma.InsertAt(i,0.0);
					m_Rm.InsertAt(i, 0.0);
					m_Pm.InsertAt(i, 0.0);
					m_Ym.InsertAt(i, 0.0);
					m_VertPower.InsertAt(i, 0.0);
					m_Oswald.InsertAt(i, 0.0);
					m_SM.InsertAt(i, 0.0);

					bInserted = true;
					break;
				}
			}
		}
	}
	if(bInserted) CalculatePoint(i);
	else
	{
		// data is appended at the end
		m_Alpha.Add(pWOpp->m_Alpha);
		m_Cl.Add(pWOpp->m_CL);
		m_CY.Add(pWOpp->m_CY);
		m_ICd.Add(pWOpp->m_InducedDrag);
		m_PCd.Add(pWOpp->m_ViscousDrag);
		m_TCd.Add(pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

		m_GCm.Add(pWOpp->m_GCm);
		m_GRm.Add(pWOpp->m_GRm);
		m_GYm.Add(pWOpp->m_GYm);
		m_VYm.Add(pWOpp->m_VYm);
		m_IYm.Add(pWOpp->m_IYm);

		m_QInfinite.Add(pWOpp->m_QInf);
		m_XCP.Add(pWOpp->m_XCP);
		m_YCP.Add(pWOpp->m_YCP);
		m_MaxBending.Add(pWOpp->m_MaxBending);
		m_Ctrl.Add(pWOpp->m_Ctrl);
		m_1Cl.Add(0.0);//make room for computed values
		m_ClCd.Add(0.0);
		m_Cl32Cd.Add(0.0);
		m_Vx.Add(0.0);
		m_Vz.Add(0.0);
		m_L.Add(0.0);
		m_D.Add(0.0);
		m_Gamma.Add(0.0);
		m_Rm.Add(0.0);
		m_Pm.Add(0.0);
		m_Ym.Add(0.0);
		m_VertPower.Add(0.0);
		m_Oswald.Add(0.0);
		m_SM.Add(0.0);
		
		CalculatePoint(size);
	}
}