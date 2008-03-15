/****************************************************************************

    Reference Foil Class
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


// RefFoil.cpp: implementation of the CFoil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "Foil.h"
#include "../Design/Spline.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFoil::CFoil()
{
	m_nFoilStyle = PS_SOLID;
	m_nFoilWidth = 1;
	m_FoilColor  = 0;

	m_iHighLight = -1;

	m_bCenterLine = false;
	m_bPoints     = false;
	m_bVisible    = true;
	m_bSaved      = true;

	m_FoilName = "";

	m_fCamber     = 0.0;
	m_fXCamber    = 0.0;
	m_fThickness  = 0.0;
	m_fXThickness = 0.0;

	n = 0;
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));
	memset(nx, 0, sizeof(nx));
	memset(ny, 0, sizeof(ny));

	nb = 0;
	memset(xb, 0, sizeof(xb));
	memset(yb, 0, sizeof(yb));


	m_iInt = 0;
	m_iExt = 0;
	m_iBaseExt = 0;
	m_iBaseInt = 0;

	m_Gap  = 0.0;

	memset(m_rpExtrados, 0, sizeof(m_rpExtrados));
	memset(m_rpIntrados, 0, sizeof(m_rpIntrados));
	memset(m_rpMid, 0, sizeof(m_rpMid));

	memset(m_BaseExtrados, 0, sizeof(m_BaseExtrados));
	memset(m_BaseIntrados, 0, sizeof(m_BaseIntrados));

//	m_rViewRect.SetRectEmpty();

	m_bTEFlap     = false;
	m_TEFlapAngle = 0.0;
	m_TEXHinge    = 80.0;
	m_TEYHinge    = 50.0;

	m_bLEFlap     = false;
	m_LEFlapAngle = 0.0;
	m_LEXHinge    = 20.0;
	m_LEYHinge    = 50.0;

}

CFoil::~CFoil()
{
}

 
bool CFoil::InitFoil()
{
	//Initializes the foil geometry,
	// constructs the upper and lower points
	// aplies to the flap deflection to the current foil if relevant

	// at this point, coordinates have been loaded
	// so has been the number of points defining the foil
	bool bNotFound = true;
	int k = 0;
//	NormalizeGeometry();

	//first time is to calculate the base foil's thickness and camber

	if(nb<=0) AfxMessageBox("Foil Error : no points");
	while (k<nb)
	{
		if (xb[k+1] < xb[k]) 
		{
			k++;
		}
		else 
		{
			if(bNotFound)
			{
				m_iBaseExt = k;
				m_BaseExtrados[k].x = xb[k];
				m_BaseExtrados[k].y = yb[k];
				bNotFound = false;
			}
			m_BaseIntrados[k-m_iBaseExt].x = xb[k];
			m_BaseIntrados[k-m_iBaseExt].y = yb[k];
			k++;
		}
	}
	m_iBaseInt = nb-m_iBaseExt-1;
	m_BaseIntrados[nb-m_iBaseExt-1].x = xb[nb-1];
	m_BaseIntrados[nb-m_iBaseExt-1].y = yb[nb-1];
	for (k=0; k<=m_iBaseExt;k++)
	{
		m_BaseExtrados[k].x = xb[m_iBaseExt-k];
		m_BaseExtrados[k].y = yb[m_iBaseExt-k];
	}

	memcpy(m_rpExtrados, m_BaseExtrados, sizeof(m_rpExtrados));
	memcpy(m_rpIntrados, m_BaseIntrados, sizeof(m_rpIntrados));
	m_iExt = m_iBaseExt;
	m_iInt = m_iBaseInt;

	CompMidLine(true);
	memcpy(m_rpBaseMid, m_rpMid, sizeof(m_rpBaseMid));


	m_Gap = m_BaseExtrados[m_iBaseExt].y-m_BaseIntrados[m_iBaseInt].y;

	m_LE.x = (m_BaseIntrados[0].x+m_BaseExtrados[0].x)/2.0;
	m_LE.y = (m_BaseIntrados[0].y+m_BaseExtrados[0].y)/2.0;
	
	m_TE.x = (m_BaseIntrados[m_iBaseInt].x+m_BaseExtrados[m_iBaseExt].x)/2.0;
	m_TE.y = (m_BaseIntrados[m_iBaseInt].y+m_BaseExtrados[m_iBaseExt].y)/2.0;


	//the second time is to get the mid line of the current foil
	//i.e. with flaps eventually
	//used for the VLM analysis
	k=0;
	bNotFound = true;
	while (k<n){
		if (x[k+1] < x[k]) 
		{
			k++;
		}
		else {
			if(bNotFound)
			{
				m_iExt = k;
				m_rpExtrados[k].x = x[k];
				m_rpExtrados[k].y = y[k];
				bNotFound = false;
			}
			m_rpIntrados[k-m_iExt].x = x[k];
			m_rpIntrados[k-m_iExt].y = y[k];
			k++;
		}
	}
	m_iInt = n-m_iExt-1;
	m_rpIntrados[n-m_iExt-1].x = x[n-1];
	m_rpIntrados[n-m_iExt-1].y = y[n-1];
	for (k=0; k<=m_iExt;k++)
	{
		m_rpExtrados[k].x = x[m_iExt-k];
		m_rpExtrados[k].y = y[m_iExt-k];
	}
	CompMidLine(false);

	return true;
}



void CFoil::DrawMidLine(CDC *pDC, double scalex, double scaley, 
						CPoint Offset, CRect DrawRect, bool IsPrinting)
{
	CPoint From, To;
	int xp, yp;

	if (IsPrinting){
		scalex =  scalex;
		scaley = -scaley;
	}

	From.x = (int)( m_rpMid[0].x*scalex)  +Offset.x;
	From.y = (int)(-m_rpMid[0].y*scaley)  +Offset.y;
	pDC->MoveTo(From);

	for (int k=1; k<1000; k++){
		To.x = (int)( m_rpMid[k].x*scalex)+Offset.x;
		To.y = (int)(-m_rpMid[k].y*scaley)+Offset.y;


		if(DrawRect.PtInRect(From) && DrawRect.PtInRect(To)){
			pDC->LineTo(To);
		}
		else if(DrawRect.PtInRect(From) && !DrawRect.PtInRect(To)){
			xp = From.x;
			yp = From.y;
			if(Intersect(xp,yp, DrawRect, From, To)){
				pDC->LineTo(xp,yp);				
			}
		}
		else if(!DrawRect.PtInRect(From) && DrawRect.PtInRect(To)){
			xp = From.x;
			yp = From.y;
			if(Intersect(xp,yp, DrawRect, From, To)){
				pDC->MoveTo(xp,yp);
				pDC->LineTo(To);
			}
		}
		else pDC->MoveTo(To);
		From = To;

	}
}



bool CFoil::DrawPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect DrawRect, bool IsPrinting)
{
	int width;
	CPoint pt1, pt2;

	if (IsPrinting){
		width = 50;
		scaley=-scaley;
	}
	else{
		width = 2;
	}
	for (int i=0; i<n;i++){
		pt1.x = (int)( x[i]*scalex+Offset.x-width);
		pt1.y = (int)(-y[i]*scaley+Offset.y-width);
		pt2.x = (int)( x[i]*scalex+Offset.x+width);
		pt2.y = (int)(-y[i]*scaley+Offset.y+width);
		if(DrawRect.PtInRect(pt1) && DrawRect.PtInRect(pt2))
			pDC->Rectangle(pt1.x, pt1.y, pt2.x, pt2.y) ;
	}
	if(m_iHighLight>=0 && !IsPrinting){
		CPen HighPen(PS_SOLID, 2, RGB(255,0,0));
		CPen *pOldPen = pDC->SelectObject(&HighPen);
		pt1.x = (int)( x[m_iHighLight]*scalex+Offset.x-width);
		pt1.y = (int)(-y[m_iHighLight]*scaley+Offset.y-width);
		pt2.x = (int)( x[m_iHighLight]*scalex+Offset.x+width);
		pt2.y = (int)(-y[m_iHighLight]*scaley+Offset.y+width);
		if(DrawRect.PtInRect(pt1) && DrawRect.PtInRect(pt2))
			pDC->Rectangle(pt1.x, pt1.y, pt2.x, pt2.y) ;
		pDC->SelectObject(pOldPen);
	
	}

	return true;
}


void CFoil::DrawFoil(CDC *pDC, double alpha, double scalex, double scaley,
					 CPoint Offset, CRect DrawRect,  bool IsPrinting)
{
	double xa,ya,sina,cosa;
	CPoint pt, From, To;
	int xp, yp;

	cosa = cos(alpha);
	sina = sin(alpha);

	if (IsPrinting){
		scalex =  scalex;
		scaley = -scaley;
	}

	xa = (x[0]-0.5)*cosa - y[0]*sina + 0.5;
	ya = (x[0]-0.5)*sina + y[0]*cosa;
	From.x = (int)( xa*scalex+Offset.x);
	From.y = (int)(-ya*scaley+Offset.y);

	if(DrawRect.PtInRect(From)) pDC->MoveTo(From);

	for (int k=1; k<n; k++){
		xa = (x[k]-0.5)*cosa - y[k]*sina+ 0.5;
		ya = (x[k]-0.5)*sina + y[k]*cosa;
		To.x = (int)( xa*scalex+Offset.x);
		To.y = (int)(-ya*scaley+Offset.y);

		if(DrawRect.PtInRect(From) && DrawRect.PtInRect(To)){
			pDC->LineTo(To);
		}
		else if(DrawRect.PtInRect(From) && !DrawRect.PtInRect(To)){
			xp = From.x;
			yp = From.y;
			if(Intersect(xp,yp, DrawRect, From, To)){
				pDC->LineTo(xp,yp);				
			}
		}
		else if(!DrawRect.PtInRect(From) && DrawRect.PtInRect(To)){
			xp = From.x;
			yp = From.y;
			if(Intersect(xp,yp, DrawRect, From, To)){
				pDC->MoveTo(xp,yp);
				pDC->LineTo(To);
			}
		}
		else pDC->MoveTo(To);
		From = To;
	}
}

bool CFoil::SetCamber(double f, double u)
{
	// keeps the same thickness distribution
	//c is the new camber, as a % of the chord
	//x is the point of max camber
	//first lets find parameters of y(x) = ax3+bx2+cx+d
	//d=0
	double u3    = u*u*u;
	double u2    = u*u;
	double du1   = 2.0*u-1.0;
	double inter =  u3 - u -(u2-u)*(3.0*u2-1.0)/du1;
	double a     = f/inter;
	double b     = a*(1-3*u2)/du1;
	double c     = -a*(2.0*u-3.0*u2)/du1;

	//for each extrados point
	for (int j=0; j<=m_iExt; j++){
		//find the opposite point
		double newy =    a * m_rpMid[j].x*m_rpMid[j].x*m_rpMid[j].x
					  + b * m_rpMid[j].x*m_rpMid[j].x
					  + c * m_rpMid[j].x;
		m_rpExtrados[j].y = newy + m_rpExtrados[j].y - m_rpMid[j].y;
		m_rpMid[j].y      = newy;
	}
	return true;
}


bool CFoil::ExportFoil(CString FileName)
{
	CStdioFile XFile;
	CFileException fe;

	BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);
	if (bOpen){
		CString strOut;

		XFile.WriteString(m_FoilName+"\n");
		for (int i=0; i< n; i++){
			strOut.Format("%8.5f   %8.5f\n",x[i],y[i]);
			XFile.WriteString(strOut);
		}
/*		for (int i=50; i>=0; i--){
			strOut.Format("%8.5f   %8.5f\n",m_rpMid[i*20].x,m_rpMid[i*20].y);
			XFile.WriteString(strOut);
		}
		for (int i=0; i<= 50; i++){
			strOut.Format("%8.5f   %8.5f\n",m_rpMid[i*20].x,-m_rpMid[i*20].y);
			XFile.WriteString(strOut);
		}*/
		
		m_bSaved = true;

		XFile.Close();
	}
	return true;
}


