# $Id: Makefile,v 1.1 1999-09-05 02:56:39 bird Exp $

#
# Common makefile for database utils - EMX/GCC version
#

# common include directory
PDWIN32_INCLUDE = ..\..\include
!include $(PDWIN32_INCLUDE)\pdwin32.tools

!ifndef OMF
OMF=0
!endif

#
# gcc defines
#
CC  =gcc
CXX =gcc
LD  =gcc

!if $(OMF) == 0
O      = o
A      = a
AR     = ar
CFZOMF =
!else
O      = obj
A      = lib
AR     = emxomfar
CFZOMF = -Zomf
!endif

CDEFINES  = -D__WIN32OS2__ -D__WINE__ -DNOTDLL
CINCLUDES = -I$(PDWIN32_INCLUDE)\Win -I$(PDWIN32_INCLUDE) -Imysql -Igd -I..\common
# Note: Add -fhandle-exceptions if old gcc version.
!ifndef DEBUG
CFLAGS        = $(CFLAGS) -Zmap -Zmt -mprobe -Wall -s -O
!else
CFLAGS        = $(CFZOMF) -Zmap -Zmt -mprobe -Wall -g
!endif
CXXFLAGS  = $(CFLAGS)
LIBS      = -L. -ldb -lmysql\libmysqlclient -lsocket -lstdcpp
LINK      = $(LD) $(CFLAGS) -o $@


#
# interference rules (the .c.obj/.cpp.obj rules are intended for Visual Slick only)
#
.c.o:
    $(CC) -c $(CFLAGS) $(CDEFINES) $(CINCLUDES) $<

.cpp.o:
    $(CXX) -c $(CXXFLAGS) $(CDEFINES) $(CINCLUDES) $<

.c.obj:
    $(CC) -c $(CFLAGS) $(CDEFINES) $(CINCLUDES) $<

.cpp.obj:
    $(CXX) -c $(CXXFLAGS) $(CDEFINES) $(CINCLUDES) $<



#
# All roule
#
all: db.$(A) APIImport.exe StateUpd.exe kHTMLPC.exe \
   ..\bin\APIImport.exe ..\bin\StateUpd.exe ..\bin\kHTMLPC.exe



#
# Database library
#
db.$(A): db.$(O)
   -$(RM) $@
   $(AR) cr $@ $**

db.$(O): db.cpp db.h mysql\mysql.h



#
# APIImport
#
APIImport.exe: APIImport.$(O) db.$(A) ..\common\commongcc.$(A)
   $(LINK) APIImport.$(O) -l..\common\commongcc $(LIBS)

..\bin\APIImport.exe: APIImport.exe
   $(CP) $** $@

..\common\commongcc.$(A):
    @cd ..\common
    @nmake /nologo commongcc.$(A)
    @cd ..\database


#
# StateUpd
#
StateUpd.exe: StateUpd.$(O) db.$(A)
   $(LINK) StateUpd.$(O)  $(LIBS)

..\bin\StateUpd.exe: StateUpd.exe
   $(CP) $** $@

StateUpd.$(O):     StateUpd.cpp db.h



#
# kHTMLPC - HTML/Sql PreCompiler.
#
kHTMLPC.exe: kHTMLPC.$(O) gd/gdgcc.$(A) db.$(A)
   $(LINK) kHTMLPC.$(O) -lgd/gdgcc  $(LIBS)

..\bin\kHTMLPC.exe: kHTMLPC.exe
   $(CP) $** $@

kHTMLPC.$(O): kHTMLPC.cpp kHTMLPC.h db.h ..\common\kLIFO.cpp    \
            ..\common\kLIFO.h ..\common\kList.h ..\common\kList.cpp \
            gd/gd.h gd/gdfontg.h gd/gdfontl.h gd/gdfontmb.h gd/gdfonts.h \
            gd/gdfontt.h


gd/gdgcc.$(A): NUL
   @cd gd
   @nmake /nologo gdgcc.$(A)
   @cd ..



#
# kHTHMLPC - interference roules for preprocessing of kSqlHtml files.
#
.SUFFIXES: .html .ksqlhtml .$(O)
.kSqlHtml.html:
    kHTMLPC $<
.kSqlHtml.obj: # Visual SlickEdit thinks everything complies to .obj files...
    kHTMLPC $<



#
# Odin32 - Creates the database. This will do a drop of any database named "Odin32"!
#
Odin32: CreateTables.sql States.sql Authors.sql
   -mysqladmin refresh
   mysqladmin DROP Odin32
   mysql < CreateTables.sql
   mysql < States.sql
   mysql < Authors.sql
   -mysqladmin refresh

Authors: Authors.sql
   mysql < Authors.sql
   -mysqladmin refresh

States: States.sql
   mysql < States.sql
   -mysqladmin refresh



#
# clean
#
clean:
   -@$(RM) *.obj
   -@$(RM) *.lib
   -@$(RM) *.o
   -@$(RM) *.a
   -@$(RM) *.dll
   -@$(RM) *.exe
   -@$(RM) *.pch
   -@$(RM) *.log
   -@$(RM) *.map
   @cd gd
   @nmake /nologo clean

