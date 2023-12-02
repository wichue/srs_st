#include "chw_adapt.h"

#include <sys/time.h>

srs_utime_t _srs_system_time_us_cache = 0;
srs_utime_t _srs_system_time_startup_time = 0;
typedef int (*srs_gettimeofday_t) (struct timeval* tv, struct timezone* tz);
srs_gettimeofday_t _srs_gettimeofday = (srs_gettimeofday_t)::gettimeofday;
srs_utime_t srs_update_system_time()
{
    timeval now;

    if (_srs_gettimeofday(&now, NULL) < 0) {
        printf("gettimeofday failed, ignore");
        return -1;
    }

    // we must convert the tv_sec/tv_usec to int64_t.
    int64_t now_us = ((int64_t)now.tv_sec) * 1000 * 1000 + (int64_t)now.tv_usec;

    // for some ARM os, the starttime maybe invalid,
    // for example, on the cubieboard2, the srs_startup_time is 1262304014640,
    // while now is 1403842979210 in ms, diff is 141538964570 ms, 1638 days
    // it's impossible, and maybe the problem of startup time is invalid.
    // use date +%s to get system time is 1403844851.
    // so we use relative time.
    if (_srs_system_time_us_cache <= 0) {
        _srs_system_time_startup_time = _srs_system_time_us_cache = now_us;
        return _srs_system_time_us_cache;
    }

    // use relative time.
    int64_t diff = now_us - _srs_system_time_us_cache;
    diff = srs_max(0, diff);
    if (diff < 0 || diff > 1000 * SYS_TIME_RESOLUTION_US) {
        printf("clock jump, history=%" PRId64 "us, now=%" PRId64 "us, diff=%" PRId64 "us", _srs_system_time_us_cache, now_us, diff);
        _srs_system_time_startup_time += diff;
    }

    _srs_system_time_us_cache = now_us;
    printf("clock updated, startup=%" PRId64 "us, now=%" PRId64 "us", _srs_system_time_startup_time, _srs_system_time_us_cache);

    return _srs_system_time_us_cache;
}

long srs_random()
{
    static bool _random_initialized = false;
    if (!_random_initialized) {
        _random_initialized = true;
        ::srandom((unsigned long)(srs_update_system_time() | (::getpid()<<13)));
    }

    return random();
}

std::string srs_random_str(int len)
{
    static string random_table = "01234567890123456789012345678901234567890123456789abcdefghijklmnopqrstuvwxyz";

    string ret;
    ret.reserve(len);
    for (int i = 0; i < len; ++i) {
        ret.append(1, random_table[srs_random() % random_table.size()]);
    }

    return ret;
}



_SrsContextId::_SrsContextId()
{
}

_SrsContextId::_SrsContextId(const _SrsContextId& cp)
{
    v_ = cp.v_;
}

_SrsContextId& _SrsContextId::operator=(const _SrsContextId& cp)
{
    v_ = cp.v_;
    return *this;
}

_SrsContextId::~_SrsContextId()
{
}

const char* _SrsContextId::c_str() const
{
    return v_.c_str();
}

bool _SrsContextId::empty() const
{
    return v_.empty();
}

int _SrsContextId::compare(const _SrsContextId& to) const
{
    return v_.compare(to.v_);
}

_SrsContextId& _SrsContextId::set_value(const std::string& v)
{
    v_ = v;
    return *this;
}

ISrsContext::ISrsContext()
{
}

ISrsContext::~ISrsContext()
{
}
