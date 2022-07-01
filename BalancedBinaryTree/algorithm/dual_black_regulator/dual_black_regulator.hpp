#pragma once

#include "dual_black_regulator.h"
#include <forward_list>

namespace btree::algorithm {

template<comparable_key key_t_>
inline btree::algorithm::dual_black_regulator<key_t_>::dual_black_regulator(
    rb_item<key_t_> *dual_black_item, bool force)
    : dual_black_item_(dual_black_item) {
  if (force && dual_black_item_->color() == 1)
    dual_black_item_->increase_color();
}

template<comparable_key key_t_>
inline bool dual_black_regulator<key_t_>::regulate(rb_item<key_t_> *&parent) {
  if (dual_black_item_->color() < 2)
    return false;

  branch_t<key_t_> parents;
  parents.emplace(parent);

  auto x_side = get_side(parent, dual_black_item_);
  auto sibling_side = opposite(x_side);
  auto sibling = parent->get_child(sibling_side);
  // case 1
  if (sibling && !sibling->color()) {
    rotate(parent, sibling_side);
    parents.emplace(parent->get_child(x_side));
  }
  // case 2,3,4
  while (!parents.empty()
      && rotate_sibling(parents.top(), x_side))
    parents.pop();

  return true;
}

template<comparable_key key_t_>
inline side dual_black_regulator<key_t_>::get_side(rb_item<key_t_> *parent,
                                                rb_item<key_t_> *child) {
  auto left_child = parent->get_child(side::left);
  return left_child == child ? side::left : side::right;
}

template<comparable_key key_t_>
inline bool dual_black_regulator<key_t_>::rotate_sibling(rb_item<key_t_> *&parent,
                                                      side x_side) {
  // suppose sibling is black.
  auto sibling_side = opposite(x_side);
  auto &sibling = parent->get_child(sibling_side);

  auto x_side_black = is_black(sibling->get_child(x_side));
  auto w_side_black = is_black(sibling->get_child(sibling_side));

  auto upgrade = x_side_black && w_side_black;
  dual_black_item_ = parent;
  if (!upgrade) {
    // w side is black, so x_side is red,
    // now make w side nephew red.
    if (w_side_black)
      rotate(sibling, x_side);
    sibling->push_down_color();
    rotate(parent, sibling_side);
  }
  dual_black_item_->pull_up_color();
  return dual_black_item_->color() > 1;
}

template<comparable_key key_t_>
inline bool dual_black_regulator<key_t_>::is_black(rb_item<key_t_> *item) {
  return !item || item->color();
}

} // namespace btree::algorithm