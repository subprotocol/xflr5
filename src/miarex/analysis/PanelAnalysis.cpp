/****************************************************************************

	PanelAnalysis Class
	Copyright (C) 2006-2014 Andre Deperrois adeperrois@xflr5.com

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
#include <QApplication>
#include "../../globals.h"
#include "../Objects3D.h"
#include "PanelAnalysis.h"


int PanelAnalysis::s_MaxMatSize = 0;
int PanelAnalysis::s_MaxRHSSize = VLMMAXRHS;

bool PanelAnalysis::s_bCancel = false;
bool PanelAnalysis::s_bWarning = false;
bool PanelAnalysis::s_bKeepOutOpp = false;
double PanelAnalysis::s_Progress=0.0;
int PanelAnalysis::s_TotalTime = 0;
bool PanelAnalysis::s_bTrefftz = true;


/**
* The public constructor
*/
PanelAnalysis::PanelAnalysis()
{
	m_nRHS = 0;

	m_Ai = m_Cl = m_ICd = NULL;
	m_F  = NULL;
	m_Vd = NULL;

	m_aij = m_aijWake = NULL;
	m_uRHS = m_vRHS = m_wRHS = m_pRHS = m_qRHS = m_rRHS = NULL;
	m_cRHS = m_uWake = m_wWake = NULL;
	m_uVl = m_wVl = NULL;
	m_Speed = NULL;
	m_Index = NULL;

	m_RHS = m_RHSRef = m_SigmaRef = m_Sigma = m_Mu = m_Cp = NULL;
	m_3DQInf = NULL;

	m_pWingList[0] = m_pWingList[1] = m_pWingList[2] = m_pWingList[3] = NULL;

	m_pWPolar = NULL;
	m_pPlane  = NULL;
	m_ppSurface = NULL;

	m_pPanel         = NULL;
	m_pWakePanel     = NULL;
	m_pRefWakePanel  = NULL;
	m_pMemPanel      = NULL;
	m_pNode          = NULL;
	m_pMemNode       = NULL;
	m_pWakeNode      = NULL;
	m_pRefWakeNode   = NULL;

	m_Alpha   = 0.0;
	m_QInf    = 0.0;
	m_OpAlpha = 0.0;
	m_OpBeta  = 0.0;
	m_Ctrl    = 0.0;
	m_vMin = m_vMax = m_vDelta = 0.0;

	m_bSequence      = false;

	m_MatSize = m_nNodes = 0;
	m_CL  = m_CX  = m_CY  = 0.0;
	m_GCm = m_GRm = m_GYm = m_VCm = m_ICm = m_VYm = m_IYm = 0.0;
	m_CP.Set( 0.0, 0.0, 0.0);
	m_ViscousDrag = m_InducedDrag = 0.0;

	m_MatSize        = 0;
	m_nNodes         = 0;
	m_NWakeColumn    = 0;
	m_nMaxWakeIter   = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	ftmp = phiG = Omega = 0.0;
	r1v = r2v = 0.0;

	Theta0 = 0.0;
	u0     = 0.0;

	//Dimensional stability derivatives
	Xu = Xw = Zu = Zw = Zq = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np = Nr = 0.0;

	// Non dimensional stability derivatives
	CXu = CZu = Cmu = CXq = CZq = Cmq = CXa = CZa = Cma = 0.0;
	CYb = CYp = CYr = Clb = Clp = Clr = Cnb = Cnp = Cnr = 0.0;
	CXe = CYe = CZe = Cle = Cme = Cne = 0.0;

	memset(m_ALong, 0, 16*sizeof(double));
	memset(m_ALat,  0, 16*sizeof(double));
	memset(m_BLong, 0,  4*sizeof(double));
	memset(m_BLat,  0,  4*sizeof(double));
	memset(m_R,     0,  9*sizeof(double));
	memset(m_Is,    0,  9*sizeof(double));
	memset(m_Ib,    0,  9*sizeof(double));
	memset(m_rLong, 0,  8*sizeof(double));
	memset(m_rLat,  0,  8*sizeof(double));
	memset(m_vLong, 0, 32*sizeof(double));
	memset(m_vLat,  0, 32*sizeof(double));
}


/**
 * The public destructor.
 *
 */
PanelAnalysis::~PanelAnalysis()
{
	ReleaseArrays();
	delete [] m_Ai;
	delete [] m_Cl;
	delete [] m_ICd;
	delete [] m_F;
	delete [] m_Vd;
}


/**
 * Reserves the memory necessary to matrix arrays.
 *@return true if the memory could be allocated, false otherwise.
 */
bool PanelAnalysis::AllocateMatrix(int matSize, int &memsize)
{
	QString strange;

	if(matSize<=s_MaxMatSize) return true;  //current analysis requires smaller size than that currently allocated

	ReleaseArrays();

	Trace("PanelAnalysis::Allocating matrix arrays");

	int size2 = matSize * matSize;
	try
	{
		m_aij      = new double[size2];
		m_aijWake  = new double[size2];

		m_uRHS  = new double[matSize];
		m_vRHS  = new double[matSize];
		m_wRHS  = new double[matSize];
		m_pRHS  = new double[matSize];
		m_qRHS  = new double[matSize];
		m_rRHS  = new double[matSize];
		m_cRHS  = new double[matSize];
		m_uWake = new double[matSize];
		m_wWake = new double[matSize];

		m_uVl   = new CVector[matSize];
		m_wVl   = new CVector[matSize];
		m_Speed = new CVector[matSize];
		m_Index = new int[matSize];
	}
	catch(std::exception & e)
	{
		ReleaseArrays();
		s_MaxMatSize = 0;
		Trace(e.what());
		strange = "Memory allocation error: the request for additional memory has been denied.\nPlease reduce the model's size.";
		Trace(strange);
		return false;
	}

	s_MaxMatSize = matSize;

	memsize  = sizeof(double)  * 2 * size2; //bytes
	memsize += sizeof(double)  * 9 * matSize; //bytes
	memsize += sizeof(CVector) * 3 * matSize;
	memsize += sizeof(int)     * 1 * matSize;

	strange = QString("PanelAnalysis::Memory allocation for the matrix arrays is %1 MB").arg((double)memsize/1024./1024., 7, 'f', 2);
	Trace(strange);

	memset(m_aij,     0, size2 * sizeof(double));
	memset(m_aijWake, 0, size2 * sizeof(double));

	memset(m_uRHS,  0, matSize*sizeof(double));
	memset(m_vRHS,  0, matSize*sizeof(double));
	memset(m_wRHS,  0, matSize*sizeof(double));
	memset(m_pRHS,  0, matSize*sizeof(double));
	memset(m_qRHS,  0, matSize*sizeof(double));
	memset(m_rRHS,  0, matSize*sizeof(double));
	memset(m_cRHS,  0, matSize*sizeof(double));
	memset(m_uWake, 0, matSize*sizeof(double));
	memset(m_wWake, 0, matSize*sizeof(double));

	memset(m_uVl,   0, matSize*sizeof(CVector));
	memset(m_wVl,   0, matSize*sizeof(CVector));
	memset(m_Speed, 0, matSize*sizeof(CVector));

	memset(m_Index, 0, matSize*sizeof(int));

	int RHSSize = 0;

	if(!AllocateRHS(matSize, RHSSize))
	{
		strange = "Memory allocation error: the request for additional memory has been denied.\nPlease educe the model's size.";
		traceLog(strange);
		return false;
	}

	memsize += RHSSize;

	strange = QString("PanelAnalysis::Memory allocation for the analysis arrays is %1 MB").arg((double)memsize/1024./1024., 7, 'f', 2);
	Trace(strange);

	return true;
}


/**
 * Reserves the memory necessary to RHS arrays.
 *@return true if the memory could be allocated, false otherwise.
 */
bool PanelAnalysis::AllocateRHS(int matSize, int &memsize)
{
	Trace("PanelAnalysis::Allocating RHS arrays");
	int size = matSize * s_MaxRHSSize;

	if(size==0) return false;

	try
	{
		m_RHS      = new double[size];
		m_RHSRef   = new double[size];
		m_SigmaRef = new double[size];
		m_Sigma    = new double[size];
		m_Mu       = new double[size];
		m_Cp       = new double[size];

		m_3DQInf   = new double[s_MaxRHSSize];
	}
	catch(std::exception &e)
	{
		ReleaseArrays();
		Trace(e.what());
		return false;
	}

	memsize = sizeof(double) * 6 * size;

	memset(m_RHS,       0, size*sizeof(double));
	memset(m_RHSRef,    0, size*sizeof(double));
	memset(m_Sigma,     0, size*sizeof(double));
	memset(m_SigmaRef,  0, size*sizeof(double));
	memset(m_Mu,        0, size*sizeof(double));
	memset(m_Cp,        0, size*sizeof(double));

	memset(m_3DQInf, 0, s_MaxRHSSize*sizeof(double));

	QString strange = QString("PanelAnalysis::Memory allocation for the RHS arrays is %1 MB").arg((double)memsize/1024./1024., 7, 'f', 2);
	Trace(strange);


	return true;
}


/**
 * Releases the memory reserved for matrix and RHS arrays
 */
void PanelAnalysis::ReleaseArrays()
{
	Trace("PanelAnalysis::Releasing Matrix and RHS arrays");

	if(m_aij)     delete [] m_aij;
	if(m_aijWake) delete [] m_aijWake;
	m_aij = m_aijWake = NULL;

	if(m_RHS)      delete [] m_RHS;
	if(m_RHSRef)   delete [] m_RHSRef;
	if(m_SigmaRef) delete [] m_SigmaRef;
	if(m_Sigma)    delete [] m_Sigma;
	if(m_Mu)       delete [] m_Mu;
	if(m_Cp)       delete [] m_Cp;
	m_RHS = m_RHSRef = m_SigmaRef = m_Sigma = m_Mu = m_Cp = NULL;

	if(m_3DQInf) delete [] m_3DQInf;
	m_3DQInf = NULL;

	if(m_uVl) delete [] m_uVl;
	if(m_wVl) delete [] m_wVl;
	m_uVl = m_wVl = NULL;

	if(m_uRHS)  delete [] m_uRHS;
	if(m_vRHS)  delete [] m_vRHS;
	if(m_wRHS)  delete [] m_wRHS;
	if(m_rRHS)  delete [] m_rRHS;
	if(m_pRHS)  delete [] m_pRHS;
	if(m_qRHS)  delete [] m_qRHS;
	if(m_cRHS)  delete [] m_cRHS;
	if(m_uWake) delete [] m_uWake;
	if(m_wWake) delete [] m_wWake;
	m_uRHS = m_vRHS = m_wRHS = m_pRHS = m_qRHS = m_rRHS = m_cRHS = m_uWake = m_wWake = NULL;

	if(m_Index) delete [] m_Index;
	m_Index = NULL;

	if(m_Speed) delete [] m_Speed;
	m_Speed = NULL;

	s_MaxMatSize = 0;
}


void PanelAnalysis::setObjectPointers(void *pPlane, void *pSurfaceList)
{
	m_pPlane    = (Plane*)pPlane;
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pPlane->wing())
		m_pWingList[iw] = m_pPlane->getWing(iw);
	}

	m_ppSurface = (QList<Surface*>*)pSurfaceList;
}


void PanelAnalysis::setRange(double vMin, double vMax, double vDelta, bool bSequence)
{
	if(vMax<vMin) vDelta = -qAbs(vDelta);

	m_bSequence = bSequence;
	m_vMin= vMin;
	m_vMax = vMax;
	m_vDelta = vDelta;

	m_nRHS = (int)qAbs((m_vMax-m_vMin)*1.0001/m_vDelta) +1;

	if(!m_bSequence) m_nRHS = 1;
	else if(m_nRHS>=VLMMAXRHS)
	{
		QString strange = QString("The number of points to be calculated will be limited to %1").arg(VLMMAXRHS);
		traceLog(strange);
		m_nRHS = VLMMAXRHS-1;
		s_MaxRHSSize = (int)((double)m_nRHS * 1.2);
	}


	//ESTIMATED UNIT TIMES FOR OPERATIONS

	if(m_pWPolar->m_bTiltedGeom || m_pWPolar->polarType()==BETAPOLAR)
	{

		//ESTIMATED UNIT TIMES FOR OPERATIONS
		//BuildInfluenceMatrix :     10 x MatSize/400
		//CreateRHS :                10
		//CreateWakeContribution :    1
		//SolveUnitRHS :            400 x MatSize/400
		//ComputeFarField :          10 x MatSize/400x nrhs
		//ComputeOnBodyCp :           1 x nrhs
		//RelaxWake :                20 x nrhs x MaxWakeIter *
		//ComputeAeroCoefs :          5 x nrhs

		s_TotalTime      =      10.*(double)m_MatSize/400.
						 +  10.
						 + 400.*(double)m_MatSize/400.
						 +  10.*(double)m_MatSize/400
						 +   1.
						 +   5.;

		s_TotalTime *= (double)m_nRHS;

	}
	else if(m_pWPolar->polarType()<FIXEDAOAPOLAR)
	{
		//BuildInfluenceMatrix :     10 x MatSize/400
		//CreateRHS :                10
		//CreateWakeContribution :    1
		//SolveUnitRHS :            400 x MatSize/400
		//ComputeFarField :          10 x MatSize/400x nrhs
		//ComputeOnBodyCp :           1 x nrhs
		//RelaxWake :                20 x nrhs x MaxWakeIter *
		//ComputeAeroCoefs :          5 x nrhs
		int MaxWakeIter = 1;

		if(!m_pWPolar->bThinSurfaces()) s_TotalTime +=1.0; //for wake contribution

		if(m_pWPolar->bWakeRollUp()) s_TotalTime += 20*m_nRHS*MaxWakeIter;

		s_TotalTime      = 10.0*(double)m_MatSize/400.
						 + 10.
						 + 400.*(double)m_MatSize/400.
						 + 10*(double)m_MatSize/400*(double)m_nRHS
						 + 1*(double)m_nRHS
						 + 5*(double)m_nRHS ;
	}
	else if(m_pWPolar->polarType()==FIXEDAOAPOLAR)
	{
		//BuildInfluenceMatrix :     10 x m_MatSize/400
		//CreateRHS :                10
		//CreateWakeContribution :    1
		//SolveUnitRHS :            400 x MatSize/400
		//ComputeFarField :          10 x MatSize/400x 1
		//ComputeOnBodyCp :           1 x nrhs
		//RelaxWake :                20 x nrhs x MaxWakeIter *
		//ComputeAeroCoefs :          5 x nrhs

		s_TotalTime      = 10.0*(double)m_MatSize/400.
						 + 10.
						 + 400.*(double)m_MatSize/400.
						 + 10*(double)m_MatSize/400*1.
						 + 1*(double)m_nRHS
						 + 5*(double)m_nRHS ;
	}
	else if(m_pWPolar->polarType()==STABILITYPOLAR)
	{
		if(!m_bSequence) m_nRHS = 1;
		else if(m_nRHS==0) m_nRHS = 1;//compute at least nominal control positions, even if none is active nor defined

		s_TotalTime      = 10.0*(double)m_MatSize/400.       //BuildInfluenceMatrix
						 + 10.                               //CreateRHS
						 + 400.*(double)m_MatSize/400.        //SolveUnitRHS
						 + 10*(double)m_MatSize/400          //ComputeFarField
						 + 2+5*6                             //ComputeStabDer
						 + 1                                 //ComputeOnBodyCp
						 + 5;                                //ComputeAeroCoefs
		s_TotalTime *= (double)m_nRHS;
	}
}


void PanelAnalysis::setParams(int nMaxWakeIter)
{
	m_nMaxWakeIter = nMaxWakeIter;
}



void PanelAnalysis::setArrayPointers(void *pPanel, void *pMemPanel, void *pWakePanel, void *pRefWakePanel,
									 void *pNode,  void *pMemNode,  void *pWakeNode,  void *pRefWakeNode)
{
	m_pPanel        = (Panel*)pPanel;
	m_pMemPanel     = (Panel*)pMemPanel;
	m_pWakePanel    = (Panel*)pWakePanel;
	m_pRefWakePanel = (Panel*)pRefWakePanel;

	m_pNode        = (CVector*) pNode;
	m_pMemNode     = (CVector*) pMemNode;
	m_pWakeNode    = (CVector*) pWakeNode;
	m_pRefWakeNode = (CVector*) pRefWakeNode;
}


void PanelAnalysis::setArraySize(int MatSize, int WakeSize, int nNodes, int nWakeNodes, int NWakeColumn)
{
	m_MatSize     = MatSize;
	m_WakeSize    = WakeSize;
	m_nNodes      = nNodes;
	m_nWakeNodes  = nWakeNodes;
	m_NWakeColumn = NWakeColumn;
}


/**
* Initializes the analysis
*/
bool PanelAnalysis::initializeAnalysis()
{
	if(!m_pPlane) return false;
	s_bCancel = false;

	QString strange;

	strange = "Launching 3D Panel Analysis....\n";
	traceLog(strange);

	traceLog(m_pPlane->planeName()+"\n");

	if(m_pWPolar->polarType()==FIXEDSPEEDPOLAR)     strange = "Type 1 - Fixed speed polar";
	else if(m_pWPolar->polarType()==FIXEDLIFTPOLAR) strange = "Type 2 - Fixed lift polar";
	else if(m_pWPolar->polarType()==FIXEDAOAPOLAR)  strange = "Type 4 - Fixed angle of attack polar";
	else if(m_pWPolar->polarType()==FIXEDAOAPOLAR)  strange = "Type 5 - Sideslip variation polar";
	else if(m_pWPolar->polarType()==STABILITYPOLAR) strange = "Type 7 - Stability polar";
	traceLog(strange+"\n");

	m_NSpanStations = 0;
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw] != NULL) m_NSpanStations += m_pWingList[iw]->m_NStation;
	}

	m_PlaneOppList.clear();

	if(m_Ai)  delete [] m_Ai;
	if(m_Cl)  delete [] m_Cl;
	if(m_ICd) delete [] m_ICd;
	if(m_F)   delete [] m_F;
	if(m_Vd)  delete [] m_Vd;
	m_Ai   = new double[ MAXWINGS*VLMMAXRHS * m_NSpanStations];
	m_Cl   = new double[ MAXWINGS*VLMMAXRHS * m_NSpanStations];
	m_ICd  = new double[ MAXWINGS*VLMMAXRHS * m_NSpanStations];
	m_F    = new CVector[ MAXWINGS*VLMMAXRHS * m_NSpanStations];
	m_Vd   = new CVector[ MAXWINGS*VLMMAXRHS * m_NSpanStations];

	s_Progress = 0.0;

	m_bPointOut = false;
	s_bCancel   = false;
	s_bWarning  = false;
	m_bWakeRollUp    = false;

	QString str = "";

	str = QString("Counted %1 panel elements\n").arg(m_MatSize,4);
	traceLog(str+"\n");

	return true;
}




bool PanelAnalysis::loop()
{
	if(m_pWPolar->polarType()<FIXEDAOAPOLAR)
	{
		if(m_pWPolar->m_bTiltedGeom) return unitLoop();
		else                         return alphaLoop();
	}
	else if(m_pWPolar->polarType()==FIXEDAOAPOLAR)
	{
		return QInfLoop();
	}
	else if(m_pWPolar->polarType()==BETAPOLAR)
	{
		return unitLoop();
	}
	else if(m_pWPolar->polarType()==STABILITYPOLAR)
	{
		return controlLoop();
	}

	restorePanels();

	return false;
}



