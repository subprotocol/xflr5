/****************************************************************************

	GraphWidget Class
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


#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include "QGraph.h"

class GraphWidget : public QWidget
{
	friend class XFoilAnalysisDlg;
	friend class LLTAnalysisDlg;
	friend class BatchDlg;

public:
		GraphWidget();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent *event);

	QWidget *ScalePage;
	QWidget *AxisPage;
	QWidget *FontPage;
	QWidget *BackPage;

	QGraph *m_pGraph;

};

#endif // GRAPHWIDGET_H
