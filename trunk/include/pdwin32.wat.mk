# $Id: pdwin32.wat.mk,v 1.1 2000-08-23 18:05:55 sandervl Exp $
#

# defines
#    DEBUG - build a debug version instead of release version
#

!ifdef DEBUG
!include $(PDWIN32_INCLUDE)/pdwin32.wat.dbg
!else
!include $(PDWIN32_INCLUDE)/pdwin32.wat.rel
!endif

# include system tools
!include $(PDWIN32_INCLUDE)/pdwin32.wat.tools

