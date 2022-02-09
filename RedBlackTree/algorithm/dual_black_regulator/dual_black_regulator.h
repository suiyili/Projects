#include "rb_item/rb_item.hpp"

using namespace btree::item;

namespace btree::algorithm {

template <comparable_key key_t_>
class dual_black_regulator final {
public:
  explicit dual_black_regulator(rb_item<key_t_>* dual_black_item, bool force = false);
  bool regulate(rb_item<key_t_>*& parent);

private:
  static side get_side(rb_item<key_t_>* parent, rb_item<key_t_>* child);
  static bool is_black(rb_item<key_t_>* item);

  bool rotate_sibling(rb_item<key_t_>*& parent, side x_side);

  rb_item<key_t_>* dual_black_item_;
};

template <comparable_key key_t_>
dual_black_regulator(rb_item<key_t_> *)->dual_black_regulator<key_t_>;

} // namespace btree::algorithm