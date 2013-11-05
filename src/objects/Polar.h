/****************************************************************************

    Polar Class
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


/*! \file
 *
 * This class defines the polar object for the 2D analysis of foils
 *
 */



#ifndef POLAR_H
#define POLAR_H


/**
*@brief
 * This class defines the polar object for the 2D analysis of foils
 *
	The class stores both the analysis parameters and the analysis results.
	Each instance of this class is uniquely associated to an instance of a Foil object.
*/


#include "OpPoint.h"
#include <QList>


class Polar
{
	friend class MainFrame;
	friend class QMiarex;
	friend class QXDirect;
	friend class BatchDlg;
	friend class BatchThreadDlg;
	friend class XFoilTask;
	friend class XFoilAnalysisDlg;
	friend class EditPlrDlg;
	friend class ObjectPropsDlg;
	friend class XFoil;

private:
	static void *s_pMainFrame;

	int m_ReType;// the reynolds type
	int m_MaType;//the mach type
	double m_ASpec;//type 4 polars
	double m_Mach;
	double m_ACrit;
	double m_XTop; 
	double m_XBot;

	int m_Style;
	int m_Width;
    QColor m_Color;

    QString m_PlrName;

	bool m_bIsVisible;
	bool m_bShowPoints;


	void AddData(OpPoint* pOpPoint);
	void AddData(void* ptrXFoil);
	void ExportPolar(QTextStream &out, enumTextFileType FileType, bool bDataOnly=false);
	void GetPolarProperties(QString &PolarProperties, bool bData=false);
	void ResetPolar();

	Polar();

	bool Serialize(QDataStream &ar, bool bIsStoring);

	void AddPoint(double Alpha, double Cd, double Cdp, double Cl, double Cm,
				  double Xtr1, double Xtr2, double HMom, double Cpmn, double Reynolds, double XCp);
	void Copy(Polar *pPolar);

	static void GetPlrVariableName(int iVar, QString &Name);


protected:
	void Remove(int i);
	void Insert(int i);
	double GetCm0();
	void GetBWStyle(QColor &color, int &style, int &width);

public:
	double GetZeroLiftAngle();
	void GetAlphaLimits(double &amin, double &amax);
	void GetClLimits(double &Clmin, double &Clmax);
	void GetLinearizedCl(double &Alpha0, double &slope);

	QString m_FoilName;
	double m_Reynolds;
	enumPolarType m_PolarType;// the Polar format type - used for Serialize action

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

};

#endif