double CFoil::GetBaseUpperY(double x)
{
	// Returns the y-coordinate on the base foil's upper surface at the x position
//	x = x*(m_BaseExtrados[m_iExt].x-m_BaseExtrados[0].x);//in case there is a flap which reduces the length
	double y;
	for (int i=0; i<m_iBaseExt; i++)
	{
		if (m_BaseExtrados[i].x <m_BaseExtrados[i+1].x  && 
			m_BaseExtrados[i].x <= x && x<=m_BaseExtrados[i+1].x )
		{
			y = (m_BaseExtrados[i].y 	+ (m_BaseExtrados[i+1].y-m_BaseExtrados[i].y)
									 /(m_BaseExtrados[i+1].x-m_BaseExtrados[i].x)*(x-m_BaseExtrados[i].x));
			return y;
		}
	}
	return 0.0;
}

double CFoil::GetBaseLowerY(double x)
{
	// Returns the y-coordinate on the base foil's lower surface at the x position
//	x = x*(m_BaseIntrados[m_iInt].x-m_BaseIntrados[0].x);//in case there is a flap which reduces the length
	double y;
	for (int i=0; i<m_iBaseInt; i++)
	{
		if (m_BaseIntrados[i].x <m_BaseIntrados[i+1].x  && 
			m_BaseIntrados[i].x <= x && x<=m_BaseIntrados[i+1].x )
		{
			y = (m_BaseIntrados[i].y 	+ (m_BaseIntrados[i+1].y-m_BaseIntrados[i].y)
							 /(m_BaseIntrados[i+1].x-m_BaseIntrados[i].x)*(x-m_BaseIntrados[i].x));
			return y;
		}
	}
	return 0.0;
}

