/****************************************************************************

    CBody Class
	Copyright (C) 2007-2012 Andre Deperrois adeperrois@xflr5.com

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


#include "Body.h"
#include "../globals.h"
#include "../mainframe.h"
#include <math.h>
#include <QMessageBox>
#include <QFileDialog>

void *Body::s_pMainFrame;
double Body::s_XPanelPos[300];


Body::Body()
{
	int i;
	m_BodyName = QObject::tr("Body Name");

	m_BodyColor = QColor(200,228, 216);
	m_BodyStyle = 0;
	m_BodyWidth = 1;


	m_iActiveFrame =  1;
	m_iHighlight   = -1;
	m_LineType     =  BODYSPLINETYPE;

	m_nxPanels = 19;
	m_nhPanels = 11;

	m_pBodyPanel = NULL; 
	m_NElements = m_nxPanels * m_nhPanels * 2;

	m_iRes = 31;

//	m_BodyLEPosition.Set(0.0,0.0,0.0);
	m_CoG.Set(0.0,0.0,0.0);
	m_VolumeMass =  m_TotalMass = 0.0;	    //for inertia calculations
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_PointMass.clear();

	m_Bunch  = 0.0;

	eps = 1.0e-06;

	m_SplineSurface.m_iuDegree = 3;
	m_SplineSurface.m_ivDegree = 3;

#ifdef SAIL7APP
	m_NSideLines = 4;
	m_SplineSurface.m_nvLines = m_NSideLines;
	for(int ifr=0; ifr<4; ifr++)
	{
		m_SplineSurface.m_pFrame.append(new CFrame);
		m_SplineSurface.m_pFrame[ifr]->m_CtrlPoint.clear();
		for(int is=0; is<m_NSideLines; is++)
		{
			m_SplineSurface.m_pFrame[ifr]->m_CtrlPoint.append(CVector(0.0,0.0,0.0));
		}
	}

	m_SplineSurface.m_nvLines = m_NSideLines;
	m_SplineSurface.m_pFrame[0]->SetuPosition(-1.0);
	m_SplineSurface.m_pFrame[1]->SetuPosition( 0.0);
	m_SplineSurface.m_pFrame[2]->SetuPosition( 3.0);
	m_SplineSurface.m_pFrame[3]->SetuPosition( 6.0);

	m_SplineSurface.m_pFrame[0]->m_CtrlPoint[0].Set(-1.0, 0.0, 0.2);
	m_SplineSurface.m_pFrame[0]->m_CtrlPoint[1].Set(-1.0, 0.0, 0.2);
	m_SplineSurface.m_pFrame[0]->m_CtrlPoint[2].Set(-1.0, 0.0, 0.2);
	m_SplineSurface.m_pFrame[0]->m_CtrlPoint[3].Set(-1.0, 0.0, 0.2);

	m_SplineSurface.m_pFrame[1]->m_CtrlPoint[0].Set(0.0, 0.0, -0.0);
	m_SplineSurface.m_pFrame[1]->m_CtrlPoint[1].Set(0.0, 2.0, -0.0);
	m_SplineSurface.m_pFrame[1]->m_CtrlPoint[2].Set(0.0, 0.8, -1.0);
	m_SplineSurface.m_pFrame[1]->m_CtrlPoint[3].Set(0.0, 0.0, -1.5);

	m_SplineSurface.m_pFrame[2]->m_CtrlPoint[0].Set(3.0, 0.0, 0.0);
	m_SplineSurface.m_pFrame[2]->m_CtrlPoint[1].Set(3.0, 1.5, 0.0);
	m_SplineSurface.m_pFrame[2]->m_CtrlPoint[2].Set(3.0, 0.5,-0.5);
	m_SplineSurface.m_pFrame[2]->m_CtrlPoint[3].Set(3.0, 0.0,-1.0);

	m_SplineSurface.m_pFrame[3]->m_CtrlPoint[0].Set(6.0,  0.0, .0);
	m_SplineSurface.m_pFrame[3]->m_CtrlPoint[1].Set(6.0,  0.0, .0);
	m_SplineSurface.m_pFrame[3]->m_CtrlPoint[2].Set(6.0,  0.0, .0);
	m_SplineSurface.m_pFrame[3]->m_CtrlPoint[3].Set(6.0,  0.0, .0);
#endif

#ifdef XFLR5
//	m_NSideLines = 5;
//	m_SplineSurface.m_nvLines = SideLines();
	for(int ifr=0; ifr<7; ifr++)
	{
		m_SplineSurface.m_pFrame.append(new Frame);
		m_SplineSurface.m_pFrame[ifr]->m_CtrlPoint.clear();
		for(int is=0; is<5; is++)
		{
			m_SplineSurface.m_pFrame[ifr]->m_CtrlPoint.append(CVector(0.0,0.0,0.0));
		}
	}

	getFrame(0)->SetuPosition(-0.10);
	getFrame(1)->SetuPosition(-0.0936);
	getFrame(2)->SetuPosition(-0.0067);
	getFrame(3)->SetuPosition( 0.0943);
	getFrame(4)->SetuPosition( 0.242);
	getFrame(5)->SetuPosition( 0.636);
	getFrame(6)->SetuPosition( 0.660);

	getFrame(0)->m_CtrlPoint[0].Set(-0.10, 0.0, -0.0124);
	getFrame(0)->m_CtrlPoint[1].Set(-0.10, 0.0, -0.0124);
	getFrame(0)->m_CtrlPoint[2].Set(-0.10, 0.0, -0.0124);
	getFrame(0)->m_CtrlPoint[3].Set(-0.10, 0.0, -0.0124);
	getFrame(0)->m_CtrlPoint[4].Set(-0.10, 0.0, -0.0124);

	getFrame(1)->m_CtrlPoint[0].Set(-0.0936, 0.000,  0.0035);
	getFrame(1)->m_CtrlPoint[1].Set(-0.0936, 0.011,  0.0003);
	getFrame(1)->m_CtrlPoint[2].Set(-0.0936, 0.013, -0.0136);
	getFrame(1)->m_CtrlPoint[3].Set(-0.0936, 0.011, -0.0257);
	getFrame(1)->m_CtrlPoint[4].Set(-0.0936, 0.000, -0.0266);

	getFrame(2)->m_CtrlPoint[0].Set(-0.0067, 0.000,  0.0378);
	getFrame(2)->m_CtrlPoint[1].Set(-0.0067, 0.028,  0.0406);
	getFrame(2)->m_CtrlPoint[2].Set(-0.0067, 0.037,  0.0016);
	getFrame(2)->m_CtrlPoint[3].Set(-0.0067, 0.034, -0.0408);
	getFrame(2)->m_CtrlPoint[4].Set(-0.0067, 0.000, -0.0445);

	getFrame(3)->m_CtrlPoint[0].Set(0.0943, 0.000,  0.0252);
	getFrame(3)->m_CtrlPoint[1].Set(0.0943, 0.012,  0.0192);
	getFrame(3)->m_CtrlPoint[2].Set(0.0943, 0.018,  0.0012);
	getFrame(3)->m_CtrlPoint[3].Set(0.0943, 0.012, -0.0168);
	getFrame(3)->m_CtrlPoint[4].Set(0.0943, 0.000, -0.0228);

	getFrame(4)->m_CtrlPoint[0].Set(0.242, 0.000,  0.0075);
	getFrame(4)->m_CtrlPoint[1].Set(0.242, 0.006,  0.0090);
	getFrame(4)->m_CtrlPoint[2].Set(0.242, 0.009,  0.0011);
	getFrame(4)->m_CtrlPoint[3].Set(0.242, 0.007, -0.0059);
	getFrame(4)->m_CtrlPoint[4].Set(0.242, 0.000, -0.0051);

	getFrame(5)->m_CtrlPoint[0].Set(0.636, 0.000,  0.0138);
	getFrame(5)->m_CtrlPoint[1].Set(0.636, 0.010,  0.0132);
	getFrame(5)->m_CtrlPoint[2].Set(0.636, 0.012, -0.0001);
	getFrame(5)->m_CtrlPoint[3].Set(0.636, 0.010, -0.0098);
	getFrame(5)->m_CtrlPoint[4].Set(0.636, 0.000, -0.0106);

	getFrame(6)->m_CtrlPoint[0].Set(0.660, 0.00,  0.0);
	getFrame(6)->m_CtrlPoint[1].Set(0.660, 0.00,  0.0);
	getFrame(6)->m_CtrlPoint[2].Set(0.660, 0.00,  0.0);
	getFrame(6)->m_CtrlPoint[3].Set(0.660, 0.00, -0.0);
	getFrame(6)->m_CtrlPoint[4].Set(0.660, 0.00, -0.0);

#endif

	SetKnots();


//	ComputeCenterLine();

	for(i=0; i<MAXBODYFRAMES;i++) m_xPanels[i] = 1;
	for(i=0; i<MAXSIDELINES;i++)  m_hPanels[i] = 1;
}




void Body::SetKnots()
{
	m_SplineSurface.SetKnots();
}



void Body::ComputeAero(double *Cp, double &XCP, double &YCP, double &ZCP,
						double &GCm, double &GRm, double &GYm, double &Alpha, CVector &CoG)
{
	int p;
	double cosa, sina, PanelLift;
	CVector PanelForce, LeverArm, WindNormal, WindDirection;
	CVector GeomMoment;

	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);


	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	for (p=0; p<m_NElements; p++)
	{
		PanelForce.x = m_pBodyPanel[p].Normal.x * (-Cp[p]) * m_pBodyPanel[p].Area;
		PanelForce.y = m_pBodyPanel[p].Normal.y * (-Cp[p]) * m_pBodyPanel[p].Area;
		PanelForce.z = m_pBodyPanel[p].Normal.z * (-Cp[p]) * m_pBodyPanel[p].Area; // N/q

		PanelLift = PanelForce.dot(WindNormal);
		XCP   += m_pBodyPanel[p].CollPt.x * PanelLift;
		YCP   += m_pBodyPanel[p].CollPt.y * PanelLift;
        ZCP   += m_pBodyPanel[p].CollPt.z * PanelLift;

		LeverArm.Set(m_pBodyPanel[p].CollPt.x - CoG.x, m_pBodyPanel[p].CollPt.y, m_pBodyPanel[p].CollPt.z-CoG.z);
		GeomMoment = LeverArm * PanelForce; // N.m/q

		GCm  += GeomMoment.y;
		GRm  += GeomMoment.dot(WindDirection);
		GYm  += GeomMoment.dot(WindNormal);
	}
}


void Body::Duplicate(Body *pBody)
{
	if(!pBody) return;

	m_BodyName        = pBody->m_BodyName;
	m_BodyColor       = pBody->m_BodyColor;
	m_nxPanels        = pBody->m_nxPanels;
	m_nhPanels        = pBody->m_nhPanels;
	m_LineType        = pBody->m_LineType;
	m_BodyDescription = pBody->m_BodyDescription;

	m_SplineSurface.ClearFrames();
	for(int i=0; i<pBody->FrameSize(); i++)
	{
		m_SplineSurface.m_pFrame.append(new Frame);
		m_SplineSurface.m_pFrame[i]->CopyFrame(pBody->m_SplineSurface.m_pFrame[i]);
		m_xPanels[i] = pBody->m_xPanels[i];
	}
	SetKnots();

	for(int i=0; i<SideLineCount(); i++)
	{
		m_hPanels[i] = pBody->m_hPanels[i];
	}

//	m_BodyLEPosition = pBody->m_BodyLEPosition;
//	SetKnots();
}


bool Body::ExportDefinition()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i, j;
	QString strong,  FileName;

	FileName = m_BodyName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(pMainFrame, QObject::tr("Export Body Definition"),
											MainFrame::s_LastDirName,
											QObject::tr("Text Format (*.txt)"));
	if(!FileName.length()) return false;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) MainFrame::s_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

	QTextStream outStream(&XFile);

	strong = "\n# This file defines a body geometry\n";
	outStream << strong;
	strong = "# The frames are defined from nose to tail\n";
	outStream << strong;
	strong = "# The numer of sidelines is defined by the number of points of the first frame\n";
	outStream << strong;
	strong = "# Each of the next frames should have the same number of points as the first\n";
	outStream << strong;
	strong = QString("# The maximum number of sidelines is %1\n").arg(MAXSIDELINES);
	outStream << strong;
	strong = QString("# The maximum number of frames is %1\n").arg(MAXBODYFRAMES);
	outStream << strong;
	strong = "# For each frame, the points are defined for the right half of the body, \n";
	outStream << strong;
	strong = "# in the clockwise direction aft looking forward\n\n";
	outStream << strong;

	outStream << (m_BodyName+"\n\n");
	outStream << ("BODYTYPE\n");
	if(m_LineType==BODYPANELTYPE)  outStream << (" 1        # Flat Panels (1) or NURBS (2)\n\n");
	if(m_LineType==BODYSPLINETYPE) outStream << (" 2        # Flat Panels (1) or NURBS (2)\n\n");

	outStream << ("OFFSET\n");
	outStream << ("0.0     0.0     0.0     #Total body offset (Y-coord is ignored)\n\n");

	for(i=0; i<FrameSize(); i++)
	{
		outStream << ("FRAME\n");
		for(j=0;j<SideLineCount(); j++)
		{
			strong = QString("%1     %2    %3\n")
					 .arg(m_SplineSurface.m_pFrame[i]->m_Position.x     * MainFrame::s_mtoUnit,14,'f',7)
					 .arg(m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].y * MainFrame::s_mtoUnit,14,'f',7)
					 .arg(m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].z * MainFrame::s_mtoUnit,14,'f',7);
			outStream << (strong);
		}
		outStream << ("\n");
	}

	return true;

}


void Body::ExportGeometry(QTextStream &outStream, int type, double mtoUnit, int nx, int nh)
{
	QString strong, LengthUnit,str;
	int k,l;
	double u, v;
	CVector Point;


	if(type==1)	str="";
	else		str=", ";

	outStream  << (m_BodyName);
	outStream  << ("\n\n");

	outStream  << (("Right Surface Points\n"));
	if(type==1) strong = "        x("+LengthUnit+")          y("+LengthUnit+")          z("+LengthUnit+")\n";
	else        strong = " x("+LengthUnit+"),"+"y("+LengthUnit+"),"+"z("+LengthUnit+")\n";
	outStream  << (strong);

	for (k=0; k<nx; k++)
	{
		strong = QString(("  Cross Section ")+str+"%1\n").arg(k+1,3);
		outStream  << (strong);

		u = (double)k / (double)(nx-1);

		for (l=0; l<nh; l++)
		{
			v = (double)l / (double)(nh-1);
			GetPoint(u,  v, true, Point);

			strong = QString("   %1"+str+"     %2"+str+"     %3\n")
					 .arg(Point.x * mtoUnit,10,'f',3)
					 .arg(Point.y * mtoUnit,10,'f',3)
					 .arg(Point.z * mtoUnit,10,'f',3);
			outStream  << (strong);
		}
		outStream  << ("\n");
	}

	outStream  << ("\n\n");
}


bool Body::Gauss(double *A, int n, double *B, int m)
{
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n)
	{
		//  find the pivot row
		A_pivot_row = pa;
		max = qAbs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = qAbs(*(pA+row))) > max)
			{
				max = dum;
				A_pivot_row = pA;
				pivot_row = i;
			}
		if (max <= 0.0)
			return false;                // the matrix A is singular

			// and if it differs from the current row, interchange the two rows.

		if (pivot_row != row) {
			for (i = row; i < n; i++) {
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<=m; k++){
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}

		// Perform forward substitution
		for (i = row+1; i<n; i++) {
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++)
				B[i+k*n] += dum * B[row+k*n];
		}
	}

	// Perform backward substitution

	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--) {
		if ( *(pa + row) == 0.0 )
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum;
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++) {
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++)
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}




double Body::Length()
{
	return qAbs(m_SplineSurface.m_pFrame.last()->m_Position.x - m_SplineSurface.m_pFrame.first()->m_Position.x);
}


CVector Body::LeadingPoint()
{
	return CVector(m_SplineSurface.m_pFrame[0]->m_Position.x,
				   0.0,
				   (m_SplineSurface.m_pFrame[0]->m_CtrlPoint.first().z + m_SplineSurface.m_pFrame[0]->m_CtrlPoint.last().z)/2.0 );
}



double Body::GetSectionArcLength(double x)
{
	//NURBS only
	if(m_LineType==BODYPANELTYPE) return 0.0;
	// aproximate arc length, used for inertia estimations
	double length = 0.0;
	double ux = Getu(x);
	CVector Pt, Pt1;
	GetPoint(ux, 0.0, true, Pt1);

	int NPoints = 10;//why not ?
	for(int i=1; i<=NPoints; i++)
	{
		GetPoint(ux, (double)i/(double)NPoints, true, Pt);
		length += sqrt((Pt.y-Pt1.y)*(Pt.y-Pt1.y) + (Pt.z-Pt1.z)*(Pt.z-Pt1.z));
		Pt1.y = Pt.y;
		Pt1.z = Pt.z;
	}
	return length*2.0; //to account for left side.
}


void Body::GetPoint(double u, double v, bool bRight, CVector &Pt)
{
	m_SplineSurface.GetPoint(u, v, Pt);
	if(!bRight)  Pt.y = -Pt.y;
}



double Body::Getu(double x)
{
	return m_SplineSurface.Getu(x,0.0);
}


double Body::Getv(double u, CVector r, bool bRight)
{
	double sine = 10000.0;

	if(u<=0.0)          return 0.0;
	if(u>=1.0)          return 0.0;
	if(r.VAbs()<1.0e-5) return 0.0;

	int iter=0;
	double v, v1, v2;

	r.Normalize();
	v1 = 0.0; v2 = 1.0;

	while(qAbs(sine)>1.0e-4 && iter<200)
	{
		v=(v1+v2)/2.0;
		GetPoint(u, v, bRight, t_R);
		t_R.x = 0.0;
		t_R.Normalize();//t_R is the unit radial vector for u,v

		sine = (r.y*t_R.z - r.z*t_R.y);

		if(bRight)
		{
			if(sine>0.0) v1 = v;
			else         v2 = v;
		}
		else
		{
			if(sine>0.0) v2 = v;
			else         v1 = v;
		}
		iter++;
	}

	return (v1+v2)/2.0;
}



bool Body::ImportDefinition(QTextStream &inStream, double mtoUnit)
{
	int res, i, j, Line, NSideLines;
	QString strong;
	bool bRead, bOK;
	QByteArray textline;
	const char *text;
	double xo,yo,zo;
	xo = yo = zo = 0.0;

	Line = 0;
	bRead  = ReadAVLString(inStream, Line, strong);
	m_BodyName = strong.trimmed();
	m_SplineSurface.ClearFrames();
	//Header data

	bRead = true;
	while (bRead)
	{
		bRead  = ReadAVLString(inStream, Line, strong);
		if(!bRead) break;
		if (strong.indexOf("BODYTYPE") >=0)
		{
			bRead  = ReadAVLString(inStream, Line, strong);
			if(!bRead) break;
			res = strong.toInt(&bOK);

			if(bOK)
			{
				if(res==1) m_LineType = BODYPANELTYPE;
				else       m_LineType = BODYSPLINETYPE;
			}
		}
		else if (strong.indexOf("OFFSET") >=0)
		{
			bRead  = ReadAVLString(inStream, Line, strong);
			if(!bRead) break;

			textline = strong.toLatin1();
			text = textline.constData();
			res = sscanf(text, "%lf  %lf  %lf", &xo, &yo, &zo);

			if(res==3)
			{
				xo /= mtoUnit;
				zo /= mtoUnit;
			}
			//y0 is ignored, body is assumed centered along x-z plane
		}
		else if (strong.indexOf("FRAME", 0)  >=0)
		{
			Frame *pNewFrame = new Frame;
			NSideLines = ReadFrame(inStream, Line, pNewFrame, mtoUnit);

			if (NSideLines)
			{
				m_SplineSurface.InsertFrame(pNewFrame);
			}
		}
	}



	for(i=1; i<FrameSize(); i++)
	{
		if(m_SplineSurface.m_pFrame[i]->m_CtrlPoint.size() != m_SplineSurface.m_pFrame[i-1]->m_CtrlPoint.size())
		{
			QString strong = QObject::tr("Error reading ")+m_BodyName+QObject::tr("\nFrames have different number of side points");
//			QMessageBox::warning(window(), QObject::tr("Error"),strong); //TODO
			return false;
		}
	}

	for(i=0; i<FrameSize(); i++)
	{
		m_SplineSurface.m_pFrame[i]->m_Position.x =  m_SplineSurface.m_pFrame[i]->m_CtrlPoint[0].x + xo;
		for(j=0; j<SideLineCount(); j++)
		{
			m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].z += zo;
		}
	}

	m_SplineSurface.SetKnots();
	return true;
}


void Body::InsertSideLine(int SideLine)
{
	if(SideLine==0) SideLine++;
	for (int i=0; i<FrameSize(); i++)
	{
		m_SplineSurface.m_pFrame[i]->InsertPoint(SideLine);
	}
	SetKnots();
}


int Body::InsertPoint(CVector Real)
{
	//Real is to be inserted in the current frame
	if(m_iActiveFrame<0)
	{
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setWindowTitle(QObject::tr("Warning"));
		msgBox.setText(QObject::tr("Please select a Frame before inserting a point"));
        msgBox.exec();

		return -1;
	}

	int i, n;
	n = (m_SplineSurface.m_pFrame[m_iActiveFrame])->InsertPoint(Real, 3);
	for (i=0; i<FrameSize(); i++)
	{
		if(i!=m_iActiveFrame)
		{
			m_SplineSurface.m_pFrame[i]->InsertPoint(n);
		}
	}
	SetKnots();
	return n;
}


int Body::InsertFrame(CVector Real)
{
	int k, n=0;

	if(Real.x<m_SplineSurface.m_pFrame[0]->m_Position.x)
	{
		m_SplineSurface.m_pFrame.prepend(new Frame(SideLineCount()));
		for (k=0; k<SideLineCount(); k++)
		{
			m_SplineSurface.m_pFrame.first()->m_CtrlPoint[k].Set(0.0,0.0,Real.z);
		}
		m_SplineSurface.m_pFrame.first()->SetuPosition(Real.x);
	}
	else if(Real.x>m_SplineSurface.m_pFrame.last()->m_Position.x)
	{
		m_SplineSurface.m_pFrame.append(new Frame(SideLineCount()));
		
		for (k=0; k<SideLineCount(); k++)
		{
			m_SplineSurface.m_pFrame.last()->m_CtrlPoint[k].Set(0.0,0.0,Real.z);
		}
		m_SplineSurface.m_pFrame.last()->SetuPosition(Real.x);
	}
	else
	{
		for (n=0; n<FrameSize()-1; n++)
		{
			if(m_SplineSurface.m_pFrame[n]->m_Position.x<=Real.x  &&  Real.x<m_SplineSurface.m_pFrame[n+1]->m_Position.x)
			{
				m_SplineSurface.m_pFrame.insert(n+1, new Frame(SideLineCount()));

				for (k=0; k<SideLineCount(); k++)
				{
					m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint[k].x = (m_SplineSurface.m_pFrame[n]->m_CtrlPoint[k].x + m_SplineSurface.m_pFrame[n+2]->m_CtrlPoint[k].x)/2.0;
					m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint[k].y = (m_SplineSurface.m_pFrame[n]->m_CtrlPoint[k].y + m_SplineSurface.m_pFrame[n+2]->m_CtrlPoint[k].y)/2.0;
					m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint[k].z = (m_SplineSurface.m_pFrame[n]->m_CtrlPoint[k].z + m_SplineSurface.m_pFrame[n+2]->m_CtrlPoint[k].z)/2.0;
				}
				break;
			}
		}
		if(n+1<FrameSize())
		{
			m_SplineSurface.m_pFrame[n+1]->SetuPosition(Real.x);
			double trans = Real.z - (m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint[0].z + m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint.last().z)/2.0;
			for (k=0; k<SideLineCount(); k++)
			{
				m_SplineSurface.m_pFrame[n+1]->m_CtrlPoint[k].z += trans;
			}
		}
	}

	m_iActiveFrame = n+1;

	if(n>=FrameSize())	m_iActiveFrame = FrameSize();
	if(n<=0)			m_iActiveFrame = 0;
	m_iHighlight = -1;
	SetKnots();

	return n+1;
}




void Body::InterpolateCurve(CVector *D, CVector *P, double *v, double *knots, int degree, int Size)
{
	int i,j;
	double Nij[MAXBODYFRAMES* MAXBODYFRAMES];//MAXBODYFRAMES is greater than MAXSIDELINES
	double RHS[3 * MAXBODYFRAMES];//x, y and z RHS

	//create the matrix
	for(i=0; i<Size; i++)
	{
		for(j=0; j<Size; j++)
		{
			*(Nij+i*Size + j) = SplineBlend(j, degree, v[i], knots);
		}
	}
	//create the RHS
	for(i=0; i<Size; i++)
	{
		RHS[i]        = D[i].x;
		RHS[i+Size]   = D[i].y;
		RHS[i+Size*2] = D[i].z;
	}
	//solve for the new control point coordinates
	Gauss(Nij, Size, RHS, 3);
	
	//reconstruct the control points
	for(i=0; i<Size; i++)
	{
		P[i].x = RHS[i];
		P[i].y = RHS[i+Size];
		P[i].z = RHS[i+Size*2];
	}
}




bool Body::Intersect(CVector A, CVector B, CVector &I, bool bRight)
{
	if(m_LineType==BODYPANELTYPE)        return IntersectPanels(A,B,I);
	else if (m_LineType==BODYSPLINETYPE) return IntersectNURBS(A,B,I, bRight);
	return false;
}


bool Body::IntersectNURBS(CVector A, CVector B, CVector &I, bool bRight)
{
	//intersect line AB with right or left body surface
	//intersection point is I
	CVector N, tmp, M0, M1;
	double u, v, dist, t, tp;
	int iter = 0;
	int itermax = 20;
	double dmax = 1.0e-5;
	dist = 1000.0;//m

	M0.Set(0.0, A.y, A.z);
	M1.Set(0.0, B.y, B.z);

	if(M0.VAbs()<M1.VAbs())
	{
		tmp = A;		A   = B;		B   = tmp;
	}
	//M0 is the outside Point, M1 is the inside point
	M0 = A; M1 = B; 

	//define which side to intersect with
	if(M0.y>=0.0) bRight = true; else bRight = false;

	if(!IsInNURBSBody(M1))
	{
		//consider no intersection (not quite true in special high dihedral cases)
		I = M1;
		return false;
	}
 
	I = (M0+M1)/2.0; t=0.5;

	while(dist>dmax && iter<itermax)
	{
		//first we get the u parameter corresponding to point I
		tp = t;
		u = Getu(I.x);
		t_Q.Set(I.x, 0.0, 0.0);
		t_r = (I-t_Q);
		v = Getv(u, t_r, bRight);
		GetPoint(u, v, bRight, t_N);

		//project t_N on M0M1 line
		t = - ( (M0.x - t_N.x) * (M1.x-M0.x) + (M0.y - t_N.y) * (M1.y-M0.y) + (M0.z - t_N.z)*(M1.z-M0.z))
			 /( (M1.x -  M0.x) * (M1.x-M0.x) + (M1.y -  M0.y) * (M1.y-M0.y) + (M1.z -  M0.z)*(M1.z-M0.z));

		I.x = M0.x + t * (M1.x-M0.x);
		I.y = M0.y + t * (M1.y-M0.y);
		I.z = M0.z + t * (M1.z-M0.z);

//		dist = sqrt((t_N.x-I.x)*(t_N.x-I.x) + (t_N.y-I.y)*(t_N.y-I.y) + (t_N.z-I.z)*(t_N.z-I.z));
		dist = qAbs(t-tp);
		iter++; 
	}

	return dist<dmax;
}


bool Body::IntersectPanels(CVector A, CVector B, CVector &I)
{
	bool b1, b2, b3, b4, b5;
	int i,k;
	double r,s,t;
	CVector LA, TA, LB, TB, U, V, W, H, D1, D2, N, C, P;
	bool bIntersect = false;

	U = B-A;
	U.Normalize();

	for (i=0; i<FrameSize()-1; i++)
	{
		for (k=0; k<SideLineCount()-1; k++)
		{
			//build the four corner points of the Quad Panel
			LB.x =  m_SplineSurface.m_pFrame[i]->m_Position.x     ;
			LB.y =  m_SplineSurface.m_pFrame[i]->m_CtrlPoint[k].y  ;
			LB.z =  m_SplineSurface.m_pFrame[i]->m_CtrlPoint[k].z  ;

			TB.x =  m_SplineSurface.m_pFrame[i+1]->m_Position.x;
			TB.y =  m_SplineSurface.m_pFrame[i+1]->m_CtrlPoint[k].y;
			TB.z =  m_SplineSurface.m_pFrame[i+1]->m_CtrlPoint[k].z;

			LA.x =  m_SplineSurface.m_pFrame[i]->m_Position.x     ;
			LA.y =  m_SplineSurface.m_pFrame[i]->m_CtrlPoint[k+1].y;
			LA.z =  m_SplineSurface.m_pFrame[i]->m_CtrlPoint[k+1].z;

			TA.x =  m_SplineSurface.m_pFrame[i+1]->m_Position.x;
			TA.y =  m_SplineSurface.m_pFrame[i+1]->m_CtrlPoint[k+1].y;
			TA.z =  m_SplineSurface.m_pFrame[i+1]->m_CtrlPoint[k+1].z;

			//does it intersect the right panel ?
			C = (LA + LB + TA + TB)/4.0;

			D1 = LA - TB;
			D2 = LB - TA;

			N = D2 * D1;
			N.Normalize();
			
			r = (C.x-A.x)*N.x + (C.y-A.y)*N.y + (C.z-A.z)*N.z ;
			s = (U.x*N.x + U.y*N.y + U.z*N.z);
            if(qAbs(s)>0.0)
			{
				t = r/s;
				P = A + U * t;

				// P is inside panel if on left side of each panel side
				W = P  - TA;
				V = TB - TA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b1 = true; else b1 = false;

				W = P  - TB;
				V = LB - TB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b2 = true; else b2 = false;

				W = P  - LB;
				V = LA - LB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b3 = true; else b3 = false;

				W = P  - LA;
				V = TA - LA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b4 = true; else b4 = false;

				W = A-P;
				V = B-P;
				if(W.dot(V)<=0.0)      b5 = true; else b5 = false;

				if(b1 && b2 && b3 && b4 && b5)
				{
					bIntersect = true;
					break;
				}
			}

			//does it intersect the left panel ?

			LB.y = -LB.y;
			LA.y = -LA.y;
			TB.y = -TB.y;
			TA.y = -TA.y;

			C = (LA + LB + TA + TB)/4.0;

			D1 = LA - TB;
			D2 = LB - TA;

			N = D2 * D1;
			N.Normalize();
			
			r = (C.x-A.x)*N.x + (C.y-A.y)*N.y + (C.z-A.z)*N.z ;
			s = (U.x*N.x + U.y*N.y + U.z*N.z);

			if(qAbs(s)>0.0)
			{
				t = r/s;
				P = A + U * t;

				// P is inside panel if on left side of each panel side
				W = P  - TA;
				V = TB - TA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b1 = true; else b1 = false;

				W = P  - TB;
				V = LB - TB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b2 = true; else b2 = false;

				W = P  - LB;
				V = LA - LB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b3 = true; else b3 = false;

				W = P  - LA;
				V = TA - LA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b4 = true; else b4 = false;

				W = A-P;
				V = B-P;
				if(W.dot(V)<=0.0)       b5 = true; else b5 = false;

				if(b1 && b2 && b3 && b4 && b5)
				{
					bIntersect = true;
					break;
				}
			}
		}
		if(bIntersect) break;
	}
	if(bIntersect) I = P;
	return bIntersect;
}



int Body::IsFramePos(CVector Real, double ZoomFactor)
{
	int k;
	for (k=0; k<FrameSize(); k++)
	{
		if (qAbs(Real.x-m_SplineSurface.m_pFrame[k]->m_Position.x) < 0.01 *Length()/ZoomFactor &&
			qAbs(Real.y-m_SplineSurface.m_pFrame[k]->zPos())       < 0.01 *Length()/ZoomFactor)
			return k;
	}
	return -10;
}


bool Body::IsInNURBSBody(CVector Pt)
{
	double u, v;
	bool bRight;

	u = Getu(Pt.x);

    /** @todo check in different cases of fin/body connection */
    if (u <= 0.0 || u >= 1.0) return false;

	t_r.Set(0.0, Pt.y, Pt.z);

    if(Pt.y>=0.0) bRight = true;	else bRight = false;

	v = Getv(u, t_r, bRight);
	GetPoint(u, v, bRight, t_N);

	t_N.x = 0.0;

	if(t_r.VAbs()>t_N.VAbs()) return false;
	return true;
}



