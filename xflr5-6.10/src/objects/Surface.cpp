/****************************************************************************

	Surface Class
	Copyright (C) 2005-2014 Andre Deperrois adeperrois@xflr5.com

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

#include <QtDebug>
#include <QMessageBox>
#include <math.h>
#include "Surface.h"
#include "../objects/Quaternion.h"


CVector Surface::LA;//save time by preventing allocation & release of memory
CVector Surface::LB;
CVector Surface::TA;
CVector Surface::TB;
CVector Surface::VTemp;
CVector *Surface::s_pNode;
Panel *Surface::s_pPanel;


/**
 * The public constructor
 */
Surface::Surface()
{
	m_bTEFlap = false;

	m_Length   = 0.0;
	m_TwistA   = 0.0;
	m_TwistB   = 0.0;
	m_posATE   = 1.0;
	m_posBTE   = 1.0;
	m_NXPanels  = 1;
	m_NYPanels  = 2;
	m_NXLead    = 1;
	m_NXFlap    = 0;
	m_XDistType = COSINE;
	m_YDistType = UNIFORM;

	m_pLeftSurface = m_pRightSurface = NULL;
	m_pFoilA   = NULL;
	m_pFoilB   = NULL;
	m_NElements = 0;

	m_bIsInSymPlane = false;
	m_bIsTipLeft    = false;
	m_bIsTipRight   = false;

	m_bIsLeftSurf   = false;
	m_bIsRightSurf  = false;

	m_bIsCenterSurf = false;
	m_bJoinRight    = true;

	m_nFlapNodes  = 0;
	m_nFlapPanels = 0;

	memset(m_FlapNode, 0, sizeof(m_FlapNode));
	memset(m_FlapPanel, 0, sizeof(m_FlapPanel));

	m_xPointA.clear();
	m_xPointA.insert(0, 1000, 0);

	m_xPointB.clear();
	m_xPointB.insert(0, 1000, 0);

}


/**
 * Adds the reference of thE input panel to the array of flap panel indexes.
 * @param pPanel the pointer of the panel to add to the flap panel list.
 */
void Surface::AddFlapPanel(Panel *pPanel)
{
	bool bFound = false;
	int i;

	//Add Nodes

	for (i=0; i<m_nFlapNodes; i++)
	{
		bFound = bFound && pPanel->m_iLA==m_FlapNode[i];
		if(pPanel->m_iLA== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iLA;
		m_nFlapNodes++;
	}

	bFound = false;
	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iLB== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iLB;
		m_nFlapNodes++;
	}

	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iTA== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iTA;
		m_nFlapNodes++;
	}

	bFound = false;
	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iTB== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iTB;
		m_nFlapNodes++;
	}

	//Add panel;
	bFound=false;
	for(i=0; i<m_nFlapPanels; i++)
	{
		if(pPanel->m_iElement==m_FlapPanel[i])
		{
			bFound =true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapPanel[m_nFlapPanels] = pPanel->m_iElement;
		m_nFlapPanels++;
	}
}

/**
 * Copy the data from another Surface object to this Surface
 * @param Surface the source Surface from which the data shall be duplicated
 */
void Surface::Copy(Surface *pSurface)
{
	m_LA.Copy(pSurface->m_LA);
	m_LB.Copy(pSurface->m_LB);
	m_TA.Copy(pSurface->m_TA);
	m_TB.Copy(pSurface->m_TB);
	m_XDistType = pSurface->m_XDistType;
	m_YDistType = pSurface->m_YDistType;
	m_NElements = pSurface->m_NElements;

	m_Length    = pSurface->m_Length;
	m_NXPanels  = pSurface->m_NXPanels;
	m_NYPanels  = pSurface->m_NYPanels;
	m_pFoilA    = pSurface->m_pFoilA;
	m_pFoilB    = pSurface->m_pFoilB;
	m_TwistA    = pSurface->m_TwistA;
	m_TwistB    = pSurface->m_TwistB;

	Normal  = pSurface->Normal;
	NormalA = pSurface->NormalA;
	NormalB = pSurface->NormalB;

	m_bIsTipLeft    = pSurface->m_bIsTipLeft;
	m_bIsTipRight   = pSurface->m_bIsTipRight;
	m_bIsLeftSurf   = pSurface->m_bIsLeftSurf;
	m_bIsRightSurf  = pSurface->m_bIsRightSurf;
	m_bIsCenterSurf = pSurface->m_bIsCenterSurf;
	m_bJoinRight    = pSurface->m_bJoinRight;
	m_bIsInSymPlane = pSurface->m_bIsInSymPlane;

	m_bTEFlap       = pSurface->m_bTEFlap;
	m_nFlapNodes  = pSurface->m_nFlapNodes;
	m_nFlapPanels = pSurface->m_nFlapPanels;
	m_HingePoint  = pSurface->m_HingePoint;
	m_HingeVector = pSurface->m_HingeVector;

	memcpy(m_FlapNode, pSurface->m_FlapNode, sizeof(m_FlapNode));
	memcpy(m_FlapPanel, pSurface->m_FlapPanel, sizeof(m_FlapPanel));

	m_xPointA.clear();
	m_xPointB.clear();

	for(int ix=0; ix<pSurface->m_xPointA.size(); ix++)
		m_xPointA.append(pSurface->m_xPointA.at(ix));
	for(int ix=0; ix<pSurface->m_xPointB.size(); ix++)
		m_xPointB.append(pSurface->m_xPointB.at(ix));
}


