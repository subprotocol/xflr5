/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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


// Spline.cpp: implementation of the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Spline.h"
#include "math.h"
#include "../XDirect/XDirect.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CSpline::CSpline(){
	m_iHighlight  = -10;
	m_iSelect     = -10;
	m_iCtrlPoints = -1;
	m_iKnots      =  0;
	m_iDegree     =  3;
	m_iRes        = 50;
	m_rViewRect.SetRectEmpty();

	memset(m_knots, 0, sizeof(m_knots));
	memset(m_Input, 0, sizeof(m_Input));
	memset(m_Output, 0, sizeof(m_Output));
}


CSpline::~CSpline(){
}


bool CSpline::AddPoint(double x, double y){
	if(m_iCtrlPoints>=SPLINECONTROLSIZE) {
		return false;
	}
	m_iCtrlPoints++;
	m_Input[m_iCtrlPoints].x = x;
	m_Input[m_iCtrlPoints].y = y;
	return true;
}

bool CSpline::DrawControlPoint(CDC *pDC, int i, double scalex,  double scaley,
							   CPoint Offset, bool IsPrinting)
{
	int width;
	if (IsPrinting){
		width = 60;
		scaley=-scaley;
	}
	else{
		width = 3;

	}

	CPoint pt;
	pt.x = (int)( m_Input[i].x*scalex)+Offset.x;
	pt.y = (int)(-m_Input[i].y*scaley)+Offset.y;
	if(!m_rViewRect.PtInRect(pt)) return false;

	if (!IsPrinting && m_iSelect==i) {
		CPen SelectPen(PS_SOLID, 2, RGB(0,0,150));
		CPen *pOldPen = pDC->SelectObject(&SelectPen);
		pDC->Ellipse(pt.x-width,
					 pt.y-width,
					 pt.x+width,
					 pt.y+width);
		pDC->SelectObject(pOldPen);
	}
	else if(!IsPrinting && m_iHighlight==i) {
		CPen HighlightPen(PS_SOLID, 2, RGB(255,0,0));
		CPen *pOldPen = pDC->SelectObject(&HighlightPen);
		pDC->Ellipse(pt.x-width,
					 pt.y-width,
					 pt.x+width,
					 pt.y+width);
		pDC->SelectObject(pOldPen);
	}
	else pDC->Ellipse(pt.x-width,
					  pt.y-width,
					  pt.x+width,
					  pt.y+width);
	return true;
}


void CSpline::DrawCtrlPoints(CDC *pDC, double scx, double scy, CPoint Offset, bool IsPrinting){
	double scalex, scaley;
	CPoint pt;
	int width;
	if (IsPrinting){
		width = 60;
		scalex=scx;
		scaley=-scy;
	}
	else{
		width = 3;
		scalex=scx;
		scaley=-scy;
	}
	for (int i=0; i<=m_iCtrlPoints; i++){
		pt.x = (int)(m_Input[i].x*scalex+Offset.x);
		pt.y = (int)(m_Input[i].y*scaley+Offset.y);
		if(m_rViewRect.PtInRect(pt)){
			if (!IsPrinting && m_iSelect==i) {
				CPen SelectPen(PS_SOLID, 2, RGB(0,0,150));
				CPen *pOldPen = pDC->SelectObject(&SelectPen);
				pDC->Ellipse(pt.x-width, pt.y-width, pt.x+width, pt.y+width);
				pDC->SelectObject(pOldPen);
			}
			else if(!IsPrinting && m_iHighlight==i) {
				CPen HighlightPen(PS_SOLID, 2, RGB(255,0,0));
				CPen *pOldPen = pDC->SelectObject(&HighlightPen);
				pDC->Ellipse( pt.x-width, pt.y-width, pt.x+width, pt.y+width);
				pDC->SelectObject(pOldPen);
			}
			else pDC->Ellipse(pt.x-width, pt.y-width, pt.x+width, pt.y+width);
		}
	}	
}

void CSpline::DrawOutputPoint(CDC *pDC, int i,double scalex, double scaley, CPoint Offset, bool IsPrinting)
{
	int width;
	if (IsPrinting){
		width = 50;
		scaley=-scaley;
	}
	else{
		width = 2;
	}

	CPoint pt;
	pt.x = (int)( m_Output[i].x*scalex+Offset.x);
	pt.y = (int)(-m_Output[i].y*scaley+Offset.y);
	if(!m_rViewRect.PtInRect(pt)) return ;

	pDC->Rectangle(pt.x-width,
				   pt.y-width,
				   pt.x+width,
				   pt.y+width);
	return;
}


