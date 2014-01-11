/****************************************************************************

    Polar Class
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

// Polar.h: interface for the CPolar class.
//
//////////////////////////////////////////////////////////////////////



#pragma once



#include "OpPoint.h"
#include <afxtempl.h>
#include "./XFoil.h"



class CPolar : public CObject  
{
//	DECLARE_SERIAL (CPolar);
	friend class CFoilAnalysisDlg;
	friend class CBatchThread;
	friend class CListPlrDlg;
	friend class CMainFrame;
	friend class CMiarex;
	friend class CCurveDlgBar;
	friend class CRenameFoilDlg;
	friend class CViscDlg;
	friend class CXDirect;
	friend class COperDlgBar;
	friend class CEditPlrDlg;
	friend class CNameDlg;
	
private:
	int m_Type;// the Polar format type - used for Serialize action
	int m_ReType;// the reynolds type
	int m_MaType;//the mach type
	double m_Reynolds;
	double m_ASpec;//type 4 polars
	double m_Mach;
	double m_ACrit;
	double m_XTop; 
	double m_XBot;

	double pi;

	int m_Style;
	int m_Width;
	COLORREF m_Color;
	// a polar is identified by the name of the Airfoil, and its own
	CString m_FoilName;
	CString m_PlrName;

	//we need to know if user wants it visible	
	bool m_bIsVisible;
	bool m_bShowPoints;

	//flap data
//	bool m_bFlap;
	// the rest is litterature
	CArray <double, double> m_Alpha;
	CArray <double, double> m_Cl;
	CArray <double, double> m_XCp;
	CArray <double, double> m_Cd;
	CArray <double, double> m_Cdp;
	CArray <double, double> m_Cm;
	CArray <double, double> m_XTr1, m_XTr2;
	CArray <double, double> m_HMom, m_Cpmn;
	CArray <double, double> m_ClCd, m_Cl32Cd;
	CArray <double, double> m_RtCl;
	CArray <double, double> m_Re;//type 4 polars

	CWnd* m_pFrame; //is mainframe
	
	void AddData(OpPoint* pOpPoint);
	void AddData(XFoil* pXFoil);
	void Export(CString FileName, int FileType);
	void ResetPolar();
	void Copy(CPolar *pPolar);

	CPolar();
	CPolar(CWnd* pParent);
	void Serialize(CArchive &ar);
	void SerializeOldFormat(CArchive &ar);
//	virtual void Serialize(CArchive &ar);
	virtual ~CPolar();

	void AddPoint(double Alpha, double Cd, double Cdp, double Cl, double Cm,
				  double Xtr1, double Xtr2, double HMom, double Cpmn, double Reynolds, double XCp);

protected:
	void GetLinearizedCl(double &Alpha0, double &slope);
	void GetAlphaLimits(double &amin, double &amax);
	void GetClLimits(double &Clmin, double &Clmax);
	void Remove(int i);
	double GetCm0();
	void GetBWStyle(COLORREF &color, int &style, int &width);
public:
	double GetZeroLiftAngle();
};