double CFoil::GetUpperY(double x)
{
	// Returns the y-coordinate on the current foil's upper surface at the x position
	x = m_rpExtrados[0].x + x*(m_rpExtrados[m_iExt].x-m_rpExtrados[0].x);//in case there is a flap which reduces the length
	double y;
	for (int i=0; i<m_iExt; i++)
	{
		if (m_rpExtrados[i].x <m_rpExtrados[i+1].x  && 
			m_rpExtrados[i].x <= x && x<=m_rpExtrados[i+1].x )
		{
			y = (m_rpExtrados[i].y 	+ (m_rpExtrados[i+1].y-m_rpExtrados[i].y)
									 /(m_rpExtrados[i+1].x-m_rpExtrados[i].x)*(x-m_rpExtrados[i].x));
			return y;
		}
	}
	return 0.0;
}


double CFoil::GetLowerY(double x)
{
	// Returns the y-coordinate on the current foil's lower surface at the x position
	x = m_rpIntrados[0].x + x*(m_rpIntrados[m_iInt].x-m_rpIntrados[0].x);//in case there is a flap which reduces the length
	double y;
	for (int i=0; i<m_iInt; i++)
	{
		if (m_rpIntrados[i].x <m_rpIntrados[i+1].x  && 
			m_rpIntrados[i].x <= x && x<=m_rpIntrados[i+1].x )
		{
			y = (m_rpIntrados[i].y 	+ (m_rpIntrados[i+1].y-m_rpIntrados[i].y)
									 /(m_rpIntrados[i+1].x-m_rpIntrados[i].x)*(x-m_rpIntrados[i].x));
			return y;
		}
	}
	return 0.0;
}

double CFoil::GetMidY(double x)
{
	//Returns current foil's mid position at the x position
	double u = GetUpperY(x);
	double l = GetLowerY(x);
	return (u+l)/2.0;
}


double CFoil::NormalizeGeometry()
{
	// Normalizes the base foil's lengh to unity
	// The current foil's length is modified by the same ratio
	int i;
	double xmin = 1.0;
	double xmax = 0.0;

	for (i=0; i<nb; i++){
		xmin = __min(xmin, xb[i]);
		xmax = __max(xmax, xb[i]);
	}
	double length = xmax - xmin;

	//reset origin
	for (i=0; i<nb; i++) xb[i] -= xmin;
	
	//set length to 1.
	for(i=0; i<nb; i++){
		xb[i] = xb[i]/length;
		yb[i] = yb[i]/length;
	}



	//reset origin
	for (i=0; i<n; i++)	x[i] -= xmin;
	
	//set length to 1.
	for(i=0; i<n; i++){
		x[i] = x[i]/length;
		y[i] = y[i]/length;
	}

	return length;
}


void CFoil::CopyFoil(CFoil *pSrcFoil)
{
	// Copies the foil from a source foil

	memcpy(x, pSrcFoil->x,  sizeof(pSrcFoil->x));
	memcpy(y, pSrcFoil->y,  sizeof(pSrcFoil->y));
	memcpy(xb,pSrcFoil->xb, sizeof(pSrcFoil->xb));
	memcpy(yb,pSrcFoil->yb, sizeof(pSrcFoil->yb));
	memcpy(nx,pSrcFoil->nx, sizeof(pSrcFoil->nx));
	memcpy(ny,pSrcFoil->ny, sizeof(pSrcFoil->ny));
	memcpy(m_rpMid,        pSrcFoil->m_rpMid,        sizeof(m_rpMid));
	memcpy(m_rpExtrados,   pSrcFoil->m_rpExtrados,   sizeof(m_rpExtrados));
	memcpy(m_rpIntrados,   pSrcFoil->m_rpIntrados,   sizeof(m_rpIntrados));
	memcpy(m_BaseExtrados, pSrcFoil->m_BaseExtrados, sizeof(m_BaseExtrados));
	memcpy(m_BaseIntrados, pSrcFoil->m_BaseIntrados, sizeof(m_BaseIntrados));
	m_iExt = pSrcFoil->m_iExt;
	m_iInt = pSrcFoil->m_iInt;
	m_iBaseExt = pSrcFoil->m_iBaseExt;
	m_iBaseInt = pSrcFoil->m_iBaseInt;
	m_Gap  = pSrcFoil->m_Gap;
	m_LE.x = pSrcFoil->m_LE.x;
	m_LE.y = pSrcFoil->m_LE.y;

	m_fThickness  = pSrcFoil->m_fThickness;
	m_fXThickness = pSrcFoil->m_fXThickness;
	m_fCamber     = pSrcFoil->m_fCamber;
	m_fXCamber    = pSrcFoil->m_fXCamber;

	n  = pSrcFoil->n;
	nb = pSrcFoil->nb;
	m_FoilName  = pSrcFoil->m_FoilName;

	m_bLEFlap     = pSrcFoil->m_bLEFlap;
	m_LEFlapAngle = pSrcFoil->m_LEFlapAngle;
	m_LEXHinge    = pSrcFoil->m_LEXHinge;
	m_LEYHinge    = pSrcFoil->m_LEYHinge;

	m_bTEFlap     = pSrcFoil->m_bTEFlap;
	m_TEFlapAngle = pSrcFoil->m_TEFlapAngle;
	m_TEXHinge    = pSrcFoil->m_TEXHinge;
	m_TEYHinge    = pSrcFoil->m_TEYHinge;

	m_FoilColor   = pSrcFoil->m_FoilColor;
	m_nFoilStyle  = pSrcFoil->m_nFoilStyle;
	m_nFoilWidth  = pSrcFoil->m_nFoilWidth;
	m_bCenterLine = pSrcFoil->m_bCenterLine;
	m_bPoints     = pSrcFoil->m_bPoints;
}


