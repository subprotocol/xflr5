/****************************************************************************

    Polar Class
	Copyright (C) 2003 Andre Deperrois xflr5@yahoo.com

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


#ifndef POLAR_H
#define POLAR_H


#include "OpPoint.h"

#include <QList>


class CPolar
{
	friend class MainFrame;
	friend class QMiarex;
	friend class QXDirect;
	friend class BatchDlg;
	friend class XFoilAnalysisDlg;
	friend class EditPlrDlg;

private:
	static void *s_pMainFrame;
	
	int m_Type;// the Polar format type - used for Serialize action
	int m_ReType;// the reynolds type
	int m_MaType;//the mach type
	double m_Reynolds;
	double m_ASpec;//type 4 polars
	double m_Mach;
	double m_ACrit;
	double m_XTop; 
	double m_XBot;
	
	int m_Style;
	int m_Width;
	QColor m_Color;
	// a polar is identified by the name of the Airfoil, and its own
	QString m_FoilName;
	QString m_PlrName;
	
	//we need to know if user wants it visible	
	bool m_bIsVisible;
	bool m_bShowPoints;
	
	//flap data
	//	bool m_bFlap;
	// the rest is litterature
	QList <double> m_Alpha;
	QList <double> m_Cl;
	QList <double> m_XCp;
	QList <double> m_Cd;
	QList <double> m_Cdp;
	QList <double> m_Cm;
	QList <double> m_XTr1, m_XTr2;
	QList <double> m_HMom, m_Cpmn;
	QList <double> m_ClCd, m_Cl32Cd;
	QList <double> m_RtCl;
	QList <double> m_Re;//type 4 polars
	
	
	void AddData(OpPoint* pOpPoint);
	void AddData(void* ptrXFoil);
	void ExportPolar(QTextStream &out, int FileType);
	void ResetPolar();
	
	CPolar();
	
	void Serialize(QDataStream &ar, bool bIsStoring);
	void AddPoint(double Alpha, double Cd, double Cdp, double Cl, double Cm,
				  double Xtr1, double Xtr2, double HMom, double Cpmn, double Reynolds, double XCp);
	void Copy(CPolar *pPolar);

protected:
	void GetLinearizedCl(double &Alpha0, double &slope);
	void GetAlphaLimits(double &amin, double &amax);
	void GetClLimits(double &Clmin, double &Clmax);
	void Remove(int i);
	void Insert(int i);
	double GetCm0();
	void GetBWStyle(QColor &color, int &style, int &width);

public:
	double GetZeroLiftAngle();
};

#endif
