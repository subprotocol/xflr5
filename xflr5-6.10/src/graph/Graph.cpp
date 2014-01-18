/****************************************************************************

    Graph Classes
	Copyright (C) 2003-2014 Andre Deperrois adeperrois@xflr5.com

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



#include "Graph.h"
#include <math.h>
#include <QtDebug>

//static variables must be initialized in C++
QColor Graph::m_CurveColors[10];


Graph::Graph()
{
	//Type is used to determine automatic scales
	m_Type = 1;

	m_X = 0;
	m_Y = 1;

	xmin         = .0;
	xmax         = .10;
	ymin         = .0;
	ymax         = .10;
	xunit        = 0.1;
	yunit        = 0.1;
	xo           = 0.0;
	yo           = 0.0;
	m_XMinorUnit = 0.01;
	m_YMinorUnit = 0.01;

	m_scalex     = 0.1;
	m_scaley     = 0.1;
	Cxmin        = 0.0;
	Cxmax        = 1.0;
	Cymin        = 0.0;
	Cymax        = 1.0;

	exp_x = 0;
	exp_y = 0;

	m_bYInverted  = false;
	m_bAutoX        = true;
	m_bAutoY        = true;
	m_bXAutoMinGrid = true;
	m_bYAutoMinGrid = true;
	m_bBorder       = true;

	m_ptoffset.rx() = 0;
	m_ptoffset.ry() = 0;

	m_CurveColors[0] = QColor(255,   0,   0);
	m_CurveColors[1] = QColor(  0,   0, 255);
	m_CurveColors[2] = QColor(  0, 255,   0);
	m_CurveColors[3] = QColor(255, 255,   0);
	m_CurveColors[4] = QColor(  0, 255, 255);
	m_CurveColors[5] = QColor(255,   0, 255);
	m_CurveColors[6] = QColor(255, 125,  70);
	m_CurveColors[7] = QColor( 70, 125, 255);
	m_CurveColors[8] = QColor(125, 255,  70);
	m_CurveColors[9] = QColor(255, 70,  200);

	m_AxisStyle   = 0;
	m_AxisWidth   = 1;
	m_BorderStyle = 0;
	m_BorderWidth = 2;
	m_XMajStyle   = 0;
	m_XMajWidth   = 1;
	m_XMinStyle   = 1;
	m_XMinWidth   = 1;
	m_YMajStyle   = 0;
	m_YMajWidth   = 1;
	m_YMinStyle   = 1;
	m_YMinWidth   = 1;

	m_iMargin = 41;

	m_h       = 0;
	m_w       = 0;

	SetDefaults();
}

Graph::~Graph()
{
}



Curve* Graph::AddCurve()
{
	Curve *pCurve = new Curve();
	if(pCurve)
	{
		int nIndex = m_oaCurves.size();
		pCurve->SetColor(m_CurveColors[nIndex%10]);
		pCurve->SetStyle(0);
		pCurve->m_pParentGraph = this;
		m_oaCurves.append(pCurve);
	}
	return pCurve;
}

/**< In the case where a curve has been constructed independantly and needs to be added to the Graph */
Curve* Graph::AddCurve(Curve *pCurve)
{
	if(pCurve)
	{
		int nIndex = m_oaCurves.size();
		pCurve->SetColor(m_CurveColors[nIndex%10]);
		pCurve->SetStyle(0);
		pCurve->m_pParentGraph = this;
		m_oaCurves.append(pCurve);
	}
	return pCurve;
}



double Graph::ClientTox(double x)
{
	return (x-m_ptoffset.x())*m_scalex;
}

double Graph::ClientToy(double y)
{
	return (y-m_ptoffset.y())*m_scaley;
}

double Graph::ClientTox(int x)
{
	return ((double)x-(double)m_ptoffset.x())*m_scalex;
}

double Graph::ClientToy(int y)
{
	return ((double)y-(double)m_ptoffset.y())*m_scaley;
}



