/****************************************************************************

    CWAdvDlg Class
	Copyright (C) 2005 Andr� Deperrois xflr5@yahoo.com

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

// WAdvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "WAdvDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CWAdvDlg dialog


CWAdvDlg::CWAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWAdvDlg)
	m_BLogFile = FALSE;
	//}}AFX_DATA_INIT
	m_NStation  = 20;
	m_AlphaPrec = 0.01;
	m_Relax     = 20.;
	m_Iter      = 100;

	m_MaxWakeIter     = 1;
	m_WakePanels      = 5;
	m_FirstPanelSize  = 0.2;//% mac
	m_WakePanelFactor = 1.02;
	m_CoreSize        = 0.0;
	m_WakeInterNodes  = 6;
	m_MinPanelSize    = 1.0;
	m_bResetWake      = true;
	m_BLogFile        = true;

	m_pMiarex = NULL;
	m_pFrame  = NULL;
	m_ControlPos = 0.75;
	m_VortexPos  = 0.25;
}


void CWAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWAdvDlg)
	DDX_Control(pDX, IDC_LOGFILE, m_ctrlLogFile);
	DDX_Control(pDX, IDC_INTERNODES, m_ctrlInterNodes);
	DDX_Control(pDX, IDC_RELAX, m_ctrlRelax);
	DDX_Control(pDX, IDC_ALPHAPREC, m_ctrlAlphaPrec);
	DDX_Control(pDX, IDC_NSTAT, m_ctrlNStation);
	DDX_Control(pDX, IDC_ITERMAX, m_ctrlIterMax);
	DDX_Control(pDX, IDC_MAXWAKEITER, m_ctrlMaxWakeIter);
	DDX_Control(pDX, IDC_WAKEPANELS, m_ctrlWakePanels);
	DDX_Control(pDX, IDC_FIRSTPANELSIZE, m_ctrlFirstPanelSize);
	DDX_Control(pDX, IDC_PANELFACTOR, m_ctrlPanelFactor);
	DDX_Control(pDX, IDC_CORESIZE, m_ctrlCoreSize);
	DDX_Control(pDX, IDC_ASTAT2, m_ctrlAStat);
	DDX_Control(pDX, IDC_MINPANELSIZE, m_ctrlMinPanelSize);
	DDX_Control(pDX, IDC_RESETWAKE, m_ctrlResetWake);
	DDX_Control(pDX, IDC_MINPANELSIZE, m_ctrlMinPanelSize);
	DDX_Control(pDX, IDC_LENGTH, m_ctrlLength);
	DDX_Control(pDX, IDC_VORTEXPOS, m_ctrlVortexPos);
	DDX_Control(pDX, IDC_CTRLPOS, m_ctrlControlPos);
	DDX_Check(pDX, IDC_LOGFILE, m_BLogFile);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDOK, m_ctrlOK);
}
 

BEGIN_MESSAGE_MAP(CWAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CWAdvDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RESETWAKE, OnResetWake)
	ON_BN_CLICKED(IDC_RESET, OnResetDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWAdvDlg message handlers

BOOL CWAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_SymbolFont.CreatePointFont(100, "Symbol");
	m_ctrlAStat.SetFont(&m_SymbolFont);

	m_ctrlRelax.SetPrecision(1);
	m_ctrlRelax.SetMin(1.0);

	m_ctrlAlphaPrec.SetPrecision(4);

	m_ctrlIterMax.SetMin(0);

	m_ctrlNStation.SetMin(0);
	m_ctrlNStation.SetMax(MAXSTATIONS);

	
	m_ctrlWakePanels.SetMin(0);
	m_ctrlWakePanels.SetMax(100000);

	m_ctrlMaxWakeIter.SetMin(0);
	m_ctrlMaxWakeIter.SetMax(100000);

	m_ctrlPanelFactor.SetMin(0.0);
	m_ctrlPanelFactor.SetMax(1000.0);
	m_ctrlCoreSize.SetMin(0.0);
	m_ctrlCoreSize.SetMax(1000000.0);

	m_ctrlFirstPanelSize.SetMin(0.0);
	m_ctrlFirstPanelSize.SetMax(1000.0);
	m_ctrlFirstPanelSize.SetPrecision(2);

/*	m_ctrlInterNodes.EnableWindow(false);
	m_ctrlPanelFactor.EnableWindow(false);
	m_ctrlResetWake.EnableWindow(false);
	m_ctrlWakePanels.EnableWindow(false);
	m_ctrlMaxWakeIter.EnableWindow(false);
	m_ctrlPanelFactor.EnableWindow(false);
	m_ctrlCoreSize.EnableWindow(false);
	m_ctrlFirstPanelSize.EnableWindow(false);*/

	CString len;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	GetLengthUnit(len,pFrame->m_LengthUnit);
	m_ctrlLength.SetWindowText(len);

	SetParams();
	m_ctrlRelax.SetFocus();

	return FALSE;
}

