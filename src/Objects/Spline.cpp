/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
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

#include "../Globals.h"
#include "Spline.h"
#include "math.h"


CSpline::CSpline()
{
	m_Style = 0;
	m_Width = 1;
	m_Color = QColor(70, 200, 120);

	m_iHighlight  = -10;
	m_iSelect     = -10;
	m_iCtrlPoints =  0;
	m_iKnots      =  0;
	m_iDegree     =  3;
	m_iRes        = 50;
        m_rViewRect.setRect(0,0,0,0);

	memset(m_knots, 0, sizeof(m_knots));
	memset(m_Input, 0, sizeof(m_Input));
	memset(m_Output, 0, sizeof(m_Output));
}


CSpline::~CSpline()
{
}


void CSpline::Copy(CSpline *pSpline)
{
	m_iCtrlPoints = pSpline->m_iCtrlPoints;
	m_iDegree     = pSpline->m_iDegree;
	m_iHighlight  = pSpline->m_iHighlight;
	m_iKnots      = pSpline->m_iKnots;
	m_iRes        = pSpline->m_iRes;
	m_iSelect     = pSpline->m_iSelect;

	memcpy(m_Input, &pSpline->m_Input, sizeof(m_Input));
	memcpy(m_Output, &pSpline->m_Output, sizeof(m_Output));
	memcpy(m_knots, &pSpline->m_knots, sizeof(m_knots));
	m_rViewRect.setCoords(pSpline->m_rViewRect.left(),  pSpline->m_rViewRect.top(),pSpline->m_rViewRect.right(),  pSpline->m_rViewRect.bottom());
}


void CSpline::CopySymetric(CSpline *pSpline)
{
	m_iCtrlPoints = pSpline->m_iCtrlPoints;
	m_iDegree     = pSpline->m_iDegree;
	m_iHighlight  = pSpline->m_iHighlight;
	m_iKnots      = pSpline->m_iKnots;
	m_iRes        = pSpline->m_iRes;
	m_iSelect     = pSpline->m_iSelect;
	for(int i=0; i<m_iCtrlPoints; i++)
	{
		m_Input[i].x =  pSpline->m_Input[i].x;
		m_Input[i].y = -pSpline->m_Input[i].y;
		m_Input[i].z =  pSpline->m_Input[i].z;
	}
	for(int i=0; i<m_iRes; i++)
	{
		m_Output[i].x =  pSpline->m_Output[i].x;
		m_Output[i].y = -pSpline->m_Output[i].y;
		m_Output[i].z =  pSpline->m_Output[i].z;
	}
	for(int i=0; i<m_iKnots; i++)
	{
		m_knots[i] = pSpline->m_knots[i];
	}
}


void CSpline::DrawCtrlPoints(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint pt;
	static int i, width;

	width  = 3;

	static QPen PointPen;
	static QBrush NoBrush(Qt::NoBrush);
	PointPen.setWidth(1);

	painter.setPen(PointPen);
	painter.setBrush(NoBrush);

	for (i=0; i<m_iCtrlPoints; i++)
	{
		pt.rx() = (int)( m_Input[i].x*scalex + Offset.x());
		pt.ry() = (int)(-m_Input[i].y*scaley + Offset.y());

		if (m_iSelect==i)
		{
			PointPen.setWidth(2);
			PointPen.setColor(QColor(0,0,150));
		}
		else if(m_iHighlight==i)
		{
			PointPen.setWidth(2);
			PointPen.setColor(QColor(255,0,0));
		}
		else
		{
			PointPen.setWidth(1);
			PointPen.setColor(m_Color);
		}
		painter.setPen(PointPen);
		painter.drawEllipse(pt.x()-width, pt.y()-width, 2*width, 2*width);
	}
	painter.restore();
}



void CSpline::DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint pt;
	static int i, width;
	static QPen OutPen;

	width = 2;

	OutPen.setColor(m_Color);
	OutPen.setStyle(Qt::SolidLine);
	OutPen.setWidth(1);
	painter.setPen(OutPen);

	for (i=0; i<m_iRes;i++)
	{
		pt.rx() = (int)( m_Output[i].x*scalex + Offset.x());
		pt.ry() = (int)(-m_Output[i].y*scaley + Offset.y());
		if(!m_rViewRect.contains(pt)) return;

		painter.drawRect(pt.x()-width, pt.y()-width, 2*width, 2*width);
	}

	painter.restore();
}