int Body::ReadFrame(QTextStream &in, int &Line, Frame *pFrame, double const &Unit)
{
	double x,y,z;

	QString strong;
	int i, res;
	i = 0;
	x=y=z=0.0;

	bool bRead =true;

	pFrame->m_CtrlPoint.clear();

	while (bRead)
	{
		if(!ReadAVLString(in, Line,  strong)) bRead = false;
		ReadValues(strong, res, x,y,z);

		if(res!=3)
		{
			bRead = false;
			Rewind1Line(in, Line, strong);
		}
		else 
		{
			pFrame->m_CtrlPoint.append(CVector(x/Unit, y/Unit, z/Unit));
			i++;
		}
		if(i>=MAXSIDELINES)
		{
			bRead = false;
		}
	}

	if(pFrame->m_CtrlPoint.size()) pFrame->m_Position.x = pFrame->m_CtrlPoint.first().x;
	return i;
}


int Body::RemoveFrame(int n)
{
	m_SplineSurface.m_pFrame.removeAt(n);

	m_iActiveFrame = qMin(n, m_SplineSurface.m_pFrame.size());
	m_iHighlight = -1;
	SetKnots();
	return m_iActiveFrame;
}


void Body::RemoveActiveFrame()
{
	m_SplineSurface.RemoveFrame(m_iActiveFrame);


	m_iHighlight = -1;
	SetKnots();
}


