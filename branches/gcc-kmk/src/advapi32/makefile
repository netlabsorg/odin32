
#
# Odin32 API
#
#       advapi32.dll makefile
#


!if "$(DEBUG)" == "1" && "$(CUSTOMBUILD)" == "1"
DEFFILE    = advapi32dbg.def
ORGDEFFILE = advapi32.def
!endif

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\advapi32.obj \
$(OBJDIR)\advapi32rsrc.obj \
$(OBJDIR)\lsa.obj \
$(OBJDIR)\registry.obj \
$(OBJDIR)\security.obj \
$(OBJDIR)\security_odin.obj \
$(OBJDIR)\systemfunction.obj \
$(OBJDIR)\service.obj \
$(OBJDIR)\eventlog.obj \
$(OBJDIR)\crypt.obj \
$(OBJDIR)\cred.obj \
$(OBJDIR)\crypt_arc4.obj\
$(OBJDIR)\crypt_des.obj\
$(OBJDIR)\crypt_lmhash.obj\
$(OBJDIR)\crypt_md4.obj\
$(OBJDIR)\crypt_md5.obj\
$(OBJDIR)\crypt_sha.obj\
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = advapi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

