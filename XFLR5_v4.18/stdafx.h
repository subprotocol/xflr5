/****************************************************************************

	stdafx
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afx.h>

#define XFLR5_DATA     377

#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3
#define MIAREX         4
#define XBODY          5


#define GLF_START_LIST 5000

#define MAXPANELS          25
#define MAXSTATIONS	      100 //max number of stations for LLT or VLM analysis
#define MAXCHORDPANELS	   30
//#define MAXVLMSURFACES     50 //2 * MAXPANELS
#define VLMMATSIZE       2000
#define VLMHALF          1000
#define MAXCONTROLS        10 //max controls per wing section
#define SPLINECONTROLSIZE  50 //maximum number of control points
#define MAXBODYFRAMES      30
#define MAXSIDELINES       20

#define IQX  302	//300 = number of surface panel nodes + 6
#define IQX2 151	//IQX/2 added arcds
#define IWX   50	// number of wake panel nodes
#define IPX    6	//6 number of qspec[s] distributions
#define ISX    3	//number of airfoil sides
#define IBX  604	//600 number of buffer airfoil nodes = 2*IQX
#define IZX  350	//350 = number of panel nodes [airfoil + wake]
#define IVX  302	//300 = number of nodes along bl on one side of airfoil and wake

//----INVERSE
#define ICX 257//   number of circle-plane points for complex mapping   ( 2^n  + 1 )
#define IMX 64// number of complex mapping coefficients  Cn
#define IMX4 16 // = IMX/4 added arcds

//#define V_ENDTHREAD (WM_APP+1) //to notify parent window that the analysis thread has ended


#define QUESTION (BB || !BB) //Shakespeare

