        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetTimeZoneInformation:PROC
        PUBLIC  O32_SetTimeZoneInformation
O32_SetTimeZoneInformation PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetTimeZoneInformation
    add    esp, 4
    pop    fs
    ret
O32_SetTimeZoneInformation ENDP

CODE32          ENDS

                END
