# $Id: Makefile,v 1.1 2005-05-11 21:27:53 sao2l02 Exp $

#
# Odin32 API
#
#       makefile for generates template-headerfiles from wine
#

NO_LIB_RULE = 1
NO_ALL_RULE = 1
NO_CLEAN_RULE = 1
NOCLEAN = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


TARGETS = \
activscp.h \
amstream.h \
amvideo.h \
austream.h \
#axcore.h \
#axextend.h \
comcat.h \
control.h \
ddstream.h \
#devenum.h \
dispex.h \
docobj.h \
#dyngraph.h \
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


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


.SUFFIXES:      .h .idl

.idl.h:
   -2 $(ODIN32_TOOLS)\widl.exe -I . -I $(ODIN32_INCLUDE)\win -I $(ODIN32_INCLUDE) -D __IBMCPP__=300 -D RC_INVOKED -D __WIN32OS2__ -W -h $<
   -2 @rm $*.h.????aa wpptmp.????aa 2>nul

#
# heder to generate from idl.
#
all: $(TARGETS)

clean:
  $(RM) *.h *.h.????aa wpptmp.????aa

cleanall: clean
  $(RM) .depend

