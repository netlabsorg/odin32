        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_OutputDebugString:PROC
        PUBLIC  O32_OutputDebugString
O32_OutputDebugString PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_OutputDebugString
    add    esp, 4
    pop    fs
    ret
O32_OutputDebugString ENDP

CODE32          ENDS

                END
