#pragma once
#include <argagg.hpp>
#include <functional>
#include <utility>

#include "register.hh"

namespace TripleA2
{
    using CommandFunction = std::function<void(const argagg::parser_results&)>;
    using CommandPair = std::pair<CommandFunction, argagg::parser>;
    using CommandRegistry = Register<CommandPair>;
}
