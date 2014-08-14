        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ResumeThread:PROC
        PUBLIC  O32_ResumeThread
O32_ResumeThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ResumeThread
    add    esp, 4
    pop    fs
    ret
O32_ResumeThread ENDP

CODE32          ENDS

                END
