/****************************************************************************

    C3DPanelThread Class
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
#include "WPolar.h"

 
// C3DPanelThread

class C3DPanelThread : public CWinThread
{
	friend class C3DPanelDlg;

	DECLARE_DYNCREATE(C3DPanelThread)

protected:
	C3DPanelThread();           // constructeur protégé utilisé par la création dynamique
	virtual ~C3DPanelThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CWnd* m_pParent;
	CWnd* m_pMiarex;
	CWPolar * m_pWPolar;
	double m_Alpha, m_AlphaMax, m_DeltaAlpha;
	double m_QInf, m_QInfMax, m_DeltaQInf;

protected:
	DECLARE_MESSAGE_MAP()
	bool AlphaLoop(void);
	bool UnitLoop(void);
	bool ReLoop(void);
	bool m_bCancel;
	bool m_bSequence;
	bool m_bFinished;
};


