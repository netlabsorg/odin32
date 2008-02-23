        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_VkKeyScan:PROC
        PUBLIC  O32_VkKeyScan
O32_VkKeyScan PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_VkKeyScan
    add    esp, 4
    pop    fs
    ret
O32_VkKeyScan ENDP

CODE32          ENDS

                END
