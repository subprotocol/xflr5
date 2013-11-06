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


/**
 *@file
 *
 * This file contains the LLTAnalysisDlg class, which is used to perform LLT analysis
 *
 */
 
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

/**
 *@class LLTAnalysis
 *@brief The class is used to perform the LLT analysis of one operating point

	The analysis is managed from the instance of the LLTAnalysisDlg class.
	All the data is in International Standard units kg, m, s.
 */
class LLTAnalysis
{
	friend class MainFrame;
	friend class QMiarex;
	friend class LLTAnalysisDlg;
public:
    LLTAnalysis(void *pParent=NULL);

private:
    double AlphaInduced(int k);
    double Beta(int m, int k);
    double Eta(int m);
    void LLTComputeWing(double QInf, double Alpha, QString &ErrorMessage);
    void LLTInitCl(double &QInf, double const Alpha);
    void LLTInitialize(double QInf);
    int LLTIterate(double &QInf, double const Alpha);
    void LLTSetBending(double QInf);
    bool LLTSetLinearSolution(double Alpha);
    double Sigma(int m);

    void *m_pParent;                            /**< A void pointer to the instance of the LLTAnalysisDlg class */
    Wing * m_pWing;                             /**< A pointer to the Wing object for which the calculation shall be performed */
    WPolar *m_pWPolar;                          /**< A pointer to the WPolar object for which the calculation shall be performed */

    bool m_bCancel;                             /**< true if the user has cancelled the analysis */
    bool m_bConverged;                          /**< true if the analysis has converged  */
    bool m_bSkip;                               /**< true if the user has requested to skip this operating point */
    bool m_bWingOut;                            /**< true if the interpolation of viscous properties falls outside the polar mesh */
	double m_Ai[MAXSPANSTATIONS+1];		        /**< Induced Angle coefficient at the span stations */
	double m_BendingMoment[MAXSPANSTATIONS+1];	/**< bending moment at the span stations */
	double m_Cl[MAXSPANSTATIONS+1];		        /**< Local lift coefficient at the span stations */
	double m_Chord[MAXSPANSTATIONS+1];          /**< chord at the span stations */
    double m_CL;                                /**< The wing's lift coefficient */
	double m_Cm[MAXSPANSTATIONS+1];			    /**< Total pitching moment coefficient at the span stations */
	double m_CmAirf[MAXSPANSTATIONS+1];		    /**< Airfoil part of the pitching moment coefficient at the span stations */
    double m_GCm;                               /**< The wing's total pitching moment */
    double m_GRm;                               /**< The wing's total rolling moment */
    double m_GYm;                               /**< The wing's total yawing moment */
	double m_ICd[MAXSPANSTATIONS+1];		    /**< Induced Drag coefficient at the span stations */
    double m_ICm;                               /**< The wing's induced pitching moment */
    double m_InducedDrag;                       /**< The wing's induced drag coefficient @todo check*/
    int m_IterLim;                              /**< Maximum number of iterations in the calculation */
    double m_IYm;                               /**< The wing's induced yawing moment */
    QString m_LengthUnit;                       /**< Name of the user-defined length unit */
    double m_Maxa;                              /**< The max value of the difference of induced angle at any span station between two iterations */
    double m_mtoUnit;                           /**< Conversion factor for the display of results in the user-defined length unit*/
	double m_Offset[MAXSPANSTATIONS+1];         /**< offset at  the span stations */
	double m_PCd[MAXSPANSTATIONS+1];		    /**< Viscous Drag coefficient at the span stations */
    QList<void *> *m_poaPolar;			        /**< A pointer to the foil polar array */
    double m_QInf0;                             /**< The freestream velocity */
	double m_Re[MAXSPANSTATIONS+1];		        /**< Reynolds number at the span stations */
	double m_SpanPos[MAXSPANSTATIONS+1];		/**< Span position of the span stations */
	double m_StripArea[MAXSPANSTATIONS+1];		/** <Local strip area at the span stations */
	double m_Twist[MAXSPANSTATIONS+1];          /**< twist at the span stations */
    double m_VCm;                               /**< The wing's viscous pitching moment */
    double m_ViscousDrag;                       /**< The wing's viscous drag coefficient @todo check*/
    double m_VYm;                               /**< The wing's viscous yawing moment */
    double m_XCP;                               /**< The x-position of the center of pressure */
	double m_XCPSpanAbs[MAXSPANSTATIONS+1];	    /**< Center of Pressure pos at the span stations */
	double m_XCPSpanRel[MAXSPANSTATIONS+1];	    /**< Center of Pressure pos at the span stations */
	double m_XTrTop[MAXSPANSTATIONS+1];		    /**< Upper transition location at the span stations */
	double m_XTrBot[MAXSPANSTATIONS+1];		    /**< Lower transition location at the span stations */
    double m_YCP;                               /**< The y-position of the center of pressure */
    double m_ZCP;                               /**< The z-position of the center of pressure */
	
	static int s_NLLTStations;                  /**< The number of LLT stations in the spanwise direction */
    static double s_RelaxMax;                   /**< The relaxation factor for the iterations */
    static double s_CvPrec;                     /**< Precision criterion to stop the iterations. The difference in induced angle at any span point between two iterations should be less than the criterion */

};

#endif // LLTANALYSIS_H
