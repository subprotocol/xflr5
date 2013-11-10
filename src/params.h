/**
*@file This files defines the values of the main constant parameters used throughout the program
* A modification of this file triggers the compilation of the whole project.
* 
* @todo Set the a dynamic memory allocation at runtime for the panel mesh and for the influence matrix and rhs. 
This will allow the user to define larger meshes, at the expense increased size of memory usage.
*/
#ifndef PARAMS_H
#define PARAMS_H

//General
#define XFLR5
#define PI             3.14159265358979  
#define MAXRECENTFILES         8  /**< Defines the maximum number of file names in the recent file list */
#define SETTINGSFORMAT    100623  /**< A random number which defines the format of the settings file */
#define PRECISION  0.00000000001  /**< Values are assumed 0 if less than this value. This is to avoid comparing the equality of two floating point numbers */
#define MAXCOLORS   30            /**< The number of the different colors used, mainly for the graph curves */


#define PLANFORMAREA        1     /** The reference area and span for aero coefficients is the planform area. @todo replace by an enumeration */
#define PROJECTEDAREA       2     /** The reference area and span for aero coefficients is the area projected on the xy plane. */


//3D analysis parameters
#define MAXWINGS            4     /**< Wing, wing2, elevator, fin, in that order.*/
#define MAXBODIES           3     /**< One only in XFLR5 */
#define MAXSPANSECTIONS    30     /**< The max number of sections on a half wing */
#define MAXSPANSTATIONS   250     /**< The max number of stations for LLT. For a VLM analysis, this is the max number of panels in the spanwise direction. */
#define MAXCHORDPANELS	   50     /**< The max number of panels in the chordwise direction for a VLM analysis. */
#define VLMMAXMATSIZE    5000     /**< The max number of VLM panels for the whole plane. Sets the size of the influence matrix and its RHS.*/
#define VLMHALF          2500     /**< Half the value of VLMMAXMATSIZE. */
#define VLMMAXRHS         100     /**< The max number of points which may be calculated in a single sequence. Has an impact on the memory reserved at program launch the size of the */
#define MAXCONTROLS        40     /**< The max number of controls */
#define MAXBODYFRAMES      60     /**< The max number of frames that can be used to define a body. */
#define MAXSIDELINES       40     /**< The max number of sidelines that can be used to define a body? */

#define MAXPOLARPOINTS   1000     /**< The max number of points on a polar. @todo check usage now that the storage has been transferred to QList type arrays. Needs to be consistent with the max number of points on a graph curve. */

#define BODYPANELTYPE       1     /**< Label for a flat panel type body. @todo replace by an enumeration */
#define BODYSPLINETYPE      2     /**< Label for a spline type body. @todo replace by an enumeration */

//Direct Design
#define MAXSTACKPOS        50     /**< The max number of undo pictures on the stack in direct design. */
#define SPLINECONTROLSIZE  50     /**< The maximum number of control points for a spline in direct design. */


//XFoil Direct Parameters - refer to XFoil documentation
#define IQX  302	/**< 300 = number of surface panel nodes + 6 */
#define IQX2 151	/**< IQX/2 */
#define IWX   50	/**< number of wake panel nodes */
#define IPX    6	/**< 6 number of qspec[s] distributions */
#define ISX    3	/**< number of airfoil sides */
#define IBX  604	/**< 600 number of buffer airfoil nodes = 2*IQX */
#define IZX  350	/**< 350 = number of panel nodes [airfoil + wake] */
#define IVX  302	/**< 300 = number of nodes along bl on one side of airfoil and wake. */

//XFoil INVERSE parameters  - refer to XFoil documentation
#define ICX 257     /**< number of circle-plane points for complex mapping   ( 2^n  + 1 ) */
#define IMX 64      /**< number of complex mapping coefficients  Cn */
#define IMX4 16     /**< = IMX/4 */



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

#define ARCBALL                 1414
#define ARCPOINT                1415

#define MODELEGEND              1416

#define BODYGEOMBASE            4321
#define BODYMESHBASE            5322
#define BODYCPBASE              6173
#define BODYFORCELISTBASE       6245


#define MAXGRAPHS 4  /**< The max number of graphs available for display in QXDirect and in QMiarex. */

/** @enum The different types of analysis methods for 3D calculations of wings and planes. */
typedef enum{LLTMETHOD, VLMMETHOD, PANELMETHOD} enumAnalysisMethod;

/** @enum The different types of polar available for 2D and 3D calculations. */
typedef enum{FIXEDSPEEDPOLAR, FIXEDLIFTPOLAR, RUBBERCHORDPOLAR, FIXEDAOAPOLAR, STABILITYPOLAR} enumPolarType;

/** @enum The different applications available to the user */
typedef enum {NOAPP, XFOILANALYSIS, DIRECTDESIGN, INVERSEDESIGN, MIAREX}	enumApp;

/** @enum The different image formats usable to export screen captures*/
typedef enum {PNG, JPEG, BMP}	enumImageFormat;

/** @enum The different formats usable to export data to text format files*/
typedef enum {TXT, CSV}	enumTextFileType;

/** @enum The different number of graphs in the polar view */
typedef enum {ONEPOLARGRAPH, TWOPOLARGRAPHS, ALLPOLARGRAPHS} enumPolarGraphView;


#define QUESTION (BB || !BB) /**< Shakespeare */

#endif // PARAMS_H
 
