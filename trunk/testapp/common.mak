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

DEFINES += -D__WIN32OS2__ -D__i386__ -DSTRICT -D_POSIX_SOURCE \
           -D_POSIX_C_SOURCE=200112 -D_EMX_SOURCE -D_XOPEN_SOURCE=600 \
           -D_SVID_SOURCE

INCLUDES += -I$(ODIN)/include -I$(ODIN)/include/win

CFLAGS += $(DEFINES) $(INCLUDES) -g -DDEBUG -L$(ODIN)/lib/Debug -lkernel32.lib

DEBUGGER = ipmd

WRC = $(ODIN)/tools/wrc/bin/Release/wrc.exe

run-%: %
	@echo [Running $<$(if $($<_ARGS), $($<_ARGS))]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN)/bin/Debug;$(ODIN)/bin;%BEGINLIBPATH% && $< $($<_ARGS)"
	@echo.

debug-%: %
	@echo [Running $<$(if $($<_ARGS), $($<_ARGS))]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN)/bin/Debug;$(ODIN)/bin;%BEGINLIBPATH% && $(DEBUGGER) $< $($<_ARGS)"
	@echo.
	
%.exe: %.c
	gcc $^ -Zomf -Zmap -Zlinker /PM:VIO -o $@ $(CFLAGS) $(foreach src, $^, $(CFLAGS.$(src))) $(CFLAGS.$@)

%.exe:
	gcc $^ -Zomf -Zmap -Zlinker /PM:VIO -o $@ $(CFLAGS) $(foreach src, $^, $(CFLAGS.$(src))) $(CFLAGS.$@)
	
%.s: %.c
	gcc $< -S -Zomf -o $@ $(CFLAGS)

%.s: %.rc
	$(WRC) -s $(DEFINES) $(INCLUDES) $< -o $@

CLEAN = *.exe *.map *.log

clean::
	-rm $(CLEAN)

define run-with-args-gen
.PHONY: run-$(1)-$(2) run-$(1)-$(3)
run-$(1)-$(2): $(1)
	@make -s run-$(1) $(1)_ARGS=$($(1)_ARGS.$(2))
run-$(1)-$(3):: run-$(1)-$(2)
debug-$(1)-$(2): $(1)
	@make -s debug-$(1) $(1)_ARGS=$($(1)_ARGS.$(2))
endef

run-with-args = $(foreach i, $(2), $(eval $(call run-with-args-gen,$(1),$(i),$(3))))run-$(1)-$(3)
