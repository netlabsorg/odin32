        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPel:PROC
        PUBLIC  _GpiQueryPel
_GpiQueryPel PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryPel
    add    esp, 8
    pop    fs
    ret
_GpiQueryPel ENDP

CODE32          ENDS

                END
