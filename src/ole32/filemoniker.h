/* $Id: filemoniker.h,v 1.1 1999-09-24 21:49:43 davidr Exp $ */
/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef FILEMONIKER_INCLUDED
#define FILEMONIKER_INCLUDED 1

/* filemoniker data structure */
typedef struct FileMonikerImpl{

    ICOM_VTABLE(IMoniker)*  lpvtbl1;  /* VTable relative to the IMoniker interface.*/

    /* The ROT (RunningObjectTable implementation) uses the IROTData interface to test whether 
     * two monikers are equal. That's whay IROTData interface is implemented by monikers.
     */
    ICOM_VTABLE(IROTData)*  lpvtbl2;  /* VTable relative to the IROTData interface.*/

    ULONG ref; /* reference counter for this object */

    LPOLESTR filePathName; /* path string identified by this filemoniker */

} FileMonikerImpl;

/* Local function used by filemoniker implementation */
HRESULT WINAPI FileMonikerImpl_Construct(FileMonikerImpl* iface, LPCOLESTR lpszPathName);
HRESULT WINAPI FileMonikerImpl_Destroy(FileMonikerImpl* iface);
int     WINAPI FileMonikerImpl_DecomposePath(LPOLESTR str, LPOLESTR** tabStr);

#endif /* FILEMONIKER_INCLUDED */
