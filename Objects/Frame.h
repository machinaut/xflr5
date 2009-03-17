/****************************************************************************

    CFrame Class
	Copyright (C) 2007 Andre Deperrois xflr5@yahoo.com

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
 
#ifndef CFRAME_H
#define CFRAME_H

#include "Spline.h"
#include "CRectangle.h"

class CFrame : public QObject
{
public:
	CFrame();
    bool SerializeFrame(QDataStream &ar, bool bIsStoring);
	int IsPoint(CVector Point, double ZoomFactor);
	int InsertPoint(CVector Real);
	void RemoveSelectedPoint();
	void RemovePoint(int n);
	void InsertPoint(int n);
	void CopyFrame(CFrame *pFrame);

	int m_iHighlight;
	int m_iSelect;
	int m_NPoints;			// the number of points defining the frame
	CVector m_Point[MAXSIDELINES];	// the points' positions
	CSpline m_Spline;

	static QRect s_rViewRect;
};


#endif

