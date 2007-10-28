#ifndef HELPER_H
#define HELPER_H

#include <QRect>
#include <math.h>
#include <vector>
#include "Vector.h"


typedef std::vector<double> tFoilCoo;

int EraseDoublePointsFoil(tFoilCoo &x,tFoilCoo &y);
bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool Intersect(int &x, int &y, const QRectF &DRect, const QPointF &Pt1, const QPointF &Pt2); 
bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M);
void TransformFoil(const tFoilCoo &x,const tFoilCoo &y, tFoilCoo &xT,tFoilCoo &yT);
inline double ToRad(double alpha){return alpha*M_PI/180.;}

#endif

