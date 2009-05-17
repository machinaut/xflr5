/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 Andre Deperrois xflr5@yahoo.com

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

#include "../Globals.h"
#include "Spline.h"
#include "math.h"
#include <QtDebug>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CSpline::CSpline()
{
	m_Style = 0;
	m_Width = 1;
	m_Color = QColor(70, 200, 120);

	m_iHighlight  = -10;
	m_iSelect     = -10;
	m_iCtrlPoints =  0;
	m_iKnots      =  0;
	m_iDegree     =  3;
	m_iRes        = 50;
        m_rViewRect.setRect(0,0,0,0);

	memset(m_knots, 0, sizeof(m_knots));
	memset(m_Input, 0, sizeof(m_Input));
	memset(m_Output, 0, sizeof(m_Output));
}


CSpline::~CSpline()
{
}

/*
bool CSpline::AddPoint(double x, double y)
{
	if(m_iCtrlPoints>=SPLINECONTROLSIZE) 
	{
		return false;
	}
	m_Input[m_iCtrlPoints].x = x;
	m_Input[m_iCtrlPoints].y = y;
	m_iCtrlPoints++;
	return true;
}*/


void CSpline::Copy(CSpline *pSpline)
{
	m_iCtrlPoints = pSpline->m_iCtrlPoints;
	m_iDegree     = pSpline->m_iDegree;
	m_iHighlight  = pSpline->m_iHighlight;
	m_iKnots      = pSpline->m_iKnots;
	m_iRes        = pSpline->m_iRes;
	m_iSelect     = pSpline->m_iSelect;

	memcpy(m_Input, &pSpline->m_Input, sizeof(m_Input));
	memcpy(m_Output, &pSpline->m_Output, sizeof(m_Output));
	memcpy(m_knots, &pSpline->m_knots, sizeof(m_knots));
	m_rViewRect.setCoords(pSpline->m_rViewRect.left(),  pSpline->m_rViewRect.top(),pSpline->m_rViewRect.right(),  pSpline->m_rViewRect.bottom());
}



void CSpline::DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint &Offset)
{
	painter.save();

	QPoint pt;
	int i, width;

	scaley = -scaley;
	width  = 3;

	QPen PointPen;
	PointPen.setWidth(1);

	painter.setPen(PointPen);
	for (i=0; i<m_iCtrlPoints; i++)
	{
		pt.rx() = (int)(m_Input[i].x*scalex + Offset.x());
		pt.ry() = (int)(m_Input[i].y*scaley + Offset.y());

//		if(m_rViewRect.contains(pt))
//		{
			if (m_iSelect==i) 
			{
				PointPen.setWidth(2);
				PointPen.setColor(QColor(0,0,150));
			}
			else if(m_iHighlight==i) 
			{
				PointPen.setWidth(2);
				PointPen.setColor(QColor(255,0,0));
			}
			else
			{
				PointPen.setWidth(1);
				PointPen.setColor(m_Color);
			}
			painter.setPen(PointPen);
			painter.drawEllipse(pt.x()-width, pt.y()-width, 2*width, 2*width);
//		}
	}
	painter.restore();
}



void CSpline::DrawOutputPoints(QPainter & painter, double scalex, double scaley, QPoint &Offset)
{
	painter.save();
	QPoint pt;
	int width;
	width = 2;

	QPen OutPen(m_Color);
	OutPen.setStyle(Qt::SolidLine);
	OutPen.setWidth(1);
	painter.setPen(OutPen);

	for (int i=0; i<m_iRes;i++)
	{
		pt.rx() = (int)( m_Output[i].x*scalex + Offset.x());
		pt.ry() = (int)(-m_Output[i].y*scaley + Offset.y());
		if(!m_rViewRect.contains(pt)) return;

		painter.drawRect(pt.x()-width, pt.y()-width, 2*width, 2*width);
	}

	painter.restore();
}


void CSpline::DrawSpline(QPainter & painter, double scalex, double scaley, QPoint &Offset)
{
	painter.save();
	scaley = -scaley;
	QPoint From, To;

	QPen SplinePen(m_Color);
	SplinePen.setStyle(GetStyle(m_Style));
	SplinePen.setWidth(m_Width);
	painter.setPen(SplinePen);

	int k;

	if(m_iCtrlPoints>0)
	{ 
		From.rx() = (int)(m_Output[0].x * scalex + Offset.x());
		From.ry() = (int)(m_Output[0].y * scaley + Offset.y());

		for(k=1; k<m_iRes;k++) 
		{
			To.rx() = (int)(m_Output[k].x * scalex + Offset.x());
			To.ry() = (int)(m_Output[k].y * scaley + Offset.y());

			painter.drawLine(From, To);

			From = To;
		}
	}
	painter.restore();
}


