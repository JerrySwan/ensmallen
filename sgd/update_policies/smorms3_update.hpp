/**
 * @file smorms3_update.hpp
 * @author Vivek Pal
 *
 * SMORMS3 update for Stochastic Gradient Descent.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_SMORMS3_UPDATE_HPP
#define MLPACK_CORE_OPTIMIZERS_SMORMS3_UPDATE_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace optimization {

/**
 * SMORMS3 is an optimizer that estimates a safe and optimal distance based on
 * curvature and normalizing the stepsize in the parameter space. It is a hybrid
 * of RMSprop and Yann LeCun’s method in "No more pesky learning rates".
 *
 * For more information, see the following.
 *
 * @code
 * @misc{Funk2015,
 *   author    = {Simon Funk},
 *   title     = {RMSprop loses to SMORMS3 - Beware the Epsilon!},
 *   year      = {2015}
 * }
 * @endcode
 */

class SMORMS3Update
{
 public:
  /**
   * Construct the SMORMS3 update policy with given epsilon parameter.
   *
   * @param psilon Value used to initialise the mean squared gradient parameter.
   */
  SMORMS3Update(const double eps = 1e-16) : eps(eps)
  { /* Do nothing. */ };

  /**
   * The Initialize method is called by SGD::Optimize method with UpdatePolicy
   * SMORMS3Update before the start of the iteration update process.
   *
   * @param n_rows number of rows in the gradient matrix.
   * @param n_cols number of columns in the gradient matrix.
   * @param lRate 
   */
  void Initialize(const size_t n_rows,
                  const size_t n_cols)
  {
    // Initialise the parameters mem, g and g2.
    mem = arma::ones<arma::mat>(n_rows, n_cols);
    g = arma::zeros<arma::mat>(n_rows, n_cols);
    g2 = arma::zeros<arma::mat>(n_rows, n_cols);

    // Initialise a matrix to be filled with lRate.
    lRateMat = arma::zeros<arma::mat>(n_rows, n_cols);
  }

  /**
   * Update step for SMORMS3.
   *
   * @param iterate Parameter that minimizes the function.
   * @param stepSize Step size to be used for the given iteration.
   * @param gradient The gradient matrix.
   */
  void Update(arma::mat& iterate,
              double stepSize,
              arma::mat& gradient)
  {
    // Update the iterate.
    arma::mat r = 1 / (mem + 1);

    g = (1 - r) % g;
    g += r % gradient;

    g2 = (1 - r) % g2;
    g2 += r % (gradient % gradient);

    arma::mat x = (g % g) / (g2 + eps);

    lRateMat.fill(stepSize);

    iterate -= gradient % arma::min(x, lRateMat) / (arma::sqrt(g2) + eps);

    mem %= (1 - x);
    mem += 1;
  }
 private:
   //! The value used to initialise the mean squared gradient parameter.
   double eps;
   // The parameters mem, g and g2.
   arma::mat mem, g, g2;
   // The matrix to be filled with lRate.
   arma::mat lRateMat;
};

} // namespace optimization
} // namespace mlpack

#endif