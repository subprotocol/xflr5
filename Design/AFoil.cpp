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



#include <QtGui>

#include "../Globals.h"
#include "../MainFrame.h"
#include "../XDirect/NacaFoilDlg.h"
#include "AFoil.h"

AFoil::AFoil(QWidget *parent)
	: QWidget(parent)
{
	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_pSF = new CSF();
	m_pSF->m_bModified = false;
	m_pSF->InitSplineFoil();


	m_pPF = new CPF();
	m_pPF->m_bModified = false;
	m_pPF->InitSplinedFoil();

	m_bZoomPlus    = false;
	m_bZoomYOnly   = false;
	m_bTrans       = false;
	m_bNeutralLine = false;
	m_bScale       = true;
	m_bCircle      = false;
	m_bShowLegend  = true;
	m_bStored      = false;

	memset(&m_TmpPic,0, sizeof(Picture));

	m_LERad   = 1.0;

	m_bXGrid     = false;
	m_XGridUnit  = 0.05;
	m_XGridStyle = 1;
	m_XGridWidth = 1;
	m_XGridColor = QColor(150,150,150);
	m_bYGrid     = false;
	m_YGridUnit  = 0.05;
	m_YGridStyle = 1;
	m_YGridWidth = 1;
	m_YGridColor = QColor(150,150,150);

	m_bXMinGrid  = false;
	m_XMinorUnit = 0.01;
	m_XMinStyle  = 2;
	m_XMinWidth  = 1;
	m_XMinColor  = QColor(70,70,70);
	m_bYMinGrid  = false;
	m_YMinorUnit = 0.01;
	m_YMinStyle  = 2;
	m_YMinWidth  = 1;
	m_YMinColor  = QColor(70,70,70);

	m_NeutralStyle = 3;
	m_NeutralWidth = 1;
	m_NeutralColor = QColor(70,70,70);

	m_fScale    = 1.0;
	m_fRefScale = 1.0;

	m_fScaleY    = 1.0;
	m_ptOffset.rx() = 0;
	m_ptOffset.ry() = 0;

	m_pBufferFoil = new CFoil();
}
