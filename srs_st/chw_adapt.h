#ifndef CHW_ADAPT_H
#define CHW_ADAPT_H

#include <string>
#include <inttypes.h>
#include <unistd.h>

using namespace std;
typedef int64_t srs_utime_t;

#define srs_min(a, b) (((a) < (b))? (a) : (b))
#define srs_max(a, b) (((a) < (b))? (b) : (a))

#define SYS_TIME_RESOLUTION_US 300*1000
#define SRS_UTIME_MILLISECONDS 1000
#define srsu2ms(us) ((us) / SRS_UTIME_MILLISECONDS)
#define srsu2msi(us) int((us) / SRS_UTIME_MILLISECONDS)

// Never timeout.
#define SRS_UTIME_NO_TIMEOUT ((srs_utime_t) -1LL)

// 自动释放对象
// To delete object.
#define SrsAutoFree(className, instance) \
    impl_SrsAutoFree<className> _auto_free_##instance(&instance, false, false, NULL)
// To delete array.
#define SrsAutoFreeA(className, instance) \
    impl_SrsAutoFree<className> _auto_free_array_##instance(&instance, true, false, NULL)
// Use free instead of delete.
#define SrsAutoFreeF(className, instance) \
    impl_SrsAutoFree<className> _auto_free_##instance(&instance, false, true, NULL)
// Use hook instead of delete.
#define SrsAutoFreeH(className, instance, hook) \
    impl_SrsAutoFree<className> _auto_free_##instance(&instance, false, false, hook)
// The template implementation.
template<class T>
class impl_SrsAutoFree
{
private:
    T** ptr;
    bool is_array;
    bool _use_free;
    void (*_hook)(T*);
public:
    // If use_free, use free(void*) to release the p.
    // If specified hook, use hook(p) to release it.
    // Use delete to release p, or delete[] if p is an array.
    impl_SrsAutoFree(T** p, bool array, bool use_free, void (*hook)(T*)) {
        ptr = p;
        is_array = array;
        _use_free = use_free;
        _hook = hook;
    }

    virtual ~impl_SrsAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (_use_free) {
            free(*ptr);
        } else if (_hook) {
            _hook(*ptr);
        } else {
            if (is_array) {
                delete[] *ptr;
            } else {
                delete *ptr;
            }
        }

        *ptr = NULL;
    }
};
//SRS 上下文ID，就是个字符串
class _SrsContextId
{
private:
    std::string v_;//上下文ID
public:
    _SrsContextId();
    _SrsContextId(const _SrsContextId& cp);
    _SrsContextId& operator=(const _SrsContextId& cp);
    virtual ~_SrsContextId();
public:
    const char* c_str() const;
    bool empty() const;
    // Compare the two context id. @see http://www.cplusplus.com/reference/string/string/compare/
    //      0	They compare equal
    //      <0	Either the value of the first character that does not match is lower in the compared string, or all compared characters match but the compared string is shorter.
    //      >0	Either the value of the first character that does not match is greater in the compared string, or all compared characters match but the compared string is longer.
    int compare(const _SrsContextId& to) const;
    // Set the value of context id.
    _SrsContextId& set_value(const std::string& v);
};
typedef _SrsContextId SrsContextId;

class ISrsContext
{
public:
    ISrsContext();
    virtual ~ISrsContext();
public:
    // Generate a new context id.
    // @remark We do not set to current thread, user should do this.
    virtual SrsContextId generate_id() = 0;//生成上下文ID
    // Get the context id of current thread.
    virtual const SrsContextId& get_id() = 0;//获取上下文ID
    // Set the context id of current thread.
    // @return the current context id.
    virtual const SrsContextId& set_id(const SrsContextId& v) = 0;//设置上下文ID
};

// @global User must implements the LogContext and define a global instance.
extern ISrsContext* _srs_context;

std::string srs_random_str(int len);//生成len个随机数
#endif // CHW_ADAPT_H
