/****************************************************************************

    CPlane Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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

// 
#include "Wing.h"
#include "Body.h"

/////////////////////////////////////////////////////////////////////////////
// CPlane dialog

class CPlane : public CObject
{
	friend class CMiarex;
	friend class CMainFrame;
//	friend class CNameDlg;
	friend class CPlaneDlg;
	friend class CPOperDlgBar;
	friend class CVLMDlg;
	friend class CUFOListDlg;
	friend class CBody;
	friend class C3DPanelDlg;
	friend class CBodyCtrlBar;
	friend class CVLMThread;

// Construction
public:
	CPlane();   // standard constructor


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlane)
	//}}AFX_VIRTUAL

// Implementation
protected:

	static CWnd * s_pMiarex;
	static CWnd * s_pFrame;
	CBody *m_pBody;
	CWing m_Wing, m_Wing2;
	CWing m_Stab;
	CWing m_Fin;
	bool m_bBody;
	bool m_bActive;
	bool m_bFin, m_bStab;
	bool m_bDoubleFin, m_bSymFin, m_bDoubleSymFin;
	bool m_bBiplane;
	double m_StabTilt;
	double m_FinTilt;
	double m_WingTilt, m_WingTilt2;
	double m_Volume;
	double m_XCmRef;
	double m_TailVolume;

	CVector m_LEStab, m_LEFin, m_LEWing, m_LEWing2;
	CString m_PlaneName;


private:
	bool SerializePlane(CArchive& ar) ;
	bool HasResults();
	void Duplicate(CPlane *pPlane);
	void ComputePlane(void);

public:

};

