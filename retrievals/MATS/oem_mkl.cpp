#include "invlib/map.h"
#include "invlib/algebra.h"
#include "invlib/optimization.h"
#include "invlib/io.h"
#include "invlib/profiling/timer.h"
#include "invlib/mkl/mkl_sparse.h"

#include <string>

using namespace invlib;

template
<
typename MatrixType,
typename VectorType
>
class LinearModel
{
public:

    LinearModel(const MatrixType &K_, const VectorType &xa_)
        : K(K_), xa(xa_), m(K_.rows()), n(K_.cols())
    {
        // Nothing to do here.
    }

    VectorType evaluate(const VectorType &x)
    {
        return K * (x - xa);
    }

    const MatrixType & Jacobian(const VectorType &x, VectorType &y)
    {
        y = K * (x - xa);
        return K;
    }

    const unsigned int m, n;

private:

    const MatrixType &K;
    const VectorType &xa;

};

int main(int argc, const char ** argv)
{
    // Define types.
    using MatrixType = Matrix<Timer<MklSparse<float, Representation::Hybrid>>>;
    using VectorType = Vector<Timer<BlasVector<float>>>;

    using SolverType      = invlib::ConjugateGradient<CGStepLimit<1000>>;
    using MinimizerType   = invlib::GaussNewton<float, SolverType>;
    using PrecisionMatrix = invlib::PrecisionMatrix<MatrixType>;
    using MAPType         = invlib::MAP<LinearModel<MatrixType, VectorType>,
                                        MatrixType,
                                        PrecisionMatrix,
                                        PrecisionMatrix>;

    std::string path{};
    if (argc > 1)
    {
        path = argv[1];
    }

    std::cout << "Loading data from " << path + "/." << std::endl;

    // Load data.
    MatrixType K(read_matrix_arts<float>(path + "/K.xml"));
    MatrixType SaInv(read_matrix_arts<float>(path + "/SaInv.xml"));
    MatrixType SeInv(read_matrix_arts<float>(path + "/SeInv.xml"));
    VectorType xa(read_vector_arts<float>(path + "/xa.xml"));
    VectorType  y(read_vector_arts<float>(path + "/y.xml"));

    std::cout << "Starting OEM:" << std::endl;

    PrecisionMatrix Pa(SaInv);
    PrecisionMatrix Pe(SeInv);

    // Setup OEM.
    SolverType                          cg(1e-6, 0);
    MinimizerType                       gn(1e-6, 10, cg);
    LinearModel<MatrixType, VectorType> F(K, xa);
    MAPType                             oem(F, xa, Pa, Pe);

    // Run OEM.
    VectorType x;

    oem.compute(x, y, gn, 2);

    write_vector_arts("x.xml", x, Format::Binary);

}
