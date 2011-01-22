/****************************************************************************

	Globals Class
	Copyright (C) 2008-2011 Andre Deperrois xflr5@yahoo.com

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
#include <QVector3D>
#include <QPen>
#include <QApplication>
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



bool Gauss(double *A, int n, double *B, int m, bool *pbCancel)
{
	static int row, i, j, pivot_row, k;
	static double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n)
	{
//		qApp->processEvents();
		if(*pbCancel) return false;
		//  find the pivot row
		A_pivot_row = pa;
		max = fabs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
		{
			if ((dum = fabs(*(pA+row))) > max)
			{
				max = dum;
				A_pivot_row = pA;
				pivot_row = i;
			}
		}
		if (max <= 0.0)
		{
			return false;                // the matrix A is singular
		}
			// and if it differs from the current row, interchange the two rows.

		if (pivot_row != row)
		{
			for (i = row; i < n; i++)
			{
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<m; k++)
			{
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}

		// Perform forward substitution
		for (i = row+1; i<n; i++)
		{
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<m; k++)
				B[i+k*n] += dum * B[row+k*n];
		}
	}
	// Perform backward substitution

	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--)
	{
//		qApp->processEvents();
		if(*pbCancel) return false;

		if ( *(pa + row) == 0.0 )
		{
			return false;           // matrix is singular
		}
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum;
		for(k=0; k<m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++)
		{
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<m; k++)
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}



void GetAreaUnit(QString &str, int unit)
{
	switch(unit)
	{
		case 0:
		{
			str="mm"+QString::fromUtf8("²");
			break;
		}
		case 1:
		{
			str="cm"+QString::fromUtf8("²");
			break;
		}
		case 2:
		{
			str="dm"+QString::fromUtf8("²");
			break;
		}
		case 3:
		{
			str="m"+QString::fromUtf8("²");
			break;
		}
		case 4:
		{
			str="in"+QString::fromUtf8("²");
			break;
		}
		case 5:
		{
			str="ft"+QString::fromUtf8("²");
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
	QString FileName = QDir::tempPath() + "/Trace.log";
	QFile *tf = new QFile(FileName);
	if (!tf->open(QIODevice::ReadWrite | QIODevice::Text)) return;

	QString strong;
	QTextStream ts(tf);
	while(!ts.atEnd()) ts>>strong;


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	ts << str << n;

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
	str += "\n";

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
	str += "\n";
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
	str += "\n";
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
	if(tau>2.0/3.0)      return 1.0;
	else if(tau>1.0/3.0) return (3.0*(tau-1.0/3.0));
	else                 return 0.0;
}


double GLGetGreen(double tau)
{
	if(tau<0.f || tau>1.0) 	return 0.0;
	else if(tau<1.0/4.0) 	return (4.0*tau);
	else if(tau>3.0/4.0) 	return (1.0-4.0*(tau-3.0/4.0));
	else                    return 1.0;
}


double GLGetBlue(double tau)
{
	if(tau>2.0/3.0)      return 0.0;
	else if(tau>1.0/3.0) return (1.0-3.0*(tau-1.0/3.0));
	else                 return 1.0;
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



////////////////////////////////////////////////////////////////////////////////
//  int Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n)   //
//                                                                            //
//  Unknown author http://mymathlib.webtrellis.net/index.html                 //
//                                                                            //
//  Description:                                                              //
//     This routine uses Crout's method to decompose a row interchanged       //
//     version of the n x n matrix A into a lower triangular matrix L and a   //
//     unit upper triangular matrix U such that A = LU.                       //
//     The matrices L and U replace the matrix A so that the original matrix  //
//     A is destroyed.                                                        //
//     Note!  In Crout's method the diagonal elements of U are 1 and are      //
//            not stored.                                                     //
//     Note!  The determinant of A is the product of the diagonal elements    //
//            of L.  (det A = det L * det U = det L).                         //
//     The LU decomposition is convenient when one needs to solve the linear  //
//     equation Ax = B for the vector x while the matrix A is fixed and the   //
//     vector B is varied.  The routine for solving the linear system Ax = B  //
//     after performing the LU decomposition for A is                         //
//                      Crout_LU_with_Pivoting_Solve.                         //
//     (see below).                                                           //
//                                                                            //
//     The Crout method with partial pivoting is: Determine the pivot row and //
//     interchange the current row with the pivot row, then assuming that     //
//     row k is the current row, k = 0, ..., n - 1 evaluate in order the      //
//     the following pair of expressions                                      //
//       L[i][k] = (A[i][k] - (L[i][0]*U[0][k] + . + L[i][k-1]*U[k-1][k]))    //
//                                 for i = k, ... , n-1,                      //
//       U[k][j] = A[k][j] - (L[k][0]*U[0][j] + ... + L[k][k-1]*U[k-1][j])    //
//                                                                  / L[k][k] //
//                                      for j = k+1, ... , n-1.               //
//       The matrix U forms the upper triangular matrix, and the matrix L     //
//       forms the lower triangular matrix.                                   //
//                                                                            //
//  Arguments:                                                                //
//     double *A       Pointer to the first element of the matrix A[n][n].    //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     int     n       The number of rows or columns of the matrix A.         //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  Failure - The matrix A is singular.                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //

bool Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress)
{
	int i, j, k;
	double *p_k, *p_row, *p_col;
	double max=0.0;

	p_col = NULL;

	//  For each row and column, k = 0, ..., n-1,
	for (k=0, p_k=A; k<n; p_k+=n, k++)
	{
	//  find the pivot row
		pivot[k] = k;
		p_col = p_k+k;
		max = fabs( *(p_k + k) );
		for (j=k+1, p_row=p_k+n; j<n; j++, p_row+=n)
		{
			if (max<fabs(*(p_row+k)))
			{
				max = fabs(*(p_row+k));
				pivot[k] = j;
				p_col = p_row;
			}
		}
		if(!p_col) return false;

		// and if the pivot row differs from the current row, then
		// interchange the two rows.
		if (pivot[k] != k)
		{
			for (j=0; j<n; j++)
			{
				max = *(p_k + j);
				*(p_k + j) = *(p_col + j);
				*(p_col + j) = max;
			}
		}

		// and if the matrix is singular, return error
		if ( *(p_k + k) == 0.0 ) return false;

		// otherwise find the upper triangular matrix elements for row k.
		for (j = k+1; j < n; j++) *(p_k + j) /= *(p_k + k);

		// update remaining matrix
		for (i = k+1, p_row = p_k + n; i < n; p_row += n, i++)
			for (j = k+1; j < n; j++) *(p_row + j) -= *(p_row + k) * *(p_k + j);

		Progress += TaskSize/(double)(n);
		qApp->processEvents();
		if(*pbCancel) return false;		
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////
//  int Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[],     //
//                                                        double x[], int n)  //
//                                                                            //
// Unknown author http://mymathlib.webtrellis.net/index.html                  //
//                                                                            //
//  Description:                                                              //
//     This routine uses Crout's method to solve the linear equation Ax = B.  //
//     This routine is called after the matrix A has been decomposed into a   //
//     product of a lower triangular matrix L and a unit upper triangular     //
//     matrix U without pivoting.  The argument LU is a pointer to the matrix //
//     the superdiagonal part of which is U and the subdiagonal together with //
//     the diagonal part is L. (The diagonal part of U is 1 and is not        //
//     stored.)   The matrix A = LU.                                          //
//     The solution proceeds by solving the linear equation Ly = B for y and  //
//     subsequently solving the linear equation Ux = y for x.                 //
//                                                                            //
//  Arguments:                                                                //
//     double *LU      Pointer to the first element of the matrix whose       //
//                     elements form the lower and upper triangular matrix    //
//                     factors of A.                                          //
//     double *B       Pointer to the column vector, (n x 1) matrix, B.       //
//     int    pivot[]  The i-th element is the pivot row interchanged with    //
//                     row i.                                                 //
//     double *x       Solution to the equation Ax = B.                       //
//     int     n       The number of rows or columns of the matrix LU.        //
//                                                                            //
//  Return Values:                                                            //
//     true  : Success                                                        //
//     false : Failure - The matrix A is singular.                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
bool Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[], double x[], int Size, bool *pbCancel)
{
	int i, k;
	double *p_k;
	double dum;

	//  Solve the linear equation Lx = B for x, where L is a lower triangular matrix.
	for (k=0, p_k=LU; k<Size; p_k+=Size, k++)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		x[k] = B[k];
		for (i=0; i<k; i++) x[k]-=x[i] * *(p_k+i);
		x[k] /= *(p_k+k);
		
		qApp->processEvents();
		if(*pbCancel) return false;
	}

	//  Solve the linear equation Ux = y, where y is the solution
	//  obtained above of Lx = B and U is an upper triangular matrix.
	//  The diagonal part of the upper triangular part of the matrix is
	//  assumed to be 1.0.
	for (k=Size-1, p_k=LU+Size*(Size-1); k>=0; k--, p_k-=Size)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		for (i=k+1; i<Size; i++) x[k]-=x[i] * *(p_k+i);
		if (*(p_k+k)==0.0)
		{
			return false;
		}
		
		qApp->processEvents();
		if(*pbCancel) return false;
	}

	return true;
}


double Det44(double *aij)
{
//	returns the determinant of a 4x4 matrix

	static int i,j,k,l,p,q;
	static double det, sign, a33[16];

	det = 0.0;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			p = 0;
			for(k=0; k<4 && k!=i; k++)
			{
				q = 0;
				for(l=0; l<4 && l!=j; l++)
				{
					*(a33+p*3+q) = *(aij+4*k+l);// could also do it by address, to be a little faster
					q++;
				}
				p++;
			}
			sign = pow(-1.0,i+j);
			det += sign * Det33(a33);
		}
	}
	return det;
}


double Det33(double *aij)
{
	//returns the determinant of a 3x3 matrix

	double det;

	det  = aij[0]*aij[4]*aij[8];
	det -= aij[0]*aij[5]*aij[7];

	det -= aij[1]*aij[3]*aij[8];
	det += aij[1]*aij[5]*aij[6];

	det += aij[2]*aij[3]*aij[7];
	det -= aij[2]*aij[4]*aij[6];

	return det;
}



complex<double> Det33(complex<double> *aij)
{
	//returns the determinant of a 3x3 matrix
	complex<double> det;

	det  = aij[0]*aij[4]*aij[8];
	det -= aij[0]*aij[5]*aij[7];

	det -= aij[1]*aij[3]*aij[8];
	det += aij[1]*aij[5]*aij[6];

	det += aij[2]*aij[3]*aij[7];
	det -= aij[2]*aij[4]*aij[6];

	return det;
}


complex<double> Cofactor44(complex<double> *aij, int &i, int &j)
{
	//returns the complex cofactor	of element i,j, in the 4x4 matrix aij
	static int k,l,p,q;
	static complex<double> a33[9];

	p = 0;
	for(k=0; k<4; k++)
	{
		if(k!=i)
		{
			q = 0;
			for(l=0; l<4; l++)
			{
				if(l!=j)
				{
					a33[p*3+q] = *(aij+4*k+l);
					q++;
				}
			}
			p++;
		}
	}
	return Det33(a33);
}


complex<double> Det44(complex<double> *aij)
{
//	returns the determinant of a 4x4 matrix

	static int i,j,k,l,p,q;
	static double sign;
	static complex<double> det, a33[16];
	det = 0.0;

	i=0;
	for(j=0; j<4; j++)
	{
		p = 0;
		for(k=0; k<4; k++)
		{
			if(k!=i)
			{
				q = 0;
				for(l=0; l<4; l++)
				{
					if(l!=j)
					{
						a33[p*3+q] = aij[4*k+l];
						q++;
					}
				}
				p++;
			}
		}
		sign = pow(-1.0,i+j);
		det += sign * aij[4*i+j] * Det33(a33);
	}

	return det;
}


bool Invert44(complex<double> *ain, complex<double> *aout)
{
	//small size, use the direct method
	static int i,j;
	static complex<double> det;
	static double sign;

	det = Det44(ain);

	if(abs(det)<PRECISION) return false;

	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			sign = pow(-1.0,i+j);
			aout[4*j+i] = sign * Cofactor44(ain, i, j)/det;
		}
	}
	return true;
}






void CharacteristicPol(double m[][4], double p[5])
{
	// input : 4x4 matrix m[][]
	// output : coefficients p[] of the matrix characteristic polynomial
	// thanks Mapple !
	// polynom can then be solved for complex roots using Bairstow's algorithm


	// lambda^4
	p[4] =  1;

	// lambda^3
	p[3] =  - m[0][0]- m[1][1]-m[2][2]- m[3][3];

	// lambda^2
	p[2] =
		+ m[0][0] * m[1][1]
		+ m[0][0] * m[2][2]
		+ m[0][0] * m[3][3]
		+ m[1][1] * m[3][3]
		+ m[1][1] * m[2][2]
		+ m[2][2] * m[3][3]
		- m[1][0] * m[0][1]
		- m[2][1] * m[1][2]
		- m[2][0] * m[0][2]
		- m[2][3] * m[3][2]
		- m[3][1] * m[1][3]
		- m[3][0] * m[0][3];

	// lambda^1
	p[1] =
		+ m[2][1] * m[1][2] * m[3][3]
		+ m[0][0] * m[2][1] * m[1][2]
		- m[3][1] * m[1][2] * m[2][3]
		+ m[3][1] * m[1][3] * m[2][2]
		+ m[1][0] * m[0][1] * m[3][3]
		+ m[1][0] * m[0][1] * m[2][2]
		- m[2][0] * m[0][1] * m[1][2]
		- m[1][0] * m[3][1] * m[0][3]
		- m[1][0] * m[2][1] * m[0][2]
		+ m[3][0] * m[1][1] * m[0][3]
		- m[3][0] * m[0][1] * m[1][3]
		+ m[2][0] * m[0][2] * m[3][3]
		- m[2][0] * m[3][2] * m[0][3]
		+ m[2][0] * m[1][1] * m[0][2]
		- m[3][0] * m[0][2] * m[2][3]
		+ m[3][0] * m[0][3] * m[2][2]
		- m[2][1] * m[3][2] * m[1][3]
		- m[0][0] * m[1][1] * m[2][2]
		+ m[0][0] * m[2][3] * m[3][2]
		+ m[1][1] * m[2][3] * m[3][2]
		- m[0][0] * m[2][2] * m[3][3]
		+ m[0][0] * m[3][1] * m[1][3]
		- m[1][1] * m[2][2] * m[3][3]
		- m[0][0] * m[1][1] * m[3][3];

	// lambda^0
	p[0] =
		+ m[2][0] * m[0][1] * m[1][2] * m[3][3]
		- m[2][0] * m[1][1] * m[0][2] * m[3][3]
		+ m[2][0] * m[1][1] * m[3][2] * m[0][3]
		- m[2][0] * m[0][1] * m[3][2] * m[1][3]
		+ m[1][0] * m[3][1] * m[0][3] * m[2][2]
		- m[1][0] * m[3][1] * m[0][2] * m[2][3]
		+ m[1][0] * m[2][1] * m[0][2] * m[3][3]
		- m[1][0] * m[2][1] * m[3][2] * m[0][3]
		- m[3][0] * m[1][1] * m[0][3] * m[2][2]
		+ m[3][0] * m[0][1] * m[1][3] * m[2][2]
		- m[3][0] * m[0][1] * m[1][2] * m[2][3]
		- m[2][0] * m[3][1] * m[1][2] * m[0][3]
		+ m[2][0] * m[3][1] * m[0][2] * m[1][3]
		- m[3][0] * m[2][1] * m[0][2] * m[1][3]
		+ m[3][0] * m[1][1] * m[0][2] * m[2][3]
		+ m[3][0] * m[2][1] * m[1][2] * m[0][3]
		- m[0][0] * m[2][1] * m[1][2] * m[3][3]
		+ m[0][0] * m[2][1] * m[3][2] * m[1][3]
		- m[1][0] * m[0][1] * m[2][2] * m[3][3]
		+ m[1][0] * m[0][1] * m[2][3] * m[3][2]
		+ m[0][0] * m[3][1] * m[1][2] * m[2][3]
		- m[0][0] * m[3][1] * m[1][3] * m[2][2]
		+ m[0][0] * m[1][1] * m[2][2] * m[3][3]
		- m[0][0] * m[1][1] * m[2][3] * m[3][2];
}


#define POLYNOMORDER    6


void TestEigen()
{
	double A[4][4];
	double p[5];
/*	A[0][0] = -0.0069; 	A[1][0] =  0.0139; 	A[2][0] =   0.0;    A[3][0] = -9.81;
	A[0][1] = -0.0905; 	A[1][1] = -0.3149; 	A[2][1] = 235.8928; A[3][1] =  0.0;
	A[0][2] =  0.0004;	A[1][2] = -0.0034;	A[2][2] = -0.4282;  A[3][2] = 0.0;
	A[0][3] =  0.0000;	A[1][3] =  0.0000;	A[2][3] =  1.0;     A[3][3] = 0.0;*/

	A[0][0] =-1.00; A[0][1] =  1.0;	A[0][2] =  1.0;	A[0][3] = -1.0;
	A[1][0] = 1.00; A[1][1] =  1.0;	A[1][2] =  2.0; A[1][3] = -1.0;
	A[2][0] = 3.00;	A[2][1] = -2.0;	A[2][2] =  1.0; A[2][3] =  1.0;
	A[3][0] = 1.00;	A[3][1] =  1.0;	A[3][2] =  2.0;	A[3][3] =  1.0;
	complex<double> AC[16];
