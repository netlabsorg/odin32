        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiOffsetElementPointer:PROC
        PUBLIC  _GpiOffsetElementPointer
_GpiOffsetElementPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiOffsetElementPointer
    add    esp, 8
    pop    fs
    ret
_GpiOffsetElementPointer ENDP

CODE32          ENDS

                END
