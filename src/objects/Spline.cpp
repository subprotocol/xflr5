/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003-2013 Andre Deperrois adeperrois@xflr5.com

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

#include "../globals.h"
#include "Spline.h"
#include "math.h"


/**
*The public constructor
*/
Spline::Spline()
{
	m_Style = 0;
	m_Width = 1;
	m_Color = QColor(70, 200, 120);

	m_CtrlPoint.clear();
#if QT_VERSION >= 0x040700
    m_CtrlPoint.reserve(50);
#endif

	m_knot.clear();
#if QT_VERSION >= 0x040700
    m_knot.reserve(100);
#endif

	m_iHighlight  = -10;
	m_iSelect     = -10;
	m_iDegree     =  3;
	m_iRes        = 79;

	m_PtWeight = 1.0;

	memset(m_Output, 0, sizeof(m_Output));
}

/**
* Copies the data from an existing Spline.
* @param pSpline a pointer to an existing Spline object.
*/
void Spline::Copy(Spline *pSpline)
{
	if(!pSpline) return;
	
	m_CtrlPoint.clear();
	for(int ic=0; ic<pSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pSpline->m_CtrlPoint.at(ic));
	}

	m_iDegree     = pSpline->m_iDegree;
	m_iHighlight  = pSpline->m_iHighlight;
	m_iRes        = pSpline->m_iRes;
	m_iSelect     = pSpline->m_iSelect;

	SplineKnots();
	SplineCurve();
}


/**
* Creates a symetric spline w.r.t. the axis y=0, from an existing Spline.
* @param pSpline a pointer to an existing Spline object.
*/
void Spline::CopySymetric(Spline *pSpline)
{
	if(!pSpline) return;
	
	m_CtrlPoint.clear();
	for(int ic=0; ic<pSpline->m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint.append(pSpline->m_CtrlPoint.at(ic));
		m_CtrlPoint[ic].y = -m_CtrlPoint[ic].y;
	}

	m_iDegree     = pSpline->m_iDegree;
	m_iHighlight  = pSpline->m_iHighlight;
	m_iRes        = pSpline->m_iRes;
	m_iSelect     = pSpline->m_iSelect;
	for(int i=0; i<m_iRes; i++)
	{
		m_Output[i].x =  pSpline->m_Output[i].x;
		m_Output[i].y = -pSpline->m_Output[i].y;
		m_Output[i].z =  pSpline->m_Output[i].z;
	}

	m_knot.clear();
	for(int i=0; i<pSpline->m_knot.size(); i++)
	{
		m_knot.append(pSpline->m_knot[i]);
	}
}

/**
*Draws the control points on a QPainter. @todo separate GUI and object for polymorphism.
*/
void Spline::DrawCtrlPoints(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset)
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

	for (i=0; i<m_CtrlPoint.size(); i++)
	{
		pt.rx() = (int)( m_CtrlPoint[i].x*scalex + Offset.x());
		pt.ry() = (int)(-m_CtrlPoint[i].y*scaley + Offset.y());

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



/**
*Draws the output points on a QPainter. @todo separate GUI and object for polymorphism.
*/
void Spline::DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
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

		painter.drawRect(pt.x()-width, pt.y()-width, 2*width, 2*width);
	}

	painter.restore();
}


/**
*Draws the spline curve on a QPainter. @todo separate GUI and object for polymorphism.
*/
void Spline::DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	painter.save();

	static QPoint From, To;
	static int k;
	static QPen SplinePen;

	SplinePen.setColor(m_Color);
	SplinePen.setStyle(getStyle(m_Style));
	SplinePen.setWidth(m_Width);
	painter.setPen(SplinePen);


	if(m_CtrlPoint.size()>=3)
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

/**
* Exports the spline output points to a text file
* @param out the stream to which the data is directed
* @param bExtrados true if the data should be written from end to beginning, false if written from beginning to end. This is the order required by foil files.
*/
void Spline::Export(QTextStream &out, bool bExtrados)
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

/** Returns the y-coordinate of the spline at the specified x-coordinate
 *@param x the x-coordinate
 *@return the y-value
 */
