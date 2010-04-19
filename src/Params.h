#ifndef PARAMS_H
#define PARAMS_H

#define PI             3.14159265358979

#define XFOILANALYSIS  1
#define DIRECTDESIGN   2
#define INVERSEDESIGN  3
#define MIAREX         4
#define XBODY          5

#define MAXPANELS          25
#define MAXSTATIONS	      100 //max number of stations for LLT or VLM analysis
#define MAXCHORDPANELS	   30
#define VLMMATSIZE       2000
#define VLMHALF          1000
#define MAXCONTROLS        10 //max controls per wing section
#define SPLINECONTROLSIZE  50 //maximum number of control points
#define MAXPICTURESIZE     40 //maximum number of undo operations in direct design
#define MAXBODYFRAMES      30
#define MAXSIDELINES       20

#define MAXRECENTFILES      8

#define MAXMASSES          20

#define WOPPVIEW            1
#define WPOLARVIEW          2
#define W3DVIEW             3
#define WCPVIEW             4
#define WSTABVIEW          10


#define IQX  302	//300 = number of surface panel nodes + 6
#define IQX2 151	//IQX/2 added arcds
#define IWX   50	// number of wake panel nodes
#define IPX    6	//6 number of qspec[s] distributions
#define ISX    3	//number of airfoil sides
#define IBX  604	//600 number of buffer airfoil nodes = 2*IQX
#define IZX  350	//350 = number of panel nodes [airfoil + wake]
#define IVX  302	//300 = number of nodes along bl on one side of airfoil and wake

//----INVERSE
#define ICX 257 // number of circle-plane points for complex mapping   ( 2^n  + 1 )
#define IMX 64  // number of complex mapping coefficients  Cn
#define IMX4 16 // = IMX/4 added arcds

#define SETTINGSFORMAT    100619
#define PRECISION  0.00000000001 //values are assumed 0 if less than this value

//AFOIL
#define MAXSTACKPOS			  50

//MIAREX
#define PANELCP				1227

#define VLMWINGLIFT			1230
#define LIFTFORCE			1231
#define VLMMOMENTS			1232

#define VLMWING2LIFT		1233
#define VLMSTABLIFT			1234
#define VLMFINLIFT			1235

#define VLMWINGDRAG			1236
#define VLMWING2DRAG		1237
#define VLMSTABDRAG			1238
#define VLMFINDRAG			1239

#define VLMWINGWASH			1241
#define VLMWING2WASH		1242
#define VLMSTABWASH			1243
#define VLMFINWASH			1244

#define VLMCTRLPTS			1250
#define VLMVORTICES			1251

#define VLMSTREAMLINES		1255
#define SURFACESPEEDS		1256

#define WINGSURFACES		1258
#define WINGOUTLINE			1259

#define WING2SURFACES		1260
#define WING2OUTLINE		1261

#define STABSURFACES		1265
#define STABOUTLINE			1266

#define FINSURFACES 		1270
#define FINOUTLINE			1271

#define MESHPANELS			1272
#define MESHBACK			1273

#define WINGLEGEND			1276
#define WOPPLEGEND			1277
#define WOPPCPLEGENDCLR		1278
#define WOPPCPLEGENDTXT		1279

#define WINGWAKEPANELS		1283

#define VLMWINGTOPTRANS     1293
#define VLMWINGBOTTRANS     1294
#define VLMWING2TOPTRANS    1295
#define VLMWING2BOTTRANS    1296
#define VLMSTABTOPTRANS     1297
#define VLMSTABBOTTRANS     1298
#define VLMFINTOPTRANS      1299
#define VLMFINBOTTRANS      1300

//3D
#define BODYGEOM            1302
#define BODYSURFACES        1303
#define BODYMESHPANELS		1312
#define BODYMESHBACK		1313

#define ARCBALL             1314
#define ARCPOINT            1315

// Modal
#define MODELEGEND          1316

#define MAXPOLARPOINTS    1000
#define MAXMODEFRAMES     100

#define QUESTION (BB || !BB) //Shakespeare

#endif // PARAMS_H
