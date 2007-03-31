/****************************************************************************

    CLLTThread Class
    Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// LLTThread.h : header file
//
#include "Wing.h"

/////////////////////////////////////////////////////////////////////////////
// CLLTThread thread

class CLLTThread : public CWinThread
{
	friend class CLLTDlg;
	DECLARE_DYNCREATE(CLLTThread)
protected:
//	CLLTThread();           // protected constructor used by dynamic creation
 
// Attributes
public:
 
// Operations
public:
	CLLTThread(CWnd* pParent = NULL);   
	virtual ~CLLTThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLLTThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	CWnd* m_pParent;
	CWnd* m_pMiarex;

	bool m_bCancel;
	bool m_bType4;
	bool m_bFinished;

	int m_IterLim;
//	Calculation data
	double pi;
	CWing *m_pWing;//pointer to the geometry class of the wing
	double m_Alpha;//Angle of Attack in °
	double m_AlphaMax;//Angle of Attack in °
	double m_DeltaAlpha;//Angle of Attack in °

	int m_Iter;

protected:
	bool ReLoop();
	bool AlphaLoop();
	// Generated message map functions
	//{{AFX_MSG(CLLTThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

	int Iterate();
	bool m_bSequence;
	bool m_bSkip;
	void Cancel();

};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
