/* $Id: winthunk.h,v 1.1 2002-02-08 15:07:16 sandervl Exp $ */
#ifndef __WIN_THUNK_H
#define __WIN_THUNK_H

void  WINAPI FT_Exit0 (CONTEXT86 *context);
void  WINAPI FT_Exit4 (CONTEXT86 *context);
void  WINAPI FT_Exit8 (CONTEXT86 *context);
void  WINAPI FT_Exit12(CONTEXT86 *context);
void  WINAPI FT_Exit16(CONTEXT86 *context);
void  WINAPI FT_Exit20(CONTEXT86 *context);
void  WINAPI FT_Exit24(CONTEXT86 *context);
void  WINAPI FT_Exit28(CONTEXT86 *context);
void  WINAPI FT_Exit32(CONTEXT86 *context);
void  WINAPI FT_Exit36(CONTEXT86 *context);
void  WINAPI FT_Exit40(CONTEXT86 *context);
void  WINAPI FT_Exit44(CONTEXT86 *context);
void  WINAPI FT_Exit48(CONTEXT86 *context);
void  WINAPI FT_Exit52(CONTEXT86 *context);
void  WINAPI FT_Exit56(CONTEXT86 *context);
void  WINAPI SMapLS( CONTEXT86 *context );
void  WINAPI SMapLS_IP_EBP_8 (CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_12(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_16(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_20(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_24(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_28(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_32(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_36(CONTEXT86 *context);
void  WINAPI SMapLS_IP_EBP_40(CONTEXT86 *context);
void  WINAPI SUnMapLS( CONTEXT86 *context );
void  WINAPI SUnMapLS_IP_EBP_8 (CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_12(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_16(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_20(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_24(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_28(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_32(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_36(CONTEXT86 *context);
void  WINAPI SUnMapLS_IP_EBP_40(CONTEXT86 *context);
void  WINAPI QT_Thunk(CONTEXT *context);
DWORD WINAPI MapSLFix(DWORD arg1);
void  WINAPI UnMapSLFixArray( SEGPTR sptr[], INT length, CONTEXT86 *context );
DWORD WINAPI K32Thk1632Prolog(DWORD arg1);
DWORD WINAPI K32Thk1632Epilog(DWORD arg1);
void  WINAPI FT_Thunk( CONTEXT86 *context );
void  WINAPI FT_Prolog(CONTEXT *context);
void  WINAPI UnMapSLFixArray( SEGPTR sptr[], INT length, CONTEXT86 *context );
VOID  WINAPI ReleaseThunkLock(DWORD *mutex_count);
VOID  WINAPI RestoreThunkLock(DWORD mutex_count);
DWORD WINAPI _ConfirmWin16Lock(void);

#endif //__WIN_THUNK_H

