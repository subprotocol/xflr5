/****************************************************************************

	WingSection Class
	Copyright (C) 2013 Andre Deperrois adeperrois@xflr5.com

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


#include "WingSection.h"

WingSection::WingSection()
{
	m_NXPanels   = 0;
	m_NYPanels   = 0;
	m_XPanelDist = 0;
	m_YPanelDist = 0;

	m_Chord    = 0.0;
	m_Length   = 0.0;
	m_YPosition      = 0.0;
	m_YProj    = 0.0;
	m_Offset   = 0.0;
	m_Dihedral = 0.0;
	m_ZPos     = 0.0;
	m_Twist    = 0.0;

	m_RightFoilName.clear();
	m_LeftFoilName.clear();
}
