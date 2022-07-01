cmake_minimum_required()
project(BalancedBinaryTree)

include(../base.cmake)

set(SRC algorithm/consecutive_red_regulator/consecutive_red_regulator_test.cpp
        algorithm/dual_black_regulator/dual_black_regulator_test.cpp
        algorithm/item_inserter/item_inserter_test.cpp
        algorithm/item_remover/item_remover_test.cpp
        rb_item/rb_item_test.cpp
        rb_tree/rb_tree_test.cpp
        generic/resource_manager/resource_manager.cpp
        )

add_executable(BalancedBinaryTree ${SRC})
target(BalancedBinaryTree)




