/* $Id: wininet.h,v 1.2 2000-02-05 02:42:19 sandervl Exp $ */
#ifndef __WININET_H__
#define __WININET_H__

#define INTERNETAPI
#define BOOLAPI INTERNETAPI BOOL WINAPI

#define URLCACHEAPI 

typedef LPVOID HINTERNET;
typedef HINTERNET * LPHINTERNET;
typedef WORD INTERNET_PORT;
typedef INTERNET_PORT * LPINTERNET_PORT;

#define INTERNET_INVALID_PORT_NUMBER    0

#define INTERNET_DEFAULT_FTP_PORT       21
#define INTERNET_DEFAULT_GOPHER_PORT    70
#define INTERNET_DEFAULT_HTTP_PORT      80
#define INTERNET_DEFAULT_HTTPS_PORT     443

#define MAX_CACHE_ENTRY_INFO_SIZE       4096

#define INTERNET_MAX_HOST_NAME_LENGTH   256
#define INTERNET_MAX_USER_NAME_LENGTH   128
#define INTERNET_MAX_PASSWORD_LENGTH    128
#define INTERNET_MAX_PORT_NUMBER_LENGTH
#define INTERNET_MAX_PORT_NUMBER_VALUE  65535
#define INTERNET_MAX_PATH_LENGTH        1024
#define INTERNET_MAX_PROTOCOL_NAME      "gopher"
#define INTERNET_MAX_URL_LENGTH         ((sizeof(INTERNET_MAX_PROTOCOL_NAME) - 1) \
                                        + sizeof("://") \
                                        + INTERNET_MAX_PATH_LENGTH)

#define INTERNET_KEEP_ALIVE_UNKNOWN     ((DWORD)-1)
#define INTERNET_KEEP_ALIVE_ENABLED     1
#define INTERNET_KEEP_ALIVE_DISABLED    0

#define INTERNET_REQFLAG_FROM_CACHE     0x00000001
#define INTERNET_REQFLAG_ASYNC          0x00000002


#define INTERNET_FLAG_IGNORE_CERT_CN_INVALID    0x00001000
#define INTERNET_FLAG_IGNORE_CERT_DATE_INVALID  0x00002000
#define INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS  0x00004000
#define INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP   0x00008000
#define INTERNET_FLAG_TRANSFER_ASCII    	FTP_TRANSFER_TYPE_ASCII
#define INTERNET_FLAG_TRANSFER_BINARY   	FTP_TRANSFER_TYPE_BINARY
#define INTERNET_FLAG_READ_PREFETCH     	0x00100000
#define INTERNET_FLAG_NO_AUTO_REDIRECT  	0x00200000
#define INTERNET_FLAG_KEEP_CONNECTION   	0x00400000
#define INTERNET_FLAG_SECURE            	0x00800000
#define INTERNET_FLAG_OFFLINE           	0x01000000
#define INTERNET_FLAG_MAKE_PERSISTENT   	0x02000000
#define INTERNET_FLAG_NO_CACHE_WRITE    	0x04000000
#define INTERNET_FLAG_DONT_CACHE        	INTERNET_FLAG_NO_CACHE_WRITE
#define INTERNET_FLAG_PASSIVE           	0x08000000
#define INTERNET_FLAG_ASYNC             	0x10000000
#define INTERNET_FLAG_EXISTING_CONNECT  	0x20000000
#define INTERNET_FLAG_RAW_DATA          	0x40000000
#define INTERNET_FLAG_RELOAD            	0x80000000


#define SECURITY_INTERNET_MASK  (INTERNET_FLAG_IGNORE_CERT_CN_INVALID    |  \
                                 INTERNET_FLAG_IGNORE_CERT_DATE_INVALID  |  \
                                 INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS  |  \
                                 INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP   )

#define SECURITY_SET_MASK       SECURITY_INTERNET_MASK

#define INTERNET_FLAGS_MASK     (INTERNET_FLAG_RELOAD               \
                                | INTERNET_FLAG_RAW_DATA            \
                                | INTERNET_FLAG_EXISTING_CONNECT    \
                                | INTERNET_FLAG_ASYNC               \
                                | INTERNET_FLAG_PASSIVE             \
                                | INTERNET_FLAG_DONT_CACHE          \
                                | INTERNET_FLAG_MAKE_PERSISTENT     \
                                | INTERNET_FLAG_OFFLINE             \
                                | INTERNET_FLAG_SECURE              \
                                | INTERNET_FLAG_KEEP_CONNECTION     \
                                | INTERNET_FLAG_NO_AUTO_REDIRECT    \
                                | INTERNET_FLAG_READ_PREFETCH       \
                                | SECURITY_INTERNET_MASK            \
                                | INTERNET_FLAG_TRANSFER_ASCII      \
                                | INTERNET_FLAG_TRANSFER_BINARY     \
                                )

#define INTERNET_OPTIONS_MASK   (~INTERNET_FLAGS_MASK)

#define INTERNET_NO_CALLBACK            0

#define INTERNET_PREFETCH_PROGRESS  0
#define INTERNET_PREFETCH_COMPLETE  1
#define INTERNET_PREFETCH_ABORTED   2

#define INTERNET_RFC1123_FORMAT     0
#define INTERNET_RFC1123_BUFSIZE   30

#define ICU_ENCODE_SPACES_ONLY 	0x04000000
#define ICU_NO_META     	0x08000000
#define ICU_DECODE      	0x10000000
#define ICU_NO_ENCODE   	0x20000000
#define ICU_USERNAME    	0x40000000
#define ICU_ESCAPE      	0x80000000

#define INTERNET_OPEN_TYPE_PRECONFIG    0
#define INTERNET_OPEN_TYPE_DIRECT       1
#define INTERNET_OPEN_TYPE_PROXY        3

#define PRE_CONFIG_INTERNET_ACCESS  	INTERNET_OPEN_TYPE_PRECONFIG
#define LOCAL_INTERNET_ACCESS       	INTERNET_OPEN_TYPE_DIRECT
#define GATEWAY_INTERNET_ACCESS     	2   
#define CERN_PROXY_INTERNET_ACCESS  	INTERNET_OPEN_TYPE_PROXY

#define INTERNET_SERVICE_FTP    	1
#define INTERNET_SERVICE_GOPHER 	2
#define INTERNET_SERVICE_HTTP   	3

#define ISO_GLOBAL      		0x00000001
#define ISO_REGISTRY    		0x00000002
#define ISO_VALID_FLAGS 		(ISO_GLOBAL | ISO_REGISTRY)

#define INTERNET_OPTION_CALLBACK            	1
#define INTERNET_OPTION_CONNECT_TIME        	2
#define INTERNET_OPTION_CONNECT_RETRIES         3
#define INTERNET_OPTION_CONNECT_BACKOFF         4
#define INTERNET_OPTION_SEND_TIME           	5
#define INTERNET_OPTION_CONTROL_SEND_TIME   	INTERNET_OPTION_SEND_TIMEOUT
#define INTERNET_OPTION_RECEIVE_TIME        	6
#define INTERNET_OPTION_CONTROL_RECEIVE_TIME 	INTERNET_OPTION_RECEIVE_TIMEOUT
#define INTERNET_OPTION_DATA_SEND_TIME      	7
#define INTERNET_OPTION_DATA_RECEIVE_TIME   	8
#define INTERNET_OPTION_HANDLE_TYPE             9
#define INTERNET_OPTION_CONTEXT_VALUE           10
#define INTERNET_OPTION_READ_BUFFER_SIZE        12
#define INTERNET_OPTION_WRITE_BUFFER_SIZE       13
#define INTERNET_OPTION_ASYNC_ID                15
#define INTERNET_OPTION_ASYNC_PRIORITY          16
#define INTERNET_OPTION_PARENT_HANDLE           21
#define INTERNET_OPTION_KEEP_CONNECTION         22
#define INTERNET_OPTION_REQUEST_FLAGS           23
#define INTERNET_OPTION_EXTENDED_ERROR          24
#define INTERNET_OPTION_OFFLINE_MODE            26
#define INTERNET_OPTION_CACHE_STREAM_HANDLE     27
#define INTERNET_OPTION_USERNAME                28
#define INTERNET_OPTION_PASSWORD                29
#define INTERNET_OPTION_ASYNC                   30
#define INTERNET_OPTION_SECURITY_FLAGS          31
#define INTERNET_OPTION_SECURITY_CERTIFICATE_STRUCT    32
#define INTERNET_OPTION_DATAFILE_NAME           33
#define INTERNET_OPTION_URL                     34
#define INTERNET_OPTION_SECURITY_CERTIFICATE    35
#define INTERNET_OPTION_SECURITY_KEY_BITNESS    36
#define INTERNET_OPTION_REFRESH                 37
#define INTERNET_OPTION_PROXY                   38
#define INTERNET_FIRST_OPTION                   INTERNET_OPTION_CALLBACK
#define INTERNET_LAST_OPTION                    INTERNET_OPTION_PROXY

