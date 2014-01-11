/****************************************************************************

	GLCreateLists 

		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/

#include "GLCreateBodyLists.h"
#include "../../misc/W3dPrefsDlg.h"
#include "../../misc/Settings.h"
#include "../../params.h"

#define NXPOINTS 87
#define NHOOPPOINTS 53

static CVector m_T[(NXPOINTS+1)*(NHOOPPOINTS+1)]; //temporary points to save calculation times for body NURBS surfaces


void GLCreateBody3DSplines( int iList, Body *pBody, int nx, int nh)
{
	int i,j,k,l;
	int p;
	double v;

	CVector Point;
	double hinc, u;
	CVector N, LATB, TALB;
	CVector LA, LB, TA, TB;

	nx = qMin(nx, NXPOINTS);
	nh = qMax(3, nh);
	nh = qMin(nh, NHOOPPOINTS);

	p = 0;
	for (k=0; k<=nx; k++)
	{
		u = (double)k / (double)nx;
		for (l=0; l<=nh; l++)
		{
			v = (double)l / (double)nh;
			pBody->GetPoint(u,  v, true, m_T[p]);
			p++;
		}
	}
	glNewList(iList, GL_COMPILE);
	{
		if(Settings::s_bAlphaChannel && pBody->m_BodyColor.alpha()<255)
		{
			glColor4d(pBody->m_BodyColor.redF(),pBody->m_BodyColor.greenF(),pBody->m_BodyColor.blueF(), pBody->m_BodyColor.alphaF());
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glColor3d(pBody->m_BodyColor.redF(),pBody->m_BodyColor.greenF(),pBody->m_BodyColor.blueF());
			glDisable(GL_BLEND);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(1.0, 1.0);

		//right side first;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];     //	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2];      //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, N.y, N.z);

				glVertex3d(m_T[p].x, m_T[p].y, m_T[p].z);
				glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(m_T[p].x,      m_T[p].y,      m_T[p].z);
					glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}
		//left side next;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];//	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2]; //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, -N.y, N.z);

				glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
				glVertex3d(m_T[p].x,      -m_T[p].y, m_T[p].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, -N.y, N.z);
					glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
					glVertex3d(m_T[p].x,      -m_T[p].y,      m_T[p].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_BLEND);
	}
	glEndList();

	glNewList(iList+MAXBODIES,GL_COMPILE);
	{
		glLineWidth(W3dPrefsDlg::s_OutlineWidth);

		glEnable(GL_LINE_STIPPLE);
		glEnable(GL_DEPTH_TEST);
		if     (W3dPrefsDlg::s_OutlineStyle == Qt::DashLine)       glLineStipple (1, 0xCFCF);
		else if(W3dPrefsDlg::s_OutlineStyle == Qt::DotLine)        glLineStipple (1, 0x6666);
		else if(W3dPrefsDlg::s_OutlineStyle == Qt::DashDotLine)    glLineStipple (1, 0xFF18);
		else if(W3dPrefsDlg::s_OutlineStyle == Qt::DashDotDotLine) glLineStipple (1, 0x7E66);
		else                                                       glLineStipple (1, 0xFFFF);

		glColor3d(W3dPrefsDlg::s_OutlineColor.redF(), W3dPrefsDlg::s_OutlineColor.greenF(), W3dPrefsDlg::s_OutlineColor.blueF());

		hinc=1./(double)(nh-1);
		u=0.0; v = 0.0;

		// sides
		for (i=0; i<pBody->FrameSize(); i++)
		{
			u = pBody->Getu(pBody->getFrame(i)->m_Position.x);

			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					pBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc;
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					pBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.x,Point.y, Point.z);
					v += hinc;
				}
			}
			glEnd();
		}

		//top line
		glBegin(GL_LINE_STRIP);
		{
			v = 0.0;
			for (int iu=0; iu<=nh; iu++)
			{
				pBody->GetPoint((double)iu/(double)nh,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
			}
		}
		glEnd();

		//bottom line
		glBegin(GL_LINE_STRIP);
		{
			v = 1.0;
			for (int iu=0; iu<=nh; iu++)
			{
				pBody->GetPoint((double)iu/(double)nh,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
			}
		}
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();
}



void GLCreateBody3DFlatPanels(int iList, Body *pBody)
{
	int j,k;
	QColor color;
	int style, width;
	CVector P1, P2, P3, P4, N, P1P3, P2P4, Tj, Tjp1;

	glNewList(iList,GL_COMPILE);
	{
		if(Settings::s_bAlphaChannel)
		{
			glColor4d(pBody->m_BodyColor.redF(),pBody->m_BodyColor.greenF(),pBody->m_BodyColor.blueF(), pBody->m_BodyColor.alphaF());
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glColor3d(pBody->m_BodyColor.redF(),pBody->m_BodyColor.greenF(),pBody->m_BodyColor.blueF());
			glDisable (GL_BLEND);
		}
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(1.0, 1.0);

		glDisable(GL_LINE_STIPPLE);
		glLineWidth(1.0);

		for (k=0; k<pBody->SideLineCount()-1;k++)
		{
			for (j=0; j<pBody->FrameSize()-1;j++)
			{
				Tj.Set(pBody->getFrame(j)->m_Position.x,     0.0, 0.0);
				Tjp1.Set(pBody->getFrame(j+1)->m_Position.x, 0.0, 0.0);

				glBegin(GL_QUADS);
				{
					P1 = pBody->getFrame(j)->m_CtrlPoint[k];       P1.x = pBody->getFrame(j)->m_Position.x;
					P2 = pBody->getFrame(j+1)->m_CtrlPoint[k];     P2.x = pBody->getFrame(j+1)->m_Position.x;
					P3 = pBody->getFrame(j+1)->m_CtrlPoint[k+1];   P3.x = pBody->getFrame(j+1)->m_Position.x;
					P4 = pBody->getFrame(j)->m_CtrlPoint[k+1];     P4.x = pBody->getFrame(j)->m_Position.x;

					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);
				}
				glEnd();
				glBegin(GL_QUADS);
				{
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					glNormal3d(N.x,  -N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);
				}
				glEnd();
			}
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	glEndList();

	glNewList(iList+MAXBODIES,GL_COMPILE);
	{
		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		color = W3dPrefsDlg::s_OutlineColor;
		style = W3dPrefsDlg::s_OutlineStyle;
		width = W3dPrefsDlg::s_OutlineWidth;

		glLineWidth(width);

		if     (style == Qt::DashLine)       glLineStipple (1, 0xCFCF);
		else if(style == Qt::DotLine)        glLineStipple (1, 0x6666);
		else if(style == Qt::DashDotLine)    glLineStipple (1, 0xFF18);
		else if(style == Qt::DashDotDotLine) glLineStipple (1, 0x7E66);
		else                             glLineStipple (1, 0xFFFF);

		glColor3d(color.redF(), color.greenF(), color.blueF());

		for (k=0; k<pBody->SideLineCount()-1;k++)
		{
			for (j=0; j<pBody->FrameSize()-1;j++)
			{
				Tj.Set(pBody->getFrame(j)->m_Position.x,     0.0, 0.0);
				Tjp1.Set(pBody->getFrame(j+1)->m_Position.x, 0.0, 0.0);

				glBegin(GL_QUADS);
				{
					P1 = pBody->getFrame(j)->m_CtrlPoint[k];		P1.x = pBody->getFrame(j)->m_Position.x;
					P2 = pBody->getFrame(j+1)->m_CtrlPoint[k];		P2.x = pBody->getFrame(j+1)->m_Position.x;
					P3 = pBody->getFrame(j+1)->m_CtrlPoint[k+1];	P3.x = pBody->getFrame(j+1)->m_Position.x;
					P4 = pBody->getFrame(j)->m_CtrlPoint[k+1];		P4.x = pBody->getFrame(j)->m_Position.x;

					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);
				}
				glEnd();
				glBegin(GL_QUADS);
				{
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					P1P3 = P3-P1;
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);
				}
				glEnd();
			}
		}

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable (GL_LINE_STIPPLE);
        glDisable(GL_DEPTH_TEST);
	}
	glEndList();
}

void GLCreateBodyMesh(int iList, Body *pBody)
{
    if(!pBody) return;

    int i,j,k,l;
    int p, nx, nh;
    double uk, v, dj, dj1, dl1;
    CVector N, LATB, TALB, LA, LB, TA, TB;
    CVector PLA, PLB, PTA, PTB;
    QColor color;

    nx = pBody->m_nxPanels;
    nh = pBody->m_nhPanels;

    if(pBody->m_LineType==BODYPANELTYPE) //LINES
    {
        glNewList(iList,GL_COMPILE);
        {
            glDisable(GL_LINE_STIPPLE);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glPolygonOffset(-1.0, -1.0);

            color = W3dPrefsDlg::s_VLMColor;

            glColor3d(color.redF(),color.greenF(),color.blueF());

            glLineWidth(1.0);

            for (i=0; i<pBody->FrameSize()-1; i++)
            {
                for (j=0; j<pBody->m_xPanels[i]; j++)
                {
                    dj  = (double) j   /(double)(pBody->m_xPanels[i]);
                    dj1 = (double)(j+1)/(double)(pBody->m_xPanels[i]);

                    //body left side
                    for (k=0; k<pBody->SideLineCount()-1; k++)
                    {
                        //build the four corner points of the strips
                        PLB.x =  (1.0- dj) * pBody->getFrame(i)->m_Position.x       +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLB.y = -(1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].y   -  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PLB.z =  (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].z   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PTB.x =  (1.0-dj1) * pBody->getFrame(i)->m_Position.x       + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTB.y = -(1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].y   - dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PTB.z =  (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].z   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PLA.x =  (1.0- dj) * pBody->getFrame(i)->m_Position.x       +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLA.y = -(1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].y -  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PLA.z =  (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].z +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        PTA.x =  (1.0-dj1) * pBody->getFrame(i)->m_Position.x       + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTA.y = -(1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].y - dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PTA.z =  (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].z + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        glBegin(GL_QUAD_STRIP);
                        {
                            N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                            LB = PLB;
                            TB = PTB;
                            glVertex3d(LB.x, LB.y, LB.z);
                            glVertex3d(TB.x, TB.y, TB.z);

                            for (l=0; l<pBody->m_hPanels[k]; l++)
                            {
                                dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
                                LA = PLB * (1.0- dl1) + PLA * dl1;
                                TA = PTB * (1.0- dl1) + PTA * dl1;

                                LATB = TB - LA;
                                TALB = LB - TA;
                                N = TALB * LATB;
                                N.Normalize();

                                glNormal3d(N.x, N.y, N.z);
                                glVertex3d(LA.x, LA.y, LA.z);
                                glVertex3d(TA.x, TA.y, TA.z);
                                TB = TA;
                                LB = LA;
                            }
                        }
                        glEnd();
                    }
                    //body right side
                    for (k=pBody->SideLineCount()-2; k>=0; k--)
                    {
                        //build the four corner points of the strips
                        PLA.x = (1.0- dj) * pBody->getFrame(i)->m_Position.x     +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLA.y = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].y   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PLA.z = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].z   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PTA.x = (1.0-dj1) * pBody->getFrame(i)->m_Position.x     + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTA.y = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].y   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PTA.z = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].z   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PLB.x = (1.0- dj) * pBody->getFrame(i)->m_Position.x     +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLB.y = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].y +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PLB.z = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].z +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        PTB.x = (1.0-dj1) * pBody->getFrame(i)->m_Position.x     + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTB.y = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].y + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PTB.z = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].z + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        glBegin(GL_QUAD_STRIP);
                        {
                            N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                            LB = PLB;
                            TB = PTB;
                            glVertex3d(LB.x, LB.y, LB.z);
                            glVertex3d(TB.x, TB.y, TB.z);

                            for (l=0; l<pBody->m_hPanels[k]; l++)
                            {
                                dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
                                LA = PLB * (1.0- dl1) + PLA * dl1;
                                TA = PTB * (1.0- dl1) + PTA * dl1;

                                LATB = TB - LA;
                                TALB = LB - TA;
                                N = TALB * LATB;
                                N.Normalize();

                                glNormal3d(N.x, N.y, N.z);
                                glVertex3d(LA.x, LA.y, LA.z);
                                glVertex3d(TA.x, TA.y, TA.z);
                                TB = TA;
                                LB = LA;
                            }
                        }
                        glEnd();
                    }
                }
            }
            glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        glEndList();
        glNewList(iList+MAXBODIES,GL_COMPILE);
        {
            glDisable (GL_LINE_STIPPLE);
            glEnable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 1.0);

		  color = Settings::s_BackgroundColor;
            glColor3d(color.redF(), color.greenF(), color.blueF());

            glLineWidth(1.0);

            for (i=0; i<pBody->FrameSize()-1; i++)
            {
                for (j=0; j<pBody->m_xPanels[i]; j++)
                {
                    dj  = (double) j   /(double)(pBody->m_xPanels[i]);
                    dj1 = (double)(j+1)/(double)(pBody->m_xPanels[i]);

                    //body left side
                    for (k=0; k<pBody->SideLineCount()-1; k++)
                    {
                        //build the four corner points of the strips
                        PLB.x =  (1.0- dj) * pBody->getFrame(i)->m_Position.x        +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLB.y = -(1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].y   -  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PLB.z =  (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].z   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PTB.x =  (1.0-dj1) * pBody->getFrame(i)->m_Position.x        + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTB.y = -(1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].y   - dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PTB.z =  (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].z   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PLA.x =  (1.0- dj) * pBody->getFrame(i)->m_Position.x        +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLA.y = -(1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].y -  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PLA.z =  (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].z +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        PTA.x =  (1.0-dj1) * pBody->getFrame(i)->m_Position.x        + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTA.y = -(1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].y - dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PTA.z =  (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].z + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        glBegin(GL_QUAD_STRIP);
                        {
                            N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                            LB = PLB;
                            TB = PTB;
                            glVertex3d(LB.x, LB.y, LB.z);
                            glVertex3d(TB.x, TB.y, TB.z);

                            for (l=0; l<pBody->m_hPanels[k]; l++)
                            {
                                dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
                                LA = PLB * (1.0- dl1) + PLA * dl1;
                                TA = PTB * (1.0- dl1) + PTA * dl1;

                                LATB = TB - LA;
                                TALB = LB - TA;
                                N = TALB * LATB;
                                N.Normalize();

                                glNormal3d(N.x, N.y, N.z);
                                glVertex3d(LA.x, LA.y, LA.z);
                                glVertex3d(TA.x, TA.y, TA.z);
                                TB = TA;
                                LB = LA;
                            }
                        }
                        glEnd();
                    }
                    //body right side
                    for (k=pBody->SideLineCount()-2; k>=0; k--)
                    {
                        //build the four corner points of the strips
                        PLA.x = (1.0- dj) * pBody->getFrame(i)->m_Position.x        +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLA.y = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].y   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PLA.z = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k].z   +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PTA.x = (1.0-dj1) * pBody->getFrame(i)->m_Position.x        + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTA.y = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].y   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].y;
                        PTA.z = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k].z   + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k].z;

                        PLB.x = (1.0- dj) * pBody->getFrame(i)->m_Position.x        +  dj * pBody->getFrame(i+1)->m_Position.x;
                        PLB.y = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].y +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PLB.z = (1.0- dj) * pBody->getFrame(i)->m_CtrlPoint[k+1].z +  dj * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        PTB.x = (1.0-dj1) * pBody->getFrame(i)->m_Position.x        + dj1 * pBody->getFrame(i+1)->m_Position.x;
                        PTB.y = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].y + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].y;
                        PTB.z = (1.0-dj1) * pBody->getFrame(i)->m_CtrlPoint[k+1].z + dj1 * pBody->getFrame(i+1)->m_CtrlPoint[k+1].z;

                        glBegin(GL_QUAD_STRIP);
                        {
                            N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                            LB = PLB;
                            TB = PTB;
                            glVertex3d(LB.x, LB.y, LB.z);
                            glVertex3d(TB.x, TB.y, TB.z);

                            for (l=0; l<pBody->m_hPanels[k]; l++)
                            {
                                dl1  = (double) (l+1)   /(double)(pBody->m_hPanels[k]);
                                LA = PLB * (1.0- dl1) + PLA * dl1;
                                TA = PTB * (1.0- dl1) + PTA * dl1;

                                LATB = TB - LA;
                                TALB = LB - TA;
                                N = TALB * LATB;
                                N.Normalize();

                                glNormal3d(N.x, N.y, N.z);
                                glVertex3d(LA.x, LA.y, LA.z);
                                glVertex3d(TA.x, TA.y, TA.z);
                                TB = TA;
                                LB = LA;
                            }
                        }
                        glEnd();
                    }
                }
            }
            glDisable (GL_LINE_STIPPLE);
        }
        glEndList();
    }
    else if(pBody->m_LineType==BODYSPLINETYPE) //NURBS
    {
        pBody->SetPanelPos();
        p = 0;
        for (k=0; k<=nx; k++)
        {
            uk  = pBody->s_XPanelPos[k];
            for (l=0; l<=nh; l++)
            {
                v = (double)l / (double)(nh);
                pBody->GetPoint(uk,  v, true, m_T[p]);
                p++;
            }
        }
        glNewList(iList,GL_COMPILE);
        {
            glDisable (GL_LINE_STIPPLE);
            glEnable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            color = W3dPrefsDlg::s_VLMColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

            glColor3d(color.redF(), color.greenF(), color.blueF());

            glLineWidth(1.0);

            //left side first;
            p=0;

            for (k=0; k<nx; k++)
            {
                glBegin(GL_QUAD_STRIP);
                {
                    N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                    glVertex3d(m_T[p].x, m_T[p].y, m_T[p].z);
                    glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);

                    p++;

                    for (l=1; l<=nh; l++)
                    {
                        glVertex3d(m_T[p].x,      m_T[p].y,      m_T[p].z);
                        glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);
                        TB = TA;
                        LB = LA;
                        p++;
                    }
                }
                glEnd();
            }

            //right side next;
            p=0;
            for (k=0; k<nx; k++)
            {
                glBegin(GL_QUAD_STRIP);
                {
                    N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                    glVertex3d(m_T[p].x,      -m_T[p].y, m_T[p].z);
                    glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);

                    p++;

                    for (l=1; l<=nh; l++)
                    {
                        glVertex3d(m_T[p].x,      -m_T[p].y,      m_T[p].z);
                        glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
                        TB = TA;
                        LB = LA;
                        p++;
                    }
                }
                glEnd();
            }
        }
        glEndList();

        glNewList(iList+MAXBODIES,GL_COMPILE);
        {
            glEnable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 1.0);
            glDisable (GL_LINE_STIPPLE);

		  color = Settings::s_BackgroundColor;
//			style = W3dPrefsDlg::s_VLMStyle;
//			width = W3dPrefsDlg::s_VLMWidth;

            glColor3d(color.redF(), color.greenF(), color.blueF());

            glLineWidth(1.0);


            //left side first;
            p=0;
            for (k=0; k<nx; k++)
            {
                glBegin(GL_QUAD_STRIP);
                {
                    N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                    glVertex3d(m_T[p].x, m_T[p].y, m_T[p].z);
                    glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);

                    p++;

                    for (l=1; l<=nh; l++)
                    {
/*						LATB = m_T[p+nh] - m_T[p];     //					LATB = TB - LA;
                        TALB = m_T[p-1]  - m_T[p+nh+1];//					TALB = LB - TA;
                        N = TALB * LATB;
                        N.Normalize();

                        glNormal3d(N.x, N.y, N.z);*/
                        glVertex3d(m_T[p].x,      m_T[p].y,      m_T[p].z);
                        glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);
                        TB = TA;
                        LB = LA;
                        p++;
                    }
                }
                glEnd();
            }
            //right side next;
            p=0;
            for (k=0; k<nx; k++)
            {
                glBegin(GL_QUAD_STRIP);
                {
                    N.Set(0.0, 0.0, 1.0);//top line normal is vertical

                    glVertex3d(m_T[p].x,      -m_T[p].y, m_T[p].z);
                    glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);

                    p++;

                    for (l=1; l<=nh; l++)
                    {
/*						LATB = m_T[p+nh] - m_T[p];     //					LATB = TB - LA;
                        TALB = m_T[p-1]  - m_T[p+nh+1];//					TALB = LB - TA;
                        N = TALB * LATB;
                        N.Normalize();

                        glNormal3d(N.x, -N.y, N.z);*/
                        glVertex3d(m_T[p].x,      -m_T[p].y,      m_T[p].z);
                        glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
                        TB = TA;
                        LB = LA;
                        p++;
                    }
                }
                glEnd();
            }
        }
        glEndList();
    }
}




 
