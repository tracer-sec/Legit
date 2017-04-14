#pragma once

#include <memory>

#ifdef _WIN32
    #define NOMINMAX
#endif

namespace Legit
{
    #ifdef _WIN32
        const char SEPARATOR = '\\';
        const wchar_t SEPARATOR_WIDE = L'\\';
    #else
        const char SEPARATOR = '/';
        const wchar_t SEPARATOR_WIDE = L'/';

        template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
    #endif
}
