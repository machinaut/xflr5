/****************************************************************************

    Vector Class
    Copyright (C) 2005 Andr�Deperrois XFLR5@yahoo.com

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

//////////////////////////////////////////////////////////////////////
//
// Vector.cpp: implementation of the CVector class.
// Simple class implementing the usual properties and methods of a 3D Vector
//
//////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector::CVector()
{
	x  = 0.0;
	y  = 0.0;
	z  = 0.0;
}
CVector::CVector(double xi, double yi, double zi)
{
	x  = xi;
	y  = yi;
	z  = zi;
}

CVector::~CVector()
{

}

void CVector::Normalize()
{
	double abs = VAbs();
	x/=abs;
	y/=abs;
	z/=abs;
}


double CVector::VAbs()
{
	return sqrt(x*x+y*y+z*z);
}

void CVector::Copy(CVector V)
{	
	x = V.x;
	y = V.y;
	z = V.z;
}
void CVector::Set(double x0, double y0, double z0)
{	
	x = x0;
	y = y0;
	z = z0;
}

double CVector::dot(CVector V)
{	
	return x*V.x + y*V.y + z*V.z;
}

bool CVector::IsSame(CVector V)
{
	double d2 = (V.x-x)*(V.x-x) + (V.y-y)*(V.y-y) + (V.z-z)*(V.z-z) ;
	double L = VAbs();
	double L2 = L*L;
	if(d2==0.0) return true;
	if(d2/L2 < 1.0/1000000.0) 
		return true;
	else 
		return false;

}

void CVector::Translate(CVector T)
{
	x += T.x;
	y += T.y;
	z += T.z;
}

void CVector::operator +=(CVector T)
{
	x += T.x;
	y += T.y;
	z += T.z;
}

void CVector::operator =(CVector T)
{
	x = T.x;
	y = T.y;
	z = T.z;
}
void CVector::operator -=(CVector T)
{
	x -= T.x;
	y -= T.y;
	z -= T.z;
}
void CVector::operator *=(double d)
{
	x *= d;
	y *= d;
	z *= d;
}

CVector CVector::operator *(double d)
{
	CVector T(x*d, y*d, z*d);
	return T;
}

CVector CVector::operator *(CVector T)
{
	CVector C;
	C.x =  y*T.z - z*T.y;
	C.y = -x*T.z + z*T.x;
	C.z =  x*T.y - y*T.x;
	return C;
}
CVector CVector::operator /(double d)
{
	CVector T(x/d, y/d, z/d);
	return T;
}

CVector CVector::operator +(CVector V)
{
	CVector T(x+V.x, y+V.y, z+V.z);
	return T;
}

CVector CVector::operator -(CVector V)
{
	CVector T(x-V.x, y-V.y, z-V.z);
	return T;
}

void  CVector::RotateX(CVector O, double XTilt)
{
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
		
	XTilt *=M_PI/180.0;
	y = O.y + OP.y * cos(XTilt) - OP.z * sin(XTilt);
	z = O.z + OP.y * sin(XTilt) + OP.z * cos(XTilt);
}

void  CVector::RotateY(CVector O, double YTilt)
{
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
		
	YTilt *=M_PI/180.0;

	x = O.x + OP.x * cos(YTilt) + OP.z * sin(YTilt);
	z = O.z - OP.x * sin(YTilt) + OP.z * cos(YTilt);
}


void  CVector::RotateZ(CVector O, double ZTilt)
{
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
	
	ZTilt *=M_PI/180.0;

	x = O.x + OP.x * cos(ZTilt) + OP.y * sin(ZTilt);
	y = O.y - OP.x * sin(ZTilt) + OP.y * cos(ZTilt);
}

void  CVector::RotateY(double YTilt)
{		
	YTilt *=M_PI/180.0;

	double xo = x;
	double zo = z;
	x =  xo * cos(YTilt) + zo * sin(YTilt);
	z = -xo * sin(YTilt) + zo * cos(YTilt);
}


