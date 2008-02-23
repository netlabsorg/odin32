        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgDeleteDraginfoStrHandles:PROC
        PUBLIC  _DrgDeleteDraginfoStrHandles
_DrgDeleteDraginfoStrHandles PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DrgDeleteDraginfoStrHandles
    add    esp, 4
    pop    fs
    ret
_DrgDeleteDraginfoStrHandles ENDP

CODE32          ENDS

                END
