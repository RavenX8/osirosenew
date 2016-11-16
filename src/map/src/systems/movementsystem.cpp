#include "systems/movementsystem.h"
#include "cmapserver.h"
#include "packetfactory.h"
#include <cmath>

using namespace Systems;
using namespace RoseCommon;

MovementSystem::MovementSystem(SystemManager &manager) {
    manager.registerDispatcher(ePacketType::PAKCS_MOUSE_CMD, &MovementSystem::processMove);
    manager.registerDispatcher(ePacketType::PAKCS_STOP_MOVING, &MovementSystem::stopMoving);
}

void MovementSystem::update(EntityManager &es, double dt) {
    Component<Position> position;
    Component<Destination> destination;
    Component<AdvancedInfo> advanced;
    for (Entity entity : es.entities_with_components(position, destination, advanced)) {
        float dx = destination->x_ - position->x_;
        float dy = destination->y_ - position->y_;
        float distance = std::sqrt(dx * dx + dy * dy);
        destination->dist_ = distance;
        float speed = advanced->runSpeed_;
        float ntime = distance / speed;
        if (ntime <= dt || distance == 0) {
            position->x_ = destination->x_;
            position->y_ = destination->y_;
            entity.remove<Destination>();
        } else {
            position->x_ += dx * dt / ntime;
            position->y_ += dy * dt / ntime;
        }
    }
}

void MovementSystem::move(Entity entity, float x, float y) {
    if (!entity)
        return;
    auto dest = entity.component<Destination>();
    if (dest) {
        dest->x_ = x;
        dest->y_ = y;
    } else {
        entity.assign<Destination>(x, y, 0);
        dest = entity.component<Destination>();
    }
    auto pos = entity.component<Position>();
    float dx = pos->x_ - dest->x_;
    float dy = pos->y_ - dest->y_;
    dest->dist_ = std::sqrt(dx * dx + dy * dy);
}

void MovementSystem::stop(Entity entity, float x, float y) {
    if (!entity || !entity.component<BasicInfo>())
        return;
    entity.remove<Destination>();
    auto position = entity.component<Position>();
    if (position) {
        float dx = position->x_ - x;
        float dy = position->y_ - y;
        if (dx * dx + dy * dy < POSITION_CHEATING) {
            position->x_ = x;
            position->y_ = y;
        } else
            logger_->warn("Player {} attempted to cheat his position : calculated position : ({}, {}), got ({}, {})",
                    entity.component<BasicInfo>()->id_, position->x_, position->y_, x, y);
    }
}

void MovementSystem::processMove(EntityManager&, CMapClient *client, Entity entity, const CliMouseCmd &packet) {
    logger_->trace("MovementSystem::processMove");
    if (!client || !entity.component<Position>() || !entity.component<BasicInfo>())
        return;
    move(entity, packet.x(), packet.y());
    CMapServer::SendPacket(client, CMapServer::eSendType::EVERYONE,
            *makePacket<ePacketType::PAKWC_MOUSE_CMD>(entity));
}

void MovementSystem::stopMoving(EntityManager&, CMapClient *client, Entity entity, const CliStopMoving &packet) {
    logger_->trace("MovementSystem::stopMoving");
    if (!client)
        return;
    stop(entity, packet.x(), packet.y());
    CMapServer::SendPacket(client, CMapServer::eSendType::EVERYONE,
            *makePacket<ePacketType::PAKWC_STOP>(entity));
}
