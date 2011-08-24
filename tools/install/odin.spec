#
# http://svn.netlabs.org/odin32
#

%define test_build  1

%define ver_major   0
%define ver_minor   1
%define ver_patch   0

%define rpm_release 1

%define pkg_name        odin

%define descr_brief Odin is a set of libraries that provide a Win32-compatible runtime environment.\
This environment is necesasry to run Win32 applications on OS/2 as well as\
native OS/2 applications built from Win32 sources using Odin SDK.

%define pkg_wps_base            ODIN
%define pkg_wps_folder_id       <%{pkg_wps_base}_FOLDER>
%define pkg_wps_folder_create   %{pkg_wps_base}_FOLDER:WPFolder|Odin|<WP_DESKTOP>
%define pkg_wps_reader()        EXENAME=e.exe;PROGTYPE=PROG_PM;PARAMETERS=((%*))

%define pkg_name_ver    %{pkg_name}-%{ver_major}.%{ver_minor}.%{ver_patch}

#------------------------------------------------------------------------------
# main package
#------------------------------------------------------------------------------

Name:       %{pkg_name}
Summary:    Odin runtime
Group:      System Environment/Libraries
Version:    %{ver_major}.%{ver_minor}.%{ver_patch}
Release:    %{rpm_release}
Vendor:     netlabs.org

# @todo?
#Requires:   libc >= 0.6.3

License:    Project Odin Software License
Url:        http://svn.netlabs.org/odin32/

Source:     odin.zip

#BuildRoot:  %{_tmppath}/%{name}-%{version}-%{release}-root

# @todo
#BuildRequires:

%description
%{descr_brief}

This package contains Odin runtime libraries and utilities.

%files
%defattr(-,root,root,-)
%doc %{_docdir}/%{pkg_name_ver}/Readme.txt
#%{_libdir}/wgss50.dll
#%{_libdir}/odincrt.dll
#%{_libdir}/ntdll.dll
#%{_libdir}/kernel32.dll

%pre
/*.cmd*/

    packages.1 = 'Odin\Odin\Odin Core Files'
    packages.2 = 'Odin\Odin\Odin System Files'
    packages.3 = 'Odin\Odin\Odin .sym files'
    packages.4 = 'Odin\Odin\Changes to Config.sys'
    packages.0 = 4

    ver = ''
    do i = 1 to packages.0
        ver = GetPkgVersion(packages.i)
        if (ver \== '') then leave
    end
    if (ver == '') then exit 0

    say; say 'ERROR:'; say
    say 'You have a WPI package "'packages.i'" version 'ver
    say 'installed on your system.'; say
    say 'You cannot have both the WPI and RPM package installed at the same time.'
    say 'Please de-install the specified WPI package using the WarpIn utility'
    say 'and try again.'; say
    exit 1

    GetPkgVersion: procedure
        parse arg aPkgId
        rc = rxFuncAdd('SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs')
        rc = SysLoadFuncs()
        WarpInDir = strip(SysIni('USER', 'WarpIN', 'Path'), 'T', '0'x)
        if (WarpInDir \== '') then do
            rc = SysFileTree(WarpInDir'\DATBAS_?.INI', 'inis', 'FO')
            if (rc == 0) then do
                do i = 1 to inis.0
                    rc = SysIni(inis.i, 'ALL:', 'apps')
                    if (rc == '') then do
                        do j = 1 to apps.0
                            apps.j = strip(apps.j, 'T', '0'x)
                            if (left(apps.j, length(aPkgId)) == aPkgId) then do
                                /* found the app */
                                ver = right(apps.j, length(apps.j) - length(aPkgId) - 1)
                                ver = translate(ver, '.', '\')
                                return ver
                            end
                        end
                    end
                    else do
                        say; say 'ERROR:'; say
                        say 'Failed to access the file "'inis.i'".'; say
                        say 'Please close the WarpIn application or, if it is not running, make sure'
                        say 'that this file is not locked by another process and try again.'; say
                        exit 5
                    end
                end
            end
        end
        return ''

%post
%wps_object_create_begin
%{pkg_wps_folder_create}
%{pkg_wps_base}_README:WPProgram|Read Me|%{pkg_wps_folder_id}|%{pkg_wps_reader %{_docdir}/%{pkg_name_ver}/Readme.txt}
%wps_object_create_end

%postun
%wps_object_delete_all

#------------------------------------------------------------------------------
%package win32k
#------------------------------------------------------------------------------

Summary:    Odin Win32k driver
Group:      System Environment/Libraries

Requires:   %{pkg_name} = %{version}-%{release}

%description win32k
%{descr_brief}

This package contains a special driver (win32k.sys) that makes native Win32
executables equal to native OS/2 executables so that they may be started in
the OS/2 environment directly, bypassing the conversion phase.

Note that this driver is NOT CURRENTLY SUPPORTED and therefore not recommended
for general use. It is only provided for testing purposes.

%files win32k
%defattr(-,root,root,-)
%{_libdir}/win32k.sys

%post win32k
%cube {ADDLINE "DEVICE=%UNIXROOT%\usr\%{_lib}\win32k.sys" (AFTER "DEVICE="} %{os2_config_sys} %{os2_config_sys}.yum >nul
echo; echo "NOTE:"
echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
echo "computer in order to activate these changes."
echo

%postun win32k
%cube {DELLINE "DEVICE=%UNIXROOT%\usr\%{_lib}\win32k.sys"} %{os2_config_sys} %{os2_config_sys}.yum >nul
echo; echo "NOTE:"
echo; echo "The file '%{os2_config_sys}' has been changed. You need to reboot your"
echo "computer in order to activate these changes."
echo

#------------------------------------------------------------------------------
%prep
#------------------------------------------------------------------------------

%if 0%{?test_build}
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

mkdir -p %{buildroot}%{_libdir}
#cp system32/wgss50.dll %{buildroot}%{_libdir}/
#cp system32/odincrt.dll %{buildroot}%{_libdir}/
#cp system32/ntdll.dll %{buildroot}%{_libdir}/
#cp system32/kernel32.dll %{buildroot}%{_libdir}/
cp system32/win32k.sys %{buildroot}%{_libdir}/
mkdir -p %{buildroot}%{_docdir}/%{pkg_name_ver}
cp Readme.txt %{buildroot}%{_docdir}/%{pkg_name_ver}/

#------------------------------------------------------------------------------
%clean
#------------------------------------------------------------------------------

rm -rf %{buildroot}

#------------------------------------------------------------------------------
%changelog