#define INTERNET_PRIORITY_FOREGROUND            1000

#define INTERNET_HANDLE_TYPE_INTERNET           1
#define INTERNET_HANDLE_TYPE_CONNECT_FTP        2
#define INTERNET_HANDLE_TYPE_CONNECT_GOPHER     3
#define INTERNET_HANDLE_TYPE_CONNECT_HTTP       4
#define INTERNET_HANDLE_TYPE_FTP_FIND           5
#define INTERNET_HANDLE_TYPE_FTP_FIND_HTML      6
#define INTERNET_HANDLE_TYPE_FTP_FILE           7
#define INTERNET_HANDLE_TYPE_FTP_FILE_HTML      8
#define INTERNET_HANDLE_TYPE_GOPHER_FIND        9
#define INTERNET_HANDLE_TYPE_GOPHER_FIND_HTML   10
#define INTERNET_HANDLE_TYPE_GOPHER_FILE        11
#define INTERNET_HANDLE_TYPE_GOPHER_FILE_HTML   12
#define INTERNET_HANDLE_TYPE_HTTP_REQUEST       13

#define SECURITY_FLAG_SECURE                    0x00000001 // can query only
#define SECURITY_FLAG_SSL                       0x00000002
#define SECURITY_FLAG_SSL3                      0x00000004
#define SECURITY_FLAG_PCT                       0x00000008
#define SECURITY_FLAG_PCT4                      0x00000010
#define SECURITY_FLAG_IETFSSL4                  0x00000020
#define SECURITY_FLAG_40BIT                     0x10000000
#define SECURITY_FLAG_128BIT                    0x20000000
#define SECURITY_FLAG_56BIT                     0x40000000
#define SECURITY_FLAG_UNKNOWNBIT                0x80000000
#define SECURITY_FLAG_NORMALBITNESS             SECURITY_FLAG_40BIT
#define SECURITY_FLAG_IGNORE_CERT_CN_INVALID    INTERNET_FLAG_IGNORE_CERT_CN_INVALID
#define SECURITY_FLAG_IGNORE_CERT_DATE_INVALID  INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
#define SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTPS  INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
#define SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTP   INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP


#define INTERNET_STATUS_RESOLVING_NAME          10
#define INTERNET_STATUS_NAME_RESOLVED           11
#define INTERNET_STATUS_CONNECTING_TO_SERVER    20
#define INTERNET_STATUS_CONNECTED_TO_SERVER     21
#define INTERNET_STATUS_SENDING_REQUEST         30
#define INTERNET_STATUS_REQUEST_SENT            31
#define INTERNET_STATUS_RECEIVING_RESPONSE      40
#define INTERNET_STATUS_RESPONSE_RECEIVED       41
#define INTERNET_STATUS_CTL_RESPONSE_RECEIVED   42
#define INTERNET_STATUS_PREFETCH                43
#define INTERNET_STATUS_CLOSING_CONNECTION      50
#define INTERNET_STATUS_CONNECTION_CLOSED       51
#define INTERNET_STATUS_HANDLE_CREATED          60
#define INTERNET_STATUS_HANDLE_CLOSING          70
#define INTERNET_STATUS_REQUEST_COMPLETE        100
#define INTERNET_STATUS_REDIRECT                110
#define INTERNET_INVALID_STATUS_CALLBACK        ((INTERNET_STATUS_CALLBACK)(-1L))

#define FTP_TRANSFER_TYPE_UNKNOWN   		0x00000000
#define FTP_TRANSFER_TYPE_ASCII     		0x00000001
#define FTP_TRANSFER_TYPE_BINARY    		0x00000002
#define FTP_TRANSFER_TYPE_MASK      		(FTP_TRANSFER_TYPE_ASCII | FTP_TRANSFER_TYPE_BINARY)


#define MAX_GOPHER_DISPLAY_TEXT     128
#define MAX_GOPHER_SELECTOR_TEXT    256
#define MAX_GOPHER_HOST_NAME        INTERNET_MAX_HOST_NAME_LENGTH
#define MAX_GOPHER_LOCATOR_LENGTH   (1 + MAX_GOPHER_DISPLAY_TEXT        \
                                    + 1                                 \
                                    + MAX_GOPHER_SELECTOR_TEXT          \
                                    + 1                                 \
                                    + MAX_GOPHER_HOST_NAME              \
                                    + 1                                 \
                                    + INTERNET_MAX_PORT_NUMBER_LENGTH   \
                                    + 1                                 \
                                    + 1                                 \
                                    + 2)


#define MAX_GOPHER_CATEGORY_NAME    	128
#define MAX_GOPHER_ATTRIBUTE_NAME   	128
#define MIN_GOPHER_ATTRIBUTE_LENGTH 	256


#define GOPHER_INFO_CATEGORY        	TEXT("+INFO")
#define GOPHER_ADMIN_CATEGORY       	TEXT("+ADMIN")
#define GOPHER_VIEWS_CATEGORY       	TEXT("+VIEWS")
#define GOPHER_ABSTRACT_CATEGORY    	TEXT("+ABSTRACT")
#define GOPHER_VERONICA_CATEGORY    	TEXT("+VERONICA")

#define GOPHER_ADMIN_ATTRIBUTE      	TEXT("Admin")
#define GOPHER_MOD_DATE_ATTRIBUTE   	TEXT("Mod-Date")
#define GOPHER_TTL_ATTRIBUTE        	TEXT("TTL")
#define GOPHER_SCORE_ATTRIBUTE      	TEXT("Score")
#define GOPHER_RANGE_ATTRIBUTE      	TEXT("Score-range")
#define GOPHER_SITE_ATTRIBUTE       	TEXT("Site")
#define GOPHER_ORG_ATTRIBUTE        	TEXT("Org")
#define GOPHER_LOCATION_ATTRIBUTE   	TEXT("Loc")
#define GOPHER_GEOG_ATTRIBUTE       	TEXT("Geog")
#define GOPHER_TIMEZONE_ATTRIBUTE   	TEXT("TZ")
#define GOPHER_PROVIDER_ATTRIBUTE   	TEXT("Provider")
#define GOPHER_VERSION_ATTRIBUTE    	TEXT("Version")
#define GOPHER_ABSTRACT_ATTRIBUTE   	TEXT("Abstract")
#define GOPHER_VIEW_ATTRIBUTE       	TEXT("View")
#define GOPHER_TREEWALK_ATTRIBUTE   	TEXT("treewalk")

#define GOPHER_ATTRIBUTE_ID_BASE        0xabcccc00

#define GOPHER_CATEGORY_ID_ALL          (GOPHER_ATTRIBUTE_ID_BASE + 1)
#define GOPHER_CATEGORY_ID_INFO         (GOPHER_ATTRIBUTE_ID_BASE + 2)
#define GOPHER_CATEGORY_ID_ADM       	(GOPHER_ATTRIBUTE_ID_BASE + 3)
#define GOPHER_CATEGORY_ID_VIEWS        (GOPHER_ATTRIBUTE_ID_BASE + 4)
#define GOPHER_CATEGORY_ID_ABSTRACT     (GOPHER_ATTRIBUTE_ID_BASE + 5)
#define GOPHER_CATEGORY_ID_VERONICA     (GOPHER_ATTRIBUTE_ID_BASE + 6)
#define GOPHER_CATEGORY_ID_ASK          (GOPHER_ATTRIBUTE_ID_BASE + 7)
#define GOPHER_CATEGORY_ID_UNKNOWN      (GOPHER_ATTRIBUTE_ID_BASE + 8)

