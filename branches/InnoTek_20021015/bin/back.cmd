@echo off
@md backup\%1 > nul
@copy *.c backup\%1 > nul
@copy *.l backup\%1 > nul
@copy *.y backup\%1 > nul
@copy *.cpp backup\%1 > nul
@copy *.rc backup\%1 > nul
@copy *.dlg backup\%1 > nul
@copy *.hpp backup\%1 > nul
@copy *.asm backup\%1 > nul
@copy *.def backup\%1 > nul
@copy *.h backup\%1 > nul
@copy makefile.* backup\%1 > nul