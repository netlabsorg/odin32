# $Id: common.post.mk,v 1.2 2001-09-30 00:18:11 bird Exp $
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


# dummy
$(COMMONLIB):


#
# Rules for making
#
common_lib: commonforce
    @$(DODIRS)  "$(ODIN32_TCOMMON)" \
!ifdef MAKE_CMD
    $(MAKE_CMD) \
!else
    $(MAKE) -nologo \
!endif
        $(OBJDIR)\common.lib


#
# Dummy rule which forces a target to be made
#
commonforce:
    @$(ECHO) ...
