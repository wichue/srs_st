#ifndef SRS_KERNEL_ERROR_HPP
#define SRS_KERNEL_ERROR_HPP

#include <string>
#define srs_min(a, b) (((a) < (b))? (a) : (b))
#define srs_max(a, b) (((a) < (b))? (b) : (a))
#define srs_freep(p) \
    delete p; \
    p = NULL; \
    (void)0
// Please use the freepa(T[]) to free an array, otherwise the behavior is undefined.
#define srs_freepa(pa) \
    delete[] pa; \
    pa = NULL; \
    (void)0

/**************************************************/
/* The system error. */
#define SRS_ERRNO_MAP_SYSTEM(XX) \
    XX(ERROR_SOCKET_CREATE                 , 1000, "SocketCreate", "Create socket fd failed") \
    XX(ERROR_SYSTEM_FILE_SETVBUF           , 1096, "FileSetVBuf", "Failed to set file vbuf") \
    XX(ERROR_THREAD           , 1099, "FileSetVBuf", "ERROR_THREAD") \

/**************************************************/
/* RTMP protocol error. */
#define SRS_ERRNO_MAP_RTMP(XX) \
    XX(ERROR_RTMP_PLAIN_REQUIRED           , 2000, "RtmpPlainRequired", "RTMP handshake requires plain text") \
    XX(ERROR_CONTROL_REPUBLISH             , 2999, "RtmpRepublish", "RTMP stream is republished")

// For human readable error generation. Generate integer error code.
#define SRS_ERRNO_GEN(n, v, m, s) n = v,
enum SrsErrorCode {
#ifndef _WIN32
    ERROR_SUCCESS = 0,
#endif
    SRS_ERRNO_MAP_SYSTEM(SRS_ERRNO_GEN)
    SRS_ERRNO_MAP_RTMP(SRS_ERRNO_GEN)
};
#undef SRS_ERRNO_GEN

// The complex error carries code, message, callstack and instant variables,
// which is more strong and easy to locate problem by log,
// 错误类包含代码、消息、调用堆栈和即时变量，更强大，更容易通过日志定位问题，
// please @read https://github.com/ossrs/srs/issues/913
class SrsCplxError
{
private:
    int code;
    SrsCplxError* wrapped;
    std::string msg;

    std::string func;
    std::string file;
    int line;

    /*SrsContextId*/std::string cid;//上下文ID改为std::string ,简化依赖
    int rerrno;

    std::string desc;//详细描述
    std::string _summary;//摘要
private:
    SrsCplxError();
public:
    virtual ~SrsCplxError();
private:
    virtual std::string description();
    virtual std::string summary();
public:
    static SrsCplxError* create(const char* func, const char* file, int line, int code, const char* fmt, ...);
    static SrsCplxError* wrap(const char* func, const char* file, int line, SrsCplxError* err, const char* fmt, ...);
    static SrsCplxError* success();
    static SrsCplxError* copy(SrsCplxError* from);
    static std::string description(SrsCplxError* err);
    static std::string summary(SrsCplxError* err);
    static int error_code(SrsCplxError* err);
    static std::string error_code_str(SrsCplxError* err);
    static std::string error_code_longstr(SrsCplxError* err);
public:
    static void srs_assert(bool expression);
};
typedef SrsCplxError* srs_error_t;

// Error helpers, should use these functions to new or wrap an error.
#define srs_success 0 // SrsCplxError::success()
#define srs_error_new(ret, fmt, ...) SrsCplxError::create(__FUNCTION__, __FILE__, __LINE__, ret, fmt, ##__VA_ARGS__)
#define srs_error_wrap(err, fmt, ...) SrsCplxError::wrap(__FUNCTION__, __FILE__, __LINE__, err, fmt, ##__VA_ARGS__)
#define srs_error_copy(err) SrsCplxError::copy(err)
#define srs_error_desc(err) SrsCplxError::description(err)
#define srs_error_summary(err) SrsCplxError::summary(err)
#define srs_error_code(err) SrsCplxError::error_code(err)
#define srs_error_code_str(err) SrsCplxError::error_code_str(err)
#define srs_error_code_longstr(err) SrsCplxError::error_code_longstr(err)
#define srs_error_reset(err) srs_freep(err); err = srs_success

#ifndef srs_assert
#define srs_assert(expression) SrsCplxError::srs_assert(expression)
#endif

#endif