/**
 * Returns the quarter-chord point of a specified strip
 * @param k the 0-based index of the strip for which the quarter-chord point shall be returned.
 * @param Pt the quarter-chord point
 * @param tau the relative span position of the Pt
 */
void Surface::GetC4(int k, CVector &Pt, double &tau)
{
	GetPanel(k,m_NXPanels-1,MIDSURFACE);
	double xl = (LA.x+LB.x)/2.0;
	double yl = (LA.y+LB.y)/2.0;
	double zl = (LA.z+LB.z)/2.0;
	GetPanel(k,0,MIDSURFACE);
	double xt = (TA.x+TB.x)/2.0;
	double yt = (TA.y+TB.y)/2.0;
	double zt = (TA.z+TB.z)/2.0;
	Pt.x = xl*.75 + xt*.25;
	Pt.y = yl*.75 + yt*.25;
	Pt.z = zl*.75 + zt*.25;

	tau = sqrt((Pt.y-m_LA.y)*(Pt.y-m_LA.y) + (Pt.z-m_LA.z)*(Pt.z-m_LA.z)) / m_Length;
}


/**
 * Returns the chord length of the specified strip
 * @param k the 0-based index of the strip for which the chord shall be returned.
 * @return the chord length
 */
double Surface::GetChord(int const &k)
{
	static double y1, y2;
	GetyDist(k, y1, y2);
	return GetChord((y1+y2)/2.0);
}


/**
 * Returns the chord length at the specified relative span position.
 * @param tau the relative percentage of the Surface's span length
 * @return the chord length
 */
double Surface::GetChord(double const &tau)
{
	//assumes LA-TB have already been loaded
	static CVector V1, V2;
	static double ChordA, ChordB;

	V1 = m_TA-m_LA;
	V2 = m_TB-m_LB;

	ChordA = V1.VAbs();
	ChordB = V2.VAbs();

	return ChordA + (ChordB-ChordA) * qAbs(tau);
}


/**
 * Returns the strip's offset in the x direction at the specified relative span position.
 * @param tau the relative percentage of the Surface's span length
 * @return the offset in the x-direction
 */
double Surface::GetOffset(double const &tau)
{
	//chord spacing
	return m_LA.x + (m_LB.x-m_LA.x) * qAbs(tau);
}

/**
 * Returns the area of the virtual foil at a specified relative span position. Used in Inertia calaculations.
 * @param tau the relative percentage of the Surface's span length
 * @return the cross area at the specified location
 */
double Surface::GetFoilArea(double const &tau)
{
	double area, chord;
	if(m_pFoilA && m_pFoilB)
	{
		chord = GetChord(tau);
		area = (m_pFoilA->GetArea() + m_pFoilB->GetArea())/2.0*chord*chord;//m2
		return area;
	}
	else
		return 0.0;
}

/**
 * Returns the normal vector at a specified relative span position.
 * @param tau the relative percentage of the Surface's span length
 * @return N the average normal at the specified location
 */
void Surface::GetNormal(double yrel, CVector &N)
{
	N = NormalA * (1.0-yrel) + NormalB * yrel;
	N.Normalize();
}




/**
 * Returns the leading point of the specified strip
 * @param k the 0-based index of the strip for which the leading point shall be returned.
 * @param C the strip's leading point.
 */
void Surface::GetLeadingPt(int k, CVector &C)
{
	GetPanel(k,m_NXPanels-1, MIDSURFACE);

	C.x    = (LA.x+LB.x)/2.0;
	C.y    = (LA.y+LB.y)/2.0;
	C.z    = (LA.z+LB.z)/2.0;
}



/**
 * Returns the trailing point of the specified strip
 * @param k the 0-based index of the strip for which the trailing point shall be returned.
 * @param C the strip's leading point.
 */
void Surface::GetTrailingPt(int k, CVector &C)
{
	GetPanel(k,0,MIDSURFACE);

	C.x    = (TA.x+TB.x)/2.0;
	C.y    = (TA.y+TB.y)/2.0;
	C.z    = (TA.z+TB.z)/2.0;
}


/**
 * Calculates the corner points of the panel with index k in the span direction and index l in the chordwise direction.
 * The point coordinates are loaded in the memeber variables LA, LB, TA, TB.
 *
 * Assumes the side points have been set previously
 *
 * @param k the index of the strip 0<=k<m_NYPanels
 * @param l the index of the panel in the chordwise direction. 0<=l<m_NXPanels
 * @param pos defines on which surface (BOTSURFACE, TOPSURFACE, MIDSURFACE) the node positions should be calculated.
 */
