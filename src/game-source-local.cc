/*
 * game-source-local.cc - Saved games source
 *
 * Copyright (C) 2018  Wicked_Digger <wicked_digger@mail.ru>
 *
 * This file is part of freeserf.
 *
 * freeserf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freeserf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with freeserf.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "src/game-source-local.h"
#include "src/player.h"
#include "src/characters.h"

PlayerInfo::Color def_color[4] = {
  {0x00, 0xe3, 0xe3},
  {0xcf, 0x63, 0x63},
  {0xdf, 0x7f, 0xef},
  {0xef, 0xef, 0x8f}
};

GameInfoLocal::GameInfoLocal(const Random &_random_base) {
  map_size = 3;
  name = _random_base;
  set_random_base(_random_base);
}

void
GameInfoLocal::set_random_base(const Random &base) {
  Random random = base;
  random_base = base;

  players.clear();

  // Player 0
  players.push_back(std::make_shared<PlayerInfoLocal>(&random));
  players[0]->set_character(12);
  players[0]->set_intelligence(40);

  // Player 1
  players.push_back(std::make_shared<PlayerInfoLocal>(&random));

  uint32_t val = random.random();
  if ((val & 7) != 0) {
    // Player 2
    players.push_back(std::make_shared<PlayerInfoLocal>(&random));
    uint32_t val = random.random();
    if ((val & 3) == 0) {
      // Player 3
      players.push_back(std::make_shared<PlayerInfoLocal>(&random));
    }
  }

  int i = 0;
  for (PPlayerInfoLocal info : players) {
    info->set_color(def_color[i++]);
  }
}

void
GameInfoLocal::add_player(const PPlayerInfoLocal &player) {
  players.push_back(player);
}

void
GameInfoLocal::add_player(size_t character, const PlayerInfo::Color &_color,
                          unsigned int _intelligence, unsigned int _supplies,
                          unsigned int _reproduction) {
  PPlayerInfoLocal player = std::make_shared<PlayerInfoLocal>(character,
                                                              _color,
                                                              _intelligence,
                                                              _supplies,
                                                              _reproduction);
  add_player(player);
}

void
GameInfoLocal::remove_all_players() {
  players.clear();
}

PlayerInfoLocal::PlayerInfoLocal(Random *random_base) {
  size_t character = (((random_base->random() * 10) >> 16) + 1) & 0xFF;
  set_character(character);
  set_intelligence(((random_base->random() * 41) >> 16) & 0xFF);
  set_supplies(((random_base->random() * 41) >> 16) & 0xFF);
  set_reproduction(((random_base->random() * 41) >> 16) & 0xFF);
  set_castle_pos({-1, -1});
}

PlayerInfoLocal::PlayerInfoLocal(size_t character,
                                 const PlayerInfo::Color &_color,
                                 unsigned int _intelligence,
                                 unsigned int _supplies,
                                 unsigned int _reproduction) {
  set_character(character);
  set_intelligence(_intelligence);
  set_supplies(_supplies);
  set_reproduction(_reproduction);
  set_color(_color);
  set_castle_pos({-1, -1});
}

void
PlayerInfoLocal::set_castle_pos(PlayerInfo::BuildingPos _castle_pos) {
  castle_pos = _castle_pos;
}

void
PlayerInfoLocal::set_character(size_t index) {
  Character &character = Characters::characters.get_character(index);
  face = character.get_face();
  name = character.get_name();
  characterization = character.get_characterization();
}

bool
PlayerInfoLocal::has_castle() const {
  return (castle_pos.col >= 0);
}

// GameSourceLocal

GameSourceLocal::GameSourceLocal() {
}

PGameInfo
GameSourceLocal::game_info_at(const size_t &pos) const {
  if (pos >= count()) {
    return nullptr;
  }

  return game_info;
}

PGameInfo
GameSourceLocal::create_game(const std::string &path) const {
  return nullptr;
}

bool
GameSourceLocal::publicate(PGame game) {
  return false;
}
