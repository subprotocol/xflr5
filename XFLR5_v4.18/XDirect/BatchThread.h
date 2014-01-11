/****************************************************************************

	Batch Thread class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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
// BatchThread.h : header file
//


#include "XDirect.h"
/////////////////////////////////////////////////////////////////////////////
// CBatchThread thread

class CBatchThread : public CWinThread{
	friend class CBatchDlg;
	DECLARE_DYNCREATE(CBatchThread)
protected:
	CBatchThread(CWnd* pParent = NULL);           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBatchThread();

	// Generated message map functions
	//{{AFX_MSG(CBatchThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	bool AlphaLoop();
	bool Iterate();
	bool ReLoop();
	void CreatePolar(double Spec, double Mach, double NCrit);
	void Cancel();
	void SetPlrName();

	CStdioFile *m_pXFile;
	CWnd*  m_pParent;
	XFoil* m_pXFoil;
	CXDirect* m_pXDirect;
	CPolar* m_pCurPolar;

	double m_SpMin, m_SpMax, m_SpInc;
	double m_ReMin, m_ReMax, m_ReInc;
	
	double *m_ReList;
	double *m_MachList;
	double *m_NCritList;
	int m_NRe;

	bool m_bFromList;
	bool m_bInitBL;
	bool m_bShowTextOutput;
	bool m_bType4;
	bool m_bAlpha;
	bool m_bCalc;
	bool m_bAutotDelete;
	bool m_bCancel;
	bool m_bFromZero;

	bool m_bSkipPoint;
	bool m_bSkipPolar;

	int m_Iterations;
	int m_IterLim;
};

/////////////////////////////////////////////////////////////////////////////

