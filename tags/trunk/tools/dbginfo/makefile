# $Id: makefile,v 1.1 2000-03-27 10:20:42 bird Exp $


all: Sym2Hll dbgLXDumper.exe

Sym2Hll:
    icc /Ti+ /Tm- /W3 /I..\common /B"/NOE" Sym2Hll.cpp kHll.cpp ..\common\commonicc.lib


dbgLXDumper.exe:
    icc /Ti+ dbgLXDumper.c


