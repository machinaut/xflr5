/****************************************************************************

    CUnitsDlg Class
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

// UserVarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "UserVarDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CUserVarDlg dialog


CUserVarDlg::CUserVarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserVarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserVarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_XSel = -1;
	m_YSel = -1;
	m_bWing = true;
}

void CUserVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserVarDlg)
	DDX_Control(pDX, IDC_YSEL, m_ctrlYSel);
	DDX_Control(pDX, IDC_XSEL, m_ctrlXSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserVarDlg, CDialog)
	//{{AFX_MSG_MAP(CUserVarDlg)
	ON_LBN_SELCHANGE(IDC_XSEL, OnSelChangeXSel)
	ON_LBN_SELCHANGE(IDC_YSEL, OnSelChangeYSel)
	ON_LBN_DBLCLK(IDC_YSEL, OnDblclkYsel)
	ON_LBN_DBLCLK(IDC_XSEL, OnDblclkXsel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserVarDlg message handlers

BOOL CUserVarDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(m_bWing){
		m_ctrlXSel.AddString("Alpha");						//0
		m_ctrlXSel.AddString("Lift coef.");					//1
		m_ctrlXSel.AddString("Viscous drag coef.");			//2
		m_ctrlXSel.AddString("Induced drag coef.");			//3
		m_ctrlXSel.AddString("Total drag coef.");			//4
		m_ctrlXSel.AddString("Airf. pitching moment coef.");//5
		m_ctrlXSel.AddString("Geom. pitching moment coef.");//6
		m_ctrlXSel.AddString("Pitching moment coef.");		//7
		m_ctrlXSel.AddString("Rolling moment coef.");		//8
		m_ctrlXSel.AddString("Geom. yawing moment coef.");  //9
		m_ctrlXSel.AddString("Induced yawing moment coef.");//10
		m_ctrlXSel.AddString("Glide ratio Cl/Cd");			//11
		m_ctrlXSel.AddString("Power factor Cl^(3/2)/Cd");	//12
		m_ctrlXSel.AddString("1/Rt(Cl)");					//13
		m_ctrlXSel.AddString("Lift");						//14
		m_ctrlXSel.AddString("Drag");						//15
		m_ctrlXSel.AddString("Vx");							//16
		m_ctrlXSel.AddString("Vz");							//17
		m_ctrlXSel.AddString("VInf");						//18
		m_ctrlXSel.AddString("Descent angle atan(Cd/Cl)");	//19
		m_ctrlXSel.AddString("Pitching Moment");			//20
		m_ctrlXSel.AddString("Rolling Moment");				//21
		m_ctrlXSel.AddString("Yawing Moment");				//22
		m_ctrlXSel.AddString("Centre of pressure X-Pos");	//23
		m_ctrlXSel.AddString("Centre of pressure Y-Pos");	//24
		m_ctrlXSel.AddString("Bending moment");				//25
		m_ctrlXSel.AddString("m.g.Vz");						//26
		m_ctrlXSel.AddString("Oswald");						//27
		m_ctrlXSel.AddString("(XCp-Xcg)/MAC");				//28

		m_ctrlYSel.AddString("Alpha");						//0
		m_ctrlYSel.AddString("Lift coef.");					//1
		m_ctrlYSel.AddString("Viscous drag coef.");			//2
		m_ctrlYSel.AddString("Induced drag coef.");			//3
		m_ctrlYSel.AddString("Total drag coef.");			//4
		m_ctrlYSel.AddString("Airf. pitching moment coef.");//5
		m_ctrlYSel.AddString("Geom. pitching moment coef.");//6
		m_ctrlYSel.AddString("Pitching moment coef.");		//7
		m_ctrlYSel.AddString("Rolling moment coef.");		//8
		m_ctrlYSel.AddString("Geom. yawing moment coef.");  //9
		m_ctrlYSel.AddString("Induced yawing moment coef.");//10
		m_ctrlYSel.AddString("Glide ratio Cl/Cd");			//11
		m_ctrlYSel.AddString("Power factor Cl^(3/2)/Cd");	//12
		m_ctrlYSel.AddString("1/Rt(Cl)");					//13
		m_ctrlYSel.AddString("Lift");						//14
		m_ctrlYSel.AddString("Drag");						//15
		m_ctrlYSel.AddString("Vx");							//16
		m_ctrlYSel.AddString("Vz");							//17
		m_ctrlYSel.AddString("VInf");						//18
		m_ctrlYSel.AddString("Descent angle atan(Cd/Cl)");	//19
		m_ctrlYSel.AddString("Pitching Moment");			//20
		m_ctrlYSel.AddString("Rolling Moment");				//21
		m_ctrlYSel.AddString("Yawing Moment");				//22
		m_ctrlYSel.AddString("Centre of pressure X-Pos");	//23
		m_ctrlYSel.AddString("Centre of pressure Y-Pos");	//24
		m_ctrlYSel.AddString("Bending moment");				//25
		m_ctrlYSel.AddString("m.g.Vz");						//26
		m_ctrlYSel.AddString("Oswald");						//27
		m_ctrlYSel.AddString("(XCp-Xcg)/MAC");				//28
	}
	else {
		m_ctrlXSel.AddString("Alpha");
		m_ctrlXSel.AddString("Cl");
		m_ctrlXSel.AddString("Cd");
		m_ctrlXSel.AddString("Cd x 10000");
		m_ctrlXSel.AddString("Cdp");
		m_ctrlXSel.AddString("Cm");
		m_ctrlXSel.AddString("Xtr1");
		m_ctrlXSel.AddString("Xtr2");
		m_ctrlXSel.AddString("HMom");
		m_ctrlXSel.AddString("Cpmin");
		m_ctrlXSel.AddString("Cl/Cd");
		m_ctrlXSel.AddString("|Cl|^(3/2)/Cd");
		m_ctrlXSel.AddString("1/Cl^1/2");
		m_ctrlXSel.AddString("Re");
		m_ctrlXSel.AddString("XCp");

		m_ctrlYSel.AddString("Alpha");
		m_ctrlYSel.AddString("Cl");
		m_ctrlYSel.AddString("Cd");
		m_ctrlYSel.AddString("Cd x 10000");
		m_ctrlYSel.AddString("Cdp");
		m_ctrlYSel.AddString("Cm");
		m_ctrlYSel.AddString("Xtr1");
		m_ctrlYSel.AddString("Xtr2");
		m_ctrlYSel.AddString("HMom");
		m_ctrlYSel.AddString("Cpmin");
		m_ctrlYSel.AddString("Cl/Cd");
		m_ctrlYSel.AddString("|Cl|^(3/2)/Cd");
		m_ctrlYSel.AddString("1/Cl^1/2");
		m_ctrlYSel.AddString("Re");
		m_ctrlYSel.AddString("XCp");
	}

	m_ctrlXSel.SetCurSel(m_XSel);
	m_ctrlYSel.SetCurSel(m_YSel);

	m_ctrlYSel.SetFocus();

	return FALSE; 
}

void CUserVarDlg::OnSelChangeXSel() 
{
	m_XSel = m_ctrlXSel.GetCurSel();
}

void CUserVarDlg::OnSelChangeYSel() 
{
	m_YSel = m_ctrlYSel.GetCurSel();
}

void CUserVarDlg::OnDblclkYsel() 
{
	m_YSel = m_ctrlYSel.GetCurSel();
	OnOK();	
}

void CUserVarDlg::OnDblclkXsel() 
{
	m_XSel = m_ctrlXSel.GetCurSel();
	OnOK();		
}
