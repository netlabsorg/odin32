        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosStartTimer:PROC
        PUBLIC  _DosStartTimer
_DosStartTimer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosStartTimer
    add    esp, 12
    pop    fs
    ret
_DosStartTimer ENDP

CODE32          ENDS

                END
