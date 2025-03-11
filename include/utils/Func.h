#ifndef CHAMELIB_FUNC_H
#define CHAMELIB_FUNC_H

#include <pbc/pbc.h>
#include <stdexcept>
#include <string>

#include "../curve/params.h"

class Func{
    public:
        static void pbc_mpz_trace_n(mpz_t res, mpz_t q, mpz_t trace, int n);

        static void pbc_mpz_curve_order_extn(mpz_t res, mpz_t q, mpz_t t, int k);
        
        static void GetNdonr(mpz_t res, int group, int curve);
};

#endif  // CHAMELIB_FUNC_H