        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetDialogBaseUnits:PROC
        PUBLIC  O32_GetDialogBaseUnits
O32_GetDialogBaseUnits PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetDialogBaseUnits
    pop    fs
    ret
O32_GetDialogBaseUnits ENDP

CODE32          ENDS

                END
