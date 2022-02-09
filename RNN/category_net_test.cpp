#ifdef TEST

#include <catch2/catch_test_macros.hpp>
#include "category_net.hpp"

using namespace Catch;

namespace rnn {

SCENARIO("next predict test", "[category_net]") {
  GIVEN("a set of sample data") {
    const std::size_t a = 0U, c = 1U, e = 2U, h = 3U, i = 4U,
        m = 5U, s = 6U, t = 7;
    std::array<std::size_t, 11> origin {m, a, t, h, e, m, a, t, i, c, s};
    WHEN("train net with them") {
      category_net<sizeof(origin), 4U> category;

      std::vector<std::size_t> input(origin.size() - 1, 0U);
      constexpr std::size_t num_batch = 8;
      for(std::size_t batch = 0; batch < num_batch; ++batch) {
        for(std::size_t label_pos = 0; label_pos < origin.size(); ++label_pos) {
          auto label = origin[label_pos];
          for(std::size_t j = 0; j < input.size(); ++j) {
            auto k = j + label_pos + 1;
            input[j] = origin[k % origin.size()];
          }
          category.train(input, label);
        }
      }
      THEN("it should predict the last label") {
        constexpr std::size_t test_batch = 2;
        constexpr auto total = test_batch * origin.size();
        std::size_t correctness = 0;
        for(std::size_t batch = 0; batch < test_batch; ++batch) {
          for(std::size_t label_pos = 0; label_pos < origin.size(); ++label_pos) {
            auto label = origin[label_pos];
            for(std::size_t j = 0; j < input.size(); ++j) {
              auto k = j + label_pos + 1;
              input[j] = origin[k % origin.size()];
            }
            if( label == category.predict(input))
              ++correctness;
          }
        }
        REQUIRE(((real)correctness) / total > 0.9);
      }
    }
  }
}
}  // namespace rnn

#endif  // TEST