ifndef ODIN
$(error ODIN variable must be set)
else
ifeq ($(shell ls $(ODIN)/include/odin.h),)
$(error $(ODIN) does not contain include/odin.h)
endif
endif

.PHONY: default

default: all

ODIN_DOS = $(subst /,\,$(ODIN))

CFLAGS += -D__WIN32OS2__ -D__i386__ -DSTRICT -D_POSIX_SOURCE \
          -D_POSIX_C_SOURCE=200112 -D_EMX_SOURCE -D_XOPEN_SOURCE=600 \
          -D_SVID_SOURCE
CFLAGS += -I$(ODIN)/include -I$(ODIN)/include/win

CFLAGS += -g -DDEBUG -L$(ODIN)/lib/Debug -lkernel32.lib

DEBUGGER = ipmd

run-%: %
	@echo [Running $<]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN_DOS)/bin/Debug;$(ODIN_DOS)/bin;%BEGINLIBPATH% && $<"

debug-%: %
	@echo [Debugging $<]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN_DOS)/bin/Debug;$(ODIN_DOS)/bin;%BEGINLIBPATH% && $(DEBUGGER) $<"
	
%.exe: %.c
	gcc $< -Zomf -Zmap -Zlinker /PM:VIO -o $@ $(CFLAGS)

%.s: %.c
	gcc $< -S -Zomf -o $@ $(CFLAGS)

CLEAN = *.exe *.map *.log

clean::
	-rm $(CLEAN)
