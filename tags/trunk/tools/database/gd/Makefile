#Depending on your system, you will need to modify this makefile.

#If you do not have gcc, change the setting for CC, but you must
#use an ANSI standard C compiler (NOT the old SunOS 4.1.3 cc
#compiler; get gcc if you are still using it).

#If the ar command fails on your system, consult the ar manpage
#for your system.

all: gdgcc.a gdgcc.lib gdicc.lib

gdgcc.a: force
   @nmake /nologo OMF=0 -f makefile.gcc $@

gdgcc.lib: force
   @nmake /nologo OMF=1 -f makefile.gcc $@

gdicc.lib: force
   @nmake /nologo -f makefile.icc $@

force:
    @echo ...

clean:
   @nmake /nologo -f makefile.icc clean
   @nmake /nologo OMF=0 -f makefile.gcc clean
   @nmake /nologo OMF=1 -f makefile.gcc clean

