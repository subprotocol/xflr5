/****************************************************************************

    Spline Foil Class
	Copyright (C) 2003-2010 Andre Deperrois adeperrois@xflr5.com
 
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



#include "math.h"
#include "SplineFoil.h"
#include "Foil.h"
#include "../globals.h"

/**
 * The public costructor.
 */
SplineFoil::SplineFoil()
{
	m_FoilStyle    = 0;
	m_FoilWidth    = 1;
	m_FoilColor    = QColor(119, 183, 83);
	m_OutPoints    = 0;
	m_bVisible     = true;
	m_bOutPoints   = false;
	m_bModified    = false;
	m_bCenterLine  = false;
	m_Intrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);
	m_Extrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);
	m_bSymetric = false;
}


/**
 * Overloaded constructor.
 */
SplineFoil::SplineFoil(SplineFoil *pSF)
{
	Copy(pSF);
}


/**
 * Sets the display style from the input parameters.
 * @param style the index of the style.
 * @param width the curve's width.
 * @param color te curve's color.
 */
void SplineFoil::SetCurveParams(int style, int width, QColor color)
{
	m_FoilStyle = style;
	m_FoilWidth = width;
	m_FoilColor = color;
	m_Intrados.SetSplineParams(style, width, color);
	m_Extrados.SetSplineParams(style, width, color);
}

/**
 * Initializes the SplineFoil object with stock data.
 */
void SplineFoil::InitSplineFoil()
{
	m_bModified   = false;
	m_strFoilName = QObject::tr("Spline Foil");
	
	m_Extrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);
	m_Extrados.m_CtrlPoint.clear();
	m_Extrados.InsertPoint(0.0 , 0.0);
	m_Extrados.InsertPoint(0.0 , 0.00774066);
	m_Extrados.InsertPoint(0.0306026, 0.0343829);
	m_Extrados.InsertPoint(0.289036 , 0.0504014);
	m_Extrados.InsertPoint(0.576000,  0.0350933);
	m_Extrados.InsertPoint(0.736139 , 0.0269428);
	m_Extrados.InsertPoint(1. , 0.);

	m_Intrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);
	m_Intrados.m_CtrlPoint.clear();
	m_Intrados.InsertPoint(0. , 0.);
	m_Intrados.InsertPoint(0. , -0.00774066);
	m_Intrados.InsertPoint(0.0306026, -0.0343829);
	m_Intrados.InsertPoint(0.289036 , -0.0504014);
	m_Intrados.InsertPoint(0.576000,  -0.0350933);
	m_Intrados.InsertPoint(0.736139 , -0.0269428);
	m_Intrados.InsertPoint(1. , 0.);

	CompMidLine();
	m_OutPoints = m_Extrados.m_iRes + m_Intrados.m_iRes;

}

/**
 * Calculates the SplineFoil's mid-camber line and stores the resutls in the memeber array.
 * @return
 */
void SplineFoil::CompMidLine()
{
	double x, yex, yin;
	m_fThickness = 0.0;
	m_fCamber    = 0.0;
	m_fxCambMax  = 0.0;
	m_fxThickMax = 0.0;

	m_rpMid[0].x   = 0.0;
	m_rpMid[0].y   = 0.0;

	double step = 1.0/(double)MIDPOINTCOUNT;

	for (int k=0; k<MIDPOINTCOUNT; k++)
	{
		x = k*step;
		yex = m_Extrados.GetY(x);
		yin = m_Intrados.GetY(x);
		m_rpMid[k].x = x;
		m_rpMid[k].y = (yex+yin)/2.0;
		if(qAbs(yex-yin)>m_fThickness)
		{
			m_fThickness = qAbs(yex-yin);
			m_fxThickMax = x;
		}
		if(qAbs(m_rpMid[k].y)>qAbs(m_fCamber))
		{
			m_fCamber = m_rpMid[k].y;
			m_fxCambMax = x;
		}	
	}
	m_rpMid[MIDPOINTCOUNT-1].x = 1.0;
	m_rpMid[MIDPOINTCOUNT-1].y = 0.0;
}


/**
 * Initializes this SplineFoil object with the data from another.
 * @param pSF a pointer to the source SplineFoil object.
 */
void SplineFoil::Copy(SplineFoil* pSF)
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
	m_bSymetric  = pSF->m_bSymetric;
}


/**
 * Exports the current SplineFoil to a Foil object.
 * @param pFoil a pointer to the existing Foil object to be loaded with the SplineFoil points.
 */
