#include "lua/lua_runtime.hpp"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace vk::core {

LuaRuntime::LuaRuntime() {
    state = luaL_newstate();
    if (state != nullptr) { installSafeLibraries(); }
}

LuaRuntime::~LuaRuntime() {
    if (state != nullptr) {
        lua_close(state);
        state = nullptr;
    }
}

void LuaRuntime::installSafeLibraries() {
    if (state == nullptr) { return; }
    luaL_requiref(state, "_G", luaopen_base, 1);
    lua_pop(state, 1);
    luaL_requiref(state, "string", luaopen_string, 1);
    lua_pop(state, 1);
    luaL_requiref(state, "table", luaopen_table, 1);
    lua_pop(state, 1);
    luaL_requiref(state, "math", luaopen_math, 1);
    lua_pop(state, 1);
}

Result<std::string> LuaRuntime::execute(std::string_view source, std::string_view chunkName) {
    if (state == nullptr) { return fail(ErrorCode::InvalidArgument, "lua state not initialized"); }

    if (luaL_loadbufferx(state, source.data(), source.size(), std::string(chunkName).c_str(), "t") != LUA_OK) {
        std::string message = lua_tostring(state, -1);
        lua_pop(state, 1);
        return fail(ErrorCode::InvalidArgument, message);
    }
    if (lua_pcall(state, 0, 1, 0) != LUA_OK) {
        std::string message = lua_tostring(state, -1);
        lua_pop(state, 1);
        return fail(ErrorCode::Unknown, message);
    }

    std::string result;
    if (lua_isstring(state, -1) != 0) {
        result = lua_tostring(state, -1);
    }
    lua_pop(state, 1);
    return result;
}

}
