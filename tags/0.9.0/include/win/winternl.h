#ifndef __WINE_WINTERNL_H__
#define __WINE_WINTERNL_H__

#include <ntddk.h>

#define InitializeObjectAttributes(p,n,a,r,s) \
    do { \
        (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
        (p)->RootDirectory = r; \
        (p)->Attributes = a; \
        (p)->ObjectName = n; \
        (p)->SecurityDescriptor = s; \
        (p)->SecurityQualityOfService = NULL; \
    } while (0)



#endif /* __WINE_WINTERNL_H__ */
