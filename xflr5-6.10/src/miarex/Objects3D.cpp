/****************************************************************************

	Objects3D    Copyright (C) 2014 Andre Deperrois adeperrois@xflr5.com

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

#include "./Miarex.h"
#include "../misc/RenameDlg.h"

#include "Objects3D.h"
#include <QApplication>
#include <QMessageBox>
#include <QLocale>
#include <QProcess>
#include <QtDebug>



QList<Surface *> Objects3D::s_SurfaceList;

CVector *Objects3D::s_Node = NULL;
CVector *Objects3D::s_MemNode = NULL;
CVector *Objects3D::s_WakeNode = NULL;
CVector *Objects3D::s_RefWakeNode = NULL;

Panel *Objects3D::s_Panel = NULL;
Panel *Objects3D::s_MemPanel = NULL;
Panel *Objects3D::s_WakePanel = NULL;
Panel *Objects3D::s_RefWakePanel = NULL;


PanelAnalysis *Objects3D::s_pPanelAnalysis = NULL;

int Objects3D::s_MaxPanelSize = 0;

int Objects3D::s_MatSize     = 0;
int Objects3D::s_WakeSize    = 0;
int Objects3D::s_nNodes      = 0;
int Objects3D::s_nWakeNodes  = 0;
int Objects3D::s_NWakeColumn = 0;
int Objects3D::s_MaxWakeIter = 5;

double Objects3D::s_LastWOpp = -9999999.0;



QList <void *> Objects3D::s_oaPlane;
QList <void *> Objects3D::s_oaWPolar;
QList <void *> Objects3D::s_oaPOpp;
QList <void *> Objects3D::s_oaBody;




Objects3D::Objects3D()
{
	s_Node  = s_MemNode  = s_WakeNode  = s_RefWakeNode  = NULL;
	s_Panel = s_MemPanel = s_WakePanel = s_RefWakePanel = NULL;
}


/**
 * Reserves the memory necessary to all the arrays used in a Panel analysis.
 *@return true if the memory could be allocated, false otherwise.
 */
bool Objects3D::AllocatePanelArrays(int &memsize)
{
//	Trace(QString("QMiarex::Allocating() %1 Panels").arg(s_MaxPanelSize));

	try
	{
		s_Node        = new CVector[2*s_MaxPanelSize];
		s_MemNode     = new CVector[2*s_MaxPanelSize];
		s_WakeNode    = new CVector[2*s_MaxPanelSize];
		s_RefWakeNode = new CVector[2*s_MaxPanelSize];

		s_Panel        = new Panel[s_MaxPanelSize];
		s_MemPanel     = new Panel[s_MaxPanelSize];
		s_WakePanel    = new Panel[s_MaxPanelSize];
		s_RefWakePanel = new Panel[s_MaxPanelSize];
	}
	catch(std::exception & e)
	{
		ReleasePanelMemory();
		s_MaxPanelSize = 0;

		Trace(e.what());
		QString strange = "Memory allocation error: the request for additional memory has been denied.\nPlease reduce the model's size.";
		Trace(strange);
		return false;
	}

	memsize  = sizeof(CVector) * 8 * 2 * s_MaxPanelSize; //bytes
	memsize += sizeof(Panel)   * 8 * 2 * s_MaxPanelSize; //bytes

	Trace(QString("Objects3D::   ...Allocated %1MB for the panel and node arrays").arg((double)memsize/1024./1024.));

	Panel::s_pNode = s_Node;
	Panel::s_pWakeNode = s_WakeNode;

	Surface::s_pPanel = s_Panel;
	Surface::s_pNode  = s_Node;

	QMiarex::s_pPanel = s_Panel;
	QMiarex::s_pNode = s_Node;

	return true;
}



/**
 * Releases the memory allocated to the Panel and node arrays.
 * Sets the pointers to NULL and the matrixsize to 0.
 */
void Objects3D::ReleasePanelMemory()
{
	Trace("Objects3D::ReleasingPanelMemory()");

	if(s_Node)        delete[] s_Node;
	if(s_MemNode)     delete[] s_MemNode;
	if(s_WakeNode)    delete[] s_WakeNode;
	if(s_RefWakeNode) delete[] s_RefWakeNode;
	s_Node = s_MemNode = s_WakeNode = s_RefWakeNode = NULL;

	if(s_Panel)        delete[] s_Panel;
	if(s_MemPanel)     delete[] s_MemPanel;
	if(s_WakePanel)    delete[] s_WakePanel;
	if(s_RefWakePanel) delete[] s_RefWakePanel;
	s_Panel = s_MemPanel = s_WakePanel = s_RefWakePanel = NULL;

	s_MatSize = 0;
	s_nNodes = 0;
}




/**
 * Creates local pointers in the classes, for programming convenience.
 */
void Objects3D::setStaticPointers()
{
	QMiarex::m_poaPlane  = &s_oaPlane;
	QMiarex::m_poaWPolar = &s_oaWPolar;
	QMiarex::m_poaPOpp   = &s_oaPOpp;
	QMiarex::m_poaBody   = &s_oaBody;
}



/**
 * If the body is associated to a plane, duplicates the body and attaches it to the Plane
 * Else destroys the Body.
 * Used only to import body Objects from files prior to v6.09.06
 * @param a pointer to the Body object to be inserted
 * @return true if the body is associated to a plane, false otherwise
 */
void Objects3D::addBody(Body *pBody)
{
	for(int ip=0; ip<s_oaPlane.size(); ip++)
	{
		Plane *pPlane = (Plane*)s_oaPlane.at(ip);
		if(pPlane->m_BodyName==pBody->m_BodyName)
		{
			// duplicate the body - create one for each plane
			// no more bodies associated to multiple plane
			Body *planeBody = new Body();
			planeBody->Duplicate(pBody);

			//attach it and rename it
			pPlane->setBody(planeBody);
		}
	}
/*	for(int ib=0; ib<m_poaBody->size(); ib++)
	{
		Body *pOldBody = (Body*)m_poaBody->at(ib);
		if(pOldBody==pBody)
		{
			m_poaBody->removeAt(ib);
			delete pBody; //discarded
		}
	} */
}



/**
 * Inserts a plane in the array of plane objects
 * @param pPlane: the plane to be inserted
 */
void Objects3D::addPlane(Plane *pPlane)
{
	int i,j;
	Plane *pOldPlane;

	for (i=0; i<s_oaPlane.size(); i++)
	{
		pOldPlane = (Plane*)s_oaPlane.at(i);
		if (pOldPlane->planeName() == pPlane->planeName())
		{
			//a plane with this name already exists
			// if its the same plane, just return
			if(pOldPlane==pPlane) return;

			// if its an old plane with the same name, delete and insert at its place
			deletePlane(pOldPlane);
			s_oaPlane.insert(i, pPlane);
			return;
		}
	}

	// the plane does not exist, just insert in alphabetical order
	for (j=0; j<s_oaPlane.size(); j++)
	{
		pOldPlane = (Plane*)s_oaPlane.at(j);
		if (pPlane->planeName() < pOldPlane->planeName())
		{
			s_oaPlane.insert(j, pPlane);
			return;
		}
	}

	//could not be inserted, append
	s_oaPlane.append(pPlane);
}



/**
* Duplicates the currently selected plane, if any and opens it for edition
* @param pCurPlane a pointer to the Plane object to duplicate
* @return a pointer to the Plane object which has been created
*/
Plane* Objects3D::duplicatePlane(Plane *pPlane)
{
	if(!pPlane) return NULL;
	Plane* pNewPlane= new Plane;
	pNewPlane->Duplicate(pPlane);
	return setModPlane(pNewPlane);
}




/**
 * Following the selection of a wing or a plane, this subroutine creates the panels
 * associated to all of the surface objects.
 *
 * m_Panel is the array of panels in the following order
 *         main wing left side, main wing right side
 *         second wing
 *         elevator
 *         fin
 *         body
 * Each panel gets the index of its symmetric, if any, else this index is set to -1
 *
 * A copy of the panels is saved to the MemPanel and MemNode arrays
 *@return true if successful, false if the panels could not be properly created ot if no object is active
*/
bool Objects3D::initializePanels(Plane *pPlane, WPolar *pWPolar)
{
	if(!pPlane) return false;
	int Nel=0;

	// first check that the total number of panels that will be created does not exceed
	// the currently allocated memory size for the influence atrix.

	int PanelArraySize = calculateMatSize(pPlane, pWPolar);
	int memsize = 0;

	if(PanelArraySize>s_MaxPanelSize)
	{

		Trace(QString("Objects3D::Requesting additional memory for %1 panels").arg(PanelArraySize));

		// allocate 10% more than needed to avoid repeating the operation if the user requirement increases sightly again.
		s_MaxPanelSize = (int)((double)PanelArraySize *1.1);
		ReleasePanelMemory();

		if(!AllocatePanelArrays(memsize))
		{
			s_MaxPanelSize = 0;
			return false;
		}
	}

	//if a WPolar is defined, allocate the matrix
	if(pWPolar)
	{
		int MatrixSize=0;

		if(!s_pPanelAnalysis->AllocateMatrix(s_MaxPanelSize, MatrixSize))
		{
			ReleasePanelMemory();
			return false;
		}

		Trace("");
		memsize += MatrixSize;
	}


	// all set to create the panels

	s_MatSize     = 0;
	s_nNodes      = 0;
	s_NWakeColumn = 0;
	s_nWakeNodes  = 0;
	s_WakeSize    = 0;

	memset(s_Panel, 0, s_MaxPanelSize * sizeof(Panel));
	memset(s_Node,  0, 2 * s_MaxPanelSize * sizeof(CVector));

	Panel *ptr = s_Panel;

//	dlg.setValue(5);
//	int NXWakePanels;
//	if(pCurWPolar)	NXWakePanels = pCurWPolar->m_NXWakePanels;
//	else                NXWakePanels = 1;

	Wing *pWingList[MAXWINGS];
	pWingList[0] = pPlane->wing();
	pWingList[1] = pPlane->wing2();
	pWingList[2] = pPlane->stab();
	pWingList[3] = pPlane->fin();


	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(pWingList[iw])
		{
			pWingList[iw]->m_MatSize = 0;
			for(int jSurf=0; jSurf<pWingList[iw]->m_Surface.size(); jSurf++)
			{
				pWingList[iw]->m_Surface.at(jSurf)->ResetFlap();
				Nel = createWingElements(pPlane, pWPolar, pWingList[iw]->m_Surface.at(jSurf));
				pWingList[iw]->m_MatSize += Nel;
			}
			pWingList[iw]->m_pWingPanel = ptr;
			ptr += pWingList[iw]->m_MatSize;
		}
	}

	bool bBodyEl = false;
	if(pPlane && pPlane->body())
	{
		if(!pWPolar) bBodyEl = true;//no risk...
		else if(pWPolar->m_AnalysisMethod==PANELMETHOD && !pWPolar->m_bIgnoreBodyPanels)
		{
			bBodyEl = true;
		}
	}

	if(bBodyEl)
	{
		Nel = createBodyElements(pPlane);

		if(pPlane && pPlane->body())
			pPlane->body()->m_pBodyPanel = ptr;

	}

	//back-up the current geometry
	memcpy(s_MemPanel, s_Panel, s_MatSize* sizeof(Panel));
	memcpy(s_MemNode,  s_Node,  s_nNodes * sizeof(CVector));
	memcpy(s_RefWakePanel, s_WakePanel, s_WakeSize* sizeof(Panel));
	memcpy(s_RefWakeNode,  s_WakeNode,  s_nWakeNodes * sizeof(CVector));


//	dlg.setValue(100);

	return true;
}





/**
 * Rotates all the panels about the z-axis
 * @param pPanel a pointer to the array of surface mesh panels
 * @param pNode  a pointer to the array of surface panel nodes
 * @param pWakePanel a pointer to the array of wake mesh panels
 * @param pWakeNode  a pointer to the array of wake panel nodes
 * @param beta the rotation angle in degrees
 * @param P the point of origin of the rotation
 */
