/****************************************************************************

    WPolar Class
    Copyright (C) 2005 André Deperrois XFLR5@yahoo.com

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


// WOpp.cpp: implementation of the CWOpp class.
// THe class for Wing Operating Points
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "WOpp.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWOpp::CWOpp()
{
	m_bOut         = false;
	m_bVLM1        = true;
	m_bThinSurface = true;
	m_bTiltedGeom  = false;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_WingType    = 0;

	m_Color = RGB(255,0,0);
	m_Style = PS_SOLID;
	m_Width = 1;

	m_nWakeNodes     = 0;
	m_NXWakePanels   = 0;
	m_FirstWakePanel = 0.0;
	m_WakeFactor     = 1.0;

	m_NStation     = 0;
	m_NVLMPanels   = 0;
	m_nFlaps       = 0;
	m_AnalysisType = 0;

	m_Alpha               = 0.0;
	m_Beta                = 0.0;
	m_Ctrl                = 0.0;
	m_CL                  = 0.0;
	m_CY                  = 0.0;
	m_CX                  = 0.0;
	m_ViscousDrag         = 0.0;
	m_InducedDrag         = 0.0;
	m_GCm                 = 0.0;
	m_GRm                 = 0.0;
	m_GYm                 = 0.0;
	m_VYm                 = 0.0;
	m_IYm                 = 0.0;
	m_XCP                 = 0.0;
	m_YCP                 = 0.0;
	m_Ctrl                = 0.0;

	memset(m_Ai,0,sizeof(m_Ai));
	memset(m_Twist,0,sizeof(m_Twist));
	memset(m_Cl,0,sizeof(m_Cl));
	memset(m_PCd,0,sizeof(m_PCd));
	memset(m_ICd,0,sizeof(m_ICd));
	memset(m_Cm,0,sizeof(m_Cm));
	memset(m_CmAirf,0,sizeof(m_CmAirf));
	memset(m_CmXRef,0,sizeof(m_CmXRef));
	memset(m_XCPSpanRel,0,sizeof(m_XCPSpanRel));
	memset(m_XCPSpanAbs,0,sizeof(m_XCPSpanAbs));
	memset(m_Chord,0,sizeof(m_Chord));
	memset(m_SpanPos,0,sizeof(m_SpanPos));
	memset(m_StripArea,0,sizeof(m_StripArea));
	memset(m_Re,0,sizeof(m_Re));
	memset(m_Twist,0,sizeof(m_Twist));
	memset(m_XTrTop,0,sizeof(m_XTrTop));
	memset(m_XTrBot,0,sizeof(m_XTrBot));
	memset(m_BendingMoment,0,sizeof(m_BendingMoment));
	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_Vd,0,sizeof(m_Vd));
	memset(m_F,0,sizeof(m_F));
	memset(m_G,0,sizeof(m_G));
	memset(m_Sigma,0,sizeof(m_Sigma));
	memset(m_FlapMoment,0,sizeof(m_FlapMoment));
}

CWOpp::~CWOpp()
{

}

bool CWOpp::SerializeWOpp(CArchive &ar)
{
	int ArchiveFormat;
	int a,p,k;
	float f, f1, f2;

	if(ar.IsStoring()){
		ar << 1015;
		//1015 : added M_CX and m_CY values
		//1014 : redefined moment coefficients
		//1013 : added m_bTiltedGeom
		//1012 : added m_StripArea
		//1011 : added m_Sigma
		//1010 : added flap moments, changed lengths to m
		//1009 : added Vortex strengths
		//1008 : added wake node number and pos
		//1007 : added m_XCPSpanAbs
		//1006 : added SurfaceForce
		//1005 : added downwash and VLM methods
		//1004 : added WingType
		//1003 : added Cl Distribution
		//1002 : added bending moment
		//write variables
		ar << m_WingName << m_PlrName;
		if(m_bIsVisible)   ar << 1; else ar<<0;
		if(m_bShowPoints)  ar << 1; else ar<<0;
		if(m_bOut)         ar << 1; else ar<<0;
		ar << m_AnalysisType;
		if(m_bVLM1)        ar << 1; else ar<<0;
		if(m_bThinSurface) ar << 1; else ar<<0;
		if(m_bTiltedGeom)  ar << 1; else ar<<0;

		ar << m_Style << m_Width << m_Color;
		ar << m_Type << m_NStation;
		ar << (float)m_Alpha << (float)m_QInf << (float)m_Weight << (float)m_Span << (float)m_MAChord;
		ar << (float)m_CL << (float)m_ViscousDrag << (float)m_InducedDrag ;
		ar << (float)m_Beta;
		ar << (float)m_CX << (float)m_CY;

		ar << (float)m_GCm << (float)m_GRm << (float)m_GYm;
		ar << 0.0f << (float)m_VYm;
		ar << (float)m_IYm;
	
		ar << (float)m_XCP << (float)m_YCP;

		for (k=0; k<m_NStation; k++)
		{
			ar << (float)m_Re[k] << (float)m_Chord[k] << (float)m_Twist[k];
			ar << (float)m_Ai[k] << (float)m_Cl[k] << (float)m_PCd[k] << (float)m_ICd[k];
			ar << (float)m_Cm[k] << (float)m_CmAirf[k] << (float)m_CmXRef[k];
			ar << (float)m_XCPSpanRel[k]<< (float)m_XCPSpanAbs[k];
			ar << (float)m_XTrTop[k] << (float)m_XTrBot[k];
			ar << (float)m_BendingMoment[k];
			ar << (float)m_Vd[k].x << (float)m_Vd[k].y << (float)m_Vd[k].z;
			ar << (float)m_F[k].x << (float)m_F[k].y << (float)m_F[k].z;
		}
		for (k=0; k<=m_NStation; k++) ar << (float)m_SpanPos[k] << (float)m_StripArea[k];
		ar << m_NVLMPanels;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_Cp[p] ;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_G[p] ;
		if(m_AnalysisType==3)
		{
			for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_Sigma[p] ;
		}

		ar << m_WingType;
		ar << m_nWakeNodes << m_NXWakePanels << m_FirstWakePanel << m_WakeFactor;
		ar << m_nFlaps;
		for(k=0; k<m_nFlaps; k++)
		{
			ar << (float)m_FlapMoment[k];
		}
	}
	else 
	{
		try
		{			
			ar >> ArchiveFormat;
			if(ArchiveFormat<1001|| ArchiveFormat>1100) return false;
			//read variables
			ar >> m_WingName >> m_PlrName;
			if(!m_WingName.GetLength() || !m_PlrName.GetLength()) return false;

			ar >> a;
			if (a!=0 && a!=1)
			{
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(a) m_bIsVisible = true; else m_bIsVisible = false;
			ar >> a;
			if (a!=0 && a!=1)
			{
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(a) m_bShowPoints = true; else m_bShowPoints = false;

			ar >> a;
			if (a!=0 && a!=1)
			{
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(a) m_bOut = true; else m_bOut = false;

			ar >> m_AnalysisType;
			if (a<=0 && a>=10)
			{
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(m_AnalysisType==0) m_AnalysisType=2;

			if(ArchiveFormat>=1005)
			{
				ar >> a;
				if (a!=0 && a!=1){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					throw pfe;
				}
				if(a) m_bVLM1 = true; else m_bVLM1 = false;

				ar >> a;
				if (a!=0 && a!=1){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					throw pfe;
				}
				if(a) m_bThinSurface = true; else m_bThinSurface = false;
			}
			if(ArchiveFormat>=1013)
			{
				ar >> a;
				if (a!=0 && a!=1){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					throw pfe;
				}
				if(a) m_bTiltedGeom = true; else m_bTiltedGeom = false;
			}

			ar >> m_Style >> m_Width >> m_Color;
			ar >> m_Type >> m_NStation;
			ar >> f; m_Alpha =f;
			ar >> f; m_QInf =f;
			ar >> f; m_Weight =f;
			ar >> f; m_Span =f;
			ar >> f; m_MAChord =f;
			ar >> f; m_CL =f;
			ar >> f; m_ViscousDrag =f;
			ar >> f; m_InducedDrag =f;
			if(ArchiveFormat>=1015)
			{
				ar >> f; m_Beta=f;
				ar >> f; m_CX =f;
				ar >> f; m_CY =f;
			}
			ar >> f; m_GCm =f;
			ar >> f; m_GRm =f;
			ar >> f; m_GYm =f;
			ar >> f; //m_VCm =f;
			ar >> f; m_VYm =f;
			ar >> f; m_IYm =f;

			if(ArchiveFormat<1014 && m_AnalysisType>1)
			{
				m_GCm = m_GRm = m_GYm = m_VYm = m_IYm = 0.0;
			}

			ar >> f; m_XCP =f;
			ar >> f; m_YCP =f;

			for (k=0; k<m_NStation; k++)
			{
				ar >> f; m_Re[k] =f;
				ar >> f; m_Chord[k] =f;
				ar >> f; m_Twist[k] =f;
				ar >> f; m_Ai[k] =f;
				ar >> f; m_Cl[k] =f;
				ar >> f; m_PCd[k] =f;
				ar >> f; m_ICd[k] =f;
				ar >> f; m_Cm[k] =f;
				ar >> f; m_CmAirf[k] =f;
				ar >> f; m_CmXRef[k] =f;
				ar >> f; m_XCPSpanRel[k] =f;
				if(ArchiveFormat>=1007){ar >> f; m_XCPSpanAbs[k] =f;}
				ar >> f; m_XTrTop[k] =f;
				ar >> f; m_XTrBot[k] =f;
				if(ArchiveFormat>=1002)	{ar >> f; m_BendingMoment[k]=f;}
				else m_BendingMoment[k] = 0.0;
				if(ArchiveFormat>=1005)	
				{
					ar >> f; m_Vd[k].x=f;
					ar >> f; m_Vd[k].y=f;
					ar >> f; m_Vd[k].z=f;
				}
				else
				{
					m_Vd[k].x = 0.0;
					m_Vd[k].y = 0.0;
					m_Vd[k].z = 0.0;
				}
				if(ArchiveFormat>=1006)	
				{
					ar >> f; m_F[k].x=f;
					ar >> f; m_F[k].y=f;
					ar >> f; m_F[k].z=f;
				}
				else {
					m_F[k].x = 0.0;
					m_F[k].y = 0.0;
					m_F[k].z = 0.0;
				}
			}
			m_MaxBending = 0.0;
			for (k=0; k<m_NStation; k++) 
			{
				m_MaxBending = max(m_MaxBending, m_BendingMoment[k]);
			}
			for (k=0; k<=m_NStation; k++) 
			{
				ar >> f1;
				if(m_AnalysisType==1  && ArchiveFormat<=1004) m_SpanPos[k] = -f1;
				else                                          m_SpanPos[k] =  f1;
				if(ArchiveFormat>=1012) 
				{
					ar >> f2; 
					m_StripArea[k] = f2;
				}
				else m_StripArea[k] = 0.0;

			}
			if(ArchiveFormat>=1003)
			{
				ar>> m_NVLMPanels;
				for (p=0; p<m_NVLMPanels;p++)
				{
					ar >> f; m_Cp[p] =f;

				}
			}
			if(ArchiveFormat>=1009)
			{
				for (p=0; p<m_NVLMPanels;p++)
				{
					ar >> f; m_G[p] =f;
					if(ArchiveFormat<1010) m_G[p] =  f/1000.0;
				}
			}
			if(ArchiveFormat>1010)
			{
				if(m_AnalysisType==3){	
					for (p=0; p<m_NVLMPanels;p++)
					{
						ar >> f; m_Sigma[p] = f;
					}
				}
			}
			if(ArchiveFormat>=1004)
			{
				ar>> m_WingType;
			}
			if(ArchiveFormat>=1008)
			{
				ar >> m_nWakeNodes >> m_NXWakePanels >> m_FirstWakePanel >> m_WakeFactor;
			}
			if(ArchiveFormat>=1010)
			{
				ar >> m_nFlaps;
				for(k=0; k<m_nFlaps; k++){
					ar >>f;  m_FlapMoment[k] = f;
				}
			}
			if(ArchiveFormat<1010)
			{
				for(k=0; k<m_NStation; k++){
					m_Chord[k]      /=1000.0;
					m_XCPSpanAbs[k] /=1000.0;
					m_SpanPos[k]    /=1000.0;
				}
				m_MAChord /=1000.0;
				m_Span    /=1000.0;
				m_XCP     /=1000.0;
				m_YCP     /=1000.0;
			}
		}
		catch (CArchiveException *ex)
		{
/*			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error importing OpPoint : ");
			str += szCause;
			AfxMessageBox(str);*/
			ex->Delete();
			return false;
		}
	}
	return true;
}


