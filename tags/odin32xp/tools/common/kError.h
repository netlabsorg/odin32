/* $Id: kError.h,v 1.1 2002-02-24 02:47:24 bird Exp $
 *
 * Error Wrapper.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */

#ifndef _kError_h_
#define _kError_h_



class kError
{
public:
    /**
     * OS/2 Error Codes.
     */
    enum enmError
    {
        NO = 0,
        INVALID_FUNCTION        = 1,
        FILE_NOT_FOUND          = 2,
        PATH_NOT_FOUND          = 3,
        TOO_MANY_OPEN_FILES     = 4,
        ACCESS_DENIED           = 5,
        INVALID_HANDLE          = 6,
        ARENA_TRASHED           = 7,
        NOT_ENOUGH_MEMORY       = 8,
        INVALID_BLOCK           = 9,
        BAD_ENVIRONMENT         = 10,
        BAD_FORMAT              = 11,
        INVALID_ACCESS          = 12,
        INVALID_DATA            = 13,
        INVALID_DRIVE           = 15,
        CURRENT_DIRECTORY       = 16,
        NOT_SAME_DEVICE         = 17,
        NO_MORE_FILES           = 18,
        WRITE_PROTECT           = 19,
        BAD_UNIT                = 20,
        NOT_READY               = 21,
        BAD_COMMAND             = 22,
        CRC                     = 23,
        BAD_LENGTH              = 24,
        SEEK                    = 25,
        NOT_DOS_DISK            = 26,
        SECTOR_NOT_FOUND        = 27,
        OUT_OF_PAPER            = 28,
        WRITE_FAULT             = 29,
        READ_FAULT              = 30,
        GEN_FAILURE             = 31,
        SHARING_VIOLATION       = 32,
        LOCK_VIOLATION          = 33,
        WRONG_DISK              = 34,
        FCB_UNAVAILABLE         = 35,
        SHARING_BUFFER_EXCEEDED = 36,
        CODE_PAGE_MISMATCHED    = 37,
        HANDLE_EOF              = 38,
        HANDLE_DISK_FULL        = 39,
        NOT_SUPPORTED           = 50,
        REM_NOT_LIST            = 51,
        DUP_NAME                = 52,
        BAD_NETPATH             = 53,
        NETWORK_BUSY            = 54,
        DEV_NOT_EXIST           = 55,
        TOO_MANY_CMDS           = 56,
        ADAP_HDW_ERR            = 57,
        BAD_NET_RESP            = 58,
        UNEXP_NET_ERR           = 59,
        BAD_REM_ADAP            = 60,
        PRINTQ_FULL             = 61,
        NO_SPOOL_SPACE          = 62,
        PRINT_CANCELLED         = 63,
        NETNAME_DELETED         = 64,
        NETWORK_ACCESS_DENIED   = 65,
        BAD_DEV_TYPE            = 66,
        BAD_NET_NAME            = 67,
        TOO_MANY_NAMES          = 68,
        TOO_MANY_SESS           = 69,
        SHARING_PAUSED          = 70,
        REQ_NOT_ACCEP           = 71,
        REDIR_PAUSED            = 72,
        SBCS_ATT_WRITE_PROT     = 73,
        SBCS_GENERAL_FAILURE    = 74,
        XGA_OUT_MEMORY          = 75,
        FILE_EXISTS             = 80,
        DUP_FCB                 = 81,
        CANNOT_MAKE             = 82,
        FAIL_I24                = 83,
        OUT_OF_STRUCTURES       = 84,
        ALREADY_ASSIGNED        = 85,
        INVALID_PASSWORD        = 86,
        INVALID_PARAMETER       = 87,
        NET_WRITE_FAULT         = 88,
        NO_PROC_SLOTS           = 89,
        NOT_FROZEN              = 90,
        TSTOVFL                 = 91,
        TSTDUP                  = 92,
        NO_ITEMS                = 93,
        INTERRUPT               = 95,
        DEVICE_IN_USE           = 99,
        TOO_MANY_SEMAPHORES     = 100,
        EXCL_SEM_ALREADY_OWNED  = 101,
        SEM_IS_SET              = 102,
        TOO_MANY_SEM_REQUESTS   = 103,
        INVALID_AT_INTERRUPT_TIME= 104,
        SEM_OWNER_DIED          = 105,
        SEM_USER_LIMIT          = 106,
        DISK_CHANGE             = 107,
        DRIVE_LOCKED            = 108,
        BROKEN_PIPE             = 109,
        OPEN_FAILED             = 110,
        BUFFER_OVERFLOW         = 111,
        DISK_FULL               = 112,
        NO_MORE_SEARCH_HANDLES  = 113,
        INVALID_TARGET_HANDLE   = 114,
        PROTECTION_VIOLATION    = 115,
        VIOKBD_REQUEST          = 116,
        INVALID_CATEGORY        = 117,
        INVALID_VERIFY_SWITCH   = 118,
        BAD_DRIVER_LEVEL        = 119,
        CALL_NOT_IMPLEMENTED    = 120,
        SEM_TIMEOUT             = 121,
        INSUFFICIENT_BUFFER     = 122,
        INVALID_NAME            = 123,
        INVALID_LEVEL           = 124,
        NO_VOLUME_LABEL         = 125,
        MOD_NOT_FOUND           = 126,
        PROC_NOT_FOUND          = 127,
        WAIT_NO_CHILDREN        = 128,
        CHILD_NOT_COMPLETE      = 129,
        DIRECT_ACCESS_HANDLE    = 130,
        NEGATIVE_SEEK           = 131,
        SEEK_ON_DEVICE          = 132,
        IS_JOIN_TARGET          = 133,
        IS_JOINED               = 134,
        IS_SUBSTED              = 135,
        NOT_JOINED              = 136,
        NOT_SUBSTED             = 137,
        JOIN_TO_JOIN            = 138,
        SUBST_TO_SUBST          = 139,
        JOIN_TO_SUBST           = 140,
        SUBST_TO_JOIN           = 141,
        BUSY_DRIVE              = 142,
        SAME_DRIVE              = 143,
        DIR_NOT_ROOT            = 144,
        DIR_NOT_EMPTY           = 145,
        IS_SUBST_PATH           = 146,
        IS_JOIN_PATH            = 147,
        PATH_BUSY               = 148,
        IS_SUBST_TARGET         = 149,
        SYSTEM_TRACE            = 150,
        INVALID_EVENT_COUNT     = 151,
        TOO_MANY_MUXWAITERS     = 152,
        INVALID_LIST_FORMAT     = 153,
        LABEL_TOO_LONG          = 154,
        TOO_MANY_TCBS           = 155,
        SIGNAL_REFUSED          = 156,
        DISCARDED               = 157,
        NOT_LOCKED              = 158,
        BAD_THREADID_ADDR       = 159,
        BAD_ARGUMENTS           = 160,
        BAD_PATHNAME            = 161,
        SIGNAL_PENDING          = 162,
        UNCERTAIN_MEDIA         = 163,
        MAX_THRDS_REACHED       = 164,
        MONITORS_NOT_SUPPORTED  = 165,
        UNC_DRIVER_NOT_INSTALLED= 166,
        LOCK_FAILED             = 167,
        SWAPIO_FAILED           = 168,
        SWAPIN_FAILED           = 169,
        BUSY                    = 170,
        INT_TOO_LONG            = 171,
        CANCEL_VIOLATION        = 173,
        ATOMIC_LOCK_NOT_SUPPORTED= 174,
        READ_LOCKS_NOT_SUPPORTED= 175,
        INVALID_SEGMENT_NUMBER  = 180,
        INVALID_CALLGATE        = 181,
        INVALID_ORDINAL         = 182,
        ALREADY_EXISTS          = 183,
        NO_CHILD_PROCESS        = 184,
        CHILD_ALIVE_NOWAIT      = 185,
        INVALID_FLAG_NUMBER     = 186,
        SEM_NOT_FOUND           = 187,
        INVALID_STARTING_CODESEG= 188,
        INVALID_STACKSEG        = 189,
        INVALID_MODULETYPE      = 190,
        INVALID_EXE_SIGNATURE   = 191,
        EXE_MARKED_INVALID      = 192,
        BAD_EXE_FORMAT          = 193,
        ITERATED_DATA_EXCEEDS_64k= 194,
        INVALID_MINALLOCSIZE    = 195,
        DYNLINK_FROM_INVALID_RING= 196,
        IOPL_NOT_ENABLED        = 197,
        INVALID_SEGDPL          = 198,
        AUTODATASEG_EXCEEDS_64k = 199,
        RING2SEG_MUST_BE_MOVABLE= 200,
        RELOC_CHAIN_XEEDS_SEGLIM= 201,
        INFLOOP_IN_RELOC_CHAIN  = 202,
        ENVVAR_NOT_FOUND        = 203,
        NOT_CURRENT_CTRY        = 204,
        NO_SIGNAL_SENT          = 205,
        FILENAME_EXCED_RANGE    = 206,
        RING2_STACK_IN_USE      = 207,
        META_EXPANSION_TOO_LONG = 208,
        INVALID_SIGNAL_NUMBER   = 209,
        THREAD_1_INACTIVE       = 210,
        INFO_NOT_AVAIL          = 211,
        LOCKED                  = 212,
        BAD_DYNALINK            = 213,
        TOO_MANY_MODULES        = 214,
        NESTING_NOT_ALLOWED     = 215,
        CANNOT_SHRINK           = 216,
        ZOMBIE_PROCESS          = 217,
        STACK_IN_HIGH_MEMORY    = 218,
        INVALID_EXITROUTINE_RING= 219,
        GETBUF_FAILED           = 220,
        FLUSHBUF_FAILED         = 221,
        TRANSFER_TOO_LONG       = 222,
        FORCENOSWAP_FAILED      = 223,
        SMG_NO_TARGET_WINDOW    = 224,
        NO_CHILDREN             = 228,
        INVALID_SCREEN_GROUP    = 229,
        BAD_PIPE                = 230,
        PIPE_BUSY               = 231,
        NO_DATA                 = 232,
        PIPE_NOT_CONNECTED      = 233,
        MORE_DATA               = 234,
        VC_DISCONNECTED         = 240,
        CIRCULARITY_REQUESTED   = 250,
        DIRECTORY_IN_CDS        = 251,
        INVALID_FSD_NAME        = 252,
        INVALID_PATH            = 253,
        INVALID_EA_NAME         = 254,
        EA_LIST_INCONSISTENT    = 255,
        EA_LIST_TOO_LONG        = 256,
        NO_META_MATCH           = 257,
        FINDNOTIFY_TIMEOUT      = 258,
        NO_MORE_ITEMS           = 259,
        SEARCH_STRUC_REUSED     = 260,
        CHAR_NOT_FOUND          = 261,
        TOO_MUCH_STACK          = 262,
        INVALID_ATTR            = 263,
        INVALID_STARTING_RING   = 264,
        INVALID_DLL_INIT_RING   = 265,
        CANNOT_COPY             = 266,
        DIRECTORY               = 267,
        OPLOCKED_FILE           = 268,
        OPLOCK_THREAD_EXISTS    = 269,
        VOLUME_CHANGED          = 270,
        FINDNOTIFY_HANDLE_IN_USE= 271,
        FINDNOTIFY_HANDLE_CLOSED= 272,
        NOTIFY_OBJECT_REMOVED   = 273,
        ALREADY_SHUTDOWN        = 274,
        EAS_DIDNT_FIT           = 275,
        EA_FILE_CORRUPT         = 276,
        EA_TABLE_FULL           = 277,
        INVALID_EA_HANDLE       = 278,
        NO_CLUSTER              = 279,
        CREATE_EA_FILE          = 280,
        CANNOT_OPEN_EA_FILE     = 281,
        EAS_NOT_SUPPORTED       = 282,
        NEED_EAS_FOUND          = 283,
        DUPLICATE_HANDLE        = 284,
        DUPLICATE_NAME          = 285,
        EMPTY_MUXWAIT           = 286,
        MUTEX_OWNED             = 287,
        NOT_OWNER               = 288,
        PARAM_TOO_SMALL         = 289,
        TOO_MANY_HANDLES        = 290,
        TOO_MANY_OPENS          = 291,
        WRONG_TYPE              = 292,
        UNUSED_CODE             = 293,
        THREAD_NOT_TERMINATED   = 294,
        INIT_ROUTINE_FAILED     = 295,
        MODULE_IN_USE           = 296,
        NOT_ENOUGH_WATCHPOINTS  = 297,
        TOO_MANY_POSTS          = 298,
        ALREADY_POSTED          = 299,
        ALREADY_RESET           = 300,
        SEM_BUSY                = 301,


