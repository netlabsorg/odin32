# $Id: common.post.mk,v 1.1 2000-12-02 23:26:58 bird Exp $
#
# Common rules for tools.
#
# Requires that ODIN32_TCOMMON is defined to point to the
# /tools/common directory
#
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#



#
# Rules for making
#
$(ODIN32_TCOMMON)\$(OBJDIR)\common.a  \
$(ODIN32_TCOMMON)\$(OBJDIR)\common.lib : commonforce
    $(DODIRS)  "$(ODIN32_TCOMMON)" \
!ifdef MAKE_CMD
    $(MAKE_CMD) \
!else
    $(MAKE) -nologo \
!endif
        $(OBJDIR)\$(@F)


#
# Dummy rule which forces a target to be made
#
commonforce:
    $(ECHO) ...