void Graph::CopySettings(Graph *pGraph, bool bScales)
{
	if(bScales)
	{
		xmin            = pGraph->xmin;
		xmax            = pGraph->xmax;
		xo              = pGraph->xo;
		xunit           = pGraph->xunit;

		ymin            = pGraph->ymin;
		ymax            = pGraph->ymax;
		yo              = pGraph->yo;
		yunit           = pGraph->yunit;

		m_scalex        = pGraph->m_scalex;
		m_scaley        = pGraph->m_scaley;
	}

	m_AxisColor     = pGraph->m_AxisColor;
	m_BkColor       = pGraph->m_BkColor;
	m_bBorder       = pGraph->m_bBorder;
	m_BorderColor   = pGraph->m_BorderColor;
	m_BorderStyle   = pGraph->m_BorderStyle;
	m_BorderWidth   = pGraph->m_BorderWidth;
	m_LabelColor    = pGraph->m_LabelColor;
	m_TitleColor    = pGraph->m_TitleColor;
	m_AxisStyle     = pGraph->m_AxisStyle;
	m_AxisWidth     = pGraph->m_AxisWidth;
	m_XMajClr       = pGraph->m_XMajClr;
	m_XMajStyle     = pGraph->m_XMajStyle;
	m_XMajWidth     = pGraph->m_XMajWidth;
	m_XMinClr       = pGraph->m_XMinClr;
	m_XMinStyle     = pGraph->m_XMinStyle;
	m_XMinWidth     = pGraph->m_XMinWidth;
	m_YMajClr       = pGraph->m_YMajClr;
	m_YMajStyle     = pGraph->m_YMajStyle;
	m_YMajWidth     = pGraph->m_YMajWidth;
	m_YMinClr       = pGraph->m_YMinClr;
	m_YMinStyle     = pGraph->m_YMinStyle;
	m_YMinWidth     = pGraph->m_YMinWidth;

	m_bAutoX        = pGraph->m_bAutoX;
	m_bAutoY        = pGraph->m_bAutoY;
	m_bXAutoMinGrid = pGraph->m_bXAutoMinGrid;
	m_bYAutoMinGrid = pGraph->m_bYAutoMinGrid;
	m_bYInverted    = pGraph->m_bYInverted;
	m_bXMajGrid     = pGraph->m_bXMajGrid;
	m_bXMinGrid     = pGraph->m_bXMinGrid;
	m_bYMajGrid     = pGraph->m_bYMajGrid;
	m_bYMinGrid     = pGraph->m_bYMinGrid;
	m_bBorder       = pGraph->m_bBorder;
	m_iMargin       = pGraph->m_iMargin;
}


void Graph::DeleteCurve(int index)
{
    Curve * pCurve = GetCurve(index);
    m_oaCurves.removeAt(index);
	delete pCurve;
}


void Graph::DeleteCurve(Curve *pCurve)
{
	Curve *pOldCurve = NULL;
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pOldCurve = (Curve*)m_oaCurves.at(i);
		if(pOldCurve==pCurve)
		{
			m_oaCurves.removeAt(i);
			delete pCurve;
			return;
		}
	}
}


void Graph::DeleteCurve(QString CurveTitle)
{
	Curve *pOldCurve = NULL;
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pOldCurve = (Curve*)m_oaCurves.at(i);
		if(pOldCurve->m_CurveName==CurveTitle)
		{
			m_oaCurves.removeAt(i);
			delete pOldCurve;
			return;
		}
	}
}


void Graph::DeleteCurves()
{
    int nIndex = (int)m_oaCurves.size();
	for (int i=nIndex-1; i>=0;i--)
		delete GetCurve(i);

    m_oaCurves.clear();//removes the pointers

    if (m_bAutoX && !m_Type)
    {
		xmin =  0.0;
		xmax =  0.1;
	}

	if (m_bAutoY && !m_Type){
		ymin =  0.0;
		ymax =  0.1;
	}
}


