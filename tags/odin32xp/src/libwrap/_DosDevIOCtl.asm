        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosDevIOCtl:PROC
        PUBLIC  _DosDevIOCtl
_DosDevIOCtl PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    push   dword ptr [esp+40]
    call   DosDevIOCtl
    add    esp, 36
    pop    fs
    ret
_DosDevIOCtl ENDP

CODE32          ENDS

                END
