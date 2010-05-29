/****************************************************************************

	Splined Curve Foil class
    Copyright (C) 2004 Andre Deperrois XFLR5@yahoo.com
 
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
#include "Pf.h"
#include "math.h"
#include "Spline.h"


CPF::CPF()
{
	m_FoilStyle    = 0;
	m_FoilWidth    = 1;
	m_FoilColor    = QColor(50,180,130);
	m_bOutPoints   = false;
	m_OutPoints    = 0;
	m_bVisible     = true;
	m_bOutPoints   = false;
	m_bCenterLine  = false;
	memset(m_rpMid, 0, sizeof(m_rpMid));
}



bool CPF::CompMidLine(bool first)
{
	double x, yex, yin, step;
	int l;
	m_fThickness = 0.0;
	m_fCamber    = 0.0;
	m_fxCambMax  = 0.0;
	m_fxThickMax = 0.0;

	m_rpMid[0].x   = 0.0;
	m_rpMid[0].y   = 0.0;
	m_rpMid[100].x = 1.0;
	m_rpMid[100].y = 0.0;

	
	step = 0.001;
	for (l=0; l<=1000; l++)
	{
		x = l*step;
		if(first)
		{
			yex = m_Extrados.GetY(x);
			yin = m_Intrados.GetY(x);
		}
		else
		{
			yex = m_Extrados.m_Outy[l];
			yin = m_Intrados.m_Outy[l];
		}
		m_rpMid[l].x = x;
		m_rpMid[l].y = (yex+yin)/2.0;

		if(fabs(yex-yin)>m_fThickness)
		{
			m_fThickness = fabs(yex-yin);
			m_fxThickMax = x;
		}
		if(fabs(m_rpMid[l].y)>fabs(m_fCamber))
		{
			m_fCamber = m_rpMid[l].y;
			m_fxCambMax = x;
		}
	}
	return true;
}


void CPF::Copy(CPF* pPF)
{
	memcpy(&m_Extrados, &pPF->m_Extrados, sizeof(m_Extrados));
	memcpy(&m_Intrados, &pPF->m_Intrados, sizeof(m_Intrados));
	memcpy(&m_rpMid, pPF->m_rpMid, sizeof(m_rpMid));
	m_fCamber    = pPF->m_fCamber;
	m_fThickness = pPF->m_fThickness;
	m_fxCambMax  = pPF->m_fxCambMax;
	m_fxThickMax = pPF->m_fxThickMax;
	m_FoilColor  = pPF->m_FoilColor;
	m_FoilStyle  = pPF->m_FoilStyle;
	m_FoilWidth  = pPF->m_FoilWidth;
}


void CPF::DrawFoil(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	m_Extrados.DrawSplines(painter, scalex, scaley, Offset);
	m_Intrados.DrawSplines(painter, scalex, scaley, Offset);
	CompMidLine();
}


void CPF::DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	painter.save();
	QPoint From, To;
	From.rx() = (int)( m_rpMid[0].x*scalex) + Offset.x();
	From.ry() = (int)(-m_rpMid[0].y*scaley) + Offset.y();

	for (int k=1; k<=100; k++)
	{
		To.rx() = (int)( m_rpMid[k*10].x*scalex)+Offset.x();
		To.ry() = (int)(-m_rpMid[k*10].y*scaley)+Offset.y();
		painter.drawLine(From, To);
		From = To;
	}
	painter.restore();
}



void CPF::DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	m_Extrados.DrawControlPoints(painter, scalex, scaley, Offset);
	m_Intrados.DrawControlPoints(painter, scalex, scaley, Offset);
	m_Extrados.DrawRearPoint(painter, scalex, scaley, Offset);
	m_Intrados.DrawRearPoint(painter, scalex, scaley, Offset);

}



void CPF::ExportToBuffer(CFoil *pFoil)
{
	m_Extrados.ExportToBuffer(pFoil, true);
	m_Intrados.ExportToBuffer(pFoil, false);
	memcpy(pFoil->xb, pFoil->x, sizeof(pFoil->x));
	memcpy(pFoil->yb, pFoil->y, sizeof(pFoil->y));
	pFoil->nb = pFoil->n;
}


void CPF::ExportToFile(QTextStream &out)
{
	m_Extrados.ExportToFile(out,true);
	m_Intrados.ExportToFile(out,false);
}

bool CPF::InitSplinedFoil()
{
	m_bModified   = false;
	m_strFoilName = QObject::tr("Splined Points Foil");
	m_Extrados.m_iPoints = 0;
	m_Extrados.InsertPoint(1.00,  0.0000);
	m_Extrados.InsertPoint(0.887, 0.0173);
	m_Extrados.InsertPoint(0.657, 0.0438);
	m_Extrados.InsertPoint(0.383, 0.0612);
	m_Extrados.InsertPoint(0.247, 0.0603);
	m_Extrados.InsertPoint(0.129, 0.0484);
	m_Extrados.InsertPoint(0.041, 0.0279);
	m_Extrados.InsertPoint(0.000, 0.0000);
	m_Extrados.m_Slope[0].x  = 0.0;
	m_Extrados.m_Slope[0].y  = 1.0;
	m_Extrados.m_RearPoint.x = 1.06119;
	m_Extrados.m_RearPoint.y = -0.00948;

	m_Intrados.m_iPoints = 0;
	m_Intrados.InsertPoint(1.000,  0.0000);
	m_Intrados.InsertPoint(0.032, -0.0196);
	m_Intrados.InsertPoint(0.119, -0.0300);
	m_Intrados.InsertPoint(0.423, -0.0254);
	m_Intrados.InsertPoint(0.837, -0.0023);
	m_Intrados.InsertPoint(0.941, -0.0009);
	m_Intrados.InsertPoint(0.000,  0.0000);
	m_Intrados.m_RearPoint.x = 1.0612;
	m_Intrados.m_RearPoint.y = 0.00;
	m_Intrados.m_Slope[0].x =  0.0;
	m_Intrados.m_Slope[0].y = -1.0;

	m_Extrados.CompSlopes();
	m_Intrados.CompSlopes();
	CompMidLine();
	m_OutPoints = m_Extrados.m_iPoints + m_Intrados.m_iPoints;
	return true;
}


bool CPF::Serialize(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat,k ;
	//starting with v3.05
	if(bIsStoring)
	{
		ar << 100306;
		m_strFoilName="abcde";
		WriteCString(ar, m_strFoilName);
		WriteCOLORREF(ar, m_FoilColor);
		ar << m_FoilStyle << m_FoilWidth;
		ar << m_Extrados.m_iPoints;
		for (k=0; k<m_Extrados.m_iPoints;k++){
			ar << (float)m_Extrados.m_ctrlPoint[k].x << (float)m_Extrados.m_ctrlPoint[k].y;
		}

		// number of points on intrados (minus 1, C++ convention ?)
		ar << m_Intrados.m_iPoints;
		for (k=0; k<m_Intrados.m_iPoints;k++)
		{
			ar << (float)m_Intrados.m_ctrlPoint[k].x << (float)m_Intrados.m_ctrlPoint[k].y;
		}

		ar << (float)m_Extrados.m_RearPoint.x << (float)m_Extrados.m_RearPoint.y;
		ar << (float)m_Intrados.m_RearPoint.x << (float)m_Intrados.m_RearPoint.y;

		if(m_bVisible)    ar<<1; else ar <<0;
		if(m_bOutPoints)  ar<<1; else ar <<0;
		if(m_bCenterLine) ar<<1; else ar <<0;

		m_bModified = false;
		return true;
	}
	else
	{
		float f;
		ar >> ArchiveFormat;
		if(ArchiveFormat<100000 || ArchiveFormat>100500 ) return false;
		ReadCString(ar, m_strFoilName); //m_strFoilName = "Splined Points Foil";
		ReadCOLORREF(ar, m_FoilColor);
		ar >> m_FoilStyle >> m_FoilWidth;

		ar >> m_Extrados.m_iPoints;
		for (k=0; k<m_Extrados.m_iPoints;k++)
		{
			ar >> f; m_Extrados.m_ctrlPoint[k].x =f;
			ar >> f; m_Extrados.m_ctrlPoint[k].y =f;
		}
		if(ArchiveFormat<100306) {ar >>f; ar >>f; m_Extrados.m_iPoints++;}

		ar >> m_Intrados.m_iPoints;
		for (k=0; k<m_Intrados.m_iPoints;k++)
		{
			ar >> f; m_Intrados.m_ctrlPoint[k].x =f;
			ar >> f; m_Intrados.m_ctrlPoint[k].y =f;
		}
		if(ArchiveFormat<100306) {ar >>f; ar >>f; m_Intrados.m_iPoints++;}

		ar >> f; m_Extrados.m_RearPoint.x =f;
		ar >> f; m_Extrados.m_RearPoint.y =f;
		ar >> f; m_Intrados.m_RearPoint.x =f;
		ar >> f; m_Intrados.m_RearPoint.y =f;

		ar >> k;
		if(k !=0 && k !=1) return false;
		if(k) m_bVisible = true; else m_bVisible = false;

		ar >> k;
		if(k !=0 && k !=1) return false;
		if(k) m_bOutPoints = true; else m_bOutPoints = false;

		ar >> k;
		if(k !=0 && k !=1) return false;
		if(k) m_bCenterLine = true; else m_bCenterLine = false;

		Update();
		m_bModified = false;
		return true;
	}
}


void CPF::SetCurveParams(int style, int width, QColor color)
{
	m_FoilStyle = style;
	m_FoilWidth = width;
	m_FoilColor = color;
	m_Extrados.SetCurveParams(style, width, color);
	m_Intrados.SetCurveParams(style, width, color);
}



void CPF::SetOutPoints(bool state)
{
	m_Extrados.m_bOutPts = state;
	m_Intrados.m_bOutPts = state;
}


void CPF::SetViewRect(QRect rc)
{
	m_Intrados.m_rViewRect = rc;
	m_Extrados.m_rViewRect = rc;
}



void CPF::Update()
{
	m_Extrados.CompSlopes();
	m_Intrados.CompSlopes();
	m_OutPoints = m_Extrados.m_iPoints + m_Intrados.m_iPoints;

}

int CSplinedPoints::IsRearPoint(CVector Real, double ZoomFactor)
{
	if (fabs(Real.x-m_RearPoint.x)<0.003/ZoomFactor &&
		fabs(Real.y-m_RearPoint.y)<0.003/ZoomFactor) return -1;
	return -10;
}



CSplinedPoints::CSplinedPoints()
{
	m_Style = 0;
	m_Width = 1;
	m_Color = QColor(100,150, 60);

	m_iHighlight = -10;
	m_iSelect    = -10;
	m_iPoints    = 0;
	m_bOutPts    = false;
	m_Freq       = 8;
	m_rViewRect.setWidth(0);
	m_rViewRect.setHeight(0);
	memset(m_Outy, 0, sizeof(m_Outy));
	memset(m_ctrlPoint, 0, sizeof(m_ctrlPoint));
	memset(m_Slope, 0, sizeof(m_Slope));
}



bool CSplinedPoints::CompSlopes()
{
	double val;
	int k;

	// infinite slope at origin, set in CPF
	// depends on wether upper or lower surface
	m_Slope[m_iPoints-1].x = m_RearPoint.x-1.0;
	m_Slope[m_iPoints-1].y = m_RearPoint.y-0.0;
	val = m_Slope[m_iPoints-1].x*m_Slope[m_iPoints-1].x + m_Slope[m_iPoints-1].y * m_Slope[m_iPoints-1].y;
	val = sqrt(val);
	if(fabs(val)>0.0000001)
	{
		m_Slope[m_iPoints-1].x = m_Slope[m_iPoints-1].x /val;
		m_Slope[m_iPoints-1].y = m_Slope[m_iPoints-1].y /val;
	}
	else
	{// anything, points are merged
		m_Slope[m_iPoints-1].x = sqrt(1.0/2.0);
		m_Slope[m_iPoints-1].y = sqrt(1.0/2.0);
	}
	for (k=1; k<m_iPoints-1; k++)
	{
		m_Slope[k].x = m_ctrlPoint[k+1].x-m_ctrlPoint[k-1].x;
		m_Slope[k].y = m_ctrlPoint[k+1].y-m_ctrlPoint[k-1].y;
		val = m_Slope[k].x*m_Slope[k].x + m_Slope[k].y * m_Slope[k].y;
		val = sqrt(val);
		if(fabs(val)>0.0000001)
		{
			m_Slope[k].x = m_Slope[k].x /val;
			m_Slope[k].y = m_Slope[k].y /val;
		}
		else
		{// anything, points are merged
			m_Slope[k].x = sqrt(1.0/2.0);
			m_Slope[k].y = sqrt(1.0/2.0);
		}
	}
	return true;
}


void CSplinedPoints::DrawControlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	int i, width;
	width = 3;

	QPen PointPen;
	QPoint pt;

	for(i=0; i<m_iPoints; i++)
	{
		pt.rx() = (int)( m_ctrlPoint[i].x*scalex)+Offset.x();
		pt.ry() = (int)(-m_ctrlPoint[i].y*scaley)+Offset.y();
		if(m_rViewRect.contains(pt))
		{
			if (m_iSelect==i)
			{
				PointPen.setWidth(2);
				PointPen.setColor(QColor(130,130,255));
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
			painter.drawEllipse(pt.x()-width, pt.y()-width,2*width,2*width);
		}
	}
}


void CSplinedPoints::DrawRearPoint(QPainter &painter, double scalex, double scaley, QPoint Offset)
{
	painter.save();
	QPen RearPen;

	if(m_iHighlight ==-1)
	{
		RearPen.setColor(QColor(255,0,0));
		RearPen.setWidth(2);
		painter.setPen(RearPen);
	}
	else if(m_iSelect ==-1)
	{
		RearPen.setColor(QColor(0,0,150));
		RearPen.setWidth(2);
		painter.setPen(RearPen);
	}
	else
	{
		RearPen.setColor(m_Color);
		RearPen.setWidth(1);
		painter.setPen(RearPen);
	}

	painter.drawEllipse((int)(m_RearPoint.x*scalex+Offset.x()-3), (int)(-m_RearPoint.y*scaley+Offset.y()-3), 6, 6);
	painter.restore();
}


void CSplinedPoints::DrawSplines(QPainter & painter, double scalex, double scaley, QPoint Offset)
{
	int k,l;
	double dist, x1, x2;
	CSpline LinkSpline;
	LinkSpline.SetSplineParams(m_Style, m_Width, m_Color);
	LinkSpline.m_iRes = m_Freq;
	LinkSpline.m_iDegree = 3;
	LinkSpline.m_iCtrlPoints = 3;
	LinkSpline.SplineKnots();
	LinkSpline.m_rViewRect = m_rViewRect;

	l=0;

	for (k=0; k<m_iPoints-1; k++)
	{
		dist = fabs(m_ctrlPoint[k+1].x-m_ctrlPoint[k].x);
		LinkSpline.m_iCtrlPoints = 0;
		LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
		LinkSpline.InsertPoint(m_ctrlPoint[k].x   + (dist)/3.0 * m_Slope[k].x,
							   m_ctrlPoint[k].y   + (dist)/3.0 * m_Slope[k].y);
		LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (dist)/3.0 * m_Slope[k+1].x,
							   m_ctrlPoint[k+1].y - (dist)/3.0 * m_Slope[k+1].y);
		LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
		LinkSpline.SplineCurve();
		LinkSpline.DrawSpline(painter, scalex, scaley, Offset);

		if(m_bOutPts)
		{
			LinkSpline.DrawOutputPoints(painter, scalex, scaley, Offset);
		}

		x1 = qMin(m_ctrlPoint[k].x, m_ctrlPoint[k+1].x);
		x2 = qMax(m_ctrlPoint[k].x, m_ctrlPoint[k+1].x);

		while(x1>=0 && x1<x2 && l<=1000)
		{
//			l=(int)(x1*1000.001);
			m_Outy[l] = LinkSpline.GetY(x1);
			x1 =0.001*(double)l;
			l++;
		}

//		l=(int)(x2*1000);
//		m_Outy[l] = LinkSpline.GetY(x2);
	}
}



void CSplinedPoints::ExportToBuffer(CFoil *pFoil, bool bExtrados)
{
	int j,k;
	QString strOut;
	CSpline LinkSpline;
	LinkSpline.m_iRes = m_Freq;
	LinkSpline.m_rViewRect = m_rViewRect;
	if(bExtrados)
	{
		pFoil->n = 0;
		for (k=m_iPoints-2;k>=0; k--)
		{
			LinkSpline.m_iCtrlPoints = 0;
			LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k].x + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].x,
								   m_ctrlPoint[k].y + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].x,
								   m_ctrlPoint[k+1].y - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			for (j=LinkSpline.m_iRes-1;j>=1; j--)
			{
				pFoil->x[pFoil->n] = LinkSpline.m_Output[j].x;
				pFoil->y[pFoil->n] = LinkSpline.m_Output[j].y;
				pFoil->n++;
			}
		}

	}
	else
	{
		for (k=0; k<m_iPoints-1; k++)
		{
			LinkSpline.m_iCtrlPoints = 0;
			LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k].x + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].x,
								   m_ctrlPoint[k].y + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].x,
								   m_ctrlPoint[k+1].y - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			for (j=0; j<LinkSpline.m_iRes-1;j++)
			{
				pFoil->x[pFoil->n] = LinkSpline.m_Output[j].x;
				pFoil->y[pFoil->n] = LinkSpline.m_Output[j].y;
				pFoil->n++;
			}
		}
		pFoil->x[pFoil->n] = 1.0;
		pFoil->y[pFoil->n] = 0.0;
		pFoil->n++;
	}
}




void CSplinedPoints::ExportToFile(QTextStream &out, bool bExtrados)
{
	QString strOut;
	CSpline LinkSpline;
	LinkSpline.m_iRes = m_Freq;
	LinkSpline.m_rViewRect = m_rViewRect;

	if(bExtrados)
	{
		for (int k=m_iPoints-2;k>=0; k--)
		{
			LinkSpline.m_iCtrlPoints = 0;
			LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k].x + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].x,
								   m_ctrlPoint[k].y + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].x,
								   m_ctrlPoint[k+1].y - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			for (int j=LinkSpline.m_iRes-1;j>=1; j--)
			{
				strOut = QString(" %1  %2\n")
						 .arg(LinkSpline.m_Output[j].x,7,'f',4)
						 .arg(LinkSpline.m_Output[j].y,7,'f',4);
				out << strOut;
			}
		}
	}
	else
	{
		for (int k=0; k<m_iPoints-1; k++)
		{
			LinkSpline.m_iCtrlPoints = 0;
			LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k].x + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].x,
								   m_ctrlPoint[k].y + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].x,
								   m_ctrlPoint[k+1].y - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			for (int j=0; j<LinkSpline.m_iRes-1;j++)
			{
				strOut = QString(" %1  %2\n")
						 .arg(LinkSpline.m_Output[j].x,7,'f',4)
						 .arg(LinkSpline.m_Output[j].y,7,'f',4);
				out << strOut;
			}
		}
		strOut = QString(" %1  %2\n")
						 .arg(1.0,7,'f',4)
						 .arg(0.0,7,'f',4);
		out << strOut;
	}
}





double CSplinedPoints::GetY(double x)
{
	int k;
	CSpline LinkSpline;
	LinkSpline.m_iRes = m_Freq;
	LinkSpline.m_iCtrlPoints = 0;
	for (k=0; k<m_iPoints; k++)
	{
		if(fabs(x-m_ctrlPoint[k].x )<0.001) return m_ctrlPoint[k].y;
		if (m_ctrlPoint[k].x < x && x<=m_ctrlPoint[k+1].x)
		{
			LinkSpline.InsertPoint(m_ctrlPoint[k].x, m_ctrlPoint[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k].x   + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].x,
								   m_ctrlPoint[k].y   + (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].x,
								   m_ctrlPoint[k+1].y - (m_ctrlPoint[k+1].x-m_ctrlPoint[k].x)/3.0 * m_Slope[k+1].y);
			LinkSpline.InsertPoint(m_ctrlPoint[k+1].x, m_ctrlPoint[k+1].y);
			LinkSpline.SplineKnots();
			LinkSpline.SplineCurve();
			return LinkSpline.GetY(x);;
		}
	}
	return 0.0;
}


int CSplinedPoints::InsertPoint(double x, double y)
{
	int j,k;
	int ipoint = 0;

	if(m_iPoints==0)
	{	//No points yet
		m_ctrlPoint[0].x = x;
		m_ctrlPoint[0].y = y;
		m_iPoints++;
	}
	else{
		if(x<m_ctrlPoint[0].x)
		{	// if we're the new minimum point
			for (j=m_iPoints; j>=0; j--)
			{
				m_ctrlPoint[j] = m_ctrlPoint[j-1];
			}
			m_ctrlPoint[0].x = x;
			m_ctrlPoint[0].y = y;
			ipoint = 0;
			m_iPoints++;
		}
		else{// if we're the new maximum point
			if(x>=m_ctrlPoint[m_iPoints-1].x)
			{
				m_ctrlPoint[m_iPoints].x = x;
				m_ctrlPoint[m_iPoints].y = y;
				ipoint = m_iPoints;
				m_iPoints++;
			}
			else
			{
				// else if we're in between
				for (k=0; k<m_iPoints; k++)
				{
					if (x>m_ctrlPoint[k].x && x<m_ctrlPoint[k+1].x)
					{
						for (j=m_iPoints; j>k+1; j--)
						{
							m_ctrlPoint[j] = m_ctrlPoint[j-1];
						}
						m_ctrlPoint[k+1].x = x;
						m_ctrlPoint[k+1].y = y;
						ipoint = k;
						m_iPoints++;
						break;
					}
				}
			}
		}
	}

	CompSlopes();
	return ipoint;
}


int CSplinedPoints::IsControlPoint(CVector Real, double ZoomFactor)
{
	for (int k=0; k<m_iPoints; k++)
	{
		if (fabs(Real.x-m_ctrlPoint[k].x)<0.003/ZoomFactor &&
			fabs(Real.y-m_ctrlPoint[k].y)<0.003/ZoomFactor) return k;
	}
	return -10;
}


int CSplinedPoints::IsControlPoint(double x, double y, double Zoom)
{
	for (int k=0; k<m_iPoints; k++)
	{
		if (fabs(x-m_ctrlPoint[k].x)/Zoom<0.003 && fabs(y-m_ctrlPoint[k].y)/Zoom<0.003) return k;
	}
	return -10;
}


bool CSplinedPoints::RemovePoint(int k)
{
	int j;
	if (k>0 && k<m_iPoints)
	{
		for (j=k; j<m_iPoints; j++){
			m_ctrlPoint[j] = m_ctrlPoint[j+1];
		}
		m_iPoints--;
	}
	CompSlopes();
	return true;
}


void CSplinedPoints::SetCurveParams(int style, int width, QColor color)
{
	m_Style = style;
	m_Width = width;
	m_Color = color;
}