void Surface::GetPanel(int const &k, int const &l, enumPanelPosition const &pos)
{
	GetyDist(k,y1,y2);
	if(pos==MIDSURFACE)
	{
		LA.x = SideA[l+1].x * (1.0-y1) + SideB[l+1].x* y1;
		LA.y = SideA[l+1].y * (1.0-y1) + SideB[l+1].y* y1;
		LA.z = SideA[l+1].z * (1.0-y1) + SideB[l+1].z* y1;
		TA.x = SideA[l].x   * (1.0-y1) + SideB[l].x  * y1;
		TA.y = SideA[l].y   * (1.0-y1) + SideB[l].y  * y1;
		TA.z = SideA[l].z   * (1.0-y1) + SideB[l].z  * y1;
		LB.x = SideA[l+1].x * (1.0-y2) + SideB[l+1].x* y2;
		LB.y = SideA[l+1].y * (1.0-y2) + SideB[l+1].y* y2;
		LB.z = SideA[l+1].z * (1.0-y2) + SideB[l+1].z* y2;
		TB.x = SideA[l].x   * (1.0-y2) + SideB[l].x  * y2;
		TB.y = SideA[l].y   * (1.0-y2) + SideB[l].y  * y2;
		TB.z = SideA[l].z   * (1.0-y2) + SideB[l].z  * y2;
	}
	else if (pos==BOTSURFACE)
	{
		LA = SideA_B[l+1] * (1.0-y1) + SideB_B[l+1]* y1;
		TA = SideA_B[l]   * (1.0-y1) + SideB_B[l]  * y1;
		LB = SideA_B[l+1] * (1.0-y2) + SideB_B[l+1]* y2;
		TB = SideA_B[l]   * (1.0-y2) + SideB_B[l]  * y2;

		LA.x = SideA_B[l+1].x * (1.0-y1) + SideB_B[l+1].x* y1;
		LA.y = SideA_B[l+1].y * (1.0-y1) + SideB_B[l+1].y* y1;
		LA.z = SideA_B[l+1].z * (1.0-y1) + SideB_B[l+1].z* y1;
		TA.x = SideA_B[l].x   * (1.0-y1) + SideB_B[l].x  * y1;
		TA.y = SideA_B[l].y   * (1.0-y1) + SideB_B[l].y  * y1;
		TA.z = SideA_B[l].z   * (1.0-y1) + SideB_B[l].z  * y1;
		LB.x = SideA_B[l+1].x * (1.0-y2) + SideB_B[l+1].x* y2;
		LB.y = SideA_B[l+1].y * (1.0-y2) + SideB_B[l+1].y* y2;
		LB.z = SideA_B[l+1].z * (1.0-y2) + SideB_B[l+1].z* y2;
		TB.x = SideA_B[l].x   * (1.0-y2) + SideB_B[l].x  * y2;
		TB.y = SideA_B[l].y   * (1.0-y2) + SideB_B[l].y  * y2;
		TB.z = SideA_B[l].z   * (1.0-y2) + SideB_B[l].z  * y2;
	}
	else if (pos==TOPSURFACE)
	{
		LA.x = SideA_T[l+1].x * (1.0-y1) + SideB_T[l+1].x* y1;
		LA.y = SideA_T[l+1].y * (1.0-y1) + SideB_T[l+1].y* y1;
		LA.z = SideA_T[l+1].z * (1.0-y1) + SideB_T[l+1].z* y1;
		TA.x = SideA_T[l].x   * (1.0-y1) + SideB_T[l].x  * y1;
		TA.y = SideA_T[l].y   * (1.0-y1) + SideB_T[l].y  * y1;
		TA.z = SideA_T[l].z   * (1.0-y1) + SideB_T[l].z  * y1;
		LB.x = SideA_T[l+1].x * (1.0-y2) + SideB_T[l+1].x* y2;
		LB.y = SideA_T[l+1].y * (1.0-y2) + SideB_T[l+1].y* y2;
		LB.z = SideA_T[l+1].z * (1.0-y2) + SideB_T[l+1].z* y2;
		TB.x = SideA_T[l].x   * (1.0-y2) + SideB_T[l].x  * y2;
		TB.y = SideA_T[l].y   * (1.0-y2) + SideB_T[l].y  * y2;
		TB.z = SideA_T[l].z   * (1.0-y2) + SideB_T[l].z  * y2;
	}
}

/**
 * Returns the strip width at a specified
 * @param k the index of the strip 0<=k<m_NYPanels
 * @return the strip width
 */
double Surface::GetStripWidth(int const &k)
{
	GetPanel(k, 0, MIDSURFACE);
	return qAbs(LA.y-LB.y);
}

/**
 * Returns the postion of a surface point at the position specified by the input parameters.
 * @param xArel the relative position at the left Foil
 * @param xBrel the relative position at the right Foil
 * @param yrel the relative span position
 * @param Point a reference of the requested point's position
 * @param pos defines on which surface (BOTSURFACE, TOPSURFACE, MIDSURFACE) the point is calculated
 */
void Surface::GetSurfacePoint(double const &xArel, double const &xBrel, double const &yrel,
							  CVector &Point, int const &pos)
{
	static CVector APt, BPt;
	static double TopA, TopB, BotA, BotB;

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		TopA = m_pFoilA->GetUpperY(xArel)*GetChord(0.0);
		TopB = m_pFoilB->GetUpperY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * TopA;
		APt.y +=  Normal.y * TopA;
		APt.z +=  Normal.z * TopA;
		BPt.x +=  Normal.x * TopB;
		BPt.y +=  Normal.y * TopB;
		BPt.z +=  Normal.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		BotA = m_pFoilA->GetLowerY(xArel)*GetChord(0.0);
		BotB = m_pFoilB->GetLowerY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * BotA;
		APt.y +=  Normal.y * BotA;
		APt.z +=  Normal.z * BotA;
		BPt.x +=  Normal.x * BotB;
		BPt.y +=  Normal.y * BotB;
		BPt.z +=  Normal.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}


