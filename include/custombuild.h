#ifndef __CUSTOMBUILD_H__
#define __CUSTOMBUILD_H__

extern BOOL fCustomBuild;

void InitDirectoriesCustom(char *szSystemDir, char *szWindowsDir);

void DisableOdinIni();
void DisableOdinSysMenuItems();

#endif  /*__CUSTOMBUILD_H__*/

