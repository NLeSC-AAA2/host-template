#pragma once
#include <map>
#include <memory>
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
            auto result = dir().emplace(name, std::forward<Args>(args)...);
            if (!result.second) {
                throw std::runtime_error("Key already in Register: " + name);
            }
        }

        static T &get(std::string const &name)
        {
            return dir().at(name);
        }
    };

    template <typename T>
    std::unique_ptr<std::map<std::string, T>> Register<T>::s_dir;
}
