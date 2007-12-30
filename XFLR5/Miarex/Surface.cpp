/****************************************************************************

    CSurface Class
    Copyright (C) 2005 André Deperrois XFLR5@yahoo.com

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

//////////////////////////////////////////////////////////////////////
//
// Surface.cpp: implementation of the CSurface class
// This class is associated to a surface used in VLM calculations
// The surface is always associated to a Wing "Panel" 
// and each of the wing's panels (left and right) is associated to 1 VLM surface
// The Surface's geometry is defined by
//    - its Leading edge  : m_LA, m_LB
//    - its Trailing edge : m_TA, m_TB
//    - its left and right twist
//    - its left and right foils
//    - its Normal vector
//    - its left and right normal vectors NormalA and NormalB equal to 
//      the average of the normals of two continuous surfaces ; used to
//	define junction between panels
//
// A suffixes refer to the left side, B refers to the right side
// L refers to the leading side refers to the right side
// The corner points are always defined from left tip to right tip,
// i.e. from y<0 to y>0
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "WPolar.h"
#include <math.h>
#include ".\surface.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector CSurface::LA;//save time by preventing allocation & release of memory
CVector CSurface::LB;
CVector CSurface::TA;
CVector CSurface::TB;
CVector CSurface::VTemp;
CVector CSurface::SideA[MAXCHORDPANELS];	 
CVector CSurface::SideB[MAXCHORDPANELS];	 
CVector CSurface::SideA_T[MAXCHORDPANELS];	 
CVector CSurface::SideB_T[MAXCHORDPANELS];	 

CSurface::CSurface()
{
	pi = 3.141592654;
	m_bTEFlap = false;

	m_SineFactor = 1.0;
	m_Dihedral = 0.0;
	m_Length   = 0.0;
	m_TwistA   = 0.0;
	m_TwistB   = 0.0;
	m_posATE   = 1.0;
	m_posBTE   = 1.0;
	m_NXPanels  = 1;
	m_NYPanels  = 2;
	m_NXLead    = 1;
	m_NXFlap    = 0;
	m_XDistType = 0;
	m_YDistType = 3;

	m_pFoilA   = NULL;
	m_pFoilB   = NULL;
	m_NElements = 0;

	m_bIsTipLeft  = false;
	m_bIsTipRight = false;

	m_bIsLeftSurf  = false;
	m_bIsRightSurf = false;

/*  3        equal         |   |   |   |   |   |   |   |   |

    2        sine          || |  |   |    |    |     |     |

    1        cosine        ||  |    |      |      |    |  ||

    0        equal         |   |   |   |   |   |   |   |   |

   -1        cosine        ||  |    |      |      |    |  ||

   -2       -sine          |     |     |    |    |   |  | ||

   -3        equal         |   |   |   |   |   |   |   |   |*/
}

CSurface::~CSurface()
{

}


void CSurface::GetLeadingPt(CVector &C, int k)
{
	GetPanel(k,m_NXPanels-1,VTemp , 0);

	C.x    = (LA.x+LB.x)/2.0;
	C.y    = (LA.y+LB.y)/2.0;
	C.z    = (LA.z+LB.z)/2.0;
}

void CSurface::GetTrailingPt(CVector &C, int k)
{
	GetPanel(k,0,VTemp,0);

	C.x    = (TA.x+TB.x)/2.0;
	C.y    = (TA.y+TB.y)/2.0;
	C.z    = (TA.z+TB.z)/2.0;
}

void CSurface::GetC4(int k, CVector &Pt)
{
	GetPanel(k,m_NXPanels-1,VTemp,0);
	double xl = (LA.x+LB.x)/2.0;
	double yl = (LA.y+LB.y)/2.0;
	double zl = (LA.z+LB.z)/2.0;
	GetPanel(k,0,VTemp,0);
	double xt = (TA.x+TB.x)/2.0;
	double yt = (TA.y+TB.y)/2.0;
	double zt = (TA.z+TB.z)/2.0;
	Pt.x = xl*.75 + xt*.25;
	Pt.y = yl*.75 + yt*.25;
	Pt.z = zl*.75 + zt*.25;
}

double CSurface::GetStripArea(int k)
{
	return Getdl(k) * GetChord(k);
}


