/*
 * text-input.cc - Text input GUI component
 *
 * Copyright (C) 2015-2017  Wicked_Digger <wicked_digger@mail.ru>
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

#include "src/text-input.h"

TextInput::TextInput(unsigned int _width, unsigned int _height)
  : Control(_width, _height) {
  max_length = 0;
  filter = NULL;
  draw_focus = true;
  color_focus = Color(0x00, 0x8b, 0x47);
  color_text = Color::green;
  color_background = Color::black;
}

void
TextInput::set_text(const std::string &text) {
  this->text = text;
  invalidate();
}

std::string
TextInput::get_text() {
  return text;
}

void
TextInput::draw(Frame *frame, unsigned int x, unsigned int y) {
  frame->fill_rect(x, y, width, height, color_background);
  if (draw_focus && is_focused()) {
    frame->draw_rect(x, y, width, height, color_focus);
  }
  int ch_width = width/8;
  std::string str = text;
  int cx = 0;
  int cy = 0;
  if (draw_focus) {
    cx = 1;
    cy = 1;
  }
  while (str.length()) {
    std::string substr = str.substr(0, ch_width);
    str.erase(0, ch_width);
    frame->draw_string(x + cx, y + cy, substr, color_text);
    cy += 8;
  }
}

bool
TextInput::handle_click_left(int x, int y) {
  own_focus();
  return true;
}

bool
TextInput::handle_key_pressed(char key, int modifier) {
  if (!is_focused()) {
    return false;
  }

  if ((max_length != 0) && (text.length() >= max_length)) {
    return true;
  }

  if ((key == '\b') && (text.length() > 0)) {
    text = text.substr(0, text.length() - 1);
    invalidate();
    return true;
  }

  if (filter != NULL) {
    if (!filter(key, this)) {
      return true;
    }
  }

  text += key;

  invalidate();

  return true;
}

void
TextInput::set_max_length(unsigned int max_len) {
  max_length = max_len;
  if (max_length != 0) {
    if (text.length() > max_length) {
      text = text.substr(0, max_length);
    }
  }
}

