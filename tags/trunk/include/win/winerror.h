/* $Id: winerror.h,v 1.4 1999-11-08 13:46:45 sandervl Exp $ */
#ifndef __WINE_WINERROR_H
#define __WINE_WINERROR_H


extern int WIN32_LastError;

#define FACILITY_NULL      0
#define FACILITY_RPC       1
#define FACILITY_DISPATCH  2
#define FACILITY_STORAGE   3
#define FACILITY_ITF		4
#define FACILITY_WIN32		7
#define FACILITY_WINDOWS   8
#define FACILITY_SSPI      9
#define FACILITY_CONTROL   10
#define FACILITY_CERT      11
#define FACILITY_INTERNET  12

#define SEVERITY_ERROR		1


#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define MAKE_SCODE(sev,fac,code) \
        ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define SUCCEEDED(stat) ((HRESULT)(stat)>=0)
#define FAILED(stat) ((HRESULT)(stat)<0)

#define HRESULT_CODE(hr) ((hr) & 0xFFFF)
#define SCODE_CODE(sc)   ((sc) & 0xFFFF)

#define HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1FFF)
#define SCODE_FACILITY(sc)  (((sc) >> 16) & 0x1FFF)

/* ERROR_UNKNOWN is a placeholder for error conditions which haven't
 * been tested yet so we're not exactly sure what will be returned.
 * All instances of ERROR_UNKNOWN should be tested under Win95/NT
 * and replaced.
 */
#define ERROR_UNKNOWN               99999

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1

