#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int k;
    int tau;
};

const TestParams test_values[] = {
    {1024, 512}
};

class CH_KEF_MH_RSA_F_AM_2004_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSA_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSA_F_AM_2004_Test, Test){
    CH_KEF_MH_RSA_F_AM_2004 ch;

    CH_KEF_MH_RSA_F_AM_2004_pk pk;
    CH_KEF_MH_RSA_F_AM_2004_sk sk;
    mpz_t h;
    mpz_t B;
    mpz_t m,m_p;
    mpz_t label;
    mpz_t r, r_p;
    mpz_inits(h, B, r, m, label, r_p, m_p, NULL);

    ch.SetUp();

    ch.KeyGen(pk, sk, GetParam().k, GetParam().tau);

    mpz_set_ui(m, 42525346346746);
    mpz_set_ui(label, 424253532414);
 
    ch.Hash(h, r, B, pk, sk, m, label);

    ASSERT_TRUE(ch.Check(pk, m, label, h, r));

    mpz_set_ui(m_p, 96725346346246);

    ch.Adapt(r_p, pk, m, m_p, label, h, B, r);

    ASSERT_TRUE(ch.Verify(pk, m_p, label, h, r_p));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

