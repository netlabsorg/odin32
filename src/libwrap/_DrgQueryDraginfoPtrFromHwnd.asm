        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgQueryDraginfoPtrFromHwnd:PROC
        PUBLIC  _DrgQueryDraginfoPtrFromHwnd
_DrgQueryDraginfoPtrFromHwnd PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgQueryDraginfoPtrFromHwnd
    add    esp, 4
    pop    fs
    ret
_DrgQueryDraginfoPtrFromHwnd ENDP

CODE32          ENDS

                END