//___________________Start Gets______________________________________________________________

int Graph::GetCurveCount()
{
    return (int)m_oaCurves.size();
}


QColor Graph::GetLabelColor()
{
	return m_LabelColor;
}

QColor Graph::GetAxisColor()
{
	return m_AxisColor;
}

QColor Graph::GetBorderColor()
{
	return m_BorderColor;
}

QColor Graph::GetBackColor()
{
	return m_BkColor;
}

int Graph::GetAxisStyle()
{
	return m_AxisStyle;
}

int Graph::GetAxisWidth()
{
	return m_AxisWidth;
}


bool Graph::GetAutoX()
{
	return m_bAutoX;
}
bool Graph::GetAutoY()
{
	return m_bAutoY;
}




bool Graph::GetAutoXMin()
{
	return m_bXAutoMinGrid;
}
bool Graph::GetAutoYMin()
{
	return m_bYAutoMinGrid;
}



bool Graph::GetBorder()
{
	return m_bBorder;
}



int Graph::GetBorderStyle()
{
	return m_BorderStyle;
}

int Graph::GetBorderWidth()
{
	return m_BorderWidth;
}
void Graph::GetClientRect(QRect &Rect)
{
	Rect = m_rCltRect;
}

Curve* Graph::GetCurve(int nIndex)
{
    if(m_oaCurves.size()>nIndex)
		return (Curve*)m_oaCurves[nIndex];
	else return NULL;
}


Curve* Graph::GetCurve(QString CurveTitle)
{
	QString strong;
	Curve * pCurve;
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pCurve = (Curve*)m_oaCurves.at(i);
		if(pCurve)
		{
			pCurve->title(strong);
			if(strong==CurveTitle) return pCurve;
		}
	}
	return NULL;
}


bool Graph::GetInverted()
{
	if(m_bYInverted) return true;
	else return false;
}

int Graph::GetMargin()
{
	return m_iMargin;
}


void Graph::GetGraphName(QString &GraphName)
{
	GraphName = m_GraphName;
}

QString Graph::GetGraphName()
{
	return m_GraphName;
}

QColor Graph::GetTitleColor()
{
	return m_TitleColor;
}
double Graph::GetX0()
{
	return xo;
}


bool Graph::GetXMajGrid()
{
	return m_bXMajGrid;
}


void Graph::GetXMajGrid(bool &bstate, QColor &clr, int &style, int &width)
{
	bstate = m_bXMajGrid;
	clr   = m_XMajClr;
	style = m_XMajStyle;
	width = m_XMajWidth;
}


bool Graph::GetXMinGrid()
{
	return m_bXMinGrid;
}

void Graph::GetXMinGrid(bool &state, bool &bAuto, QColor &clr, int &style, int &width, double &unit)
{
	state = m_bXMinGrid;
	bAuto = m_bXAutoMinGrid;
	clr   = m_XMinClr;
	style = m_XMinStyle;
	width = m_XMinWidth;
	unit  = m_XMinorUnit;
}

double Graph::GetXMin()
{
	return xmin;
}

double Graph::GetXMax()
{
	return xmax;
}


double Graph::GetXScale()
{
	return m_scalex;
}
void Graph::GetXTitle(QString &str)
{
	str = m_XTitle;
}

double Graph::GetXUnit()
{
	return xunit;
}


int Graph::GetXVariable()
{
	return m_X;
}



bool Graph::GetYMajGrid()
{
	return m_bYMajGrid;
}

void Graph::GetYMajGrid(bool &state, QColor &clr, int &style, int &width)
{
	state = m_bYMajGrid;
	clr   = m_YMajClr;
	style = m_YMajStyle;
	width = m_YMajWidth;
}

void Graph::GetYTitle(QString &str)
{
	str = m_YTitle;
}


