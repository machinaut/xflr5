/****************************************************************************

    SettingsDlg Class
    Copyright (C) 2006 Andr� Deperrois XFLR5@yahoo.com

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

#pragma once

// SettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog
#include "FloatEdit.h"
#include "NumEdit.h"
#include "ClrClasses.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{     
	friend class CMainFrame;
// Construction
public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGSLDG };
	CButton	m_ctrlVLMMesh;
//	CClrBtn	m_ctrlWingSurfaces;
	CButton	m_ctrlDownwash;
	CButton	m_ctrlWake;
	CButton	m_ctrlXCP;
	CButton	m_ctrlIDrag, m_ctrlVDrag;
	CButton	m_ctrlBotTrans;
	CButton	m_ctrlTopTrans;
	CButton	m_ctrlWingPanels;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
private:
	CWnd* m_pParent;

//	COLORREF m_WingSurfaces;

	int m_VLMStyle, m_VLMWidth;
	COLORREF m_VLMColor;
	int m_OutlineStyle, m_OutlineWidth;
	COLORREF m_OutlineColor;
	int m_TopStyle, m_TopWidth;
	COLORREF m_TopColor;
	int m_BotStyle, m_BotWidth;
	COLORREF m_BotColor;
	int m_XCPStyle, m_XCPWidth;
	COLORREF m_XCPColor;
	int m_IDragStyle, m_IDragWidth;
	COLORREF m_IDragColor;
	int m_VDragStyle, m_VDragWidth;
	COLORREF m_VDragColor;
	int m_DownwashStyle, m_DownwashWidth;
	COLORREF m_DownwashColor;
	int m_WakeStyle, m_WakeWidth;
	COLORREF m_WakeColor;

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	afx_msg void OnWingColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTopTrans();
	afx_msg void OnBotTrans();
	afx_msg void OnXCP();
	afx_msg void OnIDrag();
	afx_msg void OnVDrag();
	afx_msg void OnDownwash();
	afx_msg void OnWake();
	afx_msg void OnVLMMesh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
