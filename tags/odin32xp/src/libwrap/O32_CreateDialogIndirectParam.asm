        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateDialogIndirectParam:PROC
        PUBLIC  O32_CreateDialogIndirectParam
O32_CreateDialogIndirectParam PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_CreateDialogIndirectParam
    add    esp, 20
    pop    fs
    ret
O32_CreateDialogIndirectParam ENDP

CODE32          ENDS

                END
