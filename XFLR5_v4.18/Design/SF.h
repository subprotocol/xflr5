/****************************************************************************

    Spline Foil Class
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

// SF.h: interface for the CSF class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "Spline.h"


class CSF  
{
	friend class CAFoil;
	friend class CSplinesCtrlDlg;
	friend class CAFoilCtrlBar;
	friend class CMainFrame;
	friend class CEditFoilDlg;

public:
	CSF();
	virtual ~CSF();

private:
	bool CompMidLine();
	bool DrawCtrlPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool Export(CStdioFile *pFile);
	bool InitSplineFoil();
	bool LoadFile(CStdioFile *pFile);
	bool SaveFile(CStdioFile *pFile);
	bool Serialize(CArchive &ar);	

	void Copy(CSF* pSF);
	void DrawMidLine(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void DrawFoil(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void DrawOutPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void ExportToBuffer(CFoil *pFoil);
	void SetViewRect(CRect rc);
	void Update(bool bExtrados);
	void UpdateSelected(double x, double y);
	void UpdateKnots();

	bool m_bModified;
	bool m_bVisible, m_bOutPoints, m_bCenterLine;
	
	int m_OutPoints;
	int	m_FoilStyle;
	int m_FoilWidth;
	double m_fCamber;
	double m_fThickness;
	double m_fxCambMax, m_fxThickMax;
	CString m_strFoilName;
	COLORREF m_FoilColor;
	CSpline m_Extrados;
	CSpline m_Intrados;
	CVector m_rpMid[1001];
};