void Objects3D::rotateGeomZ(Panel *pPanel, CVector *pNode, Panel *pWakePanel, CVector *pWakeNode,
					   int nPanels, int nWakePanels, int nNodes, int/* nWakeNodes*/, int NWakeColumn,
					   double const &Beta, CVector const &P, int NXWakePanels)
{
	int n, p, pw, kw, lw;
	int iLA, iLB, iTA, iTB;
	CVector Pt, Trans;

	for (n=0; n<nNodes; n++)	pNode[n].RotateZ(P, Beta);

	for (p=0; p<nPanels; p++)
	{
		// get the index of the panel's four corner points
		iLA = pPanel[p].m_iLA; iLB = pPanel[p].m_iLB;
		iTA = pPanel[p].m_iTA; iTB = pPanel[p].m_iTB;

		//set the new panel geometry
		if(pPanel[p].m_Pos>=MIDSURFACE)       pPanel[p].SetPanelFrame(pNode[iLA], pNode[iLB], pNode[iTA], pNode[iTB]);
		else if (pPanel[p].m_Pos==BOTSURFACE) pPanel[p].SetPanelFrame(pNode[iLB], pNode[iLA], pNode[iTB], pNode[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge and aligned with the freestream velocity vector
	pw=0;

	for (kw=0; kw<NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = pWakeNode[pWakePanel[pw].m_iLA];
		Pt.RotateZ(P, Beta);
		//define the translation to be applied to the column's left points
		Trans = Pt-pWakeNode[pWakePanel[pw].m_iLA] ;

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<NXWakePanels; lw++)
		{
			if(lw==0) pWakeNode[pWakePanel[pw].m_iLA] += Trans;
			pWakeNode[pWakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}
	
	//last column, process B side of the column
	pw -= NXWakePanels;
	//consider the first panel of the column;
	Pt = pWakeNode[pWakePanel[pw].m_iLB];
	Pt.RotateZ(P, Beta);
	//define the translation to be applied to the column's left points
	Trans = Pt - pWakeNode[pWakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all left A nodes
	for(lw=0; lw<NXWakePanels; lw++)
	{
		if(lw==0) pWakeNode[pWakePanel[pw].m_iLB] += Trans;
		pWakeNode[pWakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< nWakePanels; pw++)
	{
		// get the index of the panel's four corner points
		iLA = pWakePanel[pw].m_iLA; iLB = pWakePanel[pw].m_iLB;
		iTA = pWakePanel[pw].m_iTA; iTB = pWakePanel[pw].m_iTB;
		
		//set the new panel geometry
		pWakePanel[pw].SetPanelFrame(pWakeNode[iLA], pWakeNode[iLB], pWakeNode[iTA], pWakeNode[iTB]);
	}
}



/**
 * Rotates all the panels about the y-axis
 * @param Angle the rotation angle in degrees
 * @param P the point of origin of the rotation
 */
void Objects3D::rotateGeomY(Panel *pPanel, CVector *pNode, Panel *pWakePanel, CVector *pWakeNode,
					   int nPanels, int nWakePanels, int nNodes, int /*nWakeNodes*/, int NWakeColumn,
					   double const &Alpha, CVector const &P, int NXWakePanels)
{
	int n, p, pw, kw, lw;

	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Trans;

	for (n=0; n<nNodes; n++)
		pNode[n].RotateY(P, Alpha);

	for (p=0; p<nPanels; p++)
	{
		iLA = pPanel[p].m_iLA; iLB = pPanel[p].m_iLB;
		iTA = pPanel[p].m_iTA; iTB = pPanel[p].m_iTB;

		LATB = pNode[iLA] - pNode[iTB];
		TALB = pNode[iTA] - pNode[iLB];

		if(pPanel[p].m_Pos==MIDSURFACE || pPanel[p].m_Pos==TOPSURFACE) pPanel[p].SetPanelFrame(pNode[iLA], pNode[iLB], pNode[iTA], pNode[iTB]);
		else if (pPanel[p].m_Pos==BOTSURFACE)                          pPanel[p].SetPanelFrame(pNode[iLB], pNode[iLA], pNode[iTB], pNode[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;


	for (kw=0; kw<NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = pWakeNode[pWakePanel[pw].m_iLA];
		Pt.RotateY(P, Alpha);
		//define the translation to be applied to the column's left points
		Trans = Pt - pWakeNode[pWakePanel[pw].m_iLA];

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<NXWakePanels; lw++)
		{
			if(lw==0) pWakeNode[pWakePanel[pw].m_iLA] += Trans;
			pWakeNode[pWakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}

	//do the same for the very last right wake node column
	pw -= NXWakePanels;
	Pt = pWakeNode[pWakePanel[pw].m_iLB];
	Pt.RotateY(P, Alpha);
	//define the translation to be applied to the column's left points
	Trans = Pt-pWakeNode[pWakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all right B nodes
	for(lw=0; lw<NXWakePanels; lw++)
	{
		if(lw==0) pWakeNode[pWakePanel[pw].m_iLB] += Trans;
		pWakeNode[pWakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw<nWakePanels; pw++)
	{
		iLA = pWakePanel[pw].m_iLA; iLB = pWakePanel[pw].m_iLB;
		iTA = pWakePanel[pw].m_iTA; iTB = pWakePanel[pw].m_iTB;
		pWakePanel[pw].SetPanelFrame(pWakeNode[iLA], pWakeNode[iLB], pWakeNode[iTA], pWakeNode[iTB]);
	}
}



void Objects3D::InsertPOpp(PlaneOpp *pPOpp)
{
	PlaneOpp *pOldPOpp = NULL;
	bool bIsInserted = false;

	for (int i=0; i<s_oaPOpp.size(); i++)
	{
		pOldPOpp = (PlaneOpp*)s_oaPOpp.at(i);
		if (pPOpp->m_PlaneName == pOldPOpp->m_PlaneName)
		{
			if (pPOpp->m_PlrName == pOldPOpp->m_PlrName)
			{
				if(pPOpp->m_WPolarType<FIXEDAOAPOLAR)
				{
					if(qAbs(pPOpp->m_Alpha - pOldPOpp->m_Alpha)<0.005)
					{
						//replace existing point
						pPOpp->m_Color = pOldPOpp->m_Color;
						pPOpp->m_Style = pOldPOpp->m_Style;
						pPOpp->m_Width = pOldPOpp->m_Width;
						pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
						pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;

						s_oaPOpp.removeAt(i);
						delete pOldPOpp;
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						break;
					}
					else if (pPOpp->m_Alpha > pOldPOpp->m_Alpha)
					{
						//insert point
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						i = s_oaPOpp.size();// to break
					}
				}
				else if (pPOpp->m_WPolarType==FIXEDAOAPOLAR)
				{
					if(qAbs(pPOpp->m_QInf - pOldPOpp->m_QInf)<0.1)
					{
						//replace existing point
						pPOpp->m_Color = pOldPOpp->m_Color;
						pPOpp->m_Style = pOldPOpp->m_Style;
						pPOpp->m_Width = pOldPOpp->m_Width;
						pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
						pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;

						s_oaPOpp.removeAt(i);
						delete pOldPOpp;
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						i = s_oaPOpp.size();// to break
					}
					else if (pPOpp->m_QInf > pOldPOpp->m_QInf)
					{
						//insert point
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						i = s_oaPOpp.size();// to break
					}
				}
				else if(pPOpp->m_WPolarType==STABILITYPOLAR)
				{
					if(qAbs(pPOpp->m_Ctrl - pOldPOpp->m_Ctrl)<0.005)
					{
						//replace existing point
						pPOpp->m_Color = pOldPOpp->m_Color;
						pPOpp->m_Style = pOldPOpp->m_Style;
						pPOpp->m_Width = pOldPOpp->m_Width;
						pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
						pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;

						s_oaPOpp.removeAt(i);
						delete pOldPOpp;
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						i = s_oaPOpp.size();// to break
					}
					else if (pPOpp->m_Ctrl > pOldPOpp->m_Ctrl)
					{
						//insert point
						s_oaPOpp.insert(i, pPOpp);
						bIsInserted = true;
						i = s_oaPOpp.size();// to break
					}
				}
			}
		}
	}

	if (!bIsInserted) 	s_oaPOpp.append(pPOpp);
}




/**
 * Checks if the input point is close to a wake node within the tolerances set in the CVector class
 * Returns the index of a node if found, else returns -1
 *@param Pt : the point to identify
 *@return the index of the node with coordinates equal to the input Pt
*/
int Objects3D::IsWakeNode(CVector &Pt)
{
	//
	// returns the index of a wake node if found, else returns NULL
	//
	int in;
	for (in=0; in<s_nWakeNodes; in++)
	{
		if(Pt.IsSame(s_WakeNode[in])) return in;
	}
	return -1;
}


/**
 * Checks if the input point is close to a mesh node within the tolerances set in the CVector class
 * Returns the index of a node if found, else returns -1
 *@param Pt : the point to identify
 *@return the index of the node with coordinates equal to the input Pt
*/
int Objects3D::IsNode(CVector &Pt)
{
	int in;
	for (in=s_nNodes-1; in>=0; in--)
	{
		if(Pt.IsSame(s_Node[in])) return in;
	}
	return -1;
}



/**
 * At panels on the side of the surfaces, connects the element to the next surface
 *
 * In the case where the number of chordwise panels is different between two adjacent surfaces,
 * We need to correct the ideal connection that was set in the CreateElements() method.
 * This is the case for instance for a flap.
 * The algorithm below is artisanal and not robust... ideally the connections should be set manually
 *
 * Uses VSAERO method
 *
*/
void Objects3D::JoinSurfaces(WPolar*pWPolar, Surface *pLeftSurf, Surface *pRightSurf, int pl, int pr)
{

	if(!pWPolar || pWPolar->m_AnalysisMethod!=PANELMETHOD) return;//panel analysis only

	//pl and pr are respectively the left surface's and the right surface's first panel index
	int ls, lr, lclose, ppl, ppr;
	double dist, x,y,z, mindist;
	lclose=0;
	CVector MidNormal = pLeftSurf->Normal + pRightSurf->Normal;
	MidNormal.Normalize();

	int coef = 1;
	if(pWPolar && pWPolar->m_AnalysisMethod==PANELMETHOD && !pWPolar->m_bThinSurfaces) coef = 2;

	//left surface's right side
	ppl = pl;
	ppr = pr;
	if(pLeftSurf->m_bIsTipLeft && !pWPolar->m_bThinSurfaces) ppl+= pLeftSurf->m_NXPanels;//left tip patch
	ppl += (pLeftSurf->m_NYPanels-1) * coef*pLeftSurf->m_NXPanels;
	//ppl is now set at left surface's first bottom panel of tip right strip

	//Process left bottom side first
	for (ls=0; ls<pLeftSurf->m_NXPanels; ls++)
	{
		if(ls>=pLeftSurf->m_NXFlap) //flaps are not connected
		{
			mindist = 1.0e100;
			for (lr=0; lr<pRightSurf->m_NXPanels; lr++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = s_Panel[ppr+lr].CollPt.x - s_Panel[ppl+ls].CollPt.x;
				y = s_Panel[ppr+lr].CollPt.y - s_Panel[ppl+ls].CollPt.y;
				z = s_Panel[ppr+lr].CollPt.z - s_Panel[ppl+ls].CollPt.z;
				dist = qAbs(x*s_Panel[ppl+ls].l.x + y*s_Panel[ppl+ls].l.y + z*s_Panel[ppl+ls].l.z);
				if(dist<mindist)
				{
					lclose = lr;
					mindist = dist ;
				}
			}
			if(lclose>=pRightSurf->m_NXFlap)
			{
				s_Panel[ppl+ls].m_iPL = ppr+lclose;

			}
			else
				s_Panel[ppl+ls].m_iPL = -1;
		}
		else
			s_Panel[ppl+ls].m_iPL = -1;//flap is not connected
	}

	//Process left top side next
	for (ls=pLeftSurf->m_NXPanels;ls<coef*pLeftSurf->m_NXPanels; ls++)
	{
		if(ls < coef*pLeftSurf->m_NXPanels-pLeftSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (lr=pRightSurf->m_NXPanels; lr<coef*pRightSurf->m_NXPanels; lr++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = s_Panel[ppr+lr].CollPt.x - s_Panel[ppl+ls].CollPt.x;
				y = s_Panel[ppr+lr].CollPt.y - s_Panel[ppl+ls].CollPt.y;

				z = s_Panel[ppr+lr].CollPt.z - s_Panel[ppl+ls].CollPt.z;
				dist = qAbs(x*s_Panel[ppl+ls].l.x + y*s_Panel[ppl+ls].l.y + z*s_Panel[ppl+ls].l.z);
				if(dist<mindist)
				{
					lclose = lr;
					mindist = dist ;
				}
			}
			if(lclose< coef*pRightSurf->m_NXPanels-pRightSurf->m_NXFlap)
			{
				s_Panel[ppl+ls].m_iPR = ppr+lclose;
			}
			else
				s_Panel[ppl+ls].m_iPR = -1;
		}
		else
			s_Panel[ppl+ls].m_iPR = -1;
	}

	//Move on to right surface's left connections
	//ppr is set at right surface's first bottom panel of tip left strip
	ppl = pl;
	if(pLeftSurf->m_bIsTipLeft && !pWPolar->m_bThinSurfaces) ppl+= pLeftSurf->m_NXPanels;//left tip patch
	ppl += (pLeftSurf->m_NYPanels-1) * coef*pLeftSurf->m_NXPanels;

	//Process right bottom side
	for (lr=0;lr<pRightSurf->m_NXPanels; lr++)
	{
		if(lr>=pRightSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (ls=0; ls<pLeftSurf->m_NXPanels; ls++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = s_Panel[ppl+ls].CollPt.x - s_Panel[ppr+lr].CollPt.x;
				y = s_Panel[ppl+ls].CollPt.y - s_Panel[ppr+lr].CollPt.y;
				z = s_Panel[ppl+ls].CollPt.z - s_Panel[ppr+lr].CollPt.z;
				dist = qAbs(x*s_Panel[ppr+lr].l.x + y*s_Panel[ppr+lr].l.y + z*s_Panel[ppr+lr].l.z);
				if(dist<mindist)
				{
					lclose = ls;
					mindist = dist ;
				}
			}
			if(lclose>=pLeftSurf->m_NXFlap)
			{
				s_Panel[ppr+lr].m_iPR = ppl+lclose;
			}
			else
				s_Panel[ppr+lr].m_iPR = -1;
		}
		else
			s_Panel[ppr+lr].m_iPR = -1;
	}


	//Process right top side
	for (lr=pRightSurf->m_NXPanels;lr<coef*pRightSurf->m_NXPanels; lr++)
	{
		if(lr < 2*pRightSurf->m_NXPanels-pRightSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (ls=pLeftSurf->m_NXPanels; ls<2*pLeftSurf->m_NXPanels; ls++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = s_Panel[ppl+ls].CollPt.x - s_Panel[ppr+lr].CollPt.x;
				y = s_Panel[ppl+ls].CollPt.y - s_Panel[ppr+lr].CollPt.y;
				z = s_Panel[ppl+ls].CollPt.z - s_Panel[ppr+lr].CollPt.z;
				dist = qAbs(x*s_Panel[ppr+lr].l.x + y*s_Panel[ppr+lr].l.y + z*s_Panel[ppr+lr].l.z);
				if(dist<mindist)
				{
					lclose =  ls;
					mindist = dist ;
				}
			}
			if(lclose < 2*pLeftSurf->m_NXPanels-pLeftSurf->m_NXFlap)
			{
				s_Panel[ppr+lr].m_iPL = ppl+lclose;
			}
			else
				s_Panel[ppr+lr].m_iPL = -1;
		}
		else
			s_Panel[ppr+lr].m_iPL = -1;
	}
}




/**
* Adds the WPolar pointed by pWPolar to the m_oaWPolar array.
* Inserts it in alphabetical order using the Plane Name and the polar name
*@param pWPolar a pointer to the instance of the CWPolar object to be inserted
*@return a pointer  to the instance of the CWPolar object which has been inserted, or NULL if the operation failed
*/
void Objects3D::addWPolar(WPolar *pWPolar)
{
	int j;
	WPolar *pOldWPlr;

	for (int ip=0; ip<s_oaWPolar.size(); ip++)
	{
		pOldWPlr = (WPolar*)s_oaWPolar.at(ip);
		if (pOldWPlr->polarName()==pWPolar->polarName() && pOldWPlr->planeName()==pWPolar->planeName())
		{
			s_oaWPolar.removeAt(ip);
			delete pOldWPlr;
			s_oaWPolar.insert(ip, pWPolar);
			return;
		}
	}


	//if it doesn't exist, find its place in alphabetical order and insert it
	for (j=0; j<s_oaWPolar.size(); j++)
	{
		pOldWPlr = (WPolar*)s_oaWPolar.at(j);
		//first key is the Plane name
		if(pWPolar->planeName().compare(pOldWPlr->planeName(), Qt::CaseInsensitive)<0)
		{
			s_oaWPolar.insert(j, pWPolar);
			return;
		}
		else if (pWPolar->planeName() == pOldWPlr->planeName())
		{
			// sort by polar name
			if(pWPolar->polarName().compare(pOldWPlr->polarName(), Qt::CaseInsensitive)<0)
			{
				s_oaWPolar.insert(j, pWPolar);
				return;
			}
		}
	}

	s_oaWPolar.append(pWPolar);
}



/**
 * Creates the body panels for the active CBody object
 * The panels are created in the following order
 *    - for the port side  first, then for the starboard side
 *    - from bottom to top
 *    - from tip to tail
 * The panels are appended to the existing array of panels
 * @return the number of panels which have been created and appended
 */
int Objects3D::createBodyElements(Plane *pCurPlane)
{
	if(!pCurPlane) return 0;
	if(!pCurPlane->body()) return 0;

	Body *pCurBody = pCurPlane->body();

	int i,j,k,l;
	double uk, uk1, v, dj, dj1, dl1;
	double dpx, dpz;
	CVector LATB, TALB;
	CVector LA, LB, TA, TB;
	CVector PLA, PTA, PLB, PTB;

	int n0, n1, n2, n3, lnx, lnh;
	int nx = pCurBody->m_nxPanels;
	int nh = pCurBody->m_nhPanels;
	int p = 0;

	int InitialSize = s_MatSize;
	int FullSize =0;

	lnx = 0;

	if(pCurPlane && pCurPlane->body())
	{
		dpx = pCurPlane->BodyPos().x;
		dpz = pCurPlane->BodyPos().z;
	}
	else dpx=dpz=0.0;

	if(pCurBody->m_LineType==BODYPANELTYPE)
	{
		nx = 0;
		for(i=0; i<pCurBody->FrameSize()-1; i++) nx+=pCurBody->m_xPanels[i];
		nh = 0;
		for(i=0; i<pCurBody->SideLineCount()-1; i++) nh+=pCurBody->m_hPanels[i];
		FullSize = nx*nh*2;
		pCurBody->m_nxPanels = nx;
		pCurBody->m_nhPanels = nh;

		for (i=0; i<pCurBody->FrameSize()-1; i++)
		{
			for (j=0; j<pCurBody->m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(pCurBody->m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(pCurBody->m_xPanels[i]);

				//body left side
				lnh = 0;
				for (k=0; k<pCurBody->SideLineCount()-1; k++)
				{
					//build the four corner points of the strips
					PLB.x =  (1.0- dj) * pCurBody->FramePosition(i)      +  dj * pCurBody->FramePosition(i+1)       +dpx;
					PLB.y = -(1.0- dj) * pCurBody->getFrame(i)->m_CtrlPoint[k].y   -  dj * pCurBody->getFrame(i+1)->m_CtrlPoint[k].y;
					PLB.z =  (1.0- dj) * pCurBody->getFrame(i)->m_CtrlPoint[k].z   +  dj * pCurBody->getFrame(i+1)->m_CtrlPoint[k].z    +dpz;

					PTB.x =  (1.0-dj1) * pCurBody->FramePosition(i)      + dj1 * pCurBody->FramePosition(i+1)       +dpx;
					PTB.y = -(1.0-dj1) * pCurBody->getFrame(i)->m_CtrlPoint[k].y   - dj1 * pCurBody->getFrame(i+1)->m_CtrlPoint[k].y;
					PTB.z =  (1.0-dj1) * pCurBody->getFrame(i)->m_CtrlPoint[k].z   + dj1 * pCurBody->getFrame(i+1)->m_CtrlPoint[k].z    +dpz;

					PLA.x =  (1.0- dj) * pCurBody->FramePosition(i)      +  dj * pCurBody->FramePosition(i+1)       +dpx;
					PLA.y = -(1.0- dj) * pCurBody->getFrame(i)->m_CtrlPoint[k+1].y -  dj * pCurBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
					PLA.z =  (1.0- dj) * pCurBody->getFrame(i)->m_CtrlPoint[k+1].z +  dj * pCurBody->getFrame(i+1)->m_CtrlPoint[k+1].z  +dpz;

					PTA.x =  (1.0-dj1) * pCurBody->FramePosition(i)      + dj1 * pCurBody->FramePosition(i+1)       +dpx;
					PTA.y = -(1.0-dj1) * pCurBody->getFrame(i)->m_CtrlPoint[k+1].y - dj1 * pCurBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
					PTA.z =  (1.0-dj1) * pCurBody->getFrame(i)->m_CtrlPoint[k+1].z + dj1 * pCurBody->getFrame(i+1)->m_CtrlPoint[k+1].z  +dpz;

					LB = PLB;
					TB = PTB;

					for (l=0; l<pCurBody->m_hPanels[k]; l++)
					{
						dl1  = (double)(l+1) / (double)(pCurBody->m_hPanels[k]);
						LA = PLB * (1.0- dl1) + PLA * dl1;
						TA = PTB * (1.0- dl1) + PTA * dl1;

						n0 = IsNode(LA);
						n1 = IsNode(TA);
						n2 = IsNode(LB);
						n3 = IsNode(TB);

						if(n0>=0) {
							s_Panel[s_MatSize].m_iLA = n0;
						}
						else {
							s_Panel[s_MatSize].m_iLA = s_nNodes;
							s_Node[s_nNodes].Copy(LA);
							s_nNodes++;
						}

						if(n1>=0) {
							s_Panel[s_MatSize].m_iTA = n1;
						}
						else {
							s_Panel[s_MatSize].m_iTA = s_nNodes;
							s_Node[s_nNodes].Copy(TA);
							s_nNodes++;
						}

						if(n2>=0) {
							s_Panel[s_MatSize].m_iLB = n2;
						}
						else {
							s_Panel[s_MatSize].m_iLB = s_nNodes;
							s_Node[s_nNodes].Copy(LB);
							s_nNodes++;
						}

						if(n3 >=0) {
							s_Panel[s_MatSize].m_iTB = n3;
						}
						else {
							s_Panel[s_MatSize].m_iTB = s_nNodes;
							s_Node[s_nNodes].Copy(TB);
							s_nNodes++;
						}

						LATB = TB - LA;
						TALB = LB - TA;
						s_Panel[s_MatSize].Normal = LATB * TALB;
						s_Panel[s_MatSize].Area =  s_Panel[s_MatSize].Normal.VAbs()/2.0;
						s_Panel[s_MatSize].Normal.Normalize();

						s_Panel[s_MatSize].m_bIsInSymPlane  = false;
						s_Panel[s_MatSize].m_bIsLeading     = false;
						s_Panel[s_MatSize].m_bIsTrailing    = false;
						s_Panel[s_MatSize].m_Pos = BODYSURFACE;
						s_Panel[s_MatSize].m_iElement = s_MatSize;
						s_Panel[s_MatSize].m_bIsLeftPanel  = true;
						s_Panel[s_MatSize].SetPanelFrame(LA, LB, TA, TB);

						// set neighbour panels

						s_Panel[s_MatSize].m_iPD = s_MatSize + nh;
						s_Panel[s_MatSize].m_iPU = s_MatSize - nh;

						if(lnx==0)      s_Panel[s_MatSize].m_iPU = -1;// no panel downstream
						if(lnx==nx-1)	s_Panel[s_MatSize].m_iPD = -1;// no panel upstream

						s_Panel[s_MatSize].m_iPL = s_MatSize + 1;
						s_Panel[s_MatSize].m_iPR = s_MatSize - 1;

						if(lnh==0)     s_Panel[s_MatSize].m_iPR = InitialSize + FullSize - p - 1;
						if(lnh==nh-1)  s_Panel[s_MatSize].m_iPL = InitialSize + FullSize - p - 1;

						s_MatSize++;
						p++;
						LB = LA;
						TB = TA;
						lnh++;
					}
				}
				lnx++;
			}
		}
	}
	else if(pCurBody->m_LineType==BODYSPLINETYPE)
	{
		FullSize = 2*nx*nh;
		//start with left side... same as for wings
		for (k=0; k<nx; k++)
		{
			uk  = pCurBody->s_XPanelPos[k];
			uk1 = pCurBody->s_XPanelPos[k+1];

			pCurBody->GetPoint(uk,  0, false, LB);
			pCurBody->GetPoint(uk1, 0, false, TB);

			LB.x += dpx;
			LB.z += dpz;
			TB.x += dpx;
			TB.z += dpz;

			for (l=0; l<nh; l++)
			{
				//start with left side... same as for wings
				v = (double)(l+1) / (double)(nh);
				pCurBody->GetPoint(uk,  v, false, LA);
				pCurBody->GetPoint(uk1, v, false, TA);

				LA.x += dpx;
				LA.z += dpz;
				TA.x += dpx;
				TA.z += dpz;

				n0 = IsNode(LA);
				n1 = IsNode(TA);
				n2 = IsNode(LB);
				n3 = IsNode(TB);

				if(n0>=0) {
					s_Panel[s_MatSize].m_iLA = n0;
				}
				else {
					s_Panel[s_MatSize].m_iLA = s_nNodes;
					s_Node[s_nNodes].Copy(LA);
					s_nNodes++;
				}

				if(n1>=0) {
					s_Panel[s_MatSize].m_iTA = n1;
				}
				else {
					s_Panel[s_MatSize].m_iTA = s_nNodes;
					s_Node[s_nNodes].Copy(TA);
					s_nNodes++;
				}

				if(n2>=0) {
					s_Panel[s_MatSize].m_iLB = n2;
				}
				else {
					s_Panel[s_MatSize].m_iLB = s_nNodes;
					s_Node[s_nNodes].Copy(LB);
					s_nNodes++;
				}

				if(n3 >=0) {
					s_Panel[s_MatSize].m_iTB = n3;
				}
				else {
					s_Panel[s_MatSize].m_iTB = s_nNodes;
					s_Node[s_nNodes].Copy(TB);
					s_nNodes++;
				}

				LATB = TB - LA;
				TALB = LB - TA;
				s_Panel[s_MatSize].Normal = LATB * TALB;
				s_Panel[s_MatSize].Area =  s_Panel[s_MatSize].Normal.VAbs()/2.0;
				s_Panel[s_MatSize].Normal.Normalize();

				s_Panel[s_MatSize].m_bIsInSymPlane  = false;
				s_Panel[s_MatSize].m_bIsLeading     = false;
				s_Panel[s_MatSize].m_bIsTrailing    = false;
				s_Panel[s_MatSize].m_Pos = BODYSURFACE;
				s_Panel[s_MatSize].m_iElement = s_MatSize;
				s_Panel[s_MatSize].m_bIsLeftPanel  = true;
				s_Panel[s_MatSize].SetPanelFrame(LA, LB, TA, TB);

				// set neighbour panels

				s_Panel[s_MatSize].m_iPD = s_MatSize + nh;
				s_Panel[s_MatSize].m_iPU = s_MatSize - nh;

				if(k==0)    s_Panel[s_MatSize].m_iPU = -1;// no panel downstream
				if(k==nx-1)	s_Panel[s_MatSize].m_iPD = -1;// no panel upstream

				s_Panel[s_MatSize].m_iPL = s_MatSize + 1;
				s_Panel[s_MatSize].m_iPR = s_MatSize - 1;

				if(l==0)     s_Panel[s_MatSize].m_iPR = InitialSize + FullSize - p - 1;
				if(l==nh-1)  s_Panel[s_MatSize].m_iPL = InitialSize + FullSize - p - 1;

				s_MatSize++;
				p++;
				LB = LA;
				TB = TA;
			}
		}
	}

	//right side next
	i = s_MatSize;
	//right side next
	for (k=nx-1; k>=0; k--)
	{
		for (l=nh-1; l>=0; l--)
		{
			i--;
			LA = s_Node[s_Panel[i].m_iLB];
			TA = s_Node[s_Panel[i].m_iTB];
			LB = s_Node[s_Panel[i].m_iLA];
			TB = s_Node[s_Panel[i].m_iTA];

			LA.y = -LA.y;
			LB.y = -LB.y;
			TA.y = -TA.y;
			TB.y = -TB.y;

			n0 = IsNode(LA);
			n1 = IsNode(TA);
			n2 = IsNode(LB);
			n3 = IsNode(TB);

			if(n0>=0) {
				s_Panel[s_MatSize].m_iLA = n0;
			}
			else {
				s_Panel[s_MatSize].m_iLA = s_nNodes;
				s_Node[s_nNodes].Copy(LA);
				s_nNodes++;
			}

			if(n1>=0) {
				s_Panel[s_MatSize].m_iTA = n1;
			}
			else {
				s_Panel[s_MatSize].m_iTA = s_nNodes;
				s_Node[s_nNodes].Copy(TA);
				s_nNodes++;
			}

			if(n2>=0) {
				s_Panel[s_MatSize].m_iLB = n2;
			}
			else {
				s_Panel[s_MatSize].m_iLB = s_nNodes;
				s_Node[s_nNodes].Copy(LB);
				s_nNodes++;
			}

			if(n3 >=0) {
				s_Panel[s_MatSize].m_iTB = n3;
			}
			else {
				s_Panel[s_MatSize].m_iTB = s_nNodes;
				s_Node[s_nNodes].Copy(TB);
				s_nNodes++;
			}

			LATB = TB - LA;
			TALB = LB - TA;
			s_Panel[s_MatSize].Normal = LATB * TALB;
			s_Panel[s_MatSize].Area =  s_Panel[s_MatSize].Normal.VAbs()/2.0;
			s_Panel[s_MatSize].Normal.Normalize();

			s_Panel[s_MatSize].m_bIsInSymPlane  = false;
			s_Panel[s_MatSize].m_bIsLeading     = false;
			s_Panel[s_MatSize].m_bIsTrailing    = false;
			s_Panel[s_MatSize].m_Pos = BODYSURFACE;
			s_Panel[s_MatSize].m_iElement = s_MatSize;
			s_Panel[s_MatSize].m_bIsLeftPanel  = false;
			s_Panel[s_MatSize].SetPanelFrame(LA, LB, TA, TB);

			// set neighbour panels
			// valid only for Panel Analysis

			s_Panel[s_MatSize].m_iPD = s_MatSize - nh;
			s_Panel[s_MatSize].m_iPU = s_MatSize + nh;

			if(k==0)	s_Panel[s_MatSize].m_iPU = -1;// no panel downstream
			if(k==nx-1)	s_Panel[s_MatSize].m_iPD = -1;// no panel upstream

			s_Panel[s_MatSize].m_iPL = s_MatSize + 1;
			s_Panel[s_MatSize].m_iPR = s_MatSize - 1;

			if(l==0)     s_Panel[s_MatSize].m_iPL = InitialSize + FullSize - p - 1;
			if(l==nh-1)  s_Panel[s_MatSize].m_iPR = InitialSize + FullSize - p - 1;

			s_MatSize++;
			p++;
			LB = LA;
			TB = TA;
		}
	}
	pCurBody->m_NElements = s_MatSize-InitialSize;
	return pCurBody->m_NElements;
}


/**
* Creates the panel elements for the active surface.
* The panels are appended at the end of the existing array of panel
* The panels are created from left to right on a surface
* The panels are created depending on the current WPolar:
*   No WPolar --> panel elements on top & bottom surfaces, just for cosmetics
*   VLM       --> panel elements on mid camber line from T.E. to L.E.
*   3D Panels --> panel elements on left tip surface
*                 panel elements on each strip, starting on the bottom T.E. to the L.E back to the opt T.E
*                 panel elements on right tip surface
*
*@param a pointer to the surface for which the panels will be created
*@return the number of panels which have been created and appended
*/
int Objects3D::createWingElements(Plane *pPlane, WPolar *pWPolar, Surface *pSurface)
{
	//TODO : for  a gap at the wing's center, need to separate m_iPL and m_iPR at the tips;
	bool bNoJoinFlap;
	int k,l;
	int n0, n1, n2, n3;

	int InitialSize = s_MatSize;
	enumPanelPosition side;
	CVector LA, LB, TA, TB;

	bool bThickSurfaces = true;
	if(!pPlane->isWing()) bThickSurfaces= false;
	if(pWPolar)
	{
		if(pWPolar->m_AnalysisMethod == LLTMETHOD) bThickSurfaces = false;
		if(pWPolar->m_AnalysisMethod == VLMMETHOD) bThickSurfaces = false;
		if(pWPolar->m_bThinSurfaces) bThickSurfaces = false;
	}

	if (bThickSurfaces && pWPolar && pSurface->m_bIsTipLeft)
	{
		//then left tip surface, add side panels
		for (l=0; l<pSurface->m_NXPanels; l++)
		{
			s_Panel[s_MatSize].m_bIsLeading     = false;
			s_Panel[s_MatSize].m_bIsTrailing    = false;
			s_Panel[s_MatSize].m_bIsWakePanel   = false;
			s_Panel[s_MatSize].m_bIsInSymPlane  = false; //even if part of a fin

			pSurface->GetPanel(0, l, BOTSURFACE);
			LA.Copy(pSurface->LA);
			TA.Copy(pSurface->TA);

			pSurface->GetPanel(0, l, TOPSURFACE);
			LB.Copy(pSurface->LA);
			TB.Copy(pSurface->TA);

			n0 = IsNode(LA);
			if(n0>=0) 	s_Panel[s_MatSize].m_iLA = n0;
			else {
				s_Panel[s_MatSize].m_iLA = s_nNodes;
				s_Node[s_nNodes].Copy(LA);
				s_nNodes++;
			}

			n1 = IsNode(TA);
			if(n1>=0) 	s_Panel[s_MatSize].m_iTA = n1;
			else {
				s_Panel[s_MatSize].m_iTA = s_nNodes;
				s_Node[s_nNodes].Copy(TA);
				s_nNodes++;
			}

			n2 = IsNode(LB);
			if(n2>=0) 	s_Panel[s_MatSize].m_iLB = n2;
			else {
				s_Panel[s_MatSize].m_iLB = s_nNodes;
				s_Node[s_nNodes].Copy(LB);
				s_nNodes++;
			}

			n3 = IsNode(TB);
			if(n3>=0) 	s_Panel[s_MatSize].m_iTB = n3;
			else {
				s_Panel[s_MatSize].m_iTB = s_nNodes;
				s_Node[s_nNodes].Copy(TB);
				s_nNodes++;
			}

			s_Panel[s_MatSize].m_Pos = SIDESURFACE;
			s_Panel[s_MatSize].m_iElement = s_MatSize;
			s_Panel[s_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;
			s_Panel[s_MatSize].SetPanelFrame(LA, LB, TA, TB);
			s_Panel[s_MatSize].m_iWake = -1;

			s_Panel[s_MatSize].m_iPD = s_MatSize-1;
			s_Panel[s_MatSize].m_iPU = s_MatSize+1;
			if(l==0)                      s_Panel[s_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1) s_Panel[s_MatSize].m_iPU = -1;// no panel upstream
			s_Panel[s_MatSize].m_iPL = -1;
			s_Panel[s_MatSize].m_iPR = -1;

			s_MatSize++;
		}
	}

	for (k=0; k<pSurface->m_NYPanels; k++)
	{
		//add "horizontal" panels, mid side, or following a strip from bot to top if 3D Panel
		if(bThickSurfaces)   side = BOTSURFACE;  //start with lower surf, as recommended by K&P
		else                 side = MIDSURFACE;
		//from T.E. to L.E.
		for (l=0; l<pSurface->m_NXPanels; l++)
		{
			pSurface->GetPanel(k,l,side);

			n0 = IsNode(pSurface->LA);
			n1 = IsNode(pSurface->TA);
			n2 = IsNode(pSurface->LB);
			n3 = IsNode(pSurface->TB);

			if(l==0)                      s_Panel[s_MatSize].m_bIsTrailing = true;
			if(l==pSurface->m_NXPanels-1) s_Panel[s_MatSize].m_bIsLeading  = true;
			s_Panel[s_MatSize].m_bIsWakePanel   = false;
			s_Panel[s_MatSize].m_bIsInSymPlane  = pSurface->m_bIsInSymPlane;

			bNoJoinFlap = side==0 &&  l<pSurface->m_NXFlap && k==0;

			if(n0>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				s_Panel[s_MatSize].m_iLA = n0;
			}
			else {
				s_Panel[s_MatSize].m_iLA = s_nNodes;
				s_Node[s_nNodes].Copy(pSurface->LA);
				s_nNodes++;
			}

			if(n1>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				s_Panel[s_MatSize].m_iTA = n1;
			}
			else {
				s_Panel[s_MatSize].m_iTA = s_nNodes;
				s_Node[s_nNodes].Copy(pSurface->TA);
				s_nNodes++;
			}

			bNoJoinFlap = side==0 &&  l<pSurface->m_NXFlap && k==pSurface->m_NYPanels-1;

			if(n2>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				s_Panel[s_MatSize].m_iLB = n2;
			}
			else {
				s_Panel[s_MatSize].m_iLB = s_nNodes;
				s_Node[s_nNodes].Copy(pSurface->LB);
				s_nNodes++;
			}

			if(n3>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				s_Panel[s_MatSize].m_iTB = n3;
			}
			else {
				s_Panel[s_MatSize].m_iTB = s_nNodes;
				s_Node[s_nNodes].Copy(pSurface->TB);
				s_nNodes++;
			}

			s_Panel[s_MatSize].m_Pos = side;
			s_Panel[s_MatSize].m_iElement = s_MatSize;
			s_Panel[s_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;

			if(side==MIDSURFACE)        s_Panel[s_MatSize].SetPanelFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);
			else if (side==BOTSURFACE)  s_Panel[s_MatSize].SetPanelFrame(pSurface->LB, pSurface->LA, pSurface->TB, pSurface->TA);

			// set neighbour panels
			// valid only for Panel 2-sided Analysis
			// we are on the bottom or middle surface
			s_Panel[s_MatSize].m_iPD = s_MatSize-1;
			s_Panel[s_MatSize].m_iPU = s_MatSize+1;
			if(l==0)                                         s_Panel[s_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1 && side==MIDSURFACE) s_Panel[s_MatSize].m_iPU = -1;// no panel upstream

			if(side!=MIDSURFACE)
			{
				//wings are modelled as thick surfaces
				s_Panel[s_MatSize].m_iPL = s_MatSize + 2*pSurface->m_NXPanels;
				s_Panel[s_MatSize].m_iPR = s_MatSize - 2*pSurface->m_NXPanels;
				//todo : do not link to right wing if there is a gap in between
				if(k==0                      && pSurface->m_bIsTipLeft)		s_Panel[s_MatSize].m_iPR = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	s_Panel[s_MatSize].m_iPL = -1;
			}
			else
			{
				//wings are modelled as thin surfaces
				s_Panel[s_MatSize].m_iPR = s_MatSize + pSurface->m_NXPanels;
				s_Panel[s_MatSize].m_iPL = s_MatSize - pSurface->m_NXPanels;
				if(k==0                      && pSurface->m_bIsTipLeft)		s_Panel[s_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	s_Panel[s_MatSize].m_iPR = -1;
			}

			//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
			if(k==0)                      s_Panel[s_MatSize].m_iPR = -1;
			if(k==pSurface->m_NYPanels-1) s_Panel[s_MatSize].m_iPL = -1;

			if(pWPolar && s_Panel[s_MatSize].m_bIsTrailing && pWPolar->m_AnalysisMethod==PANELMETHOD)
			{
				s_Panel[s_MatSize].m_iWake = s_WakeSize;//next wake element
				s_Panel[s_MatSize].m_iWakeColumn = s_NWakeColumn;
				if(pWPolar->m_bThinSurfaces)
				{
					createWakeElems(s_MatSize, pPlane, pWPolar);
					s_NWakeColumn++;
				}
			}

			if(l<pSurface->m_NXFlap) pSurface->AddFlapPanel(s_Panel+s_MatSize);

			s_MatSize++;
		}

		if (bThickSurfaces)
		{
			//add top side if 3D Panels
			side = TOPSURFACE; //next upper surf, as recommended by K&P
			//from L.E. to T.E.
			for (l=pSurface->m_NXPanels-1;l>=0; l--)
			{
				pSurface->GetPanel(k,l,side);
				n0 = IsNode(pSurface->LA);
				n1 = IsNode(pSurface->TA);
				n2 = IsNode(pSurface->LB);
				n3 = IsNode(pSurface->TB);

				if(l==0)                      s_Panel[s_MatSize].m_bIsTrailing = true;
				if(l==pSurface->m_NXPanels-1) s_Panel[s_MatSize].m_bIsLeading  = true;
				s_Panel[s_MatSize].m_bIsWakePanel   = false;
				s_Panel[s_MatSize].m_bIsInSymPlane  = pSurface->m_bIsInSymPlane;

				if(n0>=0)
					s_Panel[s_MatSize].m_iLA = n0;
				else {
					s_Panel[s_MatSize].m_iLA = s_nNodes;
					s_Node[s_nNodes].Copy(pSurface->LA);
					s_nNodes++;
				}

				if(n1>=0)
					s_Panel[s_MatSize].m_iTA = n1;
				else {
					s_Panel[s_MatSize].m_iTA = s_nNodes;
					s_Node[s_nNodes].Copy(pSurface->TA);
					s_nNodes++;
				}

				if(n2>=0)
					s_Panel[s_MatSize].m_iLB = n2;
				else {
					s_Panel[s_MatSize].m_iLB = s_nNodes;
					s_Node[s_nNodes].Copy(pSurface->LB);
					s_nNodes++;
				}

				if(n3 >=0)
					s_Panel[s_MatSize].m_iTB = n3;
				else {
					s_Panel[s_MatSize].m_iTB = s_nNodes;
					s_Node[s_nNodes].Copy(pSurface->TB);
					s_nNodes++;
				}

				s_Panel[s_MatSize].m_Pos = side;
				s_Panel[s_MatSize].m_iElement = s_MatSize;
				s_Panel[s_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;

				s_Panel[s_MatSize].SetPanelFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);

				// set neighbour panels
				// valid only for Panel 2-sided Analysis
				// we are on the top surface
				s_Panel[s_MatSize].m_iPD = s_MatSize+1;
				s_Panel[s_MatSize].m_iPU = s_MatSize-1;
				if(l==0)                      s_Panel[s_MatSize].m_iPD = -1;// no panel downstream
//				if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_iPU = -1;// no panel upstream

				s_Panel[s_MatSize].m_iPL = s_MatSize - 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution
				s_Panel[s_MatSize].m_iPR = s_MatSize + 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution

				if(k==0                      && pSurface->m_bIsTipLeft)		s_Panel[s_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	s_Panel[s_MatSize].m_iPR = -1;

				//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
				if(k==0)                      s_Panel[s_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1) s_Panel[s_MatSize].m_iPR = -1;


				if(pWPolar && s_Panel[s_MatSize].m_bIsTrailing && pWPolar->m_AnalysisMethod==PANELMETHOD)
				{
					s_Panel[s_MatSize].m_iWake = s_WakeSize;//next wake element
					s_Panel[s_MatSize].m_iWakeColumn = s_NWakeColumn;
					createWakeElems(s_MatSize, pPlane, pWPolar);
				}

				if(l<pSurface->m_NXFlap) pSurface->AddFlapPanel(s_Panel+s_MatSize);
				s_MatSize++;
			}
			s_NWakeColumn++;
		}
	}

	if (bThickSurfaces && pWPolar && pSurface->m_bIsTipRight)
	{	//right tip surface
		k = pSurface->m_NYPanels-1;
		for (l=0; l< pSurface->m_NXPanels; l++)
		{
			s_Panel[s_MatSize].m_bIsTrailing    = false;
			s_Panel[s_MatSize].m_bIsLeading     = false;
			s_Panel[s_MatSize].m_bIsWakePanel   = false;
			s_Panel[s_MatSize].m_bIsInSymPlane  = false;//even if part of a fin

			pSurface->GetPanel(k,l,TOPSURFACE);
			LA.Copy(pSurface->LB);
			TA.Copy(pSurface->TB);

			pSurface->GetPanel(k,l,BOTSURFACE);
			LB.Copy(pSurface->LB);
			TB.Copy(pSurface->TB);

			n0 = IsNode(LA);//answer should be yes
			if(n0>=0) 				s_Panel[s_MatSize].m_iLA = n0;
			else {
				s_Panel[s_MatSize].m_iLA = s_nNodes;
				s_Node[s_nNodes].Copy(LA);
				s_nNodes++;
			}
			n1 = IsNode(TA);//answer should be yes
			if(n1>=0) 				s_Panel[s_MatSize].m_iTA = n1;
			else {
				s_Panel[s_MatSize].m_iTA = s_nNodes;
				s_Node[s_nNodes].Copy(TA);
				s_nNodes++;
			}
			n2 = IsNode(LB);//answer should be yes
			if(n2>=0) 				s_Panel[s_MatSize].m_iLB = n2;
			else {
				s_Panel[s_MatSize].m_iLB = s_nNodes;
				s_Node[s_nNodes].Copy(LB);
				s_nNodes++;
			}
			n3 = IsNode(TB);//answer should be yes
			if(n3>=0)				s_Panel[s_MatSize].m_iTB = n3;
			else {
				s_Panel[s_MatSize].m_iTB = s_nNodes;
				s_Node[s_nNodes].Copy(TB);
				s_nNodes++;
			}


			s_Panel[s_MatSize].m_iPD = s_MatSize-1;
			s_Panel[s_MatSize].m_iPU = s_MatSize+1;
			if(l==0)                      s_Panel[s_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1) s_Panel[s_MatSize].m_iPU = -1;// no panel upstream
			s_Panel[s_MatSize].m_iPL = -1;
			s_Panel[s_MatSize].m_iPR = -1;

			s_Panel[s_MatSize].m_Pos = SIDESURFACE;
			s_Panel[s_MatSize].m_iElement = s_MatSize;
			s_Panel[s_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;
			s_Panel[s_MatSize].SetPanelFrame(LA, LB, TA, TB);
			s_Panel[s_MatSize].m_iWake = -1;
			s_MatSize++;
		}
	}

	pSurface->m_NElements = s_MatSize-InitialSize;
	return pSurface->m_NElements;
}


/**
* Creates a column of wake elements shed from a panel at the trailing edge of the wing's surface
* @param PanelIndex the index of the panel on the trailing edge of the surface which will shed the column of wake panels
*/
bool Objects3D::createWakeElems(int PanelIndex, Plane *pPlane, WPolar* pWPolar)
{
	if(!pWPolar) return false;
	if(!s_Panel[PanelIndex].m_bIsTrailing) return false;
	//creates elements trailing panel p
	int l, n0, n1,n2, n3;
	double dxA, dxB;
	int mw = s_WakeSize;// number of wake panels
	CVector LATB, TALB;
	CVector LA, LB, TA,TB;//wake panel's corner points

	int NXWakePanels;
	double WakePanelFactor;
	double TotalWakeLength;

//	double slope = m_pSurface[0]->m_pFoilA->GetCamberSlope(1.0);
//	CVector TE(1.0,0.0,slope);
//	TE.Normalize();


	NXWakePanels    = pWPolar->m_NXWakePanels;
	WakePanelFactor = pWPolar->m_WakePanelFactor;
	TotalWakeLength = pWPolar->m_TotalWakeLength;

	TA = s_Node[s_Panel[PanelIndex].m_iTA];
	TB = s_Node[s_Panel[PanelIndex].m_iTB];
	dxA = TotalWakeLength*pPlane->mac() - s_Node[s_Panel[PanelIndex].m_iTA].x;
	dxB = TotalWakeLength*pPlane->mac() - s_Node[s_Panel[PanelIndex].m_iTB].x;

	if(WakePanelFactor==1.0)
	{
		dxA /= NXWakePanels;
		dxB /= NXWakePanels;
	}
	else
	{
		dxA *= (1.0-WakePanelFactor)/(1.0-pow(WakePanelFactor, NXWakePanels));
		dxB *= (1.0-WakePanelFactor)/(1.0-pow(WakePanelFactor, NXWakePanels));
	}

	for (l=0; l<NXWakePanels; l++)
	{
		LA = TA;
		LB = TB;
		TA.x += dxA;
		TB.x += dxB;
//		TA += TE * dxA;
//		TB += TE * dxB;
		dxA *= WakePanelFactor;
		dxB *= WakePanelFactor;

		n0 = IsWakeNode(LA);
		n1 = IsWakeNode(TA);
		n2 = IsWakeNode(LB);
		n3 = IsWakeNode(TB);

		if(n0>=0) {
			s_WakePanel[mw].m_iLA = n0;
		}
		else {
			s_WakePanel[mw].m_iLA = s_nWakeNodes;
			s_WakeNode[s_nWakeNodes].Copy(LA);
			s_nWakeNodes++;
		}

		if(n1>=0) {
			s_WakePanel[mw].m_iTA = n1;
		}
		else {
			s_WakePanel[mw].m_iTA = s_nWakeNodes;
			s_WakeNode[s_nWakeNodes].Copy(TA);
			s_nWakeNodes++;
		}

		if(n2>=0) {
			s_WakePanel[mw].m_iLB = n2;
		}
		else {
			s_WakePanel[mw].m_iLB = s_nWakeNodes;
			s_WakeNode[s_nWakeNodes].Copy(LB);
			s_nWakeNodes++;
		}

		if(n3 >=0) {
			s_WakePanel[mw].m_iTB = n3;
		}
		else {
			s_WakePanel[mw].m_iTB = s_nWakeNodes;
			s_WakeNode[s_nWakeNodes].Copy(TB);
			s_nWakeNodes++;
		}

		LATB = TB - LA;
		TALB = LB - TA;

		s_WakePanel[mw].Normal = LATB * TALB;

		s_WakePanel[mw].m_Pos = MIDSURFACE;
		s_WakePanel[mw].m_bIsWakePanel = true;
		s_WakePanel[mw].Area =  s_WakePanel[mw].Normal.VAbs()/2.0;
		s_WakePanel[mw].Normal.Normalize();
		s_WakePanel[mw].SetPanelFrame(LA,LB, TA, TB);
		s_WakePanel[mw].m_bIsLeftPanel  = false;

		if(l==0)					s_WakePanel[mw].m_iPD = -1;// no panel downstream
		else if(l==NXWakePanels)	s_WakePanel[mw].m_iPU = -1;// no panel upstream
		else						s_WakePanel[mw].m_iPD = mw+1;

		s_WakePanel[mw].m_iPL = -1;
		s_WakePanel[mw].m_iPR = -1;

		mw++;
	}

	s_WakeSize = mw;
	return true;
}


int Objects3D::calculateMatSize(Plane *pPlane, WPolar *pWPolar)
{
	int nx, nh;
	int PanelArraySize = 0;

	if(!pPlane) return 0;

	//Count the wing panels
	for (int js=0; js<s_SurfaceList.size(); js++)
	{
		PanelArraySize += s_SurfaceList.at(js)->m_NXPanels * s_SurfaceList.at(js)->m_NYPanels ;
	}

	if(pPlane->isWing() && (!pWPolar || !pWPolar->m_bThinSurfaces))
	{
		PanelArraySize *= 2;
		for (int js=0; js<s_SurfaceList.size(); js++)
		{
			if(s_SurfaceList.at(js)->m_bIsTipLeft || s_SurfaceList.at(js)->m_bIsTipRight)
				PanelArraySize += s_SurfaceList.at(js)->m_NXPanels;//tip patches
		}
	}

	// add the number of body panels
	//create the body elements only if there is a body, and the analysis is not of the VLM Type
	if(pPlane && pPlane->body())
	{
		Body *pCurBody = pPlane->body();

		if(pWPolar && pWPolar->m_AnalysisMethod==PANELMETHOD && pWPolar->m_bIgnoreBodyPanels)
		{
		}
		else
		{
			if(pCurBody->m_LineType==BODYPANELTYPE)
			{
				nx = 0;
				for(int i=0; i<pCurBody->FrameSize()-1; i++) nx+=pCurBody->m_xPanels[i];
				nh = 0;
				for(int i=0; i<pCurBody->SideLineCount()-1; i++) nh+=pCurBody->m_hPanels[i];
				PanelArraySize += nx*nh*2;
			}
			else PanelArraySize += 2 * pCurBody->m_nxPanels * pCurBody->m_nhPanels;
		}
	}
	return PanelArraySize;
}



/**
 * Deletes a Plane object from the array.
 * @param pPlane a pointer to the Plane object to be deleted
 */
void Objects3D::deletePlane(Plane *pPlane)
{
	if(!pPlane || !pPlane->planeName().length()) return;
	Plane *pOldPlane;
	int i;

	deletePlaneResults(pPlane);

	for (i=s_oaPlane.size()-1; i>=0; i--)
	{
		pOldPlane = (Plane*)s_oaPlane.at(i);
		if (pOldPlane == pPlane)
		{
			s_oaPlane.removeAt(i);
			delete pPlane;
			break;
		}
	}
}


/**
 * Deletes the WPolar and PlaneOpp objects associated to the plane.
 * @param pPlane a pointer to the Plane object for which the results will be deleted
 */
void Objects3D::deletePlaneResults(Plane *pPlane)
{
	if(!pPlane || !pPlane->planeName().length()) return ;
	WPolar* pWPolar;
	PlaneOpp * pPOpp;

	int i;

	//first remove all POpps associated to the plane
	for (i=s_oaPOpp.size()-1; i>=0; i--)
	{
		pPOpp = (PlaneOpp*)s_oaPOpp.at(i);
		if(pPOpp->m_PlaneName == pPlane->planeName())
		{
			s_oaPOpp.removeAt(i);
			delete pPOpp;
		}
	}

	//next remove all PPolars associated to the plane
	for (i=s_oaWPolar.size()-1; i>=0; i--)
	{
		pWPolar = (WPolar*)s_oaWPolar.at(i);
		if (pWPolar->m_PlaneName == pPlane->planeName())
		{
			pWPolar->ClearData();
			//results only... means that the areas and spans have been edited... update polar
			if( pWPolar->m_RefAreaType==PLANFORMAREA)
			{
				pWPolar->m_WArea = pPlane->m_Wing[0].m_PlanformArea;
				pWPolar->m_WSpan = pPlane->m_Wing[0].m_PlanformSpan;
			}
			else
			{
				pWPolar->m_WArea = pPlane->m_Wing[0].m_ProjectedArea;
				pWPolar->m_WSpan = pPlane->m_Wing[0].m_ProjectedSpan;
			}
			pWPolar->m_WMAChord  = pPlane->m_Wing[0].m_MAChord;
		}
	}
}




/**
* Returns a pointer to the body with the name BodyName
*@param BodyName the QString holding the name of the body
*@return a pointer to the body with the requested body name, or NULL if none has been found
*/
Body * Objects3D::getBody(QString BodyName)
{
	Body* pBody;
	for (int ib=0; ib<s_oaBody.size(); ib++)
	{
		pBody = (Body*)s_oaBody.at(ib);
		if (pBody->m_BodyName == BodyName) return pBody;
	}

	Plane *pPlane;
	for(int ip=0; ip<s_oaPlane.size(); ip++)
	{
		pPlane = (Plane*)s_oaPlane.at(ip);
		if(pPlane->body())
		{
			if((pPlane->planeName()+"/Body") == BodyName) return pPlane->body();
		}
	}

	return NULL;
}



/**
* Returns a pointer to the OpPoint with the name of the current plane and current WPolar, and which matches the input parameter
* @param x the aoa, of velocity, or control parameter for which the CPOpp object is requested
* @return a pointer to the instance of the CPOpp object, or NULL if non has been found
*/
PlaneOpp * Objects3D::getPlaneOpp(Plane *pPlane, WPolar* pWPolar, double x)
{
	int i;
	if(!pPlane || !pWPolar) return NULL;

	PlaneOpp* pPOpp;

	for (i=0; i<s_oaPOpp.size(); i++)
	{
		pPOpp = (PlaneOpp*)s_oaPOpp.at(i);
		if ((pPOpp->m_PlaneName == pPlane->planeName()) && (pPOpp->m_PlrName == pWPolar->m_WPlrName))
		{
			if     (pWPolar->m_WPolarType< FIXEDAOAPOLAR && qAbs(pPOpp->m_Alpha - x)<0.005)  return pPOpp;
			else if(pWPolar->m_WPolarType==FIXEDAOAPOLAR  && qAbs(pPOpp->m_QInf - x)<0.005)  return pPOpp;
			else if(pWPolar->m_WPolarType==STABILITYPOLAR && qAbs(pPOpp->m_Ctrl - x)<0.005)  return pPOpp;
		}
	}
	return NULL;
}


/**
* Returns a pointer to the polar with the name of the input parameter
* @param WPolarName the name of the CWPolar object
* @return a pointer to the instance of the CWPolar object, or NULL if non has been found
*/
WPolar* Objects3D::getWPolar(Plane *pPlane, QString WPolarName)
{
	WPolar *pWPolar;
	int i;
	if(!pPlane) return NULL;

	for (i=0; i<s_oaWPolar.size(); i++)
	{
		pWPolar = (WPolar*)s_oaWPolar.at(i);
		if (pWPolar->m_PlaneName == pPlane->planeName() && pWPolar->m_WPlrName == WPolarName)
			return pWPolar;
	}
	return NULL;
}


/**
* Returns a pointer to the plane with the name PlaneName
*@param PlaneName a QString object with the name of the requested CPlane object
*@return a pointer to the instance of the CPlane object, or NULL if non has been found
*/
Plane * Objects3D::getPlane(QString PlaneName)
{
	int i;
	Plane* pPlane;
	for (i=0; i<s_oaPlane.size(); i++)
	{
		pPlane = (Plane*)s_oaPlane.at(i);
		if (pPlane->planeName() == PlaneName) return pPlane;
	}
	return NULL;
}





/**
 * Returns a pointer to the main wing of the plane defined by its name
 * @param PlaneName the source Plane's name
 * @return a pointer to the Plane's main wing.
 */
Wing* Objects3D::getWing(QString PlaneName)
{
	for(int ip=0; ip<s_oaPlane.size(); ip++)
	{
		Plane *pPlane = (Plane*)s_oaPlane.at(ip);
		if(QString::compare(pPlane->planeName(), PlaneName, Qt::CaseInsensitive)==0) return pPlane->m_Wing;
	}
	return NULL;
}



/**
 * Inserts a modified Plane object in the array, i.a.w. user instructions
 * @param pModPlane a pointer to the instance of the Plane object to be inserted
 * @return a pointer to the Plane object which was successfully inserted, false otherwise
 */
Plane * Objects3D::setModPlane(Plane *pModPlane)
{
	if(!pModPlane) return NULL;
	Plane * pPlane;
	bool bExists = true;
	int resp, k, l;

	QString OldName = pModPlane->planeName();

	QStringList NameList;
	for(k=0; k<s_oaPlane.size(); k++)
	{
		pPlane = (Plane*)s_oaPlane.at(k);
		NameList.append(pPlane->planeName());
	}

	RenameDlg renDlg;
	renDlg.InitDialog(&NameList, pModPlane->planeName(),QObject::tr("Enter the new name for the Plane :"));

	while (bExists)
	{
		resp = renDlg.exec();
		if(resp==QDialog::Accepted)
		{
			if (OldName == renDlg.newName()) return pModPlane;

			//Is the new name already used ?
			bExists = false;
			for (k=0; k<s_oaPlane.size(); k++)
			{
				pPlane = (Plane*)s_oaPlane.at(k);
				if (pPlane->planeName() == renDlg.newName())
				{
					bExists = true;
					break;
				}
			}

			if(!bExists)
			{
				// we have a valid name
				// rename the plane
				pModPlane->RenamePlane(renDlg.newName());

				bool bInserted = false;
				for (l=0; l<s_oaPlane.size();l++)
				{
					pPlane = (Plane*)s_oaPlane.at(l);
					if(pPlane == pModPlane)
					{
						// remove the current Plane from the array
						s_oaPlane.removeAt(l);
						// but don't delete it !
						break;
					}
				}
				//and re-insert it
				for (l=0; l<s_oaPlane.size();l++)
				{
					pPlane = (Plane*)s_oaPlane.at(l);
					if(pPlane->planeName().compare(pModPlane->planeName(), Qt::CaseInsensitive) >0)
					{
						//then insert before
						s_oaPlane.insert(l, pModPlane);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)	s_oaPlane.append(pModPlane);
				break;

			}
		}
		else if(resp ==10)
		{
			//the user wants to overwrite the old plane/wing
			if (OldName == renDlg.newName()) return pModPlane;//don't bother to overwrite itself

			pPlane = getPlane(renDlg.newName());
			deletePlane(pPlane);

			pModPlane->RenamePlane(renDlg.newName());

			//place the Plane in alphabetical order in the array
			//remove the current Plane from the array
			for (l=0; l<s_oaPlane.size();l++)
			{
				pPlane = (Plane*)s_oaPlane.at(l);
				if(pPlane == pModPlane)
				{
					s_oaPlane.removeAt(l);
					// but don't delete it !
					break;
				}
			}
			//and re-insert it
			bool bInserted = false;
			for (l=0; l<s_oaPlane.size();l++)
			{
				pPlane = (Plane*)s_oaPlane.at(l);
				if(pPlane->planeName().compare(pModPlane->planeName(), Qt::CaseInsensitive) <0)
				{
					//then insert before
					s_oaPlane.insert(l, pModPlane);
					bInserted = true;
					break;
				}
			}
			if(!bInserted) s_oaPlane.append(pModPlane);
			bExists = false;
		}
		else
		{
			return NULL;//cancelled
		}
	}
	return pModPlane;
}



/**
 * Inserts a new WPolar object in the array.
 * If the WPolar's name already exists, finds a new one, eventually by overwriting an old WPolar.
 * @param pModWPolar a pointer to the instance of the WPolar object to be inserted
 * @return a pointer to the polar which has been set, or NULL if failure
 */
WPolar* Objects3D::insertNewWPolar(WPolar *pNewWPolar, Plane *pCurPlane)
{
	if(!pNewWPolar) return NULL;
	WPolar *pWPolar, *pOldWPolar;

	bool bExists = true;
	int resp, k, l;

	//check if this WPolar is already inserted
	for(int ip=0; ip<s_oaWPolar.size(); ip++)
	{
		pOldWPolar = (WPolar*)s_oaWPolar.at(ip);
		if(pOldWPolar==pNewWPolar)
		{
			Trace("this WPolar is already in the array, nothing inserted");
			return NULL;
		}
	}

	//make a list of existing names
	QStringList NameList;
	for(k=0; k<s_oaWPolar.size(); k++)
	{
		pWPolar = (WPolar*)s_oaWPolar.at(k);
		if(pWPolar->m_PlaneName==pCurPlane->planeName()) NameList.append(pWPolar->m_WPlrName);
	}

	//Is the new WPolar's name already used ?
	bExists = false;
	for (k=0; k<NameList.count(); k++)
	{
		if(pNewWPolar->polarName()==NameList.at(k))
		{
			bExists = true;
			break;
		}
	}

	if(!bExists)
	{
		//just insert the WPolar in alphabetical order
		for (l=0; l<s_oaWPolar.size();l++)
		{
			pOldWPolar = (WPolar*)s_oaWPolar.at(l);

			if(pOldWPolar->m_WPlrName.compare(pNewWPolar->m_WPlrName, Qt::CaseInsensitive) >0)
			{
				//then insert before
				s_oaWPolar.insert(l, pNewWPolar);
				return pNewWPolar;
			}
		}
		//not inserted, append
		s_oaWPolar.append(pNewWPolar);
		return pNewWPolar;
	}

	// an old object with the WPolar's name exists for this Plane, ask for a new one
	RenameDlg dlg;
	dlg.InitDialog(&NameList, pNewWPolar->polarName(), QObject::tr("Enter the new name for the Polar:"));
	resp = dlg.exec();

	if(resp==10)
	{
		//user wants to overwrite an existing name
		//so find the existing WPolar with that name
		pWPolar = NULL;
		for(int ipb=0; ipb<s_oaWPolar.size(); ipb++)
		{
			pOldWPolar = (WPolar*)s_oaWPolar.at(ipb);
			if(pOldWPolar->polarName()==pNewWPolar->polarName() && pOldWPolar->m_PlaneName==pCurPlane->planeName())
			{
				pWPolar = pOldWPolar;
				break;
			}
		}

		if(pWPolar)
		{
			//remove and delete its children POpps from the array
			for (l=s_oaPOpp.size()-1;l>=0; l--)
			{
				PlaneOpp *pPOpp = (PlaneOpp*)s_oaPOpp.at(l);
				if (pPOpp->planeName()==pWPolar->planeName() && pPOpp->polarName()==pWPolar->polarName())
				{
					s_oaPOpp.removeAt(l);
					delete pPOpp;
				}
			}
		}

		//room has been made, insert the new WPolar in alphabetical order
		for (l=0; l<s_oaWPolar.size();l++)
		{
			pOldWPolar = (WPolar*)s_oaWPolar.at(l);

			if(pOldWPolar->m_WPlrName.compare(pNewWPolar->m_WPlrName, Qt::CaseInsensitive) >0)
			{
				//then insert before
				s_oaWPolar.insert(l, pNewWPolar);
				return pNewWPolar;
			}
		}
		//not inserted, append
		s_oaWPolar.append(pNewWPolar);
		return pNewWPolar;

	}
	else if(resp==QDialog::Rejected)
	{
		return NULL;
	}
	else if(resp==QDialog::Accepted)
	{
		//not rejected, no overwrite, else the user has selected a non-existing name, rename and insert
		pNewWPolar->m_WPlrName=dlg.newName();
		for (l=0; l<s_oaWPolar.size();l++)
		{
			pOldWPolar = (WPolar*)s_oaWPolar.at(l);

			if(pOldWPolar->m_WPlrName.compare(pNewWPolar->m_WPlrName, Qt::CaseInsensitive) >0)
			{
				//then insert before
				s_oaWPolar.insert(l, pNewWPolar);
				return pNewWPolar;
			}
		}
		//not inserted, append
		s_oaWPolar.append(pNewWPolar);
		return pNewWPolar;

	}
	return NULL;//should never gethere
}




/**
 * Sets the active polar
 * Builds the array of panels depending on the polar type
 * @param bCurrent if true, the active polar is set anew
 * @param WPlrName the name of the polar to set for the active wing or plane
 */
WPolar* Objects3D::setWPolarObject(Plane *pCurPlane, WPolar *pCurWPolar, bool bCurrentWPolar, QString WPlrName)
{
	WPolar *pWPolar = NULL;
	WPolar *pOldWPolar = NULL;
	QString PlaneName;
	int i,j,k,m, NStation;
	double SpanPos;

	if(pCurPlane)     PlaneName = pCurPlane->planeName();
	else return NULL;

	if(bCurrentWPolar)
	{
		//if we already know which WPolar object
		pWPolar = pCurWPolar;
	}

	if(!pWPolar && WPlrName.length())
	{
		//if we know its name
		pWPolar = getWPolar(pCurPlane, WPlrName);
	}

	if(!pWPolar)
	{
		//if we don't know anything, find the first for this plane
		for (i=0; i<s_oaWPolar.size(); i++)
		{
			pOldWPolar = (WPolar*)s_oaWPolar.at(i);
			if (pOldWPolar->m_PlaneName == PlaneName)
			{
				pWPolar = pOldWPolar;
				break;
			}
		}
	}


	if (!initializePanels(pCurPlane, pWPolar)) return NULL;

	if(!pWPolar) return NULL;

	//initialize the analysis pointers.
	//do it now, in case the user asks for streamlines from an existing file
	s_pPanelAnalysis->setWPolar(pWPolar);
	s_pPanelAnalysis->setObjectPointers(pCurPlane, &Objects3D::s_SurfaceList);
	s_pPanelAnalysis->setArrayPointers(s_Panel, s_MemPanel, s_WakePanel, s_RefWakePanel, s_Node, s_MemNode, s_WakeNode, s_RefWakeNode);
	s_pPanelAnalysis->setArraySize(Objects3D::s_MatSize, Objects3D::s_WakeSize, Objects3D::s_nNodes, Objects3D::s_nWakeNodes, Objects3D::s_NWakeColumn);


	//set sideslip
	CVector RefPoint(0.0, 0.0, 0.0);
	if(qAbs(pWPolar->m_Beta)>0.001)
	{
		rotateGeomZ(s_MemPanel, s_MemNode, s_WakePanel, s_WakeNode,
					s_MatSize, s_WakeSize, s_nNodes, s_nWakeNodes, s_NWakeColumn,
					pWPolar->m_Beta, RefPoint, pWPolar->m_NXWakePanels);
	}

	Wing *pWingList[MAXWINGS];
	pWingList[0] = pCurPlane->wing();
	pWingList[1] = pCurPlane->wing2();
	pWingList[2] = pCurPlane->stab();
	pWingList[3] = pCurPlane->fin();

	if(pWPolar->m_AnalysisMethod>LLTMETHOD)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(pWingList[iw])
			{
				pWingList[iw]->ComputeChords();

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<pWingList[iw]->m_Surface.size(); j++)	NStation += pWingList[iw]->m_Surface.at(j)->m_NYPanels;

				for (j=(int)(pWingList[iw]->m_Surface.size()/2); j<pWingList[iw]->m_Surface.size(); j++)
				{
					for(k=0; k<pWingList[iw]->m_Surface.at(j)->m_NYPanels; k++)
					{
						pWingList[iw]->m_SpanPos[m+NStation/2] = SpanPos + pWingList[iw]->m_Surface.at(j)->GetStripSpanPos(k);
						m++;
					}
					SpanPos += pWingList[iw]->m_Surface.at(j)->m_Length;
				}

				for(m=0; m<NStation/2; m++) pWingList[iw]->m_SpanPos[m] = -pWingList[iw]->m_SpanPos[NStation-m-1];
			}
		}
	}
	else if(pWPolar->m_AnalysisMethod==LLTMETHOD)
	{
//			pCurPlane->m_Wing[0].m_NStation  = m_NStation;
//			pCurPlane->m_Wing[0].m_bLLT      = true;
	}


	if(pCurPlane) setPlaneOppObject(pCurPlane, pWPolar, NULL, false, s_LastWOpp);

	if(pWPolar->m_bAutoInertia)
	{
		if(pCurPlane)
		{
			pWPolar->m_Mass = pCurPlane->TotalMass();
			pWPolar->m_CoG    = pCurPlane->CoG();
			pWPolar->m_CoGIxx = pCurPlane->m_CoGIxx;
			pWPolar->m_CoGIyy = pCurPlane->m_CoGIyy;
			pWPolar->m_CoGIzz = pCurPlane->m_CoGIzz;
			pWPolar->m_CoGIxz = pCurPlane->m_CoGIxz;
		}

	}
	return pWPolar;
}


/**
* Searches for an operating point with aoa or velocity or control paramter x, for the active polar
* Sets it as active, if valid
* else sets the current WOpp, if any
* else sets the comboBox's first, if any
* else sets it to NULL
*@param bCurrent, if true, uses the x value of the current operating point; this is useful if the user has changed the polar, but wants to display the same aoa for instance
*@return true if a new valid operating point has been selected
*/
PlaneOpp* Objects3D::setPlaneOppObject(Plane *pPlane, WPolar *pWPolar, PlaneOpp *pCurPOpp, bool bCurrent, double x)
{
	if(!pPlane || !pWPolar) return NULL;

	PlaneOpp *pPOpp = NULL;
	if(bCurrent) pPOpp = pCurPOpp;
	else         pPOpp = getPlaneOpp(pPlane, pWPolar, x);

	// first restore the panel geometry
	memcpy(s_Panel, s_MemPanel, s_MatSize* sizeof(Panel));
	memcpy(s_Node,  s_MemNode,  s_nNodes * sizeof(CVector));

	if(!pPOpp)
	{
		if(pWPolar)
			pPOpp = getPlaneOpp(pPlane, pWPolar, pWPolar->m_AMem);
		else
			pPOpp = getPlaneOpp(pPlane, pWPolar, s_LastWOpp);
	}

	if(!pPOpp)
	{
		//try to select the first in the object array
		for(int iPOpp=0; iPOpp<s_oaPOpp.size(); iPOpp++)
		{
			PlaneOpp *pOldPOpp = (PlaneOpp*)s_oaPOpp.at(iPOpp);
			if(pOldPOpp->m_PlaneName==pPlane->planeName() && pOldPOpp->m_PlrName==pWPolar->polarName())
			{
				pPOpp = pOldPOpp;
				break;
			}
		}
	}


	if(pPOpp)
	{
/*		m_LastWOpp = pCurPOpp->m_Alpha;

		for(int iw=0; iw<MAXWINGS;iw++)
		{
			if(pCurPOpp->m_pPlaneWOpp[iw]) m_pWOpp[iw] = pCurPOpp->m_pPlaneWOpp[iw];
			else                             m_pWOpp[iw] = NULL;
		}*/

		if(pWPolar) pWPolar->m_AMem = pPOpp->m_Alpha;


		//if we have a type 7 polar, set the panels in the control's position
		if(pWPolar && pWPolar->m_WPolarType==STABILITYPOLAR)
		{
			//set the controls
			int nCtrls;
			QString strong;
			setControlPositions(pPlane, pWPolar, s_Panel, s_Node, pPOpp->m_Ctrl, nCtrls, strong, false);
		}
	}

	return pPOpp;
}



/**
 * Renames the active wing or plane
 * Updates the references in child polars and oppoints
 * @param PlaneName the new name for the wing or plane
 */
void Objects3D::renamePlane(QString PlaneName)
{
	QString OldName;
	PlaneOpp *pPOpp;
	int l;
	WPolar *pWPolar;
	Plane *pPlane = getPlane(PlaneName);
	if(pPlane)
	{
		OldName = pPlane->planeName();
		setModPlane(pPlane);

		pPlane->RenameWings();

		for (l=s_oaWPolar.size()-1;l>=0; l--)
		{
			pWPolar = (WPolar*)s_oaWPolar.at(l);
			if (pWPolar->m_PlaneName == OldName)
			{
				pWPolar->m_PlaneName = pPlane->planeName();
			}
		}
		for (l=s_oaPOpp.size()-1;l>=0; l--)
		{
			pPOpp = (PlaneOpp*)s_oaPOpp.at(l);
			if (pPOpp->m_PlaneName == OldName)
			{
				pPOpp->m_PlaneName = pPlane->planeName();
			}
		}
//		return pPlane->PlaneName();
	}
//	return "";
}







/**
 * Called by a stability analysis to modify the panels and nodes by setting the control positions to the specified position t
 * The panels and nodes to be rotated are pointed by pPanel and pNode
 * @param pPanel a pointer to the array of surface panels to be rotated
 * @param pNode  a pointer to the array of mesh nodes to be rotated
 * @param t the value of the control parameter which defines the amount of rotation
 * @param &NCtrls counts tha active controls
 * @param &out the output message for the log file
 * @param bBCOnly, if true, then only the control points and normal vector and rotates; if not,the whole geometry is rotated
 */
void Objects3D::setControlPositions(Plane *pPlane, WPolar *pWPolar, Panel *pPanel, CVector *pNode, double t, int &NCtrls, QString &out, bool bBCOnly)
{
	if(!pWPolar || !pPlane) return;

	QString strange;
	Quaternion Quat;
	int j, nFlap;
	double angle, TotalAngle;
	Wing *pWing;
	CVector YVector(0.0, 1.0, 0.0);
	CVector W;

	// update the variables & geometry
	// if plane : WingTilt, elevator Tilt
	// if flaps : wing flaps, elevator flaps

	//the CG position is fixed for this analysis

	NCtrls = 0;

	Wing *pWingList[MAXWINGS];
	pWingList[0] = pPlane->wing();
	pWingList[1] = pPlane->wing2();
	pWingList[2] = pPlane->stab();
	pWingList[3] = pPlane->fin();


	if(!pPlane->isWing())
	{
		//wing incidence
		if(qAbs(pWPolar->m_ControlGain[0])>0.0)
		{
			//wing tilt
			angle = pWPolar->m_ControlGain[0] * t; //maxcontrol is the gain
			TotalAngle = pPlane->WingTiltAngle(0) + angle;
			strange = QString::fromUtf8("         Rotating the wing by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);

			strange += "\n";
			out +=strange;

			Quat.Set(angle, YVector);

			if(bBCOnly)
			{
				for(int p=0; p<pPlane->wing()->m_MatSize; p++)
				{
					memcpy(pPanel+p, s_MemPanel+p, sizeof(Panel));
					(pPanel+p)->RotateBC(pPlane->WingLE(0), Quat);
				}
			}
			else
			{
				for(int n=0; n<s_nNodes; n++)
				{
					if(pPlane->wing()->IsWingNode(n))
					{
							pNode[n].Copy(s_MemNode[n]);
							W = pNode[n] - pPlane->WingLE(0);
							Quat.Conjugate(W);
							pNode[n] = W + pPlane->WingLE(0);
					}
				}
				for(int p=0; p<s_MatSize; p++)
				{
					if(pPlane->wing()->IsWingPanel(p)) s_Panel[p].SetPanelFrame();
				}
			}
		}
		NCtrls=1;

		if(pPlane->stab())
		{
			//elevator incidence
			if(qAbs(pWPolar->m_ControlGain[1])>0.0)
			{
				//Elevator tilt
				angle = pWPolar->m_ControlGain[1] * t; //maxcontrol is the gain
				TotalAngle = pPlane->WingTiltAngle(2) + angle;
				strange = QString::fromUtf8("         Rotating the elevator by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);

				strange += "\n";
				out +=strange;

				Quat.Set(angle, YVector);
				if(!bBCOnly)
				{
					for(int n=0; n<s_nNodes; n++)
					{
						if(pPlane->stab()->IsWingNode(n))
						{
								pNode[n].Copy(s_MemNode[n]);
								W = pNode[n] - pPlane->WingLE(2);
								Quat.Conjugate(W);
								pNode[n] = W + pPlane->WingLE(2);
						}
					}
					for(int p=0; p<s_MatSize; p++)
					{
						if(pWingList[2]->IsWingPanel(p)) s_Panel[p].SetPanelFrame();
					}
				}
				else
				{
					for(int p=0; p<pPlane->stab()->m_MatSize; p++)
					{
						pPlane->stab()->m_pWingPanel[p].RotateBC(pPlane->WingLE(2), Quat);
					}
				}
			}
			NCtrls = 2;
		}
	}

	// flap controls
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		pWing = pWingList[iw];
		if(pWing)
		{
			nFlap = 0;
			for (j=0; j<pWing->m_Surface.size(); j++)
			{
				if(pWing->m_Surface.at(j)->m_bTEFlap)
				{
					if(qAbs(pWPolar->m_ControlGain[NCtrls])>0.0)
					{

						angle = pWPolar->m_ControlGain[NCtrls] * t; //maxcontrol is the gain

						if(pWing->m_Surface.at(j)->m_pFoilA->m_TEFlapAngle && pWing->m_Surface.at(j)->m_pFoilA->m_TEFlapAngle)
							TotalAngle = angle + (pWing->m_Surface.at(j)->m_pFoilA->m_TEFlapAngle + pWing->m_Surface.at(j)->m_pFoilB->m_TEFlapAngle)/2.0;
						else
							TotalAngle = angle;

						strange = QString::fromUtf8("         Rotating the flap by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);

						strange += "\n";
						out +=strange;

						if(qAbs(angle)>PRECISION)
						{
//							pWing->m_Surface.at(j)->RotateFlap(angle, bBCOnly);
							Quat.Set(angle, pWing->m_Surface.at(j)->m_HingeVector);
							if(bBCOnly)
							{
								for(int p=0; p<s_MatSize; p++)
								{
									if(pWing->m_Surface.at(j)->IsFlapPanel(p))
									{
										memcpy(pPanel+p, s_MemPanel+p, sizeof(Panel));
										pPanel[p].RotateBC(pWing->m_Surface.at(j)->m_HingePoint, Quat);
									}
								}
							}
							else
							{
								for(int n=0; n<s_nNodes; n++)
								{
									if(pWing->m_Surface.at(j)->IsFlapNode(n))
									{
										pNode[n].Copy(s_MemNode[n]);
										W = pNode[n] - pWing->m_Surface.at(j)->m_HingePoint;
										Quat.Conjugate(W);
										pNode[n] = W + pWing->m_Surface.at(j)->m_HingePoint;
									}
								}
								for(int p=0; p<s_MatSize; p++)
								{
									if(pWing->m_Surface.at(j)->IsFlapPanel(p)) s_Panel[p].SetPanelFrame();
								}
							}
						}
					}
					nFlap++;
					NCtrls++;
				}
			}
		}
	}
}




void Objects3D::initPanelAnalysis(Plane*, WPolar* pWPolar, double V0, double VMax, double VDelta, bool bSequence)
{
	s_pPanelAnalysis->setWPolar(pWPolar);
	s_pPanelAnalysis->setRange(V0, VMax, VDelta, bSequence);
	s_pPanelAnalysis->setParams(s_MaxWakeIter);

	if(pWPolar->m_WPolarType==FIXEDAOAPOLAR)
	{
		s_pPanelAnalysis->m_Alpha      = pWPolar->m_ASpec;
	}
	else if(pWPolar->m_WPolarType==STABILITYPOLAR)
	{
		s_pPanelAnalysis->m_Alpha      = pWPolar->m_ASpec;
	}
	else
	{
		s_pPanelAnalysis->m_QInf       = pWPolar->m_QInf;
	}
}




void Objects3D::deleteObjects()
{
	// clear everything
	int i;
	void *pObj;

	for (i=s_oaPlane.size()-1; i>=0; i--)
	{
		pObj = s_oaPlane.at(i);
		s_oaPlane.removeAt(i);
		delete (Plane*)pObj;
	}

	for (i=s_oaPOpp.size()-1; i>=0; i--)
	{
		pObj = s_oaPOpp.at(i);
		s_oaPOpp.removeAt(i);
		delete (PlaneOpp*)pObj;
	}

	for (i=s_oaWPolar.size()-1; i>=0; i--)
	{
		pObj = s_oaWPolar.at(i);
		s_oaWPolar.removeAt(i);
		delete (WPolar*)pObj;
	}

	for (i=s_oaBody.size()-1; i>=0; i--)
	{
		pObj = s_oaBody.at(i);
		s_oaBody.removeAt(i);
		delete (Body*)pObj;
	}

}




/**
 * Sets the active plane
 * Tries first using the name, then the input pointer, then the first in the array
 * Sets an active polar and operating point for this plane, if any are available
 * @param PlaneName the name of the plane to be set as active
 */
Plane * Objects3D::setPlaneObject(QString PlaneName, Plane *pCurPlane)
{
	int j;
	Plane *pPlane = NULL;

	if(PlaneName.length())
	{
		pPlane = Objects3D::getPlane(PlaneName);
	}
	else
	{
		if(pCurPlane)
		{
			PlaneName = pCurPlane->planeName();
			pPlane = pCurPlane;
		}
		else
		{
			// Find out the first in Alphabetical order
			if(s_oaPlane.size())
			{
				pPlane = (Plane*)s_oaPlane.at(0);
				if(pPlane) PlaneName = pPlane->planeName();
			}
		}
	}


	if(!pPlane)
	{
		return NULL;
	}



	double dx, dz;
	dx=dz=0.0;

	if(pPlane->body())
	{
		dx = pPlane->BodyPos().x;
		dz = pPlane->BodyPos().z;
		pPlane->body()->SetKnots();
		pPlane->body()->SetPanelPos();
	}

	Body *pCurBody = pPlane->body();

	//create the array of wing Surface
	s_SurfaceList.clear();

	Wing *pWingList[MAXWINGS];
	pWingList[0] = pPlane->wing();
	pWingList[1] = pPlane->wing2();
	pWingList[2] = pPlane->stab();
	pWingList[3] = pPlane->fin();


	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(pWingList[iw])
		{
			if(!pPlane && iw==0)  pWingList[iw]->CreateSurfaces(CVector(0,0,0), 0.0, 0.0);
			else if(iw<3)         pWingList[iw]->CreateSurfaces(pPlane->WingLE(iw),   0.0, pPlane->WingTiltAngle(iw));
			else if(iw==3)        pWingList[iw]->CreateSurfaces(pPlane->WingLE(iw), -90.0, pPlane->WingTiltAngle(iw));
			for (j=0; j<pWingList[iw]->m_Surface.size(); j++)
			{
				pWingList[iw]->m_Surface.at(j)->SetSidePoints(pCurBody, dx, dz);
				s_SurfaceList.append(pWingList[iw]->m_Surface.at(j));
			}
			pWingList[iw]->ComputeBodyAxisInertia();
		}
	}

	pPlane->ComputeBodyAxisInertia();

	return pPlane;
}












