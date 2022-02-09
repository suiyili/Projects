#ifdef TEST

#include "consecutive_red_regulator.hpp"
#include <catch2/catch_test_macros.hpp>

namespace btree::algorithm {

using rb_item_t = rb_item<int>;

struct two_level_tree {
  two_level_tree();
  rb_item_t root;
  rb_item_t left_child;
  rb_item_t right_child;
};

struct three_level_tree : public two_level_tree {
  three_level_tree();
  rb_item_t left_left_grand;
  rb_item_t left_right_grand;
  rb_item_t right_left_grand;
  rb_item_t right_right_grand;
};

SCENARIO("regulate consecutive red test", "[regulate_consecutive_red]") {

  GIVEN("a pair of consecutive red nodes") {
    three_level_tree tree;
    tree.left_child.decrease_color();
    tree.left_right_grand.decrease_color();

    CHECK(tree.left_child.color() == 0);
    CHECK(tree.left_right_grand.color() == 0);

    AND_GIVEN("the sibling of them is red") {
      tree.right_child.decrease_color();
      CHECK(tree.right_child.color() == 0);

      WHEN("regulate the consecutive red nodes") {
        auto root_ptr = &tree.root;
        consecutive_red_regulator<int> regulator;
        auto regulated =
            regulator.regulate(tree.left_child.get_child(side::right));
        regulated &= regulator.regulate(tree.root.get_child(side::left));
        regulated &= regulator.regulate(root_ptr);

        THEN("root will push down color") {
          REQUIRE(regulated);

          REQUIRE(root_ptr->color() == 0U);
          REQUIRE(tree.left_child.color() == 1U);
          REQUIRE(tree.right_child.color() == 1U);
        }
      }
    }AND_GIVEN("the sibling is black") {
      CHECK(tree.right_child.color() == 1);
      WHEN("regulate the consecutive red nodes") {
        auto root_ptr = &tree.root;
        consecutive_red_regulator<int> regulator;
        auto regulated =
            regulator.regulate(tree.left_child.get_child(side::right));
        regulated &= regulator.regulate(tree.root.get_child(side::left));
        regulated &= regulator.regulate(root_ptr);

        THEN("root will rotate the consecutive red nodes") {
          REQUIRE(regulated);

          REQUIRE(root_ptr == &tree.left_right_grand);
          REQUIRE(root_ptr->color() == 1U);

          auto left = root_ptr->get_child(side::left);
          REQUIRE(left == &tree.left_child);
          REQUIRE(left->color() == 0U);

          auto right = root_ptr->get_child(side::right);
          REQUIRE(right == &tree.root);
          REQUIRE(right->color() == 0U);
        }
      }
    }
  }

  GIVEN("no consecutive red nodes") {
    three_level_tree tree;
    tree.left_right_grand.decrease_color();

    CHECK(tree.left_child.color() == 1);
    CHECK(tree.left_right_grand.color() == 0);

    WHEN("regulate the nodes") {
      consecutive_red_regulator<int> regulator;
      auto regulated =
          regulator.regulate(tree.left_child.get_child(side::right));
      regulated &= regulator.regulate(tree.root.get_child(side::left));
      THEN("it should failed") {
        REQUIRE_FALSE(regulated);
      }
    }
  }
}

two_level_tree::two_level_tree()
    : root(4), left_child(2),
      right_child(6) {
  root.increase_color();
  left_child.increase_color();
  right_child.increase_color();
  root.get_child(side::left) = &left_child;
  root.get_child(side::right) = &right_child;
}

three_level_tree::three_level_tree()
    : left_left_grand(1),
      left_right_grand(3),
      right_left_grand(5),
      right_right_grand(7) {

  left_left_grand.increase_color();
  left_right_grand.increase_color();
  right_left_grand.increase_color();
  right_right_grand.increase_color();

  left_child.get_child(side::left) = &left_left_grand;
  left_child.get_child(side::right) = &left_right_grand;
  right_child.get_child(side::left) = &right_left_grand;
  right_child.get_child(side::right) = &right_right_grand;
}

} // namespace btree::algorithm

#endif // TEST