/****************************************************************************

    CBody Class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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

// Body.cpp
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "Body.h"
#include <math.h>

double CBody::s_xKnots[MAXBODYFRAMES];
double CBody::s_hKnots[30];
CRect CBody::s_rViewRect;
CWnd *CBody::s_pMainFrame;

	// CBody

CBody::CBody()
{
	m_BodyName = "Body";

	m_BodyColor = RGB(255,200,200);
	m_BodyStyle = PS_SOLID;
	m_BodyWidth = 1;

	m_FramePosition[0].Set( -0.1,  0.0,  0.0);
	m_FramePosition[1].Set( -0.07, 0.0,  0.015);
	m_FramePosition[2].Set(  0.1,  0.0,  0.01);
	m_FramePosition[3].Set(  0.6,  0.0,  0.0);

	m_bLocked        = false;
	m_bClosedSurface = false;

	m_NStation   = 4;
	m_NSideLines = 5;

	m_iActiveFrame =  1;
	m_iHighlight   = -1;
	m_LineType     =  1;

	m_nxDegree = 3;
	m_nhDegree = 3;
	m_nxKnots = 0;
	m_nhKnots = 0;

	m_nxPanels = 20;
	m_nhPanels = 10;

	m_pPanel = NULL;
	m_NElements = m_nxPanels * m_nhPanels * 2;

	m_iRes = 30;

	eps = 1.0e-10;

	m_Frame[0].m_Point[0].Set(0.,0.0,0.0);
	m_Frame[0].m_Point[1].Set(0.,0.0,0.0);
	m_Frame[0].m_Point[2].Set(0.,0.0,0.0);
	m_Frame[0].m_Point[3].Set(0.,0.0,0.0);
	m_Frame[0].m_Point[4].Set(0.,0.0,0.0);

	m_Frame[m_NStation-1].m_Point[0].Set(0.,0.0,0.0);
	m_Frame[m_NStation-1].m_Point[1].Set(0.,0.0,0.0);
	m_Frame[m_NStation-1].m_Point[2].Set(0.,0.0,0.0);
	m_Frame[m_NStation-1].m_Point[3].Set(0.,0.0,0.0);
	m_Frame[m_NStation-1].m_Point[4].Set(0.,0.0,0.0);

	SetKnots();
}

CBody::~CBody()
{
}

CFrame * CBody::GetFrame(int iSelect)
{
	if(iSelect>=0 && iSelect<m_NStation)
	{
		return m_Frame + iSelect;
	}
	return NULL;
}

void CBody::Scale(double XFactor, double YFactor, double ZFactor)
{
	int i,j;
	for (i=0; i<m_NStation; i++)
	{
		m_FramePosition[i].x *= XFactor;
		m_FramePosition[i].y *= YFactor;
		m_FramePosition[i].z *= ZFactor;

		for(j=0; j<m_Frame[i].m_NPoints; j++)
		{
			m_Frame[i].m_Point[j].y *= YFactor;
			m_Frame[i].m_Point[j].z *= ZFactor;
		}
	}
	ComputeCenterLine();
}

double CBody::GetLength()
{
	return abs(m_FramePosition[m_NStation-1].x - m_FramePosition[0].x);
}


bool CBody::SerializeBody(CArchive &ar)
{
	int ArchiveFormat;
	int k;
	float f;

	if(ar.IsStoring())
	{
		ar << 1000;

		//1000 : first format
		ar << m_BodyName;
		ar << m_BodyColor;
		ar << m_LineType;
		ar << m_NSideLines;
		ar << m_NStation;
		ar << m_iRes;
		ar << m_nxDegree << m_nhDegree;
		ar << m_nxPanels << m_nhPanels;

		if(m_bClosedSurface) ar<<1; else ar <<0;

		for(k=0; k<m_NStation; k++)
		{
			m_Frame[k].SerializeFrame(ar);
		}
		ar << m_NStation;
		for (k=0; k<m_NStation;k++){
			ar << (float)m_FramePosition[k].x << (float)m_FramePosition[k].z;
		}

		ar << 0.0f << 0.0f;
	}
	else {
		try{			
			ar >> ArchiveFormat;
			if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
			ar >> m_BodyName;
			ar >> m_BodyColor;
			ar >> m_LineType;
			ar >> m_NSideLines;
			ar >> m_NStation;

			ar >> m_iRes;
			ar >> m_nxDegree >> m_nhDegree;
			ar >> m_nxPanels >> m_nhPanels;

			ar >> k;
			if(k!=0 && k!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if (k) m_bClosedSurface = true; else m_bClosedSurface = false;
			
			for(k=0; k<m_NStation; k++)
			{
				m_Frame[k].SerializeFrame(ar);
			}
			//Serialize Bodyline
			ar >> m_NStation;
			for (k=0; k<m_NStation;k++)
			{
				ar >> f; m_FramePosition[k].x =f;
				ar >> f; m_FramePosition[k].z =f;
			}
			ar >> f; 
			ar >> f; 

			SetKnots();

		}
		catch (CArchiveException *ex)
		{
			ex->Delete();
			return false;
		}
	}
	return true;
}

void CBody::ComputeCenterLine()
{
	int i;
	for(i=0; i<m_NStation; i++)
	{
		m_FramePosition[i].z=(m_Frame[i].m_Point[0].z + m_Frame[i].m_Point[m_NSideLines-1].z)/2.0;
	}
}

void CBody::UpdateFramePos(int iFrame)
{
	int i;
	double z;
	z  = m_FramePosition[iFrame].z;
	z -= (m_Frame[iFrame].m_Point[0].z + m_Frame[iFrame].m_Point[m_NSideLines-1].z)/2.0;
	
	for(i=0; i<m_NSideLines; i++)
	{
		m_Frame[iFrame].m_Point[i].z += z;
	}
}

void CBody::InsertSideLine(int SideLine)
{
	if(SideLine==0) SideLine++;
	for (int i=0; i<m_NStation; i++)
	{
		m_Frame[i].InsertPoint(SideLine);
	}
	m_NSideLines++;
	SetKnots();
}

void CBody::RemoveSideLine(int SideLine)
{
	for (int i=0; i<m_NStation; i++)
	{
		m_Frame[i].RemovePoint(SideLine);
	}
	m_NSideLines--;
	SetKnots();
}

void CBody::InsertPoint(CVector Real)
{
	//Real is to be inserted in the current frame
	if(m_iActiveFrame<0)
	{
		AfxMessageBox("Please select a Frame before inserting a point", MB_OKCANCEL);
		return;
	}
	int i, n;
	n = (m_Frame+m_iActiveFrame)->InsertPoint(Real);
	for (i=0; i<m_NStation; i++)
	{
		if(i!=m_iActiveFrame)
		{
			(m_Frame+i)->InsertPoint(n);
		}
	}
	SetKnots();
}


CFrame * CBody::InsertFrame(CVector Real)
{
	int k, n;

	bool bFound = false;
	for (n=0; n<m_NStation-1; n++)
	{
		if(m_FramePosition[n].x<=Real.x  && Real.x <m_FramePosition[n+1].x)
		{
			bFound = true;
			break;
		}
	}
	if (bFound)
	{
		for(k=m_NStation; k>n; k--)	
		{
            m_Frame[k].CopyFrame(m_Frame+k-1);
			m_FramePosition[k] = m_FramePosition[k-1];
		}
	}
	m_FramePosition[n+1] = Real;

	double trans = Real.z - (m_Frame[n+1].m_Point[0].z + m_Frame[n+1].m_Point[m_NSideLines-1].z)/2.0;
		           
	for (k=0; k<m_NSideLines; k++)
	{
		m_Frame[n+1].m_Point[k].z += trans;
	}


	m_iActiveFrame = n+1;
	m_NStation++;
	if(n>=m_NStation)	m_iActiveFrame = m_NStation;
	if(n<=0)			m_iActiveFrame = 0;
	m_iHighlight = -1;
	SetKnots();

	return m_Frame+n+1;
}


int CBody::IsFramePos(CVector Real, double ZoomFactor)
{ 
	int k;
	for (k=0; k<m_NStation; k++)
	{
		if (abs(Real.x-m_FramePosition[k].x)<0.006/ZoomFactor &&
			abs(Real.y-m_FramePosition[k].z)<0.006/ZoomFactor) return k;
	}
	return -10;
}


void CBody::Duplicate(CBody *pBody)
{
	m_NSideLines     = pBody->m_NSideLines;
	m_NStation       = pBody->m_NStation;
	m_nxDegree       = pBody->m_nxDegree;
	m_nhDegree       = pBody->m_nhDegree;
	m_nxPanels       = pBody->m_nxPanels;
	m_nhPanels       = pBody->m_nhPanels;
	m_LineType       = pBody->m_LineType;
	m_bClosedSurface = pBody->m_bClosedSurface;

	memcpy(m_Frame, pBody->m_Frame, sizeof(m_Frame));
	memcpy(m_FramePosition, pBody->m_FramePosition, sizeof(m_FramePosition));

	SetKnots();
}


int CBody::RemoveFrame(int n)
{
	int k;
//	m_BodyLine.RemovePoint(n);
	for(k=n; k<m_NStation; k++)	
	{
		m_FramePosition[k] = m_FramePosition[k+1];
		m_Frame[k].CopyFrame(m_Frame+k+1);
	}
	m_NStation--;
	m_iActiveFrame = n;
	if(n>=m_NStation)	m_iActiveFrame = m_NStation;
	if(n<=0)			m_iActiveFrame = 0;
	m_iHighlight = -1;
	SetKnots();
	return m_iActiveFrame;
}


void CBody::RemoveActiveFrame()
{
	int k;

	for(k=m_iActiveFrame; k<m_NStation; k++)	
	{
		m_FramePosition[k] = m_FramePosition[k+1];
		m_Frame[k].CopyFrame(m_Frame+k+1);
	}
	m_NStation--;

	m_iHighlight = -1;
	SetKnots();
}


void CBody::SetKnots()
{
	int j;
	double a,b;
	m_nxDegree = min(m_nxDegree, m_NStation);
	m_nxKnots  = m_nxDegree + m_NStation   + 1;

	// x-dir knots
	for (j=0; j<m_nxKnots; j++) 
	{
		if (j<m_nxDegree+1)  s_xKnots[j] = 0.0;
		else 
		{
			if (j<m_NStation) 
			{
				a = (double)(j-m_nxDegree);
				b = (double)(m_nxKnots-2*m_nxDegree-1);
				if(abs(b)>0.0) s_xKnots[j] = (GLfloat)(a/b);
				else           s_xKnots[j] = 1.0f; // added arcds
			}
			else s_xKnots[j] = 1.0f;	
		}
	}

	//hoop knots
	
	m_nhDegree = min(m_nhDegree, m_NSideLines);
	m_nhKnots  = m_nhDegree + m_NSideLines + 1;
	for (j=0; j<m_nhKnots; j++) 
	{
		if (j<m_nhDegree+1)  s_hKnots[j] = 0.0;
		else 
		{
			if (j<m_NSideLines) 
			{
				a = (double)(j-m_nhDegree);
				b = (double)(m_nhKnots-2*m_nhDegree-1);
				if(abs(b)>0.0) s_hKnots[j] = (GLfloat)(a/b);
				else           s_hKnots[j] = 1.0f; // added arcds
			}
			else s_hKnots[j] = 1.0f;	
		}
	}
}

double CBody::SplineBlend(int index,  int p, double t, double *knots)
{
//	Calculate the blending value, this is done recursively.
//	If the numerator and denominator are 0 the expression is 0.
//	If the denominator is 0 the expression is 0
//
//	   i   is the control point's index
//	   p   is the spline's degree 	
//	   t   is the spline parameter
//

	if (p == 0) 
	{
		if ((knots[index] <= t) && (t < knots[index+1]) )	value = 1.0;
//		else if (abs(knots[index]-knots[index+1])<pres)	    value = 0.0;
		else 						                value = 0.0;
	} 
	else
	{
		if (abs(knots[index+p] - knots[index])<eps && abs(knots[index+p+1] - knots[index+1])<eps) 
			value = 0.0;
		else if (abs(knots[index+p] - knots[index])<eps)
			value = (knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1, p-1, t, knots);
		else if (abs(knots[index+p+1] - knots[index+1])<eps)
			value = (t-knots[index])     / (knots[index+p] - knots[index])    * SplineBlend(index,   p-1, t, knots);
		else 
			value = (t-knots[index])     / (knots[index+p]  -knots[index])    * SplineBlend(index,   p-1, t, knots) + 
			        (knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1 ,p-1, t, knots);
	}
	return value;
}

CVector CBody::GetPoint(double u, double v)
{
	//returns the point corresponding to the parmatric values u and v
	//assumes that the knots have been set previously
	CVector V, Vh;
	int i,j;

	if(u>=1.0) u=0.99999999999;
	if(v>=1.0) v=0.99999999999;
	for(i=0; i<m_NStation; i++)
	{
		Vh.Set(0.0,0.0,0.0);
		for(j=0; j<m_NSideLines; j++)
		{
			cs =  SplineBlend(j, m_nhDegree, v, s_hKnots);
			Vh.x += m_FramePosition[i].x    * cs;
			Vh.y += m_Frame[i].m_Point[j].y * cs;
			Vh.z += m_Frame[i].m_Point[j].z * cs;
		}
		bs = SplineBlend(i, m_nxDegree, u, s_xKnots);
		V.x += Vh.x * bs;
		V.y += Vh.y * bs;
		V.z += Vh.z * bs;
	}
	return V;
}


double CBody::Getu(double x)
{
	if(x<=m_FramePosition[0].x)            return 0.0;
	if(x>=m_FramePosition[m_NStation-1].x) return 1.0;
	if(abs(m_FramePosition[m_NStation-1].x-m_FramePosition[0].x)<0.0000001) return 0.0;

	int i,j;
	double eps, u2, u1, b, c, u, v, xx, xh;
	eps = 0.00001;
	u1 = 0.0; u2 = 1.0;

	v = 0.0;//use top line, but doesn't matter 
	while(abs(u2-u1)>eps)
	{
		u=(u1+u2)/2.0;
		xx = 0.0;
		for(i=0; i<m_NStation; i++) //browse all points
		{
			xh = 0.0;
			for(j=0; j<m_NSideLines; j++)
			{
				c =  SplineBlend(j, m_nhDegree, v, s_hKnots);
				xh += m_FramePosition[i].x * c;
			}
			b = SplineBlend(i, m_nxDegree, u, s_xKnots);
			xx += xh * b;
		}
		if(xx>x) u2 = u;
		else     u1 = u;
	}

	return (u1+u2)/2.0;
}


void CBody::Export()
{
	CStdioFile XFile;
	CFileException fe;
	CString Header, strong, FileName, OutString, LengthUnit;
	int j,k;

	CMainFrame *pMainFrame = (CMainFrame*)s_pMainFrame;
	GetLengthUnit(LengthUnit, pMainFrame->m_LengthUnit);

	FileName = m_BodyName;
	FileName.Replace("/", " ");
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT,_T("Text Format (.txt)|*.txt"));

	if(IDOK==XFileDlg.DoModal()) 
	{
		BOOL bOpen = XFile.Open(XFileDlg.GetFileName(), CFile::modeCreate | CFile::modeWrite, &fe);
		if (bOpen)
		{
			XFile.WriteString(m_BodyName);
			XFile.WriteString("\n\n");
	
			strong.Format("%3d", m_LineType);
			strong +="       // Surface type :   1=Lines   2=Splines\n";
			XFile.WriteString(strong);

			strong.Format("%3d", m_NStation);
			strong +="       // Number of frame stations\n";
			XFile.WriteString(strong);

			strong.Format("%3d", m_NSideLines);
			strong +="       // Number of sidelines\n";
			XFile.WriteString(strong);

			strong.Format("%3d", m_nxDegree);
			strong +="       // Spline degree - axial direction\n";
			XFile.WriteString(strong);

			strong.Format("%3d", m_nhDegree);
			strong +="       // Spline degree - hoop direction\n";
			XFile.WriteString(strong);
			XFile.WriteString("\n\n");

			XFile.WriteString("        x("+LengthUnit+")          y("+LengthUnit+")          z("+LengthUnit+")\n");
			for (j=0; j<m_NStation; j++)
			{
				strong.Format("Frame %3d\n", j+1);
				XFile.WriteString(strong);
				for (k=0; k<m_NSideLines; k++)
				{
					strong.Format("   %10.3f     %10.3f     %10.3f\n", 
						m_FramePosition[j].x * pMainFrame->m_mtoUnit,
						m_Frame[j].m_Point[k].y * pMainFrame->m_mtoUnit,
						m_Frame[j].m_Point[k].z * pMainFrame->m_mtoUnit);     
					XFile.WriteString(strong);
				}
				XFile.WriteString("\n\n");
			}

			XFile.WriteString("\n\n");
		}
	}
}


bool CBody::SetModified()
{
	if(m_bLocked)
	{
		//unlock...
		//check if it is used by one or more planes
		int k; 
		CString strong;
		CMainFrame *pMainFrame = (CMainFrame*)s_pMainFrame;
		CPlane *pPlane;
		bool bIsInUse = false;
		int resp = IDYES;
		for(k=0; k<pMainFrame->m_oaPlane.GetSize(); k++)
		{
			pPlane = (CPlane*)pMainFrame->m_oaPlane.GetAt(k);
			if(pPlane->m_bBody && pPlane->m_pBody==this)
			{
				bIsInUse = true;
				break;
			}
		}
		if(bIsInUse)
		{
			strong = "The current body "+m_BodyName+" is used by one or more planes.\n The associated results will be deleted. Continue ?";
			resp = AfxMessageBox(strong, MB_YESNOCANCEL);
		}
		if(resp==IDYES)
		{
			for(k=0; k<pMainFrame->m_oaPlane.GetSize(); k++)
			{
				pPlane = (CPlane*)pMainFrame->m_oaPlane.GetAt(k);
				if(pPlane->m_bBody && pPlane->m_pBody==this)
				{
					pMainFrame->DeletePlane(pPlane, true);
				}
			}
			m_bLocked = false;
			return true;
		}
	}
	return false;
}






















