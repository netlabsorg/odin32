: $Id: back.cmd,v 1.1 1999-05-24 20:19:05 ktk Exp $ 

@echo off
@md backup\%1 > nul
@copy *.c backup\%1 > nul
@copy *.cpp backup\%1 > nul
@copy *.rc backup\%1 > nul
@copy *.dlg backup\%1 > nul
@copy *.hpp backup\%1 > nul
@copy *.asm backup\%1 > nul
@copy *.def backup\%1 > nul
@copy *.h backup\%1 > nul
@copy makefile.* backup\%1 > nul