/**
 * Returns the postion of a surface point at the position specified by the input parameters.
 * @param xArel the relative position at the left Foil
 * @param xBrel the relative position at the right Foil
 * @param yrel the relative span position
 * @param Point a reference of the requested point's position
 * @param PtNormal the normal to the surface at the requested point
 * @param pos defines on which surface (BOTSURFACE, TOPSURFACE, MIDSURFACE) the point is calculated
 */
void Surface::GetSurfacePointNormal(double const &xArel, double const &xBrel, double const &yrel,
							 CVector &Point, CVector &PtNormal, int const &pos)
{
	static CVector APt, BPt, Nc, u;
	static double TopA, TopB, BotA, BotB, nxA, nxB, nyA, nyB, theta;
	static Quaternion q;
	
	//define the strip's normal
	GetNormal(yrel, Nc);

	//define the quaternion to rotate the unit vector (0,0,1) to Nc
	//use the dot product to get the rotation angle, and the crossproduct to get the rotation vector
	theta = acos(Nc.z);
	u.x = -Nc.y;
	u.y =  Nc.x;
	u.z =  0.0;
	q.Set(theta*180.0/PI, u);

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetUpperY(xArel, TopA, nxA, nyA);
		m_pFoilB->GetUpperY(xBrel, TopB, nxB, nyB);
		TopA *= GetChord(0.0);
		TopB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * TopA;
		APt.y +=  NormalA.y * TopA;
		APt.z +=  NormalA.z * TopA;
		BPt.x +=  NormalB.x * TopB;
		BPt.y +=  NormalB.y * TopB;
		BPt.z +=  NormalB.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetLowerY(xArel, BotA, nxA, nyA);
		m_pFoilB->GetLowerY(xBrel, BotB, nxB, nyB);
		BotA *= GetChord(0.0);
		BotB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * BotA;
		APt.y +=  NormalA.y * BotA;
		APt.z +=  NormalA.z * BotA;
		BPt.x +=  NormalB.x * BotB;
		BPt.y +=  NormalB.y * BotB;
		BPt.z +=  NormalB.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}

/**
 * Returns the chord length, cross-section area, and quarter-chord point of a given strip,
 * @param tau the relative percentage of the Surface's span length
 * @param Chord a reference to the chord length
 * @param Area a reference to the cross-section area
 * @param PtC4 a reference to the quarter-chord point
 */
void Surface::GetSection(double const &tau, double &Chord, double &Area, CVector &PtC4)
{
	//explicit double calculations are much faster than vector algebra
	LA.x = m_LA.x * (1.0-tau) + m_LB.x * tau;
	LA.y = m_LA.y * (1.0-tau) + m_LB.y * tau;
	LA.z = m_LA.z * (1.0-tau) + m_LB.z * tau;
	TA.x = m_TA.x * (1.0-tau) + m_TB.x * tau;
	TA.y = m_TA.y * (1.0-tau) + m_TB.y * tau;
	TA.z = m_TA.z * (1.0-tau) + m_TB.z * tau;
	PtC4.x = .75 * LA.x + .25 * TA.x;
	PtC4.y = .75 * LA.y + .25 * TA.y;
	PtC4.z = .75 * LA.z + .25 * TA.z;
	
	Chord = sqrt((LA.x-TA.x)*(LA.x-TA.x) + (LA.y-TA.y)*(LA.y-TA.y) + (LA.z-TA.z)*(LA.z-TA.z));

	if(m_pFoilA && m_pFoilB)
	{
		Area = (m_pFoilA->GetArea() * tau + m_pFoilB->GetArea() * (1.0-tau))*Chord*Chord;//m2
	}
	else
	{
		Area = 0.0;
	}
}


/**
 * Returns the absolute position of a specified strip.
 *
 * Returns the average span position of the strip; necessary for strips 'distorted' by the fuselage;
 *
 * @param k the 0-based index of the strip for which the position shall be returned.
 * @return the absolute position of the strip
 */
double Surface::GetStripSpanPos(int const &k)
{
	int  l;
	double YPos = 0.0;
	double ZPos = 0.0;

	for(l=0; l<m_NXPanels; l++)
	{
		GetPanel(k,l, MIDSURFACE);
		YPos += (LA.y+LB.y+TA.y+TB.y)/4.0;
		ZPos += (LA.z+LB.z+TA.z+TB.z)/4.0;
	}

	YPos /= m_NXPanels;
	ZPos /= m_NXPanels;

	YPos -= (m_LA.y + m_TA.y)/2.0;
	ZPos -= (m_LA.z + m_TA.z)/2.0;

	return sqrt(YPos*YPos+ZPos*ZPos);
}


/**
 * Returns the twist of the specified strip
 * @param k the 0-based index of the strip for which the leading point shall be returned.
 * @return the strip's twist.
 */
double Surface::GetTwist(int const &k)
{
	GetPanel(k, 0, MIDSURFACE);
	double y = (LA.y+LB.y+TA.y+TB.y)/4.0;
	return  m_TwistA + (m_TwistB-m_TwistA) *(y-m_LA.y)/(m_LB.y-m_LA.y);
}


/**
 * Returns the relative left and right span positions of a given strip
 * @param k the 0-based index of the strip.
 * @param y1 a reference to the relative left span position.
 * @param y2 a reference to the relative left span position.
 */