#define NO_ERROR                    0
#define ERROR_SUCCESS               0
#define ERROR_INVALID_FUNCTION      1
#define ERROR_FILE_NOT_FOUND        2
#define ERROR_PATH_NOT_FOUND        3
#define ERROR_TOO_MANY_OPEN_FILES   4
#define ERROR_ACCESS_DENIED         5
#define ERROR_INVALID_HANDLE        6
#define ERROR_ARENA_TRASHED         7
#define ERROR_NOT_ENOUGH_MEMORY     8
#define ERROR_INVALID_BLOCK         9
#define ERROR_BAD_ENVIRONMENT       10
#define ERROR_BAD_FORMAT            11
#define ERROR_INVALID_ACCESS        12
#define ERROR_INVALID_DATA          13
#define ERROR_OUTOFMEMORY           14
#define ERROR_INVALID_DRIVE         15
#define ERROR_CURRENT_DIRECTORY     16
#define ERROR_NOT_SAME_DEVICE       17
#define ERROR_NO_MORE_FILES         18
#define ERROR_WRITE_PROTECT         19
#define ERROR_BAD_UNIT              20
#define ERROR_NOT_READY             21
#define ERROR_BAD_COMMAND           22
#define ERROR_CRC                   23
#define ERROR_BAD_LENGTH            24
#define ERROR_SEEK                  25
#define ERROR_NOT_DOS_DISK          26
#define ERROR_SECTOR_NOT_FOUND      27
#define ERROR_WRITE_FAULT           29
#define ERROR_READ_FAULT            30
#define ERROR_GEN_FAILURE           31
#define ERROR_SHARING_VIOLATION     32
#define ERROR_LOCK_VIOLATION        33
#define ERROR_WRONG_DISK            34
#define ERROR_SHARING_BUFFER_EXCEEDED 36
#define ERROR_HANDLE_EOF            38
#define ERROR_HANDLE_DISK_FULL      39
#define ERROR_NOT_SUPPORTED         50
#define ERROR_REM_NOT_LIST          51
#define ERROR_DUP_NAME              52
#define ERROR_BAD_NETPATH           53
#define ERROR_NETWORK_BUSY          54
#define ERROR_DEV_NOT_EXIST         55
#define ERROR_ADAP_HDW_ERR          57
#define ERROR_BAD_NET_RESP          58
#define ERROR_UNEXP_NET_ERR         59
#define ERROR_BAD_REM_ADAP          60
#define ERROR_PRINTQ_FULL           61
#define ERROR_NO_SPOOL_SPACE        62
#define ERROR_PRINT_CANCELLED       63
#define ERROR_NETNAME_DELETED       64
#define ERROR_NETWORK_ACCESS_DENIED 65
#define ERROR_BAD_DEV_TYPE          66
#define ERROR_BAD_NET_NAME          67
#define ERROR_TOO_MANY_NAMES        68
#define ERROR_TOO_MANY_SESS         69
#define ERROR_SHARING_PAUSED        70
#define ERROR_REQ_NOT_ACCEP         71
#define ERROR_REDIR_PAUSED          72
#define ERROR_FILE_EXISTS           80
#define ERROR_CANNOT_MAKE           82
#define ERROR_FAIL_I24              83
#define ERROR_OUT_OF_STRUCTURES     84
#define ERROR_ALREADY_ASSIGNED      85
#define ERROR_INVALID_PASSWORD      86
#define ERROR_INVALID_PARAMETER     87
#define ERROR_NET_WRITE_FAULT       88
#define ERROR_NO_PROC_SLOTS         89
#define ERROR_TOO_MANY_SEMAPHORES   100
#define ERROR_EXCL_SEM_ALREADY_OWNED 101
#define ERROR_SEM_IS_SET            102
#define ERROR_TOO_MANY_SEM_REQUESTS 103
#define ERROR_INVALID_AT_INTERRUPT_TIME 104
#define ERROR_SEM_OWNER_DIED        105
#define ERROR_SEM_USER_LIMIT        106
#define ERROR_DISK_CHANGE           107
#define ERROR_DRIVE_LOCKED          108
#define ERROR_BROKEN_PIPE           109
#define ERROR_OPEN_FAILED           110
#define ERROR_BUFFER_OVERFLOW       111
#define ERROR_DISK_FULL             112
#define ERROR_NO_MORE_SEARCH_HANDLES 113
#define ERROR_INVALID_TARGET_HANDLE 114
#define ERROR_INVALID_CATEGORY      117
#define ERROR_INVALID_VERIFY_SWITCH 118
#define ERROR_BAD_DRIVER_LEVEL      119
#define ERROR_CALL_NOT_IMPLEMENTED  120
#define ERROR_SEM_TIMEOUT           121
#define ERROR_INSUFFICIENT_BUFFER   122
#define ERROR_INVALID_NAME          123
#define ERROR_INVALID_LEVEL         124
#define ERROR_NO_VOLUME_LABEL       125
#define ERROR_MOD_NOT_FOUND         126
#define ERROR_PROC_NOT_FOUND        127
#define ERROR_WAIT_NO_CHILDREN      128
#define ERROR_CHILD_NOT_COMPLETE    129
#define ERROR_DIRECT_ACCESS_HANDLE  130
#define ERROR_NEGATIVE_SEEK         131
#define ERROR_SEEK_ON_DEVICE        132
#define ERROR_IS_JOIN_TARGET        133
#define ERROR_IS_JOINED             134
#define ERROR_IS_SUBSTED            135
#define ERROR_NOT_JOINED            136
#define ERROR_NOT_SUBSTED           137
#define ERROR_JOIN_TO_JOIN          138
#define ERROR_SUBST_TO_SUBST        139
#define ERROR_JOIN_TO_SUBST         140
#define ERROR_SUBST_TO_JOIN         141
#define ERROR_BUSY_DRIVE            142
#define ERROR_SAME_DRIVE            143
#define ERROR_DIR_NOT_ROOT          144
#define ERROR_DIR_NOT_EMPTY         145
#define ERROR_IS_SUBST_PATH         146
#define ERROR_IS_JOIN_PATH          147
#define ERROR_PATH_BUSY             148
#define ERROR_IS_SUBST_TARGET       149
#define ERROR_SYSTEM_TRACE          150
#define ERROR_INVALID_EVENT_COUNT   151
#define ERROR_TOO_MANY_MUXWAITERS   152
#define ERROR_INVALID_LIST_FORMAT   153
#define ERROR_LABEL_TOO_LONG        154
#define ERROR_TOO_MANY_TCBS         155
#define ERROR_SIGNAL_REFUSED        156
#define ERROR_DISCARDED             157
#define ERROR_NOT_LOCKED            158
#define ERROR_BAD_THREADID_ADDR     159
#define ERROR_BAD_ARGUMENTS         160
#define ERROR_BAD_PATHNAME          161
#define ERROR_SIGNAL_PENDING        162
#define ERROR_MAX_THRDS_REACHED     164
#define ERROR_LOCK_FAILED           167
#define ERROR_BUSY                  170
#define ERROR_CANCEL_VIOLATION      173
#define ERROR_ATOMIC_LOCKS_NOT_SUPPORTED 174
#define ERROR_INVALID_SEGMENT_NUMBER 180
#define ERROR_INVALID_ORDINAL       182
#define ERROR_ALREADY_EXISTS        183
#define ERROR_INVALID_FLAG_NUMBER   186
#define ERROR_SEM_NOT_FOUND         187
#define ERROR_INVALID_STARTING_CODESEG 188
#define ERROR_INVALID_STACKSEG      189
#define ERROR_INVALID_MODULETYPE    190
#define ERROR_INVALID_EXE_SIGNATURE 191
#define ERROR_EXE_MARKED_INVALID    192
#define ERROR_BAD_EXE_FORMAT        193
#define ERROR_ITERATED_DATA_EXCEEDS_64k 194
#define ERROR_INVALID_MINALLOCSIZE  195
#define ERROR_DYNLINK_FROM_INVALID_RING 196
#define ERROR_IOPL_NOT_ENABLED      197
#define ERROR_INVALID_SEGDPL        198
#define ERROR_AUTODATASEG_EXCEEDS_64k 199
#define ERROR_RING2SEG_MUST_BE_MOVABLE 200
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM 201
#define ERROR_INFLOOP_IN_RELOC_CHAIN 202
#define ERROR_ENVVAR_NOT_FOUND      203
#define ERROR_NO_SIGNAL_SENT        205
#define ERROR_FILENAME_EXCED_RANGE  206
#define ERROR_RING2_STACK_IN_USE    207
#define ERROR_META_EXPANSION_TOO_LONG 208
#define ERROR_INVALID_SIGNAL_NUMBER 209
#define ERROR_THREAD_1_INACTIVE     210
#define ERROR_LOCKED                212
#define ERROR_TOO_MANY_MODULES      214
#define ERROR_NESTING_NOT_ALLOWED   215
#define ERROR_EXE_MACHINE_TYPE_MISMATCH 216
#define ERROR_BAD_PIPE              230
#define ERROR_PIPE_BUSY             231
#define ERROR_NO_DATA               232
#define ERROR_PIPE_NOT_CONNECTED    233
#define ERROR_MORE_DATA             234
#define ERROR_VC_DISCONNECTED       240
#define ERROR_INVALID_EA_NAME       254
#define ERROR_EA_LIST_INCONSISTENT  255
#define ERROR_NO_MORE_ITEMS         259
#define ERROR_CANNOT_COPY           266
#define ERROR_DIRECTORY             267
#define ERROR_EAS_DIDNT_FIT         275
#define ERROR_EA_FILE_CORRUPT       276
#define ERROR_EA_TABLE_FULL         277
#define ERROR_INVALID_EA_HANDLE     278
#define ERROR_EAS_NOT_SUPPORTED     282
#define ERROR_NOT_OWNER             288
#define ERROR_TOO_MANY_POSTS        298
#define ERROR_PARTIAL_COPY          299
#define ERROR_OPLOCK_NOT_GRANTED    300
#define ERROR_INVALID_OPLOCK_PROTOCOL 301
#define ERROR_MR_MID_NOT_FOUND      317
#define ERROR_INVALID_ADDRESS       487
#define ERROR_ARITHMETIC_OVERFLOW   534
#define ERROR_PIPE_CONNECTED        535
#define ERROR_PIPE_LISTENING        536
#define ERROR_EA_ACCESS_DENIED      994
#define ERROR_OPERATION_ABORTED     995
#define ERROR_IO_INCOMPLETE         996
#define ERROR_IO_PENDING            997
#define ERROR_NOACCESS              998
#define ERROR_SWAPERROR             999

