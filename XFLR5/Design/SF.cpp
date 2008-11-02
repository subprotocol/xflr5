/****************************************************************************

    Spline Foil Class
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


// SF.cpp: implementation of the CSF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "SF.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSF::CSF()
{
	m_FoilStyle = PS_SOLID;
	m_FoilWidth = 1;
	m_FoilColor   = RGB(50,180,130);
	m_OutPoints    = 0;
	m_bVisible     = true;
	m_bOutPoints   = false;
	m_bModified    = false;
	m_bCenterLine  = false;
}

CSF::~CSF(){

}

bool CSF::InitSplineFoil()
{
	m_bModified   = false;
	m_strFoilName = "Spline Foil";

	m_Extrados.m_iCtrlPoints = 0;
	m_Extrados.m_iRes = 30;
	m_Extrados.InsertPoint(0.f , 0.f);
	m_Extrados.InsertPoint(0.f , 0.00774066f);
	m_Extrados.InsertPoint(0.0306026f, 0.0343829f);
	m_Extrados.InsertPoint(0.289036f , 0.0504014f);
	m_Extrados.InsertPoint(0.576000f,  0.0350933f);
	m_Extrados.InsertPoint(0.736139f , 0.0269428f);
	m_Extrados.InsertPoint(1.f , 0.f);
	m_Extrados.SplineKnots();
	m_Extrados.SplineCurve();

	m_Intrados.m_iCtrlPoints = 0;
	m_Intrados.m_iRes = 30;
	m_Intrados.InsertPoint(0.f , 0.f);
	m_Intrados.InsertPoint(0.f , -0.00774066f);
	m_Intrados.InsertPoint(0.0306026f, -0.0343829f);
	m_Intrados.InsertPoint(0.289036f , -0.0504014f);
	m_Intrados.InsertPoint(0.576000f,  -0.0350933f);
	m_Intrados.InsertPoint(0.736139f , -0.0269428f);
	m_Intrados.InsertPoint(1.f , 0.f);
	m_Intrados.SplineKnots();
	m_Intrados.SplineCurve();

	CompMidLine();
	m_OutPoints = m_Extrados.m_iRes + m_Intrados.m_iRes;

	return true;
}

bool CSF::CompMidLine()
{
	double x, yex, yin;
	m_fThickness = 0.0;
	m_fCamber    = 0.0;
	m_fxCambMax  = 0.0;
	m_fxThickMax = 0.0;

	m_rpMid[0].x   = 0.0;
	m_rpMid[0].y   = 0.0;
	m_rpMid[100].x = 1.0;
	m_rpMid[100].y = 0.0;
	// use 0.01 step;
	double step = 0.001;
	for (int k=0; k<=1000; k++){
		x = k*step;
		yex = m_Extrados.GetY(x);
		yin = m_Intrados.GetY(x);
		m_rpMid[k].x = x;
		m_rpMid[k].y = (yex+yin)/2.0;
		if(abs(yex-yin)>m_fThickness){
			m_fThickness = abs(yex-yin);
			m_fxThickMax = x;
		}
		if(abs(m_rpMid[k].y)>abs(m_fCamber)){
			m_fCamber = m_rpMid[k].y;
			m_fxCambMax = x;
		}	
	}
	return true;
}



void CSF::DrawFoil(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting)
{
	m_Extrados.DrawSpline(pDC, scalex, scaley, Offset, IsPrinting);
	m_Intrados.DrawSpline(pDC, scalex, scaley, Offset, IsPrinting);
}


void CSF::DrawMidLine(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting)
{
	int k;
	if (IsPrinting){
		scaley=-scaley;
	}

	pDC->MoveTo((int)( m_rpMid[0].x*scalex)  +Offset.x,
				(int)(-m_rpMid[0].y*scaley)  +Offset.y);
	for (k=1; k<=100; k++)
	{
		pDC->LineTo((int)( m_rpMid[k*10].x*scalex)+Offset.x,
					(int)(-m_rpMid[k*10].y*scaley)+Offset.y);
	}
}


void CSF::UpdateKnots()
{
	m_Extrados.SplineKnots();
	m_Intrados.SplineKnots();
}

void CSF::Update(bool bExtrados)
{
	if(bExtrados)
	{
		m_Extrados.SplineCurve();
	}
	else
	{
		m_Intrados.SplineCurve();
	}
	CompMidLine();
	m_OutPoints = m_Extrados.m_iRes + m_Intrados.m_iRes;
}


bool CSF::Export(CStdioFile *pFile)
{
	m_Extrados.Export(pFile, true);
	m_Intrados.Export(pFile, false);
	return true;

}

void CSF::ExportToBuffer(CFoil *pFoil)
{
	int i;
	int j = m_Extrados.m_iRes;
	int k = m_Intrados.m_iRes;
	for (i=0; i<m_Extrados.m_iRes; i++) {
		pFoil->x[j-i-1] = m_Extrados.m_Output[i].x;
		pFoil->y[j-i-1] = m_Extrados.m_Output[i].y;
	}
	for (i=1; i<m_Intrados.m_iRes;i++) {
		pFoil->x[i+j-1] = m_Intrados.m_Output[i].x;
		pFoil->y[i+j-1] = m_Intrados.m_Output[i].y;
	}
	pFoil->n = j+k-1;
	memcpy(pFoil->xb, pFoil->x, sizeof(pFoil->x));
	memcpy(pFoil->yb, pFoil->y, sizeof(pFoil->y));
	pFoil->nb = pFoil->n;
	pFoil->m_FoilName = m_strFoilName;
}

void CSF::Copy(CSF* pSF)
{
	m_FoilColor = pSF->m_FoilColor;
	m_FoilStyle = pSF->m_FoilStyle;
	m_FoilWidth = pSF->m_FoilWidth;
	m_Extrados.Copy(&pSF->m_Extrados);
	m_Intrados.Copy(&pSF->m_Intrados);
	m_OutPoints  = pSF->m_OutPoints;
	m_fCamber    = pSF->m_fCamber;
	m_fThickness = pSF->m_fThickness;
	m_fxCambMax  = pSF->m_fxCambMax; 
	m_fxThickMax = pSF->m_fxThickMax;
}

bool CSF::DrawCtrlPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting)
{
	int i;
	for (i=0; i<m_Extrados.m_iCtrlPoints;i++)
	{
		m_Extrados.DrawControlPoint(pDC, i, scalex, scaley, Offset, IsPrinting);
	}
	for (i=0; i<m_Intrados.m_iCtrlPoints;i++)
	{
		m_Intrados.DrawControlPoint(pDC, i, scalex, scaley, Offset, IsPrinting);
	}
	return true;
}



void CSF::UpdateSelected(double x, double y)
{
	int i;
	for (i=0; i<m_Extrados.m_iCtrlPoints;i++)
	{
		if (m_Extrados.m_iSelect == i)
		{
			m_Extrados.m_Input[i].x = x;
			m_Extrados.m_Input[i].y = y;
			Update(true);
			return;
		}
	}
	for (i=0; i<m_Intrados.m_iCtrlPoints;i++)
	{
		if (m_Intrados.m_iSelect == i)
		{
			m_Intrados.m_Input[i].x = x;
			m_Intrados.m_Input[i].y = y;
			Update(false);
			return;
		}
	}
}

void CSF::DrawOutPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting)
{
	int i;
	for (i=0; i<m_Extrados.m_iRes;i++)
	{
		m_Extrados.DrawOutputPoint(pDC, i, scalex, scaley, Offset, IsPrinting);
	}
	for (i=0; i<m_Intrados.m_iRes;i++)
	{
		m_Intrados.DrawOutputPoint(pDC, i, scalex, scaley, Offset, IsPrinting);
	}
	return;
}



bool CSF::LoadFile(CStdioFile *pFile)
{
	CString strIn;
	int k;

	m_bModified = false;

	m_Extrados.m_iCtrlPoints = -1;
	m_Intrados.m_iCtrlPoints = -1;


	// read FoilColor
	pFile->ReadString(strIn);
	sscanf(strIn, "%d", &m_FoilColor);

	// number of points on extrados
	pFile->ReadString(strIn);
	sscanf(strIn, "%d", &m_Extrados.m_iCtrlPoints);
	// extrados spline degree
	pFile->ReadString(strIn);
	sscanf(strIn, "%d", &m_Extrados.m_iDegree);
	// then extrados coords
	for (k=0; k<m_Extrados.m_iCtrlPoints;k++)
	{
		pFile->ReadString(strIn);
		sscanf(strIn,"%lf%lf", &m_Extrados.m_Input[k].x, &m_Extrados.m_Input[k].y);
	}
	// number of points on intrados
	pFile->ReadString(strIn);
	sscanf(strIn, "%d", &m_Intrados.m_iCtrlPoints);
	// intrados spline degree
	pFile->ReadString(strIn);
	sscanf(strIn, "%d", &m_Intrados.m_iDegree);
	// then intrados coords
	for (k=0; k<m_Intrados.m_iCtrlPoints;k++)
	{
		pFile->ReadString(strIn);
		sscanf(strIn,"%lf%lf", &m_Intrados.m_Input[k].x, &m_Intrados.m_Input[k].y);
	}

	// extrados Output points
	pFile->ReadString(strIn);
	if(!strIn.IsEmpty()) sscanf(strIn, "%d", &m_Extrados.m_iRes);
	else m_Extrados.m_iRes = 66;
	// intrados Output points
	if(!strIn.IsEmpty()) sscanf(strIn, "%d", &m_Intrados.m_iRes);
	else m_Intrados.m_iRes = 90;

	m_Extrados.SplineKnots();
	m_Extrados.SplineCurve();
	m_Intrados.SplineKnots();
	m_Intrados.SplineCurve();

	CompMidLine();

	return true;
}


bool CSF::SaveFile(CStdioFile *pFile)
{
	CString strong;
	int k;

	// FoilName
	strong = m_strFoilName + "\n";
	pFile->WriteString(strong);

	//FoilColor
	strong.Format("%d\n", m_FoilColor);
	pFile->Write(strong, strong.GetLength());

	// number of points on extrados (minus 1, C++ convention ?)
	strong.Format("%d\n", m_Extrados.m_iCtrlPoints);
	pFile->Write(strong, strong.GetLength());
	// extrados spline degree
	strong.Format("%d\n", m_Extrados.m_iDegree);
	pFile->Write(strong, strong.GetLength());
	// then extrados coords
	for (k=0; k<m_Extrados.m_iCtrlPoints;k++)
	{
		strong.Format(" %f  %f\n", 
			m_Extrados.m_Input[k].x, m_Extrados.m_Input[k].y);
		pFile->Write(strong, strong.GetLength());
	}

	// number of points on intrados (minus 1, C++ convention ?)
	strong.Format("%d\n", m_Intrados.m_iCtrlPoints);
	pFile->Write(strong, strong.GetLength());
	// intrados spline degree
	strong.Format("%d\n", m_Intrados.m_iDegree);
	pFile->Write(strong, strong.GetLength());
	// then extrados coords
	for (k=0; k<m_Intrados.m_iCtrlPoints;k++)
	{
		strong.Format(" %f  %f\n", 
			m_Intrados.m_Input[k].x, m_Intrados.m_Input[k].y);
		pFile->Write(strong, strong.GetLength());
	}

	// number of output points
	strong.Format("%d\n", m_Extrados.m_iRes);
	pFile->Write(strong, strong.GetLength());
	// number of output points
	strong.Format("%d\n", m_Intrados.m_iRes);
	pFile->Write(strong, strong.GetLength());
	
	m_bModified = false;
	return true;
}


void CSF::SetViewRect(CRect rc)
{
	m_Intrados.m_rViewRect = rc;
	m_Extrados.m_rViewRect = rc;
}


bool CSF::Serialize(CArchive &ar)
{
	float f;
		int ArchiveFormat,k ;

	if(ar.IsLoading())
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat < 100000 || ArchiveFormat > 110000) return false;
		ar >> m_strFoilName; m_strFoilName = "Spline Foil";
		ar >> m_FoilColor >> m_FoilStyle >> m_FoilWidth;

		ar >> m_Extrados.m_iCtrlPoints;
		ar >> m_Extrados.m_iDegree;
		for (k=0; k<m_Extrados.m_iCtrlPoints;k++)
		{
			ar >> f; m_Extrados.m_Input[k].x =f;
			ar >> f; m_Extrados.m_Input[k].y =f;
		}
		if(ArchiveFormat<100306) 
		{
			ar >> f; m_Extrados.m_Input[k].x =f;
			ar >> f; m_Extrados.m_Input[k].y =f;
		}

		ar >> m_Intrados.m_iCtrlPoints;
		ar >> m_Intrados.m_iDegree;
		for (k=0; k<m_Intrados.m_iCtrlPoints;k++)
		{
			ar >> f; m_Intrados.m_Input[k].x =f;
			ar >> f; m_Intrados.m_Input[k].y =f;
		}
		if(ArchiveFormat<100306) 
		{
			ar >> f; m_Extrados.m_Input[k].x =f;
			ar >> f; m_Extrados.m_Input[k].y =f;
		}
		ar >> m_Extrados.m_iRes >> m_Intrados.m_iRes;
		ar >> k;
		if(k !=0 && k !=1)
		{
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bVisible = true; else m_bVisible = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bOutPoints = true; else m_bOutPoints = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bCenterLine = true; else m_bCenterLine = false;

		m_Extrados.SplineKnots();
		m_Intrados.SplineKnots();
		Update(true);
		Update(false);
		m_bModified = false;
		return true;
	}
	else
	{
		ar << 100306;
		// 100306 : changed to C convention	the number of control points

		ar << m_strFoilName;
		ar << m_FoilColor << m_FoilStyle << m_FoilWidth;

		ar << m_Extrados.m_iCtrlPoints;
		ar << m_Extrados.m_iDegree;
		for (k=0; k<m_Extrados.m_iCtrlPoints;k++)
		{
			ar << (float)m_Extrados.m_Input[k].x << (float)m_Extrados.m_Input[k].y;
		}

		ar << m_Intrados.m_iCtrlPoints;
		ar << m_Intrados.m_iDegree;
		for (k=0; k<m_Intrados.m_iCtrlPoints;k++)
		{
			ar << (float)m_Intrados.m_Input[k].x << (float)m_Intrados.m_Input[k].y;
		}

		ar << m_Extrados.m_iRes << m_Intrados.m_iRes;
		if(m_bVisible)    ar<<1; else ar <<0;
		if(m_bOutPoints)  ar<<1; else ar <<0;
		if(m_bCenterLine) ar<<1; else ar <<0;

		m_bModified = false;
		return true;
	}
}
