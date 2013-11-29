/****************************************************************************

    POpp Class
	Copyright (C) 2006-2009 Andre Deperrois adeperrois@xflr5.com

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


#include "PlaneOpp.h"
#include "../globals.h"


/**
*The public constructor
*/
PlaneOpp::PlaneOpp(int PanelArraySize)
{
	m_PlaneName   = "";
	m_PlrName     = "";
	m_NStation    = 0;
	m_NPanels     = 0;
	m_Color       = QColor(255,0,0);
	m_Style       = 0;
	m_Width       = 1;
	m_WPolarType  = FIXEDSPEEDPOLAR;

	m_bIsVisible  = true;
	m_bShowPoints = false;


	m_bOut        = false;
	m_bVLM1       = true;

	m_Alpha               = 0.0;
	m_Beta                = 0.0;
	m_Bank                = 0.0;
	m_QInf                = 0.0;
	m_Ctrl                = 0.0;


//	for (int iw=0; iw<MAXWINGS; iw++) m_bWing[iw] = false;
//	m_bWing[0] = true;

	for (int iw=0; iw<MAXWINGS; iw++) m_pPlaneWOpp[iw] = NULL;

	m_Cp = m_G = m_Sigma = NULL;
	Allocate(PanelArraySize);
}


/**
 * Adds a WingOpp to the PlaneOpp and initializes the data
 * @param iw the index of the wing for which a WingOpp is added
 * @param PanelArraySize the number of panels on the wing
 */
void PlaneOpp::AddWingOpp(int iw, int PanelArraySize)
{
//	m_bWing[iw] = true;
	m_pPlaneWOpp[iw] = new WingOpp(PanelArraySize);
	m_pPlaneWOpp[iw]->m_Color = m_Color;
}



/**
 * The public destructor
 */
PlaneOpp::~PlaneOpp()
{
	Release();
}



/** Allocate memory to the arrays */
void PlaneOpp::Allocate(int PanelArraySize)
{
	Release();

	m_NPanels = PanelArraySize;

	m_Cp    = new float[PanelArraySize];
	m_Sigma = new float[PanelArraySize];
	m_G     = new float[PanelArraySize];
	memset(m_G,     0, PanelArraySize * sizeof(float));
	memset(m_Sigma, 0, PanelArraySize * sizeof(float));
	memset(m_Cp,    0, PanelArraySize * sizeof(float));
}


/**
 * Releases memory allocated on the heap
 */
void PlaneOpp::Release()
{
	if(m_Cp)    delete [] m_Cp;
	if(m_Sigma) delete [] m_Sigma;
	if(m_G)     delete [] m_G;

	m_Cp = NULL;
	m_Sigma = NULL;
	m_G = NULL;

	for (int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pPlaneWOpp[iw] != NULL) delete m_pPlaneWOpp[iw];
		m_pPlaneWOpp[iw] = NULL;
	}
}



