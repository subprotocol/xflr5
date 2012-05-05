/****************************************************************************

    LLTAnalysis Class
	Copyright (C) 2011 Andre Deperrois adeperrois@xflr5.com

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
//
//Class used to perform LLT analysis
//GUI-independant
//

#ifndef LLTANALYSIS_H
#define LLTANALYSIS_H

#include "../params.h"
#include "../globals.h"
#include "../objects/Wing.h"
#include <QList>


class LLTAnalysis
{
	friend class MainFrame;
	friend class QMiarex;
	friend class LLTAnalysisDlg;
public:
    LLTAnalysis(void *pParent=NULL);

private:

    void LLTInitialize(double QInf);
    void LLTSetBending(double QInf);
    void LLTComputeWing(double QInf, double Alpha, QString &ErrorMessage);
    bool LLTSetLinearSolution(double Alpha);
    void LLTInitCl(double &QInf, double const Alpha);
    int LLTIterate(double &QInf, double const Alpha);

    double Sigma(int m);
    double Eta(int m);
    double Beta(int m, int k);
    double AlphaInduced(int k);

    void *m_pParent;
    CWing * m_pWing;
    CWPolar *m_pWPolar;

    QList<void *> *m_poaPolar;			// a pointer to the foil polar array

    static int s_NLLTStations;
    static double s_RelaxMax;
    static double s_CvPrec;

    bool m_bSkip;
    bool m_bCancel;
    bool m_bConverged;
    bool m_bWingOut;

    int m_IterLim;

	double m_Chord[MAXSPANSTATIONS+1];		//chord at stations
	double m_Offset[MAXSPANSTATIONS+1];		//offset at LLT stations
	double m_Twist[MAXSPANSTATIONS+1];		//twist at LLT stations
	double m_SpanPos[MAXSPANSTATIONS+1];		//Span position of  LLT stations
	double m_StripArea[MAXSPANSTATIONS+1];		//Local strip area at  LLT stations


	double m_Re[MAXSPANSTATIONS+1];
	double m_Cl[MAXSPANSTATIONS+1];
	double m_Ai[MAXSPANSTATIONS+1];
	double m_ICd[MAXSPANSTATIONS+1];		//Induced Drag coefficient at stations
	double m_PCd[MAXSPANSTATIONS+1];		//Viscous Drag coefficient at stations
	double m_Cm[MAXSPANSTATIONS+1];			//Total pitching moment coefficient at stations
	double m_CmAirf[MAXSPANSTATIONS+1];		//Aill part of Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSPANSTATIONS+1];	//Center of Pressure pos at stations
	double m_XCPSpanAbs[MAXSPANSTATIONS+1];	//Center of Pressure pos at stations
	double m_BendingMoment[MAXSPANSTATIONS+1];	//bending moment at stations
	double m_XTrTop[MAXSPANSTATIONS+1];		//Upper transition location at stations
	double m_XTrBot[MAXSPANSTATIONS+1];		//Lower transition location at stations

    double m_mtoUnit;
    QString m_LengthUnit;

    double m_QInf0;
    double m_Maxa;

    double m_CL;
    double m_InducedDrag;
    double m_ViscousDrag;

    double m_VYm, m_IYm, m_GYm;
    double m_VCm, m_ICm, m_GCm;
    double m_GRm;

    double m_XCP, m_YCP;
};

#endif // LLTANALYSIS_H
