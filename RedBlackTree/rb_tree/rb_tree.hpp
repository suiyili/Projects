#include "rb_tree.h"

namespace btree {

template <comparable_key key_t_> inline rb_tree<key_t_>::rb_tree() : root_(nullptr) {}

template <comparable_key key_t_> inline rb_tree<key_t_>::~rb_tree() {
  clear(root_);
  return;
}

template <comparable_key key_t_>
inline rb_tree<key_t_>::rb_tree(rb_tree<key_t_> &&other) : root_(other.root_) {
  other.root_ = nullptr;
}

template <comparable_key key_t_>
inline bool rb_tree<key_t_>::contains(const key_t_ &key) {
  auto cur = root_;
  while (cur != nullptr) {
    const key_t_& cur_key = *cur;
    if (key < cur_key)
      cur = cur->get_child(side::left);
    else if (cur_key < key)
      cur = cur->get_child(side::right);
    else
      return true;
  }
  return false;
}

template <comparable_key key_t_>
inline void rb_tree<key_t_>::accept(tree_visitor_i<key_t_> &visitor) {
  visitor.visit(root_);
}

template <comparable_key key_t_>
inline void rb_tree<key_t_>::clear(rb_item<key_t_> *to_clear) {
  if (to_clear) {
    clear(to_clear->get_child(side::left));
    clear(to_clear->get_child(side::right));
    delete to_clear;
  }
}

} // namespace btree