#define ERROR_STACK_OVERFLOW                  1001L
#define ERROR_INVALID_MESSAGE                 1002L
#define ERROR_CAN_NOT_COMPLETE                1003L
#define ERROR_INVALID_FLAGS                   1004L
#define ERROR_UNRECOGNIZED_VOLUME             1005L
#define ERROR_FILE_INVALID                    1006L
#define ERROR_FULLSCREEN_MODE                 1007L
#define ERROR_NO_TOKEN                        1008L
#define ERROR_BADDB                           1009L
#define ERROR_BADKEY                          1010L
#define ERROR_CANTOPEN                        1011L
#define ERROR_CANTREAD                        1012L
#define ERROR_CANTWRITE                       1013L
#define ERROR_REGISTRY_RECOVERED              1014L
#define ERROR_REGISTRY_CORRUPT                1015L
#define ERROR_REGISTRY_IO_FAILED              1016L
#define ERROR_NOT_REGISTRY_FILE               1017L
#define ERROR_KEY_DELETED                     1018L
#define ERROR_NO_LOG_SPACE                    1019L
#define ERROR_KEY_HAS_CHILDREN                1020L
#define ERROR_CHILD_MUST_BE_VOLATILE          1021L
#define ERROR_NOTIFY_ENUM_DIR                 1022L
#define ERROR_DEPENDENT_SERVICES_RUNNING      1051L
#define ERROR_INVALID_SERVICE_CONTROL         1052L
#define ERROR_SERVICE_REQUEST_TIMEOUT         1053L
#define ERROR_SERVICE_NO_THREAD               1054L
#define ERROR_SERVICE_DATABASE_LOCKED         1055L
#define ERROR_SERVICE_ALREADY_RUNNING         1056L
#define ERROR_INVALID_SERVICE_ACCOUNT         1057L
#define ERROR_SERVICE_DISABLED                1058L
#define ERROR_CIRCULAR_DEPENDENCY             1059L
#define ERROR_SERVICE_DOES_NOT_EXIST          1060L
#define ERROR_SERVICE_CANNOT_ACCEPT_CTRL      1061L
#define ERROR_SERVICE_NOT_ACTIVE              1062L
#define ERROR_FAILED_SERVICE_CONTROLLER_CONNECT 1063L
#define ERROR_EXCEPTION_IN_SERVICE            1064L
#define ERROR_DATABASE_DOES_NOT_EXIST         1065L
#define ERROR_SERVICE_SPECIFIC_ERROR          1066L
#define ERROR_PROCESS_ABORTED                 1067L
#define ERROR_SERVICE_DEPENDENCY_FAIL         1068L
#define ERROR_SERVICE_LOGON_FAILED            1069L
#define ERROR_SERVICE_START_HANG              1070L
#define ERROR_INVALID_SERVICE_LOCK            1071L
#define ERROR_SERVICE_MARKED_FOR_DELETE       1072L
#define ERROR_SERVICE_EXISTS                  1073L
#define ERROR_ALREADY_RUNNING_LKG             1074L
#define ERROR_SERVICE_DEPENDENCY_DELETED      1075L
#define ERROR_BOOT_ALREADY_ACCEPTED           1076L
#define ERROR_SERVICE_NEVER_STARTED           1077L
#define ERROR_DUPLICATE_SERVICE_NAME          1078L
#define ERROR_END_OF_MEDIA                    1100L
#define ERROR_FILEMARK_DETECTED               1101L
#define ERROR_BEGINNING_OF_MEDIA              1102L
#define ERROR_SETMARK_DETECTED                1103L
#define ERROR_NO_DATA_DETECTED                1104L
#define ERROR_PARTITION_FAILURE               1105L
#define ERROR_INVALID_BLOCK_LENGTH            1106L
#define ERROR_DEVICE_NOT_PARTITIONED          1107L
#define ERROR_UNABLE_TO_LOCK_MEDIA            1108L
#define ERROR_UNABLE_TO_UNLOAD_MEDIA          1109L
#define ERROR_MEDIA_CHANGED                   1110L
#define ERROR_BUS_RESET                       1111L
#define ERROR_NO_MEDIA_IN_DRIVE               1112L
#define ERROR_NO_UNICODE_TRANSLATION          1113L
#define ERROR_DLL_INIT_FAILED                 1114L
#define ERROR_SHUTDOWN_IN_PROGRESS            1115L
#define ERROR_NO_SHUTDOWN_IN_PROGRESS         1116L
#define ERROR_IO_DEVICE                       1117L
#define ERROR_SERIAL_NO_DEVICE                1118L
#define ERROR_IRQ_BUSY                        1119L
#define ERROR_MORE_WRITES                     1120L
#define ERROR_COUNTER_TIMEOUT                 1121L
#define ERROR_FLOPPY_ID_MARK_NOT_FOUND        1122L
#define ERROR_FLOPPY_WRONG_CYLINDER           1123L
#define ERROR_FLOPPY_UNKNOWN_ERROR            1124L
#define ERROR_FLOPPY_BAD_REGISTERS            1125L
#define ERROR_DISK_RECALIBRATE_FAILED         1126L
#define ERROR_DISK_OPERATION_FAILED           1127L
#define ERROR_DISK_RESET_FAILED               1128L
#define ERROR_EOM_OVERFLOW                    1129L
#define ERROR_NOT_ENOUGH_SERVER_MEMORY        1130L
#define ERROR_POSSIBLE_DEADLOCK               1131L
#define ERROR_MAPPED_ALIGNMENT                1132L
#define ERROR_BAD_DEVICE                      1200L
#define ERROR_CONNECTION_UNAVAIL              1201L
#define ERROR_DEVICE_ALREADY_REMEMBERED       1202L
#define ERROR_NO_NET_OR_BAD_PATH              1203L
#define ERROR_BAD_PROVIDER                    1204L
#define ERROR_CANNOT_OPEN_PROFILE             1205L
#define ERROR_BAD_PROFILE                     1206L
#define ERROR_NOT_CONTAINER                   1207L
#define ERROR_EXTENDED_ERROR                  1208L
#define ERROR_INVALID_GROUPNAME               1209L
#define ERROR_INVALID_COMPUTERNAME            1210L
#define ERROR_INVALID_EVENTNAME               1211L
#define ERROR_INVALID_DOMAINNAME              1212L
#define ERROR_INVALID_SERVICENAME             1213L
#define ERROR_INVALID_NETNAME                 1214L
#define ERROR_INVALID_SHARENAME               1215L
#define ERROR_INVALID_PASSWORDNAME            1216L
#define ERROR_INVALID_MESSAGENAME             1217L
#define ERROR_INVALID_MESSAGEDEST             1218L
#define ERROR_SESSION_CREDENTIAL_CONFLICT     1219L
#define ERROR_REMOTE_SESSION_LIMIT_EXCEEDED   1220L
#define ERROR_DUP_DOMAINNAME                  1221L
#define ERROR_NO_NETWORK                      1222L
#define ERROR_CANCELLED                       1223L
#define ERROR_USER_MAPPED_FILE                1224L
#define ERROR_CONNECTION_REFUSED              1225L
#define ERROR_GRACEFUL_DISCONNECT             1226L
#define ERROR_ADDRESS_ALREADY_ASSOCIATED      1227L
#define ERROR_ADDRESS_NOT_ASSOCIATED          1228L
#define ERROR_CONNECTION_INVALID              1229L
#define ERROR_CONNECTION_ACTIVE               1230L
#define ERROR_NETWORK_UNREACHABLE             1231L
#define ERROR_HOST_UNREACHABLE                1232L
#define ERROR_PROTOCOL_UNREACHABLE            1233L
#define ERROR_PORT_UNREACHABLE                1234L
#define ERROR_REQUEST_ABORTED                 1235L
#define ERROR_CONNECTION_ABORTED              1236L
#define ERROR_RETRY                           1237L
#define ERROR_CONNECTION_COUNT_LIMIT          1238L
#define ERROR_LOGIN_TIME_RESTRICTION          1239L
#define ERROR_LOGIN_WKSTA_RESTRICTION         1240L
#define ERROR_INCORRECT_ADDRESS               1241L
#define ERROR_ALREADY_REGISTERED              1242L
#define ERROR_SERVICE_NOT_FOUND               1243L
#define ERROR_NOT_AUTHENTICATED     	      1244L
#define ERROR_NOT_LOGGED_ON                   1245L
#define ERROR_CONTINUE                        1246L
#define ERROR_ALREADY_INITIALIZED             1247L
#define ERROR_NO_MORE_DEVICES                 1248L

