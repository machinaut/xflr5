/****************************************************************************

    Wing Class
	Copyright (C) 2005-2008 Andre Deperrois xflr5@yahoo.com

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

#ifndef CWING_H
#define CWING_H

// Wing.h : header file
//

#include "Surface.h"
#include "Panel.h"

/////////////////////////////////////////////////////////////////////////////
// CWing dialog


class CWing
{ 
	friend class QMiarex;
	friend class MainFrame;
	friend class CWPolar;
	friend class CPOpp;
	friend class CPlane;
	friend class CSurface;
	friend class WingDlg;
	friend class GL3dViewDlg;
	friend class LLTAnalysisDlg;
        friend class VLMAnalysisDlg;
        friend class PanelAnalysisDlg;
        friend class PlaneDlg;
	friend class WPolarDlg;
	friend class ImportWingDlg;

	// Construction
public:

    CWing();   // standard constructor

// Implementation
protected:
	CSurface m_Surface[2*MAXPANELS];
	double m_VLMQInf[100];

	bool CreateSurfaces(CVector const &T, double XTilt, double YTilt);//generic surface, LLT, VLM or Panel

	bool VLMSetAutoMesh(int total = 0);
	int  VLMGetPanelTotal(void);
	void VLMSetBending();
	void VLMTrefftz(double *Gamma, int pos, CVector &Force, double & Drag, bool bTilted);
	void VLMComputeWing(double *Gamma, double *Cp, double &VDrag, double &XCP, double &YCP,
                        double &GCm, double &VCm, double &GRm, double &GYm, double &IYm, double &VYm, bool bViscous, bool bTilted);

	void PanelComputeWing(double *Cp, double &VDrag, double &XCP, double &YCP, double &GCm, double &GRm, double &GYm, double &VCm, double &VYm, double &IYm, bool bViscous, bool bThinSurface, bool bTilted);
	void PanelTrefftz(double *Cp, double *Mu, double *Sigma, int pos,  CVector &Force, double &Drag, bool bTilted, bool bThinSurf, CPanel *pWakePanel, CVector *pWakeNode);
	void PanelSetBending();

	void VLMCubicSplines(double *Gamma);
	bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
	double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d);


	bool LLTInitialize();
	bool LLTSetLinearSolution();
	void LLTInitCl();
	void LLTComputeWing();
	int  LLTIterate();
	double Sigma(int m);
	double Eta(int m);
	double AlphaInduced(int k);
	bool CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB, double *xPointA, double *xPointB, int &NXLead, int &NXFlap);
	void GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t);
	void Duplicate(CWing *pWing);
	void ComputeChords(int NStation=0);
	void ComputeDihedrals();
	void ComputeGeometry();

	void GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos);
    void InsertSection(double TPos, double TChord, double TOffset, double TZPos, double Twist, QString Foil,int NChord, int NSpan, int SSpan);
	void SetSweep(double Sweep);
	void SetTwist(double Twist);
	void ScaleSpan(double NewSpan);
	void ScaleChord(double NewChord);
	bool Gauss(double *A, int n, double *B, int m);
	bool SerializeWing(QDataStream &ar, bool bIsStoring);
//	bool ExportAVLWing(CStdioFile *pXFile, int index, double x, double y, double z, double Thetax, double Thetay);

	double GetAverageSweep();
	double GetChord(double yob);
	double GetC4(double yob, double xRef);
	double GetDihedral(double yob);
	double GetOffset(double yob);
	double GetTwist(double y);
	double GetZPos(double y);
	double Getyrel(double SpanPos);
	double Beta(int m, int k);

//	bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
//	double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d);


//__________________________Variables_______________________

	static void* s_pMainFrame;		//pointer to the Frame window
	static void* s_pMiarex;	//pointer to the Miarex Application window
	static void* s_pVLMDlg;	//pointer to the VLM analysis dialog class
	static void* s_pLLTDlg;	//pointer to the VLM analysis dialog class
	static void* s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
	static CVector *m_pWakeNode;			//pointer to the VLM wake node array
	static CPanel *m_pWakePanel;			//pointer to the VLM Wake Panel array
	static int s_NLLTStations;
	static double s_CvPrec;	// Precision required for LLT convergence
	static double s_RelaxMax;	// relaxation factor for LLT convergence

    QString m_WingName;	//the wing's name

    QFile * m_pXFile;	// a pointer to the output .log file
	
	bool m_bInitCalc;	//
	bool m_bVLMAutoMesh; 	// true if the mesh should be set automatically
	bool m_bWingOut;	// true if the wing OpPoint is outside the flight envelope of the available Type1 polars
	bool m_bConverged;	// true if LLT || VLM calculation has converged
	bool m_bTrace;		// true if the messages need to be traces
	bool m_bSymetric;	// true if the wing's geometry is symmetric
	bool m_bVLMSymetric;	// true if the vlm calculation is symmetric
	bool m_bLLT;		// true if performing an LLT calculation, false if VLM
	bool m_bTrans;		// true if the wing is being dragged 
	bool m_bIsFin, m_bDoubleFin, m_bSymFin, m_bDoubleSymFin; //fin flags
	bool m_bVLM1;		// true if classic, false if quad ring (K&P) method
	bool m_bCancel;		// true if the calculation is to be aborted
	bool m_bChanged;	// true if the user has made changes to the wing's definition

	int m_Type;		// the LLT analysis type
	int m_NPanel;		// number of span panels in wing definition
	int m_NStation;		// number of stations for wing calculation
	int m_iSection;		// the currently selected section
	int m_nNodes;		// the number of VLM panel nodes
	int m_NSurfaces; 	// The number of VLM Surfaces (=2 x Wing Panels)
	int m_MatSize;	// Max Size for the VLMMatrix
	int m_AVLIndex;		// need to export to AVL

	int m_nFlaps;		// the number of T.E. flaps, numbered from left wing to right wing
	double m_FlapMoment[2*MAXPANELS];

	double pi;		//
	double m_Span;		// wing span
	double m_GChord;	// mean geometric chord
	double m_MAChord;	// mean aerodynamic chord
	double m_yMac;		// mean aerodynamic chord span position
	double m_Density, m_Viscosity; //fluid properties
	double m_Area;		// wing surface
	double m_Volume;	// for tentative wieght calculations
	double m_AR;		// Aspect ratio
	double m_TR;		// Taper ratio
	double m_CL;		//Lift
	double m_InducedDrag, m_ViscousDrag;
	double m_XCP, m_YCP;	// Centre of pressure's position
	double m_QInf;		// Freestream speed
	double m_QInf0;		// Freestream speed for initial Type 2 analysis
	double m_Weight;	// For type 2 analysis
	double m_Alpha;		// angle of attack
	double m_Maxa; 		// Used in LLT
	double m_VYm; 
	double m_IYm;		// Induced Yawing Moment
	double m_GCm, m_GRm, m_GYm;		// Geometric Yawing Moment

	int m_NXPanels[MAXPANELS+1]; 		// VLM Panels along chord, for each Wing Panel
	int m_NYPanels[MAXPANELS+1]; 		// VLM Panels along span, for each Wing Panel
	int m_XPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel
	int m_YPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel
	double m_TChord[MAXPANELS+1];		// Chord length at each panel side
	double m_TLength[MAXPANELS+1];		// the length of each panel
	double m_TPos[MAXPANELS+1];		// b-position of each panel end on developed surface
	double m_TYProj[MAXPANELS+1];		// b-position of each panel end projected on horizontal surface
	double m_TOffset[MAXPANELS+1];		// b-position of each panel end
	double m_TDihedral[MAXPANELS+1];	// b-position of each panel end
	double m_TZPos[MAXPANELS+1];		// vertical offset - calculation result only
	double m_TTwist[MAXPANELS+1];		//Twist value of each foil (measured to the wing root)

	// Span Coefficients  resulting from VLM or LLT calculation
	double m_Ai[MAXSTATIONS+1];		//Induced angles, in degrees
	double m_Cl[MAXSTATIONS+1];		//Lift coefficient at stations
	double m_PCd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_ICd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_Cm[MAXSTATIONS+1];		//Pitching moment coefficient at stations
	double m_CmAirf[MAXSTATIONS+1];		//Aill part of Pitching moment coefficient at stations
	double m_CmXRef[MAXSTATIONS+1];		//Geometric part of Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_XCPSpanAbs[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_Re[MAXSTATIONS+1];		//Reynolds number at stations
	double m_Chord[MAXSTATIONS+1];		//chord at stations
	double m_Offset[MAXSTATIONS+1];		//offset at LLT stations
	double m_XTrTop[MAXSTATIONS+1];		//Upper transition location at stations
	double m_XTrBot[MAXSTATIONS+1];		//Lower transition location at stations
	double m_Twist[MAXSTATIONS+1];		//twist at LLT stations
	double m_StripArea[MAXSTATIONS+1];		
	double m_BendingMoment[MAXSTATIONS+1];	//bending moment at stations
	double m_SpanPos[MAXSTATIONS+1];	//span positions of LLT stations
	double m_xHinge[MAXCHORDPANELS];		//chorwise position of flap hinges
	double m_xPanel[MAXCHORDPANELS];	//chorwise position of VLM panels


 	CPanel *m_pPanel;			//pointer to the VLM Panel array

    QStringList m_RFoil;			// name of the right foils
    QStringList m_LFoil;			// name of the left foils

	CVector m_Vd[MAXSTATIONS];		// downwash vector at span stations
	CVector m_F[MAXSTATIONS];		// lift vector at span stations

    QColor m_WingColor;
};

#endif
