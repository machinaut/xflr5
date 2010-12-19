# This is a spec file for the installation of XFLR5 v6
%define name xflr5
%define release 2
%define version 6.02

BuildRoot: %{_tmppath}/%{name}-%{version}
Summary: XFLR5 is an analysis tool for airfoils and planes operating at low Re numbers
License: GPL
Name: %{name}
Version: %{version}
Release: %{release}
Source: %{name}-%{version}.tar.gz
Packager: x-andre
Group: Applications/Engineering
%if 0%{?fedora_version}  
BuildRequires: qt-devel >= 4.4.3 gcc-c++  , Mesa-devel
%else  
BuildRequires: libqt4-devel >= 4.4.3 gcc-c++  , Mesa-devel
%endif  

%description
XFLR5 is a design tool intended for the design and analysis of reduced scaled model airplanes.
It includes the XFoil program for the analysis of airfoils.
It includes a non-linear Lifting Line method (LLT), two Vortex Lattice Methods (VLM),
and a 3D panel first-order method for the analysis of wings and planes.

XFLR5 v5 is a re-write of XFLR5 v4 using Qt4 libraries instead of Microsoft's MFC.

XFLR5 v6 introduces stability and control analysis

%prep
#%{name}         = xflr5
#%{_docdir}      = /usr/share/doc/packages
#%{_datadir}     = /usr/share
#%{_bindir}      = /usr/bin
#%{_tmppath}     = /home/windsoarer/Qt/rpmbuild/tmp
#%{buildroot}    = /home/windsoarer/Qt/rpmbuild/BUILDROOT/xflr5-6.00-1.x86_64

%setup -q

%build 
%if 0%{?fedora_version}  
qmake-qt4 -makefile %{name}.pro 'target.path = %{buildroot}%{_bindir}'  
qmake-qt4 'target.path = %{buildroot}%{_bindir}'  
%else  
qmake -makefile %{name}.pro 'target.path = %{buildroot}%{_bindir}'  
qmake 'target.path = %{buildroot}%{_bindir}'   
%endif  

echo $RPM_BUILD_ROOT 
echo %{buildroot}%{_bindir} 

make 

 
%install
%__mkdir -p %{buildroot}%{_bindir} 
%__mkdir -p %{buildroot}/usr/share/%{name}/translations 
%__mkdir -p %{buildroot}/usr/share/applications
%__mkdir -p %{buildroot}/usr/share/pixmaps 
%__mkdir -p %{buildroot}%{_docdir}/%{name}  
make install DESTDIR=$RPM_BUILD_ROOT $INSTALL_TARGET 
#make install  DESTDIR=%{buildroot}%{_bindir} $INSTALL_TARGET
%__install -m 644 images/%{name}.png    %{buildroot}%{_datadir}/pixmaps/%{name}.png  
%__install -m 644 suse/%{name}.desktop  %{buildroot}%{_datadir}/applications  
%__install -m 644 translations/*.qm     %{buildroot}/usr/share/%{name}/translations 
%__install -m 644 doc/*.pdf             %{buildroot}/usr/share/%{name} 


%files
%defattr(-,root,root)
  
#%if 0%{suse_version} > 1110 || 0%{fedora_version} || 0%{mandriva_version}  
%{_datadir}/applications/%{name}.desktop  
#%endif  
  
%{_datadir}/pixmaps/%{name}.png  
%{_datadir}/%{name}  
%docdir %{buildroot}/usr/share/%{name}/translations

%doc License.txt
%{_bindir}/%{name} 

%clean
#%{__rm} -rf $RPM_BUILD_ROOT 

echo %{_bindir}/%{name} 


%changelog
*Sun Dec 19 2010 A. Deperrois
XFLR5v6.02 beta rpm release
- added an option to load a background image in the foil direct design module
- corrected some errors in CoG and inertia automatic calculations, with impact on results
- did some deep code cleaning
- re-organized the panel analysis code to perform only one far field calculation per operating point
- optimized the panel method for increased speed of execution
- corrected the mesh edition for NURBS bodies
- changed the export format for graphs
- implemented the stability method for mixed panel/vlm method
- implemented the stability method for full 3D panel method
- corrected the bug which lead to incorrect results in 3D panel with Neumann B.C.
- corrected the error in the jump in potential at the wing's trailing edage in full 3d panel method
- set the estimation of inertia properties at the time of polar definition instead of evaluating at the time of analysis
- fixed the bug which could cause a crash when dragging a point in splines design
- corrected the asymetric transition location for symetric wings
- corrected the source strength influence of thick panels on thin surfaces, with impact on results for planes with body
- corrected the Cp 3D color display for VLM2 analysis
- added the influence of pressure forces acting on the body to induced pitching moment coefficient
- added calculation of neutral point position based on Cma/Cza ratio
- corrected various minor bugs
- made several minor enhancements

*Thu Oct 14 2010 A. Deperrois
XFLR5v6.01 beta rpm release
- Corrected the incorrect balance speed in 3D type 2 polars for planes
- Corrected the final moment calculation in stability analysis
- Corrected the incorrect lift calculation in stability analysis

*Sat Sep 19 2010 A. Deperrois
XFLR5v6.00 beta rpm release

*Fri Apr 3  2010 A. Deperrois
First rpm release
