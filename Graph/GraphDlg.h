/****************************************************************************

	GraphDlg  Classes
	Copyright (C) 2008 Andre Deperrois xflr5@yahoo.com

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

#ifndef GRAPHDLG_H
#define GRAPHDLG_H

#include <QDialog>
#include <QtGui>
#include "../Graph/Graph.h"
#include "../Misc/FloatEdit.h"
#include "../Misc/ColorButton.h"
#include "../Misc/LineButton.h"

class GraphDlg : public QDialog
{
	Q_OBJECT
	friend class Graph;

private:
	void SetLayout();

public:
	GraphDlg();
	void SetParams();

private slots:
	void OnOK();
	void OnTitleFont();
	void OnLegendFont();
	void OnLabelFont();
	void OnTitleColor();
	void OnLegendColor();
	void OnLabelColor();
	void OnRestoreParams();
	void OnAxisStyle();
	void OnXMajGridStyle();
	void OnXMinGridStyle();
	void OnYMajGridStyle();
	void OnYMinGridStyle();
	void OnXMajGridShow(int state);
	void OnYMajGridShow(int state);
	void OnXMinGridShow(int state);
	void OnYMinGridShow(int state);
	void OnGraphBorder(int state);
	void OnGraphBackColor();
	void OnBorderStyle();

public:
	QWidget *ScalePage, *FontPage, *BackPage, *GridPage;

	QPushButton *OKButton,*CancelButton, *RestoreButton;
	QPushButton *m_pctrlTitleButton, * m_pctrlLegendButton, *m_pctrlLabelButton;
	ColorButton *m_pctrlTitleColor, *m_pctrlLabelColor, *m_pctrlLegendColor;
	QLabel *m_pctrlTitleLabel, *m_pctrlLabelLabel, *m_pctrlLegendLabel;

	QCheckBox *m_pctrlXAuto, *m_pctrlYAuto, *m_pctrlYInverted;
	FloatEdit *m_pctrlXMin, *m_pctrlXMax, *m_pctrlXOrigin,*m_pctrlXUnit;
	FloatEdit *m_pctrlYMin, *m_pctrlYMax, *m_pctrlYOrigin,*m_pctrlYUnit;

	QCheckBox *m_pctrlXMajGridShow, *m_pctrlYMajGridShow, *m_pctrlXMinGridShow, *m_pctrlYMinGridShow;
	LineButton *m_pctrlAxisStyle, *m_pctrlXMajGridStyle, *m_pctrlYMajGridStyle, *m_pctrlXMinGridStyle, *m_pctrlYMinGridStyle;
	FloatEdit *m_pctrlXMinorUnit, *m_pctrlYMinorUnit;

	QCheckBox *m_pctrlGraphBorder;
	ColorButton *m_pctrlGraphBack;
	LineButton *m_pctrlBorderStyle;

	Graph *m_pGraph, *m_pMemGraph;
	QFont *m_pTitleFont, *m_pLegendFont, *m_pLabelFont;

private:

};

#endif // GRAPHDLG_H