void SplineFoil::ExportToBuffer(void *pFoilPtr)
{
	if(!pFoilPtr) return;

	Foil *pFoil = (Foil*)pFoilPtr;
	int i;
	int j = m_Extrados.m_iRes;
	int k = m_Intrados.m_iRes;
	for (i=0; i<m_Extrados.m_iRes; i++)
	{
		pFoil->x[j-i-1] = m_Extrados.m_Output[i].x;
		pFoil->y[j-i-1] = m_Extrados.m_Output[i].y;
	}
	for (i=1; i<m_Intrados.m_iRes;i++)
	{
		pFoil->x[i+j-1] = m_Intrados.m_Output[i].x;
		pFoil->y[i+j-1] = m_Intrados.m_Output[i].y;
	}
	pFoil->n = j+k-1;
	memcpy(pFoil->xb, pFoil->x, sizeof(pFoil->x));
	memcpy(pFoil->yb, pFoil->y, sizeof(pFoil->y));
	pFoil->nb = pFoil->n;
	pFoil->m_FoilName = m_strFoilName;
}

/**
 * Exports the SplineFoil's output points to a text file.
 * @param out the QTextStream to which the output is directed.
 */
void SplineFoil::ExportToFile(QTextStream &out)
{
	m_Extrados.Export(out, true);
	m_Intrados.Export(out, false);
}


/**
 * Loads or saves the data of this SplineFoil to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool SplineFoil::Serialize(QDataStream &ar, bool bIsStoring)
{
	float f,x,y;
	int ArchiveFormat,k,n;

	if(bIsStoring)
	{
		//no storage in.wpa format anymore
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat < 100000 || ArchiveFormat > 110000) return false;
		ReadCString(ar, m_strFoilName);
		m_strFoilName = QObject::tr("Spline Foil");
		ReadCOLORREF(ar, m_FoilColor);
		ar >>m_FoilStyle >> m_FoilWidth;

		m_Extrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);
		m_Intrados.SetSplineParams(m_FoilStyle, m_FoilWidth, m_FoilColor);

		ar >> n;// m_Extrados.m_iCtrlPoints;
		m_Extrados.m_CtrlPoint.clear();
		ar >> m_Extrados.m_iDegree;

		for (k=0; k<n; k++)
		{
			ar >> x;
			ar >> y;
			m_Extrados.m_CtrlPoint.append(CVector(x,y,0.0));
		}
		if(ArchiveFormat<100306)
		{
			ar >> f;
			ar >> f;
		}

		ar >> n;// m_Intrados.m_iCtrlPoints;
		m_Intrados.m_CtrlPoint.clear();
		ar >> m_Intrados.m_iDegree;

		for (k=0; k<n; k++)
		{
			ar >> x;
			ar >> y;
			m_Intrados.m_CtrlPoint.append(CVector(x,y,0.0));
		}
		if(ArchiveFormat<100306)
		{
			ar >> f;
			ar >> f;
		}
		ar >> m_Extrados.m_iRes >> m_Intrados.m_iRes;

		ar >> k;
		if(k!=0 && k!=1) return false;
		if(k) m_bVisible = true; else m_bVisible = false;

		ar >> k;
		if(k!=0 && k!=1) return false;

		if(k) m_bOutPoints = true; else m_bOutPoints = false;

		ar >> k;
		if(k!=0 && k!=1) return false;

		if(k) m_bCenterLine = true; else m_bCenterLine = false;


		m_Extrados.SplineKnots();
		m_Extrados.SplineCurve();
		m_Intrados.SplineKnots();
		m_Intrados.SplineCurve();

		UpdateSplineFoil();

	}
	m_bModified = false;
	return true;
}


/**
 * Loads or saves the data of this SplineFoil to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool SplineFoil::SerializeXFL(QDataStream &ar, bool bIsStoring)
{
	double dble, x,y;
	int ArchiveFormat,k,n;

	if(bIsStoring)
	{
		ar << 200001;

		ar << m_strFoilName;
		ar << m_FoilColor;
		ar << m_FoilStyle << m_FoilWidth;
		ar << m_bVisible << m_bCenterLine << m_bOutPoints;

		ar << m_Extrados.m_iDegree << m_Intrados.m_iDegree;
		ar << m_Extrados.m_iRes << m_Intrados.m_iRes;

		ar << m_Extrados.m_CtrlPoint.size();
		for (k=0; k<m_Extrados.m_CtrlPoint.size();k++)
		{
			ar << m_Extrados.m_CtrlPoint[k].x << m_Extrados.m_CtrlPoint[k].y;
		}

		ar << m_Intrados.m_CtrlPoint.size();
		for (k=0; k<m_Intrados.m_CtrlPoint.size();k++)
		{
			ar << m_Intrados.m_CtrlPoint[k].x << m_Intrados.m_CtrlPoint[k].y;
		}

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<10; i++) ar << i;
		for (int i=0; i<10; i++) ar << (double)i;
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat < 200000 || ArchiveFormat > 210000) return false;

		ar >> m_strFoilName;
		ar >> m_FoilColor;
		ar >> m_FoilStyle >> m_FoilWidth;
		ar >> m_bVisible >> m_bCenterLine >> m_bOutPoints;

		ar >> m_Extrados.m_iDegree >> m_Intrados.m_iDegree;
		ar >> m_Extrados.m_iRes >> m_Intrados.m_iRes;

		m_Extrados.m_CtrlPoint.clear();
		ar >> n;
		for (k=0; k<n;k++)
		{
			ar >> x >> y;
			m_Extrados.m_CtrlPoint.append(CVector(x, y, 0.0));
		}

		m_Intrados.m_CtrlPoint.clear();
		ar >> n;
		for (k=0; k<n;k++)
		{
			ar >> x >> y;
			m_Intrados.m_CtrlPoint.append(CVector(x, y, 0.0));
		}

		// space allocation
		for (int i=0; i<10; i++) ar >> k;
		for (int i=0; i<10; i++) ar >> dble;

		m_Extrados.SplineKnots();
		m_Extrados.SplineCurve();
		m_Intrados.SplineKnots();
		m_Intrados.SplineCurve();

		UpdateSplineFoil();

	}
	m_bModified = false;
	return true;
}


/**
 * Updates the mid camber line and the number of points after a modification.
 */
