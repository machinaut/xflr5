/****************************************************************************

    CSurface Class
    Copyright (C) 2005 Andr� Deperrois XFLR5@yahoo.com

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
// Surface.cpp: implementation of the CSurface class
// This class is associated to a surface used in VLM calculations
// The surface is always associated to a Wing "Panel" 
// and each of the wing's panels (left and right) is associated to 1 VLM surface
// The Surface's geometry is defined by
//    - its Leading edge  : m_LA, m_LB
//    - its Trailing edge : m_TA, m_TB
//    - its left and right twist
//    - its left and right foils
/
// The corner points are always defined from left tip to right tip,
// i.e. from y<0 to y>0
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "WPolar.h"
#include <math.h>
#include ".\surface.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector CSurface::LA;//save time by preventing allocation & release of memory
CVector CSurface::LB;
CVector CSurface::TA;
CVector CSurface::TB;
CVector CSurface::VTemp;

CSurface::CSurface()
{
	pi = 3.141592654;
	m_bTEFlap = false;

//	m_Color  = RGB(150,20,20);
	m_Dihedral = 0.0;
	m_Length   = 0.0;
	m_TwistA   = 0.0;
	m_TwistB   = 0.0;
	m_posATE   = 1.0;
	m_posBTE   = 1.0;
	m_NXPanels  = 1;
	m_NYPanels  = 2;
	m_NX1       = 1;
	m_NX2       = 0;
	m_XDistType = 0;
	m_YDistType = 3;
	m_iPos      = 0;
	m_pFoilA   = NULL;
	m_pFoilB   = NULL;


/*  3        equal         |   |   |   |   |   |   |   |   |

    2        sine          || |  |   |    |    |     |     |

    1        cosine        ||  |    |      |      |    |  ||

    0        equal         |   |   |   |   |   |   |   |   |

   -1        cosine        ||  |    |      |      |    |  ||

   -2       -sine          |     |     |    |    |   |  | ||

   -3        equal         |   |   |   |   |   |   |   |   |*/
}

CSurface::~CSurface()
{

}


void CSurface::GetLeadingPt(CVector &C, int k)
{
	GetPanel(k,m_NXPanels-1,VTemp , 0);

	C.x    = (LA.x+LB.x)/2.0;
	C.y    = (LA.y+LB.y)/2.0;
	C.z    = (LA.z+LB.z)/2.0;
}

void CSurface::GetTrailingPt(CVector &C, int k)
{
	GetPanel(k,0,VTemp,0);

	C.x    = (TA.x+TB.x)/2.0;
	C.y    = (TA.y+TB.y)/2.0;
	C.z    = (TA.z+TB.z)/2.0;
}

void CSurface::GetC4(int k, CVector &Pt)
{
	GetPanel(k,m_NXPanels-1,VTemp,0);
	double xl = (LA.x+LB.x)/2.0;
	double yl = (LA.y+LB.y)/2.0;
	double zl = (LA.z+LB.z)/2.0;
	GetPanel(k,0,VTemp,0);
	double xt = (TA.x+TB.x)/2.0;
	double yt = (TA.y+TB.y)/2.0;
	double zt = (TA.z+TB.z)/2.0;
	Pt.x = xl*.75 + xt*.25;
	Pt.y = yl*.75 + yt*.25;
	Pt.z = zl*.75 + zt*.25;
}

double CSurface::GetStripArea(int k)
{
	return Getdl(k) * GetChord(k);
}


int CSurface::Getk(double y)
{
	int k;
	double y1, y2;
	for(k=0; k<m_NYPanels; k++){
		GetyDist(k,y1,y2);
		y1 = m_LA.y + (m_LB.y-m_LA.y) * y1;
		y2 = m_LA.y + (m_LB.y-m_LA.y) * y2;
		if(y1<=y && y<=y2) return k;
	}
	return -1;
}

void CSurface::Getyz(int k, double &y, double &z)
{
	double y1, y2;
	GetyDist(k,y1,y2);
	GetPoint(0.0, 0.0, y1, LA);
	GetPoint(0.0, 0.0, y2, LB);

	y = (LA.y+LB.y)/2.0;
	z = (LA.z+LB.z)/2.0;
}


