/****************************************************************************

    OpPoint Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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


#ifndef OPPOINT_H
#define OPPOINT_H

#include "../params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <QDataStream>

class OpPoint
{

	friend class MainFrame;
	friend class Polar;
	friend class QXDirect;
	friend class ObjectPropsDlg;

	// An operating point is the result of an XFoil calculation
	// for a given Reynolds
public:
	OpPoint();

private:
	static void *s_pMainFrame;
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

	QString m_strFoilName;
	QString m_strPlrName;

	bool m_bIsVisible, m_bShowPoints;
	int m_Style, m_Width;
	QColor m_Color;
	

private:
	bool ExportOpp(QTextStream &out, QString Version, int FileType, bool bDataOnly=false);
	bool Serialize(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	void GetOppProperties(QString &OpPointProperties, bool bData=false);
};

#endif
