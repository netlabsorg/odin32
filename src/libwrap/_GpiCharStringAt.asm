        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCharStringAt:PROC
        PUBLIC  _GpiCharStringAt
_GpiCharStringAt PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiCharStringAt
    add    esp, 16
    pop    fs
    ret
_GpiCharStringAt ENDP

CODE32          ENDS

                END