void Body::RemoveSideLine(int SideLine)
{
	for (int i=0; i<m_SplineSurface.m_pFrame.size(); i++)
	{
		m_SplineSurface.m_pFrame[i]->RemovePoint(SideLine);
	}
	SetKnots();
}


void Body::Scale(double XFactor, double YFactor, double ZFactor, bool bFrameOnly, int FrameID)
{
	int i,j;
	for (i=0; i<FrameSize(); i++)
	{
		if((bFrameOnly &&  i==FrameID) || !bFrameOnly)
		{
			if(!bFrameOnly) m_SplineSurface.m_pFrame[i]->m_Position.x *= XFactor;

			for(j=0; j<m_SplineSurface.m_pFrame[i]->m_CtrlPoint.size(); j++)
			{
				m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].x  = m_SplineSurface.m_pFrame[i]->m_Position.x;
				m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].y *= YFactor;
				m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].z *= ZFactor;
			}
		}
	}
//	ComputeCenterLine();
}

bool Body::SerializeBody(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int i,k, nStations;
	float f,g,h;
	double x,y,z;

	if(bIsStoring)
	{
		ar << 1006;

		//1006 : added body LEPosition
		//1005 : added body alpha color + provisions
		//1004 : QFLRv0.03	: added mass properties for inertia calculations
		//1003 : QFLR5 v0.02 : added body description field
		//1002 : Added axial and hoop mesh panel numbers for linetype fuselage
		//1001 : Added bunching parameter
		//1000 : first format
		WriteCString(ar, m_BodyName);
		WriteCString(ar, m_BodyDescription);

		WriteCOLORREF(ar, m_BodyColor);
		if(m_LineType==BODYPANELTYPE) ar << 1;
		else                          ar << 2;
		ar << SideLineCount();
		ar << FrameSize();
		ar << m_iRes;
		ar << 3 << 3;//		ar << m_nxDegree << m_nhDegree;
		ar << m_nxPanels << m_nhPanels;
		ar << (float)m_Bunch;

		for(k=0; k<FrameSize(); k++)  ar << m_xPanels[k];
		for(k=0; k<SideLineCount(); k++) ar << m_hPanels[k];

		ar<<0;//		if(m_bClosedSurface) ar<<1; else ar <<0;

		for(k=0; k<FrameSize(); k++)
		{
			m_SplineSurface.m_pFrame[k]->SerializeFrame(ar, bIsStoring);
		}

		ar << 0;//ar << m_NStations; again ?
		for (k=0; k<FrameSize(); k++)
		{
			ar << (float)FramePosition(k) << 0.0f;// << (float)m_FramePosition[k].z;
		}
		ar << (float)m_VolumeMass;
		ar << m_PointMass.size();
		for(i=0; i<m_PointMass.size(); i++) ar << (float)m_PointMass[i]->mass();
		for(i=0; i<m_PointMass.size(); i++) ar << (float)m_PointMass[i]->position().x << (float)m_PointMass[i]->position().y << (float)m_PointMass[i]->position().z;
		for(i=0; i<m_PointMass.size(); i++)  WriteCString(ar, m_PointMass[i]->tag());

		ar << (float)m_BodyColor.alphaF();
//		ar << m_BodyLEPosition.x<< m_BodyLEPosition.y<< m_BodyLEPosition.z;
		ar << 0.0 << 0.0 << 0.0;
		ar << 0.0f;
	}
	else
	{
		int NSideLines;
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;

		ReadCString(ar, m_BodyName);
		if(ArchiveFormat>=1003) ReadCString(ar, m_BodyDescription);

		ReadCOLORREF(ar, m_BodyColor);
		ar >> k;
		if(k==1) m_LineType = BODYPANELTYPE;
		else     m_LineType = BODYSPLINETYPE;
		ar >> NSideLines;
		ar >> nStations;
		ar >> m_iRes;
		ar >> k >> k; //ar >> m_nxDegree >> m_nhDegree;
		ar >> m_nxPanels >> m_nhPanels;

		if(ArchiveFormat>=1001)
		{
			ar >> f; m_Bunch = f;
		}

		if(ArchiveFormat>=1002)
		{
			for(k=0; k<nStations; k++)  ar >> m_xPanels[k];
			for(k=0; k<NSideLines; k++) ar >> m_hPanels[k];
		}

		ar >> k; // m_bClosedSurface
		if(k!=0 && k!=1) return false;

		m_SplineSurface.ClearFrames();
		for(k=0; k<nStations; k++)
		{
			m_SplineSurface.m_pFrame.append(new Frame);
			m_SplineSurface.m_pFrame[k]->SerializeFrame(ar, bIsStoring);
		}
		//Serialize Bodyline
		ar >>k;//	ar >> m_NStations; again ?

		for (k=0; k<nStations;k++)
		{
			ar >> f; m_SplineSurface.m_pFrame[k]->SetuPosition(f);
			for(int ic=0; ic<m_SplineSurface.m_pFrame[k]->m_CtrlPoint.size(); ic++)
			{
				m_SplineSurface.m_pFrame[k]->m_CtrlPoint[ic].x = f;
			}
			ar >> f; //m_FramePosition[k].z =f;
		}
		if(ArchiveFormat>=1004)
		{
			ar >> f;  m_VolumeMass = f;
			int nMass;
			ar >> nMass;

			QVarLengthArray<double> mass;
			QVarLengthArray<CVector> position;
			QVarLengthArray<QString> tag;

			for(int im=0; im<nMass; im++)
			{
				ar >> f;
				mass.append(f);
			}
			for(int im=0; im<nMass; im++)
			{
				ar >> f >> g >> h;
				position.append(CVector(f,g,h));
			}
			for(int im=0; im<nMass; im++)
			{
				tag.append("");
				ReadCString(ar, tag[im]);
			}

			m_PointMass.clear();
			for(int im=0; im<nMass; im++)
			{
				m_PointMass.append(new PointMass(mass[im], position[im], tag[im]));
			}
		}
		ar >> f;
		if(ArchiveFormat>=1005) m_BodyColor.setAlphaF(f);
		if(ArchiveFormat>=1006)
		{
			ar >> x >> y >> z;
//			m_BodyLEPosition.Set(x,y,z);
		}
//		else m_BodyLEPosition.Set(0.0,0.0,0.0);
		ar >> f;

		SetKnots();
	}
	return true;
}




