/****************************************************************************

	Global Functions 
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
/**
* @file
* This file contains the declaration of methods used throughout the program and not specific to one application.
*/

#ifndef GLOBALS_H
#define GLOBALS_H


#include <QFile>
#include <QString>
#include <QTextStream>
#include <QColor>
#include <complex>
#include "params.h"
#include "objects/CVector.h"
#include "objects/Foil.h"
#include "objects/Polar.h"
#include <QList>

using namespace std;

bool IsEven(int n);
bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal,
			   CVector const &A,  CVector const &U,  CVector &I, double &dist);



bool ReadAVLString(QTextStream &in, int &Line, QString &strong);
bool Rewind1Line(QTextStream &in, int &Line, QString &strong);

void ReadValues(QString line, int &res, double &x, double &y, double &z);

void ExpFormat(double &f, int &exp);
void ReynoldsFormat(QString &str, double f);

bool Gauss(double *A, int n, double *B, int m, bool *pbCancel=NULL);


double GLGetRed(double tau);
double GLGetGreen(double tau);
double GLGetBlue(double tau);

double IntegralC2(double y1, double y2, double c1, double c2);
double IntegralCy(double y1, double y2, double c1, double c2);

QColor randomColor();

void ReadCString(QDataStream &ar, QString &strong);
void WriteCString(QDataStream &ar, QString const &strong);
void ReadCOLORREF(QDataStream &ar, QColor &color);
void WriteCOLORREF(QDataStream &ar, QColor const &color);

void Trace(int n);
void Trace(QString msg);
void Trace(QString msg, bool b);
void Trace(QString msg, int n);
void Trace(QString msg, double f);


Qt::PenStyle getStyle(int s);


double Det44(double *aij);
double Det33(double *aij);
complex<double> Det33(complex<double> *aij);
complex<double> Det44(complex<double> *aij);
complex<double> Cofactor44(complex<double> *aij, int &i, int &j);
bool Invert44(complex<double> *ain, complex<double> *aout);


void TestEigen();
void CharacteristicPol(double m[][4], double p[5]);
bool LinBairstow(double *p, complex<double> *root, int n);
bool Eigenvector(double a[][4], complex<double> lambda, complex<double> *V);


bool Crout_LU_Decomposition_with_Pivoting(double *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress);
bool Crout_LU_with_Pivoting_Solve(double *LU, double B[], int pivot[], double x[], int n, bool *pbCancel);

bool Crout_LU_Decomposition_with_Pivoting(float *A, int pivot[], int n, bool *pbCancel, double TaskSize, double &Progress);
bool Crout_LU_with_Pivoting_Solve(float *LU, double B[], int pivot[], double x[], int Size, bool *pbCancel);


void *GetPlrVariable(Polar *pPolar, int iVar);
double GetVar(int nVar, Foil *pFoil0, Foil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError);
double GetZeroLiftAngle(Foil *pFoil0, Foil *pFoil1, double Re, double Tau);
double GetCl(Foil  *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
double GetCm(Foil  *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
double GetCm0(Foil *pFoil0, Foil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError);
double GetCd(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
double GetXCp(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau,  bool &bOutRe, bool &bError);
double GetXTr(Foil *pFoil0, Foil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError);
double GetPlrPointFromCl(Foil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError);
double GetPlrPointFromAlpha(Foil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError);
void GetLinearizedPolar(Foil *pFoil0, Foil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope);

bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
double SplineBlend(int const &index, int const &p, double const &t, double *knots);


int Compare(complex<double> a, complex<double>b);
void ComplexSort(complex<double>*array, int ub);

#endif // FUNCTIONS_H
 
