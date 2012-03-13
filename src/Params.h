#ifndef PARAMS_H
#define PARAMS_H

//General
#define PI             3.14159265358979
#define MAXRECENTFILES         8
#define SETTINGSFORMAT    100623
#define PRECISION  0.00000000001 //values are assumed 0 if less than this value
#define MAXCOLORS   30

// Applications
#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3
#define MIAREX         4

//GLVIEW
#define GLMIAREXVIEW  3
#define GLBODYVIEW  5
#define GLWINGVIEW  7



//Miarex View parameters
#define WOPPVIEW            1
#define WPOLARVIEW          2
#define W3DVIEW             3
#define WCPVIEW             4
#define WSTABVIEW          10
#define STABTIMEVIEW		0
#define STABPOLARVIEW		1
#define STAB3DVIEW		    3


//Polar reference area and span
#define PLANFORMAREA        1
#define PROJECTEDAREA       2


//3D analysis parameters
#define MAXWINGS            4 //wing, wing2, elevator, fin, in that order
#define MAXSPANSECTIONS    30 // max number of sections on a half wing
#define MAXSPANSTATIONS   250 // max number of stations for LLT or VLM analysis
#define MAXCHORDPANELS	   50 // max number of panels in the chordwise direction
#define VLMMAXMATSIZE    5000 // max number of VLM panels over the whole plane
#define VLMHALF          2500
#define VLMMAXRHS          50 // max number of points which may be calculated in a single sequence
#define MAXCONTROLS        40 // max controls per wing section
#define MAXBODYFRAMES      60
#define MAXSIDELINES       40
#define MAXMASSES          30
#define MAXPOLARPOINTS   1000
#define MAXMODEFRAMES     200

#define BODYPANELTYPE       1
#define BODYSPLINETYPE      2

//Direct Design
#define MAXSTACKPOS        50 // max number of undo pictures on the stack in direct design
#define SPLINECONTROLSIZE  50 // maximum number of control points


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
#define PANELFORCEARROWS        1391
#define PANELFORCELEGENDTXT     1392

#define GLBODYGEOM            1400
#define GLBODYSURFACES        1401
#define GLBODYMESHPANELS      1402
#define GLBODYMESHBACK        1403

#define ARCBALL             1414
#define ARCPOINT            1415

#define MODELEGEND          1416

#define SOLIDLINE           0
#define DASHLINE            1
#define DOTLINE             2
#define DASHDOTLINE         3
#define DASHDOTDOTLINE      4

#define MAXGRAPHS 4

typedef enum{LLTMETHOD, VLMMETHOD, PANELMETHOD} enumAnalysisMethod;
typedef enum{FIXEDSPEEDPOLAR, FIXEDLIFTPOLAR, RUBBERCHORDPOLAR, FIXEDAOAPOLAR, STABILITYPOLAR} enumPolarType;


#define QUESTION (BB || !BB) //Shakespeare

#endif // PARAMS_H
