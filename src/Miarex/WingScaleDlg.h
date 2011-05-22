/****************************************************************************

	WingScaleDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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

#ifndef WINGSCALEDLG_H
#define WINGSCALEDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "../Misc/FloatEdit.h"


class WingScaleDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;
public:
	WingScaleDlg(void *pParent = NULL);
	void InitDialog(double const &RefSpan, double const &RefChord, double const &RefSweep, double const &RefTwist);
private:
	void SetupLayout();
	void ReadData();
	void SetResults();
	void EnableControls();

private slots:
	void OnOK();
	void OnClickedCheckBox();
	void OnEditingFinished();


public:

	QCheckBox *m_pctrlSpan, *m_pctrlChord, *m_pctrlSweep, *m_pctrlTwist;
	FloatEdit  *m_pctrlNewSpan, *m_pctrlNewChord, *m_pctrlNewSweep, *m_pctrlNewTwist;
	QLabel *m_pctrlRefSpan, *m_pctrlRefChord, *m_pctrlRefSweep, *m_pctrlRefTwist;
	QLabel *m_pctrlSpanRatio, *m_pctrlChordRatio, *m_pctrlSweepRatio, *m_pctrlTwistRatio;
	QLabel *m_pctrlUnit20, *m_pctrlUnit21;
	static void *s_pMainFrame;

	bool m_bSweep, m_bSpan, m_bChord, m_bTwist;
	double m_NewSweep, m_NewChord, m_NewTwist, m_NewSpan;
	double m_RefSweep, m_RefChord, m_RefTwist, m_RefSpan;
};

#endif // WINGSCALEDLG_H