        /*
         * Custom errors.
         */
        first_custom_error = 0x10000,
        /* Generic */
        INVALID_ADDRESS,
        INVALID_OFFSET,
        INVALID_SIGNATURE,
        INCORRECT_VERSION,
        INTERNAL_ERROR,
        UNSUPPORTED_ENDIAN,
        STRING_TO_LONG,
        BAD_ALIGNMENT,
        BAD_STRING_LENGTH,
        BAD_STRUCTURE,

        /* .DEF-Files */
        DEF_MULIPLE_MODULE_STATEMENT,
        DEF_NO_MODULE_STATEMENT,
        DEF_BAD_LIBRARY_STATEMENT,
        DEF_BAD_IMPORT,
        DEF_BAD_EXPORT,
        DEF_UNKOWN_VERB,

        /* .SDF-Files */
        SDF_TOO_MANY,

        /* .LX-Files */
        INVALID_PAGE,
        BAD_ITERPAGE,
        BAD_COMPESSED_PAGE
    };


public:
    /**
     * Constructor.
     * @param   iErrorNo    Error number this object should represent.
     *                      If not specified a NO_ERROR object is created.
     */
    kError(int iErrorNo = 0) : iErrorNo(iErrorNo) { }

    /**
     * Constructor.
     * @param   iErrorNo    Error number this object should represent.
     *                      If not specified a NO_ERROR object is created.
     */
    kError(enum enmError enmErrorNo = kError::NO) : iErrorNo(enmErrorNo) { }

