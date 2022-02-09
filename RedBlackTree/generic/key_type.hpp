#pragma once

namespace btree::generic {
template <typename key_t_>
concept comparable_key = requires(key_t_ k1, key_t_ k2) {
  k1 <=> k2;
};
} // namespace btree
