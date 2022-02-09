#include "gru_cell_old.hpp"
#include "generic/activity.hpp"
#include "generic/utils.hpp"
#include <Eigen/SVD>

namespace rnn {
gru_cell_old::gru_cell_old(const int inputDim, const int hiddenDim) {
  this->Wxr = MatD(hiddenDim, inputDim);
  this->Whr = MatD(hiddenDim, hiddenDim);
  this->br = VecD::Zero(hiddenDim);

  this->Wxz = MatD(hiddenDim, inputDim);
  this->Whz = MatD(hiddenDim, hiddenDim);
  this->bz = VecD::Zero(hiddenDim);

  this->Wxu = MatD(hiddenDim, inputDim);
  this->Whu = MatD(hiddenDim, hiddenDim);
  this->bu = VecD::Zero(hiddenDim);
}

void gru_cell_old::init(rnn::generic::rand &rnd, const real scale) {
  rnd.uniform(this->Wxr, scale);
  rnd.uniform(this->Whr, scale);

  rnd.uniform(this->Wxz, scale);
  rnd.uniform(this->Whz, scale);

  rnd.uniform(this->Wxu, scale);
  rnd.uniform(this->Whu, scale);

  this->Whr = Eigen::JacobiSVD<MatD>(this->Whr, Eigen::ComputeFullV |
      Eigen::ComputeFullU).matrixU();
  this->Whz = Eigen::JacobiSVD<MatD>(this->Whz, Eigen::ComputeFullV |
      Eigen::ComputeFullU).matrixU();
  this->Whu = Eigen::JacobiSVD<MatD>(this->Whu, Eigen::ComputeFullV |
      Eigen::ComputeFullU).matrixU();
}

gru_cell_old::State gru_cell_old::forward(const VecD &xt, const gru_cell_old::State &prev) {
//U=Wx, W=Wh, h^=u
  gru_cell_old::State cur;
  cur.r = br + Wxr * xt + Whr * prev.h;
  cur.z = bz + Wxz * xt + Whz * prev.h;

  activity::logistic(cur.r);
  activity::logistic(cur.z);

  cur.rh = cur.r.array() * prev.h.array();
  cur.u = bu + Wxu * xt + Whu * cur.rh;
  activity::tanh(cur.u);
  cur.h = (1.0 - cur.z.array()) * cur.u.array() +
      cur.z.array() * prev.h.array();
  return cur;
}

void gru_cell_old::backward(gru_cell_old::State *prev, gru_cell_old::State *cur, gru_cell_old::Grad &grad,
                        const VecD &xt) {
  VecD delr, delz, delu, delrh;
  //d7=delz
  delz = activity::logisticPrime(cur->z).array() * cur->delh.array() *
      (cur->u - prev->h).array();
  delu =
      activity::tanhPrime(cur->u).array() * cur->delh.array() * cur->z.array();
  delrh = Whu.transpose() * delu;
  delr =
      activity::logisticPrime(cur->r).array() * delrh.array() * prev->h.array();

  cur->delx = Wxr.transpose() * delr
      + Wxz.transpose() * delz
      + Wxu.transpose() * delu;

  prev->delh.noalias() += Whr.transpose() * delr + Whz.transpose() * delz;
  prev->delh.array() += delrh.array() * cur->r.array() +
          cur->delh.array() * (1.0 - cur->z.array());

  grad.Wxr.noalias() += delr * xt.transpose();
  grad.Whr.noalias() += delr * prev->h.transpose();

  grad.Wxz.noalias() += delz * xt.transpose();
  grad.Whz.noalias() += delz * prev->h.transpose();

  grad.Wxu.noalias() += delu * xt.transpose();
  grad.Whu.noalias() += delu * cur->rh.transpose();

  grad.br += delr;
  grad.bz += delz;
  grad.bu += delu;
}

void gru_cell_old::sgd(const gru_cell_old::Grad &grad, const real learningRate) {
  Wxr -= learningRate * grad.Wxr;
  Whr -= learningRate * grad.Whr;
  br -= learningRate * grad.br;

  Wxz -= learningRate * grad.Wxz;
  Whz -= learningRate * grad.Whz;
  bz -= learningRate * grad.bz;

  Wxu -= learningRate * grad.Wxu;
  Whu -= learningRate * grad.Whu;
  bu -= learningRate * grad.bu;
}

void gru_cell_old::save(std::ofstream &ofs) {
  rnn::generic::save(ofs, Wxr);
  rnn::generic::save(ofs, Whr);
  rnn::generic::save(ofs, br);
  rnn::generic::save(ofs, Wxz);
  rnn::generic::save(ofs, Whz);
  rnn::generic::save(ofs, bz);
  rnn::generic::save(ofs, Wxu);
  rnn::generic::save(ofs, Whu);
  rnn::generic::save(ofs, bu);
}

void gru_cell_old::load(std::ifstream &ifs) {
  rnn::generic::load(ifs, Wxr);
  rnn::generic::load(ifs, Whr);
  rnn::generic::load(ifs, br);
  rnn::generic::load(ifs, Wxz);
  rnn::generic::load(ifs, Whz);
  rnn::generic::load(ifs, bz);
  rnn::generic::load(ifs, Wxu);
  rnn::generic::load(ifs, Whu);
  rnn::generic::load(ifs, bu);
}

void gru_cell_old::State::clear() {
  this->h = VecD();
  this->u = VecD();
  this->r = VecD();
  this->z = VecD();
  this->rh = VecD();
  this->delh = VecD();
  this->delx = VecD();
}

gru_cell_old::Grad::Grad(const gru_cell_old &gru) {
  this->Wxr = MatD::Zero(gru.Wxr.rows(), gru.Wxr.cols());
  this->Whr = MatD::Zero(gru.Whr.rows(), gru.Whr.cols());
  this->br = VecD::Zero(gru.br.rows());

  this->Wxz = MatD::Zero(gru.Wxz.rows(), gru.Wxz.cols());
  this->Whz = MatD::Zero(gru.Whz.rows(), gru.Whz.cols());
  this->bz = VecD::Zero(gru.bz.rows());

  this->Wxu = MatD::Zero(gru.Wxu.rows(), gru.Wxu.cols());
  this->Whu = MatD::Zero(gru.Whu.rows(), gru.Whu.cols());
  this->bu = VecD::Zero(gru.bu.rows());
};

void gru_cell_old::Grad::init() {
  this->Wxr.setZero();
  this->Whr.setZero();
  this->br.setZero();
  this->Wxz.setZero();
  this->Whz.setZero();
  this->bz.setZero();
  this->Wxu.setZero();
  this->Whu.setZero();
  this->bu.setZero();
}

real gru_cell_old::Grad::norm() {
  return
      this->Wxr.squaredNorm() + this->Whr.squaredNorm() +
          this->br.squaredNorm() +
          this->Wxz.squaredNorm() + this->Whz.squaredNorm() +
          this->bz.squaredNorm() +
          this->Wxu.squaredNorm() + this->Whu.squaredNorm() +
          this->bu.squaredNorm();
}

void gru_cell_old::Grad::operator+=(const gru_cell_old::Grad &grad) {
  this->Wxr += grad.Wxr;
  this->Whr += grad.Whr;
  this->br += grad.br;
  this->Wxz += grad.Wxz;
  this->Whz += grad.Whz;
  this->bz += grad.bz;
  this->Wxu += grad.Wxu;
  this->Whu += grad.Whu;
  this->bu += grad.bu;
}
}