rem THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
rem PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
rem TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
rem INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
rem DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
rem THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
rem EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
rem FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
rem 
rem USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
rem RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
rem TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
rem AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
rem SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
rem THE UNITED STATES.  
rem 
rem COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED

rem
rem   This batch file compiles with watcom for DOS2 env.
rem
rem   This is an example of the commands that are needed to
rem   build this lib
rem
rem   You will need to modify this file to adopt to your
rem   build environment


rem This sets up the watcom compiler vars
rem you probably need to change these for you development env

set WATCOM=c:\watcom
set PATH=%PATH%;c:watcom\binnt;c:\watcom\binw
set INCLUDE=c:\watcom\h;c:\watcom\h\nt;..\..\include

rem
rem if you want to build for Win32 instead of DOS
rem change bt=NT and copy the glide library from WIN32
rem Change NT to DOS4G on link line
rem

SET WCC386= -otexan -I..\..\include -D__DOS32__ -w4 -e25 -7 -fp5 -zq -5r -ei -mf -bt#dos      
wcc386 fxpci.c
wcc386 sst1_pci.c

create tmp.lnk
echo +fxpci.obj  >> tmp.lnk
echo +sst1_pci.obj >> tmp.lnk

wlib -b -c -n -q -p=512 fxpci @tmp.lnk
del tmp.lnk

copy fxpci.lib ..\..\lib

