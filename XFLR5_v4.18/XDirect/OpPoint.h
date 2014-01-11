/****************************************************************************

    OpPoint Class
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

// OpPoint.h: interface for the OpPoint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


class OpPoint : public CObject  {
	friend class CXDirect;
	friend class CMainFrame;
	friend class CCurveDlgBar;
	friend class CPolar;

	// An operating point is the result of an XFoil calculation
	// for given Reynolds 
public:
	virtual ~OpPoint();
	OpPoint();

private:
	bool m_bVisc, m_bDispSurf;
	bool m_bTEFlap, m_bLEFlap;
//	int Format;
	int n, nd1, nd2, nd3;
	double Reynolds;
	double Mach;
	double Alpha; // index for polar
	double Cl, Cm, Cd, Cdp, Xtr1, Xtr2, ACrit;
	double m_XCP;
	double x[IQX], y[IQX];
//	double s[IQX];// foil coordinates
	double Cpv[IQX], Cpi[IQX];
	double Qv[IQX], Qi[IQX];
	double xd1[IQX], yd1[IQX];// first...
	double xd2[IWX], yd2[IWX];// ...second...
	double xd3[IWX], yd3[IWX];// ...and third part of the boundary layer
	double m_TEHMom, m_LEHMom, XForce, YForce;
	double Cpmn;

	CString m_strFoilName;
	CString m_strPlrName;

	bool m_bIsVisible, m_bShowPoints;
	int m_Style, m_Width;
	COLORREF m_Color;
	
	CWnd *m_pXDirect;
	

private:
	void Export(CString FileName, CString Version, int FileType);
	bool SerializeOpp(CArchive &ar, int ArchiveFormat=0);
};
