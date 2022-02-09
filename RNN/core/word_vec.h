
#include "generic/rnn_type.hpp"

namespace rnn::core {

template<std::size_t hidden_dim_t_, std::size_t output_dim_t_>
class word_vec final {
public:
  explicit word_vec(real scale);
  rnn_vector<output_dim_t_> predict(const rnn_vector<hidden_dim_t_>& input);
  rnn_vector<hidden_dim_t_> train(const rnn_vector<output_dim_t_>& estimated,
                                  std::size_t sample);
  void learn(real rate = 1.0);
private:
  rnn_matrix<output_dim_t_, hidden_dim_t_> w_;
  rnn_vector<output_dim_t_> b_;
  rnn_vector<hidden_dim_t_> input_;
  rnn_matrix<output_dim_t_, hidden_dim_t_> w_grad_;
  rnn_vector<output_dim_t_> b_grad_;
};


}
