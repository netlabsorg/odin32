        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGetDateTime:PROC
        PUBLIC  _DosGetDateTime
_DosGetDateTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosGetDateTime
    add    esp, 4
    pop    fs
    ret
_DosGetDateTime ENDP

CODE32          ENDS

                END
