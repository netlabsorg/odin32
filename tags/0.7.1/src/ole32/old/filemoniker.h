/* $Id: filemoniker.h,v 1.1 2001-04-26 19:26:10 sandervl Exp $ */
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
    BOOL bIsLongFileName;  /* Is the path a valid dos 8.3 file */
    BOOL bIsNetworkPath;   /* Is the path a network path */
    WORD wNetworkDomainLenght;  /*Lenght of the \\Domain\share network name, otherwise 0xFFFF for regular file*/

} FileMonikerImpl;

/* Local function used by filemoniker implementation */
HRESULT WINAPI FileMonikerImpl_Construct(FileMonikerImpl* iface, LPCOLESTR lpszPathName);
HRESULT WINAPI FileMonikerImpl_Destroy(FileMonikerImpl* iface);
int     WINAPI FileMonikerImpl_DecomposePath(LPOLESTR str, LPOLESTR** tabStr);
void WINAPI FileMonikerImpl_CheckFileFormat(FileMonikerImpl* iface, LPCOLESTR lpszPathName);
HRESULT FileMonikerImpl_GetSizeToSave(IMoniker* iface,ULARGE_INTEGER* pcbSize);

#endif /* FILEMONIKER_INCLUDED */
