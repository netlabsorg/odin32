        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiFillPath:PROC
        PUBLIC  _GpiFillPath
_GpiFillPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiFillPath
    add    esp, 12
    pop    fs
    ret
_GpiFillPath ENDP

CODE32          ENDS

                END