bool Graph::GetYMinGrid()
{
	return m_bYMinGrid;
}


void Graph::GetYMinGrid(bool &state, bool &bAuto, QColor &clr, int &style, int &width, double &unit)
{
	state = m_bYMinGrid;
	bAuto = m_bYAutoMinGrid;
	clr   = m_YMinClr;
	style = m_YMinStyle;
	width = m_YMinWidth;
	unit  = m_YMinorUnit;
}


double Graph::GetY0()
{
	return yo;
}
double Graph::GetYMin()
{
	return ymin;
}
double Graph::GetYMax()
{
	return ymax;
}
double Graph::GetYUnit()
{
	return yunit;
}


double Graph::GetYScale()
{
	return m_scaley;
}


int Graph::GetYVariable()
{
	return m_Y;
}


bool Graph::Init()
{
	//graph width and height
	m_w =  m_rCltRect.width()  -2*m_iMargin;
	m_h =  m_rCltRect.height() -2*m_iMargin;

	SetXScale();
	SetYScale();

	if(m_bXAutoMinGrid) m_XMinorUnit = xunit/5.0;
	if(m_bYAutoMinGrid) m_YMinorUnit = yunit/5.0;

	return true;
}


bool Graph::IsInDrawRect(QPoint const &pt)
{
	if(m_rCltRect.contains(pt)) return true;
	else return false;
}

bool Graph::IsInDrawRect(int const &x, int const &y)
{
	if(m_rCltRect.contains(x,y)) return true;
	else return false;
}


void Graph::ResetCurves()
{
	Curve *pCurve;
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pCurve = (Curve*)m_oaCurves.at(i);
		pCurve->clear();
	}
}


void Graph::ResetLimits()
{
	ResetXLimits();
	ResetYLimits();
}


void Graph::ResetXLimits()
{
	if(m_bAutoX)
	{
		xmin =  0.0;
		xmax =  0.1;
		xo   =  0.0;
	}
}


void Graph::ResetYLimits()
{
	if(m_bAutoY)
	{
		ymin =   0.000;
		ymax =   0.001;
		yo   =   0.000;
	}
}


void Graph::Scale(double zoom)
{
	if (zoom<0.01) zoom =0.01;
	m_bAutoX = false;
	m_bAutoY = false;

	double xm = (xmin + xmax)/2.0;
	xmin = xm+(xmin-xm)*zoom;
	xmax = xm+(xmax-xm)*zoom;

	double ym = (ymin + ymax)/2.0;
	ymin = ym+(ymin-ym)*zoom;
	ymax = ym+(ymax-ym)*zoom;
}


void Graph::Scalex(double zoom)
{
	if (zoom<0.01) zoom =0.01;
	m_bAutoX = false;

	double xm = (xmin + xmax)/2.0;
	xmin = xm+(xmin-xm)*zoom;
	xmax = xm+(xmax-xm)*zoom;

}

void Graph::Scaley(double zoom)
{
	if (zoom<0.01) zoom =0.01;
	m_bAutoY = false;

	double ym = (ymin + ymax)/2.0;
	ymin = ym+(ymin-ym)*zoom;
	ymax = ym+(ymax-ym)*zoom;
}

//___________________Start Sets______________________________________________________________


void Graph::SetAuto(bool bAuto)
{
	m_bAutoX = bAuto;
	m_bAutoY = bAuto;
	ResetXLimits();
	ResetYLimits();
}

void Graph::SetAutoX(bool bAuto)
{
	m_bAutoX = bAuto;
	ResetXLimits();
}
void Graph::SetAutoY(bool bAuto)
{
	m_bAutoY = bAuto;
	ResetYLimits();
}
void Graph::SetAutoXMinUnit(bool bAuto)
{
	m_bXAutoMinGrid = bAuto;
	if(bAuto) m_XMinorUnit = xunit/5.0;
}