/**
* Launches a calculation over the input sequence of aoa.
* Used for type 1 & 2 analysis, without tilted geometry.
*
* The calculation is performed for two unit RHS, and all the Operating POints are calculated by linear combination.
* The two unit RHS are for a unit velocity along the x-axis, and for a unit velocity along the z-axis.
*@return true if all the aoa were computed successfully, false otherwise. Interpolation issues are not counted as unsuccessful.
*/
bool PanelAnalysis::alphaLoop()
{
	QString str;
//    nrhs  = (int)qAbs((m_AlphaMax-m_Alpha)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) m_nRHS = 1;


	s_Progress = 0.0;
	qApp->processEvents();

	str = QString("   Solving the problem... \n");
	traceLog(str);

	BuildInfluenceMatrix();
	if (s_bCancel) return true;


	CreateUnitRHS();
	if (s_bCancel) return true;


	if(!m_pWPolar->bThinSurfaces())
	{
		//compute wake contribution
		CreateWakeContribution();


		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}
	if (s_bCancel) return true;


	if (!SolveUnitRHS())
	{
		s_bWarning = true;
		return true;
	}
	if (s_bCancel) return true;


	CreateSourceStrength(m_vMin, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	CreateDoubletStrength(m_vMin, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	ComputeFarField(1.0, m_vMin, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	for(int q=0; q<m_nRHS; q++)
		ComputeBalanceSpeeds(m_vMin+q*m_vDelta, q);

	ScaleResultstoSpeed(m_nRHS);
	if (s_bCancel) return true;

	ComputeOnBodyCp(m_vMin, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	ComputeAeroCoefs(m_vMin, m_vDelta, m_nRHS);

	return true;
}



/**
* Builds the influence matrix, both for VLM or Panel calculations.
*/
void PanelAnalysis::BuildInfluenceMatrix()
{
	CVector C, CC, V;
	int m, mm, p, pp;
	double phi;

	int Size = m_MatSize;
//	if(m_b3DSymetric) Size = m_SymSize;

	traceLog("      Creating the influence matrix...");
	traceLog("\n");
	m=0;
	for(p=0; p<m_MatSize; p++)
	{
		if(s_bCancel) return;
//		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
//		{
			//for each Boundary Condition point
			if(m_pPanel[p].m_Pos!=MIDSURFACE)
			{
				//Thick surfaces, 3D-panel type BC, use collocation point
				C = m_pPanel[p].CollPt;
			}
			else
			{
				//Thin surface, VLM type BC, use control point
				C = m_pPanel[p].CtrlPt;
			}
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			mm = 0;
			for(pp=0; pp<m_MatSize; pp++)
			{
				if(s_bCancel) return;
//				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel)
//				{
					//for each panel, get the unit doublet or vortex influence at the boundary condition pt
					GetDoubletInfluence(C, m_pPanel+pp, V, phi);

/*					if(m_b3DSymetric && m_pPanel[pp].m_iSym>=0) // add symmetric contribution
					{
						GetDoubletInfluence(CC, m_pPanel+pp, VS, phiSym);
						V.x += VS.x;
						V.y -= VS.y;
						V.z += VS.z;
						phi += phiSym;
					}*/

					if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE) m_aij[m*Size+mm] = V.dot(m_pPanel[p].Normal);
					else if(m_pWPolar->bDirichlet())                              m_aij[m*Size+mm] = phi;

					mm++;
//				}
			}
			m++;
//		}
		s_Progress += 10.0*(double)Size/400./(double)Size;
		qApp->processEvents();
	}
}


/**
 * Creates the source strengths for all requested RHS in a Panel analysis, using the specified boundary conditions (BC).
 * BC may be of the Neumann or Dirichlet type depending on the analysis type and on the geometry
 *
 * Uses NASA 4023 equation (20) & (22)
 *
 * The computation is performed for a unit speed. The results are scaled to speed later depending on the polar type.
 *
 *@param Alpha0 the first aoa in the sequence
 *@param AlphaDeltathe aoa increment
 *@param the total number of aoa and of source arrays to build
*/
void PanelAnalysis::CreateSourceStrength(double Alpha0, double AlphaDelta, int nval)
{
	int p, pp, q;
	double alpha;
	CVector WindDirection;

	traceLog("      Creating source strengths...\n");

	p=0;
	for (q=0; q<nval;q++)
	{
		alpha = Alpha0+q*AlphaDelta;
		WindDirection.Set(cos(alpha*PI/180.0), 0.0, sin(alpha*PI/180.0));

		for (pp=0; pp<m_MatSize; pp++)
		{
			if(s_bCancel) return;
			if(m_pPanel[pp].m_Pos!=MIDSURFACE) m_Sigma[p] = -1.0/4.0/PI* WindDirection.dot(m_pPanel[pp].Normal);
			else                               m_Sigma[p] =  0.0;
			p++;
		}
	}
}


/**
* Creates a RHS vector array for a given field of velocity vectors.
* If the input pointer VField is NULL, a uniform field of freestream velocities is considered.
* If the pointer is not NULL, it points to an array of velocity vectors on the panels. Used for evaluating the stability derivatives for the rotation dof.
* @param RHS A pointer to the RHS to build
* @param VField a pointer to the array of the freesteam velocity vectors on the panels.
* @param VInf the freestream velocity vector
*/
void PanelAnalysis::CreateRHS(double *RHS, CVector VInf, double *VField)
{
	int m, p, pp;
	double  phi, sigmapp;
	CVector V, C, VPanel;

	m = 0;

	for (p=0; p<m_MatSize; p++)
	{
		if(s_bCancel) return;
		if(VField)
		{
			VPanel.x = *(VField             +p);
			VPanel.y = *(VField+  m_MatSize +p);
			VPanel.z = *(VField+2*m_MatSize +p);
		}
		else VPanel = VInf;


//		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
//		{
			if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
			{
				// first term of RHS is -V.n
				RHS[m] = - m_pPanel[p].Normal.dot(VPanel);
//				if(RHS[m]<PRECISION) RHS[m] = 0.0;
			}
			else if(m_pWPolar->bDirichlet()) RHS[m] = 0.0;
			if(m_pPanel[p].m_Pos!=MIDSURFACE) C = m_pPanel[p].CollPt;
			else                              C = m_pPanel[p].CtrlPt;

			for (pp=0; pp<m_MatSize; pp++)
			{
				// Consider only the panels positioned on thick surfaces,
				// since the source strength is zero on thin surfaces
				if(m_pPanel[pp].m_Pos!=MIDSURFACE)
				{
					// Define the source strength on panel pp
					sigmapp = -1.0/4.0/PI * m_pPanel[pp].Normal.dot(VPanel);

					// Add to RHS the source influence of panel pp on panel p
					GetSourceInfluence(C, m_pPanel+pp, V, phi);

					if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
					{
						// Apply Neumann B.C.
						// NASA4023 eq. (22) and (23)
						// The RHS term is sigma[pp]*DJK = nj.Vjk
						RHS[m] -= V.dot(m_pPanel[p].Normal) * sigmapp;
					}
					else if(m_pWPolar->bDirichlet())
					{
						//NASA4023 eq. (20)
						RHS[m] -= (phi * sigmapp);
					}
				}
			}
			m++;
//		}
		s_Progress += 5.0/(double)m_MatSize;

		qApp->processEvents();
	}
}


/**
*Builds the two unit RHS for freestream velocities directed along x and z
*/
void PanelAnalysis::CreateUnitRHS()
{
	traceLog("      Creating the unit RHS vectors...\n");

	CVector VInf;

	VInf.Set(1.0, 0.0, 0.0);
	CreateRHS(m_uRHS,  VInf);
	VInf.Set(0.0, 0.0, 1.0);
	CreateRHS(m_wRHS, VInf);
}


/**
* In the case of a panel analysis, adds the contribution of the wake columns to the coefficients of the influence matrix
* Method :
* 	- follow the method described in NASA 4023 eq. (44)
*	- add the wake's doublet contribution to the matrix
*	- add the difference in potential at the trailing edge panels to the RHS
* Only a flat wake is considered. Wake roll-up has been tested but did not prove robust enough for implementation.
*/
void PanelAnalysis::CreateWakeContribution()
{
	int kw, lw, pw, p, pp, Size;

	CVector V, C, CC, TrPt;
	double phi;
	double* PHC = new double[m_NWakeColumn];
	CVector *VHC = new CVector[m_NWakeColumn];

	traceLog("      Adding the wake's contribution...\n");

	Size = m_MatSize;
//	if(m_b3DSymetric) Size = m_SymSize;

	int m, mm;
	m = mm = 0;


	for(p=0; p<m_MatSize; p++)
	{
		if(s_bCancel) return;
//		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
		{
			m_uWake[m] = m_wWake[m] = 0.0;
			C    = m_pPanel[p].CollPt;
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			//____________________________________________________________________________
			//build the contributions of each wake column at point C
			//we have m_NWakeColum to consider
			pw=0;
			for (kw=0; kw<m_NWakeColumn; kw++)
			{
				PHC[kw] = 0.0;
				VHC[kw].Set(0.0,0.0,0.0);
				//each wake column has m_NXWakePanels
				for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
				{
					GetDoubletInfluence(C, m_pWakePanel+pw, V, phi, true, true);

					PHC[kw] += phi;
					VHC[kw] += V;

/*					if(m_b3DSymetric && m_pPanel[p].m_bIsLeftPanel)
					{
						GetDoubletInfluence(CC,  m_pWakePanel+pw, VS, phiSym, true,true);
						PHC[kw]    +=  phiSym;
						VHC[kw].x  +=  VS.x;
						VHC[kw].y  -=  VS.y;
						VHC[kw].z  +=  VS.z;
					}*/
					pw++;
				}
			}
			//____________________________________________________________________________
			//Add the contributions of the trailing panels to the matrix coefficients and to the RHS
			mm = 0;
			for(pp=0; pp<m_MatSize; pp++) //for each matrix column
			{
//				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel)
//				{
					if(s_bCancel) return;
					m_aijWake[m*Size+mm] = 0.0;
					// Is the panel pp shedding a wake ?
					if(m_pPanel[pp].m_bIsTrailing)
					{
						// If so, we need to add the contributions of the wake column
						// shedded by this panel to the RHS and to the Matrix
						// Get trailing point where the jup in potential is evaluated v6.02
						TrPt = (m_pNode[m_pPanel[pp].m_iTA] + m_pNode[m_pPanel[pp].m_iTB])/2.0;

						if(m_pPanel[pp].m_Pos==MIDSURFACE)
						{
							//The panel shedding a wake is on a thin surface
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//then add the velocity contribution of the wake column to the matrix coefficient
								m_aijWake[m*Size+mm] += VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
							else if(m_pWPolar->bDirichlet())
							{
								//then add the potential contribution of the wake column to the matrix coefficient
								m_aijWake[m*Size+mm] += PHC[m_pPanel[pp].m_iWakeColumn];
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
						}
						else if(m_pPanel[pp].m_Pos==BOTSURFACE)
						{
							//the panel sedding a wake is on the bottom side, substract
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//use Neumann B.C.
								m_aijWake[m*Size+mm] -= VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//corrected in v6.02;
								m_uWake[m] -= TrPt.x  * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								m_wWake[m] -= TrPt.z  * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->bDirichlet())
							{
								m_aijWake[m*Size+mm] -= PHC[m_pPanel[pp].m_iWakeColumn];
								m_uWake[m] +=  TrPt.x * PHC[m_pPanel[pp].m_iWakeColumn];
								m_wWake[m] +=  TrPt.z * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
						else if(m_pPanel[pp].m_Pos==TOPSURFACE)
						{
							//the panel sedding a wake is on the top side, add
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//use Neumann B.C.
								m_aijWake[m*Size+mm] += VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//corrected in v6.02;
								m_uWake[m] += TrPt.x * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								m_wWake[m] += TrPt.z * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->bDirichlet())
							{
								m_aijWake[m*Size+mm] += PHC[m_pPanel[pp].m_iWakeColumn];
								m_uWake[m] -= TrPt.x * PHC[m_pPanel[pp].m_iWakeColumn];
								m_wWake[m] -= TrPt.z * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
					}
					mm++;
//				}
			}
			m++;
		}
		s_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}

	delete [] PHC;
	delete [] VHC;
}


/**
* In the case of a panel analysis, adds the contribution of the wake columns to the coefficients of the influence matrix
* Method :
* 	- follow the method described in NASA 4023 eq. (44)
*	- add the wake's doublet contribution to the matrix
*	- add the potential difference at the trailing edge panels to the RHS ; the potential's origin
*     is set arbitrarily to the geometrical orgin so that phi = V.dot(WindDirectio) x point_position
* Only a flat wake is considered. Wake roll-up has been tested but did not prove robust enough for implementation.
*/
void PanelAnalysis::CreateWakeContribution(double *pWakeContrib, CVector WindDirection)
{
	int kw, lw, pw, p, pp;

	static CVector V, C, CC, TrPt;
	double phi;
	double* PHC = new double[m_NWakeColumn];
	CVector* VHC = new CVector[m_NWakeColumn];

	traceLog("      Adding the wake's contribution...\n");

//	if(m_b3DSymetric) Size = m_SymSize;
//	else              Size = m_MatSize;

	int m, mm;
	m = mm = 0;

	for(p=0; p<m_MatSize; p++)
	{
		if(s_bCancel) return;
//		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
//		{
			pWakeContrib[m] = 0.0;
			C    = m_pPanel[p].CollPt;
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			//____________________________________________________________________________
			//build the contributions of each wake column at point C
			//we have m_NWakeColum to consider
			pw=0;
			for (kw=0; kw<m_NWakeColumn; kw++)
			{
				PHC[kw] = 0.0;
				VHC[kw].Set(0.0,0.0,0.0);
				//each wake column has m_NXWakePanels
				for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
				{
					GetDoubletInfluence(C, m_pWakePanel+pw, V, phi, true, true);

					PHC[kw] += phi;
					VHC[kw] += V;

/*					if(m_b3DSymetric && m_pPanel[p].m_bIsLeftPanel)
					{
						GetDoubletInfluence(CC, m_pWakePanel+pw, VS, phiSym, true, true);
						PHC[kw]    +=  phiSym;
						VHC[kw].x  +=  VS.x;
						VHC[kw].y  -=  VS.y;
						VHC[kw].z  +=  VS.z;
					}*/
					pw++;
				}
			}
			//____________________________________________________________________________
			//Add the contributions to the matrix coefficients and to the RHS
			mm = 0;
			for(pp=0; pp<m_MatSize; pp++) //for each matrix column
			{
//				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel)
//				{
					if(s_bCancel) return;

					// Is the panel pp shedding a wake ?
					if(m_pPanel[pp].m_bIsTrailing)
					{
						// Get trailing point where the jump in potential is evaluated
						TrPt = (m_pNode[m_pPanel[pp].m_iTA] + m_pNode[m_pPanel[pp].m_iTB])/2.0;
						// If so, we need to add the contributions of the wake column
						// shedded by this panel to the RHS and to the Matrix
						if(m_pPanel[pp].m_Pos==MIDSURFACE)
						{
							//The panel shedding a wake is on a thin surface
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//then add the velocity contribution of the wake column to the matrix coefficient
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
							else if(m_pWPolar->bDirichlet())
							{
								//then add the potential contribution of the wake column to the matrix coefficient
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
						}
						else if(m_pPanel[pp].m_Pos==BOTSURFACE)//bottom side, substract
						{
							//evaluate the potential on the bottom side panel pp which is shedding a wake
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//use Neumann B.C.
								pWakeContrib[m] -= TrPt.dot(WindDirection) * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->bDirichlet())
							{
								pWakeContrib[m] += TrPt.dot(WindDirection) * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
						else if(m_pPanel[pp].m_Pos==TOPSURFACE)  //top side, add
						{
							//evaluate the potential on the top side panel pp which is shedding a wake
							if(!m_pWPolar->bDirichlet() || m_pPanel[p].m_Pos==MIDSURFACE)
							{
								//use Neumann B.C.
								pWakeContrib[m] += TrPt.dot(WindDirection) * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->bDirichlet())
							{
								pWakeContrib[m] -= TrPt.dot(WindDirection) * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
					}
					mm++;
//				}
			}
			m++;
//		}
		s_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}

	delete [] PHC;
	delete [] VHC;
}


/**
* This method performs the computation in the far-field (Trefftz) plane.
* For each of the wings, calculates, the resulting Force vector and induced drag, and the coefficients for each chordwise strip.
* The calculations are made based on the source and doublet strengths or on the vortex circulations which have been previously computed.
* The results are used a first time to calculate the balance velocities, and a second time for the calculation of aero coefficients
* @param QInf the freestream velocity
* @param Alpha0 the first aoa in the sequence
* @param AlphaDelta the aoa increment
* @param nval the number of aoa in the sequence
*/
void PanelAnalysis::ComputeFarField(double QInf, double Alpha0, double AlphaDelta, int nval)
{
	QString strong;
	int q, pos;
	double alpha, IDrag, *Mu, *Sigma;
	double ThinSize = 0.0;
	CVector WindNormal, WingForce;

	traceLog("      Calculating aerodynamic coefficients in the far field plane\n");

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw]) ThinSize += (double)m_pWingList[iw]->m_MatSize;
	}

	for (q=0; q<nval;q++)
	{
		if(m_pWPolar->polarType()==FIXEDAOAPOLAR)       alpha = m_OpAlpha;
		else if(m_pWPolar->polarType()==BETAPOLAR)      alpha = m_OpAlpha;
		else if(m_pWPolar->polarType()==STABILITYPOLAR) alpha= m_OpAlpha;
		else                                            alpha = Alpha0 + q*AlphaDelta;

		WindNormal.Set(-sin(alpha*PI/180.0),   0.0, cos(alpha*PI/180.0));

		pos = 0;
		Mu     = m_Mu    + q*m_MatSize;
		Sigma  = m_Sigma + q*m_MatSize;

		strong = "        Calculating point " + QString("%1").arg(alpha,7,'f',2)+QString::fromUtf8("°....\n");
		traceLog(strong);

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				WingForce.Set(0.0, 0.0,		0.0);
				m_pWingList[iw]->PanelTrefftz(this, QInf, alpha, Mu, Sigma, pos, WingForce, IDrag, m_pWPolar, m_pWakePanel, m_pWakeNode);

				//save the results... will save another FF calculation when computing operating point
				m_WingForce[q*MAXWINGS+iw] = WingForce;
				m_WingIDrag[q*MAXWINGS+iw] = IDrag;

				memcpy(m_Cl  + (q*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_Cl,  m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_ICd + (q*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_ICd, m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_Ai  + (q*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_Ai,  m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_F   + (q*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_F,   m_pWingList[iw]->m_NStation*sizeof(CVector));
				memcpy(m_Vd  + (q*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_Vd,  m_pWingList[iw]->m_NStation*sizeof(CVector));

				pos += m_pWingList[iw]->m_MatSize;

				s_Progress += 10.0 * (double)m_pWingList[iw]->m_MatSize/ThinSize *(double)m_MatSize/400.;
				qApp->processEvents();
				if(s_bCancel)return;
			}
		}
	}
}


/**
* In the case of Type 2 or Type 7 polars, computes the velocity for which the lift force balances the plane's weight.
* Assumes the lift force on each lifting wing has been calculated for a unit velocity.
* @param Alpha the angle of attack to calculate
* @param q the index of the aoa in the sequence
*/
void PanelAnalysis::ComputeBalanceSpeeds(double Alpha, int q)
{
	QString strong;
	CVector Force, WindNormal;
	double TempCl,Lift;
	WindNormal.Set(-sin(Alpha*PI/180.0),   0.0, cos(Alpha*PI/180.0));

	Force.Set(0.0,0.0,0.0);

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw]) Force += m_WingForce[q*4+iw];
	}

	if (m_pWPolar->polarType()==FIXEDSPEEDPOLAR || m_pWPolar->polarType()==BETAPOLAR)
	{
		m_3DQInf[q] = m_pWPolar->m_QInfSpec;
	}
	else if(m_pWPolar->polarType()==FIXEDLIFTPOLAR)
	{
		Lift =  Force.dot(WindNormal) ;      //N/q, for 1/ms
		TempCl = Lift/m_pWPolar->m_WArea;
		if(Lift<=0.0)
		{
			strong = "           "+QString("Found a negative lift for Alpha=%1.... skipping the angle...\n").arg(Alpha, 5,'f',2);
			traceLog(strong);
			m_bPointOut = true;
			s_bWarning  = true;
			m_3DQInf[q] = -100.0;
		}
		else
		{
			m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Mass/m_pWPolar->m_Density/TempCl/m_pWPolar->m_WArea);
			strong = QString("           Alpha=%1   QInf=%2m/s").arg(Alpha, 5,'f',2).arg(m_3DQInf[q],5,'f',2);
			strong+="\n";
			traceLog(strong);
		}
	}
}


/**
* Scales the unit results using the specified freestream velocity.
* The velocity may have been specified directly in the case of a type 1 polar, or may have been calculated to balance the plane's weight.
* The velocity for each aoa are provided in the member variable array m_3DQInf
* @param nval the number of aoa in the sequence
*/
void PanelAnalysis::ScaleResultstoSpeed(int nval)
{
	int p, q, pp;
	//______________________________________________________________________________________
	// Scale RHS and Sigma i.a.w. speeds (so far we have unit doublet and source strengths)

	QString strong="\n";
	traceLog(strong);

	memcpy(m_SigmaRef, m_Sigma, nval*m_MatSize*sizeof(double));
	memcpy(m_RHSRef,   m_Mu,    nval*m_MatSize*sizeof(double));

	if(m_pWPolar->polarType()!=FIXEDAOAPOLAR)
	{
		p=0;
		for (q=0; q<nval;q++)
		{
			for(pp=0; pp<m_MatSize; pp++)
			{
				m_Mu[p]     *= m_3DQInf[q];
				m_Sigma[p]  *= m_3DQInf[q];
				p++;
			}
		}
	}
	else
	{
		//type 4, we scale the first single rhs for all specified speed values
		p=0;
		for (q=0; q<nval;q++)
		{
			for(pp=0; pp<m_MatSize; pp++)
			{
				m_Mu[p]    = m_RHSRef[pp]   * m_3DQInf[q];
				m_Sigma[p] = m_SigmaRef[pp] * m_3DQInf[q];
				p++;
			}
		}
	}

	//scale the strip force and downwash fields
	if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR)
	{
		for (q=0; q<nval;q++)
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_pWingList[iw])
				{
					for(int m=0; m<m_pWingList[iw]->m_NStation; m++)
					{
						m_F[ m+ (q*MAXWINGS+iw)*m_NSpanStations] *= m_3DQInf[q] * m_3DQInf[q];
						m_Vd[m+ (q*MAXWINGS+iw)*m_NSpanStations] *= m_3DQInf[q] ;
					}
				}
			}
		}
	}
	else
	{
		for (q=0; q<nval;q++)
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_pWingList[iw])
				{
					for(int m=0; m<m_pWingList[iw]->m_NStation; m++)
					{
						m_F[ m+ (q*MAXWINGS+iw)*m_NSpanStations] = m_F[ m] * m_3DQInf[q] * m_3DQInf[q];
						m_Vd[m+ (q*MAXWINGS+iw)*m_NSpanStations] = m_Vd[m] * m_3DQInf[q];
					}
					m_WingForce[q*MAXWINGS+iw] = m_WingForce[iw];
					m_WingIDrag[q*MAXWINGS+iw] = m_WingIDrag[iw];
				}
			}
		}
	}

}


