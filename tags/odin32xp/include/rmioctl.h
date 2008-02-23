/**************************************************************************
 *
 * SOURCE FILE NAME =  RMIOCTL.H
 *
 * DESCRIPTIVE NAME =  RM defines and structures related to RM IOCTLs
 *
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1994, 1995
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *             RESTRICTED MATERIALS OF IBM
 *             IBM CONFIDENTIAL
 *
 * VERSION = V1.01
 *
 * DATE
 *
 * DESCRIPTION :
 *
 * Purpose:
 *
 *
 *
 * FUNCTIONS  :
 *
 * NOTES
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 *
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *   DATE      FLAG        APAR   CHANGE DESCRIPTION
 *   --------  ----------  -----  --------------------------------------
 *
 ****************************************************************************/
#ifndef __RM_HEADER__
#include "rmbase.h"
#endif

#ifndef __RM_IOCTL__
#define __RM_IOCTL__

#define CAT_RM                    0x80      /* Resource Manager Category  */
#define FUNC_RM_GET_NODEINFO      0x01      /* Get RM Structure           */
#define FUNC_RM_ENUM_NODES        0x02      /* Get Physical Topology      */
#define FUNC_RM_ENUM_DRIVERS      0x03      /* Get DriverHandles          */


/******************************************/
/* RM_NODE   Ring 3 Super Structure       */
/******************************************/

typedef struct {
   ULONG          Count;
   RESOURCESTRUCT Resource[1];
} RESOURCELIST, FAR *PRESOURCELIST, NEAR *NPRESOURCELIST;

typedef struct {

   ULONG             VersionInfo;
   ULONG             NodeType;
   RMHANDLE          DriverHandle;
   union {
     PADAPTERSTRUCT  pAdapterNode;
     PDEVICESTRUCT   pDeviceNode;
     PLDEVSTRUCT     pLDevNode;
     PSYSNAMESTRUCT  pSysNameNode;
     PDRIVERSTRUCT   pDriverNode;
     PDETECTEDSTRUCT pDetectedNode;
     PRESOURCESTRUCT pResourceNode;
   };
   PRESOURCELIST     pResourceList;

} RM_NODE, FAR *PRM_NODE, NEAR *NPRM_NODE;

/* pRM_NODE->NodeType */

#define RMTYPE_ADAPTER    0x0000
#define RMTYPE_DEVICE     0x0001
#define RMTYPE_LDEV       0x0002
#define RMTYPE_SYSNAME    0x0003
#define RMTYPE_DRIVER     0x0004
#define RMTYPE_DETECTED   0x0005
#define RMTYPE_RESOURCE   0x0006

/**************************/
/* FUNC_RM_GETNODEINFO    */
/**************************/

typedef struct {
   RMHANDLE RMHandle;
   ULONG    Linaddr;
} RM_GETNODE_PARM, FAR *PRM_GETNODE_PARM, NEAR *NPRM_GETNODE_PARM;

typedef struct {
   ULONG          RMNodeSize;
   RM_NODE        RMNode;
} RM_GETNODE_DATA, FAR *PRM_GETNODE_DATA, NEAR *NPRM_GETNODE_DATA;


#define MAX_RESOURCES  10
#define MAX_TEXT_DATA 128

#define MAX_RM_NODE_SIZE ( sizeof(RM_NODE) +                          \
                           sizeof(ADAPTERSTRUCT)  + MAX_TEXT_DATA +  \
                           sizeof(DRIVERSTRUCT)   + MAX_TEXT_DATA +  \
                           sizeof(RESOURCESTRUCT) * MAX_RESOURCES       )


/**************************/
/* FUNC_RM_ENUM_NODE      */
/**************************/

typedef struct {
   USHORT   Command;       /* Size of Data Packet      */
} RM_ENUMNODES_PARM, FAR *PRM_ENUMNODES_PARM, NEAR *NPRM_ENUMNODES_PARM;

/* pRM_ENUMNODES_PARM->Command */
#define RM_COMMAND_PHYS    0
#define RM_COMMAND_LOG     1
#define RM_COMMAND_DRVR    2
#define RM_COMMAND_PREVIOUS_DETECT  3
#define RM_COMMAND_CURRENT_DETECT   4

typedef struct {
   RMHANDLE RMHandle;
   ULONG    Depth;
} NODEENTRY, FAR *PNODEENTRY, NEAR *NPNODEENTRY;

typedef struct {
   ULONG     NumEntries;        /* Number of Topology Entries       */
   NODEENTRY NodeEntry[1];      /* First Entry in Map of Nodes      */
} RM_ENUMNODES_DATA, FAR *PRM_ENUMNODES_DATA, NEAR *NPRM_ENUMNODES_DATA;

#define MAX_ENUM_SIZE ( sizeof(NODEENTRY) * 255 + sizeof(ULONG))


#endif  /* __RM_IOCTL__ */
