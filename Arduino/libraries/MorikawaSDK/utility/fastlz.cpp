/*
 FastLZ - lightning-fast lossless compression library
 
 Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)
 Copyright (C) 2006 Ariya Hidayat (ariya@kde.org)
 Copyright (C) 2005 Ariya Hidayat (ariya@kde.org)
 Portions Copyright (C) 2013 - 2013 HORIGUCHI Junshi (zap00365@nifty.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "fastlz.h"

#if !defined(FASTLZ__COMPRESSOR) && !defined(FASTLZ_DECOMPRESSOR)

/*
 * Always check for bound when decompressing.
 * Generally it is best to leave it defined.
 */
#define FASTLZ_SAFE

/*
 * Give hints to the compiler for branch prediction optimization.
 */
#if defined(__GNUC__) && (__GNUC__ > 2)
#define FASTLZ_EXPECT_CONDITIONAL(c)    (__builtin_expect((c), 1))
#define FASTLZ_UNEXPECT_CONDITIONAL(c)  (__builtin_expect((c), 0))
#else
#define FASTLZ_EXPECT_CONDITIONAL(c)    (c)
#define FASTLZ_UNEXPECT_CONDITIONAL(c)  (c)
#endif

/*
 * Use inlined functions for supported systems.
 */
#if defined(__GNUC__) || defined(__DMC__) || defined(__POCC__) || defined(__WATCOMC__) || defined(__SUNPRO_C)
#define FASTLZ_INLINE inline
#elif defined(__BORLANDC__) || defined(_MSC_VER) || defined(__LCC__)
#define FASTLZ_INLINE __inline
#else
#define FASTLZ_INLINE
#endif

/*
 * Prevent accessing more than 8-bit at once, except on x86 architectures.
 */
#if !defined(FASTLZ_STRICT_ALIGN)
#define FASTLZ_STRICT_ALIGN
#if defined(__i386__) || defined(__386)  /* GNU C, Sun Studio */
#undef FASTLZ_STRICT_ALIGN
#elif defined(__i486__) || defined(__i586__) || defined(__i686__) /* GNU C */
#undef FASTLZ_STRICT_ALIGN
#elif defined(_M_IX86) /* Intel, MSVC */
#undef FASTLZ_STRICT_ALIGN
#elif defined(__386)
#undef FASTLZ_STRICT_ALIGN
#elif defined(_X86_) /* MinGW */
#undef FASTLZ_STRICT_ALIGN
#elif defined(__I86__) /* Digital Mars */
#undef FASTLZ_STRICT_ALIGN
#endif
#endif



/*
 * FIXME: use preprocessor magic to set this on different platforms!
 */
typedef unsigned char  flzuint8;
typedef unsigned int   flzuint16;
typedef unsigned long  flzuint32;

/*
 * Portions Copyright (C) 2013 - 2013 HORIGUCHI Junshi (zap00365@nifty.com)
 */
#include "TSTMorikawa.h"

static tst::TSTError (tst::TSTMorikawa::*const g_write[tst::STORAGE_LIMIT])(unsigned long, void const*, unsigned int, unsigned int*) = {
    NULL, &tst::TSTMorikawa::writeSharedMemory, &tst::TSTMorikawa::writeFRAM, &tst::TSTMorikawa::writeFlashROM
};
static tst::TSTError (tst::TSTMorikawa::*const g_read[tst::STORAGE_LIMIT])(unsigned long, void*, unsigned int, unsigned int*) = {
    NULL, &tst::TSTMorikawa::readSharedMemory, &tst::TSTMorikawa::readFRAM, &tst::TSTMorikawa::readFlashROM
};

template <typename T>
class StorageIO {
    private:
        template <typename M>
        class Helper {
            private:
                        StorageIO*  _storage;
                        long        _offset;
            
            public:
                explicit            Helper          (StorageIO* storage, long offset);
                                    ~Helper         (void);
                        Helper&     operator=       (Helper const& param);
                        Helper&     operator=       (T param);
                                    operator T      (void) const;
            private:
                explicit            Helper          (void);
        };
    
