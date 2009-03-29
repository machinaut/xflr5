/****************************************************************************

    WPolar Class
	Copyright (C) 2005-2008 Andre Deperrois XFLR5@yahoo.com

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


// WPolar.cpp: implementation of the CWPolar class.
// The class for wing polars
//
//////////////////////////////////////////////////////////////////////

#include "WPolar.h"
#include "../MainFrame.h"
#include <math.h>
#include "../Globals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWPolar::CWPolar()
{
	m_bIsVisible  = true;
	m_bShowPoints = false;

	m_bVLM1         = true;
	m_bThinSurfaces = true;
	m_bWakeRollUp   = false;
	m_bTiltedGeom   = false;
	m_bViscous      = true;
	m_bPolar        = true;
	m_bGround       = false;

	m_NXWakePanels = 1;
	m_TotalWakeLength = 1.0;
	m_WakePanelFactor =1.1;

	m_AnalysisType = 0;
	m_Type   = 1;
	m_RefAreaType = 1;
	m_Style  = 0;
	m_Width  = 1;
	m_Color  = 255;//red

	m_Beta      = 0.0;
	m_QInf      = 10.0;
	m_Weight    = 1.0;
	m_XCmRef    = 0.0;
	m_ASpec     = 0.0;
	m_WArea     = 0.0;
	m_WMAChord  = 0.0;
	m_WSpan     = 0.0;
	m_AMem      = 0.0;
	m_Height    = 0.0;
	m_Density   = 1.225;
	m_Viscosity = 1.5e-5;//m2/s

	m_nControls = 0;
	memset(m_MinControl, 0, sizeof(m_MinControl));
	memset(m_MaxControl, 0, sizeof(m_MaxControl));
	memset(m_bActiveControl, 0, sizeof(m_bActiveControl));

	pi = 3.141592654;
}




CWPolar::~CWPolar()
{
	ResetWPlr();
//qDebug() << "Destroyed WPolar";
}



void CWPolar::AddPoint(CWOpp *pWOpp)
{
	bool bInserted = false;
	int i;
	int size = (int)m_Alpha.size();

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_Type <4)
			{
				if (fabs(pWOpp->m_Alpha - m_Alpha[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  =  pWOpp->m_QInf;
					m_XCP[i]        =  pWOpp->m_XCP;
					m_YCP[i]        =  pWOpp->m_YCP;
					m_MaxBending[i] =  pWOpp->m_MaxBending;
					m_Ctrl[i]       =  pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pWOpp->m_Alpha < m_Alpha[i])
				{
					// sort by crescending alphas
					m_Alpha.insert(i, pWOpp->m_Alpha);
					m_Cl.insert(i,  pWOpp->m_CL);
					m_CY.insert(i,  pWOpp->m_CY);
					m_ICd.insert(i, pWOpp->m_InducedDrag);
					m_PCd.insert(i, pWOpp->m_ViscousDrag);
					m_TCd.insert(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i, pWOpp->m_GCm);
					m_GRm.insert(i, pWOpp->m_GRm);
					m_GYm.insert(i, pWOpp->m_GYm);
					m_VYm.insert(i, pWOpp->m_VYm);
					m_IYm.insert(i, pWOpp->m_IYm);

					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_XCP.insert(i,  pWOpp->m_XCP);
					m_YCP.insert(i,  pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i, pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if (m_Type==4)
			{
				// type 4, sort by speed
				if (fabs(pWOpp->m_QInf - m_QInfinite[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;
					m_Ctrl[i]       = pWOpp->m_Ctrl;

					bInserted = true;
					break;
				}
				else if (pWOpp->m_QInf < m_QInfinite[i])
				{
					// sort by crescending alphas
					m_Alpha.insert(i, pWOpp->m_Alpha);
					m_Cl.insert(i, pWOpp->m_CL);
					m_CY.insert(i, pWOpp->m_CY);
					m_ICd.insert(i, pWOpp->m_InducedDrag);
					m_PCd.insert(i, pWOpp->m_ViscousDrag);
					m_TCd.insert(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i, pWOpp->m_GCm);
					m_GRm.insert(i, pWOpp->m_GRm);
					m_GYm.insert(i, pWOpp->m_GYm);
					m_VYm.insert(i, pWOpp->m_VYm);
					m_IYm.insert(i, pWOpp->m_IYm);

					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_XCP.insert(i,  pWOpp->m_XCP);
					m_YCP.insert(i,  pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i, pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if (m_Type==5 || m_Type==6)
			{
				// type 5 or 6, sort by crescending ctrl value
				if (fabs(pWOpp->m_Ctrl - m_Ctrl[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;
					m_Ctrl[i]       = pWOpp->m_Ctrl;

					bInserted = true;
					break;
				}
				else if (pWOpp->m_Ctrl < m_Ctrl[i])
				{
					// sort by crescending control values
					m_Alpha.insert(i, pWOpp->m_Alpha);
					m_Cl.insert(i, pWOpp->m_CL);
					m_CY.insert(i, pWOpp->m_CY);
					m_ICd.insert(i, pWOpp->m_InducedDrag);
					m_PCd.insert(i, pWOpp->m_ViscousDrag);
					m_TCd.insert(i, pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i, pWOpp->m_GCm);
					m_GRm.insert(i, pWOpp->m_GRm);
					m_GYm.insert(i, pWOpp->m_GYm);
					m_VYm.insert(i, pWOpp->m_VYm);
					m_IYm.insert(i, pWOpp->m_IYm);

					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_XCP.insert(i,  pWOpp->m_XCP);
					m_YCP.insert(i,  pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i, pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
		}
	}
	if(bInserted) CalculatePoint(i);
	else
	{
		// data is appended at the end
		m_Alpha.append(pWOpp->m_Alpha);
		m_Cl.append(pWOpp->m_CL);
		m_CY.append(pWOpp->m_CY);
		m_ICd.append(pWOpp->m_InducedDrag);
		m_PCd.append(pWOpp->m_ViscousDrag);
		m_TCd.append(pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

		m_GCm.append(pWOpp->m_GCm);
		m_GRm.append(pWOpp->m_GRm);
		m_GYm.append(pWOpp->m_GYm);
		m_VYm.append(pWOpp->m_VYm);
		m_IYm.append(pWOpp->m_IYm);

		m_QInfinite.append(pWOpp->m_QInf);
		m_XCP.append(pWOpp->m_XCP);
		m_YCP.append(pWOpp->m_YCP);
		m_MaxBending.append(pWOpp->m_MaxBending);
		m_Ctrl.append(pWOpp->m_Ctrl);

		m_1Cl.append(0.0);//make room for computed values
		m_ClCd.append(0.0);
		m_Cl32Cd.append(0.0);
		m_Vx.append(0.0);
		m_Vz.append(0.0);
		m_L.append(0.0);
		m_D.append(0.0);
		m_Gamma.append(0.0);
		m_Rm.append(0.0);
		m_Pm.append(0.0);
		m_Ym.append(0.0);
		m_VertPower.append(0.0);
		m_Oswald.append(0.0);
		m_SM.append(0.0);

		CalculatePoint(size);
	}
}

void CWPolar::AddPoint(CPOpp *pPOpp)
{
	bool bInserted = false;
	int i;
	int size = (int)m_Alpha.size();
	CWOpp *pWOpp = &pPOpp->m_WingWOpp;

	if(size)
	{
		for (i=0; i<size; i++)
		{
			if(m_Type <4)
			{
				if (fabs(pPOpp->m_Alpha - m_Alpha[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      =  pWOpp->m_Alpha;
					m_QInfinite[i]  =  pWOpp->m_QInf;
					m_Cl[i]         =  pWOpp->m_CL;
					m_CY[i]         =  pWOpp->m_CY;
					m_ICd[i]        =  pWOpp->m_InducedDrag;
					m_PCd[i]        =  pWOpp->m_ViscousDrag;
					m_TCd[i]        =  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        =  pWOpp->m_GCm;
					m_GRm[i]        =  pWOpp->m_GRm;
					m_GYm[i]        =  pWOpp->m_GYm;
					m_VYm[i]        =  pWOpp->m_VYm;
					m_IYm[i]        =  pWOpp->m_IYm;

					m_XCP[i]        =  pWOpp->m_XCP;
					m_YCP[i]        =  pWOpp->m_YCP;
					m_MaxBending[i] =  pWOpp->m_MaxBending;
					m_Ctrl[i]       =  pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Alpha < m_Alpha[i])
				{
					// sort by crescending alphas
					m_Alpha.insert(i, pWOpp->m_Alpha);
					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_Cl.insert(i,   pWOpp->m_CL);
					m_CY.insert(i,   pWOpp->m_CY);
					m_ICd.insert(i,  pWOpp->m_InducedDrag);
					m_PCd.insert(i,  pWOpp->m_ViscousDrag);
					m_TCd.insert(i,  pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i,  pWOpp->m_GCm);
					m_GRm.insert(i,  pWOpp->m_GRm);
					m_GYm.insert(i,  pWOpp->m_GYm);
					m_VYm.insert(i,  pWOpp->m_VYm);
					m_IYm.insert(i,  pWOpp->m_IYm);

					m_XCP.insert(i,  pWOpp->m_XCP);
					m_YCP.insert(i,  pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i, pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if(m_Type==4)
			{
				// type 4, sort by speed
				if (fabs(pPOpp->m_QInf - m_QInfinite[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      = pWOpp->m_Alpha;
					m_Cl[i]         = pWOpp->m_CL;
					m_CY[i]         = pWOpp->m_CY;
					m_ICd[i]        = pWOpp->m_InducedDrag;
					m_PCd[i]        = pWOpp->m_ViscousDrag;
					m_TCd[i]        = pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        = pWOpp->m_GCm;
					m_GRm[i]        = pWOpp->m_GRm;
					m_GYm[i]        = pWOpp->m_GYm;
					m_VYm[i]        = pWOpp->m_VYm;
					m_IYm[i]        = pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;
					m_Ctrl[i]       = pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pPOpp->m_QInf < m_QInfinite[i])
				{
					// sort by crescending speed
					m_Alpha.insert(i,     pWOpp->m_Alpha);
					m_Cl.insert(i,        pWOpp->m_CL);
					m_CY.insert(i,        pWOpp->m_CY);
					m_ICd.insert(i,       pWOpp->m_InducedDrag);
					m_PCd.insert(i,       pWOpp->m_ViscousDrag);
					m_TCd.insert(i,       pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i,       pWOpp->m_GCm);
					m_GRm.insert(i,       pWOpp->m_GRm);
					m_GYm.insert(i,       pWOpp->m_GYm);
					m_VYm.insert(i,       pWOpp->m_VYm);
					m_IYm.insert(i,       pWOpp->m_IYm);

					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_XCP.insert(i,       pWOpp->m_XCP);
					m_YCP.insert(i,       pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i,       pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
			else if(m_Type==5 || m_Type==6)
			{
				// type 5 or 6, sort by control value
				if (fabs(pPOpp->m_Ctrl - m_Ctrl[i]) < 0.001)
				{
					// then erase former result
					m_Alpha[i]      = pWOpp->m_Alpha;
					m_Cl[i]         = pWOpp->m_CL;
					m_CY[i]         = pWOpp->m_CY;
					m_ICd[i]        = pWOpp->m_InducedDrag;
					m_PCd[i]        = pWOpp->m_ViscousDrag;
					m_TCd[i]        = pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag;

					m_GCm[i]        = pWOpp->m_GCm;
					m_GRm[i]        = pWOpp->m_GRm;
					m_GYm[i]        = pWOpp->m_GYm;
					m_VYm[i]        = pWOpp->m_VYm;
					m_IYm[i]        = pWOpp->m_IYm;

					m_QInfinite[i]  = pWOpp->m_QInf;
					m_XCP[i]        = pWOpp->m_XCP;
					m_YCP[i]        = pWOpp->m_YCP;
					m_MaxBending[i] = pWOpp->m_MaxBending;
					m_Ctrl[i]       = pWOpp->m_Ctrl;
					bInserted = true;
					break;
				}
				else if (pPOpp->m_Ctrl < m_Ctrl[i])
				{
					// sort by crescending control values
					m_Alpha.insert(i,     pWOpp->m_Alpha);
					m_Cl.insert(i,        pWOpp->m_CL);
					m_CY.insert(i,        pWOpp->m_CY);
					m_ICd.insert(i,       pWOpp->m_InducedDrag);
					m_PCd.insert(i,       pWOpp->m_ViscousDrag);
					m_TCd.insert(i,       pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

					m_GCm.insert(i,       pWOpp->m_GCm);
					m_GRm.insert(i,       pWOpp->m_GRm);
					m_GYm.insert(i,       pWOpp->m_GYm);
					m_VYm.insert(i,       pWOpp->m_VYm);
					m_IYm.insert(i,       pWOpp->m_IYm);

					m_QInfinite.insert(i, pWOpp->m_QInf);
					m_XCP.insert(i,       pWOpp->m_XCP);
					m_YCP.insert(i,       pWOpp->m_YCP);
					m_MaxBending.insert(i, pWOpp->m_MaxBending);
					m_Ctrl.insert(i,       pWOpp->m_Ctrl);

					m_1Cl.insert(i,0.0);//make room for computed values
					m_ClCd.insert(i,0.0);
					m_Cl32Cd.insert(i,0.0);
					m_Vx.insert(i,0.0);
					m_Vz.insert(i,0.0);
					m_L.insert(i,0.0);
					m_D.insert(i,0.0);
					m_Gamma.insert(i,0.0);
					m_Rm.insert(i, 0.0);
					m_Pm.insert(i, 0.0);
					m_Ym.insert(i, 0.0);
					m_VertPower.insert(i, 0.0);
					m_Oswald.insert(i, 0.0);
					m_SM.insert(i, 0.0);

					bInserted = true;
					break;
				}
			}
		}
	}
	if(bInserted) CalculatePoint(i);
	else
	{
		// data is appended at the end
		m_Alpha.append(pWOpp->m_Alpha);
		m_Cl.append(pWOpp->m_CL);
		m_CY.append(pWOpp->m_CY);
		m_ICd.append(pWOpp->m_InducedDrag);
		m_PCd.append(pWOpp->m_ViscousDrag);
		m_TCd.append(pWOpp->m_InducedDrag + pWOpp->m_ViscousDrag);

		m_GCm.append(pWOpp->m_GCm);
		m_GRm.append(pWOpp->m_GRm);
		m_GYm.append(pWOpp->m_GYm);
		m_VYm.append(pWOpp->m_VYm);
		m_IYm.append(pWOpp->m_IYm);

		m_QInfinite.append(pWOpp->m_QInf);
		m_XCP.append(pWOpp->m_XCP);
		m_YCP.append(pWOpp->m_YCP);
		m_MaxBending.append(pWOpp->m_MaxBending);
		m_Ctrl.append(pWOpp->m_Ctrl);
		m_1Cl.append(0.0);//make room for computed values
		m_ClCd.append(0.0);
		m_Cl32Cd.append(0.0);
		m_Vx.append(0.0);
		m_Vz.append(0.0);
		m_L.append(0.0);
		m_D.append(0.0);
		m_Gamma.append(0.0);
		m_Rm.append(0.0);
		m_Pm.append(0.0);
		m_Ym.append(0.0);
		m_VertPower.append(0.0);
		m_Oswald.append(0.0);
		m_SM.append(0.0);

		CalculatePoint(size);
	}
}


void CWPolar::CalculatePoint(int i)
{
	//finish calculations
//	double cl = m_Cl[i];
//	double tcd = m_TCd[i];
//	double Qinf =  m_QInfinite[i];

	m_ClCd[i]   =  m_Cl[i]/m_TCd[i];

	if(m_Cl[i]>0.0) {
		m_1Cl[i]    = (double)(1./sqrt(m_Cl[i]));
		m_Cl32Cd[i] = (double)pow(m_Cl[i],1.5)/m_TCd[i];
	}
	else {
		m_1Cl[i]    = -1.0;//will not be plotted
		m_Cl32Cd[i] =  -(double)pow(-m_Cl[i],1.5)/m_TCd[i];
	}

	if(fabs(m_Cl[i])>0.) m_Gamma[i]  =  atan(m_TCd[i]/m_Cl[i]) * 180.0/pi;
	else m_Gamma[i] = 90.0;
	m_Vz[i] = (double)sqrt(2*m_Weight*9.81/m_Density/m_WArea)/m_Cl32Cd[i];
	m_Vx[i] = m_QInfinite[i] * (double)cos(m_Gamma[i]*pi/180.0);

	//dynamic pressure
	double q =  0.5 * m_Density * m_QInfinite[i]*m_QInfinite[i];

	m_L[i]  = q * m_Cl[i]*m_WArea;
	m_D[i]  = q * m_TCd[i]*m_WArea;

	m_Rm[i] = q * m_WArea * m_GRm[i] * m_WSpan;// in N.m
	m_Ym[i] = q * m_WArea * m_GYm[i] * m_WSpan;// in N.m
	m_Pm[i] = q * m_WArea * m_GCm[i] * m_WMAChord;// in N.m

	//power for horizontal flight
	m_VertPower[i] = m_Weight * 9.81 * m_Vz[i];

	double AR      = m_WSpan*m_WSpan/m_WArea;

	if(m_ICd[i]==0.0)	m_Oswald[i] = 0.0;
	else				m_Oswald[i] = m_Cl[i]*m_Cl[i]/pi/m_ICd[i]/AR;

	m_SM[i]        = (m_XCP[i]-m_XCmRef)/m_WMAChord *100.00;
}


/*	N =  Cn.q.s.b
	L =  Ct.q.s.b
	R =  Cm.q.s.c'
*/