#define ERROR_NOT_ALL_ASSIGNED                1300L
#define ERROR_SOME_NOT_MAPPED                 1301L
#define ERROR_NO_QUOTAS_FOR_ACCOUNT           1302L
#define ERROR_LOCAL_USER_SESSION_KEY          1303L
#define ERROR_NULL_LM_PASSWORD                1304L
#define ERROR_UNKNOWN_REVISION                1305L
#define ERROR_REVISION_MISMATCH               1306L
#define ERROR_INVALID_OWNER                   1307L
#define ERROR_INVALID_PRIMARY_GROUP           1308L
#define ERROR_NO_IMPERSONATION_TOKEN          1309L
#define ERROR_CANT_DISABLE_MANDATORY          1310L
#define ERROR_NO_LOGON_SERVERS                1311L
#define ERROR_NO_SUCH_LOGON_SESSION           1312L
#define ERROR_NO_SUCH_PRIVILEGE               1313L
#define ERROR_PRIVILEGE_NOT_HELD              1314L
#define ERROR_INVALID_ACCOUNT_NAME            1315L
#define ERROR_USER_EXISTS                     1316L
#define ERROR_NO_SUCH_USER                    1317L
#define ERROR_GROUP_EXISTS                    1318L
#define ERROR_NO_SUCH_GROUP                   1319L
#define ERROR_MEMBER_IN_GROUP                 1320L
#define ERROR_MEMBER_NOT_IN_GROUP             1321L
#define ERROR_LAST_ADMIN                      1322L
#define ERROR_WRONG_PASSWORD                  1323L
#define ERROR_ILL_FORMED_PASSWORD             1324L
#define ERROR_PASSWORD_RESTRICTION            1325L
#define ERROR_LOGON_FAILURE                   1326L
#define ERROR_ACCOUNT_RESTRICTION             1327L
#define ERROR_INVALID_LOGON_HOURS             1328L
#define ERROR_INVALID_WORKSTATION             1329L
#define ERROR_PASSWORD_EXPIRED                1330L
#define ERROR_ACCOUNT_DISABLED                1331L
#define ERROR_NONE_MAPPED                     1332L
#define ERROR_TOO_MANY_LUIDS_REQUESTED        1333L
#define ERROR_LUIDS_EXHAUSTED                 1334L
#define ERROR_INVALID_SUB_AUTHORITY           1335L
#define ERROR_INVALID_ACL                     1336L
#define ERROR_INVALID_SID                     1337L
#define ERROR_INVALID_SECURITY_DESCR          1338L
#define ERROR_BAD_INHERITANCE_ACL             1340L
#define ERROR_SERVER_DISABLED                 1341L
#define ERROR_SERVER_NOT_DISABLED             1342L
#define ERROR_INVALID_ID_AUTHORITY            1343L
#define ERROR_ALLOTTED_SPACE_EXCEEDED         1344L
#define ERROR_INVALID_GROUP_ATTRIBUTES        1345L
#define ERROR_BAD_IMPERSONATION_LEVEL         1346L
#define ERROR_CANT_OPEN_ANONYMOUS             1347L
#define ERROR_BAD_VALIDATION_CLASS            1348L
#define ERROR_BAD_TOKEN_TYPE                  1349L
#define ERROR_NO_SECURITY_ON_OBJECT           1350L
#define ERROR_CANT_ACCESS_DOMAIN_INFO         1351L
#define ERROR_INVALID_SERVER_STATE            1352L
#define ERROR_INVALID_DOMAIN_STATE            1353L
#define ERROR_INVALID_DOMAIN_ROLE             1354L
#define ERROR_NO_SUCH_DOMAIN                  1355L
#define ERROR_DOMAIN_EXISTS                   1356L
#define ERROR_DOMAIN_LIMIT_EXCEEDED           1357L
#define ERROR_INTERNAL_DB_CORRUPTION          1358L
#define ERROR_INTERNAL_ERROR                  1359L
#define ERROR_GENERIC_NOT_MAPPED              1360L
#define ERROR_BAD_DESCRIPTOR_FORMAT           1361L
#define ERROR_NOT_LOGON_PROCESS               1362L
#define ERROR_LOGON_SESSION_EXISTS            1363L
#define ERROR_NO_SUCH_PACKAGE                 1364L
#define ERROR_BAD_LOGON_SESSION_STATE         1365L
#define ERROR_LOGON_SESSION_COLLISION         1366L
#define ERROR_INVALID_LOGON_TYPE              1367L
#define ERROR_CANNOT_IMPERSONATE              1368L
#define ERROR_RXACT_INVALID_STATE             1369L
#define ERROR_RXACT_COMMIT_FAILURE            1370L
#define ERROR_SPECIAL_ACCOUNT                 1371L
#define ERROR_SPECIAL_GROUP                   1372L
#define ERROR_SPECIAL_USER                    1373L
#define ERROR_MEMBERS_PRIMARY_GROUP           1374L
#define ERROR_TOKEN_ALREADY_IN_USE            1375L
#define ERROR_NO_SUCH_ALIAS                   1376L
#define ERROR_MEMBER_NOT_IN_ALIAS             1377L
#define ERROR_MEMBER_IN_ALIAS                 1378L
#define ERROR_ALIAS_EXISTS                    1379L
#define ERROR_LOGON_NOT_GRANTED               1380L
#define ERROR_TOO_MANY_SECRETS                1381L
#define ERROR_SECRET_TOO_LONG                 1382L
#define ERROR_INTERNAL_DB_ERROR               1383L
#define ERROR_TOO_MANY_CONTEXT_IDS            1384L
#define ERROR_LOGON_TYPE_NOT_GRANTED          1385L
#define ERROR_NT_CROSS_ENCRYPTION_REQUIRED    1386L
#define ERROR_NO_SUCH_MEMBER                  1387L
#define ERROR_INVALID_MEMBER                  1388L
#define ERROR_TOO_MANY_SIDS                   1389L
#define ERROR_LM_CROSS_ENCRYPTION_REQUIRED    1390L
#define ERROR_NO_INHERITANCE                  1391L
#define ERROR_FILE_CORRUPT                    1392L
#define ERROR_DISK_CORRUPT                    1393L
#define ERROR_NO_USER_SESSION_KEY             1394L

