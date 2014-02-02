
/****************************************************************************

	GLLightDlg class
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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


#ifndef GLLIGHTDLG_H
#define GLLIGHTDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QSettings>

class GLLightDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class ThreeDWidget;

public:
    GLLightDlg(QWidget *pParent=NULL);
	void Apply();
	void ReadParams(void);
	void SetDefaults(double size);
	void SetParams(void);
	static bool LoadSettings(QSettings *pSettings);
	static bool SaveSettings(QSettings *pSettings);

	static bool IsLightOn() {return s_bLight;}
	static void SetLightOn(bool bLight) {s_bLight = bLight;}

private:
	void SetupLayout();
	void showEvent(QShowEvent *event);
	void SetEnabled();


private slots:
	void OnSlider(int);
	void OnChanged();
	void OnDefaults();
	void OnLight();

private:
	QSlider *m_pctrlRed;
	QSlider *m_pctrlGreen;
	QSlider *m_pctrlBlue;
	QSlider *m_pctrlAmbient;
	QSlider *m_pctrlDiffuse;
	QSlider *m_pctrlSpecular;
	QSlider *m_pctrlMatAmbient;
	QSlider *m_pctrlMatDiffuse;
	QSlider *m_pctrlMatSpecular;
	QSlider *m_pctrlMatShininess;
	QSlider *m_pctrlMatEmission;
	QSlider *m_pctrlXLight;
	QSlider *m_pctrlYLight;
	QSlider *m_pctrlZLight;
	QCheckBox *m_pctrlColorMaterial;
	QCheckBox *m_pctrlDepthTest;
	QCheckBox *m_pctrlCullFaces;
	QCheckBox *m_pctrlShade;
	QCheckBox *m_pctrlSmooth;
	QCheckBox *m_pctrlLocalView;
	QCheckBox *m_pctrlLight;

	QPushButton *m_pctrlDefaults, *m_pctrlClose;

private:
	void *m_pGL3dBodyDlg;
	void *m_pGL3dWingDlg;
	void *m_p3DWidget;

	static float s_Ambient, s_Diffuse, s_Specular;
	static float s_MatAmbient, s_MatDiffuse, s_MatSpecular, s_MatEmission;
	static float s_Red, s_Green, s_Blue;
	static float s_XLight, s_YLight, s_ZLight;
	static int s_iMatShininess;
	static bool s_bCullFaces, s_bShade, s_bSmooth, s_bLocalView, s_bDepthTest;
	static bool s_bColorMaterial;
	static bool s_bLight;

	double m_Size;

};

#endif // GLLIGHTDLG_H