double CSurface::Getdl(int k)
{
	CVector L;
	double y1, y2;
	GetyDist(k,y1,y2);
	GetPoint(0.0, 0.0, y1, LA);
	GetPoint(0.0, 0.0, y2, LB);
	L.x = 0.0;
	L.y = LB.y - LA.y;
	L.z = LB.z - LA.z;
	return L.VAbs();
}

double CSurface::GetTwist(int k)
{
	double y,z;
	Getyz(k,y,z);
	return  m_TwistA + (m_TwistB-m_TwistA) *(y-m_LA.y)/(m_LB.y-m_LA.y);
}

void CSurface::SetTwist()
{
	double xc4,zc4;
	//"A" section first
	xc4 = m_LA.x + (m_TA.x-m_LA.x)/4.0;
	zc4 = m_LA.z + (m_TA.z-m_LA.z)/4.0;
	m_LA.x = xc4 + (m_LA.x-xc4) * cos(m_TwistA *pi/180.0);
	m_LA.z = zc4 - (m_LA.x-xc4) * sin(m_TwistA *pi/180.0);
	m_TA.x = xc4 + (m_TA.x-xc4) * cos(m_TwistA *pi/180.0);
	m_TA.z = zc4 - (m_TA.x-xc4) * sin(m_TwistA *pi/180.0);
	//"B" Section next
	xc4 = m_LB.x + (m_TB.x-m_LB.x)/4.0;
	zc4 = m_LB.z + (m_TB.z-m_LB.z)/4.0;
	m_LB.x = xc4 + (m_LB.x-xc4) * cos(m_TwistB *pi/180.0);
	m_LB.z = zc4 - (m_LB.x-xc4) * sin(m_TwistB *pi/180.0);
	m_TB.x = xc4 + (m_TB.x-xc4) * cos(m_TwistB *pi/180.0);
	m_TB.z = zc4 - (m_TB.x-xc4) * sin(m_TwistB *pi/180.0);
}


double CSurface::GetChord(int k)
{
	double tau;
	CVector V1, V2;
	V1.x = 0.0;
	V1.y = LA.y-m_LA.y;
	V1.z = LA.z-m_LA.z;
	V2.x = 0.0;
	V2.y = LB.y-m_LA.y;
	V2.z = LB.z-m_LA.z;
	tau = (V1.VAbs()+V2.VAbs())/2.0/m_Length;
	return GetChord(tau);
}

double CSurface::GetChord(double tau)
{
	//assumes LA-TB have already been loaded
	CVector V1, V2;
	V1.x = m_TA.x-m_LA.x;
	V1.y = m_TA.y-m_LA.y;
	V1.z = m_TA.z-m_LA.z;
	V2.x = m_TB.x-m_LB.x;
	V2.y = m_TB.y-m_LB.y;
	V2.z = m_TB.z-m_LB.z;

	double ChordA = V1.VAbs();
	double ChordB = V2.VAbs();
	
	return ChordA + (ChordB-ChordA) * fabs(tau);
}

double CSurface::GetOffset(double tau)
{
	//chord spacing
	return m_LA.x + (m_LB.x-m_LA.x) * fabs(tau);
}
void CSurface::GetNormal(CVector &N)
{
	CVector LATB, TALB;

	LATB.x = m_TB.x - m_LA.x;
	LATB.y = m_TB.y - m_LA.y;
	LATB.z = m_TB.z - m_LA.z;
	TALB.x = m_LB.x - m_TA.x;
	TALB.y = m_LB.y - m_TA.y;
	TALB.z = m_LB.z - m_TA.z;
	CrossProduct(LATB, TALB, N);
	N.Normalize();
}


int CSurface::GetNPanels()
{
	return m_NXPanels * m_NYPanels;
}