#define GOPHER_ATTRIBUTE_ID_ALL         (GOPHER_ATTRIBUTE_ID_BASE + 9)
#define GOPHER_ATTRIBUTE_ID_ADM      	(GOPHER_ATTRIBUTE_ID_BASE + 10)
#define GOPHER_ATTRIBUTE_ID_MOD_DATE    (GOPHER_ATTRIBUTE_ID_BASE + 11)
#define GOPHER_ATTRIBUTE_ID_TTL         (GOPHER_ATTRIBUTE_ID_BASE + 12)
#define GOPHER_ATTRIBUTE_ID_SCORE       (GOPHER_ATTRIBUTE_ID_BASE + 13)
#define GOPHER_ATTRIBUTE_ID_RANGE       (GOPHER_ATTRIBUTE_ID_BASE + 14)
#define GOPHER_ATTRIBUTE_ID_SITE        (GOPHER_ATTRIBUTE_ID_BASE + 15)
#define GOPHER_ATTRIBUTE_ID_ORG         (GOPHER_ATTRIBUTE_ID_BASE + 16)
#define GOPHER_ATTRIBUTE_ID_LOCATION    (GOPHER_ATTRIBUTE_ID_BASE + 17)
#define GOPHER_ATTRIBUTE_ID_GEOG        (GOPHER_ATTRIBUTE_ID_BASE + 18)
#define GOPHER_ATTRIBUTE_ID_TIMEZONE    (GOPHER_ATTRIBUTE_ID_BASE + 19)
#define GOPHER_ATTRIBUTE_ID_PROVIDER    (GOPHER_ATTRIBUTE_ID_BASE + 20)
#define GOPHER_ATTRIBUTE_ID_VERSION     (GOPHER_ATTRIBUTE_ID_BASE + 21)
#define GOPHER_ATTRIBUTE_ID_ABSTRACT    (GOPHER_ATTRIBUTE_ID_BASE + 22)
#define GOPHER_ATTRIBUTE_ID_VIEW        (GOPHER_ATTRIBUTE_ID_BASE + 23)
#define GOPHER_ATTRIBUTE_ID_TREEWALK    (GOPHER_ATTRIBUTE_ID_BASE + 24)
#define GOPHER_ATTRIBUTE_ID_UNKNOWN     (GOPHER_ATTRIBUTE_ID_BASE + 25)

#define GOPHER_TYPE_TEXT_FILE       0x00000001
#define GOPHER_TYPE_DIRECTORY       0x00000002
#define GOPHER_TYPE_CSO             0x00000004
#define GOPHER_TYPE_ERROR           0x00000008
#define GOPHER_TYPE_MAC_BINHEX      0x00000010
#define GOPHER_TYPE_DOS_ARCHIVE     0x00000020
#define GOPHER_TYPE_UNIX_UUENCODED  0x00000040
#define GOPHER_TYPE_INDEX_SERVER    0x00000080
#define GOPHER_TYPE_TELNET          0x00000100
#define GOPHER_TYPE_BINARY          0x00000200
#define GOPHER_TYPE_REDUNDANT       0x00000400
#define GOPHER_TYPE_TN3270          0x00000800
#define GOPHER_TYPE_GIF             0x00001000
#define GOPHER_TYPE_IMAGE           0x00002000
#define GOPHER_TYPE_BITMAP          0x00004000
#define GOPHER_TYPE_MOVIE           0x00008000
#define GOPHER_TYPE_SOUND           0x00010000
#define GOPHER_TYPE_HTML            0x00020000
#define GOPHER_TYPE_PDF             0x00040000
#define GOPHER_TYPE_CALENDAR        0x00080000
#define GOPHER_TYPE_INLINE          0x00100000
#define GOPHER_TYPE_UNKNOWN         0x20000000
#define GOPHER_TYPE_ASK             0x40000000
#define GOPHER_TYPE_GOPHER_PLUS     0x80000000

#define IS_GOPHER_FILE(type)            (((type) & GOPHER_TYPE_FILE_MASK) ? TRUE : FALSE)
#define IS_GOPHER_DIRECTORY(type)       (((type) & GOPHER_TYPE_DIRECTORY) ? TRUE : FALSE)
#define IS_GOPHER_PHONE_SERVER(type)    (((type) & GOPHER_TYPE_CSO) ? TRUE : FALSE)
#define IS_GOPHER_ERROR(type)           (((type) & GOPHER_TYPE_ERROR) ? TRUE : FALSE)
#define IS_GOPHER_INDEX_SERVER(type)    (((type) & GOPHER_TYPE_INDEX_SERVER) ? TRUE : FALSE)
#define IS_GOPHER_TELNET_SESSION(type)  (((type) & GOPHER_TYPE_TELNET) ? TRUE : FALSE)
#define IS_GOPHER_BACKUP_SERVER(type)   (((type) & GOPHER_TYPE_REDUNDANT) ? TRUE : FALSE)
#define IS_GOPHER_TN3270_SESSION(type)  (((type) & GOPHER_TYPE_TN3270) ? TRUE : FALSE)
#define IS_GOPHER_ASK(type)             (((type) & GOPHER_TYPE_ASK) ? TRUE : FALSE)
#define IS_GOPHER_PLUS(type)            (((type) & GOPHER_TYPE_GOPHER_PLUS) ? TRUE : FALSE)
#define IS_GOPHER_TYPE_KNOWN(type)      (((type) & GOPHER_TYPE_UNKNOWN) ? FALSE : TRUE)

#define GOPHER_TYPE_FILE_MASK       (GOPHER_TYPE_TEXT_FILE          \
                                    | GOPHER_TYPE_MAC_BINHEX        \
                                    | GOPHER_TYPE_DOS_ARCHIVE       \
                                    | GOPHER_TYPE_UNIX_UUENCODED    \
                                    | GOPHER_TYPE_BINARY            \
                                    | GOPHER_TYPE_GIF               \
                                    | GOPHER_TYPE_IMAGE             \
                                    | GOPHER_TYPE_BITMAP            \
                                    | GOPHER_TYPE_MOVIE             \
                                    | GOPHER_TYPE_SOUND             \
                                    | GOPHER_TYPE_HTML              \
                                    | GOPHER_TYPE_PDF               \
                                    | GOPHER_TYPE_CALENDAR          \
                                    | GOPHER_TYPE_INLINE            \
                                    )


#define HTTP_MAJOR_VERSION      		1
#define HTTP_MINOR_VERSION      		0

#define HTTP_VERSION            		TEXT("HTTP/1.0")

#define HTTP_QUERY_MIME_VERSION                 0
#define HTTP_QUERY_CONTENT_TYPE                 1
#define HTTP_QUERY_CONTENT_TRANSFER_ENCODING    2
#define HTTP_QUERY_CONTENT_ID                   3
#define HTTP_QUERY_CONTENT_DESCRIPTION          4
#define HTTP_QUERY_CONTENT_LENGTH               5
#define HTTP_QUERY_CONTENT_LANGUAGE             6
#define HTTP_QUERY_ALLOW                        7
#define HTTP_QUERY_PUBLIC                       8
#define HTTP_QUERY_DATE                         9
#define HTTP_QUERY_EXPIRES                      10
#define HTTP_QUERY_LAST_MODIFIED                11
#define HTTP_QUERY_MESSAGE_ID                   12
#define HTTP_QUERY_URI                          13
#define HTTP_QUERY_DERIVED_FROM                 14
#define HTTP_QUERY_COST                         15
#define HTTP_QUERY_LINK                         16
#define HTTP_QUERY_PRAGMA                       17
#define HTTP_QUERY_VERSION                      18
#define HTTP_QUERY_STATUS_CODE                  19
#define HTTP_QUERY_STATUS_TEXT                  20
#define HTTP_QUERY_RAW_HEADERS                  21
#define HTTP_QUERY_RAW_HEADERS_CRLF             22
#define HTTP_QUERY_CONNECTION                   23
#define HTTP_QUERY_ACCEPT                       24
#define HTTP_QUERY_ACCEPT_CHARSET               25
#define HTTP_QUERY_ACCEPT_ENCODING              26
#define HTTP_QUERY_ACCEPT_LANGUAGE              27
#define HTTP_QUERY_AUTHORIZATION                28
#define HTTP_QUERY_CONTENT_ENCODING             29
#define HTTP_QUERY_FORWARDED                    30
#define HTTP_QUERY_FROM                         31
#define HTTP_QUERY_IF_MODIFIED_SINCE            32
#define HTTP_QUERY_LOCATION                     33
#define HTTP_QUERY_ORIG_URI                     34
#define HTTP_QUERY_REFERER                      35
#define HTTP_QUERY_RETRY_AFTER                  36
#define HTTP_QUERY_SERVER                       37
#define HTTP_QUERY_TITLE                        38
#define HTTP_QUERY_USER_AGENT                   39
#define HTTP_QUERY_WWW_AUTHENTICATE             40
#define HTTP_QUERY_PROXY_AUTHENTICATE           41
#define HTTP_QUERY_ACCEPT_RANGES                42
#define HTTP_QUERY_SET_COOKIE                   43
#define HTTP_QUERY_COOKIE                       44
#define HTTP_QUERY_MAX                          44
#define HTTP_QUERY_CUSTOM                       65535

#define HTTP_QUERY_FLAG_REQUEST_HEADERS         0x80000000
#define HTTP_QUERY_FLAG_SYSTEMTIME              0x40000000
#define HTTP_QUERY_FLAG_NUMBER                  0x20000000
#define HTTP_QUERY_FLAG_COALESCE                0x10000000
#define HTTP_QUERY_MODIFIER_FLAGS_MASK          (HTTP_QUERY_FLAG_REQUEST_HEADERS    \
                                                | HTTP_QUERY_FLAG_SYSTEMTIME        \
                                                | HTTP_QUERY_FLAG_NUMBER            \
                                                | HTTP_QUERY_FLAG_COALESCE          \
                                                )

