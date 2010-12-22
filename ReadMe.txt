	
*******************************************************************************


                                        XFLR5

                     A tool for the design of Airfoils, Wings and Planes
                          operating at low Reynolds numbers

                                Copyright (C) 2009

                                     Andre Deperrois


*******************************************************************************




                       **************************

Credits to :
	Mark Drela and Harold Youngren for developping XFoil up to v6.94 (http://raphael.mit.edu/xfoil/)
	Matthieu Scherrer for the idea of applying the lifting line theory (http://perso.wanadoo.fr/scherrer/matthieu/english/miarexe.html)
	Paul Bourke for the B-Spline algorithms (http://astronomy.swin.edu.au/~pbourke/curves/spline/)


                       **************************


Licence :

    This program is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; 
    either version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this program; if not, write to the 
    Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


                       **************************


The source code can be downloaded/checked out from the SVN repository. 
https://xflr5.svn.sourceforge.net/svnroot/xflr5/branches/QFLR5 

In Windows for instance, this can be done with an SVN client such as TortoiseSVN http://tortoisesvn.tigris.org/ 
 

Compilation instructions
        - To compile under Ubuntu/Debian Linux, please do the following:
          go to the directory, type dpkg-buildpackage -rfakeroot.

        - To compile manually on any Linux distro, please follow the following steps:
          1. qmake
          2. make
          3. sudo make install 

        - To compile on MacOSX, please follow the following steps:
          1. qmake -makefile
          2. qmake -spec macx-g++
          3. make -j3
          4. ./mac/makedist	

	- To compile on Windows, the easiest way is to download QtCreator from Trolltech.
	  QtCreator comes with the MinGW compiler.
