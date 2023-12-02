//
// Copyright (c) 2013-2023 The SRS Authors
//
// SPDX-License-Identifier: MIT or MulanPSL-2.0
//

#ifndef SRS_PROTOCOL_LOG_HPP
#define SRS_PROTOCOL_LOG_HPP

#include <map>
#include <string>

#include <srs_protocol_st.hpp>
#include "chw_adapt.h"

// The st thread context, get_id will get the st-thread id,
// which identify the client.
// st线程上下文，get_id将得到标识客户端的st线程id。
class SrsThreadContext : public ISrsContext
{
private:
    std::map<srs_thread_t, SrsContextId> cache;
public:
    SrsThreadContext();
    virtual ~SrsThreadContext();
public:
    virtual SrsContextId generate_id();
    virtual const SrsContextId& get_id();
    virtual const SrsContextId& set_id(const SrsContextId& v);
private:
    virtual void clear_cid();
};

// Set the context id of specified thread, not self.
// 设置指定线程的上下文id，而不是设置自身。
extern const SrsContextId& srs_context_set_cid_of(srs_thread_t trd, const SrsContextId& v);

// The context restore stores the context and restore it when done.
// 上下文恢复存储上下文，并在完成时进行恢复。
// Usage:
//      SrsContextRestore(_srs_context->get_id());
#define SrsContextRestore(cid) impl_SrsContextRestore _context_restore_instance(cid)
class impl_SrsContextRestore
{
private:
    SrsContextId cid_;
public:
    impl_SrsContextRestore(SrsContextId cid);
    virtual ~impl_SrsContextRestore();
};

#endif
