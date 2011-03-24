#ifndef PARAMS_H
#define PARAMS_H

//General
#define PI             3.14159265358979
#define MAXRECENTFILES         8
#define SETTINGSFORMAT    100623
#define PRECISION  0.00000000001 //values are assumed 0 if less than this value

// Applications
#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3
#define MIAREX         4
#define XBODY          5

//View parameters
#define WOPPVIEW            1
#define WPOLARVIEW          2
#define W3DVIEW             3
#define WCPVIEW             4
#define WSTABVIEW          10
#define STABTIMEVIEW		0
#define STABPOLARVIEW		1
#define STAB3DVIEW		    3


//Polar types
#define FIXEDSPEEDPOLAR     1
#define FIXEDLIFTPOLAR      2
#define RUBBERCHORDPOLAR    3
#define FIXEDAOAPOLAR       4
#define STABILITYPOLAR      7

//Polar reference area and span
#define PLANFORMAREA        1
#define PROJECTEDAREA       2

// 3D Analysis methods
#define LLTMETHOD           1
#define VLMMETHOD           2
#define PANELMETHOD         3

//3D analysis parameters
#define MAXWINGS            4 //wing, wing2, elevator, fin, in that order
#define MAXPANELS          50 // max number of sections on a half wing
#define MAXSTATIONS       500 // max number of stations for LLT or VLM analysis
#define MAXCHORDPANELS	  100
#define VLMMAXMATSIZE    5000
#define VLMHALF          2500
#define VLMMAXRHS         100 // max number of points which may be calculated in a single sequence
#define MAXCONTROLS        40 // max controls per wing section
#define SPLINECONTROLSIZE  50 // maximum number of control points
#define MAXPICTURESIZE     40 // maximum number of undo operations in direct design
#define MAXBODYFRAMES      60
#define MAXSIDELINES       40
#define MAXMASSES          40
#define MAXPOLARPOINTS   1000
#define MAXMODEFRAMES     200

#define BODYPANELTYPE       1
#define BODYSPLINETYPE      2



//XFoil Direct Parameters
#define IQX  302	//300 = number of surface panel nodes + 6
#define IQX2 151	//IQX/2 added arcds
#define IWX   50	// number of wake panel nodes
#define IPX    6	//6 number of qspec[s] distributions
#define ISX    3	//number of airfoil sides
#define IBX  604	//600 number of buffer airfoil nodes = 2*IQX
#define IZX  350	//350 = number of panel nodes [airfoil + wake]
#define IVX  302	//300 = number of nodes along bl on one side of airfoil and wake

//XFoil INVERSE parameters
#define ICX 257 // number of circle-plane points for complex mapping   ( 2^n  + 1 )
#define IMX 64  // number of complex mapping coefficients  Cn
#define IMX4 16 // = IMX/4 added arcds


//Foil Direct design
#define MAXSTACKPOS               50 // max number of undo pictures on the stack

//MIAREX

//3D OpenGl list references

#define VLMSTREAMLINES          1220
#define SURFACESPEEDS           1221

#define LIFTFORCE               1222
#define VLMMOMENTS              1223

#define VLMWINGLIFT             1232
#define VLMWING2LIFT            1233
#define VLMSTABLIFT             1234
#define VLMFINLIFT              1235

#define VLMWINGDRAG             1236
#define VLMWING2DRAG            1237
#define VLMSTABDRAG             1238
#define VLMFINDRAG              1239

#define VLMWINGWASH             1241
#define VLMWING2WASH            1242
#define VLMSTABWASH             1243
#define VLMFINWASH              1244

#define VLMWINGTOPTRANS         1245
#define VLMWING2TOPTRANS        1246
#define VLMSTABTOPTRANS         1247
#define VLMFINTOPTRANS          1248

#define VLMWINGBOTTRANS         1249
#define VLMWING2BOTTRANS        1250
#define VLMSTABBOTTRANS         1251
#define VLMFINBOTTRANS          1252

#define WINGSURFACES            1300
#define WING2SURFACES           1301
#define STABSURFACES            1302
#define FINSURFACES             1303

#define WINGOUTLINE             1304
#define WING2OUTLINE            1305
#define STABOUTLINE             1306
#define FINOUTLINE              1307

#define MESHPANELS              1372
#define MESHBACK                1373

#define WINGLEGEND              1376
#define WOPPLEGEND              1377
#define WOPPCPLEGENDCLR         1378
#define WOPPCPLEGENDTXT         1379

#define WINGWAKEPANELS          1383

#define VLMCTRLPTS              1385
#define VLMVORTICES             1386

#define PANELCP                 1390
#define PANELFORCEARROWS     1391
#define PANELFORCELEGENDTXT     1392

#define BODYGEOM            1400
#define BODYSURFACES        1401
#define BODYMESHPANELS      1402
#define BODYMESHBACK        1403

#define ARCBALL             1414
#define ARCPOINT            1415

#define MODELEGEND          1416

#define QUESTION (BB || !BB) //Shakespeare

#endif // PARAMS_H
