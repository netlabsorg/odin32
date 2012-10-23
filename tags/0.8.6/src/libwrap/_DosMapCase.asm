        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosMapCase:PROC
        PUBLIC  _DosMapCase
_DosMapCase PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosMapCase
    add    esp, 12
    pop    fs
    ret
_DosMapCase ENDP

CODE32          ENDS

                END
