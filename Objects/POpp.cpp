/****************************************************************************

    POpp Class
    Copyright (C) 2006 Andre Deperrois XFLR5@yahoo.com

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

// POpp.cpp: implementation of the CPOpp class.
//
//////////////////////////////////////////////////////////////////////


#include "POpp.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPOpp::CPOpp()
{
	m_PlaneName   = "";
	m_PlrName     = "";
	m_NStation    = 0;
	m_NPanels  = 0;
	m_Color       = QColor(255,0,0);
	m_Style       = 0;
	m_Width       = 1;
	m_Type        = 1;
	m_VLMType     = 1;

	m_bIsVisible  = true;
	m_bShowPoints = false;
	m_bBiplane    = false;
	m_bStab       = false;
	m_bFin        = false;
	m_bOut        = false;
	m_bVLM1       = true;

	m_Weight              = 0.0;
	m_Alpha               = 0.0;
	m_Beta                = 0.0;
	m_QInf                = 0.0;
	m_Ctrl                = 0.0;

	memset(m_Cp, 0, sizeof(m_Cp));
	memset(m_G,  0, sizeof(m_G));
}

CPOpp::~CPOpp()
{

}


bool CPOpp::SerializePOpp(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int a, k;
//	int p, p0;
	float f;

	if(bIsStoring)
	{
		ar << 1007;
		//1007 : added Sideslip Beta
		//1006 : added Panel's source strengths Sigma
		//1005 : added second wing results for a biplane
		//1004 : converted units to SI
		//1003 : added vortices strengths
		//1002 : added Cp, suppressed Cl and other parameters
		//1001 : suppressed Ai, Cd, SpanPos, 

		//write variables
		WriteCString(ar, m_PlaneName);
		WriteCString(ar, m_PlrName);
		if(m_bBiplane)    ar << 1; else ar<<0;
		if(m_bStab)       ar << 1; else ar<<0;
		if(m_bFin)        ar << 1; else ar<<0;
		if(m_bIsVisible)  ar << 1; else ar<<0;
		if(m_bShowPoints) ar << 1; else ar<<0;
		if(m_bOut)        ar << 1; else ar<<0;
		if(m_bVLM1)       ar << 1; else ar<<0;
		ar << 1;

		ar << m_Style << m_Width;
		WriteCOLORREF(ar, m_Color);
		ar << m_Type << m_NStation;
		ar << (float)m_Alpha << (float)m_QInf << (float)m_Weight;
		ar << (float)m_Beta;

		ar << m_NPanels;
		for (k=0; k<=m_NPanels; k++) ar << (float)m_Cp[k];
		for (k=0; k<=m_NPanels; k++) ar << (float)m_G[k];
		for (k=0; k<=m_NPanels; k++) ar << (float)m_Sigma[k];

		ar << m_VLMType;

		m_WingWOpp.SerializeWOpp(ar, bIsStoring);
		if(m_bBiplane)	m_Wing2WOpp.SerializeWOpp(ar, bIsStoring);
		if(m_bStab)		m_StabWOpp.SerializeWOpp(ar, bIsStoring);
		if(m_bFin)		m_FinWOpp.SerializeWOpp(ar, bIsStoring);
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
		//read variables
		ReadCString(ar, m_PlaneName);
		ReadCString(ar, m_PlrName);

		if(ArchiveFormat>=1005)
		{
			ar >> a;
			if (a!=0 && a!=1)
			{
				return false;
			}
			if(a) m_bBiplane = true; else m_bBiplane = false;
		}

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bStab = true; else m_bStab = false;
		ar >> a;
		if (a!=0 && a!=1) return false;


		if(a) m_bFin = true; else m_bFin = false;
		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bIsVisible = true; else m_bIsVisible = false;
		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bShowPoints = true; else m_bShowPoints = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bOut = true; else m_bOut = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

		if(a) m_bVLM1 = true; else m_bVLM1 = false;

		ar >> a;
		if (a!=0 && a!=1) return false;

//		if(a) m_bMiddle = true; else m_bMiddle = false;

		ar >> m_Style >> m_Width;
		ReadCOLORREF(ar, m_Color);
		ar >> m_Type >> m_NStation;
		ar >> f;		m_Alpha = f;
		ar >> f;        m_QInf  = f;
		ar >> f;        m_Weight = f;

		if(ArchiveFormat>=1007)
		{
			ar>>f; m_Beta = f;
		}
		if(ArchiveFormat<1002)
		{
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
			ar >> f;
		}
		if(ArchiveFormat<1001)
		{
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
			for (k=0; k<=m_NStation; k++)
			{
				ar >> f;
			}
		}
		if(ArchiveFormat>=1002)
		{
			ar >>m_NPanels;

			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
				m_Cp[k] = f;
			}
		}
		if(ArchiveFormat>=1003)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
				if(ArchiveFormat<1004)	m_G[k] = f/1000.0;
				else 					m_G[k] = f;
			}
		}

		if(ArchiveFormat>=1006)
		{
			for (k=0; k<=m_NPanels; k++)
			{
				ar >> f;
				m_Sigma[k] = f;
			}
		}

		ar >> m_VLMType;

		if (!m_WingWOpp.SerializeWOpp(ar, bIsStoring))
		{
			return false;
		}

		if(ArchiveFormat>=1005)
		{
			if(m_bBiplane)
			{
				if (!m_Wing2WOpp.SerializeWOpp(ar, bIsStoring))
				{
					return false;
				}
			}
		}
		if(m_bStab)
		{
			if (!m_StabWOpp.SerializeWOpp(ar, bIsStoring))
				{
					return false;
				}

		}
		if(m_bFin)
		{
			if (!m_FinWOpp.SerializeWOpp(ar, bIsStoring))
			{
				return false;
			}
		}
	}
	return true;
}