void Body::SetPanelPos()
{
	int i;
/*	for(i=0; i<=m_nxPanels; i++)
	{
		s_XPanelPos[i] =(double)i/(double)m_nxPanels;
	}
	return;*/
	double y, x;
	double a = (m_Bunch+1.0)*.48 ;
	a = 1./(1.0-a);

	double norm = 1/(1+exp(0.5*a));

	for(i=0; i<=m_nxPanels; i++)
	{
		x = (double)(i)/(double)m_nxPanels;
		y = 1.0/(1.0+exp((0.5-x)*a));
		s_XPanelPos[i] =0.5-((0.5-y)/(0.5-norm))/2.0;
	}
}




void Body::Translate(double XTrans, double YTrans, double ZTrans, bool bFrameOnly, int FrameID)
{
	int i,j;
	for (i=0; i<FrameSize(); i++)
	{
		if((bFrameOnly &&  i==FrameID) || !bFrameOnly)
		{
			m_SplineSurface.m_pFrame[i]->m_Position.x += XTrans;
//			m_SplineSurface.m_pFrame[i]->m_Position.y += YTrans;
//			m_SplineSurface.m_pFrame[i]->m_Position.z += ZTrans;

			for(j=0; j<m_SplineSurface.m_pFrame[i]->m_CtrlPoint.size(); j++)
			{
				m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].x += XTrans;
				m_SplineSurface.m_pFrame[i]->m_CtrlPoint[j].z += ZTrans;
			}
		}
	}
