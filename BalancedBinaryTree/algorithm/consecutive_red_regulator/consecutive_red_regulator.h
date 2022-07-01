#include "rb_item/rb_item.hpp"

using namespace btree::item;

namespace btree::algorithm {

template <comparable_key key_t_> class consecutive_red_regulator final {
public:
  consecutive_red_regulator();
  bool regulate(rb_item<key_t_> *&parent);

private:
  static side get_child_side(rb_item<key_t_>* parent, rb_item<key_t_>* child);
  rb_item<key_t_> **chain_[2];
  std::uint8_t count_;
};

} // namespace btree::algorithm