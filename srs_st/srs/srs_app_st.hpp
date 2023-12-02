#ifndef SRS_APP_ST_HPP
#define SRS_APP_ST_HPP

#include <string>
#include "chw_adapt.h"

#include <srs_kernel_error.hpp>
#include <srs_protocol_st.hpp>
#include "srs_protocol_log.hpp"

class SrsFastCoroutine;
// 每个协程都要继承这个类
class ISrsCoroutineHandler
{
public:
    ISrsCoroutineHandler();
    virtual ~ISrsCoroutineHandler();
public:
    // Do the work. The ST-coroutine will terminated normally if it returned.
    // @remark If the cycle has its own loop, it must check the thread pull.
    // 协程处理函数，如果返回则协程结束
    virtual srs_error_t cycle() = 0;
};

// Start the object, generally a croutine.
// 通常是启动一个ST对象
class ISrsStartable
{
public:
    ISrsStartable();
    virtual ~ISrsStartable();
public:
    virtual srs_error_t start() = 0;
};

// The corotine object.
// 协程基类
class SrsCoroutine : public ISrsStartable
{
public:
    SrsCoroutine();
    virtual ~SrsCoroutine();
public:
    virtual void stop() = 0;
    virtual void interrupt() = 0;
    // @return a copy of error, which should be freed by user.
    //      NULL if not terminated and user should pull again.
    virtual srs_error_t pull() = 0;
    // Get and set the context id of coroutine.
    virtual const SrsContextId& cid() = 0;
    virtual void set_cid(const SrsContextId& cid) = 0;
};

// An empty coroutine, user can default to this object before create any real coroutine.
// @see https://github.com/ossrs/srs/pull/908
// 一个空的协程，用户可以在创建任何真正的协程序之前默认为这个对象。
class SrsDummyCoroutine : public SrsCoroutine
{
private:
    SrsContextId cid_;
public:
    SrsDummyCoroutine();
    virtual ~SrsDummyCoroutine();
public:
    virtual srs_error_t start();
    virtual void stop();
    virtual void interrupt();
    virtual srs_error_t pull();
    virtual const SrsContextId& cid();
    virtual void set_cid(const SrsContextId& cid);
};

// A ST-coroutine is a lightweight thread, just like the goroutine.
// But the goroutine maybe run on different thread, while ST-coroutine only
// run in single thread, because it use setjmp and longjmp, so it may cause
// problem in multiple threads. For SRS, we only use single thread module,
// like NGINX to get very high performance, with asynchronous and non-blocking
// sockets.
// @reamrk For multiple processes, please use go-oryx to fork many SRS processes.
//      Please read https://github.com/ossrs/go-oryx
// @remark For debugging of ST-coroutine, read _st_iterate_threads_flag of ST/README
//      https://github.com/ossrs/state-threads/blob/st-1.9/README#L115
// @remark We always create joinable thread, so we must join it or memory leak,
//      Please read https://github.com/ossrs/srs/issues/78

// ST-coroutine是一个轻量级的线程，就像goroutine一样。
// 但是goroutine可能在不同的线程上运行，而ST-coroutine只在单个线程中运行，因为它使用了setjmp和longjmp，所以它可能会在多个线程中导致问题。
// 对于SRS，我们只使用单线程模块，类似NGINX，来获得非常高的性能，具有异步和非阻塞套接字。
// 对于多个进程，请使用go-oryx来fork多个SRS进程。
//SrsSTCoroutine 是对协程的封装
class SrsSTCoroutine : public SrsCoroutine
{
private:
    SrsFastCoroutine* impl_;
public:
    // Create a thread with name n and handler h.
    // @remark User can specify a cid for thread to use, or we will allocate a new one.
    SrsSTCoroutine(std::string n, ISrsCoroutineHandler* h);
    SrsSTCoroutine(std::string n, ISrsCoroutineHandler* h, SrsContextId cid);
    virtual ~SrsSTCoroutine();
public:
    // Set the stack size of coroutine, default to 0(64KB).
    void set_stack_size(int v);
public:
    // Start the thread.
    // @remark Should never start it when stopped or terminated.
    virtual srs_error_t start();//启动协程
    // Interrupt the thread then wait to terminated.
    // @remark If user want to notify thread to quit async, for example if there are
    //      many threads to stop like the encoder, use the interrupt to notify all threads
    //      to terminate then use stop to wait for each to terminate.
    virtual void stop();//停止协程
    // Interrupt the thread and notify it to terminate, it will be wakeup if it's blocked
    // in some IO operations, such as st_read or st_write, then it will found should quit,
    // finally the thread should terminated normally, user can use the stop to join it.
    //中断线程并通知它终止，如果它在一些IO操作中被阻止，如st_read或st_write，它就会被唤醒，然后它就会发现应该退出，最后线程会正常终止，用户可以使用stop加入它。
    virtual void interrupt();
    // Check whether thread is terminated normally or error(stopped or termianted with error),
    // and the thread should be running if it return ERROR_SUCCESS.
    // @remark Return specified error when thread terminated normally with error.
    // @remark Return ERROR_THREAD_TERMINATED when thread terminated normally without error.
    // @remark Return ERROR_THREAD_INTERRUPED when thread is interrupted.
    virtual srs_error_t pull();//检查线程是否正常终止或错误（停止或出现错误），如果返回ERROR_SUCCESS，线程应该运行。
    // Get and set the context id of thread.
    virtual const SrsContextId& cid();//获取上下文ID
    virtual void set_cid(const SrsContextId& cid);//设置上下文ID
};

// High performance coroutine.
// 高性能协程，真正调用ST库启动协成
class SrsFastCoroutine
{
private:
    std::string name;
    int stack_size;
    ISrsCoroutineHandler* handler;//基类指针，指向要启动协程的那个对象
private:
    srs_thread_t trd;
    SrsContextId cid_;//当前协程的上下文ID
    srs_error_t trd_err;
private:
    bool started;
    bool interrupted;
    bool disposed;
    // Cycle done, no need to interrupt it.
    bool cycle_done;
private:
    // Sub state in disposed, we need to wait for thread to quit.
    // 子状态被处理后，我们需要等待线程退出。
    bool stopping_;
    SrsContextId stopping_cid_;
public:
    SrsFastCoroutine(std::string n, ISrsCoroutineHandler* h);
    SrsFastCoroutine(std::string n, ISrsCoroutineHandler* h, SrsContextId cid);
    virtual ~SrsFastCoroutine();
public:
    void set_stack_size(int v);
public:
    srs_error_t start();//创建协程
    void stop();
    void interrupt();
    inline srs_error_t pull() {
        if (trd_err == srs_success) {
            return srs_success;
        }
        return srs_error_copy(trd_err);
    }
    const SrsContextId& cid();//获取上下文ID
    virtual void set_cid(const SrsContextId& cid);//设置上下文ID
private:
    srs_error_t cycle();//启动协程处理函数
    static void* pfn(void* arg);
};

// Like goroutine sync.WaitGroup.
// 类似go语言的sync.WaitGroup
class SrsWaitGroup
{
private:
    int nn_;
    srs_cond_t done_;
public:
    SrsWaitGroup();
    virtual ~SrsWaitGroup();
public:
    // When start for n coroutines.
    void add(int n);
    // When coroutine is done.
    void done();
    // Wait for all corotine to be done.
    void wait();
};

#endif

