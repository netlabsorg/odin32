        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetTimeZoneInformation:PROC
        PUBLIC  O32_GetTimeZoneInformation
O32_GetTimeZoneInformation PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetTimeZoneInformation
    add    esp, 4
    pop    fs
    ret
O32_GetTimeZoneInformation ENDP

CODE32          ENDS

                END
