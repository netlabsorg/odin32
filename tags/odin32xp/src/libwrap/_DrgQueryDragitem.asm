        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDragitem:PROC
        PUBLIC  _DrgQueryDragitem
_DrgQueryDragitem PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   DrgQueryDragitem
    add    esp, 16
    pop    fs
    ret
_DrgQueryDragitem ENDP

CODE32          ENDS

                END
