/**
 * @file hmm_viterbi_main.cpp
 * @author Ryan Curtin
 *
 * Compute the most probably hidden state sequence of a given observation
 * sequence for a given HMM.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include <mlpack/prereqs.hpp>
#include <mlpack/core/util/cli.hpp>
#include <mlpack/core/util/mlpack_main.hpp>

#include "hmm.hpp"
#include "hmm_model.hpp"

#include <mlpack/methods/gmm/gmm.hpp>

using namespace mlpack;
using namespace mlpack::hmm;
using namespace mlpack::distribution;
using namespace mlpack::util;
using namespace mlpack::gmm;
using namespace arma;
using namespace std;

PROGRAM_INFO("Hidden Markov Model (HMM) Viterbi State Prediction", "This "
    "utility takes an already-trained HMM, specified as " +
    PRINT_PARAM_STRING("input_model") + ", and evaluates the most probable "
    "hidden state sequence of a given sequence of observations (specified as "
    "'" + PRINT_PARAM_STRING("input") + ", using the Viterbi algorithm.  The "
    "computed state sequence may be saved using the " +
    PRINT_PARAM_STRING("output") + " output parameter."
    "\n\n"
    "For example, to predict the state sequence of the observations " +
    PRINT_DATASET("obs") + " using the HMM " + PRINT_MODEL("hmm") + ", "
    "storing the predicted state sequence to " + PRINT_DATASET("states") +
    ", the following command could be used:"
    "\n\n" +
    PRINT_CALL("hmm_viterbi", "input", "obs", "input_model", "hmm", "output",
        "states"));

PARAM_MATRIX_IN_REQ("input", "Matrix containing observations,", "i");
PARAM_MODEL_IN_REQ(HMMModel, "input_model", "Trained HMM to use.", "m");
PARAM_UMATRIX_OUT("output", "File to save predicted state sequence to.", "o");

// Because we don't know what the type of our HMM is, we need to write a
// function that can take arbitrary HMM types.
struct Viterbi
{
  template<typename HMMType>
  static void Apply(HMMType& hmm, void* /* extraInfo */)
  {
    std::cout << __func__ << ": In PerformAction<Viterbi>" << std::endl;
    // Load observations.
    mat dataSeq = std::move(CLI::GetParam<arma::mat>("input"));
    std::cout << __func__ << ": Data read:" << std::endl << dataSeq << std::endl;

    // See if transposing the data could make it the right dimensionality.
    std::cout << __func__ << ": dataSeq.n_cols = " << dataSeq.n_cols << std::endl;
    std::cout << __func__ << ": hmm.Emission.size() = " << hmm.Emission().size() << std::endl;
    std::cout << __func__ << ": hmm.Emission()[0].Dimensionality() = " << hmm.Emission()[0].Dimensionality() << std::endl;
    if ((dataSeq.n_cols == 1) && (hmm.Emission()[0].Dimensionality() == 1))
    {
      Log::Info << "Data sequence appears to be transposed; correcting."
          << endl;
      std::cout << "Data sequence appears to be transposed; correcting."
          << endl;
      dataSeq = dataSeq.t();
    }
    std::cout << __func__ << ": Corrected dataSeq orientation!" << std::endl;

    // Verify correct dimensionality.
    std::cout << __func__ << ": dataSeq.n_rows = " << dataSeq.n_rows << std::endl;
    std::cout << __func__ << ": hmm.Emission()[0].Dimensionality() = " << hmm.Emission()[0].Dimensionality() << std::endl;
    if (dataSeq.n_rows != hmm.Emission()[0].Dimensionality())
    {
      Log::Fatal << "Observation dimensionality (" << dataSeq.n_rows << ") "
          << "does not match HMM Gaussian dimensionality ("
          << hmm.Emission()[0].Dimensionality() << ")!" << endl;
    }
    std::cout << __func__ << ": Verified correct dimensionality!" << std::endl;

    arma::Row<size_t> sequence;
    std::cout << __func__ << ": Calling hmm.Predict()" << std::endl;
    hmm.Predict(dataSeq, sequence);

    // Save output.
    CLI::GetParam<arma::Mat<size_t>>("output") = std::move(sequence);
    std::cout << __func__ << ": Predicted state sequence:" << std::endl << CLI::GetParam<arma::Mat<size_t>>("output") << std::endl;
  }
};

static void mlpackMain()
{
  RequireAtLeastOnePassed({ "output" }, false, "no results will be saved");

  std::cout << __func__ << ": calling PerformAction<Viterbi>" << std::endl;
  CLI::GetParam<HMMModel*>("input_model")->PerformAction<Viterbi>((void*) NULL);
}