void CWPolar::Copy(CWPolar *pWPolar)
{
	int i;
	m_bIsVisible    = pWPolar->m_bIsVisible;
	m_bPolar        = pWPolar->m_bPolar;
	m_bShowPoints   = pWPolar->m_bShowPoints;
	m_bTiltedGeom   = pWPolar->m_bTiltedGeom;
	m_bViscous      = pWPolar->m_bViscous;
	m_bVLM1         = pWPolar->m_bVLM1;
	m_bWakeRollUp   = pWPolar->m_bWakeRollUp;
	m_AnalysisType  = pWPolar->m_AnalysisType;
	m_bThinSurfaces = pWPolar->m_bThinSurfaces;
	m_nControls     = pWPolar->m_nControls;

	int size  = (int)m_Alpha.size();
	for(i=size-1; i>=0; i--)
		Remove(i);

	size  = (int)pWPolar->m_Alpha.size();
	for(i=0; i<size; i++)
	{
		m_Alpha.insert(i,      pWPolar->m_Alpha[i]);
		m_Cl.insert(i,         pWPolar-> m_Cl[i]);
		m_CY.insert(i,         pWPolar-> m_CY[i]);
		m_ICd.insert(i,        pWPolar-> m_ICd[i]);
		m_PCd.insert(i,        pWPolar-> m_PCd[i]);
		m_TCd.insert(i,        pWPolar-> m_TCd[i]);

		m_GCm.insert(i,        pWPolar-> m_GCm[i]);
		m_GRm.insert(i,        pWPolar-> m_GRm[i]);
		m_GYm.insert(i,        pWPolar-> m_GYm[i]);
		m_VYm.insert(i,        pWPolar-> m_VYm[i]);
		m_IYm.insert(i,        pWPolar-> m_IYm[i]);

		m_ClCd.insert(i,       pWPolar-> m_ClCd[i]);
		m_1Cl.insert(i,        pWPolar-> m_1Cl[i]);
		m_Cl32Cd.insert(i,     pWPolar-> m_Cl32Cd[i]);
		m_QInfinite.insert(i,  pWPolar-> m_QInfinite[i]);

		m_Gamma.insert(i,      pWPolar-> m_Gamma[i]);
		m_XCP.insert(i,        pWPolar-> m_XCP[i]);
		m_YCP.insert(i,        pWPolar-> m_YCP[i]);
		m_MaxBending.insert(i, pWPolar-> m_MaxBending[i]);
		m_VertPower.insert(i,  pWPolar-> m_VertPower[i]);

		m_Oswald.insert(i,     pWPolar-> m_Oswald[i]);
		m_SM.insert(i,         pWPolar-> m_SM[i]);
		m_Ctrl.insert(i,       pWPolar-> m_Ctrl[i]);
		m_L.insert(i,          pWPolar-> m_L[i]);
		m_D.insert(i,          pWPolar-> m_D[i]);
		m_Vx.insert(i,         pWPolar-> m_Vx[i]);

		m_Vz.insert(i,         pWPolar-> m_Vz[i]);
		m_Pm.insert(i,         pWPolar-> m_Pm[i]);
		m_Ym.insert(i,         pWPolar-> m_Ym[i]);
		m_Rm.insert(i,         pWPolar-> m_Rm[i]);
	}
}

