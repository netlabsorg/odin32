rem // This is an example environment script for building Odin.
rem // Please refer to ReadMe files for the detailed information about
rem // the required tools.
rem // Note that you will also need to use Configure.cmd to create
rem // required configuration files.

@echo off

rem // set up VAC3 environment
call vac3.cmd

rem // we only need gcc for NTDLL so unset CCENV to avoid 
rem // building everything with it
call gcc335l.cmd
set CCENV=

rem // set PATH=D:\Dev\ddk\base\tools;%PATH%

rem // set up WarpIn environment (for WPI creation)
rem // set PATH=C:\ecs\install\WARPIN;%PATH%
rem // set BEGINLIBPATH=C:\ecs\install\WARPIN;%BEGINLIBPATH%
