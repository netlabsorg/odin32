        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCharStringPos:PROC
        PUBLIC  _GpiCharStringPos
_GpiCharStringPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiCharStringPos
    add    esp, 24
    pop    fs
    ret
_GpiCharStringPos ENDP

CODE32          ENDS

                END
