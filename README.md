# Odin32

Odin is a Win32 runtime for OS/2 and ArcaOS. It lets you run Windows
applications on OS/2 as if they were native OS/2 programs (`pe.exe app.exe`),
and it provides the Win32 API so Windows source code can be rebuilt for OS/2
(Odin32). Conceptually, Odin is to OS/2 what Wine is to Linux.

- Upstream: <https://github.com/netlabsorg/odin32>
- License: Project Odin Software License, see [LICENSE.TXT](LICENSE.TXT)
- Change history: [changelog](changelog)

## Status: 0.9.1a1 (alpha)

This tree is the 0.9.0 source plus a large set of changes that revive the build
on a current ArcaOS toolchain and improve compatibility with newer Windows
programs. It is an **alpha for community testing**, not a stable release.

Highlights (full list in [changelog](changelog)):

- Builds with GCC 9.2 (kLIBC) and the Open Watcom linker (wlink).
- Windows XP/Vista/7-era APIs added so newer Win32 programs can load at all;
  new compatibility DLLs `bcrypt`, `dbghelp`, `dwmapi`, `msimg32`, `sensapi`
  (mostly stubs).
- `msvcrt.dll`: about 100 new functions (secure string functions, 64-bit time
  and file functions, `_l` locale variants, `fopen_s`, ...).
- comctl32 fixes (rebar, ComboBoxEx), `IFileOpenDialog`/`IFileSaveDialog`,
  PNG icons, throttled idle `PeekMessage` polling.
- winmm: silent "null" wave device when MMPM/2 is not usable, minimal MCI
  `avivideo` driver (no picture).
- `ODIN_TRACE=<file>` lightweight API trace that works in release builds.

Tested on an ArcaOS 5.1.1 VirtualBox guest (single CPU) with 7-Zip, PuTTY 0.85,
Notepad++ and small Win32 test programs. Real-hardware sound (MMPM/2) has not
been re-tested after the winmm changes.

### Known problems

- With no working MMPM/2 audio, one tested game (Beavis and Butt-Head
  "Hock-A-Loogie") freezes for 10-30 seconds at the start of a round and has
  hung a single-CPU VM at 100% CPU.
- MCI video playback only reports success; nothing is displayed.
- Layered windows, DWM and visual styles are stubs only.
- Debug (non-release) builds are not verified.

## Installing a release package

Unzip `Odin-0.9.1a1.zip` into the root of the boot drive (UNIXROOT):

| Path | Content |
|---|---|
| `usr\bin` | `pe.exe`, `pec.exe`, `regsvr32.exe` |
| `usr\lib` | Odin DLLs and `.sym` files |
| `usr\share\doc\odin-0.9.1a1` | ChangeLog, license, documentation, release notes |

Close all Odin programs first and back up the old files. `usr\lib\ODIN.INI` is
not part of the package and is left alone. Run a program with:

```
pe.exe c:\path\to\program.exe
```

## Building

Requirements (ArcaOS):

- GCC 9.2 with kLIBC, installed under `C:\usr`
- kBuild (`kmk`)
- Open Watcom (`wlink`, `C:\WATCOM`)
- OS/2 Toolkit 4.5 (`C:\os2tk45`, provides headers and `alp.exe`)

Setup:

1. Copy `LocalConfig.kmk.tpl` to `LocalConfig.kmk` and set `PATH_SDK_OS2TK4`
   (for example `C:/os2tk45`).
2. Copy `env.tpl.cmd` to `env.cmd` and set up the GCC environment there.
3. In the shell that runs `kmk`, set the linker environment:

```
set EMXOMFLD_TYPE=WLINK
set EMXOMFLD_LINKER=wlink.exe
set EMXOMFLD_PRELINK=0
set WATCOM=C:\WATCOM
set BUILD_PLATFORM=
```

Build everything, or a single component:

```
kmk -k
kmk -k -C src/winmm
```

Output goes to `out\os2.x86\release\stage\bin` (DLLs, EXEs, `.sym`) and
`out\os2.x86\release\stage\lib` (import libraries). The version used in the
DLL bldlevel strings is set in [include/odinbuild.h](include/odinbuild.h).

More background: [doc/GCCBuildInstructions.txt](doc/GCCBuildInstructions.txt),
[doc/Readme.txt](doc/Readme.txt), [doc/Odin.ini.txt](doc/Odin.ini.txt).

## Tracing and bug reports

Set `ODIN_TRACE` to a file name before starting `pe.exe` to get a one-line-per-
event API trace:

```
set ODIN_TRACE=C:\odin_trace.txt
pe.exe program.exe
```

When reporting a problem, include the program and version, what you did, what
you expected, what happened, your OS/2 or ArcaOS version, whether it is a VM,
and the trace file if you can. See [doc/ReportingBugs.txt](doc/ReportingBugs.txt).

## Repository layout

| Directory | Content |
|---|---|
| `src` | Odin DLLs and the PE loader (`pe.exe`), one directory per DLL |
| `include` | Win32 and Odin headers, GCC/kLIBC shims in `include/incl_gcc` |
| `lib` | Support libraries |
| `tools` | Build tools (`impdef`, `winerc`, `xx2lx`, ...) |
| `doc` | Documentation |
| `testapp` | Test applications |
