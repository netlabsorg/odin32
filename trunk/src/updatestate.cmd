@echo off
rem $Id: updatestate.cmd,v 1.1 1999-12-06 23:51:41 bird Exp $
cd %1
echo %1
..\..\tools\bin\stateupd.exe -ie- -ib-
rem dir *.def
cd ..
