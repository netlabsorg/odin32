
#include "config.h"

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "winerror.h"
#include "winnls.h"
#include "wine/obj_base.h"
#include "strmif.h"
#include "errors.h"

#include "debugtools.h"
DEFAULT_DEBUG_CHANNEL(quartz);


static
LPWSTR QUARTZ_strncpyAtoW( LPWSTR lpwstr, LPCSTR lpstr, INT wbuflen )
{
	INT	len;

	len = MultiByteToWideChar( CP_ACP, 0, lpstr, -1, lpwstr, wbuflen );
	if ( len == 0 )
		*lpwstr = 0;
	return lpwstr;
}


static LPCSTR hresult_to_string( HRESULT hr )
{
	switch ( hr )
	{
	#define	ENTRY(x)	case x: return (const char*)#x
	/* some known codes */
	ENTRY(S_OK);
	ENTRY(S_FALSE);
	ENTRY(E_FAIL);
	ENTRY(E_POINTER);
	ENTRY(E_NOTIMPL);
	ENTRY(E_NOINTERFACE);
	ENTRY(E_OUTOFMEMORY);
	ENTRY(CLASS_E_CLASSNOTAVAILABLE);
	ENTRY(CLASS_E_NOAGGREGATION);

	/* vfwmsgs.h */
	ENTRY(VFW_S_NO_MORE_ITEMS);
	ENTRY(VFW_E_BAD_KEY);
	ENTRY(VFW_E_INVALIDMEDIATYPE);
	ENTRY(VFW_E_INVALIDSUBTYPE);
	ENTRY(VFW_E_NEED_OWNER);
	ENTRY(VFW_E_ENUM_OUT_OF_SYNC);
	ENTRY(VFW_E_ALREADY_CONNECTED);
	ENTRY(VFW_E_FILTER_ACTIVE);
	ENTRY(VFW_E_NO_TYPES);
	ENTRY(VFW_E_NO_ACCEPTABLE_TYPES);
	ENTRY(VFW_E_INVALID_DIRECTION);
	ENTRY(VFW_E_NOT_CONNECTED);
	ENTRY(VFW_E_NO_ALLOCATOR);
	ENTRY(VFW_E_RUNTIME_ERROR);
	ENTRY(VFW_E_BUFFER_NOTSET);
	ENTRY(VFW_E_BUFFER_OVERFLOW);
	ENTRY(VFW_E_BADALIGN);
	ENTRY(VFW_E_ALREADY_COMMITTED);
	ENTRY(VFW_E_BUFFERS_OUTSTANDING);
	ENTRY(VFW_E_NOT_COMMITTED);
	ENTRY(VFW_E_SIZENOTSET);
	ENTRY(VFW_E_NO_CLOCK);
	ENTRY(VFW_E_NO_SINK);
	ENTRY(VFW_E_NO_INTERFACE);
	ENTRY(VFW_E_NOT_FOUND);
	ENTRY(VFW_E_CANNOT_CONNECT);
	ENTRY(VFW_E_CANNOT_RENDER);
	ENTRY(VFW_E_CHANGING_FORMAT);
	ENTRY(VFW_E_NO_COLOR_KEY_SET);
	ENTRY(VFW_E_NOT_OVERLAY_CONNECTION);
	ENTRY(VFW_E_NOT_SAMPLE_CONNECTION);
	ENTRY(VFW_E_PALETTE_SET);
	ENTRY(VFW_E_COLOR_KEY_SET);
	ENTRY(VFW_E_NO_COLOR_KEY_FOUND);
	ENTRY(VFW_E_NO_PALETTE_AVAILABLE);
	ENTRY(VFW_E_NO_DISPLAY_PALETTE);
	ENTRY(VFW_E_TOO_MANY_COLORS);
	ENTRY(VFW_E_STATE_CHANGED);
	ENTRY(VFW_E_NOT_STOPPED);
	ENTRY(VFW_E_NOT_PAUSED);
	ENTRY(VFW_E_NOT_RUNNING);
	ENTRY(VFW_E_WRONG_STATE);
	ENTRY(VFW_E_START_TIME_AFTER_END);
	ENTRY(VFW_E_INVALID_RECT);
	ENTRY(VFW_E_TYPE_NOT_ACCEPTED);
	ENTRY(VFW_E_SAMPLE_REJECTED);
	ENTRY(VFW_E_SAMPLE_REJECTED_EOS);
	ENTRY(VFW_S_DUPLICATE_NAME);
	ENTRY(VFW_E_DUPLICATE_NAME);
	ENTRY(VFW_E_TIMEOUT);
	ENTRY(VFW_E_INVALID_FILE_FORMAT);
	ENTRY(VFW_E_ENUM_OUT_OF_RANGE);
	ENTRY(VFW_E_CIRCULAR_GRAPH);
	ENTRY(VFW_E_NOT_ALLOWED_TO_SAVE);
	ENTRY(VFW_E_TIME_ALREADY_PASSED);
	ENTRY(VFW_E_ALREADY_CANCELLED);
	ENTRY(VFW_E_CORRUPT_GRAPH_FILE);
	ENTRY(VFW_E_ADVISE_ALREADY_SET);
	ENTRY(VFW_S_STATE_INTERMEDIATE);
	ENTRY(VFW_E_NO_MODEX_AVAILABLE);
	ENTRY(VFW_E_NO_ADVISE_SET);
	ENTRY(VFW_E_NO_FULLSCREEN);
	ENTRY(VFW_E_IN_FULLSCREEN_MODE);
	ENTRY(VFW_E_UNKNOWN_FILE_TYPE);
	ENTRY(VFW_E_CANNOT_LOAD_SOURCE_FILTER);
	ENTRY(VFW_S_PARTIAL_RENDER);
	ENTRY(VFW_E_FILE_TOO_SHORT);
	ENTRY(VFW_E_INVALID_FILE_VERSION);
	ENTRY(VFW_S_SOME_DATA_IGNORED);
	ENTRY(VFW_S_CONNECTIONS_DEFERRED);
	ENTRY(VFW_E_INVALID_CLSID);
	ENTRY(VFW_E_INVALID_MEDIA_TYPE);
	ENTRY(VFW_E_SAMPLE_TIME_NOT_SET);
	ENTRY(VFW_S_RESOURCE_NOT_NEEDED);
	ENTRY(VFW_E_MEDIA_TIME_NOT_SET);
	ENTRY(VFW_E_NO_TIME_FORMAT_SET);
	ENTRY(VFW_E_MONO_AUDIO_HW);
	ENTRY(VFW_S_MEDIA_TYPE_IGNORED);
	ENTRY(VFW_E_NO_DECOMPRESSOR);
	ENTRY(VFW_E_NO_AUDIO_HARDWARE);
	ENTRY(VFW_S_VIDEO_NOT_RENDERED);
	ENTRY(VFW_S_AUDIO_NOT_RENDERED);
	ENTRY(VFW_E_RPZA);
	ENTRY(VFW_S_RPZA);
	ENTRY(VFW_E_PROCESSOR_NOT_SUITABLE);
	ENTRY(VFW_E_UNSUPPORTED_AUDIO);
	ENTRY(VFW_E_UNSUPPORTED_VIDEO);
	ENTRY(VFW_E_MPEG_NOT_CONSTRAINED);
	ENTRY(VFW_E_NOT_IN_GRAPH);
	ENTRY(VFW_S_ESTIMATED);
	ENTRY(VFW_E_NO_TIME_FORMAT);
	ENTRY(VFW_E_READ_ONLY);
	ENTRY(VFW_S_RESERVED);
	ENTRY(VFW_E_BUFFER_UNDERFLOW);
	ENTRY(VFW_E_UNSUPPORTED_STREAM);
	ENTRY(VFW_E_NO_TRANSPORT);
	ENTRY(VFW_S_STREAM_OFF);
	ENTRY(VFW_S_CANT_CUE);
	ENTRY(VFW_E_BAD_VIDEOCD);
	ENTRY(VFW_S_NO_STOP_TIME);
	ENTRY(VFW_E_OUT_OF_VIDEO_MEMORY);
	ENTRY(VFW_E_VP_NEGOTIATION_FAILED);
	ENTRY(VFW_E_DDRAW_CAPS_NOT_SUITABLE);
	ENTRY(VFW_E_NO_VP_HARDWARE);
	ENTRY(VFW_E_NO_CAPTURE_HARDWARE);
	ENTRY(VFW_E_DVD_OPERATION_INHIBITED);
	ENTRY(VFW_E_DVD_INVALIDDOMAIN);
	ENTRY(VFW_E_DVD_NO_BUTTON);
	ENTRY(VFW_E_DVD_GRAPHNOTREADY);
	ENTRY(VFW_E_DVD_RENDERFAIL);
	ENTRY(VFW_E_DVD_DECNOTENOUGH);
	ENTRY(VFW_E_DDRAW_VERSION_NOT_SUITABLE);
	ENTRY(VFW_E_COPYPROT_FAILED);
	ENTRY(VFW_S_NOPREVIEWPIN);
	ENTRY(VFW_E_TIME_EXPIRED);
	ENTRY(VFW_S_DVD_NON_ONE_SEQUENTIAL);
	ENTRY(VFW_E_DVD_WRONG_SPEED);
	ENTRY(VFW_E_DVD_MENU_DOES_NOT_EXIST);
	ENTRY(VFW_E_DVD_CMD_CANCELLED);
	ENTRY(VFW_E_DVD_STATE_WRONG_VERSION);
	ENTRY(VFW_E_DVD_STATE_CORRUPT);
	ENTRY(VFW_E_DVD_STATE_WRONG_DISC);
	ENTRY(VFW_E_DVD_INCOMPATIBLE_REGION);
	ENTRY(VFW_E_DVD_NO_ATTRIBUTES);
	ENTRY(VFW_E_DVD_NO_GOUP_PGC);
	ENTRY(VFW_E_DVD_LOW_PARENTAL_LEVEL);
	ENTRY(VFW_E_DVD_NOT_IN_KARAOKE_MODE);
	ENTRY(VFW_S_DVD_CHANNEL_CONTENTS_NOT_AVAILABLE);
	ENTRY(VFW_S_DVD_NOT_ACCURATE);
	ENTRY(VFW_E_FRAME_STEP_UNSUPPORTED);
	ENTRY(VFW_E_DVD_STREAM_DISABLED);
	ENTRY(VFW_E_DVD_TITLE_UNKNOWN);
	ENTRY(VFW_E_DVD_INVALID_DISC);
	ENTRY(VFW_E_DVD_NO_RESUME_INFORMATION);
	ENTRY(VFW_E_PIN_ALREADY_BLOCKED_ON_THIS_THREAD);
	ENTRY(VFW_E_PIN_ALREADY_BLOCKED);
	ENTRY(VFW_E_CERTIFICATION_FAILURE);
	#undef	ENTRY
	}

	return NULL;
}

/***********************************************************************
 *	AMGetErrorTextA	(quartz.@)
 */
DWORD WINAPI AMGetErrorTextA(HRESULT hr, LPSTR pszbuf, DWORD dwBufLen)
{
	LPCSTR	lpszRes;
	DWORD len;

	lpszRes = hresult_to_string( hr );
	if ( lpszRes == NULL )
		return 0;
	len = (DWORD)(strlen(lpszRes)+1);
	if ( len > dwBufLen )
		return 0;

	memcpy( pszbuf, lpszRes, len );
	return len;
}

/***********************************************************************
 *	AMGetErrorTextW	(quartz.@)
 */
DWORD WINAPI AMGetErrorTextW(HRESULT hr, LPWSTR pwszbuf, DWORD dwBufLen)
{
	CHAR	szBuf[MAX_ERROR_TEXT_LEN+1];
	DWORD	dwLen;

	dwLen = AMGetErrorTextA(hr,szBuf,MAX_ERROR_TEXT_LEN);
	if ( dwLen == 0 )
		return 0;
	szBuf[dwLen] = 0;

	QUARTZ_strncpyAtoW( pwszbuf, szBuf, dwBufLen );

	return lstrlenW( pwszbuf );
}