bool CWOpp::Export(	CStdioFile *pXFile, int FileType)
{
	CString Header, strong;
	int k;

	if(FileType==1) Header.Format("  y-span        Chord      Ai         Cl        PCd          ICd        CmGeom      CmAirf      XTrtop    XTrBot      XCP       BM\n");
	else            Header.Format("  y-span,Chord,Ai,Cl,PCd,ICd,CmGeom,CmAirf,XTrtop,XTrBot,XCP,BM\n");
	pXFile->WriteString(Header);

	int nStart;
	if(m_AnalysisType==1) nStart = 1;	
	else nStart = 0;
	for (k=nStart; k<m_NStation; k++)
	{
		if(FileType==1) strong.Format("%10.4f  %9.4f   %7.3f   %9.6f   %9.6f   %9.6f   %9.6f   %9.6f    %7.4f   %7.4f   %7.4f   %7.4f\n",
			m_SpanPos[k], m_Chord[k], m_Ai[k], m_Cl[k], m_PCd[k], m_ICd[k],	m_CmXRef[k],m_CmAirf[k],m_XTrTop[k],m_XTrBot[k], m_XCPSpanRel[k], m_BendingMoment[k]);
		else            strong.Format("%10.4f,%9.4f,%7.3f,%9.6f,%9.6f,%9.6f,%9.6f,%9.6f,%7.4f,%7.4f,%7.4f,%7.4f\n",
			m_SpanPos[k], m_Chord[k], m_Ai[k], m_Cl[k], m_PCd[k], m_ICd[k],	m_CmXRef[k],m_CmAirf[k],m_XTrTop[k],m_XTrBot[k], m_XCPSpanRel[k], m_BendingMoment[k]);

		pXFile->WriteString(strong);
	}
	pXFile->WriteString("\n\n");

	return true;
}

double CWOpp::GetMaxLift()
{
	int i,nStart;
	if(m_AnalysisType==1) nStart = 1;	
	else nStart = 0;
	double maxlift = 0.0;
	for (i=nStart; i<m_NStation; i++)
	{
		if(m_Cl[i] * m_Chord[i]/m_MAChord>maxlift)
		{
			maxlift = m_Cl[i] * m_Chord[i]/m_MAChord;
		}
	}
	return maxlift;
}


void CWOpp::GetBWStyle(COLORREF &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
	GetBWColor(color, style, width);
}


