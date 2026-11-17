# Odin32

Odin is a Win32 runtime for OS/2 and ArcaOS. It lets you run Windows
applications on OS/2 as if they were native OS/2 programs (`pe.exe app.exe`),
and it provides the Win32 API so Windows source code can be rebuilt for OS/2
(Odin32). Conceptually, Odin is to OS/2 what Wine is to Linux.

- Upstream: <https://github.com/netlabsorg/odin32>
- License: Project Odin Software License, see [LICENSE.TXT](LICENSE.TXT)
- Change history: [changelog](changelog)

## Status: 0.9.1a16 (alpha)

This tree is the 0.9.0 source plus a large set of changes that revive the build
on a current ArcaOS toolchain and improve compatibility with newer Windows
programs. It is an **alpha for community testing**, not a stable release.

New in 0.9.1a16 (on top of 0.9.1a15):

- Java 21 applications: the JDK splash screen and AWT tray icons no longer crash (Open32's
  `StretchDIBits` and `CreateDIBitmap` mishandled 32 bpp bitmaps; gdi32 now converts them to 24 bpp),
  the AWT clipboard code finds `GetUpdatedClipboardFormats`, and `GetCurrentDirectoryW` handles a
  size query. Pixelitor and muCommander run; see "Java 21 test package" below.
- New `opengl32.dll`: OpenGL for Win32 programs on top of Mesa 7.11.2 (see "OpenGL" below). JOGL and
  Java 3D 1.6 initialise; Sweet Home 3D 7.5 starts but its 3D view is still empty.

New in 0.9.1a15 (on top of 0.9.1a14):

- Supermium 150 (32-bit Chromium) start-up: `ConvertStringSidToSidW`, the `netapi32` join-information
  functions and `GetComputerNameExW` were missing or failed and Chromium treats that as fatal;
  `VirtualAlloc(MEM_RESERVE)` over an already mapped address now fails like on Windows. The
  browser reaches the creation of its first window (no window yet, then 100% CPU).
- Diagnostics: `ODIN_TRACE_ONLY` accepts 1000 characters, `ODIN_TRACE_TID`, exception traces dump
  strings/objects from the stack and registers, more trace tags.

New in 0.9.1a14 (on top of 0.9.1a13):

- `pe.exe` main thread stack 8 MB, TLS callback relocation fix for images loaded away from their preferred
  address, new `USERENV.DLL`, `VirtualAlloc(MEM_RESET)`, `GetProcessTimes` for `OpenProcess` handles.

New in 0.9.1a13 (on top of 0.9.1a12):

- `TransactNamedPipe` passed its buffers to `DosTransactNPipe` swapped; trace lines carry the process id.

New in 0.9.1a12 (on top of 0.9.1a11):

- Handle inheritance between Odin processes (`ODIN_INHERIT`): pipes and files passed to a child by
  value keep their handle value, `CreateNamedPipe` creates new instances first,
  `GetFileInformationByHandleEx` implements `FileNameInfo`, and fibers are implemented
  (`ODIN_NOFIBER=1` as a workaround). Supermium's crashpad handler child now starts and runs.

New in 0.9.1a11 (on top of 0.9.1a10):

- Supermium 150 gets through `chrome_elf.dll` and into crashpad's handler start (still no window):
  loader-lock owner fix, `CloseHandle` of Open32-only handles, `FILE_APPEND_DATA`/`FILE_WRITE_DATA`
  opens write, and `InitializeProcThreadAttributeList`/`UpdateProcThreadAttribute`. New
  `ODIN_WATCH=<hex address>` trace.

New in 0.9.1a10 (on top of 0.9.1a9):

- Named pipes: a client can open a pipe right after `CreateNamedPipe`, `ConnectNamedPipe` reports
  `ERROR_PIPE_CONNECTED`, and `SetHandleInformation` works for file mappings; Firefox now gets
  through its IPC pipe set-up (still no window).
- Supermium 150 (with its Win7 compatibility layer) starts and runs `chrome_elf.dll`'s
  initialisation; new ntdll loader functions, PEB process parameters and loader lock, mitigation
  policy stubs, security-descriptor builders and a built-in random provider in advapi32.

New in 0.9.1a9 (on top of 0.9.1a8):

- Firefox 161 still shows no window; it creates the main window hidden and waits for a content
  process that is never started. New tracing (`ODIN_TRACE_ONLY`, window, message, pipe and
  completion-port trace lines, module list in the hang dump) and `CreateProcessAsUser` now
  starts the process instead of failing.

New in 0.9.1a8 (on top of 0.9.1a7):

- Firefox 161 (Windows 7 build) runs for several minutes and starts its UI scripts and
  background services; still no window. Fixes behind that: `GetEnvironmentVariable` reports
  "not set", `MoveFileEx` replaces existing files, message-loop waits with `MWMO_INPUTAVAILABLE`,
  safe resource lookups, no process kill on a long critical-section wait, a new `wtsapi32` DLL,
  and more user32/wininet functions. `ODIN_TRACE` now also writes `TERMDUMP` lines that show
  who ended the process.

