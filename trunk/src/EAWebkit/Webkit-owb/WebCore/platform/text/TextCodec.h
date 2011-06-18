/*
 * Copyright (C) 2004, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

/*
* This file was modified by Electronic Arts Inc Copyright � 2009-2010
*/

#ifndef TextCodec_h
#define TextCodec_h

#include <wtf/FastAllocBase.h>
#include <memory>
#include <wtf/Noncopyable.h>
#include <wtf/Vector.h>
#include <wtf/unicode/Unicode.h>

#include "PlatformString.h"

namespace WebCore {
    class TextEncoding;

    // Specifies what will happen when a character is encountered that is
    // not encodable in the character set.
    enum UnencodableHandling {
        // Substitutes the replacement character "?".
        QuestionMarksForUnencodables,

        // Encodes the character as an XML entity. For example, U+06DE
        // would be "&#1758;" (0x6DE = 1758 in octal).
        EntitiesForUnencodables,

        // Encodes the character as en entity as above, but escaped
        // non-alphanumeric characters. This is used in URLs.
        // For example, U+6DE would be "%26%231758%3B".
        URLEncodedEntitiesForUnencodables,
    };

    typedef char UnencodableReplacementArray[32];

    class TextCodec : Noncopyable {
public:
#if NO_MACRO_NEW
	// Placement operator new.
void* operator new(size_t, void* p) { return p; }
void* operator new[](size_t, void* p) { return p; }
 
void* operator new(size_t size)
{
     void* p = fastMalloc(size);
     fastMallocMatchValidateMalloc(p, WTF::Internal::AllocTypeClassNew);
     return p;
}
 
void operator delete(void* p)
{
     fastMallocMatchValidateFree(p, WTF::Internal::AllocTypeClassNew);
     fastFree(p);  // We don't need to check for a null pointer; the compiler does this.
}
 
void* operator new[](size_t size)
{
     void* p = fastMalloc(size);
     fastMallocMatchValidateMalloc(p, WTF::Internal::AllocTypeClassNewArray);
     return p;
}
 
void operator delete[](void* p)
{
     fastMallocMatchValidateFree(p, WTF::Internal::AllocTypeClassNewArray);
     fastFree(p);  // We don't need to check for a null pointer; the compiler does this.
}
#endif //NO_MACRO_NEW
	public:
        virtual ~TextCodec();

        String decode(const char* str, size_t length, bool flush = false)
        {
            bool ignored;
            return decode(str, length, flush, false, ignored);
        }
        
        virtual String decode(const char*, size_t length, bool flush, bool stopOnError, bool& sawError) = 0;
        virtual CString encode(const UChar*, size_t length, UnencodableHandling) = 0;

        // Fills a null-terminated string representation of the given
        // unencodable character into the given replacement buffer. 
        // The length of the string (not including the null) will be returned.
        static int getUnencodableReplacement(unsigned codePoint, UnencodableHandling, UnencodableReplacementArray);
    };

    typedef void (*EncodingNameRegistrar)(const char* alias, const char* name);

    typedef std::auto_ptr<TextCodec> (*NewTextCodecFunction)(const TextEncoding&, const void* additionalData);
    typedef void (*TextCodecRegistrar)(const char* name, NewTextCodecFunction, const void* additionalData);

} // namespace WebCore

#endif // TextCodec_h