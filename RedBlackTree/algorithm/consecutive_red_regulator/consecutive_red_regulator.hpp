#pragma once

#include "consecutive_red_regulator.h"

using namespace btree::item;

namespace btree::algorithm {

template <comparable_key key_t_>
inline consecutive_red_regulator<key_t_>::consecutive_red_regulator()
    : chain_{nullptr, nullptr}, count_(0U) {}

template <comparable_key key_t_>
inline bool consecutive_red_regulator<key_t_>::regulate(rb_item<key_t_> *&item) {
  if (item->color() == 0) {
    if (count_ == sizeof(chain_))
      throw std::runtime_error("more than 2 consecutive red nodes");
    chain_[count_++] = &item;
  } else {
    auto two_red = count_ == 2;
    count_ = 0U;
    if (two_red) {
      auto red_child_side = get_child_side(item, *chain_[1]);
      auto sibling = item->get_child(opposite(red_child_side));
      if (sibling && sibling->color() == 0)
        item->push_down_color();
      else {
        auto red_grand_side = get_child_side(*chain_[1], *chain_[0]);
        if (red_child_side != red_grand_side)
          // rotate to make them same side.
          rotate(*chain_[1], red_grand_side);

        rotate(item, red_child_side);
      }
      chain_[count_++] = &item;
    } else
      return false;
  }
  return true;
}

template <comparable_key key_t_>
inline side
consecutive_red_regulator<key_t_>::get_child_side(rb_item<key_t_> *parent,
                                               rb_item<key_t_> *child) {
  return parent->get_child(side::left) == child ? side::left : side::right;
}

} // namespace btree::algorithm