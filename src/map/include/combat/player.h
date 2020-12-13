#pragma once

#include "cli_stat_add_req.h"
#include "cli_toggle_move.h"
#include "dataconsts.h"
#include <optional>


class EntitySystem;

namespace Player {
void add_stat(EntitySystem&, RoseCommon::Entity, const RoseCommon::Packet::CliStatAddReq&);
void toggle_player_move(EntitySystem&, RoseCommon::Entity, const RoseCommon::Packet::CliToggleMove&);
}