void CFoil::SetNaca009()
{	
	// Reset the foil to Naca 009
	x[0]  = 1.00000    ; y[0]  = 0.00000;
	x[1]  = 0.99572    ; y[1]  = 0.00057;
	x[2]  = 0.98296    ; y[2]  = 0.00218;
	x[3]  = 0.96194    ; y[3]  = 0.00463;
	x[4]  = 0.93301    ; y[4]  = 0.00770;
	x[5]  = 0.89668    ; y[5]  = 0.01127;
	x[6]  = 0.85355    ; y[6]  = 0.01522;
	x[7]  = 0.80438    ; y[7]  = 0.01945;
	x[8]  = 0.75000    ; y[8]  = 0.02384;
	x[9]  = 0.69134    ; y[9]  = 0.02823;
	x[10] = 0.62941    ; y[10] = 0.03247;
	x[11] = 0.56526    ; y[11] = 0.03638;
	x[12] = 0.50000    ; y[12] = 0.03978;
	x[13] = 0.43474    ; y[13] = 0.04248;
	x[14] = 0.37059    ; y[14] = 0.04431;
	x[15] = 0.33928    ; y[15] = 0.04484;
	x[16] = 0.30866    ; y[16] = 0.04509;
	x[17] = 0.27886    ; y[17] = 0.04504;
	x[18] = 0.25000    ; y[18] = 0.04466;
	x[19] = 0.22221    ; y[19] = 0.04397;
	x[20] = 0.19562    ; y[20] = 0.04295;
	x[21] = 0.17033    ; y[21] = 0.04161;
	x[22] = 0.14645    ; y[22] = 0.03994;
	x[23] = 0.12408    ; y[23] = 0.03795;
	x[24] = 0.10332    ; y[24] = 0.03564;
	x[25] = 0.08427    ; y[25] = 0.03305;
	x[26] = 0.06699    ; y[26] = 0.03023;
	x[27] = 0.05156    ; y[27] = 0.02720;
	x[28] = 0.03806    ; y[28] = 0.02395;
	x[29] = 0.02653    ; y[29] = 0.02039;
	x[30] = 0.01704    ; y[30] = 0.01646;
	x[31] = 0.00961    ; y[31] = 0.01214;
	x[32] = 0.00428    ; y[32] = 0.00767;
	x[33] = 0.00107    ; y[33] = 0.00349;
	x[34] = 0.00000    ; y[34] = 0.00000;
	for (int i=0; i<34; i++){
		x[i+35] =  x[33-i];
		y[i+35] = -y[33-i];
	}
	n = 69;
	nb = 69;
	memcpy(xb,x, sizeof(x));
	InitFoil();
}

void CFoil::Serialize(CArchive &ar)
{
	// saves or loads the foil to the archive ar

	int ArchiveFormat = 1005;
	// 1005 : added LE Flap data
	// 1004 : added Points and Centerline property
	// 1003 : added Visible property
	// 1002 : added color and style save
	// 1001 : initial format
	int p;

	if(ar.IsStoring()){
		ar << ArchiveFormat;
		ar << m_FoilName;
		ar << m_nFoilStyle << m_nFoilWidth << m_FoilColor;
		if (m_bVisible)		ar << 1; else ar << 0;
		if (m_bPoints)		ar << 1; else ar << 0;//1004
		if (m_bCenterLine)	ar << 1; else ar << 0;//1004
		if (m_bLEFlap)		ar << 1; else ar << 0;
		ar << (float)m_LEFlapAngle << (float)m_LEXHinge << (float)m_LEYHinge;
		if (m_bTEFlap)		ar << 1; else ar << 0;
		ar << (float)m_TEFlapAngle << (float)m_TEXHinge << (float)m_TEYHinge;
		ar << 1.f << 1.f << 9.f;//formerly transition parameters
		ar << nb;
		for (int j=0; j<nb; j++){
			ar << (float)xb[j] << (float)yb[j];
		}
		ar << n;
		for (j=0; j<n; j++){
			ar << (float)x[j] << (float)y[j];
		}
	}
	else {
		float f,g;
		ar >> ArchiveFormat;
		//Archive Format 1001 : adds the modified foil to the file
		ar >> m_FoilName;
		if(ArchiveFormat>=1002){
			ar >> m_nFoilStyle >> m_nFoilWidth >> m_FoilColor;
		}
		if(ArchiveFormat>=1003){
			ar >> p;
			if(p) m_bVisible = true; else m_bVisible = false;
		}
		if(ArchiveFormat>=1004){
			ar >> p;
			if(p) m_bPoints = true; else m_bPoints = false;
			ar >> p;
			if(p) m_bCenterLine = true; else m_bCenterLine = false;
		}

		if(ArchiveFormat>=1005){
			ar >> p;
			if (p) m_bLEFlap = true; else m_bLEFlap = false;
			ar >> f; m_LEFlapAngle =f;
			ar >> f; m_LEXHinge = f;
			ar >> f; m_LEYHinge = f;
		}
		ar >> p;
		if (p) m_bTEFlap = true; else m_bTEFlap = false;
		ar >> f; m_TEFlapAngle =f;
		ar >> f; m_TEXHinge = f;
		ar >> f; m_TEYHinge = f;

		ar >> f >> f >> f; //formerly transition parameters
		ar >> nb;
		if(nb>IBX){
			m_FoilName = "";
			return;
		}
		for (int j=0; j<nb; j++){
			ar >> f >> g;
			xb[j]  = f;  yb[j]=g;
		}
		if(ArchiveFormat>=1001){
			ar >> n;
			if(n>IBX){
				m_FoilName = "";
				return;
			}
			for (j=0; j<n; j++){
				ar >> f >> g;
				x[j]=f; y[j]=g;
			}
			if(nb==0 && n!=0){
				nb = n;
				memcpy(xb,x, sizeof(xb));
				memcpy(yb,y, sizeof(yb));
			}
		}
		else{
			memcpy(x,xb, sizeof(xb));
			memcpy(y,yb, sizeof(yb));
			n=nb;
		}
	}
}

