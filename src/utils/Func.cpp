#include "utils/Func.h"

void Func::pbc_mpz_trace_n(mpz_t res, mpz_t q, mpz_t trace, int n) {
    int i;
    mpz_t c2, c1, c0, t0;
    mpz_inits(c2, c1, c0, t0, NULL);

    // c2 = 2
    mpz_set_ui(c2, 2);
    // c1 = trace
    mpz_set(c1, trace);

    for (i = 2; i <= n; i++) {
        // c0 = trace * c1
        mpz_mul(c0, trace, c1);
        // t0 = q * c2
        mpz_mul(t0, q, c2);
        // c0 -= t0
        mpz_sub(c0, c0, t0);
        // c2 = c1
        mpz_set(c2, c1);
        // c1 = c0
        mpz_set(c1, c0);
    }
    mpz_set(res, c1);
    mpz_clears(c2, c1, c0, t0, NULL);
}

void Func::pbc_mpz_curve_order_extn(mpz_t res, mpz_t q, mpz_t t, int k) {
    mpz_t tmp, tmp_2;
    mpz_inits(tmp, tmp_2, NULL);

    mpz_pow_ui(tmp, q, k);
    mpz_add_ui(tmp, tmp, 1);

    pbc_mpz_trace_n(tmp_2, q, t, k);

    mpz_sub(res, tmp, tmp_2);

    mpz_clears(tmp, tmp_2, NULL);
}

void Func::GetNdonr(mpz_t res, int group, int curve){
    mpz_set_ui(res, 1);
    switch(group){
        case Group::G1:
            return;
        case Group::GT:
            return;
        case Group::G2:
            switch (curve){
            case::Curve::A:
            case Curve::A1:
            case Curve::E:
                return;
            case Curve::D_159:
            case Curve::D_201:
            case Curve::D_224:
            case Curve::D_105171_196_185:
            case Curve::D_277699_175_167:
            case Curve::D_278027_190_181:
            {
                mpz_t q, n, r, t, k;
                mpz_inits(q, n, r, t, k, NULL);
                int k_;

                CurveParams::GetMpz(q, "q", curve);
                CurveParams::GetMpz(n, "n", curve);
                CurveParams::GetMpz(k, "k", curve);
                CurveParams::GetMpz(r, "r", curve);

                // k /= 2
                mpz_div_ui(k, k, 2);
                // k_ = k
                k_ = mpz_get_ui(k);

                mpz_sub(t, q, n);
                mpz_add_ui(t, t, 1);
                mpz_neg(t, t);
                
                pbc_mpz_curve_order_extn(res, q, t, k_);

                // res /= r
                mpz_div(res, res, r);

                mpz_clears(q, n, r, t, k, NULL);
                return;
            }
            case Curve::F:
            case Curve::SM9:
            {
                mpz_t q, r, t;
                mpz_inits(q, r, t, NULL);

                CurveParams::GetMpz(q, "q", curve);
                CurveParams::GetMpz(r, "r", curve);

                mpz_sub(t, q, r);
                mpz_add_ui(t, t, 1);

                pbc_mpz_curve_order_extn(res, q, t, 12);
                mpz_div(res, res, r);
                mpz_div(res, res, r);

                mpz_clears(q, r, t, NULL);
                return;
            }
                
            case Curve::G_149:
            {
                mpz_t q, n, r, t;
                mpz_inits(q, n, r, t, NULL);

                CurveParams::GetMpz(q, "q", curve);
                CurveParams::GetMpz(n, "n", curve);
                CurveParams::GetMpz(r, "r", curve);

                mpz_sub(t, q, n);
                mpz_add_ui(t, t, 1);
                mpz_neg(t, t);

                pbc_mpz_curve_order_extn(res, q, t, 5);
                mpz_div(res, res, r);

                mpz_clears(q, n, r, t, NULL);
                return;
            }
            default:
                throw std::runtime_error("Invalid curve");
            }
        default:
            throw std::runtime_error("Invalid group");
    }
}