void Graph::SetAutoXUnit()
{
//	xunit = 100.0*m_scalex;
	xunit = (xmax-xmin)/3.0;

	if (xunit<1.0)
	{
		exp_x = (int)log10(xunit*1.00001)-1;
		exp_x = qMax(-4, exp_x);
	}
	else exp_x = (int)log10(xunit*1.00001);
	int main_x = (int)(xunit/pow(10.0, exp_x)*1.000001);


	if(main_x<2)
		xunit = pow(10.0,exp_x);
	else if (main_x<5)
		xunit = 2.0*pow(10.0,exp_x);
	else
		xunit = 5.0*pow(10.0,exp_x);

}


void Graph::SetAutoYMinUnit(bool bAuto)
{
	m_bYAutoMinGrid = bAuto;
	if(bAuto) m_YMinorUnit = yunit/5.0;
}


void Graph::SetAutoYUnit()
{
//	yunit = 100.0 * m_scaley;
	yunit = (ymax-ymin)/5.0;
	if (yunit<1.0)
	{
		exp_y = (int)log10(yunit*1.00001)-1;
//		exp_y = qMax(-4, exp_y);
	}
	else  exp_y = (int)log10(yunit*1.00001);

	int main_y = (int)(yunit/pow(10.0, exp_y));

	if(main_y<2)
		yunit = pow(10.0,exp_y);
	else if (main_y<5)
		yunit = 2.0*pow(10.0,exp_y);
	else
		yunit = 5.0*pow(10.0,exp_y);	
}

void Graph::SetAxisData(int s, int w, QColor clr)
{
	m_AxisStyle = s;
	m_AxisWidth = w;
	m_AxisColor = clr;
}

void Graph::SetAxisColor(QColor crColor)
{
	m_AxisColor = crColor;
}

void Graph::SetAxisStyle(int nStyle)
{
	m_AxisStyle = nStyle;
}

void Graph::SetAxisWidth(int Width)
{
	m_AxisWidth = Width;
}


void Graph::SetBkColor(QColor cr)
{
	m_BkColor = cr;
}

void Graph::SetBorderColor(QColor crBorder)
{
	m_BorderColor = crBorder;
}

void Graph::SetBorder(bool bBorder)
{
	m_bBorder = bBorder;
}

void Graph::SetBorderWidth(int w)
{
	m_BorderWidth = w;
}

void Graph::SetBorderStyle(int s)
{
	m_BorderStyle = s;
}

void Graph::SetDrawRect(QRect &Rect)
{
	m_rCltRect = Rect;
}

void Graph::SetGraphName(QString GraphName)
{
	m_GraphName = GraphName;
}

void Graph::SetDefaults()
{
	m_BkColor = QColor(0,9,13);
	m_BorderColor = QColor(200,200,200);
	m_BorderStyle = 0;
	m_BorderWidth = 3;

	m_AxisStyle = 0;
	m_AxisWidth = 1;

	m_bYInverted = false;

	SetAxisColor(QColor(200,200,200));
	SetTitleColor(QColor(255,255,255));
	SetLabelColor(QColor(255,255,255));

	m_bXMajGrid = true;
	m_bYMajGrid = true;
	m_bXMinGrid = false;
	m_bYMinGrid = false;

	m_XMajStyle = 1;
	m_YMajStyle = 1;
	m_XMajWidth = 1;
	m_YMajWidth = 1;
	m_XMajClr   = QColor(90,90,90);
	m_YMajClr   = QColor(90,90,90);

	m_XMinStyle = 1;
	m_YMinStyle = 1;
	m_XMinWidth = 1;
	m_YMinWidth = 1;
	m_XMinClr   = QColor(50,50,50);
	m_YMinClr   = QColor(50,50,50);

	m_XMinorUnit = 0.1;
	m_YMinorUnit = 0.1;
}




void Graph::SetLabelColor(QColor crColor)
{
	m_LabelColor = crColor;
}


