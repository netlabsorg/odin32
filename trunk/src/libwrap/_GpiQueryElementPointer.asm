        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryElementPointer:PROC
        PUBLIC  _GpiQueryElementPointer
_GpiQueryElementPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryElementPointer
    add    esp, 4
    pop    fs
    ret
_GpiQueryElementPointer ENDP

CODE32          ENDS

                END
