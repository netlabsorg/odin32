/** @file
 * Memory RTL function wrappers for GCC.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <odin.h>
#include <os2sel.h>

#include <memory.h>

#ifdef DEBUG
unsigned long nrcalls_malloc = 0;
unsigned long nrcalls_free   = 0;
unsigned long totalmemalloc  = 0;

// currently, it's a dumb stub
void _LNK_CONV getcrtstat(unsigned long *pnrcalls_malloc,
                          unsigned long *pnrcalls_free,
                          unsigned long *ptotalmemalloc)
{
    *pnrcalls_malloc = nrcalls_malloc;
    *pnrcalls_free   = nrcalls_free;
    *ptotalmemalloc  = totalmemalloc;
}
#endif

extern "C"
{

// these are GCC kLIBC exports
void *_std_malloc(size_t sz);
void *_std_realloc(void *ptr, size_t sz);
void *_std_calloc(size_t cnt, size_t sz);
void _std_free(void *ptr);

void *malloc(size_t sz)
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
	return ptr;
}

void free(void *ptr)
{
    unsigned short sel = RestoreOS2FS();
	_std_free(ptr);
	SetFS(sel);
}

}