void Surface::GetyDist(int const &k, double &y1, double &y2)
{
	//leading edge

	double YPanels, dk;
	YPanels = (double)m_NYPanels;
	dk      = (double)k;

	if(m_YDistType==COSINE)
	{
		//cosine case
		y1  = 1.0/2.0*(1.0-cos( dk*PI   /YPanels));
		y2  = 1.0/2.0*(1.0-cos((dk+1)*PI/YPanels));
	}
	else if(m_YDistType== INVERSESINE)
	{
		//sine case
		y1  = 1.0*(sin( dk*PI   /2.0/YPanels));
		y2  = 1.0*(sin((dk+1)*PI/2.0/YPanels));
	}
	else if(m_YDistType==SINE)
	{
		//-sine case
		y1  = 1.0*(1.-cos( dk*PI   /2.0/YPanels));
		y2  = 1.0*(1.-cos((dk+1)*PI/2.0/YPanels));
	}
	else
	{
		//equally spaced case
		y1 =  dk     /YPanels;
		y2 = (dk+1.0)/YPanels;
	}
}


/**
 * Initializes the Surface
 */
void Surface::Init()
{
	CVector DL, DC;
	DL.Set(m_LB.x-m_LA.x, m_LB.y-m_LA.y, m_LB.z-m_LA.z);
	DC.Set(m_TA.x-m_LA.x, m_TA.y-m_LA.y, m_TA.z-m_LA.z);
//	Length = DL.VAbs();
//	Chord  = DC.VAbs();
//	u.Set(DC.x/Chord,  DC.y/Chord,  DC.z/Chord);
//	v.Set(DL.x/Length, DL.y/Length, DL.z/Length);

	m_bIsTipLeft   = false;
	m_bIsTipRight  = false;
	m_bIsLeftSurf  = false;
	m_bIsRightSurf = false;

	CVector LATB, TALB;

	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}


/**
 * Returns true if the specified panel is located on the T.E. flap
 * @param p the index of the panel
 * @return true if the panel is located on the T.E. flap
 */
bool Surface::IsFlapPanel(int const &p)
{
	int pp;
	for(pp=0; pp<m_nFlapPanels; pp++)
	{
		if (p==m_FlapPanel[pp]) return true;
	}
	return false;
}


/**
 * Returns true if the specified node is located on the T.E. flap
 * @param nNode the index of the node
 * @return true if the node is located on the T.E. flap
 */
bool Surface::IsFlapNode(int const &nNode)
{
	int pp;
	for(pp=0; pp<m_nFlapPanels; pp++)
	{
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iLA) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iLB) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iTA) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iTB) return true;
	}
	return false;
}


/** Clears the array of flap panel and node references */
void Surface::ResetFlap()
{
	int i;
	for(i=0; i<200; i++)
	{
		m_FlapPanel[i] = 30000;
		m_FlapNode[i]  = 30000;
	}
	m_nFlapPanels = 0;
	m_nFlapNodes = 0;
}

/**
 * Rotates a flap panels around its hinge axis.
 * @param Angle the rotation angle in degrees
 * @return false if the left and right Foil objects do not have an identical default flap angle, true otherwise.
 */
bool Surface::RotateFlap(double const &Angle)
{
	//The average angle between the two tip foil is cancelled
	//Instead, the Panels are rotated by Angle around the hinge point and hinge vector

	int k,l,p;
	double alpha0;
	Quaternion Quat;
	CVector R, S;
	
	p     = 0;

	if(m_pFoilA && m_pFoilB)
	{
		//get the approximate initial angle
		if(qAbs(m_pFoilA->m_TEFlapAngle - m_pFoilB->m_TEFlapAngle)>0.1)
		{
			QMessageBox msgBox;
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setWindowTitle(QObject::tr("Warning"));
			msgBox.setText(QObject::tr("Continuous foils for surface do not have the same initial flap angle... aborting\n"));
			msgBox.exec();

			return false;
		}
		alpha0 = (m_pFoilA->m_TEFlapAngle + m_pFoilB->m_TEFlapAngle)/2.0;

		Quat.Set(Angle-alpha0, m_HingeVector);


		for (k=0; k<m_nFlapNodes; k++)
		{
			R.x = s_pNode[m_FlapNode[k]].x - m_HingePoint.x;
			R.y = s_pNode[m_FlapNode[k]].y - m_HingePoint.y;
			R.z = s_pNode[m_FlapNode[k]].z - m_HingePoint.z;
			Quat.Conjugate(R,S);

			s_pNode[m_FlapNode[k]].x = S.x + m_HingePoint.x;
			s_pNode[m_FlapNode[k]].y = S.y + m_HingePoint.y;
			s_pNode[m_FlapNode[k]].z = S.z + m_HingePoint.z;
		}

		for(l=0; l<m_nFlapPanels; l++)
		{
			k = m_FlapPanel[l];
			if(s_pPanel[k].m_Pos==BOTSURFACE)
			{
				s_pPanel[k].SetPanelFrame(
					s_pNode[s_pPanel[k].m_iLB],
					s_pNode[s_pPanel[k].m_iLA],
					s_pNode[s_pPanel[k].m_iTB],
					s_pNode[s_pPanel[k].m_iTA]);
			}
			else
			{
				s_pPanel[k].SetPanelFrame(
					s_pNode[s_pPanel[k].m_iLA],
					s_pNode[s_pPanel[k].m_iLB],
					s_pNode[s_pPanel[k].m_iTA],
					s_pNode[s_pPanel[k].m_iTB]);
			}
		}
	}
	else p+= m_NYPanels * m_NXPanels;

	return true;
}


