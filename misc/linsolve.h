/***************************************************************************
                          linsolve.h  -  description
                             -------------------
    begin                : Wed Jun 27 2001
    copyright            : (C) 2001 by Andres Chavarria
    email                : Andres.Chavarria@iwr.uni-heidelberg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LINSOLVE_H
#define LINSOLVE_H

#include <iostream>
#include <math.h>

// begin namespace nNumerics
namespace nNumerics{


inline int nIndex(int j,int i,int Nx)
{
	return i*Nx+j;
}

// sum the c* line j to the line i
inline void SumLinesMat(double *A, int i,int j, double c, int Nx)
{
	for(int J=0;J<Nx;J++){
    A[nIndex(J,i,Nx)]+=c*A[nIndex(J,j,Nx)];
  }
}


// sum the c*element  j to the element i
inline void SumLinesVec(double *b, int i,int j, double c)
{
	b[i]+=c*b[j];
}


// Vertausche line i and line j
inline void VertauscheMat(double *A,int i,int j, int Nx)
{
	double MomVal;
	
	for(int J=0;J<Nx;J++){
		MomVal=A[nIndex(J,i,Nx)];
		A[nIndex(J,i,Nx)]=A[nIndex(J,j,Nx)];
		A[nIndex(J,j,Nx)]=MomVal;
	}
}

// Vertausche line i and line j
inline void VertauscheVec(double *b,int i,int j)
{
	double MomVal;
	
	MomVal=b[i];
	b[i]=b[j];
	b[j]=MomVal;
}



// solver for quadratic matrix system Ax=b
inline int linsolve(double *A, double *b,int Nx)
{
	// momentary val for a Pivotelement
	double Pivotelement;
	double c;
	int Index,i,j;
	
	// Gauss-Jordan for the calculation of the inverse
  for(j=0;j<Nx;j++){
		
// 		for(int k=0;k<Nx;k++){
// 			for(int l=0;l<Nx;l++){
// 				std::cout<<A[k*Nx+l]<<" ";
// 			}
// 			std::cout<<"|"<<b[k]<<"\n";
// 		}
// 		std::cout<<"\n";


		Pivotelement=A[nIndex(j,j,Nx)];
		// pivot ist null, also vertausche bis keine Null mehr in Diagonale ist
		// here begins the elimination
		if(Pivotelement==0.){	
			Index=0;
			while(j+Index<Nx&&A[nIndex(j,j+Index,Nx)]==0.){Index++;}
			if(Index+j==Nx){return 1;}						// Rank of Matrix isn't n
			
			VertauscheMat(A,j,j+Index,Nx);
			VertauscheVec(b,j,j+Index);
			Pivotelement=A[nIndex(j,j,Nx)];
		}
		
		for(i=j+1;i<Nx;i++){
			// if element is cero then continue. This fastens when applying linsolve
			// on a band-matrix
			if(A[nIndex(j,i,Nx)]==0.)continue;
			c=-A[nIndex(j,i,Nx)]/Pivotelement;
			SumLinesMat(A,i,j,c,Nx);
			SumLinesVec(b,i,j,c);
			A[nIndex(j,i,Nx)]=0.;
		}
	}

	// ok now we have Ax=b with a new b and a upper triangular A
	// solve by successive backward solving
	// x_n = b_n/a_nn  and   x_j = 1/a_jj (b_j - \sum_{i=j+1}^n a_ji x_i)

	b[Nx-1]/=A[nIndex(Nx-1,Nx-1,Nx)];
	
	for(j=Nx-2;j>=0;j--){
		for(i=j+1;i<=Nx;i++)b[j]-=A[nIndex(i,j,Nx)]*b[i];
		b[j]/=A[nIndex(j,j,Nx)];
	}

	// b has now the solution
	// A was changed
  return 0;
}

// end namespace nNumerics
};
#endif



