/****************************************************************************

	QXInverse Class
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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


#ifndef QXINVERSE_H
#define QXINVERSE_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include "../Misc/FloatEdit.h"
#include "../XDirect/XFoil.h"
#include "XInverse.h"
#include "../Graph/QGraph.h"
#include "../Objects/Spline.h"

class QXInverse : public QWidget
{
	Q_OBJECT

	friend class MainFrame;
	friend class TwoDWidget;

public:
	QXInverse(QWidget *parent = NULL);
	void SetupLayout();
	void InitDialog();
 
private slots:
	void OnGraphOptions();
	void OnTangentSpline();
	void OnShowSpline();

private:
	void UpdateView();

	void wheelEvent(QWheelEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event) ;
	void mouseReleaseEvent(QMouseEvent *event) ;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void PaintView(QPainter &painter);
	void PaintGraph(QPainter &painter);
	void PaintFoil(QPainter &painter);
	void SetScale(QRect CltRect);
	void ReleaseZoom();
	void Smooth(int Pos1 = -1, int Pos2 = -1);
	void CreateMCurve();
	void CreateQCurve();
	void CancelSpline();
	void CancelSmooth();

private:
	QTextEdit *m_pctrlOutput;
	QLabel *m_pctrlSpecif;
	QRadioButton *m_pctrlSpecAlpha, *m_pctrlSpecCl;
	QPushButton *m_pctrlExec, *m_pctrlFilter, *m_pctrlPert, *m_pctrlApplySpline, *m_pctrlNewSpline, *m_pctrlResetQSpec, *m_pctrlSmooth;
	QCheckBox *m_pctrlShowSpline, *m_pctrlTangentSpline, *m_pctrlSymm;
	FloatEdit *m_pctrlSpec, *m_pctrlFilterParam, *m_pctrlTGapy, *m_pctrlTGapx, *m_pctrlTAngle;

	QTextEdit *m_pctrlMSpec;
	FloatEdit *m_pctrlIter;
	QPushButton	*m_pctrlMark;
	QCheckBox *m_pctrlCpxx;

	QVBoxLayout *m_pctrlMInvLayout,*m_pctrlFInvLayout;

	void * m_p2DWidget;
	void * m_pMainFrame;

	void *m_pXFoil;

	QList<void *> *m_poaFoil;

	CFoil* m_pCurFoil;
	CFoil* m_pRefFoil;
	CFoil* m_pModFoil;

	CSpline m_Spline;

	QCursor m_hcArrow;
	QCursor m_hcMove;
	QCursor m_hcCross;


	bool m_bLoaded;
	bool m_bTrans;
	bool m_bModFoil, m_bRefFoil;
	bool m_bGetPos;
	bool m_bMark, m_bMarked;
	bool m_bSpline, m_bSplined;
	bool m_bSmooth;
	bool m_bZoomPlus;
	bool m_bFullInverse;// mixed inverse if false
	bool m_bSaved;
	bool m_bReflected;
	bool m_bShowPoints;
	bool m_bTransGraph;
	bool m_bRefCurves;
	bool m_bTangentSpline;


	int m_Mk1, m_Mk2;
	int m_tmpPos, m_Pos1, m_Pos2, m_nPos, m_SplineLeftPos, m_SplineRightPos;
	int m_LogPixelsY;
	int m_SplineStyle, m_SplineWidth;
	double xd, yd;
	double xu, yu;
	double m_fScale, m_fRefScale;

	int m_ReflectedStyle, m_ReflectedWidth;
	QColor m_ReflectedClr;

	QGraph m_QGraph;
	CCurve* m_pQCurve;
	CCurve* m_pQVCurve;
	CCurve* m_pMCurve;
	CCurve* m_pReflectedCurve;


	QColor m_SplineClr;

	QRect m_rCltRect, m_rGraphRect;
	QRect m_ZoomRect;

	QPoint m_PointDown;
	QPoint m_ptPopUp;
	QPoint m_ptOffset;

// temporary allocations
	QPoint tanpt;
	QPoint P0, P1, P2;

};

#endif // QXINVERSE_H