#define ERROR_INVALID_WINDOW_HANDLE           1400L
#define ERROR_INVALID_MENU_HANDLE             1401L
#define ERROR_INVALID_CURSOR_HANDLE           1402L
#define ERROR_INVALID_ACCEL_HANDLE            1403L
#define ERROR_INVALID_HOOK_HANDLE             1404L
#define ERROR_INVALID_DWP_HANDLE              1405L
#define ERROR_TLW_WITH_WSCHILD                1406L
#define ERROR_CANNOT_FIND_WND_CLASS           1407L
#define ERROR_WINDOW_OF_OTHER_THREAD          1408L
#define ERROR_HOTKEY_ALREADY_REGISTERED       1409L
#define ERROR_CLASS_ALREADY_EXISTS            1410L
#define ERROR_CLASS_DOES_NOT_EXIST            1411L
#define ERROR_CLASS_HAS_WINDOWS               1412L
#define ERROR_INVALID_INDEX                   1413L
#define ERROR_INVALID_ICON_HANDLE             1414L
#define ERROR_PRIVATE_DIALOG_INDEX            1415L
#define ERROR_LISTBOX_ID_NOT_FOUND            1416L
#define ERROR_NO_WILDCARD_CHARACTERS          1417L
#define ERROR_CLIPBOARD_NOT_OPEN              1418L
#define ERROR_HOTKEY_NOT_REGISTERED           1419L
#define ERROR_WINDOW_NOT_DIALOG               1420L
#define ERROR_CONTROL_ID_NOT_FOUND            1421L
#define ERROR_INVALID_COMBOBOX_MESSAGE        1422L
#define ERROR_WINDOW_NOT_COMBOBOX             1423L
#define ERROR_INVALID_EDIT_HEIGHT             1424L
#define ERROR_DC_NOT_FOUND                    1425L
#define ERROR_INVALID_HOOK_FILTER             1426L
#define ERROR_INVALID_FILTER_PROC             1427L
#define ERROR_HOOK_NEEDS_HMOD                 1428L
#define ERROR_GLOBAL_ONLY_HOOK                1429L
#define ERROR_JOURNAL_HOOK_SET                1430L
#define ERROR_HOOK_NOT_INSTALLED              1431L
#define ERROR_INVALID_LB_MESSAGE              1432L
#define ERROR_SETCOUNT_ON_BAD_LB              1433L
#define ERROR_LB_WITHOUT_TABSTOPS             1434L
#define ERROR_DESTROY_OBJECT_OF_OTHER_THREAD  1435L
#define ERROR_CHILD_WINDOW_MENU               1436L
#define ERROR_NO_SYSTEM_MENU                  1437L
#define ERROR_INVALID_MSGBOX_STYLE            1438L
#define ERROR_INVALID_SPI_VALUE               1439L
#define ERROR_SCREEN_ALREADY_LOCKED           1440L
#define ERROR_HWNDS_HAVE_DIFF_PARENT          1441L
#define ERROR_NOT_CHILD_WINDOW                1442L
#define ERROR_INVALID_GW_COMMAND              1443L
#define ERROR_INVALID_THREAD_ID               1444L
#define ERROR_NON_MDICHILD_WINDOW             1445L
#define ERROR_POPUP_ALREADY_ACTIVE            1446L
#define ERROR_NO_SCROLLBARS                   1447L
#define ERROR_INVALID_SCROLLBAR_RANGE         1448L
#define ERROR_INVALID_SHOWWIN_COMMAND         1449L
#define ERROR_COMMITMENT_LIMIT                1455L

