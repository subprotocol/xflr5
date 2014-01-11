/****************************************************************************

	XFLR5 Application

	Copyright (C) 2008-2013 Andre Deperrois adeperrois@xflr5.com

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


#include "XFLR5Application.h"
#include "mainframe.h"
#include <QSplashScreen>
#include <QMessageBox>


/**
*The app's point of entry !
*/
int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x040600
//	QGL::setPreferredPaintEngine (QPaintEngine::OpenGL);
#endif
#ifdef Q_OS_MACX
	if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
	{
		// fix Mac OS X 10.9 (mavericks) font issue
		// https://bugreports.qt-project.org/browse/QTBUG-32789
		QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
	}
#endif
	XFLR5Application app(argc, argv);
	return app.exec();
}