//	complex<double> V[4];
	complex <double> lambda(2.0, 0.0);
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4;j++)
		{
			AC[i*4+j] = complex<double>(A[i][j],0.0);
		}
	}

	CharacteristicPol(A, p);

	complex<double> roots[POLYNOMORDER];

	if(LinBairstow(p, roots, 4))
	{
	}
	else
	{
	}
}


bool Eigenvector(double a[][4], complex<double> lambda, complex<double> *V)
{
	//________________________________________________________________________
	//Solves the system A.V = lambda.V for a 4x4 complex matrix
	// in input :
	//    - matrix A
	//    - the array of complex eigenvalues
	// in output
	//    - the array of complex eigenvectors
	//
	// eigen vector is solved by direct matrix solution
	// one of the vector's component is set to 1, to avoid the trivial solution V=0;
	//
	// Andre Deperrois October 2009
	//
	//________________________________________________________________________

	static complex<double> detm, detr;
	static complex<double> r[9], m[9];
	int ii, jj, i, j, kp;

	// first find a pivot for which the  associated n-1 determinant is not zero
	bool bFound = false;
	kp=0;
	do
	{
		V[kp] = 1.0;
		ii= 0;
		for(i=0;i<4 ; i++)
		{
			if(i!=kp)
			{
				jj=0;
				for(j=0; j<4; j++)
				{
					if(j!=kp)
					{
						m[ii*3+jj] = a[i][j];
						jj++;
					}
				}
				m[ii*3+ii] -= lambda;
				ii++;
			}
		}
		detm = Det33(m);
		bFound = std::abs(detm)>0.0;
		if(bFound || kp>=3) break;
		kp++;
	}while(true);

	if(!bFound) return false;

	// at this point we have identified pivot kp
	// with a non-zero subdeterminant.
	// so solve for the other 3 eigenvector components.
	// using Cramer's rule

	//create rhs determinant
	jj=0;
	for(j=0; j<4; j++)
	{
		memcpy(r,m, 9*sizeof(complex<double>));
		if(j!=kp)
		{
			ii= 0;
			for(i=0; i<4; i++)
			{
				if(i!=kp)
				{
					r[ii*3+jj] = - a[i][kp];
					ii++;
				}
			}
			detr  = Det33(r);
			V[j] = detr/detm;
			jj++;
		}
	}

	return true;
}

