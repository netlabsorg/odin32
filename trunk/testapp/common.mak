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
	@echo [Running $<$(if $($<_ARGS), $($<_ARGS))]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN_DOS)/bin/Debug;$(ODIN_DOS)/bin;%BEGINLIBPATH% && $< $($<_ARGS)"
	@echo.

debug-%: %
	@echo [Running $<$(if $($<_ARGS), $($<_ARGS))]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN_DOS)/bin/Debug;$(ODIN_DOS)/bin;%BEGINLIBPATH% && $(DEBUGGER) $< $($<_ARGS)"
	@echo.
	
%.exe: %.c
	gcc $< -Zomf -Zmap -Zlinker /PM:VIO -o $@ $(CFLAGS)

%.s: %.c
	gcc $< -S -Zomf -o $@ $(CFLAGS)

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
