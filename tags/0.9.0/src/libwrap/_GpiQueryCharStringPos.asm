        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharStringPos:PROC
        PUBLIC  _GpiQueryCharStringPos
_GpiQueryCharStringPos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiQueryCharStringPos
    add    esp, 24
    pop    fs
    ret
_GpiQueryCharStringPos ENDP

CODE32          ENDS

                END
