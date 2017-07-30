        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetCharExtra:PROC
        PUBLIC  _GpiSetCharExtra
_GpiSetCharExtra PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetCharExtra
    add    esp, 8
    pop    fs
    ret
_GpiSetCharExtra ENDP

CODE32          ENDS

                END
