        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetTag:PROC
        PUBLIC  _GpiSetTag
_GpiSetTag PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetTag
    add    esp, 8
    pop    fs
    ret
_GpiSetTag ENDP

CODE32          ENDS

                END
