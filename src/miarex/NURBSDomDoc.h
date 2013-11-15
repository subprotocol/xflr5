/****************************************************************************

		 NURBSDomDoc Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

/**
 * @file
 * This file is an attempt to implement an XML format for the input/output of NURBS definitions
 * Unfinished. @todo : finish the implementation.
*/

#ifndef NURBSDOMDOC_H
#define NURBSDOMDOC_H

#include <QDomDocument>
#include <QFile>

/**
 * @class NURBSDomDoc
 * This class is an attempt to implement an XML format for the input/output of plan definitions
 * Unfinished. @todo : finish the implementation.
*/
class NURBSDomDoc : public QDomDocument
{
public:
	NURBSDomDoc(void *pNURBS=NULL);
	QDomNode GetNode(QDomNode const &node, QString const &NodeName);
	QDomElement GetElement(const QDomNode &node, QString const &TagName, QString const &AttributeName);
	QDomElement GetElement(QDomNode const &node, QString const &elementName);
	void ReadNodes(QDomNode node);
	void Export(QTextStream &outStream);
	bool Import(QFile *pFile);

	void *m_pNURBS;
};

#endif // NURBSDOMDOC_H