/**
* Computes the  viscous and inviscid aerodynamic coefficients.
* @param V0 the initial aoa or velocity, depending on the polar type
* @param VDelta the aoa or velocity increment, depending on the polar type
* @param nrhs the number of points in the sequence
*/
void PanelAnalysis::ComputeAeroCoefs(double V0, double VDelta, int nrhs)
{
	int q;
	QString str;

	if(m_pWPolar->m_WPolarType==FIXEDAOAPOLAR)
	{
		for (q=0; q<nrhs; q++)
		{
			if(s_bCancel) return;
			str = QString("      Computing Plane for QInf=%1m/s").arg((V0+q*VDelta),7,'f',2);
			traceLog(str);
			computePlane(m_Alpha, V0+q*VDelta, m_OpBeta, q);
			s_Progress += 5.0*(double)nrhs /(double)nrhs;
			qApp->processEvents();
		}
	}
	else if(m_pWPolar->polarType()==BETAPOLAR)
	{
		for (q=0; q<nrhs; q++)
		{
			if(s_bCancel) return;
			str = QString("      Computing Plane for beta=%1").arg((m_OpBeta),0,'f',1);
			str += QString::fromUtf8("°\n");
			traceLog(str);
			computePlane(m_Alpha, m_3DQInf[q], V0+q*VDelta, q);
			s_Progress += 5.0*(double)nrhs /(double)nrhs;
			qApp->processEvents();
		}
	}
	else
	{
		for (q=0; q<nrhs; q++)
		{
			if(s_bCancel) return;
			if(m_3DQInf[q]>0.0)
			{
				if(!m_pWPolar->m_bTiltedGeom) str = QString("      Computing Plane for alpha=%1").arg(V0+q*VDelta,7,'f',2);
				else                          str = QString("      Computing Plane for alpha=%1").arg(m_OpAlpha,7,'f',2);
				str += QString::fromUtf8("°\n");
				traceLog(str);
				computePlane(V0+q*VDelta, m_3DQInf[q], m_OpBeta, q);
			}
			s_Progress += 5.0*(double)nrhs/(double)nrhs;
			qApp->processEvents();
		}
	}
}



/**
 * This method calculates the all the viscous and inviscid aerodynamic coefficients for one aoa.
 * The method calls the method in the QMiarex class which creates the operating points, updates the polar data, and updates the graphs.
 * The method uses in input the result data stored in the member variables.
 * @param Alpha the aoa of this calculation
 * @param QInf the freesteam velocity of this calculation
 * @param qrhs the index of the current right hand side calculation
*/
void PanelAnalysis::computePlane(double Alpha, double QInf, double Beta, int qrhs)
{
	int pos;
	double *Mu, *Sigma;
	double cosa, sina;
	double Lift, IDrag, VDrag ,XCP, YCP, ZCP, WingVDrag;
	CVector WindNormal, WindDirection, WindSide;
	CVector Force;
	QString OutString;
	//   Define wind (stability) axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);

	WindNormal.Set(  -sina, 0.0, cosa);
	WindDirection.Set(cosa, 0.0, sina);
	WindSide = WindNormal * WindDirection;

	Mu     = m_Mu    + qrhs*m_MatSize;
	Sigma  = m_Sigma + qrhs*m_MatSize;

	m_QInf      = QInf;

	if(m_pWPolar->m_bTiltedGeom || m_pWPolar->isBetaPolar()) Alpha     = m_OpAlpha;
	else                                                     m_OpAlpha = Alpha;

	for(int iw=0; iw<MAXWINGS; iw++)
		if(m_pWingList[iw])m_pWingList[iw]->m_bWingOut = false;
//	m_pWing->m_bWingOut                = false;

	if(QInf >0.0)
	{
		traceLog("       Calculating aerodynamic coefficients...\n");
		m_bPointOut = false;
		Force.Set(0.0, 0.0, 0.0);

		Lift = IDrag = VDrag = XCP = YCP = ZCP = 0.0;
		m_GCm = m_VCm = m_ICm = m_GRm = m_GYm = m_VYm = m_IYm = 0.0;

		pos = 0;

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				traceLog("         Calculating wing..." + m_pWingList[iw]->m_WingName+"\n");
				//restore the saved FF results
				if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR)
				{
					memcpy(m_pWingList[iw]->m_Cl,  m_Cl  + (qrhs*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_ICd, m_ICd + (qrhs*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_Ai,  m_Ai  + (qrhs*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_F,   m_F   + (qrhs*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(CVector));
					memcpy(m_pWingList[iw]->m_Vd,  m_Vd  + (qrhs*MAXWINGS+iw)*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(CVector));
				}
				else
				{
					memcpy(m_pWingList[iw]->m_Cl,  m_Cl +iw*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_ICd, m_ICd+iw*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_Ai,  m_Ai +iw*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(double));
					memcpy(m_pWingList[iw]->m_F,   m_F  +iw*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(CVector));
					memcpy(m_pWingList[iw]->m_Vd,  m_Vd +iw*m_NSpanStations, m_pWingList[iw]->m_NStation*sizeof(CVector));
				}

				Force += m_WingForce[qrhs*MAXWINGS+iw];
				IDrag += m_WingIDrag[qrhs*MAXWINGS+iw];

				//Get viscous interpolations
				m_pWingList[iw]->PanelComputeViscous(QInf, m_pWPolar, WingVDrag, m_pWPolar->bViscous(), OutString);
				VDrag += WingVDrag;

				traceLog(OutString);
				if(m_pWingList[iw]->m_bWingOut)  m_bPointOut = true;


				//Compute moment coefficients
				m_pWingList[iw]->PanelComputeOnBody(QInf, Alpha, m_Cp+qrhs*m_MatSize+pos, m_Mu+qrhs*m_MatSize+pos,
													XCP, YCP, ZCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
													m_pWPolar, m_pWPolar->m_CoG);


				m_pWingList[iw]->PanelComputeBending(m_pWPolar->bThinSurfaces());

				pos += m_pWingList[iw]->m_MatSize;
			}
		}

		if(m_pPlane->body() && m_pWPolar->m_AnalysisMethod==PANELMETHOD && !m_pWPolar->m_bIgnoreBodyPanels)
		{
			double ICm = 0.0;
			traceLog("       Calculating body...\n");
			m_pPlane->body()->ComputeAero(m_Cp+qrhs*m_MatSize+pos, XCP, YCP, ZCP, ICm, m_GRm, m_GYm, Alpha, m_pWPolar->m_CoG);
			m_ICm += ICm;
			m_GCm += ICm;

			//the body does not shed any wake --> no induced lift or drag
		}

		if(!s_bTrefftz) SumPanelForces(m_Cp+qrhs*m_MatSize, Alpha, Lift, IDrag);

		m_CL          =       Force.dot(WindNormal)    /m_pWPolar->m_WArea;
		m_CX          =       Force.dot(WindDirection) /m_pWPolar->m_WArea;
		m_CY          =       Force.dot(WindSide)      /m_pWPolar->m_WArea;

		m_InducedDrag =  1.0*IDrag/m_pWPolar->m_WArea;
		m_ViscousDrag =  1.0*VDrag/m_pWPolar->m_WArea;

		if(qAbs(Force.dot(WindNormal))>0.0)
		{
			m_CP.x         = XCP/Force.dot(WindNormal);
			m_CP.y         = YCP/Force.dot(WindNormal);
			m_CP.z         = ZCP/Force.dot(WindNormal);
		}
		else
		{
			m_CP.Set(0.0,0.0,0.0);
		}

		m_GCm *= 1.0 / m_pWPolar->m_WArea /m_pPlane->mac();
		m_VCm *= 1.0 / m_pWPolar->m_WArea /m_pPlane->mac();
		m_ICm *= 1.0 / m_pWPolar->m_WArea /m_pPlane->mac();

		m_GRm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		m_GYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_VYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_IYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		if(m_bPointOut) s_bWarning = true;

		if(m_pWPolar->m_WPolarType==STABILITYPOLAR) m_Alpha = m_AlphaEq; // so it is set by default at the end of the analyis

		PlaneOpp *pPOpp = createPlaneOpp(m_Cp+qrhs*m_MatSize, Mu, Sigma);
		m_PlaneOppList.append(pPOpp);

		traceLog("\n");
	}
	else m_bPointOut = true;

	qApp->processEvents();
}


/**
* Returns the estimation of the panel's lift coeficient based on the vortex circulation.
* @param p the index of the panel
* @param Gamma the pointer to the array of vortex circulations
* @param Cp a pointer to the array of resulting panel lift coefficients
* @param VInf the freestream velocity vector
*/
void PanelAnalysis::GetVortexCp(const int &p, double *Gamma, double *Cp, CVector &VInf)
{
	static CVector PanelForce, Force;
	// for each panel along the chord, add the lift coef
	PanelForce  = VInf * m_pPanel[p].Vortex;
	PanelForce *= Gamma[p] * m_pWPolar->m_Density;                 //Newtons

	if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
	{
		Force       = VInf* m_pPanel[p].Vortex;
		Force      *= Gamma[p+1] * m_pWPolar->m_Density;           //Newtons
		PanelForce -= Force;
	}

	Cp[p]  = -2.0 * PanelForce.dot(m_pPanel[p].Normal) /m_pPanel[p].Area/m_pWPolar->m_Density;
}

/**
* This method calculates the Cp coefficient on a panel based on the distribution of doublet strengths.
*The Cp coefficient is the derivative of the doublet strength on the surface
*This calculation follows the method provided in document NASA 4023.
* @param p the index of the panel for which the calculation is performed
* @param Mu the array of doublet strength values
* @param Cp a reference to the Cp variable to evaluate
* @param VLocal a reference to the CVector holding the local velocity on the panel
* @param QInf the freestream velocity
* @param Vx the x-component of the freestream velocity vector
* @param Vz the z-component of the freestream velocity vector
*/
void PanelAnalysis::GetDoubletDerivative(const int &p, double *Mu, double &Cp, CVector &VLocal, double const &QInf, double Vx, double Vy, double Vz)
{
	static int PL,PR, PU, PD;
	static double DELQ, DELP, mu0,mu1,mu2, x0,x1,x2, Speed2;
	static CVector VTot;//total local panel speed
	static CVector S2, Sl2;

	PL = m_pPanel[p].m_iPL;
	PR = m_pPanel[p].m_iPR;
	PU = m_pPanel[p].m_iPU;
	PD = m_pPanel[p].m_iPD;

	if(PL>=0 && PR>=0)
	{
		//we have two side neighbours
		x1  = 0.0;
		x0  = x1 - m_pPanel[p].SMQ - m_pPanel[PL].SMQ;
		x2  = x1 + m_pPanel[p].SMQ + m_pPanel[PR].SMQ;
		mu0 = Mu[PL];
		mu1 = Mu[p];
		mu2 = Mu[PR];
		DELQ =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PL>=0 && PR<0)
	{
		// no right neighbour
		// do we have two left neighbours ?
		if(m_pPanel[PL].m_iPL>=0){
			x2  = 0.0;
			x1  = x2 - m_pPanel[p].SMQ  - m_pPanel[PL].SMQ;
			x0  = x1 - m_pPanel[PL].SMQ - m_pPanel[m_pPanel[PL].m_iPL].SMQ;

			mu0 = Mu[m_pPanel[PL].m_iPL];
			mu1 = Mu[PL];
			mu2 = Mu[p];
			DELQ =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = -(Mu[PL]-Mu[p])/(m_pPanel[p].SMQ  + m_pPanel[PL].SMQ);
		}
	}
	else if(PL<0 && PR>=0)
	{
		// no left neighbour
		// do we have two right neighbours ?
		if(m_pPanel[PR].m_iPR>=0){
			x0  = 0.0;
			x1  = x0 + m_pPanel[p].SMQ  + m_pPanel[PR].SMQ;
			x2  = x1 + m_pPanel[PR].SMQ + m_pPanel[m_pPanel[PR].m_iPR].SMQ;
			mu0 = Mu[p];
			mu1 = Mu[PR];
			mu2 = Mu[m_pPanel[PR].m_iPR];
			DELQ =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = (Mu[PR]-Mu[p])/(m_pPanel[p].SMQ  + m_pPanel[PR].SMQ);
		}
	}
	else
	{
		DELQ = 0.0;
		//Cannot calculate a derivative on one panel only
	}

	if(PU>=0 && PD>=0)
	{
		//we have one upstream and one downstream neighbour
		x1  = 0.0;
		x0  = x1 - m_pPanel[p].SMP - m_pPanel[PU].SMP;
		x2  = x1 + m_pPanel[p].SMP + m_pPanel[PD].SMP;
		mu0 = Mu[PU];
		mu1 = Mu[p];
		mu2 = Mu[PD];
		DELP =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PU>=0 && PD<0)
	{
		// no downstream neighbour
		// do we have two upstream neighbours ?
		if(m_pPanel[PU].m_iPU>=0)
		{
			x2  = 0.0;
			x1  = x2 - m_pPanel[p ].SMP  - m_pPanel[PU].SMP;
			x0  = x1 - m_pPanel[PU].SMP  - m_pPanel[m_pPanel[PU].m_iPU].SMP;
			mu0 = Mu[m_pPanel[PU].m_iPU];
			mu1 = Mu[PU];
			mu2 = Mu[p];
			DELP =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = -(Mu[PU]-Mu[p])/(m_pPanel[p].SMP  + m_pPanel[PU].SMP);
		}
	}
	else if(PU<0 && PD>=0)
	{
		// no upstream neighbour
		// do we have two downstream neighbours ?
		if(m_pPanel[PD].m_iPD>=0)
		{
			x0  = 0.0;
			x1  = x0 + m_pPanel[p].SMP  + m_pPanel[PD].SMP;
			x2  = x1 + m_pPanel[PD].SMP + m_pPanel[m_pPanel[PD].m_iPD].SMP;
			mu0 = Mu[p];
			mu1 = Mu[PD];
			mu2 = Mu[m_pPanel[PD].m_iPD];
			DELP =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = (Mu[PD]-Mu[p])/(m_pPanel[p].SMP  + m_pPanel[PD].SMP);
		}
	}
	else
	{
		DELP = 0.0;
	}

	//find middle of side 2
	S2 = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0 - m_pPanel[p].CollPt;
	//convert to local coordinates
	Sl2   = m_pPanel[p].GlobalToLocal(S2);
	VTot  = m_pPanel[p].GlobalToLocal(Vx, Vy, Vz);

	//in panel referential
	VLocal.x = -4.0*PI*(m_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
	VLocal.y = -4.0*PI*DELQ;
//	Vl.z =  4.0*PI*Sigma[p];

	VTot +=VLocal;
	VTot.z = 0.0;

	Speed2 = VTot.x*VTot.x + VTot.y*VTot.y;

	Cp  = 1.0-Speed2/QInf/QInf;
}


/**
* Calculates the Cp coefficient on each panel, using hte vortex circulations or the doublet strengths, depending on the analysis method.
* @param V0 the first value in the sequence, either aoa for type 1 & 2 polars or velocity for type 4
* @param VDelta the increment value of the input parameter, either aoa for type 1 & 2 polars or velocity for type 4
* @param nval the number of values in the sequence
*/
void PanelAnalysis::ComputeOnBodyCp(double V0, double VDelta, int nval)
{
	//following VSAERO theory manual
	//the on-body tangential perturbation speed is the derivative of the doublet strength
	int p, q;
	static double Alpha, *Mu, *Cp;
	static CVector WindDirection, VInf, VLocal;
	double Speed2, cosa, sina;

	//______________________________________________________________________________________
	traceLog("      Computing On-Body Speeds...\n");

	if(m_pWPolar->m_WPolarType != FIXEDAOAPOLAR)
	{
		for (q=0; q<nval; q++)
		{
			//   Define wind axis
			Alpha = V0 + (double)q * VDelta;
			cosa = cos(Alpha*PI/180.0);
			sina = sin(Alpha*PI/180.0);
			WindDirection.Set(cosa, 0.0, sina);
			VInf = WindDirection * m_3DQInf[q];

			Mu     = m_Mu    + q * m_MatSize;
//			Sigma  = m_Sigma + q * m_MatSize;
			Cp     = m_Cp    + q * m_MatSize;

			for (p=0; p<m_MatSize; p++)
			{
				if(m_pPanel[p].m_Pos!=MIDSURFACE)
				{
					VLocal  = m_pPanel[p].GlobalToLocal(VInf);
					VLocal += m_uVl[p]*cosa*m_3DQInf[q] + m_wVl[p]*sina*m_3DQInf[q];
					Speed2 = VLocal.x*VLocal.x + VLocal.y*VLocal.y;
					Cp[p]  = 1.0-Speed2/m_3DQInf[q]/m_3DQInf[q];
				}
				else GetVortexCp(p, Mu, Cp, WindDirection);

				if(s_bCancel) return;
			}
			if(s_bCancel) return;
			s_Progress += 1.0 *(double)nval/(double)nval;
			qApp->processEvents();
		}
	}
	else //FIXEDAOAPOLAR
	{
		//   Define wind axis
		WindDirection.Set(cos(m_Alpha*PI/180.0), 0.0, sin(m_Alpha*PI/180.0));
		for (q=0; q<nval; q++)
		{
			VInf = WindDirection * m_3DQInf[q];

			Mu     = m_Mu    + q * m_MatSize;
//			Sigma  = m_Sigma + q * m_MatSize;
			Cp     = m_Cp    + q * m_MatSize;

			for (p=0; p<m_MatSize; p++)
			{
				if(s_bCancel) break;

				if(m_pPanel[p].m_Pos!=MIDSURFACE) GetDoubletDerivative(p, Mu, Cp[p], VLocal, m_3DQInf[q], VInf.x, VInf.y, VInf.z);
				else                              GetVortexCp(p, Mu, Cp, WindDirection);
			}

			s_Progress += 1.0  *(double)nval/(double)nval;
			qApp->processEvents();
		}
		for (q=1; q<nval; q++)
		{
			if(s_bCancel) return;
			for (p=0; p<m_MatSize; p++)
			{
				m_Cp[p+q*m_MatSize] = m_Cp[p];
			}
		}
	}
}



/**
* Returns the influence at point C of the panel pPanel.
* If the panel pPanel is located on a thin surface, then its the influence of a vortex.
* If it is on a thick surface, then its a doublet.
*
*@param C the point where the influence is to be evaluated
*@param pPanel a pointer to the Panel with the doublet strength
*@param V the perturbation velocity at point C
*@param phi the potential at point C
*@param bWake true if the panel is located on the wake
*@param bAll true if the influence of the bound vortex should be evaluated, in the case of a VLM analysis.
*/
void PanelAnalysis::GetDoubletInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi, bool bWake, bool bAll)
{
	if(pPanel->m_Pos!=MIDSURFACE || pPanel->m_bIsWakePanel)
		pPanel->DoubletNASA4023(C, V, phi, bWake);
	else
	{
		VLMGetVortexInfluence(pPanel, C, V, bAll);
		phi = 0.0;
	}

	if(m_pWPolar->bGround())
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pWPolar->m_Height);

		if(pPanel->m_Pos!=MIDSURFACE || pPanel->m_bIsWakePanel)	pPanel->DoubletNASA4023(CG, VG, phiG, bWake);
		else
		{
			VLMGetVortexInfluence(pPanel, CG, VG, bAll);
			phiG = 0.0;
		}
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


/**
* Returns the influence at point C of a uniform source distribution on the panel pPanel
* The panel is necessarily located on a thick surface, else the source strength is zero
*@param C the point where the influence is to be evaluated
*@param pPanel a pointer to the Panel with the doublet strength
*@param V the perturbation velocity at point C
*@param phi the potential at point C
*/
void PanelAnalysis::GetSourceInfluence(CVector const &C, Panel *pPanel, CVector &V, double &phi)
{

	pPanel->SourceNASA4023(C, V, phi);

	if(m_pWPolar->bGround())
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pWPolar->m_Height);
		pPanel->SourceNASA4023(CG, VG, phiG);
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


/**
* Returns the perturbation velocity vector at a given point, due to the distribution of source and doublet/circulation strengths.
* @param C the point where the influence is to be evaluated
* @param Mu a pointer to the array of doublet strength or vortex circulations
* @param sigma a pointer to the array of source strengths
* @param VT the resulting perturbation velocity
* @param bAll true if the influence of the bound vortex should be included, in the case of a VLM analysis
*/
void PanelAnalysis::GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll)
{
	CVector V;
	int pp, pw, lw;
	double phi, sign;

	VT.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		if(s_bCancel) return;

		if(m_pPanel[pp].m_Pos!=MIDSURFACE) //otherwise Sigma[pp] =0.0, so contribution is zero also
		{
			GetSourceInfluence(C, m_pPanel+pp, V, phi);
			VT += V * Sigma[pp] ;
		}
		GetDoubletInfluence(C, m_pPanel+pp, V, phi, false, bAll);

		VT += V * Mu[pp];

		// Is the panel pp shedding a wake ?
		if(m_pPanel[pp].m_bIsTrailing && m_pPanel[pp].m_Pos!=MIDSURFACE)
		{
			//If so, we need to add the contribution of the wake column shedded by this panel
			if(m_pPanel[pp].m_Pos==BOTSURFACE) sign=-1.0; else sign=1.0;
			pw = m_pPanel[pp].m_iWake;
			for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, m_pWakePanel+pw+lw, V, phi, true, bAll);
				VT += V * Mu[pp]*sign;
			}
		}
	}
}





