        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCharString:PROC
        PUBLIC  _GpiCharString
_GpiCharString PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiCharString
    add    esp, 12
    pop    fs
    ret
_GpiCharString ENDP

CODE32          ENDS

                END