double Spline::GetY(double const &x)
{
	static int i;
	static double y;

	if(x<=0.0 || x>=1.0) return 0.0;

	for (i=0; i<m_iRes-1; i++)
	{
		if (m_Output[i].x <m_Output[i+1].x  && m_Output[i].x <= x && x<=m_Output[i+1].x )
		{
			y = (m_Output[i].y 	+ (m_Output[i+1].y-m_Output[i].y)
			/(m_Output[i+1].x-m_Output[i].x)*(x-m_Output[i].x));
			return y;
		}
	}
	return 0.0;
}


/**
* Inserts a new point in the array of control points, using crescending x values as the key for insertion
* @param x the x-coordinate of the point to insert
* @param y the y-coordinate of the point to insert
* @return true unless the max number of points has been reached
*/
bool Spline::InsertPoint(double const &x, double const &y)
{
	static int k;

	if (x>=0.0 && x<=1.0)
	{ 
		//No points yet
		if(m_CtrlPoint.size()==0)
		{
			m_CtrlPoint.append(CVector(x,y,0.0));
		}
		else 
		{
			if(x<m_CtrlPoint.first().x)
			{
				// if we're the new minimum point
				m_CtrlPoint.prepend(CVector(x,y,0.0));
				m_iSelect = 0;
			}
			else if(x>=m_CtrlPoint.last().x)
			{
				// if we're the new maximum point
				m_CtrlPoint.append(CVector(x,y,0.0));
				m_iSelect = m_CtrlPoint.size();
			}
			else
			{
				// else if we're in between
				for (k=0; k<m_CtrlPoint.size()-1; k++)
				{
					if (x>=m_CtrlPoint[k].x && x<m_CtrlPoint[k+1].x)
					{
						m_CtrlPoint.insert(k+1, CVector(x,y,0.0));
						m_iSelect = k+1;
						break;
					}
				}
			}
		}
	}
	SplineKnots();
	SplineCurve();
	return true;
}

/**
* Checks if an input point matches with a control point
*@param Real the input point to compare with the control points
*@return the index of the first control point which matches, or -10 if none matches.
*/
int Spline::IsControlPoint(CVector const &Real)
{
	static int k;
	for (k=0; k<m_CtrlPoint.size(); k++)
	{
		if(qAbs(Real.x-m_CtrlPoint[k].x)<0.005 && qAbs(Real.y-m_CtrlPoint[k].y)<0.005) return k;
	}
	return -10;
}


/**
* Checks if an input point matches with a control point
*@param Real the input point to compare with the control points
*@param ZoomFactor the scaling factor to withdraw from the input point @todo withdrawal to be performed from within the calling function.
*@return the index of the first control point which matches, or -10 if none matches.
*/
int Spline::IsControlPoint(CVector const &Real, double const &ZoomFactor)
{
	static int k;
	for (k=0; k<m_CtrlPoint.size(); k++)
	{
		if (qAbs(Real.x-m_CtrlPoint[k].x)<0.006/ZoomFactor && qAbs(Real.y-m_CtrlPoint[k].y)<0.006/ZoomFactor) return k;
	}
	return -10;
}



/**
* Checks if an input point matches with a control point
*@param Real the input point to compare with the control points
*@param zx the scaling factor of the x-scale, to withdraw from the input point @todo withdrawal to be performed from within the calling function.
*@param zy the scaling factor of the y-scale, to withdraw from the input point @todo withdrawal to be performed from within the calling function.
*@return the index of the first control point which matches, or -10 if none matches.
*/
int Spline::IsControlPoint(double const &x, double const &y, double const &zx, double const &zy)
{
	static int k;
	for (k=0; k<m_CtrlPoint.size(); k++)
	{
		if(qAbs((x-m_CtrlPoint[k].x)/zx)<4.0 && qAbs((y-m_CtrlPoint[k].y)/zy)<4.0) return k;
	}
	return -10;
}

/**
* Removes a point from the array of control points, only if the remaining number of points is strictly greater than the spline's degree
*@param k the index of the control point to remove in the array
*@return false if the remaining number of points is equal or less than the spline's degree, true otherwise.
*/
bool Spline::RemovePoint(int const &k)
{
	if(m_CtrlPoint.size()<=m_iDegree+1) return false; // no less...

	if (k>0 && k<m_CtrlPoint.size())
	{
		m_CtrlPoint.removeAt(k);
		SplineKnots();
		SplineCurve();
	}
	return true;
}


