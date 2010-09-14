/****************************************************************************

	QFLR5 Application

	Copyright (C) 2008-2010 Andre Deperrois XFLR5@yahoo.com

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
#include "QFLR5Application.h"
#include "MainFrame.h"
#include <QSplashScreen>
#include <QtDebug>

CFoil *g_pCurFoil;	//make it common to all applications

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x040600
        QGL::setPreferredPaintEngine (QPaintEngine::OpenGL);
#endif
        QFLR5Application app(argc, argv);

	QPixmap pixmap;
	qDebug() << pixmap.load(":/images/splash.png");
	QSplashScreen splash(pixmap);
	splash.setWindowFlags(Qt::SplashScreen);
	splash.show();

	QString StyleName = "Cleanlooks";
	QString LanguagePath ="";

	QString str;
	int a,b,c,d,k;
	a=150;
	b=50;
	c=800;
	d=700;


#ifdef Q_WS_MAC
        QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"XFLR5");
#else
        QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif
	bool bMaximized = true;
	bool bOK;
	settings.beginGroup("MainFrame");
	{
		k = settings.value("FrameGeometryx").toInt(&bOK);
		if(bOK) a = k;
		k = settings.value("FrameGeometryy").toInt(&bOK);
		if(bOK) b = k;
		k = settings.value("SizeWidth").toInt(&bOK);
		if(bOK) c = k;
		k = settings.value("SizeHeight").toInt(&bOK);
		if(bOK) d = k;
		bMaximized = settings.value("SizeMaximized").toBool();
		str = settings.value("StyleName").toString();
		if(str.length()) StyleName = str,
		str = settings.value("LanguageFilePath").toString();
		if(str.length()) LanguagePath = str;
	}
	settings.endGroup();

	QTranslator qflr5Translator;

	if(LanguagePath.length())
	{
		if(qflr5Translator.load(LanguagePath)) app.installTranslator(&qflr5Translator);
	}

	QPoint pt(a,b);
	QSize sz(c,d);

	if(StyleName.length())	qApp->setStyle(StyleName);

	MainFrame w;
	app.setQFLR5MainWindow(&w);
	w.resize(sz);
	w.move(pt);
	if(bMaximized)	w.showMaximized();
	else                w.show();

	QString PathName, Extension;
	PathName=argv[1];

	Extension = PathName.right(4);
	if(Extension.compare(".wpa",Qt::CaseInsensitive)==0 ||
	   Extension.compare(".plr",Qt::CaseInsensitive)==0)
	{
		int iApp = w.LoadXFLR5File(PathName);

		if (iApp == MIAREX)             w.OnMiarex();
		else if (iApp == XFOILANALYSIS) w.OnXDirect();
	}

	splash.finish(&w);

	return app.exec();
}

