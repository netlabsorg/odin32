        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_UnpackDDElParam:PROC
        PUBLIC  O32_UnpackDDElParam
O32_UnpackDDElParam PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_UnpackDDElParam
    add    esp, 16
    pop    fs
    ret
O32_UnpackDDElParam ENDP

CODE32          ENDS

                END
