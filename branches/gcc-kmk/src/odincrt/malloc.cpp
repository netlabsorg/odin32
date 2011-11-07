/** @file
 * Memory RTL function wrappers for GCC.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <os2sel.h>

extern "C"
{

// these are GCC kLIBC exports
void _std_malloc(size_t sz);
void *_std_realloc(void *ptr, size_t sz);
void *_std_calloc(size_t cnt, size_t sz);
void _std_free(void *ptr);

void malloc(size_t sz)
{
    unsigned short sel = RestoreOS2FS();
    void *ptr = _std_malloc(sz);
	SetFS(sel);
	return ptr;
}

void *realloc(void *ptr, size_t sz)
{
    unsigned short sel = RestoreOS2FS();
    void *newPtr = _std_realloc(ptr, sz);
	SetFS(sel);
	return newPtr;
}

void *calloc(size_t cnt, size_t sz)
{
    unsigned short sel = RestoreOS2FS();
    void *ptr = _std_calloc(cnt, sz);
	SetFS(sel);
	return rc;
}

void free(void *ptr)
{
    unsigned short sel = RestoreOS2FS();
	_std_free(ptr);
	SetFS(sel);
}

}