void SplineFoil::UpdateSplineFoil()
{
	CompMidLine();
	m_OutPoints = m_Extrados.m_iRes + m_Intrados.m_iRes;
}


/**
 * Draws the SplineFoil's control points.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale of the view in the x direction
 * @param scaley the scale of the view in the y direction
 * @param Offset the postion of the SplineFoil's leading edge point
 */
void SplineFoil::DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	m_Extrados.DrawCtrlPoints(painter, scalex, scaley, Offset);
	m_Intrados.DrawCtrlPoints(painter, scalex, scaley, Offset);
}

/**
 * Draws the SplineFoil's output points.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale of the view in the x direction
 * @param scaley the scale of the view in the y direction
 * @param Offset the postion of the SplineFoil's leading edge point
 */
void SplineFoil::DrawOutPoints(QPainter & painter, double scalex, double scaley, QPoint Offset)
{
	m_Extrados.DrawOutputPoints(painter, scalex, scaley, Offset);
	m_Intrados.DrawOutputPoints(painter, scalex, scaley, Offset);
}

/**
 * Draws the SplineFoil's curves.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale of the view in the x direction
 * @param scaley the scale of the view in the y direction
 * @param Offset the postion of the SplineFoil's leading edge point
 */
void SplineFoil::DrawFoil(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	m_Extrados.DrawSpline(painter, scalex, scaley, Offset);
	m_Intrados.DrawSpline(painter, scalex, scaley, Offset);
}


/**
 * Draws the SplineFoil's mid camber line.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale of the view in the x direction
 * @param scaley the scale of the view in the y direction
 * @param Offset the postion of the SplineFoil's leading edge point
 */
void SplineFoil::DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	painter.save();
	int k;
	QPoint From, To;
	
	QPen MidPen(m_FoilColor);
	MidPen.setStyle(Qt::DashLine);
	MidPen.setWidth(1);
	painter.setPen(MidPen);

	From = QPoint((int)(m_rpMid[0].x*scalex) + Offset.x(), (int)(-m_rpMid[0].y*scaley) + Offset.y());

	for (k=1; k<MIDPOINTCOUNT; k+=10)
	{
		To.rx() = (int)( m_rpMid[k].x*scalex) + Offset.x();
		To.ry() = (int)(-m_rpMid[k].y*scaley) + Offset.y();
		painter.drawLine(From, To);
		From = To;
	}

	painter.drawLine(From, QPoint((int)(m_rpMid[MIDPOINTCOUNT-1].x*scalex) + Offset.x(), (int)(-m_rpMid[MIDPOINTCOUNT-1].y*scaley) + Offset.y()));
	painter.restore();
}
