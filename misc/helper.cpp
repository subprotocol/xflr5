#include <iostream>
#include "helper.h"


bool IsBetween(int f, int f1, int f2)
{
	if (f2 < f1){
		int tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(f<f1) return false;
	else if(f>f2) return false;
	return true;
}


bool IsBetween(int f, double f1, double f2)
{
	double ff = f;
	if (f2 < f1){
		double tmp = f2;
		f2 = f1;
		f1 = tmp;
	}
	if(ff<f1) return false;
	else if(ff>f2) return false;
	return true;
}



bool Intersect(int &x, int &y, const QRect &DRect, const QPoint &Pt1, const QPoint &Pt2)
{

	int xt, yt;
	double x1 = (double)Pt1.x();
	double x2 = (double)Pt2.x();
	double y1 = (double)Pt1.y();
	double y2 = (double)Pt2.y();


	if (IsBetween(DRect.left(), Pt1.x(), Pt2.x())){
		yt = int(y1 + (y2-y1)/(x2-x1) * ((double)DRect.left()-x1));
		if (IsBetween(yt, DRect.bottom(), DRect.top())) {
			x = DRect.left();
			y = yt;
			return true;
		}
	}
	if (IsBetween(DRect.right(), Pt1.x(), Pt2.x())){
		yt = int(y1 + (y2-y1)/(x2-x1) * ((double)DRect.right()-x1));
		if (IsBetween(yt, DRect.bottom(), DRect.top())) {
			x = DRect.right();
			y = yt;
			return true;
		}
	}
	if (IsBetween(DRect.bottom(), Pt1.y(), Pt2.y())){
		xt = int(x1 + (x2-x1)/(y2-y1) * ((double)DRect.bottom()-y1));
		if (IsBetween(xt, DRect.left(), DRect.right())) {
			x = xt;
			y = DRect.bottom();
			return true;
		}
	}
	if (IsBetween(DRect.top(), Pt1.y(), Pt2.y())){
		xt = int(x1 + (x2-x1)/(y2-y1) * ((double)DRect.top()-y1));
		if (IsBetween(xt, DRect.left(), DRect.right())) {
			x = xt;
			y = DRect.top();
			return true;
		}
	}
	return false;
}



bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M)
{//ABCD are assumed to lie in the xy plane
	//returns true and intersection point M if AB and CD intersect inside
	//returns false and intersection point M if AB and CD intersect outside
	M->x = 0.0;
	M->y = 0.0;
	M->z = 0.0;
	CVector AB(B.x-A.x, B.y-A.y, B.z-A.z); 
	CVector CD(D.x-C.x, D.y-C.y, D.z-C.z); 

	//Cramer's rule

	double Det  = -AB.x * CD.y + CD.x * AB.y;
	if(Det==0.0) {//vectors are parallel, no intersection
		return false;
	}
	double Det1 = -(C.x-A.x)*CD.y + (C.y-A.y)*CD.x;
	double Det2 = -(C.x-A.x)*AB.y + (C.y-A.y)*AB.x;

	double t = Det1/Det;
	double u = Det2/Det;

	M->x = A.x + t*AB.x;
	M->y = A.y + t*AB.y;

	if (0.0<=t && t<=1.0 && 0.0<=u && u<=1.0)	return true;//M is between A and B
	else										return false;//M is outside
}
 
