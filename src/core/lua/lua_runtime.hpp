#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "util/result.hpp"

struct lua_State;

namespace vk::core {

class LuaRuntime {
public:
    LuaRuntime();
    ~LuaRuntime();

    VK_NONCOPYABLE(LuaRuntime);
    VK_NONMOVABLE(LuaRuntime);

    [[nodiscard]] Result<std::string> execute(std::string_view source, std::string_view chunkName = "chunk");
    void installSafeLibraries();

private:
    lua_State* state { nullptr };
};

}