/**
 * Rotates the entire surface around the x-axis
 * @param O a point on the axis of rotation
 * @param XTilt the rotation angle in degrees
 */
void Surface::RotateX(CVector const&O, double XTilt)
{
	m_LA.RotateX(O, XTilt);
	m_LB.RotateX(O, XTilt);
	m_TA.RotateX(O, XTilt);
	m_TB.RotateX(O, XTilt);
	m_HingePoint.RotateX(O, XTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateX(Origin, XTilt);
	NormalA.RotateX(Origin, XTilt);
	NormalB.RotateX(Origin, XTilt);
	m_HingeVector.RotateX(Origin, XTilt);
}


/**
 * Rotates the entire surface around the y-axis
 * @param O a point on the axis of rotation
 * @param YTilt the rotation angle in degrees
 */
void Surface::RotateY(CVector const &O, double YTilt)
{
	m_LA.RotateY(O, YTilt);
	m_LB.RotateY(O, YTilt);
	m_TA.RotateY(O, YTilt);
	m_TB.RotateY(O, YTilt);
	m_HingePoint.RotateY(O, YTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateY(Origin, YTilt);
	NormalA.RotateY(Origin, YTilt);
	NormalB.RotateY(Origin, YTilt);
	m_HingeVector.RotateY(Origin, YTilt);
}

/**
 * Rotates the entire surface around the z-axis
 * @param O a point on the axis of rotation
 * @param ZTilt the rotation angle in degrees
 */
void Surface::RotateZ(CVector const &O, double ZTilt)
{
	m_LA.RotateZ(O, ZTilt);
	m_LB.RotateZ(O, ZTilt);
	m_TA.RotateZ(O, ZTilt);
	m_TB.RotateZ(O, ZTilt);
	m_HingePoint.RotateZ(O, ZTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateZ(Origin, ZTilt);
	NormalA.RotateZ(Origin, ZTilt);
	NormalB.RotateZ(Origin, ZTilt);
	m_HingeVector.RotateZ(Origin, ZTilt);
}

/**
 * Initializes the flap data
 */
void Surface::SetFlap()
{
	if(m_pFoilA && m_pFoilA->m_bTEFlap)
	{
		m_posATE = m_pFoilA->m_TEXHinge/100.0;
		if(m_posATE>1.0) m_posATE = 1.0; else if(m_posATE<0.0) m_posATE = 0.0;
	}
	else m_posATE = 1.0;

	if(m_pFoilB && m_pFoilB->m_bTEFlap)
	{
		m_posBTE = m_pFoilB->m_TEXHinge/100.0;
		if(m_posBTE>1.0) m_posBTE = 1.0; else if(m_posBTE<0.0) m_posBTE = 0.0;
	}
	else m_posBTE = 1.0;

	if(m_pFoilA && m_pFoilB) m_bTEFlap = m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap;
	else                     m_bTEFlap = false;


	if(m_pFoilA && m_pFoilB && m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap)
	{
		CVector HB;
		//create a hinge unit vector and initialize hinge moment
		GetSurfacePoint(m_posATE, m_posBTE, 0.0, m_HingePoint, 0);
		GetSurfacePoint(m_posATE, m_posBTE, 1.0, HB, 0);
		m_HingeVector = HB-m_HingePoint;
		m_HingeVector.Normalize();
	}
}


/** Sets the surface average normal vector */
void Surface::SetNormal()
{
	static CVector LATB, TALB;
	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}


/**
 * Creates the master points on the left and right ends.
 * One of the most difficult part of the code to implement. The algorithm still isn't very robust.
 * @param pBody a pointer to the Body object, or NULL if none.
 * @param dx the x-component of the translation to apply to the body.
 * @param dz the z-component of the translation to apply to the body.
 */
void Surface::SetSidePoints(Body * pBody, double dx, double dz)
{
	//creates the left and right tip points between which the panels will be interpolated
	int l;
	static double zA, zB;
	double cosdA, cosdB;
	static Body TBody;
	if(pBody)
	{
		TBody.Duplicate(pBody);
		TBody.Translate(dx, 0.0, dz);
	}

	cosdA = Normal.dot(NormalA);
	cosdB = Normal.dot(NormalB);
	chordA  = GetChord(0.0);//todo : compare with |m_LA-m_TA|
	chordB  = GetChord(1.0);

	cosdA = cosdB = 1.0;

	//SideA, SideB are mid points (VLM) or bottom points (3DPanels)
	//SideA_T, SideB_T, are top points (3DPanels);
	SideA.clear();
	SideA_T.clear();
	SideA_B.clear();
	SideB.clear();
	SideB_T.clear();
	SideB_B.clear();

	for(int i=0; i<m_NXPanels+1; i++)
	{
		SideA.append(CVector(0,0,0));
		SideA_B.append(CVector(0,0,0));
		SideA_T.append(CVector(0,0,0));
		SideB.append(CVector(0,0,0));
		SideB_B.append(CVector(0,0,0));
		SideB_T.append(CVector(0,0,0));
	}

	if(m_pFoilA && m_pFoilB)
	{
		zA = m_pFoilA->GetLowerY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetLowerY(m_xPointB[0])*chordB;
		SideA_B[0] = m_TA + NormalA * zA/cosdA;
		SideB_B[0] = m_TB + NormalB * zB/cosdB;

		zA = m_pFoilA->GetUpperY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetUpperY(m_xPointB[0])*chordB;
		SideA_T[0] = m_TA + NormalA * zA/cosdA;
		SideB_T[0] = m_TB + NormalB * zB/cosdB;

		zA = m_pFoilA->GetMidY(m_xPointA[0])*chordA;
		zB = m_pFoilB->GetMidY(m_xPointB[0])*chordB;
		SideA[0]   = m_TA + NormalA * zA/cosdA;
		SideB[0]   = m_TB + NormalB * zB/cosdB;
	}
	else
	{
		SideA[0]   = m_TA;
		SideB[0]   = m_TB;
		SideA_T[0] = m_TA;
		SideB_T[0] = m_TB;
		SideA_B[0] = m_TA;
		SideB_B[0] = m_TB;
	}

	if(pBody && m_bIsCenterSurf && m_bIsLeftSurf)
	{
		if(TBody.Intersect(SideA_B[0], SideB_B[0], SideB_B[0], false)) m_bJoinRight = false;
		if(TBody.Intersect(SideA_T[0], SideB_T[0], SideB_T[0], false)) m_bJoinRight = false;
		if(TBody.Intersect(SideA[0],   SideB[0],   SideB[0],   false)) m_bJoinRight = false;;
	}
	else if(pBody && m_bIsCenterSurf && m_bIsRightSurf)
	{
		TBody.Intersect(SideA_B[0], SideB_B[0], SideA_B[0], true);
		TBody.Intersect(SideA_T[0], SideB_T[0], SideA_T[0], true);
		TBody.Intersect(SideA[0],   SideB[0],   SideA[0],   true);
	}


	for (l=0; l<m_NXPanels; l++)
	{
		xLA = m_xPointA[l+1];
		xLB = m_xPointB[l+1];
		xTA = m_xPointA[l];
		xTB = m_xPointB[l];

		GetSurfacePoint(xLA, xLB, 0.0, LA, 0);
		GetSurfacePoint(xTA, xTB, 0.0, TA, 0);

		GetSurfacePoint(xLA, xLB, 1.0, LB, 0);
		GetSurfacePoint(xTA, xTB, 1.0, TB, 0);

		if (m_pFoilA && m_pFoilB)
		{
			//create bottom surface side points
			zA = m_pFoilA->GetLowerY(xLA)*chordA;
			zB = m_pFoilB->GetLowerY(xLB)*chordB;
			SideA_B[l+1]   = LA + NormalA * zA/cosdA;
			SideB_B[l+1]   = LB + NormalB * zB/cosdB;

			//create top surface side points
			zA = m_pFoilA->GetUpperY(xLA)*chordA;
			zB = m_pFoilB->GetUpperY(xLB)*chordB;
			SideA_T[l+1] = LA + NormalA * zA/cosdA;
			SideB_T[l+1] = LB + NormalB * zB/cosdB;

			//create middle surface side points
			zA = m_pFoilA->GetMidY(xLA)*chordA;
			zB = m_pFoilB->GetMidY(xLB)*chordB;
			SideA[l+1]   = LA + NormalA * zA/cosdA;
			SideB[l+1]   = LB + NormalB * zB/cosdB;
		}
		else
		{
			SideA[l+1]   = LA;
			SideB[l+1]   = LB;
			SideA_T[l+1] = LA;
			SideB_T[l+1] = LB;
			SideA_B[l+1] = LA;
			SideB_B[l+1] = LB;
		}

		if(pBody && m_bIsCenterSurf && m_bIsLeftSurf)
		{
			if(TBody.Intersect(SideA_B[l+1], SideB_B[l+1], SideB_B[l+1], false)) m_bJoinRight = false;
			if(TBody.Intersect(SideA_T[l+1], SideB_T[l+1], SideB_T[l+1], false)) m_bJoinRight = false;
			if(TBody.Intersect(SideA[l+1],   SideB[l+1],   SideB[l+1],   false)) m_bJoinRight = false;
		}
		else if(pBody && m_bIsCenterSurf && m_bIsRightSurf)
		{
			TBody.Intersect(SideA_B[l+1], SideB_B[l+1], SideA_B[l+1], true);
			TBody.Intersect(SideA_T[l+1], SideB_T[l+1], SideA_T[l+1], true);
			TBody.Intersect(SideA[l+1],   SideB[l+1],     SideA[l+1], true);
		}
	}

	//merge trailing edge points in case the foil has a T.E. gap

	CVector Node;

	Node = (SideA_B[0] + SideA_T[0])/2.0;
	SideA_B[0].Set(Node);
	SideA_T[0].Set(Node);

	Node = (SideB_B[0] + SideB_T[0])/2.0;
	SideB_B[0].Set(Node);
	SideB_T[0].Set(Node);
}


/**
 * Translates the entire Surface.
 * @param T the translation vector.
 */
void Surface::Translate(CVector const &T)
{
	m_LA.Translate(T);
	m_LB.Translate(T);
	m_TA.Translate(T);
	m_TB.Translate(T);
	m_HingePoint.Translate(T);
}

/**
 * Translates the entire Surface.
 * @param tx the x-component of the translation.
 */
void Surface::Translate(double tx, double ty, double tz)
{
	m_LA.Translate(tx, ty, tz);
	m_LB.Translate(tx, ty, tz);
	m_TA.Translate(tx, ty, tz);
	m_TB.Translate(tx, ty, tz);
	m_HingePoint.Translate(tx, ty, tz);
}


/**
 * Creates relative position of the master points at the left and right tip of the Surface.
 * The chordwise panel distribution is set i.a.w. with the flap hinges, if any.
 * The positions are stored in the member variables m_xPointA and m_xPointB.
 */
void Surface::CreateXPoints()
{
	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	if(m_pFoilA && m_pFoilA->m_bTEFlap) xHingeA=m_pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(m_pFoilB && m_pFoilB->m_bTEFlap) xHingeB=m_pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * (double)m_NXPanels*1.000123);// to avoid numerical errors if exact division
	NXFlapB = (int)((1.0-xHingeB) * (double)m_NXPanels *1.000123);

	if(m_pFoilA && m_pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(m_pFoilB && m_pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;

	NXLeadA = m_NXPanels - NXFlapA;
	NXLeadB = m_NXPanels - NXFlapB;

	m_NXFlap  = qMax(NXFlapA, NXFlapB);
	if(m_NXFlap>m_NXPanels/2) m_NXFlap=(int)m_NXPanels/2;
	m_NXLead  = m_NXPanels - m_NXFlap;

	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(m_XDistType==COSINE)
			m_xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(m_XDistType==COSINE)
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(m_XDistType==COSINE)
			m_xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}

	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(m_XDistType==COSINE)
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}

	m_xPointA[m_NXPanels] = 0.0;
	m_xPointB[m_NXPanels] = 0.0;
}


/**
 * Sets the surface twist - method 1
 */
void Surface::SetTwist1()
{
	static CVector A4, B4, L, U, T, O;
	O.Set(0.0,0.0,0.0);

	A4 = m_LA *3.0/4.0 + m_TA * 1/4.0;
	B4 = m_LB *3.0/4.0 + m_TB * 1/4.0;
	L = B4 - A4;
	L.Normalize();

	// create a vector perpendicular to NormalA and x-axis
	T.x = 0.0;
	T.y = +NormalA.z;
	T.z = -NormalA.y;
	//rotate around this axis
	U = m_LA-A4;
	U.Rotate(T, m_TwistA);
	m_LA = A4+ U;

	U = m_TA-A4;
	U.Rotate(T, m_TwistA);
	m_TA = A4 + U;

	NormalA.Rotate(T, m_TwistA);

	// create a vector perpendicular to NormalB and x-axis
	T.x = 0.0;
	T.y = +NormalB.z;
	T.z = -NormalB.y;

	U = m_LB-B4;
	U.Rotate(T, m_TwistB);
	m_LB = B4+ U;

	U = m_TB-B4;
	U.Rotate(T, m_TwistB);
	m_TB = B4 + U;

	NormalB.Rotate(T, m_TwistB);
}


/**
 * Sets the surface twist - method 2, @deprecated
 */
void Surface::SetTwist2()
{
	double xc4,zc4;
	CVector O(0.0,0.0,0.0);

	CVector LA = m_LA;
	CVector TA = m_TA;
	CVector LB = m_LB;
	CVector TB = m_TB;

	//"A" section first
	xc4 = m_LA.x + (m_TA.x-m_LA.x)/4.0;
	zc4 = m_LA.z + (m_TA.z-m_LA.z)/4.0;
	m_LA.x = xc4 + (LA.x-xc4) * cos(m_TwistA *PI/180.0) - (LA.z-zc4) * sin(m_TwistA *PI/180.0);
	m_LA.z = zc4 - (LA.x-xc4) * sin(m_TwistA *PI/180.0) + (LA.z-zc4) * cos(m_TwistA *PI/180.0);
	m_TA.x = xc4 + (TA.x-xc4) * cos(m_TwistA *PI/180.0) - (TA.z-zc4) * sin(m_TwistA *PI/180.0);
	m_TA.z = zc4 - (TA.x-xc4) * sin(m_TwistA *PI/180.0) + (TA.z-zc4) * cos(m_TwistA *PI/180.0);
	NormalA.RotateY(O, m_TwistA);

	//"B" Section next
	xc4 = m_LB.x + (m_TB.x-m_LB.x)/4.0;
	zc4 = m_LB.z + (m_TB.z-m_LB.z)/4.0;
	m_LB.x = xc4 + (LB.x-xc4) * cos(m_TwistB *PI/180.0) - (LB.z-zc4) * sin(m_TwistB *PI/180.0);
	m_LB.z = zc4 - (LB.x-xc4) * sin(m_TwistB *PI/180.0) + (LB.z-zc4) * cos(m_TwistB *PI/180.0);;
	m_TB.x = xc4 + (TB.x-xc4) * cos(m_TwistB *PI/180.0) - (TB.z-zc4) * sin(m_TwistB *PI/180.0);;
	m_TB.z = zc4 - (TB.x-xc4) * sin(m_TwistB *PI/180.0) + (TB.z-zc4) * cos(m_TwistB *PI/180.0);;
	NormalB.RotateY(O, m_TwistB);
}

















