#ifndef RNN_CORE_WORD_VEC_HPP
#define RNN_CORE_WORD_VEC_HPP

#include "word_vec.h"

namespace rnn::core {

template<std::size_t hidden_dim_t_, std::size_t output_dim_t_>
inline word_vec<hidden_dim_t_, output_dim_t_>::word_vec(real scale)
    :w_(decltype(w_)::Random()),
     b_(decltype(b_)::Random()),
     w_grad_(decltype(w_grad_)::Zero()),
     b_grad_(decltype(b_grad_)::Zero()) {
  w_ *= scale;
}

template<std::size_t hidden_dim_t_, std::size_t output_dim_t_>
inline rnn_vector<output_dim_t_>
word_vec<hidden_dim_t_, output_dim_t_>::predict(const rnn_vector<hidden_dim_t_> &input) {
  input_ = input;
  decltype(b_) output = w_ * input + b_;

  output.array() -= output.maxCoeff();
  output.array() = output.array().exp();
  return output / output.sum();
}

template<std::size_t hidden_dim_t_, std::size_t output_dim_t_>
inline rnn_vector<hidden_dim_t_>
word_vec<hidden_dim_t_, output_dim_t_>::train(const rnn_vector<output_dim_t_> &estimated,
                                              std::size_t sample) {
  rnn_vector<output_dim_t_> errors(rnn_vector<output_dim_t_>::Zero());
  errors[sample] = 1.0;
  errors = estimated - errors;

  w_grad_ += errors * input_.transpose();
  b_grad_ += errors;
  return w_.transpose() * errors;
}

template<std::size_t hidden_dim_t_, std::size_t output_dim_t_>
inline void word_vec<hidden_dim_t_, output_dim_t_>::learn(real rate) {
  w_ -= rate * w_grad_;
  b_ -= rate * b_grad_;
}

}

#endif //RNN_CORE_WORD_VEC_HPP
