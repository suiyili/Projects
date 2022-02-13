#ifdef TEST

#include <catch2/catch_test_macros.hpp>
#include "category_net.hpp"
#include <random>
#include <algorithm>

using namespace Catch;

namespace rnn {

SCENARIO("next predict test", "[category_net]") {
  GIVEN("a set of sample data") {
    const std::size_t I = 0U, like = 1U, doo = 2U, noot = 3U, hate = 4U, is = 5U,
    my = 6U, favorite = 7U, math = 8U, subject = 9U;
    const std::size_t bad = 0, good = 1;

    std::vector<std::pair<std::vector<std::size_t>, std::size_t>> data {
        {{I, like, math}, good},
        {{I, hate, math}, bad},
        {{I, doo, like, math}, good},
        {{I, doo, noot, like, math}, bad},
        {{I, doo, noot, hate, math}, good},
        {{math, is, my, favorite, subject}, good},
        {{math, is, noot, my, favorite, subject}, bad},
        {{I, doo, noot, hate, math}, good},
        {{I, hate, my, subject, is, math}, bad}
    };

    WHEN("train net with them") {
      std::random_device rd;
      std::mt19937 gen(rd());

      category_net<subject + 1, 8U, good + 1> category;
      const real learning_rate = 1.f;
      constexpr std::size_t num_batch = 4096;

      for (std::size_t batch = 0; batch < num_batch; ++batch) {
        std::shuffle(data.begin(), data.end(), gen);
        for (auto&[text, label] : data) {
          category.train(text, label);category.learn(learning_rate);
        }
      }

      THEN("it should predict the last label") {
        constexpr std::size_t test_batch = 64;
        auto total = test_batch * data.size();
        real correctness = 0.f;
        for (std::size_t batch = 0; batch < test_batch; ++batch) {
          std::shuffle(data.begin(), data.end(), gen);
          for (auto&[text, label] : data) {
            if (label == category.predict(text))
              ++correctness;
          }
        }
        REQUIRE(correctness / total > 0.95);
      }
    }
  }
}
}  // namespace rnn

#endif  // TEST