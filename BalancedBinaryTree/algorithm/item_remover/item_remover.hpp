#pragma once

#include "../dual_black_regulator/dual_black_regulator.hpp"
#include "../../generic/error_type.hpp"
#include "item_remover.h"

namespace btree::algorithm {

template <comparable_key key_t_>
inline item_remover<key_t_>::item_remover(const key_t_ &key_to_remove)
    : to_remove_(key_to_remove) {}

template <comparable_key key_t_>
inline void item_remover<key_t_>::visit(rb_item<key_t_> *&root) {
  /*
  1. stack the branch with removing item
  1.1 from top to the removing item.
  1.1 include extened items which needs min cut to extend the branch.

  2. if to be removed item has:
  2.1 no children, then make it color = 2 and remove it after graft.
  2.2 single child, replace it with the child value and sum the color.

  2. if any node has color > 1, then apply removal graft.
  */
  auto branch = find_removing_branch(root);
  rb_item<key_t_> *&top = branch.top();
  auto removed = remove(branch.top());
  // regulate color
  dual_black_regulator regulator(top, !removed);
  branch.pop();
  while (!branch.empty() && regulator.regulate(branch.top()))
    branch.pop();

  // make top black.
  if (root->color() == 0)
    root->increase_color();
  if (!removed)
    std::swap(removed, top);
  delete removed;
}

template <comparable_key key_t_>
inline branch_t<key_t_>
item_remover<key_t_>::find_removing_branch(rb_item<key_t_> *&root) {
  branch_t<key_t_> branch_stack;

  branch_stack.emplace(root);

  while (branch_stack.top()) {
    auto& top = *branch_stack.top();
    auto& top_key = (const key_t_&)top;
    if (to_remove_ < top_key)
      branch_stack.emplace(top.get_child(side::left));
    else if (top_key < to_remove_)
      branch_stack.emplace(top.get_child(side::right));
    else {
      append_right_min(branch_stack);
      return std::move(branch_stack);
    }
  }
  throw item_not_exists("can not find item with given key");
}

template <comparable_key key_t_>
inline void item_remover<key_t_>::append_right_min(branch_t<key_t_> &branch) {
  auto &to_remove = branch.top().get();
  if (both_children_available(*to_remove)) {
    rb_item<key_t_> **next = &to_remove->get_child(side::right);
    while (*next) {
      branch.emplace(*next);
      next = &(*next)->get_child(side::left);
    }
    auto& k =(key_t_ &) *to_remove;
    k = (key_t_&)*branch.top();
  }
}

template <comparable_key key_t_>
inline rb_item<key_t_> *item_remover<key_t_>::remove(rb_item<key_t_> *&to_remove) {
  rb_item<key_t_> *rid = nullptr;
  auto right_child = get_single_child(*to_remove);
  if (right_child) {
    if (to_remove->color() > 0)
      to_remove->push_down_color();
    rid = to_remove;
    to_remove = right_child;
  }
  return rid;
}

template <comparable_key key_t_>
inline bool item_remover<key_t_>::both_children_available(rb_item<key_t_> &parent) {
  return parent.get_child(side::left) && parent.get_child(side::right);
}
template<comparable_key key_t_>
rb_item<key_t_> *item_remover<key_t_>::get_single_child(rb_item<key_t_> &parent) {
  auto left_child = parent.get_child(side::left);
  return left_child ? left_child : parent.get_child(side::right);
}

} // namespace btree::algorithm