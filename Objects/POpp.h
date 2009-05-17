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

// POpp.h: interface for the CPOpp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef POPP_H
#define POPP_H


#include "WOpp.h"
#include <QDataStream>

class CPOpp
{
	friend class QMiarex;
	friend class MainFrame;
	friend class CWPolar;

public:
	CPOpp();
	virtual ~CPOpp();
	bool SerializePOpp(QDataStream &ar, bool bIsStoring);

private:
	CWOpp m_WingWOpp;		// the wing's operating point
	CWOpp m_Wing2WOpp;		// the second wing's operating point
	CWOpp m_StabWOpp;		// the elevator's operating point
	CWOpp m_FinWOpp;		// the fin's operating point

	QString m_PlaneName;		// the plane's name to which the POpp belongs
	QString m_PlrName;		// the polar's name to which the POpp belongs
	
	double m_Alpha, m_QInf; 	// angle of attack and speed 
	double m_Beta;
	double  m_Ctrl;			//Control Variable
	double m_Weight;		// the plane's wieght
	double m_Cp[VLMMATSIZE];	// the Cp array
	double m_G[VLMMATSIZE];		// the VLM vortex strengths, or the panel's doublet's strengths
	double m_Sigma[VLMMATSIZE];		// the panel's source strengths
	int m_NStation;			// unused
	int m_NPanels;		// the number of VLM or 3D-panels
	int m_Type;			// analysis type
	int m_VLMType;			// for future use

	bool m_bBiplane; 		// if a biplane
	bool m_bStab, m_bFin;		// if there is an elevator and a fin, respectively
	bool m_bVLM1;			// true if the POpp is the result of a classic VLM analysis
	bool m_bOut;			// true if the point is unconverged (interpolation outside the polar mesh)
	bool m_bIsVisible;
	bool m_bShowPoints;

	int m_Style;
	int m_Width;
	QColor m_Color;

public:
};
#endif
