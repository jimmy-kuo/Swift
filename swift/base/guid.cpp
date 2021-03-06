/*
 * Copyright (c) 2014 ApusApp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "swift/base/guid.h"

#include <stdio.h>  // snprintf
#include <cstdlib>  // strtoull

#include "swift/base/stringpiece.h"
#include "swift/base/random.h"

namespace swift {

const char* Guid::kHexChars = "0123456789ABCDEF";

// static public
bool Guid::RandomDataToGuidString(const uint64_t bytes[2], std::string& guid)
{
    char buf[64];
    size_t n = snprintf(buf, sizeof(buf), "%08X-%04X-%04X-%04X-%012llX",
                        static_cast<uint32_t>(bytes[0] >> 32),
                        static_cast<uint32_t>((bytes[0] >> 16) & 0x0000ffff),
                        static_cast<uint32_t>(bytes[0] & 0x0000ffff),
                        static_cast<uint32_t>(bytes[1] >> 48),
                        bytes[1] & 0x0000ffffffffffffULL);
    if (36 == n) {
        guid = std::move(std::string(buf, n));
        return true;
    }

    return false;
}

// static public
bool Guid::Generate(std::string& guid)
{
    uint64_t bytes[2] = { Random::RandUInt64(), Random::RandUInt64() };
    return RandomDataToGuidString(bytes, guid);
}

// static public
bool Guid::IsValid(const std::string& guid)
{
    size_t len = guid.size();
    if (36u != len) {
        return false;
    }

    StringPiece hex_chars(kHexChars, 16);
    for (size_t i = 0; i < len; ++i) {
        char c = guid[i];
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if ('-' != c) {
                return false;
            }
        } else {
            if (StringPiece::npos == hex_chars.find(c)) {
                return false;
            }
        }
    }

    return true;
}

// public
Guid::Guid()
{
    for (uint32_t i = 0; i < sizeof(bytes_) / sizeof(uint64_t); ++i) {
        bytes_[i] = Random::RandUInt64();
    }
}

// public
Guid::Guid(const std::string& guid)
{
    if (36u == guid.size() && IsValid(guid)) {
        size_t n = 0;
        size_t j = 0;
        char buf[17] = { '\0' };
        for (size_t i = 0; i < guid.size(); ++i) {
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                continue;
            }

            buf[j++] = guid[i];
            if (17 == i || 35 == i) {
                buf[16] = '\0';
                bytes_[n++] = std::strtoull(buf, nullptr, 16);
                j = 0;
            }
        }
    } else {
        for (uint32_t i = 0; i < sizeof(bytes_) / sizeof(uint64_t); ++i) {
            bytes_[i] = 0u;
        }
    }
}

// public
Guid::Guid(const Guid& guid)
{
    if (this != &guid) {
        for (uint32_t i = 0; i < sizeof(bytes_) / sizeof(uint64_t); ++i) {
            bytes_[i] = guid.bytes_[i];
        }
    }
}

// public
Guid& Guid::operator= (const Guid& rhs)
{
    if (this != &rhs) {
        for (uint32_t i = 0; i < sizeof(bytes_) / sizeof(uint64_t); ++i) {
            bytes_[i] = rhs.bytes_[i];
        }
    }

    return *this;
}

} // namespace swift
