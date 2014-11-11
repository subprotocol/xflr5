

/**
*@file
*
* This files defines the values of the main constant parameters used throughout the program.
*
* A modification of this file triggers the compilation of the whole project.
* 
*/
#ifndef PARAMS_H
#define PARAMS_H


#define VERSIONNAME     "xflr5 v6.10.04"

//General
#define XFLR5
#define PI             3.14159265358979  
#define MAXRECENTFILES         8  /**< Defines the maximum number of file names in the recent file list */
#define SETTINGSFORMAT     53639  /**< A random number which defines the format of the settings file */
#define PRECISION  0.00000000001  /**< Values are assumed 0 if less than this value. This is to avoid comparing the equality of two floating point numbers */




//3D analysis parameters
#define MAXWINGS            4     /**< Wing, wing2, elevator, fin, in that order.*/
#define MAXBODIES           1     /**< One only in XFLR5 */
#define MAXSPANSTATIONS   250     /**< The max number of stations for LLT. For a VLM analysis, this is the max number of panels in the spanwise direction. */



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

#define MIDPOINTCOUNT 100  /** the number of points which define the foil's mid camber line */


//MIAREX

//3D OpenGl list references

#define VLMSTREAMLINES          1220
#define SURFACESPEEDS           1221
#define LIFTFORCE               1222
#define VLMMOMENTS              1223
#define VLMWINGLIFT             1232
#define VLMWINGDRAG             1236
#define VLMWINGWASH             1241
#define VLMWINGTOPTRANS         1245
#define VLMWINGBOTTRANS         1249
#define WINGSURFACES            1300
#define WINGOUTLINE             1304

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


#define BODYGEOMBASE            4321
#define BODYMESHBASE            5322
#define BODYCPBASE              6173
#define BODYFORCELISTBASE       6245


#define MAXGRAPHS 4  /**< The max number of graphs available for display in QXDirect and in QMiarex. */

/** @enum The different types of analysis methods for 3D calculations of wings and planes. */
typedef enum{LLTMETHOD, VLMMETHOD, PANELMETHOD} enumAnalysisMethod;

/** @enum The different types of polar available for 2D and 3D calculations. */
typedef enum{FIXEDSPEEDPOLAR, FIXEDLIFTPOLAR, RUBBERCHORDPOLAR, FIXEDAOAPOLAR, STABILITYPOLAR, BETAPOLAR} enumPolarType;

/** @enum The different applications available to the user */
typedef enum {NOAPP, XFOILANALYSIS, DIRECTDESIGN, INVERSEDESIGN, MIAREX}	enumApp;

/**< @enum The different image formats usable to export screen captures*/
typedef enum {PNG, JPEG, BMP}	enumImageFormat;

/** @enum The different formats usable to export data to text format files*/
typedef enum {TXT, CSV}	enumTextFileType;

/** @enum The different number of graphs in the polar view */
typedef enum {ONEGRAPH, TWOGRAPHS, ALLGRAPHS} enumGraphView;

/** @enum The different type of reference areaand length */
typedef enum {PLANFORMREFDIM, PROJECTEDREFDIM, MANUALREFDIM} enumRefDimension;

#define QUESTION (BB || !BB) /**< Shakespeare */

#endif // PARAMS_H
 