#define ERROR_EVENTLOG_FILE_CORRUPT           1500L
#define ERROR_EVENTLOG_CANT_START             1501L
#define ERROR_LOG_FILE_FULL                   1502L
#define ERROR_EVENTLOG_FILE_CHANGED           1503L

#define ERROR_INVALID_USER_BUFFER             1784L
#define ERROR_UNRECOGNIZED_MEDIA              1785L
#define ERROR_NO_TRUST_LSA_SECRET             1786L
#define ERROR_NO_TRUST_SAM_ACCOUNT            1787L
#define ERROR_TRUSTED_DOMAIN_FAILURE          1788L
#define ERROR_TRUSTED_RELATIONSHIP_FAILURE    1789L
#define ERROR_TRUST_FAILURE                   1790L

#define ERROR_NETLOGON_NOT_STARTED            1792L
#define ERROR_ACCOUNT_EXPIRED                 1793L
#define ERROR_REDIRECTOR_HAS_OPEN_HANDLES     1794L
#define ERROR_PRINTER_DRIVER_ALREADY_INSTALLED 1795L
#define ERROR_UNKNOWN_PORT                    1796L
#define ERROR_UNKNOWN_PRINTER_DRIVER          1797L
#define ERROR_UNKNOWN_PRINTPROCESSOR          1798L
#define ERROR_INVALID_SEPARATOR_FILE          1799L
#define ERROR_INVALID_PRIORITY                1800L
#define ERROR_INVALID_PRINTER_NAME            1801L
#define ERROR_PRINTER_ALREADY_EXISTS          1802L
#define ERROR_INVALID_PRINTER_COMMAND         1803L
#define ERROR_INVALID_DATATYPE                1804L
#define ERROR_INVALID_ENVIRONMENT             1805L

#define ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT 1807L
#define ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT 1808L
#define ERROR_NOLOGON_SERVER_TRUST_ACCOUNT    1809L
#define ERROR_DOMAIN_TRUST_INCONSISTENT       1810L
#define ERROR_SERVER_HAS_OPEN_HANDLES         1811L
#define ERROR_RESOURCE_DATA_NOT_FOUND         1812L
#define ERROR_RESOURCE_TYPE_NOT_FOUND         1813L
#define ERROR_RESOURCE_NAME_NOT_FOUND         1814L
#define ERROR_RESOURCE_LANG_NOT_FOUND         1815L
#define ERROR_NOT_ENOUGH_QUOTA                1816L

#define ERROR_INVALID_TIME                    1901L
#define ERROR_INVALID_FORM_NAME               1902L
#define ERROR_INVALID_FORM_SIZE               1903L
#define ERROR_ALREADY_WAITING                 1904L
#define ERROR_PRINTER_DELETED                 1905L
#define ERROR_INVALID_PRINTER_STATE           1906L
#define ERROR_PASSWORD_MUST_CHANGE            1907L
#define ERROR_DOMAIN_CONTROLLER_NOT_FOUND     1908L
#define ERROR_ACCOUNT_LOCKED_OUT              1909L
#define ERROR_INVALID_PIXEL_FORMAT            2000L
#define ERROR_BAD_DRIVER                      2001L
#define ERROR_INVALID_WINDOW_STYLE            2002L
#define ERROR_METAFILE_NOT_SUPPORTED          2003L
#define ERROR_TRANSFORM_NOT_SUPPORTED         2004L
#define ERROR_CLIPPING_NOT_SUPPORTED          2005L

#define ERROR_BAD_USERNAME                    2202L
#define ERROR_NOT_CONNECTED                   2250L
#define ERROR_OPEN_FILES                      2401L
#define ERROR_ACTIVE_CONNECTIONS              2402L
#define ERROR_DEVICE_IN_USE                   2404L

