
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
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;

public:
    GLLightDlg();
	void InitDialog();
	void Apply();
	void ReadParams(void);
	void SetParams(void);
	bool LoadSettings(QSettings *pSettings);
	bool SaveSettings(QSettings *pSettings);

private:
	void SetupLayout();
	void showEvent(QShowEvent *event);
	void SetEnabled();


private slots:
	void OnSlider(int pos);
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
	void *m_pMiarex;

	float m_Ambient, m_Diffuse, m_Specular;
	float m_MatAmbient, m_MatDiffuse, m_MatSpecular, m_MatEmission;
	float m_Red, m_Green, m_Blue;
	float m_XLight, m_YLight, m_ZLight;
	int m_iMatShininess;
	bool m_bCullFaces, m_bShade, m_bSmooth, m_bLocalView, m_bDepthTest;
	bool m_bColorMaterial;
	bool m_bLight;

public:

};

#endif // GLLIGHTDLG_H