/**
* Launches a calculation over the input sequence of velocity.
* Used for type 4 analysis, without tilted geometry.
*
* The calculation is performed for two unit RHS, and all the Operating POints are calculated by linear combination.
* The two unit RHS are for a unit velocity along the x-axis, and for a unit velocity along the z-axis.
*@return true if all the aoa were computed successfully, false otherwise. Interpolation issues are not counted as unsuccessful.
*/
bool PanelAnalysis::QInfLoop()
{
	QString str;
	double Alpha = 0.0;

	m_QInf = m_vMin;

	if(!m_pWPolar->bThinSurfaces()) s_TotalTime +=1.0; //for wake contribution

	s_Progress = 0.0;
	qApp->processEvents();

	if(m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(m_pPanel,     m_pMemPanel,     m_MatSize * sizeof(Panel));
		memcpy(m_pNode,      m_pMemNode,      m_nNodes  * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(Panel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		Objects3D::rotateGeomY(m_pPanel, m_pNode, m_pWakePanel, m_pWakeNode,
							   m_MatSize, m_WakeSize, m_nNodes, m_nWakeNodes, m_NWakeColumn,
							   m_pWPolar->m_AlphaSpec, O, m_pWPolar->m_NXWakePanels);

		m_OpAlpha = m_pWPolar->m_AlphaSpec;
		Alpha = 0.0;
	}
	else
	{
		Alpha = m_Alpha;
		m_OpAlpha = m_Alpha;
	}

	str = QString("   Solving the problem... \n");
	traceLog("\n"+str);

	BuildInfluenceMatrix();
	if (s_bCancel) return true;

	CreateUnitRHS();
	if (s_bCancel) return true;

	CreateSourceStrength(m_Alpha, 0.0, 1);
	if (s_bCancel) return true;

	if(!m_pWPolar->bThinSurfaces())
	{
		//compute wake contribution
		CreateWakeContribution();

		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}
	if (s_bCancel) return true;

	if (!SolveUnitRHS())
	{
		s_bWarning = true;
		return true;
	}
	if (s_bCancel) return true;

	CreateDoubletStrength(Alpha, m_vDelta, 1);
	if (s_bCancel) return true;


	ComputeFarField(1.0, m_OpAlpha, 0.0, 1);
	if (s_bCancel) return true;


	for(int q=0; q<m_nRHS; q++)
		m_3DQInf[q] = m_QInf+q*m_vDelta;

	ScaleResultstoSpeed(m_nRHS);
	if (s_bCancel) return true;


	ComputeOnBodyCp(m_QInf, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	ComputeAeroCoefs(m_QInf, m_vDelta, m_nRHS);
	if (s_bCancel) return true;

	return true;
}



/**
* Solves the linear system for the unit RHS, using LU decomposition.
*   Method :
* 	  - If the polar is of type 1 or 2, solve the linear system
*	  - for cosine and sine parts, for a unit speed
*	  - If the polar is of type 4, solve only for unit speed and for the specified Alpha
*	  - Reconstruct right side results if calculation was symetric
*	  - Sort results i.a.w. panel numbering
*
* Calculates the local velocities on each panel
*/
bool PanelAnalysis::SolveUnitRHS()
{
	double taskTime = 400.0;
	int Size = m_MatSize;
//	if(m_b3DSymetric) Size = m_SymSize;

	memcpy(m_RHS,      m_uRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_wRHS, Size * sizeof(double));

	traceLog("      Performing LU Matrix decomposition...\n");

	if(!Crout_LU_Decomposition_with_Pivoting(m_aij, m_Index, Size, &s_bCancel, taskTime*(double)m_MatSize/400.0, s_Progress))
	{
		traceLog("      Singular Matrix.... Aborting calculation...\n");
		return false;
	}


	traceLog("      Solving the LU system...\n");
	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,      Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+Size, Size, &s_bCancel);



	memcpy(m_uRHS, m_RHS,           m_MatSize*sizeof(double));
	memcpy(m_wRHS, m_RHS+m_MatSize, m_MatSize*sizeof(double));

	//   Define unit local velocity vector, necessary for moment calculations in stability analysis of 3D panels
	CVector u(1.0, 0.0, 0.0);
	CVector w(0.0, 0.0, 1.0);
	double Cp;
	for (int p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_Pos!=MIDSURFACE)
		{
			GetDoubletDerivative(p, m_uRHS, Cp, m_uVl[p], 1.0, u.x, u.y, u.z);
			GetDoubletDerivative(p, m_wRHS, Cp, m_wVl[p], 1.0, w.x, w.y, w.z);
		}
		if(s_bCancel) return false;
	}

	return true;
}



/**
*
* Creates the doublet strength or the vortex circulations for all the operating points from the unit sine and cosine unit results.
* The doublet and source strengths are for a unit speed.
* The scaling to speed is performed at the next step, depending on the polar type.
*
* @param Alpha0 the first aoa in the sequence
* @param AlphaDelta the aoa increment
* @param nval the number of aoa to calculate
*/
void PanelAnalysis::CreateDoubletStrength(double Alpha0, double AlphaDelta, int nval)
{

	int  q, p;
	double alpha, cosa, sina;

//	int Size = m_MatSize;
//	if(m_b3DSymetric) Size = m_SymSize;

	traceLog("      Calculating doublet strength...\n");
//	if(m_pWPolar->m_WPolarType!=FIXEDAOAPOLAR) nrhs = nval;
//	else                                 nrhs = 0;

	//______________________________________________________________________________________
	//	reconstruct all results from cosine and sine unit vectors
	for (q=0; q<nval;q++)
	{
		alpha = Alpha0 + q * AlphaDelta;
		cosa = cos(alpha*PI/180.0);
		sina = sin(alpha*PI/180.0);
		for(p=0; p<m_MatSize; p++)
		{
			m_Mu[p+q*m_MatSize] = cosa*m_uRHS[p] + sina*m_wRHS[p];
		}
	}
}



/**
* Performs the summation of on-body forces to calculate the total lift and drag forces
* @param Cp a pointer to the array of previously calculated Cp coefficients
* @param Alpha the aoa for which this calculation is performed
* @param Lift the resulting lift force
* @param Drag the resulting drag force
*/
void PanelAnalysis::SumPanelForces(double *Cp, double Alpha, double &Lift, double &Drag)
{
	int p;
	CVector PanelForce;

	for(p=0; p<m_MatSize; p++)
	{
		PanelForce += m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;
	}

	Lift = PanelForce.z * cos(Alpha*PI/180.0) - PanelForce.x * sin(Alpha*PI/180.0);
	Drag = PanelForce.x * cos(Alpha*PI/180.0) + PanelForce.z * sin(Alpha*PI/180.0);
}



/**
* Launches the calculation of operating points, when linear combination is not an option.
* This is the case for analysis of tilted geometries, or with wake roll-up, of for Beta-type polars
*
*@return true if the aoa was computed successfully, false otherwise.
*/
bool PanelAnalysis::unitLoop()
{
	QString str;
	CVector O(0.0,0.0,0.0);

	int n, nWakeIter, MaxWakeIter;

	if(!m_pWPolar->m_bWakeRollUp) MaxWakeIter = 1;
	else                          MaxWakeIter = qMax(m_nMaxWakeIter, 1);

	s_Progress = 0.0;
	qApp->processEvents();

	str = QString("   Solving the problem...\n");
	traceLog("\n"+str);

	for (n=0; n<m_nRHS; n++)
	{
		if(m_pWPolar->polarType() == FIXEDLIFTPOLAR || m_pWPolar->polarType() == FIXEDSPEEDPOLAR)
		{
			m_OpAlpha = m_vMin+n*m_vDelta;
			m_OpBeta  = m_pWPolar->m_BetaSpec;
		}
		else if(m_pWPolar->polarType() == BETAPOLAR)
		{
			m_OpAlpha = m_pWPolar->m_AlphaSpec;
			m_OpBeta  = m_vMin+n*m_vDelta;
		}	
			
		if(m_pWPolar->polarType()!=BETAPOLAR) str = QString("      \n    Processing Alpha= %1\n").arg(m_OpAlpha,0,'f',1);
		else                                  str = QString("      \n    Processing Beta= %1\n").arg(m_OpBeta,0,'f',1);
		traceLog(str);

		//reset the initial geometry before a new angle is processed
		memcpy(m_pPanel,     m_pMemPanel,     m_MatSize    * sizeof(Panel));
		memcpy(m_pNode,      m_pMemNode,      m_nNodes     * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize   * sizeof(Panel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		Objects3D::rotateGeomY(m_pPanel, m_pNode, m_pWakePanel, m_pWakeNode,
			                   m_MatSize, m_WakeSize, m_nNodes, m_nWakeNodes,
			                   m_NWakeColumn, m_OpAlpha, O, m_pWPolar->m_NXWakePanels);

		if(m_pWPolar->polarType()==BETAPOLAR)
		{
			Objects3D::rotateGeomZ(m_pPanel, m_pNode, m_pWakePanel, m_pWakeNode,
			                       m_MatSize, m_WakeSize, m_nNodes, m_nWakeNodes, m_NWakeColumn,
			                       m_OpBeta, O, m_pWPolar->m_NXWakePanels);
		}

		BuildInfluenceMatrix();
		if (s_bCancel) return true;

		CreateUnitRHS();
		if (s_bCancel) return true;

		CreateSourceStrength(0.0, m_vDelta, 1);
		if (s_bCancel) return true;

		for (nWakeIter = 0; nWakeIter<MaxWakeIter; nWakeIter++)
		{
			if(m_pWPolar->m_bWakeRollUp)
			{
				str = QString("      Wake iteration %1\n").arg(nWakeIter+1,3);
				traceLog(str);
			}

			if (s_bCancel) return true;

			//todo : check... may not be quite correct
			if(!m_pWPolar->bThinSurfaces())
			{
				//compute wake contribution
				CreateWakeContribution();
				//add wake contribution to matrix and RHS
				for(int p=0; p<m_MatSize; p++)
				{
					m_uRHS[p]+= m_uWake[p];
					m_wRHS[p]+= m_wWake[p];
					for(int pp=0; pp<m_MatSize; pp++)
					{
						m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
					}
				}
			}

			if (s_bCancel) return true;

			if (!SolveUnitRHS())
			{
				s_bWarning = true;
				return true;
			}
			if (s_bCancel) return true;

			CreateDoubletStrength(0.0, m_vDelta, 1);
			if (s_bCancel) return true;

			ComputeFarField(1.0, 0.0, m_vDelta, 1);
			if (s_bCancel) return true;

			ComputeBalanceSpeeds(0.0, 0);
			if (s_bCancel) return true;

			ScaleResultstoSpeed(1);
			if (s_bCancel) return true;

			ComputeOnBodyCp(0.0, m_vDelta, 1);
			if (s_bCancel) return true;

//			if(MaxWakeIter>0 && m_pWPolar->m_bWakeRollUp) RelaxWake();
		}

		ComputeAeroCoefs(0.0, m_vDelta, 1);
	}
	return true;
}





/**
* Returns the perturbation velocity created at a point C by a horseshoe or quad vortex with unit circulation located on a panel pPanel
* @param pPanel a pointer to the Panel where the vortex is located
* @param C the point where the perrturbation is evaluated
* @param V a reference to the resulting perturbation velocity vector
* @param bAll true if the influence of the bound vector should be included. Not necessary in the case of a far-field evaluation.
*/
void PanelAnalysis::VLMGetVortexInfluence(Panel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	static int lw, pw, p;
	static CVector AA1, BB1, VT;

	p = pPanel->m_iElement; 

	V.x = V.y = V.z = 0.0;

	if(m_pWPolar->m_bVLM1)
	{
		//just get the horseshoe vortex's influence
		VLMCmn(pPanel->VA, pPanel->VB, C, V, bAll);
	}
	else
	{
		// we have quad vortices
		// so we follow Katz and Plotkin's lead
		if(!pPanel->m_bIsTrailing)
		{
			if(bAll)
			{
				VLMQmn(pPanel->VA, pPanel->VB, m_pPanel[p-1].VA, m_pPanel[p-1].VB, C, V);
			}
		}
		else
		{
			// then panel p is trailing and shedding a wake
			if(!m_bWakeRollUp)
			{
				// since Panel p+1 does not exist...
				// we define the points AA=A+1 and BB=B+1
				AA1.x = m_pNode[pPanel->m_iTA].x + (m_pNode[pPanel->m_iTA].x-pPanel->VA.x)/3.0;
				AA1.y = m_pNode[pPanel->m_iTA].y;
				AA1.z = m_pNode[pPanel->m_iTA].z;
				BB1.x = m_pNode[pPanel->m_iTB].x + (m_pNode[pPanel->m_iTB].x-pPanel->VB.x)/3.0;
				BB1.y = m_pNode[pPanel->m_iTB].y;
				BB1.z = m_pNode[pPanel->m_iTB].z;

				// first we get the quad vortex's influence
				if (bAll)
				{
					VLMQmn(pPanel->VA, pPanel->VB, AA1, BB1, C, V);
				}

				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,C,VT,bAll);

				V.x += VT.x;
				V.y += VT.y;
				V.z += VT.z;
			}
			else
			{
				// if there is a wake roll-up required
				pw = pPanel->m_iWake;
				// first close the wing's last vortex ring at T.E.
				if (bAll)
				{
					VLMQmn(pPanel->VA, pPanel->VB, m_pWakePanel[pw].VA, m_pWakePanel[pw].VB, C, V);
				}

				//each wake panel has the same vortex strength than the T.E. panel
				//so we just cumulate their unit influences
				if(bAll)
				{
					for (lw=0; lw<m_pWPolar->m_NXWakePanels-1; lw++)
					{
						VLMQmn(m_pWakePanel[pw  ].VA, m_pWakePanel[pw  ].VB,
							   m_pWakePanel[pw+1].VA, m_pWakePanel[pw+1].VB, C, VT);
						V += VT;

						pw++;
					}
				}
//
// For the very last wake panel downstream, just add a horseshoe vortex influence
//
// TODO : check influence on results
//
//				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//
// simple really !
//
			}
		}
	}
}



/**
* Performs a stability analysis for each requested position of the control surfaces.
*
*/
bool PanelAnalysis::controlLoop()
{
	//
	//	Loop for each control value
	//          Update the geometry, design variables
	//          Build the influence matrix
	//          Perform LU matrix decomposition
	//          Solve a first time the VLM problem to find the trimmed conditions:
	//              - solve for unit RHS
	//              - iterate to find equilibrium aoa such that Cm=0 in steady level flight or banked turn
	//          Build the rotation matrix from body axes to stability axes
	//          Build the RHS for unit velocity fields in stability axes
	//          Build the RHS for unit control rotations (Normals only) in stability axes
	//          Solve the VLM problem for RHS in stability axes
	//          Compute stability derivatives
	//          Compute control derivatives
	//          Compute inertia in stability axis
	//          Longitudinal stability :
	//             - Build longitudinal state matrix
	//             - Solve for eigenvalues and eigenvectors
	//          Lateral stability
	//             - Build lateral state matrix
	//             - Solve for eigenvalues and eigenvectors
	//          Compute aero coeffs for alpha_eq
	//          Store OpPoint and polar data
	//	end loop
	//
	int i;
	QString str, outString;


	m_Ib[0][0] = m_pWPolar->m_CoGIxx;
	m_Ib[1][1] = m_pWPolar->m_CoGIyy;
	m_Ib[2][2] = m_pWPolar->m_CoGIzz;
	m_Ib[0][2] = m_Ib[2][0] = m_pWPolar->m_CoGIxz;
	m_Ib[1][0] = m_Ib[1][2] = m_Ib[0][1] = m_Ib[2][1] = 0.0;


	str = QString("   Mass=%1 kg").arg(m_pWPolar->m_Mass, 12,'f',3)+"\n";
	traceLog(str);

	str = "\n   ___Center of Gravity Position - Body axis____\n";
	traceLog(str);
	str = QString("    CoG_x=%1 m").arg(m_pWPolar->m_CoG.x, 12,'f',4)+"\n";
	traceLog(str);
	str = QString("    CoG_y=%1 m").arg(m_pWPolar->m_CoG.y, 12,'f',4)+"\n";
	traceLog(str);
	str = QString("    CoG_z=%1 m").arg(m_pWPolar->m_CoG.z, 12,'f',4)+"\n";
	traceLog(str);

	str = "\n   ___Inertia - Body Axis - CoG Origin____\n";
	traceLog(str);
	str = QString("    Ibxx=%1 kg.m2").arg(m_Ib[0][0], 12,'g',4);
	traceLog(str+"\n");
	str = QString("    Ibyy=%1 kg.m2").arg(m_Ib[1][1], 12,'g',4);
	traceLog(str+"\n");
	str = QString("    Ibzz=%1 kg.m2").arg(m_Ib[2][2], 12,'g',4);
	traceLog(str+"\n");
	str = QString("    Ibxz=%1 kg.m2").arg(m_Ib[0][2], 12,'g',4);
	traceLog(str+"\n\n");


	s_Progress = 0.0;
	qApp->processEvents();

	m_bTrace = true;

	str = QString("   Solving the problem... \n\n");
	traceLog("\n"+str);

	for (i=0; i<m_nRHS; i++)
	{
		// create the geometry for the control parameter
		// so first restore the initial geometry
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(Panel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));

		//define the control position for this iteration
		m_Ctrl = m_vMin +(double)i *m_vDelta;
		str = QString("      Calculation for control position %1\n").arg(m_Ctrl ,5,'f',2);
		traceLog(str);
		outString.clear();

		Objects3D::setControlPositions(m_pPlane, m_pWPolar, m_pPanel, m_pNode, m_Ctrl, m_NCtrls, outString, true);

		traceLog(outString);
		if(s_bCancel) break;

		// next find the balanced and trimmed conditions
		if(!ComputeTrimmedConditions())
		{
			if(s_bCancel) break;
			//no zero moment alpha
			str = QString("      Unsuccessfull attempt to trim the model for control position=%1 - skipping.\n\n\n").arg(m_Ctrl,5,'f',2);
			traceLog(str);
			s_bWarning = true;
		}
		else
		{
			m_3DQInf[i] = u0;
			m_QInf      = u0;

			if (s_bCancel) return true;

			//Build the rotation matrix from body axes to stability axes
			BuildRotationMatrix();
			if(s_bCancel) break;

			// Compute inertia in stability axes
			ComputeStabilityInertia();
			if(s_bCancel) break;

			str = "\n      ___Inertia - Stability Axis - CoG Origin____\n";
			traceLog(str);
			str = QString("      Isxx=%1 ").arg(m_Is[0][0], 12,'g',4);
			traceLog(str+"\n");
			str = QString("      Isyy=%1 ").arg(m_Is[1][1], 12,'g',4);
			traceLog(str+"\n");
			str = QString("      Iszz=%1 ").arg(m_Is[2][2], 12,'g',4);
			traceLog(str+"\n");
			str = QString("      Isxz=%1 ").arg(m_Is[0][2], 12,'g',4);
			traceLog(str+"\n\n");

			// Compute stability and control derivatives in stability axes
			// viscous or not viscous ?
			ComputeStabilityDerivatives();
			if(s_bCancel) break;

			ComputeControlDerivatives(); //single derivative, wrt the polar's control variable
			if(s_bCancel) break;

			ComputeNDStabDerivatives();

			// Construct the state matrices - longitudinal and lateral
			BuildStateMatrices();

			// Solve for eigenvalues
			if(!SolveEigenvalues())
			{
				str = QString("      Unsuccessfull attempt to compute eigenvalues for Control=%1 - skipping.\n\n\n").arg(m_Ctrl,10,'f',3);
				traceLog(str);
				s_bWarning = true;
			}
			else
			{
				// Compute aero coefficients for trimmed conditions
				ComputeFarField(m_QInf, m_AlphaEq, 0.0, 1);
				if (s_bCancel) return true;

				ComputeOnBodyCp(m_AlphaEq, 0.0, 1);
				if (s_bCancel) return true;


				str = QString("      Computing Plane for alpha=%1").arg(m_AlphaEq,7,'f',2);
				str += QString::fromUtf8("°\n");
				traceLog(str);
				computePlane(m_AlphaEq, u0, m_OpBeta, 0);

				if (s_bCancel) return true;
			}
			str = QString("\n     ______Finished operating point calculation for control position %1________\n\n\n\n\n").arg(m_Ctrl, 5,'f',2);
			traceLog(str);
		}
		if(s_bCancel) break;
	}
	return true;
}



/**
* Extracts the eigenvalues and eigenvectors from the state matrices.
* Stores the results in the memeber variables if successful.
* @return true if the extraction was successful.
*/
bool PanelAnalysis::SolveEigenvalues()
{
	// Finds the eigenvalues and eigenvectors of the state matrices ALong and ALat
	static double pLong[5], pLat[5];//the coefficients of the characteristic polynomial
	static int i;
	QString str;

	CharacteristicPol(m_ALong, pLong);

	if(!LinBairstow(pLong, m_rLong, 4))
	{
		traceLog("\n       Error extracting longitudinal eigenvalues\n");
		return false;
	}

	//sort them
	ComplexSort(m_rLong, 4);

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALong, m_rLong[i], m_vLong+i*4))
		{
			str = QString("Error extracting longitudinal eigenvector for mode %1\n").arg(i);
			return false;
		}
	}

	str = "\n\n      ___Longitudinal modes____\n\n";
	traceLog(str);

	str = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
		  .arg(m_rLong[0].real(),9, 'g', 4).arg(m_rLong[0].imag(),9, 'g', 4)
		  .arg(m_rLong[1].real(),9, 'g', 4).arg(m_rLong[1].imag(),9, 'g', 4)
		  .arg(m_rLong[2].real(),9, 'g', 4).arg(m_rLong[2].imag(),9, 'g', 4)
		  .arg(m_rLong[3].real(),9, 'g', 4).arg(m_rLong[3].imag(),9, 'g', 4);
	traceLog(str);
	str=("                    _____________________________________________________________________________________________________\n");
	traceLog(str);

	i=0;
	str = QString("      Eigenvector: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
			  .arg(m_vLong[i].real(),    9, 'g', 4).arg(m_vLong[i].imag(),    9, 'g', 4)
			  .arg(m_vLong[i+4].real(),  9, 'g', 4).arg(m_vLong[i+4].imag(),  9, 'g', 4)
			  .arg(m_vLong[i+8].real(),  9, 'g', 4).arg(m_vLong[i+8].imag(),  9, 'g', 4)
			  .arg(m_vLong[i+12].real(), 9, 'g', 4).arg(m_vLong[i+12].imag(), 9, 'g', 4);
	traceLog(str);
	for (i=1; i<4; i++)
	{
			str = QString("                   %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_vLong[i].real(),    9, 'g', 4).arg(m_vLong[i].imag(),    9, 'g', 4)
					  .arg(m_vLong[i+4].real(),  9, 'g', 4).arg(m_vLong[i+4].imag(),  9, 'g', 4)
					  .arg(m_vLong[i+8].real(),  9, 'g', 4).arg(m_vLong[i+8].imag(),  9, 'g', 4)
					  .arg(m_vLong[i+12].real(), 9, 'g', 4).arg(m_vLong[i+12].imag(), 9, 'g', 4);
			traceLog(str);
	}
	str = "\n";
	traceLog(str);

	CharacteristicPol(m_ALat, pLat);

	if(!LinBairstow(pLat, m_rLat, 4))
	{
		traceLog("\n       Error extracting lateral eigenvalues\n");
		return false;
	}

	//sort them
	ComplexSort(m_rLat, 4);

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALat, m_rLat[i], m_vLat+i*4))
		{
			str = QString("Error extracting lateral eigenvector for mode %1\n").arg(i);
			return false;
		}
	}


	str = "\n\n      ___Lateral modes____\n\n";
	traceLog(str);

	str = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
		  .arg(m_rLat[0].real(),9, 'g', 4).arg(m_rLat[0].imag(),9, 'g', 4)
		  .arg(m_rLat[1].real(),9, 'g', 4).arg(m_rLat[1].imag(),9, 'g', 4)
		  .arg(m_rLat[2].real(),9, 'g', 4).arg(m_rLat[2].imag(),9, 'g', 4)
		  .arg(m_rLat[3].real(),9, 'g', 4).arg(m_rLat[3].imag(),9, 'g', 4);
	traceLog(str);
	str=("                    _____________________________________________________________________________________________________\n");
	traceLog(str);

	i=0;
	str = QString("      Eigenvector: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
			  .arg(m_vLat[i].real(),    9, 'g', 4).arg(m_vLat[i].imag(),    9, 'g', 4)
			  .arg(m_vLat[i+4].real(),  9, 'g', 4).arg(m_vLat[i+4].imag(),  9, 'g', 4)
			  .arg(m_vLat[i+8].real(),  9, 'g', 4).arg(m_vLat[i+8].imag(),  9, 'g', 4)
			  .arg(m_vLat[i+12].real(), 9, 'g', 4).arg(m_vLat[i+12].imag(), 9, 'g', 4);
	traceLog(str);
	for (i=1; i<4; i++)
	{
			str = QString("                   %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_vLat[i].real(),    9, 'g', 4).arg(m_vLat[i].imag(),    9, 'g', 4)
					  .arg(m_vLat[i+4].real(),  9, 'g', 4).arg(m_vLat[i+4].imag(),  9, 'g', 4)
					  .arg(m_vLat[i+8].real(),  9, 'g', 4).arg(m_vLat[i+8].imag(),  9, 'g', 4)
					  .arg(m_vLat[i+12].real(), 9, 'g', 4).arg(m_vLat[i+12].imag(), 9, 'g', 4);
			traceLog(str);
	}
	str = "\n";
	traceLog(str);

	return true;
}

/**
* Computes the non-dimensional stability derivatives.
* Outputs the results to the log file
*/
void PanelAnalysis::ComputeNDStabDerivatives()
{
	QString str;
	double b, S, mac, q, theta0, Cw0;
	double mass = m_pWPolar->m_Mass;
	double rho = m_pWPolar->m_Density;

	q = 1./2. * m_pWPolar->m_Density * u0 * u0;
	b   = m_pWPolar->m_WSpan;
	S   = m_pWPolar->m_WArea;
	mac = m_pPlane->mac();
	theta0 = 0.0;//steady level flight only ?

	Cw0 = mass * 9.81/q/S; //E&R p.127
//	Cx0 =  Cw0 * sin(theta0); //E&R p.119
//	Cz0 = -Cw0 * cos(theta0); //E&R p.118

	//E&R p. 118, table 4.4
	CXu = (Xu - rho * u0*S*Cw0*sin(theta0))/(0.5*rho*u0*S);
	CZu = (Zu + rho * u0*S*Cw0*cos(theta0))/(0.5*rho*u0*S);
	Cmu = Mu /(0.5*rho*u0*mac*S);
	CXa = Xw /(0.5*rho*u0*S);
	CZa = Zw /(0.5*rho*u0*S);
	Cma = Mw /(0.5*rho*u0*mac*S);
	CXq = Xq /(.25*rho*u0*mac*S);
	CZq = Zq /(.25*rho*u0*mac*S);
	Cmq = Mq /(.25*rho*u0*mac*mac*S);

	XNP = m_pWPolar->m_CoG.x + Cma/CZa * mac; //E&R (eq. 2.3.5 p.29)

	CYb = Yv*    u0     /(q*S);
	CYp = Yp* 2.*u0     /(q*S*b);
	CYr = Yr* 2.*u0     /(q*S*b);
	Clb = Lv*    u0     /(q*S*b);
	Clp = Lp*(2.*u0/b)  /(q*S*b);
	Clr = Lr*(2.*u0/b)  /(q*S*b);
	Cnb = Nv*    u0     /(q*S*b);
	Cnp = Np*(2.*u0/b)  /(q*S*b);
	Cnr = Nr*(2.*u0/b)  /(q*S*b);

	CXe = Xde/(q*S);
	CYe = Yde/(q*S);
	CZe = Zde/(q*S);
	Cle = Lde/(q*S*b);
	Cme = Mde/(q*S*mac);
	Cne = Nde/(q*S*b);

	// no OpPoint, we output the data to the log file
	str = "      Longitudinal derivatives\n";
	traceLog(str);
	str = QString("      Xu=%1         Cxu=%2\n").arg(Xu,12,'g',5).arg(CXu, 12, 'g', 5);
	traceLog(str);
	str = QString("      Xw=%1         Cxa=%2\n").arg(Xw,12,'g',5).arg(CXa, 12, 'g', 5);
	traceLog(str);
	str = QString("      Zu=%1         Czu=%2\n").arg(Zu,12,'g',5).arg(CZu, 12, 'g', 5);
	traceLog(str);
	str = QString("      Zw=%1         CLa=%2\n").arg(Zw,12,'g',5).arg(-CZa,12, 'g', 5);
	traceLog(str);
	str = QString("      Zq=%1         CLq=%2\n").arg(Zq,12,'g',5).arg(-CZq,12, 'g', 5);
	traceLog(str);
	str = QString("      Mu=%1         Cmu=%2\n").arg(Mu,12,'g',5).arg(Cmu, 12, 'g', 5);
	traceLog(str);
	str = QString("      Mw=%1         Cma=%2\n").arg(Mw,12,'g',5).arg(Cma, 12, 'g', 5);
	traceLog(str);
	str = QString("      Mq=%1         Cmq=%2\n").arg(Mq,12,'g',5).arg(Cmq, 12, 'g', 5);
	traceLog(str);

	str = QString("      Neutral Point position=%1 m").arg(XNP, 10,'f',5);
	str +="\n\n";
	traceLog(str);

	str = "\n      Lateral derivatives\n";
	traceLog(str);
	str = QString("      Yv=%1         CYb=%2\n").arg(Yv,12,'g',5).arg(CYb,12,'g',5);
	traceLog(str);
	str = QString("      Yp=%1         CYp=%2\n").arg(Yp,12,'g',5).arg(CYp,12,'g',5);
	traceLog(str);
	str = QString("      Yr=%1         CYr=%2\n").arg(Yr,12,'g',5).arg(CYr,12,'g',5);
	traceLog(str);
	str = QString("      Lv=%1         Clb=%2\n").arg(Lv,12,'g',5).arg(Clb,12,'g',5);
	traceLog(str);
	str = QString("      Lp=%1         Clp=%2\n").arg(Lp,12,'g',5).arg(Clp,12,'g',5);
	traceLog(str);
	str = QString("      Lr=%1         Clr=%2\n").arg(Lr,12,'g',5).arg(Clr,12,'g',5);
	traceLog(str);
	str = QString("      Nv=%1         Cnb=%2\n").arg(Nv,12,'g',5).arg(Cnb,12,'g',5);
	traceLog(str);
	str = QString("      Np=%1         Cnp=%2\n").arg(Np,12,'g',5).arg(Cnp,12,'g',5);
	traceLog(str);
	str = QString("      Nr=%1         Cnr=%2\n\n").arg(Nr,12,'g',5).arg(Cnr,12,'g',5);
	traceLog(str);


	//output control derivatives

	str = QString("      Control derivatives \n");
	traceLog(str);

	str = QString("      Xde=%1        CXde=%2\n").arg(Xde,12,'g',5).arg(Xde/(q*S),12,'g',5);
	traceLog(str);

	str = QString("      Yde=%1        CYde=%2\n").arg(Yde,12,'g',5).arg(Yde/(q*S),12,'g',5);
	traceLog(str);

	str = QString("      Zde=%1        CZde=%2\n").arg(Zde,12,'g',5).arg(Zde/(q*S),12,'g',5);
	traceLog(str);

	str = QString("      Lde=%1        CLde=%2\n").arg(Lde,12,'g',5).arg(Lde/(q*S*b),12,'g',5);
	traceLog(str);

	str = QString("      Mde=%1        CMde=%2\n").arg(Mde,12,'g',5).arg(Mde/(q*S*mac),12,'g',5);
	traceLog(str);

	str = QString("      Nde=%1        CNde=%2\n").arg(Nde,12,'g',5).arg(Nde/(q*S*b),12,'g',5);
	traceLog(str+"\n");

	str ="\n";
	traceLog(str);

}


/**
* Calculates the forces using a farfield method.
* Calculates the moments by a near field method, i.e. direct summation on the panels.
* Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
* @param Mu a pointer to the array of doublet strengths or vortex circulations
* @param Sigma a pointer to the array of source strengths
* @param *VInf a pointer to the array of the velocity vectors on the panels
* @param Force the resulting force vector
* @param Moment the resulting moment vector
* @param bTilted  true if the calculation is performed on a tilted geometry
*/
void PanelAnalysis::Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment)
{
	if(!m_pPanel || !m_pWPolar) return;

	static bool bOutRe, bError, bOut, bOutCl;
	static int j, k, l, p, pp, m, nw, iTA, iTB;
	static double cosa, sina, Re, PCd, Cl, Cp, tau, StripArea, ViscousDrag;
	static double QInf, QInfStrip, qdyn, GammaStrip;
	static CVector  C, PtC4, LeverArm, WindDirection, WindNormal, PanelLeverArm, Wg;
	static CVector Velocity, StripForce, ViscousMoment, dF, PanelForce, PanelForcep1;

	bOut = bOutCl = bError = false;

	int coef = 2;
	if (m_pWPolar->bThinSurfaces()) coef = 1;

	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	//   Define the wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	p=m=0;

	Force.Set( 0.0, 0.0, 0.0);
	Moment.Set(0.0, 0.0, 0.0);
	ViscousDrag = 0.0;
	ViscousMoment.Set(0.0,0.0,0.0);

	for(j=0; j<m_ppSurface->size(); j++)
	{
		if(m_ppSurface->at(j)->m_bIsTipLeft && !m_pWPolar->bThinSurfaces()) p+=m_ppSurface->at(j)->m_NXPanels;//tip patch panels

		for(k=0; k<m_ppSurface->at(j)->m_NYPanels; k++)
		{
			//Get the strip area
			pp=p;
			StripArea = 0.0;
			for (l=0; l<coef*m_ppSurface->at(j)->m_NXPanels; l++)
			{
				StripArea  += m_pPanel[pp].Area;
				pp++;
			}

			//Get the strip's lifting force
			if(m_pPanel[p].m_Pos!=MIDSURFACE)
			{
				StripArea /=2.0;
				//FF force
				nw  = m_pPanel[p].m_iWake;
				iTA = m_pWakePanel[nw].m_iTA;
				iTB = m_pWakePanel[nw].m_iTB;
				C = (m_pWakeNode[iTA] + m_pWakeNode[iTB])/2.0;
				GetSpeedVector(C, Mu, Sigma, Wg, false);
				Wg.x += VInf[p            ];
				Wg.y += VInf[p+m_MatSize  ];
				Wg.z += VInf[p+2*m_MatSize];

				GammaStrip = (-Mu[p+coef*m_ppSurface->at(j)->m_NXPanels-1] + Mu[p]) *4.0*PI;

				StripForce  = m_pPanel[p].Vortex * Wg;
				StripForce *= GammaStrip;                            //Newtons/rho
				Force += StripForce;

				Velocity.x = *(VInf               +p);
				Velocity.y = *(VInf +   m_MatSize +p);
				Velocity.z = *(VInf + 2*m_MatSize +p);
				QInfStrip = Velocity.VAbs(); //used for viscous drag at the next step

				p+=m_ppSurface->at(j)->m_NXPanels*coef;
			}
			else
			{
				//iPos=0, VLM type panel
				StripForce.Set(0.0,0.0,0.0);
				for(l=0; l<m_ppSurface->at(j)->m_NXPanels; l++)
				{
					Velocity.x = *(VInf               +p);
					Velocity.y = *(VInf +   m_MatSize +p);
					Velocity.z = *(VInf + 2*m_MatSize +p);
					QInfStrip = Velocity.VAbs();

					//FF force
					if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
					{
						C = m_pPanel[p].CtrlPt;
						C.x = m_pPlane->planformSpan() * 100.0;

						GetSpeedVector(C, Mu, Sigma, Wg, false);
						Wg += Velocity; //total speed vector

						//induced force
						dF  = Wg * m_pPanel[p].Vortex;
						dF *=  Mu[p];  // N/rho

						Force += dF;        // N/rho
						StripForce += dF;
					}
					//On-Body moment
					PanelForce  = Velocity * m_pPanel[p].Vortex;
					PanelForce *= Mu[p];                                 //Newtons/rho

					if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
					{
						PanelForcep1  = Velocity * m_pPanel[p].Vortex;
						PanelForcep1 *= Mu[p+1];                          //Newtons/rho

						PanelForce -= PanelForcep1;
					}

					PanelLeverArm = m_pPanel[p].VortexPos - m_pWPolar->m_CoG;
					Moment += PanelLeverArm * PanelForce;                     // N.m/rho
					p++;
				}

			}
			if(m_pWPolar->bViscous())
			{
				//add the viscous drag component to force and moment
				qdyn = 0.5 * m_pWPolar->m_Density * QInfStrip * QInfStrip;
				m_ppSurface->at(j)->GetC4(k, PtC4, tau);
				Re = m_ppSurface->at(j)->GetChord(tau) * QInfStrip /m_pWPolar->m_Viscosity;
				Cl = StripForce.dot(WindNormal)*m_pWPolar->m_Density/qdyn/StripArea;
				PCd    = GetVar(2, m_ppSurface->at(j)->m_pFoilA, m_ppSurface->at(j)->m_pFoilB, Re, Cl, tau, bOutRe, bError);
				PCd   *= StripArea * 1/2*QInfStrip*QInfStrip;                // Newtons/rho
				bOut = bOut || bOutRe || bError;
				ViscousDrag += PCd ;                                         // Newtons/rho

				LeverArm   = PtC4 - m_pWPolar->m_CoG;

				ViscousMoment.x += PCd * (WindDirection.y*LeverArm.z - WindDirection.z*LeverArm.y);   // N.m/rho
				ViscousMoment.y += PCd * (WindDirection.z*LeverArm.x - WindDirection.x*LeverArm.z);
				ViscousMoment.z += PCd * (WindDirection.x*LeverArm.y - WindDirection.y*LeverArm.x);
			}
			m++;
		}
	}

	if(!m_pWPolar->bThinSurfaces())
	{
		//On-Body moment
		// same as before, except that we take into account tip patches
		CVector VLocal;
		Moment.Set(0.0,0.0,0.0);
		for(p=0; p<m_MatSize; p++)
		{
			Velocity.x = *(VInf               +p);
			Velocity.y = *(VInf +   m_MatSize +p);
			Velocity.z = *(VInf + 2*m_MatSize +p);
			QInf = Velocity.VAbs();

			GetDoubletDerivative(p, Mu, Cp, VLocal, QInf, Velocity.x, Velocity.y, Velocity.z);
			PanelForce = m_pPanel[p].Normal * (-Cp) * m_pPanel[p].Area *1/2.*QInf*QInf;      // Newtons/rho

			PanelLeverArm = m_pPanel[p].CollPt - m_pWPolar->m_CoG;
			Moment += PanelLeverArm * PanelForce;                     // N.m/rho
		}
	}

	if(m_pWPolar->bThinSurfaces()) Force -= WindDirection*Force.dot(WindDirection)/2.0;
	if(m_pWPolar->bViscous())
	{
		Force += WindDirection * ViscousDrag;
		Moment += ViscousMoment;
	}

	Force  *= m_pWPolar->m_Density;                          // N
	Moment *= m_pWPolar->m_Density;                          // N.m
}



