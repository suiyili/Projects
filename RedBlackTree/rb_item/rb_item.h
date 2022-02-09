#pragma once

#include "generic/key_type.hpp"
#include <memory>
#include <stack>

using namespace btree::generic;

namespace btree::item {

enum class side : bool {
  left = true,
  right = false,
};

static side opposite(side which);

template <comparable_key key_t_> class rb_item final {
public:
  using value_type = key_t_;

  template<typename... _key_args>
  explicit rb_item(_key_args&&... args);

  ~rb_item() = default;

  static void *operator new(std::size_t size);
  static void operator delete(void *p);

  explicit operator key_t_ &();

  [[nodiscard]] std::uint8_t color() const;
  void increase_color();
  bool decrease_color();

  void pull_up_color();
  void push_down_color();

  rb_item *&get_child(side which);

private:  
  void push_down_color(rb_item *child);
  void pull_up_color(rb_item *child);

  key_t_ key_;
  unsigned char color_ = 0U;
  rb_item *left_ = nullptr;
  rb_item *right_ = nullptr;
};

template <comparable_key key_t_>
void rotate(rb_item<key_t_>*& item, side which);

template<comparable_key key_t_>
using branch_t = std::stack<std::reference_wrapper<rb_item<key_t_>*>>;

} // namespace btree::item