#define HTTP_QUERY_HEADER_MASK                  (~HTTP_QUERY_MODIFIER_FLAGS_MASK)

#define HTTP_STATUS_OK              200
#define HTTP_STATUS_CREATED         201
#define HTTP_STATUS_ACCEPTED        202
#define HTTP_STATUS_PARTIAL         203
#define HTTP_STATUS_NO_CONTENT      204

#define HTTP_STATUS_AMBIGUOUS       300
#define HTTP_STATUS_MOVED           301
#define HTTP_STATUS_REDIRECT        302
#define HTTP_STATUS_REDIRECT_METHOD 303
#define HTTP_STATUS_NOT_MODIFIED    304

#define HTTP_STATUS_BAD_REQUEST     400
#define HTTP_STATUS_DENIED          401
#define HTTP_STATUS_PAYMENT_REQ     402
#define HTTP_STATUS_FORBIDDEN       403
#define HTTP_STATUS_NOT_FOUND       404
#define HTTP_STATUS_BAD_METHOD      405
#define HTTP_STATUS_NONE_ACCEPTABLE 406
#define HTTP_STATUS_PROXY_AUTH_REQ  407
#define HTTP_STATUS_REQUEST_TIME    408
#define HTTP_STATUS_CONFLICT        409
#define HTTP_STATUS_GONE            410
#define HTTP_STATUS_AUTH_REFUSED    411

#define HTTP_STATUS_SERVER_ERROR    500
#define HTTP_STATUS_NOT_SUPPORTED   501
#define HTTP_STATUS_BAD_GATEWAY     502
#define HTTP_STATUS_SERVICE_UNAVAIL 503
#define HTTP_STATUS_GATEWAY_TIME    504

#define HTTP_ADDREQ_INDEX_MASK      			0x0000FFFF
#define HTTP_ADDREQ_FLAGS_MASK      			0xFFFF0000
#define HTTP_ADDREQ_FLAG_ADD_IF_NEW 			0x10000000
#define HTTP_ADDREQ_FLAG_ADD        			0x20000000
#define HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA       	0x40000000
#define HTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON   	0x01000000
#define HTTP_ADDREQ_FLAG_COALESCE                  	HTTP_ADDREQ_FLAG_COALESCE_WITH_COMMA
#define HTTP_ADDREQ_FLAG_REPLACE    			0x80000000

#define FLAGS_ERROR_UI_FILTER_FOR_ERRORS        	0x01
#define FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS     	0x02
#define FLAGS_ERROR_UI_FLAGS_GENERATE_DATA      	0x04
#define FLAGS_ERROR_UI_FLAGS_NO_UI              	0x08

#define INTERNET_ERROR_BASE                     	12000

#define ERROR_INTERNET_OUT_OF_HANDLES           (INTERNET_ERROR_BASE + 1)
#define ERROR_INTERNET_TIME                 	(INTERNET_ERROR_BASE + 2)
#define ERROR_INTERNET_EXTENDED_ERROR           (INTERNET_ERROR_BASE + 3)
#define ERROR_INTERNET_INTERNAL_ERROR           (INTERNET_ERROR_BASE + 4)
#define ERROR_INTERNET_INVALID_URL              (INTERNET_ERROR_BASE + 5)
#define ERROR_INTERNET_UNRECOGNIZED_SCHEME      (INTERNET_ERROR_BASE + 6)
#define ERROR_INTERNET_NAME_NOT_RESOLVED        (INTERNET_ERROR_BASE + 7)
#define ERROR_INTERNET_PROTOCOL_NOT_FOUND       (INTERNET_ERROR_BASE + 8)
#define ERROR_INTERNET_INVALID_OPTION           (INTERNET_ERROR_BASE + 9)
#define ERROR_INTERNET_BAD_OPTION_LENGTH        (INTERNET_ERROR_BASE + 10)
#define ERROR_INTERNET_OPTION_NOT_SETTABLE      (INTERNET_ERROR_BASE + 11)
#define ERROR_INTERNET_SHUTDOWN                 (INTERNET_ERROR_BASE + 12)
#define ERROR_INTERNET_INCORRECT_USER_NAME      (INTERNET_ERROR_BASE + 13)
#define ERROR_INTERNET_INCORRECT_PASSWORD       (INTERNET_ERROR_BASE + 14)
#define ERROR_INTERNET_LOGIN_FAILURE            (INTERNET_ERROR_BASE + 15)
#define ERROR_INTERNET_INVALID_OPERATION        (INTERNET_ERROR_BASE + 16)
#define ERROR_INTERNET_OPERATION_CANCELLED      (INTERNET_ERROR_BASE + 17)
#define ERROR_INTERNET_INCORRECT_HANDLE_TYPE    (INTERNET_ERROR_BASE + 18)
#define ERROR_INTERNET_INCORRECT_HANDLE_STATE   (INTERNET_ERROR_BASE + 19)
#define ERROR_INTERNET_NOT_PROXY_REQUEST        (INTERNET_ERROR_BASE + 20)
#define ERROR_INTERNET_REGISTRY_VALUE_NOT_FOUND (INTERNET_ERROR_BASE + 21)
#define ERROR_INTERNET_BAD_REGISTRY_PARAMETER   (INTERNET_ERROR_BASE + 22)
#define ERROR_INTERNET_NO_DIRECT_ACCESS         (INTERNET_ERROR_BASE + 23)
#define ERROR_INTERNET_NO_CONTEXT               (INTERNET_ERROR_BASE + 24)
#define ERROR_INTERNET_NO_CALLBACK              (INTERNET_ERROR_BASE + 25)
#define ERROR_INTERNET_REQUEST_PENDING          (INTERNET_ERROR_BASE + 26)
#define ERROR_INTERNET_INCORRECT_FORMAT         (INTERNET_ERROR_BASE + 27)
#define ERROR_INTERNET_ITEM_NOT_FOUND           (INTERNET_ERROR_BASE + 28)
#define ERROR_INTERNET_CANNOT_CONNECT           (INTERNET_ERROR_BASE + 29)
#define ERROR_INTERNET_CONNECTION_ABORTED       (INTERNET_ERROR_BASE + 30)
#define ERROR_INTERNET_CONNECTION_RESET         (INTERNET_ERROR_BASE + 31)
#define ERROR_INTERNET_FORCE_RETRY              (INTERNET_ERROR_BASE + 32)
#define ERROR_INTERNET_INVALID_PROXY_REQUEST    (INTERNET_ERROR_BASE + 33)

#define ERROR_INTERNET_HANDLE_EXISTS            (INTERNET_ERROR_BASE + 36)
#define ERROR_INTERNET_SEC_CERT_DATE_INVALID    (INTERNET_ERROR_BASE + 37)
#define ERROR_INTERNET_SEC_CERT_CN_INVALID      (INTERNET_ERROR_BASE + 38)
#define ERROR_INTERNET_HTTP_TO_HTTPS_ON_REDIR   (INTERNET_ERROR_BASE + 39)
#define ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR   (INTERNET_ERROR_BASE + 40)
#define ERROR_INTERNET_MIXED_SECURITY           (INTERNET_ERROR_BASE + 41)
#define ERROR_INTERNET_CHG_POST_IS_NON_SECURE   (INTERNET_ERROR_BASE + 42)
#define ERROR_INTERNET_POST_IS_NON_SECURE       (INTERNET_ERROR_BASE + 43)

#define ERROR_FTP_TRANSFER_IN_PROGRESS          (INTERNET_ERROR_BASE + 110)
#define ERROR_FTP_DROPPED                       (INTERNET_ERROR_BASE + 111)

#define ERROR_GOPHER_PROTOCOL_ERROR             (INTERNET_ERROR_BASE + 130)
#define ERROR_GOPHER_NOT_FILE                   (INTERNET_ERROR_BASE + 131)
#define ERROR_GOPHER_DATA_ERROR                 (INTERNET_ERROR_BASE + 132)
#define ERROR_GOPHER_END_OF_DATA                (INTERNET_ERROR_BASE + 133)
#define ERROR_GOPHER_INVALID_LOCATOR            (INTERNET_ERROR_BASE + 134)
#define ERROR_GOPHER_INCORRECT_LOCATOR_TYPE     (INTERNET_ERROR_BASE + 135)
#define ERROR_GOPHER_NOT_GOPHER_PLUS            (INTERNET_ERROR_BASE + 136)
#define ERROR_GOPHER_ATTRIBUTE_NOT_FOUND        (INTERNET_ERROR_BASE + 137)
#define ERROR_GOPHER_UNKNOWN_LOCATOR            (INTERNET_ERROR_BASE + 138)

