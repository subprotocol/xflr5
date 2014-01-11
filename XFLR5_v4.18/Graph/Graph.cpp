/****************************************************************************

    Graph Classes
    Copyright (C) 2003-2008 André Deperrois xflr5@yahoo.com

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


// Graph.cpp: implementation of the Graph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "Graph.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//static variables must be initialized in C++
COLORREF Graph::m_CurveColors[10];
int Graph::m_CurveStyles[5];


Graph::Graph()
{
	//Type is used to determine automatic scales
	m_Type = 1;

	xmin         =  9999999.0;
	xmax         = -9999999.0;
	ymin         =  9999999.0;
	ymax         = -9999999.0;
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
	m_bIsPrinting   = false;

	m_offset.x = 0;
	m_offset.y = 0;

	m_CurveColors[0] = RGB(255,   0,   0);
	m_CurveColors[1] = RGB(  0,   0, 255);
	m_CurveColors[2] = RGB(  0, 255,   0);
	m_CurveColors[3] = RGB(255, 255,   0);
	m_CurveColors[4] = RGB(  0, 255, 255);
	m_CurveColors[5] = RGB(255,   0, 255);
	m_CurveColors[6] = RGB(255, 125,  70);
	m_CurveColors[7] = RGB( 70, 125, 255);
	m_CurveColors[8] = RGB(125, 255,  70);
	m_CurveColors[9] = RGB(255, 70,  200);

	m_CurveStyles[0] = PS_SOLID;
	m_CurveStyles[1] = PS_DASH;
	m_CurveStyles[2] = PS_DOT;
	m_CurveStyles[3] = PS_DASHDOT;
	m_CurveStyles[4] = PS_DASHDOTDOT;

	m_iMargin = 44;
	m_top     = 0;
	m_left    = 0;
	m_h       = 0;
	m_w       = 0;
	m_rDrawRect.SetRectEmpty();

	SetDefaults();
}


Graph::~Graph()
{
	DeleteCurves();
}


void Graph::DrawAxes(CDC *pDC)
{
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = m_AxisColor;
	style = m_nStyle;
	width = m_nWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen AxisPen;
	AxisPen.CreatePen(PS_GEOMETRIC | m_nStyle, GetPenWidth(m_nWidth,m_bIsPrinting), &lb);

	CPen *pOldPen = pDC->SelectObject(&AxisPen);

	//vertical axis
	double xp;
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	pDC->MoveTo(int(xp/m_scalex)        +m_offset.x,
				int(ymin/m_scaley)      +m_offset.y);
	pDC->LineTo(int(xp/m_scalex)        +m_offset.x, 
				int(ymax/m_scaley)      +m_offset.y);

	//horizontal axis
	double yp;
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else					 yp = ymin;

	pDC->MoveTo(int(xmin/m_scalex)  +m_offset.x,
				int(yp/m_scaley)    +m_offset.y);
	pDC->LineTo(int(xmax/m_scalex)  +m_offset.x, 
				int(yp/m_scaley)    +m_offset.y);

	pDC->SelectObject(pOldPen);
	AxisPen.DeleteObject();
}


void Graph::DrawXTicks(CDC *pDC)
{
	if(abs(xunit)<0.00000001) return;
	if(abs(xmax-xmin)/xunit>30.0) return;

	CString strLabel;
	int exp;
	double main;
	int TickSize;
	int xExpOff, yExpOff;
	int xMainOff;
	LOGFONT TempFont;
	memcpy(&TempFont, &m_LabelLogFont, sizeof(LOGFONT));

	if(m_bIsPrinting) 
	{
		TickSize = -300;
		TempFont.lfHeight = TempFont.lfHeight*30;
		xExpOff  = 900;
		yExpOff  = -100;
		xMainOff = 700;
	}
	else {
		TickSize = 5;
		xExpOff  = 24;
		yExpOff  = 3;
		xMainOff = 14;
	}

	CFont LabelFont;
	LabelFont.CreateFontIndirect(&TempFont);
	CFont* pOldFont = pDC->SelectObject(&LabelFont);

	CFont ExponentFont;
	LOGFONT ExpFont;	
	memcpy(&ExpFont, &TempFont, sizeof(LOGFONT));
	ExpFont.lfHeight = (int)(ExpFont.lfHeight*10/12);
	ExponentFont.CreateFontIndirect(&ExpFont);

	pDC->SetBkMode(TRANSPARENT);

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color = m_AxisColor;
	int style = m_nStyle;
	int width = m_nWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen AxisPen;
	AxisPen.CreatePen(PS_GEOMETRIC | m_nStyle, GetPenWidth(m_nWidth,m_bIsPrinting), &lb);


	if(IsBlackAndWhite()) pDC->SetTextColor(RGB(0,0,0));
	else pDC->SetTextColor(m_LabelColor);

	CPen *pOldPen = pDC->SelectObject(&AxisPen);

	double xt = xo-int((xo-xmin)*1.0001/xunit)*xunit;//one tick at the origin

	double yp;
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	while(xt<=xmax*1.0001)
	{
		//Draw ticks
		if(xt>=xmin){
			pDC->MoveTo(int(xt/m_scalex)           + m_offset.x,
						int(yp/m_scaley) +TickSize + m_offset.y);
			pDC->LineTo(int(xt/m_scalex)           + m_offset.x, 
						int(yp/m_scaley)           + m_offset.y);

			if(exp_x>=4 || exp_x<=-4) 
			{
				pDC->SetTextAlign(TA_RIGHT);
				main = xt;
				ExpFormat(main, exp);

				strLabel.Format("%5.1f 10",main);
				pDC->TextOut(int(xt/m_scalex) +xMainOff          +m_offset.x,
							 int(yp/m_scaley) +int(TickSize*1.5) +m_offset.y,
							 strLabel);
				strLabel.Format("%d", exp);
				pDC->SelectObject(&ExponentFont);
				pDC->TextOut(int(xt/m_scalex) + xExpOff                     +m_offset.x,
							 int(yp/m_scaley) - yExpOff + int(TickSize*1.5) +m_offset.y,
							 strLabel);
				pDC->SelectObject(&LabelFont);
			}
			else 
			{
				pDC->SetTextAlign(TA_CENTER);
				if(exp_x>=0) strLabel.Format("%.f",xt);
				else if (exp_x>=-1) strLabel.Format("%.1f",xt);
				else if (exp_x>=-2) strLabel.Format("%.2f",xt);
				else if (exp_x>=-3) strLabel.Format("%.3f",xt);
						 
				pDC->TextOut(int(xt/m_scalex)                    +m_offset.x,
							 int(yp/m_scaley) +int(TickSize*1.5) +m_offset.y,
							 strLabel);
			}
		}
		xt += xunit ;

	}
	pDC->SelectObject(pOldPen);

	AxisPen.DeleteObject();
	
	pDC->SelectObject(pOldFont);
	LabelFont.DeleteObject();
	ExponentFont.DeleteObject();
}

void Graph::DrawYTicks(CDC *pDC)
{
	if(abs(xunit)<0.00000001) return;
	if(abs(ymax-ymin)/yunit>30.0) return;

	CString strLabel, strLabelExp;
	int exp;
	double main;
	int TickSize;
	int xExpOff, yExpOff;
	LOGFONT TempFont;
	memcpy(&TempFont, &m_LabelLogFont, sizeof(LOGFONT));

	if(m_bIsPrinting) 
	{
		TickSize = 300;
		TempFont.lfHeight = TempFont.lfHeight*30;
		xExpOff  = 100;
		yExpOff  = -100;
	}
	else 
	{
		TickSize = 5;
		xExpOff  = 7;
		yExpOff  = 2;
	}

	CFont LabelFont;
	LabelFont.CreateFontIndirect(&TempFont);
	CFont* pOldFont = pDC->SelectObject(&LabelFont);

	CFont ExponentFont;
	LOGFONT ExpFont;	
	memcpy(&ExpFont, &TempFont, sizeof(LOGFONT));
	ExpFont.lfHeight = (int)(ExpFont.lfHeight*10/12);
	ExponentFont.CreateFontIndirect(&ExpFont);

	pDC->SetBkMode(TRANSPARENT);

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color = m_AxisColor;
	int style = m_nStyle;
	int width = m_nWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen AxisPen;
	AxisPen.CreatePen(PS_GEOMETRIC | m_nStyle, GetPenWidth(m_nWidth,m_bIsPrinting), &lb);

	if(IsBlackAndWhite()) pDC->SetTextColor(RGB(0,0,0));
	else pDC->SetTextColor(m_LabelColor);

	CPen *pOldPen = pDC->SelectObject(&AxisPen);

	double yt = yo-int((yo-ymin)*1.0001/yunit)*yunit;//one tick at the origin

	double xp;
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	while(yt<=ymax*1.0001)
	{
		//Draw ticks
		if(yt>=ymin)
		{
			pDC->MoveTo((int)(xp/m_scalex)          +m_offset.x,
						(int)(yt/m_scaley)          +m_offset.y);
			pDC->LineTo((int)(xp/m_scalex)-TickSize +m_offset.x,
						(int)(yt/m_scaley)          +m_offset.y);

			if(exp_y>=3 || exp_y<=-3) 
			{
				pDC->SetTextAlign(TA_RIGHT);

				main = yt;
				ExpFormat(main, exp);

				if(abs(yt)<1.e-7) 
				{
					main = 0.0;
					exp  = 0;
				}

				strLabel.Format("%4.1f 10",main);
				strLabelExp.Format("%d", exp);


				if (!m_bIsPrinting)
				{
					pDC->TextOut((int)(xp/m_scalex)-int(TickSize*2.0) +m_offset.x,
								 (int)(yt/m_scaley)-int(TickSize*1.5) +m_offset.y,
								 strLabel);
					pDC->SelectObject(&ExponentFont);
					if(exp_y>=3)
					{
						pDC->TextOut(int(xp/m_scalex) -int(TickSize*2.0) + xExpOff +m_offset.x,
									 int(yt/m_scaley) -int(TickSize*2.0) - yExpOff +m_offset.y,
									 strLabelExp);
					}
					else
					{
						pDC->TextOut(int(xp/m_scalex) -int(TickSize*2.0) + xExpOff +2 +m_offset.x,
									 int(yt/m_scaley) -int(TickSize*2.0) - yExpOff +m_offset.y,
									 strLabelExp);
					}
				}
				else
				{
					pDC->TextOut((int)(xp/m_scalex)-int(TickSize*2.) +m_offset.x,
								 (int)(yt/m_scaley)+int(TickSize)    +m_offset.y,
								 strLabel);
					pDC->SelectObject(&ExponentFont);
					pDC->TextOut(int(xp/m_scalex) -int(TickSize*2.)+ xExpOff +m_offset.x,
								 int(yt/m_scaley) +int(TickSize)   - yExpOff +m_offset.y,
								 strLabelExp);
				}

				pDC->SelectObject(&LabelFont);
			}
			else 
			{
				pDC->SetTextAlign(TA_RIGHT);
				if(exp_y>=0) strLabel.Format("%.f",yt);
				else if (exp_y>=-1) strLabel.Format("%.1f",yt);
				else if (exp_y>=-2) strLabel.Format("%.2f",yt);
				else if (exp_y>=-3) strLabel.Format("%.3f",yt);
						 
			if (!m_bIsPrinting)
				pDC->TextOut((int)(xp/m_scalex)-int(TickSize*2.)  +m_offset.x,
							 (int)(yt/m_scaley)-int(TickSize*1.5) +m_offset.y,
							 strLabel);
			else
				pDC->TextOut((int)(xp/m_scalex)-int(TickSize*2.)  +m_offset.x,
							 (int)(yt/m_scaley)+int(TickSize)     +m_offset.y,
							 strLabel);
			}
		}
		yt += yunit ;

	}
	pDC->SelectObject(pOldPen);

	AxisPen.DeleteObject();
	
	pDC->SelectObject(pOldFont);
	LabelFont.DeleteObject();
	ExponentFont.DeleteObject();
}

void Graph::DrawXMajGrid(CDC *pDC)
{
	if(abs(xunit)<0.00000001) return;
	if(abs(xmax-xmin)/xunit>30.0) return;
	int YMin, YMax;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = m_XMajClr;
	style = m_XMajStyle;
	width = m_XMajWidth;
	GetBWColor(color, style, width);

	if(m_XMajWidth<=1) width = m_XMajWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width, m_bIsPrinting), &lb);

	if(!m_bIsPrinting){
		YMin = max(int(ymin/m_scaley)+ m_offset.y, m_rDrawRect.top);
		YMax = min(int(ymax/m_scaley)+ m_offset.y, m_rDrawRect.bottom);
	}
	else{
		YMin = min(int(ymin/m_scaley)+ m_offset.y, -m_rDrawRect.top);
		YMax = max(int(ymax/m_scaley)+ m_offset.y, -m_rDrawRect.bottom);
	}


	CPen *pOldPen = pDC->SelectObject(&GridPen);
	double xt = xo-int((xo-xmin)*1.0001/xunit)*xunit;//one tick at the origin
	while(xt<=xmax*1.001){
		if(xt>=xmin){
			pDC->MoveTo(int(xt/m_scalex) + m_offset.x, YMin);
			pDC->LineTo(int(xt/m_scalex) + m_offset.x, YMax);
		}
		xt += xunit ;
	}
	pDC->SelectObject(pOldPen);	
	
	GridPen.DeleteObject();
}



void Graph::DrawYMajGrid(CDC *pDC)
{
	if(abs(yunit)<0.00000001) return;
	if(abs(ymax-ymin)/yunit>30.0) return;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = m_YMajClr;
	style = m_YMajStyle;
	width = m_YMajWidth;
	GetBWColor(color, style, width);

	if(m_YMajWidth<=1) width = m_YMajWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width, m_bIsPrinting), &lb);
	
	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double yt = yo-int((yo-ymin)*1.0001/yunit)*yunit;//one tick at the origin

	int XMin = max(int(xmin/m_scalex)+ m_offset.x, m_rDrawRect.left);
	int XMax = min(int(xmax/m_scalex)+ m_offset.x, m_rDrawRect.right);


	while(yt<=ymax*1.0001){
		if(yt>=ymin){
			pDC->MoveTo(XMin, (int)(yt/m_scaley)   + m_offset.y);
			pDC->LineTo(XMax, (int)(yt/m_scaley)   + m_offset.y);
		}
		yt += yunit ;
	}

	pDC->SelectObject(pOldPen);
	GridPen.DeleteObject();
}

void Graph::DrawXMinGrid(CDC *pDC)
{
	if(abs(xunit)<0.00000001) return;
	if(abs(m_XMinorUnit)<0.00000001) return;
	if(abs(xmax-xmin)/xunit>30.0) return;
	if(abs(xmax-xmin)/m_XMinorUnit>100.0) return;
	int YMin, YMax;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = m_XMinClr;
	style = m_XMinStyle;
	width = m_XMinWidth;
	GetBWColor(color, style, width);

	if(m_XMinWidth<=1) width = m_XMinWidth;
	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);

	if(!m_bIsPrinting){
		YMin = max(int(ymin/m_scaley)+ m_offset.y, m_rDrawRect.top);
		YMax = min(int(ymax/m_scaley)+ m_offset.y, m_rDrawRect.bottom);
	}
	else{
		YMin = min(int(ymin/m_scaley)+ m_offset.y, -m_rDrawRect.top);
		YMax = max(int(ymax/m_scaley)+ m_offset.y, -m_rDrawRect.bottom);
	}
	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double xDelta = m_XMinorUnit;
	double xt = xo-int((xo-xmin)*1.0001/xDelta)*xDelta;//one tick at the origin

	while(xt<=xmax*1.001){
		if(xt>=xmin){
			pDC->MoveTo(int(xt/m_scalex) + m_offset.x, YMin);
			pDC->LineTo(int(xt/m_scalex) + m_offset.x, YMax);
		}
		xt += xDelta;
	}
	pDC->SelectObject(pOldPen);
	GridPen.DeleteObject();
}

void Graph::DrawYMinGrid(CDC *pDC)
{
	if(abs(yunit)<0.00000001) return;
	if(abs(m_YMinorUnit)<0.00000001) return;
	if(abs(ymax-ymin)/yunit>30.0) return;
	if(abs(ymax-ymin)/m_YMinorUnit>100.0) return;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = m_YMinClr;
	style = m_YMinStyle;
	width = m_YMinWidth;
	GetBWColor(color, style, width);

	if(m_YMinWidth<=1) width = m_YMinWidth;
	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);
	

	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double yDelta = m_YMinorUnit;
	double yt = yo-int((yo-ymin)*1.0001/yDelta)*yDelta;//one tick at the origin
	int XMin = max(int(xmin/m_scalex)+ m_offset.x, m_rDrawRect.left);
	int XMax = min(int(xmax/m_scalex)+ m_offset.x, m_rDrawRect.right);

	while(yt<=ymax*1.0001){
		if(yt>=ymin){
			pDC->MoveTo(XMin, (int)(yt/m_scaley)   + m_offset.y);
			pDC->LineTo(XMax, (int)(yt/m_scaley)   + m_offset.y);
		}
		yt += yDelta ;
	}

	pDC->SelectObject(pOldPen);
	GridPen.DeleteObject();
}


bool Graph::Init()
{
	int Margin;
	
	if(m_bIsPrinting) {
		Margin =  1500;
		m_top     = -m_rDrawRect.top;
		m_left    =  m_rDrawRect.left;
	}
	else {
		Margin = m_iMargin;
		m_top     = m_rDrawRect.top;
		m_left    = m_rDrawRect.left;
	}
	//graph width and height
	m_w =  m_rDrawRect.right - m_rDrawRect.left   -2*Margin;
	m_h = -m_rDrawRect.top   + m_rDrawRect.bottom -2*Margin;

	SetXScale();
	SetYScale();

	if(m_bXAutoMinGrid) m_XMinorUnit = xunit/5.0;
	if(m_bYAutoMinGrid) m_YMinorUnit = yunit/5.0;

	return true;
}


void Graph::DrawGraph(CDC *pDC, CRect* pDRect, bool bIsPrinting)
{
	m_bIsPrinting = bIsPrinting;
	m_rDrawRect.CopyRect(pDRect);

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color;
	int style, width;
	if(m_bBorder){
		color = m_BorderColor;
	}
	else{
		color = m_BkColor;
	}
	style = m_BorderStyle;
	width = m_BorderWidth;
	GetBWColor(color, style, width);
	lb.lbColor = color;

	CPen BorderPen;
	BorderPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&BorderPen);


	CRect BorderRect;
	BorderRect.CopyRect(m_rDrawRect);

	if(bIsPrinting ){	
		BorderRect.top = - BorderRect.top;
		BorderRect.bottom = - BorderRect.bottom;
		if(!IsBlackAndWhite()){
			CBrush BkBrush;
			if(GetGraphBk())
				BkBrush.CreateSolidBrush(m_BkColor);
			else 
				BkBrush.CreateStockObject(WHITE_BRUSH);
			CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
			pDC->Rectangle(&BorderRect);
			pDC->SelectObject(pOldBrush);
			BkBrush.DeleteObject();
		}
		else{
			if(m_bBorder){
				CBrush BkBrush;
				BkBrush.CreateStockObject(WHITE_BRUSH);
				CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
				pDC->Rectangle(&BorderRect);
				pDC->SelectObject(pOldBrush);
				BkBrush.DeleteObject();
			}
		}
	}
	else {
		CBrush BkBrush(m_BkColor);
		CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
		pDC->Rectangle(&BorderRect);
		pDC->SelectObject(pOldBrush);
		BkBrush.DeleteObject();
	}

	pDC->SelectObject(pOldPen);

	BorderPen.DeleteObject();

	Init();

	if(m_bXMinGrid) DrawXMinGrid(pDC);
	if(m_bYMinGrid) DrawYMinGrid(pDC);
	if(m_bXMajGrid) DrawXMajGrid(pDC);
	if(m_bYMajGrid) DrawYMajGrid(pDC);

	DrawAxes(pDC);
	DrawXTicks(pDC);
	DrawYTicks(pDC);
	for (int nc=0; nc < m_oaCurves.GetSize(); nc++) 
		DrawCurve(nc, pDC);
	DrawTitles(pDC);
}


CCurve* Graph::GetCurve(int nIndex)
{
	if(m_oaCurves.GetSize()>nIndex)
		return (CCurve*)m_oaCurves[nIndex];
	else return NULL;
}


CCurve* Graph::AddCurve()
{
	CCurve *pCurve = new CCurve();
	if(pCurve){
		int nIndex = (int)m_oaCurves.GetSize();
		pCurve->SetColor(m_CurveColors[nIndex%10]);
		pCurve->SetStyle(PS_SOLID);
		m_oaCurves.Add(pCurve);
	}
	return pCurve;
}

void Graph::DeleteCurve(int index)
{
	CCurve * pCurve = GetCurve(index);
	m_oaCurves.RemoveAt(index);//removes the pointers
	delete pCurve;
}


void Graph::DeleteCurves()
{
	int nIndex = (int)m_oaCurves.GetSize();
	for (int i=nIndex-1; i>=0;i--)
		delete GetCurve(i);

	m_oaCurves.RemoveAll();//removes the pointers

	if (m_bAutoX && !m_Type){
		xmin =  0.0;
		xmax =  0.1;
	}

	if (m_bAutoY && !m_Type){
		ymin =  0.0;
		ymax =  0.1;
	}
}


int Graph::GetCurveCount()
{
	return (int)m_oaCurves.GetSize();
}


void Graph::DrawCurve(int nIndex, CDC *pDC)
{
	int x,y;
	int ptside;
	if(m_bIsPrinting) ptside = 50;
	else              ptside = 2;

	COLORREF color;
	int style, width;

	CBrush FillBrush;
	if(!m_bIsPrinting) FillBrush.CreateSolidBrush(m_BkColor);
	else{
		if(GetGraphBk())FillBrush.CreateSolidBrush(m_BkColor);
		else FillBrush.CreateSolidBrush(RGB(255,255,255));
	}
	CBrush *pOldBrush = pDC->SelectObject(&FillBrush);

	GetCurve(nIndex)->GetBWStyle(color, style, width);

	CPen CurvePen;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = color;
	CurvePen.CreatePen(PS_GEOMETRIC | style,
					   GetPenWidth(width,m_bIsPrinting), &lb);

	CPen *pOldPen = pDC->SelectObject(&CurvePen);

	CCurve* pCurve = GetCurve(nIndex);
	CPoint From, To;
	CPoint Min(int(xmin/m_scalex) +m_offset.x, int(ymin/m_scaley) +m_offset.y);
	CPoint Max(int(xmax/m_scalex) +m_offset.x, int(ymax/m_scaley) +m_offset.y);
	CRect rViewRect(Min, Max);
	rViewRect.NormalizeRect();
	rViewRect.InflateRect(1,1,1,1);

	if(pCurve->n>1 && pCurve->IsVisible()) {
		From.x = int(pCurve->x[0]/m_scalex+m_offset.x);
		From.y = int(pCurve->y[0]/m_scaley+m_offset.y);
		pDC->MoveTo(From);
		if(rViewRect.PtInRect(From) && pCurve->PointsVisible()) {
			pDC->Rectangle(From.x-ptside,From.y-ptside, From.x+ptside,From.y+ptside);
		}
		for (int i=1; i<pCurve->n;i++) {
			To.x = int(pCurve->x[i]/m_scalex+m_offset.x);
			To.y = int(pCurve->y[i]/m_scaley+m_offset.y);
			if(rViewRect.PtInRect(From) && rViewRect.PtInRect(To)){
				pDC->LineTo(To);
				if(pCurve->PointsVisible()) 
					pDC->Rectangle(From.x-ptside,From.y-ptside, From.x+ptside,From.y+ptside);
			}
			else if(rViewRect.PtInRect(From) && !rViewRect.PtInRect(To)){
				x = From.x;
				y = From.y;
				if(Intersect(x,y, rViewRect, From, To)){
					pDC->LineTo(x,y);
					if(pCurve->PointsVisible()) 
						pDC->Rectangle(From.x-ptside,From.y-ptside, From.x+ptside,From.y+ptside);
					
				}
			}
			else if(!rViewRect.PtInRect(From) && rViewRect.PtInRect(To)){
				x = From.x;
				y = From.y;
				if(Intersect(x,y, rViewRect, From, To)){
					pDC->MoveTo(x,y);
					pDC->LineTo(To);
					if(pCurve->PointsVisible())
						pDC->Rectangle(To.x-ptside,To.y-ptside, To.x+ptside,To.y+ptside);
				}
			}
			else pDC->MoveTo(To);
			From = To;
		}
		if(pCurve->PointsVisible() && rViewRect.PtInRect(To)) 
			pDC->Rectangle(To.x-ptside,To.y-ptside, To.x+ptside,To.y+ptside);
	}
	else if(pCurve->n == 1 && pCurve->IsVisible() && pCurve->PointsVisible()){
		To.x = int(pCurve->x[0]/m_scalex+m_offset.x);
		To.y = int(pCurve->y[0]/m_scaley+m_offset.y);
		if(rViewRect.PtInRect(To))
			pDC->Rectangle(To.x-ptside,To.y-ptside, To.x+ptside,To.y+ptside);
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	CurvePen.DeleteObject();
	FillBrush.DeleteObject();
}


void Graph::DrawLegend(CDC* pDC, bool bIsPrinting, CPoint &Place)
{
	int LegendSize, ypos;
	COLORREF color;
	int style, width;
	LOGFONT TempFont;
	memcpy(&TempFont, &m_LegendLogFont, sizeof(LOGFONT));

	if(bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*24;
		LegendSize = 1500;
		ypos = -500;
	}
	else{ 
//		TempFont.lfHeight = 12;
		LegendSize = 30;
		ypos = 12;
	}
	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);

	CFont* pOldFont = pDC->SelectObject(&ThisFont);
	CString strLabel;
	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);
	if(IsBlackAndWhite()) pDC->SetTextColor(0);
	else				  pDC->SetTextColor(m_LegendColor);
	
	CCurve* pCurve;

	int npos = 0;
	for (int nc=0; nc< m_oaCurves.GetSize(); nc++){
		pCurve = (CCurve*) m_oaCurves[nc];
		if(pCurve->IsVisible()){
			CString strong;
			strong = pCurve->GetTitle();
			if(pCurve->n>0 && strong.GetLength()){//is there anything to draw ?
				color = pCurve->GetColor();
				width = pCurve->GetWidth();
				style = pCurve->GetStyle();
				GetBWColor(color, style, width);

				LOGBRUSH lb;
				lb.lbStyle = BS_SOLID;
				lb.lbColor = color;

				CPen LegendPen(PS_GEOMETRIC | style,
							   GetPenWidth(width,m_bIsPrinting), &lb);

				CPen *pOldPen = pDC->SelectObject(&LegendPen);

				pDC->MoveTo(Place.x, Place.y + ypos*npos);
				pDC->LineTo(Place.x + (int)(LegendSize),
							Place.y + ypos*npos);
				pDC->TextOut(Place.x + (int)(1.5*LegendSize),
							 Place.y  + ypos*npos-(int)(ypos/2),
							 pCurve->GetTitle());
				pDC->SelectObject(pOldPen);
				npos++;
			}
		}
	}
	pDC->SelectObject(pOldFont);
	ThisFont.DeleteObject();
}


void Graph::DrawTitles(CDC *pDC)
{
	//draws the x & y axis name

	int XPosXTitle, YPosXTitle;
	int XPosYTitle, YPosYTitle;
	LOGFONT TempFont;
	memcpy(&TempFont, &m_TitleLogFont, sizeof(LOGFONT));

	if(m_bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*30;
		XPosXTitle =  250;
		YPosXTitle =  250;
		XPosYTitle = -500;
		YPosYTitle =  500;
	}
	else{ 
		XPosXTitle = 5;
		YPosXTitle = -10;
		XPosYTitle = -5;
		YPosYTitle = -10;
	}

	double xp;
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else					 xp = xmin;

	double yp;
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);
	CFont * pOldFont = pDC->SelectObject(&ThisFont);

	CString strLabel;

	if(IsBlackAndWhite()) pDC->SetTextColor(RGB(0,0,0));
	else pDC->SetTextColor(m_TitleColor);
	pDC->SetBkMode(TRANSPARENT);

	pDC->SetTextAlign(TA_LEFT);
	pDC->TextOut((int)(xmax/m_scalex) + m_offset.x + XPosXTitle,
				 (int)(yp/m_scaley)   + m_offset.y + YPosXTitle, m_XTitle);
	pDC->SetTextAlign(TA_CENTER);
	pDC->TextOut(m_offset.x + int(xp/m_scalex) + XPosYTitle,
				 m_top-YPosYTitle, m_YTitle);

	pDC->SelectObject(pOldFont);
	ThisFont.DeleteObject();
}

void Graph::GetTitleLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_TitleLogFont, sizeof(LOGFONT));
}

void Graph::SetTitleLogFont(LOGFONT *plgft){
	memcpy(&m_TitleLogFont, plgft, sizeof(LOGFONT));
}

void Graph::GetLabelLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_LabelLogFont, sizeof(LOGFONT));
}

void Graph::SetLabelLogFont(LOGFONT *plgft){
	memcpy(&m_LabelLogFont, plgft, sizeof(LOGFONT));
}

void Graph::GetLegendLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_LegendLogFont, sizeof(LOGFONT));
}

void Graph::SetLegendLogFont(LOGFONT *plgft){
	memcpy(&m_LegendLogFont, plgft, sizeof(LOGFONT));
}

COLORREF Graph::GetTitleColor(){
	return m_TitleColor;
}

void Graph::SetTitleColor(COLORREF crColor){
	m_TitleColor = crColor;
}

COLORREF Graph::GetLabelColor(){
	return m_LabelColor;
}

void Graph::SetLabelColor(COLORREF crColor){
	m_LabelColor = crColor;
}
COLORREF Graph::GetLegendColor(){
	return m_LegendColor;
}

void Graph::SetLegendColor(COLORREF crColor){
	m_LegendColor = crColor;
}

COLORREF Graph::GetAxisColor(){
	return m_AxisColor;
}

COLORREF Graph::GetBorderColor(){
	return m_BorderColor;
}

COLORREF Graph::GetBackColor(){
	return m_BkColor;
}

int Graph::GetAxisStyle(){
	return m_nStyle;
}
int Graph::GetAxisWidth(){
	return m_nWidth;
}

void Graph::SetAxisData(int s, int w, COLORREF clr){
	m_nStyle = s;
	m_nWidth = w;
	m_AxisColor = clr;
}

void Graph::SetAxisColor(COLORREF crColor){
	m_AxisColor = crColor;
}
void Graph::SetAxisStyle(int nStyle){
	m_nStyle = nStyle;
}
void Graph::SetAxisWidth(int nWidth){
	m_nWidth = nWidth;
}

void Graph::SetDefaults()
{
	m_BkColor = RGB(0,30,50);
	m_BorderColor = RGB(200,200,200);
	m_BorderStyle = PS_SOLID;
	m_BorderWidth = 3;

	m_LogPixelsY = 96;

	m_nStyle = PS_SOLID;
	m_nWidth = 1;

	m_bYInverted = false;

	CFont TmpFont;
	TmpFont.CreatePointFont(85, "Comic Sans MS");
	LOGFONT LgFt; 
	TmpFont.GetLogFont(&LgFt);

	SetTitleLogFont(&LgFt);
	SetLegendLogFont(&LgFt);
	
	LgFt.lfHeight = - MulDiv(8, m_LogPixelsY, 72);
	SetLabelLogFont(&LgFt);

	SetAxisColor(15780518);
	SetTitleColor(RGB(255,255,255));
	SetLabelColor(RGB(255,255,255));
	SetLegendColor(RGB(255,255,255));

	m_bXMajGrid = true;
	m_bYMajGrid = true;
	m_bXMinGrid = false;
	m_bYMinGrid = false;

	m_XMajStyle = PS_DASH;
	m_YMajStyle = PS_DASH;
	m_XMajWidth = 0;
	m_YMajWidth = 0;
	m_XMajClr   = RGB(120,120,120);
	m_YMajClr   = RGB(120,120,120);

	m_XMinStyle = PS_DASH;
	m_YMinStyle = PS_DASH;
	m_XMinWidth = 0;//-> half a point
	m_YMinWidth = 0;//-> half a point
	m_XMinClr   = RGB(70,70,70);
	m_YMinClr   = RGB(70,70,70);

	m_XMinorUnit = 0.1;
	m_YMinorUnit = 0.1;
}


/****************************************************************************

    Curve Class
    Copyright (C) 2003-2008 André Deperrois XFLR5@yahoo.com

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


// Curve.cpp: implementation of the CCurve class.
//
//////////////////////////////////////////////////////////////////////


CCurve::CCurve()
{
	CurveColor = RGB(255,0,0);
	memset(x,0,sizeof(x));
	memset(y,0,sizeof(y));
	n=0;
	m_strName = "";
	m_bIsVisible = true;
	m_bShowPoints = false;
	CurveWidth = 1;
	CurveStyle = PS_SOLID;
}

CCurve::~CCurve()
{
}

double CCurve::GetxMin()
{
	double xMin = 99999999.0;
//	if(n==0) xmin = .0; 
//	else
		for(int i=0; i<n;i++)
			xMin = __min(xMin, x[i]);
	return xMin;
}


double CCurve::GetxMax()
{
	double xMax = -99999999.0;
//	if(n==0) xmax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			xMax = __max(xMax, x[i]);
	return xMax;
}

double CCurve::GetyMin()
{
	double yMin = 99999999.0;
//	if(n==0) ymin = .0; 
//	else
		for(int i=0; i<n;i++)
			yMin = __min(yMin, y[i]);
	return yMin;
}


double CCurve::GetyMax()
{
	double yMax = -99999999.0;
//	if(n==0) ymax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			yMax = __max(yMax, y[i]);
	return yMax;
}


int CCurve::AddPoint(double xn, double yn)
{
	x[n] = xn;
	y[n] = yn;
	n++;
	return n;
}

void CCurve::ResetCurve()
{
	n = 0;
}

bool Graph::GetAutoX()
{
	return m_bAutoX;
}
bool Graph::GetAutoY()
{
	return m_bAutoY;
}
bool Graph::GetBorder()
{
	return m_bBorder;
}

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


void Graph::ResetLimits()
{
	ResetXLimits();
	ResetYLimits();
}

void Graph::ResetXLimits()
{
	if(m_bAutoX){
		xmin =  0.0;
		xmax =  0.1;
		xo   =  0.0;
	}
}

void Graph::ResetYLimits()
{
	if(m_bAutoY){
		ymin =   0.000;
		ymax =   0.001;
		yo   =   0.000;
	}
}

void Graph::SetX0(double f){
	xo = f;
}
void Graph::SetXMin(double f){
	xmin = f;
}
void Graph::SetXMax(double f){
	xmax = f;
}
void Graph::SetXUnit(double f){
	xunit = f;
}
void Graph::SetY0(double f){
	yo = f;
}
void Graph::SetYMin(double f){
	ymin = f;
}
void Graph::SetYMax(double f){
	ymax = f;
}
void Graph::SetYUnit(double f){
	yunit = f;
}
void Graph::SetXMinorUnit(double f){
	m_XMinorUnit = f;
}
void Graph::SetYMinorUnit(double f){
	m_YMinorUnit = f;
}
double Graph::GetX0(){
	return xo;
}
double Graph::GetXMin(){
	return xmin;
}
double Graph::GetXMax(){
	return xmax;
}
double Graph::GetXUnit(){
	return xunit;
}
double Graph::GetY0(){
	return yo;
}
double Graph::GetYMin(){
	return ymin;
}
double Graph::GetYMax(){
	return ymax;
}
double Graph::GetYUnit(){
	return yunit;
}
LPCTSTR  CCurve::GetTitle(){
	return m_strName;
}

void CCurve::SetTitle(LPCTSTR Title){
	m_strName = Title;
}

void CCurve::SetColor(COLORREF clr)
{
	CurveColor = clr;
}
void CCurve::SetStyle(int nStyle)
{
	CurveStyle = nStyle;
}
void CCurve::SetWidth(int nWidth)
{
	CurveWidth = nWidth;
}

COLORREF CCurve::GetColor(){
	return CurveColor;
}

void CCurve::GetBWStyle(COLORREF &color, int &style, int &width)
{
	color = CurveColor;
	style = CurveStyle;
	width = CurveWidth;
	GetBWColor(color, style, width);
}

int CCurve::GetStyle()
{
	return CurveStyle;
}

int CCurve::GetWidth()
{
	return CurveWidth;
}

void Graph::SetXTitle(LPCTSTR str)
{
	m_XTitle = str;
}

void Graph::SetYTitle(LPCTSTR str)
{
	m_YTitle = str;
}

void Graph::GetXTitle(CString &str)
{
	str = m_XTitle;
}

void Graph::GetYTitle(CString &str)
{
	str = m_YTitle;
}

bool Graph::GetInverted()
{
	if(m_bYInverted) return true;
	else return false;
}

void Graph::SetInverted(bool bInverted)
{
	m_bYInverted = bInverted;
}

bool CCurve::IsVisible()
{
	return m_bIsVisible;
}

void CCurve::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
}

void Graph::SetLogPixelsY(int n)
{
	m_LogPixelsY = n;
}

int Graph::GetLogPixelsY()
{
	return m_LogPixelsY;
}

void Graph::SetMargin(int m)
{
	m_iMargin = m;
}

int Graph::GetMargin()
{
	return m_iMargin;
}

void Graph::SetBkColor(COLORREF cr)
{
	m_BkColor = cr;
}

void Graph::SetBorderColor(COLORREF crBorder)
{
	m_BorderColor = crBorder;
}

void Graph::SetBorder(bool bBorder){
	m_bBorder = bBorder;
}

void Graph::SetXMajGrid(bool state, COLORREF clr, int style, int width){
	m_bXMajGrid = state;
	m_XMajClr   = clr;
	m_XMajStyle = style;
	m_XMajWidth = width;
}

void Graph::SetYMajGrid(bool state, COLORREF clr, int style, int width){
	m_bYMajGrid = state;
	m_YMajClr   = clr;
	m_YMajStyle = style;
	m_YMajWidth = width;
}

void Graph::GetXMajGrid(bool *pstate, COLORREF *pclr, int *pstyle, int *pwidth){
	*pstate = m_bXMajGrid;
	*pclr   = m_XMajClr;
	*pstyle = m_XMajStyle;
	*pwidth = m_XMajWidth;
}

void Graph::GetYMajGrid(bool *pstate, COLORREF *pclr, int *pstyle, int *pwidth){
	*pstate = m_bYMajGrid;
	*pclr   = m_YMajClr;
	*pstyle = m_YMajStyle;
	*pwidth = m_YMajWidth;
}

void Graph::SetXMinGrid(bool state, bool bAuto, COLORREF clr, int style, int width, double unit)
{
	m_bXMinGrid = state;
	m_bXAutoMinGrid = bAuto;
	m_XMinClr   = clr;
	m_XMinStyle = style;
	m_XMinWidth = width;
	if(unit>0.0) m_XMinorUnit  = unit;
}

void Graph::SetYMinGrid(bool state, bool bAuto, COLORREF clr, int style, int width, double unit)
{
	m_bYMinGrid = state;
	m_bYAutoMinGrid = bAuto;
	m_YMinClr   = clr;
	m_YMinStyle = style;
	m_YMinWidth = width;
	if(unit>0.0) m_YMinorUnit  = unit;
}

void Graph::GetXMinGrid(bool *pstate, bool *pbAuto, COLORREF *pclr, int *pstyle, int *pwidth, double *punit){
	*pstate = m_bXMinGrid;
	*pbAuto = m_bYAutoMinGrid;
	*pclr   = m_XMinClr;
	*pstyle = m_XMinStyle;
	*pwidth = m_XMinWidth;
	*punit  = m_XMinorUnit;
}

void Graph::GetYMinGrid(bool *pstate, bool *pbAuto, COLORREF *pclr, int *pstyle, int *pwidth, double *punit){
	*pstate = m_bYMinGrid;
	*pbAuto = m_bXAutoMinGrid;
	*pclr   = m_YMinClr;
	*pstyle = m_YMinStyle;
	*pwidth = m_YMinWidth;
	*punit  = m_YMinorUnit;
}

bool Graph::Serialize(CArchive &ar)
{
	int k;
	COLORREF cr, cr1, cr2;
	CString str;
	LOGFONT lgft;
	bool bs, ba;
	int b,a,s,w;
	double f;
	if(ar.IsStoring()){
		//write variables
		ar << GetAxisColor();
		ar << GetAxisStyle();
		ar << GetAxisWidth();

		ar << GetTitleColor();
		ar << GetLabelColor();
		ar << GetLegendColor();

		GetTitleLogFont(&lgft);
		SerializeLogFont(ar, &lgft);

		GetLabelLogFont(&lgft);
		SerializeLogFont(ar, &lgft);

		GetLegendLogFont(&lgft);
		SerializeLogFont(ar, &lgft);

		GetXMajGrid(&bs,&cr,&s,&w);
		if(bs) b = 1; else b = 0;
		ar << b << cr << s << w;
		GetYMajGrid(&bs,&cr,&s,&w);
		if(bs) b = 1; else b = 0;
		ar << b << cr << s << w;

		GetXMinGrid(&bs,&ba,&cr,&s,&w,&f);
		if(ba) a = 1; else a = 0;
		if(bs) b = 1; else b = 0;
		ar << b << a << cr << s << w << f;

		GetYMinGrid(&bs,&ba,&cr,&s,&w,&f);
		if(ba) a = 1; else a = 0;
		if(bs) b = 1; else b = 0;
		ar << b << a << cr << s << w << f;

		if(m_bBorder) b=1; else b=0;
		ar << b << m_BorderColor << m_BorderStyle << m_BorderWidth;
		ar << m_BkColor;

		if(m_bYInverted) b=1; else b=0;
		ar << b;
		return true;
	}
	else {
		//read variables
		try{
			ar >>  cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetAxisColor(cr);
			ar >> k;
			SetAxisStyle(k);
			ar >> k;
			SetAxisWidth(k);

			ar >>  cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetTitleColor(cr);
			ar >> cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetLabelColor(cr);
			ar >>  cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetLegendColor(cr);

			if(!SerializeLogFont(ar, &lgft)) return false;
			SetTitleLogFont(&lgft);

			if(!SerializeLogFont(ar, &lgft)) return false;
			SetLabelLogFont(&lgft);

			if(!SerializeLogFont(ar, &lgft)) return false;
			SetLegendLogFont(&lgft);

			ar >> b >> cr >> s >> w;
			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) bs = true;else bs = false;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(s <0 || s> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(w <0 || w> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetXMajGrid(bs,cr,s,w);
			ar >> b >> cr >> s >> w;
			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) bs = true;else bs = false;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(s <0 || s> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(w <0 || w> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetYMajGrid(bs,cr,s,w);

			ar >> b >> a >> cr >> s >> w >> f;
			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) bs = true;else bs = false;
			if(a!=0 && a!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (a) ba = true;else ba = false;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(s <0 || s> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(w <0 || w> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(f <0.0){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetXMinGrid(bs,ba,cr,s,w,f);

			ar >> b >> a >> cr >> s >> w >> f;
			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) bs = true;else bs = false;
			if(a!=0 && a!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (a) ba = true;else ba = false;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(s <0 || s> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(w <0 || w> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(f <0.0){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetYMinGrid(bs,ba,cr,s,w,f);

			ar >> b >> cr1 >> s >> w;

			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) m_bBorder = true; else m_bBorder = false;
			if(cr1 <0 || cr1> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetBorderColor(cr1);
			if(s <0 || s> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(w <0 || w> 10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetBorderStyle(s);
			SetBorderWidth(w);
			
			ar >> cr2;
			if(cr2 <0 || cr2> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			SetBkColor(cr2);

			ar >> b ;

			if(b!=0 && b!=1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if (b) m_bYInverted = true; else m_bYInverted = false;
			return true;
		}
		catch (CException *ex){
			ex->Delete();
			return false;
		}
	}
}


void Graph::CopySettings(Graph *pGraph)
{
	m_AxisColor     = pGraph->m_AxisColor;
	m_BkColor       = pGraph->m_BkColor;
	m_bBorder       = pGraph->m_bBorder;
	m_BorderColor   = pGraph->m_BorderColor;
	m_BorderStyle   = pGraph->m_BorderStyle;
	m_BorderWidth   = pGraph->m_BorderWidth;
	m_bXMajGrid     = pGraph->m_bXMajGrid;
	m_bXMinGrid     = pGraph->m_bXMinGrid;
	m_bYMajGrid     = pGraph->m_bYMajGrid;
	m_bYMinGrid     = pGraph->m_bYMinGrid;
	m_LabelColor    = pGraph->m_LabelColor;
	m_LabelLogFont  = pGraph->m_LabelLogFont;
	m_LegendColor   = pGraph->m_LegendColor;
	m_LegendLogFont = pGraph->m_LegendLogFont;
	m_TitleColor    = pGraph->m_TitleColor;
	m_TitleLogFont  = pGraph->m_TitleLogFont;
	m_nStyle        = pGraph->m_nStyle;
	m_nWidth        = pGraph->m_nWidth;
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
}

bool Graph::SerializeLogFont(CArchive &ar, LOGFONT *plgft){
	if(ar.IsStoring()){
		if(plgft->lfCharSet == 0) plgft->lfCharSet = DEFAULT_CHARSET;
		ar << plgft->lfCharSet;
		ar << plgft->lfClipPrecision;
		ar << plgft->lfEscapement;
		CString strong;
		strong = plgft->lfFaceName;//let MFC do the conversion...
		ar << strong.GetLength();
		ar.WriteString(strong);
		ar << plgft->lfHeight;
		ar << plgft->lfItalic;
		ar << plgft->lfOrientation;
		ar << plgft->lfOutPrecision;
		ar << plgft->lfPitchAndFamily;
		ar << plgft->lfQuality;
		ar << plgft->lfStrikeOut;
		ar << plgft->lfUnderline;
		ar << plgft->lfWeight;
		ar << plgft->lfWidth;
	}
	else{
		try{
			int n;
			ar >> plgft->lfCharSet;
			if(plgft->lfCharSet == 0) plgft->lfCharSet = DEFAULT_CHARSET;
			ar >> plgft->lfClipPrecision;
			ar >> plgft->lfEscapement;
			ar >> n;
			if(n>32){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}

			ar.ReadString(plgft->lfFaceName, n);

			ar >> plgft->lfHeight;
			ar >> plgft->lfItalic;
			ar >> plgft->lfOrientation;
			ar >> plgft->lfOutPrecision;
			ar >> plgft->lfPitchAndFamily;
			ar >> plgft->lfQuality;
			ar >> plgft->lfStrikeOut;
			ar >> plgft->lfUnderline;
			ar >> plgft->lfWeight;
			ar >> plgft->lfWidth;
		}
		catch (CException *ex){
			ex->Delete();
			return false;
		}
	}
	return true;
}


double Graph::ClientTox(int x)
{
	return ((double)x-(double)m_offset.x)*m_scalex;
}

double Graph::ClientToy(int y)
{
	return ((double)y-(double)m_offset.y)*m_scaley;
}

int Graph::xToClient(double x)
{
	return (int)(x/m_scalex + m_offset.x);
}

int Graph::yToClient(double y)
{
	return (int)(y/m_scaley + m_offset.y);
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
void Graph::SetWindow(double x1, double x2, double y1, double y2)
{
	m_bAutoX = false;
	m_bAutoY = false;
	xmin = x1;
	xmax = x2;
	ymin = y1;
	ymax = y2;
}


void CCurve::ShowPoints(bool bShow)
{
	m_bShowPoints = bShow;
}

int Graph::GetBorderStyle()
{
	return m_BorderStyle;
}

int Graph::GetBorderWidth()
{
	return m_BorderWidth;
}

void Graph::SetBorderWidth(int w)
{
	m_BorderWidth = w;
}

void Graph::SetBorderStyle(int s)
{
	m_BorderStyle = s;
}

double Graph::GetXScale()
{
	return m_scalex;
}

double Graph::GetYScale()
{
	return m_scaley;
}

CPoint Graph::GetOffset()
{
	return m_offset;
}

bool CCurve::PointsVisible()
{
	return m_bShowPoints;
}


int CCurve::GetClosestPoint(double xs)
{
	int ref = -1;
	double dist;
	double distmax = 1.e10;
	if (n<1) return -1;
	for(int i=0; i<n; i++)
	{
		dist = abs(xs-x[i]);
		if (dist<distmax){
			distmax = dist;
			ref = i;
		}
	}
	return ref;
}
void CCurve::Copy(CCurve *pCurve)
{
	if(!pCurve) return;
	int i;
	n  = pCurve->n;
	for (i=0; i<n ;i++)
	{
		x[i] = pCurve->x[i];
		y[i] = pCurve->y[i];
	}
	CurveColor = pCurve->CurveColor;
	CurveStyle = pCurve->CurveStyle;
	CurveWidth = pCurve->CurveWidth;
	m_bIsVisible = pCurve->m_bIsVisible;
	m_bShowPoints = pCurve->m_bShowPoints;
	m_strName = pCurve->m_strName;
}

CVector CCurve::GetPoint(int ref)
{
	CVector r;
	if(ref<0 || ref>=n){
		r.x = 0.0;
		r.y = 0.0;
	}
	else{
		r.x = x[ref];
		r.y = y[ref];
	}
	return r;
}

CVector CCurve::GetClosestRealPoint(double xs)
{
	CVector r;
	int ref = -1;
	double dist;
	double distmax = 1.e10;
	if (n<1) {
		r.x = 0.0;
		r.y = 0.0;
		return r;
	}
	for(int i=0; i<n; i++){
		dist = abs(xs-x[i]);
		if (dist<distmax){
			distmax = dist;
			ref = i;
		}
	}
	
	r.x = x[ref];
	r.y = y[ref];
	
	return r;
}

void Graph::Highlight(CDC* pDC, CCurve *pCurve, int ref)
{
	int x = int(pCurve->x[ref]/m_scalex)  +m_offset.x;
	int y = int(pCurve->y[ref]/m_scaley)  +m_offset.y;

	CPen HighlightPen(PS_SOLID, 2, RGB(255,100,100));
	CPen *pOldPen = pDC->SelectObject(&HighlightPen);
	CRect r;
	r.left   = x-3;
	r.right  = x+3;
	r.top    = y-3;
	r.bottom = y+3;
	pDC->Rectangle(&r);
	pDC->SelectObject(pOldPen);
}

void Graph::GetDrawRect(CRect &Rect)
{
	Rect.CopyRect(&m_rDrawRect);
}

void Graph::SetDrawRect(CRect &Rect)
{
	m_rDrawRect = Rect;
}

bool Graph::IsInDrawRect(CPoint pt)
{
	if(m_rDrawRect.PtInRect(pt)) return true;
	else return false;
}

bool Graph::IsInDrawRect(int x, int y)
{
	CPoint pt(x,y);
	if(m_rDrawRect.PtInRect(pt)) return true;
	else return false;
}

void Graph::SetType(int type)
{
	m_Type = type;
}

bool Graph::SetXScale()
{
	CCurve *pCurve;
	int nc;

	if(m_bAutoX)
	{	
		bool bCurve = false;

		if (m_oaCurves.GetSize())
		{
			//init only if we have a curve
			for (nc=0; nc < m_oaCurves.GetSize(); nc++)
			{
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>1){
					bCurve = true;
					break;//there is something to draw
				}
			}
		}
		if (bCurve)
		{
			Cxmin =  9999999.0;
			Cxmax = -9999999.0;
			for (nc=0; nc < m_oaCurves.GetSize(); nc++){
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>0){
					Cxmin = __min(Cxmin, pCurve->GetxMin());
					Cxmax = __max(Cxmax, pCurve->GetxMax());
				}
			}

			if(Cxmax<=Cxmin)
				Cxmax = (Cxmin+1.0)*2.0;
			
			if(m_Type == 1)
			{
				xmin = __min(xmin, Cxmin);
				xmax = __max(xmax, Cxmax);
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
		{// until things are made clear
			for (nc=0; nc < m_oaCurves.GetSize(); nc++)
			{
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>0)
				{
					xmin = __min(xmin, pCurve->x[0]);
					xmax = __max(xmax, pCurve->x[0]);
				}
			}
		}
		xo=0.0;

		if(abs((xmin-xmax)/xmin)<0.001)
		{
			if(abs(xmin)<0.00001) xmax = 1.0;
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

		if(m_w<=0) return false;

		m_scalex   = (xmax-xmin)/(double)m_w;


		//try to set an automatic scale for X Axis

		SetAutoXUnit();		

	}
	else
	{ 
		//scales are set manually
		if(m_w<=0) return false;
	
		m_scalex   =  (xmax-xmin)/(double)m_w;

		if (xunit<1.0)
		{
			exp_x = (int)log10(xunit)-1;
			exp_x=__max(-4, exp_x);
		}
		else exp_x = (int)log10(xunit);

	}

	//graph center position
	int Xg = int((m_rDrawRect.right + m_rDrawRect.left)/2);
	// curves center position
	int Xc = (int)((xmin+xmax)/2.0/m_scalex);
	// center graph in drawing rectangle
	m_offset.x = Xg-Xc;

	if(m_bIsPrinting)
	{
		m_scalex   = m_scalex ;
		m_offset.x = m_offset.x;
	}

	return true;
}

bool Graph::SetYScale()
{
	CCurve *pCurve;
	if(m_bAutoY){
		
		bool bCurve = false;
		if (m_oaCurves.GetSize()){//init only if we have a curve
			for (int nc=0; nc < m_oaCurves.GetSize(); nc++){
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>0){
					bCurve = true;
					break;
				}
			}
		}
		if(bCurve){
			Cymin =  9999999.0;
			Cymax = -9999999.0;
			for (int nc=0; nc < m_oaCurves.GetSize(); nc++){
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>0){
					Cymin = __min(Cymin, pCurve->GetyMin());
					Cymax = __max(Cymax, pCurve->GetyMax());
				}
			}
			if(Cymax<=Cymin){
				Cymax = (Cymin+1.0)*2.0;
			}

			if(m_Type == 1){
				ymin = __min(ymin, Cymin);
				ymax = __max(ymax, Cymax);
			}
			else{
				ymin = Cymin;
				ymax = Cymax;
			}
			if(Cymin>=0.0) ymin = 0.0;
			if(Cymax<=0.0) ymax = 0.0;
		}
		else{// until things are made clear
			for (int nc=0; nc < m_oaCurves.GetSize(); nc++){
				pCurve = (CCurve*)m_oaCurves[nc];
				if (pCurve->IsVisible() && pCurve->n>0){
					ymin = __min(ymin, pCurve->y[0]);
					ymax = __max(ymax, pCurve->y[0]);
				}
			}
		}
		yo=0.0;

		if (abs((ymin-ymax)/ymin)<0.001){
			if(abs(ymin)<0.00001) ymax = 1.0;
			else {
				ymax = 2.0 * ymin;
				if(ymax < ymin){
					double tmp = ymax;
					ymax = ymin;
					ymin = tmp;
				}
			}
		}

		if(m_h<=0) return false;

		if (!m_bYInverted) {
			m_scaley   = -(ymax-ymin)/(double)m_h;
		}
		else{
			m_scaley   =  (ymax-ymin)/(double)m_h;
		}

		//try to set an automatic scale for Y Axis		

		SetAutoYUnit();


	}
	else{ //scales are set manually
		if(m_h<=0) return false;

		if (!m_bYInverted) {
			m_scaley   = -(ymax-ymin)/(double)m_h;
		}
		else{
			m_scaley   =  (ymax-ymin)/(double)m_h;
		}

		if (yunit<1.0){
			exp_y = (int)log10(yunit)-1;
			exp_y = __max(-4, exp_y);
		}
		else  exp_y = (int)log10(yunit);

	}

	//graph center position
	int Yg = int((m_rDrawRect.top + m_rDrawRect.bottom)/2);
	// curves center position
	int Yc = (int)((ymin+ymax)/2.0/m_scaley);
	// center graph in drawing rectangle
	m_offset.y = Yg-Yc;

	if(m_bIsPrinting){
		m_scaley   = -m_scaley;
		m_offset.y = -m_offset.y;
	}
	return true;
}

void Graph::SetAutoXUnit()
{
	int nxmin, nxmax;
	if(m_bIsPrinting) xunit = 2500.0*m_scalex;
	else xunit = 100.0*m_scalex;

	if (xunit<1.0) {
		exp_x = (int)log10(xunit)-1;
		exp_x=__max(-4, exp_x);
	}
	else exp_x = (int)log10(xunit);
	int main_x = (int)(xunit/pow(10.0, exp_x)*1.000001);


	if(main_x<2) 
		xunit = pow(10.0,exp_x);
	else if (main_x<5)  
		xunit = 2.0*pow(10.0,exp_x);
	else 
		xunit = 5.0*pow(10.0,exp_x);


	// finally reset scales to a multiple of units
	nxmin = int((xmin-xo)/xunit*1.0001);
	if (nxmin*xunit > xmin)  
		nxmin--;//*1.0000001 to account for doubleing point computation errors
	nxmax = int((xmax-xo)/xunit);
	if (nxmax*xunit*1.0001 < xmax) 
		nxmax++;
	int nx = nxmax - nxmin;
	xmin = xo + (double)nxmin * xunit;
	xmax = xo + (double)nxmax * xunit;
	m_scalex  =  (double)nx * xunit /(double)m_w;

}

void Graph::SetAutoYUnit()
{
	int nymin, nymax;
	if (m_bIsPrinting) yunit = abs(2000.0*m_scaley);
	else yunit = abs(100.0*m_scaley);

	if (yunit<1.0){
		exp_y = (int)log10(yunit)-1;
		exp_y = __max(-4, exp_y);
	}
	else  exp_y = (int)log10(yunit);
	int main_y = (int)(yunit/pow(10.0, exp_y));

	if(main_y<2) 
		yunit = pow(10.0,exp_y);
	else if (main_y<5)
		yunit = 2.0*pow(10.0,exp_y);
	else 
		yunit = 5.0*pow(10.0,exp_y);
	// finally reset scales to a multiple of units
	nymin = (int)((ymin-yo)/yunit*1.000001);
	if (nymin*yunit*1.0001 > ymin) nymin--;
	nymax = (int)((ymax-yo)/yunit);
	if (nymax*yunit*1.0001 < ymax) nymax++;
	int ny = nymax - nymin;
	ymax = yo + nymax * yunit;
	ymin = yo + nymin * yunit;	
	m_scaley   = -ny * yunit /(double)m_h;

	if(m_bYInverted) m_scaley = -m_scaley;
}

void Graph::SetAutoXMinUnit(bool bAuto)
{
	m_bXAutoMinGrid = bAuto;
	if(bAuto) m_XMinorUnit = xunit/5.0;
}

void Graph::SetAutoYMinUnit(bool bAuto)
{
	m_bYAutoMinGrid = bAuto;
	if(bAuto) m_YMinorUnit = yunit/5.0;
}

void Graph::SetGraphName(CString GraphName)
{
	m_GraphName = GraphName;
}

void Graph::GetGraphName(CString &GraphName)
{
	GraphName = m_GraphName;
}

void Graph::ExportToFile(CStdioFile &XFile, int FileType)
{
	int i,j;
	CCurve *pCurve;
	CString strong;

	for(i=0; i<(int)m_oaCurves.GetSize(); i++)
	{
		pCurve = GetCurve(i);
		if(pCurve && pCurve->n>0)
		{
			strong = pCurve->GetTitle();
			if(FileType==1)	XFile.WriteString("       x               "+strong+" \n");
			else            XFile.WriteString("x,"+strong+"\n");
			for(j=0; j<pCurve->n; j++)
			{
				if(FileType==1)	strong.Format("%14.5e     %14.5e\n",pCurve->x[j], pCurve->y[j]);
				else            strong.Format("%14.5e, %14.5e\n",pCurve->x[j], pCurve->y[j]);
				XFile.WriteString(strong);
			}
			XFile.WriteString("\n\n");
		}
	}
}