New in 0.9.1a7 (on top of 0.9.1a6):

- Firefox 161 (Windows 7 build) starts, creates its profile and runs for about 70 seconds; no
  window is shown yet. Behind that: working waitable timers, token handles that can be closed,
  `GetSecurityInfo`, COM apartment types, `iphlpapi` change notifications, message-only windows,
  and `ODIN_TRACE` no longer clobbering `GetLastError`.

New in 0.9.1a6 (on top of 0.9.1a5):

- Firefox 161 progress: the emulated Windows version is now also visible through the PEB,
  `VerifyVersionInfoW` compares for real, `NtQuerySystemInformation` returns the process list,
  `CreateWellKnownSid` works, and the `windowscodecs` stub DLL is now called `wincodec`.
  Firefox still does not show a window.
- New debugging aids: `PE-LOAD` lines in the `ODIN_TRACE` file and `ODIN_HANGDUMP=<seconds>`.

New in 0.9.1a5 (on top of 0.9.1a4):

- About 70 more Windows functions (ntdll `memcpy`/`memset`/keyed events, kernel32 job objects,
  work items, processor information, advapi32/shlwapi/psapi/dbghelp/ws2_32 additions, new
  `cfgmgr32` and `windowscodecs` stub DLLs) needed by the Firefox 161 Windows 7 build, and a
  `Win7` setting for `[WinVersion]` in `ODIN.INI`. Firefox loads and starts but does not show a
  window yet.

New in 0.9.1a4 (on top of 0.9.1a3):

- kernel32: `java -jar x.jar` works (`SetFilePointerEx` returned a negative position
  for a seek back from the end of a file) and appending writes with an OVERLAPPED
  offset of -1 no longer fail with "Negative seek".
- dsound: `DSOUND_LOG=<file>` logs the MMPM/2 calls made when opening the audio device.

New in 0.9.1a3 (on top of 0.9.1a2):

- Runs Java 21 too (BellSoft Liberica 21, the newest 32-bit Windows build of OpenJDK):
  `iphlpapi` network interface functions (`GetIfTable2`, `GetUnicastIpAddressTable`, ...),
  `GetActiveProcessorCount`, `IsProcessInJob` and a few more exports.

New in 0.9.1a2 (on top of 0.9.1a1):

- Runs Temurin 17 (the 32-bit Windows build of OpenJDK 17): many newer kernel32
  APIs, I/O completion ports, memory-map and `VirtualAlloc` fixes, `ws2_32`
  `getaddrinfo`/`inet_pton`/..., new stub DLLs `mswsock`, `powrprof`, `winhttp`.
  Swing/Java2D, servers (Netty, Jetty), TLS, `javac` and `jshell` work; no IPv6
  sockets and no `AsynchronousSocketChannel`.
