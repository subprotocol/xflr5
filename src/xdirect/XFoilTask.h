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


#include "XFoil.h"
#include <QRunnable>
#include "../objects/Polar.h"

// this class runs an XFoil analysis in a thread separate from the main thread

/**
*@class XFoilTask
* This file implements the management task of an XFoil calculation. Used in multithreaded analysis. 
*/
class XFoilTask : public QRunnable
{
public:
	XFoilTask();

public:
	void run();
	bool AlphaSequence();

	bool Init(Foil *pFoil, Polar *pPolar);
	bool Iterate();

	static bool s_bCancel;          /**< true if the user has asked to cancel the analysis */
	static bool s_bAutoInitBL;      /**< true if the BL initialization is left to the code's decision */
	static void *s_pBatchThreadDlg; /**< A static pointer to the instance of the calling BathThreadDlg class */
	
	
	int m_Id;                /**< @todo check usage */
	int m_Iterations;        /**< The number of iterations already performed */
	Foil *m_pFoil;           /**< A pointer to the instance of the Foil object for which the calculation is performed */
	Polar *m_pPolar;         /**< A pointer to the instance of the Polar object for which the calculation is performed */
	bool m_bIsFinished;      /**< true if the calculation is over */
	XFoil XFoilInstance;     /**< An instance of the XFoil class specific for this object @todo check the behaviour of static variables in XFoil methods */
};

#endif // XFOILTASK_H