#define ERROR_UNKNOWN_PRINT_MONITOR           3000L
#define ERROR_PRINTER_DRIVER_IN_USE           3001L
#define ERROR_SPOOL_FILE_NOT_FOUND            3002L
#define ERROR_SPL_NO_STARTDOC                 3003L
#define ERROR_SPL_NO_ADDJOB                   3004L
#define ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED 3005L
#define ERROR_PRINT_MONITOR_ALREADY_INSTALLED 3006L
#define ERROR_WINS_INTERNAL                   4000L
#define ERROR_CAN_NOT_DEL_LOCAL_WINS          4001L
#define ERROR_STATIC_INIT                     4002L
#define ERROR_INC_BACKUP                      4003L
#define ERROR_FULL_BACKUP                     4004L
#define ERROR_REC_NON_EXISTENT                4005L
#define ERROR_RPL_NOT_ALLOWED                 4006L

/* HRESULT values for OLE, SHELL and other Interface stuff */
/* the codes 4000-40ff are reserved for OLE */
#define NOERROR                                0L
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)

#define DISP_E_UNKNOWNINTERFACE 0x80020001L
#define DISP_E_MEMBERNOTFOUND   0x80020003L
#define DISP_E_PARAMNOTFOUND    0x80020004L
#define DISP_E_TYPEMISMATCH     0x80020005L
#define DISP_E_UNKNOWNNAME      0x80020006L
#define DISP_E_NONAMEDARGS      0x80020007L
#define DISP_E_BADVARTYPE   0x80020008L
#define DISP_E_EXCEPTION        0x80020009L
#define DISP_E_OVERFLOW     0x8002000AL
#define DISP_E_BADINDEX         0x8002000BL
#define DISP_E_UNKNOWNLCID      0x8002000CL
#define DISP_E_ARRAYISLOCKED    0x8002000DL
#define DISP_E_BADPARAMCOUNT    0x8002000EL
#define DISP_E_PARAMNOTOPTIONAL 0x8002000FL

#define TYPE_E_ELEMENTNOTFOUND           0x8002802BL
#define TYPE_E_CANTLOADLIBRARY  0x80029C4AL

/* OLE Clipboard */
#define CLIPBRD_E_FIRST         0x800401D0L
#define CLIPBRD_E_LAST          0x800401DFL
#define CLIPBRD_S_FIRST         0x000401D0L
#define CLIPBRD_S_LAST          0x000401DFL
#define CLIPBRD_E_CANT_OPEN     0x800401D0L
#define CLIPBRD_E_CANT_EMPTY    0x800401D1L
#define CLIPBRD_E_CANT_SET      0x800401D2L
#define CLIPBRD_E_BAD_DATA      0x800401D3L
#define CLIPBRD_E_CANT_CLOSE    0x800401D4L

/* Drag and Drop */
#define DRAGDROP_S_DROP   0x00040100L
#define DRAGDROP_S_CANCEL 0x00040101L
#define DRAGDROP_E_NOTREGISTERED      0x80040100L
#define DRAGDROP_E_ALREADYREGISTERED  0x80040101L
#define DRAGDROP_S_USEDEFAULTCURSORS  0x00040102L

#define	E_UNEXPECTED			0x8000FFFF

#define E_NOTIMPL			0x80004001
#define E_NOINTERFACE			0x80004002
#define E_POINTER			0x80004003
#define E_ABORT				0x80004004
#define E_FAIL				0x80004005
#define E_UNSPEC E_FAIL /* must to be defined (used by FileMoniker, IOleLink and DoDragDrop as a return value) */

/*#define CO_E_INIT_TLS			0x80004006
#define CO_E_INIT_SHARED_ALLOCATOR	0x80004007
#define CO_E_INIT_MEMORY_ALLOCATOR	0x80004008
#define CO_E_INIT_CLASS_CACHE		0x80004009
#define CO_E_INIT_RPC_CHANNEL		0x8000400A
#define CO_E_INIT_TLS_SET_CHANNEL_CONTROL	0x8000400B
#define CO_E_INIT_TLS_CHANNEL_CONTROL	0x8000400C
#define CO_E_INIT_UNACCEPTED_USER_ALLOCATOR	0x8000400D
#define CO_E_INIT_SCM_MUTEX_EXISTS	0x8000400E
#define CO_E_INIT_SCM_FILE_MAPPING_EXISTS	0x8000400F
#define CO_E_INIT_SCM_MAP_VIEW_OF_FILE	0x80004010
#define CO_E_INIT_SCM_EXEC_FAILURE	0x80004011
#define CO_E_INIT_ONLY_SINGLE_THREADED	0x80004012 */

#define CO_S_NOTALLINTERFACES           0x00080012
#define CO_E_NOTINITIALIZED             0x800401F0
#define CO_E_ERRORINDLL                 0x800401F9
#define CO_E_OBJISREG                   0x800401FB

#define OLE_E_FIRST 0x80040000L
#define OLE_E_LAST  0x800400FFL
#define OLE_S_FIRST 0x00040000L
#define OLE_S_LAST  0x000400FFL

#define	OLE_E_ENUM_NOMORE		      0x80040002
#define	OLE_E_ADVISENOTSUPPORTED  0x80040003
#define	OLE_E_NOCONNECTION        0x80040004
#define	OLE_E_NOTRUNNING          0x80040005
#define	OLE_E_NOCACHE             0x80040006
#define	OLE_E_BLANK		            0x80040007
#define	OLE_E_NOT_INPLACEACTIVE		0x80040010
#define	OLE_E_STATIC              0x8004000B
#define	OLE_E_PROMPTSAVECANCELLED 0x8004000C
#define OLE_S_USEREG                            0x00040000

