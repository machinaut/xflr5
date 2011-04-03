/****************************************************************************

    CSurface Class
	Copyright (C) 2005-2009 Andre Deperrois XFLR5@yahoo.com

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
//    - its Normal vector
//    - its left and right normal vectors NormalA and NormalB are 
//      the average of the normals of two continuous surfaces ; used to
//	define junction between panels
//
// A suffixes refer to the left side, B refers to the right side
// L refers to the leading side refers to the right side
// The corner points are always defined from left tip to right tip,
// i.e. from y<0 to y>0
//
//////////////////////////////////////////////////////////////////////

#include <QMessageBox>
#include <math.h>
#include "Surface.h"
#include "../Objects/Quaternion.h"
#include <QtDebug>


CVector CSurface::LA;//save time by preventing allocation & release of memory
CVector CSurface::LB;
CVector CSurface::TA;
CVector CSurface::TB;
CVector CSurface::VTemp;
CVector *CSurface::s_pNode;
CPanel *CSurface::s_pPanel;

CSurface::CSurface()
{
	m_bTEFlap = false;

	m_Length   = 0.0;
	m_TwistA   = 0.0;
	m_TwistB   = 0.0;
	m_posATE   = 1.0;
	m_posBTE   = 1.0;
	m_NXPanels  = 1;
	m_NYPanels  = 2;
	m_NXLead    = 1;
	m_NXFlap    = 0;
	m_XDistType = 0;
	m_YDistType = 3;

	m_pLeftSurface = m_pRightSurface = NULL;
	m_pFoilA   = NULL;
	m_pFoilB   = NULL;
	m_NElements = 0;

	m_bIsInSymPlane = false;
	m_bIsTipLeft    = false;
	m_bIsTipRight   = false;

	m_bIsLeftSurf   = false;
	m_bIsRightSurf  = false;

	m_bIsCenterSurf = false;
	m_bJoinRight    = true;

	m_nFlapNodes  = 0;
	m_nFlapPanels = 0;

	memset(m_xPointA, 0, sizeof(m_xPointA));
	memset(m_xPointB, 0, sizeof(m_xPointB));
	memset(m_FlapNode, 0, sizeof(m_FlapNode));
	memset(m_FlapPanel, 0, sizeof(m_FlapPanel));
}



void CSurface::AddFlapPanel(CPanel *pPanel)
{
	bool bFound = false;
	int i;

	//Add Nodes

	for (i=0; i< m_nFlapNodes; i++)
	{
		bFound = bFound && pPanel->m_iLA==m_FlapNode[i];
		if(pPanel->m_iLA== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iLA;
		m_nFlapNodes++;
	}

	bFound = false;
	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iLB== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iLB;
		m_nFlapNodes++;
	}

	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iTA== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iTA;
		m_nFlapNodes++;
	}

	bFound = false;
	for (i=0; i< m_nFlapNodes; i++)
	{
		if(pPanel->m_iTB== m_FlapNode[i])
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapNode[m_nFlapNodes] = pPanel->m_iTB;
		m_nFlapNodes++;
	}

	//Add panel;
	bFound=false;
	for(i=0; i<m_nFlapPanels; i++)
	{
		if(pPanel->m_iElement==m_FlapPanel[i])
		{
			bFound =true;
			break;
		}
	}
	if(!bFound)
	{
		m_FlapPanel[m_nFlapPanels] = pPanel->m_iElement;
		m_nFlapPanels++;
	}
}


void CSurface::Copy(CSurface const &Surface)
{
	m_LA.Copy(Surface.m_LA);
	m_LB.Copy(Surface.m_LB);
	m_TA.Copy(Surface.m_TA);
	m_TB.Copy(Surface.m_TB);
	m_XDistType = Surface.m_XDistType;
	m_YDistType = Surface.m_YDistType;
	m_NElements = Surface.m_NElements;

	m_Length    = Surface.m_Length;
	m_NXPanels  = Surface.m_NXPanels;
	m_NYPanels  = Surface.m_NYPanels;
	m_pFoilA    = Surface.m_pFoilA;
	m_pFoilB    = Surface.m_pFoilB;
	m_TwistA    = Surface.m_TwistA;
	m_TwistB    = Surface.m_TwistB;

	Normal  = Surface.Normal;
	NormalA = Surface.NormalA;
	NormalB = Surface.NormalB;

	m_bIsTipLeft    = Surface.m_bIsTipLeft;
	m_bIsTipRight   = Surface.m_bIsTipRight;
	m_bIsLeftSurf   = Surface.m_bIsLeftSurf;
	m_bIsRightSurf  = Surface.m_bIsRightSurf;
	m_bIsCenterSurf = Surface.m_bIsCenterSurf;
	m_bJoinRight    = Surface.m_bJoinRight;
	m_bIsInSymPlane = Surface.m_bIsInSymPlane;

	m_bTEFlap       = Surface.m_bTEFlap;
	m_nFlapNodes  = Surface.m_nFlapNodes;
	m_nFlapPanels = Surface.m_nFlapPanels;
	m_HingePoint  = Surface.m_HingePoint;
	m_HingeVector = Surface.m_HingeVector;

	memcpy(m_FlapNode, Surface.m_FlapNode, sizeof(m_FlapNode));
	memcpy(m_FlapPanel, Surface.m_FlapPanel, sizeof(m_FlapPanel));
	memcpy(m_xPointA, Surface.m_xPointA, sizeof(m_xPointA));
	memcpy(m_xPointB, Surface.m_xPointB, sizeof(m_xPointB));
}


void CSurface::CreateXZSymetric(CSurface const &Surface)
{
	//reflects the surface about the XZ plane
	m_LA.x =  Surface.m_LB.x;
	m_LA.y = -Surface.m_LB.y;
	m_LA.z =  Surface.m_LB.z;
	m_LB.x =  Surface.m_LA.x;
	m_LB.y = -Surface.m_LA.y;
	m_LB.z =  Surface.m_LA.z;

	m_TA.x =  Surface.m_TB.x;
	m_TA.y = -Surface.m_TB.y;
	m_TA.z =  Surface.m_TB.z;
	m_TB.x =  Surface.m_TA.x;
	m_TB.y = -Surface.m_TA.y;
	m_TB.z =  Surface.m_TA.z;

	m_Length    = Surface.m_Length;

	m_XDistType = Surface.m_XDistType;
	m_YDistType = Surface.m_YDistType;
	m_NXPanels  = Surface.m_NXPanels;
	m_NYPanels  = Surface.m_NYPanels;
	m_NElements = Surface.m_NElements;

	Normal.x  =  Surface.Normal.x;
	Normal.y  = -Surface.Normal.y;
	Normal.z  =  Surface.Normal.z;

	NormalA.x  =  Surface.NormalB.x;
	NormalA.y  = -Surface.NormalB.y;
	NormalA.z  =  Surface.NormalB.z;

	NormalB.x  =  Surface.NormalA.x;
	NormalB.y  = -Surface.NormalA.y;
	NormalB.z  =  Surface.NormalA.z;

	m_pFoilA = Surface.m_pFoilB;
	m_pFoilB = Surface.m_pFoilA;

	m_TwistA = Surface.m_TwistB;
	m_TwistB = Surface.m_TwistA;

	m_bIsTipLeft  = Surface.m_bIsTipRight;
	m_bIsTipRight = Surface.m_bIsTipLeft;

	m_bIsLeftSurf  = Surface.m_bIsRightSurf;
	m_bIsRightSurf = Surface.m_bIsLeftSurf;

	m_bIsInSymPlane = Surface.m_bIsInSymPlane;

	m_NXFlap = Surface.m_NXFlap;
	m_NXLead = Surface.m_NXLead;
	memcpy(m_xPointA, Surface.m_xPointB, sizeof(m_xPointA));
	memcpy(m_xPointB, Surface.m_xPointA, sizeof(m_xPointB));
}



void CSurface::GetC4(int k, CVector &Pt, double &tau)
{
	GetPanel(k,m_NXPanels-1,0);
	double xl = (LA.x+LB.x)/2.0;
	double yl = (LA.y+LB.y)/2.0;
	double zl = (LA.z+LB.z)/2.0;
	GetPanel(k,0,0);
	double xt = (TA.x+TB.x)/2.0;
	double yt = (TA.y+TB.y)/2.0;
	double zt = (TA.z+TB.z)/2.0;
	Pt.x = xl*.75 + xt*.25;
	Pt.y = yl*.75 + yt*.25;
	Pt.z = zl*.75 + zt*.25;

//	return relative span position;

	tau = sqrt((Pt.y-m_LA.y)*(Pt.y-m_LA.y)+(Pt.z-m_LA.z)*(Pt.z-m_LA.z))/m_Length;
}


double CSurface::GetChord(int const &k)
{
	static double y1, y2;
	GetyDist(k, y1, y2);
	return GetChord((y1+y2)/2.0);
}


double CSurface::GetChord(double const &tau)
{
	//assumes LA-TB have already been loaded
	static CVector V1, V2;
	static double ChordA, ChordB;

	V1 = m_TA-m_LA;
	V2 = m_TB-m_LB;

	ChordA = V1.VAbs();
	ChordB = V2.VAbs();

	return ChordA + (ChordB-ChordA) * fabs(tau);
}



double CSurface::GetFoilArea(double const &tau)
{
	double area, chord;
	if(m_pFoilA && m_pFoilB)
	{
		chord = GetChord(tau);
		area = (m_pFoilA->GetArea() + m_pFoilB->GetArea())/2.0*chord*chord;//m2
		return area;
	}
	else
		return 0.0;
}


void CSurface::GetNormal(double yrel, CVector &N)
{
	N = NormalA * (1.0-yrel) + NormalB * yrel;
	N.Normalize();
}




void CSurface::GetLeadingPt(int k, CVector &C)
{
	GetPanel(k,m_NXPanels-1, 0);

	C.x    = (LA.x+LB.x)/2.0;
	C.y    = (LA.y+LB.y)/2.0;
	C.z    = (LA.z+LB.z)/2.0;
}


double CSurface::GetOffset(double const &tau)
{
	//chord spacing
	return m_LA.x + (m_LB.x-m_LA.x) * fabs(tau);
}


void CSurface::GetPanel(int const &k, int const &l, int const &pos)
{
	// Assumption : side points have been set for this surface
	// Loads the corner points of the panel k,l in PTA, PTB, PLA, PLB
	// Method used to generate the panels

	GetyDist(k,y1,y2);
	if(pos==0)
	{
		LA.x = SideA[l+1].x * (1.0-y1) + SideB[l+1].x* y1;
		LA.y = SideA[l+1].y * (1.0-y1) + SideB[l+1].y* y1;
		LA.z = SideA[l+1].z * (1.0-y1) + SideB[l+1].z* y1;
		TA.x = SideA[l].x   * (1.0-y1) + SideB[l].x  * y1;
		TA.y = SideA[l].y   * (1.0-y1) + SideB[l].y  * y1;
		TA.z = SideA[l].z   * (1.0-y1) + SideB[l].z  * y1;
		LB.x = SideA[l+1].x * (1.0-y2) + SideB[l+1].x* y2;
		LB.y = SideA[l+1].y * (1.0-y2) + SideB[l+1].y* y2;
		LB.z = SideA[l+1].z * (1.0-y2) + SideB[l+1].z* y2;
		TB.x = SideA[l].x   * (1.0-y2) + SideB[l].x  * y2;
		TB.y = SideA[l].y   * (1.0-y2) + SideB[l].y  * y2;
		TB.z = SideA[l].z   * (1.0-y2) + SideB[l].z  * y2;
	}
	else if (pos==-1)
	{
		LA = SideA_B[l+1] * (1.0-y1) + SideB_B[l+1]* y1;
		TA = SideA_B[l]   * (1.0-y1) + SideB_B[l]  * y1;
		LB = SideA_B[l+1] * (1.0-y2) + SideB_B[l+1]* y2;
		TB = SideA_B[l]   * (1.0-y2) + SideB_B[l]  * y2;

		LA.x = SideA_B[l+1].x * (1.0-y1) + SideB_B[l+1].x* y1;
		LA.y = SideA_B[l+1].y * (1.0-y1) + SideB_B[l+1].y* y1;
		LA.z = SideA_B[l+1].z * (1.0-y1) + SideB_B[l+1].z* y1;
		TA.x = SideA_B[l].x   * (1.0-y1) + SideB_B[l].x  * y1;
		TA.y = SideA_B[l].y   * (1.0-y1) + SideB_B[l].y  * y1;
		TA.z = SideA_B[l].z   * (1.0-y1) + SideB_B[l].z  * y1;
		LB.x = SideA_B[l+1].x * (1.0-y2) + SideB_B[l+1].x* y2;
		LB.y = SideA_B[l+1].y * (1.0-y2) + SideB_B[l+1].y* y2;
		LB.z = SideA_B[l+1].z * (1.0-y2) + SideB_B[l+1].z* y2;
		TB.x = SideA_B[l].x   * (1.0-y2) + SideB_B[l].x  * y2;
		TB.y = SideA_B[l].y   * (1.0-y2) + SideB_B[l].y  * y2;
		TB.z = SideA_B[l].z   * (1.0-y2) + SideB_B[l].z  * y2;
	}
	else if (pos==1)
	{
		LA.x = SideA_T[l+1].x * (1.0-y1) + SideB_T[l+1].x* y1;
		LA.y = SideA_T[l+1].y * (1.0-y1) + SideB_T[l+1].y* y1;
		LA.z = SideA_T[l+1].z * (1.0-y1) + SideB_T[l+1].z* y1;
		TA.x = SideA_T[l].x   * (1.0-y1) + SideB_T[l].x  * y1;
		TA.y = SideA_T[l].y   * (1.0-y1) + SideB_T[l].y  * y1;
		TA.z = SideA_T[l].z   * (1.0-y1) + SideB_T[l].z  * y1;
		LB.x = SideA_T[l+1].x * (1.0-y2) + SideB_T[l+1].x* y2;
		LB.y = SideA_T[l+1].y * (1.0-y2) + SideB_T[l+1].y* y2;
		LB.z = SideA_T[l+1].z * (1.0-y2) + SideB_T[l+1].z* y2;
		TB.x = SideA_T[l].x   * (1.0-y2) + SideB_T[l].x  * y2;
		TB.y = SideA_T[l].y   * (1.0-y2) + SideB_T[l].y  * y2;
		TB.z = SideA_T[l].z   * (1.0-y2) + SideB_T[l].z  * y2;
	}
}


double CSurface::GetPanelWidth(int const &k)
{
	GetPanel(k, 0, 0);
	return fabs(LA.y-LB.y);
}


void CSurface::GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos)
{
	static CVector APt, BPt;
	static double TopA, TopB, BotA, BotB;

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		TopA = m_pFoilA->GetUpperY(xArel)*GetChord(0.0);
		TopB = m_pFoilB->GetUpperY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * TopA;
		APt.y +=  Normal.y * TopA;
		APt.z +=  Normal.z * TopA;
		BPt.x +=  Normal.x * TopB;
		BPt.y +=  Normal.y * TopB;
		BPt.z +=  Normal.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		BotA = m_pFoilA->GetLowerY(xArel)*GetChord(0.0);
		BotB = m_pFoilB->GetLowerY(xBrel)*GetChord(1.0);
		APt.x +=  Normal.x * BotA;
		APt.y +=  Normal.y * BotA;
		APt.z +=  Normal.z * BotA;
		BPt.x +=  Normal.x * BotB;
		BPt.y +=  Normal.y * BotB;
		BPt.z +=  Normal.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}



void CSurface::GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, CVector &PtNormal, int const &pos)
{
	static CVector APt, BPt, Nc, u;
	static double TopA, TopB, BotA, BotB, nxA, nxB, nyA, nyB, theta;
	static Quaternion q;
	
	//define the strip's normal
	GetNormal(yrel, Nc);

	//define the quaternion to rotate the unit vector (0,0,1) to Nc
	//use the dot product to get the rotation angle, and the crossproduct to get the rotation vector
	theta = acos(Nc.z);
	u.x = -Nc.y;
	u.y =  Nc.x;
	u.z =  0.0;
	q.Set(theta*180.0/PI, u);

	APt.x = m_LA.x * (1.0-xArel) + m_TA.x * xArel;
	APt.y = m_LA.y * (1.0-xArel) + m_TA.y * xArel;
	APt.z = m_LA.z * (1.0-xArel) + m_TA.z * xArel;
	BPt.x = m_LB.x * (1.0-xBrel) + m_TB.x * xBrel;
	BPt.y = m_LB.y * (1.0-xBrel) + m_TB.y * xBrel;
	BPt.z = m_LB.z * (1.0-xBrel) + m_TB.z * xBrel;

	if(pos==1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetUpperY(xArel, TopA, nxA, nyA);
		m_pFoilB->GetUpperY(xBrel, TopB, nxB, nyB);
		TopA *= GetChord(0.0);
		TopB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * TopA;
		APt.y +=  NormalA.y * TopA;
		APt.z +=  NormalA.z * TopA;
		BPt.x +=  NormalB.x * TopB;
		BPt.y +=  NormalB.y * TopB;
		BPt.z +=  NormalB.z * TopB;
	}
	else if(pos==-1 && m_pFoilA && m_pFoilB)
	{
		m_pFoilA->GetLowerY(xArel, BotA, nxA, nyA);
		m_pFoilB->GetLowerY(xBrel, BotB, nxB, nyB);
		BotA *= GetChord(0.0);
		BotB *= GetChord(1.0);

		// rotate the point's normal vector i.a.w. dihedral and local washout
		PtNormal.x = nxA * (1.0-yrel) + nxB * yrel;
		PtNormal.y = 0.0;
		PtNormal.z = nyA * (1.0-yrel) + nyB * yrel;
		q.Conjugate(PtNormal.x, PtNormal.y, PtNormal.z);

		APt.x +=  NormalA.x * BotA;
		APt.y +=  NormalA.y * BotA;
		APt.z +=  NormalA.z * BotA;
		BPt.x +=  NormalB.x * BotB;
		BPt.y +=  NormalB.y * BotB;
		BPt.z +=  NormalB.z * BotB;
	}
	Point.x = APt.x * (1.0-yrel)+  BPt.x * yrel ;
	Point.y = APt.y * (1.0-yrel)+  BPt.y * yrel ;
	Point.z = APt.z * (1.0-yrel)+  BPt.z * yrel ;
}


void CSurface::GetSection(double const &tau, double &Chord, double &Area, CVector &PtC4)
{
	//explicit double calculations are much faster than vector algebra
	LA.x = m_LA.x * (1.0-tau) + m_LB.x * tau;
	LA.y = m_LA.y * (1.0-tau) + m_LB.y * tau;
	LA.z = m_LA.z * (1.0-tau) + m_LB.z * tau;
	TA.x = m_TA.x * (1.0-tau) + m_TB.x * tau;
	TA.y = m_TA.y * (1.0-tau) + m_TB.y * tau;
	TA.z = m_TA.z * (1.0-tau) + m_TB.z * tau;
	PtC4.x = .75 * LA.x + .25 * TA.x;
	PtC4.y = .75 * LA.y + .25 * TA.y;
	PtC4.z = .75 * LA.z + .25 * TA.z;
	
	Chord = sqrt((LA.x-TA.x)*(LA.x-TA.x) + (LA.y-TA.y)*(LA.y-TA.y) + (LA.z-TA.z)*(LA.z-TA.z));

	if(m_pFoilA && m_pFoilB)
	{
		Area = (m_pFoilA->GetArea() * tau + m_pFoilB->GetArea() * (1.0-tau))*Chord*Chord;//m2
	}
	else
	{
		Area = 0.0;
	}
}


double CSurface::GetStripSpanPos(int const &k)
{
	int  l;
	double YPos = 0.0;
	double ZPos = 0.0;
	//get average span position of the strip
	// necessary for strips 'distorted' by fuselage;

	for(l=0; l<m_NXPanels; l++)
	{
		GetPanel(k,l, 0);
		YPos += (LA.y+LB.y+TA.y+TB.y)/4.0;
		ZPos += (LA.z+LB.z+TA.z+TB.z)/4.0;
	}

	YPos /= m_NXPanels;
	ZPos /= m_NXPanels;

	YPos -= (m_LA.y + m_TA.y)/2.0;
	ZPos -= (m_LA.z + m_TA.z)/2.0;

	return sqrt(YPos*YPos+ZPos*ZPos);
}


void CSurface::GetTrailingPt(int k, CVector &C)
{
	GetPanel(k,0,0);

	C.x    = (TA.x+TB.x)/2.0;
	C.y    = (TA.y+TB.y)/2.0;
	C.z    = (TA.z+TB.z)/2.0;
}



double CSurface::GetTwist(int const &k)
{
	GetPanel(k, 0, 0);
	double y = (LA.y+LB.y+TA.y+TB.y)/4.0;
	return  m_TwistA + (m_TwistB-m_TwistA) *(y-m_LA.y)/(m_LB.y-m_LA.y);
}



void CSurface::GetyDist(int const &k, double &y1, double &y2)
{
	//leading edge

	double YPanels, dk;
	YPanels = (double)m_NYPanels;
	dk      = (double)k;

	if(m_YDistType==1)
	{
		//cosine case
		y1  = 1.0/2.0*(1.0-cos( dk*PI   /YPanels));
		y2  = 1.0/2.0*(1.0-cos((dk+1)*PI/YPanels));
	}
	else if(m_YDistType==-2)
	{
		//sine case
		y1  = 1.0*(sin( dk*PI   /2.0/YPanels));
		y2  = 1.0*(sin((dk+1)*PI/2.0/YPanels));
	}
	else if(m_YDistType==2)
	{
		//-sine case
		y1  = 1.0*(1.-cos( dk*PI   /2.0/YPanels));
		y2  = 1.0*(1.-cos((dk+1)*PI/2.0/YPanels));
	}
	else
	{
		//equally spaced case
		y1 =  dk     /YPanels;
		y2 = (dk+1.0)/YPanels;
	}
}


void CSurface::Init()
{
	DL.Set(m_LB.x-m_LA.x,m_LB.y-m_LA.y,m_LB.z-m_LA.z);
	DC.Set(m_TA.x-m_LA.x,m_TA.y-m_LA.y,m_TA.z-m_LA.z);
	Length = DL.VAbs();
	Chord  = DC.VAbs();
	u.Set(DC.x/Chord,  DC.y/Chord,  DC.z/Chord);
	v.Set(DL.x/Length, DL.y/Length, DL.z/Length);

	m_bIsTipLeft   = false;
	m_bIsTipRight  = false;
	m_bIsLeftSurf  = false;
	m_bIsRightSurf = false;

	CVector LATB, TALB;

	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}


bool CSurface::IsFlapPanel(int const &p)
{
	int pp;
	for(pp=0; pp<m_nFlapPanels; pp++)
	{
		if (p==m_FlapPanel[pp]) return true;
	}
	return false;
}


bool CSurface::IsFlapNode(int const &nNode)
{
	int pp;
	for(pp=0; pp<m_nFlapPanels; pp++)
	{
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iLA) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iLB) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iTA) return true;
		if(nNode==s_pPanel[m_FlapPanel[pp]].m_iTB) return true;
	}
	return false;
}


void CSurface::ResetFlap()
{
	int i;
	for(i=0; i<200; i++)
	{
		m_FlapPanel[i] = 30000;
		m_FlapNode[i]  = 30000;
	}
	m_nFlapPanels = 0;
	m_nFlapNodes = 0;
}


bool CSurface::RotateFlap(double const &Angle, bool bBCOnly)
{
	//The average angle between the two tip foil is cancelled
	//Instead, the Panels are rotated by Angle around the hinge point and hinge vector

	int k,l,p;
	double alpha0;
	Quaternion Quat;
	CVector R, S;
	
	p     = 0;

	if(m_pFoilA && m_pFoilB)
	{
		//get the approximate initial angle
		if(fabs(m_pFoilA->m_TEFlapAngle - m_pFoilB->m_TEFlapAngle)>0.1)
		{
			QMessageBox msgBox;
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setWindowTitle(tr("Warning"));
			msgBox.setText(tr("Continuous foils for surface do not have the same initial flap angle... aborting\n"));
			msgBox.exec();

			return false;
		}
		alpha0 = (m_pFoilA->m_TEFlapAngle + m_pFoilB->m_TEFlapAngle)/2.0;

		Quat.Set(Angle-alpha0, m_HingeVector);

/*		for(l=0; l<m_nFlapPanels; l++)
		{
			k = m_FlapPanel[l];
			if(bBCOnly) s_pPanel[k].RotateBC(m_HingePoint, Quat);
			else        s_pPanel[k].RotatePanel(m_HingePoint, Quat);
		}*/

		for (k=0; k<m_nFlapNodes; k++)
		{
			R.x = s_pNode[m_FlapNode[k]].x - m_HingePoint.x;
			R.y = s_pNode[m_FlapNode[k]].y - m_HingePoint.y;
			R.z = s_pNode[m_FlapNode[k]].z - m_HingePoint.z;
			Quat.Conjugate(R,S);

			s_pNode[m_FlapNode[k]].x = S.x + m_HingePoint.x;
			s_pNode[m_FlapNode[k]].y = S.y + m_HingePoint.y;
			s_pNode[m_FlapNode[k]].z = S.z + m_HingePoint.z;
		}

		for(l=0; l<m_nFlapPanels; l++)
		{
			k = m_FlapPanel[l];
			if(s_pPanel[k].m_iPos==-1)
			{
				s_pPanel[k].SetFrame(
					s_pNode[s_pPanel[k].m_iLB],
					s_pNode[s_pPanel[k].m_iLA],
					s_pNode[s_pPanel[k].m_iTB],
					s_pNode[s_pPanel[k].m_iTA]);
			}
			else
			{
				s_pPanel[k].SetFrame(
					s_pNode[s_pPanel[k].m_iLA],
					s_pNode[s_pPanel[k].m_iLB],
					s_pNode[s_pPanel[k].m_iTA],
					s_pNode[s_pPanel[k].m_iTB]);
			}
		}
	}
	else p+= m_NYPanels * m_NXPanels;

	return true;
}


