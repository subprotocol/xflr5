/****************************************************************************

    OpPoint Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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

#include "../globals.h"
#include "../xdirect/analysis/XFoil.h"
#include "OpPoint.h"

QList <void *> OpPoint::s_oaOpp;
OpPoint *OpPoint::s_pCurOpp=NULL;
bool OpPoint::s_bStoreOpp = true;



/**
 * The public constructor
 */
OpPoint::OpPoint()
{
	m_bViscResults = false;//not a  viscous point a priori
	m_bBL          = false;// no boundary layer surface either
	m_bTEFlap      = false;
	m_bLEFlap      = false;

	Xtr1   = 0.0;
	Xtr2   = 0.0;
	XForce = 0.0;
	YForce = 0.0;
	Cpmn   = 0.0;
	m_XCP  = 0.0;
	m_LEHMom   = 0.0; m_TEHMom = 0.0;

	memset(Qi,  0, sizeof(Qi));
	memset(Qv,  0, sizeof(Qv));
	memset(Cpi,  0, sizeof(Cpi));
	memset(Cpv,  0, sizeof(Cpv));
//	memset(x,  0, sizeof(x));
//	memset(y,  0, sizeof(y));

	memset(xd1,  0, sizeof(xd1));
	memset(xd2,  0, sizeof(xd2));
	memset(xd3,  0, sizeof(xd3));
	memset(yd1,  0, sizeof(yd1));
	memset(yd2,  0, sizeof(yd2));
	memset(yd3,  0, sizeof(yd3));
	nd1 = 0;
	nd2 = 0;
	nd3 = 0;
//	Format = 1;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_Style = 0;
	m_Width = 1;
	m_Color.setHsv((int)(((double)qrand()/(double)RAND_MAX)*360),
				   (int)(((double)qrand()/(double)RAND_MAX)*155)+100,
				   (int)(((double)qrand()/(double)RAND_MAX)*155)+100,
					255);

}




/**
 * Loads or saves the data of this OpPoint to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool OpPoint::SerializeOppWPA(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	int a, b, k, Format;
	float f,gg;

	if(bIsStoring)
	{
		/** deprecated, we dont't store in .wpa format anymore */
	}
	else
	{

		if(ArchiveFormat>=100002) ar>>Format;
		else Format = 0;
		//read variables
		ReadCString(ar, m_strFoilName);
		ReadCString(ar, m_strPlrName);

		ar >> f; Reynolds =f;
		ar >> f; Mach = f;
		ar >> f; Alpha = f;
		ar >> n >> nd1 >> nd2 >> nd3;
		ar >> a >> b;
		if(a) m_bViscResults = true; else m_bViscResults = false;
		if(a!=0 && a!=1) return false;

		if(b) m_bBL = true; else m_bBL = false;
		if(b!=0 && b!=1) return false;

		ar >> f; Cl = f;
		ar >> f; Cm = f;
		ar >> f; Cd = f;
		ar >> f; Cdp = f;
		ar >> f; Xtr1 = f;
		ar >> f; Xtr2 = f;
		ar >> f; ACrit =f;
		ar >> f; m_TEHMom = f;
		ar >> f; Cpmn = f;
		for (k=0; k<n; k++)	{
			ar >> f; Cpv[k] = f;
			ar >> f; Cpi[k] = f;
		}

//			if (Format ==2) {
		for (k=0; k<n; k++)
		{
			if(Format<=100002)	ar >> f; //s[k]  = f;
			ar >> f; Qv[k] = f;
			ar >> f; Qi[k] = f;
		}
//			}
		for (k=0; k<=nd1; k++)
		{
			ar >> f >> gg;
            xd1[k] = f;
			yd1[k] = gg;
        }
		for (k=0; k<nd2; k++)
		{
			ar >> f >> gg;
            xd2[k] = f;
			yd2[k] = gg;
        }
		for (k=0; k<nd3; k++)
		{
			ar >> f >> gg;
            xd3[k] = f;
			yd3[k] = gg;
        }
		if(ArchiveFormat>=100002)
		{
			ar>>m_Style>>m_Width;
			ReadCOLORREF(ar, m_Color);

			ar >> a ;
			if(a!=0 && a!=1) return false;
			if(a) m_bIsVisible = true; else m_bIsVisible = false;

			ar >> a ;
			if(a!=0 && a!=1) return false;

			if(a) m_bShowPoints = true; else m_bShowPoints = false;
        }
	}
	return true;
}