void CSpline::Export(QTextStream &out, bool bExtrados)
{
	int k;
	QString strOut;

	if(bExtrados)
	{
		for (k=m_iRes-1;k>=0; k--)
		{
			strOut= QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
	else
	{
		for (k=1;k<m_iRes; k++)
		{
			strOut=QString(" %1  %2\n").arg(m_Output[k].x,7,'f',4).arg( m_Output[k].y,7,'f',4);
			out << strOut;
		}
	}
}


double CSpline::GetY(double x)
{
        int i;
	if(x<=0.0 || x>=1.0) return 0.0;
	double y;

        for (i=0; i<m_iRes-1; i++)
        {
		if (m_Output[i].x <m_Output[i+1].x  && 
                        m_Output[i].x <= x && x<=m_Output[i+1].x )
                {
			y = (m_Output[i].y 	+ (m_Output[i+1].y-m_Output[i].y)
								 /(m_Output[i+1].x-m_Output[i].x)*(x-m_Output[i].x));
			return y;
		}
	}
	return 0.0; //we'll have to do better...
}



bool CSpline::InsertPoint(double x, double y)
{
	int j,k;
	if(m_iCtrlPoints>=SPLINECONTROLSIZE) return false;
	if (x>=0.0 && x<=1.0)
	{ 
		//No points yet
		if(m_iCtrlPoints<=0)
		{
			m_iCtrlPoints++;;
			m_Input[0].x = x;
			m_Input[0].y = y;
		}
		else 
		{
			if(x<m_Input[0].x)
			{// if we're the new minimum point
				for (j=m_iCtrlPoints; j>=0; j--)
				{
					m_Input[j] = m_Input[j-1];
				}
				m_Input[0].x = x;
				m_Input[0].y = y;
				m_iCtrlPoints++;
			}
			else{// if we're the new maximum point
				if(x>=m_Input[m_iCtrlPoints-1].x)
				{
					m_Input[m_iCtrlPoints].x = x;
					m_Input[m_iCtrlPoints].y = y;
					m_iCtrlPoints++;
				}
				else{// else if we're in between
					for (k=0; k<m_iCtrlPoints; k++)
					{
						if (x>m_Input[k].x && x<m_Input[k+1].x)
						{
							for (int j=m_iCtrlPoints; j>k+1; j--)
							{
								m_Input[j] = m_Input[j-1];
							}
							m_Input[k+1].x = x;
							m_Input[k+1].y = y;
							m_iCtrlPoints++;
							break;
						}
					}
				}
			}
		}
	}
	SplineKnots();
	return true;
}


int CSpline::IsControlPoint(CVector Real)
{
	for (int k=0; k<m_iCtrlPoints; k++)
	{
                if(fabs(Real.x-m_Input[k].x)<0.005 && fabs(Real.y-m_Input[k].y)<0.005) return k;
	}
	return -10;
}

int CSpline::IsControlPoint(CVector Real, double ZoomFactor)
{
	for (int k=0; k<m_iCtrlPoints; k++)
	{
                if (fabs(Real.x-m_Input[k].x)<0.006/ZoomFactor && fabs(Real.y-m_Input[k].y)<0.006/ZoomFactor) return k;
	}
	return -10;
}


int CSpline::IsControlPoint(double x, double y, double zx, double zy)
{
	for (int k=0; k<m_iCtrlPoints; k++)
	{
                if(fabs((x-m_Input[k].x)/zx)<4.0 && fabs((y-m_Input[k].y)/zy)<4.0) return k;
	}
	return -10;
}


bool CSpline::RemovePoint(int k)
{
	if (k>0 && k<m_iCtrlPoints){
		for (int j=k; j<m_iCtrlPoints; j++){
			m_Input[j] = m_Input[j+1];
		}
		m_iCtrlPoints--;
		SplineKnots();
		SplineCurve();
	}
	return true;
}


void CSpline::SetStyle(int style)
{
	m_Style = style;
}



void CSpline::SetWidth(int width)
{
	m_Width = width;
}



void CSpline::SetColor(QColor color)
{
	m_Color = color;
}



void CSpline::SetSplineParams(int style, int width, QColor color)
{
	m_Width = width;
	m_Style = style;
	m_Color = color;
}





double CSpline::SplineBlend(int i,  int p, double t)
{
/*	Calculate the blending value, this is done recursively.
	If the numerator and denominator are 0 the expression is 0.
	If the denominator is 0 the expression is 0 */
//
//	   i   is the control point's index
//	   p   is the spline's degree 	
//	   t   is the spline parameter
//

	double value;
	double pres = 1.e-10;

	if (p == 0) 
	{
		if ((m_knots[i] <= t) && (t < m_knots[i+1]) )	value = 1.0;
//		else if (fabs(m_knots[i]-m_knots[i+1])<pres)	    value = 0.0;
		else 						                    value = 0.0;
	} 
	else
	{
                if (fabs(m_knots[i+p] - m_knots[i])<pres && fabs(m_knots[i+p+1] - m_knots[i+1])<pres)
			value = 0.0;
                else if (fabs(m_knots[i+p] - m_knots[i])<pres)
			value = (m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1, p-1, t);
                else if (fabs(m_knots[i+p+1]-m_knots[i+1])<pres)
			value = (t - m_knots[i])   / (m_knots[i+p] - m_knots[i])   * SplineBlend(i,   p-1, t);
		else 
			value = (t - m_knots[i])   / (m_knots[i+p]-m_knots[i])	   * SplineBlend(i,   p-1, t) + 
			        (m_knots[i+p+1]-t) / (m_knots[i+p+1]-m_knots[i+1]) * SplineBlend(i+1 ,p-1, t);
	}
	return value;
}



void CSpline::SplineCurve()
{
	double t, increment;
	double b;
	int i,j;

	if (m_iCtrlPoints>0)
	{
		t = 0;
		increment = 1.0/(double)(m_iRes - 1);
		
		for (j=0;j<m_iRes;j++)
		{
			m_Output[j].x = 0;
			m_Output[j].y = 0;

			for (i=0; i<m_iCtrlPoints; i++)
			{
				b = SplineBlend(i, m_iDegree, t);
				m_Output[j].x += m_Input[i].x * b;
				m_Output[j].y += m_Input[i].y * b;
			}
			t += increment;
		}
		m_Output[m_iRes-1] = m_Input[m_iCtrlPoints-1];
	}
}


void CSpline::SplineKnots()
{
	double a,b;
	int j;
		int iDegree = qMin(m_iDegree, m_iCtrlPoints);

	m_iKnots  = iDegree + m_iCtrlPoints + 1;
	for (j=0; j<m_iKnots; j++) 
	{
		if (j<iDegree+1)  m_knots[j] = 0.0;
		else 
		{
			if (j<m_iCtrlPoints) 
			{
				a = (double)(j-iDegree);
				b = (double)(m_iKnots-2*iDegree-1);
                                if(fabs(b)>0.0) m_knots[j] = a/b;
				else           m_knots[j] = 1.0; // added arcds
			}
			else m_knots[j] = 1.0;	
		}
	}
}





double Bernstein(int i, int n, double u)
{
	int k;
        double pui, pu1i1;

	int fi  = 1;
	int fni = 1;

	for(k=2; k<=i; k++)   fi  *=k;
	for(k=i+1; k<=n; k++) fni *=k;

	pui   = 1.0;
	for(k=0; k<i;   k++) pui*=u;

	pu1i1 = 1.0;
	for(k=0; k<n-i; k++) pu1i1*=1.0-u;

	return pui * pu1i1 * (double)fni/(double)fi;
}


double BezierBlend(int k,int n,double u)
{
	int nn,kn,nkn;
	double blend=1.0;

	nn = n;
	kn = k;
	nkn = n - k;

	while (nn >= 1) 
	{
		blend *= nn;
		nn--;
		if (kn > 1)
		{
			blend /= (double)kn;
			kn--;
		}
		if (nkn > 1)
		{
			blend /= (double)nkn;
			nkn--;
		}
	}

	if (k > 0)   for(kn=0; kn<k;   kn++) blend *= u;
	if (n-k > 0) for(kn=0; kn<n-k; kn++) blend *= 1.0-u;

	return(blend);
}