#define CM_ITER_MAX 50
/**
* Finds the zero-pitching-moment aoa such that Cm=0.
* Proceeds by iteration between -PI/4 and PI/4
* @return true if an equlibrium angle was found false otherwise.
*/
bool PanelAnalysis::GetZeroMomentAngle()
{
	static int iter;
	static double a, a0, a1, Cm, Cm0, Cm1, tmp;
	static double eps = 1.e-7;

	iter = 0;
	a0 = -PI/4.0;
	a1 =  PI/4.0;

	a = 0.0;
	Cm0 = ComputeCm(a0*180.0/PI);
	Cm1 = ComputeCm(a1*180.0/PI);
	Cm = 1.0;

	//are there two initial values of opposite signs ?
	while(Cm0*Cm1>0.0 && iter <=20)
	{
		a0 *=0.9;
		a1 *=0.9;
		Cm0 = ComputeCm(a0*180.0/PI);
		Cm1 = ComputeCm(a1*180.0/PI);
		iter++;
		qApp->processEvents();
		if(s_bCancel) break;
	}
	if(iter>=100 || s_bCancel) return false;

	iter = 0;

	//Cm0 and Cm1 are of opposite sign
	if(Cm0>Cm1)
	{
		tmp = Cm1;
		Cm1 = Cm0;
		Cm0 = tmp;
		tmp = a0;
		a0  =  a1;
		a1  = tmp;
	}

	while (qAbs(Cm)>eps && iter<=CM_ITER_MAX)
	{
		a = a0 - (a1-a0) * Cm0/(Cm1-Cm0);
		Cm = ComputeCm(a*180.0/PI);
		if(Cm>0.0)
		{
			a1  = a;
			Cm1 = Cm;
		}
		else
		{
			a0  = a;
			Cm0 = Cm;
		}
		iter++;
		qApp->processEvents();
		if(s_bCancel) break;
	}

	if(iter>=CM_ITER_MAX || s_bCancel) return false;

	m_AlphaEq = a*180.0/PI;
	Cm = ComputeCm(m_AlphaEq);// for information only, should be zero

	return true;
}


