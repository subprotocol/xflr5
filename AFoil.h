/****************************************************************************

	AFoil Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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


#ifndef AFOIL_H
#define AFOIL_H

#include <QWidget>

#include "../Misc/FloatEdit.h"
#include "../Objects/Foil.h"
#include "../Objects/Sf.h"
#include "../Objects/Pf.h"
#include "../XDirect/XFoil.h"


struct Picture
{
	public:
		double xExt[SPLINECONTROLSIZE];
		double yExt[SPLINECONTROLSIZE];
		double xInt[SPLINECONTROLSIZE];
		double yInt[SPLINECONTROLSIZE];
		CVector ExtRearPt, IntRearPt;
		int m_iExt, m_iInt;
};

class AFoil : public QWidget
{
	friend class MainFrame;
	friend class Q2DWidget;
	Q_OBJECT
public:
	AFoil(QWidget *parent = NULL);


private:
	bool m_bScale;
	bool m_bSF;//else m_bPF
	bool m_bZoomPlus;
	bool m_bZoomYOnly;
	bool m_bNeutralLine;
	bool m_bTrans;
	bool m_bCircle;
	bool m_bStored;
	bool m_bXGrid,m_bYGrid;
	bool m_bXMinGrid, m_bYMinGrid;
	bool m_bShowLegend;

	int m_XGridStyle, m_YGridStyle;
	int m_XGridWidth, m_YGridWidth;
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	int m_NeutralStyle, m_NeutralWidth;
	int m_StackPos, m_StackSize;// undo : stack position and stack size

	double m_LERad;
	double m_fScale;
	double m_fScaleY;//ratio between y and x scales;
	double m_fRefScale;
	double m_XGridUnit, m_YGridUnit;
	double m_XMinorUnit, m_YMinorUnit;

	QList<void *> *m_poaFoil;
	XFoil *m_pXFoil;

	CSF *m_pSF;
	CPF *m_pPF;

	QPoint m_ptOffset;
	QPoint m_ptPopUp;
	QPoint m_PointDown;
	QRect m_rDrawRect;
	QRect m_rCltRect;
	QRect m_ZoomRect;
	CVector m_MousePos;
	CFoil *m_pBufferFoil;

	QColor m_XGridColor,m_YGridColor;
	QColor m_XMinColor,m_YMinColor;
	QColor m_NeutralColor;

	QCursor m_hcArrow;
	QCursor m_hcMove;
	QCursor m_hcCross;

	Picture m_TmpPic;
	Picture m_UndoPic[50];

	void * m_pMainFrame;//Mainframe
};

#endif // AFOIL_H



