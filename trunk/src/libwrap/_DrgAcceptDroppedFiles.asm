        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DrgAcceptDroppedFiles:PROC
        PUBLIC  _DrgAcceptDroppedFiles
_DrgAcceptDroppedFiles PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DrgAcceptDroppedFiles
    add    esp, 20
    pop    fs
    ret
_DrgAcceptDroppedFiles ENDP

CODE32          ENDS

                END