- Fixes found with Notepad++ and PhotoFiltre 7 in user32, gdi32, comctl32,
  comdlg32 and shlwapi (Vista button styles, 32-bit alpha icons, DIB sections
  with V4/V5 headers, text baseline, file dialogs, WM_GETTEXT overflow, a PM
  deadlock while waiting for another thread's window).
- shell32: the user profile folder follows `%USERPROFILE%`, else `%HOME%`.

Highlights of 0.9.1a1 (full list in [changelog](changelog)):

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
Notepad++, PhotoFiltre 7, Temurin 17 and small Win32 test programs. Real-hardware sound (MMPM/2) has not
been re-tested after the winmm changes.

### Known problems

- With no working MMPM/2 audio, one tested game (Beavis and Butt-Head
  "Hock-A-Loogie") freezes for 10-30 seconds at the start of a round and has
  hung a single-CPU VM at 100% CPU.
- MCI video playback only reports success; nothing is displayed.
- Layered windows, DWM and visual styles are stubs only.
- Debug (non-release) builds are not verified.
- The version string inside the DLLs (`ODIN32_VERSION` in `include/odinbuild.h`) still says
  0.9.1a1 although the tree is 0.9.1a16.
- Java on Odin: see `Java17-on-Odin_17-0-17b10a3.zip`, `Java21-on-Odin_21-0-13b5a2.zip` and the self-contained
  `Java21-Odin-test-package-r2.zip` (separate downloads) for the launcher, install notes and limits (heap
  about 700 MB, no IPv6). Java sound needs a working MMPM/2 in new processes.
- yEd 3.25.1: while its modal "Welcome" dialog is open most of the main window is not painted and the
  menu bar is never painted (start it with a graph file argument to avoid the dialog). The cause is
  not found: Odin's DC clip regions are correct and Java issues only small blits for the frame.
- Sweet Home 3D 7.5: starts, 2D plan works, the 3D view stays empty (Java 3D runs on `opengl32.dll`
  but nothing is drawn yet).
- A JDK run from a VirtualBox shared folder stops during start-up (delay-load failure in
  `SystemProps.platformProperties`); run it from a local drive.
- Supermium 150 / Firefox 161: no window yet (see the 0.9.1a5-a15 notes).
- After a program that used a system-tray icon ends, an empty `TrayMessageWindow` entry can remain in
  the window list.

## OpenGL

`opengl32.dll` (new in 0.9.1a16, source in `src/opengl32`) gives Win32 programs OpenGL through Mesa:

- the 1081 `gl*` entry points are generated stdcall forwarders (`gen_glwrap.py` writes `glwrap.c` and
  `opengl32.def` from the Mesa headers) to the cdecl exports of `MesaGL7.dll`;
- the `wgl*` functions create OSMesa contexts that render into a 32 bpp buffer sized after the window;
  `wglSwapBuffers` copies it to the window with `StretchDIBits`. Two pixel formats (double and single
  buffered, RGBA, 24 bit depth, 8 bit stencil) are offered; contexts are not shared and extension
  functions come from the generated table;
- `MesaGL7.dll` is the Mesa 7.11.2 OS/2 port (GL 2.1, software rendering) and is not part of this tree; it must
  be found through LIBPATH or `BEGINLIBPATH` (the Java 21 test package puts it next to `opengl32.dll`).

State: JOGL reports `GL 2.1 Mesa 7.11.2` and Java 3D 1.6 creates its contexts; Sweet Home 3D's 3D view
is still empty. Debug aid: `OGL32_LOG=<file>`.

## Java 21 test package

`Java21-Odin-test-package-r2.zip` (separate download, built from this tree) unpacks to `C:\java21` and
runs OpenJDK 21 with its own copy of Odin in `C:\java21\OdinRT`, started through `javarun java ...`;
the installed Odin and Java are not touched. Tried with it: Pixelitor 4.3.1 (runs), muCommander 1.6.2 (runs,
needs the `--add-opens` options of its `mucommander.sh`), Freeplane 1.13.4 (starts), yEd 3.25.1 and Sweet
Home 3D 7.5 (partly, see Known problems).

## Installing a release package

Unzip `Odin-<version>.zip` (for example `Odin-0.9.1a4.zip`) into the root of the boot drive (UNIXROOT):

| Path | Content |
|---|---|
| `usr\bin` | `pe.exe`, `pec.exe`, `regsvr32.exe` |
| `usr\lib` | Odin DLLs and `.sym` files |
| `usr\share\doc\odin-<version>` | ChangeLog, license, documentation, release notes |

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

Other environment variables (all optional; they only affect the process that sees them):

| Variable | Effect |
|---|---|
| `ODIN_TRACE=<file>` | write the API trace to this file (needed by the variables below) |
| `ODIN_TRACE_ONLY=<tag>,<tag>...` | log only lines whose tag starts with one of these prefixes (up to 1000 characters), for example `EXC,LoadLibrary,CREATEWIN` |
| `ODIN_TRACE_TID=<hex>` | also log every tag of one OS/2 thread |
| `ODIN_TRACE_FULL` | longer exception dumps |
| `ODIN_NOTERMTRACE` | leave out the `TERM*` lines written at process end |
| `ODIN_HANGDUMP=<seconds>` | after this time dump instruction pointers and return addresses of all threads to the trace file (three times, 10 s apart) |
| `ODIN_WATCH=<hex address>` | log changes of the dword at this address |
| `ODIN_INI=<file>` | use this `ODIN.INI` instead of the one next to `KERNEL32.DLL` |
| `ODIN_NOFIBER=1` | run fiber-based code on the thread stack (workaround for Chromium) |
| `DSOUND_LOG=<file>`, `DSOUND_DEVICE=<n>` | log the MMPM/2 calls of `dsound`; select the audio device |
| `OGL32_LOG=<file>` | log the WGL calls of `opengl32.dll` |

Typical starting points: `ODIN_TRACE_ONLY=EXC` shows exceptions (with registers and stack); `PE-LOAD`,
`LoadLibrary`, `DosLoadModule-FAIL` and `GetProcAddress-MISS` show which DLLs and exports a program
looks for and does not find; `C:\POPUPLOG.OS2` holds the registers of silent OS/2 traps.

When reporting a problem, include the program and version, what you did, what
you expected, what happened, your OS/2 or ArcaOS version, whether it is a VM,
and the trace file if you can. See [doc/ReportingBugs.txt](doc/ReportingBugs.txt).

## Repository layout

| Directory | Content |
|---|---|
| `src` | Odin DLLs and the PE loader (`pe.exe`), one directory per DLL (`src/opengl32` is OpenGL over Mesa) |
| `include` | Win32 and Odin headers, GCC/kLIBC shims in `include/incl_gcc` |
| `lib` | Support libraries |
| `tools` | Build tools (`impdef`, `winerc`, `xx2lx`, ...) |
| `doc` | Documentation |
| `testapp` | Test applications |
