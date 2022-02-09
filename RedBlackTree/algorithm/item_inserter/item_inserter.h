#include "rb_tree/tree_visitor_i.hpp"

namespace btree::algorithm {

template<comparable_key key_t_>
class item_inserter final : public tree_visitor_i<key_t_> {
public:
  template<typename ... _key_args>
  explicit item_inserter(_key_args&&... args);
  ~item_inserter() override;

  item_inserter(const item_inserter &other) = delete;
  item_inserter(item_inserter &&other) noexcept;
  item_inserter &operator=(const item_inserter &other) = delete;
  item_inserter &operator=(item_inserter &&other) = delete;

private:
  void visit(rb_item<key_t_> *&root) override;
  branch_t<key_t_> insert_to_branch(rb_item<key_t_> *&root);
  void regulate_red_color(branch_t<key_t_> branch);

  rb_item<key_t_>* to_insert_;
};

} // namespace btree::algorithm