    private:
                tst::TSTMorikawa*   _morikawa;
                tst::StorageType    _storage;
                unsigned long       _address;
        mutable tst::TSTError       _error;
    
    public:
        explicit                    StorageIO       (void);
        explicit                    StorageIO       (tst::TSTMorikawa* morikawa, tst::StorageType storage, unsigned long address);
                                    ~StorageIO      (void);
                StorageIO&          operator=       (unsigned long param);
                StorageIO&          operator+=      (long param);
                StorageIO&          operator-=      (long param);
                StorageIO&          operator++      (void);
                StorageIO           operator++      (int);
                StorageIO&          operator--      (void);
                StorageIO           operator--      (int);
                Helper<T>           operator*       (void);
                Helper<T>           operator[]      (long param);
                StorageIO           operator+       (long param) const;
                StorageIO           operator-       (long param) const;
                long                operator-       (StorageIO const& param) const;
                bool                operator==      (StorageIO const& param) const;
                bool                operator!=      (StorageIO const& param) const;
                bool                operator<       (StorageIO const& param) const;
                bool                operator<=      (StorageIO const& param) const;
                bool                operator>       (StorageIO const& param) const;
                bool                operator>=      (StorageIO const& param) const;
                unsigned long       address         (void) const;
                tst::TSTError       error           (void) const;
    private:
                void                set             (long offset, T param);
                T                   get             (long offset) const;
};

template <typename T>
inline StorageIO<T>::StorageIO(void) : _morikawa(NULL), _error(tst::TSTERROR_OK)
{
}

template <typename T>
inline StorageIO<T>::StorageIO(tst::TSTMorikawa* morikawa, tst::StorageType storage, unsigned long address) : _morikawa(morikawa), _error(tst::TSTERROR_OK)
{
    _storage = storage;
    _address = address;
}

template <typename T>
inline StorageIO<T>::~StorageIO(void)
{
}

template <typename T>
inline StorageIO<T>& StorageIO<T>::operator=(unsigned long param)
{
    _address = param;
    return *this;
}

template <typename T>
inline StorageIO<T>& StorageIO<T>::operator+=(long param)
{
    _address += param * sizeof(T);
    return *this;
}

template <typename T>
inline StorageIO<T>& StorageIO<T>::operator-=(long param)
{
    _address -= param * sizeof(T);
    return *this;
}

template <typename T>
inline StorageIO<T>& StorageIO<T>::operator++(void)
{
    _address += sizeof(T);
    return *this;
}

template <typename T>
inline StorageIO<T> StorageIO<T>::operator++(int)
{
    StorageIO result(*this);
    
    _address += sizeof(T);
    return result;
}

template <typename T>
inline StorageIO<T>& StorageIO<T>::operator--(void)
{
    _address -= sizeof(T);
    return *this;
}

template <typename T>
inline StorageIO<T> StorageIO<T>::operator--(int)
{
    StorageIO result(*this);
    
    _address -= sizeof(T);
    return result;
}

template <typename T>
inline StorageIO<T>::Helper<T> StorageIO<T>::operator*(void)
{
    return Helper<T>(this, 0);
}

template <typename T>
inline StorageIO<T>::Helper<T> StorageIO<T>::operator[](long param)
{
    return Helper<T>(this, param);
}

template <typename T>
inline StorageIO<T> StorageIO<T>::operator+(long param) const
{
    StorageIO result(*this);
    
    result._address += param * sizeof(T);
    return result;
}

template <typename T>
inline StorageIO<T> StorageIO<T>::operator-(long param) const
{
    StorageIO result(*this);
    
    result._address -= param * sizeof(T);
    return result;
}

template <typename T>
inline long StorageIO<T>::operator-(StorageIO const& param) const
{
    return (_address - param._address) / sizeof(T);
}

