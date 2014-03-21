/****************************************************************************

    XFLR5Application  Class
    Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com
                       Francesco Meschia francesco.meschia@gmail.com

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

#include <QFileOpenEvent>
#include <QSplashScreen>
#include <QDateTime>
#include <QMessageBox>
#include "XFLR5Application.h"
#include "mainframe.h"


XFLR5Application::XFLR5Application(int &argc, char** argv) : QApplication(argc, argv)
{
	QPixmap pixmap;
	pixmap.load(":/images/splash.png");
	QSplashScreen splash(pixmap);
	splash.setWindowFlags(Qt::SplashScreen);
	splash.show();

	QString StyleName;
	QString LanguagePath ="";

	QString str;
	int a,b,c,d,k;
	a=150;
	b=50;
	c=800;
	d=700;


#ifdef Q_WS_MAC
    QSettings settings(QSettings::NativeFormat,QSettings::UserScope,"sourceforge.net","xflr5");
#else
	QSettings settings(QSettings::IniFormat,QSettings::UserScope,"XFLR5");
#endif

	qsrand(QDateTime::currentDateTime().toTime_t());

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

		str = settings.value("LanguageFilePath").toString();
		if(str.length()) LanguagePath = str;

		str = settings.value("StyleName").toString();
		if(str.length()) StyleName = str;
	}
	settings.endGroup();

	QTranslator xflr5Translator;

	if(LanguagePath.length())
	{
		if(xflr5Translator.load(LanguagePath)) installTranslator(&xflr5Translator);
	}

	QPoint pt(a,b);
	QSize sz(c,d);

	if(StyleName.length())	qApp->setStyle(StyleName);

	MainFrame *w = MainFrame::self();
	MainFrame::self()->resize(sz);
	MainFrame::self()->move(pt);
	if(bMaximized)	MainFrame::self()->showMaximized();
	else            MainFrame::self()->show();


#ifndef Q_WS_MAC
	QString PathName, Extension;
	PathName=argv[1];


	Extension = PathName.right(4);
	if(Extension.compare(".wpa",Qt::CaseInsensitive)==0 ||	Extension.compare(".plr",Qt::CaseInsensitive)==0)
	{
		int iApp = w->LoadXFLR5File(PathName);

		if (iApp == MIAREX)             w->OnMiarex();
		else if (iApp == XFOILANALYSIS) w->OnXDirect();
	}
#endif

	splash.finish(w);
}


bool XFLR5Application::event(QEvent *event)
{
	int iApp;
	switch (event->type())
	{
		case QEvent::FileOpen:
		{
			iApp = MainFrame::self()->LoadXFLR5File(static_cast<QFileOpenEvent *>(event)->file());
			if (iApp == MIAREX)             MainFrame::self()->OnMiarex();
			else if (iApp == XFOILANALYSIS) MainFrame::self()->OnXDirect();

			return true;
		}
		default:
			return QApplication::event(event);
	}
}


