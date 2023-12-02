#ifndef SRS_PROTOCOL_ST_HPP
#define SRS_PROTOCOL_ST_HPP

#include "chw_adapt.h"
#include <string>

#include <srs_kernel_error.hpp>
// Wrap for coroutine.
// 对ST库的封装，提供接口
typedef void* srs_netfd_t;
typedef void* srs_thread_t;
typedef void* srs_cond_t;
typedef void* srs_mutex_t;

// Initialize ST, requires epoll for linux.
extern srs_error_t srs_st_init();
// Destroy ST, free resources for asan detecting.
extern void srs_st_destroy(void);

// Close the netfd, and close the underlayer fd.
// @remark when close, user must ensure io completed.
extern void srs_close_stfd(srs_netfd_t& stfd);

// Set the FD_CLOEXEC of FD.
extern srs_error_t srs_fd_closeexec(int fd);

// Set the SO_REUSEADDR of fd.
extern srs_error_t srs_fd_reuseaddr(int fd);

// Set the SO_REUSEPORT of fd.
extern srs_error_t srs_fd_reuseport(int fd);

// Set the SO_KEEPALIVE of fd.
extern srs_error_t srs_fd_keepalive(int fd);

// Get current coroutine/thread.
extern srs_thread_t srs_thread_self();
extern void srs_thread_exit(void* retval);
extern int srs_thread_join(srs_thread_t thread, void **retvalp);
extern void srs_thread_interrupt(srs_thread_t thread);
extern void srs_thread_yield();

// For utest to mock the thread create.
typedef void* (*_ST_THREAD_CREATE_PFN)(void *(*start)(void *arg), void *arg, int joinable, int stack_size);
extern _ST_THREAD_CREATE_PFN _pfn_st_thread_create;

// For client, to open socket and connect to server.
// @param tm The timeout in srs_utime_t.
extern srs_error_t srs_tcp_connect(std::string server, int port, srs_utime_t tm, srs_netfd_t* pstfd);

// For server, listen at TCP endpoint.
extern srs_error_t srs_tcp_listen(std::string ip, int port, srs_netfd_t* pfd);

// For server, listen at UDP endpoint.
extern srs_error_t srs_udp_listen(std::string ip, int port, srs_netfd_t* pfd);

// Wrap for coroutine.
extern srs_cond_t srs_cond_new();
extern int srs_cond_destroy(srs_cond_t cond);
extern int srs_cond_wait(srs_cond_t cond);
extern int srs_cond_timedwait(srs_cond_t cond, srs_utime_t timeout);
extern int srs_cond_signal(srs_cond_t cond);
extern int srs_cond_broadcast(srs_cond_t cond);

extern srs_mutex_t srs_mutex_new();
extern int srs_mutex_destroy(srs_mutex_t mutex);
extern int srs_mutex_lock(srs_mutex_t mutex);
extern int srs_mutex_unlock(srs_mutex_t mutex);

extern int srs_key_create(int* keyp, void (*destructor)(void*));
extern int srs_thread_setspecific(int key, void* value);
extern int srs_thread_setspecific2(srs_thread_t thread, int key, void* value);
extern void* srs_thread_getspecific(int key);

extern int srs_netfd_fileno(srs_netfd_t stfd);

extern int srs_usleep(srs_utime_t usecs);

extern srs_netfd_t srs_netfd_open_socket(int osfd);
extern srs_netfd_t srs_netfd_open(int osfd);

extern int srs_recvfrom(srs_netfd_t stfd, void *buf, int len, struct sockaddr *from, int *fromlen, srs_utime_t timeout);
extern int srs_sendto(srs_netfd_t stfd, void *buf, int len, const struct sockaddr *to, int tolen, srs_utime_t timeout);
extern int srs_recvmsg(srs_netfd_t stfd, struct msghdr *msg, int flags, srs_utime_t timeout);
extern int srs_sendmsg(srs_netfd_t stfd, const struct msghdr *msg, int flags, srs_utime_t timeout);

extern srs_netfd_t srs_accept(srs_netfd_t stfd, struct sockaddr *addr, int *addrlen, srs_utime_t timeout);

extern ssize_t srs_read(srs_netfd_t stfd, void *buf, size_t nbyte, srs_utime_t timeout);

extern bool srs_is_never_timeout(srs_utime_t tm);

#endif
