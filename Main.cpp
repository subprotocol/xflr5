/****************************************************************************

	QFLR5 Application

	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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


#include <QtGui/QApplication>
#include "MainFrame.h"

int main(int argc, char *argv[])
{
	QString StyleName = "Cleanlooks";

	QString FileName   = QDir::tempPath() + "/QFLR5.set";
	QFile *pXFile = new QFile(FileName);
	int a,b,c,d,k;
	a=b=50;
	c=800;
	d=500;
	bool bMaximized = true;
 
	if (pXFile->open(QIODevice::ReadOnly))
	{
		QDataStream ar(pXFile);
		ar >> k;//format

		ar >> a >> b >> c >> d;
		ar >> bMaximized;
		ar >> StyleName;
		pXFile->close();
	}
	QPoint pt(a,b);
	QSize sz(c,d);

	// QT Help Warning: To ensure that the application's style is set correctly, it is best to call this function before the QApplication constructor, if possible.
	qApp->setStyle(StyleName);

	QApplication app(argc, argv);

	MainFrame w;
	w.setWindowTitle("QFLR5");

	w.resize(sz);
	w.move(pt);

	if(w.m_bMaximized)	w.showMaximized();
	else                w.show();
	return app.exec();
}

