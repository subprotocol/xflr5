/***************************************************************************
 *   Copyright (C) 2007 by Andres Chavarria Krauser   *
 *   andres.chavarria@bioquant.uni-heidelberg.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NNUMERICSCUBICSPLINE_H
#define NNUMERICSCUBICSPLINE_H

#include <fstream>
#include <vector>
#include <limits>
#include "linsolve.h"


namespace nNumerics {

/**
	@author Andres Chavarria Krauser <andres.chavarria@bioquant.uni-heidelberg.de>
*/
class CubicSplineES{
public:
    CubicSplineES(const std::vector<double> &X, const std::vector<double> &Y): x(X), y(Y), N(X.size()){
			A=new double[(N/*-2*/)*(N/*-2*/)];
			zx=new double [N/*-2*/];
			zy=new double [N/*-2*/];

			FillMatrix(A,N/*-2*/);
			FillRHS(zx,x,N/*-2*/);
			nNumerics::linsolve(A,zx,N/*-2*/);


			FillMatrix(A,N/*-2*/);
			FillRHS(zy,y,N/*-2*/);
			nNumerics::linsolve(A,zy,N/*-2*/);
		};

    ~CubicSplineES(){
			delete []zy;
			delete []zx;
			delete []A;
		};
		
		double operator()(const double t,int coordinate){
			double val=0.;
			int tInt=static_cast<int>(t);
			double *z=0;

			if(coordinate==0)z=zx;
			else if(coordinate==1)z=zy;

// 			if(tInt<N-2){
				val+=z[tInt+1]*pow(t-tInt,3)/6.;
				val-=z[tInt+1]*(t-tInt)/6.;
// 			}
// 			if(tInt>0){
				val+=z[tInt]*pow(tInt+1-t,3)/6.;
				val-=z[tInt]*(tInt+1-t)/6.;
// 			}
			if(coordinate==0){
				val+=x[tInt+1]*(t-tInt);
				val+=x[tInt]*(tInt+1-t);
			}else if(coordinate==1){
				val+=y[tInt+1]*(t-tInt);
				val+=y[tInt]*(tInt+1-t);
			}
			return val;
		}

		double tangent(const double t, int coordinate){
			double val=0.;
/*			int tInt=static_cast<int>(t);
			TNT::Array1D<double> *z=0;

			if(coordinate==0)z=&zx;
			else if(coordinate==1)z=&zy;

			if(tInt<N-2){
				val+=(*z)[tInt+1]*pow(t-tInt,2)/2.;
				val-=(*z)[tInt+1]/6.;
			}
			if(tInt>0){
				val-=(*z)[tInt]*pow(tInt+1-t,2)/2.;
				val-=(*z)[tInt]/6.;
			}
			if(coordinate==0){
				val+=x[tInt+1];
				val-=x[tInt];
			}else if(coordinate==1){
				val+=y[tInt+1];
				val-=y[tInt];
			}*/
			return val;
		}
private:
	void FillMatrix(double *A,int n){
			for(unsigned int i=0;i<n*n;i++)A[i]=0.;
			// t equally spaced
			A[nIndex(0,0,n)]=1.;
//			A[nIndex(0,0,n)]=4.; A[nIndex(1,0,n)]=1.;
			for(unsigned int i=1;i<n-1;i++){
				A[nIndex(i-1,i,n)]=1.;
				A[nIndex(i,i,n)]=4.;
				A[nIndex(i+1,i,n)]=1.;
			}
			//A[nIndex(n-1,n-1,n)]=4.; A[nIndex(n-2,n-1,n)]=1.;
			A[nIndex(n-1,n-1,n)]=1.;
	}

	void FillRHS(double *b,const std::vector<double> &X,int n){
			b[0]=0.;
			for(unsigned int i=1;i<n-1;i++){
				b[i]=6.*(X[i+/*2*/1]-2.*X[i/*+1*/]+X[i-1]);
			}
			b[n-1]=0.;
	}

private:
	const std::vector<double> &x;
	const std::vector<double> &y;
	const int N;
	double *A;
  double *zx;
	double *zy;
};


