/****************************************************************************

    CLLTThread Class
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

// WThread.cpp : implementation file
// This class will run the LLT thread and call the appropriate
// methods implemented in the CWing class
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "LLTThread.h"
#include "Miarex.h"
#include "LLTDlg.h"
#include "../XDirect/Foil.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CLLTThread

IMPLEMENT_DYNCREATE(CLLTThread, CWinThread)

CLLTThread::CLLTThread(CWnd * pParent)
{
	pi = 3.141592654;
	m_pParent   = pParent;
	m_bCancel   = false;
	m_bSkip     = false;
	m_bFinished = false;
	m_bType4    = false;

	m_IterLim = 100;

	m_Alpha = 0.0;
}

CLLTThread::~CLLTThread()
{
}

BOOL CLLTThread::InitInstance()
{
	CString str;
//	CLLTDlg *pIDlg = (CLLTDlg*)m_pParent;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	m_IterLim  = pMiarex->m_Iter;

	if(!m_bType4) 
		AlphaLoop();
	else 
		ReLoop();

	if (!m_bCancel) if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString("\r\nAnalysis completed successfully\r\n");

	Sleep(100);
	return FALSE;
}

int CLLTThread::ExitInstance()
{
	m_pParent->PostMessage(W_ENDTHREAD);
	m_bFinished = true;

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLLTThread, CWinThread)
	//{{AFX_MSG_MAP(CLLTThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLLTThread message handlers

int CLLTThread::Iterate()
{
	CString str;
	CLLTDlg * pIDlg = (CLLTDlg*)m_pParent;
	int resp;
	int   iter = 0;
	bool  bConverged = false;

	while(iter<m_IterLim && !m_bSkip){
		if(m_bCancel) break;
		iter++;
		resp = m_pWing->LLTIterate();
		if(resp == 1) {
			bConverged = true;
			pIDlg->m_State=1;
			pIDlg->m_pIterCurve->AddPoint((double)iter, m_pWing->m_Maxa);
			if(!m_bType4) pIDlg->UpdateView(m_pWing->m_Alpha);
			else pIDlg->UpdateView(m_pWing->m_QInf);
			return iter;
		}
		else if (resp==-1) {
			bConverged = false;
			break;// Type 2, lift <0
		}
		// else continue iterations

		pIDlg->m_pIterCurve->AddPoint((double)iter, m_pWing->m_Maxa);
		if(!m_bType4) pIDlg->UpdateView(m_pWing->m_Alpha);
		else pIDlg->UpdateView(m_pWing->m_QInf);
		
	}
	if(m_bSkip){
		if(!m_bType4)	str.Format("Alpha = %6.2f, skipped after %d iterations \r\n",m_pWing->m_Alpha,iter);
		else str.Format("QInf = %8.2f skipped after %d iterations \r\n",m_pWing->m_QInf,iter);
		if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
		m_bSkip = false;
	}
	else if (resp<0){//negative lift
		pIDlg->m_State=2;
		if(!m_bType4) pIDlg->UpdateView(m_pWing->m_Alpha);
		else pIDlg->UpdateView(m_pWing->m_QInf);
		m_pWing->m_bInitCalc = true;
		return -1;
	}
	else if(!bConverged && !m_bCancel){
		pIDlg->m_State=2;
		if(!m_bType4) pIDlg->UpdateView(m_pWing->m_Alpha);
		else pIDlg->UpdateView(m_pWing->m_QInf);
		m_pWing->m_bInitCalc = true;
		return -2;
	}
	return iter;
}


void CLLTThread::Cancel()
{
	m_bCancel = true;
}


bool CLLTThread::AlphaLoop()
{
	CString str;
	CLLTDlg * pIDlg = (CLLTDlg*)m_pParent;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	int iter;

	m_pWing->m_CvPrec = pMiarex->m_CvPrec;
	m_pWing->m_Alpha = m_Alpha;

	if(m_pWing->m_bTrace) {
		m_pWing->m_pXFile->WriteString("Launching analysis....\r\n\r\n");
		str.Format("Max iterations     = %d\r\n", m_IterLim);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Alpha precision    = %.6f�\r\n", m_pWing->m_CvPrec);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Relaxation factor  = %.1f\r\n", pMiarex->m_Relax);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Number of stations = %d\r\n\r\n", m_pWing->m_NStation);
		m_pWing->m_pXFile->WriteString(str);
	}

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -(double)fabs(m_DeltaAlpha);
	int ia  = (int)fabs((m_AlphaMax-m_Alpha)*1.001f/m_DeltaAlpha);

	if(!m_bSequence) ia = 0;

	if(!m_pWing->LLTInitialize()) return FALSE;
	pIDlg->m_IterGraph.ResetLimits();
	pIDlg->m_IterGraph.SetXMax((double)m_IterLim);
	pIDlg->m_IterGraph.SetYMinGrid(true, true, RGB(100,100,100), 2, 1, 4);
	for (int i=0; i<=ia; i++){
		if(m_bCancel) {
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString("Analysis cancelled on user request....\r\n");
			break;
		}
		pIDlg->m_IterGraph.SetYMin(0.0);
		pIDlg->m_IterGraph.SetYMax(0.5);
		pIDlg->m_pIterCurve->ResetCurve();
		pIDlg->m_State=0;
		if(m_pWing->m_bInitCalc) m_pWing->LLTSetLinearSolution();
		
		m_pWing->LLTInitCl();//with new angle...
		str.Format("Calculating Alpha = %5.2f... \r\n", m_pWing->m_Alpha);
		if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
		iter = Iterate();
		if(iter<0){
			//unconverged
			pIDlg->m_bWarning = true;
			str.Format("\r\n");
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = true;
		}
		else if (iter<m_IterLim && !m_bCancel){//converged, 
			str.Format("    ...converged after %d iterations \r\n",iter);
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->LLTComputeWing();// generates wing results, 
			if (m_pWing->m_bWingOut) pIDlg->m_bWarning = true;
			pMiarex->AddWOpp(m_pWing->m_bWingOut);// Adds WOpp point and adds result to polar
			str.Format("\r\n");
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = false;
		}
		else {
			if (m_pWing->m_bWingOut) pIDlg->m_bWarning = true;
			str.Format("    ...unconverged after %2d iterations\r\n\r\n", iter);
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = true;
		}
		m_pWing->m_Alpha+=m_DeltaAlpha;
		Sleep(100);
	}
	return true;
}

bool CLLTThread::ReLoop()
{
	//Alpha stands for QInf...

	CString str;
	CLLTDlg * pIDlg = (CLLTDlg*)m_pParent;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	int iter;

	m_pWing->m_CvPrec = pMiarex->m_CvPrec;
	m_pWing->m_QInf = m_Alpha;
	//Alpha has been set at CMiarex::SetWPlr


	if(m_pWing->m_bTrace) {
		m_pWing->m_pXFile->WriteString("Launching analysis....\r\n\r\n");
		str.Format("Max iterations     = %d\r\n", m_IterLim);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Alpha precision    = %.6f�\r\n", m_pWing->m_CvPrec);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Relaxation factor  = %.1f\r\n", pMiarex->m_Relax);
		m_pWing->m_pXFile->WriteString(str);
		str.Format("Number of stations = %d\r\n\r\n", m_pWing->m_NStation);
		m_pWing->m_pXFile->WriteString(str);
	}

	
	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -(double)fabs(m_DeltaAlpha);
	int ia  = (int)fabs((m_AlphaMax-m_Alpha)*1.001/m_DeltaAlpha);

	if(!m_bSequence) ia = 0;

	if(!m_pWing->LLTInitialize()) return FALSE;
	pIDlg->m_IterGraph.ResetLimits();
	pIDlg->m_IterGraph.SetXMax((double)m_IterLim);
	pIDlg->m_IterGraph.SetYMinGrid(true, true, RGB(100,100,100), 2, 1, 4);
	for (int i=0; i<=ia; i++){
		if(m_bCancel) {
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString("Analysis cancelled on user request....\r\n");
			break;
		}
		pIDlg->m_IterGraph.SetYMin(0.0);
		pIDlg->m_IterGraph.SetYMax(0.5);
		pIDlg->m_pIterCurve->ResetCurve();
		pIDlg->m_State=0;
		if(m_pWing->m_bInitCalc)m_pWing->LLTSetLinearSolution();
		m_pWing->LLTInitCl();//with new angle...
		
		str.Format("Calculating QInf = %6.2f... \r\n", m_pWing->m_QInf);
		if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
		iter = Iterate();
		if(iter<0){
			//unconverged
			pIDlg->m_bWarning = true;
			str.Format("\r\n");
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = true;
		}
		else if (iter<m_IterLim  && !m_bCancel){//converged, 
			str.Format("    ...converged after %d iterations \r\n",iter);
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->LLTComputeWing();// generates wing results, 
			if (m_pWing->m_bWingOut)pIDlg->m_bWarning = true;
			pMiarex->AddWOpp(m_pWing->m_bWingOut);// Adds WOpp point and adds result to polar
			str.Format("\r\n");
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = false;
		}
		else {
//			m_pWing->LLTComputeWing();// generates wing results, 
			if (m_pWing->m_bWingOut) pIDlg->m_bWarning = true;
			str.Format("    ...unconverged after %2d iterations\r\n\r\n", iter);
			if(m_pWing->m_bTrace) m_pWing->m_pXFile->WriteString(str);
			m_pWing->m_bInitCalc = true;
		}
		m_pWing->m_QInf+=m_DeltaAlpha;
		Sleep(50);
	}
	return true;
}
