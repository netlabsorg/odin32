# $Id: makefile,v 1.2 2000-02-25 00:39:57 sandervl Exp $
#
# PD-Win32 API
#
#       main glide makefile
#

PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB = ..\..\..\lib
PDWIN32_BIN = ..\..\..\bin
PDWIN32_TOOLS = ..\..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk

TARGET1 = cvg\glide2x
TARGET2 = sst1\glide2x

all: $(TARGET1).dll $(TARGET2).dll
lib: $(TARGET1).lib

$(TARGET1).dll:
    cd cvg
    $(MAKE_CMD)
    cd ..

$(TARGET2).dll:
    cd sst1
    $(MAKE_CMD)
    cd ..

$(TARGET1).lib:
    cd cvg
    $(MAKE_CMD) $(@F)
    cd ..

clean:
    cd cvg
    $(MAKE_CMD) clean
    cd ..\sst1
    $(MAKE_CMD) clean
    cd ..

dep:
    cd cvg
    $(MAKE_CMD) NODEP=1 dep
    cd ..\sst1
    $(MAKE_CMD) NODEP=1 dep
    cd ..