#define TOLERANCE   1.e-8
#define MAXBAIRSTOWITER 30

bool LinBairstow(double *p, complex<double> *root, int n)
{
	//________________________________________________________________________
	//
	// Finds the complex roots of a polynom P(x) using Lin-Bairstow's method
	// P(x) = Sum p_i x^i	    i = 0..n;
	// The polynoms coefficient are in array p
	//
	// Andre Deperrois October 2009
	//
	//________________________________________________________________________

	double b[POLYNOMORDER], c[POLYNOMORDER];
	int i, k, nn, iter;
	double r,s,d0,d1,d2;
	double Delta;

	memset(b, 0, POLYNOMORDER*sizeof(double));
	memset(c, 0, POLYNOMORDER*sizeof(double));

	//T(x) = x2 -rx -s;
	//R(x) = u(x-r)+v //remainder of deivision of by Q
	//Q(x) = Sum b_i x^(i-2)

	//P(x) = (x2-rx+s) Q(x) + u(x-r) +v

	nn=n ;//initial order is polynom order

	r=-2.0;//initial guesses
	s=-1.0;

	do
	{
		iter = 0;
		do
		{
			//compute recursively the coefs b_i of polynom Q
			b[nn]   = p[nn];
			b[nn-1] = p[nn-1] + r * b[nn];
			for(k=nn-2; k>=0; k--) b[k] = p[k] + r*b[k+1] + s*b[k+2];

			//build the partial derivatives c_i
			c[nn]   = b[nn];
			c[nn-1] = b[nn-1] + r * c[nn];
			for(k=nn-2; k>=1; k--) c[k] = b[k] + r*c[k+1] + s*c[k+2];

			d0 = c[1]*c[3] - c[2]*c[2];
			d1 = (-b[0]*c[3]+b[1]*c[2])/d0;
			d2 = (-b[1]*c[1]+b[0]*c[2])/d0;
			r+=d1;
			s+=d2;
			iter++;
		} while((std::abs(d1)> TOLERANCE || std::abs(d2)> TOLERANCE) && iter < MAXBAIRSTOWITER);

		if(iter>=MAXBAIRSTOWITER)return false;
		//we have a division
		//so find the roots of the remainder R
		Delta = r*r+4.0*s;
		if(Delta<0.0)
		{
			//complex roots
			root[nn-1] = complex<double>(r/2.0,  sqrt(fabs(Delta))/2.0);
			root[nn-2] = complex<double>(r/2.0, -sqrt(fabs(Delta))/2.0);
		}
		else
		{
			//real roots
			root[nn-1] = complex<double>(r/2.0 + sqrt(Delta)/2.0, 0.0);
			root[nn-2] = complex<double>(r/2.0 - sqrt(Delta)/2.0, 0.0);
		}

		//deflate polynom order
		for(i=nn; i>=2; i--)
		{
			p[i-2] = b[i];
		}
		nn-=2;
		if(nn==2)
		{
			//last two roots, solve directly
			if(fabs(p[2])<PRECISION)
			{
				// one last root, but we should never get here
				if(fabs(p[1])>PRECISION)
				{
					//last single root, real
					root[0] = -p[0]/p[1];
				}
				else return false;
			}
			else
			{
				Delta = p[1]*p[1]-4.0*p[0]*p[2];
				if(Delta<0)
				{
					//complex roots
					root[nn-1] = complex<double>(-p[1]/2.0/p[2],  sqrt(fabs(Delta))/2.0/p[2]);
					root[nn-2] = complex<double>(-p[1]/2.0/p[2], -sqrt(fabs(Delta))/2.0/p[2]);
				}
				else
				{
					//real roots
					root[nn-1] = complex<double>((-p[1]+sqrt(Delta))/2.0/p[2],  0.0);
					root[nn-2] = complex<double>((-p[1]-sqrt(Delta))/2.0/p[2],  0.0);
				}
			}
			break;
		}
		if(nn==1)
		{
			if(fabs(p[1])>PRECISION)
			{
				//last single root, real
				root[0] = -p[0]/p[1];
			}
			else return false;
			break;
		}

	}while(nn>2);
	return true;
}



