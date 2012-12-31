        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DevCloseDC:PROC
        PUBLIC  _DevCloseDC
_DevCloseDC PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DevCloseDC
    add    esp, 4
    pop    fs
    ret
_DevCloseDC ENDP

CODE32          ENDS

                END
