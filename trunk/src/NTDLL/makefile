# $Id: makefile,v 1.1 1999-05-24 20:19:31 ktk Exp $ 

#
# PD-Win32 API 
#
#	NTDLL.dll makefile
# 

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


OBJS = NTDLL.obj

TARGET = NTDLL

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def $(PDWIN32_LIB)\kernel32.lib
# PMWINX.LIB LIBULS.LIB LIBCONV.LIB
	$(CP) $@ $(PDWIN32_BIN)

$(TARGET).lib: $(TARGET).dll
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET).dll
	$(CP) $@ $(PDWIN32_LIB)



ntdll.obj: \
    .\ntdll.c \
    .\ntdll.h \
    $(PDWIN32_INCLUDE)\unicode.h


clean:
	$(RM) *.obj *.lib *.dll *~ *.map *.pch 
	$(RM) $(PDWIN32_LIB)\$(TARGET).lib
	$(RM) $(PDWIN32_BIN)\$(TARGET).dll