class CubicSpline2D{
public:
		CubicSpline2D(){
			initialized=false;
			xP=yP=0; N=0;	
			A=dx=dy=0;
		};
	
    CubicSpline2D(const std::vector<double> &X, const std::vector<double> &Y){
			initialized=false;
			xP=yP=0; N=0;	
			A=dx=dy=0;

			Init(X,Y);
		};

    virtual ~CubicSpline2D(){
			Clear();
		};
		
		void Clear(){
			initialized=false;
			xP=yP=0; N=0;
			if(dy){delete []dy;dy=0;}
			if(dx){delete []dx;dx=0;}
			if(A){delete []A;A=0;}
		};


		void Init(const std::vector<double> &X, const std::vector<double> &Y){
			if(X.size()!=N){
				Clear();
				xP=&X;yP=&Y; N=X.size();
				A=new double[N*N];
				dx=new double [N];
				dy=new double [N];
			}

			FillMatrix(A,N);
			FillRHS(dx,*xP,N);
			linsolve(A,dx,N);

			FillMatrix(A,N);
			FillRHS(dy,*yP,N);
			linsolve(A,dy,N);
			initialized=true;
		}

		double operator()(const double t,int coo){
			if(!initialized)return std::numeric_limits<double>::quiet_NaN();
			double val=0.;
			int tInt=static_cast<int>(t);
			double tn=t-tInt;

			if(coo==0){
				val=(*xP)[tInt] + dx[tInt]*tn;
				val+=(3.*((*xP)[tInt+1]-(*xP)[tInt]) - 2.*dx[tInt] - dx[tInt+1])*pow(tn,2);
				val+=(2.*((*xP)[tInt]-(*xP)[tInt+1]) + dx[tInt] + dx[tInt+1])*pow(tn,3);
			}else if(coo==1){
				val=(*yP)[tInt] + dy[tInt]*tn;
				val+=(3.*((*yP)[tInt+1]-(*yP)[tInt]) - 2.*dy[tInt] - dy[tInt+1])*pow(tn,2);
				val+=(2.*((*yP)[tInt]-(*yP)[tInt+1]) + dy[tInt] + dy[tInt+1])*pow(tn,3);
			}else{
				val=std::numeric_limits<double>::quiet_NaN();
			}
			return val;
		}

		double tangent(const double t, int coo){
			double val=0.;
			return val;
		}
private:
	void FillMatrix(double *A,int n){
			for(unsigned int i=0;i<n*n;i++)A[i]=0.;
			// t equally spaced
			A[nIndex(0,0,n)]=2.; A[nIndex(1,0,n)]=1.;
			for(unsigned int i=1;i<n-1;i++){
				A[nIndex(i-1,i,n)]=1.;
				A[nIndex(i,i,n)]=4.;
				A[nIndex(i+1,i,n)]=1.;
			}
			A[nIndex(n-1,n-1,n)]=2.; A[nIndex(n-2,n-1,n)]=1.;
	}

	void FillRHS(double *b,const std::vector<double> &X,int n){
			b[0]=3.*(X[1]-X[0]);
			for(unsigned int i=1;i<n-1;i++){
				b[i]=3.*(X[i+1]-X[i-1]);
			}
			b[n-1]=3.*(X[n-1]-X[n-2]);
	}

private:
	bool initialized;
	const std::vector<double> *xP;
	const std::vector<double> *yP;
	int N;
	double *A;
  double *dx;
	double *dy;
};


