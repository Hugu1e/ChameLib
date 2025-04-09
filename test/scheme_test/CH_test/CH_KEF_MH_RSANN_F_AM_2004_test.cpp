#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int k;
};

const TestParams test_values[] = {
    {512}
};

class CH_KEF_MH_RSANN_F_AM_2004_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_RSANN_F_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_RSANN_F_AM_2004_Test, Test){
    CH_KEF_MH_RSANN_F_AM_2004 ch;

    CH_KEF_MH_RSANN_F_AM_2004_pk pk;
    CH_KEF_MH_RSANN_F_AM_2004_sk sk;
    CH_KEF_MH_RSANN_F_AM_2004_h h;
    CH_KEF_MH_RSANN_F_AM_2004_r r, r_p;
    
    mpz_t m,m_p;
    mpz_t label;
    
    mpz_inits(m, m_p, label, NULL);
    RandomGenerator::RandomInLength(m, 64);
    RandomGenerator::RandomInLength(m_p, 64);
    RandomGenerator::RandomInLength(label, 64);
    
    ch.SetUp();

    ch.KeyGen(pk, sk, GetParam().k);

    ch.Hash(h, r, m, label, pk);

    ASSERT_TRUE(ch.Check(h, m, r, label, pk));

    mpz_set_ui(m_p, 96725346346246);

    ch.Adapt(r_p, m_p, h, m, r, label, sk, pk);

    ASSERT_TRUE(ch.Verify(h, m_p, r_p, label, pk));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