void CSurface::RotateX(CVector const&O, double XTilt)
{
	m_LA.RotateX(O, XTilt);
	m_LB.RotateX(O, XTilt);
	m_TA.RotateX(O, XTilt);
	m_TB.RotateX(O, XTilt);
	m_HingePoint.RotateX(O, XTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateX(Origin, XTilt);
	NormalA.RotateX(Origin, XTilt);
	NormalB.RotateX(Origin, XTilt);
	m_HingeVector.RotateX(Origin, XTilt);
}

void CSurface::RotateY(CVector const &O, double YTilt)
{
	m_LA.RotateY(O, YTilt);
	m_LB.RotateY(O, YTilt);
	m_TA.RotateY(O, YTilt);
	m_TB.RotateY(O, YTilt);
	m_HingePoint.RotateY(O, YTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateY(Origin, YTilt);
	NormalA.RotateY(Origin, YTilt);
	NormalB.RotateY(Origin, YTilt);
	m_HingeVector.RotateY(Origin, YTilt);
}


void CSurface::RotateZ(CVector const &O, double ZTilt)
{
	m_LA.RotateZ(O, ZTilt);
	m_LB.RotateZ(O, ZTilt);
	m_TA.RotateZ(O, ZTilt);
	m_TB.RotateZ(O, ZTilt);
	m_HingePoint.RotateZ(O, ZTilt);

	CVector Origin(0.0,0.0,0.0);
	Normal.RotateZ(Origin, ZTilt);
	NormalA.RotateZ(Origin, ZTilt);
	NormalB.RotateZ(Origin, ZTilt);
	m_HingeVector.RotateZ(Origin, ZTilt);
}


void CSurface::SetFlap()
{
	if(m_pFoilA && m_pFoilA->m_bTEFlap)
	{
		m_posATE = m_pFoilA->m_TEXHinge/100.0;
		if(m_posATE>1.0) m_posATE = 1.0; else if(m_posATE<0.0) m_posATE = 0.0;
	}
	else m_posATE = 1.0;

	if(m_pFoilB && m_pFoilB->m_bTEFlap)
	{
		m_posBTE = m_pFoilB->m_TEXHinge/100.0;
		if(m_posBTE>1.0) m_posBTE = 1.0; else if(m_posBTE<0.0) m_posBTE = 0.0;
	}
	else m_posBTE = 1.0;

	if(m_pFoilA && m_pFoilB) m_bTEFlap = m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap;
	else                     m_bTEFlap = false;


	if(m_pFoilA && m_pFoilB && m_pFoilA->m_bTEFlap && m_pFoilB->m_bTEFlap)
	{
		CVector HB;
		//create a hinge unit vector and initialize hinge moment
		GetPoint(m_posATE, m_posBTE, 0.0, m_HingePoint, 0);
		GetPoint(m_posATE, m_posBTE, 1.0, HB, 0);
		m_HingeVector = HB-m_HingePoint;
		m_HingeVector.Normalize();
	}
}


void CSurface::SetSidePoints(CBody * pBody, double dx, double dz)
{
	//creates the left and right tip points between which the panels will be interpolated
	int l;
	double cosdA = Normal.dot(NormalA);
	double cosdB = Normal.dot(NormalB);
	
	//SideA, SideB are mid points (VLM) or bottom points (3DPanels)
	//SideA_T, SideB_T, are top points (3DPanels);

	for (l=0; l<m_NXPanels; l++)
	{
		xLA = m_xPointA[l+1];
		xLB = m_xPointB[l+1];
		xTA = m_xPointA[l];
		xTB = m_xPointB[l];

		chordA  = GetChord(0.0);//todo : compare with |m_LA-m_TA|
		chordB  = GetChord(1.0);

		GetPoint(xLA, xLB, 0.0, LA, 0); 
		GetPoint(xTA, xTB, 0.0, TA, 0);

		GetPoint(xLA, xLB, 1.0, LB, 0);
		GetPoint(xTA, xTB, 1.0, TB, 0);

		if (m_pFoilA && m_pFoilB)
		{
			//create bottom surface side points
			zA = m_pFoilA->GetLowerY(xLA)*chordA;
			zB = m_pFoilB->GetLowerY(xLB)*chordB;
			SideA_B[l+1]   = LA + NormalA * zA/cosdA;
			SideB_B[l+1]   = LB + NormalB * zB/cosdB;

			//create top surface side points
			zA = m_pFoilA->GetUpperY(xLA)*chordA;
			zB = m_pFoilB->GetUpperY(xLB)*chordB;
			SideA_T[l+1] = LA + NormalA * zA/cosdA;
			SideB_T[l+1] = LB + NormalB * zB/cosdB;

			//create middle surface side points

			zA = m_pFoilA->GetMidY(xLA)*chordA;
			zB = m_pFoilB->GetMidY(xLB)*chordB;
			SideA[l+1]   = LA + NormalA * zA/cosdA;
			SideB[l+1]   = LB + NormalB * zB/cosdB;

			if(l==0)
			{
				zA = m_pFoilA->GetLowerY(xTA)*chordA;
				zB = m_pFoilB->GetLowerY(xTB)*chordB;
				SideA_B[0] = TA + NormalA * zA/cosdA;
				SideB_B[0] = TB + NormalB * zB/cosdB;

				zA = m_pFoilA->GetUpperY(xTA)*chordA;
				zB = m_pFoilB->GetUpperY(xTB)*chordB;
				SideA_T[0] = TA + NormalA * zA/cosdA;
				SideB_T[0] = TB + NormalB * zB/cosdB;

				zA = m_pFoilA->GetMidY(xTA)*chordA;
				zB = m_pFoilB->GetMidY(xTB)*chordB;
				SideA[0]   = TA + NormalA * zA/cosdA;
				SideB[0]   = TB + NormalB * zB/cosdB;
			}
		}
		else
		{
			SideA[l+1]   = LA;
			SideB[l+1]   = LB;
			SideA_T[l+1] = LA;
			SideB_T[l+1] = LB;
			SideA_B[l+1] = LA;
			SideB_B[l+1] = LB;
			if(l==0)
			{
				SideA[0]   = TA;
				SideB[0]   = TB;
				SideA_T[0] = TA;
				SideB_T[0] = TB;
				SideA_B[0] = TA;
				SideB_B[0] = TB;
			}
		}

		SideA_B[l+1].x -=dx;    
		SideA_B[l+1].z -=dz;    
		SideB_B[l+1].x -=dx;    
		SideB_B[l+1].z -=dz;    
		SideA_T[l+1].x -=dx;    
		SideA_T[l+1].z -=dz;    
		SideB_T[l+1].x -=dx;    
		SideB_T[l+1].z -=dz;    
		SideA[l+1].x -=dx;    
		SideA[l+1].z -=dz;    
		SideB[l+1].x -=dx;    
		SideB[l+1].z -=dz;    


		if(pBody && m_bIsCenterSurf && m_bIsLeftSurf)
		{

			if(pBody->Intersect(SideA_B[l+1], SideB_B[l+1], SideB_B[l+1], false)) m_bJoinRight = false;
			if(pBody->Intersect(SideA_T[l+1], SideB_T[l+1], SideB_T[l+1], false)) m_bJoinRight = false;
			if(pBody->Intersect(SideA[l+1],   SideB[l+1],   SideB[l+1],   false)) m_bJoinRight = false;

		}

		else if(pBody && m_bIsCenterSurf && m_bIsRightSurf)
		{
			pBody->Intersect(SideA_B[l+1], SideB_B[l+1], SideA_B[l+1], true);
			pBody->Intersect(SideA_T[l+1], SideB_T[l+1], SideA_T[l+1], true);
			pBody->Intersect(SideA[l+1],   SideB[l+1],     SideA[l+1], true);
		}

		if(l==0)
		{
			SideA_B[0].x -=dx;    
			SideA_B[0].z -=dz;    
			SideB_B[0].x -=dx;    
			SideB_B[0].z -=dz;    
			SideA_T[0].x -=dx;    
			SideA_T[0].z -=dz;    
			SideB_T[0].x -=dx;    
			SideB_T[0].z -=dz;    
			SideA[0].x -=dx;    
			SideA[0].z -=dz;    
			SideB[0].x -=dx;    
			SideB[0].z -=dz;    

			if(pBody && m_bIsCenterSurf && m_bIsLeftSurf)
			{
				if(pBody->Intersect(SideA_B[0], SideB_B[0], SideB_B[0], false)) m_bJoinRight = false;
				if(pBody->Intersect(SideA_T[0], SideB_T[0], SideB_T[0], false)) m_bJoinRight = false;
				if(pBody->Intersect(SideA[0],   SideB[0],   SideB[0],   false)) m_bJoinRight = false;;
			}
			else if(pBody && m_bIsCenterSurf && m_bIsRightSurf)
			{
				pBody->Intersect(SideA_B[0], SideB_B[0], SideA_B[0], true);
				pBody->Intersect(SideA_T[0], SideB_T[0], SideA_T[0], true);
				pBody->Intersect(SideA[0],   SideB[0],     SideA[0], true);
			}
			SideA_B[0].x += dx;    
			SideA_B[0].z += dz;    
			SideB_B[0].x += dx;    
			SideB_B[0].z += dz;    
			SideA_T[0].x += dx;    
			SideA_T[0].z += dz;    
			SideB_T[0].x += dx;    
			SideB_T[0].z += dz;    
			SideA[0].x += dx;    
			SideA[0].z += dz;    
			SideB[0].x += dx;    
			SideB[0].z += dz;    
		}

		SideA_B[l+1].x +=dx;    
		SideA_B[l+1].z +=dz;    
		SideB_B[l+1].x +=dx;    
		SideB_B[l+1].z +=dz;    
		SideA_T[l+1].x +=dx;    
		SideA_T[l+1].z +=dz;    
		SideB_T[l+1].x +=dx;    
		SideB_T[l+1].z +=dz;    
		SideA[l+1].x +=dx;    
		SideA[l+1].z +=dz;    
		SideB[l+1].x +=dx;    
		SideB[l+1].z +=dz;    
	}

	//merge trailing edge points in case the foil has a T.E. gap
	
	CVector Node;

	Node = (SideA_B[0] + SideA_T[0])/2.0;
	SideA_B[0].Set(Node);
	SideA_T[0].Set(Node);

	Node = (SideB_B[0] + SideB_T[0])/2.0;
	SideB_B[0].Set(Node);
	SideB_T[0].Set(Node);
}



void CSurface::SetNormal()
{
	static CVector LATB, TALB;
	LATB = m_TB - m_LA;
	TALB = m_LB - m_TA;
	Normal = LATB * TALB;
	Normal.Normalize();
}


void CSurface::SetTwist()
{
	static CVector A4, B4, L, U, T, O;
	O.Set(0.0,0.0,0.0);

	A4 = m_LA *3.0/4.0 + m_TA * 1/4.0;
	B4 = m_LB *3.0/4.0 + m_TB * 1/4.0;
	L = B4 - A4;
	L.Normalize();

	// create a vector perpendicular to NormalA and x-axis
	T.x = 0.0;
	T.y = +NormalA.z;
	T.z = -NormalA.y;
	//rotate around this axis
	U = m_LA-A4;
	U.Rotate(T, m_TwistA);
	m_LA = A4+ U;

	U = m_TA-A4;
	U.Rotate(T, m_TwistA);
	m_TA = A4 + U;

	NormalA.Rotate(T, m_TwistA);

	// create a vector perpendicular to NormalB and x-axis
	T.x = 0.0;
	T.y = +NormalB.z;
	T.z = -NormalB.y;

	U = m_LB-B4;
	U.Rotate(T, m_TwistB);
	m_LB = B4+ U;

	U = m_TB-B4;
	U.Rotate(T, m_TwistB);
	m_TB = B4 + U;

	NormalB.Rotate(T, m_TwistB);
}




void CSurface::SetTwist_old()
{
	double xc4,zc4;
	CVector O(0.0,0.0,0.0);

	//"A" section first
	xc4 = m_LA.x + (m_TA.x-m_LA.x)/4.0;
	zc4 = m_LA.z + (m_TA.z-m_LA.z)/4.0;
	m_LA.x = xc4 + (m_LA.x-xc4) * cos(m_TwistA *PI/180.0);
	m_LA.z = zc4 - (m_LA.x-xc4) * sin(m_TwistA *PI/180.0);
	m_TA.x = xc4 + (m_TA.x-xc4) * cos(m_TwistA *PI/180.0);
	m_TA.z = zc4 - (m_TA.x-xc4) * sin(m_TwistA *PI/180.0);
	NormalA.RotateY(O, m_TwistA);

	//"B" Section next
	xc4 = m_LB.x + (m_TB.x-m_LB.x)/4.0;
	zc4 = m_LB.z + (m_TB.z-m_LB.z)/4.0;
	m_LB.x = xc4 + (m_LB.x-xc4) * cos(m_TwistB *PI/180.0);
	m_LB.z = zc4 - (m_LB.x-xc4) * sin(m_TwistB *PI/180.0);
	m_TB.x = xc4 + (m_TB.x-xc4) * cos(m_TwistB *PI/180.0);
	m_TB.z = zc4 - (m_TB.x-xc4) * sin(m_TwistB *PI/180.0);
	NormalB.RotateY(O, m_TwistB);
}



void CSurface::Translate(CVector const &T)
{
	m_LA.Translate(T);
	m_LB.Translate(T);
	m_TA.Translate(T);
	m_TB.Translate(T);
	m_HingePoint.Translate(T);
}




void CSurface::CreateXPoints()
{
	//
	// Creates the points at the surfaces two end sections
	// The points will be used later to create the mesh
	// the chordwise panel distribution is set i.a.w. with the flap hinges, if any;
	//
	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	if(m_pFoilA && m_pFoilA->m_bTEFlap) xHingeA=m_pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(m_pFoilB && m_pFoilB->m_bTEFlap) xHingeB=m_pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * (double)m_NXPanels*1.000123);// to avoid numerical errors if exact division
	NXFlapB = (int)((1.0-xHingeB) * (double)m_NXPanels *1.000123);

	if(m_pFoilA && m_pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(m_pFoilB && m_pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;

	NXLeadA = m_NXPanels - NXFlapA;
	NXLeadB = m_NXPanels - NXFlapB;

	m_NXFlap  = qMax(NXFlapA, NXFlapB);
	if(m_NXFlap>m_NXPanels/2) m_NXFlap=(int)m_NXPanels/2;
	m_NXLead  = m_NXPanels - m_NXFlap;

	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(m_XDistType==1)
			m_xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(m_XDistType==1)
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}

	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(m_XDistType==1)
			m_xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}

	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(m_XDistType==1)
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*PI /dl2));
		else
			m_xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}

	m_xPointA[m_NXPanels] = 0.0;
	m_xPointB[m_NXPanels] = 0.0;
}


















