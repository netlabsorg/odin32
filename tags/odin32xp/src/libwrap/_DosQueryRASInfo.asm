        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryRASInfo:PROC
        PUBLIC  _DosQueryRASInfo
_DosQueryRASInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosQueryRASInfo
    add    esp, 8
    pop    fs
    ret
_DosQueryRASInfo ENDP

CODE32          ENDS

                END
