        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetElementPointerAtLabel:PROC
        PUBLIC  _GpiSetElementPointerAtLabel
_GpiSetElementPointerAtLabel PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetElementPointerAtLabel
    add    esp, 8
    pop    fs
    ret
_GpiSetElementPointerAtLabel ENDP

CODE32          ENDS

                END
