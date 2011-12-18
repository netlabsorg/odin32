        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetLogicalDrives:PROC
        PUBLIC  O32_GetLogicalDrives
O32_GetLogicalDrives PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetLogicalDrives
    pop    fs
    ret
O32_GetLogicalDrives ENDP

CODE32          ENDS

                END