/**
 * Creates the longitudinal and lateral state matrices
 * from the derivatives and inertias calculated previously

 * Creates the control state matrix from the control derivatives
*/
void PanelAnalysis::BuildStateMatrices()
{
	static int i;
	static double Ipxx, Ipzz, Ipzx;
	static double Ixx,Iyy,Izz, Izx;
	QString strange;
	double Mass = m_pWPolar->m_Mass;

	//use inertia measured in stability axis, CoG origin
	Ixx = m_Is[0][0];
	Iyy = m_Is[1][1];
	Izz = m_Is[2][2];
	Izx = m_Is[0][2];

	//____________________Longitudinal stability_____________
	m_ALong[0][0] = Xu/Mass;
	m_ALong[0][1] = Xw/Mass;
	m_ALong[0][2] = 0.0;
	m_ALong[0][3] = -9.81*cos(Theta0*PI/180.0);

	m_ALong[1][0] =  Zu                            /(Mass-Zwp);
	m_ALong[1][1] =  Zw                            /(Mass-Zwp);
	m_ALong[1][2] = (Zq+Mass*u0)                   /(Mass-Zwp);
	m_ALong[1][3] = -9.81*Mass*sin(Theta0*PI/180.0)/(Mass-Zwp);

	m_ALong[2][0] = (Mu + Mwp*Zu/(Mass-Zwp))                 /Iyy;
	m_ALong[2][1] = (Mw + Mwp*Zw/(Mass-Zwp))                 /Iyy;
	m_ALong[2][2] = (Mq + Mwp*(Zq+Mass*u0)/(Mass-Zwp))       /Iyy;
	m_ALong[2][3] = (Mwp*(-Mass*9.81*sin(Theta0))/(Mass-Zwp))/Iyy;

	m_ALong[3][0] = 0.0;
	m_ALong[3][1] = 0.0;
	m_ALong[3][2] = 1.0;
	m_ALong[3][3] = 0.0;

	strange = "      _____State matrices__________\n";
	traceLog(strange);
	strange = "       Longitudinal state matrix\n";
	traceLog(strange);
	for (i=0; i<4; i++)
	{
		strange = QString("        %1      %2      %3      %4\n")
					.arg(m_ALong[i][0], 14, 'g', 6)
					.arg(m_ALong[i][1], 14, 'g', 6)
					.arg(m_ALong[i][2], 14, 'g', 6)
					.arg(m_ALong[i][3], 14, 'g', 6);
		traceLog(strange);
	}


	//____________________Lateral stability_____________
	Ipxx = (Ixx * Izz - Izx*Izx)/Izz;
	Ipzz = (Ixx * Izz - Izx*Izx)/Ixx;
	Ipzx =  Izx/(Ixx * Izz - Izx*Izx);

	m_ALat[0][0] = Yv/Mass;
	m_ALat[0][1] = Yp/Mass;
	m_ALat[0][2] = Yr/Mass - u0;
	m_ALat[0][3] = 9.81 * cos(Theta0*PI/180.0);

	m_ALat[1][0] = Lv/Ipxx+Ipzx*Nv;
	m_ALat[1][1] = Lp/Ipxx+Ipzx*Np;
	m_ALat[1][2] = Lr/Ipxx+Ipzx*Nr;
	m_ALat[1][3] = 0.0;

	m_ALat[2][0] = Lv*Ipzx+ Nv/Ipzz;
	m_ALat[2][1] = Lp*Ipzx+ Np/Ipzz;
	m_ALat[2][2] = Lr*Ipzx+ Nr/Ipzz;
	m_ALat[2][3] = 0.0;

	m_ALat[3][0] = 0.0;
	m_ALat[3][1] = 1.0;
	m_ALat[3][2] = tan(Theta0*PI/180.0);
	m_ALat[3][3] = 0.0;

	strange = "       Lateral state matrix\n";
	traceLog(strange);
	for (i=0; i<4; i++)
	{
		strange = QString("        %1      %2      %3      %4\n")
					.arg(m_ALat[i][0], 14, 'g', 6)
					.arg(m_ALat[i][1], 14, 'g', 6)
					.arg(m_ALat[i][2], 14, 'g', 6)
					.arg(m_ALat[i][3], 14, 'g', 6);
		traceLog(strange);
	}

	strange ="\n";
	traceLog(strange);

	//build the control matrix
//	for(i=0; i<m_pWPolar->m_nControls; i++)
	{
		// per radian
		m_BLong[0] = Xde/Mass;
		m_BLong[1] = Zde/Mass;
		m_BLong[2] = Mde/Iyy;
		m_BLong[3] = 0.0;

		m_BLat[0] = Yde/Mass;
		m_BLat[1] = Lde/Ipxx+Nde*Ipzx;
		m_BLat[2] = Lde*Ipzx+Nde/Ipzz;
		m_BLat[3] = 0.0;
	}

	strange = "      _____Control Matrices__________\n";
	traceLog(strange);
	strange = "       Longitudinal control matrix\n";
	traceLog(strange);

	strange = QString("        %1\n      %2\n      %3\n      %4\n\n")
				.arg(m_BLong[0], 13, 'g', 7)
				.arg(m_BLong[1], 13, 'g', 7)
				.arg(m_BLong[2], 13, 'g', 7)
				.arg(m_BLong[3], 13, 'g', 7);
	traceLog(strange);
	strange = "       Lateral control matrix\n";
	traceLog(strange);

	strange = QString("        %1\n      %2\n      %3\n      %4\n\n")
				.arg(m_BLat[0], 13, 'g', 7)
				.arg(m_BLat[1], 13, 'g', 7)
				.arg(m_BLat[2], 13, 'g', 7)
				.arg(m_BLat[3], 13, 'g', 7);
	traceLog(strange);
}


/**
 *  This methods builds the rotation matrix from geometrical axis to stability axis,
 *  i.e. if V is a vector defined in the body axis, its coordinates in stability axis are v = R.V
 *  In XFLR5, the x-body-axis is pointing backward, and the convention
 *  for stability axes is with the x-axis pointing forward.
 *  The rotation matrix is set in accordance.
*/
void PanelAnalysis::BuildRotationMatrix()
{

	m_R[0][0] = -cos(m_AlphaEq*PI/180.0);
	m_R[1][0] =  0.0;
	m_R[2][0] =  sin(m_AlphaEq*PI/180.0);
	m_R[0][1] =  0.0;
	m_R[1][1] =  1.0;
	m_R[2][1] =  0.0;
	m_R[0][2] = -sin(m_AlphaEq*PI/180.0);
	m_R[1][2] =  0.0;
	m_R[2][2] = -cos(m_AlphaEq*PI/180.0);
}


