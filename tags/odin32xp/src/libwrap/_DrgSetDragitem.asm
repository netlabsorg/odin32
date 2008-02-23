        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgSetDragitem:PROC
        PUBLIC  _DrgSetDragitem
_DrgSetDragitem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DrgSetDragitem
    add    esp, 16
    pop    fs
    ret
_DrgSetDragitem ENDP

CODE32          ENDS

                END
