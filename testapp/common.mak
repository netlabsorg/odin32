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

WRC = $(ODIN)/tools/wrc/bin/Release/wrc.exe

ODIN_FLAVOR := Debug

ifneq ($(RELEASE)$(REL),)
ODIN_FLAVOR := Release
endif

RUN_SHELL =
DEBUG_SHELL = ipmd

run-%: %
	@echo [Running $<$(if $($<_ARGS), $($<_ARGS)) (shell=$(RUN_SHELL),flavor=$(ODIN_FLAVOR))]
	@echo.
	@cmd /c "set BEGINLIBPATH=$(ODIN_DOS)\bin\$(ODIN_FLAVOR);$(ODIN_DOS)\bin;%BEGINLIBPATH% && $(RUN_SHELL) $< $($<_ARGS)"
	@echo.

debug-%: RUN_SHELL = $(DEBUG_SHELL)
debug-%: run-%
	@rem dummy, needed for the pattern rule to work...	

debug: RUN_SHELL = $(DEBUG_SHELL)
debug: run

ipmd-%: RUN_SHELL = ipmd
ipmd-%: run-%
	@rem dummy, needed for the pattern rule to work...	

ipmd: RUN_SHELL = ipmd
ipmd: run

idbug-%: RUN_SHELL = idbug
idbug-%: run-%
	@rem dummy, needed for the pattern rule to work...	

idbug: RUN_SHELL = idbug
idbug: run

r-%: ODIN_FLAVOR = Release
r-%: %
	@rem dummy, needed for the pattern rule to work...	

d-%: ODIN_FLAVOR = Debug
d-%: %
	@rem dummy, needed for the pattern rule to work...	

help:
	@echo.
	@echo Targets:
	@echo   run-EXE       Run EXE in normal shell (default: cmd, RUN_SHELL overrides)
	@echo   debug-EXE     Run EXE in debug shell (default: ipmd, DEBUG_SHELL overrides)
	@echo   ipmd-EXE      Run EXE in IPMD shell
	@echo   idbug-EXE     Run EXE in IDBUG shell
	@echo.
	@echo Flavors:
	@echo   d-TARGET      Run against debug Odin libraries (default)
	@echo   r-TARGET      Run against release Odin libraries (RELEASE=1 makes it default)
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
