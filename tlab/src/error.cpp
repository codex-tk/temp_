#include <iostream>
#include <tlab/error.hpp>

namespace tlab {

class error_category_impl : public ::std::error_category {
public:
    error_category_impl(void) noexcept {
        _error_message[tlab::errc::success] = "success";
        _error_message[tlab::errc::operation_canceled] = "operation_canceled";
    }

    virtual const char *name() const noexcept override {
        return "tlab_error_category";
    }

    virtual ::std::string message(int code) const override {
        auto it = _error_message.find(static_cast<tlab::errc>(code));
        if (it != _error_message.end())
            return it->second;
        return std::string();
    }

    virtual bool equivalent(int code, const ::std::error_condition &cond) const
        noexcept override {
        if (cond.value() == 0 && code == 0)
            return true;
        if (cond.category() == std::generic_category()) {
            switch (static_cast<std::errc>(cond.value())) {
            case ::std::errc::operation_canceled:
                return static_cast<tlab::errc>(code) ==
                       tlab::errc::operation_canceled;
            default: break;
            }
        }
        return false;
    }
    /*
    virtual bool equivalent(const std::error_code& ec, int errval) const {
            switch (static_cast<tlab::errc>(errval)) {
            case tlab::errc::success:
                    if (!ec) return true;
                    break;
            }
            return false;
    }
    */
    static error_category_impl &instance(void) {
        static error_category_impl category;
        return category;
    }

private:
    ::std::map<tlab::errc, std::string> _error_message;
};

#if defined(_WIN32) || defined(__WIN32__)

class windows_error_category_impl : public std::error_category {
public:
    windows_error_category_impl(void) noexcept {}

    virtual const char *name() const noexcept override {
        return "windows_error_category";
    }

    virtual std::string message(int code) const override {
        const size_t buffer_size = 4096;
        DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
        LPCVOID lpSource = NULL;
        char buffer[buffer_size] = {
            0,
        };
        unsigned long result;
        result = ::FormatMessageA(dwFlags, lpSource, code,
                                  MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                                  buffer, buffer_size, NULL);

        if (result == 0) {
            std::ostringstream os;
            os << "unknown error code: " << code << ".";
            return os.str();
        }
        char *pos = strrchr(buffer, '\r');
        if (pos)
            *pos = '\0';
        return std::string(buffer);
    }
    static windows_error_category_impl &instance(void) {
        static windows_error_category_impl category;
        return category;
    }
};

#endif

#if defined(_WIN32) || defined(__WIN32__)
std::error_code system_error(void) {
    return std::error_code(WSAGetLastError(),
                           tlab::windows_error_category_impl::instance());
}
#else
std::error_code system_error(void) {
    return std::error_code(errno, std::generic_category());
}
#endif

std::error_code make_error_code(tlab::errc ec) {
    return std::error_code(static_cast<int>(ec),
                           tlab::error_category_impl::instance());
}

std::error_condition make_error_condition(tlab::errc ec) {
    return std::error_condition(
        static_cast<int>(ec), tlab::error_category_impl::instance());
}

} // namespace tlab