#define ERROR_HTTP_HEADER_NOT_FOUND             (INTERNET_ERROR_BASE + 150)
#define ERROR_HTTP_DOWNLEVEL_SERVER             (INTERNET_ERROR_BASE + 151)
#define ERROR_HTTP_INVALID_SERVER_RESPONSE      (INTERNET_ERROR_BASE + 152)
#define ERROR_HTTP_INVALID_HEADER               (INTERNET_ERROR_BASE + 153)
#define ERROR_HTTP_INVALID_QUERY_REQUEST        (INTERNET_ERROR_BASE + 154)
#define ERROR_HTTP_HEADER_ALREADY_EXISTS        (INTERNET_ERROR_BASE + 155)
#define ERROR_HTTP_REDIRECT_FAILED              (INTERNET_ERROR_BASE + 156)

#define INTERNET_ERROR_LAST                     ERROR_HTTP_REDIRECT_FAILED

#define CACHE_ENTRY_ATTRIBUTE_FC    		0x00000004
#define CACHE_ENTRY_HITRATE_FC      		0x00000010
#define CACHE_ENTRY_MODTIME_FC      		0x00000040
#define CACHE_ENTRY_EXPTIME_FC      		0x00000080
#define CACHE_ENTRY_ACCTIME_FC      		0x00000100
#define CACHE_ENTRY_SYNCTIME_FC     		0x00000200
#define CACHE_ENTRY_HEADERINFO_FC   		0x00000400

#define NORMAL_CACHE_ENTRY      0x00000001
#define STABLE_CACHE_ENTRY      0x00000002
#define STICKY_CACHE_ENTRY      0x00000004

#define SPARSE_CACHE_ENTRY      0x00010000
#define OCX_CACHE_ENTRY         0x00020000

//
// INTERNET_CACHE_ENTRY_INFO -
//

typedef struct _INTERNET_CACHE_ENTRY_INFOA {
    DWORD dwStructSize;
    LPSTR lpszSourceUrlName;
    LPSTR lpszLocalFileName;
    DWORD CacheEntryType;
    DWORD dwUseCount;
    DWORD dwHitRate;
    DWORD dwSizeLow;
    DWORD dwSizeHigh;
    FILETIME LastModifiedTime;
    FILETIME ExpireTime;
    FILETIME LastAccessTime;
    FILETIME LastSyncTime;
    LPBYTE lpHeaderInfo;
    DWORD dwHeaderInfoSize;
    LPSTR lpszFileExtension;
    DWORD dwReserved;
} INTERNET_CACHE_ENTRY_INFOA, *LPINTERNET_CACHE_ENTRY_INFOA;

typedef struct _INTERNET_CACHE_ENTRY_INFOW {
    DWORD dwStructSize;
    LPSTR lpszSourceUrlName;
    LPWSTR  lpszLocalFileName;
    DWORD CacheEntryType;
    DWORD dwUseCount;
    DWORD dwHitRate;
    DWORD dwSizeLow;
    DWORD dwSizeHigh;
    FILETIME LastModifiedTime;
    FILETIME ExpireTime;
    FILETIME LastAccessTime;
    FILETIME LastSyncTime;
    LPBYTE lpHeaderInfo;
    DWORD dwHeaderInfoSize;
    LPWSTR lpszFileExtension;
    DWORD dwReserved;
} INTERNET_CACHE_ENTRY_INFOW, *LPINTERNET_CACHE_ENTRY_INFOW;

#define INTERNET_CACHE_ENTRY_INFO 	WINELIB_NAME_AW(INTERNET_CACHE_ENTRY_INFO)
#define LPINTERNET_CACHE_ENTRY_INFO 	WINELIB_NAME_AW(LPINTERNET_CACHE_ENTRY_INFO)

typedef enum {
    INTERNET_SCHEME_PARTIAL = -2,
    INTERNET_SCHEME_UNKNOWN = -1,
    INTERNET_SCHEME_DEFAULT = 0,
    INTERNET_SCHEME_FTP,
    INTERNET_SCHEME_GOPHER,
    INTERNET_SCHEME_HTTP,
    INTERNET_SCHEME_HTTPS,
    INTERNET_SCHEME_FILE,
    INTERNET_SCHEME_NEWS,
    INTERNET_SCHEME_MAILTO,
    INTERNET_SCHEME_FIRST = INTERNET_SCHEME_FTP,
    INTERNET_SCHEME_LAST = INTERNET_SCHEME_MAILTO
} INTERNET_SCHEME, *LPINTERNET_SCHEME;

typedef struct {
    DWORD dwResult;
    DWORD dwError;
} INTERNET_ASYNC_RESULT, *LPINTERNET_ASYNC_RESULT;

typedef struct {
    DWORD dwStatus;
    DWORD dwSize;
} INTERNET_PREFETCH_STATUS, *LPINTERNET_PREFETCH_STATUS;

typedef struct {
    DWORD dwAccessType;
    LPCTSTR lpszProxy;
    LPCTSTR lpszProxyBypass;
} INTERNET_PROXY_INFO, *LPINTERNET_PROXY_INFO;

typedef struct {
    DWORD   dwStructSize;       
    LPSTR   lpszScheme;
    DWORD   dwSchemeLength;
    INTERNET_SCHEME nScheme;
    LPSTR   lpszHostName;
    DWORD   dwHostNameLength;
    INTERNET_PORT nPort;
    LPSTR   lpszUserName;
    DWORD   dwUserNameLength;
    LPSTR   lpszPassword;
    DWORD   dwPasswordLength;   
    LPSTR   lpszUrlPath;        
    DWORD   dwUrlPathLength;    
    LPSTR   lpszExtraInfo;      
    DWORD   dwExtraInfoLength;  
} URL_COMPONENTSA,* LPURL_COMPONENTSA;

typedef struct {
    DWORD   dwStructSize;       
    LPWSTR  lpszScheme;         
    DWORD   dwSchemeLength;     
    INTERNET_SCHEME nScheme;    
    LPWSTR  lpszHostName;       
    DWORD   dwHostNameLength;   
    INTERNET_PORT nPort;        
    LPWSTR  lpszUserName;       
    DWORD   dwUserNameLength;   
    LPWSTR  lpszPassword;       
    DWORD   dwPasswordLength;   
    LPWSTR  lpszUrlPath;        
    DWORD   dwUrlPathLength;    
    LPWSTR  lpszExtraInfo;      
    DWORD   dwExtraInfoLength;  
} URL_COMPONENTSW,* LPURL_COMPONENTSW;

DECL_WINELIB_TYPE_AW(URL_COMPONENTS)
DECL_WINELIB_TYPE_AW(LPURL_COMPONENTS)

typedef struct {
    FILETIME ftExpiry;
    FILETIME ftStart;
    LPTSTR lpszSubjectInfo;
    LPTSTR lpszIssuerInfo;
    LPTSTR lpszProtocolName;
    LPTSTR lpszSignatureAlgName;
    LPTSTR lpszEncryptionAlgName;
    DWORD dwKeySize;
} INTERNET_CERTIFICATE_INFO, *LPINTERNET_CERTIFICATE_INFO;

typedef VOID (* CALLBACK  INTERNET_STATUS_CALLBACK)(
    HINTERNET hInternet,
    DWORD dwContext,
    DWORD dwInternetStatus,
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
    );

typedef INTERNET_STATUS_CALLBACK * LPINTERNET_STATUS_CALLBACK;

typedef struct {
    CHAR   DisplayString[MAX_GOPHER_DISPLAY_TEXT + 1];
    DWORD GopherType;
    DWORD SizeLow;
    DWORD SizeHigh;
    FILETIME LastModificationTime;
    CHAR   Locator[MAX_GOPHER_LOCATOR_LENGTH + 1];
} GOPHER_FIND_DATAA, * LPGOPHER_FIND_DATAA;

typedef struct {
    WCHAR  DisplayString[MAX_GOPHER_DISPLAY_TEXT + 1];
    DWORD GopherType;
    DWORD SizeLow;
    DWORD SizeHigh;
    FILETIME LastModificationTime;
    WCHAR  Locator[MAX_GOPHER_LOCATOR_LENGTH + 1];
} GOPHER_FIND_DATAW, * LPGOPHER_FIND_DATAW;

#define GOPHER_FIND_DATA 	WINELIB_NAME_AW(GOPHER_FIND_DATA)
#define LPGOPHER_FIND_DATA 	WINELIB_NAME_AW(LPGOPHER_FIND_DATA)

typedef struct {
    LPCTSTR Comment;
    LPCTSTR EmailAddress;
} GOPHER_ADMIN_ATTRIBUTE_TYPE, *LPGOPHER_ADMIN_ATTRIBUTE_TYPE;

typedef struct {
    FILETIME DateAndTime;
} GOPHER_MOD_DATE_ATTRIBUTE_TYPE, *LPGOPHER_MOD_DATE_ATTRIBUTE_TYPE;