/**
 * Loads or saves the data of this operating point to a binary file. 
 * This method serializes the data associated to the plane, then calls the serialization methods
 * of the wings.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool PlaneOpp::SerializePOpp(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int a, k;
	float f, g, h;

	if(bIsStoring)
	{
		ar << 1009;
		//1009 : restored the serialization of Cp, Gamma, Sigma arrays...
		//1008 : removed the serialization of Cp, Gamma, Sigma arrays
		//1007 : added Sideslip Beta
		//1006 : added Panel's source strengths Sigma
		//1005 : added second wing results for a biplane
		//1004 : converted units to SI
		//1003 : added vortices strengths
		//1002 : added Cp, suppressed Cl and other parameters
		//1001 : suppressed Ai, Cd, SpanPos, 

		//write variables
		WriteCString(ar, m_PlaneName);
		WriteCString(ar, m_PlrName);
		for(int iw=1; iw<MAXWINGS; iw++)
		{
			if(m_pPlaneWOpp[iw])  ar << 1; else ar<<0;
		}

		if(m_bIsVisible)  ar << 1; else ar<<0;
		if(m_bShowPoints) ar << 1; else ar<<0;
		if(m_bOut)        ar << 1; else ar<<0;
		if(m_bVLM1)       ar << 1; else ar<<0;
		ar << 1;

		ar << m_Style << m_Width;
		WriteCOLORREF(ar, m_Color);

		if(m_WPolarType==FIXEDSPEEDPOLAR)      ar<<1;
		else if(m_WPolarType==FIXEDLIFTPOLAR)  ar<<2;
		else if(m_WPolarType==FIXEDAOAPOLAR)   ar<<4;
		else if(m_WPolarType==STABILITYPOLAR)  ar<<7;
		else ar << 0;

		ar << m_NStation;
		ar << (float)m_Alpha << (float)m_QInf << 0.0f;//<< (float)m_Weight;
		ar << (float)m_Beta;

		ar << m_NPanels;
		for (k=0; k<m_NPanels; k++) ar<<(float)m_Cp[k]<<(float)m_Sigma[k]<<(float)m_G[k];
		ar << 0;// m_VLMType;

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pPlaneWOpp[iw])
			{
				m_pPlaneWOpp[iw]->SerializeWingOpp(ar, bIsStoring);
			}
		}
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
		//read variables
		ReadCString(ar, m_PlaneName);
		ReadCString(ar, m_PlrName);

		//always a main wing
		if(m_pPlaneWOpp[0]!=NULL) delete m_pPlaneWOpp[0];
		m_pPlaneWOpp[0] = new WingOpp();

		if(ArchiveFormat>=1005)
		{
			ar >> a;
			if (a!=0 && a!=1) return false;
			if(a)
			{
				if(m_pPlaneWOpp[1]!=NULL) delete m_pPlaneWOpp[1];
				m_pPlaneWOpp[1] = new WingOpp();
			}
		}

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a)
		{
			if(m_pPlaneWOpp[2]!=NULL) delete m_pPlaneWOpp[2];
			m_pPlaneWOpp[2] = new WingOpp();
		}

		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a)
		{
			if(m_pPlaneWOpp[3]!=NULL) delete m_pPlaneWOpp[3];
			m_pPlaneWOpp[3] = new WingOpp();
		}


		ar >> a;
		if (a!=0 && a!=1) return false;
		if(a) m_bIsVisible = true; else m_bIsVisible = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bShowPoints = true; else m_bShowPoints = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bOut = true; else m_bOut = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bVLM1 = true; else m_bVLM1 = false;

		ar >> a;
		if (a!=0 && a!=1) return false;
//		if(a) m_bMiddle = true; else m_bMiddle = false;

		ar >> m_Style >> m_Width;
		ReadCOLORREF(ar, m_Color);

		ar >>k;
		if(k==1)      m_WPolarType = FIXEDSPEEDPOLAR;
		else if(k==2) m_WPolarType = FIXEDLIFTPOLAR;
		else if(k==4) m_WPolarType = FIXEDAOAPOLAR;
		else if(k==7) m_WPolarType = STABILITYPOLAR;
		else return false;


		ar >> m_NStation;
		ar >> f;		m_Alpha = f;
		ar >> f;        m_QInf  = f;
		ar >> f;//        m_Weight = f;

		if(ArchiveFormat>=1007)
		{
			ar>>f; m_Beta = f;
		}
		if(ArchiveFormat<1002)
		{
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
		}
		if(ArchiveFormat<1001)
		{
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
		}
		if(ArchiveFormat>=1002 )
		{
			ar >>m_NPanels;

			if(ArchiveFormat<=1007)
			{
				for (k=0; k<=m_NPanels; k++)
				{
					ar >> f;
	//				m_Cp[k] = f;
				}
			}
		}
		if(ArchiveFormat>=1003 && ArchiveFormat<=1007)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
//				if(ArchiveFormat<1004)	m_G[k] = f/1000.0;
//				else 					m_G[k] = f;
			}
		}

		if(ArchiveFormat>=1006 && ArchiveFormat<=1007)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
//				m_Sigma[k] = f;
			}
		}

		if(ArchiveFormat>=1009)
		{
			if(m_G!=NULL)     delete [] m_G;
			if(m_Sigma!=NULL) delete [] m_Sigma;
			if(m_Cp!=NULL)    delete [] m_Cp;

			m_G     = new float[m_NPanels];
			m_Sigma = new float[m_NPanels];
			m_Cp    = new float[m_NPanels];

			for (k=0; k<m_NPanels; k++)
			{
				ar >> f >> g >> h;
				m_Cp[k]    = f;
				m_Sigma[k] = g;
				m_G[k]     = h;
			}
		}

		ar >> k; //VLMType

		if (!m_pPlaneWOpp[0]->SerializeWingOpp(ar, bIsStoring))
		{
			return false;
		}


		if(ArchiveFormat>=1005)
		{
			if(m_pPlaneWOpp[1])
			{
				if (!m_pPlaneWOpp[1]->SerializeWingOpp(ar, bIsStoring))
				{
					return false;
				}
			}
		}
		if(m_pPlaneWOpp[2])
		{
			if (!m_pPlaneWOpp[2]->SerializeWingOpp(ar, bIsStoring))
			{
				return false;
			}

		}
		if(m_pPlaneWOpp[3])
		{
			if (!m_pPlaneWOpp[3]->SerializeWingOpp(ar, bIsStoring))
			{
				return false;
			}
		}

		m_Ctrl = m_pPlaneWOpp[0]->m_Ctrl; //forgot to save the variable in the POpp serialization...

	}
	return true;
}

