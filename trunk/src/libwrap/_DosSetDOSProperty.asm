        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetDOSProperty:PROC
        PUBLIC  _DosSetDOSProperty
_DosSetDOSProperty PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DosSetDOSProperty
    add    esp, 16
    pop    fs
    ret
_DosSetDOSProperty ENDP

CODE32          ENDS

                END