typedef struct {
    DWORD Ttl;
} GOPHER_TTL_ATTRIBUTE_TYPE, *LPGOPHER_TTL_ATTRIBUTE_TYPE;

typedef struct {
    INT Score;
} GOPHER_SCORE_ATTRIBUTE_TYPE, *LPGOPHER_SCORE_ATTRIBUTE_TYPE;

typedef struct {
    INT LowerBound;
    INT UpperBound;
} GOPHER_SCORE_RANGE_ATTRIBUTE_TYPE, *LPGOPHER_SCORE_RANGE_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Site;
} GOPHER_SITE_ATTRIBUTE_TYPE, *LPGOPHER_SITE_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Organization;
} GOPHER_ORGANIZATION_ATTRIBUTE_TYPE, *LPGOPHER_ORGANIZATION_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Location;
} GOPHER_LOCATION_ATTRIBUTE_TYPE, *LPGOPHER_LOCATION_ATTRIBUTE_TYPE;

typedef struct {
    INT DegreesNorth;
    INT MinutesNorth;
    INT SecondsNorth;
    INT DegreesEast;
    INT MinutesEast;
    INT SecondsEast;
} GOPHER_GEOGRAPHICAL_LOCATION_ATTRIBUTE_TYPE, *LPGOPHER_GEOGRAPHICAL_LOCATION_ATTRIBUTE_TYPE;

typedef struct {
    INT Zone;
} GOPHER_TIMEZONE_ATTRIBUTE_TYPE, *LPGOPHER_TIMEZONE_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Provider;
} GOPHER_PROVIDER_ATTRIBUTE_TYPE, *LPGOPHER_PROVIDER_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Version;
} GOPHER_VERSION_ATTRIBUTE_TYPE, *LPGOPHER_VERSION_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR ShortAbstract;
    LPCTSTR AbstractFile;
} GOPHER_ABSTRACT_ATTRIBUTE_TYPE, *LPGOPHER_ABSTRACT_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR ContentType;
    LPCTSTR Language;
    DWORD Size;
} GOPHER_VIEW_ATTRIBUTE_TYPE, *LPGOPHER_VIEW_ATTRIBUTE_TYPE;

typedef struct {
    BOOL TreeWalk;
} GOPHER_VERONICA_ATTRIBUTE_TYPE, *LPGOPHER_VERONICA_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR QuestionType;
    LPCTSTR QuestionText;
} GOPHER_ASK_ATTRIBUTE_TYPE, *LPGOPHER_ASK_ATTRIBUTE_TYPE;

typedef struct {
    LPCTSTR Text;
} GOPHER_UNKNOWN_ATTRIBUTE_TYPE, *LPGOPHER_UNKNOWN_ATTRIBUTE_TYPE;

typedef struct {
    DWORD CategoryId;
    DWORD AttributeId;
    union {
        GOPHER_ADMIN_ATTRIBUTE_TYPE Admin;
        GOPHER_MOD_DATE_ATTRIBUTE_TYPE ModDate;
        GOPHER_TTL_ATTRIBUTE_TYPE Ttl;
        GOPHER_SCORE_ATTRIBUTE_TYPE Score;
        GOPHER_SCORE_RANGE_ATTRIBUTE_TYPE ScoreRange;
        GOPHER_SITE_ATTRIBUTE_TYPE Site;
        GOPHER_ORGANIZATION_ATTRIBUTE_TYPE Organization;
        GOPHER_LOCATION_ATTRIBUTE_TYPE Location;
        GOPHER_GEOGRAPHICAL_LOCATION_ATTRIBUTE_TYPE GeographicalLocation;
        GOPHER_TIMEZONE_ATTRIBUTE_TYPE TimeZone;
        GOPHER_PROVIDER_ATTRIBUTE_TYPE Provider;
        GOPHER_VERSION_ATTRIBUTE_TYPE Version;
        GOPHER_ABSTRACT_ATTRIBUTE_TYPE Abstract;
        GOPHER_VIEW_ATTRIBUTE_TYPE View;
        GOPHER_VERONICA_ATTRIBUTE_TYPE Veronica;
        GOPHER_ASK_ATTRIBUTE_TYPE Ask;
        GOPHER_UNKNOWN_ATTRIBUTE_TYPE Unknown;
    } AttributeType;
} GOPHER_ATTRIBUTE_TYPE, *LPGOPHER_ATTRIBUTE_TYPE;

typedef BOOL (* CALLBACK  GOPHER_ATTRIBUTE_ENUMERATOR)(
    LPGOPHER_ATTRIBUTE_TYPE lpAttributeInfo,
    DWORD dwError
    );

//WININET apis
BOOL WINAPI InternetTimeFromSystemTime(CONST SYSTEMTIME *pst, DWORD dwRFC,
                                       LPSTR lpszTime, DWORD cbTime);

BOOL WINAPI InternetCrackUrlA(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags,
                              LPURL_COMPONENTSA lpUrlComponents);

BOOL WINAPI InternetCrackUrlW(LPCWSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags,
                              LPURL_COMPONENTSW lpUrlComponents);

#define InternetCrackUrl WINELIB_NAME_AW(InternetCrackUrl)

BOOL WINAPI InternetCreateUrlA(LPURL_COMPONENTSA lpUrlComponents, DWORD dwFlags,
                               LPSTR lpszUrl, LPDWORD lpdwUrlLength);

BOOL WINAPI InternetCreateUrlW(LPURL_COMPONENTSW lpUrlComponents, DWORD dwFlags,
                               LPWSTR lpszUrl, LPDWORD lpdwUrlLength);

#define InternetCreateUrl WINELIB_NAME_AW(InternetCreateUrl)

BOOL WINAPI InternetCanonicalizeUrlA(LPCSTR lpszUrl, LPSTR lpszBuffer,
                                     LPDWORD lpdwBufferLength, DWORD dwFlags);

BOOL WINAPI InternetCanonicalizeUrlW(LPCWSTR lpszUrl, LPWSTR lpszBuffer,
                                     LPDWORD lpdwBufferLength, DWORD dwFlags);

#define InternetCanonicalizeUrl WINELIB_NAME_AW(InternetCanonicalizeUrl)


BOOL WINAPI InternetCombineUrlA(LPCSTR lpszBaseUrl, LPCSTR lpszRelativeUrl,
                                LPSTR lpszBuffer, LPDWORD lpdwBufferLength,
                                DWORD dwFlags);

BOOL WINAPI InternetCombineUrlW(LPCWSTR lpszBaseUrl, LPCWSTR lpszRelativeUrl,
                                LPWSTR lpszBuffer, LPDWORD lpdwBufferLength,
                                DWORD dwFlags);

#define InternetCombineUrl WINELIB_NAME_AW(InternetCombineUrl)


HINTERNET WINAPI InternetOpenA(LPCSTR lpszAgent, DWORD dwAccessType,
                               LPCSTR lpszProxy, LPCSTR lpszProxyBypass,
                               DWORD dwFlags);

HINTERNET WINAPI InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType,
                               LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass,
                               DWORD dwFlags);

#define InternetOpen WINELIB_NAME_AW(InternetOpen)

BOOL WINAPI InternetCloseHandle(HINTERNET hInternet);

HINTERNET WINAPI InternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName,
                                  INTERNET_PORT nServerPort, LPCSTR lpszUserName,
                                  LPCSTR lpszPassword, DWORD dwService,
                                  DWORD dwFlags, DWORD dwContext);

HINTERNET WINAPI InternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName,
                                  INTERNET_PORT nServerPort, LPCWSTR lpszUserName,
                                  LPCWSTR lpszPassword, DWORD dwService,
                                  DWORD dwFlags, DWORD dwContext);

#define InternetConnect WINELIB_NAME_AW(InternetConnect)


HINTERNET WINAPI InternetOpenUrlA(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders,
                                  DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext);

HINTERNET WINAPI InternetOpenUrlW(HINTERNET hInternet, LPCWSTR lpszUrl, LPCWSTR lpszHeaders,
                                  DWORD dwHeadersLength, DWORD dwFlags, DWORD dwContext);

#define InternetOpenUrl WINELIB_NAME_AW(InternetOpenUrl)

BOOL WINAPI InternetReadFile(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead,
                             LPDWORD lpdwNumberOfBytesRead);


DWORD WINAPI InternetSetFilePointer(HINTERNET hFile, LONG lDistanceToMove,
                                    PVOID pReserved, DWORD dwMoveMethod,
                                    DWORD dwContext);


BOOL WINAPI InternetWriteFile(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite,
                              LPDWORD lpdwNumberOfBytesWritten);


