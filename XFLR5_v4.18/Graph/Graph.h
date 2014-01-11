/****************************************************************************

    Graph Classes
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
// Curve.h: interface for the CCurve class.
//
//////////////////////////////////////////////////////////////////////
#include"../misc/Vector.h"

#pragma once


class CCurve : public CObject  {

public:
	CVector GetClosestRealPoint(double xs);
	CVector GetPoint(int ref);

	bool PointsVisible();
	bool IsVisible();

	void Copy(CCurve *pCurve);
	void ShowPoints(bool bShow);
	void SetVisible(bool bVisible);
	void SetColor(COLORREF clr);
	void SetStyle(int nStyle);
	void SetWidth(int nWidth);
	void SetTitle(LPCTSTR Title);
	void GetBWStyle(COLORREF &color, int &style, int &width);
	void GetBWStyle();
	void ResetCurve();

	double GetxMin();
	double GetxMax();
	double GetyMin();
	double GetyMax();

	int AddPoint(double xn, double yn);
	int GetClosestPoint(double xs);
	int GetStyle();
	int GetWidth();

	COLORREF  GetColor();
	LPCTSTR  GetTitle();


	//	Curve Data
	int n;
	double x[600];
	double y[600];
	bool m_bShowPoints;

	CCurve();
	virtual ~CCurve();

private:	
	bool m_bIsVisible;
	COLORREF CurveColor;
	CString m_strName;
	int CurveStyle;
	int CurveWidth;
};

 
// Graph.h: interface for the Graph class.
//
//////////////////////////////////////////////////////////////////////

class Graph 
{
public: 

	bool GetAutoX();
	bool GetAutoY();
	bool GetBorder();
	bool GetInverted();
	bool IsInDrawRect(CPoint pt);
	bool IsInDrawRect(int x, int y);
	bool Init();
	bool Serialize(CArchive &ar);
	bool SerializeLogFont(CArchive &ar, LOGFONT *plgft);

	double ClientTox(int x);
	double ClientToy(int y);

	int xToClient(double x);
	int yToClient(double y);

	void CopySettings(Graph* pGraph);
	void Highlight(CDC* pDC, CCurve *pCurve, int ref);
	void ResetLimits();
	void Scale(double zoom);
	void Scalex(double zoom);
	void Scaley(double zoom);
	void SetAutoXMinUnit(bool bAuto);
	void SetAutoYMinUnit(bool bAuto);
	void SetAutoXUnit();
	void SetAutoYUnit();
	void SetAxisData(int s, int w, COLORREF clr);
	void SetBkColor(COLORREF cr);
	void SetBorderColor(COLORREF crBorder);
	void SetBorder(bool bBorder);
	void SetBorderStyle(int s);
	void SetBorderWidth(int w);
	void SetDrawRect(CRect &Rect);
	void SetMargin(int m);
	void SetInverted(bool bInverted);
	void SetType(int type);
	void SetXTitle(LPCTSTR str);
	void SetYTitle(LPCTSTR str);
	void SetX0(double f);
	void SetXMax(double f);
	void SetXMin(double f);
	void SetXMinorUnit(double f);
	void SetXUnit(double f);
	void SetY0(double f);
	void SetYMax(double f);
	void SetYMin(double f);
	void SetYMinorUnit(double f);
	void SetYUnit(double f);
	void SetXMajGrid(bool state, COLORREF clr, int style, int width);
	void SetYMajGrid(bool state, COLORREF clr, int style, int width);
	void SetXMinGrid(bool state, bool bAuto, COLORREF clr, int style, int width, double unit = -1.f);
	void SetYMinGrid(bool state, bool bAuto, COLORREF clr, int style, int width, double unit = -1.f);
	void SetAuto(bool bAuto);
	void SetAutoX(bool bAuto);
	void SetAutoY(bool bAuto);
	void SetLogPixelsY(int n);
	void SetAxisColor(COLORREF crColor);
	void SetAxisStyle(int nStyle);
	void SetAxisWidth(int nWidth);
	void SetTitleColor(COLORREF crColor);
	void SetLabelColor(COLORREF crColor);
	void SetLegendColor(COLORREF crColor);
	void SetTitleLogFont(LOGFONT *plgft);
	void SetLabelLogFont(LOGFONT *plgft);
	void SetLegendLogFont(LOGFONT *plgft);
	void SetWindow(double x1, double x2, double y1, double y2);

	COLORREF GetAxisColor();
	COLORREF GetBorderColor();
	COLORREF GetBackColor();
	COLORREF GetTitleColor();
	COLORREF GetLabelColor();
	COLORREF GetLegendColor();

	int GetMargin();
	int GetBorderWidth();
	int GetBorderStyle();
	int GetCurveCount();
	int GetLogPixelsY();
	int GetAxisStyle();
	int GetAxisWidth();
	double GetX0();
	double GetXMin();
	double GetXMax();
	double GetXUnit();
	double GetY0();
	double GetYMin();
	double GetYMax();
	double GetYUnit();
	double GetXScale();
	double GetYScale();
	bool SetXScale();
	bool SetYScale();
	void GetXMajGrid(bool *pstate, COLORREF *pclr, int *pstyle, int *pwidth);
	void GetYMajGrid(bool *pstate, COLORREF *pclr, int *pstyle, int *pwidth);
	void GetXMinGrid(bool *pstate, bool *pbAuto, COLORREF *pclr, int *pstyle, int *pwidth, double *punit);
	void GetYMinGrid(bool *pstate, bool *pbAuto, COLORREF *pclr, int *pstyle, int *pwidth, double *punit);
	void GetXTitle(CString &str);
	void GetYTitle(CString &str);

	void GetTitleLogFont(LOGFONT *plgft);
	void GetLabelLogFont(LOGFONT *plgft);
	void GetLegendLogFont(LOGFONT *plgft);
	void GetDrawRect(CRect &Rect);
	CPoint GetOffset();

	void SetDefaults();

	void DrawGraph(CDC *pDC, CRect* pDRect, bool bIsPrinting);
	void DrawLegend(CDC* pDC, bool bIsPrinting, CPoint &Place);

	void SetGraphName(CString GraphName);
	void GetGraphName(CString &GraphName);
	void ExportToFile(CStdioFile &XFile, int FileType);

	CCurve* GetCurve(int nIndex);
	CCurve* AddCurve();
	void DeleteCurve(int index);
	void DeleteCurves();
	void ResetXLimits();
	void ResetYLimits();

	Graph();
	virtual ~Graph();

	static COLORREF m_CurveColors[10];
	static int      m_CurveStyles[5];

private:
	void DrawXMajGrid(CDC *pDC);
	void DrawYMajGrid(CDC *pDC);
	void DrawXMinGrid(CDC *pDC);
	void DrawYMinGrid(CDC *pDC);
	void DrawAxes(CDC *pDC);
	void DrawCurve(int nIndex, CDC *pDC);
	void DrawXTicks(CDC *pDC);
	void DrawYTicks(CDC *pDC);
	void DrawTitles(CDC* pDC);

	CString m_GraphName;

	int m_Type;
	int m_LogPixelsY;
	int m_top;
	int m_left;

	CRect m_rDrawRect;
	bool m_bXMajGrid, m_bXMinGrid;
	bool m_bYMajGrid, m_bYMinGrid;

	bool m_bXAutoMinGrid, m_bYAutoMinGrid;

	bool m_bYInverted;
	bool m_bAutoX, m_bAutoY;
	bool m_bIsPrinting;
	bool m_bBorder;

	int m_nStyle;// axis style
	int m_nWidth;// axis width

	int m_XMajStyle, m_YMajStyle;
	int m_XMajWidth, m_YMajWidth;
	COLORREF m_XMajClr,   m_YMajClr;
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	COLORREF m_XMinClr,   m_YMinClr;
	double m_XMinorUnit,m_YMinorUnit;

	CString m_XTitle;
	CString m_YTitle;
	CObArray m_oaCurves;

	CPoint m_offset;

	int exp_x, exp_y;
	double xo, yo;
	double xunit, yunit;
	double xmin, ymin, xmax, ymax;
	double Cxmin, Cxmax, Cymin, Cymax;
	double m_scalex, m_scaley;
	int m_h, m_w; //graph width and height
	int m_iMargin;	COLORREF m_AxisColor;

	LOGFONT m_TitleLogFont;
	LOGFONT m_LabelLogFont;
	LOGFONT m_LegendLogFont;

	COLORREF m_TitleColor;
	COLORREF m_LabelColor;
	COLORREF m_LegendColor;

	COLORREF m_BkColor;
	COLORREF m_BorderColor;
	int m_BorderStyle, m_BorderWidth;

};

