        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_timeGetTime:PROC
        PUBLIC  O32_timeGetTime
O32_timeGetTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_timeGetTime
    pop    fs
    ret
O32_timeGetTime ENDP

CODE32          ENDS

                END
