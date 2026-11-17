/* opengl32 - shared declarations between the generated glwrap.c and wgl.cpp */
#ifndef GLWRAP_H
#define GLWRAP_H

#ifdef __cplusplus
extern "C" {
#endif

extern void *glw_fn[];             /* cdecl entry points in MesaGL7.dll, indexed like glw_names */
extern const int glw_count;
extern const char * const glw_names[];

#ifdef __cplusplus
}
#endif

#endif
