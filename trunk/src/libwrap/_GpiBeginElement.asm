        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiBeginElement:PROC
        PUBLIC  _GpiBeginElement
_GpiBeginElement PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiBeginElement
    add    esp, 12
    pop    fs
    ret
_GpiBeginElement ENDP

CODE32          ENDS

                END
