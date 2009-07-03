/****************************************************************************

        QFLR5Application Class

    Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com
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
#ifndef QFLR5APPLICATION_H
#define QFLR5APPLICATION_H

#include <QApplication>
#include "MainFrame.h"

class QFLR5Application : public QApplication
{
    Q_OBJECT
    private:
        MainFrame *mainFrame;
    protected:
        bool event(QEvent *);
    public:
        QFLR5Application(int&, char**&);
        void setQFLR5MainWindow(MainFrame *);
};

#endif // QFLR5APPLICATION_H
