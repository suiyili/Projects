#ifdef TEST

#include "rb_item.hpp"
#include <catch2/catch_test_macros.hpp>

namespace btree::item {

using rb_item_t = rb_item<int>;

SCENARIO("rb item test", "[rb_item]") {

  GIVEN("init data") {
    WHEN("contruct the tree item") {
      rb_item_t item(5);
      THEN("the default color is red") { REQUIRE(item.color() == 0U); }
    }
  }

  GIVEN("a black parent with one of child is red") {
    rb_item_t parent(5);
    parent.increase_color();
    rb_item_t left_child(4);
    parent.get_child(side::left) = &left_child;

    CHECK(left_child.color() == 0);

    WHEN("pull up color") {
      THEN("it will throw") {
        REQUIRE_THROWS_AS(parent.pull_up_color(), color_error);
      }
    }
    WHEN("push down color") {
      parent.push_down_color();
      THEN("parent decrease color") {
        REQUIRE(parent.color() == 0U);
        AND_THEN("the child will increase color") {
          REQUIRE(left_child.color() == 1U);
        }
      }
    }
  }

  GIVEN("a parent without children") {
    rb_item_t parent(5);

    CHECK_FALSE(parent.get_child(side::left));
    CHECK_FALSE(parent.get_child(side::right));

    WHEN("pull up color") {
      parent.pull_up_color();
      THEN("parent should increase color") { REQUIRE(parent.color() == 1U); }
    }

    AND_GIVEN("the parent is red") {
      CHECK(parent.color() == 0);
      WHEN("push down color") {
        THEN("it will throw") {
          REQUIRE_THROWS_AS(parent.push_down_color(), color_error);
        }
      }
    }
    AND_GIVEN("the parent is black") {
      parent.increase_color();
      CHECK(parent.color() == 1);
      WHEN("push down color") {
        parent.push_down_color();
        THEN("parent decrease color") { REQUIRE(parent.color() == 0U); }
      }
    }
  }

  GIVEN("a black parent with only one black child") {
    rb_item_t parent(5);
    parent.increase_color();

    rb_item_t left_child(4);
    parent.get_child(side::left) = &left_child;
    left_child.increase_color();

    CHECK(parent.color() == 1);
    CHECK_FALSE(parent.get_child(side::right));
    CHECK(left_child.color() == 1);

    WHEN("pull up color") {
      parent.pull_up_color();
      THEN("parent should increase color") { REQUIRE(parent.color() == 2U); }
      AND_THEN("children will decrease color") {
        REQUIRE(left_child.color() == 0);
      }
    }

    WHEN("push down color") {
      parent.push_down_color();
      THEN("parent decrease color") {
        REQUIRE(parent.color() == 0U);
        AND_THEN("the child will increase color") {
          REQUIRE(left_child.color() == 2U);
        }
      }
    }
  }

  GIVEN("a parent with both children") {
    rb_item_t parent(5);

    rb_item_t left_child(4);
    parent.get_child(side::left) = &left_child;
    left_child.increase_color();
    rb_item_t right_child(6);
    parent.get_child(side::right) = &right_child;

    CHECK(parent.color() == 0);
    CHECK(parent.get_child(side::left));
    CHECK(parent.get_child(side::right));
    CHECK(left_child.color() == 1);
    CHECK(right_child.color() == 0);

    WHEN("rotate with the black child") {
      THEN("it should throw") {
        auto root = &parent;
        REQUIRE_THROWS_AS(rotate(root, side::left), rotate_error);
      }
    }
    WHEN("rotate with the red child") {
      parent.increase_color();
      auto root = &parent;
      rotate(root, side::right);
      THEN("it should switch parent child position") {
        REQUIRE(root == &right_child);
        REQUIRE(root->get_child(side::left) == &parent);
        AND_THEN("it also should swtich their color") {
          REQUIRE(right_child.color() == 1);
          REQUIRE(parent.color() == 0);
        }
      }
    }
  }
}

} // namespace btree::item

#endif // TEST