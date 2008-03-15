/****************************************************************************

    CGLLight Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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
// GLLight.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "Miarex.h"
#include ".\gllight.h"


/////////////////////////////////////////////////////////////////////////////
// CGLLight dialog


CGLLight::CGLLight(CWnd* pParent /*=NULL*/)
	: CDialog(CGLLight::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGLLight)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCullFaces = false;
	m_bShade     = true;
	m_bSmooth    = true;
	m_bLocalView = true;
	m_bDepthTest = true;
	m_bColorMaterial = true;


	m_Red   = 1.0f;
	m_Green = 1.0f;
	m_Blue  = 1.0f;

	m_Ambient      = 0.25f;
	m_Diffuse      = 0.58f;
	m_Specular     = 0.05f;

	m_MatAmbient   = -0.51f;
	m_MatDiffuse   = -0.43f;
	m_MatSpecular  = -0.08f;
	m_MatEmission  = -0.04f;
	m_iMatShininess = 0;

	m_XLight   =  0.56f;
	m_YLight   =  0.02f;
	m_ZLight   =  0.68f;

}


void CGLLight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGLLight)
	DDX_Control(pDX, IDC_RED, m_ctrlRed);
	DDX_Control(pDX, IDC_GREEN, m_ctrlGreen);
	DDX_Control(pDX, IDC_BLUE, m_ctrlBlue);
	DDX_Control(pDX, IDC_AMBIENT, m_ctrlAmbient);
	DDX_Control(pDX, IDC_DIFFUSE, m_ctrlDiffuse);
	DDX_Control(pDX, IDC_SPECULAR, m_ctrlSpecular);
	DDX_Control(pDX, IDC_MATAMBIENT, m_ctrlMatAmbient);
	DDX_Control(pDX, IDC_MATSPECULAR, m_ctrlMatSpecular);
	DDX_Control(pDX, IDC_MATSHININESS, m_ctrlMatShininess);
	DDX_Control(pDX, IDC_MATDIFFUSE, m_ctrlMatDiffuse);
	DDX_Control(pDX, IDC_MATEMISSION, m_ctrlMatEmission);
	DDX_Control(pDX, IDC_XLIGHT, m_ctrlXLight);
	DDX_Control(pDX, IDC_YLIGHT, m_ctrlYLight);
	DDX_Control(pDX, IDC_ZLIGHT, m_ctrlZLight);
	DDX_Control(pDX, IDC_CULLFACES, m_ctrlCullFaces);
	DDX_Control(pDX, IDC_COLORMATERIAL, m_ctrlColorMaterial);
	DDX_Control(pDX, IDC_DEPTHTEST, m_ctrlDepthTest);
	DDX_Control(pDX, IDC_SHADE, m_ctrlShade);
	DDX_Control(pDX, IDC_SMOOTH, m_ctrlSmooth);
	DDX_Control(pDX, IDC_LOCALVIEW, m_ctrlLocalView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGLLight, CDialog)
	//{{AFX_MSG_MAP(CGLLight)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_DEPTHTEST, OnChanged)
	ON_BN_CLICKED(IDC_CULLFACES, OnChanged)
	ON_BN_CLICKED(IDC_COLORMATERIAL, OnColorMaterial)
	ON_BN_CLICKED(IDC_SHADE, OnChanged)
	ON_BN_CLICKED(IDC_SMOOTH, OnChanged)
	ON_BN_CLICKED(IDC_LOCALVIEW, OnChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLLight message handlers
BOOL CGLLight::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect WndRect;
	GetWindowRect(WndRect);
	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-WndRect.Width()-10,60,0,0,SWP_NOSIZE);

	m_ctrlRed.SetRange(0,100);
	m_ctrlRed.SetTicFreq(5);
	m_ctrlGreen.SetRange(0,100);
	m_ctrlGreen.SetTicFreq(5);
	m_ctrlBlue.SetRange(0,100);
	m_ctrlBlue.SetTicFreq(5);

	m_ctrlAmbient.SetRange(0,100);
	m_ctrlAmbient.SetTicFreq(5);
	m_ctrlDiffuse.SetRange(0,100);
	m_ctrlDiffuse.SetTicFreq(5);
	m_ctrlSpecular.SetRange(0,100);
	m_ctrlSpecular.SetTicFreq(5);

	m_ctrlMatAmbient.SetRange(-100,100);
	m_ctrlMatAmbient.SetTicFreq(10);
	m_ctrlMatDiffuse.SetRange(-100,100);
	m_ctrlMatDiffuse.SetTicFreq(10);
	m_ctrlMatSpecular.SetRange(-100,100);
	m_ctrlMatSpecular.SetTicFreq(10);
	m_ctrlMatEmission.SetRange(-100,100);
	m_ctrlMatEmission.SetTicFreq(10);
	m_ctrlMatShininess.SetRange(0,128);
	m_ctrlMatShininess.SetTicFreq(12);

	m_ctrlXLight.SetRange(0,100);
	m_ctrlXLight.SetTicFreq(5);
	m_ctrlYLight.SetRange(0,100);
	m_ctrlYLight.SetTicFreq(5);
	m_ctrlZLight.SetRange(0,100);
	m_ctrlZLight.SetTicFreq(5);

	SetParams();
	m_ctrlAmbient.SetFocus();
	return FALSE; 
}



