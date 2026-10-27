/*
 * USER32: PNG-compressed icon images (Vista style .ico / RT_ICON entries).
 * Decodes the PNG (own inflate, after Mark Adler's puff) and returns an ordinary
 * DIB icon image (BITMAPINFOHEADER + 32bpp XOR + 1bpp AND mask) that the existing
 * CURSORICON_CreateFromResource path understands.
 */
#include <os2win.h>
#include <string.h>
#include <stdlib.h>

//---- inflate (puff) ------------------------------------------------------------------
#define MAXBITS 15
#define MAXLCODES 286
#define MAXDCODES 30
#define MAXCODES (MAXLCODES + MAXDCODES)
#define FIXLCODES 288

struct InfState {
    const BYTE *in; unsigned inlen, incnt;
    unsigned bitbuf, bitcnt;
    BYTE *out; unsigned outcap, outlen;
    int bad;
};
struct Huff { short count[MAXBITS + 1]; short symbol[MAXCODES]; };

static int Bits(InfState *s, int need)
{
    long val = s->bitbuf;
    while((int)s->bitcnt < need) {
        if(s->incnt >= s->inlen) { s->bad = 1; return 0; }
        val |= (long)(s->in[s->incnt++]) << s->bitcnt;
        s->bitcnt += 8;
    }
    s->bitbuf = (unsigned)(val >> need);
    s->bitcnt -= need;
    return (int)(val & ((1L << need) - 1));
}