template <typename T>
inline bool StorageIO<T>::operator==(StorageIO const& param) const
{
    return (_address == param._address);
}

template <typename T>
inline bool StorageIO<T>::operator!=(StorageIO const& param) const
{
    return (_address != param._address);
}

template <typename T>
inline bool StorageIO<T>::operator<(StorageIO const& param) const
{
    return (_address < param._address);
}

template <typename T>
inline bool StorageIO<T>::operator<=(StorageIO const& param) const
{
    return (_address <= param._address);
}

template <typename T>
inline bool StorageIO<T>::operator>(StorageIO const& param) const
{
    return (_address > param._address);
}

template <typename T>
inline bool StorageIO<T>::operator>=(StorageIO const& param) const
{
    return (_address >= param._address);
}

template <typename T>
inline unsigned long StorageIO<T>::address(void) const
{
    return _address;
}

template <typename T>
inline tst::TSTError StorageIO<T>::error(void) const
{
    return _error;
}

template <typename T>
void StorageIO<T>::set(long offset, T param)
{
    tst::TSTError error;
    
    if ((error = (_morikawa->*g_write[_storage])(_address + offset * sizeof(T), &param, sizeof(param), NULL)) != tst::TSTERROR_OK) {
        _error = error;
    }
    return;
}

template <typename T>
T StorageIO<T>::get(long offset) const
{
    tst::TSTError error;
    T result(0);
    
    if ((error = (_morikawa->*g_read[_storage])(_address + offset * sizeof(T), &result, sizeof(result), NULL)) != tst::TSTERROR_OK) {
        _error = error;
    }
    return result;
}

template <typename T>
template <typename M>
inline StorageIO<T>::Helper<M>::Helper(StorageIO* storage, long offset) : _storage(storage), _offset(offset)
{
}

template <typename T>
template <typename M>
inline StorageIO<T>::Helper<M>::~Helper(void)
{
}

template <typename T>
template <typename M>
inline StorageIO<T>::Helper<M>& StorageIO<T>::Helper<M>::operator=(Helper const& param)
{
    _storage->set(_offset, param._storage->get(param._offset));
    return *this;
}

template <typename T>
template <typename M>
inline StorageIO<T>::Helper<M>& StorageIO<T>::Helper<M>::operator=(T param)
{
    _storage->set(_offset, param);
    return *this;
}

template <typename T>
template <typename M>
inline StorageIO<T>::Helper<M>::operator T(void) const
{
    return _storage->get(_offset);
}

/*
 * End of appended by HORIGUCHI Junshi (zap00365@nifty.com)
 */



#define MAX_COPY       32
#define MAX_LEN       264  /* 256 + 8 */
#define MAX_DISTANCE 8192

#if !defined(FASTLZ_STRICT_ALIGN)
#define FASTLZ_READU16(p) *((const flzuint16*)(p))
#else
#define FASTLZ_READU16(p) ((p)[0] | (p)[1]<<8)
#endif

#define HASH_LOG  13
#define HASH_SIZE (1<< HASH_LOG)
#define HASH_MASK  (HASH_SIZE-1)
#define HASH_FUNCTION(v,p) { v = FASTLZ_READU16(p); v ^= FASTLZ_READU16(p+1)^(v>>(16-HASH_LOG));v &= HASH_MASK; }

#undef FASTLZ_LEVEL
#define FASTLZ_LEVEL 1

#undef FASTLZ_COMPRESSOR
#undef FASTLZ_DECOMPRESSOR
#define FASTLZ_COMPRESSOR fastlz1_compress
#define FASTLZ_DECOMPRESSOR fastlz1_decompress
static FASTLZ_INLINE long FASTLZ_COMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, StorageIO<unsigned long>& htab, tst::TSTError* error);
static FASTLZ_INLINE long FASTLZ_DECOMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, long maxout, tst::TSTError* error);
#include "fastlz.cpp"

#undef FASTLZ_LEVEL
#define FASTLZ_LEVEL 2

