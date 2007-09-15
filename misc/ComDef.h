#ifndef COMDEF_H
#define COMDEF_H

#define XFLR5_DATA     377

#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3
#define MIAREX         4


#define GLF_START_LIST 5000

#define MAXPANELS          30
#define MAXSTATIONS	      100 //max number of stations for LLT or VLM analysis
#define MAXVLMSURFACES     80 
#define VLMMATSIZE       2000
#define MAXCONTROLS        10 //max controls per wing section
#define SPLINECONTROLSIZE  50 //maximum number of control points

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

#define __min(a,b) std::min(a,b)
#define __max(a,b) std::max(a,b)

#endif 