//DEL bool CSpline::DrawSpline(CDC *pDC, double scale, CPoint Offset, bool IsPrinting)
//DEL {
//DEL 	double scalex, scaley;
//DEL 	CPoint From, To;
//DEL 	int x,y;
//DEL 
//DEL 	if (IsPrinting){
//DEL 		scalex=scale;
//DEL 		scaley=-scale;
//DEL 	}
//DEL 	else{
//DEL 		scalex=scale;
//DEL 		scaley=scale;
//DEL 	}
//DEL 	if(m_iCtrlPoints>0){
//DEL 
//DEL 		From.x = (int)( m_Output[0].x*scalex+Offset.x);
//DEL 		From.y = (int)(-m_Output[0].y*scaley+Offset.y);
//DEL 		pDC->MoveTo(From);
//DEL 		for (int k=1; k<m_iRes;k++) {
//DEL 			To.x = (int)( m_Output[k].x*scalex+Offset.x);
//DEL 			To.y = (int)(-m_Output[k].y*scaley+Offset.y);
//DEL 			if(m_rViewRect.PtInRect(From) && m_rViewRect.PtInRect(To)){
//DEL 				pDC->LineTo(To);
//DEL 			}
//DEL 			else if(m_rViewRect.PtInRect(From) && !m_rViewRect.PtInRect(To)){
//DEL 				x = From.x;
//DEL 				y = From.y;
//DEL 				if(Intersect(x, y, m_rViewRect, From, To))	
//DEL 					pDC->LineTo(x,y);
//DEL 			}
//DEL 			else if(!m_rViewRect.PtInRect(From) && m_rViewRect.PtInRect(To)){
//DEL 				x = From.x;
//DEL 				y = From.y;
//DEL 				if(Intersect(x,y, m_rViewRect, From, To)){
//DEL 					pDC->MoveTo(x,y);
//DEL 					pDC->LineTo(To);
//DEL 				}
//DEL 			}
//DEL 			else 
//DEL 				pDC->MoveTo(To);
//DEL 			From = To;
//DEL 		}
//DEL 	}
//DEL 	return true;
//DEL }


bool CSpline::DrawSpline(CDC *pDC, double scx, double scy, 
						 CPoint Offset, bool IsPrinting)
{
	double scalex, scaley;
	CPoint From, To;
	int x,y;

	if (IsPrinting){
		scalex=scx;
		scaley=-scy;
	}
	else{
		scalex=scx;
		scaley=scy;
	}
	if(m_iCtrlPoints>0){
		From.x = (int)( m_Output[0].x*scalex+Offset.x);
		From.y = (int)(-m_Output[0].y*scaley+Offset.y);
		pDC->MoveTo(From);
		for(int k=1; k<m_iRes;k++) {
			To.x = (int)( m_Output[k].x*scalex+Offset.x);
			To.y = (int)(-m_Output[k].y*scaley+Offset.y);
			if(m_rViewRect.PtInRect(From) && m_rViewRect.PtInRect(To)){
				pDC->LineTo(To);
			}
			else if(m_rViewRect.PtInRect(From) && !m_rViewRect.PtInRect(To)){
				x = From.x;
				y = From.y;
				if(Intersect(x,y, m_rViewRect, From, To))	pDC->LineTo(x,y);
			}
			else if(!m_rViewRect.PtInRect(From) && m_rViewRect.PtInRect(To)){
				x = From.x;
				y = From.y;
				if(Intersect(x,y, m_rViewRect, From, To)){
					pDC->MoveTo(x,y);
					pDC->LineTo(To);
				}
			}
			else pDC->MoveTo(To);
			From = To;
		}
	}
	return true;
}

void CSpline::Export(CStdioFile *pFile, bool bExtrados)
{
	CString strOut;
	if(bExtrados){
		for (int k=m_iRes-1;k>=0; k--){
			strOut.Format(" %7.4f  %7.4f\n", m_Output[k].x, m_Output[k].y);
			pFile->WriteString(strOut);
		}
	}
	else {
		for (int k=1;k<=m_iRes-1; k++){
			strOut.Format(" %7.4f  %7.4f\n", m_Output[k].x, m_Output[k].y);
			pFile->WriteString(strOut);
		}
	}
}


double CSpline::GetY(double x)
{
	if(x<=0.0 || x>=1.0) return 0.0;
	double y;
	for (int i=0; i<m_iRes-1; i++){

		if (m_Output[i].x <m_Output[i+1].x  && 
			m_Output[i].x <= x && x<=m_Output[i+1].x ){
			y = (m_Output[i].y 	+ (m_Output[i+1].y-m_Output[i].y)
								 /(m_Output[i+1].x-m_Output[i].x)*(x-m_Output[i].x));
			return y;
		}
	}
	return 0.0; //we'll have to do better...
}



