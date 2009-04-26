/****************************************************************************

	FoilSelectionDlg Classes
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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "../Objects/Foil.h"
#include "FoilSelectionDlg.h"

FoilSelectionDlg::FoilSelectionDlg()
{
	m_poaFoil = NULL;
	m_FoilName = "";
	SetupLayout();
}




void FoilSelectionDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlNameList = new QListWidget;
	m_pctrlNameList->setMinimumHeight(300);
	connect(m_pctrlNameList, SIGNAL(itemClicked(QListWidgetItem *)),       this, SLOT(OnSelChangeList(QListWidgetItem *)));
	connect(m_pctrlNameList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnDoubleClickList(QListWidgetItem *)));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}

void FoilSelectionDlg::OnOK()
{
	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();
	m_FoilName = pItem->text();
	accept();
}

void FoilSelectionDlg::OnSelChangeList(QListWidgetItem *pItem)
{
	m_FoilName = pItem->text();
}


void FoilSelectionDlg::OnDoubleClickList(QListWidgetItem *pItem)
{
	OnOK();
}

void FoilSelectionDlg::InitDialog()
{
	if(!m_poaFoil) return;
	CFoil *pFoil;
	for (int i=0; i<m_poaFoil->size(); i++)
	{
		pFoil = (CFoil*)m_poaFoil->at(i);
		m_pctrlNameList->addItem(pFoil->m_FoilName);
	}
}