void CWPolar::Export(QTextStream &out, int FileType)
{
	MainFrame* pMainFrame = (MainFrame*)m_pParent;
	int j;
	QString strOut, Header, strong, str;



	if (FileType==1)
	{
	//	strong ="\n    " + pFrame->m_VersionName + "\n\n";
		strong = "QFLR5_v.0001\n\n";//TODO
		out << strong;

		strong ="Wing name :        "+ m_UFOName + "\n";
		out << strong;

		strong ="Wing polar name :  "+ m_PlrName + "\n";
		out << strong;

		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		str +="\n\n";
		strong = QString("Freestream speed : %1 ").arg(m_QInf*pMainFrame->m_mstoUnit,3,'f',1);
		strong +=str;
		out << strong;

		Header = "   alpha      CL          ICd        PCd        TCd        CY        GCm         GRm        GYm       IYm       QInf        XCP\n";
		out << Header;
		Header = " _________  ________   ________  _________  _________  _________  _________  _________  _________  _________  _________  _________\n";
		out << Header;
		for (j=0; j<m_Alpha.size(); j++)
		{
			strong = QString(" %1  %2  %3  %4  %5  %6  %7  %8  %9  %10  %11  %12\n")
					 .arg(m_Alpha[j],8,'f',3)
					 .arg(m_Cl[j], 9,'f',6)
					 .arg(m_ICd[j],9,'f',6)
					 .arg(m_PCd[j],9,'f',6)
					 .arg(m_TCd[j],9,'f',6)
					 .arg(m_CY[j] ,9,'f',6)
					 .arg(m_GCm[j],9,'f',6)
					 .arg(m_GRm[j],9,'f',6)
					 .arg(m_GYm[j],9,'f',6)
					 .arg(m_IYm[j],9,'f',6)
					 .arg(m_QInfinite[j],8,'f',4)
					 .arg(m_XCP[j],9,'f',4);

			out << strong;
		}
	}
	else if (FileType==2)
	{
		QString strOut;
		QString Header, strong;

	//	strong ="\n    " + pFrame->m_VersionName + "\n\n";
		strong = "QFLR5_v.0001\n\n";//TODO
		out << strong;

		strong ="Wing name :, "+ m_UFOName + "\n";
		out << strong;

		strong ="Wing polar name :, "+ m_PlrName + "\n";
		out << strong;

		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		str +="\n\n";
		strong = QString("Freestream speed :, %1 ").arg(m_QInf*pMainFrame->m_mstoUnit,3,'f',1);
		strong +=str;
		out << strong;

		Header = "alpha, CL, ICd, PCd, TCd, CY, GCm, GRm,GYm, IYm, QInf, XCP\n";
		out << Header;
		Header = " _________,  ________,   ________,  _________,  _________,  _________,  _________,  _________,  _________,  _________,  _________,  _________\n";
		out << Header;
		for (j=0; j<m_Alpha.size(); j++)
		{
//			strong.Format(" %8.3f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %9.6f,  %8.4f,  %9.4f\n",
			strong = QString(" %1,  %2,  %3,  %4,  %5,  %6,  %7,  %8,  %9,  %10,  %11,  %12\n")
					 .arg(m_Alpha[j],8,'f',3)
					 .arg(m_Cl[j], 9,'f',6)
					 .arg(m_ICd[j],9,'f',6)
					 .arg(m_PCd[j],9,'f',6)
					 .arg(m_TCd[j],9,'f',6)
					 .arg(m_CY[j] ,9,'f',6)
					 .arg(m_GCm[j],9,'f',6)
					 .arg(m_GRm[j],9,'f',6)
					 .arg(m_GYm[j],9,'f',6)
					 .arg(m_IYm[j],9,'f',6)
					 .arg(m_QInfinite[j],8,'f',4)
					 .arg(m_XCP[j],9,'f',4);

			out << strong;

		}
	}
	out << "\n\n";

	return ;
}



