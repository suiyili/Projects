#ifdef TEST

#include <catch2/catch_test_macros.hpp>
#include "item_inserter.hpp"

namespace btree::algorithm {

using rb_item_t = rb_item<int>;
using item_inserter_t = item_inserter<int>;

const int existing_key = 1;
static rb_item<int> *set_up_tree();

SCENARIO("item inserter test", "[item_inserter]") {
  GIVEN("a rb-tree with items setup") {
    auto root = set_up_tree();

    WHEN("inserting a new item") {
      const int key_to_insert = 4;
      item_inserter_t inserter(key_to_insert);
      tree_visitor_i<int> &visitor = inserter;
      visitor.visit(root);

      THEN("it should follow red black tree insert rule") {
        /*
          expected result:
                   7(b)
                /        \
              2(r)       11(r)
             /   \      /    \
           1(b)  5(b)  8(b)  14(b)
                 /            \
                4(r)          15(r)

        */
        REQUIRE(((int)*root == 7));
        REQUIRE(root->color() == 1U);

        auto& left = root->get_child(side::left);
        REQUIRE(((int)*left == 2));
        REQUIRE(left->color() == 0U);

        auto& ll = left->get_child(side::left);
        REQUIRE(((int)*ll == existing_key));
        REQUIRE(ll->color() == 1U);

        auto& lr = left->get_child(side::right);
        REQUIRE(((int)*lr == 5));
        REQUIRE(lr->color() == 1U);

        auto& lrl = lr->get_child(side::left);
        REQUIRE(((int)*lrl == 4));
        REQUIRE(lrl->color() == 0U);

        auto& right = root->get_child(side::right);
        REQUIRE(((int)*right == 11));
        REQUIRE(right->color() == 0U);

        auto& rl = right->get_child(side::left);
        REQUIRE(((int)*rl == 8));
        REQUIRE(rl->color() == 1U);

        auto& rr = right->get_child(side::right);
        REQUIRE(((int)*rr == 14));
        REQUIRE(rr->color() == 1U);

        auto& rrr = rr->get_child(side::right);
        REQUIRE(((int)*rrr == 15));
        REQUIRE(rrr->color() == 0U);
      }
    }

    AND_WHEN("inserting an existing item") {
      THEN("it will throw") {
        item_inserter_t inserter(existing_key);
        tree_visitor_i<int> &visitor = inserter;

        REQUIRE_THROWS_AS(visitor.visit(root), item_already_exists);
      }
    }
  }
}

inline rb_item_t *set_up_tree() {
  /*
                     11(b)
                   /       \
                2(r)        14(b)
               /   \         \
             1(b)  7(b)      15(r)
                  /   \
                5(r)  8(r)

  */
  auto i_15 = new rb_item_t(15);
  auto i_14 = new rb_item_t(14);
  i_14->increase_color();
  i_14->get_child(side::right) = i_15;

  auto i_5 = new rb_item_t(5);
  auto i_8 = new rb_item_t(8);

  auto i_7 = new rb_item_t(7);
  i_7->increase_color();
  i_7->get_child(side::left) = i_5;
  i_7->get_child(side::right) = i_8;

  auto i_1 = new rb_item_t(existing_key);
  i_1->increase_color();

  auto i_2 = new rb_item_t(2);
  i_2->get_child(side::left) = i_1;
  i_2->get_child(side::right) = i_7;

  auto i_11 = new rb_item_t(11);
  i_11->increase_color();
  i_11->get_child(side::left) = i_2;
  i_11->get_child(side::right) = i_14;

  return i_11;
}

} // namespace btree::algorithm

#endif // TEST