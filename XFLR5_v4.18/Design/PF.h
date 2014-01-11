/****************************************************************************

	Splined Curve Foil class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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



// PF.h: interface for the CPF class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../XDirect/Foil.h"

class CSplinedPoints 
{
public:
	bool CompSlopes();
//	bool DrawControlPoint(CDC *pDC, int i, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool DrawControlPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool DrawSplines(CDC* pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool RemovePoint(int k);

	int InsertPoint(double x, double y);
	int IsControlPoint(CVector Real, double ZoomFactor);
	int IsControlPoint(double x, double y, double Zoom);
	int IsRearPoint(CVector Real, double ZoomFactor);

	double GetY(double x);

	void DrawRearPoint(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void Export(CStdioFile *pFile, bool bExtrados);
	void ExportToBuffer(CFoil *pFoil, bool bExtrados);

	CSplinedPoints();
	virtual ~CSplinedPoints();

	bool m_bOutPts;
	int m_iHighlight;
	int m_iSelect;
	int m_iPoints;				// number of control points
	int m_Freq;
	double m_Outy[1001];
	CVector m_RearPoint;
	CVector m_ctrlPoint[SPLINECONTROLSIZE]; //control points of the foil
	CVector m_Slope[SPLINECONTROLSIZE];    //slope vector at each point

	CRect m_rViewRect;

};


class CPF 
{
	friend class CAFoil;
	friend class CSplinesCtrlDlg;
	friend class CAFoilCtrlBar;
	friend class CMainFrame;
	friend class CEditFoilDlg;

public:
	CPF();
	virtual ~CPF();

private:
	bool m_bModified;
	bool m_bVisible, m_bOutPoints, m_bCenterLine;

	int m_OutPoints;

	int	m_FoilStyle;
	int m_FoilWidth;

	double m_fCamber;
	double m_fThickness;
	double m_fxCambMax;
	double m_fxThickMax;

	COLORREF m_FoilColor;

	CString m_strFoilName;

	CSplinedPoints m_Extrados;
	CSplinedPoints m_Intrados;
	CVector m_rpMid[1001];

	bool CompMidLine(bool first = false);
	bool DrawCtrlPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool Export(CStdioFile *pFile);
	bool InitSplinedFoil();
	bool LoadFile(CStdioFile *pFile);
	bool SaveFile(CStdioFile *pFile);
	bool Serialize(CArchive &ar);

	void Copy(CPF* pPF);
	void DrawFoil(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void DrawMidLine(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void ExportToBuffer(CFoil *pFoil);
	void SetOutPoints(bool state);
	void SetViewRect(CRect rc);
	void Update(bool bExtrados);

};
 
