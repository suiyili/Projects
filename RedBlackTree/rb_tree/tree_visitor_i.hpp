#pragma once

#include "../rb_item/rb_item.hpp"
using namespace btree::item;

namespace btree {

template<comparable_key key_t_>
class tree_visitor_i {
public:
  virtual ~tree_visitor_i() = default;
  virtual void visit(rb_item<key_t_> *&root) = 0;
};

} // namespace btree