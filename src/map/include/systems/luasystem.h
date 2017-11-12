#pragma once

#include "system.h"
#include "throwassert.h"
#include "function_traits.h"

#include <sol.hpp>

#include <vector>
#include <algorithm>

namespace Systems {

class LuaSystem : public System {
    public:
        enum class luaFunctions {
            onInit,
            onCreate,
            onRemove,
            onEquip,
            onUnEquip,
            onDrop,
            onPickup,
            onUse
        };
    
        LuaSystem(SystemManager &manager) : System(manager) {}
        virtual ~LuaSystem() = default;

        void loadScript(Entity e, const std::string& luaScript) {
          auto lua = e.component<Lua>();
          throw_assert(lua, "The entity doesn't have a lua table");
          loadScript(*lua, luaScript);
        }
    
        void loadScript(Lua& luaEnv, const std::string& luaScript) {
            if (!luaEnv.env_)
                luaEnv.env_ = std::make_unique<sol::environment>(state_, sol::create);
            luaEnv.env_->set_function("testCpp",
                                [this](std::string data) {
                                    logger_->warn("test lua: {}", data);
                              });
          state_.script(luaScript, *luaEnv.env_);
        }

        template <luaFunctions func, typename... Args>
        void callLuaFunction(Entity e, Args... args) {
          auto lua = e.component<Lua>();
          throw_assert(lua && lua->env_, "The entity doesn't have a lua table");
          callLuaFunction<func>(*lua, args...);
        }
    
        template <luaFunctions func, typename... Args>
        auto callLuaFunction(Lua& luaEnv, Args... args);
    
    virtual void update(EntityManager&, double) {}

        /*void unregisterEntity(Entity e) {
          auto it = std::find(callbacks_.begin(), callbacks_.end(), Callback{e});
            if (it != callbacks_.end())
              callbacks_.erase(it);
        }

        virtual void update(EntityManager&, double dt) {
            for (auto &it : callbacks_) {
                it.dt += dt;
                if (it.dt >= it.timeout) {
                    it.dt = 0.f;
                }
            }
        }*/

    private:
        sol::state state_;

        /*struct Callback {
            Entity e;
            std::string name{};
            double timeout = 0;
            double dt = 0;

            bool operator==(const Callback& c) const { return e == c.e; }
        };
        std::vector<Callback> callbacks_;*/
            

};


namespace {
template <LuaSystem::luaFunctions> struct LuaFunction {};
template <> struct LuaFunction<LuaSystem::luaFunctions::onInit> { static constexpr char name[] = "onInit"; using type = void(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onCreate> { static constexpr char name[] = "onCreate"; using type = void(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onRemove> { static constexpr char name[] = "onRemove"; using type = void(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onEquip> { static constexpr char name[] = "onEquip"; using type = bool(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onUnEquip> { static constexpr char name[] = "onUnEquip"; using type = bool(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onDrop> { static constexpr char name[] = "onDrop"; using type = void(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onPickup> { static constexpr char name[] = "onPickup"; using type = void(); };
template <> struct LuaFunction<LuaSystem::luaFunctions::onUse> { static constexpr char name[] = "onUse"; using type = void(); };
}

template <LuaSystem::luaFunctions func, typename... Args>
auto LuaSystem::callLuaFunction(Lua& luaEnv, Args... args) {
    auto& env = *luaEnv.env_;
    using Func = LuaFunction<func>;
    sol::function f = env[Func::name];
    using FuncType = function_traits<typename Func::type>;
    static_assert(std::is_same<typename FuncType::arguments, std::tuple<Args...>>::value, "Incorrect parameters for the lua function");
    return static_cast<typename FuncType::return_type>(f(args...));
}
}
