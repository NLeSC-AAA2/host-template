#pragma once
#include <map>
#include <utility>
#include <string>

namespace TripleA2
{
    template <typename T>
    class Register {
        using dir_t = std::map<std::string, T>;
        static std::unique_ptr<dir_t> s_dir;

        static dir_t &dir() {
            if (!s_dir) {
                s_dir = std::make_unique<dir_t>();
            }
            return *s_dir;
        }

    public:
        template <typename ...Args>
        Register(std::string const &name, Args &&...args)
        {
            dir()[name] = T(std::forward<Args>(args)...);
        }

        static T &get(std::string const &name)
        {
            return dir()[name];
        }
    };

    template <typename T>
    std::unique_ptr<std::map<std::string, T>> Register<T>::s_dir;
}