//	ComputeCenterLine();
}


void Body::Translate(CVector T, bool bFrameOnly, int FrameID)
{
	Translate(T.x, T.y, T.z, bFrameOnly, FrameID);
}



Frame *Body::getFrame(int k)
{
	if(k>=0 && k<FrameSize()) return m_SplineSurface.m_pFrame[k];
	return NULL;
}



double Body::FramePosition(int iFrame)
{
	return m_SplineSurface.m_pFrame[iFrame]->m_Position.x;
}



Frame *Body::activeFrame()
{
	if(m_iActiveFrame>=0 && m_iActiveFrame<FrameSize()) return m_SplineSurface.m_pFrame[m_iActiveFrame];
	return NULL;
}


void Body::SetActiveFrame(Frame *pFrame)
{
	for(int ifr=0; ifr<m_SplineSurface.m_pFrame.size(); ifr++)
	{
		if(m_SplineSurface.m_pFrame.at(ifr)==pFrame)
		{
			m_iActiveFrame = ifr;
			break;
		}
	}
}




void Body::ComputeBodyAxisInertia()
{
	// Calculates the inertia tensor in geometrical (body) axis :
	//  - adds the volume inertia AND the point masses of all components
	//  - the body axis is the frame in which all the geometry has been defined
	//  - the origin=BodyFrame;
	//  - the center of gravity is calculated from component masses and is NOT the CoG defined in the polar

	int i;
	CVector LA, VolumeCoG;
	double Ixx, Iyy, Izz, Ixz, VolumeMass;
	Ixx = Iyy = Izz = Ixz = VolumeMass = 0.0;

	ComputeVolumeInertia(VolumeCoG, Ixx, Iyy, Izz, Ixz);
	m_TotalMass = m_VolumeMass;

	m_CoG = VolumeCoG *m_VolumeMass;

	// add point masses
	for(i=0; i<m_PointMass.size(); i++)
	{
		m_TotalMass += m_PointMass[i]->mass();
		m_CoG += m_PointMass[i]->position() * m_PointMass[i]->mass();
	}

	if(m_TotalMass>0) m_CoG = m_CoG/m_TotalMass;
	else              m_CoG.Set(0.0,0.0,0.0);

	// The CoG position is now available, so calculate the inertia w.r.t the CoG
	// using Huyghens theorem
	//LA is the displacement vector from the centre of mass to the new axis
	LA = m_CoG-VolumeCoG;
	m_CoGIxx = Ixx + m_VolumeMass * (LA.y*LA.y + LA.z*LA.z);
	m_CoGIyy = Iyy + m_VolumeMass * (LA.x*LA.x + LA.z*LA.z);
	m_CoGIzz = Izz + m_VolumeMass * (LA.x*LA.x + LA.y*LA.y);
	m_CoGIxz = Ixz + m_VolumeMass * LA.x*LA.z;

	for(i=0; i<m_PointMass.size(); i++)
	{
		LA = m_PointMass[i]->position() - m_CoG;
		m_CoGIxx += m_PointMass[i]->mass() * (LA.y*LA.y + LA.z*LA.z);
		m_CoGIyy += m_PointMass[i]->mass() * (LA.x*LA.x + LA.z*LA.z);
		m_CoGIzz += m_PointMass[i]->mass() * (LA.x*LA.x + LA.y*LA.y);
		m_CoGIxz -= m_PointMass[i]->mass() * (LA.x*LA.z);
	}
}



