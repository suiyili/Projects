#ifndef RNN_CORE_CATEGORY_VEC_HPP
#define RNN_CORE_CATEGORY_VEC_HPP

#include "category_vec.h"

namespace rnn::core {

template<std::size_t hidden_dim_t_, std::size_t cat_dim_t_>
inline category_vec<hidden_dim_t_, cat_dim_t_>::category_vec(real scale)
    :w_(decltype(w_)::Random()),
     b_(decltype(b_)::Random()),
     w_grad_(decltype(w_grad_)::Zero()),
     b_grad_(decltype(b_grad_)::Zero()) {
  w_ *= scale;
  b_ *= scale;
}

template<std::size_t hidden_dim_t_, std::size_t cat_dim_t_>
inline rnn_vector<cat_dim_t_>
category_vec<hidden_dim_t_, cat_dim_t_>::predict(const rnn_vector<hidden_dim_t_> &input) {
  input_ = input;
  decltype(b_) output = w_ * input + b_;

  output.array() -= output.maxCoeff();
  output.array() = output.array().exp();
  return output / output.sum();
}

template<std::size_t hidden_dim_t_, std::size_t cat_dim_t_>
inline rnn_vector<hidden_dim_t_>
category_vec<hidden_dim_t_, cat_dim_t_>::train(const rnn_vector<cat_dim_t_> &estimated,
                                                  std::size_t sample) {
  rnn_vector<cat_dim_t_> errors(rnn_vector<cat_dim_t_>::Zero());
  errors[sample] = 1.0;
  errors = estimated - errors;

  w_grad_.noalias() += errors * input_.transpose();
  b_grad_.noalias() += errors;
  ++count_;
  return w_.transpose() * errors;
}

template<std::size_t hidden_dim_t_, std::size_t cat_dim_t_>
inline void category_vec<hidden_dim_t_, cat_dim_t_>::learn(real rate) {
  if(!count_) return;
  auto r = rate / count_;
  w_.noalias() -= r * w_grad_;
  b_.noalias() -= r * b_grad_;
  w_grad_ = decltype(w_grad_)::Zero();
  b_grad_ = decltype(b_grad_)::Zero();
  count_ = 0;
}

}

#endif //RNN_CORE_CATEGORY_VEC_HPP
