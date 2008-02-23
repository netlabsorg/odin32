        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgSetDragPointer:PROC
        PUBLIC  _DrgSetDragPointer
_DrgSetDragPointer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   DrgSetDragPointer
    add    esp, 8
    pop    fs
    ret
_DrgSetDragPointer ENDP

CODE32          ENDS

                END