void CWPolar::Remove(int i)
{
	m_Alpha.removeAt(i);
	m_Cl.removeAt(i);
	m_CY.removeAt(i);
	m_ICd.removeAt(i);
	m_PCd.removeAt(i);
	m_TCd.removeAt(i);

	m_GCm.removeAt(i);
	m_GRm.removeAt(i);
	m_GYm.removeAt(i);
	m_VYm.removeAt(i);
	m_IYm.removeAt(i);

	m_XCP.removeAt(i);
	m_YCP.removeAt(i);
	m_MaxBending.removeAt(i);
	m_VertPower.removeAt(i);

	m_Oswald.removeAt(i);
	m_SM.removeAt(i);
	m_Ctrl.removeAt(i);
	m_ClCd.removeAt(i);
	m_1Cl.removeAt(i);
	m_Cl32Cd.removeAt(i);

	m_QInfinite.removeAt(i);
	m_Gamma.removeAt(i);
	m_L.removeAt(i);
	m_D.removeAt(i);
	m_Vx.removeAt(i);

	m_Vz.removeAt(i);
	m_Pm.removeAt(i);
	m_Ym.removeAt(i);
	m_Rm.removeAt(i);
}

void CWPolar::ResetWPlr()
{
	m_Alpha.clear();
	m_Cl.clear();
	m_CY.clear();
	m_ICd.clear();
	m_PCd.clear();
	m_TCd.clear();

	m_GCm.clear();
	m_GRm.clear();
	m_GYm.clear();
	m_VYm.clear();
	m_IYm.clear();

	m_XCP.clear();
	m_YCP.clear();
	m_MaxBending.clear();
	m_VertPower.clear();

	m_Oswald.clear();
	m_SM.clear();
	m_Ctrl.clear();
	m_ClCd.clear();
	m_1Cl.clear();
	m_Cl32Cd.clear();

	m_QInfinite.clear();
	m_Gamma.clear();
	m_L.clear();
	m_D.clear();
	m_Vx.clear();

	m_Vz.clear();
	m_Pm.clear();
	m_Ym.clear();
	m_Rm.clear();
}


