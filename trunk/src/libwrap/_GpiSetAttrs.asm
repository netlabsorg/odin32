        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetAttrs:PROC
        PUBLIC  _GpiSetAttrs
_GpiSetAttrs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiSetAttrs
    add    esp, 20
    pop    fs
    ret
_GpiSetAttrs ENDP

CODE32          ENDS

                END