double GetVar(QList<void*>*m_poaPolar, int nVar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError)
{
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;
	double Var0, Var1;
	if(!pFoil0)
	{
		Cl = 0.0;
		Var0 = 0.0;
	}
	else Var0 = GetPlrPointFromCl(m_poaPolar, pFoil0, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;


	if(!pFoil1)
	{
		Cl = 0.0;
		Var1 = 0.0;
	}
	else Var1 = GetPlrPointFromCl(m_poaPolar, pFoil1, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Var0 + Tau * Var1);
}


void * GetPlrVariable(CPolar *pPolar, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar

	void * pVar;
	switch (iVar)
	{
		case 0:
			pVar = &pPolar->m_Alpha;
			break;
		case 1:
			pVar = &pPolar->m_Cl;
			break;
		case 2:
			pVar = &pPolar->m_Cd;
			break;
		case 3:
			pVar = &pPolar->m_Cdp;
			break;
		case 4:
			pVar = &pPolar->m_Cm;
			break;
		case 5:
			pVar = &pPolar->m_XTr1;
			break;
		case 6:
			pVar = &pPolar->m_XTr2;
			break;
		case 7:
			pVar = &pPolar->m_HMom;
			break;
		case 8:
			pVar = &pPolar->m_Cpmn;
			break;
		case 9:
			pVar = &pPolar->m_ClCd;
			break;
		case 10:
			pVar = &pPolar->m_Cl32Cd;
			break;
		case 11:
			pVar = &pPolar->m_XCp;
			break;
		default:
			pVar = &pPolar->m_Alpha;
			break;
	}
	return pVar;
}


double GetPlrPointFromAlpha(QList<void*>*m_poaPolar, CFoil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError)
{
/*	Var
	0 =	m_Alpha;
	1 = m_Cl;
	2 = m_Cd;
	3 = m_Cdp;
	4 = m_Cm;
	5, 6 = m_XTr1, m_XTr2;
	7, 8 = m_HMom, m_Cpmn;
	9,10 = m_ClCd, m_Cl32Cd;
	11 = m_XCp
*/

	QList <double> *pX;
	double amin, amax;
	double Var1, Var2, u;
	amin = amax = Var1 = Var2 = u = 0.0;
	int i;
	CPolar *pPolar;

	bOutRe = false;
	bError = false;

	if(!pFoil)
	{
		bOutRe = true;
		bError = true;
		return 0.000;
	}


	int size;
	int n = 0;

	// Are there any Type 1 polars available for this foil ?
	for (i = 0; i< m_poaPolar->size(); i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = m_poaPolar->size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName) && pPolar->m_Alpha.size()>0)
		{
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds)
			{
				bOutRe = true;
				//interpolate Alpha on this polar
				pX = (QList <double> *) GetPlrVariable(pPolar, PlrVar);
				size = pPolar->m_Alpha.size();
				if(Alpha < pPolar->m_Alpha[0])
				{
					return (*pX)[0];
				}
				else if(Alpha > pPolar->m_Alpha[size-1])
				{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++)
				{
					if(pPolar->m_Alpha[i] <= Alpha && Alpha < pPolar->m_Alpha[i+1])
					{
						//interpolate
						if(pPolar->m_Alpha[i+1]-pPolar->m_Alpha[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else
						{
							u = (Alpha - pPolar->m_Alpha[i])
									 /(pPolar->m_Alpha[i+1]-pPolar->m_Alpha[i]);
							return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
						}
					}
				}
				break;
			}
			break;
		}
	}

	// if not Find the two polars
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)  && pPolar->m_Alpha.size()>0)
		{
			// we have found the first type 1 polar for this foil
			pPolar->GetAlphaLimits(amin, amax);
			if (pPolar->m_Reynolds <= Re)
			{
				if(amin <= Alpha && Alpha <= amax)
				{
					pPolar1 = pPolar;
				}
			}
			else {
				if(amin <= Alpha && Alpha <= amax)
				{
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}

	if (!pPolar2)
	{
		//then Re is greater than that of any polar
		// so use last polar and interpolate alphas on this polar
		bOutRe = true;
		if(!pPolar1)
		{
			bError = true;
			return 0.000;
		}
		size = pPolar1->m_Alpha.size();
		if(!size)
		{
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	     return (*pX)[0];
		if(Alpha > pPolar1->m_Alpha[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++)
		{
			if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1])
			{
				//interpolate
				if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001){//do not divide by zero
					return (*pX)[i];
				}
				else
				{
					u = (Alpha - pPolar1->m_Alpha[i])
							 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
					return (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
				}
			}
		}
		//Out in Re, out in alpha...
		return (*pX)[size-1] ;

	}
	else
	{
		// Re is between that of polars 1 and 2
		// so interpolate alphas for each

		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Alpha.size();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	          Var1 = (*pX)[0];
		else if(Alpha > pPolar1->m_Alpha[size-1]) Var1 = (*pX)[size-1];
		else
		{
			for (i=0; i<size-1; i++)
			{
				if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1]){
					//interpolate
					if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001)//do not divide by zero
						Var1 = (*pX)[i];
					else
					{
						u = (Alpha - pPolar1->m_Alpha[i])
								 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
						Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
					}
				}
			}
		}

		size = (int)pPolar2->m_Alpha.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
		if(Alpha < pPolar2->m_Alpha[0])
		{
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[0];
		}
		else if(Alpha > pPolar2->m_Alpha[size-1])
		{
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[size-1];
		}
		else{
			for (i=0; i<size-1; i++)
			{
				if(pPolar2->m_Alpha[i] <= Alpha && Alpha < pPolar2->m_Alpha[i+1])
				{
					//interpolate
					pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
					if(pPolar2->m_Alpha[i+1]-pPolar2->m_Alpha[i] < 0.00001)//do not divide by zero
						Var2 = (*pX)[i];
					else{
						u = (Alpha - pPolar2->m_Alpha[i])
								 /(pPolar2->m_Alpha[i+1]-pPolar2->m_Alpha[i]);
						Var2 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
					}
				}
			}
		}
		// then interpolate Variable

		double v =   (Re - pPolar1->m_Reynolds)
				  / (pPolar2->m_Reynolds - pPolar1->m_Reynolds);
		double Var = Var1 + v * (Var2-Var1);
		return Var;
	}