void Graph::SetInverted(bool bInverted)
{
	m_bYInverted = bInverted;
}


void Graph::SetMargin(int m)
{
	m_iMargin = m;
}



void Graph::SetTitleColor(QColor crColor)
{
	m_TitleColor = crColor;
}




void Graph::SetType(int type)
{
	m_Type = type;
}



void Graph::SetVariables(int const & X, int const & Y)
{
	m_X = X;
	m_Y = Y;
}


void Graph::SetWindow(double x1, double x2, double y1, double y2)
{
	m_bAutoX = false;
	m_bAutoY = false;
	xmin = x1;
	xmax = x2;
	ymin = y1;
	ymax = y2;
}

void Graph::SetX0(double f){
	xo = f;
}



void Graph::SetXMajGrid(bool const &state, QColor const &clr, int const &style, int const &width)
{
	m_bXMajGrid = state;
	m_XMajClr   = clr;
	m_XMajStyle = style;
	m_XMajWidth = width;
}


void Graph::SetXMajGrid(bool const &bGrid)
{
	m_bXMajGrid = bGrid;
}

void Graph::SetXMinGrid(bool const &bGrid)
{
	m_bXMinGrid = bGrid;
}



void Graph::SetXMax(double f){
	xmax = f;
}


void Graph::SetXMin(double f){
	xmin = f;
}

void Graph::SetXMinGrid(bool state, bool bAuto, QColor clr, int style, int width, double unit)
{
	m_bXMinGrid = state;
	m_bXAutoMinGrid = bAuto;
	m_XMinClr   = clr;
	m_XMinStyle = style;
	m_XMinWidth = width;
	if(unit>0.0) m_XMinorUnit  = unit;
}



void Graph::SetXMinorUnit(double f){
	m_XMinorUnit = f;
}


bool Graph::SetXScale()
{
	Curve *pCurve;
	int nc;

	if(m_bAutoX)
	{
		bool bCurve = false;

		if (m_oaCurves.size())
		{
			//init only if we have a curve
			for (nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible()) && pCurve->size()>1)
				{
					bCurve = true;
					break;//there is something to draw
				}
			}
		}
		if (bCurve)
		{
			Cxmin =  9999999.0;
			Cxmax = -9999999.0;
			for (nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible())  && pCurve->size()>0)
				{
					Cxmin = qMin(Cxmin, pCurve->xMin());
					Cxmax = qMax(Cxmax, pCurve->xMax());
				}
			}

			if(Cxmax<=Cxmin)
				Cxmax = (Cxmin+1.0)*2.0;

			if(m_Type == 1)
			{
				xmin = qMin(xmin, Cxmin);
				xmax = qMax(xmax, Cxmax);
			}
			else
			{
				xmin = Cxmin;
				xmax = Cxmax;
			}
			if(Cxmin>=0.0) xmin = 0.0;
			if(Cxmax<=0.0) xmax = 0.0;

		}
		else
		{
			// until things are made clear
			for (nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible())  && pCurve->size()>0)
				{
					xmin = qMin(xmin, pCurve->x[0]);
					xmax = qMax(xmax, pCurve->x[0]);
				}
			}
		}
		xo=0.0;

		if(qAbs((xmin-xmax)/xmin)<0.001)
		{
			if(qAbs(xmin)<0.00001) xmax = 1.0;
			else
			{
				xmax = 2.0 * xmin;
				if(xmax < xmin)
				{
					double tmp = xmax;
					xmax = xmin;
					xmin = tmp;
				}
			}
		}

		if(m_w<=0.0) return false;

		m_scalex   = (xmax-xmin)/m_w;


		//try to set an automatic scale for X Axis

		SetAutoXUnit();
	}
	else
	{
		//scales are set manually
		if(m_w<=0.0) return false;

//		m_scalex   =  (xmax-xmin)/m_w;
		if (xunit<1.0)
		{
			exp_x = (int)log10(xunit*1.00001)-1;
			exp_x = qMax(-4, exp_x);
		}
		else exp_x = (int)log10(xunit*1.00001);

	}
	m_scalex   =  (xmax-xmin)/m_w;

	//graph center position
	int Xg = (m_rCltRect.right() + m_rCltRect.left())/2;
	// curves center position
	int Xc = (int)((xmin+xmax)/2.0/m_scalex);
	// center graph in drawing rectangle
	m_ptoffset.rx() = (Xg-Xc);
	return true;
}



