        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosClose:PROC
        PUBLIC  _DosClose
_DosClose PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosClose
    add    esp, 4
    pop    fs
    ret
_DosClose ENDP

CODE32          ENDS

                END
