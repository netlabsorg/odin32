#
# http://svn.netlabs.org/odin32
#
%define test_mode   1

%define ver_major   0
%define ver_minor   1
%define ver_patch   0

%define rpm_release 1

Name:       odin
Vendor:     netlabs.org
License:    Project Odin Software License
Url:        http://svn.netlabs.org/odin32/

Version:    %{ver_major}.%{ver_minor}.%{ver_patch}
Release:    %{rpm_release}

Source:     %{name}-%{version}.zip

%define descr_brief Odin is a set of libraries that provide a Win32-compatible runtime environment.\
This environment is necesasry to run Win32 applications on OS/2 as well as\
native OS/2 applications built from Win32 sources using Odin SDK.

%define pkg_docdir      %{_docdir}/%{name}

%define pkg_wps_base            ODIN
%define pkg_wps_folder_id       <%{pkg_wps_base}_FOLDER>
%define pkg_wps_folder_create   %{pkg_wps_base}_FOLDER:WPFolder|Odin|<WP_DESKTOP>
%define pkg_wps_view_txt()      EXENAME=e.exe;PROGTYPE=PROG_PM;PARAMETERS=((%*))
%define pkg_wps_view_inf()      EXENAME=view.exe;PROGTYPE=PROG_PM;PARAMETERS=((%*))

#------------------------------------------------------------------------------
# commons
#------------------------------------------------------------------------------

# installation paths
%define odin_libdir     %{_libdir}
%define odin_bindir     %{_bindir}
%define odin_systemdir  %{odin_libdir}
%define odin_windowsdir %{_var}/lib/odin

# disable lxlite compression (this causes problems for some DLLs)
%define __os_install_post   %{nil}

#------------------------------------------------------------------------------
# main package
#------------------------------------------------------------------------------

Summary:    Odin (dummy)
Group:      System Environment/Libraries

%description
%{descr_brief}

This package is not actually generated (due to the missing %files section).
It is present only because RPM requires Summary: and %description sections for
the main package to exist.

#------------------------------------------------------------------------------
%package -n lib%{name}
#------------------------------------------------------------------------------

Summary:    Odin runtime
Group:      System Environment/Libraries

%description -n lib%{name}
%{descr_brief}

This package contains core Odin runtime libraries.