void Graph::SetXUnit(double f){
	xunit = f;
}

void Graph::SetXTitle(QString str)
{
	m_XTitle = str;
}



void Graph::SetXVariable(int const & X)
{
	m_X = X;
}




void Graph::SetYMin(double f){
	ymin = f;
}

void Graph::SetYMinorUnit(double f){
	m_YMinorUnit = f;
}


void Graph::SetYMax(double f){
	ymax = f;
}

void Graph::SetY0(double f){
	yo = f;
}

void Graph::SetYTitle(QString str)
{
	m_YTitle = str;
}

void Graph::SetYUnit(double f){
	yunit = f;
}



bool Graph::SetYScale()
{
	int nc;
	Curve *pCurve;

	if(m_bAutoY)
	{
		bool bCurve = false;
		if (m_oaCurves.size())
		{
			//init only if we have a curve
			for (nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible())  && pCurve->size()>0)
					{
						bCurve = true;
						break;
					}
			}
		}
		if(bCurve)
		{
			Cymin =  9999999.0;
			Cymax = -9999999.0;
			for (nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible()) && pCurve->size()>0)
				{
					Cymin = qMin(Cymin, pCurve->yMin());
					Cymax = qMax(Cymax, pCurve->yMax());
				}
			}
			if(Cymax<=Cymin)
			{
				Cymax = (Cymin+1.0)*2.0;
			}

			if(m_Type == 1)
			{
				ymin = qMin(ymin, Cymin);
				ymax = qMax(ymax, Cymax);
			}
			else
			{
				ymin = Cymin;
				ymax = Cymax;
			}
			if(Cymin>=0.0) ymin = 0.0;
			if(Cymax<=0.0) ymax = 0.0;
		}
		else
		{
			// until things are made clear
			for (int nc=0; nc < m_oaCurves.size(); nc++)
			{
				pCurve = (Curve*)m_oaCurves[nc];
				if ((pCurve->IsVisible() ||pCurve->PointsVisible())  && pCurve->size()>0)
				{
					ymin = qMin(ymin, pCurve->y[0]);
					ymax = qMax(ymax, pCurve->y[0]);
				}
			}
		}
		yo=0.0;

		if (qAbs((ymin-ymax)/ymin)<0.001)
		{
			if(qAbs(ymin)<0.00001) ymax = 1.0;
			else
			{
				ymax = 2.0 * ymin;
				if(ymax < ymin)
				{
					double tmp = ymax;
					ymax = ymin;
					ymin = tmp;
				}
			}
		}

		if(m_h<=0.0) return false;

		if (!m_bYInverted)
		{
			m_scaley   = -(ymax-ymin)/m_h;
		}
		else
		{
			m_scaley   =  (ymax-ymin)/m_h;
		}

		//try to set an automatic scale for Y Axis
		SetAutoYUnit();
	}
	else
	{
		//scales are set manually
		if(m_h<=0) return false;

		if (!m_bYInverted)
		{
			m_scaley   = -(ymax-ymin)/m_h;
		}
		else
		{
			m_scaley   =  (ymax-ymin)/m_h;
		}

		if (yunit<1.0)
		{
			exp_y = (int)log10(yunit*1.00001)-1;
			exp_y = qMax(-4, exp_y);
		}
		else  exp_y = (int)log10(yunit*1.00001);

	}

	//graph center position
	int Yg = (m_rCltRect.top() + m_rCltRect.bottom())/2;
	// curves center position
	int Yc = (int)((ymin+ymax)/2.0/m_scaley);
	// center graph in drawing rectangle
	m_ptoffset.ry() = (Yg-Yc);

	return true;
}

