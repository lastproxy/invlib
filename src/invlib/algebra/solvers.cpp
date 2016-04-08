// ------------------ //
//  Standard Solver   //
// ------------------ //

template<typename VectorType, typename MatrixType>
auto Standard::solve(const MatrixType &A,const VectorType &v)
    -> VectorType
{
    return A.solve(v);
}

// -------------------------  //
//  Conjugate Gradient Solver //
// -------------------------  //

ConjugateGradient::ConjugateGradient(double tol)
    : tolerance(tol)
{
    // Nothing to do here.
}

template
<
typename VectorType,
typename MatrixType,
template <LogType> class Log
>
auto ConjugateGradient::solve(const MatrixType &A,
                              const VectorType &v,
                              int verbosity)
    -> VectorType
{
    using RealType = typename VectorType::RealType;

    Log<LogType::SOL_CG> log(verbosity);

    unsigned int n = v.rows();
    RealType tol, alpha, beta, rnorm, vnorm;
    VectorType x, r, p, xnew, rnew, pnew;

    x = v;
    r = A * x - v;
    p = -1.0 * r;
    vnorm = v.norm();
    rnorm = r.norm();

    log.init(tolerance, rnorm, vnorm);

    int i = 0;
    while (rnorm / vnorm > tolerance)
    {
        alpha = dot(r, r) / dot(p, A * p);
        xnew  = x + alpha *     p;
        rnew  = r + alpha * A * p;
        beta  = dot(rnew, rnew) / dot(r, r);
        pnew  = beta * p - rnew;

        x = xnew;
        r = rnew; rnorm = r.norm();
        p = pnew;

        i++;
        if (i % 10 == 0)
            log.step(i, rnorm);
    }
    log.finalize(i);

    return x;
}

// -----------------------  //
//  Preconditioned Solver   //
// -----------------------  //

template<typename Solver, typename Transformation>
PreconditionedSolver<Solver, Transformation>::
PreconditionedSolver(Solver s, Transformation t)
    : solver(s), transformation(t)
{
    // Nothin to do here.
}

template<typename Solver, typename Transformation>
    template<typename VectorType, typename MatrixType>
auto PreconditionedSolver<Solver, Transformation>::solve(const MatrixType &A,
                                                         const VectorType &v)
    -> VectorType
{
    VectorType vt = transformation.apply(v);
    auto At       = transformation.apply(A);
    VectorType x  = transformation.apply(solver.solve(At, vt));
    return x;
}