#undef MAX_DISTANCE
#define MAX_DISTANCE 8191
#define MAX_FARDISTANCE (65535+MAX_DISTANCE-1)

#undef FASTLZ_COMPRESSOR
#undef FASTLZ_DECOMPRESSOR
#define FASTLZ_COMPRESSOR fastlz2_compress
#define FASTLZ_DECOMPRESSOR fastlz2_decompress
static FASTLZ_INLINE long FASTLZ_COMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, StorageIO<unsigned long>& htab, tst::TSTError* error);
static FASTLZ_INLINE long FASTLZ_DECOMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, long maxout, tst::TSTError* error);
#include "fastlz.cpp"

long fastlz_compress(tst::TSTMorikawa* morikawa, tst::StorageType istorage, unsigned long input, long length, tst::StorageType ostorage, unsigned long output, tst::StorageType wstorage, unsigned long work, tst::TSTError* error)
{
    StorageIO<unsigned char> isio(morikawa, istorage, input);
    StorageIO<unsigned char> osio(morikawa, ostorage, output);
    StorageIO<unsigned long> wsio(morikawa, wstorage, work);
    
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    /* for short block, choose fastlz1 */
    if(length < 65536)
        return fastlz1_compress(morikawa, isio, length, osio, wsio, error);
    
    /* else... */
    return fastlz2_compress(morikawa, isio, length, osio, wsio, error);
}

long fastlz_decompress(tst::TSTMorikawa* morikawa, tst::StorageType istorage, unsigned long input, long length, tst::StorageType ostorage, unsigned long output, long maxout, tst::TSTError* error)
{
    StorageIO<unsigned char> isio(morikawa, istorage, input);
    StorageIO<unsigned char> osio(morikawa, ostorage, output);
    
    /* magic identifier for compression level */
    int level = (*isio >> 5) + 1;
    
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    if(level == 1)
        return fastlz1_decompress(morikawa, isio, length, osio, maxout, error);
    if(level == 2)
        return fastlz2_decompress(morikawa, isio, length, osio, maxout, error);
    
    /* unknown level, trigger error */
    return 0;
}

long fastlz_compress_level(tst::TSTMorikawa* morikawa, int level, tst::StorageType istorage, unsigned long input, long length, tst::StorageType ostorage, unsigned long output, tst::StorageType wstorage, unsigned long work, tst::TSTError* error)
{
    StorageIO<unsigned char> isio(morikawa, istorage, input);
    StorageIO<unsigned char> osio(morikawa, ostorage, output);
    StorageIO<unsigned long> wsio(morikawa, wstorage, work);
    
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    if(level == 1)
        return fastlz1_compress(morikawa, isio, length, osio, wsio, error);
    if(level == 2)
        return fastlz2_compress(morikawa, isio, length, osio, wsio, error);
    
    return 0;
}

#else /* !defined(FASTLZ_COMPRESSOR) && !defined(FASTLZ_DECOMPRESSOR) */

