        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetDlgCtrlID:PROC
        PUBLIC  O32_GetDlgCtrlID
O32_GetDlgCtrlID PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetDlgCtrlID
    add    esp, 4
    pop    fs
    ret
O32_GetDlgCtrlID ENDP

CODE32          ENDS

                END
