/****************************************************************************

	XFoilTask Class
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

/** @file This file implements the management task of an XFoil calculation. Used in multithreaded analysis. */

#ifndef XFOILTASK_H
#define XFOILTASK_H

#include "../../graph/Graph.h"
#include "../../graph/Curve.h"
#include "XFoil.h"
#include <QRunnable>
#include "../../objects/Polar.h"
#include "../../objects/Foil.h"

// this class runs an XFoil analysis in a thread separate from the main thread

/**
*@class XFoilTask
* This file implements the management task of an XFoil calculation. Used in multithreaded analysis. 
*/
class XFoilTask : public QRunnable
{
public:
	XFoilTask(QWidget *pParent = NULL);

public:
	void run();
	bool AlphaSequence();
	bool ReSequence();

	bool InitializeTask(Foil *pFoil, Polar *pPolar, bool bViscous=true, bool bInitBL=true, bool bFromZero=false);
	bool Iterate();

	void setSequence(double bAlpha, double SpMin, double SpMax, double SpInc);
	void setReRange(double ReMin, double ReMax, double ReInc);
	void setGraphPointers(Graph *pGraph=NULL, Curve *pCurve0=NULL, Curve *pCurve1=NULL);
	void traceLog(QString str);

	static bool s_bSkipPolar;
	static bool s_bCancel;          /**< true if the user has asked to cancel the analysis */
	static bool s_bAutoInitBL;      /**< true if the BL initialization is left to the code's decision */
	static int s_IterLim;
	static bool s_bSkipOpp;
	
	int m_Id;                /**< @todo check usage */
	int m_Iterations;        /**< The number of iterations already performed */
	Foil *m_pFoil;           /**< A pointer to the instance of the Foil object for which the calculation is performed */
	Polar *m_pPolar;         /**< A pointer to the instance of the Polar object for which the calculation is performed */
	bool m_bIsFinished;      /**< true if the calculation is over */
	XFoil XFoilInstance;     /**< An instance of the XFoil class specific for this object @todo check the behaviour of static variables in XFoil methods */

	QTextStream m_OutStream;
	QString m_OutMessage;

	Graph *m_pGraph;
	Curve *m_pCurve0, *m_pCurve1;

	double m_AlphaMin, m_AlphaMax, m_AlphaInc;
	double m_ClMin, m_ClMax, m_ClInc;
	double m_ReMin, m_ReMax, m_ReInc;

	bool m_bAlpha, m_bFromZero, m_bInitBL, m_bErrors;

	QList<OpPoint*> m_OppList;

	QWidget *m_pParent;
};

#endif // XFOILTASK_H