    /**
     * Gets the message text for a given error number.
     * @returns Message text. Caller must delete it.
     */
    static char *getText(int iErrorNo);

    /**
     * Gets the message text for this error.
     * @returns Message text. Caller must delete it.
     */
    char *getText(void) const
    {
        return kError::getText(iErrorNo);
    }

    /**
     * Gets the error number for this error.
     * @returns The error number.
     */
    int getErrno(void) const
    {
        return iErrorNo;
    }


    KBOOL operator !=(int iErrorNo) const
    {
        return this->iErrorNo != iErrorNo;
    }

    KBOOL operator ==(int iErrorNo) const
    {
        return this->iErrorNo == iErrorNo;
    }

    KBOOL operator >(int iErrorNo) const
    {
        return this->iErrorNo > iErrorNo;
    }

    KBOOL operator >=(int iErrorNo) const
    {
        return this->iErrorNo >= iErrorNo;
    }

    KBOOL operator <(int iErrorNo) const
    {
        return this->iErrorNo < iErrorNo;
    }

    KBOOL operator <=(int iErrorNo) const
    {
        return this->iErrorNo <= iErrorNo;
    }


    KBOOL operator !=(enum enmError enmErrorNo) const
    {
        return this->iErrorNo != enmErrorNo;
    }

    KBOOL operator ==(enum enmError enmErrorNo) const
    {
        return this->iErrorNo == enmErrorNo;
    }

    KBOOL operator >(enum enmError enmErrorNo) const
    {
        return this->iErrorNo > enmErrorNo;
    }

    KBOOL operator >=(enum enmError enmErrorNo) const
    {
        return this->iErrorNo >= enmErrorNo;
    }

    KBOOL operator <(enum enmError enmErrorNo) const
    {
        return this->iErrorNo < enmErrorNo;
    }

    KBOOL operator <=(enum enmError enmErrorNo) const
    {
        return this->iErrorNo <= enmErrorNo;
    }



protected:
    int iErrorNo;
};




#endif