bool CGLLight::LoadSettings(CArchive &ar)
{
	int k;
	//  we're reading/loading

	try{
		ar >> m_Ambient;
		ar >> m_Diffuse;
		ar >> m_Specular;
		if (m_Ambient   <-100.0 || m_Ambient >100.0 ||
			m_Diffuse   <-100.0 || m_Diffuse >100.0 ||
			m_Specular  <-100.0 || m_Specular >100.0 )
		{
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
			return false;
		}
		ar >> m_MatAmbient >> m_MatDiffuse >> m_MatSpecular  >> m_MatEmission;
		ar >> m_iMatShininess;
		if (m_MatAmbient   <-100.0 || m_MatAmbient >100.0 ||
			m_MatDiffuse   <-100.0 || m_MatDiffuse >100.0 ||
			m_MatSpecular  <-100.0 || m_MatSpecular >100.0 ||
			m_iMatShininess <-100  || m_iMatShininess >200 ||
			m_MatEmission  <-100.0 || m_MatEmission >100.0)
		{
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
			return false;
		}

		ar >> m_XLight >> m_YLight >> m_ZLight;
		if (m_XLight <-100.0 || m_XLight >100.0 ||
			m_YLight <-100.0 || m_YLight >100.0 ||
			m_ZLight <-100.0 || m_ZLight >100.0){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
			return false;
		}
	
		ar >> m_Red >> m_Green >> m_Blue;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bCullFaces = true; else m_bCullFaces = false;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bSmooth = true; else m_bSmooth = false;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bShade = true; else m_bShade = false;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bLocalView = true; else m_bLocalView = false;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bDepthTest = true; else m_bDepthTest = false;

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bColorMaterial = true; else m_bColorMaterial = false;

		return true;
	}
	catch (CArchiveException *ex){
		ex->Delete();
		return false;
	}
	catch (CException *ex){
		ex->Delete();
		return false;
	}	
}
 
bool CGLLight::SaveSettings(CArchive &ar)
{
	//  we're reading/loading

	ar << m_Ambient << m_Diffuse << m_Specular;
	ar << m_MatAmbient << m_MatDiffuse << m_MatSpecular << m_MatEmission;
	ar << m_iMatShininess;
	ar << m_XLight << m_YLight << m_ZLight;
	ar << m_Red << m_Green << m_Blue;

	if(m_bCullFaces)		ar << 1; else ar << 0;
	if(m_bSmooth)			ar << 1; else ar << 0;
	if(m_bShade)			ar << 1; else ar << 0;
	if(m_bLocalView)		ar << 1; else ar << 0;
	if(m_bDepthTest)		ar << 1; else ar << 0;
	if(m_bColorMaterial)	ar << 1; else ar << 0;
	return true;
}

void CGLLight::Apply() 
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	ReadParams();
	pMiarex->UpdateView();
}

void CGLLight::OnDefaults()
{
	m_bCullFaces = false;
	m_bShade     = true;
	m_bSmooth    = true;
	m_bLocalView = true;
	m_bDepthTest = true;
	m_bColorMaterial = true;

	m_Red   = 1.0f;
	m_Green = 1.0f;
	m_Blue  = 1.0f;

	m_Ambient      = 0.25f;
	m_Diffuse      = 0.58f;
	m_Specular     = 0.05f;

	m_MatAmbient   = -0.51f;
	m_MatDiffuse   = -0.43f;
	m_MatSpecular  = -0.08f;
	m_MatEmission  = -0.04f;
	m_iMatShininess = 0;

	m_XLight   =  0.56f;
	m_YLight   =  0.02f;
	m_ZLight   =  0.68f;

	SetParams();

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->UpdateView();
}

void CGLLight::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Apply();
}