int CSurface::Getk(double srel)
{
	//srel is relative length along DL
	int k;
	double s1, s2;
	if(srel<=0.0) return 0;
	if(srel>=1.0) return m_NYPanels-1;

	for(k=0; k<m_NYPanels; k++)
	{
		GetyDist(k,s1,s2);
//		s1 = m_LA.y + (m_LB.y-m_LA.y) * s1;
//		s2 = m_LA.y + (m_LB.y-m_LA.y) * s2;
		if(s1<=srel && srel<=s2) return k;
	}
	return -1;
}

void CSurface::Getyz(int k, double &y, double &z)
{
	double y1, y2;
	GetyDist(k,y1,y2);
	GetPoint(0.0, 0.0, y1, LA);
	GetPoint(0.0, 0.0, y2, LB);

	y = (LA.y+LB.y)/2.0;
	z = (LA.z+LB.z)/2.0;
}


double CSurface::Getdl(int k)
{
	CVector L;
	double y1, y2;
	GetyDist(k,y1,y2);
	GetPoint(0.0, 0.0, y1, LA);
	GetPoint(0.0, 0.0, y2, LB);
	L.x = 0.0;
	L.y = LB.y - LA.y;
	L.z = LB.z - LA.z;
	return L.VAbs();
}

double CSurface::GetTwist(int k)
{
	double y,z;
	Getyz(k,y,z);
	return  m_TwistA + (m_TwistB-m_TwistA) *(y-m_LA.y)/(m_LB.y-m_LA.y);
}

void CSurface::GetNormal(double yrel, CVector &N)
{
	N = NormalA * (1.0-yrel) + NormalB * yrel;
	N.Normalize();
}

void CSurface::SetTwist()
{
	double xc4,zc4;
	CVector LATB, TALB;
	CVector O(0.0,0.0,0.0);

	//"A" section first
	xc4 = m_LA.x + (m_TA.x-m_LA.x)/4.0;
	zc4 = m_LA.z + (m_TA.z-m_LA.z)/4.0;
	m_LA.x = xc4 + (m_LA.x-xc4) * cos(m_TwistA *pi/180.0);
	m_LA.z = zc4 - (m_LA.x-xc4) * sin(m_TwistA *pi/180.0);
	m_TA.x = xc4 + (m_TA.x-xc4) * cos(m_TwistA *pi/180.0);
	m_TA.z = zc4 - (m_TA.x-xc4) * sin(m_TwistA *pi/180.0);
	NormalA.RotateY(O, m_TwistA);

	//"B" Section next
	xc4 = m_LB.x + (m_TB.x-m_LB.x)/4.0;
	zc4 = m_LB.z + (m_TB.z-m_LB.z)/4.0;
	m_LB.x = xc4 + (m_LB.x-xc4) * cos(m_TwistB *pi/180.0);
	m_LB.z = zc4 - (m_LB.x-xc4) * sin(m_TwistB *pi/180.0);
	m_TB.x = xc4 + (m_TB.x-xc4) * cos(m_TwistB *pi/180.0);
	m_TB.z = zc4 - (m_TB.x-xc4) * sin(m_TwistB *pi/180.0);
	NormalB.RotateY(O, m_TwistB);
}

double CSurface::GetChord(int k)
{
	double y1, y2;
	GetyDist(k, y1, y2);
	return GetChord((y1+y2)/2.0);
}

double CSurface::GetChord(double tau)
{
	//assumes LA-TB have already been loaded
	CVector V1, V2;
	V1 = m_TA-m_LA;
	V2 = m_TB-m_LB;

	double ChordA = V1.VAbs();
	double ChordB = V2.VAbs();
	
	return ChordA + (ChordB-ChordA) * abs(tau);
}

double CSurface::GetOffset(double tau)
{
	//chord spacing
	return m_LA.x + (m_LB.x-m_LA.x) * fabs(tau);
}

void CSurface::SetNormal()
{
	CVector LATB, TALB;
	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}

