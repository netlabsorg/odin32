        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetDefAttrs:PROC
        PUBLIC  _GpiSetDefAttrs
_GpiSetDefAttrs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiSetDefAttrs
    add    esp, 16
    pop    fs
    ret
_GpiSetDefAttrs ENDP

CODE32          ENDS

                END