bool CWPolar::SerializeWPlr(QDataStream &ar, bool bIsStoring)
{
	int n;
	float f;
	int i, j, ArchiveFormat;// identifies the format of the file
	//Call Base class function
	if(bIsStoring)
	{
		//write variables
		
		ar << 1016; // identifies the format of the file
					// 1016 : added lateral force coefficient
					// 1015 : added lateral force coefficient
					// 1014 : added control results
					// 1013 : added control variables
					// 1012 : redefined the moment coefficients
					// 1011 : added wake roll-up parameters
					// 1010 : added ground effect variables langth changed length unit to m
					// 1009 : added viscous flag
					// 1008 : added Tilted Geometry flag
					// 1007 : added NXWakePanels
					// 1006 : added Wake Roll Up flag
					// 1005 : added VLM method types
					// 1004 : with corrected PCd calculation
					// 1003 : added bending moment V18
					// 1002 : added XCmRef 
					// 1001 : v0.00
		WriteCString(ar,m_UFOName);
		WriteCString(ar,m_PlrName);
		ar << (float)m_WArea << (float)m_WMAChord << (float)m_WSpan ;
		ar << m_Style  << m_Width;
		WriteCOLORREF(ar, m_Color);
		ar << m_AnalysisType;
		if (m_bVLM1)         ar << 1; else ar << 0;
		if (m_bThinSurfaces) ar << 1; else ar << 0;
		if (m_bTiltedGeom)   ar << 1; else ar << 0;
		if (m_bWakeRollUp)   ar << 1; else ar << 0;
		if (m_bViscous)      ar << 1; else ar << 0;
		if (m_bGround)       ar << 1; else ar << 0;
		ar << (float)m_Height;

		ar << m_NXWakePanels << (float)m_TotalWakeLength << (float)m_WakePanelFactor;

		if (m_bIsVisible)  ar << 1; else ar << 0;
		if (m_bShowPoints) ar << 1; else ar << 0;
		ar << m_Type;
		ar << (float)m_QInf;
		ar << (float)m_Weight;
		ar << (float)m_ASpec ;
		ar << (float)m_Beta ;
		ar << (float)m_XCmRef;
		ar <<( float)m_Density << (float)m_Viscosity;

		ar << m_RefAreaType;

		ar <<(int)m_Alpha.size();
		for (i=0; i< (int)m_Alpha.size(); i++)
		{
			ar << (float)m_Alpha[i] << (float)m_Cl[i] << (float)m_CY[i] << (float)m_ICd[i] << (float)m_PCd[i] ;

			ar << (float)m_GCm[i] << (float)m_GRm[i] << (float)m_GYm[i] << 0.0f << (float)m_VYm[i] << (float)m_IYm[i];

			ar << (float)m_QInfinite[i];

			ar << (float)m_XCP[i] << (float)m_YCP[i] << (float)m_MaxBending[i];

			ar << (float)m_Ctrl[i];
		}

		ar << m_nControls;
		for(i=0; i<m_nControls; i++)
		{
			ar << (float)m_MinControl[i] << (float)m_MaxControl[i];
		}
		for(i=0; i<m_nControls; i++)
		{
			if(m_bActiveControl[i])	ar<<1 ; else ar <<0;
		}

		return true;
	}
	else 
	{
		//read variables
		ar >> ArchiveFormat;
		if (ArchiveFormat <=1000 || ArchiveFormat>1100)
		{
			m_PlrName ="";
			return false;
		}
		ReadCString(ar, m_UFOName);
		ReadCString(ar, m_PlrName);

		ar>> f;
		m_WArea = f;
		if (m_WArea<0){
			m_PlrName ="";
			return false;
		}

		ar>> f;
		m_WMAChord = f;
		if (m_WMAChord<0){
			m_PlrName ="";
			return false;
		}

		ar>> f;
		m_WSpan = f;
		if (m_WSpan<0){
			m_PlrName ="";
			return false;
		}

		ar >> m_Style  >> m_Width;
		if (m_Style<0 || m_Style> 10){
			m_PlrName ="";
			return false;
		}
		if (m_Width<0 || m_Width> 10){
			m_PlrName ="";
			return false;
		}
		ReadCOLORREF(ar, m_Color);

		ar >> m_AnalysisType; 
		if (m_AnalysisType<0 || m_AnalysisType>10)
		{
			m_PlrName ="";
			return false;
		}
		if(m_AnalysisType==0) m_AnalysisType=2;

		if(ArchiveFormat>=1005)
		{
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bVLM1 =true; else m_bVLM1 = false;
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bThinSurfaces =true; else m_bThinSurfaces = false;
		}
		if(ArchiveFormat>=1008)
		{
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bTiltedGeom =true; else m_bTiltedGeom = false;
		}

		if(ArchiveFormat>=1006)
		{
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bWakeRollUp =true; else m_bWakeRollUp = false;
		}
		if(ArchiveFormat>=1009)
		{
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bViscous =true; else m_bViscous = false;
		}


		if(ArchiveFormat>=1010)
		{
			ar >> n; 
			if (n!=0 && n!=1)
			{
				m_PlrName ="";
				return false;
			}
			if(n) m_bGround =true; else m_bGround = false;
			ar >> f; m_Height = f;
		}

		if(ArchiveFormat>=1007)
		{
			ar >> m_NXWakePanels; 
			if (m_NXWakePanels<0 || m_NXWakePanels>1000)
			{
				m_PlrName ="";
				return false;
			}
		}

		if(ArchiveFormat>=1011)
		{
			ar >> f; 			m_TotalWakeLength  = f;
			ar >> f; 			m_WakePanelFactor = f;
		}

		ar >> n; 
		if (n!=0 && n!=1)
		{
			m_PlrName ="";
			return false;
		}
		else {
			if(n) m_bIsVisible =true; else m_bIsVisible = false;
		}
		ar >> n; 
		if (n!=0 && n!=1){
			m_PlrName ="";
			return false;
		}
		else {
			if(n) m_bShowPoints =true; else m_bShowPoints = false;
		}

		ar >> n;
		if (n<1 || n>10){
			m_PlrName ="";
			return false;
		}
		m_Type = n;

		ar >> f;	m_QInf = f;
		ar >> f;	m_Weight = f;
		ar >> f;	m_ASpec = f;
		if(ArchiveFormat>=1015) ar >> f;	m_Beta = f;

		if(ArchiveFormat>=1002) ar >> f; m_XCmRef = f;

		ar >> f;	m_Density=f;
		ar >> f;	m_Viscosity=f;

		if(ArchiveFormat>=1016) ar >> m_RefAreaType;
		else                    m_RefAreaType = 1;

		ar >> n;
		if (n<0 || n> 100000){
			m_PlrName ="";
			return false;
		}
		if(ArchiveFormat<1010){
			m_WArea    /=100.0;
			m_WMAChord /=1000.0;
			m_WSpan    /=1000.0;
			m_XCmRef   /=1000.0;
		}
	
		float Alpha,  Cl, CY=0.0, ICd, PCd, GCm, GRm, GYm, VCm, VYm, IYm, QInfinite, XCP, YCP, Ctrl;
		float Cb = 0.0;
		bool bExists;
		for (i=0; i< n; i++)
		{
			ar >> Alpha >> Cl;
			if(ArchiveFormat>=1015) ar>>CY;
			ar >> ICd >> PCd;
			ar >> GCm >> GRm >> GYm >> VCm >> VYm >> IYm;
			if(ArchiveFormat<1012) GCm = GRm = GYm = VCm = VYm = IYm = 0.0;
			ar >> QInfinite >> XCP >> YCP;
			if(ArchiveFormat<1010)
			{
				XCP   /=1000.0;
				YCP   /=1000.0;
			}

			if (ArchiveFormat>=1003)
				ar >> Cb;
			else
				Cb = 0.0;

			if (ArchiveFormat>=1014) ar >> Ctrl;
			else					 Ctrl = 0.0;

			bExists = false;
			if(m_Type!=4)
			{
				for (j=0; j<m_Alpha.size(); j++)
				{
					if(fabs(Alpha-m_Alpha[j])<0.001)
					{
						bExists = true;
						break;
					}
				}
			}
			else 
			{
				for (j=0; j<m_Alpha.size(); j++)
				{
					if(fabs(QInfinite-m_QInfinite[j])<0.001)
					{
						bExists = true;
						break;
					}
				}
			}
			if(!bExists)
			{
				m_Alpha.append(Alpha);
				m_Cl.append(Cl);
				m_CY.append(CY);
				m_ICd.append(ICd);
				m_PCd.append(PCd);
				m_TCd.append(ICd+PCd);

				m_GCm.append(GCm);
				m_GRm.append(GRm);
				m_GYm.append(GYm);
				m_VYm.append(VYm);
				m_IYm.append(IYm);

				m_QInfinite.append(QInfinite);

				m_XCP.append(XCP);
				m_YCP.append(YCP);
				m_MaxBending.append(Cb);
				m_Ctrl.append(Ctrl);

				m_1Cl.append(0.0);
				m_ClCd.append(0.0);
				m_Cl32Cd.append(0.0);
				m_Vx.append(0.0);
				m_Vz.append(0.0);
				m_L.append(0.0);
				m_D.append(0.0);
				m_Gamma.append(0.0);
				m_Rm.append(0.0);
				m_Pm.append(0.0);
				m_Ym.append(0.0);
				m_VertPower.append(0.0);
				m_Oswald.append(0.0);
				m_SM.append(0.0);

				CalculatePoint((int)m_Alpha.size()-1);
			}
		}
		if(ArchiveFormat>1012)
		{
			ar >> m_nControls;
			if(abs(m_nControls)>1000) m_nControls = 0;
			for(i=0; i<m_nControls; i++)
			{
				ar >> f; m_MinControl[i] = f;
				ar >> f; m_MaxControl[i] = f;
			}
			for(i=0; i<m_nControls; i++)
			{
				ar >> n; 
				if (n!=0 && n!=1){
					m_PlrName ="";
					return false;
				}
				else {
					if(n) m_bActiveControl[i] =true; else m_bActiveControl[i] = false;
				}
			}
		}
	}

	return true;
}