/**
 * Loads or saves the data of this OpPoint to a binary file
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool OpPoint::SerializeOppXFL(QDataStream &ar, bool bIsStoring, int ArchiveFormat)
{
	int k;
	float f0,f1;
	double dble;

	if(bIsStoring)
	{
		ar << 200003;
		//200003 : first xfl format
		//write variables
		ar << m_strFoilName;
		ar << m_strPlrName;

		ar << m_Style << m_Width;
		ar << m_Color;
		ar << m_bIsVisible << m_bShowPoints;

		ar << Reynolds << Mach << Alpha;
		ar << n << nd1 << nd2 << nd3;

		ar << m_bViscResults;
		ar << m_bBL;

		ar << Cl << Cm << Cd << Cdp;
		ar << Xtr1 << Xtr2 << m_XCP;
		ar << ACrit << m_TEHMom << Cpmn;

		for (k=0; k<n; k++)	     ar << (float)Cpv[k] << (float)Cpi[k];
		for (k=0; k<n; k++)	     ar << (float)Qv[k]  << (float)Qi[k];
		for (k=0; k<=nd1; k++)   ar << (float)xd1[k] << (float)yd1[k];
		for (k=0; k<nd2; k++)	ar << (float)xd2[k] << (float)yd2[k];
		for (k=0; k<nd3; k++)	ar << (float)xd3[k] << (float)yd3[k];

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<20; i++) ar << i;
		for (int i=0; i<50; i++) ar << (double)i;
	}
	else
	{

		ar >> ArchiveFormat;
		//write variables
		ar >> m_strFoilName;
		ar >> m_strPlrName;

		ar >> m_Style >> m_Width;
		ar >> m_Color;
		ar >> m_bIsVisible >> m_bShowPoints;

		ar >> Reynolds >> Mach >> Alpha;
		ar >> n >> nd1 >> nd2 >> nd3;

		ar >> m_bViscResults;
		ar >> m_bBL;

		ar >> Cl >> Cm >> Cd >> Cdp;
		ar >> Xtr1 >> Xtr2 >> m_XCP;
		ar >> ACrit >> m_TEHMom >> Cpmn;

		for (k=0; k<n; k++)
		{
			ar >> f0 >> f1;
			Cpv[k] = f0;
			Cpi[k] = f1;
		}
		for (k=0; k<n; k++)
		{
			ar >> f0 >> f1;
			Qv[k] = f0;
			Qi[k] = f1;
		}
		for (k=0; k<=nd1; k++)
		{
			ar >> f0 >> f1;
			xd1[k] = f0;
			yd1[k] = f1;
		}
		for (k=0; k<nd2; k++)
		{
			ar >> f0 >> f1;
			xd2[k] = f0;
			yd2[k] = f1;
		}
		for (k=0; k<nd3; k++)
		{
			ar >> f0 >> f1;
			xd3[k] = f0;
			yd3[k] = f1;
		}

		// space allocation
		for (int i=0; i<20; i++) ar >> k;
		for (int i=0; i<50; i++) ar >> dble;
	}
	return true;
}



/**
 * Exports the data of the polar to a text file
 * @param out the instance of output QtextStream
 * @param Version the version name of the program
 * @param FileType TXT if the data is separated by spaces, CSV for a comma separator
 * @param bDataOnly true if the analysis parameters should not be output
 */

