# $Id: makefile,v 1.2 2000-03-31 15:35:10 bird Exp $


all: Sym2Hll dbgLXDumper.exe

Sym2Hll:
    icc /Ti+ /Tm- /W3 /I..\common /B"/NOE" Sym2Hll.cpp kHll.cpp ..\common\commonicc.lib


dbgLXDumper.obj: dbgLXDumper.c hll.h
    icc -c /Ti+ dbgLXDumper.c

dummy.c:
    echo creating <<$@
/* dummy.c */
int idummy;
<<keep

dummy.obj: dummy.c
    icc -c  $**

dbgLXDumper.exe: dbgLXDumper.obj dummy.obj
    icc /Ti+ dbgLXDumper.obj dummy.obj


