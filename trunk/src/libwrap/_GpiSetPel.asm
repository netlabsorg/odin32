        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPel:PROC
        PUBLIC  _GpiSetPel
_GpiSetPel PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetPel
    add    esp, 8
    pop    fs
    ret
_GpiSetPel ENDP

CODE32          ENDS

                END
