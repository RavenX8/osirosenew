#include "script_loader.h"

using namespace LuaScript;

ScriptLoader::File::File(std::string const& path) : path_(path) {
    //TODO: find filename
    //prob split on '/' and take latest, then split on '.' and take everything but the last one if there's more than one
}

ScriptLoader::ScriptLoader(std::shared_ptr<EntitySystem> entity_system, uint16_t map_id, std::string const& path):
entity_system_(entity_system),
logger_(Core::CLog::GetLogger(Core::log_type::SCRIPTLOADER).lock()),
path_(path), map_id_(map_id) {
    state_.open_libraries(); //FIXME: check if we need all libs

    state_.set_function("include", [this](std::string path) {
        load_script(path);
    });
}

void ScriptLoader::load_script() {
    reload_scripts(path_);
}

void ScriptLoader::load_npcs() {
    for (auto& [file, entities] : npc_files_) {
        entity_system_->bulk_destroy(entities);
        entities.clear();
        load_script(file.path_);
    }
}

void ScriptLoader::load_warpgates() {
    for (auto& [file, entities] : warpgate_files_) {
        entity_system_->bulk_destroy(entities);
        entities.clear();
        load_script(file.path_);
    }
}

void ScriptLoader::load_spawners() {
    for (auto& [file, entities] : spawner_files_) {
        entity_system_->bulk_destroy(entities);
        entities.clear();
        load_script(file.path_);
    }
}

void ScriptLoader::load_script(std::string const& path) {
    try {
        sol::environment env{state_, sol::create, state_.globals()};
        
        auto [warpgate_file, ok] = warpgate_files_.insert(File{path}, {});
        env.set_function("warp_gate", [warpgate_file](std::string alias, int dest_map_id, float dest_x, float dest_y, float dest_z, int map_id, float x, float y, float z, float angle, float x_scale, float y_scale, float z_scale) {
            wargate_file->second.push_back(entity_system_->create_warpgate(alias, dest_map_id, dest_x, dest_y, dest_z, map_id, x, y, z, angle, x_scale, y_scale, z_scale));
        });
        
        auto [npc_file, ok] = npc_files_.insert(File{path}, {});
        env.set_function("npc", [npc_file](std::string npc_lua, int npc_id, int map_id, float x, float y, float z, float angle) {
            npc_file->second.push_back(entity_system_->create_npc(npc_lua, npc_id, map_id, x, y, z, angle));
        });
        
        auto [spawner_file, ok] = spawner_files_.insert(File{path}, {});
        env.set_function("mob", [spawner_file](std::string alias, int mob_id, int mob_count, int limit, int interval, int range, int map_id, float x, float y, float z) {
            spawner_file->second.push_back(entity_system->create_spawner(alias, mob_id, mob_count, limit, interval, range, map_id, x, y, z));
        });
        
        state_.script(path, env);
        logger_->info("Finished (re)loading scripts from '{}'", path);
    } catch (const sol::error& e) {
        logger_->error("Error (re)loading lua scripts '{}' : {}", path, e.what());
    }
}