/**
 * Computes the trimmed condition for a stability analysis
 * Method :
 *   - For level flight, find the a.o.a. such that Cm=0
 *   - Reconstruct the right side circulations if the calculation was symmetric
 *   - Sort results i.a.w. panel numbering
 *   - Set trimmed parameters for level flight or other
*/
bool PanelAnalysis::ComputeTrimmedConditions()
{
	QString strong, strange;
	int p;
	static double Lift, phi, VerticalCl;
	static CVector VInf, Force, Moment, WindNormal;

	// find aoa such that Cm=0;

	//Build the unit RHS vectors along x and z in Body Axis
	CreateUnitRHS();
	if (s_bCancel) return false;

	// build the influence matrix in Body Axis
	BuildInfluenceMatrix();
	if (s_bCancel) return false;

	if(!m_pWPolar->bThinSurfaces())
	{
		//compute wake contribution
		CreateWakeContribution();
		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}

	if (!SolveUnitRHS())	//solve for the u,w unit vectors
	{
		s_bWarning = true;
		return false;
	}

	strong ="      Searching for zero-moment angle... ";
	if(!GetZeroMomentAngle())
	{
		strong += "none found\n";
		traceLog(strong);
		return false;
	}
	strong += QString("Alpha=%1").arg(m_AlphaEq,0,'f',5) + QString::fromUtf8("°") +"\n";
	traceLog(strong);

	CreateSourceStrength(m_AlphaEq, 0.0, 1);
	if (s_bCancel) return true;

	//reconstruct doublet strengths from unit cosine and sine vectors
	CreateDoubletStrength(m_AlphaEq, 0.0, 1.0);
	qApp->processEvents();
	if(s_bCancel) return false;

	//______________________________________________________________________________________
	// Calculate the trimmed conditions for this control setting and calculated Alpha_eq
/*		phi = bank angle
		V   = sqrt(2 m g / rho S CL cos(phi))    (airspeed)
		R   = V^2 / g tan(phi)        (turn radius, positive for right turn)
		W   = V / R                   (turn rate, positive for right turn)
		p   = 0                       (roll rate, zero for steady turn)
		q   = W sin(phi)              (pitch rate, positive nose upward)
		r   = W cos(phi)              (yaw rate, positive for right turn) */

	//so far we have a unit Vortex Strength
	// find the speeds which will create a lift equal to the weight

	traceLog("      Calculating speed to balance the weight...");

	WindNormal.Set(-sin(m_AlphaEq*PI/180.0), 0.0, cos(m_AlphaEq*PI/180.0));
	VInf.Set(       cos(m_AlphaEq*PI/180.0), 0.0, sin(m_AlphaEq*PI/180.0));
	for(p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = VInf.x;
		m_RHS[51*m_MatSize+p] = VInf.y;
		m_RHS[52*m_MatSize+p] = VInf.z;
	}

	Lift = 0.0;
	p=0;

	u0 = 1.0;

	Forces(m_Mu, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force, Moment);

	phi = m_pWPolar->m_BankAngle *PI/180.0;
	Lift   = Force.dot(WindNormal);		//N/rho ; bank effect not included
	VerticalCl = Lift*2.0/m_pWPolar->m_WArea * cos(phi)/m_pWPolar->m_Density;
	if(Lift<=0.0)
	{
		u0 = -100.0;
		strong = QString("  Found a negative lift for Alpha=%1.... skipping the angle...\n").arg(m_AlphaEq,0,'f',5);
		if(m_bTrace) traceLog(strong);
		m_bPointOut = true;
		s_bWarning = true;
		return false;
	}
	else
	{
		double radius, W, p, q, r; //trimmed flight data

		u0 =  sqrt( 2.0* 9.81 * m_pWPolar->m_Mass /m_pWPolar->m_Density/m_pWPolar->m_WArea / VerticalCl );
		strong = QString("VInf = %1 m/s").arg(u0,0,'f',5);
		strong+= strange + "\n";
		if(m_bTrace) traceLog(strong);

		if(qAbs(m_pWPolar->m_BankAngle)>PRECISION)
		{
			radius = u0*u0/9.81/tan(phi);
			W      = u0/radius;
			p      = 0.0;
			q      = W * sin(phi);
			r      = W * cos(phi);

			strong = QString("      Phi         =%1").arg(m_pWPolar->m_BankAngle,5,'f',2);
			strong += QString::fromUtf8("°");
			if(m_bTrace) traceLog(strong);

			strong = QString("      Turn radius =%1").arg(radius,5,'f',2);
			if(m_bTrace) traceLog(strong);

			strong = QString("      Turn rate   =%1").arg(W,5,'f',2);
			if(m_bTrace) traceLog(strong);

			strong = QString("      Roll rate   =%1").arg(p,5,'f',2);
			if(m_bTrace) traceLog(strong);

			strong = QString("      Pitch rate  =%1").arg(q,5,'f',2);
			if(m_bTrace) traceLog(strong);

			strong = QString("      Yaw rate    =%1").arg(r,5,'f',2);
			if(m_bTrace) traceLog(strong);
		}
	}

	//______________________________________________________________________________________
	// Scale circulations to speeds

	for(p=0; p<m_MatSize; p++)
	{
		m_Mu[p]    *= u0;
		m_Sigma[p] *= u0;
	}
	VInf *= u0;

	// Store the force and moment acting on the surfaces
	// Will be of use later for stability control derivatives
	// need to re-calculate because of viscous drag which is not linear

	for(p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = VInf.x;
		m_RHS[51*m_MatSize+p] = VInf.y;
		m_RHS[52*m_MatSize+p] = VInf.z;
	}

	Forces(m_Mu, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force0, Moment0);

	return true;
}


/**
* Calculates the stability derivatives.
* @todo implement automatic differentiation. Considerable task.
 The stability derivatives are estimated by forward difference at U=(U0,0,0).
 The reference condition has been saved during the calculation of the trimmed condition.
*/
void PanelAnalysis::ComputeStabilityDerivatives()
{
	static CVector V0, Force, Moment, CGM, is, js, ks, Vi, Vj, Vk, Ris, Rjs, Rks, WindDirection, WindNormal;
	static int p;
	static double alpha, sina, cosa, deltaspeed, deltarotation;
	QString strong;
	int Size= m_MatSize;
//	if(m_b3DSymetric) Size = m_SymSize;

	strong = "      Calculating the stability derivatives\n";
	traceLog(strong);

	deltaspeed    = 0.01;         //  m/s   for forward difference estimation
	deltarotation = 0.001;       //  rad/s for forward difference estimation

	// Define the stability axes
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	WindDirection.Set(cosa, 0.0, sina);
	WindNormal.Set(-sina, 0.0, cosa);

	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);

	V0 = is * (-u0); //is the steady state velocity vector, if no sideslip

	//______________________________________________________________________________
	// RHS for unit speed vectors
	// The change in wind velocity is opposite to the change in plane velocity
	Vi = V0 - is * deltaspeed; //a positive increase in axial speed is a positive increase in wind speed
	Vj = V0 - js * deltaspeed; //a plane movement to the right is a wind flow to the left, i.e. negative y
	Vk = V0 - ks * deltaspeed; //a plane movement downwards (Z_stability>0) is a positive increase of V in geometry axes

	for (p=0; p<m_MatSize; p++)
	{
		//re-use existing memory to define the velocity field
		m_RHS[50*m_MatSize+p] = Vi.x;
		m_RHS[51*m_MatSize+p] = Vi.y;
		m_RHS[52*m_MatSize+p] = Vi.z;
		m_RHS[53*m_MatSize+p] = Vj.x;
		m_RHS[54*m_MatSize+p] = Vj.y;
		m_RHS[55*m_MatSize+p] = Vj.z;
		m_RHS[56*m_MatSize+p] = Vk.x;
		m_RHS[57*m_MatSize+p] = Vk.y;
		m_RHS[58*m_MatSize+p] = Vk.z;
		m_Sigma[p]             = -1.0/4.0/PI* (m_RHS[50*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[51*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[52*m_MatSize+p] *m_pPanel[p].Normal.z);
		m_Sigma[p+ m_MatSize]  = -1.0/4.0/PI* (m_RHS[53*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[54*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[55*m_MatSize+p] *m_pPanel[p].Normal.z);
		m_Sigma[p+2*m_MatSize] = -1.0/4.0/PI* (m_RHS[56*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[57*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[58*m_MatSize+p] *m_pPanel[p].Normal.z);
	}

	CreateRHS(m_uRHS, Vi);
	CreateRHS(m_vRHS, Vj);
	CreateRHS(m_wRHS, Vk);

	//______________________________________________________________________________
	// RHS for unit rotation vectors around Stability axis
	// stability axis origin is CoG

	for (p=0; p<m_MatSize; p++)
	{
		//re-use existing memory to define the velocity field
		if(m_pPanel[p].m_Pos==MIDSURFACE) CGM = m_pPanel[p].VortexPos - m_pWPolar->m_CoG;
		else                      CGM = m_pPanel[p].CollPt    - m_pWPolar->m_CoG;

		// a rotation of the plane about a vector is the opposite of a rotation of the freestream about this vector
		Ris = is*CGM * (-deltarotation) + V0;
		Rjs = js*CGM * (-deltarotation) + V0;
		Rks = ks*CGM * (-deltarotation) + V0;

		m_RHS[59*m_MatSize+p] = Ris.x;
		m_RHS[60*m_MatSize+p] = Ris.y;
		m_RHS[61*m_MatSize+p] = Ris.z;
		m_RHS[62*m_MatSize+p] = Rjs.x;
		m_RHS[63*m_MatSize+p] = Rjs.y;
		m_RHS[64*m_MatSize+p] = Rjs.z;
		m_RHS[65*m_MatSize+p] = Rks.x;
		m_RHS[66*m_MatSize+p] = Rks.y;
		m_RHS[67*m_MatSize+p] = Rks.z;
		m_Sigma[p+3*m_MatSize] = -1.0/4.0/PI* (m_RHS[59*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[60*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[61*m_MatSize+p]*m_pPanel[p].Normal.z);
		m_Sigma[p+4*m_MatSize] = -1.0/4.0/PI* (m_RHS[62*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[63*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[64*m_MatSize+p]*m_pPanel[p].Normal.z);
		m_Sigma[p+5*m_MatSize] = -1.0/4.0/PI* (m_RHS[65*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[66*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[67*m_MatSize+p]*m_pPanel[p].Normal.z);
	}

	CreateRHS(m_pRHS, WindDirection, m_RHS+59*m_MatSize);
	CreateRHS(m_qRHS, WindDirection, m_RHS+62*m_MatSize);
	CreateRHS(m_rRHS, WindDirection, m_RHS+65*m_MatSize);

	if(!m_pWPolar->bThinSurfaces())
	{
		// Compute the wake's contribution
		// We ignore the perturbations and consider only the potential of the steady state flow
		// Clearly an approximation which is also implicit in the VLM formulation
		CreateWakeContribution(m_uWake,  WindDirection);// re-use m_uWake memory, which is re-calculated anyway at the next control iteration

		//add wake contribution to all 6 RHS
		for(p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p]*u0;
			m_vRHS[p]+= m_uWake[p]*u0;
			m_wRHS[p]+= m_uWake[p]*u0;
			m_pRHS[p]+= m_uWake[p]*u0;
			m_qRHS[p]+= m_uWake[p]*u0;
			m_rRHS[p]+= m_uWake[p]*u0;
		}
	}


	// The LU matrix is unchanged, so back-substitute for unit vortex circulations
	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,             Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_vRHS, m_Index, m_RHS+  m_MatSize, Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+2*m_MatSize, Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_pRHS, m_Index, m_RHS+3*m_MatSize, Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_qRHS, m_Index, m_RHS+4*m_MatSize, Size, &s_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_rRHS, m_Index, m_RHS+5*m_MatSize, Size, &s_bCancel);

	memcpy(m_uRHS, m_RHS,             m_MatSize*sizeof(double));
	memcpy(m_vRHS, m_RHS+  m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_wRHS, m_RHS+2*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_pRHS, m_RHS+3*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_qRHS, m_RHS+4*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_rRHS, m_RHS+5*m_MatSize, m_MatSize*sizeof(double));


	// Compute stabiliy and control derivatives
	Xu = Xw = Zu = Zw = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np  = Nr  = 0.0;

	//________________________________________________
	// 1st ORDER STABILITY DERIVATIVES
	// x-derivatives________________________
	alpha = atan2(Vi.z, Vi.x) * 180.0/PI;// =m_AlphaEq....
	Forces(m_uRHS, m_Sigma, alpha, m_RHS+50*m_MatSize, Force, Moment);
	Xu = (Force - Force0).dot(is)   /deltaspeed;
	Zu = (Force - Force0).dot(ks)   /deltaspeed;
	Mu = (Moment- Moment0).dot(js)  /deltaspeed;

	// y-derivatives________________________
	alpha = atan2(Vj.z, Vj.x)*180.0/PI;// =m_AlphaEq....
	Forces(m_vRHS, m_Sigma+m_MatSize, alpha, m_RHS+53*m_MatSize, Force, Moment);
	Yv = (Force - Force0).dot(js)   /deltaspeed;
//	Lv = (Moment.dot(WindDirection) - Moment0.dot(is)) /deltaspeed;
	Nv = (Moment.dot(WindNormal)    - Moment0.dot(ks)) /deltaspeed;
	Lv = (Moment - Moment0).dot(is) /deltaspeed;
	Nv = (Moment - Moment0).dot(ks) /deltaspeed;

	// z-derivatives________________________
	alpha = atan2(Vk.z, Vk.x)* 180.0/PI;
	Forces(m_wRHS, m_Sigma+2*m_MatSize, alpha, m_RHS+56*m_MatSize, Force, Moment);
	Xw = (Force - Force0).dot(is)   /deltaspeed;
	Zw = (Force - Force0).dot(ks)   /deltaspeed;
	Mw = (Moment - Moment0).dot(js) /deltaspeed;

	s_Progress +=1;
	qApp->processEvents();

	// p-derivatives
	Forces(m_pRHS, m_Sigma+3*m_MatSize, m_AlphaEq, m_RHS+59*m_MatSize, Force, Moment);
	Yp = (Force-Force0).dot(js)   /deltarotation;
	Lp = (Moment-Moment0).dot(is) /deltarotation;
	Np = (Moment-Moment0).dot(ks) /deltarotation;

	// q-derivatives
	Forces(m_qRHS, m_Sigma+4*m_MatSize, m_AlphaEq, m_RHS+62*m_MatSize, Force, Moment);
	Xq = (Force-Force0).dot(is)   /deltarotation;
	Zq = (Force-Force0).dot(ks)   /deltarotation;
	Mq = (Moment-Moment0).dot(js) /deltarotation;

	// r-derivatives
	Forces(m_rRHS, m_Sigma+5*m_MatSize, m_AlphaEq, m_RHS+65*m_MatSize, Force, Moment);
	Yr = (Force-Force0).dot(js)   /deltarotation;
	Lr = (Moment-Moment0).dot(is) /deltarotation;
	Nr = (Moment-Moment0).dot(ks) /deltarotation;

	s_Progress +=1;
	qApp->processEvents();

	//________________________________________________
	// 2nd ORDER STABILITY DERIVATIVES
	// Zwp & Mwp ... ?
	// M. Drela's answer to the question posted on Yahoo Groups:

	/*	May I take this opportunity to ask you about the stability derivatives w.r.t. alpha dot ?
	Are they ignored in AVL and if so, is it a safe assumption ?

	Yes, the _alphadot derivatives are ignored, for the simple reason that there's no way to do it
	in an algorithmic way for a general configuration. The usual estimates in Etkin or Nelson assume
	 a "typical" tailed airplane configuration, with a small tail and a well-defined tail arm.
	These estimates won't work for other configurations like canard, tandem, or flying wing.

	Normally, only the Cm_alphadot derivative is significant, and slightly augments the pitch-damping
	derivative Cm_q. Leaving it out therefore underpredicts pitch damping slightly, so this is a
	conservative approximation. And pitch damping is not a major concern in any case.
	Simple static pitch stability is more important, and that's not affected by alphadot.

	All this is for a conventional configuration. Not sure what the impact is on the pitch damping of flying wings. */
}

/**
 * Returns the inertia tensor in stability axis

 * R  is the rotation matrix from Body Frame to stability frame
 * Ib is the inertia tensor in the body frame, with origin at the CoG

 * The body frame is the one in which the geometry has been defined,
 * i.e. the x axis points backwards, y is starboard and z is upwards

 * Is is the inertia tensor in stability axes with origin at the CoG
 *                               Is = tR.Ib.R
*/
void PanelAnalysis::ComputeStabilityInertia()
{

	static int i,j;
	static double tR[3][3], tmp[3][3];

	tR[0][0] = m_R[0][0];
	tR[0][1] = m_R[1][0];
	tR[0][2] = m_R[2][0];
	tR[1][0] = m_R[0][1];
	tR[2][0] = m_R[0][2];

	tR[1][1] = m_R[1][1];
	tR[1][2] = m_R[2][1];
	tR[2][1] = m_R[1][2];

	tR[2][2] = m_R[2][2];

	// tmp = Ib.R
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			tmp[i][j] = m_Ib[i][0]*m_R[0][j] + m_Ib[i][1]*m_R[1][j] + m_Ib[i][2]*m_R[2][j];
		}
	}

	// Is = tR.tmp
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			m_Is[i][j] = tR[i][0]*tmp[0][j] + tR[i][1]*tmp[1][j] + tR[i][2]*tmp[2][j];
		}
	}
}


/**
 * Calculates the control derivatives for small control deflections, using forward derivatives

 * The geometry has been previously modified to set the control in the position defined for this iteration of the analysis

 * The problem is not linear, since we take into account viscous forces
 * Therefore to get the derivative, we need to make the difference between two states
 * We use forward differences between the reference state corresponding to the trimmed conditions
 * and the same state + application of a delta angle to all the controls

 * AVL ignores the modification of the matrix terms and therefore requires only a single LU decomposition throughout the problem.

 * The same method is applied here
 *  - Save the reference geometry
 *  - Change the control point positions and the normals of the flaps
 *  - re-generate the RHS vector
 *  - the geometry is reset at the next iteration loop
*/
void PanelAnalysis::ComputeControlDerivatives()
{
	CVector WindDirection, H, Force, Moment, V0, is, js, ks;
	static int j, p, pos, NCtrls;
	static double DeltaAngle, SignedDeltaAngle, cosa, sina;
//	double q, S, b, mac;
	QString str;
	Quaternion Quat;

//	q = 1./2. * m_pWPolar->m_Density * u0 * u0;
//	b   = m_pWPolar->m_WSpan;
//	S   = m_pWPolar->m_WArea;
//	mac = m_pPlane->mac();


	bool bActive = false;
	for(int c=0; c<m_NCtrls; c++)
	{
		if(qAbs(m_pWPolar->m_ControlGain[c])>PRECISION)
		{
			bActive = true;
			break;
		}
	}
	if(!bActive)
	{
		str = "\n      No active control - skipping control derivatives\n\n\n";
		traceLog(str);
		return;
	}

	// Define the stability axes and the freestream velocity field
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	V0.Set(u0*cosa, 0.0, u0*sina);
	WindDirection.Set(cosa, 0.0, sina);
	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);

	DeltaAngle = 0.001;

	pos = 0;
	Xde = Yde =  Zde = Lde = Mde = Nde= 0.0;

	NCtrls = 0;

	if(!m_pPlane->isWing())
	{
		//Wing tilt
		if(qAbs(m_pWPolar->m_ControlGain[0])>PRECISION)
		{
			//rotate the normals and control point positions
			H.Set(0.0, 1.0, 0.0);
			if(qAbs(m_pWPolar->m_ControlGain[0])>PRECISION)
				SignedDeltaAngle = DeltaAngle * m_pWPolar->m_ControlGain[0]/qAbs(m_pWPolar->m_ControlGain[0]);
			else SignedDeltaAngle = DeltaAngle;

			Quat.Set(SignedDeltaAngle*180.0/PI, H);

			for(p=0; p<m_pWingList[0]->m_MatSize; p++)
			{
				(m_pWingList[0]->m_pWingPanel+p)->RotateBC(m_pPlane->WingLE(0), Quat);
			}
		}

		pos = m_pWingList[0]->m_MatSize;
		NCtrls = 1;
	}

	if(m_pPlane && m_pWingList[2])
	{
		//Elevator tilt
		if (qAbs(m_pWPolar->m_ControlGain[1])>PRECISION)
		{
			H.Set(0.0, 1.0, 0.0);

			if(qAbs(m_pWPolar->m_ControlGain[1])>PRECISION)
			   SignedDeltaAngle = DeltaAngle * m_pWPolar->m_ControlGain[1]/qAbs(m_pWPolar->m_ControlGain[1]);
			else SignedDeltaAngle = DeltaAngle;

			Quat.Set(SignedDeltaAngle*180.0/PI, H);

			for(p=0; p<m_pWingList[2]->m_MatSize; p++)
			{
				(m_pWingList[2]->m_pWingPanel+p)->RotateBC(m_pPlane->WingLE(2), Quat);
			}
		}
		pos += m_pWingList[2]->m_MatSize;
		NCtrls = 2;
	}

	//flap tilt
	// TODO : exclude fin flaps, or add them to StabPolarDlg
	for (j=0; j<m_ppSurface->size(); j++)
	{
		if(m_ppSurface->at(j)->m_bTEFlap)
		{
			if (qAbs(m_pWPolar->m_ControlGain[NCtrls])>PRECISION)
			{
				//Add delta rotations to initial control setting and to wing or flap delta rotation
				if(qAbs(m_pWPolar->m_ControlGain[NCtrls])>PRECISION)
				   SignedDeltaAngle = DeltaAngle * m_pWPolar->m_ControlGain[NCtrls]/qAbs(m_pWPolar->m_ControlGain[NCtrls]);
				else SignedDeltaAngle = DeltaAngle;

				Quat.Set(SignedDeltaAngle*180.0/PI, m_ppSurface->at(j)->m_HingeVector);
				for(p=0; p<m_MatSize;p++)
				{
					if(m_ppSurface->at(j)->IsFlapPanel(p))
					{
						m_pPanel[p].RotateBC(m_ppSurface->at(j)->m_HingePoint, Quat);
					}
				}
			}
			NCtrls++;
		}
	}

	//create the RHS
	CreateRHS(m_cRHS, V0);

	if(!m_pWPolar->bThinSurfaces())
	{
		CreateWakeContribution(m_uWake,  WindDirection);// re-use m_uWake memory, which is re-calculated anyway at the next control iteration
		for(p=0; p<m_MatSize; p++)	m_cRHS[p]+= m_uWake[p]*u0;
	}

	//Solve the system
	for (int p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = V0.x;
		m_RHS[51*m_MatSize+p] = V0.y;
		m_RHS[52*m_MatSize+p] = V0.z;
	}

	QString strong = "      Calculating the control derivatives\n\n";
	traceLog(strong);

	Crout_LU_with_Pivoting_Solve(m_aij, m_cRHS, m_Index, m_RHS, m_MatSize, &s_bCancel);
	memcpy(m_cRHS, m_RHS, m_MatSize*sizeof(double));

	Forces(m_cRHS, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force, Moment);

	// make the forward difference with nominal results
	// which gives the stability derivative for a rotation of control ic
	Xde = (Force-Force0).dot(is)/DeltaAngle;
	Yde = (Force-Force0).dot(js)/DeltaAngle;
	Zde = (Force-Force0).dot(ks)/DeltaAngle;
	Lde = (Moment - Moment0).dot(is) /DeltaAngle;  // N.m/rad
	Mde = (Moment - Moment0).dot(js) /DeltaAngle;
	Nde = (Moment - Moment0).dot(ks) /DeltaAngle;
}


/**
* Returns the geometric pitching moment coefficient for the specified angle of attack
* The effect of the viscous drag is not included.
*@param Alpha the aoa for which Cm is calculated
*/
double PanelAnalysis::ComputeCm(double Alpha)
{
	static int p;
	static double Cm, cosa, sina, Gamma, Gammap1;
	static CVector VInf, Force, PanelLeverArm, ForcePt, PanelForce, WindDirection, VLocal;
	double Speed2, Cp;
	// Define the wind axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);
	WindDirection.Set( cosa, 0.0, sina);
	VInf.Set(cosa, 0.0, sina);

	Cm = 0.0;
	for(p=0; p<m_MatSize; p++)
	{
		//write vector operations in-line, more efficient
		if(m_pPanel[p].m_Pos!=MIDSURFACE)
		{
			//first calculate Cp for this angle
			VLocal  = m_pPanel[p].GlobalToLocal(VInf);
			VLocal += m_uVl[p]*cosa + m_wVl[p]*sina;
			Speed2 = VLocal.x*VLocal.x + VLocal.y*VLocal.y;
			Cp  = 1.0-Speed2; // QInf=unit, /1.0/1.0;
			m_Cp[p] = Cp; //todo : remove, for information only

			//next calculate the force acting on the panel
			ForcePt = m_pPanel[p].CollPt;
			PanelForce = m_pPanel[p].Normal * (-Cp) * m_pPanel[p].Area;      // Newtons/q
		}
		else
		{
			// for each panel along the chord, add the lift coef
			Gamma   = m_uRHS[p]  *cosa + m_wRHS[p]  *sina;
			Gammap1 = m_uRHS[p+1]*cosa + m_wRHS[p+1]*sina;
			ForcePt = m_pPanel[p].VortexPos;
			PanelForce  = WindDirection * m_pPanel[p].Vortex;
			PanelForce *= 2.0 * Gamma;                                       //Newtons/q   (QInf = unit)
			if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
			{
				Force       = WindDirection * m_pPanel[p].Vortex;
				Force      *= 2.0 * Gammap1;       //Newtons/q/QInf
				PanelForce -= Force;
			}
			m_Cp[p] = PanelForce.dot(m_pPanel[p].Normal)/m_pPanel[p].Area; //todo : remove, for information only
		}
		PanelLeverArm.x = ForcePt.x - m_pWPolar->m_CoG.x;
		PanelLeverArm.y = ForcePt.y - m_pWPolar->m_CoG.y;
		PanelLeverArm.z = ForcePt.z - m_pWPolar->m_CoG.z;
		Cm += -PanelLeverArm.x * PanelForce.z + PanelLeverArm.z*PanelForce.x; //N.m/rho
	}

	Cm *= m_pWPolar->m_Density;
	return Cm;
}


/**
* QUAD VORTEX FORMULATION
*
* LA, LB, TA, TB are the vortex's four corners
* LA and LB are at the 3/4 point of panel nx
* TA and TB are at the 3/4 point of panel nx+1
*
*    LA__________LB               |
*    |           |                |
*    |           |                | freestream speed
*    |           |                |
*    |           |                \/
*    |           |
*    TA__________TB
*
*
* Returns the velocity greated by a ring vortex with unit circulation at a distant point
*
* Notes :
*  - The geometry has been rotated by the sideslip angle, hence, there is no need to align the trailing vortices with sideslip
*  - Vectorial operations are written inline to save computing times -->longer code, but 4x more efficient....
*
* @param LA the leading left point of the quad vortex
* @param LB the leading right point of the quad vortex
* @param TA the trailing left point of the quad vortex
* @param TB the trailing right point of the quad vortex
* @param C the point where the velocity is calculated
* @param V the resulting velocity vector at point C
*/
void PanelAnalysis::VLMQmn(CVector &LA, CVector &LB, CVector &TA, CVector &TB, CVector const &C, CVector &V)
{
	//
	// C is the point where the induced speed is calculated
	// V is the resulting speed
	//
	// Vectorial operations are written explicitly to save computing times (4x more efficient)
	//

	//we use a default core size, unless the user has specified one
	double CoreSize = 0.0001;
	if(qAbs(Panel::coreSize())>PRECISION) CoreSize = Panel::coreSize();


	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	m_pR[0] = &LB;
	m_pR[1] = &TB;
	m_pR[2] = &TA;
	m_pR[3] = &LA;
	m_pR[4] = &LB;


	for (int i=0; i<4; i++)
	{
		r0.x = m_pR[i+1]->x - m_pR[i]->x;
		r0.y = m_pR[i+1]->y - m_pR[i]->y;
		r0.z = m_pR[i+1]->z - m_pR[i]->z;
		r1.x = C.x - m_pR[i]->x;
		r1.y = C.y - m_pR[i]->y;
		r1.z = C.z - m_pR[i]->z;
		r2.x = C.x - m_pR[i+1]->x;
		r2.y = C.y - m_pR[i+1]->y;
		r2.z = C.z - m_pR[i+1]->z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		r1v = sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z));
		r2v = sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/r1v - (r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/r2v;
			V.x += Psi.x * Omega/4.0/PI;
			V.y += Psi.y * Omega/4.0/PI;
			V.z += Psi.z * Omega/4.0/PI;
		}
	}
}


/**
* HORSESHOE VORTEX FORMULATION/
*
*    LA__________LB               |
*    |           |                |
*    |           |                | freestream speed
*    |           |                |
*    |           |                \/
*    |           |
*    \/          \/
*
*
* Returns the velocity greated by a horseshoe vortex with unit circulation at a distant point
*
* Notes :
*  - The geometry has been rotated by the sideslip angle, hence, there is no need to align the trailing vortices with sideslip
*  - Vectorial operations are written inline to save computing times -->longer code, but 4x more efficient....
*
* @param A the left point of the bound vortex
* @param B the right point of the bound vortex
* @param C the point where the velocity is calculated
* @param V the resulting velocity vector at point C
* @param bAll true if the influence of the bound vortex should be evaluated; false for a distant point in the far field.
*/
void PanelAnalysis::VLMCmn(CVector const &A, CVector const &B, CVector const &C, CVector &V, bool const &bAll)
{
	//we use a default core size, unless the user has specified one
	double CoreSize = 0.0001;
	if(qAbs(Panel::coreSize())>PRECISION) CoreSize = Panel::coreSize();

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	if(bAll)
	{
		r0.x = B.x - A.x;
		r0.y = B.y - A.y;
		r0.z = B.z - A.z;

		r1.x = C.x - A.x;
		r1.y = C.y - A.y;
		r1.z = C.z - A.z;

		r2.x = C.x - B.x;
		r2.y = C.y - B.y;
		r2.z = C.z - B.z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) >CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				  -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

			V.x = Psi.x * Omega/4.0/PI;
			V.y = Psi.y * Omega/4.0/PI;
			V.z = Psi.z * Omega/4.0/PI;
		}
	}
	// We create Far points to align the trailing vortices with the reference axis
	// The trailing vortex legs are not aligned with the free-stream, i.a.w. the small angle approximation
	// If this approximation is not valid, then the geometry should be tilted in the polar definition

	// calculate left contribution
	Far.x = A.x +  1.0e10;
	Far.y = A.y;
	Far.z = A.z;// + (Far.x-A.x) * tan(m_Alpha*PI/180.0);

	r0.x = A.x - Far.x;
	r0.y = A.y - Far.y;
	r0.z = A.z - Far.z;

	r1.x = C.x - A.x;
	r1.y = C.y - A.y;
	r1.z = C.z - A.z;

	r2.x = C.x - Far.x;
	r2.y = C.y - Far.y;
	r2.z = C.z - Far.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	t.x=1.0; t.y=0.0; t.z=0.0;

	h.x =  r1.y*t.z - r1.z*t.y;
	h.y = -r1.x*t.z + r1.z*t.x;
	h.z =  r1.x*t.y - r1.y*t.x;

	//Next add 'left' semi-infinite contribution
	//eq.6-56

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
			   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}

	// calculate right vortex contribution
	Far.x = B.x +  1.0e10;
	Far.y = B.y ;
	Far.z = B.z;// + (Far.x-B.x) * tan(m_Alpha*PI/180.0);

	r0.x = Far.x - B.x;
	r0.y = Far.y - B.y;
	r0.z = Far.z - B.z;

	r1.x = C.x - Far.x;
	r1.y = C.y - Far.y;
	r1.z = C.z - Far.z;

	r2.x = C.x - B.x;
	r2.y = C.y - B.y;
	r2.z = C.z - B.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	//Last add 'right' semi-infinite contribution
	h.x =  r2.y*t.z - r2.z*t.y;
	h.y = -r2.x*t.z + r2.z*t.x;
	h.z =  r2.x*t.y - r2.y*t.x;

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
			   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}
}


