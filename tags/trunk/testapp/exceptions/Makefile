# $Id: Makefile,v 1.1 2001-04-19 18:03:49 bird Exp $

#
# This is something at least :-)
#
#

basic.exe   \
condexc.exe \
ctrexc.exe  \
rethrow.exe \
sample.exe: $(@B).cpp
    wcl386 -xs -bt=nt -l=nt -d2 /hc /"option map" -I$(WATCOM)\h\nt $** /Fe=$@



