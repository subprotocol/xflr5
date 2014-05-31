/****************************************************************************

	Globals Class
	Copyright (C) 2008-2013 Andre Deperrois adeperrois@xflr5.com

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

/**@file This file contains the definitions of methods used throughout the program and not specific to one application. */

#include "globals.h"
#include "mainframe.h"
#include <QtDebug>
#include <QPen>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime> 
#include <QByteArray>
#include <math.h>



/** 
* Returns a double number as its root and its base 10 exponent
* @param f the double number to reformat; is returned as f = f/pow(10.0,exp);
* @param exp the base 10 exponent of f.
*/
void ExpFormat(double &f, int &exp)
{
	if (f==0.0)
	{
		exp = 0;
		f = 0.0;
		return;
	}
	double f1 = qAbs(f);
//	int sgn = int(f/f1);
	if(f1<1)
		exp = (int)log10(f1)-1;
	else
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);

}


/**
* Solves a linear system using Gauss partial pivot method
*@param A a pointer to the single dimensionnal array of double values. Size is n².
*@param n the size of the square matrix
*@param B a pointer to the array of m RHS 
*@param m the number of RHS arrays to solve
*@param pbCancel a pointer to the boolean variable which holds true if the operation shold be interrupted.
*@return true if the problem was successfully solved.
*/
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
		max = qAbs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
		{
			if ((dum = qAbs(*(pA+row))) > max)
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


/**
* Tests if a given integer is between two other integers
*@param f the integer to test
*@param f1 the first bound
*@param f2 the second bound
*@return true if f1<f<f2 or f2<f<f1
*/
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


/**
* Tests if a given integer is between two double values
*@param f the integer to test
*@param f1 the first bound
*@param f2 the second bound
*@return true if f1<f<f2 or f2<f<f1
*/
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


/**
* Tests if a given integer is an even number
*@param n the integer to test
*@return true if n is an even number
*/
bool IsEven(int n)
{
	if(n%2==0) return true;
	else return false;
}

/**
* Returns the index of a Qt-style based on the index of the style in the array 
*@param s the index of the style
*@return The index of the Qt-style 
*/
Qt::PenStyle getStyle(int s)
{
	if(s==0)      return Qt::SolidLine;
	else if(s==1) return Qt::DashLine;
	else if(s==2) return Qt::DotLine;
	else if(s==3) return Qt::DashDotLine;
	else if(s==4) return Qt::DashDotDotLine;
	return Qt::SolidLine;
}


/**
*Reads one line from an AVL-format text file
*@deprecated the option to map AVL data was too comlplex and has been disabled.
*/
bool ReadAVLString(QTextStream &in, int &Line, QString &strong)
{
	bool bComment = true;
	int pos;

	while(bComment && !in.atEnd())
	{
		bComment = false;

		strong = in.readLine();
		if(in.atEnd()) return false;

		strong = strong.trimmed();
		pos = strong.indexOf("#",0);
		if(pos>=0) strong = strong.left(pos);
		pos = strong.indexOf("!",0);
		if(pos>=0) strong = strong.left(pos);

		if(strong.isEmpty()) bComment = true;

		Line++;
	}

	if(in.atEnd())
	{
		return false;
	}
	return true;
}



/**
* Reads the RGB int values of a color from binary datastream and returns a QColor. Inherited from the MFC versions of XFLR5.
*@param ar the binary datastream
*@param color the QColor read from the stream
*/
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

/**
* Writes the RGB int values of a color to a binary datastream. Inherited from the MFC versions of XFLR5.
*@param ar the binary datastream
*@param color the QColor to write to the stream
*/
void WriteCOLORREF(QDataStream &ar, QColor const &color)
{
	qint32 colorref;
	int r,g,b;

	color.getRgb(&r,&g,&b);

	colorref = b*256*256+g*256+r;
	ar << colorref;
}


/**
* Reads a sequence of characters from a binary stream and returns a QString. Inherited from the MFC versions of XFLR5.
*@param ar the binary datastream
*@param strong the QString read from the stream
*/
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

/**
* Writes a sequence of characters from a QStrinf to a binary stream. Inherited from the MFC versions of XFLR5.
*@param ar the binary datastream
*@param strong the QString to output to the stream
*/
void WriteCString(QDataStream &ar, QString const &strong)
{
	qint8 qi = strong.length();

	QByteArray textline;
	char *text;
	textline = strong.toLatin1();
	text = textline.data();
	ar << qi;
	ar.writeRawData(text, qi);
}




/**
*Rewinds one line from a QTextStream opened for reading
*@param in the stream opened for reading
*@param Line the index of the next Line to read
*@param strong the line to =ewind
*/
bool Rewind1Line(QTextStream &in, int &Line, QString &strong)
{
	int length = strong.length() * 1+2;//1 char takes one byte in the file ?

	int pos = in.pos();
	in.seek(pos-length);
	Line--;
	return true;
}


/**
* Takes a double number holding the value of a Reynolds number and returns a string.
*@param str the return string  with the formatted number
*@param f the Reynolds number to be formatted
*/
void ReynoldsFormat(QString &str, double f)
{
	int i, q, r, exp;
	f = (int(f/1000.0))*1000.0;

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


/**
* Outputs in a debug file the current time and the value of the integer passed as an input parameter.
* The file is in the user's default temporary directory with the name Trace.log
* The file is in the user's default temporary directory with the name Trace.log. 
* Used for debugging.
*@param n the integer to output
*/
void Trace(int n)
{
	if(!MainFrame::s_bTrace) return;
	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("hh:mm:ss  ");

	if(MainFrame::s_pTraceFile && MainFrame::s_pTraceFile->isOpen())
	{
		QTextStream ts(MainFrame::s_pTraceFile);
		ts << str << n << "\n";
	}
	qDebug()<<str<<n;
}


void Trace(QString msg, bool b)
{
	if(!MainFrame::s_bTrace) return;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("hh:mm:ss  ");
	if(b) str += msg + "=true";
	else  str += msg + "=false";

	if(MainFrame::s_pTraceFile && MainFrame::s_pTraceFile->isOpen())
	{
		QTextStream ts(MainFrame::s_pTraceFile);
		ts << str << "\n";
	}
	qDebug()<<str;
}

/**
* Outputs in a debug file the current time and a string message passed as an input parameter.
* The file is in the user's default temporary directory with the name Trace.log. 
* Used for debugging.
*@param msg the message to output
*/
void Trace(QString msg)
{
	if(!MainFrame::s_bTrace) return;
	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("hh:mm:ss  ")+ msg;

	if(MainFrame::s_pTraceFile && MainFrame::s_pTraceFile->isOpen())
	{
		QTextStream ts(MainFrame::s_pTraceFile);
		ts<<str<<"\n";
	}
	qDebug()<<str;
}


/**
* Outputs in a debug file the current time, a string message and the value of the integer passed as an input parameter.
* The file is in the user's default temporary directory with the name Trace.log. 
* Used for debugging.
*@param msg the message to output
*@param n the integer to output
*/
void Trace(QString msg, int n)
{
	if(!MainFrame::s_bTrace) return;

	QString strong;
	strong = QString("  %1").arg(n);
	strong = msg + strong;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("hh:mm:ss  ");
	str += strong;

	if(MainFrame::s_pTraceFile && MainFrame::s_pTraceFile->isOpen())
	{
		QTextStream ts(MainFrame::s_pTraceFile);
		ts << str << "\n";
	}
	qDebug()<<str;
}


/**
* Outputs in a debug file the current time, a string message and the value of the floating number passed as an input parameter.
* The file is in the user's default temporary directory with the name Trace.log. 
* Used for debugging.
*@param msg the message to output
*@param f the float number to output
*/
void Trace(QString msg, double f)
{
	if(!MainFrame::s_bTrace) return;

	QString strong;
	strong = QString("  %1").arg(f);
	strong = msg + strong;

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("hh:mm:ss  ");

	str += strong;

	if(MainFrame::s_pTraceFile && MainFrame::s_pTraceFile->isOpen())
	{
		QTextStream ts(MainFrame::s_pTraceFile);
		ts << str << "\n";
	}
	qDebug()<<str;
}


/**
* Returns the red component of a color scale depending on an input parameter with value between 0 and 1.
* Used to draw a color scale between 0=blue and 1=red
*@param tau the input parameter between 0 and 1.
*@return the red component of the color
*/
double GLGetRed(double tau)
{
	if(tau>2.0/3.0)      return 1.0;
	else if(tau>1.0/3.0) return (3.0*(tau-1.0/3.0));
	else                 return 0.0;
}


/**
* Returns the green component of a color scale depending on an input parameter with value between 0 and 1.
* Used to draw a color scale between 0=blue and 1=red
*@param tau the input parameter between 0 and 1.
*@return the green component of the color
*/
double GLGetGreen(double tau)
{
	if(tau<0.f || tau>1.0) 	return 0.0;
	else if(tau<1.0/4.0) 	return (4.0*tau);
	else if(tau>3.0/4.0) 	return (1.0-4.0*(tau-3.0/4.0));
	else                    return 1.0;
}


/**
* Returns the blue component of a color scale depending on an input parameter with value between 0 and 1.
* Used to draw a color scale between 0=blue and 1=red
*@param tau the input parameter between 0 and 1.
*@return the blue component of the color
*/
double GLGetBlue(double tau)
{
	if(tau>2.0/3.0)      return 0.0;
	else if(tau>1.0/3.0) return (1.0-3.0*(tau-1.0/3.0));
	else                 return 1.0;
}


/**
 Auxiliary integral used in LLT calculations
*/
double IntegralC2(double y1, double y2, double c1, double c2)
{
	double res = 0.0;

	if (qAbs(y2-y1)<1.e-5) return 0.0;
	double g = (c2-c1)/(y2-y1);

	res = (c1-g*y1)*(c1-g*y1)*(y2-y1) +
		  g * (c1-g*y1)      *(y2*y2-y1*y1)+
		  g*g/3.0            *(y2*y2*y2-y1*y1*y1);

	return res;
}


/**
 Auxiliary integral used in LLT calculations
*/
double IntegralCy(double y1, double y2, double c1, double c2)
{
	double res = 0.0;
	if (qAbs(y2-y1)<1.e-5) return (y1+y2)/2.0 * (c1+c2)/2.0;

	double g = (c2-c1)/(y2-y1);
	res = (c1-g*y1)/2.0 *(y2*y2 - y1*y1) + g/3.0 * (y2*y2*y2-y1*y1*y1);
	return res;
}

/**
* Extracts three double values from a QString, and returns the number of extracted values.
*/
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



/**
  int Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n)   
                                                                            
  Unknown author http:mymathlib.webtrellis.net/index.html                 
                                                                            
  Description:                                                              
     This routine uses Crout's method to decompose a row interchanged       
     version of the n x n matrix A into a lower triangular matrix L and a   
     unit upper triangular matrix U such that A = LU.                       
     The matrices L and U replace the matrix A so that the original matrix  
     A is destroyed.                                                        
     Note!  In Crout's method the diagonal elements of U are 1 and are      
            not stored.                                                     
     Note!  The determinant of A is the product of the diagonal elements    
            of L.  (det A = det L * det U = det L).                         
     The LU decomposition is convenient when one needs to solve the linear  
     equation Ax = B for the vector x while the matrix A is fixed and the   
     vector B is varied.  The routine for solving the linear system Ax = B  
     after performing the LU decomposition for A is                         
                      Crout_LU_with_Pivoting_Solve.                         
     (see below).                                                           
                                                                            
     The Crout method with partial pivoting is: Determine the pivot row and 
     interchange the current row with the pivot row, then assuming that     
     row k is the current row, k = 0, ..., n - 1 evaluate in order the      
     the following pair of expressions                                      
       L[i][k] = (A[i][k] - (L[i][0]*U[0][k] + . + L[i][k-1]*U[k-1][k]))    
                                 for i = k, ... , n-1,                      
       U[k][j] = A[k][j] - (L[k][0]*U[0][j] + ... + L[k][k-1]*U[k-1][j])    
                                                                  / L[k][k] 
                                      for j = k+1, ... , n-1.               
       The matrix U forms the upper triangular matrix, and the matrix L     
       forms the lower triangular matrix.                                   
                                                                            
  Arguments:                                                                
     double *A       Pointer to the first element of the matrix A[n][n].    
     int    pivot[]  The i-th element is the pivot row interchanged with    
                     row i.                                                 
     int     n       The number of rows or columns of the matrix A.         
                                                                            
  Return Values:                                                            
     0  Success                                                             
    -1  Failure - The matrix A is singular.                                 
                                                                            
*/
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
		max = qAbs( *(p_k + k) );
		for (j=k+1, p_row=p_k+n; j<n; j++, p_row+=n)
		{
			if (max<qAbs(*(p_row+k)))
			{
				max = qAbs(*(p_row+k));
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

bool Crout_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress)
{
	int i, j, k;
	float *p_k, *p_row, *p_col;
	float max=0.0;

	p_col = NULL;

	//  For each row and column, k = 0, ..., n-1,
	for (k=0, p_k=A; k<n; p_k+=n, k++)
	{
	//  find the pivot row
		pivot[k] = k;
		p_col = p_k+k;
		max = qAbs( *(p_k + k) );
		for (j=k+1, p_row=p_k+n; j<n; j++, p_row+=n)
		{
			if (max<qAbs(*(p_row+k)))
			{
				max = qAbs(*(p_row+k));
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


/**
  int Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[],     
                                                        double x[], int n)  
                                                                            
 Unknown author http:mymathlib.webtrellis.net/index.html                  
                                                                            
  Description:                                                              
     This routine uses Crout's method to solve the linear equation Ax = B.  
     This routine is called after the matrix A has been decomposed into a   
     product of a lower triangular matrix L and a unit upper triangular     
     matrix U without pivoting.  The argument LU is a pointer to the matrix 
     the superdiagonal part of which is U and the subdiagonal together with 
     the diagonal part is L. (The diagonal part of U is 1 and is not        
     stored.)   The matrix A = LU.                                          
     The solution proceeds by solving the linear equation Ly = B for y and  
     subsequently solving the linear equation Ux = y for x.                 
                                                                            
  Arguments:                                                                
     double *LU      Pointer to the first element of the matrix whose       
                     elements form the lower and upper triangular matrix    
                     factors of A.                                          
     double *B       Pointer to the column vector, (n x 1) matrix, B.       
     int    pivot[]  The i-th element is the pivot row interchanged with    
                     row i.                                                 
     double *x       Solution to the equation Ax = B.                       
     int     n       The number of rows or columns of the matrix LU.        
                                                                            
  Return Values:                                                            
     true  : Success                                                        
     false : Failure - The matrix A is singular.                            
                                                                            
*/
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

bool Crout_LU_with_Pivoting_Solve(float *matLU, double B[], int pivot[], double x[], int Size, bool *pbCancel)
{
	int i, k;
	float *pk;
	double dum;

	//  Solve the linear equation Lx = B for x, where L is a lower triangular matrix.
	for (k=0, pk=matLU; k<Size; pk+=Size, k++)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		x[k] = B[k];
		for (i=0; i<k; i++) x[k]-=x[i] * *(pk+i);
		x[k] /= *(pk+k);

		qApp->processEvents();
		if(*pbCancel) return false;
	}

	//  Solve the linear equation Ux = y, where y is the solution
	//  obtained above of Lx = B and U is an upper triangular matrix.
	//  The diagonal part of the upper triangular part of the matrix is
	//  assumed to be 1.0.
	for (k=Size-1, pk=matLU+Size*(Size-1); k>=0; k--, pk-=Size)
	{
		if (pivot[k] != k)
		{
			dum=B[k]; B[k]=B[pivot[k]]; B[pivot[k]]=dum;
		}

		for (i=k+1; i<Size; i++) x[k]-=x[i] * *(pk+i);
		if (fabs(*(pk+k))<PRECISION)
		{
			return false;
		}

		qApp->processEvents();
		if(*pbCancel) return false;
	}

	return true;
}


/** 
*Returns the determinant of a 4x4 matrix
*@param aij a pointer to a one-dimensional array holding the 16 double values of the matrix
*@return the matrix's determinant
*/
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



/** 
*Returns the determinant of a 3x3 matrix
*@param aij a pointer to a one-dimensional array holding the 9 double values of the matrix
*@return the matrix's determinant
*/
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



/** 
*Returns the determinant of a complex 3x3 matrix
*@param aij a pointer to a one-dimensional array holding the 9 complex values of the matrix
*@return the matrix's determinant
*/
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


/** 
*Returns the cofactor of an element in a 4x4 matrix of complex values.
*@param aij a pointer to a one-dimensional array holding the 16 complex values of the matrix.
*@param i the number of the element's line, starting at 0.
*@param j the number of the element's column, starting at 0.
*@return the cofactor of element (i,j).
*/
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

/** 
*Returns the determinant of a complex 4x4 matrix
*@param aij a pointer to a one-dimensional array holding the 16 complex double values of the matrix
*@return the matrix's determinant
*/
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

/** 
*Inverts a complex 4x4 matrix
*@param ain in input, a pointer to a one-dimensional array holding the 16 complex values of the input matrix
*@param aout in output, a pointer to a one-dimensional array holding the 16 complex values of the inverted matrix
*@return if the inversion was successful
*/
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




/**
* Returns the coefficients of the characteristic polynomial of a 4x4 matrix of double values. Thanks Mapple.
* The polynom can then be solved for complex roots using Bairstow's algorithm
*@param m the 4x4 matrix
*@param p the array holding the 5 coefficients of the matrix characteristic polynomial
*/
void CharacteristicPol(double m[][4], double p[5])
{
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

/** Text function, for debugging purposes only*/
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


/**________________________________________________________________________
* Finds the eigenvector associated to an eigenvalue.
* Solves the system A.V = lambda.V where A is a 4x4 complex matrix
* in input :
*    - matrix A
*    - the array of complex eigenvalues
* in output
*    - the array of complex eigenvectors
*
* The eigenvector is calculated by direct matrix inversion.
* One of the vector's component is set to 1, to avoid the trivial solution V=0;
*
* (c) Andre Deperrois October 2009
*@param a the complex two-dimensional 4x4 input matrix to diagonalize
*@param lambda the output array of four complex eigenvalues
*@param V the eigenvector as a one-dimensional array of complex values
*________________________________________________________________________ */
bool Eigenvector(double a[][4], complex<double> lambda, complex<double> *V)
{
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

/**
* Finds the complex roots of a polynom P(x) using Lin-Bairstow's method
* P(x) = Sum p_i x^i	    i = 0..n;
* The polynoms coefficient are in array p
*
* Andre Deperrois October 2009
*@param p the array of the polynoms double's coefficients
*@param root the array of the polynom's complex roots
*@param n the polynom's order
*@return true if the extraction was successful
*/
bool LinBairstow(double *p, complex<double> *root, int n)
{
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
			root[nn-1] = complex<double>(r/2.0,  sqrt(qAbs(Delta))/2.0);
			root[nn-2] = complex<double>(r/2.0, -sqrt(qAbs(Delta))/2.0);
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
			if(qAbs(p[2])<PRECISION)
			{
				// one last root, but we should never get here
				if(qAbs(p[1])>PRECISION)
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
					root[nn-1] = complex<double>(-p[1]/2.0/p[2],  sqrt(qAbs(Delta))/2.0/p[2]);
					root[nn-2] = complex<double>(-p[1]/2.0/p[2], -sqrt(qAbs(Delta))/2.0/p[2]);
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
			if(qAbs(p[1])>PRECISION)
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


/**
*Interpolates a variable on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param nVar the index of the variable to interpolate.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Cl the lift coefficient at the point's position, used as the input parameter.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetVar(int nVar, Foil *pFoil0, Foil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError)
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
	else Var0 = GetPlrPointFromCl(pFoil0, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;


	if(!pFoil1)
	{
		Cl = 0.0;
		Var1 = 0.0;
	}
	else Var1 = GetPlrPointFromCl(pFoil1, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Var0 + Tau * Var1);
}

/**
* Returns a pointer to a variable array of a polar object, based on the variable's index
* @param pPolar the pointer to the polar object
* @param iVar the index of the variable
* @return the pointer to the array holding the values of the variable
*/
void * GetPlrVariable(Polar *pPolar, int iVar)
{
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


/**
* Returns the value of an aero coefficient, interpolated on a polar mesh, and based on the value of the Reynolds Number and of the aoa.
* Proceeds by identifiying the two polars surronding Re, then interpolating both with the value of Alpha, 
* last by interpolating the requested variable between the values measured on the two polars.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil the pointer to the foil
*@param Re the Reynolds number .
*@param Alpha the angle of attack.
*@param PlrVar the index of the variable to interpolate.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetPlrPointFromAlpha(Foil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError)
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
	Polar *pPolar;

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
	for (i=0; i<Polar::s_oaPolar.size(); i++)
	{
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) && (pPolar->foilName() == pFoil->foilName()))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	Polar * pPolar1 = NULL;
	Polar * pPolar2 = NULL;
	int nPolars = Polar::s_oaPolar.size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) &&
			(pPolar->foilName() == pFoil->foilName()) &&
			pPolar->m_Alpha.size()>0)
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
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) &&
		   (pPolar->foilName() == pFoil->foilName())  &&
			pPolar->m_Alpha.size()>0)
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


/**
* Returns the value of an aero coefficient, interpolated on a polar mesh, and based on the value of the Reynolds Number and of the lift coefficient.
* Proceeds by identifiying the two polars surronding Re, then interpolating both with the value of Alpha, 
* last by interpolating the requested variable between the values measured on the two polars.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil the pointer to the foil
*@param Re the Reynolds number .
*@param Cl the lift coefficient, used as the input parameter for interpolation.
*@param PlrVar the index of the variable to interpolate.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetPlrPointFromCl(Foil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError)
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
	Polar *pPolar;
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
	for (i = 0; i< Polar::s_oaPolar.size(); i++)
	{
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) && (pPolar->foilName() == pFoil->foilName()))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	Polar * pPolar1 = NULL;
	Polar * pPolar2 = NULL;
	int nPolars = Polar::s_oaPolar.size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) &&
		   (pPolar->foilName() == pFoil->foilName()) &&
			pPolar->m_Cl.size()>0)
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
		pPolar = (Polar*)Polar::s_oaPolar.at(i);
		if((pPolar->polarType()==FIXEDSPEEDPOLAR) && (pPolar->foilName() == pFoil->foilName())  && pPolar->m_Cl.size()>0)
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
			dist = qAbs(pPolar1->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (qAbs(pPolar1->m_Cl[i])< dist)
				{
					dist = qAbs(pPolar1->m_Cl[i]);
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
						if(qAbs(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]) < 0.00001)
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
						if(qAbs(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]) < 0.00001){//do not divide by zero
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
			dist = qAbs(pPolar2->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (qAbs(pPolar2->m_Cl[i])< dist)
				{
					dist = qAbs(pPolar2->m_Cl[i]);
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
						if(qAbs(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]) < 0.00001)
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
						if(qAbs(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]) < 0.00001)
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

/**
*Interpolates the lift coefficient on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param nVar the index of the variable to interpolate.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Alpha the apparent aoa at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetCl(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cl0, Cl1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cl0 = 2.0*PI*(Alpha*PI/180.0);
	else
		Cl0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
		Cl1 = 2.0*PI*(Alpha*PI/180.0);
	else
		Cl1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Cl0 + Tau * Cl1);
}

/**
*Returns the zero-lift moment coefficient interpolated on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value for the zero-moment lift coefficient.
*/
double GetCm0(Foil *pFoil0, Foil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError)
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
		Cl1 = GetCl(pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
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
	Cm0 = GetCm(pFoil0, pFoil1, Re, Alpha-1.0, Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	Cm1 = GetCm(pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	double Res = Cm0 + (Cm1-Cm0)*(0.0-Cl0)/(Cl1-Cl0);

	return Res;
}

/**
*Interpolates the moment coefficient on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param nVar the index of the variable to interpolate.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Alpha the apparent aoa  at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetCm(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cm0, Cm1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cm0 = 0.0;
	else
		Cm0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1)
		Cm1 = 0.0;
	else
		Cm1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cm0 + Tau * Cm1);
}


/**
*Interpolates the drag coefficient on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param nVar the index of the variable to interpolate.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Alpha the apparent aoa  at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetCd(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated viscous drag
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
	else Cd0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Cl = 2.0*PI*(Alpha*PI/180.0);
		Cd1 = Cl*Cl/PI/AR;
	}
	else Cd1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cd0 + Tau * Cd1);
}


/**
*Interpolates the center of pressure's x-position coefficient on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Alpha the apparent aoa  at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetXCp(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated center of pressure position

	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double XCp0, XCp1;

	if(!pFoil0) return 0.0;
	else XCp0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1) return 0.0;
	else XCp1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * XCp0 + Tau * XCp1);
}


/**
*Interpolates transition locations on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Alpha the apparent aoa  at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param bTop true if the upper transition is requested, false otherwise
*@param bOutRe true if Cl is outside the min or max Cl of the polar mesh.
*@param bError if Re is outside the min or max Reynolds number of the polar mesh.
*@return the interpolated value.
*/
double GetXTr(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated position of the transition on the  surface specified by bTop


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
		if(bTop) Tr0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Tr1 = 1.0;
	}
	else
	{
		if(bTop) Tr1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Tr0 + Tau * Tr1);
}


/**
*Interpolates the zero-lift angle on the polar mesh, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Tau the relative position of the point between the two foils.
*@return the interpolated value.
*/
double GetZeroLiftAngle(Foil *pFoil0, Foil *pFoil1, double Re, double Tau)
{
	//returns the 0-lift angle of the foil, at Reynolds=Re
	//if the polar doesn't reach to 0-lift, returns Alpha0 = 0;
	double a01, a02;
	double Alpha00, Alpha01;
	int i;
	//Find the two polars which enclose Reynolds
	int size = 0;
	Polar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0) Alpha00 = 0.0;
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<Polar::s_oaPolar.size(); i++)
		{
			pPolar = (Polar*)Polar::s_oaPolar.at(i);
			if(pPolar->foilName() == pFoil0->foilName()) size++;
		}
		if(size)
		{
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil0->foilName())
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil0->foilName())
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
		for (i=0; i<Polar::s_oaPolar.size(); i++)
		{
			pPolar = (Polar*)Polar::s_oaPolar.at(i);
			if(pPolar->foilName() == pFoil1->foilName()) size++;
		}
		if(size)
		{
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil1->foilName())
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil1->foilName())
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


/**
*Returns the coefficient of an approximate linearized Cl=f(aoa) curve, based on the geometrical position of a point between two sections on a wing.
*@param m_poaPolar the pointer to the array of polars.
*@param pFoil0 the pointer to the left foil  of the wing's section.
*@param pFoil1 the pointer to the left foil  of the wing's section.
*@param Re the Reynolds number at the point's position.
*@param Tau the relative position of the point between the two foils.
*@param Alpha0 the zero-lift angle; if the interpolation fails, returns Alpha0 = 0
*@param Slope the slope of the lift curve; if the interpolation fails, returns Slope = 2 PI
*/
void GetLinearizedPolar(Foil *pFoil0, Foil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope)
{
	double Alpha00, Alpha01;
	double Slope0, Slope1;
	double AlphaTemp1, AlphaTemp2, SlopeTemp1, SlopeTemp2;
	int i;

//Find the two polars which enclose the Reynolds number
	int size = 0;
	Polar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0)
	{
		Alpha00 = 0.0;
		Slope0 = 2.0 * PI *PI/180.0;
	}
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<Polar::s_oaPolar.size(); i++)
		{
			pPolar = (Polar*)Polar::s_oaPolar.at(i);
			if(pPolar->foilName() == pFoil0->foilName()) size++;
		}
		if(size)
		{
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil0->foilName())
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil0->foilName())
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
		for (i=0; i<Polar::s_oaPolar.size(); i++)
		{
			pPolar = (Polar*)Polar::s_oaPolar.at(i);
			if(pPolar->foilName() == pFoil1->foilName()) size++;
		}
		if(size)
		{
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil1->foilName())
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<Polar::s_oaPolar.size(); i++)
			{
				pPolar = (Polar*)Polar::s_oaPolar.at(i);
				if(pPolar->foilName() == pFoil1->foilName())
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



/**
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
*/
bool SplineInterpolation(int n, double *x, double *y, double *a, double *b, double *c, double *d)
{
	if(n>50) return false;
	int i,size;

	double *M = new double[16*n*n];
	double *RHS = new double[4*n*n];

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

	delete [] M;
	delete [] RHS;
	return true;
}


/**
* Method for the comparison of two complex number
*@param a first complex number
*@param b second complex number
*@return 1 if Real(a) > Real(b), -1 if Real(a)<Real(b); if Real(a)=Real(b), returns 1 if Imag(a)>Image(b), -1 otherwise.
*/
int Compare(complex<double> a, complex<double>b)
{
	if(a.real()>b.real())       return  1;
	else if (a.real()<b.real()) return -1;
	else
	{	//same real part
		if(a.imag()>b.imag())         return  1;
		else if (a.imag()<b.imag())   return -1;
		else return 0;
	}
	return 0;
}


/**
* Bubble sort algorithm for complex numbers
*@param array the array of complex numbers to sort
*@param ub the size of the aray
*/
void ComplexSort(complex<double>*array, int ub)
{
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



/**
* Returns the intersection of a ray with the object's panels
* The ray is defined by a mouse click and is perpendicular to the viewport
*	A is the ray's origin,
*	U is the ray's direction
*	LA, LB, TA, TB define a quadrangle in 3D space.
*	N is the normal to the quadrangle
*	I is the resulting intersection point of the ray and the quadrangle, if inside the quadrangle
*	dist = |AI|
*	The return value is true if the intersection inside the quadrangle, false otherwise
**/
bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal,
			   CVector const &A,  CVector const &U,  CVector &I, double &dist)
{
	static CVector P, W, V, T;
	bool b1, b2, b3, b4;
	double r,s;

	r = (LA.x-A.x)*Normal.x + (LA.y-A.y)*Normal.y + (LA.z-A.z)*Normal.z ;
	s = U.x*Normal.x + U.y*Normal.y + U.z*Normal.z;

	dist = 10000.0;

	if(qAbs(s)>0.0)
	{
		dist = r/s;

		//inline operations to save time
		P.x = A.x + U.x * dist;
		P.y = A.y + U.y * dist;
		P.z = A.z + U.z * dist;

		// P is inside the panel if on left side of each panel side
		W.x = P.x  - TA.x;
		W.y = P.y  - TA.y;
		W.z = P.z  - TA.z;
		V.x = TB.x - TA.x;
		V.y = TB.y - TA.y;
		V.z = TB.z - TA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b1 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - TB.x;
		W.y = P.y  - TB.y;
		W.z = P.z  - TB.z;
		V.x = LB.x - TB.x;
		V.y = LB.y - TB.y;
		V.z = LB.z - TB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b2 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - LB.x;
		W.y = P.y  - LB.y;
		W.z = P.z  - LB.z;
		V.x = LA.x - LB.x;
		V.y = LA.y - LB.y;
		V.z = LA.z - LB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b3 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		W.x = P.x  - LA.x;
		W.y = P.y  - LA.y;
		W.z = P.z  - LA.z;
		V.x = TA.x - LA.x;
		V.y = TA.y - LA.y;
		V.z = TA.z - LA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		b4 = (T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0);

		if(b1 && b2 && b3 && b4)
		{
			I.Set(P.x, P.y, P.z);
			return true;
		}
	}
	return false;
}

/**
*	Calculates the blending value of a point on a BSpline. This is done recursively.
*	If the numerator and denominator are 0 the expression is 0.
*	If the denominator is 0 the expression is 0
*
*	   index   is the control point's index
*	   p       is the spline's degree
*	   t       is the spline parameter
*/
double SplineBlend(int const &index, int const &p, double const &t, double *knots)
{
	static double eps = 1.e-6;
	double value;

	if(p==0)
	{
		if ((knots[index] <= t) && (t < knots[index+1]) ) value = 1.0;
//		else if (abs(knots[index]-knots[index+1])<pres)   value = 0.0;
		else                                              value = 0.0;
	}
	else
	{
		if (qAbs(knots[index+p] - knots[index])<eps && qAbs(knots[index+p+1] - knots[index+1])<eps)
			value = 0.0;
		else if (qAbs(knots[index+p] - knots[index])<eps)
			value = (knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1, p-1, t, knots);
		else if (qAbs(knots[index+p+1] - knots[index+1])<eps)
			value = (t-knots[index])     / (knots[index+p] - knots[index])    * SplineBlend(index,   p-1, t, knots);
		else
			value = (t-knots[index])     / (knots[index+p]  -knots[index])    * SplineBlend(index,   p-1, t, knots) +
					(knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1 ,p-1, t, knots);
	}
	return value;
}


QColor randomColor()
{
	QColor clr;
	clr.setHsv((int)(((double)qrand()/(double)RAND_MAX)*360),
			 (int)(((double)qrand()/(double)RAND_MAX)*101)+ 99,
			 (int)(((double)qrand()/(double)RAND_MAX)*100)+155,
			 255);
	return clr;
}















