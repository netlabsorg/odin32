        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryElementType:PROC
        PUBLIC  _GpiQueryElementType
_GpiQueryElementType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiQueryElementType
    add    esp, 16
    pop    fs
    ret
_GpiQueryElementType ENDP

CODE32          ENDS

                END
