/****************************************************************************

	AboutQ5 Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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

//Global functions


#include "Globals.h"
#include <QPen>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime> 
#include <QByteArray>
#include <math.h>
#include <QtDebug>

void ExpFormat(double &f, int &exp)
{
	if (f==0.0)
	{
		exp = 0;
		f = 0.0;
		return;
	}
	double f1 = fabs(f);
//	int sgn = int(f/f1);
	if(f1<1)
		exp = (int)log10(f1)-1;
	else
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);

}



void GetAreaUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm2";
			break;
		}
		case 1:
		{
			str="cm2";
			break;
		}
		case 2:
		{
			str="dm2";
			break;
		}
		case 3:
		{
			str="m2";
			break;
		}
		case 4:
		{
			str="in2";
			break;
		}
		case 5:
		{
			str="ft2";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}
void GetLengthUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm";
			break;
		}
		case 1:
		{
			str="cm";
			break;
		}
		case 2:
		{
			str="dm";
			break;
		}
		case 3:
		{
			str="m";
			break;
		}
		case 4:
		{
			str="in";
			break;
		}
		case 5:
		{
			str="ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}

void GetForceUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="N.m";
			break;
		}
		case 1:{
			str="lbf";
			break;
		}

		default:{
			str=" ";
			break;
		}
	}
}

void GetMomentUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="N.m";
			break;
		}
		case 1:
		{
			str="lbf.in";
			break;
		}
		case 2:
		{
			str="lbf.ft";
			break;
		}
		default:
		{
			str=" ";
			break;
		}
	}
}


void GetSpeedUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="m/s";
			break;
		}
		case 1:{
			str="km/h";
			break;
		}
		case 2:{
			str="ft/s";
			break;
		}
		case 3:{
			str="kt";
			break;
		}
		case 4:{
			str="mph";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}


void GetWeightUnit(QString &str, int unit)
{
	switch(unit){
		case 0:{
			str="g";
			break;
		}
		case 1:{
			str="kg";
			break;
		}
		case 2:{
			str="oz";
			break;
		}
		case 3:{
			str="lb";
			break;
		}
		default:{
			str=" ";
			break;
		}
	}
}

bool IsBetween(int f, int f1, int f2)
{
	if (f2 < f1)
	{
		int tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(f<f1) return false;
	else if(f>f2) return false;
	return true;
}


bool IsEven(int n)
{
	if(n%2==0) return true;
	else return false;
}

bool IsBetween(int f, double f1, double f2)
{
	double ff = f;
	if (f2 < f1)
	{
		double tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(ff<f1)       return false;
	else if(ff>f2) return false;
	return true;
}


Qt::PenStyle GetStyle(int s)
{
	if(s==0)      return Qt::SolidLine;
	else if(s==1) return Qt::DashLine;
	else if(s==2) return Qt::DotLine;
	else if(s==3) return Qt::DashDotLine;
	else if(s==4) return Qt::DashDotDotLine;
	return Qt::SolidLine;
}



bool ReadAVLString(QTextStream &in, int &Line, QString &strong)
{
	bool bComment = true;
	int pos;
	QString str;

	while(bComment && !in.atEnd())
	{
		bComment = false;

		strong = in.readLine();
//		bRead = pXFile->ReadString(strong);
		if(in.atEnd()) return false;


		pos = strong.indexOf("#",0);
		if(pos==0)		bComment = true;

		pos = strong.indexOf("!",0);
		if(pos==0)		bComment = true;

		str = strong;
		str.replace(" ", "");
		if(str.isEmpty())	bComment = true;

		Line++;
	}

	if(in.atEnd())
	{
		return false;
	}
	return true;
}


void ReadCOLORREF(QDataStream &ar, QColor &color)
{
	qint32 colorref;
	int r,g,b;

	ar >> colorref;
	b = (int)(colorref/256/256);
	colorref -= b*256*256;
	g = (int)(colorref/256);
	r = colorref - g*256;
	color = QColor(r,g,b,255);

}

void ReadCString(QDataStream &ar, QString &strong)
{
	qint8 qi, ch;
	char c;

	ar >> qi;
	strong.clear();
	for(int j=0; j<qi;j++)
	{
		strong += " ";
		ar >> ch;
		c = char(ch);
		strong[j] = c;
	}
}


bool Rewind1Line(QTextStream &in, int &Line, QString &strong)
{
	int length = strong.length() * 1+2;//1 char takes one byte in the file ?

	int pos = in.pos();
	in.seek(pos-length);
	Line--;
	return true;
}


void ReynoldsFormat(QString &str, double f)
{
	int i, q, r, exp;
	exp = (int)log10(f);
	r = exp%3;
	q = (exp-r)/3;

	QString strong;
	strong = QString("%1").arg(f,0,'f',0);

	int l = strong.length();

	for (i=0; i<q; i++){
		strong.insert(l-3*(i+1)-i," ");
		l++;
	}

	for (i=strong.length(); i<9; i++){
		strong = " "+strong;
	}

	str = strong;
}


void SetUnits(int LUnit, int AUnit, int SUnit, int WUnit, int FUnit, int MUnit,
			  double &mtoUnit, double &m2toUnit, double &mstoUnit,  double &kgtoUnit, double &NtoUnit, double &NmtoUnit)
{
	switch(LUnit)
	{
		case 0:
		{//mdm
			mtoUnit  = 1000.0;
			break;
		}
		case 1:{//cm
			mtoUnit  = 100.0;
			break;
		}
		case 2:{//dm
			mtoUnit  = 10.0;
			break;
		}
		case 3:{//m
			mtoUnit  = 1.0;
			break;
		}
		case 4:{//in
			mtoUnit  = 1000.0/25.4;
			break;
		}
		case 5:{///ft
			mtoUnit  = 1000.0/25.4/12.0;
			break;
		}
		default:{//m
			mtoUnit  = 1.0;
			break;
		}
	}
	switch(AUnit)
	{
		case 0:{//mm²
			m2toUnit = 1000000.0;
			break;
		}
		case 1:{//cm²
			m2toUnit = 10000.0;
			break;
		}
		case 2:{//dm²
			m2toUnit = 100.0;
			break;
		}
		case 3:{//m²
			m2toUnit = 1.0;
			break;
		}
		case 4:{//in²
			m2toUnit = 1./0.254/0.254*100.0;
			break;
		}
		case 5:{//ft²
			m2toUnit = 1./0.254/0.254/144.0*100.0;
			break;
		}
		default:{
			m2toUnit = 1.0;
			break;
		}
	}

	switch(WUnit){
		case 0:{///g
			kgtoUnit = 1000.0;
			break;
		}
		case 1:{//kg
			kgtoUnit = 1.0;

			break;
		}
		case 2:{//oz
			kgtoUnit = 1./ 2.83495e-2;
			break;
		}
		case 3:{//lb
			kgtoUnit = 1.0/0.45359237;
			break;
		}
		default:{
			kgtoUnit = 1.0;
			break;
		}
	}
	switch(SUnit){
		case 0:{// m/s
			mstoUnit = 1.0;
			break;
		}
		case 1:{// km/h
			mstoUnit = 3600.0/1000.0;
			break;
		}
		case 2:{// ft/s
			mstoUnit = 100.0/2.54/12.0;
			break;
		}
		case 3:{// kt (int.)
			mstoUnit = 1.0/0.514444;
			break;
		}
		case 4:{// mph
			mstoUnit = 3600.0/1609.344;
			break;
		}
		default:{
			mstoUnit = 1.0;
			break;
		}
	}

	switch(FUnit){
		case 0:{//N
			NtoUnit = 1.0;
			break;
		}
		case 1:{//lbf
			NtoUnit = 1.0/4.44822;
			break;
		}
		default:{
			NtoUnit = 1.0;
			break;
		}
	}
	switch(MUnit){
		case 0:{//N.m
			NmtoUnit = 1.0;
			break;
		}
		case 1:{//lbf.in
			NmtoUnit = 1.0/4.44822/0.0254;
			break;
		}
		case 2:{//lbf.0t
			NmtoUnit = 1.0/4.44822/12.0/0.0254;
			break;
		}
		default:{
			NmtoUnit = 1.0;
			break;
		}
	}
}


void Trace(int n)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	ts << str;

	tf->close();

}


void Trace(QString msg)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;



	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	str += msg;
	str += "\r\n";

	ts << str;

	tf->close();

}


void Trace(QString msg, int n)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;



	strong = QString("  %1").arg(n);
	strong = msg + strong;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	str += strong;
	str += "\r\n";
	ts << str;;
	tf->close();
}


void Trace(QString msg, double f)
{
//	if(!((CXFLR5App*)AfxGetApp())->bTrace) return;
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;


	strong = QString("  %1").arg(f);
	strong = msg + strong;


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	str += strong;
	str += "\r\n";
	ts << str;


	tf->close();
}


void WriteCOLORREF(QDataStream &ar, QColor const &color)
{
	qint32 colorref;
	int r,g,b;

	color.getRgb(&r,&g,&b);

	colorref = b*256*256+g*256+r;
	ar << colorref;
}



void WriteCString(QDataStream &ar, QString const &strong)
{
	qint8 qi = strong.length();

	QByteArray textline;
	char *text;
	textline = strong.toAscii();
	text = textline.data();

	ar << qi;
	ar.writeRawData(text, qi);
}



double GLGetRed(double tau)
{
	if(tau>2.0/3.0)       return 1.0;
	else if(tau>1.0/3.0)  return (3.0*(tau-1.0/3.0));
	else				  return 0.0;
}

double GLGetGreen(double tau)
{
	if(tau<0.f || tau>1.0) 	return 0.0;
	else if(tau<1.0/4.0) 	return (4.0*tau);
	else if(tau>3.0/4.0) 	return (1.0-4.0*(tau-3.0/4.0));
	else					return 1.0;
}

double GLGetBlue(double tau)
{
	if(tau>2.0/3.0) 		return 0.0;
	else if(tau>1.0/3.0)	return (1.0-3.0*(tau-1.0/3.0));
	else		    		return 1.0;
}


double IntegralC2(double y1, double y2, double c1, double c2)
{
	// Auxiliary integral used in LLT calculations

	double res = 0.0;

	if (fabs(y2-y1)<1.e-5) return 0.0;
	double g = (c2-c1)/(y2-y1);

	res = (c1-g*y1)*(c1-g*y1)*(y2-y1) +
		  g * (c1-g*y1)      *(y2*y2-y1*y1)+
		  g*g/3.0            *(y2*y2*y2-y1*y1*y1);

	return res;
}


double IntegralCy(double y1, double y2, double c1, double c2)
{
	// Auxiliary integral used in LLT calculations

	double res = 0.0;
	if (fabs(y2-y1)<1.e-5) return (y1+y2)/2.0 * (c1+c2)/2.0;

	double g = (c2-c1)/(y2-y1);
	res = (c1-g*y1)/2.0 *(y2*y2 - y1*y1) + g/3.0 * (y2*y2*y2-y1*y1*y1);
	return res;
}


void ReadValues(QString line, int &res, double &x, double &y, double &z)
{
/*	char *sx = new char[30];
	char *sy = new char[30];
	char *text;*/
	QString str;
	bool bOK;

	line = line.simplified();
	int pos = line.indexOf(" ");
	res = 0;
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	x = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		y=z=0.0;
		return;
	}

	line = line.trimmed();
	pos = line.indexOf(" ");
	if(pos>0)
	{
		str = line.left(pos);
		line = line.right(line.length()-pos);
	}
	else
	{
		str = line;
		line = "";
	}
	y = str.toDouble(&bOK);
	if(bOK) res++;
	else
	{
		z=0.0;
		return;
	}

	line = line.trimmed();
	if(!line.isEmpty())
	{
		z = line.toDouble(&bOK);
		if(bOK) res++;
	}
	else z=0.0;
}




