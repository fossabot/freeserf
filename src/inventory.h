/*
 * inventory.h - Resources related definitions.
 *
 * Copyright (C) 2015  Wicked_Digger <wicked_digger@mail.ru>
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

#ifndef SRC_INVENTORY_H_
#define SRC_INVENTORY_H_

#include "src/resource.h"
#include "src/serf.h"
#include "src/objects.h"

class Flag;
typedef std::shared_ptr<Flag> PFlag;
typedef std::weak_ptr<Flag> WFlag;
class SaveReaderBinary;
class SaveReaderText;
class SaveWriterText;
class Player;
typedef std::shared_ptr<Player> PPlayer;
typedef std::weak_ptr<Player> WPlayer;
class Building;
typedef std::shared_ptr<Building> PBuilding;
typedef std::weak_ptr<Building> WBuilding;

class Inventory : public GameObject, public std::enable_shared_from_this<Inventory> {
 public:
  typedef enum Mode {
    ModeIn = 0,    // 00
    ModeStop = 1,  // 01
    ModeOut = 3,   // 11
  } Mode;

 protected:
  WPlayer owner;           // Player owning this inventory
  WFlag flag;              // Flag connected to this inventory
  WBuilding building;      // Building containing this inventory
  ResourceMap resources;   // Count of resources
  struct out_queue {       // Resources waiting to be moved out
    Resource::Type type;
    unsigned int dest;
  } out_queue[2];
  unsigned int serfs_out;  // Count of serfs waiting to move out
  int generic_count;       // Count of generic serfs
  int res_dir;
  Serf::SerfMap serfs;     // Indices to serfs of each type

 public:
  Inventory(PGame game, unsigned int index);
  virtual ~Inventory();

  PPlayer get_owner() { return owner.lock(); }
  void set_owner(PPlayer owner_) { owner = owner_; }

  PFlag get_flag() { return flag.lock(); }
  void set_flag(PFlag flag_) { flag = flag_; }

  PBuilding get_building() { return building.lock(); }
  void set_building(PBuilding building_) { building = building_; }

  Inventory::Mode get_res_mode() { return (Inventory::Mode)(res_dir & 3); }
  void set_res_mode(Inventory::Mode mode) { res_dir = (res_dir & 0xFC) | mode; }
  Inventory::Mode get_serf_mode() {
    return (Inventory::Mode)((res_dir >> 2) & 3); }
  void set_serf_mode(Inventory::Mode mode) {
    res_dir = (res_dir & 0xF3) | (mode << 2); }
  bool have_any_out_mode() { return ((res_dir & 0x0A) != 0); }

  int get_serf_queue_length() { return serfs_out; }
  void serf_away() { serfs_out--; }
  bool call_out_serf(PSerf serf);
  PSerf call_out_serf(Serf::Type type);
  bool call_internal(PSerf serf);
  PSerf call_internal(Serf::Type type);
  void serf_come_back() { generic_count++; }
  size_t free_serf_count() { return generic_count; }
  bool have_serf(Serf::Type type) { return (serfs[type] != 0); }

  unsigned int get_count_of(Resource::Type resource) {
    return resources[resource]; }
  ResourceMap get_all_resources() { return resources; }
  void pop_resource(Resource::Type resource) { resources[resource]--; }
  void push_resource(Resource::Type resource);

  bool has_resource_in_queue() {
    return (out_queue[0].type != Resource::TypeNone); }
  bool is_queue_full() { return (out_queue[1].type != Resource::TypeNone); }
  void get_resource_from_queue(Resource::Type *res, int *dest);
  void reset_queue_for_dest(PFlag flag);

  bool has_food() { return (resources[Resource::TypeFish] != 0 ||
                            resources[Resource::TypeMeat] != 0 ||
                            resources[Resource::TypeBread] != 0); }

  /* Take resource from inventory and put in out queue.
   The resource must be present.*/
  void add_to_queue(Resource::Type type, unsigned int dest);

  /* Create initial resources */
  void apply_supplies_preset(unsigned int supplies);

  PSerf call_transporter(bool water);

  bool promote_serf_to_knight(PSerf serf);

  PSerf spawn_serf_generic();
  bool specialize_serf(PSerf serf, Serf::Type type);
  PSerf specialize_free_serf(Serf::Type type);
  unsigned int serf_potential_count(Serf::Type type);

  void serf_idle_in_stock(PSerf serf);
  void knight_training(PSerf serf, int p);

  friend SaveReaderBinary&
    operator >> (SaveReaderBinary &reader, Inventory &inventory);
  friend SaveReaderText&
    operator >> (SaveReaderText &reader, Inventory &inventory);
  friend SaveWriterText&
    operator << (SaveWriterText &writer, Inventory &inventory);
};

typedef std::shared_ptr<Inventory> PInventory;

#endif  // SRC_INVENTORY_H_
