        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetEditMode:PROC
        PUBLIC  _GpiSetEditMode
_GpiSetEditMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetEditMode
    add    esp, 8
    pop    fs
    ret
_GpiSetEditMode ENDP

CODE32          ENDS

                END
