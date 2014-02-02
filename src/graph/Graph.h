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


#ifndef GRAPH_H
#define GRAPH_H

#include <QFile>
#include <QPoint>
#include <QRect>
#include <QColor>
#include <QList>
#include "Curve.h"

class Graph 
{
	friend class GLGraph;
	friend class QGraph;
	friend class GraphWidget;
	friend class QXDirect;
public:

	bool GetAutoX();
	bool GetAutoY();
	bool GetAutoXMin();
	bool GetAutoYMin();
	bool GetBorder();
	bool GetInverted();
	bool IsInDrawRect(int const &x, int const &y);
	bool IsInDrawRect(QPoint const &pt);
	bool Init();

	double ClientTox(int x);
	double ClientToy(int y);

	double ClientTox(double x);
	double ClientToy(double y);

	int xToClient(double x);
	int yToClient(double y);

	void CopySettings(Graph* pGraph, bool bScales=true);
	void DeselectPoint();
	Curve * GetCurvePoint(const int &xClt, const int &yClt, int &nSel);
	Curve * GetClosestPoint(double const &x, double const &y, double &xSel, double &ySel, int &nSel);
	void ResetLimits();
	void ResetCurves();
	void Scale(double zoom);
	void Scalex(double zoom);
	void Scaley(double zoom);
	void SetAutoXMinUnit(bool bAuto);
	void SetAutoYMinUnit(bool bAuto);
	void SetAutoXUnit();
	void SetAutoYUnit();
	void SetAxisData(int s, int w, QColor clr);
	void SetBkColor(QColor cr);
	void SetBorderColor(QColor crBorder);
	void SetBorder(bool bBorder);
	void SetBorderStyle(int s);
	void SetBorderWidth(int w);
	void SetDrawRect(QRect &Rect);
	void SetMargin(int m);
	void SetInverted(bool bInverted);
	void SetType(int type);
	void SetXTitle(QString str);
	void SetYTitle(QString str);
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
	void SetXMajGrid(bool const &state, QColor const &clr, int const &style, int const &width);
	void SetYMajGrid(bool const &state, QColor const &clr, int const &style, int const &width);
	void SetXMinGrid(bool state, bool bAuto, QColor clr, int style, int width, double unit = -1.0);
	void SetYMinGrid(bool state, bool bAuto, QColor clr, int style, int width, double unit = -1.0);
	void SetAuto(bool bAuto);
	void SetAutoX(bool bAuto);
	void SetAutoY(bool bAuto);
	void SetAxisColor(QColor crColor);
	void SetAxisStyle(int nStyle);
	void SetAxisWidth(int Width);
	void SetTitleColor(QColor crColor);
	void SetLabelColor(QColor crColor);
	void SetWindow(double x1, double x2, double y1, double y2);

	QColor GetAxisColor();
	QColor GetBorderColor();
	QColor GetBackColor();
	QColor GetTitleColor();
	QColor GetLabelColor();

	int GetBorderStyle();
	int GetCurveCount();
	int GetMargin();
	int GetAxisStyle();
	int GetAxisWidth();
	int GetBorderWidth();
	int GetXVariable();
	int GetYVariable();
	void SetVariables(int const & X, int const & Y);
	void SetXVariable(int const & X);
	void SetYVariable(int const & Y);
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
	
	bool GetXMajGrid();
	bool GetYMajGrid();
	bool GetXMinGrid();
	bool GetYMinGrid();
	bool SelectPoint(QString const &CurveName, int sel);
	bool SetXScale();
	bool SetYScale();
	
	void SetXMajGrid(bool const &bGrid);
	void SetYMajGrid(bool const &bGrid);
	void SetXMinGrid(bool const &bGrid);
	void SetYMinGrid(bool const &bGrid);
	void GetXMajGrid(bool &bstate, QColor &clr, int &style, int &width);
	void GetYMajGrid(bool &bstate, QColor &clr, int &style, int &width);
	void GetXMinGrid(bool &bstate, bool &bAuto, QColor &clr, int &style, int &width, double &unit);
	void GetYMinGrid(bool &bstate, bool &bAuto, QColor &clr, int &style, int &width, double &unit);
	void GetXTitle(QString &str);
	void GetYTitle(QString &str);

	void GetClientRect(QRect &Rect);

	void SetDefaults();
	void SetGraphName(QString GraphName);
	void GetGraphName(QString &GraphName);

	QString GetGraphName();
	
	Curve* GetCurve(int nIndex);
	Curve* GetCurve(QString CurveTitle);
	Curve* AddCurve();
	Curve* AddCurve(Curve *pCurve);
	void DeleteCurve(int index);
	void DeleteCurve(Curve *pCurve);
	void DeleteCurve(QString CurveTitle);
	void DeleteCurves();
	void ResetXLimits();
	void ResetYLimits();

	int curveCount() {return m_oaCurves.size();}
	QList<void*> *getCurves(){return &m_oaCurves;}

	Graph();
	virtual ~Graph();

	static QColor m_CurveColors[10];

private:

	QString m_GraphName;        /** The graph's name, used for little else than to identify it in the settings file */

	int m_Type;

	QRect m_rCltRect;         //in screen coordinates

	bool m_bXMajGrid, m_bXMinGrid;
	bool m_bYMajGrid, m_bYMinGrid;

	bool m_bXAutoMinGrid, m_bYAutoMinGrid;

	bool m_bYInverted;
	bool m_bAutoX, m_bAutoY;
	bool m_bBorder;

	int m_AxisStyle;// axis style
	int m_AxisWidth;// axis width

	int m_XMajStyle, m_YMajStyle;
	int m_XMajWidth, m_YMajWidth;
	QColor m_XMajClr,   m_YMajClr;
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	QColor m_XMinClr,   m_YMinClr;
	double m_XMinorUnit,m_YMinorUnit;

	QString m_XTitle;
	QString m_YTitle;
	QList<void*> m_oaCurves;

	QPoint m_ptoffset; //in screen coordinates, w.r.t. the client area

	int exp_x, exp_y;
	double xo, yo;
	double xunit, yunit;
	double xmin, ymin, xmax, ymax;
	double Cxmin, Cxmax, Cymin, Cymax;
	double m_scalex, m_scaley;
	double m_h, m_w; //graph width and height
	int m_iMargin;
	QColor m_AxisColor;


	QColor m_TitleColor;
	QColor m_LabelColor;

	QColor m_BkColor;
	QColor m_BorderColor;
	int m_BorderStyle;
	int m_BorderWidth;

	int m_X, m_Y; //index of X and Y variables
};

#endif
