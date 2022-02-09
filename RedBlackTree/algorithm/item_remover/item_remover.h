#include "rb_tree/tree_visitor_i.hpp"

namespace btree::algorithm {

template<comparable_key key_t_>
class item_remover final : public tree_visitor_i<key_t_> {
public:
  explicit item_remover(const key_t_ &key_to_remove);
  ~item_remover() = default;

private:

  void visit(rb_item<key_t_> *&root) override;
  
  branch_t<key_t_> find_removing_branch(rb_item<key_t_> *&root);
  void append_right_min(branch_t<key_t_>& branch);
  rb_item<key_t_>* remove(rb_item<key_t_> *&item);
  static bool both_children_available(rb_item<key_t_>& parent);
  static rb_item<key_t_>* get_single_child(rb_item<key_t_>& parent);

  const key_t_ &to_remove_;
};

template <comparable_key key_t_>
item_remover(const key_t_ &key_to_remove)->item_remover<key_t_>;

} // namespace btree::algorithm