#define DV_E_FORMATETC            0x80040064
#define DV_E_DVASPECT             0x8004006B
#define DV_E_LINDEX               0x80040068
#define DV_E_TYMED                0x80040069

#define CLASS_E_NOAGGREGATION     0x80040110
#define	CLASS_E_CLASSNOTAVAILABLE 0x80040111

#define DATA_S_SAMEFORMATETC      0x80040130

#define E_ACCESSDENIED			      0x80070005
#define E_HANDLE            			0x80070006
#define	E_OUTOFMEMORY			        0x8007000E
#define	E_INVALIDARG			        0x80070057

/*#define OLE_E_FIRST 0x80040000L */
/*#define OLE_E_LAST  0x800400FFL */
/*#define OLE_S_FIRST 0x00040000L */
/*#define OLE_S_LAST  0x000400FFL */

#define MK_S_REDUCED_TO_SELF            0x000401E2
#define MK_S_ME                         0x000401E4
#define MK_S_HIM                        0x000401E5
#define MK_S_US                         0x000401E6
#define MK_S_MONIKERALREADYREGISTERED   0x000401E7

#define MK_E_EXCEEDEDDEADLINE                     0x800401E1
#define MK_E_NEEDGENERIC                          0x800401E2
#define MK_E_UNAVAILABLE                          0x800401E3
#define MK_E_SYNTAX                               0x800401E4
#define MK_E_NOOBJECT                             0x800401E5
#define MK_E_INVALIDEXTENSION                     0x800401E6
#define MK_E_INTERMEDIATEINTERFACENOTSUPPORTED    0x800401E7
#define MK_E_NOTBINDABLE                          0x800401E8
#define MK_E_NOTBOUND                             0x800401E9
#define MK_E_CANTOPENFILE                         0x800401EA
#define MK_E_MIUSTBOTHERUSER                      0x800401EB
#define MK_E_NOINVERSE                            0x800401EC
#define MK_E_NOSTORAGE                            0x800401ED
#define MK_E_NOPREFIX                             0x800401EE

#define STG_E_INVALIDFUNCTION		0x80030001
#define STG_E_FILENOTFOUND		0x80030002
#define STG_E_PATHNOTFOUND		0x80030003
#define STG_E_TOOMANYOPENFILES		0x80030004
#define STG_E_ACCESSDENIED		0x80030005
#define STG_E_INVALIDHANDLE		0x80030006
#define STG_E_INSUFFICIENTMEMORY	0x80030008
#define STG_E_INVALIDPOINTER		0x80030009
#define STG_E_NOMOREFILES		0x80030012
#define STG_E_DISKISWRITEPROTECTED	0x80030013
#define STG_E_SEEKERROR			0x80030019
#define STG_E_WRITEFAULT		0x8003001D
#define STG_E_READFAULT			0x8003001E
#define STG_E_SHAREVIOLATION		0x80030020
#define STG_E_LOCKVIOLATION		0x80030021
#define STG_E_FILEALREADYEXISTS		0x80030050
#define STG_E_INVALIDPARAMETER		0x80030057
#define STG_E_MEDIUMFULL		0x80030070
#define STG_E_ABNORMALAPIEXIT		0x800300FA
#define STG_E_INVALIDHEADER		0x800300FB
#define STG_E_INVALIDNAME		0x800300FC
#define STG_E_UNKNOWN			0x800300FD
#define STG_E_UNIMPLEMENTEDFUNCTION	0x800300FE
#define STG_E_INVALIDFLAG		0x800300FF
#define STG_E_INUSE			0x80030100
#define STG_E_NOTCURRENT		0x80030101
#define STG_E_REVERTED			0x80030102
#define STG_E_CANTSAVE			0x80030103
#define STG_E_OLDFORMAT			0x80030104
#define STG_E_OLDDLL			0x80030105
#define STG_E_SHAREREQUIRED		0x80030106
#define STG_E_NOTFILEBASEDSTORAGE	0x80030107
#define STG_E_EXTANTMARSHALLINGS	0x80030108

/* alten versionen
#define E_NOTIMPL			0x80000001
#define E_OUTOFMEMORY			0x80000002
#define E_INVALIDARG			0x80000003
#define E_NOINTERFACE			0x80000004
#define E_POINTER			0x80000005
#define E_HANDLE			0x80000006
#define E_ABORT				0x80000007
#define	E_FAIL				0x80000008
#define E_ACCESSDENIED			0x80000009 */

/* Obtained from lcc-win32 include files */
#define GDI_ERROR			0xffffffff


/* registry errors */
#define REGDB_E_READREGDB               0x80040150
#define REGDB_E_CLASSNOTREG             0x80040154

#define INPLACE_E_NOTUNDOABLE           0x800401A0
#define INPLACE_E_NOTOOLSPACE           0x800401A1

#define DATA_E_FORMATETC                DV_E_FORMATETC

#define CLASSFACTORY_E_FIRST            0x80040110L
#define CLASSFACTORY_E_LAST             0x8004011FL
#define CLASSFACTORY_S_FIRST            0x80040110L
#define CLASSFACTORY_S_LAST             0x8004011FL

#define CLASS_E_NOTLICENSED             (CLASSFACTORY_E_FIRST+2)
#define CLASS_E_NOAGGREGATION           0x80040110
#define	CLASS_E_CLASSNOTAVAILABLE	      0x80040111


#define OLEOBJ_E_NOVERBS                0x00040180L
#define OLEOBJ_E_INVALIDVERB            0x00040181L
#define OLEOBJ_S_INVALIDVERB            0x00040180L

#endif  /* __WINE_WINERROR_H */