void PanelAnalysis::restorePanels()
{
	if(m_pWPolar && (m_pWPolar->m_WPolarType==STABILITYPOLAR || m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
	{
		//restore the panels and nodes;
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(Panel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(Panel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));
	}
}




/**
* Creates the plane's operating point,
* fills it with the input resulting from the  analysis, and returns a pointer to the PlaneOpp object which has been created.

*@param bPointOut: if true, part of the oppoint viscous properties could not be interpolated
*@param Cp: the array of Cp value for each panel
*@param Gamma: the array of circulation or doublet strengths Gamma for each panel
*@param Sigma: the array of source strengths for a panel analysis
*@return a pointer to the PlaneOpp object which has been created
*/
PlaneOpp* PanelAnalysis::createPlaneOpp(double *Cp, double *Gamma, double *Sigma)
{
	PlaneOpp *pPOpp = NULL;
	WingOpp *pWOpp=NULL;
	int i,l;
	double Cb = 0.0;

	pPOpp = new PlaneOpp(m_pPlane, m_pWPolar, m_MatSize);
	if(!pPOpp) return NULL;

//		pPOpp->m_Color = MainFrame::GetColor(6);

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			pPOpp->AddWingOpp(iw, m_pWingList[iw]->m_MatSize);
			pPOpp->m_pPlaneWOpp[iw]->createWOpp(m_pWingList[iw], m_pWPolar);
		}
	}

	pPOpp->m_bOut  = m_bPointOut;
	pPOpp->m_Alpha = m_Alpha;
	pPOpp->m_QInf  = m_QInf;


	pWOpp = pPOpp->m_pPlaneWOpp[0];


	for (l=0; l<m_pPlane->m_Wing[0].m_NStation; l++)
	{
		if(qAbs(m_pPlane->m_Wing[0].m_BendingMoment[l])>qAbs(Cb))	Cb = m_pPlane->m_Wing[0].m_BendingMoment[l];
	}


	pWOpp->m_MaxBending = (float)Cb;

	if(m_pWPolar->m_AnalysisMethod==VLMMETHOD)
	{
		traceLog("OldVLM polar\n");
	}
	else if(m_pWPolar->m_AnalysisMethod==PANELMETHOD)
	{
		//get the data from the PanelAnalysis dialog, and from the plane object
		pPOpp->m_NPanels             = m_MatSize;

		pPOpp->m_Alpha               = m_OpAlpha;
		pPOpp->m_QInf                = m_QInf;
		pPOpp->m_CL                  = m_CL;
		pPOpp->m_CX                  = m_CX;
		pPOpp->m_CY                  = m_CY;
		pPOpp->m_ICD                 = m_InducedDrag;
		pPOpp->m_VCD                 = m_ViscousDrag;

		pPOpp->m_GCm                 = m_GCm;
		pPOpp->m_VCm                 = m_VCm;
		pPOpp->m_ICm                 = m_ICm;

		pPOpp->m_GRm                 = m_GRm;

		pPOpp->m_GYm                 = m_GYm;
		pPOpp->m_VYm                 = m_VYm;
		pPOpp->m_IYm                 = m_IYm;

		pPOpp->m_CP                  = m_CP;

		if(m_pWPolar->polarType()!=BETAPOLAR) pPOpp->m_Beta = m_pWPolar->m_BetaSpec;
		else                                  pPOpp->m_Beta = m_OpBeta;

		if(m_pWPolar->m_WPolarType==STABILITYPOLAR)
		{
			pPOpp->m_Alpha            = m_AlphaEq;
			pPOpp->m_QInf             = u0;
			pPOpp->m_Ctrl             = m_Ctrl;

			pPOpp->m_Ctrl             = m_Ctrl;
			pWOpp->m_QInf             = u0;
			pWOpp->m_Alpha            = m_AlphaEq;

			for(i=0; i<4; i++)
			{
				pPOpp->m_EigenValue[i]   = m_rLong[i];
				pPOpp->m_EigenValue[i+4] = m_rLat[i];
				for(l=0;l<4; l++)
				{
					pPOpp->m_EigenVector[i][l]   = m_vLong[4*i+l];
					pPOpp->m_EigenVector[i+4][l] = m_vLat[4*i+l];
				}
			}


			for(i=0; i<4; i++)
			{
				pPOpp->m_EigenValue[i]   = m_rLong[i];
				pPOpp->m_EigenValue[i+4] = m_rLat[i];
				for(l=0;l<4; l++)
				{
					pPOpp->m_EigenVector[i][l]   = m_vLong[4*i+l];
					pPOpp->m_EigenVector[i+4][l] = m_vLat[4*i+l];
				}
			}

			pPOpp->m_XNP = XNP;

			pPOpp->CXu =  CXu;
			pPOpp->CZu =  CZu;
			pPOpp->Cmu =  Cmu;

			pPOpp->CXa =  CXa;
			pPOpp->CLa = -CZa;
			pPOpp->Cma =  Cma;

			pPOpp->CXq =  CXq;
			pPOpp->CLq = -CZq;
			pPOpp->Cmq =  Cmq;

			pPOpp->CYb =  CYb;
			pPOpp->Clb =  Clb;
			pPOpp->Cnb =  Cnb;

			pPOpp->CYp =  CYp;
			pPOpp->Clp =  Clp;
			pPOpp->Cnp =  Cnp;

			pPOpp->CYr =  CYr;
			pPOpp->Clr =  Clr;
			pPOpp->Cnr =  Cnr;

			//Only one control derivative for all the controls of the polar
			pPOpp->m_pPlaneWOpp[0]->m_nControls = 1;
			pPOpp->CXe = CXe;
			pPOpp->CYe = CYe;
			pPOpp->CZe = CZe;
			pPOpp->CLe = Cle;
			pPOpp->CMe = Cme;
			pPOpp->CNe = Cne;


			memcpy(pPOpp->m_ALong, m_ALong, 16*sizeof(double));
			memcpy(pPOpp->m_ALat,  m_ALat,  16*sizeof(double));

			memcpy(pPOpp->m_BLong, m_BLong, 4*sizeof(double));
			memcpy(pPOpp->m_BLat,  m_BLat,  4*sizeof(double));

		}
		else
		{
			pPOpp->m_Ctrl = 0.0;
			memset(pPOpp->m_EigenValue, 0, sizeof(pPOpp->m_EigenValue));
			memset(pPOpp->m_EigenVector, 0, sizeof(pPOpp->m_EigenVector));
			memset(pPOpp->m_ALong, 0, 16*sizeof(double));
			memset(pPOpp->m_ALat,  0,  16*sizeof(double));
		}

	}

	for(int iw=0;iw<MAXWINGS; iw++)
	{
		if(pPOpp->m_pPlaneWOpp[iw])
		{
			pPOpp->m_pPlaneWOpp[iw]->m_Alpha = pPOpp->m_Alpha;
			pPOpp->m_pPlaneWOpp[iw]->m_QInf  = pPOpp->m_QInf;


			pPOpp->m_pPlaneWOpp[iw]->m_FlapMoment.clear();
			for (i=0; i<m_pPlane->m_Wing[iw].m_nFlaps; i++)
			{
				pPOpp->m_pPlaneWOpp[iw]->m_FlapMoment.append(m_pPlane->m_Wing[iw].m_FlapMoment.at(i));
			}
		}
	}

	memcpy(pPOpp->m_dCp,    Cp,    pPOpp->m_NPanels*sizeof(double));
	memcpy(pPOpp->m_dG,     Gamma, pPOpp->m_NPanels*sizeof(double));
	memcpy(pPOpp->m_dSigma, Sigma, pPOpp->m_NPanels*sizeof(double));

	int pos = 0;
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			if(pPOpp->m_pPlaneWOpp[iw])
			{
				pPOpp->m_pPlaneWOpp[iw]->m_dCp    = pPOpp->m_dCp    + pos;
				pPOpp->m_pPlaneWOpp[iw]->m_dG     = pPOpp->m_dG     + pos;
				pPOpp->m_pPlaneWOpp[iw]->m_dSigma = pPOpp->m_dSigma + pos;
			}
			pos +=m_pWingList[iw]->m_MatSize;
		}
	}

	//add the data to the polar object
	if(PlaneOpp::s_bKeepOutOpps || !pPOpp->isOut())
		m_pWPolar->AddPlaneOpPoint(pPOpp);

	return pPOpp;
}



/**
 * Adds the input message to the ouput message. The message is read and cleared from the calling dialog class.
 * @param str the message to output
*/
void PanelAnalysis::traceLog(QString str)
{
	m_OutMessage += str;
	qApp->processEvents();
}