void CSurface::Init()
{
	DL.Set(m_LB.x-m_LA.x,m_LB.y-m_LA.y,m_LB.z-m_LA.z);
	DC.Set(m_TA.x-m_LA.x,m_TA.y-m_LA.y,m_TA.z-m_LA.z);
	Length = DL.VAbs();
	Chord  = DC.VAbs();
	u.Set(DC.x/Chord,  DC.y/Chord,  DC.z/Chord);
	v.Set(DL.x/Length, DL.y/Length, DL.z/Length);

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

void CSurface::GetPanel(int k, int l, int pos)
{
	// Assumption : side points have been set for this surface
	// Loads the corner points of the panel k,l in PTA, PTB, PLA, PLB
	// Method used to generate the panels

	GetyDist(k,y1,y2);
	if(pos==0 || pos==-1)
	{
		LA = SideA[l+1] * (1.0-y1) + SideB[l+1]* y1;
		TA = SideA[l]   * (1.0-y1) + SideB[l]  * y1;
		LB = SideA[l+1] * (1.0-y2) + SideB[l+1]* y2;
		TB = SideA[l]   * (1.0-y2) + SideB[l]  * y2;
	}
	else	
	{
		LA = SideA_T[l+1] * (1.0-y1) + SideB_T[l+1]* y1;
		TA = SideA_T[l]   * (1.0-y1) + SideB_T[l]  * y1;
		LB = SideA_T[l+1] * (1.0-y2) + SideB_T[l+1]* y2;
		TB = SideA_T[l]   * (1.0-y2) + SideB_T[l]  * y2;
	}
}


void CSurface::GetPanel(int k, int l, CVector &dF, int pos)
{
	// Method used to get panel information for display
	// pos =  0 : mid camber line
	// pos =  1 : top
	// pos = -1 : bottom
	// loads the corner points of the panel k,l in TA, TB, LA, LB
	CVector PNA, PNB;
//	GetxDist(l, xLA, xTA, xLB, xTB);
	xLA = m_xPointA[l+1];
	xLB = m_xPointB[l+1];
	xTA = m_xPointA[l];
	xTB = m_xPointB[l];
	GetyDist(k,y1,y2);

	chordA  = GetChord(0.0);
	chordB  = GetChord(1.0);
	GetPoint(xLA, xLB, y1, LA); 
	GetPoint(xLA, xLB, y2, LB);
	GetPoint(xTA, xTB, y1, TA);
	GetPoint(xTA, xTB, y2, TB);

	GetNormal(y1, NA);
	GetNormal(y2, NB);

	if (m_pFoilA && m_pFoilB){
		if(pos==1){
			zA = m_pFoilA->GetUpperY(xLA)*chordA;
			zB = m_pFoilB->GetUpperY(xLB)*chordB;
		}
		else if(pos==-1){
			zA = m_pFoilA->GetLowerY(xLA)*chordA;
			zB = m_pFoilB->GetLowerY(xLB)*chordB;
		}
		else{
			zA = m_pFoilA->GetMidY(xLA)*chordA;
			zB = m_pFoilB->GetMidY(xLB)*chordB;
		}
		LA += NA * (zA+ (zB-zA)*y1);
		LB += NB * (zA+ (zB-zA)*y2);

		if(pos==1){
			zA = m_pFoilA->GetUpperY(xTA)*chordA;
			zB = m_pFoilB->GetUpperY(xTB)*chordB;
		}
		else if(pos==-1){
			zA = m_pFoilA->GetLowerY(xTA)*chordA;
			zB = m_pFoilB->GetLowerY(xTB)*chordB;
		}
		else{
			zA = m_pFoilA->GetMidY(xTA)*chordA;
			zB = m_pFoilB->GetMidY(xTB)*chordB;
		}
		TA += NA * (zA+ (zB-zA)*y1);
		TB += NB * (zA+ (zB-zA)*y2);
	}

/*	V1 = TB - LA;
	V2 = LB - TA;
	if(pos==1 || pos==0) dF = V1 * V2;
	else                 dF = V2 * V1;*/

	dF = (NA + NB) *0.5;
	dF.Normalize();

	if(pos==-1) dF.Set(-dF.x, -dF.y, -dF.z);
}


void CSurface::GetPoint(double xArel, double xBrel, double yrel, CVector &Point, int pos)
{
	CVector APt, BPt;

	APt = m_LA * (1.0-xArel) + m_TA * xArel;
	BPt = m_LB * (1.0-xBrel) + m_TB * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB){
		double TopA = m_pFoilA->GetUpperY(xArel)*GetChord(0.0);
		double TopB = m_pFoilB->GetUpperY(xBrel)*GetChord(1.0);
		APt +=  Normal * TopA;
		BPt +=  Normal * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB){
		double BotA = m_pFoilA->GetLowerY(xArel)*GetChord(0.0);
		double BotB = m_pFoilB->GetLowerY(xBrel)*GetChord(1.0);
		APt += Normal * BotA;
		BPt += Normal * BotB;
	}
	Point = APt * (1.0-yrel)+  BPt * yrel ;
}



