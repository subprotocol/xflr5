#ifndef HELPER_H
#define HELPER_H

#include <QRect>
#include "Vector.h"

bool IsBetween(int f, int f1, int f2);
bool IsBetween(int f, double f1, double f2);
bool Intersect(int &x, int &y, const QRect &DRect, const QPoint &Pt1, const QPoint &Pt2); 
bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M);

#endif

