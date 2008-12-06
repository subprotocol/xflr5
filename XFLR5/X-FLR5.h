/****************************************************************************

	X-FLR6 Application

    Copyright (C) 2003-2007 André Deperrois XFLR5@yahoo.com

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

// X-FLR5.h : main header file for the X-FLR6 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "misc/Vector.h"
// OpenGL headers
#include <gl\gl.h>
#include <gl\glu.h>

// Globals
//int GetGdiObjectsCount();
//int GetXFLR5processID();

int GetPenWidth(int w, bool IsPrinting);
bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool Intersect(int &x, int &y, CRect &DRect, CPoint &Pt1, CPoint &Pt2);
bool IsEven(int n);
bool Rewind1Line(CStdioFile *pXFile, int &Line, CString &strong);
BOOL ReadAVLString(CStdioFile *pXFile, int &Line, CString &strong);

double sign(double a);

void GetPaperSize(DEVMODE *pDevMode, int &pw, int &ph);
void ExpFormat(double &f, int &exp);
void ReynoldsFormat(CString &str, double f);
void GetSpeedUnit(CString &str, UINT unit);
void GetWeightUnit(CString &str, UINT unit);
void GetAreaUnit(CString &str, UINT unit);
void GetLengthUnit(CString &str, UINT unit);
void GetMomentUnit(CString &str, UINT unit);
void GetForceUnit(CString &str, UINT unit);
void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit,
			  double &mtoUnit, double &m2toUnit, double &mstoUnit,
			  double &kgtoUnit, double &NtoUnit, double &NmtoUnit);
void DecompRGB(COLORREF rgb, double &r, double &g, double &b);
void DecompRGB(COLORREF rgb, UINT &r, UINT &g, UINT &b);
double GLGetRed(double tau);
double GLGetGreen(double tau);
double GLGetBlue(double tau);
void FlushMouse(HWND hWnd);
BOOL bSetupPixelFormat(HDC hDC);
void Trace(CString msg);
void Trace(int n);
void Trace(CString msg, int n);
void Trace(CString msg, double f);
void Trace(CString msg, void *p);

void SetBlackAndWhite(bool bBW);
void GetBWColor(COLORREF &color, int &nStyle, int &nWidth);
void SetGraphDlgPage(int page);
void SetPrintOptions(bool bWindowBk, bool bGraphBk, bool bColor);
int GetGraphDlgPage();
bool IsBlackAndWhite();
bool GetGraphBk();
bool GetWindowBk();
bool GetPrintColor();
bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M);
bool GaussSeidel (double *a, int MatSize, double *b, double *xk, double eps, int IterMax);
bool Gauss(double *A, int n, double *B, int m);
double IntegralC2(double y1, double y2, double c1, double c2);
double IntegralCy(double y1, double y2, double c1, double c2);

// CXFLR5App:
// See X-FLR6.cpp for the implementation of this class
//

class CXFLR5App : public CWinApp
{
public:
	CXFLR5App();
	BOOL FirstInstance(CCommandLineInfo &CmdInfo); 


	CString TraceFileName;
	bool bTrace;
	bool g_bBlackAndWhite;
	bool g_bGraphBk;
	bool g_bWindowBk;
	bool g_bColor;
	int g_GraphDlgPage;
	double m_PenWidth;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	HINSTANCE ExecuteLink();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_ctrlSLink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CRect m_rectText;
	CFont m_fontControl;
	HCURSOR m_hcHand;

protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

extern CXFLR5App theApp;

struct CDoubleRect
{
	public:
		CDoubleRect();
		double left, top, right, bottom;
		bool PtInRect(CVector Real);
		bool PtInRect(double x, double y);
};