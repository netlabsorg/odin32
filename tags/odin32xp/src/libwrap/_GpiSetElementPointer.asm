        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetElementPointer:PROC
        PUBLIC  _GpiSetElementPointer
_GpiSetElementPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetElementPointer
    add    esp, 8
    pop    fs
    ret
_GpiSetElementPointer ENDP

CODE32          ENDS

                END
