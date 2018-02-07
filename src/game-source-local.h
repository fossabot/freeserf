/*
 * game-source-local.h - Saved games source
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

#ifndef SRC_GAME_SOURCE_LOCAL_H_
#define SRC_GAME_SOURCE_LOCAL_H_

#include <vector>
#include <string>
#include <memory>

#include "src/game-manager.h"
#include "src/random.h"

class PlayerInfoLocal : public PlayerInfoBase {
 public:
  explicit PlayerInfoLocal(Random *random_base);
  PlayerInfoLocal(size_t character, const PlayerInfo::Color &_color,
                  unsigned int _intelligence, unsigned int _supplies,
                  unsigned int _reproduction);

  virtual bool has_castle() const;

  void set_intelligence(unsigned int _intelligence) {
    intelligence = _intelligence; }
  void set_supplies(unsigned int _supplies) { supplies = _supplies; }
  void set_reproduction(unsigned int _reproduction) {
    reproduction = _reproduction; }
  void set_castle_pos(PlayerInfo::BuildingPos _castle_pos);
  void set_character(size_t character);
  void set_color(const PlayerInfo::Color &_color) { color = _color; }
};

typedef std::shared_ptr<PlayerInfoLocal> PPlayerInfoLocal;

class GameInfoLocal : public GameInfo {
 protected:
  unsigned int map_size;
  Random random_base;
  std::vector<PPlayerInfoLocal> players;
  std::string name;
  std::string path;

 public:
  explicit GameInfoLocal(const Random &random_base);

  virtual std::string get_name() const { return name; }
  virtual std::string get_path() const { return path; }
  virtual unsigned int get_map_size() const { return map_size; }
  void set_map_size(unsigned int size) { map_size = size; }
  virtual Random get_random_base() const { return random_base; }
  void set_random_base(const Random &base);
  virtual size_t get_player_count() const { return players.size(); }
  virtual PPlayerInfo get_player(size_t player) const {
    return players[player];
  }

  void add_player(const PPlayerInfoLocal &player);
  void add_player(size_t character, const PlayerInfo::Color &_color,
                  unsigned int _intelligence, unsigned int _supplies,
                  unsigned int _reproduction);
  void remove_all_players();
};

typedef std::shared_ptr<GameInfoLocal> PGameInfoLocal;

class GameSourceLocal : public GameSource {
 protected:
  PGameInfoLocal game_info;

 public:
  GameSourceLocal();
  virtual ~GameSourceLocal() {}

  virtual std::string get_name() const { return "local"; }
  virtual size_t count() const { return 1; }
  virtual PGameInfo game_info_at(const size_t &pos) const;
  virtual PGameInfo create_game(const std::string &path) const;
  virtual bool publicate(PGame game);
};

#endif  // SRC_GAME_SOURCE_LOCAL_H_