//	AfxMessageBox("Error interpolating", MB_OK);
//	bOutRe = true;
//	bError = true;
//	return 0.000;// we missed something somewhere...
}


double GetPlrPointFromCl(QList<void*>*m_poaPolar, CFoil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError)
{
/*	Var
	0 =	m_Alpha;
	1 = m_Cl;
	2 = m_Cd;
	3 = m_Cdp;
	4 = m_Cm;
	5, 6 = m_XTr1, m_XTr2;
	7, 8 = m_HMom, m_Cpmn;
	9,10 = m_ClCd, m_Cl32Cd;
*/
	QList <double> *pX;
	double Clmin, Clmax;
	CPolar *pPolar;
	double Var1, Var2, u, dist;
	Var1 = Var2 = u = dist = 0.0;
	int pt;
	int size;
	int n, i;

	bOutRe = false;
	bError = false;

	if(!pFoil)
	{
		bOutRe = true;
		bError = true;
		return 0.000;
	}

		n=0;
	// Are there any Type 1 polars available for this foil ?
	for (i = 0; i< m_poaPolar->size(); i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = m_poaPolar->size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName) && pPolar->m_Cl.size()>0)
		{
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds)
			{
				bOutRe = true;
				//interpolate Cl on this polar
				pX = (QList <double> *) GetPlrVariable(pPolar, PlrVar);
				size = (int)pPolar->m_Cl.size();
				if(Cl < pPolar->m_Cl[0])
				{
					return (*pX)[0];
				}
				if(Cl > pPolar->m_Cl[size-1])
				{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++)
				{
					if(pPolar->m_Cl[i] <= Cl && Cl < pPolar->m_Cl[i+1])
					{
					//interpolate
						if(pPolar->m_Cl[i+1]-pPolar->m_Cl[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else {
							u = (Cl - pPolar->m_Cl[i])
									 /(pPolar->m_Cl[i+1]-pPolar->m_Cl[i]);
							return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
						}
					}
				}
				break;
			}
			break;
		}
	}

	// if not Find the two polars
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)  && pPolar->m_Cl.size()>0)
		{
			// we have found the first type 1 polar for this foil
			pPolar->GetClLimits(Clmin, Clmax);
			if (pPolar->m_Reynolds <= Re)
			{
				if(Clmin <= Cl && Cl <= Clmax)
				{
					pPolar1 = pPolar;
				}
			}
			else
			{
				if(Clmin <= Cl && Cl <= Clmax)
				{
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}

	if (!pPolar2)
	{
		//then Re is greater than that of any polar
		// so use last polar and interpolate Cls on this polar
		bOutRe = true;
		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Cl < pPolar1->m_Cl[0])	   return (*pX)[0];
		if(Cl > pPolar1->m_Cl[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++)
		{
			if(pPolar1->m_Cl[i] <= Cl && Cl < pPolar1->m_Cl[i+1])
			{
				//interpolate
				if(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i] < 0.00001)
				{//do not divide by zero
					return (*pX)[i];
				}
				else
				{
					u = (Cl - pPolar1->m_Cl[i])
							 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
					return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
				}
			}
		}
		//Out in Re, out in Cl...
		return (*pX)[size-1];
	}
	else
	{
		// Re is between that of polars 1 and 2
		// so interpolate Cls for each
		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		pPolar1->GetClLimits(Clmin, Clmax);
		if(Cl < Clmin)
		{
			Var1 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax)
		{
			Var1 = (*pX)[size-1];
			bOutRe = true;
		}
		else
		{
			//first Find the point closest to Cl=0
			pt = 0;
			dist = fabs(pPolar1->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (fabs(pPolar1->m_Cl[i])< dist)
				{
					dist = fabs(pPolar1->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar1->m_Cl[pt])
			{
				for (i=pt; i>0; i--)
				{
					if(Cl<= pPolar1->m_Cl[i] && Cl > pPolar1->m_Cl[i-1])
					{
						//interpolate
						if(fabs(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]) < 0.00001)
						{
							//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar1->m_Cl[i-1])
									 /(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]);
							Var1 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else
			{
				for (i=pt; i<size-1; i++)
				{
					if(pPolar1->m_Cl[i] <=Cl && Cl < pPolar1->m_Cl[i+1])
					{
						//interpolate
						if(fabs(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]) < 0.00001){//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar1->m_Cl[i])
									 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
							Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
					}
				}
			}
		}
		size = (int)pPolar2->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
		pPolar2->GetClLimits(Clmin, Clmax);

		if(Cl < Clmin)
		{
			Var2 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax)
		{
			Var2 = (*pX)[size-1];
			bOutRe = true;
		}
		else
		{
			//first Find the point closest to Cl=0
			pt = 0;
			dist = fabs(pPolar2->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (fabs(pPolar2->m_Cl[i])< dist)
				{
					dist = fabs(pPolar2->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar2->m_Cl[pt])
			{
				for (i=pt; i>0; i--)
				{
					if(Cl<= pPolar2->m_Cl[i] && Cl > pPolar2->m_Cl[i-1])
					{
						//interpolate
						if(fabs(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]) < 0.00001)
						{//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar2->m_Cl[i-1])
									 /(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]);
							Var2 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else
			{
				for (i=pt; i<size-1; i++)
				{
					if(pPolar2->m_Cl[i] <=Cl && Cl < pPolar2->m_Cl[i+1])
					{
						//interpolate
						if(fabs(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]) < 0.00001)
						{
							//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar2->m_Cl[i])
									 /(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]);
							Var2 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
					}
				}
			}
		}

		// then interpolate Variable

		double v =   (Re - pPolar1->m_Reynolds) / (pPolar2->m_Reynolds - pPolar1->m_Reynolds);
		double Var = Var1 + v * (Var2-Var1);
		return Var;
	}
}


double GetCl(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cl0, Cl1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cl0 = 2.0*PI*(Alpha*PI/180.0);
	else
		Cl0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
		Cl1 = 2.0*PI*(Alpha*PI/180.0);
	else
		Cl1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Cl0 + Tau * Cl1);
}


double GetCm0(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError)
{
	//Find 0-lift angle for local foil
	double Alpha;
	double Cm0, Cm1;
	double Cl0 = 1.0;
	double Cl1;
	bOutRe = false;
	bError = false;
	bool IsOutRe;
	bool IsError;

	bOutRe = false;
	for (int i=-10; i<10; i++)
	{
		Alpha = (double)i;
		Cl1 = GetCl(m_poaPolar, pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
		if(Cl1>0.0)
		{
			if(IsOutRe) bOutRe = true;
			if(IsError) bError = true;
			break;
		}
		Cl0 = Cl1;
	}
	if(Cl0>0.0)
	{
		return 0.0;
	}
	Cm0 = GetCm(m_poaPolar, pFoil0, pFoil1, Re, Alpha-1.0, Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	Cm1 = GetCm(m_poaPolar, pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	double Res = Cm0 + (Cm1-Cm0)*(0.0-Cl0)/(Cl1-Cl0);

	return Res;
}


double GetCm(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cm0, Cm1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cm0 = 0.0;
	else
		Cm0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1)
		Cm1 = 0.0;
	else
		Cm1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cm0 + Tau * Cm1);
}


double GetCd(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated viscous drag
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Cd0, Cd1, Cl;
	if(!pFoil0)
	{
		Cl = 2.0*PI*(Alpha*PI/180.0);
		Cd0 = Cl*Cl/PI/AR;
	}
	else Cd0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Cl = 2.0*PI*(Alpha*PI/180.0);
		Cd1 = Cl*Cl/PI/AR;
	}
	else Cd1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cd0 + Tau * Cd1);
}


double GetXCp(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated center of pressure position

//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double XCp0, XCp1;

	if(!pFoil0) return 0.0;
	else XCp0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1) return 0.0;
	else XCp1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * XCp0 + Tau * XCp1);
}


double GetXTr(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated position of the transition on the  surface specified by bTop

//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Tr0, Tr1;
	if(!pFoil0)
	{
		Tr0 = 1.0;
	}
	else
	{
		if(bTop) Tr0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr0 = GetPlrPointFromAlpha(m_poaPolar, pFoil0, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Tr1 = 1.0;
	}
	else
	{
		if(bTop) Tr1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr1 = GetPlrPointFromAlpha(m_poaPolar, pFoil1, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Tr0 + Tau * Tr1);
}


double GetZeroLiftAngle(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau)
{
	//returns the 0-lift angle of the foil, at Reynolds=Re
	//if the polar doesn't reach to 0-lift, returns Alpha0 = 0;
	double a01, a02;
	double Alpha00, Alpha01;
	int i;
	//Find the two polars which enclose Reynolds
	int size = 0;
	CPolar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0) Alpha00 = 0.0;
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha00 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha00 = 0.0;
	}

	if(!pFoil1) Alpha01 = 0.0;
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha01 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha01 = 0.0;
	}

	return ((1-Tau) * Alpha00 + Tau * Alpha01);
}


void GetLinearizedPolar(QList<void*>*m_poaPolar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope)
{
	//returns the 0-lift angle of the foil, at Reynolds=Re
	//if the polar doesn't reach to 0-lift, returns Alpha0 = 0;
	double Alpha00, Alpha01;
	double Slope0, Slope1;
	double AlphaTemp1, AlphaTemp2, SlopeTemp1, SlopeTemp2;
	int i;

//Find the two polars which enclose Reynolds
	int size = 0;
	CPolar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0)
	{
		Alpha00 = 0.0;
		Slope0 = 2.0 * PI *PI/180.0;
	}
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			pPolar1->GetLinearizedCl(AlphaTemp1, SlopeTemp1);
			pPolar2->GetLinearizedCl(AlphaTemp2, SlopeTemp2);
			Alpha00 = AlphaTemp1 +
					 (AlphaTemp2-AlphaTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
			Slope0  = SlopeTemp1 +
					 (SlopeTemp2-SlopeTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else
		{
			Alpha00 = 0.0;
			Slope0  = 2.0 * PI *PI/180.0;
		}
	}

	if(!pFoil1)
	{
		Alpha01 = 0.0;
		Slope1 = 2.0*PI *PI/180.0;
	}
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			pPolar1->GetLinearizedCl(AlphaTemp1, SlopeTemp1);
			pPolar2->GetLinearizedCl(AlphaTemp2, SlopeTemp2);
			Alpha01 = AlphaTemp1 +
			         (AlphaTemp2-AlphaTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
			Slope1  =  SlopeTemp1 +
			          (SlopeTemp2-SlopeTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else 
		{
			Alpha01 = 0.0;
			Slope1 = 2.0*PI *PI/180.0;
		}
	}

	Alpha0 = ((1-Tau) * Alpha00 + Tau * Alpha01);
	Slope  = ((1-Tau) * Slope0  + Tau * Slope1);
}




bool SplineInterpolation(int n, double *x, double *y, double *a, double *b, double *c, double *d)
{
//
// Given an array of n+1 pairs (x[i], y[i]), with i ranging from 0 to n,
// this function calculates the 3rd order cubic spline which interpolate the pairs.
//
// The spline is defined for each interval [x[j], x[j+1]) by n third order polynomial functions
//              p_j(x) = ax3 + bx2 + cx + d
//
// The equations to determine the coefficients a,b,c,d are
//
// Interpolation : 2n conditions
//    p_j(x[j])   = y[j];
//    p_j(x[j+1]) = y[j+1];
//
// Continuity of 1st and 2nd order derivatives at internal points: 2(n-1) conditions
//    p_j'(x[j]) = p_j+1'(x[j])
//    p_j"(x[j]) = p_j+1"(x[j])
//
// Second order derivative is zero at the end points : 2 conditions
//    p_j"(x[0]) =  p_j"(x[n]) =0
//
//
// This sets a linear system of size 4n which is solved by the Gauss algorithm for coefs a,b,c and d
// The RHS vector is
//	  a[0]
//	  b[0]
//	  c[0]
//	  d[0]
//	  a[1]
//    ...
//	  d[n-1]
//

	if(n>50) return false;
	int i,size;

	double M[16*MAXSTATIONS*MAXSTATIONS];// size is 4 coefs x maxstations
	double RHS[4*MAXSTATIONS*MAXSTATIONS];

	memset(M, 0, 16*n*n*sizeof(double));
	memset(RHS, 0, 4*n*sizeof(double));

	size = 4*n;
//	Interpolation conditions
	for (i=0; i<n; i++)
	{
		//pj(x[i]) = y[i]
		M[2*i*size +4*i]     = x[i]*x[i]*x[i];
		M[2*i*size +4*i + 1] = x[i]*x[i];
		M[2*i*size +4*i + 2] = x[i];
		M[2*i*size +4*i + 3] = 1.0;

		//pj(x[i+1]) = y[i+1]
		M[(2*i+1)*size +4*i]     = x[i+1]*x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 1] = x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 2] = x[i+1];
		M[(2*i+1)*size +4*i + 3] = 1.0;

		RHS[2*i]   = y[i];
		RHS[2*i+1] = y[i+1];
	}

//  Derivation conditions
	for (i=1; i<n; i++)
	{
		//continuity of 1st order derivatives

		M[(2*n+i)*size + 4*(i-1)]     =  3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*(i-1)+1]   =  2.0     *x[i];
		M[(2*n+i)*size + 4*(i-1)+2]   =  1.0;

		M[(2*n+i)*size + 4*i]   = -3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*i+1] = -2.0     *x[i];
		M[(2*n+i)*size + 4*i+2] = -1.0;

		RHS[2*n+i]   = 0.0;

		//continuity of 2nd order derivatives
		M[(3*n+i)*size + 4*(i-1)]     =  6.0*x[i];
		M[(3*n+i)*size + 4*(i-1)+1]   =  2.0     ;

		M[(3*n+i)*size + 4*i]   = -6.0*x[i];
		M[(3*n+i)*size + 4*i+1] = -2.0     ;

		RHS[3*n+i]   = 0.0;
	}