static FASTLZ_INLINE long FASTLZ_COMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, StorageIO<unsigned long>& htab, tst::TSTError* error)
{
    StorageIO<unsigned char> ip = input;
    StorageIO<unsigned char> ip_bound = ip + length - 2;
    StorageIO<unsigned char> ip_limit = ip + length - 12;
    StorageIO<unsigned char> op = output;
    
    StorageIO<unsigned long> hslot;
    flzuint32 hval;
    
    flzuint32 copy;
    
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    if (morikawa->hasAbnormalShutdown()) {
        *error = tst::TSTERROR_INVALID_STATE;
        return 0;
    }
    
    /* sanity check */
    if(FASTLZ_UNEXPECT_CONDITIONAL(length < 4))
    {
        if(length)
        {
            /* create literal copy only */
            *op++ = length-1;
            ip_bound++;
            while(ip <= ip_bound)
                *op++ = *ip++;
            
            if ((*error = ip.error()) == tst::TSTERROR_OK) {
                *error = op.error();
            }
            if (*error != tst::TSTERROR_OK) {
                return 0;
            }
            return length+1;
        }
        else
            return 0;
    }
    
    /* initializes hash table */
    for (hslot = htab; hslot < htab + HASH_SIZE; hslot++)
        *hslot = ip.address();
    
    /* we start with literal copy */
    copy = 2;
    *op++ = MAX_COPY-1;
    *op++ = *ip++;
    *op++ = *ip++;
    
    /* main loop */
    while(FASTLZ_EXPECT_CONDITIONAL(ip < ip_limit))
    {
        if (morikawa->hasAbnormalShutdown()) {
            *error = tst::TSTERROR_ABNORMAL_SHUTDOWN;
            return 0;
        }
        
        StorageIO<unsigned char> ref = ip;
        flzuint32 distance;
        
        /* minimum match length */
        flzuint32 len = 3;
        
        /* comparison starting-point */
        StorageIO<unsigned char> anchor = ip;
        
        /* check for a run */
#if FASTLZ_LEVEL==2
        if(ip[0] == ip[-1] && FASTLZ_READU16(ip-1)==FASTLZ_READU16(ip+1))
        {
            distance = 1;
            ip += 3;
            ref = anchor - 1 + 3;
            goto match;
        }
#endif
        
        /* find potential match */
        HASH_FUNCTION(hval,ip);
        hslot = htab + hval;
        ref = htab[hval];
        
        /* calculate distance to the match */
        distance = anchor - ref;
        
        /* update hash table */
        *hslot = anchor.address();
        
        /* is this a match? check the first 3 bytes */
        if(distance==0 ||
#if FASTLZ_LEVEL==1
           (distance >= MAX_DISTANCE) ||
#else
           (distance >= MAX_FARDISTANCE) ||
#endif
           *ref++ != *ip++ || *ref++!=*ip++ || *ref++!=*ip++)
            goto literal;
        
#if FASTLZ_LEVEL==2
        /* far, needs at least 5-byte match */
        if(distance >= MAX_DISTANCE)
        {
            if(*ip++ != *ref++ || *ip++!= *ref++)
                goto literal;
            len += 2;
        }
        
    match:
#endif
        
        /* last matched byte */
        ip = anchor + len;
        
        /* distance is biased */
        distance--;
        
        if(!distance)
        {
            /* zero distance means a run */
            flzuint8 x = ip[-1];
            while(ip < ip_bound)
                if(*ref++ != x) break; else ip++;
        }
        else
            for(;;)
            {
                /* safe because the outer check against ip limit */
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                if(*ref++ != *ip++) break;
                while(ip < ip_bound)
                    if(*ref++ != *ip++) break;
                break;
            }
        
        /* if we have copied something, adjust the copy count */
        if(copy)
        /* copy is biased, '0' means 1 byte copy */
            *(op-copy-1) = copy-1;
        else
        /* back, to overwrite the copy count */
            op--;
        
        /* reset literal counter */
        copy = 0;
        
        /* length is biased, '1' means a match of 3 bytes */
        ip -= 3;
        len = ip - anchor;
        
        /* encode the match */
#if FASTLZ_LEVEL==2
        if(distance < MAX_DISTANCE)
        {
            if(len < 7)
            {
                *op++ = (len << 5) + (distance >> 8);
                *op++ = (distance & 255);
            }
            else
            {
                *op++ = (7 << 5) + (distance >> 8);
                for(len-=7; len >= 255; len-= 255)
                    *op++ = 255;
                *op++ = len;
                *op++ = (distance & 255);
            }
        }
        else
        {
            /* far away, but not yet in the another galaxy... */
            if(len < 7)
            {
                distance -= MAX_DISTANCE;
                *op++ = (len << 5) + 31;
                *op++ = 255;
                *op++ = distance >> 8;
                *op++ = distance & 255;
            }
            else
            {
                distance -= MAX_DISTANCE;
                *op++ = (7 << 5) + 31;
                for(len-=7; len >= 255; len-= 255)
                    *op++ = 255;
                *op++ = len;
                *op++ = 255;
                *op++ = distance >> 8;
                *op++ = distance & 255;
            }
        }
#else
        
        if(FASTLZ_UNEXPECT_CONDITIONAL(len > MAX_LEN-2))
            while(len > MAX_LEN-2)
            {
                *op++ = (7 << 5) + (distance >> 8);
                *op++ = MAX_LEN - 2 - 7 -2;
                *op++ = (distance & 255);
                len -= MAX_LEN-2;
            }
        
        if(len < 7)
        {
            *op++ = (len << 5) + (distance >> 8);
            *op++ = (distance & 255);
        }
        else
        {
            *op++ = (7 << 5) + (distance >> 8);
            *op++ = len - 7;
            *op++ = (distance & 255);
        }
#endif
        
        /* update the hash at match boundary */
        HASH_FUNCTION(hval,ip);
        htab[hval] = (ip++).address();
        HASH_FUNCTION(hval,ip);
        htab[hval] = (ip++).address();
        
        /* assuming literal copy */
        *op++ = MAX_COPY-1;
        
        goto endofwhile;
        
    literal:
        *op++ = *anchor++;
        ip = anchor;
        copy++;
        if(FASTLZ_UNEXPECT_CONDITIONAL(copy == MAX_COPY))
        {
            copy = 0;
            *op++ = MAX_COPY-1;
        }
        
    endofwhile:
        if ((*error = ip.error()) == tst::TSTERROR_OK) {
            if ((*error = op.error()) == tst::TSTERROR_OK) {
                if ((*error = htab.error()) == tst::TSTERROR_OK) {
                    if ((*error = hslot.error()) == tst::TSTERROR_OK) {
                        if ((*error = ref.error()) == tst::TSTERROR_OK) {
                            *error = anchor.error();
                        }
                    }
                }
            }
        }
        if (*error != tst::TSTERROR_OK) {
            return 0;
        }
    }
    
    /* left-over as literal copy */
    ip_bound++;
    while(ip <= ip_bound)
    {
        *op++ = *ip++;
        copy++;
        if(copy == MAX_COPY)
        {
            copy = 0;
            *op++ = MAX_COPY-1;
        }
    }
    
    /* if we have copied something, adjust the copy length */
    if(copy)
        *(op-copy-1) = copy-1;
    else
        op--;
    
#if FASTLZ_LEVEL==2
    /* marker for fastlz2 */
    *output = *output | (1 << 5);
#endif
    
    if ((*error = ip.error()) == tst::TSTERROR_OK) {
        if ((*error = op.error()) == tst::TSTERROR_OK) {
            if ((*error = htab.error()) == tst::TSTERROR_OK) {
                if ((*error = hslot.error()) == tst::TSTERROR_OK) {
                    *error = output.error();
                }
            }
        }
    }
    if (*error != tst::TSTERROR_OK) {
        return 0;
    }
    return op - output;
}