bool CSpline::InsertPoint(double x, double y)
{
	if(m_iCtrlPoints>=SPLINECONTROLSIZE) return false;
	if (x>=0.0 && x<=1.0){//No points yet
		if(m_iCtrlPoints<0){
			m_iCtrlPoints=0;
			m_Input[0].x = x;
			m_Input[0].y = y;
		}
		else{
			if(x<m_Input[0].x){// if we're the new minimum point
				m_iCtrlPoints++;
				for (int j=m_iCtrlPoints; j>=0; j--){
					m_Input[j] = m_Input[j-1];
				}
				m_Input[0].x = x;
				m_Input[0].y = y;
			}
			else{// if we're the new maximum point
				if(x>=m_Input[m_iCtrlPoints].x){
					m_iCtrlPoints++;
					m_Input[m_iCtrlPoints].x = x;
					m_Input[m_iCtrlPoints].y = y;
				}
				else{// else if we're in between
					for (int k=0; k<m_iCtrlPoints; k++){
						if (x>m_Input[k].x && x<m_Input[k+1].x){
							m_iCtrlPoints++;
							for (int j=m_iCtrlPoints; j>k+1; j--){
								m_Input[j] = m_Input[j-1];
							}
							m_Input[k+1].x = x;
							m_Input[k+1].y = y;
						}
					}
				}
			}
		}
	}
	return true;
}


int CSpline::IsControlPoint(CVector Real)
{
	for (int k=0; k<=m_iCtrlPoints; k++){
		if(fabs(Real.x-m_Input[k].x)<0.005 && fabs(Real.y-m_Input[k].y)<0.005) return k;
	}
	return -10;
}

int CSpline::IsControlPoint(CVector Real, double ZoomFactor)
{
	for (int k=0; k<=m_iCtrlPoints; k++){
		if (fabs(Real.x-m_Input[k].x)<0.006/ZoomFactor &&
			fabs(Real.y-m_Input[k].y)<0.006/ZoomFactor) return k;
	}
	return -10;
}

int CSpline::IsControlPoint(double x, double y, double zx, double zy)
{
	for (int k=0; k<=m_iCtrlPoints; k++){
		if(fabs((x-m_Input[k].x)/zx)<4.f && fabs((y-m_Input[k].y)/zy)<4.f) 
			return k;
	}
	return -10;
}





bool CSpline::RemovePoint(int k)
{
	if (k>0 && k<m_iCtrlPoints){
		for (int j=k; j<=m_iCtrlPoints; j++){
			m_Input[j] = m_Input[j+1];
		}
		m_iCtrlPoints--;
		SplineKnots();
		SplineCurve();
	}
	return true;
}



double CSpline::SplineBlend(int i,  int p, double t)
{
/*	Calculate the blending value, this is done recursively.
	If the numerator and denominator are 0 the expression is 0.
	If the denominator is 0 the expression is 0 */
	double value;
	double pres = (double)1.e-10;
	if (p == 0) {
		if ((m_knots[i] <= t) && (t < m_knots[i+1]) )
			value = 1.0;
		else if (fabs(m_knots[i]-m_knots[i+1])<pres)
			value = 1.0;
		else 
			value = 0.0;
	} else {
		if (fabs(m_knots[i+p]   - m_knots[i])  < pres &&
			fabs(m_knots[i+p+1] - m_knots[i+1])< pres) 
			value = 0.0;
		else if (fabs(m_knots[i+p] - m_knots[i]) <pres)
			value = (m_knots[i+p+1] - t) / (m_knots[i+p+1] - m_knots[i+1]) 
					* SplineBlend(i+1,p-1,t);
		else if (fabs(m_knots[i+p+1] - m_knots[i+1])<pres)
			value = (t - m_knots[i]) / (m_knots[i+p] - m_knots[i])
					* SplineBlend(i,p-1,t);
		else
			value = (t - m_knots[i]) / (m_knots[i+p] - m_knots[i])
					* SplineBlend(i,p-1,t) + 
		            (m_knots[i+p+1] - t) / (m_knots[i+p+1] - m_knots[i+1])
					* SplineBlend(i+1,p-1,t);
	}
	return value;
}



void CSpline::SplineCurve()
{
	if(m_iCtrlPoints>0){
		double t, increment;
		double b;
		t = 0;
		increment = 1/(double)(m_iRes - 1);
		
		for (int j=0;j<m_iRes-1;j++) {
			m_Output[j].x = 0;
			m_Output[j].y = 0;
			for (int i=0;i<=m_iCtrlPoints;i++) {
				b = SplineBlend(i, __min(m_iDegree, m_iCtrlPoints), t);
				m_Output[j].x += (double)(m_Input[i].x * b);
				m_Output[j].y += (double)(m_Input[i].y * b);
			}
			t += increment;
		}
		m_Output[m_iRes-1] = m_Input[m_iCtrlPoints];
	}
}


void CSpline::SplineKnots()
{
	int iDegree = __min(m_iDegree, m_iCtrlPoints);

	m_iKnots  = iDegree + m_iCtrlPoints + 1;
	for (int j=0; j<=m_iKnots; j++) {

		if (j <= iDegree) 
			m_knots[j] = 0.0;
		else {
			if (j < m_iKnots-iDegree) {
				double a = (double)(j-iDegree);
				double b = (double)(m_iKnots-2*iDegree);
				if(fabs(b)>0.0)
					m_knots[j]=a/b;
				else m_knots[j] = 1.0;	// added arcds
			}
			else m_knots[j] = 1.0;	
		}
	}
}

