        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetCurrentDir:PROC
        PUBLIC  _DosSetCurrentDir
_DosSetCurrentDir PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSetCurrentDir
    add    esp, 4
    pop    fs
    ret
_DosSetCurrentDir ENDP

CODE32          ENDS

                END