void CSurface::GetPanel(int k, int l, CVector &dF, int pos)
{
	// pos =  0 : mid camber line
	// pos =  1 : top
	// pos = -1 : bottom
	// loads the corner points of the panel k,l in TA, TB, LA, LB

	double y1, y2, xA1, xA2, xB1, xB2;
	double chordA, chordB, zA, zB;
	CVector V1, V2;
	CVector DL(m_LB.x-m_LA.x,m_LB.y-m_LA.y,m_LB.z-m_LA.z);
	CVector DC(m_TA.x-m_LA.x,m_TA.y-m_LA.y,m_TA.z-m_LA.z);
	double Length = DL.VAbs();
	double Chord  = DC.VAbs();
	CVector u(DC.x/Chord,  DC.y/Chord,  DC.z/Chord);
	CVector v(DL.x/Length, DL.y/Length, DL.z/Length);

	GetxDist(l, xA1, xA2, xB1, xB2);
	GetyDist(k,y1,y2);

	chordA  = GetChord(0.0);
	chordB  = GetChord(1.0);
	GetPoint(xA1, xB1, y1, LA); 
	GetPoint(xA1, xB1, y2, LB);
	GetPoint(xA2, xB2, y1, TA);
	GetPoint(xA2, xB2, y2, TB);

	V1.x = TB.x - LA.x;
	V1.y = TB.y - LA.y;
	V1.z = TB.z - LA.z;
	V2.x = LB.x - TA.x;
	V2.y = LB.y - TA.y;
	V2.z = LB.z - TA.z;
	CrossProduct(V1, V2, dF);
	dF.Normalize();

	if (m_pFoilA && m_pFoilB){
		if(pos==1){
			zA = m_pFoilA->GetUpperY(xA1)*chordA;
			zB = m_pFoilB->GetUpperY(xB1)*chordB;
		}
		else if(pos==-1){
			zA = m_pFoilA->GetLowerY(xA1)*chordA;
			zB = m_pFoilB->GetLowerY(xB1)*chordB;
		}
		else{
			zA = m_pFoilA->GetMidY(xA1)*chordA;
			zB = m_pFoilB->GetMidY(xB1)*chordB;
		}
		LA.x += (zA+ (zB-zA)*y1) *dF.x;
		LA.y += (zA+ (zB-zA)*y1) *dF.y;
		LA.z += (zA+ (zB-zA)*y1) *dF.z;
		LB.x += (zA+ (zB-zA)*y2) *dF.x;
		LB.y += (zA+ (zB-zA)*y2) *dF.y;
		LB.z += (zA+ (zB-zA)*y2) *dF.z;

		if(pos==1){
			zA = m_pFoilA->GetUpperY(xA2)*chordA;
			zB = m_pFoilB->GetUpperY(xB2)*chordB;
		}
		else if(pos==-1){
			zA = m_pFoilA->GetLowerY(xA2)*chordA;
			zB = m_pFoilB->GetLowerY(xB2)*chordB;
		}
		else{
			zA = m_pFoilA->GetMidY(xA2)*chordA;
			zB = m_pFoilB->GetMidY(xB2)*chordB;
		}
		TA.x += (zA+ (zB-zA)*y1) *dF.x;
		TA.y += (zA+ (zB-zA)*y1) *dF.y;
		TA.z += (zA+ (zB-zA)*y1) *dF.z;
		TB.x += (zA+ (zB-zA)*y2) *dF.x;
		TB.y += (zA+ (zB-zA)*y2) *dF.y;
		TB.z += (zA+ (zB-zA)*y2) *dF.z;
	}
	V1.x = TB.x - LA.x;
	V1.y = TB.y - LA.y;
	V1.z = TB.z - LA.z;
	V2.x = LB.x - TA.x;
	V2.y = LB.y - TA.y;
	V2.z = LB.z - TA.z;

	if(pos==1 || pos==0) CrossProduct(V1, V2, dF);
	else                 CrossProduct(V2, V1, dF);

	dF.Normalize();
}

void CSurface::GetPoint(double xArel, double xBrel, double yrel, CVector &Point, int pos)
{
	//xrel, yrel are the planform positions, i.e. with the surface in the x-y plane
	CVector APt, BPt;
	CVector N;
	GetNormal(N);

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;

	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB){
		double TopA = m_pFoilA->GetUpperY(xArel)*GetChord(0.0);
		double TopB = m_pFoilB->GetUpperY(xBrel)*GetChord(1.0);
		APt.x += TopA * N.x;
		APt.y += TopA * N.y;
		APt.z += TopA * N.z;

		BPt.x += TopB * N.x;
		BPt.y += TopB * N.y;
		BPt.z += TopB * N.z;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB){
		double BotA = m_pFoilA->GetLowerY(xArel)*GetChord(0.0);
		double BotB = m_pFoilB->GetLowerY(xBrel)*GetChord(1.0);
		APt.x += BotA * N.x;
		APt.y += BotA * N.y;
		APt.z += BotA * N.z;

		BPt.x += BotB * N.x;
		BPt.y += BotB * N.y;
		BPt.z += BotB * N.z;
	}
	Point.x = (1.0-yrel)*APt.x + yrel * BPt.x;
	Point.y = (1.0-yrel)*APt.y + yrel * BPt.y;
	Point.z = (1.0-yrel)*APt.z + yrel * BPt.z;
}



