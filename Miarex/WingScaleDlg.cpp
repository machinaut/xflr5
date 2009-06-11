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

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>
#include "WingScaleDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"




WingScaleDlg::WingScaleDlg(void *pParent)
{
	setWindowTitle("Scale Wing Dlg");
	m_pMainFrame = NULL;
	m_bSweep = m_bSpan = m_bChord = m_bTwist = false;

	m_NewSweep = m_NewChord = m_NewTwist = m_NewSpan = false;
	m_RefSweep = m_RefChord = m_RefTwist = m_RefSpan = 1.0;
	SetupLayout();
}


void WingScaleDlg::SetupLayout()
{
	QGridLayout *ScaleLayout = new QGridLayout;
	m_pctrlSpan  = new QCheckBox("Span Scaling");
	m_pctrlChord = new QCheckBox("Chord Scaling");
	m_pctrlSweep = new QCheckBox("Sweep Scaling");
	m_pctrlTwist = new QCheckBox("Twist Scaling");

	m_pctrlNewSpan  = new FloatEdit(0,3);
	m_pctrlNewChord = new FloatEdit(0,3);
	m_pctrlNewTwist = new FloatEdit(0,3);
	m_pctrlNewSweep = new FloatEdit(0,3);

	m_pctrlRefSpan  = new QLabel("0.000");
	m_pctrlRefChord = new QLabel("0.000");
	m_pctrlRefSweep = new QLabel("0.000");
	m_pctrlRefTwist = new QLabel("0.000");
	m_pctrlRefSpan->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlRefChord->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlRefSweep->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlRefTwist->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	m_pctrlSpanRatio  = new QLabel("1.000");
	m_pctrlChordRatio = new QLabel("1.000");
	m_pctrlSweepRatio = new QLabel("1.000");
	m_pctrlTwistRatio = new QLabel("1.000");
	m_pctrlSpanRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlChordRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlSweepRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_pctrlTwistRatio->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	QLabel *lab11 = new QLabel("Reference");
	QLabel *lab12 = new QLabel("New");
	QLabel *lab13 = new QLabel("Ratio");
	lab11->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	lab12->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	lab13->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	ScaleLayout->addWidget(lab11, 1,2);
	ScaleLayout->addWidget(lab12, 1,3);
	ScaleLayout->addWidget(lab13, 1,5);

	ScaleLayout->addWidget(m_pctrlSpan,      2,1);
	ScaleLayout->addWidget(m_pctrlRefSpan,   2,2);
	ScaleLayout->addWidget(m_pctrlNewSpan,   2,3);
	ScaleLayout->addWidget(m_pctrlSpanRatio, 2,5);

	ScaleLayout->addWidget(m_pctrlChord,     3,1);
	ScaleLayout->addWidget(m_pctrlRefChord,  3,2);
	ScaleLayout->addWidget(m_pctrlNewChord,  3,3);
	ScaleLayout->addWidget(m_pctrlChordRatio,3,5);

	ScaleLayout->addWidget(m_pctrlSweep,     4,1);
	ScaleLayout->addWidget(m_pctrlRefSweep,  4,2);
	ScaleLayout->addWidget(m_pctrlNewSweep,  4,3);
	ScaleLayout->addWidget(m_pctrlSweepRatio,4,5);

	ScaleLayout->addWidget(m_pctrlTwist,     5,1);
	ScaleLayout->addWidget(m_pctrlRefTwist,  5,2);
	ScaleLayout->addWidget(m_pctrlNewTwist,  5,3);
	ScaleLayout->addWidget(m_pctrlTwistRatio,5,5);

	m_pctrlUnit20 = new QLabel;
	m_pctrlUnit21 = new QLabel;
	ScaleLayout->addWidget(m_pctrlUnit20, 2,4);
	ScaleLayout->addWidget(m_pctrlUnit21, 3,4);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	QHBoxLayout *StretchLayout = new QHBoxLayout;
	StretchLayout->addStretch(1);
	StretchLayout->addLayout(ScaleLayout);
	StretchLayout->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addLayout(StretchLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);

	connect(OKButton, SIGNAL(clicked()), this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(m_pctrlSpan,  SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlChord, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlSweep, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));
	connect(m_pctrlTwist, SIGNAL(clicked()), this, SLOT(OnClickedCheckBox()));

	connect(m_pctrlNewSpan,  SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewChord, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewSweep, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
	connect(m_pctrlNewTwist, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
}


void WingScaleDlg::InitDialog(double const &RefSpan, double const &RefChord, double const &RefSweep, double const &RefTwist)
{
	QString len;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	GetLengthUnit(len, pMainFrame->m_LengthUnit);
	m_pctrlUnit20->setText(len);
	m_pctrlUnit21->setText(len);

	m_RefSpan  = RefSpan;
	m_RefChord = RefChord;
	m_RefSweep = RefSweep;
	m_RefTwist = RefTwist; 

	m_NewSpan  = RefSpan;
	m_NewChord = RefChord;
	m_NewSweep = RefSweep;
	m_NewTwist = RefTwist; 

	m_pctrlSpan->setChecked(m_bSpan);//(false)
	m_pctrlChord->setChecked(m_bChord);
	m_pctrlTwist->setChecked(m_bTwist);
	m_pctrlSweep->setChecked(m_bSweep);

	QString strong;

	strong = QString("%1").arg(m_RefSpan * pMainFrame->m_mtoUnit,8,'f',3);
	m_pctrlRefSpan->setText(strong);

	strong = QString("%1").arg(m_RefChord * pMainFrame->m_mtoUnit,8,'f',3);
	m_pctrlRefChord->setText(strong);

	strong = QString("%1").arg(m_RefSweep,8,'f',2);
	m_pctrlRefSweep->setText(strong);

	strong = QString("%1").arg(m_RefTwist,8,'f',2);
	m_pctrlRefTwist->setText(strong);

	m_pctrlNewSpan->SetValue(m_NewSpan * pMainFrame->m_mtoUnit);
	m_pctrlNewChord->SetValue(m_NewChord * pMainFrame->m_mtoUnit);
	m_pctrlNewSweep->SetValue(m_NewSweep);
	m_pctrlNewTwist->SetValue(m_NewTwist);

	SetResults();
	EnableControls();
}



void WingScaleDlg::OnClickedCheckBox()
{
	ReadData();
	EnableControls();
}


void WingScaleDlg::OnOK()
{
	ReadData();
	accept();
}

void WingScaleDlg::OnEditingFinished()
{
	ReadData();
	SetResults();
}



void WingScaleDlg::EnableControls()
{
	m_pctrlNewSpan->setEnabled(m_bSpan);
	m_pctrlNewChord->setEnabled(m_bChord);
	m_pctrlNewSweep->setEnabled(m_bSweep);
	m_pctrlNewTwist->setEnabled(m_bTwist);
}


void WingScaleDlg::ReadData()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	m_bSpan  = m_pctrlSpan->isChecked();
	m_bChord = m_pctrlChord->isChecked();
	m_bSweep = m_pctrlSweep->isChecked();
	m_bTwist = m_pctrlTwist->isChecked();

	m_NewSpan  = m_pctrlNewSpan->GetValue()  / pMainFrame->m_mtoUnit;
	m_NewChord = m_pctrlNewChord->GetValue() / pMainFrame->m_mtoUnit;
	m_NewSweep = m_pctrlNewSweep->GetValue();
	m_NewTwist = m_pctrlNewTwist->GetValue();
}


void WingScaleDlg::SetResults()
{
	QString strong;

	if(m_RefSpan>0.0)  strong = QString("%1").arg(m_NewSpan/m_RefSpan, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlSpanRatio->setText(strong);

	if(m_RefChord>0.0) strong = QString("%1").arg(m_NewChord/m_RefChord, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlChordRatio->setText(strong);

	if(m_RefSweep>0.0) strong = QString("%1").arg(m_NewSweep/m_RefSweep, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlSweepRatio->setText(strong);

	if(m_RefTwist>0.0) strong = QString("%1").arg(m_NewTwist/m_RefTwist, 6,'f',3);
	else               strong =" 1.000";
	m_pctrlTwistRatio->setText(strong);

}





