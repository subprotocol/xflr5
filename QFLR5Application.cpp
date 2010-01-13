/****************************************************************************

    QFLR5Application  Class
    Copyright (C) 2008 Andre Deperrois xflr5@yahoo.com
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

#include "QFLR5Application.h"
#include "MainFrame.h"

QFLR5Application::QFLR5Application(int &argc, char**& argv) : QApplication(argc, argv)
{
}

bool QFLR5Application::event(QEvent *event)
{
	int iApp;
	switch (event->type())
	{
		case QEvent::FileOpen:
		{
			iApp = this->mainFrame->LoadXFLR5File(static_cast<QFileOpenEvent *>(event)->file());
			if (iApp == MIAREX)             this->mainFrame->OnMiarex();
			else if (iApp == XFOILANALYSIS) this->mainFrame->OnXDirect();

			return true;
		}
    default:
        return QApplication::event(event);
    }
}

void QFLR5Application::setQFLR5MainWindow(MainFrame *mf)
{
	this->mainFrame = mf;
}