double CFoil::GetLength()
{
	//Returns the foil's length

	return max(m_rpExtrados[m_iExt].x, m_rpExtrados[m_iInt].x);
}

double CFoil::GetArea()
{
	int i,ip;
	double area = 0.0;
	for (i=0; i<nb; i++){
		if(i==nb-1)	ip = 0;
		else		ip = i+1;
		area +=  0.5*(yb[i]+yb[ip]) * (xb[i]-xb[ip]);
	}

	return abs(area);
}
double CFoil::GetCamber(double x)
{
	//returns the camber value at position x
	for (int i=0; i<=1000; i++){
		if ((m_rpMid[i].x <= x) && (x < m_rpMid[i+1].x)){
			return (m_rpMid[i+1].y+m_rpMid[i].y)/2.0;
		}
	}
	return 0.0;
}

double CFoil::GetCamberAngle(double x)
{
	//returns the camber angle at position x

	for (int i=0; i<=1000; i++){
		if ((m_rpMid[i].x <= x) && (x < m_rpMid[i+1].x)){
			double dx = m_rpMid[i+1].x-m_rpMid[i].x;
			double dy = m_rpMid[i+1].y-m_rpMid[i].y;
			double alpha = atan(dy/dx)*180.0/3.141592654;
			return alpha;
		}
	}
	return 0.0;
}

double CFoil::GetCamberSlope(double x)
{
	//returns the camber slope at position x

	for (int i=0; i<1000; i++){
		if ((m_rpMid[i].x <= x) && (x < m_rpMid[i+1].x)){
			double dx = m_rpMid[i+1].x-m_rpMid[i].x;
			double dy = m_rpMid[i+1].y-m_rpMid[i].y;
			return atan2(dy,dx);
		}
	}
	if(x>=1.0)
	{
			double dx = m_rpMid[1000].x-m_rpMid[999].x;
			double dy = m_rpMid[1000].y-m_rpMid[999].y;
			return atan2(dy,dx);
	}
	return 0.0;
}

double CFoil::GetTopSlope(double x)
{
	//returns the upper slope at position x

	for (int i=0; i<m_iExt; i++){
		if ((m_rpExtrados[i].x <= x) && (x < m_rpExtrados[i+1].x)){
			double dx = m_rpExtrados[i+1].x-m_rpExtrados[i].x;
			double dy = m_rpExtrados[i+1].y-m_rpExtrados[i].y;
			return -atan2(dy,dx);
		}
	}
	return 0.0;
}

double CFoil::GetBotSlope(double x)
{
	//returns the bottom slope at position x

	for (int i=0; i<m_iInt; i++){
		if ((m_rpIntrados[i].x <= x) && (x < m_rpIntrados[i+1].x)){
			double dx = m_rpIntrados[i+1].x-m_rpIntrados[i].x;
			double dy = m_rpIntrados[i+1].y-m_rpIntrados[i].y;
			return -atan2(dy,dx);
		}
	}
	return 0.0;
}

double CFoil::DeRotate()
{
	//De-rotates the foil, i.e. aligns the mid-line with the x-axis

	double angle, cosa, sina;//we're dealing with real small angles
	int i;
	// first find offset
	//and translate the leading edge to the origin point
	for (i=0; i<nb; i++){
		xb[i] -= m_LE.x;
		yb[i] -= m_LE.y;
	}
	for (i=0; i<n; i++){
		x[i] -= m_LE.x;
		y[i] -= m_LE.y;
	}
	InitFoil();//to get the new LE and TE

	// then find current angle
	angle = atan2(m_TE.y-m_LE.y, m_TE.x-m_LE.x);// xle=tle=0;

	//rotate about the L.E.
	cosa = cos(angle);
	sina = sin(angle);
	for (i=0; i<nb; i++){
		xb[i] = ( (xb[i]-m_LE.x)*cosa + (yb[i]-m_LE.y)*sina);
		yb[i] = (-(xb[i]-m_LE.x)*sina + (yb[i]-m_LE.y)*cosa);
	}

	for (i=0; i<n; i++){
		x[i] = ( (x[i]-m_LE.x)*cosa + (y[i]-m_LE.y)*sina);
		y[i] = (-(x[i]-m_LE.x)*sina + (y[i]-m_LE.y)*cosa);
	}

	
	InitFoil();

	return angle*180.0/3.141592654;
}

int CFoil::IsPoint(CVector Real)
{
	// Returns the index of foil's point which coincides with the input point, if any
	// Otherwise returns -10

	for (int k=0; k<n; k++){
		if(fabs(Real.x-x[k])<0.005 && fabs(Real.y-y[k])<0.005) return k;
	}
	return -10;
}



void  CFoil::SetTEFlap(bool bFlap, double xhinge, double yhinge, double angle)
{
	// Sets a trailing edge flap properties
	// x and y hinge are in relative % coordinates
	// angle is in °

	m_bTEFlap     = bFlap;
	m_TEXHinge    = xhinge;
	m_TEYHinge    = yhinge;
	m_TEFlapAngle = angle;
}

void  CFoil::SetLEFlap(bool bFlap, double xhinge, double yhinge, double angle)
{
	// Sets a leading edge flap properties
	// x and y hinge are in relative % coordinates
	// angle is in °
	m_bLEFlap     = bFlap;
	m_LEXHinge    = xhinge;
	m_LEYHinge    = yhinge;
	m_LEFlapAngle = angle;
}