void CSurface::SetSidePoints(int iAnalysisType)
{
	//creates the left and right tip points between which the panels will be interpolated
	int l;
	double cosdA = Normal.dot(NormalA);
	double cosdB = Normal.dot(NormalB);

	//SideA, SideB are mid points (VLM) or bottom points (3DPanels)
	//SideA_T, SideB_T, are top points (3DPanels);

	for (l=0; l<m_NXPanels; l++)
	{
		xLA = m_xPointA[l+1];
		xLB = m_xPointB[l+1];
		xTA = m_xPointA[l];
		xTB = m_xPointB[l];

		chordA  = GetChord(0.0);//TODO : compare with |m_LA-m_TA|
		chordB  = GetChord(1.0);

		GetPoint(xLA, xLB, 0.0, LA, 0); 
		GetPoint(xTA, xTB, 0.0, TA, 0);

		GetPoint(xLA, xLB, 1.0, LB, 0);
		GetPoint(xTA, xTB, 1.0, TB, 0);

		if (m_pFoilA && m_pFoilB)
		{
			if(iAnalysisType==3)
			{
				//create bottom surface points
				zA = m_pFoilA->GetLowerY(xLA)*chordA;
				zB = m_pFoilB->GetLowerY(xLB)*chordB;
				SideA[l+1]   = LA + NormalA * zA/cosdA;
				SideB[l+1]   = LB + NormalB * zB/cosdB;

				//create top surface points
				zA = m_pFoilA->GetUpperY(xLA)*chordA;
				zB = m_pFoilB->GetUpperY(xLB)*chordB;
				SideA_T[l+1] = LA + NormalA * zA/cosdA;
				SideB_T[l+1] = LB + NormalB * zB/cosdB;
				if(l==0)
				{
					zA = m_pFoilA->GetLowerY(xTA)*chordA;
					zB = m_pFoilB->GetLowerY(xTB)*chordB;
					SideA[0]   = TA + NormalA * zA/cosdA;
					SideB[0]   = TB + NormalB * zB/cosdB;

					zA = m_pFoilA->GetUpperY(xTA)*chordA;
					zB = m_pFoilB->GetUpperY(xTB)*chordB;
					SideA_T[0] = TA + NormalA * zA/cosdA;
					SideB_T[0] = TB + NormalB * zB/cosdB;
				}
			}
			else //iAnalysisType==2, VLM get mid surface points
			{ 
				zA = m_pFoilA->GetMidY(xLA)*chordA;
				zB = m_pFoilB->GetMidY(xLB)*chordB;
				SideA[l+1]   = LA + NormalA * zA/cosdA;
				SideB[l+1]   = LB + NormalB * zB/cosdB;

				if(l==0)
				{
					zA = m_pFoilA->GetMidY(xTA)*chordA;
					zB = m_pFoilB->GetMidY(xTB)*chordB;
					SideA[0]   = TA + NormalA * zA/cosdA;
					SideB[0]   = TB + NormalB * zB/cosdB;
				}
			}
		}
		else
		{
			SideA[l+1] = LA;
			SideB[l+1] = LB;
			SideA_T[l+1] = LA;
			SideB_T[l+1] = LB;
		}
	}
}


void CSurface::Copy(CSurface &Surface)
{
	m_LA.Copy(Surface.m_LA);
	m_LB.Copy(Surface.m_LB);
	m_TA.Copy(Surface.m_TA);
	m_TB.Copy(Surface.m_TB);
	m_Dihedral  = Surface.m_Dihedral;
	m_XDistType = Surface.m_XDistType;
	m_YDistType = Surface.m_YDistType;
	m_NElements = Surface.m_NElements;
	m_SineFactor = Surface.m_SineFactor;

	m_Dihedral  = Surface.m_Dihedral;
	m_Length    = Surface.m_Length;
	m_NXPanels  = Surface.m_NXPanels;
	m_NYPanels  = Surface.m_NYPanels;
	m_pFoilA    = Surface.m_pFoilA;
	m_pFoilB    = Surface.m_pFoilB;
	m_TwistA    = Surface.m_TwistA;
	m_TwistB    = Surface.m_TwistB;

	Normal  = Surface.Normal;
	NormalA = Surface.NormalA;
	NormalB = Surface.NormalB;

	m_bIsTipLeft  = Surface.m_bIsTipLeft;
	m_bIsTipRight = Surface.m_bIsTipRight;
	m_bIsLeftSurf = Surface.m_bIsLeftSurf;
	m_bIsRightSurf = Surface.m_bIsRightSurf;
	
	memcpy(m_xPointA, Surface.m_xPointA, sizeof(m_xPointA));
	memcpy(m_xPointB, Surface.m_xPointB, sizeof(m_xPointB));
}

