#ifndef RNN_END_IO_NET_HPP
#define RNN_END_IO_NET_HPP

#include "category_net.h"
#include <limits>

namespace rnn {

template<std::size_t word_capacity_t_, std::size_t hidden_dim_t_>
inline category_net<word_capacity_t_, hidden_dim_t_>::category_net()
: init_h_(gru_state<hidden_dim_t_>::gru_vector::Random()),
output_(1.0) {
}

template<std::size_t word_capacity_t_, std::size_t hidden_dim_t_>
inline void category_net<word_capacity_t_, hidden_dim_t_>::train(const std::vector<size_t> &input, size_t label) {

  auto estimated = estimate(input);
  auto bias = output_.train(estimated, label);
  for (auto i = series_.rbegin(); i != series_.rend(); ++i) {
    auto prev = std::next(i);
    auto prev_h = prev == series_.rend()
             ? &init_h_
             : &prev->get_output();
    i->set_bias(bias);
    bias = i->backward(*prev_h);
  }
}

template<std::size_t word_capacity_t_, std::size_t hidden_dim_t_>
inline size_t category_net<word_capacity_t_, hidden_dim_t_>::predict(const std::vector<size_t> &input) {
  auto estimated = estimate(input);
  std::size_t j = 0;
  for(std::size_t i  = 0; i < word_capacity_t_; ++i)
    if(estimated[j] < estimated[i])
      j = i;
  return j;
}

template<std::size_t word_capacity_t_, std::size_t hidden_dim_t_>
rnn_vector<word_capacity_t_> category_net<word_capacity_t_, hidden_dim_t_>::estimate(const std::vector<size_t> &input) {
  auto current = series_.begin();
  auto prev_h = &init_h_;
  for (auto i: input) {
    if (current == series_.end())
      current = series_.emplace(current);
    current->set_cell(words_[i]);
    current->forward(*prev_h);
    prev_h = &current->get_output();
    current = std::next(current);
  }
  return output_.predict(*prev_h);
}

}

#endif //RNN_END_IO_NET_HPP
