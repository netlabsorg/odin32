










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