void CSpline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint From, To;
	static int k;
	static QPen SplinePen;

	SplinePen.setColor(m_Color);
	SplinePen.setStyle(GetStyle(m_Style));
	SplinePen.setWidth(m_Width);
	painter.setPen(SplinePen);


	if(m_iCtrlPoints>0)
	{ 
		From.rx() = (int)( m_Output[0].x * scalex + Offset.x());
		From.ry() = (int)(-m_Output[0].y * scaley + Offset.y());

		for(k=1; k<m_iRes;k++) 
		{
			To.rx() = (int)( m_Output[k].x * scalex + Offset.x());
			To.ry() = (int)(-m_Output[k].y * scaley + Offset.y());

			painter.drawLine(From, To);

			From = To;
		}
	}
	painter.restore();
}


void CSpline::Export(QTextStream &out, bool bExtrados)
{
	int k;
	QString strOut;

	if(bExtrados)
	{
		for (k=m_iRes-1;k>=0; k--)
		{
			strOut= QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
	else
	{
		for (k=1;k<m_iRes; k++)
		{
			strOut=QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
}


double CSpline::GetY(double const &x)
{
        static int i;
	static double y;

	if(x<=0.0 || x>=1.0) return 0.0;

        for (i=0; i<m_iRes-1; i++)
        {
		if (m_Output[i].x <m_Output[i+1].x  && 
                        m_Output[i].x <= x && x<=m_Output[i+1].x )
                {
			y = (m_Output[i].y 	+ (m_Output[i+1].y-m_Output[i].y)
								 /(m_Output[i+1].x-m_Output[i].x)*(x-m_Output[i].x));
			return y;
		}
	}
	return 0.0; //we'll have to do better...
}



bool CSpline::InsertPoint(double const &x, double const &y)
{
	static int j,k;
	if(m_iCtrlPoints>=SPLINECONTROLSIZE) return false;
	if (x>=0.0 && x<=1.0)
	{ 
		//No points yet
		if(m_iCtrlPoints<=0)
		{
			m_iCtrlPoints++;;
			m_Input[0].x = x;
			m_Input[0].y = y;
		}
		else 
		{
			if(x<m_Input[0].x)
			{// if we're the new minimum point
				for (j=m_iCtrlPoints; j>=0; j--)
				{
					m_Input[j] = m_Input[j-1];
				}
				m_Input[0].x = x;
				m_Input[0].y = y;
				m_iCtrlPoints++;
			}
			else{// if we're the new maximum point
				if(x>=m_Input[m_iCtrlPoints-1].x)
				{
					m_Input[m_iCtrlPoints].x = x;
					m_Input[m_iCtrlPoints].y = y;
					m_iCtrlPoints++;
				}
				else{// else if we're in between
					for (k=0; k<m_iCtrlPoints; k++)
					{
						if (x>m_Input[k].x && x<m_Input[k+1].x)
						{
							for (int j=m_iCtrlPoints; j>k+1; j--)
							{
								m_Input[j] = m_Input[j-1];
							}
							m_Input[k+1].x = x;
							m_Input[k+1].y = y;
							m_iCtrlPoints++;
							break;
						}
					}
				}
			}
		}
	}
	SplineKnots();
	return true;
}


int CSpline::IsControlPoint(CVector const &Real)
{
	static int k;
	for (k=0; k<m_iCtrlPoints; k++)
	{
                if(fabs(Real.x-m_Input[k].x)<0.005 && fabs(Real.y-m_Input[k].y)<0.005) return k;
	}
	return -10;
}


int CSpline::IsControlPoint(CVector const &Real, double const &ZoomFactor)
{
	static int k;
	for (k=0; k<m_iCtrlPoints; k++)
	{
                if (fabs(Real.x-m_Input[k].x)<0.006/ZoomFactor && fabs(Real.y-m_Input[k].y)<0.006/ZoomFactor) return k;
	}
	return -10;
}


int CSpline::IsControlPoint(double const &x, double const &y, double const &zx, double const &zy)
{
	static int k;
	for (k=0; k<m_iCtrlPoints; k++)
	{
                if(fabs((x-m_Input[k].x)/zx)<4.0 && fabs((y-m_Input[k].y)/zy)<4.0) return k;
	}
	return -10;
}


bool CSpline::RemovePoint(int const &k)
{
	static int j;
	if (k>0 && k<m_iCtrlPoints)
	{
		for (j=k; j<m_iCtrlPoints; j++)
		{
			m_Input[j] = m_Input[j+1];
		}
		m_iCtrlPoints--;
		SplineKnots();
		SplineCurve();
	}
	return true;
}


void CSpline::SetStyle(int style)
{
	m_Style = style;
}



void CSpline::SetWidth(int width)
{
	m_Width = width;
}



void CSpline::SetColor(QColor color)
{
	m_Color = color;
}



void CSpline::SetSplineParams(int style, int width, QColor color)
{
	m_Width = width;
	m_Style = style;
	m_Color = color;
}





double CSpline::SplineBlend(int const &i,  int const &p, double const &t)
{
/*	Calculate the blending value, this is done recursively.
	If the numerator and denominator are 0 the expression is 0.
	If the denominator is 0 the expression is 0 */
//
//	   i   is the control point's index
//	   p   is the spline's degree 	
//	   t   is the spline parameter
//

	static double pres = 1.e-10; //same for all the recursive calls...

	if (p == 0) 
	{
		if ((m_knots[i] <= t) && (t < m_knots[i+1]) )  return  1.0;
//		else if (fabs(m_knots[i]-m_knots[i+1])<pres)   return  0.0;
		else                                           return  0.0;
	} 
	else
	{
		if (fabs(m_knots[i+p] - m_knots[i])<pres && fabs(m_knots[i+p+1] - m_knots[i+1])<pres)
			return  0.0;

		else if (fabs(m_knots[i+p] - m_knots[i])<pres)
			return  (m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1, p-1, t);

		else if (fabs(m_knots[i+p+1]-m_knots[i+1])<pres)
			return  (t - m_knots[i])   / (m_knots[i+p] - m_knots[i])   * SplineBlend(i,   p-1, t);

		else
			return  (t - m_knots[i])   / (m_knots[i+p]-m_knots[i])	   * SplineBlend(i,   p-1, t) + 
					(m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1 ,p-1, t);
	}
}



void CSpline::SplineCurve()
{
	static double t, increment, b;
	static int i,j;

	if (m_iCtrlPoints>0)
	{
		t = 0;
		increment = 1.0/(double)(m_iRes - 1);
		
		for (j=0;j<m_iRes;j++)
		{
			m_Output[j].x = 0;
			m_Output[j].y = 0;

			for (i=0; i<m_iCtrlPoints; i++)
			{
				b = SplineBlend(i, m_iDegree, t);
				m_Output[j].x += m_Input[i].x * b;
				m_Output[j].y += m_Input[i].y * b;
			}
			t += increment;
		}
		m_Output[m_iRes-1] = m_Input[m_iCtrlPoints-1];
	}
}


void CSpline::SplineKnots()
{
	static double a,b;
	static int j, iDegree;

	iDegree = qMin(m_iDegree, m_iCtrlPoints);

	m_iKnots  = iDegree + m_iCtrlPoints + 1;
	for (j=0; j<m_iKnots; j++) 
	{
		if (j<iDegree+1)  m_knots[j] = 0.0;
		else 
		{
			if (j<m_iCtrlPoints) 
			{
				a = (double)(j-iDegree);
				b = (double)(m_iKnots-2*iDegree-1);
                                if(fabs(b)>0.0) m_knots[j] = a/b;
				else            m_knots[j] = 1.0; // added arcds
			}
			else m_knots[j] = 1.0;	
		}
	}
}




double Bernstein(int const &i, int const &n, double const &u)
{
	static int k, fi, fni;
	static double pui, pu1i1;

	fi  = 1;
	fni = 1;

	for(k=2;     k<=i; k++) fi  *=k;
	for(k=n-i+1; k<=n; k++) fni *=k;

	pui   = 1.0;
	for(k=0; k<i;   k++) pui*=u;

	pu1i1 = 1.0;
	for(k=0; k<n-i; k++) pu1i1*=1.0-u;

	return pui * pu1i1 * (double)fni/(double)fi;
}


double BezierBlend(int const &k, int const &n, double const&u)
{
	static int nn,kn,nkn;
	double blend=1.0;

	nn = n;
	kn = k;
	nkn = n - k;

	while (nn >= 1) 
	{
		blend *= nn;
		nn--;
		if (kn > 1)
		{
			blend /= (double)kn;
			kn--;
		}
		if (nkn > 1)
		{
			blend /= (double)nkn;
			nkn--;
		}
	}

	if (k > 0)   for(kn=0; kn<k;   kn++) blend *= u;
	if (n-k > 0) for(kn=0; kn<n-k; kn++) blend *= 1.0-u;

	return(blend);
}


