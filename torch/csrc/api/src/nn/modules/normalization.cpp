#include <torch/nn/modules/normalization.h>

#include <torch/cuda.h>
#include <torch/utils.h>
#include <torch/nn/init.h>

#include <ostream>
#include <utility>

namespace F = torch::nn::functional;

namespace torch {
namespace nn {

LayerNormImpl::LayerNormImpl(const LayerNormOptions& options_) : options(options_) { // NOLINT(modernize-pass-by-value)
  reset();
}

void LayerNormImpl::reset() {
  if (options.elementwise_affine()) {
    weight = register_parameter("weight", torch::empty(options.normalized_shape()));
    bias = register_parameter("bias", torch::empty(options.normalized_shape()));
  } else {
    weight = register_parameter("weight", torch::Tensor(), /*requires_grad=*/false);
    bias = register_parameter("bias", torch::Tensor(), /*requires_grad=*/false);
  }
  if (options.elementwise_affine()) {
    torch::nn::init::ones_(weight);
    torch::nn::init::zeros_(bias);
  }
}

void LayerNormImpl::pretty_print(std::ostream& stream) const {
  stream << std::boolalpha
         << "torch::nn::LayerNorm(" << torch::IntArrayRef(options.normalized_shape())
         << ", eps=" << options.eps()
         << ", elementwise_affine=" << options.elementwise_affine()
         << ")";
}

torch::Tensor LayerNormImpl::forward(const Tensor& input) {
  return F::layer_norm(input, options, weight, bias);
}

// ============================================================================

LocalResponseNormImpl::LocalResponseNormImpl(const LocalResponseNormOptions& options_)
    : options(options_) {}

Tensor LocalResponseNormImpl::forward(const Tensor& input) {
  return F::local_response_norm(input, options);
}

void LocalResponseNormImpl::reset() {}

void LocalResponseNormImpl::pretty_print(std::ostream& stream) const {
  stream << std::boolalpha
         << "torch::nn::LocalResponseNorm(" <<  options.size()
         << ", alpha=" << options.alpha()
         << ", beta=" << options.beta()
         << ", k=" << options.k()
         << ")";
}
} // namespace nn
} // namespace torch
