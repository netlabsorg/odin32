#
# PD-Win32 Project
#
#
#       Create: cdmckill@novice.uwaterloo.ca
#               January 4th, 1998
#
#


!ifdef DEBUG
!include $(PDWIN32_INCLUDE)/pdwin32.dbg
!else
!include $(PDWIN32_INCLUDE)/pdwin32.rel
!endif