        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CommDlgExtendedError:PROC
        PUBLIC  O32_CommDlgExtendedError
O32_CommDlgExtendedError PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_CommDlgExtendedError
    pop    fs
    ret
O32_CommDlgExtendedError ENDP

CODE32          ENDS

                END