void CGLLight::ReadParams(void)
{
	m_Red     = (float)m_ctrlRed.GetPos()    /100.0f;
	m_Green   = (float)m_ctrlGreen.GetPos()  /100.0f;
	m_Blue    = (float)m_ctrlBlue.GetPos()   /100.0f;

	m_Ambient     = (float)m_ctrlAmbient.GetPos()    / 100.0f;
	m_Diffuse     = (float)m_ctrlDiffuse.GetPos()    / 100.0f;
	m_Specular    = (float)m_ctrlSpecular.GetPos()   / 100.0f;

	m_MatAmbient    = (float)m_ctrlMatAmbient.GetPos()    /100.0f;
	m_MatSpecular   = (float)m_ctrlMatSpecular.GetPos()   /100.0f;
	m_MatDiffuse    = (float)m_ctrlMatDiffuse.GetPos()    /100.0f;
	m_MatEmission   = (float)m_ctrlMatEmission.GetPos()   /100.0f;
	m_iMatShininess = m_ctrlMatShininess.GetPos();

	if(m_ctrlCullFaces.GetCheck())		m_bCullFaces = true;
	else								m_bCullFaces = false;
	if(m_ctrlColorMaterial.GetCheck())	m_bColorMaterial = true;
	else								m_bColorMaterial = false;
	if(m_ctrlSmooth.GetCheck())			m_bSmooth    = true;
	else								m_bSmooth    = false;
	if(m_ctrlShade.GetCheck())			m_bShade     = true;
	else								m_bShade     = false;
	if(m_ctrlLocalView.GetCheck())		m_bLocalView = true;
	else								m_bLocalView = false;
	if(m_ctrlDepthTest.GetCheck())		m_bDepthTest = true;
	else								m_bDepthTest = false;

	float factor = 50.0f; 
	m_XLight  = ((float)m_ctrlXLight.GetPos()-50.0f)/factor;
	m_YLight  = ((float)m_ctrlYLight.GetPos()-50.0f)/factor;
	m_ZLight  = ((float)m_ctrlZLight.GetPos()-50.0f)/factor;
}


void CGLLight::SetParams(void)
{
	float factor = 50.0f;
	m_ctrlXLight.SetPos((int)((m_XLight+1.0)*factor));
	m_ctrlYLight.SetPos((int)((m_YLight+1.0)*factor));
	m_ctrlZLight.SetPos((int)((m_ZLight+1.0)*factor));

	m_ctrlRed.SetPos(  (int)(m_Red*100.0));
	m_ctrlGreen.SetPos((int)(m_Green*100.0));
	m_ctrlBlue.SetPos( (int)(m_Blue*100.0));

	m_ctrlAmbient.SetPos(   (int)(m_Ambient*100.0));
	m_ctrlDiffuse.SetPos(   (int)(m_Diffuse*100.0));
	m_ctrlSpecular.SetPos(  (int)(m_Specular*100.0));

	m_ctrlMatAmbient.SetPos(   (int)(m_MatAmbient*100.0));
	m_ctrlMatDiffuse.SetPos(   (int)(m_MatDiffuse*100.0));
	m_ctrlMatSpecular.SetPos(  (int)(m_MatSpecular*100.0));
	m_ctrlMatEmission.SetPos(  (int)(m_MatEmission*100.0));
	m_ctrlMatShininess.SetPos(m_iMatShininess);

	if(m_bCullFaces) m_ctrlCullFaces.SetCheck(TRUE); else m_ctrlCullFaces.SetCheck(FALSE);
	if(m_bSmooth)    m_ctrlSmooth.SetCheck(TRUE);    else m_ctrlSmooth.SetCheck(FALSE);
	if(m_bShade)     m_ctrlShade.SetCheck(TRUE);     else m_ctrlShade.SetCheck(FALSE);
	if(m_bLocalView) m_ctrlLocalView.SetCheck(TRUE); else m_ctrlLocalView.SetCheck(FALSE);
	if(m_bDepthTest) m_ctrlDepthTest.SetCheck(TRUE); else m_ctrlDepthTest.SetCheck(FALSE);

	if(m_bColorMaterial) m_ctrlColorMaterial.SetCheck(TRUE); else m_ctrlColorMaterial.SetCheck(FALSE);
/*	m_ctrlMatAmbient.EnableWindow(!m_bColorMaterial);
	m_ctrlMatDiffuse.EnableWindow(!m_bColorMaterial);
	m_ctrlMatSpecular.EnableWindow(!m_bColorMaterial);
	m_ctrlMatShininess.EnableWindow(!m_bColorMaterial);
	m_ctrlMatEmission.EnableWindow(!m_bColorMaterial);*/
}


void CGLLight::OnChanged()
{
	Apply();
}

void CGLLight::OnColorMaterial()
{
	if(m_ctrlColorMaterial.GetCheck())	m_bColorMaterial = true;
	else								m_bColorMaterial = false;
//	m_ctrlMatAmbient.EnableWindow(!m_bColorMaterial);
//	m_ctrlMatDiffuse.EnableWindow(!m_bColorMaterial);
//	m_ctrlMatSpecular.EnableWindow(!m_bColorMaterial);
//	m_ctrlMatShininess.EnableWindow(!m_bColorMaterial);
//	m_ctrlMatEmission.EnableWindow(!m_bColorMaterial);
	Apply();
}