void CFoil::SetFlap()
{
	// modifies the current airfoil's geometry 
	// by setting a flap i.a.w. the member variables

	int i, j, k, l, p, i1, i2;
	double xh, yh, dx, dy;
	CVector M;
	bool bIntersect;
	double cosa, sina;

	double pi = 3.141592654;

	//copy the base foil to the current foil
	memcpy(m_rpExtrados, m_BaseExtrados, sizeof(m_rpExtrados));
	memcpy(m_rpIntrados, m_BaseIntrados, sizeof(m_rpIntrados));

	m_iExt = m_iBaseExt;
	m_iInt = m_iBaseInt;

// First Leading Edge Flap
	if(m_bLEFlap){
		cosa = cos(m_LEFlapAngle*pi/180.0);
		sina = sin(m_LEFlapAngle*pi/180.0);
		//first convert xhinge and yhinge in absolute coordinates
		xh = m_LEXHinge/100.0;
		double ymin = GetBaseLowerY(xh);
		double ymax = GetBaseUpperY(xh);
		yh = ymin + m_LEYHinge/100.0 * (ymax-ymin);

		// insert a breakpoint at xhinge location, if there isn't one already
		int iUpperh = 0;
		int iLowerh = 0;
		for (i=0; i<m_iExt; i++){
			if(fabs(m_rpExtrados[i].x-xh)<0.001) {
				//then no need to add an extra point, just break
				iUpperh = i;
				break;
			}
			else if(m_rpExtrados[i].x>xh){//insert one
				for(j=m_iExt+1; j>i; j--){
					m_rpExtrados[j].x = m_rpExtrados[j-1].x;
					m_rpExtrados[j].y = m_rpExtrados[j-1].y;
				}

				m_rpExtrados[i].x = xh;
				m_rpExtrados[i].y = ymax;
				iUpperh = i;
				m_iExt+=1;
				break;
			}
		}
		
		for (i=0; i<m_iInt; i++){
			if(fabs(m_rpIntrados[i].x-xh)<0.001) {
				//then no need to add an Intra point, just break
				iLowerh = i;
				break;
			}
			else if(m_rpIntrados[i].x>xh){//insert one
				for(j=m_iInt+1; j>i; j--){
					m_rpIntrados[j].x = m_rpIntrados[j-1].x;
					m_rpIntrados[j].y = m_rpIntrados[j-1].y;
				}

				m_rpIntrados[i].x = xh;
				m_rpIntrados[i].y = ymin;
				iLowerh = i;
				m_iInt+=1;
				break;
			}
		}

		// rotate all points upstream of xh
		if(m_LEFlapAngle>0.0){
			//insert an extra point on intrados
			for (i=m_iInt+1; i>iLowerh; i--){
				m_rpIntrados[i] = m_rpIntrados[i-1];
			}
			m_rpIntrados[iLowerh] = m_rpIntrados[iLowerh+1];
			iLowerh++;
			m_iInt++;

			// extend to infinity last segments around hinge on flap internal side to make sure 
			// they intersect the spline on the other side
			m_rpIntrados[iLowerh-1].x += 30.0*(m_rpIntrados[iLowerh-1].x-m_rpIntrados[iLowerh-2].x);
			m_rpIntrados[iLowerh-1].y += 30.0*(m_rpIntrados[iLowerh-1].y-m_rpIntrados[iLowerh-2].y);
			m_rpIntrados[iLowerh].x   += 30.0*(m_rpIntrados[iLowerh].x   - m_rpIntrados[iLowerh+1].x);
			m_rpIntrados[iLowerh].y   += 30.0*(m_rpIntrados[iLowerh].y   - m_rpIntrados[iLowerh+1].y);
		}
		if(m_LEFlapAngle<0.0){
			//insert an extra point on extrados
			for (i=m_iExt+1; i>iUpperh; i--){
				m_rpExtrados[i] = m_rpExtrados[i-1];
			}
			m_rpExtrados[iUpperh] = m_rpExtrados[iUpperh+1];
			iUpperh++;
			m_iExt++;

			// extend to infinity last segments around hinge on flap internal side to make sure 
			// they intersect the spline on the other side
			m_rpExtrados[iUpperh-1].x += 30.0 * (m_rpExtrados[iUpperh-1].x - m_rpExtrados[iUpperh-2].x);
			m_rpExtrados[iUpperh-1].y += 30.0 * (m_rpExtrados[iUpperh-1].y - m_rpExtrados[iUpperh-2].y);
			m_rpExtrados[iUpperh].x   += 30.0 * (m_rpExtrados[iUpperh].x   - m_rpExtrados[iUpperh+1].x);
			m_rpExtrados[iUpperh].y   += 30.0 * (m_rpExtrados[iUpperh].y   - m_rpExtrados[iUpperh+1].y);
		}
		for (i=0; i<iUpperh; i++){
			dx = m_rpExtrados[i].x-xh;
			dy = m_rpExtrados[i].y-yh;
			m_rpExtrados[i].x = xh + cosa * dx - sina * dy;
			m_rpExtrados[i].y = yh + sina * dx + cosa * dy;
		}
		for (i=0; i<iLowerh; i++){
			dx = m_rpIntrados[i].x-xh;
			dy = m_rpIntrados[i].y-yh;
			m_rpIntrados[i].x = xh + cosa * dx - sina * dy;
			m_rpIntrados[i].y = yh + sina * dx + cosa * dy;
		}

		CSpline LinkSpline;
		LinkSpline.m_iRes = 4;
		LinkSpline.m_iDegree = 3;
		LinkSpline.m_iCtrlPoints = -1;

		if(m_LEFlapAngle<0.0){

			//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
			Intersect(m_rpIntrados[iLowerh-2], m_rpIntrados[iLowerh-1],
					  m_rpIntrados[iLowerh],   m_rpIntrados[iLowerh+1], &M);
			LinkSpline.InsertPoint(m_rpIntrados[iLowerh-1].x,m_rpIntrados[iLowerh-1].y);
			LinkSpline.InsertPoint(M.x, M.y);
			LinkSpline.InsertPoint(m_rpIntrados[iLowerh].x,m_rpIntrados[iLowerh].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			//retrieve point 1 and 2 and insert them
			for (i=m_iInt; i>=iLowerh; i--){
				m_rpIntrados[i+2].x = m_rpIntrados[i].x;
				m_rpIntrados[i+2].y = m_rpIntrados[i].y;
			}

			m_rpIntrados[iLowerh+1].x = LinkSpline.m_Output[2].x;
			m_rpIntrados[iLowerh+1].y = LinkSpline.m_Output[2].y;
			m_rpIntrados[iLowerh].x   = LinkSpline.m_Output[1].x;
			m_rpIntrados[iLowerh].y   = LinkSpline.m_Output[1].y;

			m_iInt+=2;
		}
		if(m_LEFlapAngle>0.0){

			//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
			Intersect(m_rpExtrados[iUpperh-2], m_rpExtrados[iUpperh-1],
					  m_rpExtrados[iUpperh],   m_rpExtrados[iUpperh+1], &M);
			LinkSpline.InsertPoint(m_rpExtrados[iUpperh-1].x,m_rpExtrados[iUpperh-1].y);
			LinkSpline.InsertPoint(M.x, M.y);
			LinkSpline.InsertPoint(m_rpExtrados[iUpperh].x,m_rpExtrados[iUpperh].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			//retrieve point 1 and 2 and insert them
			for (i=m_iExt; i>=iUpperh; i--){
				m_rpExtrados[i+2].x = m_rpExtrados[i].x;
				m_rpExtrados[i+2].y = m_rpExtrados[i].y;
			}

			m_rpExtrados[iUpperh+1].x = LinkSpline.m_Output[2].x;
			m_rpExtrados[iUpperh+1].y = LinkSpline.m_Output[2].y;
			m_rpExtrados[iUpperh].x   = LinkSpline.m_Output[1].x;
			m_rpExtrados[iUpperh].y   = LinkSpline.m_Output[1].y;

			m_iExt+=2;
		}
		// trim upper surface first
		i1 = iUpperh;
		i2 = iUpperh-1;
		p=0;
		bIntersect = false;
		for (j=i2-1; j>0; j--){
			for (k=i1;k<m_iExt; k++){
				if(Intersect(m_rpExtrados[j], m_rpExtrados[j+1],
							 m_rpExtrados[k], m_rpExtrados[k+1], &M)){
					bIntersect = true;
					break;
				}
			}
			if(bIntersect) break;
		}

		if(bIntersect) {
			m_rpExtrados[j+1].x = M.x;
			m_rpExtrados[j+1].y = M.y;
			p=1;
			for (l=k+1;l<=m_iExt; l++){
				m_rpExtrados[j+1+p]  = m_rpExtrados[l];
				p++;
			}
			m_iExt = j+p;
		}

		// trim lower surface next
		i1 = iLowerh;
		i2 = iLowerh-1;
		p=0;
		bIntersect = false;
		for (j=i2-1; j>0; j--){
			for (k=i1;k<m_iInt; k++){
				if(Intersect(m_rpIntrados[j], m_rpIntrados[j+1],
							 m_rpIntrados[k], m_rpIntrados[k+1], &M)){
					bIntersect = true;
					break;
				}
			}
			if(bIntersect) break;
		}

		if(bIntersect) {
			m_rpIntrados[j+1].x = M.x;
			m_rpIntrados[j+1].y = M.y;
			p=1;
			for (l=k+1;l<=m_iInt; l++){
				m_rpIntrados[j+1+p]  = m_rpIntrados[l];
				p++;
			}
			m_iInt = j+p;
		}
	}

// Next Trailing Edge Flap

	if(m_bTEFlap){
		cosa = cos(m_TEFlapAngle*pi/180.0);
		sina = sin(m_TEFlapAngle*pi/180.0);
		//first convert xhinge and yhinge in absolute coordinates
		xh = m_TEXHinge/100.0;
		double ymin = GetBaseLowerY(xh);
		double ymax = GetBaseUpperY(xh);
		yh = ymin + m_TEYHinge/100.0 * (ymax-ymin);
		// insert a breakpoint at xhinge location, if there isn't one already
		int iUpperh = 0;
		int iLowerh = 0;
		for (i=0; i<m_iExt; i++){
			if(fabs(m_rpExtrados[i].x-xh)<0.001) {
				//then no need to add an extra point, just break
				iUpperh = i;
				break;
			}
			else if(m_rpExtrados[i].x>xh){//insert one
				for(j=m_iExt+1; j>i; j--){
					m_rpExtrados[j].x = m_rpExtrados[j-1].x;
					m_rpExtrados[j].y = m_rpExtrados[j-1].y;
				}
				m_rpExtrados[i].x = xh;
				m_rpExtrados[i].y = ymax;
				iUpperh = i;
				m_iExt++;
				break;
			}
		}
		
		for (i=0; i<m_iInt; i++){
			if(fabs(m_rpIntrados[i].x-xh)<0.001) {
				//then no need to add an Intra point, just break
				iLowerh = i;
				break;
			}
			else if(m_rpIntrados[i].x>xh){//insert one
				for(j=m_iInt+1; j>i; j--){
					m_rpIntrados[j].x = m_rpIntrados[j-1].x;
					m_rpIntrados[j].y = m_rpIntrados[j-1].y;
				}
				m_rpIntrados[i].x = xh;
				m_rpIntrados[i].y = ymin;
				iLowerh = i;
				m_iInt++;
				break;
			}
		}

		// rotate all points downstream of xh
		if(m_TEFlapAngle>0.0){
			//insert an extra point on intrados
			for (i=m_iInt+1; i>iLowerh; i--){
				m_rpIntrados[i] = m_rpIntrados[i-1];
			}
			m_iInt++;
			// extend to infinity last segments around hinge on flap internal side to make sure 
			// they intersect the spline on the other side
			m_rpIntrados[iLowerh+1].x += 30.0*(m_rpIntrados[iLowerh+1].x - m_rpIntrados[iLowerh+2].x);
			m_rpIntrados[iLowerh+1].y += 30.0*(m_rpIntrados[iLowerh+1].y - m_rpIntrados[iLowerh+2].y);
			m_rpIntrados[iLowerh].x   += 30.0*(m_rpIntrados[iLowerh].x   - m_rpIntrados[iLowerh-1].x);
			m_rpIntrados[iLowerh].y   += 30.0*(m_rpIntrados[iLowerh].y   - m_rpIntrados[iLowerh-1].y);
		}
		if(m_TEFlapAngle<0.0){
			//insert an extra point on extrados
			for (i=m_iExt+1; i>iUpperh; i--){
				m_rpExtrados[i] = m_rpExtrados[i-1];
			}
			m_iExt++;

			// extend to infinity last segments around hinge on flap internal side to make sure 
			// they intersect the spline on the other side
			m_rpExtrados[iUpperh+1].x += 30.0*(m_rpExtrados[iUpperh+1].x-m_rpExtrados[iUpperh+2].x);
			m_rpExtrados[iUpperh+1].y += 30.0*(m_rpExtrados[iUpperh+1].y-m_rpExtrados[iUpperh+2].y);
			m_rpExtrados[iUpperh].x   += 30.0 * (m_rpExtrados[iUpperh].x-m_rpExtrados[iUpperh-1].x);
			m_rpExtrados[iUpperh].y   += 30.0 * (m_rpExtrados[iUpperh].y-m_rpExtrados[iUpperh-1].y);
		}
		for (i=iUpperh+1; i<=m_iExt; i++){
			dx = m_rpExtrados[i].x-xh;
			dy = m_rpExtrados[i].y-yh;
			m_rpExtrados[i].x = xh + cosa * dx + sina * dy;
			m_rpExtrados[i].y = yh - sina * dx + cosa * dy;
		}
		for (i=iLowerh+1; i<=m_iInt; i++){
			dx = m_rpIntrados[i].x-xh;
			dy = m_rpIntrados[i].y-yh;
			m_rpIntrados[i].x = xh + cosa * dx + sina * dy;
			m_rpIntrados[i].y = yh - sina * dx + cosa * dy;
		}

		CSpline LinkSpline;
		LinkSpline.m_iRes = 4;
		LinkSpline.m_iDegree = 3;
		LinkSpline.m_iCtrlPoints = -1;

		if(m_TEFlapAngle<0.0){

			//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
			Intersect(m_rpIntrados[iLowerh-1], m_rpIntrados[iLowerh],
					  m_rpIntrados[iLowerh+1], m_rpIntrados[iLowerh+2], &M);
			LinkSpline.InsertPoint(m_rpIntrados[iLowerh].x,m_rpIntrados[iLowerh].y);
			LinkSpline.InsertPoint(M.x, M.y);
			LinkSpline.InsertPoint(m_rpIntrados[iLowerh+1].x,m_rpIntrados[iLowerh+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			//retrieve point 1 and 2 and insert them
			for (i=m_iInt; i>=iLowerh+1; i--){
				m_rpIntrados[i+2].x = m_rpIntrados[i].x;
				m_rpIntrados[i+2].y = m_rpIntrados[i].y;
			}

			m_rpIntrados[iLowerh+2].x = LinkSpline.m_Output[2].x;
			m_rpIntrados[iLowerh+2].y = LinkSpline.m_Output[2].y;
			m_rpIntrados[iLowerh+1].x = LinkSpline.m_Output[1].x;
			m_rpIntrados[iLowerh+1].y = LinkSpline.m_Output[1].y;

			m_iInt+=2;
		}
		else if(m_TEFlapAngle>0.0){

			//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
			Intersect(m_rpExtrados[iUpperh-1], m_rpExtrados[iUpperh],
					  m_rpExtrados[iUpperh+1], m_rpExtrados[iUpperh+2], &M);
			LinkSpline.InsertPoint(m_rpExtrados[iUpperh].x,m_rpExtrados[iUpperh].y);
			LinkSpline.InsertPoint(M.x, M.y);
			LinkSpline.InsertPoint(m_rpExtrados[iUpperh+1].x,m_rpExtrados[iUpperh+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			//retrieve point 1 and 2 and insert them
			for (i=m_iExt; i>=iUpperh+1; i--){
				m_rpExtrados[i+2].x = m_rpExtrados[i].x;
				m_rpExtrados[i+2].y = m_rpExtrados[i].y;
			}

			m_rpExtrados[iUpperh+2].x = LinkSpline.m_Output[2].x;
			m_rpExtrados[iUpperh+2].y = LinkSpline.m_Output[2].y;
			m_rpExtrados[iUpperh+1].x = LinkSpline.m_Output[1].x;
			m_rpExtrados[iUpperh+1].y = LinkSpline.m_Output[1].y;

			m_iExt+=2;
		}

		// trim upper surface first
		
		i1 = iUpperh;
		i2 = iUpperh+1;
		p=0;
		bIntersect = false;
		for (j=i2; j<m_iExt; j++){
			for (k=i1;k>0; k--){
				if(Intersect(m_rpExtrados[j], m_rpExtrados[j+1],
							m_rpExtrados[k], m_rpExtrados[k-1], &M)){
						bIntersect = true;
						break;
				}
			}
			if(bIntersect) break;
		}

		if(bIntersect) {
			m_rpExtrados[k] = M;
			p=1;
			for (l=j+1;l<=m_iExt; l++){
				m_rpExtrados[k+p]  = m_rpExtrados[l];
				p++;
			}
			m_iExt = k+p-1;
		}
	// trim lower surface next
		
		i1 = iLowerh;
		i2 = iLowerh+1;
		p=0;
		bIntersect = false;
		for (j=i2; j<m_iInt; j++){
			for (k=i1;k>0; k--){
				if(Intersect(m_rpIntrados[j], m_rpIntrados[j+1],
							m_rpIntrados[k], m_rpIntrados[k-1], &M)){
						bIntersect = true;
						break;
				}
			}
			if(bIntersect) break;
		}

		if(bIntersect) {
			m_rpIntrados[k] = M;
			p=1;
			for (l=j+1;l<=m_iInt; l++){
				m_rpIntrados[k+p]  = m_rpIntrados[l];
				p++;
			}
			m_iInt = k+p-1;
		}
	}
//And finally rebuild the current foil
	for (i=m_iExt; i>=0; i--){
		x[m_iExt-i] = m_rpExtrados[i].x;
		y[m_iExt-i] = m_rpExtrados[i].y;
	}
	for (i=1; i<=m_iInt; i++){
		x[m_iExt+i] = m_rpIntrados[i].x;
		y[m_iExt+i] = m_rpIntrados[i].y;
	}
	
	n = m_iExt + m_iInt + 1;

	InitFoil();//normals are set in InitXFoil() at calculation time
}


bool CFoil::CompMidLine(bool bParams)
{
	//  Calculates the foil's thickness and camber for the base foil

	double xt, yex, yin;
	if(bParams){
		m_fThickness  = 0.0;
		m_fCamber     = 0.0;
		m_fXCamber    = 0.0;
		m_fXThickness = 0.0;
	}

	m_rpMid[0].x    = 0.0;
	m_rpMid[0].y    = 0.0;
	m_rpMid[1000].x = 1.0;
	m_rpMid[1000].y = 0.0;
	double length = GetLength();
	double step = (m_rpExtrados[m_iExt].x-m_rpExtrados[0].x)/1000.0;

	for (int l=0; l<1000; l++){
		xt = m_rpExtrados[0].x+l*step;
		yex = GetUpperY((double)l*0.001);
		yin = GetLowerY((double)l*0.001);
		
		m_rpMid[l].x = xt;
		m_rpMid[l].y = (yex+yin)/2.0;
		if(bParams){
			if(fabs(yex-yin)>m_fThickness){
				m_fThickness  = fabs(yex-yin);
				m_fXThickness = xt;
			}
			if(fabs(m_rpMid[l].y)>fabs(m_fCamber)){
				m_fCamber  = m_rpMid[l].y;
				m_fXCamber = xt;
			}
		}
	}
	return true;
}





