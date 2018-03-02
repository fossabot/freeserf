/*
 * objects.h - Game objects collection template
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

#ifndef SRC_OBJECTS_H_
#define SRC_OBJECTS_H_

#include <map>
#include <algorithm>
#include <set>
#include <memory>
#include <limits>
#include <utility>

class Game;
typedef std::shared_ptr<Game> PGame;
typedef std::weak_ptr<Game> WGame;

class GameObject {
 private:
  unsigned int index;
  WGame game;

 public:
  GameObject() = delete;
  GameObject(PGame game, unsigned int index) : index(index), game(game) {}
  GameObject(const GameObject& that) = delete;  // Copying prohibited
  GameObject(GameObject&& that) = delete;  // Moving prohibited
  virtual ~GameObject() {}

  GameObject& operator = (const GameObject& that) = delete;
  GameObject& operator = (GameObject&& that) = delete;

  PGame get_game() const { return game.lock(); }
  unsigned int get_index() const { return index; }
};

template<class T>
class Collection {
 protected:
  typedef std::shared_ptr<T> PObject;
  typedef std::map<unsigned int, PObject> Objects;
  typedef typename Objects::iterator ObjectsIt;

  Objects objects;
  unsigned int last_object_index;
  std::set<unsigned int> free_object_indexes;
  WGame game;

 public:
  Collection() : last_object_index(0) {
  }

  explicit Collection(PGame _game) : last_object_index(0), game(_game) {
  }

  virtual ~Collection() {
    clear();
  }

  void clear() {
    objects.clear();
  }

  PObject
  allocate() {
    unsigned int new_index = 0;

    if (!free_object_indexes.empty()) {
      new_index = *free_object_indexes.begin();
      free_object_indexes.erase(free_object_indexes.begin());
    } else {
      if (last_object_index ==
          std::numeric_limits<decltype(last_object_index)>::max()) {
        return nullptr;
      }

      new_index = last_object_index;
      last_object_index++;
    }

    PGame g = game.lock();
    ObjectsIt it = objects.emplace(std::piecewise_construct,
                                   std::forward_as_tuple(new_index),
                                   std::forward_as_tuple(std::make_shared<T>(g,
                                                         new_index))).first;

    return it->second;
  }

  bool
  exists(unsigned int index) const {
    return (objects.end() != objects.find(index));
  }

  PObject
  get_or_insert(unsigned int index) {
    if (!exists(index)) {
      objects.emplace(std::piecewise_construct,
                      std::forward_as_tuple(index),
                      std::forward_as_tuple(std::make_shared<T>(game.lock(),
                                                                index)));

      std::set<unsigned int>::iterator i = free_object_indexes.find(index);
      if (i != free_object_indexes.end()) {
        free_object_indexes.erase(i);
      }
    }

    if (last_object_index <= index) {
      for (unsigned int i = last_object_index; i < index; i++) {
        free_object_indexes.insert(i);
      }
      last_object_index = index + 1;
    }

    return objects.at(index);
  }

  PObject
  operator[] (unsigned int index) {
    if (!exists(index)) return nullptr;
    return objects.at(index);
  }

  const PObject
  operator[] (unsigned int index) const {
    if (!exists(index)) return nullptr;
    return objects.at(index);
  }

  class Iterator {
   protected:
    typename Objects::iterator internal_iterator;

   public:
    explicit Iterator(typename Objects::iterator internal_iterator) {
      this->internal_iterator = internal_iterator;
    }

    Iterator&
    operator++() {
      internal_iterator++;
      return (*this);
    }

    bool
    operator==(const Iterator& right) const {
      return (internal_iterator == right.internal_iterator);
    }

    bool
    operator!=(const Iterator& right) const {
      return (!(*this == right));
    }

    PObject
    operator*() const {
      return internal_iterator->second;
    }
  };

  class ConstIterator {
   protected:
    typename Objects::const_iterator internal_iterator;

   public:
    explicit ConstIterator(typename Objects::const_iterator it) {
     this->internal_iterator = it;
    }

    ConstIterator& operator++() {
     internal_iterator++;
     return (*this);
    }

    bool operator == (const ConstIterator& right) const {
     return internal_iterator == right.internal_iterator;
    }

    bool operator != (const ConstIterator& right) const {
     return !(*this == right);
    }

    const PObject operator*() const {
     return internal_iterator->second;
    }
  };

  Iterator begin() { return Iterator(objects.begin()); }
  Iterator end() { return Iterator(objects.end()); }

  ConstIterator begin() const { return ConstIterator(objects.begin()); }
  ConstIterator end() const { return ConstIterator(objects.end()); }

  void
  erase(unsigned int index) {
    /* Decrement max_flag_index as much as possible. */
    typename Objects::iterator i = objects.find(index);
    if (i == objects.end()) {
      return;
    }
    objects.erase(i);

    if (index == last_object_index) {
      last_object_index--;
      // ToDo: remove all empty indexes if needed
    } else {
      free_object_indexes.insert(index);
    }
  }

  size_t
  size() const { return objects.size(); }
};

#endif  // SRC_OBJECTS_H_