void CSurface::Copy(CSurface &Surface)
{
	m_LA.Copy(Surface.m_LA);
	m_LB.Copy(Surface.m_LB);
	m_TA.Copy(Surface.m_TA);
	m_TB.Copy(Surface.m_TB);
	m_Dihedral  = Surface.m_Dihedral;
	m_XDistType = Surface.m_XDistType;
	m_YDistType = Surface.m_YDistType;
	m_iPos      = Surface.m_iPos;
	m_Length    = Surface.m_Length;
	m_NXPanels  = Surface.m_NXPanels;
	m_NYPanels  = Surface.m_NYPanels;
	m_pFoilA    = Surface.m_pFoilA;
	m_pFoilB    = Surface.m_pFoilB;
	m_TwistA    = Surface.m_TwistA;
	m_TwistB    = Surface.m_TwistB;

}

void CSurface::Translate(CVector T)
{
	m_LA.Translate(T);
	m_LB.Translate(T);
	m_TA.Translate(T);
	m_TB.Translate(T);
}

void CSurface::RotateX(CVector O, double XTilt)
{
	m_LA.RotateX(O, XTilt);
	m_LB.RotateX(O, XTilt);
	m_TA.RotateX(O, XTilt);
	m_TB.RotateX(O, XTilt);
}

void CSurface::RotateY(CVector O, double YTilt)
{
	m_LA.RotateY(O, YTilt);
	m_LB.RotateY(O, YTilt);
	m_TA.RotateY(O, YTilt);
	m_TB.RotateY(O, YTilt);
}


void CSurface::RotateZ(CVector O, double ZTilt)
{
	m_LA.RotateZ(O, ZTilt);
	m_LB.RotateZ(O, ZTilt);
	m_TA.RotateZ(O, ZTilt);
	m_TB.RotateZ(O, ZTilt);
}

void CSurface::GetyDist(int k, double &y1, double &y2)
{
	if(m_YDistType==1){
		//cosine case
		y1  = 1.0/2.0*(1.0-cos( k*pi   /m_NYPanels));
		y2  = 1.0/2.0*(1.0-cos((k+1)*pi/m_NYPanels));
	}
	else if(m_YDistType==-2){
		//sine case
		y1  = 1.0*(sin( k*pi   /2.0/m_NYPanels));
		y2  = 1.0*(sin((k+1)*pi/2.0/m_NYPanels));
	}
	else if(m_YDistType==2){
		//-sine case
		y1  = 1.0*(1.-cos( k*pi   /2.0/m_NYPanels));
		y2  = 1.0*(1.-cos((k+1)*pi/2.0/m_NYPanels));
	}
	else{
		//equally spaced case
		y1 =  (double)k     /(double) m_NYPanels;
		y2 = ((double)k+1.0)/(double) m_NYPanels;
	}
}

double CSurface::Getyrel(double y)
{
//	double absy = fabs(y);
//	if(y>m_LB.y)       return 1.0;      //(error)
//	else if(y<m_LA.y)  return 0.0; //(error)

	return (y-m_LA.y)/(m_LB.y-m_LA.y);

}