//	second order derivative is zero at end points = "natural spline"
	M[2*n*size]     = 6.0*x[0];
	M[2*n*size+1]   = 2.0;
	RHS[2*n]        = 0.0;

	M[3*n*size + size-4]   = 6.0*x[n];
	M[3*n*size + size-3]   = 2.0;
	RHS[3*n+1]             = 0.0;

	bool bCancel = false;
	if(!Gauss(M, 4*n, RHS, 1, &bCancel)) return false;

	for(i=0; i<n; i++)
	{
		a[i] = RHS[4*i];
		b[i] = RHS[4*i+1];
		c[i] = RHS[4*i+2];
		d[i] = RHS[4*i+3];
	}

	return true;
}


double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d)
{
	if(t<=y[0])
		return a[0] * t*t*t + b[0] *t*t + c[0] * t +d[0];

	for (int k=1; k<m; k++)
	{
		if(t<=y[k])
			return a[k-1] * t*t*t + b[k-1] *t*t + c[k-1] * t +d[k-1];

	}
	return a[m-1] * t*t*t + b[m-1] *t*t + c[m-1] * t +d[m-1];
}



int Compare(complex<double> a, complex<double>b)
{
	if(a.real()>b.real())         return  1;
	else if (a.real()<b.real()) return -1;
	else
	{	//same real part
		if(a.imag()>b.imag()) return  1;
		else if (a.imag()<b.imag())   return -1;
		else return 0;
	}
	return 0;
}


void ComplexSort(complex<double>*array, int ub)
{
	// bubble sort
	// derived from http://www.concentric.net/~ttwang/sort/sort.htm
	//
	int indx, indx2;
	complex<double> temp, temp2;
	int flipped;

	if (ub <= 1) return;

	indx = 1;
	do
	{
		flipped = 0;
		for (indx2 = ub - 1; indx2 >= indx; --indx2)
		{
			temp  = array[indx2];
			temp2 = array[indx2 - 1];
			if (Compare(temp2, temp) > 0)
			{
				array[indx2 - 1] = temp;
				array[indx2] = temp2;
				flipped = 1;
			}
		}
	} while ((++indx < ub) && flipped);
}