void Graph::SetYMajGrid(bool const &state, QColor const &clr, int const &style, int const &width)
{
	m_bYMajGrid = state;
	m_YMajClr   = clr;
	m_YMajStyle = style;
	m_YMajWidth = width;
}


void Graph::SetYMajGrid(bool const &bGrid)
{
	m_bYMajGrid = bGrid;
}

void Graph::SetYMinGrid(bool state, bool bAuto, QColor clr, int style, int width, double unit)
{
	m_bYMinGrid = state;
	m_bYAutoMinGrid = bAuto;
	m_YMinClr   = clr;
	m_YMinStyle = style;
	m_YMinWidth = width;
	if(unit>0.0) m_YMinorUnit  = unit;
}


void Graph::SetYMinGrid(bool const &bGrid)
{
	m_bYMinGrid = bGrid;
}



void Graph::SetYVariable(int const & Y)
{
	m_Y = Y;
}


int Graph::xToClient(double x)
{
	return (int)(x/m_scalex + m_ptoffset.x());
}



int Graph::yToClient(double y)
{
	return (int)(y/m_scaley + m_ptoffset.y());
}



Curve*  Graph::GetClosestPoint(const double &x, const double &y, double &xSel, double &ySel, int &nSel)
{
	static int i, n1;
	static double dist, dmax, x1, y1;
	dmax = 1.e40;
	Curve *pOldCurve, *pCurveSel;
	pCurveSel = NULL;
	
	for(i=0; i<m_oaCurves.size(); i++)
	{
		pOldCurve = (Curve*)m_oaCurves.at(i);
		pOldCurve->closestPoint(x, y, x1, y1, dist, n1);
		if(dist<dmax)
		{
			dmax = dist;
			xSel = x1;
			ySel = y1;
			pCurveSel = pOldCurve;
			nSel = i;
		}
	}
	return pCurveSel;
}


Curve* Graph::GetCurvePoint(const int &xClt, const int &yClt,int &nSel)
{
	static int i, n, xc, yc;
	static double dist, x1, y1, x,y;
	Curve *pOldCurve;

	x= ClientTox(xClt);
	y= ClientToy(yClt);
	for(i=0; i<m_oaCurves.size(); i++)
	{
		pOldCurve = (Curve*)m_oaCurves.at(i);
		pOldCurve->closestPoint(x, y, x1, y1, dist, n);

		xc = xToClient(x1);
		yc = yToClient(y1);

		if((xClt-xc)*(xClt-xc) + (yClt-yc)*(yClt-yc) <16)//sqrt(16) pixels distance
		{
			nSel = n;
			return pOldCurve;
		}
	}
	nSel = -1;
	return  NULL;
}


bool Graph::SelectPoint(QString const &CurveName, int sel)
{
	QString str;
	Curve *pCurve = NULL;
	
	if(sel<0) 
	{
//		pCurve->SetSelected(-1);
		return false;				
	}		
	
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pCurve = (Curve*)m_oaCurves.at(i);
		pCurve->title(str);
		if(str == CurveName)
		{
			if(sel>pCurve->count())
			{
				pCurve->SetSelected(-1);
				return false;				
			}
			else
			{
				pCurve->SetSelected(sel);
				return true;
			}
		}
	}
//	pCurve->SetSelected(-1);
	return false;
}



void Graph::DeselectPoint()
{
	Curve *pCurve;
	for(int i=0; i<m_oaCurves.size(); i++)
	{
		pCurve = (Curve*)m_oaCurves.at(i);
		pCurve->SetSelected(-1);
	}
}