void CSurface::SetFlap()
{
	if(m_pFoilA->m_bTEFlap) {
		m_posATE = m_pFoilA->m_TEXHinge/100.0;
		if(m_posATE>1.0) m_posATE = 1.0; else if(m_posATE<0.0) m_posATE = 0.0;
	}
	else m_posATE = 1.0;

	if(m_pFoilB->m_bTEFlap){
		m_posBTE = m_pFoilB->m_TEXHinge/100.0;
		if(m_posBTE>1.0) m_posBTE = 1.0; else if(m_posBTE<0.0) m_posBTE = 0.0;
	}
	else m_posBTE = 1.0;

	if(m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap){
		m_NX1 = (int)((m_posATE+m_posBTE)/2.0 * m_NXPanels);	//number of panels before the flap break
		m_NX2 = m_NXPanels-m_NX1;								//number of panels in the flap
		m_bTEFlap = true;
	}

	else {
		m_NX1 = m_NXPanels;
		m_NX2 = 0;
		m_bTEFlap = false;
	}
}


void CSurface::GetxDist(int l, double &xA1, double &xA2, double &xB1, double &xB2)
{
	int ll;

	if(m_bTEFlap){
		if (l<m_NX2){
			if(m_XDistType==1){
				//cosine case
				xA1 = m_posATE + (1.0-m_posATE)/2.0 *(1.0+cos(((double)l+1.0)*pi /(double)m_NX2));
				xA2 = m_posATE + (1.0-m_posATE)/2.0 *(1.0+cos( (double)l     *pi /(double)m_NX2));
			}
			else{
				//equally spaced case
				xA1 = m_posATE + (1.0-m_posATE)*( ((double)l+1.0)/(double)m_NX2);
				xA2 = m_posATE + (1.0-m_posATE)*(  (double)l     /(double)m_NX2);
			}
		}
		else{
			ll = l-m_NX2;
			if(m_XDistType==1){
				//cosine case
				xA1 = m_posATE/2.0 *(1.0+cos(((double)ll+1.0)*pi /(double)m_NX1));
				xA2 = m_posATE/2.0 *(1.0+cos( (double)ll     *pi /(double)m_NX1));
			}
			else{
				//equally spaced case
				xA1 = m_posATE*(1.0- ((double)ll+1.0)/(double)m_NX1);
				xA2 = m_posATE*(1.0-  (double)ll     /(double)m_NX1);
			}
		}
	}
	else{
		if(m_XDistType==1){
			//cosine case
			xA1 = 1.0/2.0 *(1.0+cos(((double)l+1.0)*pi /(double)m_NXPanels));
			xA2 = 1.0/2.0 *(1.0+cos( (double)l     *pi /(double)m_NXPanels));
		}
		else{
			//equally spaced case
			xA1 = 1.0- ((double)l+1.0)/(double)m_NXPanels;
			xA2 = 1.0-  (double)l     /(double)m_NXPanels;
		}
	}
	if(m_bTEFlap){
		if (l<m_NX2){
			if(m_XDistType==1){
				//cosine case
				xB1 = m_posBTE + (1.0-m_posBTE)/2.0 *(1.0 + cos(((double)l+1.0)*pi /(double)m_NX2));
				xB2 = m_posBTE + (1.0-m_posBTE)/2.0 *(1.0 + cos( (double)l     *pi /(double)m_NX2));
			}
			else{
				//equally spaced case
				xB1 = m_posBTE + (1.0-m_posBTE) *( ((double)l+1.0)/(double)m_NX2);
				xB2 = m_posBTE + (1.0-m_posBTE) *(  (double)l     /(double)m_NX2);
			}
		}
		else{
			ll = l-m_NX2;
			if(m_XDistType==1){
				//cosine case
				xB1 = m_posBTE/2.0 *(1.0 + cos(((double)ll+1.0)*pi /(double)m_NX1));
				xB2 = m_posBTE/2.0 *(1.0 + cos( (double)ll     *pi /(double)m_NX1));
			}
			else{
				//equally spaced case
				xB1 = m_posBTE*(1.0- ((double)ll+1.0)/(double)m_NX1);
				xB2 = m_posBTE*(1.0-  (double)ll     /(double)m_NX1);
			}
		}
	}
	else{
		if(m_XDistType==1){
			//cosine case
			xB1 = 1.0/2.0 *(1.0+cos(((double)l+1)*pi /(double)m_NXPanels));
			xB2 = 1.0/2.0 *(1.0+cos( (double)l   *pi /(double)m_NXPanels));
		}
		else{
			//equally spaced case
			xB1 = 1.0- ((double)l+1.0)/(double)m_NXPanels;
			xB2 = 1.0-  (double)l     /(double)m_NXPanels;
		}
	}
}

