        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiBeginArea:PROC
        PUBLIC  _GpiBeginArea
_GpiBeginArea PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiBeginArea
    add    esp, 8
    pop    fs
    ret
_GpiBeginArea ENDP

CODE32          ENDS

                END