bool ludcmp(int n, double *a, int *indx)
{
	//    *******************************************************
	//    *                                                     *
	//    *   factors a full nxn matrix into an lu form.        *
	//    *   subr. baksub can back-substitute it with some rhs.*
	//    *   assumes matrix is non-singular...                 *
	//    *    ...if it isn"t, a divide by zero will result.    *
	//    *                                                     *
	//    *   a is the matrix...                                *
	//    *     ...replaced with its lu factors.                *
	//    *                                                     *
	//    *                              mark drela  1988       *
	//    *                                                     *
	//    *  Translated to C - Andre Deperrois  2003-2009       *
	//    *******************************************************
	//
	//
	bool bimaxok = false;
	int imax = -1;
	int i, j, k;
	double vv[VLMMATSIZE];//use temporary global array
	double dum, sum, aamax;

	if(n>IQX)
	{
//		QString str("Stop ludcmp: array overflow. Increase nvx");
//		WriteString(str, true);
		return false;
	}

	for (i=1; i<=n; i++)
	{
		aamax = 0.0;
		for (j=1; j<=n; j++) aamax = qMax(fabs(*(a+i*n+j)), aamax);
		vv[i] = 1.0/aamax;
	}

	for(j=1;j<= n;j++)
	{
		for(i=1; i<=j-1; i++)
		{
			sum = *(a+i*n+j);
			for (k=1;k<= i-1;k++) sum -= *(a+i*n+k)* *(a+k*n+j);
			*(a+i*n+j) = sum;
		}

		aamax = 0.0;

		for (i=j; i<=n;i++)
		{
			sum = *(a+i*n+j);
			for (k=1;k<= j-1;k++) sum -= *(a+i*n+k)* *(a+k*n+j);
			*(a+i*n+j) = sum ;
			dum = (vv[i]*fabs(sum));
			if(dum>=aamax)
			{
				imax = i;
				aamax = dum;
				bimaxok = true;
			}
		}

		if(imax<0) return false;

		if(j!=imax)
		{
			for (k=1; k<= n; k++)
			{
				dum = *(a+imax*n+k);
				*(a+imax*n+k) = *(a+j*n+k);
				*(a+j*n+k)    = dum;
			}
			vv[imax] = vv[j];
		}

		indx[j] = imax;
		if(j!=n)
		{
			dum /= *(a+j*n+j);
			for(i=j+1; i<= n;i++) *(a+i*n+j) *= dum;
		}
	}
	return true;
}


bool baksub(int n, double *a, int *indx, double *b)
{
	double sum;
	int i, ii, ll, j;
	ii = 0;
	for (i=1;i<= n;i++)
	{
		ll = indx[i];
		sum = b[ll];
		b[ll] = b[i];
		if(ii!=0)
			for (j=ii;j<=i-1;j++) sum -= *(a+i*n+j) * b[j];
		else
			if(sum!=0.0) ii = i;

		b[i] = sum;
	}

	for (i=n; i>=1; i--)
	{
		sum = b[i];
		if(i<n)
		for (j=i+1; j<= n; j++) sum -= *(a+i*n+j) * b[j];
		b[i] = sum/ *(a+i*n+i);
	}
	return true;
}






