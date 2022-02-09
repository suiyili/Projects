#ifdef TEST

#include "dual_black_regulator.hpp"
#include <catch2/catch_test_macros.hpp>

namespace btree::algorithm {

using rb_item_t = rb_item<int>;

SCENARIO("dual black regulator test", "[dual_black_regulator]") {

  GIVEN("x child is single black") {
    /*
     *              3(b)
     *             / \
     *           1(b) 7(b)
     *                / \
     *             5(b) 8(b)
     * */
    rb_item_t parent(3);
    parent.increase_color();
    auto root = &parent;

    rb_item_t x_child(1);
    x_child.increase_color();
    parent.get_child(side::left) = &x_child;

    rb_item_t right_child(7);
    right_child.increase_color();
    parent.get_child(side::right) = &right_child;

    rb_item_t nephew_left(5);
    nephew_left.increase_color();
    right_child.get_child(side::left) = &nephew_left;

    rb_item_t nephew_right(8);
    nephew_right.increase_color();
    right_child.get_child(side::right) = &nephew_right;

    CHECK(x_child.color() == 1U);
    CHECK(right_child.color() == 1U);
    AND_GIVEN("force it as dual black") {
      dual_black_regulator regulator(&x_child, true);
      WHEN("regulate x child") {
        THEN("it will regulate color") { REQUIRE(regulator.regulate(root)); }
      }
    }
    AND_GIVEN("it as single black") {
      dual_black_regulator regulator(&x_child, false);
      WHEN("regulate x child") {
        THEN("it will do nothing") { REQUIRE_FALSE(regulator.regulate(root)); }
      }
    }
  }

  GIVEN("x child and red sibling") {
    /*         3(b)
     *        /    \
     *     1(2b)   7(r)
     *            /   \
     *          6(b)  8(b)
     * */
    rb_item_t parent(3);
    parent.increase_color();
    auto root = &parent;

    rb_item_t x_child(1);
    x_child.increase_color();
    x_child.increase_color();
    parent.get_child(side::left) = &x_child;

    rb_item_t right_child(7);
    parent.get_child(side::right) = &right_child;

    rb_item_t rl_grand(6);
    rl_grand.increase_color();
    right_child.get_child(side::left) = &rl_grand;

    rb_item_t rr_grand(8);
    rr_grand.increase_color();
    right_child.get_child(side::right) = &rr_grand;

    CHECK(parent.color() == 1U);
    CHECK(x_child.color() == 2U);
    CHECK(right_child.color() == 0U);
    CHECK(rl_grand.color() == 1U);
    CHECK(rr_grand.color() == 1U);

    WHEN("regulate x child") {
      dual_black_regulator regulator(&x_child);
      regulator.regulate(root);
      THEN("it will rotate the red sibling to top") {
        REQUIRE((root == &right_child));
        AND_THEN("it will increase its color") {
          REQUIRE(root->color() == 1);
        }
      }
      AND_THEN("it will decrease x child color") {
        REQUIRE(x_child.color() == 1U);
      }
      AND_THEN("it will keep old parent its color") {
        // parent becomes new x node and regulate again.
        // but not a dual black.
        REQUIRE(parent.color() == 1U);
      }
    }
  }

  GIVEN("x child and black sibling") {
    /*         3(b)
     *        /    \
     *     1(2b)   7(b)
     *            /   \
     *          5(r/b)  8(b)
     * */
    rb_item_t parent(3);
    auto root = &parent;

    rb_item_t x_child(1);
    x_child.increase_color();
    x_child.increase_color();
    parent.get_child(side::left) = &x_child;

    rb_item_t right_child(7);
    right_child.increase_color();
    parent.get_child(side::right) = &right_child;

    rb_item_t rl_grand(5);
    right_child.get_child(side::left) = &rl_grand;

    rb_item_t rr_grand(8);
    rr_grand.increase_color();
    right_child.get_child(side::right) = &rr_grand;

    CHECK(parent.color() == 0U);
    CHECK(x_child.color() == 2U);
    CHECK(right_child.color() == 1U);

    AND_GIVEN("only its far nephew is black") {
      /*           \
       *           7(b)
       *          /    \
       *          5(r)  8(b)
       *         /   \
       *       4(b)  6(b)
       * */

      rb_item_t rll_great(4);
      rll_great.increase_color();
      rl_grand.get_child(side::left) = &rll_great;

      rb_item_t rlr_great(6);
      rlr_great.increase_color();
      rl_grand.get_child(side::right) = &rlr_great;

      CHECK(rl_grand.color() == 0U);
      CHECK(rr_grand.color() == 1U);

      WHEN("regulate x child") {
        dual_black_regulator regulator(&x_child);
        regulator.regulate(root);

        THEN("it will rotate closest nephew as parent") {
          REQUIRE(root == &rl_grand);
        }
        THEN("the old parent will increase color") {
          REQUIRE(parent.color() == 1);
        }
        THEN("it will decrease x child color") { REQUIRE(x_child.color() == 1U); }
      }
    }

    AND_GIVEN("two nephews are black"){
      /*           \
       *           7(b)
       *          /    \
       *         5(b)  8(b)
       * */
      rl_grand.increase_color();

      CHECK(rl_grand.color() == 1U);
      CHECK(rr_grand.color() == 1U);

      WHEN("regulate x child") {

        dual_black_regulator regulator(&x_child);
        regulator.regulate(root);

        THEN("it should pull up color to parent") {
          REQUIRE(root->color() == 1U);
        }
        THEN("it will decrease x child color") {
          REQUIRE(x_child.color() == 1U);
        }
        THEN("it will increase x parent color") {
          REQUIRE(parent.color() == 1U);
        }
      }
    }
  }
}

} // namespace btree::algorithm
#endif // TEST