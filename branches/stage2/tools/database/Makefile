# $Id: Makefile,v 1.17 2001-04-17 00:24:09 bird Exp $

#
# Odin32 Api Database Utilities.
#
#


#
# Common tools macros. (MAKE_CMD)
#
!include ../../makefile.inc


#
# dummy all rule - invoking nmake withtout target caused it to do all of the targets below..
#
_all: all


#
# All the common rules like all, lib, clean and dep.
#
$(COMMONRULES):
    $(MAKE_CMD) -f APIImport.mak $@
    $(MAKE_CMD) -f StateUpd.mak $@



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

