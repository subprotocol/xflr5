/****************************************************************************

		 SailDomDoc Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/



#include <QDate>
#include <QtDebug>
#include "NURBSDomDoc.h"
#include "../objects/NURBSSurface.h"
#include "../globals.h"


NURBSDomDoc::NURBSDomDoc(void *pNURBS)
					: QDomDocument("X3D PUBLIC \"ISO//Web3D//DTD X3D 3.2//EN\" \"http://www.web3d.org/specifications/x3d-3.2.dtd\"") //!DOCTYPE
{
	QDomProcessingInstruction xmlHeaderPI = createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"" );
	appendChild(xmlHeaderPI);


	m_pNURBS = pNURBS;
}



void NURBSDomDoc::Export(QTextStream &outStream)
{
//	NURBSSurface *pNURBS  = (NURBSSurface*)m_pNURBS;
	QDomElement data;

	QDomElement NURBSdoc = createElement("X3D");
	{
		NURBSdoc.setAttribute("profile", "Interchange");
		NURBSdoc.setAttribute("version", "3.2");
		NURBSdoc.setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema-instance");
		NURBSdoc.setAttribute("xsd:noNamespaceSchemaLocation", "http://www.web3d.org/specifications/x3d-3.2.xsd");


		QDomElement head = createElement("head");

		NURBSdoc.appendChild(head);
		{
			data = createElement("component");
			data.setAttribute("level", 1);
			data.setAttribute("name", "NURBS");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "title");
			data.setAttribute("content", "NURBS Surface");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "description");
			data.setAttribute("content", "NURBS");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "creator");
			data.setAttribute("content", "XFLR5 Application");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "created");
			QDate dt = QDate::currentDate();
			QString date = dt.toString("dd.MM.yyyy");
			data.setAttribute("content", date);
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "modified");
			data.setAttribute("content", date);
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "image");
			data.setAttribute("content", "NURBS_Surface.jpg");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "reference");
			data.setAttribute("content", "http://www.xflr5.com");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "identifier");
			data.setAttribute("content", "http://www.web3d.org/x3d/content/examples/Basic/NURBS/MobiusNurbs.x3d");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "generator");
			data.setAttribute("content", "XFLR5 Application");
			head.appendChild(data);

			data = createElement("meta");
			data.setAttribute("name", "license");
			data.setAttribute("content", "GPL");
			head.appendChild(data);
		}

		QDomElement scene = createElement("Scene");
		NURBSdoc.appendChild(scene);
		{
			data = createElement("Background");
			data.setAttribute("skyColor", "1 1 1");
			data.setAttribute("transparency", 0);
			scene.appendChild(data);

			data = createElement("NavigationInfo");
			data.setAttribute("type", "\"EXAMINE\" \"ANY\"");
			data.setAttribute("transitionType", "ANIMATE");
			data.setAttribute("transitionTime", "1.0");
			scene.appendChild(data);

		}
//TODO... to be continued
		appendChild(NURBSdoc);
	}

	const int IndentSize = 4;
	save(outStream, IndentSize);
}


void NURBSDomDoc::ReadNodes(QDomNode node)
{
	while(!node.isNull())
	{
		QDomElement en = node.toElement(); // try to convert the node to an element.
		if(!en.isNull())
		{
//			qDebug()<<en.tagName()<<en.attribute("name", "No Name...") << en.attribute("value", "No value...");
		}

		// elements may also have childs...
		QDomNode m = node.firstChild();
		ReadNodes(m);
		node = node.nextSibling();
	}
}



QDomElement NURBSDomDoc::GetElement(QDomNode const &node, QString const &TagName, QString const &AttributeName)
{
	QDomElement en;
	QDomNode n = node;
	while(!n.isNull())
	{
		en = n.toElement(); // try to convert the node to an element.
		if(!en.isNull())
		{
			if(en.tagName()==TagName)
			{
				if(en.attribute("name","")==AttributeName) return en;
			}
		}

		// continue searching through child elements
		QDomNode m = n.firstChild();
		if(!m.isNull()) en = GetElement(m, TagName, AttributeName);
		else            en.clear();
		if(!en.isNull()) return en;

		//else continue searching
		n = n.nextSibling();
	}
	en.clear();
	return en;
}


bool NURBSDomDoc::Import(QFile *pFile)
{
	if(!setContent(pFile))
	{
		pFile->close();
		return false;
	}
	//TODO... to be continued
	return false;
}



















