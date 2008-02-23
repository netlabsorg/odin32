# Microsoft Visual C++ generated build script - Do not modify

PROJ = W16ODIN
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = d:\windows\freecell.exe
DLLS = odindll.dll
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = G:\MSVC\PROJECTS\ODIN\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = ODIN.C      
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /FPi87 /Zp1 /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"W16ODIN.PDB"
CFLAGS_R_WEXE = /nologo /G2 /FPi87 /Zp1 /W3 /AL /O1 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr llibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr llibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = W16ODIN.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = ODIN.SBR


ODIN_DEP = 

all:	$(PROJ).EXE $(PROJ).BSC

ODIN.OBJ:	ODIN.C $(ODIN_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c ODIN.C


$(PROJ).EXE::	ODIN.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
ODIN.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) $@


run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