void OpPoint::ExportOpp(QTextStream &out, QString Version, enumTextFileType FileType, bool bDataOnly)
{
	int k;
	QString strong;
	Foil *pFoil = Foil::curFoil();

	if(!bDataOnly)
	{
		out << Version+"\n";

		strong = m_strFoilName + "\n";
		out<< strong;
		strong = m_strPlrName + "\n";
		out<< strong;
		if(FileType==TXT) strong=QString("Alpha = %1,  Re = %2,  Ma = %3,  ACrit =%4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		else              strong=QString("Alpha =, %1,  Re =, %2,  Ma =, %3,  ACrit =, %4 \n\n")
									   .arg(Alpha,5,'f',1).arg(Reynolds,8,'f',0).arg(Mach,6,'f',4).arg(ACrit,4,'f',1);
		out<< strong;
	}

	if(FileType==TXT) out << "   x        Cpi      Cpv        Qi        Qv\n";
	else            out << "x,Cpi,Cpv,Qi,Qv\n";

	for (k=0; k<n; k++)
	{
		if(FileType==CSV) strong=QString("%1  %2   %3   %4   %5\n")
									   .arg(pFoil->x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		else              strong=QString("%1,%2,%3,%4,%5\n")
									   .arg(pFoil->x[k],7,'f',4).arg(Cpi[k],7,'f',3).arg(Cpv[k],7,'f',3).arg(Qi[k],7,'f',3).arg(Qv[k],7,'f',3);
		out<< strong;
	}

	out << "\n\n";
}


/**
 * Returns a QString object holding the description and value of the OpPoint's parameters
 * @param &OpPointProperties the reference of the QString object to be filled with the description
 * @param bData true if the analysis data should be appended to the string
 */
void OpPoint::GetOppProperties(QString &OpPointProperties, bool bData)
{
	QString strong;
	OpPointProperties.clear();

	strong  = QString(QObject::tr("Re")+"    = %1 ").arg(Reynolds,7,'f',0);
	OpPointProperties += strong +"\n";

	strong  = QString(QObject::tr("Alpha")+" = %1").arg(Alpha,6,'f',2);
	OpPointProperties += strong +QString::fromUtf8("°")+"\n";

	strong  = QString(QObject::tr("Mach")+"  = %1 ").arg(Mach,7,'f',3);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("NCrit")+" = %1 ").arg(ACrit,5,'f',1);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CL")+"    = %1 ").arg(Cl,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("CD")+"    = %1 ").arg(Cd,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cm")+"    = %1 ").arg(Cm,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cdp")+"   = %1 ").arg(Cdp,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Cpmn")+"  = %1 ").arg(Cpmn,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("XCP")+"   = %1 ").arg(m_XCP,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Top Transition")+" = %1 ").arg(Xtr1,9,'f',5);
	OpPointProperties += strong + "\n";

	strong  = QString(QObject::tr("Bot Transition")+" = %1 ").arg(Xtr2,9,'f',5);
	OpPointProperties += strong + "\n";

	if(m_bTEFlap)
	{
		strong  = QString(QObject::tr("T.E. Flap moment")+" = %1 ").arg(m_TEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}
	if(m_bLEFlap)
	{
		strong  = QString(QObject::tr("L.E. Flap moment")+" = %1 ").arg(m_LEHMom,9,'f',5);
		OpPointProperties += strong + "\n";
	}

	if(!bData) return;
	QTextStream out;
	strong.clear();
	out.setString(&strong);
	ExportOpp(out, "", TXT, true);
	OpPointProperties += "\n"+strong;
}




void OpPoint::deleteOpp(OpPoint *pOpp)
{
	OpPoint* pOldOpp;

	if(!pOpp) return;
	if(pOpp == curOpp()) setCurOpp(NULL);

	for (int iOpp=0; iOpp<s_oaOpp.size(); iOpp++)
	{
		pOldOpp =(OpPoint*)s_oaOpp.at(iOpp);
		if (pOpp == pOldOpp)
		{
			s_oaOpp.removeAt(iOpp);
			delete pOpp;
			break;
		}
	}
}





/**
* Adds the results of the XFoil Calculation to the OpPoint object
* @param pOpPoint a pointer to the instance of the OpPoint to be filled with the data from the XFoil object.
*/
void OpPoint::addXFoilData(void *pXFoilPtr)
{
	int i, j, ibl, is, k;
	XFoil *pXFoil = (XFoil*)pXFoilPtr;

	n        = pXFoil->n;
	Cd       = pXFoil->cd;
	Cdp      = pXFoil->cdp;
	Cl       = pXFoil->cl;
	m_XCP    = pXFoil->xcp;
	Cm       = pXFoil->cm;
	Reynolds = pXFoil->reinf;
	Mach     = pXFoil->minf;
	ACrit    = pXFoil->acrit;
	m_bTEFlap    = Foil::curFoil()->m_bTEFlap;
	m_bLEFlap    = Foil::curFoil()->m_bLEFlap;

	Cpmn   = pXFoil->cpmn;

	for (k=0; k<pXFoil->n; k++)
	{
//		x[k]   = m_pXFoil->x[k+1];
//		y[k]   = m_pXFoil->y[k+1];
//		s[k]   = m_pXFoil->s[k+1];
		Cpi[k] = pXFoil->cpi[k+1];
		Qi[k]  = pXFoil->qgamm[k+1];
	}

	if(pXFoil->lvisc && pXFoil->lvconv)
	{
		Xtr1 =pXFoil->xoctr[1];
		Xtr2 =pXFoil->xoctr[2];
		m_bViscResults = true;
		m_bBL = true;
		for (k=0; k<pXFoil->n; k++)
		{
			Cpv[k] = pXFoil->cpv[k+1];
			Qv[k] = pXFoil->qvis[k+1];
		}
	}
	else
	{
		m_bViscResults = false;
	}

	if(m_bTEFlap || m_bLEFlap)
	{
		setHingeMoments();
/*		m_TEHMom = m_pXFoil->hmom;
		XForce   = m_pXFoil->hfx;
		YForce   = m_pXFoil->hfy;*/
	}

	if(!pXFoil->lvisc || !pXFoil->lvconv)	return;

//---- add boundary layer on both sides of airfoil
	nd1=0;
	nd2=0;
	nd3=0;
	for (is=1; is<=2; is++)
	{
		for ( ibl=2; ibl<=pXFoil->iblte[is];ibl++)
		{
			i = pXFoil->ipan[ibl][is];
			xd1[i] = pXFoil->x[i] + pXFoil->nx[i]*pXFoil->dstr[ibl][is];
			yd1[i] = pXFoil->y[i] + pXFoil->ny[i]*pXFoil->dstr[ibl][is];
			nd1++;
		}
	}

//---- set upper and lower wake dstar fractions based on first wake point
	is=2;
	double dstrte = pXFoil->dstr[pXFoil->iblte[is]+1][is];
	double dsf1, dsf2;
	if(dstrte!=0.0)
	{
		dsf1 = (pXFoil->dstr[pXFoil->iblte[1]][1] + 0.5*pXFoil->ante) / dstrte;
		dsf2 = (pXFoil->dstr[pXFoil->iblte[2]][2] + 0.5*pXFoil->ante) / dstrte;
	}
	else
	{
		dsf1 = 0.5;
		dsf2 = 0.5;
	}

//---- plot upper wake displacement surface
	ibl = pXFoil->iblte[1];
	i = pXFoil->ipan[ibl][1];
	xd2[0] = pXFoil->x[i] + pXFoil->nx[i]*pXFoil->dstr[ibl][1];
	yd2[0] = pXFoil->y[i] + pXFoil->ny[i]*pXFoil->dstr[ibl][1];
	nd2++;

	j= pXFoil->ipan[pXFoil->iblte[is]+1][is]  -1;
	for (ibl=pXFoil->iblte[is]+1; ibl<=pXFoil->nbl[is]; ibl++)
	{
		i = pXFoil->ipan[ibl][is];
		xd2[i-j] = pXFoil->x[i] - pXFoil->nx[i]*pXFoil->dstr[ibl][is]*dsf1;
		yd2[i-j] = pXFoil->y[i] - pXFoil->ny[i]*pXFoil->dstr[ibl][is]*dsf1;
		nd2++;
	}

//---- plot lower wake displacement surface
	ibl = pXFoil->iblte[2];
	i = pXFoil->ipan[ibl][2];
	xd3[0] = pXFoil->x[i] + pXFoil->nx[i]*pXFoil->dstr[ibl][2];
	yd3[0] = pXFoil->y[i] + pXFoil->ny[i]*pXFoil->dstr[ibl][2];
	nd3++;

	j = pXFoil->ipan[pXFoil->iblte[is]+1][is]  -1;
	for (ibl=pXFoil->iblte[is]+1; ibl<=pXFoil->nbl[is]; ibl++)
	{
		i = pXFoil->ipan[ibl][is];
		xd3[i-j] = pXFoil->x[i] + pXFoil->nx[i]*pXFoil->dstr[ibl][is]*dsf2;
		yd3[i-j] = pXFoil->y[i] + pXFoil->ny[i]*pXFoil->dstr[ibl][is]*dsf2;
		nd3++;
	}

}



/**
 * Calculates the moments acting on the flap hinges
 * @param pOpPoint
 */
void OpPoint::setHingeMoments()
{
//	bool bFound;
	int i;
	double hmom, hfx, hfy;
	double dx, dy, xmid, ymid, pmid;
	double xof, yof;
	double ymin, ymax;
	xof = Foil::curFoil()->m_TEXHinge/100.0;
	ymin = Foil::curFoil()->GetBaseLowerY(xof);
	ymax = Foil::curFoil()->GetBaseUpperY(xof);
	yof = ymin + (ymax-ymin) * Foil::curFoil()->m_TEYHinge/100.0;

	if(Foil::curFoil()->m_bTEFlap)
	{
		hmom = 0.0;
		hfx  = 0.0;
		hfy  = 0.0;

		//---- integrate pressures on top and bottom sides of flap
		for (i=0;i<Foil::curFoil()->n-1;i++)
		{
			if (Foil::curFoil()->x[i]>xof &&	Foil::curFoil()->x[i+1]>xof)
			{
				dx = Foil::curFoil()->x[i+1] - Foil::curFoil()->x[i];
				dy = Foil::curFoil()->y[i+1] - Foil::curFoil()->y[i];
				xmid = 0.5*(Foil::curFoil()->x[i+1]+Foil::curFoil()->x[i]) - xof;
				ymid = 0.5*(Foil::curFoil()->y[i+1]+Foil::curFoil()->y[i]) - yof;

				if(m_bViscResults) pmid = 0.5*(Cpv[i+1] + Cpv[i]);
				else               pmid = 0.5*(Cpi[i+1] + Cpi[i]);

				hmom += pmid * (xmid*dx + ymid*dy);
				hfx  -= pmid * dy;
				hfy  += pmid * dx;
			}
		}
		//Next add top chunk left out in the previous loop
/*		bFound = false;
		for (i=0;i<MainFrame::g_ppCurFoil->n-1;i++){
			if(MainFrame::g_ppCurFoil->x[i]>xof && MainFrame::g_ppCurFoil->x[i+1]<xof){
				bFound =true;
				break;
			}
		}
		if(bFound) {
			dx = topx - x[i-1];
			dy = topy - y[i-1];
			xmid = 0.5*(topx+x[i-1]) - xof;
			ymid = 0.5*(topy+y[i-1]) - yof;
			if(s[i] != s[i-1]) frac = (tops-s[i-1])/(s[i]-s[i-1]);
			else	 frac = 0.0;

			if(lvisc) {
				topp = cpv[i]*frac + cpv[i-1]*(1.0-frac);
				pmid = 0.5*(topp+cpv[i-1]);
			}
			else{
				topp = cpi[i]*frac + cpi[i-1]*(1.0-frac);
				pmid = 0.5*(topp+cpi[i-1]);
			}
			hmom = hmom + pmid*(xmid*dx + ymid*dy);
			hfx  = hfx  - pmid* dy;
			hfy  = hfy  + pmid* dx;
		}*/
		//Then add bottom chunk left out


		//Next add internal hinge to top surface contribution

		//Next add internal hinge to bottom surface contribution

		//store the results
		m_TEHMom = hmom;
		XForce   = hfx;
		YForce   = hfy;
	}
}





OpPoint *OpPoint::getOpp(double Alpha)
{
	OpPoint* pOpPoint;
	Polar *pCurPolar = Polar::curPolar();
	if(!pCurPolar) return NULL;

	for (int i=0; i<s_oaOpp.size(); i++)
	{
		if(!pCurPolar) return NULL;
		pOpPoint = (OpPoint*)s_oaOpp.at(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == Foil::curFoil()->foilName())
		{
			if (pOpPoint->m_strPlrName == pCurPolar->polarName())
			{
				if(pCurPolar->polarType() != FIXEDAOAPOLAR)
				{
					if(qAbs(pOpPoint->Alpha - Alpha) <0.001)
					{
						return pOpPoint;
					}
				}
				else
				{
					if(qAbs(pOpPoint->Reynolds - Alpha) <0.1)
					{
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}





/**
 * Inserts a new OpPoint in the array. The OpPoints are sorted by FoilName first, then by Re number, then by aoa.
 * If an OpPoint already exists with the same combination of (FoilName, Re, aoa), it is overwritten.
 * @param pNewPoint
 */
void OpPoint::insertOpPoint(OpPoint *pNewPoint)
{
	if(!pNewPoint) return;

	OpPoint* pOpPoint;
	Polar *pPolar = Polar::getPolar(pNewPoint->m_strFoilName, pNewPoint->m_strPlrName);

	if(!pPolar)
	{
		delete pNewPoint;
		return;
	}

	// first add the OpPoint to the OpPoint Array for the current FoilName
	for (int i=0; i<s_oaOpp.size(); i++)
	{
		pOpPoint = (OpPoint*)s_oaOpp.at(i);
		if (pNewPoint->m_strFoilName.compare(pOpPoint->m_strFoilName)<0)
		{
			//insert point
			s_oaOpp.insert(i, pNewPoint);
			return;
		}
		else if (pNewPoint->m_strFoilName == pOpPoint->m_strFoilName)
		{
			if (pNewPoint->Reynolds < pOpPoint->Reynolds)
			{
				//insert point
				s_oaOpp.insert(i, pNewPoint);
				return;
			}
			else if (qAbs(pNewPoint->Reynolds-pOpPoint->Reynolds)<1.0)
			{
				if(qAbs(pNewPoint->Alpha - pOpPoint->Alpha)<0.005)
				{
					//replace existing point
					s_oaOpp.removeAt(i);
					delete pOpPoint;
					s_oaOpp.insert(i, pNewPoint);
					return;
				}
				else if (pNewPoint->Alpha > pOpPoint->Alpha)
				{
					//insert point
					s_oaOpp.insert(i, pNewPoint);
					return;
				}
			}
		}
	}

	s_oaOpp.append(pNewPoint);
}



/**
* Creates a new instance of an OpPoint.
* Loads the data from the XFoil object in that OpPoint
* Adds the data to the active Polar object
* @param pPolarPtr a point to the parent Polar object to which the OpPoint should be attached.
* @return a pointer to the OpPoint which has been created, or NULL if it wasn't stored.
*/
OpPoint* OpPoint::addOpPoint(void *pFoilPtr, void *pPolarPtr, void *pXFoilPtr)
{
	if(!pFoilPtr || !pXFoilPtr) return NULL;

	Foil *pFoil = (Foil*)pFoilPtr;

	Polar *pPolar;
	if(!pPolarPtr) pPolar = Polar::curPolar();
	else           pPolar = (Polar*)pPolarPtr;

	XFoil *pXFoil = (XFoil*)pXFoilPtr;

	OpPoint *pNewPoint = new OpPoint();
	if(pNewPoint ==NULL)
	{
		return NULL;
	}
	else
	{
		if(!pXFoil->lvconv)
		{
			delete pNewPoint;
			return NULL;
		}
		pNewPoint->Alpha = pXFoil->alfa * 180/PI;
		pNewPoint->m_strFoilName = pFoil->foilName();
		pNewPoint->m_strPlrName  = pPolar->polarName();

		pNewPoint->addXFoilData(pXFoil);
	}

	if(s_bStoreOpp)
	{
		//insert the OpPoint in the Operating points array
		OpPoint::insertOpPoint(pNewPoint);
	}

	// Now insert OpPoint in the current CPolar object

	if(pXFoil->lvconv && pPolar)
	{
		if(pPolar->polarType()==FIXEDLIFTPOLAR || pPolar->polarType()==RUBBERCHORDPOLAR)
		{
			if(pNewPoint && pNewPoint->Reynolds<1.00e8)
			{
				pPolar->AddOpPointData(pNewPoint);
			}
		}
		else
		{
			pPolar->AddOpPointData(pNewPoint);
		}
	}

	if(!OpPoint::s_bStoreOpp)
	{
		delete pNewPoint;
		pNewPoint = NULL;
	}
	OpPoint::setCurOpp(pNewPoint);

	return pNewPoint;
}












