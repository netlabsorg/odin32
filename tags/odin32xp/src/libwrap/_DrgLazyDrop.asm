        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgLazyDrop:PROC
        PUBLIC  _DrgLazyDrop
_DrgLazyDrop PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DrgLazyDrop
    add    esp, 12
    pop    fs
    ret
_DrgLazyDrop ENDP

CODE32          ENDS

                END
