        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosError:PROC
        PUBLIC  _DosError
_DosError PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosError
    add    esp, 4
    pop    fs
    ret
_DosError ENDP

CODE32          ENDS

                END
