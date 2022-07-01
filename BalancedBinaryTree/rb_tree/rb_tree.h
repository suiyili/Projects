#pragma once

#include "tree_visitor_i.hpp"

namespace btree {

template <comparable_key key_t_> class rb_tree final {
public:
  rb_tree();
  ~rb_tree();

  rb_tree(const rb_tree<key_t_> &) = delete;
  rb_tree(rb_tree<key_t_> &&other);

  rb_tree<key_t_> &operator=(const rb_tree<key_t_> &) = delete;

  bool contains(const key_t_& key);

  void accept(tree_visitor_i<key_t_> &visitor);

private:
  void clear(rb_item<key_t_> *to_clear);
#ifdef TEST
public:
#endif
  rb_item<key_t_> *root_;
};

} // namespace btree