void CWAdvDlg::ReadParams()
{
	m_Iter      = __max(m_ctrlIterMax.GetValue(),1);
	m_Relax     = __max(m_ctrlRelax.GetValue(), 1.0);
	m_AlphaPrec = __max(m_ctrlAlphaPrec.GetValue(),0.0001);
	m_NStation  = __max(m_ctrlNStation.GetValue(),2);
	if(!IsEven(m_NStation)){
		m_NStation++;
	}
	if(m_NStation>MAXSTATIONS) m_NStation=MAXSTATIONS;

	m_WakePanels      = m_ctrlWakePanels.GetValue();
	m_FirstPanelSize  = m_ctrlFirstPanelSize.GetValue();
	m_WakePanelFactor = m_ctrlPanelFactor.GetValue();
	m_CoreSize        = m_ctrlCoreSize.GetValue();
	m_MaxWakeIter     = m_ctrlMaxWakeIter.GetValue();
	m_WakeInterNodes  = m_ctrlInterNodes.GetValue();

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_MinPanelSize    = m_ctrlMinPanelSize.GetValue()/ pFrame->m_mtoUnit;

	m_VortexPos  = m_ctrlVortexPos.GetValue()/100.0;
	m_ControlPos = m_ctrlControlPos.GetValue()/100.0;
}

void CWAdvDlg::SetParams()
{
	m_ctrlIterMax.SetValue(m_Iter);
	m_ctrlRelax.SetValue(m_Relax);
	m_ctrlAlphaPrec.SetValue(m_AlphaPrec);
	m_ctrlNStation.SetValue(m_NStation);

	m_ctrlWakePanels.SetValue(m_WakePanels);
	m_ctrlFirstPanelSize.SetValue(m_FirstPanelSize);
	m_ctrlPanelFactor.SetValue(m_WakePanelFactor);
	m_ctrlCoreSize.SetValue(m_CoreSize);
	m_ctrlMaxWakeIter.SetValue(m_MaxWakeIter);
	m_ctrlInterNodes.SetValue(m_WakeInterNodes);

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_ctrlMinPanelSize.SetValue(m_MinPanelSize * pFrame->m_mtoUnit);

	if(m_bResetWake) m_ctrlResetWake.SetCheck(TRUE);
	else             m_ctrlResetWake.SetCheck(FALSE);

	if(m_BLogFile) m_ctrlLogFile.SetCheck(TRUE);
	else           m_ctrlLogFile.SetCheck(FALSE);

	m_ctrlControlPos.SetValue(m_ControlPos*100.0);
	m_ctrlVortexPos.SetValue(m_VortexPos*100.0);
}

void CWAdvDlg::OnResetWake() 
{
	if(m_ctrlResetWake.GetCheck()) m_bResetWake = true;
	else                           m_bResetWake = false;
}

void CWAdvDlg::OnResetDefaults() 
{
	m_Relax           = 20.0;
	m_AlphaPrec       = 0.01;
	m_Iter            = 100;
	m_NStation        = 20;
	m_MaxWakeIter     = 5;
	m_WakePanels      = 5;
	m_FirstPanelSize  = 0.2;//% mac
	m_WakePanelFactor = 1.02;
	m_CoreSize        = 1.0;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_BLogFile        = TRUE;
	m_VortexPos       = 0.25;
	m_ControlPos      = 0.75;
	SetParams();
}

void CWAdvDlg::OnClose() 
{
	ReadParams();		
	CDialog::OnClose();
}

void CWAdvDlg::OnOK() 
{
	ReadParams();
	CDialog::OnOK();
}



BOOL CWAdvDlg::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDOK) != ppp) {
			ReadParams();
			m_ctrlOK.SetFocus();
			return true;
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}
