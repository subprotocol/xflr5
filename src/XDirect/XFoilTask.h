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


#ifndef XFOILTASK_H
#define XFOILTASK_H


#include "XFoil.h"
#include <QRunnable>
#include "../Objects/Polar.h"

// this class runs an XFoil analysis in a thread separate from the main thread

class XFoilTask : public QRunnable
{
public:
	XFoilTask();

public:
	void run();
	bool AlphaSequence();

	bool Init(CFoil *pFoil, CPolar *pPolar);
	bool Iterate();

	static bool s_bCancel;
	static bool s_bAutoInitBL;
	static void *s_pBatchThreadDlg;
	int m_Id;
	int m_Iterations;
	CFoil *m_pFoil;
	CPolar *m_pPolar;
	bool m_bIsFinished;
	XFoil XFoilInstance;
};

#endif // XFOILTASK_H