static FASTLZ_INLINE long FASTLZ_DECOMPRESSOR(tst::TSTMorikawa* morikawa, StorageIO<unsigned char>& input, long length, StorageIO<unsigned char>& output, long maxout, tst::TSTError* error)
{
    StorageIO<unsigned char> ip = input;
    StorageIO<unsigned char> ip_limit  = ip + length;
    StorageIO<unsigned char> op = output;
    StorageIO<unsigned char> op_limit = op + maxout;
    flzuint32 ctrl = (*ip++) & 31;
    int loop = 1;
    
#ifdef OPTION_BUILD_MEMORYLOG
    tst::TSTMorikawa::saveMemoryLog();
#endif
    if (morikawa->hasAbnormalShutdown()) {
        *error = tst::TSTERROR_INVALID_STATE;
        return 0;
    }
    
    do
    {
        if (morikawa->hasAbnormalShutdown()) {
            *error = tst::TSTERROR_ABNORMAL_SHUTDOWN;
            return 0;
        }
        
        StorageIO<unsigned char> ref = op;
        flzuint32 len = ctrl >> 5;
        flzuint32 ofs = (ctrl & 31) << 8;
        
        if(ctrl >= 32)
        {
#if FASTLZ_LEVEL==2
            flzuint8 code;
#endif
            len--;
            ref -= ofs;
            if (len == 7-1)
#if FASTLZ_LEVEL==1
                len += *ip++;
            ref -= *ip++;
#else
            do
            {
                code = *ip++;
                len += code;
            } while (code==255);
            code = *ip++;
            ref -= code;
            
            /* match from 16-bit distance */
            if(FASTLZ_UNEXPECT_CONDITIONAL(code==255))
                if(FASTLZ_EXPECT_CONDITIONAL(ofs==(31 << 8)))
                {
                    ofs = (*ip++) << 8;
                    ofs += *ip++;
                    ref = op - ofs - MAX_DISTANCE;
                }
#endif
            
#ifdef FASTLZ_SAFE
            if (FASTLZ_UNEXPECT_CONDITIONAL(op + len + 3 > op_limit))
                return 0;
            
            if (FASTLZ_UNEXPECT_CONDITIONAL(ref-1 < output))
                return 0;
#endif
            
            if(FASTLZ_EXPECT_CONDITIONAL(ip < ip_limit))
                ctrl = *ip++;
            else
                loop = 0;
            
            if(ref == op)
            {
                /* optimize copy for a run */
                flzuint8 b = ref[-1];
                *op++ = b;
                *op++ = b;
                *op++ = b;
                for(; len; --len)
                    *op++ = b;
            }
            else
            {
#if !defined(FASTLZ_STRICT_ALIGN)
                const flzuint16* p;
                flzuint16* q;
#endif
                /* copy from reference */
                ref--;
                *op++ = *ref++;
                *op++ = *ref++;
                *op++ = *ref++;
                
#if !defined(FASTLZ_STRICT_ALIGN)
                /* copy a byte, so that now it's word aligned */
                if(len & 1)
                {
                    *op++ = *ref++;
                    len--;
                }
                
                /* copy 16-bit at once */
                q = (flzuint16*) op;
                op += len;
                p = (const flzuint16*) ref;
                for(len>>=1; len > 4; len-=4)
                {
                    *q++ = *p++;
                    *q++ = *p++;
                    *q++ = *p++;
                    *q++ = *p++;
                }
                for(; len; --len)
                    *q++ = *p++;
#else
                for(; len; --len)
                    *op++ = *ref++;
#endif
            }
        }
        else
        {
            ctrl++;
#ifdef FASTLZ_SAFE
            if (FASTLZ_UNEXPECT_CONDITIONAL(op + ctrl > op_limit))
                return 0;
            if (FASTLZ_UNEXPECT_CONDITIONAL(ip + ctrl > ip_limit))
                return 0;
#endif
            
            *op++ = *ip++; 
            for(--ctrl; ctrl; ctrl--)
                *op++ = *ip++;
            
            loop = FASTLZ_EXPECT_CONDITIONAL(ip < ip_limit);
            if(loop)
                ctrl = *ip++;
        }
        if ((*error = ip.error()) == tst::TSTERROR_OK) {
            if ((*error = op.error()) == tst::TSTERROR_OK) {
                *error = ref.error();
            }
        }
        if (*error != tst::TSTERROR_OK) {
            return 0;
        }
    }
    while(FASTLZ_EXPECT_CONDITIONAL(loop));
    
    if ((*error = ip.error()) == tst::TSTERROR_OK) {
        *error = op.error();
    }
    if (*error != tst::TSTERROR_OK) {
        return 0;
    }
    return op - output;
}

#endif /* !defined(FASTLZ_COMPRESSOR) && !defined(FASTLZ_DECOMPRESSOR) */
