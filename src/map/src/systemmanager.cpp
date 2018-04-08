#include "systemmanager.h"
#include "systems/system.h"
#include "entitysystem.h"

void SystemManager::update(double dt) {
    auto &entityManager = entitySystem_.getEntityManager();
    for (auto &it : systems_)
        it.second->update(entityManager, dt);
}

Entity SystemManager::getEntity(const std::string &name) const {
    return entitySystem_.getEntity(name);
}

Entity SystemManager::getEntity(uint32_t charId) const {
    return entitySystem_.getEntity(charId);
}

EntityManager &SystemManager::getEntityManager() {
    return entitySystem_.getEntityManager();
}

Entity SystemManager::buildItem(Entity creator, RoseCommon::Item&& item) {
    return entitySystem_.buildItemEntity(creator, std::move(item));
}