/** Sets the spline's drawing style 
*@param style the index of the spline's style
*/
void Spline::SetStyle(int style)
{
	m_Style = style;
}


/** Sets the spline's drawing width 
*@param width the width of the spline
*/
void Spline::SetWidth(int width)
{
	m_Width = width;
}


/**
*Sets the spline drawing color
*@param color the spline's color
*/
void Spline::SetColor(QColor color)
{
	m_Color = color;
}


/**
*Sets the spline's style, width and color
*@param style the spline's drawing style
*@param width the width of the spline
*@param color the spline's color
*/
void Spline::SetSplineParams(int style, int width, QColor color)
{
	m_Width = width;
	m_Style = style;
	m_Color = color;
}


/**	
 * Calculates the blending value. This is done recursively.
   If the numerator and denominator are 0 the expression is 0.
   If the denominator is 0 the expression is 0 
 *
 * @param  i   the control point's index
 * @param  p   the spline's degree 	
 * @param  t   the spline parameter
 * @return the blending value for this control point and the pair of degree and parameter values.
*/
double Spline::SplineBlend(int const &i,  int const &p, double const &t)
{

	static double pres = 1.e-6; //same for all the recursive calls...

	if(i+p+1>=m_knot.size())
	{
//		qDebug()<<"Error here";
		return 0.0;
	}

	if (p == 0) 
	{
		if ((m_knot[i] <= t) && (t < m_knot[i+1]) )  return  1.0;
//		else if (qAbs(m_knot[i]-m_knot[i+1])<pres)   return  0.0;
		else                                         return  0.0;
	} 
	else
	{
		if (qAbs(m_knot[i+p] - m_knot[i])<pres && qAbs(m_knot[i+p+1] - m_knot[i+1])<pres)
			return  0.0;

		else if (qAbs(m_knot[i+p] - m_knot[i])<pres)
			return  (m_knot[i+p+1]-t) / (m_knot[i+p+1]-m_knot[i+1]) * SplineBlend(i+1, p-1, t);

		else if (qAbs(m_knot[i+p+1]-m_knot[i+1])<pres)
			return  (t - m_knot[i])   / (m_knot[i+p] - m_knot[i])   * SplineBlend(i,   p-1, t);

		else
			return  (t - m_knot[i])   / (m_knot[i+p]-m_knot[i])	    * SplineBlend(i,   p-1, t) +
					(m_knot[i+p+1]-t) / (m_knot[i+p+1]-m_knot[i+1]) * SplineBlend(i+1 ,p-1, t);
	}
}


/**
* Calculates the spline's output points 
*/
void Spline::SplineCurve()
{
	static double t, increment, b, w;
	static int i,j;

	if (m_CtrlPoint.size()>=3)
	{
		t = 0;
		increment = 1.0/(double)(m_iRes - 1);
		
		for (j=0;j<m_iRes;j++)
		{
			m_Output[j].x = 0;
			m_Output[j].y = 0;
			w=0.0;

			for (i=0; i<m_CtrlPoint.size(); i++)
			{
				b = SplineBlend(i, m_iDegree, t);
				if(i!=0 && i!=m_CtrlPoint.size()-1) b *= m_PtWeight;

				m_Output[j].x += m_CtrlPoint[i].x * b;
				m_Output[j].y += m_CtrlPoint[i].y * b;
				w += b;
			}
			m_Output[j] *= 1.0/w;

			t += increment;
		}

		m_Output[m_iRes-1] = m_CtrlPoint.last();
	}
}

/**
*Generates an array of standard knot values for this spline.
*/
void Spline::SplineKnots()
{
	static double a,b;
	static int j, iDegree;

	iDegree = qMin(m_iDegree, m_CtrlPoint.size());

	double nKnots  = iDegree + m_CtrlPoint.size() + 1;
	m_knot.clear();

	for (j=0; j<nKnots; j++)
	{
		if (j<iDegree+1)  m_knot.append(0.0);
		else 
		{
			if(j<m_CtrlPoint.size())
			{
				a = (double)(j-iDegree);
				b = (double)(nKnots-2*iDegree-1);
				if(qAbs(b)>0.0) m_knot.append(a/b);
				else            m_knot.append(1.0);
			}
			else m_knot.append(1.0);
		}
	}
}




