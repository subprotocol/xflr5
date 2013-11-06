/****************************************************************************

	WOpp Class
	Copyright (C) 2005-2010 Andre Deperrois adeperrois@xflr5.com

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

#include "../mainframe.h"
#include "../miarex/Miarex.h"
#include "WingOpp.h"
#include "Plane.h"
#include "../globals.h"
#include <math.h>
#include <QTextStream>


void *WingOpp::s_pMainFrame;
void *WingOpp::s_pMiarex;

WingOpp::WingOpp()
{
	m_bOut         = false;
	m_bVLM1        = true;
	m_bThinSurface = true;
	m_bTiltedGeom  = false;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_WingType    = 0;

	m_Color = QColor(255,0,0);
	m_Style = 0;
	m_Width = 1;

	m_nWakeNodes     = 0;
	m_NXWakePanels   = 0;
	m_FirstWakePanel = 0.0;
	m_WakeFactor     = 1.0;

	m_NStation     = 0;
	m_NVLMPanels   = 0;
	m_nFlaps       = 0;
	m_nControls    = 0;
	m_AnalysisMethod = LLTMETHOD;

	m_Alpha               = 0.0;
	m_Beta                = 0.0;
	m_Phi                 = 0.0;
	m_Ctrl                = 0.0;
	m_CL                  = 0.0;
	m_CY                  = 0.0;
	m_CX                  = 0.0;
	m_VCD                 = 0.0;
	m_ICD                 = 0.0;
	m_GCm = m_VCm = m_ICm = m_GRm = m_GYm = m_VYm = m_IYm = 0.0;
	m_XCP                 = 0.0;
	m_YCP                 = 0.0;
	m_XNP	              = 0.0;

	CXu = CZu = Cmu = 0.0;
	CXa = CLa = Cma = CXq = CLq = Cmq = CYb = CYp = CYr = Clb = Clp = Clr = Cnb = Cnp = Cnr = 0.0;

	memset(m_ALong, 0, 16*sizeof(double));	
	memset(m_ALat, 0, 16*sizeof(double));	
	memset(m_BLat, 0, 4*sizeof(double));
	memset(m_BLong, 0, 4*sizeof(double));

	CXe = CYe = CZe = CLe = CMe = CNe = 0;

	memset(m_EigenValue, 0, sizeof(m_EigenValue)); //four longitudinal and four lateral modes
	memset(m_EigenVector, 0, sizeof(m_EigenVector));


	memset(m_Ai,0,sizeof(m_Ai));
	memset(m_Twist,0,sizeof(m_Twist));
	memset(m_Cl,0,sizeof(m_Cl));
	memset(m_PCd,0,sizeof(m_PCd));
	memset(m_ICd,0,sizeof(m_ICd));
	memset(m_Cm,0,sizeof(m_Cm));
	memset(m_CmAirf,0,sizeof(m_CmAirf));
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


bool WingOpp::Export(QTextStream &out, enumTextFileType FileType)
{
	QString Header, strong, Format;
	int k;

	if(FileType==TXT) Header = "  y-span        Chord      Ai         Cl        PCd          ICd        CmGeom      CmAirf      XTrtop    XTrBot      XCP       BM\n";
	else              Header = "  y-span,Chord,Ai,Cl,PCd,ICd,CmGeom,CmAirf,XTrtop,XTrBot,XCP,BM\n";
	out << Header;

	int nStart;
	if(m_AnalysisMethod==LLTMETHOD) nStart = 1;
	else                            nStart = 0;

	if(FileType==TXT) Format = "%1  %2   %3   %4   %5   %6   %7   %8    %9   %10   %11   %12\n";
	else              Format = "%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12\n";
	for (k=nStart; k<m_NStation; k++)
	{
		strong = QString(Format)
			.arg(m_SpanPos[k],10,'f',4)
			.arg(m_Chord[k],9,'f',4)
			.arg(m_Ai[k],7,'f',3)
			.arg(m_Cl[k],9,'f',6)
			.arg(m_PCd[k],9,'f',6)
			.arg(m_ICd[k],9,'f',6)
			.arg(m_Cm[k],9,'f',6)
			.arg(m_CmAirf[k],9,'f',6)
			.arg(m_XTrTop[k],7,'f',4)
			.arg(m_XTrBot[k],7,'f',4)
			.arg(m_XCPSpanRel[k],7,'f',4)
			.arg(m_BendingMoment[k],7,'f',4);
		out << strong;
	}
	out << "\n\n";

	return true;
}



double WingOpp::GetMaxLift()
{
	int i,nStart;
	if(m_AnalysisMethod==LLTMETHOD) nStart = 1;
	else                            nStart = 0;

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


void WingOpp::GetBWStyle(QColor &color, int &style, int &width)
{
	color = m_Color;
	style = m_Style;
	width = m_Width;
}



bool WingOpp::SerializeWOpp(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int a,p,k,l,n;
	float f, f0, f1, f2, f3;

	if(bIsStoring)
	{
		ar << 1022;
		//1022 : reduced the number of controls to 1
        //1021 : added ZCP
        //1020 : added CXa/CXu/CZu/Cmu + changed the provisions to zero
		//1019 : added Neutral point position + Provision for more int and float saves
		//1018 : added non dimensional stability control derivatives
		//1017 : added non dimensional stability derivatives
		//1016 : added eigenthings
		//1015 : added m_CX and m_CY values
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
		WriteCString(ar, m_WingName);
		WriteCString(ar, m_PlrName);

		if(m_bIsVisible)   ar << 1; else ar<<0;
		if(m_bShowPoints)  ar << 1; else ar<<0;
		if(m_bOut)         ar << 1; else ar<<0;

		if(m_AnalysisMethod==LLTMETHOD)        ar<<1;
		else if(m_AnalysisMethod==VLMMETHOD)   ar<<2;
		else if(m_AnalysisMethod==PANELMETHOD) ar<<3;



		if(m_bVLM1)        ar << 1; else ar<<0;
		if(m_bThinSurface) ar << 1; else ar<<0;
		if(m_bTiltedGeom)  ar << 1; else ar<<0;

		ar << m_Style << m_Width;
		WriteCOLORREF(ar,m_Color);

		if(m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
		else if(m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
		else if(m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
		else if(m_WPolarType==STABILITYPOLAR)  ar<<7;
		else ar << 0;

		ar << m_NStation;
		ar << (float)m_Alpha << (float)m_QInf << (float)m_Weight << (float)m_Span << (float)m_MAChord;
		ar << (float)m_CL << (float)m_VCD << (float)m_ICD ;
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
			ar << (float)m_Cm[k] << (float)m_CmAirf[k] << 0.0f;
            ar << (float)m_XCPSpanRel[k]<< (float)m_XCPSpanAbs[k];
			ar << (float)m_XTrTop[k] << (float)m_XTrBot[k];
			ar << (float)m_BendingMoment[k];
			ar << (float)m_Vd[k].x << (float)m_Vd[k].y << (float)m_Vd[k].z;
			ar << (float)m_F[k].x << (float)m_F[k].y << (float)m_F[k].z;
		}
		for (k=0; k<=m_NStation; k++) ar << (float)m_SpanPos[k] << (float)m_StripArea[k];
		ar << m_NVLMPanels;
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_Cp[p];
		for (p=0; p<m_NVLMPanels;p++)	ar << (float)m_G[p];

		if(m_AnalysisMethod==PANELMETHOD)
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

		for(k=0; k<8;k++)
		{
			ar << (float)(m_EigenValue[k].real()) << (float)(m_EigenValue[k].imag());
			for(l=0; l<4; l++)
			{
				ar << (float)(m_EigenVector[k][l].real()) << (float)(m_EigenVector[k][l].imag());
			}
		}


		ar<<(float)CXa <<(float)CXq <<(float)CXu <<(float)CZu <<(float)Cmu;

		ar << (float)CLa << (float)CLq << (float)Cma << (float)Cmq;
		ar << (float)CYb << (float)CYp << (float)CYr << (float)Clb << (float)Clp << (float)Clr << (float)Cnb << (float)Cnp << (float)Cnr;
		ar << m_nControls;
//			for(k=0; k<m_nControls; k++)
		{
			ar<<(float)CXe<<(float)CYe<<(float)CZe;
			ar<<(float)CLe<<(float)CMe<<(float)CNe;
			ar << (float)m_BLat[0] << (float)m_BLat[1] << (float)m_BLat[2] << (float)m_BLat[3];
			ar << (float)m_BLong[0]<< (float)m_BLong[1]<< (float)m_BLong[2]<< (float)m_BLong[3];
		}
		for(k=0; k<4; k++)
		{
			ar << (float)m_ALong[k][0]<< (float)m_ALong[k][1]<< (float)m_ALong[k][2]<< (float)m_ALong[k][3];
			ar << (float)m_ALat[k][0] << (float)m_ALat[k][1] << (float)m_ALat[k][2] << (float)m_ALat[k][3];
		}

		f = m_XNP; ar <<f;

		ar<<(float)m_Ctrl;
		//provision
		for(int i=1; i<20; i++) ar<<(float)0.0f;
		for(int i=0; i<20; i++) ar<<0;
		ar<<(float)m_ZCP;
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<1001|| ArchiveFormat>1100) return false;
		//read variables
		ReadCString(ar, m_WingName);
		ReadCString(ar, m_PlrName);

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bIsVisible = true; else m_bIsVisible = false;
		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bShowPoints = true; else m_bShowPoints = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bOut = true; else m_bOut = false;

		ar>>k;
		if(k==1)      m_AnalysisMethod=LLTMETHOD;
		else if(k==2) m_AnalysisMethod=VLMMETHOD;
		else if(k==3) m_AnalysisMethod=PANELMETHOD;
		else if(k==4) m_AnalysisMethod=VLMMETHOD;
		else return false;

		if(ArchiveFormat>=1005)
		{
			ar >> a;
			if (a!=0 && a!=1) return false;

			if(a) m_bVLM1 = true; else m_bVLM1 = false;

			ar >> a;
			if (a!=0 && a!=1) return false;
			if(a) m_bThinSurface = true; else m_bThinSurface = false;
		}
		if(ArchiveFormat>=1013)
		{
			ar >> a;
			if (a!=0 && a!=1) return false;
			if(a) m_bTiltedGeom = true; else m_bTiltedGeom = false;
		}

		ar >> m_Style >> m_Width;
		ReadCOLORREF(ar,m_Color);

		ar >>k;
		if(k==1)      m_WPolarType = FIXEDSPEEDPOLAR;
		else if(k==2) m_WPolarType = FIXEDLIFTPOLAR;
		else if(k==4) m_WPolarType = FIXEDAOAPOLAR;
		else if(k==6) m_WPolarType = STABILITYPOLAR; //former control polars
		else if(k==7) m_WPolarType = STABILITYPOLAR;
		else return false;

		ar >> m_NStation;
		ar >> f; m_Alpha =f;
		ar >> f; m_QInf =f;
		ar >> f; m_Weight =f;
		ar >> f; m_Span =f;
		ar >> f; m_MAChord =f;
		ar >> f; m_CL =f;
		ar >> f; m_VCD =f;
		ar >> f; m_ICD =f;
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

		if(ArchiveFormat<1014 && m_AnalysisMethod>LLTMETHOD)
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
			ar >> f; //f=0.0;
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
			m_MaxBending = qMax(m_MaxBending, m_BendingMoment[k]);
		}
		for (k=0; k<=m_NStation; k++)
		{
			ar >> f1;
			if(m_AnalysisMethod==LLTMETHOD  && ArchiveFormat<=1004) m_SpanPos[k] = -f1;
			else                                                    m_SpanPos[k] =  f1;
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
			if(m_AnalysisMethod==PANELMETHOD)
			{
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
			for(k=0; k<m_nFlaps; k++)
			{
				ar >>f;  m_FlapMoment[k] = f;
			}
		}

		if(ArchiveFormat<1010)
		{
			for(k=0; k<m_NStation; k++)
			{
				m_Chord[k]      /=1000.0;
				m_XCPSpanAbs[k] /=1000.0;
				m_SpanPos[k]    /=1000.0;
			}
			m_MAChord /=1000.0;
			m_Span    /=1000.0;
			m_XCP     /=1000.0;
			m_YCP     /=1000.0;
		}
		if(ArchiveFormat>=1016)
		{
			for(k=0; k<8;k++)
			{
				ar >> f1 >> f2;
				m_EigenValue[k] = complex<double>(f1, f2);

				for(l=0; l<4; l++)
				{
					ar >> f1 >> f2;
					m_EigenVector[k][l] = complex<double>(f1, f2);
				}
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

		if(ArchiveFormat>=1019)
		{
			ar>>f; m_Ctrl=f;

			//provision
			for(int i=1; i<20; i++)
			{
				ar>>f;
			}
			for(int i=0; i<20; i++)
			{
				ar>>k;
			}
		}
        if(ArchiveFormat>=1021)
        {
            ar >> f; m_ZCP=f;
        }
	}
	return true;
}




void WingOpp::GetWOppProperties(QString &WOppProperties, bool bData)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strange, lenunit, massunit, speedunit;
	GetLengthUnit(lenunit, pMainFrame->m_LengthUnit);
	GetWeightUnit(massunit, pMainFrame->m_WeightUnit);
	GetSpeedUnit(speedunit, pMainFrame->m_SpeedUnit);


	WOppProperties.clear();

	if(m_WPolarType==FIXEDSPEEDPOLAR)     strong += "Type 1 ("+QObject::tr("Fixed speed") +")\n";
	else if(m_WPolarType==FIXEDLIFTPOLAR) strong += "Type 2 ("+QObject::tr("Fixed lift") +")\n";
	else if(m_WPolarType==FIXEDAOAPOLAR)  strong += "Type 3 ("+QObject::tr("Fixed angle of attack") +")\n";
	else if(m_WPolarType==STABILITYPOLAR) strong += "Type 4 ("+QObject::tr("Stability analysis") +")\n";
	WOppProperties += strong;

//	WOppProperties += QObject::tr("Method")+" = ";
	if(m_AnalysisMethod==LLTMETHOD)                             WOppProperties +=QObject::tr("LLT");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bThinSurface)   WOppProperties +=QObject::tr("3D-Panels");
	else if(m_AnalysisMethod==PANELMETHOD && m_bVLM1)           WOppProperties +=QObject::tr("3D-Panels/VLM1");
	else if(m_AnalysisMethod==PANELMETHOD && !m_bVLM1)          WOppProperties +=QObject::tr("3D-Panels/VLM2");
	WOppProperties +="\n";


	if(m_bTiltedGeom) WOppProperties += QObject::tr("Tilted geometry")+"\n";

	if(m_bOut) WOppProperties += "Point is out of the flight envelope\n";

	strong  = QString(QObject::tr("VInf")+"  =%1 ").arg(m_QInf*pMainFrame->m_mstoUnit,7,'f',3);
	WOppProperties += "\n"+strong + speedunit+"\n";

	strong  = QString(QObject::tr("Alpha")+" =%1").arg(m_Alpha,7,'f',2);
	WOppProperties += strong +QString::fromUtf8("°")+"\n";

	strong  = QString(QObject::tr("Mass")+"  = %1 ").arg(m_Weight*pMainFrame->m_kgtoUnit,7,'f',3);
	WOppProperties += strong + massunit + "\n";

	if(fabs(m_Beta)>PRECISION)
	{
		strong  = QString(QObject::tr("Beta")+"  = %1").arg(m_Beta,7,'f',2);
		WOppProperties += strong +QString::fromUtf8("°")+"\n\n";
	}

	if(m_WPolarType==STABILITYPOLAR)
	{
		strong  = QString(QObject::tr("Control value")+" = %1").arg(m_Ctrl,5,'f',2);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("XNP")+" = %1 ").arg(m_XNP*pMainFrame->m_mtoUnit,7,'f',3);
		WOppProperties += "\n"+strong +lenunit+"\n";
	}
	strong  = QString(QObject::tr("XCP")+" = %1 ").arg(m_XCP*pMainFrame->m_mtoUnit,7,'f',3);
	WOppProperties += strong +lenunit+"\n";
	strong  = QString(QObject::tr("YCP")+" = %1 ").arg(m_YCP*pMainFrame->m_mtoUnit,7,'f',3);
    WOppProperties += strong +lenunit+"\n";
    strong  = QString(QObject::tr("ZCP")+" = %1 ").arg(m_ZCP*pMainFrame->m_mtoUnit,7,'f',3);
    WOppProperties += strong +lenunit+"\n\n";


	strong  = QString(QObject::tr("CL")+"  = %1").arg(m_CL,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CD")+"  = %1").arg(m_ICD+m_VCD,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCD")+" = %1").arg(m_VCD,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICD")+" = %1").arg(m_ICD,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CX")+"  = %1").arg(m_CX,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("CY")+"  = %1").arg(m_CY,9,'f',5);
	WOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cl")+"  = %1").arg(m_GRm,9,'f',5);
	WOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cm")+"  = %1").arg(m_GCm,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICm")+" = %1").arg(m_ICm,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCm")+" = %1").arg(m_VCm,9,'f',5);
	WOppProperties += strong +"\n";

	strong  = QString(QObject::tr("Cn")+"  = %1").arg(m_GYm,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("ICn")+" = %1").arg(m_IYm,9,'f',5);
	WOppProperties += strong +"\n";
	strong  = QString(QObject::tr("VCn")+" = %1").arg(m_VYm,9,'f',5);
	WOppProperties += strong +"\n";

	if(m_nFlaps>0)
	{
		WOppProperties += "\n";
		for(int ip=0; ip<m_nFlaps; ip++)
		{
			strong = QString("Flap Moment[%1] = %2").arg(ip+1).arg(m_FlapMoment[ip]*pMainFrame->m_NmtoUnit, 7,'g',3);
			GetMomentUnit(strange, pMainFrame->m_MomentUnit);
			WOppProperties += strong + strange +"\n";
		}
	}


	if(m_WPolarType==STABILITYPOLAR)
	{
		WOppProperties += "\n\n";
		WOppProperties += QObject::tr("Non-dimensional Stability Derivatives:")+"\n";
		strong  = QString(QObject::tr("CXu")+"  = %1").arg(CXu,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLu")+"  = %1").arg(-CZu,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cmu")+"  = %1").arg(Cmu,9,'f',5);
		WOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CXa")+"  = %1").arg(CXa,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLa")+"  = %1").arg(CLa,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cma")+"  = %1").arg(Cma,9,'f',5);
		WOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CXq")+"  = %1").arg(CXq,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("CLq")+"  = %1").arg(CLq,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cmq")+"  = %1").arg(Cmq,9,'f',5);
		WOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYb")+"  = %1").arg(CYb,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clb")+"  = %1").arg(Clb,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnb")+"  = %1").arg(Cnb,9,'f',5);
		WOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYp")+"  = %1").arg(CYp,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clp")+"  = %1").arg(Clp,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnp")+"  = %1").arg(Cnp,9,'f',5);
		WOppProperties += strong +"\n\n";
		strong  = QString(QObject::tr("CYr")+"  = %1").arg(CYr,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Clr")+"  = %1").arg(Clr,9,'f',5);
		WOppProperties += strong +"\n";
		strong  = QString(QObject::tr("Cnr")+"  = %1").arg(Cnr,9,'f',5);
		WOppProperties += strong +"\n\n";

		if(m_nControls>0)
		{
			// (only one)
			WOppProperties += QObject::tr("Non-dimensional Control Derivatives:")+"\n";
			strong  = QString(QObject::tr("CXd")+"  = %1").arg(CXe,9,'f',5);
			WOppProperties += strong +"\n";
			strong  = QString(QObject::tr("CYd")+"  = %1").arg(CYe,9,'f',5);
			WOppProperties += strong +"\n";
			strong  = QString(QObject::tr("CZd")+"  = %1").arg(CZe,9,'f',5);
			WOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cld")+"  = %1").arg(CLe,9,'f',5);
			WOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cmd")+"  = %1").arg(CMe,9,'f',5);
			WOppProperties += strong +"\n";
			strong  = QString(QObject::tr("Cnd")+"  = %1").arg(CNe,9,'f',5);
			WOppProperties += strong +"\n";
		}

		complex<double> c, angle;
		double OmegaN, Omega1, Dsi, Sigma1, sum, prod, u0, mac, span;
		u0   = m_QInf;
		mac  = m_MAChord;
		span = m_Span;


		WOppProperties += "\nLongitudinal modes:\n";
		for(int im=0; im<4; im++)
		{
			c = m_EigenValue[im];
			sum  = c.real() * 2.0;                         // is a real number
			prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
			OmegaN = fabs(c.imag());
			if(OmegaN>PRECISION)	Omega1 = sqrt(prod);
			else                    Omega1 = 0.0;
			Sigma1 = sum /2.0;
			if(Omega1>PRECISION) Dsi = -Sigma1/Omega1;
			else                 Dsi = 0.0;

			if(c.imag()>=0.0) strange = QString("  Eigenvalue    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("  Eigenvalue    = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			strange = QString("  Undamped Natural Frequency = %1 Hz").arg(OmegaN/2.0/PI, 8,'f',3);
			WOppProperties += strange +"\n";

			strange = QString("  Damped Natural Frequency   = %1 Hz").arg(Omega1/2.0/PI, 8,'f',3);
			WOppProperties += strange +"\n";

			strange = QString("  Damping Ratio              = %1 ").arg(Dsi, 8,'f',3);
			WOppProperties += strange +"\n";


			WOppProperties += "  Normalized Eigenvector:\n";
			angle = m_EigenVector[im][3];
			c = m_EigenVector[im][0]/u0;
			if(c.imag()>=0.0) strange = QString("    u/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    u/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][1]/u0;
			if(c.imag()>=0.0) strange = QString("    w/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    w/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][2]/(2.0*u0/mac);
			if(c.imag()>=0.0) strange = QString("    q/(2.u0.MAC)  = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    q/(2.u0.MAC)  = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][3]/angle;
			if(c.imag()>=0.0) strange = QString("    theta(rad)    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    theta(rad)    = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n\n";
		}

		WOppProperties += "\nLateral modes:\n";
		for(int im=4; im<8; im++)
		{
			c = m_EigenValue[im];
			sum  = c.real() * 2.0;                         // is a real number
			prod = c.real()*c.real() + c.imag()*c.imag();  // is a positive real number
			OmegaN = fabs(c.imag());
			if(OmegaN>PRECISION)	Omega1 = sqrt(prod);
			else                    Omega1 = 0.0;
			Sigma1 = sum /2.0;
			if(Omega1>PRECISION) Dsi = -Sigma1/Omega1;
			else                 Dsi = 0.0;

			if(c.imag()>=0.0) strange = QString("  Eigenvalue    = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("  Eigenvalue    = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			strange = QString("  Undamped Natural Frequency = %1 Hz").arg(OmegaN/2.0/PI, 8,'f',3);
			WOppProperties += strange +"\n";

			strange = QString("  Damped Natural Frequency   = %1 Hz").arg(Omega1/2.0/PI, 8,'f',3);
			WOppProperties += strange +"\n";

			strange = QString("  Damping Ratio              = %1 ").arg(Dsi, 8,'f',3);
			WOppProperties += strange +"\n";


			WOppProperties += "  Normalized Eigenvector:\n";

			angle = m_EigenVector[im][3];

			c = m_EigenVector[im][0]/u0;
			if(c.imag()>=0.0) strange = QString("    v/u0          = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    v/u0          = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][1]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("    p/(2.u0.Span) = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    p/(2.u0.Span) = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][2]/(2.0*u0/span);
			if(c.imag()>=0.0) strange = QString("    r/(2.u0.Span) = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    r/(2.u0.Span) = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n";

			c = m_EigenVector[im][3]/angle;
			if(c.imag()>=0.0) strange = QString("    phi(rad)      = %1+%2i").arg(c.real(),10,'f',5).arg(c.imag(),10,'f',5);
			else              strange = QString("    phi(rad)      = %1-%2i").arg(c.real(),10,'f',5).arg(fabs(c.imag()),10,'f',5);
			WOppProperties += strange +"\n\n";
		}

	}
}








