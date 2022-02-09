#pragma once

#include "../consecutive_red_regulator/consecutive_red_regulator.hpp"
#include "../../generic/error_type.hpp"
#include "item_inserter.h"

namespace btree::algorithm {

template<comparable_key key_t_>
template<typename... _key_args>
inline item_inserter<key_t_>::item_inserter(_key_args &&... args)
    : to_insert_(new rb_item<key_t_>(std::forward<_key_args>(args)...)) {
}

template<comparable_key key_t_>
item_inserter<key_t_>::~item_inserter() {
  delete to_insert_;
}

template<comparable_key key_t_>
inline item_inserter<key_t_>::item_inserter(item_inserter<key_t_> &&other) noexcept
    : to_insert_(other.to_insert_) {}

template<comparable_key key_t_>
inline void item_inserter<key_t_>::visit(rb_item<key_t_> *&root) {
  auto branch = insert_to_branch(root);
  regulate_red_color(std::move(branch));
  if (root->color() == 0)
    root->increase_color();
}

template<comparable_key key_t_>
inline branch_t<key_t_> item_inserter<key_t_>::insert_to_branch(rb_item<key_t_> *&root) {
  branch_t<key_t_> branch_stack;

  const key_t_& key = *to_insert_;
  branch_stack.push(root);

  while (branch_stack.top()) {
    auto& top = *branch_stack.top();
    const key_t_& top_key = top;
    if (key < top_key)
      branch_stack.emplace(top.get_child(side::left));
    else if (top_key < key)
      branch_stack.emplace(top.get_child(side::right));
    else
      throw item_already_exists("item already exists");
  }

  branch_stack.top().get() = to_insert_;
  to_insert_ = nullptr;

  return std::move(branch_stack);
}

template<comparable_key key_t_>
inline void item_inserter<key_t_>::regulate_red_color(branch_t<key_t_> branch) {
  consecutive_red_regulator<key_t_> regulator;
  while (!branch.empty() && regulator.regulate(branch.top()))
    branch.pop();
}

} // namespace btree::algorithm