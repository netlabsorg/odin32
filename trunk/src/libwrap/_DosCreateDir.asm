        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCreateDir:PROC
        PUBLIC  _DosCreateDir
_DosCreateDir PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosCreateDir
    add    esp, 8
    pop    fs
    ret
_DosCreateDir ENDP

CODE32          ENDS

                END