BOOL WINAPI InternetQueryDataAvailable(HINTERNET hFile,LPDWORD lpdwNumberOfBytesAvailable,    
                                       DWORD dwFlags, DWORD dwContext);


BOOL WINAPI InternetFindNextFileA(HINTERNET hFind, LPVOID lpvFindData);
BOOL WINAPI InternetFindNextFileW(HINTERNET hFind, LPVOID lpvFindData);
#define InternetFindNextFile WINELIB_NAME_AW(InternetFindNextFile)

BOOL WINAPI InternetQueryOptionA(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength);
BOOL WINAPI InternetQueryOptionW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 LPDWORD lpdwBufferLength);
#define InternetQueryOption WINELIB_NAME_AW(InternetQueryOption)

BOOL WINAPI InternetSetOptionA(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                               DWORD dwBufferLength);
BOOL WINAPI InternetSetOptionW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                               DWORD dwBufferLength);
#define InternetSetOption WINELIB_NAME_AW(InternetSetOption)

BOOL WINAPI InternetSetOptionExA(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 DWORD dwBufferLength, DWORD dwFlags);
BOOL WINAPI InternetSetOptionExW(HINTERNET hInternet, DWORD dwOption, LPVOID lpBuffer,
                                 DWORD dwBufferLength, DWORD dwFlags);
#define InternetSetOptionEx WINELIB_NAME_AW(InternetSetOptionEx)

BOOL WINAPI InternetGetLastResponseInfoA(LPDWORD lpdwError, LPSTR lpszBuffer,
                                         LPDWORD lpdwBufferLength);
BOOL WINAPI InternetGetLastResponseInfoW(LPDWORD lpdwError, LPWSTR lpszBuffer,
                                         LPDWORD lpdwBufferLength);
#define InternetGetLastResponseInfo WINELIB_NAME_AW(InternetGetLastResponseInfo)

INTERNET_STATUS_CALLBACK WINAPI InternetSetStatusCallback(HINTERNET hInternet,
                                                          INTERNET_STATUS_CALLBACK lpfnInternetCallback);

HINTERNET WINAPI FtpFindFirstFileA(HINTERNET hConnect, LPCSTR lpszSearchFile,
                                   LPWIN32_FIND_DATAA lpFindFileData,
                                   DWORD dwFlags, DWORD dwContext);
HINTERNET WINAPI FtpFindFirstFileW(HINTERNET hConnect, LPCWSTR lpszSearchFile,
                                   LPWIN32_FIND_DATAW lpFindFileData,
                                   DWORD dwFlags, DWORD dwContext);
#define FtpFindFirstFile WINELIB_NAME_AW(FtpFindFirstFile)


BOOL WINAPI FtpGetFileA(HINTERNET hConnect, LPCSTR lpszRemoteFile,
                        LPCSTR lpszNewFile, BOOL fFailIfExists,
                        DWORD dwFlagsAndAttributes, DWORD dwFlags,
                        DWORD dwContext);
BOOL WINAPI FtpGetFileW(HINTERNET hConnect, LPCWSTR lpszRemoteFile,
                        LPCWSTR lpszNewFile, BOOL fFailIfExists,
                        DWORD dwFlagsAndAttributes, DWORD dwFlags,
                        DWORD dwContext);
#define FtpGetFile WINELIB_NAME_AW(FtpGetFile)

BOOL WINAPI FtpPutFileA(HINTERNET hConnect, LPCSTR lpszLocalFile, LPCSTR lpszNewRemoteFile,
                        DWORD dwFlags, DWORD dwContext);

BOOL WINAPI FtpPutFileW(HINTERNET hConnect, LPCWSTR lpszLocalFile, LPCWSTR lpszNewRemoteFile,
                        DWORD dwFlags, DWORD dwContext);

#define FtpPutFile WINELIB_NAME_AW(FtpPutFile)

BOOL WINAPI FtpDeleteFileA(HINTERNET hConnect, LPCSTR lpszFileName);
BOOL WINAPI FtpDeleteFileW(HINTERNET hConnect, LPCWSTR lpszFileName);
#define FtpDeleteFile WINELIB_NAME_AW(FtpDeleteFile)

BOOL WINAPI FtpRenameFileA(HINTERNET hConnect, LPCSTR lpszExisting, LPCSTR lpszNew);
BOOL WINAPI FtpRenameFileW(HINTERNET hConnect, LPCWSTR lpszExisting,LPCWSTR lpszNew);
#define FtpRenameFile WINELIB_NAME_AW(FtpRenameFile)

HINTERNET WINAPI FtpOpenFileA(HINTERNET hConnect, LPCSTR lpszFileName,
                              DWORD dwAccess, DWORD dwFlags, DWORD dwContext);
HINTERNET WINAPI FtpOpenFileW(HINTERNET hConnect, LPCWSTR lpszFileName,
                              DWORD dwAccess, DWORD dwFlags, DWORD dwContext);
#define FtpOpenFile WINELIB_NAME_AW(FtpOpenFile)

BOOL WINAPI FtpCreateDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory);
BOOL WINAPI FtpCreateDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory);
#define FtpCreateDirectory WINELIB_NAME_AW(FtpCreateDirectory)

BOOL WINAPI FtpRemoveDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory);
BOOL WINAPI FtpRemoveDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory);
#define FtpRemoveDirectory WINELIB_NAME_AW(FtpRemoveDirectory)

BOOL WINAPI FtpSetCurrentDirectoryA(HINTERNET hConnect, LPCSTR lpszDirectory);
BOOL WINAPI FtpSetCurrentDirectoryW(HINTERNET hConnect, LPCWSTR lpszDirectory);
#define FtpSetCurrentDirectory WINELIB_NAME_AW(FtpSetCurrentDirectory)

BOOL WINAPI FtpGetCurrentDirectoryA(HINTERNET hConnect, LPSTR lpszCurrentDirectory,
                                    LPDWORD lpdwCurrentDirectory);
BOOL WINAPI FtpGetCurrentDirectoryW(HINTERNET hConnect, LPWSTR lpszCurrentDirectory,
                                    LPDWORD lpdwCurrentDirectory);
#define FtpGetCurrentDirectory WINELIB_NAME_AW(FtpGetCurrentDirectory)

BOOL WINAPI FtpCommandA(HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags,
                        LPCSTR lpszCommand, DWORD dwContext);
BOOL WINAPI FtpCommandW(HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags,
                        LPCWSTR lpszCommand, DWORD dwContext);
#define FtpCommand WINELIB_NAME_AW(FtpCommand)


BOOL WINAPI GopherCreateLocatorA(LPCSTR lpszHost,INTERNET_PORT nServerPort,
                                 LPCSTR lpszDisplayString, LPCSTR lpszSelectorString,
                                 DWORD dwGopherType, LPSTR lpszLocator,
                                 LPDWORD lpdwBufferLength);
BOOL WINAPI GopherCreateLocatorW(LPCWSTR lpszHost,INTERNET_PORT nServerPort,
                                 LPCWSTR lpszDisplayString, LPCWSTR lpszSelectorString,
                                 DWORD dwGopherType, LPWSTR lpszLocator,
                                 LPDWORD lpdwBufferLength);
#define GopherCreateLocator WINELIB_NAME_AW(GopherCreateLocator)

BOOL WINAPI GopherGetLocatorTypeA(LPCSTR lpszLocator, LPDWORD lpdwGopherType);
BOOL WINAPI GopherGetLocatorTypeW(LPCWSTR lpszLocator, LPDWORD lpdwGopherType);
#define GopherGetLocatorType WINELIB_NAME_AW(GopherGetLocatorType)

HINTERNET WINAPI GopherFindFirstFileA(HINTERNET hConnect, LPCSTR lpszLocator,
                                      LPCSTR lpszSearchString, LPGOPHER_FIND_DATAA lpFindData,
                                      DWORD dwFlags, DWORD dwContext);
HINTERNET WINAPI GopherFindFirstFileW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                      LPCWSTR lpszSearchString, LPGOPHER_FIND_DATAA lpFindData,
                                      DWORD dwFlags, DWORD dwContext);
#define GopherFindFirstFile WINELIB_NAME_AW(GopherFindFirstFile)

HINTERNET WINAPI GopherOpenFileA(HINTERNET hConnect, LPCSTR lpszLocator,
                                 LPCSTR lpszView, DWORD dwFlags, DWORD dwContext);
HINTERNET WINAPI GopherOpenFileW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                 LPCWSTR lpszView, DWORD dwFlags, DWORD dwContext);
#define GopherOpenFile WINELIB_NAME_AW(GopherOpenFile)

BOOL WINAPI GopherGetAttributeA(HINTERNET hConnect, LPCSTR lpszLocator,
                                LPCSTR lpszAttributeName, LPBYTE lpBuffer,
                                DWORD dwBufferLength, LPDWORD lpdwCharactersReturned,
                                GOPHER_ATTRIBUTE_ENUMERATOR lpfnEnumerator,
                                DWORD dwContext);