void CSurface::Translate(CVector T)
{
	m_LA.Translate(T);
	m_LB.Translate(T);
	m_TA.Translate(T);
	m_TB.Translate(T);
}

void CSurface::RotateX(CVector O, double XTilt)
{
	m_LA.RotateX(O, XTilt);
	m_LB.RotateX(O, XTilt);
	m_TA.RotateX(O, XTilt);
	m_TB.RotateX(O, XTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateX(Origin, XTilt);
	NormalA.RotateX(Origin, XTilt);
	NormalB.RotateX(Origin, XTilt);
}

void CSurface::RotateY(CVector O, double YTilt)
{
	m_LA.RotateY(O, YTilt);
	m_LB.RotateY(O, YTilt);
	m_TA.RotateY(O, YTilt);
	m_TB.RotateY(O, YTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateY(Origin, YTilt);
	NormalA.RotateY(Origin, YTilt);
	NormalB.RotateY(Origin, YTilt);
}


void CSurface::RotateZ(CVector O, double ZTilt)
{
	m_LA.RotateZ(O, ZTilt);
	m_LB.RotateZ(O, ZTilt);
	m_TA.RotateZ(O, ZTilt);
	m_TB.RotateZ(O, ZTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateZ(Origin, ZTilt);
	NormalA.RotateZ(Origin, ZTilt);
	NormalB.RotateZ(Origin, ZTilt);
}


void CSurface::GetyDist(int k, double &y1, double &y2)
{
	double sf, snorm, cnorm, YPanels, dk;
	YPanels = (double)m_NYPanels;
	dk      = (double)k;
	if(m_SineFactor>0.0)	sf = 1.0+1.0/m_SineFactor;
	else                    sf = 1000000.0;
	snorm   = 1.0/sin(pi/sf);
	cnorm   = 1.0/cos( pi*(1/2.0-1.0/sf));

	if(m_YDistType==1){
		//cosine case
		y1  = 1.0/2.0*(1.0-cnorm*cos( dk      * pi *(1.0-2.0*(1/2.0-1.0/sf))/YPanels + pi*(1/2.0-1.0/sf)));
		y2  = 1.0/2.0*(1.0-cnorm*cos((dk+1.0) * pi *(1.0-2.0*(1/2.0-1.0/sf))/YPanels + pi*(1/2.0-1.0/sf)));
	}
	else if(m_YDistType==-2){
		//sine case
		y1  = snorm * (sin( dk      *pi/sf/YPanels));
		y2  = snorm * (sin((dk+1.0) *pi/sf/YPanels));
	}
	else if(m_YDistType==2){
		//-sine case
		y1  = (1.0-snorm *sin((YPanels-dk)     *pi/sf/YPanels));
		y2  = (1.0-snorm *sin((YPanels-dk-1.0) *pi/sf/YPanels));
	}
	else{
		//equally spaced case
		y1 =  dk     /YPanels;
		y2 = (dk+1.0)/YPanels;
	}
}

double CSurface::Getyrel(double y)
{
	return (y-m_LA.y)/(m_LB.y-m_LA.y);
}

void CSurface::SetFlap()
{
	if(m_pFoilA->m_bTEFlap) 
	{
		m_posATE = m_pFoilA->m_TEXHinge/100.0;
		if(m_posATE>1.0) m_posATE = 1.0; else if(m_posATE<0.0) m_posATE = 0.0;
	}
	else m_posATE = 1.0;

	if(m_pFoilB->m_bTEFlap)
	{
		m_posBTE = m_pFoilB->m_TEXHinge/100.0;
		if(m_posBTE>1.0) m_posBTE = 1.0; else if(m_posBTE<0.0) m_posBTE = 0.0;
	}
	else m_posBTE = 1.0;

	m_bTEFlap = m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap;
}

