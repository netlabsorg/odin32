# $Id: makefile,v 1.3 2000-03-04 20:03:42 bird Exp $
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
    $(MAKE_CMD) dep
    cd ..\sst1
    $(MAKE_CMD) dep
    cd ..

