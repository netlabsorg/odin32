@echo off

rem :: Define linker to be used by KMK
set EMXOMFLD_TYPE=wlink
set EMXOMFLD_LINKER=wl.exe

rem :: Path to the OS/2 Toolkit
set PATH_SDK_OS2TK4=x:/os2tk45

rem :: Prevent ALP from crashing when invoked with full path.
rem :: ALP.EXE needs to be found in the PATH.
rem :: Version 4.00.008 or higher is required.
rem set PATH_TOOL_ALP=alp.exe

rem :: Start the KMK build process
kmk 