BOOL WINAPI GopherGetAttributeW(HINTERNET hConnect, LPCWSTR lpszLocator,
                                LPCWSTR lpszAttributeName, LPBYTE lpBuffer,
                                DWORD dwBufferLength, LPDWORD lpdwCharactersReturned,
                                GOPHER_ATTRIBUTE_ENUMERATOR lpfnEnumerator,
                                DWORD dwContext);
#define GopherGetAttribute WINELIB_NAME_AW(GopherGetAttribute)


HINTERNET WINAPI HttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb,
                                  LPCSTR lpszObjectName, LPCSTR lpszVersion,
                                  LPCSTR lpszReferrer, LPCSTR * lplpszAcceptTypes,
                                  DWORD dwFlags, DWORD dwContext);

HINTERNET WINAPI HttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb,
                                  LPCWSTR lpszObjectName, LPCWSTR lpszVersion,
                                  LPCWSTR lpszReferrer, LPCWSTR * lplpszAcceptTypes,
                                  DWORD dwFlags, DWORD dwContext);
#define HttpOpenRequest WINELIB_NAME_AW(HttpOpenRequest)

BOOL WINAPI HttpAddRequestHeadersA(HINTERNET hRequest, LPCSTR lpszHeaders,
                                   DWORD dwHeadersLength, DWORD dwModifiers);
BOOL WINAPI HttpAddRequestHeadersW(HINTERNET hRequest, LPCWSTR lpszHeaders,
                                   DWORD dwHeadersLength, DWORD dwModifiers);
#define HttpAddRequestHeaders WINELIB_NAME_AW(HttpAddRequestHeaders)

BOOL WINAPI HttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders,
                             DWORD dwHeadersLength, LPVOID lpOptional,
                             DWORD dwOptionalLength);
BOOL WINAPI HttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders,
                             DWORD dwHeadersLength, LPVOID lpOptional,
                             DWORD dwOptionalLength);
#define HttpSendRequest WINELIB_NAME_AW(HttpSendRequest)


BOOL WINAPI HttpQueryInfoA(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
BOOL WINAPI HttpQueryInfoW(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer,
                           LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
#define HttpQueryInfo WINELIB_NAME_AW(HttpQueryInfo)


BOOL WINAPI InternetSetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData);
BOOL WINAPI InternetSetCookieW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPCWSTR lpszCookieData);
#define InternetSetCookie WINELIB_NAME_AW(InternetSetCookie)

BOOL WINAPI InternetGetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPSTR lpCookieData,
                               LPDWORD lpdwSize);
BOOL WINAPI InternetGetCookieW(LPCWSTR lpszUrl, LPCWSTR lpszCookieName, LPWSTR lpCookieData,
                               LPDWORD lpdwSize);
#define InternetGetCookie WINELIB_NAME_AW(InternetGetCookie)

DWORD WINAPI InternetErrorDlg(HWND hWnd, HINTERNET hRequest,
                              DWORD dwError, DWORD dwFlags, LPVOID * lppvData);

DWORD WINAPI InternetConfirmZoneCrossing(HWND hWnd, LPSTR szUrlPrev,
                                         LPSTR szUrlNew, BOOL bPost);



BOOL WINAPI CreateUrlCacheEntryA(LPCSTR lpszUrlName, DWORD dwExpectedFileSize,
                                 LPCSTR lpszFileExtension, LPSTR lpszFileName,
                                 DWORD dwReserved);
BOOL WINAPI CreateUrlCacheEntryW(LPCWSTR lpszUrlName, DWORD dwExpectedFileSize,
                                 LPCWSTR lpszFileExtension, LPWSTR lpszFileName,
                                 DWORD dwReserved);
#define CreateUrlCacheEntry WINELIB_NAME_AW(CreateUrlCacheEntry)


BOOL WINAPI CommitUrlCacheEntryA(LPCSTR lpszUrlName, LPCSTR lpszLocalFileName,
                                 FILETIME ExpireTime, FILETIME LastModifiedTime,
                                 DWORD CacheEntryType, LPBYTE lpHeaderInfo,
                                 DWORD dwHeaderSize, LPCTSTR lpszFileExtension,
                                 DWORD dwReserved);
BOOL WINAPI CommitUrlCacheEntryW(LPCWSTR lpszUrlName, LPCWSTR lpszLocalFileName,
                                 FILETIME ExpireTime, FILETIME LastModifiedTime,
                                 DWORD CacheEntryType, LPBYTE lpHeaderInfo,
                                 DWORD dwHeaderSize, LPCTSTR lpszFileExtension,
                                 DWORD dwReserved);
#define CommitUrlCacheEntry WINELIB_NAME_AW(CommitUrlCacheEntry)


BOOL WINAPI RetrieveUrlCacheEntryFileA(LPCSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                       LPDWORD lpdwCacheEntryInfoBufferSize,
                                       DWORD dwReserved);
BOOL WINAPI RetrieveUrlCacheEntryFileW(LPCWSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                       LPDWORD lpdwCacheEntryInfoBufferSize,
                                       DWORD dwReserved);
#define RetrieveUrlCacheEntryFile WINELIB_NAME_AW(RetrieveUrlCacheEntryFile)


BOOL WINAPI UnlockUrlCacheEntryFile(LPCSTR lpszUrlName, DWORD dwReserved);

HANDLE WINAPI RetrieveUrlCacheEntryStreamA(LPCSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                           LPDWORD lpdwCacheEntryInfoBufferSize,
                                           BOOL fRandomRead, DWORD dwReserved);
HANDLE WINAPI RetrieveUrlCacheEntryStreamW(LPCWSTR  lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                           LPDWORD lpdwCacheEntryInfoBufferSize,
                                           BOOL fRandomRead, DWORD dwReserved);
#define RetrieveUrlCacheEntryStream WINELIB_NAME_AW(RetrieveUrlCacheEntryStream)


BOOL WINAPI ReadUrlCacheEntryStream(HANDLE hUrlCacheStream, DWORD dwLocation,
                                    LPVOID lpBuffer, LPDWORD lpdwLen,
                                    DWORD Reserved);


BOOL WINAPI UnlockUrlCacheEntryStream(HANDLE hUrlCacheStream, DWORD Reserved);

BOOL WINAPI GetUrlCacheEntryInfoA(LPCSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                  LPDWORD lpdwCacheEntryInfoBufferSize);
BOOL WINAPI GetUrlCacheEntryInfoW(LPCWSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                  LPDWORD lpdwCacheEntryInfoBufferSize);
#define GetUrlCacheEntryInfo WINELIB_NAME_AW(GetUrlCacheEntryInfo)


BOOL WINAPI SetUrlCacheEntryInfoA(LPCSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,
                                  DWORD dwFieldControl);
BOOL WINAPI SetUrlCacheEntryInfoW(LPCWSTR lpszUrlName, LPINTERNET_CACHE_ENTRY_INFOW lpCacheEntryInfo,
                                  DWORD dwFieldControl);
#define SetUrlCacheEntryInfo WINELIB_NAME_AW(SetUrlCacheEntryInfo)


HANDLE WINAPI FindFirstUrlCacheEntryA(LPCSTR lpszUrlSearchPattern,
                                      LPINTERNET_CACHE_ENTRY_INFOA lpFirstCacheEntryInfo,
                                      LPDWORD lpdwFirstCacheEntryInfoBufferSize);
HANDLE WINAPI FindFirstUrlCacheEntryW(LPCWSTR lpszUrlSearchPattern,
                                      LPINTERNET_CACHE_ENTRY_INFOW lpFirstCacheEntryInfo,
                                      LPDWORD lpdwFirstCacheEntryInfoBufferSize);
#define FindFirstUrlCacheEntry WINELIB_NAME_AW(FindFirstUrlCacheEntry)

BOOL WINAPI FindNextUrlCacheEntryA(HANDLE hEnumHandle,
                                   LPINTERNET_CACHE_ENTRY_INFOA lpNextCacheEntryInfo,
                                   LPDWORD lpdwNextCacheEntryInfoBufferSize);
BOOL WINAPI FindNextUrlCacheEntryW(HANDLE hEnumHandle,
                                   LPINTERNET_CACHE_ENTRY_INFOW lpNextCacheEntryInfo,
                                   LPDWORD lpdwNextCacheEntryInfoBufferSize);
#define FindNextUrlCacheEntry WINELIB_NAME_AW(FindNextUrlCacheEntry)

BOOL WINAPI FindCloseUrlCache(HANDLE hEnumHandle);

BOOL WINAPI DeleteUrlCacheEntry(LPCSTR lpszUrlName);

#endif // !__WININET_H__
