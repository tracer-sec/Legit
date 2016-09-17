#pragma once

namespace Legit
{
    #ifdef _WIN32
        const char SEPARATOR = '\\';
    #else
        const char SEPARATOR = '/';

        template<typename T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
    #endif
}

