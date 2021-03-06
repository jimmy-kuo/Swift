/*
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

#include <type_traits> // for static_assert
#include <sys/time.h>
#include <stdio.h>

#include "swift/base/timestamp.h"

namespace swift {

static_assert (sizeof(Timestamp) == sizeof(int64_t), "sizeof(Timestamp) must equal to sizeof(int64_t)");

Timestamp::Timestamp () : micro_seconds_since_epoch_ (0)
{
}

Timestamp::Timestamp (const Timestamp& rhs)
{
    if (this != &rhs) {
        micro_seconds_since_epoch_ = rhs.micro_seconds_since_epoch_;
    }
}

Timestamp::Timestamp (const int64_t micro_seconds_since_epoch) 
    : micro_seconds_since_epoch_ (micro_seconds_since_epoch)
{

}

Timestamp::~Timestamp ()
{

}

Timestamp& Timestamp::operator= (const Timestamp& rhs)
{
    if (this != &rhs) {
        micro_seconds_since_epoch_ = rhs.micro_seconds_since_epoch_;
    }

    return *this;
}

// static public
Timestamp Timestamp::Now ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return Timestamp (tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

// static public
Timestamp Timestamp::Invalid ()
{
    return Timestamp ();
}

// public
std::string Timestamp::ToSecDotMicroString () const
{
    char buf[32] = {0};
    int64_t seconds = static_cast<int64_t>(micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
    int64_t microseconds = micro_seconds_since_epoch_ % kMicroSecondsPerSecond;
    // PRId64 GCC compile with -D__STDC_FORMAT_MACROS and include <inttypes.h>
    int size = snprintf (buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return std::string (buf, size);
}

// public 
std::string Timestamp::ToString () const
{
    char buf[32] = {0};
    int size = snprintf (buf, sizeof(buf) - 1, "%" PRId64"", micro_seconds_since_epoch_);
    return std::string (buf, size);
}

// public
std::string Timestamp::ToFormattedString (bool show_microseconds /*= true*/) const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(micro_seconds_since_epoch_ / kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(micro_seconds_since_epoch_ % kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (show_microseconds) {
        snprintf (buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
            microseconds);
    }
    else {
        snprintf (buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return std::string (buf);
}

} // namespace swift
