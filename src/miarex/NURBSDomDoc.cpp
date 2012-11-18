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
	NURBSSurface *pNURBS  = (NURBSSurface*)m_pNURBS;
	pNURBS->SetKnots();
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
			data.setAttribute("content", "NURBS Surface");
			data.setAttribute("name", "title");
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
		{
			NURBSdoc.appendChild(scene);

			data = createElement("Background");
			data.setAttribute("skyColor", "1 1 1");
			data.setAttribute("transparency", 0);
			scene.appendChild(data);

			data = createElement("NavigationInfo");
			data.setAttribute("type", "\"EXAMINE\" \"ANY\"");
			data.setAttribute("transitionType", "ANIMATE");
			data.setAttribute("transitionTime", "1.0");
			scene.appendChild(data);

			data = createElement("Viewpoint");
			data.setAttribute("description", "Normal viewpoint");
			data.setAttribute("orientation", "1 0 0 -0.54042");
			data.setAttribute("position", "0 60 100");
			data.setAttribute("retainUserOffsets", "false");
			scene.appendChild(data);

			QDomElement shape = createElement("Shape");
			{
				scene.appendChild(shape);

				data = createElement("NurbsPatchSurface");
				{
					data.setAttribute("solid", "false");
					data.setAttribute("uDimension", pNURBS->m_iuDegree);
					data.setAttribute("uOrder", pNURBS->m_iuDegree + pNURBS->FrameSize()+1); //TODO : check order definition
					data.setAttribute("vDimension", pNURBS->m_ivDegree);
					data.setAttribute("vOrder", pNURBS->m_iuDegree + pNURBS->FramePointCount()+1); //TODO : check order definition
					QString uknots;
					for(int iu=0; iu<pNURBS->m_nuKnots; iu++)
					{
						uknots += QString("%1 ").arg(pNURBS->m_uKnots[iu], 7, 'f', 5);
					}
					data.setAttribute("uknot", uknots.trimmed());

					QString vknots;
					for(int iv=0; iv<pNURBS->m_nvKnots; iv++)
					{
						vknots += QString("%1 ").arg(pNURBS->m_vKnots[iv], 7, 'f', 5);
					}
					vknots = vknots.trimmed();
					data.setAttribute("vknot", vknots);

					data.setAttribute("uClosed", "false");
					data.setAttribute("vClosed", "false");
					data.setAttribute("uTessellation", "0");
					data.setAttribute("vTessellation", "0");
					data.setAttribute("containerField", "geometry");
					shape.appendChild(data);
				}

				data = createElement("Coordinate");
				{
					data.setAttribute("containerField", "controlPoint");
					QString pointcoordinates;
					for(int iu=0; iu<pNURBS->FrameSize(); iu++)
					{
						for(int iv=0; iv<pNURBS->FramePointCount(); iv++)
						{
							pointcoordinates += QString("%1 %2 %3")
								.arg(pNURBS->m_pFrame[iu]->m_CtrlPoint[iv].x, 11, 'f', 5)
								.arg(pNURBS->m_pFrame[iu]->m_CtrlPoint[iv].y, 11, 'f', 5)
								.arg(pNURBS->m_pFrame[iu]->m_CtrlPoint[iv].z, 11, 'f', 5);
						}
					}
					data.setAttribute("point", pointcoordinates);
					shape.appendChild(data);
				}
				QDomElement appearance = createElement("Appearance");
				{
					shape.appendChild(appearance);
					data = createElement("Material");
					data.setAttribute("diffuseColor", "0 0 1");
					appearance.appendChild(data);
				}
			}
		}

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



QDomElement NURBSDomDoc::GetElement(QDomNode const &node, QString const &elementName)
{
	QDomElement en;
	QDomNode n = node;
	while(!n.isNull())
	{
		en = n.toElement(); // try to convert the node to an element.
		if(!en.isNull())
		{
			if(en.tagName()==elementName)return en;
		}

		// continue searching through child elements
		QDomNode m = n.firstChild();
		if(!m.isNull()) en = GetElement(m, elementName);
		else            en.clear();
		if(!en.isNull()) return en;

		//else continue searching
		n = n.nextSibling();
	}
	en.clear();
	return en;
}



QDomNode NURBSDomDoc::GetNode(QDomNode const &node, QString const &NodeName)
{
	QDomNode n = node;
	while(!n.isNull())
	{
		if(n.nodeName()==NodeName)
		{
			return n;
		}

		// continue searching through the sibling nodes
//		QDomNode m = n.firstChild();
		n = n.nextSibling();
	}
	n.clear();
	return n;
}



bool NURBSDomDoc::Import(QFile *pFile)
{
	qDebug()<<"Importing";
	if(!setContent(pFile))
	{
		pFile->close();
		return false;
	}

	NURBSSurface *pNURBS  = (NURBSSurface*)m_pNURBS;

	QDomElement en;
	QDomNode n0, n1, n2;
	QDomNode node = documentElement().firstChild();

	n0 = GetNode(node, "Scene");
qDebug()<<n0.nodeName();
	n1 = GetNode(n0.firstChild(), "Shape");
	{
		qDebug()<<n1.nodeName();

		en = GetElement(n1, QString("NurbsPatchSurface"));
		qDebug() <<"element"	<<en.tagName()<<en.nodeName();
		if(!en.isNull())
		{
			pNURBS->m_iuDegree = en.attribute("uDimension","999").toInt();
			pNURBS->m_ivDegree = en.attribute("vDimension","123456").toInt();
			QString uknots = en.attribute("uknot", "1.234, 0.5678");
			QTextStream knotstream(&uknots);
			double u;
			while(!knotstream.atEnd())
			{
				 knotstream >> u;
			}
		}

		en = GetElement(n1, QString("Coordinate"));
		qDebug()<<"element"	<<en.tagName()<<en.nodeName();
		if(!en.isNull())
		{
			QString points = en.attribute("point","1,2,3");
			QTextStream pointstream(&points);
			double x,y,z;
			{
				while(!pointstream.atEnd())
				{
					 pointstream >> x>>y>>z;
					 qDebug("%13.6f     %13.6f     %13.6f ",x,y,z);
				}
			}
		}
	}



	//TODO... to be continued
	return false;
}



















