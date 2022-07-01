#pragma once

#include "../generic/error_type.hpp"
#include "rb_item.h"
#include "../generic/resource_manager/resource_manager.hpp"

namespace btree::item {

inline side opposite(side which) {
  return which == side::left ? side::right : side::left;
}

template<comparable_key key_t_>
template<typename... _key_args>
rb_item<key_t_>::rb_item(_key_args &&... args)
    : key_(std::forward<_key_args>(args)...) {
}

template<comparable_key key_t_>
inline void *rb_item<key_t_>::operator new(std::size_t size) {
  return resource_manager::allocate(size);
}

template<comparable_key key_t_>
inline void rb_item<key_t_>::operator delete(void *p) {
  resource_manager::deallocate(p, sizeof(rb_item<key_t_>));
}

template<comparable_key key_t_> inline rb_item<key_t_>::operator key_t_ &() {
  return key_;
}

template<comparable_key key_t_> inline std::uint8_t rb_item<key_t_>::color() const {
  return color_;
}

template<comparable_key key_t_> inline void rb_item<key_t_>::increase_color() {
  ++color_;
}

template<comparable_key key_t_> inline void rb_item<key_t_>::pull_up_color() {

  pull_up_color(left_);
  pull_up_color(right_);

  ++color_;
}

template<comparable_key key_t_> inline void rb_item<key_t_>::push_down_color() {
  if (color_ == 0)
    throw color_error("no color can be pushed down");

  push_down_color(left_);
  push_down_color(right_);
  --color_;
}

template<comparable_key key_t_>
inline rb_item<key_t_> *&rb_item<key_t_>::get_child(side which) {
  return which == side::left ? left_ : right_;
}

template<comparable_key key_t_> inline bool rb_item<key_t_>::decrease_color() {
  bool to_reduce = color_ > 0;
  if (to_reduce)
    --color_;
  return to_reduce;
}

template<comparable_key key_t_>
inline void rb_item<key_t_>::push_down_color(rb_item *child) {
  if (child)
    child->increase_color();
}

template<comparable_key key_t_>
inline void rb_item<key_t_>::pull_up_color(rb_item *child) {
  if (child && !child->decrease_color())
    throw color_error("no color can be pulled up");
}

template<comparable_key key_t_> void rotate(rb_item<key_t_> *&item, side which) {
  /*
 1. rotate only when one of child is red
 2. parent child color also switch.
 */

  auto &child = item->get_child(which);
  if (!child || child->color() > 0)
    throw generic::rotate_error("child color must be red");

  if (item->decrease_color())
    child->increase_color();

  auto &grand_child = child->get_child(opposite(which));
  auto temp = child;
  child = grand_child;
  grand_child = item;
  item = temp;
}

/*
 * atomic rotate
 *
 *template<typename k_t_>
void rotate(typename rb_item<k_t_>::atomic_shared_rb_item_t &item, side which) {
  auto parent = item.load(std::memory_order_relaxed);
  auto child = parent->get_child(which).load(std::memory_order_relaxed);
  if (!child || child->color() > 0)
    throw generic::rotate_error("child color must be red");

  auto parent_clone = std::make_shared<rb_item<k_t_>>((k_t_ &) *parent);
  auto child_clone = std::make_shared<rb_item<k_t_>>((k_t_ &) *child);

  if (parent->decrease_color())
    child_clone->increase_color();

  auto another = opposite(which);
  parent_clone->get_child(which).load(child->get_child(another), std::memory_order_relaxed);
  parent_clone->get_child(another).load(parent->get_child(another), std::memory_order_relaxed);
  child_clone->get_child(which).load(child->get_child(which), std::memory_order_relaxed);
  child_clone->get_child(another).load(parent_clone, std::memory_order_relaxed);

  item.store(parent_clone, std::memory_order_release);
}
 */
} // namespace btree::item
