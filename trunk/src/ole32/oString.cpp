
#include <string.h>

// Prevent CALLCONV becoming _System
#include <winbase.h>
#include <uconv.h>
#undef CALLCONV

#include <os2win.h>

#include "oString.h"

// Block size.
long oStringA::blockSize = 32;

// calculate the buffersize needed for this string (quantised to blockSize)
long oStringA::calcBufLen(long strLen)
{
    long	tmp;

    tmp = (strLen / blockSize) + 1;

    return tmp * blockSize;
}

// Get a buffer of required length...
void oStringA::getBuf(long strLen)
{
    // Sanity check...
    if (strLen < 1)
    {
    	dprintf(("Warning: oStringA::getBuf - strlen < 1!"));
    	strLen = 1;
    }

    // Allocate new buffer...
    m_bufLen = calcBufLen(strLen);
    m_strBuf = (LPSTR)HeapAlloc(m_Heap, 0, m_bufLen);
    m_strLen = 1;
    m_strBuf[0] = 0;	// Ensure terminated.
}

// Free buffer...
void oStringA::freeBuf()
{
    HeapFree(m_Heap, 0, m_strBuf);
}

// Re-allocate buffer if more space is required...
void oStringA::adjBuf(long strLen)
{
    if (strLen > m_bufLen)
    {
	LPSTR	strBuf;
	long	bufLen = calcBufLen(strLen);

	// Allocate new buffer...
	strBuf = (LPSTR)HeapAlloc(m_Heap, 0, bufLen);

	// Copy old buffer to new...
	strcpy(strBuf, m_strBuf);	// Incl. terminator...
	freeBuf();

	// Save new buffer...
	m_strBuf = strBuf;
	m_bufLen = bufLen;
    }
}

// Initialise to nothing...
oStringA::oStringA( void)
{
    m_Heap = GetProcessHeap();
    getBuf(1);	// Incl. terminator...
}

// Initialise to a specified length...
oStringA::oStringA(int defLen)
{
    m_Heap = GetProcessHeap();
    getBuf(defLen + 1);
}

// Initialise from Unicode string...
oStringA::oStringA(LPCWSTR pUnicode)
{
    long	strLen = lstrlenW(pUnicode) + 1;

    m_Heap = GetProcessHeap();
    getBuf(strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf);
    m_strLen = strLen;
}

// Initialise from ASCII string...
oStringA::oStringA(LPCSTR pAscii)
{
    long	strLen = strlen(pAscii) + 1;

    m_Heap = GetProcessHeap();
    getBuf(strLen);
    strcpy(m_strBuf, pAscii);
    m_strLen = strLen;
}

// Initialise from another oString
oStringA::oStringA(const oStringA &ref)
{
    m_Heap = GetProcessHeap();
    getBuf(ref.m_strLen);
    strcpy(m_strBuf, ref.m_strBuf);
    m_strLen = ref.m_strLen;
}

//Destroy resources...
oStringA::~oStringA()
{
    freeBuf();
}

// Return pointer to string...
oStringA::operator LPSTR()
{
    return m_strBuf;
}


// Assign
oStringA oStringA::operator =  (const oStringA & string)
{
    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf(string.m_strLen);
    strcpy(m_strBuf, string.m_strBuf);
    m_strLen = string.m_strLen;
    return *this;
}

oStringA oStringA::operator =  (LPCSTR pAscii)
{
    long	strLen = strlen(pAscii) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf(strLen);
    strcpy(m_strBuf, pAscii);
    m_strLen = strLen;
    return *this;
}

oStringA oStringA::operator =  (LPCWSTR pUnicode)
{
    long	strLen = lstrlenW(pUnicode) + 1;

    // Lose old string...
    freeBuf();

    // Setup new string...
    getBuf(strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf);
    m_strLen = strLen;
    return *this;
}


// Add String to String
oStringA oStringA::operator +  (const oStringA & string)
{
    // two terminators to account for...
    oStringA	product(m_strLen + string.m_strLen - 1);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    strcpy(product.m_strBuf + m_strLen - 1, string.m_strBuf);
    product.m_strLen = m_strLen + string.m_strLen - 1;

    return product;
}

// Add String to ASCII string
oStringA oStringA::operator +  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    oStringA	product(strLen);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    strcpy(product.m_strBuf + m_strLen - 1, pAscii);
    product.m_strLen = strLen;

    return product;
}

// Add String to UNICODE string
oStringA oStringA::operator +  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    oStringA	product(strLen);

    memcpy(product.m_strBuf, m_strBuf, m_strLen);
    UnicodeToAscii((LPWSTR)pUnicode, product.m_strBuf + m_strLen - 1);
    product.m_strLen = strLen;

    return product;
}

// Concatenate string object
oStringA oStringA::operator += (const oStringA & string)
{
    // two terminators to account for...
    long	strLen = m_strLen + string.m_strLen - 1;

    adjBuf(strLen);
    strcpy(m_strBuf + m_strLen - 1, string.m_strBuf);
    m_strLen = strLen;

    return *this;
}

// Concatenate Ascii string
oStringA oStringA::operator +=  (LPCSTR pAscii)
{
    long	strLen = m_strLen + strlen(pAscii);

    adjBuf(strLen);
    strcpy(m_strBuf + m_strLen - 1, pAscii);
    m_strLen = strLen;

    return *this;
}

// Concatenate Unicode string
oStringA oStringA::operator +=  (LPCWSTR pUnicode)
{
    long	strLen = m_strLen + lstrlenW(pUnicode);

    adjBuf(strLen);
    UnicodeToAscii((LPWSTR)pUnicode, m_strBuf + m_strLen - 1);
    m_strLen = strLen;

    return *this;
}


