/****************************************************************************

    Polar Class
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


// Polar.cpp: implementation of the CPolar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
//#include "XDirect/XDirect.h"
#include "Polar.h"
#include "../main/MainFrm.h"


//IMPLEMENT_SERIAL(CPolar, CObject, 1)

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

CPolar::CPolar(CWnd *pParent)
{
	pi = 3.141592654;
	m_pFrame = pParent;
	m_bIsVisible = true;
	m_bShowPoints = false;
	m_Style    = 0;// = PS_SOLID
	m_Width    = 1;
	m_ASpec    = 0.0;
	m_Type     = 1;
	m_ReType   = 1;
	m_MaType   = 1;
	m_Reynolds = 100000.0;
	m_Mach     = 0.0;
	m_ACrit    = 9.0;
	m_XTop     = 1.0;
	m_XBot     = 1.0;
	m_FoilName = "";
	m_PlrName  = "";
}


CPolar::~CPolar()
{
}

void CPolar::Copy(CPolar *pPolar)
{
	int i;
	int size  = (int)m_Alpha.GetSize();
	for(i=size-1; i>=0; i--)
		Remove(i);
	
	size  = (int)pPolar->m_Alpha.GetSize();
	for(i=0; i<size; i++)
	{
		m_Alpha.InsertAt(i,  pPolar->m_Alpha[i],  1);
		m_Cd.InsertAt(i,     pPolar->m_Cd[i],     1);
		m_Cdp.InsertAt(i,    pPolar->m_Cdp[i],    1);
		m_Cl.InsertAt(i,     pPolar-> m_Cl[i],    1);
		m_Cm.InsertAt(i,     pPolar->m_Cm[i],     1);
		m_XTr1.InsertAt(i,   pPolar->m_XTr1[i],   1);
		m_XTr2.InsertAt(i,   pPolar->m_XTr2[i],   1);
		m_HMom.InsertAt(i,   pPolar->m_HMom[i],   1);
		m_Cpmn.InsertAt(i,   pPolar->m_Cpmn[i],   1);
		m_ClCd.InsertAt(i,   pPolar->m_ClCd[i],   1);
		m_RtCl.InsertAt(i,   pPolar->m_RtCl[i],   1);
		m_Cl32Cd.InsertAt(i, pPolar->m_Cl32Cd[i], 1);
		m_Re.InsertAt(i,     pPolar->m_Re[i],     1);
		m_XCp.InsertAt(i,    pPolar->m_XCp[i],     1);
	}
}

void CPolar::Export(CString FileName, int FileType)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CStdioFile XFile;
	CFileException fe;
	CString strOut, Header, strong;
	int j;

	try
	{
		BOOL bOpen = XFile.Open(FileName, 
			CFile::modeCreate | CFile::modeWrite, &fe);//erase and write
		if(!bOpen)
		{
//			CFileException *pfe = new CFileException(CFileException::invalidFile);
//			pfe->m_strFileName = FileName;
			throw &fe;
		}
	}
	catch (CFileException *ex)
	{
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error exporting polar : ");
		str += szCause;
		AfxMessageBox(str);
//		ex->Delete();
		return ;
	}

	strong ="\n    " + pFrame->m_VersionName + "\n\n";
	XFile.WriteString(strong);
	strong =" Calculated polar for: ";
	strong += m_FoilName + "\n\n";
	XFile.WriteString(strong);
	strong.Format(" %d %d",m_ReType, m_MaType);
	if(m_ReType==1) strong += " Reynolds number fixed       ";
	else if(m_ReType==2) strong += " Reynolds number ~ 1/sqrt(CL)";
	else if(m_ReType==3) strong += " Reynolds number ~ 1/CL      ";
	if(m_MaType==1) strong += "   Mach number fixed         ";
	else if(m_MaType==2) strong += "   Mach number ~ 1/sqrt(CL)  ";
	else if(m_MaType==3) strong += "   Mach number ~ 1/CL        ";
	strong +="\n\n";
	XFile.WriteString(strong);
	strong.Format(" xtrf =   %.3f (top)        %.3f (bottom)\n",
		m_XTop, m_XBot);
	XFile.WriteString(strong);	
	
	strong.Format(" Mach = %7.3f     Re = %9.3f e 6     Ncrit = %7.3f\n\n", 
		m_Mach, m_Reynolds/1.e6, m_ACrit);
	XFile.WriteString(strong);

	if(m_Type != 4)
	{
		if(FileType==1)	Header.Format("  alpha     CL        CD       CDp       CM    Top Xtr Bot Xtr   Cpmin    Chinge    XCp    \n");
		else            Header.Format("alpha,CL,CD,CDp,CM,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n");
		XFile.WriteString(Header);
		if(FileType==1)
		{
			Header.Format(" ------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
			XFile.WriteString(Header);
		}
		for (j=0; j<m_Alpha.GetSize(); j++)
		{
			if(FileType==1)	strong.Format(" %7.3f  %7.4f  %8.5f  %8.5f  %7.4f",	m_Alpha[j],	m_Cl[j],m_Cd[j], m_Cdp[j],m_Cm[j]);
			else            strong.Format("%7.3f,%7.4f,%8.5f,%8.5f,%7.4f",	m_Alpha[j],	m_Cl[j],m_Cd[j], m_Cdp[j],m_Cm[j]);

			XFile.WriteString(strong);
			if(m_XTr1[j]<990.0)
			{
				if(FileType==1)	strong.Format("  %6.4f  %6.4f",m_XTr1[j], m_XTr2[j]);
				else            strong.Format(",%6.4f,%6.4f",m_XTr1[j], m_XTr2[j]);
				XFile.WriteString(strong);
			}
			if(FileType==1)	strong.Format("  %7.4f  %7.4f  %7.4f\n", m_Cpmn[j],m_HMom[j], m_XCp[j]);
			else            strong.Format(",%7.4f,%7.4f,%7.4f\n", m_Cpmn[j],m_HMom[j], m_XCp[j]);
			XFile.WriteString(strong);
		}
	}
	else 
	{
		if(FileType==1) Header.Format("  alpha     Re      CL        CD       CDp       CM    Top Xtr Bot Xtr   Cpmin    Chinge     XCp    \n");
		else            Header.Format("alpha,Re,CL,CD,CDp,CM,Top Xtr,Bot Xtr,Cpmin,Chinge,XCp\n");
		XFile.WriteString(Header);
		if(FileType==1)
		{
			Header.Format(" ------- -------- -------- --------- --------- -------- ------- ------- -------- --------- ---------\n");
			XFile.WriteString(Header);
		}
		for (j=0; j<m_Alpha.GetSize(); j++)
		{
			if(FileType==1) strong.Format(" %7.3f %8.0f  %7.4f  %8.5f  %8.5f  %7.4f", m_Alpha[j], m_Re[j], m_Cl[j], m_Cd[j], m_Cdp[j], m_Cm[j]);
			else            strong.Format(" %7.3f,%8.0f,%7.4f,%8.5f,%8.5f,%7.4f", m_Alpha[j], m_Re[j], m_Cl[j], m_Cd[j], m_Cdp[j], m_Cm[j]);
			XFile.WriteString(strong);
			if(m_XTr1[j]<990.0)
			{
				if(FileType==1) strong.Format("  %6.4f  %6.4f", m_XTr1[j], m_XTr2[j]);
				else            strong.Format(",%6.4f,%6.4f", m_XTr1[j], m_XTr2[j]);
				XFile.WriteString(strong);
			}
			if(FileType==1) strong.Format("  %7.4f  %7.4f  %7.4f\n", m_Cpmn[j],m_HMom[j], m_XCp[j]);
			else            strong.Format(",%7.4f,%7.4f,%7.4f\n", m_Cpmn[j],m_HMom[j], m_XCp[j]);
			XFile.WriteString(strong);
		}
	}
	XFile.WriteString("\n\n");
	XFile.Close();
}


void CPolar::ResetPolar()
{
//	Removes all existing OpPoints results from polar
	m_Alpha.RemoveAll();
	m_Cl.RemoveAll();
	m_Cd.RemoveAll();
	m_Cdp.RemoveAll();
	m_Cm.RemoveAll();
	m_XTr1.RemoveAll();
	m_XTr2.RemoveAll();
	m_HMom.RemoveAll();
	m_Cpmn.RemoveAll();
	m_ClCd.RemoveAll();
	m_RtCl.RemoveAll();
	m_Cl32Cd.RemoveAll();
	m_Re.RemoveAll();
	m_XCp.RemoveAll();
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

void CPolar::AddData(XFoil* pXFoil)
{
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
	int size = (int)m_Alpha.GetSize();
TRACE("%12.5f     %12.5f     %12.5f     %12.5f \n",Alpha, Cd, Cl, Cm);
	if(size)
	{
		for ( i=0; i<size; i++)
		{
			if(m_Type != 4)
			{
				if (abs(Alpha - m_Alpha[i]) < 0.001)
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
					if (Cl>=0.0) m_Cl32Cd.Add(pow(Cl, 1.5)/ Cd);
					else         m_Cl32Cd.Add(-pow(-Cl, 1.5)/ Cd);

					if(m_Type == 1)	       m_Re[i] =  Reynolds;
					else if (m_Type == 2){
						if(Cl>0.0) m_Re[i] =  Reynolds/ sqrt(Cl);
						else m_Re[i] = 0.0;
					}
					else if (m_Type == 3)  {
						if(Cl>0.0) m_Re[i] =  Reynolds/(Cl);
						else m_Re[i] = 0.0;
					}


					bInserted = true;
					break;
				}
				else if (Alpha < m_Alpha[i])
				{
					// sort by crescending alphas
					m_Alpha.InsertAt(i, Alpha, 1);
					m_Cd.InsertAt(i, Cd, 1);
					m_Cdp.InsertAt(i, Cdp, 1);
					m_Cl.InsertAt(i, Cl, 1);
					m_Cm.InsertAt(i, Cm, 1);
					m_XTr1.InsertAt(i, Xtr1, 1);
					m_XTr2.InsertAt(i, Xtr2, 1);
					m_HMom.InsertAt(i, HMom,1);
					m_Cpmn.InsertAt(i, Cpmn,1);
					m_ClCd.InsertAt(i, Cl/Cd, 1);
					m_XCp.InsertAt(i, XCp, 1);

					if(Cl>0.0)	 m_RtCl.InsertAt(i, 1.0/sqrt(Cl));
					else         m_RtCl.InsertAt(i, 0.0);

					if (Cl>=0.0) m_Cl32Cd.InsertAt(i,pow(Cl, 1.5)/ Cd);
					else         m_Cl32Cd.InsertAt(i,-pow(-Cl, 1.5)/ Cd);

					if(m_Type == 1)	 m_Re.InsertAt(i, Reynolds,1);
					else if (m_Type == 2){
						if(Cl>0) m_Re.InsertAt(i, Reynolds/sqrt(Cl));
						else m_Re[i] = 0.0;
					}

					else if (m_Type == 3){
						if(Cl>0.0) m_Re.InsertAt(i, Reynolds/Cl,1);
						else       m_Re.InsertAt(i, 0.0,1);
					}

					bInserted = true;
					break;
				}
			}
			else
			{
				//m_Type 4 polar, sort by Reynolds numbers
				if (abs(Reynolds - m_Re[i]) < 0.1)
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
					if (Cl>=0.0) m_Cl32Cd.Add(pow(Cl, 1.5)/ Cd);
					else         m_Cl32Cd.Add(-pow(-Cl, 1.5)/ Cd);
					m_Re[i] =  Reynolds;

					bInserted = true;
					break;
				}
				else if (Reynolds < m_Re[i]){// sort by crescending Reynolds numbers
					m_Alpha.InsertAt(i, Alpha, 1);
					m_Cd.InsertAt(i, Cd, 1);
					m_Cdp.InsertAt(i, Cdp, 1);
					m_Cl.InsertAt(i, Cl, 1);
					m_Cm.InsertAt(i, Cm, 1);
					m_XTr1.InsertAt(i, Xtr1, 1);
					m_XTr2.InsertAt(i, Xtr2, 1);
					m_HMom.InsertAt(i, HMom,1);
					m_Cpmn.InsertAt(i, Cpmn,1);
					m_ClCd.InsertAt(i, Cl/Cd, 1);
					m_XCp.InsertAt(i, XCp, 1);

					if(Cl>0.0)   m_RtCl.InsertAt(i, 1.0/(double)sqrt(Cl));
					else         m_RtCl.InsertAt(i, 0.0);
					if (Cl>=0.0) m_Cl32Cd.InsertAt(i, pow(Cl, 1.5)/ Cd);
					else         m_Cl32Cd.InsertAt(i,-pow(-Cl, 1.5)/ Cd);

					m_Re.InsertAt(i, Reynolds,1);

					bInserted = true;
					break;
				}
			}
		}
	}
	if(!bInserted)
	{
		// data is appended at the end
		m_Alpha.InsertAt(size, Alpha, 1);
		m_Cd.InsertAt(size, Cd, 1);
		m_Cdp.InsertAt(size, Cdp, 1);
		m_Cl.InsertAt(size, Cl, 1);
		m_Cm.InsertAt(size, Cm, 1);
		m_XTr1.InsertAt(size, Xtr1, 1);
		m_XTr2.InsertAt(size, Xtr2, 1);
		m_HMom.InsertAt(size, HMom,1);
		m_Cpmn.InsertAt(size, Cpmn,1);
		m_ClCd.InsertAt(size, Cl/Cd, 1);
		m_XCp.InsertAt(size, XCp, 1);

		if(Cl>0.0)   m_RtCl.InsertAt(size, 1.0/(double)sqrt(Cl));
		else         m_RtCl.InsertAt(size, 0.0);
		if (Cl>=0.0) m_Cl32Cd.InsertAt(size,(double)pow(Cl, 1.5)/ Cd);
		else         m_Cl32Cd.InsertAt(size,-(double)pow(-Cl, 1.5)/ Cd);

		if(m_Type == 1 || m_Type ==4) m_Re.InsertAt(size, Reynolds,1);
		else if (m_Type == 2){
			if(Cl>0) m_Re.InsertAt(size, Reynolds/(double) sqrt(Cl),1);
			else     m_Re.InsertAt(size, 0.0,1);
		}
		else if (m_Type == 3){
			if(Cl>0.0) m_Re.InsertAt(size, Reynolds/Cl,1);
			else     m_Re.InsertAt(size, 0.0,1);
		}
	}
}

void CPolar::SerializeOldFormat(CArchive &ar)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	double x[600], y[600];// foil coordinates for graph
	int nf, l;
	int n;
	int ArchiveFormat;// identifies the format of the file
	//Call Base class function
//	CObject::Serialize(ar);
	if(ar.IsStoring()){
		//write variables
		CFoil * pFoil = pFrame->GetFoil(m_FoilName);
		n = (int)m_Alpha.GetSize();
		
		ar << 4; // identifies the format of the file
				 // just in case new formats are added over time
				 // format "1" : v0.31-v0.32
				 // format "2" : v0.33 added color/style/width
				 // format "3" : v1.13 added showpoints
				 // format "4" : v1.23 added m_Re storage for Type 4 polars
		ar << m_FoilName;
		ar << m_PlrName;
		ar << 1;//	if (m_bIsModifiable) l=1; else l=0;
		ar << m_Type << m_MaType << m_ReType  ;
		ar << pFoil->n;
		for (int k=0; k< pFoil->n; k++){
			ar << pFoil->x[k] << pFoil->y[k];
		}
		ar << (int)m_Reynolds << m_Mach ;
		//format 4
		ar << m_ASpec;
		// end format 4
		ar << n << m_ACrit;
		ar << m_XTop << m_XBot;
//		if (m_bFlap) l=1; else l=0;
//		ar << l;
//		ar << m_FlapAngle << m_XHinge << m_YHinge;
		//format "2"
		ar << m_Color << m_Style << m_Width;
		//end format "2"
		//format "3"
		if (m_bShowPoints) l=1; else l=0;
		ar << l;
		//end format "3"
		for (int i=0; i< m_Alpha.GetSize(); i++){
			ar << m_Alpha[i] << m_Cd[i] << m_Cdp[i] <<m_Cl[i] << m_Cm[i];
			ar << m_XTr1[i] << m_XTr2[i];
			ar << m_HMom[i] <<	m_Cpmn[i];
			//format "4"
			ar << m_Re[i];
			// end format "4"
		}
	}
	else {
		//read variables
		double a,b,c;
		double Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re;
		int iRe;
		ar >> ArchiveFormat;
		if (ArchiveFormat !=1 && ArchiveFormat !=2 &&
			ArchiveFormat !=3 && ArchiveFormat !=4) {
			m_FoilName ="";
			return;
		}
		ar >> m_FoilName ;
		ar >> m_PlrName;
		if(m_FoilName =="" || m_PlrName =="" ) {
			m_FoilName ="";
			return;
		}
		ar >> l;
		if(l!=0 && l!=1 ) {
			m_FoilName ="";
			return;
		}
//		if (l) m_bIsModifiable =true; else m_bIsModifiable = false;
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

		ar >> nf;
		for (int k=0; k< nf; k++){
			ar >> x[k] >> y[k];
		}
		
		pFrame->XDirect.AddFoil(m_FoilName, x, y, nf);
		ar >> iRe;
		m_Reynolds = (double) iRe;
		ar >> m_Mach ;

		if(ArchiveFormat >=4){
			ar >> m_ASpec;
		}

		ar >> n >> m_ACrit;
		ar >> m_XTop >> m_XBot;

		ar >> l;//m_bFlap
//		if (l) m_bFlap =true; else m_bFlap = false;
		ar >> a >> b >> c;
		if(ArchiveFormat >=2){
			ar >> m_Color >> m_Style >> m_Width;
		}
		if(ArchiveFormat >=3){
			ar >> l;
			if(l!=0 && l!=1 ) {
				m_FoilName ="";
				return;
			}
			if (l) m_bShowPoints =true; else m_bShowPoints = false;
		}
		bool bExists;
		for (int i=0; i< n; i++){
			ar >> Alpha >> Cd >> Cdp >> Cl >> Cm;
			ar >> XTr1 >> XTr2;
			ar >> HMom >> Cpmn;
			if(ArchiveFormat >=4) ar >> Re;
			else Re = m_Reynolds;

			bExists = false;
			if(m_Type!=4){
				for (int j=0; j<m_Alpha.GetSize(); j++){
					if(fabs(Alpha-m_Alpha[j])<0.001){
						bExists = true;
						break;
					}
				}
			}
			else{
				for (int j=0; j<m_Re.GetSize(); j++){
					if(fabs(Re-m_Re[j])<0.1){
						bExists = true;
						break;
					}
				}
			}
			if(!bExists){
				AddPoint(Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, 0.0);
			}
		}
//		m_bIsVisible = false;
		
//		pXDirect->LoadFoilData(pXDirect->GetFoil(m_FoilName));// loads the coordinates in member variables
	}
}

void CPolar::Serialize(CArchive &ar)
{
	int n, l;
	int ArchiveFormat;// identifies the format of the file
	float f;

	if(ar.IsStoring()){
		//write variables
		n = (int)m_Alpha.GetSize();
		
		ar << 1004; // identifies the format of the file
		// 1004 : added XCp
		// 1003 : re-instated NCrit, XtopTr and XBotTr with polar
		ar << m_FoilName;
		ar << m_PlrName;
		ar << m_Type << m_MaType << m_ReType  ;
		ar << (int)m_Reynolds << (float)m_Mach ;
		ar << (float)m_ASpec;
		ar << n << (float)m_ACrit;
		ar << (float)m_XTop << (float)m_XBot;
		ar << m_Color << m_Style << m_Width;
		if (m_bIsVisible)  ar<<1; else ar<<0;
		if (m_bShowPoints) ar<<1; else ar<<0;
		
		for (int i=0; i< m_Alpha.GetSize(); i++){
			ar << (float)m_Alpha[i] << (float)m_Cd[i] ;
			ar << (float)m_Cdp[i] << (float)m_Cl[i] << (float)m_Cm[i];
			ar << (float)m_XTr1[i] << (float)m_XTr2[i];
			ar << (float)m_HMom[i] << (float)m_Cpmn[i];
			ar << (float)m_Re[i];
			ar << (float)m_XCp[i];
		}

		ar << m_ACrit << m_XTop << m_XBot;
	}
	else {
		//read variables
		float Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp;
		int iRe;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 ||ArchiveFormat>1100) {
			m_FoilName ="";
			return;
		}
		ar >> m_FoilName ;
		ar >> m_PlrName;
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

		ar >> m_Color >> m_Style >> m_Width;

		if(ArchiveFormat>=1002){
			ar >> l;
			if(l!=0 && l!=1 ) {
				m_FoilName ="";
				return;
			}
			if (l) m_bIsVisible =true; else m_bIsVisible = false;
		}

		ar >> l;
		if(l!=0 && l!=1 ) {
			m_FoilName ="";
			return;
		}
		if (l) m_bShowPoints =true; else m_bShowPoints = false;
			
		bool bExists;
		for (int i=0; i< n; i++){
			ar >> Alpha >> Cd >> Cdp >> Cl >> Cm;
			ar >> XTr1 >> XTr2;
			ar >> HMom >> Cpmn;

			if(ArchiveFormat >=4) ar >> Re;
			else                  Re = (float)m_Reynolds;

			if(ArchiveFormat>=1004) ar>> XCp;
			else                    XCp = 0.0;

			bExists = false;
			if(m_Type!=4){
				for (int j=0; j<m_Alpha.GetSize(); j++){
					if(fabs(Alpha-m_Alpha[j])<0.001){
						bExists = true;
						break;
					}
				}
			}
			else{
				for (int j=0; j<m_Re.GetSize(); j++){
					if(fabs(Re-m_Re[j])<0.1){
						bExists = true;
						break;
					}
				}
			}
			if(!bExists){
				AddPoint(Alpha, Cd, Cdp, Cl, Cm, XTr1, XTr2, HMom, Cpmn, Re, XCp);
			}
		}
		if(ArchiveFormat>=1003)
			ar >> m_ACrit >> m_XTop >> m_XBot;
	}
}

void CPolar::Remove(int i)
{
	m_Alpha.RemoveAt(i);
	m_Cl.RemoveAt(i);
	m_Cd.RemoveAt(i);
	m_Cdp.RemoveAt(i);
	m_Cm.RemoveAt(i);
	m_XTr1.RemoveAt(i);
	m_XTr2.RemoveAt(i);
	m_HMom.RemoveAt(i);
	m_Cpmn.RemoveAt(i);
	m_ClCd.RemoveAt(i);
	m_RtCl.RemoveAt(i);
	m_Cl32Cd.RemoveAt(i);
	m_Re.RemoveAt(i);
	m_XCp.RemoveAt(i);
}

void CPolar::GetAlphaLimits(double &amin, double &amax)
{
	if(!m_Alpha.GetSize()){
		amin = 0.0;
		amax = 0.0;
	}
	else {
		amin = m_Alpha.GetAt(0);
		amax = m_Alpha.GetAt(m_Alpha.GetUpperBound());
	}
}

void CPolar::GetClLimits(double &Clmin, double &Clmax)
{
	if(!m_Cl.GetSize()){
		Clmin = 0.0;
		Clmax = 0.0;
	}
	else {
		Clmin = 10000.0;
		Clmax =-10000.0;
		double Cl;
		for (int i=0;i<m_Cl.GetSize(); i++){
			Cl = m_Cl.GetAt(i);
			if(Clmin>Cl) Clmin = Cl;
			if(Clmax<Cl) Clmax = Cl;
		}
	}
}

double CPolar::GetCm0()
{
	double Clmin =  1000.0;
	double Clmax = -1000.0;
	for (int i=0; i<m_Cl.GetSize(); i++){
		Clmin = __min(Clmin, m_Cl[i]);
		Clmax = __max(Clmax, m_Cl[i]);
	}
	if(!(Clmin<0.0) || !(Clmax>0.0)) return 0.0;
	int k=0;
	double rr  = m_Cl[k];
	double rr1 = m_Cl[k+1];

	while (m_Cl[k+1]<0.0) {
		rr  = m_Cl[k];
		rr1 = m_Cl[k+1];
		k++;
	}
	if(k+1>=m_Cm.GetSize()) return 0.0;
	double Cm0 = m_Cm[k] + (m_Cm[k+1]-m_Cm[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Cm0;

}

double CPolar::GetZeroLiftAngle()
{
	double Clmin =  1000.0;
	double Clmax = -1000.0;
	for (int i=0; i<m_Cl.GetSize(); i++){
		Clmin = __min(Clmin, m_Cl[i]);
		Clmax = __max(Clmax, m_Cl[i]);
	}
	if(!(Clmin<0.0) || !(Clmax>0.0)) return 0.0;
	int k=0;
	double rr  = m_Cl[k];
	double rr1 = m_Cl[k+1];

	while (m_Cl[k+1]<0.0) {
		rr  = m_Cl[k];
		rr1 = m_Cl[k+1];
		k++;
	}
	if(k+1>=m_Alpha.GetSize()) return 0.0;
	double Alpha0 = m_Alpha[k] + (m_Alpha[k+1]-m_Alpha[k])*(0.0-m_Cl[k])/(m_Cl[k+1]-m_Cl[k]);
	return Alpha0;

}

void CPolar::GetLinearizedCl(double &Alpha0, double &slope)
{
	// linearize Cl vs. Alpha set of points by least square method
	int n = (int)m_Cl.GetSize();

	if(n<=1) {
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

	slope  = b1; //in cl/°
	Alpha0 = -b2/b1;

}

void CPolar::GetBWStyle(COLORREF &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
	GetBWColor(color, style, width);
}
