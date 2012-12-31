        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryEditMode:PROC
        PUBLIC  _GpiQueryEditMode
_GpiQueryEditMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryEditMode
    add    esp, 4
    pop    fs
    ret
_GpiQueryEditMode ENDP

CODE32          ENDS

                END