class CubicSpline{
public:
    CubicSpline(const std::vector<double> &X, const std::vector<double> &Y): x(X), y(Y), N(X.size()){
			A=new double[(N/*-2*/)*(N/*-2*/)];
			z=new double [N/*-2*/];

			FillMatrix(A,N/*-2*/);
			FillRHS(z,y,N/*-2*/);
 			nNumerics::linsolve(A,z,N/*-2*/);

			std::ofstream file("z.txt");
			for(unsigned int i=0;i<N/*-2*/;i++){
				file<<x[i]<<" "<<z[i]<<" "<<(Y[i+1]-Y[i/*-1*/])/(x[i+1]-x[i/*-1*/])<<"\n";
			}
		};

    ~CubicSpline(){
			delete []z;
			delete []A;
		};
		
		double operator()(const double t){
			double val=0.;
			int tInt=0;

			std::vector<double>::const_iterator iter=x.begin();
			while(iter!=x.end()){
				//std::cerr<<*(iter-1)<<" "<<t<<" "<<*iter<<"\n";
				if(/*(*(iter-1))<=t&&*/t<x[tInt]){//(*iter)){
					std::cerr<<"break "<<x[tInt]<<" "<<*(iter-1)<<" "<<t<<" "<<*iter<<"\n";break;
				}
				++tInt;
				++iter;
			}
			// iter==x.end() still needs to be treated properly
			if(iter==x.end()){
				std::cerr<<"this is the end "<<t<<" "<<*(x.end()-1)<<"\n";
				tInt=N-2;
				//return -1000;
			}
			
			--tInt;

			const double hi=(x[tInt+1]-x[tInt]);

			//if(tInt<N-1){
				val+=z[tInt+1]*pow(t-x[tInt],3)/(6.*hi);
				val-=hi*z[tInt+1]*(t-x[tInt])/6.;
			//}
			//if(tInt>0){
				val+=z[tInt]*pow(x[tInt+1]-t,3)/(6.*hi);
				val-=hi*z[tInt]*(x[tInt+1]-t)/6.;
			//}

			val+=y[tInt+1]*(t-x[tInt])/hi;
			val+=y[tInt]*(x[tInt+1]-t)/hi;

			return val;
		}

		double tangent(const double t, int coordinate){
			double val=0.;
			return val;
		}
private:
	void FillMatrix(double *A,int n){
			for(unsigned int i=0;i<n*n;i++)A[i]=0.;
			// non-equally spaced

			A[nIndex(0,0,n)]=1.;
//			A[nIndex(0,0,n)]=2.*(x[2]-x[0]); A[nIndex(1,0,n)]=x[2]-x[1];
			for(unsigned int i=1;i<n-1;i++){
/*				A[nIndex(i-1,i,n)]=x[i+1]-x[i];
				A[nIndex(i,i,n)]=2.*(x[i+2]-x[i]);
				A[nIndex(i+1,i,n)]=x[i+2]-x[i+1];*/

				A[nIndex(i-1,i,n)]=x[i]-x[i-1];
				A[nIndex(i,i,n)]=2.*(x[i+1]-x[i-1]);
				A[nIndex(i+1,i,n)]=x[i+1]-x[i];

			}
			//A[nIndex(n-1,n-1,n)]=2.*(x[n+1]-x[n-1]); A[nIndex(n-2,n-1,n)]=x[n]-x[n-1];
			A[nIndex(n-1,n-1,n)]=1.;
	}

	void FillRHS(double *b,const std::vector<double> &Y,int n){
			b[0]=0.;
			for(unsigned int i=1;i<n-1;i++){
				//b[i]=6.*((Y[i+2]-Y[i+1])/(x[i+2]-x[i+1])-(Y[i+1]-Y[i])/(x[i+1]-x[i]));
				b[i]=6.*((Y[i+1]-Y[i])/(x[i+1]-x[i])-(Y[i]-Y[i-1])/(x[i]-x[i-1]));
			}
			b[n-1]=0.;
	}

private:
	const std::vector<double> &x;
	const std::vector<double> &y;
	const int N;
	double *A;
	double *z;
};


}

#endif
