


Some mail explaination which I'll fix later.
------------------------------------------------

It used to work with Warp 4 fixpack 9 (which was the one we used at work).
There are two big issues:
 - WPS crash or PM corruption when starting icsperf due to
   exhausted stack. probably caused by changes in PM or WPS.
   This I have made working patches for.
 - The debugee traps before any tracedata is collected. This is
   because icsperf tries to load a bunch of DLL before it startes
   the process. This is of course done by Dos(32)LoadModule which
   will do some thunking in Doscall1.dll and go to a 16-bit
   kernel worker. But doscall1.dll isn't initiated and hence the
   thunking failes after some high memory thunking changes.
   This is harder to fix. Current fix calls the doscall1.dll init
   proc before loading calling DosLoadModule. This kind of works.

Then there are some unstability in the debug reader (DDE4XRDR.DLL) or icsperfs use of this. But that is fixable. Currently I can install a conditional breakpoint in the kerneldebugger to get around this.

But when tracking odin based apps, some pointer gets schrewed up during trace collection in the debugee and this traps both icsperf and the debuggee. It might have to do with high memory address. I don't currently know.










These are mine notes. for my own information..
------------------------------------------------

ICSPERF.EXE:

Trap 14 (0EH) - Page Fault 0004, Not Present, Read Access, User Mode
eax=00000000 ebx=17ae0630 ecx=17ae0630 edx=00000000 esi=01999900 edi=00000000
eip=00037fb2 esp=0195f7cc ebp=0195f858 iopl=2 rf -- -- nv up ei pl nz na po nc
cs=005b ss=0053 ds=0053 es=0053 fs=150b gs=13ab  cr2=00000000  cr3=00212000
005b:00037fb2 8b18           mov       ebx,dword ptr [eax]  ds:00000000=invalid

FIX:
r eip 00037f5c ; r esp esp + 4 ; g