void Body::ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	// Assume that the mass is distributed homogeneously in the body's skin
	// Homogeneity is questionable, but is a rather handy assumption
	// Mass in the body's skin is reasonable, given that the point masses
	// are added manually

	//evaluate roughly the Body's wetted area
	int i,j,k;
	double ux, rho;
	double dj, dj1;
	CVector Pt, LATB, TALB, N, PLA, PTA, PLB, PTB, Top, Bot;
	double BodyArea = 0.0;
	double SectionArea;
	double xpos, dl;
	CoG.Set(0.0, 0.0, 0.0);
	CoGIxx = CoGIyy = CoGIzz = CoGIxz = 0.0;

	if(m_LineType==BODYPANELTYPE)
	{
		// we use the panel division
		//first get the wetted area

		for (i=0; i<FrameSize()-1; i++)
		{
			for (k=0; k<SideLineCount()-1; k++)
			{
				//build the four corner points of the strips
				PLA.x =  FramePosition(i);
				PLA.y =  getFrame(i)->m_CtrlPoint[k].y  ;
				PLA.z =  getFrame(i)->m_CtrlPoint[k].z  ;

				PLB.x = FramePosition(i);
				PLB.y = getFrame(i)->m_CtrlPoint[k+1].y ;
				PLB.z = getFrame(i)->m_CtrlPoint[k+1].z ;

				PTA.x = FramePosition(i+1);
				PTA.y = getFrame(i+1)->m_CtrlPoint[k].y ;
				PTA.z = getFrame(i+1)->m_CtrlPoint[k].z ;

				PTB.x = FramePosition(i+1);
				PTB.y = getFrame(i+1)->m_CtrlPoint[k+1].y;
				PTB.z = getFrame(i+1)->m_CtrlPoint[k+1].z;

				LATB = PTB - PLA;
				TALB = PLB - PTA;
				N = TALB * LATB;//panel area x2
				BodyArea += N.VAbs() /2.0;
			}
		}

		BodyArea *= 2.0;
		rho = m_VolumeMass/BodyArea;
		//First get the CoG position
		for (i=0; i<FrameSize()-1; i++)
		{
			for (j=0; j<m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_xPanels[i]);
				SectionArea = 0.0;

				PLA.x = PLB.x = (1.0- dj) * FramePosition(i)  +  dj * FramePosition(i+1);
				PTA.x = PTB.x = (1.0-dj1) * FramePosition(i)  + dj1 * FramePosition(i+1);

				for (k=0; k<SideLineCount()-1; k++)
				{
					//build the four corner points of the strips
					PLB.y = (1.0- dj) * getFrame(i)->m_CtrlPoint[k].y   +  dj * getFrame(i+1)->m_CtrlPoint[k].y;
					PLB.z = (1.0- dj) * getFrame(i)->m_CtrlPoint[k].z   +  dj * getFrame(i+1)->m_CtrlPoint[k].z;

					PTB.y = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k].y   + dj1 * getFrame(i+1)->m_CtrlPoint[k].y;
					PTB.z = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k].z   + dj1 * getFrame(i+1)->m_CtrlPoint[k].z;

					PLA.y = (1.0- dj) * getFrame(i)->m_CtrlPoint[k+1].y +  dj * getFrame(i+1)->m_CtrlPoint[k+1].y;
					PLA.z = (1.0- dj) * getFrame(i)->m_CtrlPoint[k+1].z +  dj * getFrame(i+1)->m_CtrlPoint[k+1].z;

					PTA.y = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k+1].y + dj1 * getFrame(i+1)->m_CtrlPoint[k+1].y;
					PTA.z = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k+1].z + dj1 * getFrame(i+1)->m_CtrlPoint[k+1].z;

					LATB = PTB - PLA;
					TALB = PLB - PTA;
					N = TALB * LATB;//panel area x2
					SectionArea += N.VAbs() /2.0;
				}
				SectionArea *= 2.0;// to account for right side;

				// get center point for this section
				Pt.x = (PLA.x + PTA.x)/2.0;
				Pt.y = 0.0;
				Pt.z = ((1.0-dj)  * m_SplineSurface.m_pFrame[i]->zPos() + dj  * m_SplineSurface.m_pFrame[i+1]->zPos()
					   +(1.0-dj1) * m_SplineSurface.m_pFrame[i]->zPos() + dj1 * m_SplineSurface.m_pFrame[i+1]->zPos())/2.0;

				CoG.x += SectionArea*rho * Pt.x;
				CoG.y += SectionArea*rho * Pt.y;
				CoG.z += SectionArea*rho * Pt.z;
			}
		}
		if(m_VolumeMass>PRECISION) CoG *= 1.0/ m_VolumeMass;
		else                       CoG.Set(0.0, 0.0, 0.0);

		//Then Get Inertias
		// we could do it one calculation, for CG and inertia, by using Hyghens/steiner theorem
		for (i=0; i<FrameSize()-1; i++)
		{
			for (j=0; j<m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_xPanels[i]);
				SectionArea = 0.0;

				PLA.x = PLB.x = (1.0- dj) * FramePosition(i)   +  dj * FramePosition(i+1);
				PTA.x = PTB.x = (1.0-dj1) * FramePosition(i)   + dj1 * FramePosition(i+1);

				for (k=0; k<SideLineCount()-1; k++)
				{
					//build the four corner points of the strips
					PLB.y = (1.0- dj) * getFrame(i)->m_CtrlPoint[k].y   +  dj * getFrame(i+1)->m_CtrlPoint[k].y;
					PLB.z = (1.0- dj) * getFrame(i)->m_CtrlPoint[k].z   +  dj * getFrame(i+1)->m_CtrlPoint[k].z;

					PTB.y = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k].y   + dj1 * getFrame(i+1)->m_CtrlPoint[k].y;
					PTB.z = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k].z   + dj1 * getFrame(i+1)->m_CtrlPoint[k].z;

					PLA.y = (1.0- dj) * getFrame(i)->m_CtrlPoint[k+1].y +  dj * getFrame(i+1)->m_CtrlPoint[k+1].y;
					PLA.z = (1.0- dj) * getFrame(i)->m_CtrlPoint[k+1].z +  dj * getFrame(i+1)->m_CtrlPoint[k+1].z;

					PTA.y = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k+1].y + dj1 * getFrame(i+1)->m_CtrlPoint[k+1].y;
					PTA.z = (1.0-dj1) * getFrame(i)->m_CtrlPoint[k+1].z + dj1 * getFrame(i+1)->m_CtrlPoint[k+1].z;

					LATB = PTB - PLA;
					TALB = PLB - PTA;
					N = TALB * LATB;//panel area x2
					SectionArea += N.VAbs() /2.0;
				}
				SectionArea *= 2.0;// to account for right side;

				// get center point for this section
				Pt.x = (PLA.x + PTA.x)/2.0;
				Pt.y = 0.0;
				Pt.z = ((1.0-dj)  * m_SplineSurface.m_pFrame[i]->zPos() + dj  * m_SplineSurface.m_pFrame[i+1]->zPos()
					   +(1.0-dj1) * m_SplineSurface.m_pFrame[i]->zPos() + dj1 * m_SplineSurface.m_pFrame[i+1]->zPos())/2.0;

				CoGIxx += SectionArea*rho * ( (Pt.y-CoG.y)*(Pt.y-CoG.y) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
				CoGIyy += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
				CoGIzz += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.y-CoG.y)*(Pt.y-CoG.y) );
				CoGIxz -= SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.z-CoG.z) );
			}
		}
	}
	else if(m_LineType==BODYSPLINETYPE)
	{
		int NSections = 20;//why not ?
		xpos = FramePosition(0);
		dl = Length()/(double)(NSections-1);

		for (j=0; j<NSections-1; j++)
		{
			BodyArea += dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl)) /2.0;
			xpos += dl;
		}

		rho = m_VolumeMass / BodyArea;

		// First evaluate CoG, assuming each section is a point mass
		xpos = FramePosition(0);
		for (j=0; j<NSections-1; j++)
		{
			SectionArea = dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl))/2.0;
			Pt.x = xpos + dl/2.0;
			Pt.y = 0.0;
			ux = Getu(Pt.x);
			GetPoint(ux, 0.0, true, Top);
			GetPoint(ux, 1.0, true, Bot);
			Pt.z = (Top.z + Bot.z)/2.0;
			xpos += dl;

			CoG.x += SectionArea*rho * Pt.x;
			CoG.y += SectionArea*rho * Pt.y;
			CoG.z += SectionArea*rho * Pt.z;
		}
		if(m_VolumeMass>PRECISION) CoG *= 1.0/ m_VolumeMass;
		else                       CoG.Set(0.0, 0.0, 0.0);

		// Next evaluate inertia, assuming each section is a point mass
		xpos = FramePosition(0);
		for (j=0; j<NSections-1; j++)
		{
			SectionArea = dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl))/2.0;
			Pt.x = xpos + dl/2.0;
			Pt.y = 0.0;
			ux = Getu(Pt.x);
			GetPoint(ux, 0.0, true, Top);
			GetPoint(ux, 1.0, true, Bot);
			Pt.z = (Top.z + Bot.z)/2.0;

			CoGIxx += SectionArea*rho * ( (Pt.y-CoG.y)*(Pt.y-CoG.y) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
			CoGIyy += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
			CoGIzz += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.y-CoG.y)*(Pt.y-CoG.y) );
			CoGIxz -= SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.z-CoG.z) );

			xpos += dl;
		}
	}
}


double Body::TotalMass()
{
	double TotalMass = m_VolumeMass;
	for(int im=0; im<m_PointMass.size(); im++)
		TotalMass += m_PointMass[im]->mass();
	return TotalMass;
}



void Body::SetEdgeWeight(double uw, double vw)
{
	m_SplineSurface.m_EdgeWeightu = uw;
	m_SplineSurface.m_EdgeWeightv = vw;
}









