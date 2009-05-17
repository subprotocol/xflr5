/****************************************************************************

	QXInverse Class
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

 
#ifndef QXINVERSE_H
#define QXINVERSE_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "XInverse.h"
#include "../Misc/FloatEdit.h"
#include "../XDirect/XFoil.h"
#include "../Graph/QGraph.h"
#include "../Objects/Spline.h"


class QXInverse : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class TwoDWidget;
	friend class InverseOptionsDlg;
	friend class FoilSelectionDlg;

public:
	QXInverse(QWidget *parent = NULL);
	void SetupLayout();
	void InitDialog();
 
private slots:
	void OnCpxx();
	void OnInverseApp();
	void OnMarkSegment();
	void OnGraphOptions();
	void OnInverseStyles();
	void OnTangentSpline();
	void OnShowSpline();
	void OnNewSpline();
	void OnApplySpline();
	void OnSpecal();
	void OnQReset();
	void OnFilter();
	void OnSmooth();
	void OnSymm() ;
	void OnExecute();
	void OnStoreFoil();
	void OnExtractFoil();
	void OnResetFoilScale();
	void OnInsertCtrlPt();
	void OnRemoveCtrlPt();
	void OnQInitial();
	void OnQSpec();
	void OnQViscous();
	void OnQPoints();
	void OnQReflected();
	void OnPertubate();


private:
	void UpdateView();

	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	void CheckActions();
	void DrawGrid(QPainter &painter, double scale);
	void PaintView(QPainter &painter);
	void PaintGraph(QPainter &painter);
	void PaintFoil(QPainter &painter);
	void SetScale(QRect CltRect);
	void ResetQ();
	void ResetScale();
	void ResetMixedQ();
	void ReleaseZoom();
	void Smooth(int Pos1 = -1, int Pos2 = -1);
	void Clear();
	void Connect();
	void CreateMCurve();
	void CreateQCurve();
	void CancelMark();
	void CancelSpline();
	void CancelSmooth();
	void SetFoil();
	void SetTAngle(double a);
	void SetTGap(double tr, double ti);
//	void SetRect(QRect CltRect);
	void ExecMDES();
	void LoadSettings(QDataStream &ar);
	void SaveSettings(QDataStream &ar);

	bool ExecQDES();
	bool SetParams();
	bool InitXFoil(CFoil * pFoil);


	double qincom(double qc, double qinf, double tklam);

private:
	QRadioButton *m_pctrlFullInverse, *m_pctrlMixedInverse;
	QTextEdit *m_pctrlOutput;
	QLabel *m_pctrlSpecif;
	QRadioButton *m_pctrlSpecAlpha, *m_pctrlSpecCl;
	QPushButton *m_pctrlExec, *m_pctrlFilter, *m_pctrlPert, *m_pctrlApplySpline, *m_pctrlNewSpline, *m_pctrlResetQSpec, *m_pctrlSmooth;
	QCheckBox *m_pctrlShowSpline, *m_pctrlTangentSpline, *m_pctrlSymm;
	FloatEdit *m_pctrlSpec, *m_pctrlFilterParam, *m_pctrlTGapy, *m_pctrlTGapx, *m_pctrlTAngle;

	QTextEdit *m_pctrlMSpec, *m_pctrlMOutput;
	FloatEdit *m_pctrlIter;
	QPushButton *m_pctrlMark;
	QCheckBox *m_pctrlCpxx;

	QCheckBox *m_pctrlMShowSpline, *m_pctrlMTangentSpline;
	QPushButton *m_pctrlMExec, *m_pctrlMNewSpline, *m_pctrlMApplySpline, *m_pctrlMSmooth, *m_pctrlMResetQSpec;

	QWidget *m_pctrlMInvWidget,*m_pctrlFInvWidget;
	QStackedWidget *m_pctrlStackedInv;

	void * m_p2DWidget;
	void * m_pMainFrame;

	void *m_pXFoil;

	QList<void *> *m_poaFoil;

	CFoil* m_pCurFoil;
	CFoil* m_pRefFoil;
	CFoil* m_pModFoil;

	CSpline m_Spline;

	double pi;

	bool m_bLoaded;
	bool m_bTrans;
	bool m_bModFoil, m_bRefFoil;
	bool m_bGetPos;
	bool m_bMark, m_bMarked;
	bool m_bSpline, m_bSplined;
	bool m_bSmooth;
	bool m_bZoomPlus;
	bool m_bFullInverse;// mixed inverse if false
	bool m_bSaved;
	bool m_bReflected;
	bool m_bShowPoints;
	bool m_bTransGraph;
	bool m_bRefCurves;
	bool m_bTangentSpline;


	int m_Mk1, m_Mk2;
	int m_tmpPos, m_Pos1, m_Pos2, m_nPos, m_SplineLeftPos, m_SplineRightPos;
	int m_LogPixelsY;
	double xd, yd;
	double xu, yu;
	double m_fScale, m_fRefScale;

	int m_ReflectedStyle, m_ReflectedWidth;
	QColor m_ReflectedClr;

	QGraph m_QGraph;
	CCurve* m_pQCurve;
	CCurve* m_pQVCurve;
	CCurve* m_pMCurve;
	CCurve* m_pReflectedCurve;


	QRect m_rCltRect, m_rGraphRect;
	QRect m_ZoomRect;

	QPoint m_PointDown;
	QPoint m_ptPopUp;
	QPoint m_ptOffset;

// temporary allocations
	QPoint tanpt;
	QPoint P0, P1, P2;

};

#endif // QXINVERSE_H