static int Decode(InfState *s, const Huff *h)
{
    int len, code = 0, first = 0, count, index = 0;
    for(len = 1; len <= MAXBITS; len++) {
        code |= Bits(s, 1);
        if(s->bad) return -1;
        count = h->count[len];
        if(code - count < first) return h->symbol[index + (code - first)];
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return -1;
}

static int Construct(Huff *h, const short *length, int n)
{
    int symbol, len, left;
    short offs[MAXBITS + 1];
    for(len = 0; len <= MAXBITS; len++) h->count[len] = 0;
    for(symbol = 0; symbol < n; symbol++) h->count[length[symbol]]++;
    if(h->count[0] == n) return 0;
    left = 1;
    for(len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= h->count[len];
        if(left < 0) return left;
    }
    offs[1] = 0;
    for(len = 1; len < MAXBITS; len++) offs[len + 1] = offs[len] + h->count[len];
    for(symbol = 0; symbol < n; symbol++)
        if(length[symbol] != 0) h->symbol[offs[length[symbol]]++] = symbol;
    return left;
}

static int PutByte(InfState *s, BYTE b)
{
    if(s->outlen >= s->outcap) {
        unsigned ncap = s->outcap ? s->outcap * 2 : 65536;
        BYTE *n = (BYTE *)realloc(s->out, ncap);
        if(!n) return 0;
        s->out = n;
        s->outcap = ncap;
    }
    s->out[s->outlen++] = b;
    return 1;
}

static int Codes(InfState *s, const Huff *lencode, const Huff *distcode)
{
    static const short lens[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
    static const short lext[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
    static const short dists[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
    static const short dext[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
    int symbol, len;
    unsigned dist;
    do {
        symbol = Decode(s, lencode);
        if(symbol < 0) return 1;
        if(symbol < 256) {
            if(!PutByte(s, (BYTE)symbol)) return 1;
        }
        else if(symbol > 256) {
            symbol -= 257;
            if(symbol >= 29) return 1;
            len = lens[symbol] + Bits(s, lext[symbol]);
            symbol = Decode(s, distcode);
            if(symbol < 0 || symbol >= 30) return 1;
            dist = dists[symbol] + Bits(s, dext[symbol]);
            if(s->bad || dist > s->outlen) return 1;
            while(len--) if(!PutByte(s, s->out[s->outlen - dist])) return 1;
        }
    } while(symbol != 256);
    return 0;
}

static int Inflate(InfState *s)
{
    int last, type;
    static Huff fixl, fixd;
    static int fixInit = 0;
    do {
        last = Bits(s, 1);
        type = Bits(s, 2);
        if(s->bad) return 1;
        if(type == 0) {
            unsigned len;
            s->bitbuf = 0; s->bitcnt = 0;
            if(s->incnt + 4 > s->inlen) return 1;
            len = s->in[s->incnt] | (s->in[s->incnt + 1] << 8);
            if(len != (unsigned)(~(s->in[s->incnt + 2] | (s->in[s->incnt + 3] << 8)) & 0xFFFF)) return 1;
            s->incnt += 4;
            if(s->incnt + len > s->inlen) return 1;
            while(len--) if(!PutByte(s, s->in[s->incnt++])) return 1;
        }
        else if(type == 1) {
            if(!fixInit) {
                short lengths[FIXLCODES];
                int sym;
                for(sym = 0; sym < 144; sym++) lengths[sym] = 8;
                for(; sym < 256; sym++) lengths[sym] = 9;
                for(; sym < 280; sym++) lengths[sym] = 7;
                for(; sym < FIXLCODES; sym++) lengths[sym] = 8;
                Construct(&fixl, lengths, FIXLCODES);
                for(sym = 0; sym < MAXDCODES; sym++) lengths[sym] = 5;
                Construct(&fixd, lengths, MAXDCODES);
                fixInit = 1;
            }
            if(Codes(s, &fixl, &fixd)) return 1;
        }
        else if(type == 2) {
            static const short order[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
            short lengths[MAXCODES];
            Huff lencode, distcode;
            int nlen = Bits(s, 5) + 257, ndist = Bits(s, 5) + 1, ncode = Bits(s, 4) + 4, index;
            if(s->bad || nlen > MAXLCODES || ndist > MAXDCODES) return 1;
            for(index = 0; index < ncode; index++) lengths[order[index]] = Bits(s, 3);
            for(; index < 19; index++) lengths[order[index]] = 0;
            if(Construct(&lencode, lengths, 19) != 0) return 1;
            index = 0;
            while(index < nlen + ndist) {
                int symbol = Decode(s, &lencode), len, rep;
                if(symbol < 0) return 1;
                if(symbol < 16) lengths[index++] = symbol;
                else {
                    len = 0;
                    if(symbol == 16) {
                        if(index == 0) return 1;
                        len = lengths[index - 1];
                        rep = 3 + Bits(s, 2);
                    }
                    else if(symbol == 17) rep = 3 + Bits(s, 3);
                    else rep = 11 + Bits(s, 7);
                    if(index + rep > nlen + ndist) return 1;
                    while(rep--) lengths[index++] = len;
                }
            }
            if(lengths[256] == 0) return 1;
            if(Construct(&lencode, lengths, nlen) < 0) return 1;
            if(Construct(&distcode, lengths + nlen, ndist) < 0) return 1;
            if(Codes(s, &lencode, &distcode)) return 1;
        }
        else return 1;
    } while(!last);
    return 0;
}

//---- PNG -> DIB icon image -----------------------------------------------------------
static DWORD BE32(const BYTE *p) { return ((DWORD)p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]; }

static int Paeth(int a, int b, int c)
{
    int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    if(pa <= pb && pa <= pc) return a;
    if(pb <= pc) return b;
    return c;
}

/* returns malloc'ed icon DIB image (caller frees) or NULL */
BYTE *PngToIconDIB(const BYTE *png, UINT cb, UINT *pcbOut)
{
    static const BYTE sig[8] = {0x89,'P','N','G',0x0D,0x0A,0x1A,0x0A};
    UINT pos = 8, w = 0, h = 0, depth = 0, ctype = 0, interlace = 0, nIdat = 0, i;
    BYTE *idat = NULL, *raw = NULL, plte[256][3], trns[256];
    UINT nPlte = 0, nTrns = 0, bpp, stride, rawLen;
    InfState st;
    BYTE *result = NULL, *prev, *cur, *xor_, *andm;
    UINT andStride, xorStride, y, x;
    BITMAPINFOHEADER *bih;

    if(cb < 33 || memcmp(png, sig, 8)) return NULL;
    memset(trns, 255, sizeof(trns));
    memset(&st, 0, sizeof(st));

    while(pos + 12 <= cb) {
        DWORD len = BE32(png + pos);
        const BYTE *type = png + pos + 4, *d = png + pos + 8;
        if(pos + 12 + len > cb) break;
        if(!memcmp(type, "IHDR", 4) && len >= 13) {
            w = BE32(d); h = BE32(d + 4); depth = d[8]; ctype = d[9]; interlace = d[12];
        }
        else if(!memcmp(type, "PLTE", 4)) {
            nPlte = len / 3; if(nPlte > 256) nPlte = 256;
            memcpy(plte, d, nPlte * 3);
        }
        else if(!memcmp(type, "tRNS", 4)) {
            nTrns = len; if(nTrns > 256) nTrns = 256;
            if(ctype == 3) memcpy(trns, d, nTrns);
        }
        else if(!memcmp(type, "IDAT", 4)) {
            BYTE *n = (BYTE *)realloc(idat, nIdat + len);
            if(!n) goto fail;
            idat = n;
            memcpy(idat + nIdat, d, len);
            nIdat += len;
        }
        else if(!memcmp(type, "IEND", 4)) break;
        pos += 12 + len;
    }
    if(!w || !h || w > 512 || h > 512 || interlace || !idat || nIdat < 6) goto fail;
    if(ctype == 3 ? !(depth == 1 || depth == 2 || depth == 4 || depth == 8)
                  : !(depth == 8 || depth == 16)) goto fail;
    switch(ctype) {
    case 0: bpp = 1; break;
    case 2: bpp = 3; break;
    case 3: bpp = 1; break;
    case 4: bpp = 2; break;
    case 6: bpp = 4; break;
    default: goto fail;
    }
    {
        UINT bitsPerPixel = (ctype == 3 ? depth : bpp * depth);
        stride = (w * bitsPerPixel + 7) / 8;
        if(depth == 16) bpp *= 2;
        else bpp = (bitsPerPixel + 7) / 8;
    }

    st.in = idat + 2; st.inlen = nIdat - 2;          // skip zlib header
    if(Inflate(&st)) goto fail;
    rawLen = (stride + 1) * h;
    if(st.outlen < rawLen) goto fail;
    raw = st.out;

    // unfilter in place
    prev = NULL;
    for(y = 0; y < h; y++) {
        BYTE ft;
        cur = raw + y * (stride + 1);
        ft = cur[0];
        cur++;
        for(i = 0; i < stride; i++) {
            int a = i >= bpp ? cur[i - bpp] : 0;
            int b = prev ? prev[i] : 0;
            int c = (prev && i >= bpp) ? prev[i - bpp] : 0;
            switch(ft) {
            case 1: cur[i] += a; break;
            case 2: cur[i] += b; break;
            case 3: cur[i] += (a + b) / 2; break;
            case 4: cur[i] += Paeth(a, b, c); break;
            default: break;
            }
        }
        prev = cur;
    }

    xorStride = w * 4;
    andStride = ((w + 31) / 32) * 4;
    *pcbOut = sizeof(BITMAPINFOHEADER) + xorStride * h + andStride * h;
    result = (BYTE *)calloc(1, *pcbOut);
    if(!result) goto fail;
    bih = (BITMAPINFOHEADER *)result;
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biWidth = w;
    bih->biHeight = h * 2;
    bih->biPlanes = 1;
    bih->biBitCount = 32;
    bih->biCompression = BI_RGB;
    bih->biSizeImage = xorStride * h;
    xor_ = result + sizeof(BITMAPINFOHEADER);
    andm = xor_ + xorStride * h;

    for(y = 0; y < h; y++) {
        const BYTE *src = raw + y * (stride + 1) + 1;
        BYTE *dst = xor_ + (h - 1 - y) * xorStride;     // bottom-up
        BYTE *am  = andm + (h - 1 - y) * andStride;
        for(x = 0; x < w; x++) {
            BYTE r, g, b, a = 255;
            switch(ctype) {
            case 0: r = g = b = (depth == 16) ? src[x * 2] : src[x]; break;
            case 2:
                if(depth == 8) { r = src[x * 3]; g = src[x * 3 + 1]; b = src[x * 3 + 2]; }
                else { r = src[x * 6]; g = src[x * 6 + 2]; b = src[x * 6 + 4]; }
                break;
            case 3: {
                UINT idx;
                if(depth == 8) idx = src[x];
                else {
                    UINT perByte = 8 / depth, sh = 8 - depth * (1 + (x % perByte));
                    idx = (src[x / perByte] >> sh) & ((1 << depth) - 1);
                }
                r = plte[idx][0]; g = plte[idx][1]; b = plte[idx][2];
                a = trns[idx];
                break;
            }
            case 4:
                if(depth == 8) { r = g = b = src[x * 2]; a = src[x * 2 + 1]; }
                else { r = g = b = src[x * 4]; a = src[x * 4 + 2]; }
                break;
            default:
                if(depth == 8) { r = src[x * 4]; g = src[x * 4 + 1]; b = src[x * 4 + 2]; a = src[x * 4 + 3]; }
                else { r = src[x * 8]; g = src[x * 8 + 2]; b = src[x * 8 + 4]; a = src[x * 8 + 6]; }
                break;
            }
            if(a < 128) { r = g = b = 0; am[x >> 3] |= 0x80 >> (x & 7); }
            dst[x * 4]     = b;
            dst[x * 4 + 1] = g;
            dst[x * 4 + 2] = r;
            dst[x * 4 + 3] = a;
        }
    }
    free(idat);
    free(st.out);
    return result;

fail:
    free(idat);
    free(st.out);
    free(result);
    return NULL;
}
