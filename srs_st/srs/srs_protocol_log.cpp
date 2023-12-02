//
// Copyright (c) 2013-2023 The SRS Authors
//
// SPDX-License-Identifier: MIT or MulanPSL-2.0
//

#include <srs_protocol_log.hpp>

#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <sstream>
using namespace std;

#include <srs_kernel_error.hpp>

//SrsPps* _srs_pps_cids_get = NULL;
//SrsPps* _srs_pps_cids_set = NULL;

#define SRS_BASIC_LOG_SIZE 8192

SrsThreadContext::SrsThreadContext()
{
}

SrsThreadContext::~SrsThreadContext()
{
}

SrsContextId SrsThreadContext::generate_id()
{
    SrsContextId cid = SrsContextId();
    return cid.set_value(srs_random_str(8));
}

static SrsContextId _srs_context_default;
static int _srs_context_key = -1;
void _srs_context_destructor(void* arg)
{
    SrsContextId* cid = (SrsContextId*)arg;
    srs_freep(cid);
}

const SrsContextId& SrsThreadContext::get_id()
{
//    ++_srs_pps_cids_get->sugar;

    if (!srs_thread_self()) {
        return _srs_context_default;
    }

    void* cid = srs_thread_getspecific(_srs_context_key);
    if (!cid) {
        return _srs_context_default;
    }

    return *(SrsContextId*)cid;
}

const SrsContextId& SrsThreadContext::set_id(const SrsContextId& v)
{
    return srs_context_set_cid_of(srs_thread_self(), v);
}

void SrsThreadContext::clear_cid()
{
}

const SrsContextId& srs_context_set_cid_of(srs_thread_t trd, const SrsContextId& v)
{
//    ++_srs_pps_cids_set->sugar;

    if (!trd) {
        _srs_context_default = v;
        return v;
    }

    SrsContextId* cid = new SrsContextId();
    *cid = v;

    if (_srs_context_key < 0) {
        int r0 = srs_key_create(&_srs_context_key, _srs_context_destructor);
        srs_assert(r0 == 0);
    }

    int r0 = srs_thread_setspecific2(trd, _srs_context_key, cid);
    srs_assert(r0 == 0);

    return v;
}

impl_SrsContextRestore::impl_SrsContextRestore(SrsContextId cid)
{
    cid_ = cid;
}

impl_SrsContextRestore::~impl_SrsContextRestore()
{
    _srs_context->set_id(cid_);
}