%files -n lib%{name}
%defattr(-,root,root,-)
%doc %{pkg_docdir}/
%exclude %{pkg_docdir}/odinuser.inf
%if "%{odin_libdir}" != "%{_libdir}"
%dir %{odin_libdir}/
%endif
%if "%{odin_bindir}" != "%{_bindir}"
%{odin_bindir}/
%else
%{odin_bindir}/*.exe
%endif
%exclude %{odin_bindir}/pe.exe
%exclude %{odin_bindir}/pec.exe
%exclude %{odin_bindir}/xx2lx.exe
%if "%{odin_systemdir}" != "%{odin_libdir}"
%{odin_systemdir}/
%else
%{odin_systemdir}/*
%endif
%exclude %{odin_systemdir}/capi2032.dll
%exclude %{odin_systemdir}/secur32.dll
%exclude %{odin_systemdir}/schannel.dll
%exclude %{odin_systemdir}/win32k.sys
%exclude %{odin_systemdir}/win32kCC.exe
%exclude %{odin_systemdir}/kRx.exe
# files/directories generated by odininst.exe removed on uninstall
%attr(0755,root,root) %ghost %{odin_systemdir}/Drivers/
%attr(0755,root,root) %ghost %{odin_systemdir}/Drivers/etc/
%attr(0644,root,root) %ghost %{odin_systemdir}/ODIN.INI
# files/directories generated by odininst.exe preserved on uninstall
%attr(0755,root,root) %ghost %config %{odin_windowsdir}/

%pre -n lib%{name}
%warpin_conflicts_begin
Odin\Odin\Odin Core Files
Odin\Odin\Odin System Files
Odin\Odin\Odin .sym files
Odin\Odin\Changes to Config.sys
%warpin_conflicts_end

%post -n lib%{name}

CFGSYS_CHANGED=
%if "%{odin_systemdir}" != "%{_libdir}"
export ODIN_SYSTEMDIR_D="$(echo %{odin_systemdir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {ADDSTRING ";%ODIN_SYSTEMDIR_D%" IN "LIBPATH=" (FIRST AFTER} %{os2_config_sys} >nul
CFGSYS_CHANGED=y
%endif
%if "%{odin_bindir}" != "%{_bindir}"
export ODIN_BINDIR_D="$(echo %{odin_bindir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {ADDSTRING ";%ODIN_BINDIR_D%" IN "SET PATH=" (FIRST AFTER} %{os2_config_sys} >nul
%endif
if [ -n "$CFGSYS_CHANGED" ]; then
    echo; echo "NOTE:"
    echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
    echo "computer in order to activate these changes."
    echo
fi

#initialize system directories, registry and INI files
ODIN_WINDOWSDIR_D=$(echo "%{odin_windowsdir}" | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')
echo \
"[DEVDIRECTORIES]
WINDOWS=$ODIN_WINDOWSDIR_D
" \
> "%{odin_systemdir}/ODIN.INI"
# mkdir -p fails on /@unixroot, replace it with the real value
mkdir -p "$(echo %{odin_windowsdir} | sed -re 's,/@unixroot,'$UNIXROOT',g')"
(cd "%{odin_systemdir}/"; "%{odin_systemdir}"/odininst.exe)

%postun -n lib%{name}

CFGSYS_CHANGED=
%if "%{odin_bindir}" != "%{_bindir}"
export ODIN_BINDIR_D="$(echo %{odin_bindir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {DELSTRING ";%ODIN_BINDIR_D%" IN "SET PATH=" (FIRST} %{os2_config_sys} >nul
CFGSYS_CHANGED=y
%endif
%if "%{odin_systemdir}" != "%{_libdir}"
export ODIN_SYSTEMDIR_D="$(echo %{odin_systemdir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {DELSTRING ";%ODIN_SYSTEMDIR_D%" IN "LIBPATH=" (FIRST} %{os2_config_sys} >nul
%endif
if [ -n "$CFGSYS_CHANGED" ]; then
    echo; echo "NOTE:"
    echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
    echo "computer in order to activate these changes."
    echo
fi

#------------------------------------------------------------------------------
%package -n lib%{name}-capi20
#------------------------------------------------------------------------------

Summary:    Odin CAPI 2.0 library
Group:      System Environment/Libraries

Requires:   lib%{name} = %{version}-%{release}

%description -n lib%{name}-capi20
%{descr_brief}

This package contains the Common ISDN Application Programming Interface (CAPI)
runtime library needed for applications working with ISDN devices.

Note that this library requires a native OS/2 CAPI library to be installed (it
is usually supplied with the ISDN device drivers). You may use a virtual package
libcapi20-dummy to satisfy this requirement if you have installed the necessary
CAPI DLLs manually.

%files -n lib%{name}-capi20
%defattr(-,root,root,-)
%{odin_systemdir}/capi2032.dll

#------------------------------------------------------------------------------
%package -n lib%{name}-security
#------------------------------------------------------------------------------

Summary:    Odin security library
Group:      System Environment/Libraries

Requires:   lib%{name} = %{version}-%{release}

%description -n lib%{name}-security
%{descr_brief}

This package contains the Windows Secure Service Provider Interface (SSPI)
runtime library needed for applications using this interface.

Note that this library requires a native OS/2 NETAPI library to be installed
(which is part of the IBM Peer component or the IBM HPFS386 package). You may
use a virtual package libnetapi-dummy to satisfy this requirement if you have
installed the necessary NETAPI DLLs manually.

%files -n lib%{name}-security
%defattr(-,root,root,-)
%{odin_systemdir}/secur32.dll

#------------------------------------------------------------------------------
%package exe-tools
#------------------------------------------------------------------------------

Summary:    Odin EXE tools
Group:      System Environment/Tools

Requires:   lib%{name} = %{version}-%{release}

%description exe-tools
%{descr_brief}

This package contains command line tools used to convert Windows executables
to OS/2 executables so that they can be run on OS/2 in the Odin environment.

Note that these tools are NOT CURRENTLY SUPPORTED and therefore not recommended
for general use. They are only provided for testing purposes.

%files exe-tools
%defattr(-,root,root,-)
%{odin_bindir}/pe.exe
%{odin_bindir}/pec.exe
%{odin_bindir}/xx2lx.exe

#------------------------------------------------------------------------------
%package win32k
#------------------------------------------------------------------------------

Summary:    Odin Win32k driver
Group:      System Environment/Libraries

Requires:   lib%{name} = %{version}-%{release}

%description win32k
%{descr_brief}

This package contains a special driver (win32k.sys) that makes native Win32
executables equal to native OS/2 executables so that they may be started in
the OS/2 environment directly, bypassing the conversion phase.

Note that this driver is NOT CURRENTLY SUPPORTED and therefore not recommended
for general use. It is only provided for testing purposes.

%files win32k
%defattr(-,root,root,-)
%{odin_systemdir}/win32k.sys
%{odin_systemdir}/win32kCC.exe
%{odin_systemdir}/kRx.exe

%post win32k
export ODIN_SYSTEMDIR_D="$(echo %{odin_systemdir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {ADDLINE "DEVICE=%ODIN_SYSTEMDIR_D%\win32k.sys" (AFTER "DEVICE="} %{os2_config_sys} >nul
echo; echo "NOTE:"
echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
echo "computer in order to activate these changes."
echo

%postun win32k
export ODIN_SYSTEMDIR_D="$(echo %{odin_systemdir} | sed -re 's,/@unixroot,'$UNIXROOT',g' -e 's,/,\\,g')"
%cube {DELLINE "DEVICE=%ODIN_SYSTEMDIR_D%\win32k.sys"} %{os2_config_sys} >nul
echo; echo "NOTE:"
echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
echo "computer in order to activate these changes."
echo

#------------------------------------------------------------------------------
%package doc
#------------------------------------------------------------------------------

Summary:    Odin documentation
Group:      Documentation
BuildArch:  noarch

Requires:   lib%{name} = %{version}-%{release}

%description doc
%{descr_brief}

This package contains Odin user's manual.

%files doc
%defattr(-,root,root,-)
%doc %{pkg_docdir}/odinuser.inf

%post doc
%wps_object_create_begin -n %{name}-doc
%{pkg_wps_folder_create}
%{pkg_wps_base}_README:WPProgram|Read Me|%{pkg_wps_folder_id}|%{pkg_wps_view_txt %{pkg_docdir}/Readme.txt}
%{pkg_wps_base}_CHANGELOG:WPProgram|ChangeLog|%{pkg_wps_folder_id}|%{pkg_wps_view_txt %{pkg_docdir}/ChangeLog}
%{pkg_wps_base}_LICENSE:WPProgram|License|%{pkg_wps_folder_id}|%{pkg_wps_view_txt %{pkg_docdir}/LICENSE.TXT}
%{pkg_wps_base}_WGSS50_LICENSE:WPProgram|WGSS50 License|%{pkg_wps_folder_id}|%{pkg_wps_view_txt %{pkg_docdir}/WGSS50.lic}
%{pkg_wps_base}_MANUAL:WPProgram|User's Manual|%{pkg_wps_folder_id}|%{pkg_wps_view_inf %{pkg_docdir}/odinuser.inf}
%wps_object_create_end

%postun doc
%wps_object_delete_all -n %{name}-doc

#------------------------------------------------------------------------------
%prep
#------------------------------------------------------------------------------

%if 0%{?test_mode}
%setup -TD
%else
# @todo move the zip to the right place!
%setup -q
%endif

#------------------------------------------------------------------------------
%build
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
%install
#------------------------------------------------------------------------------

rm -rf %{buildroot}

mkdir -p %{buildroot}%{pkg_docdir}
cp *.txt %{buildroot}%{pkg_docdir}/
cp LICENSE.TXT %{buildroot}%{pkg_docdir}/
cp ChangeLog %{buildroot}%{pkg_docdir}/
cp WGSS50.lic %{buildroot}%{pkg_docdir}/
cp odinuser.inf %{buildroot}%{pkg_docdir}/

mkdir -p %{buildroot}%{odin_windowsdir}/

mkdir -p %{buildroot}%{odin_systemdir}/
cp -dp system32/*.dll %{buildroot}%{odin_systemdir}/
cp -dp system32/*.exe %{buildroot}%{odin_systemdir}/
cp -dp system32/win32k.sys %{buildroot}%{odin_systemdir}/

mkdir -p %{buildroot}%{odin_bindir}/
cp system32/*.exe %{buildroot}%{odin_bindir}/

# these are internal tools that should not be in PATH
mv %{buildroot}%{odin_bindir}/odininst.exe %{buildroot}%{odin_systemdir}/
mv %{buildroot}%{odin_bindir}/win32kCC.exe %{buildroot}%{odin_systemdir}/
mv %{buildroot}%{odin_bindir}/kRx.exe %{buildroot}%{odin_systemdir}/

#------------------------------------------------------------------------------
%clean
#------------------------------------------------------------------------------

%if !0%{?test_mode}
rm -rf %{buildroot}
%endif

#------------------------------------------------------------------------------
%changelog

