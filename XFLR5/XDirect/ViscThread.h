/****************************************************************************

    Iteration Thread Class
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


#pragma once

// ViscThread.h : header file
// 
 

#include "XFoil.h"

/////////////////////////////////////////////////////////////////////////////
// CViscThread thread

class CViscThread : public CWinThread{

	DECLARE_DYNCREATE(CViscThread)
protected:

// Attributes
public:
	XFoil *m_pXFoil;
	double m_fAlphaMin, m_fAlphaMax, m_fDAlpha;
	int m_Iterations, m_IterLim;

	bool m_bFinished;
	bool m_bSkip, m_bExit, m_bSuspend, m_bCalc;
	bool m_bAutoInitBL;

	CWnd* m_pParent;

	void SetAlphaMin(double alpha);
	void SetAlphaMax(double alpha);
	void SetDAlpha(double alpha);

// Operations
public:
	bool m_bType4;
	CViscThread(CWnd* pParent = NULL);
	virtual ~CViscThread();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViscThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViscThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	bool Iterate();
	bool AlphaLoop();
	bool ReLoop();
};

/////////////////////////////////////////////////////////////////////////////

