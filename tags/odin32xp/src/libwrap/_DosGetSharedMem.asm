        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosGetSharedMem:PROC
        PUBLIC  _DosGetSharedMem
_DosGetSharedMem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DosGetSharedMem
    add    esp, 8
    pop    fs
    ret
_DosGetSharedMem ENDP

CODE32          ENDS

                END
