#ifndef __ODINTRACE_H__
#define __ODINTRACE_H__

//Release-build API trace, enabled by the environment variable ODIN_TRACE.
void OdinTrace(const char *tag, const char *str, DWORD a, DWORD b);
void OdinFSCheck(const char *where, DWORD ra);
#define OFSC(name) OdinFSCheck(name, (DWORD)__builtin_return_address(0))
void OdinTraceModule(const char *tag, DWORD eip);
void OdinTraceW(const char *tag, const WCHAR *wstr, DWORD a, DWORD b);

#endif
