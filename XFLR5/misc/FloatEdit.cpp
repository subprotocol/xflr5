/****************************************************************************

    FloatEdit Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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

// FloatEdit.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "FloatEdit.h"
#include <math.h>



/////////////////////////////////////////////////////////////////////////////
// CFloatEdit

CFloatEdit::CFloatEdit()
{
	m_fMin = -1.e10;
	m_fMax =  1.e10;
	m_iPrecision = 2;
}

CFloatEdit::~CFloatEdit()
{
}


BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
	//{{AFX_MSG_MAP(CFloatEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloatEdit message handlers

void CFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar){
		case 8:{ //BackSpace
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
			}
		
		case ' ': return;
		default: {
			if (nChar != 45 &&
				nChar != 46 &&
				nChar !=101 &&
				!(nChar>=48 && nChar<=57)) return;
			CString oldstr;
			GetWindowText(oldstr);
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			if (!IsValid())	SetWindowText(oldstr);
		 }
	}
}

bool CFloatEdit::IsValid()
{
	CString str;
	GetWindowText(str);
	str.Replace(" ","");
	double f;
	char lp[10];
	if (str=="." || str=="-" || str=="-." || sscanf(str, "%lf%s", &f, lp) == 1){
		return true;
	}
	str.Empty();
	return false;
}

double CFloatEdit::GetValue()
{
	double f;
	if (IsValid())	{
		CString str;
		GetWindowText(str);
		str.Replace(" ","");
		sscanf(str, "%lf", &f);
		return f;
	}
	else return 0.0;//perfectible...
}

bool CFloatEdit::GetValue(double &f)
{
	if (IsValid())	{
		CString str;

		GetWindowText(str);
		str.Replace(" ","");
		sscanf(str, "%lf", &f);
		return true;
	}
	else return false;
}

void CFloatEdit::SetValue(double f)
{
 	CString str, str1;
	int q, r, exp, i, pos, l;

	if ((f==0.0 || abs(f)>pow(10.0, -m_iPrecision)) && f <1000.0)
	{
		switch (m_iPrecision)
		{
			//there probably is a more elegant way to do this,
			case 0:
				str.Format("%.0f", f);
				break;
			case 1:
				str.Format("%.1f", f);
				break;
			case 2:
				str.Format("%.2f", f);
				break;
			case 3:
				str.Format("%.3f", f);
				break;
			case 4:
				str.Format("%.4f", f);
				break;
			case 5:
				str.Format("%.5f", f);
				break;
			case 6:
				str.Format("%.6f", f);
				break;
			case 7:
				str.Format("%.7f", f);
				break;
			case 8:
				str.Format("%.8f", f);
				break;
			case 9:
				str.Format("%.9f", f);
				break;
			default:
				str.Format("%.2f", f);
		}
	}
	else if(f>=1000.0)
	{
		exp = (int)log10(f);
		r = exp%3;
		q = (exp-r)/3;

		CString strong;
		strong.Format("%.0f", f);

		switch (m_iPrecision)
		{

			//there probably is a more elegant way to do this,
			case 0:
				strong.Format("%.0f", f);
				break;
			case 1:
				strong.Format("%.1f", f);
				break;
			case 2:
				strong.Format("%.2f", f);
				break;
			case 3:
				strong.Format("%.3f", f);
				break;
			case 4:
				strong.Format("%.4f", f);
				break;
			case 5:
				strong.Format("%.5f", f);
				break;
			case 6:
				strong.Format("%.6f", f);
				break;
			case 7:
				strong.Format("%.7f", f);
				break;
			case 8:
				strong.Format("%.8f", f);
				break;
			case 9:
				strong.Format("%.9f", f);
				break;
			default:
				strong.Format("%.2f", f);
		}

		l = strong.GetLength();
		pos = strong.Find(".");
		if(pos<0) pos = l;
		pos = l-pos;

		for (i=0; i<q; i++)
		{
			strong.Insert(l-3*(i+1)-i-pos," ");
			l++;
		}

		str = strong;
	}
	else 
	{
		exp  = (int)log10(f)-1;
		str1.Format("e%d", exp);

		double main = (float)(f/pow(10.0, exp)*1.000001);
		switch (m_iPrecision)
		{
			case 0:
				str.Format("%.0f", main);
				break;
			case 1:
				str.Format("%.1f", main);
				break;
			case 2:
				str.Format("%.2f", main);
				break;
			case 3:
				str.Format("%.3f", main);
				break;
			case 4:
				str.Format("%.4f", main);
				break;
			case 5:
				str.Format("%.5f", main);
				break;
			default:
				str.Format("%.2f", main);
		}
		str += str1;
	}
 	SetWindowText(str);
}



void CFloatEdit::OnSetFocus() 
{
	//save initial text just in case
	GetWindowText(m_iniStr);
	m_iniStr.Replace(" ","");
	SetWindowText(m_iniStr);
	SetSel(0,-1);
}

bool CFloatEdit::CheckBeforeExit()
{
	double fValue;
	bool bValid = GetValue(fValue);
	if (!bValid || fValue<m_fMin || fValue> m_fMax){
		return false;
	}
	return true;
}

void CFloatEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	
	if(!CheckBeforeExit()) 
		SetWindowText(m_iniStr);//user blundered
	else {
		double fValue = GetValue();
		SetValue(fValue);
	}
}

void CFloatEdit::Clear()
{
	SetWindowText(" ");
}

void CFloatEdit::SetPrecision(int i)
{
	m_iPrecision = i;
}

void CFloatEdit::SetMin(double f)
{
	m_fMin = f;
}

void CFloatEdit::SetMax(double f)
{
	m_fMax = f;
}

BOOL CFloatEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && GetFocus() == this){ 
		GetParent()->PostMessage(WM_KEYDOWN, VK_ESCAPE);
		return true; // no need to process further
	} 
	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetFocus() == this){ 
		if(!CheckBeforeExit()) 
			SetWindowText(m_iniStr);//user blundered
		GetParent()->PostMessage(WM_KEYDOWN, VK_RETURN);
		return true;// no need to process further
	}
	return CEdit::PreTranslateMessage(pMsg);
}

