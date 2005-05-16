# $Id: Makefile,v 1.2 2005-05-16 14:17:05 sao2l02 Exp $

#
# Odin32 API
#
#       makefile for generates template-headerfiles from wine
#

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


TARGETS = \
activscp.h \
amstream.h \
amvideo.h \
austream.h \
comcat.h \
control.h \
ddstream.h \
dispex.h \
docobj.h \
exdisp.h \
mediaobj.h \
mlang.h \
mmstream.h \
mshtmhst.h \
mshtml.h \
oaidl.h \
objidl.h \
ocidl.h \
oleidl.h \
propidl.h \
pstore.h \
richole.h \
sensevts.h \
servprov.h \
shldisp.h \
shobjidl.h \
shtypes.h \
strmif.h \
unknwn.h \
urlmon.h \
wtypes.h


IDL_INC = \
axcore.idl \
axextend.idl \
devenum.idl \
dyngraph.idl


.SUFFIXES:      .h .idl

.idl.h:
   -2 $(ODIN32_TOOLS)\widl.exe -I . -I $(ODIN32_INCLUDE)\win -I $(ODIN32_INCLUDE) -D __IBMCPP__=300 -D RC_INVOKED -D __WIN32OS2__ -W -h $<
   -2 @rm $*.h.????aa wpptmp.????aa 2>nul

#
# heder to generate from idl.
#
all $(IDL_INC): $(TARGETS)

clean:
  $(RM) *.h *.h.????aa wpptmp.????aa

cleanall: clean
  $(RM) .depend

cleandlls dep needed:
    @echo Nothing to do for $@

