/****************************************************************************

		 NURBSDomDoc Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef NURBSDOMDOC_H
#define NURBSDOMDOC_H

#include <QDomDocument>
#include <QFile>


class NURBSDomDoc : public QDomDocument
{
public:
	NURBSDomDoc(void *pNURBS=NULL);
	QDomElement GetElement(const QDomNode &node, QString const &TagName, QString const &AttributeName);
	void ReadNodes(QDomNode node);
	void Export(QTextStream &outStream);
	bool Import(QFile *pFile);

	void *m_pNURBS;
};

#endif // NURBSDOMDOC_H
