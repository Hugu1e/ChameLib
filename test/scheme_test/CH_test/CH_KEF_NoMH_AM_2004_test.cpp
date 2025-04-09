#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int k;
};

const TestParams test_values[] = {
    {512}
};

class CH_KEF_NoMH_AM_2004_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_NoMH_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_NoMH_AM_2004_Test, Test){
    CH_KEF_NoMH_AM_2004 ch = CH_KEF_NoMH_AM_2004();

    CH_KEF_NoMH_AM_2004_pk pk;
    CH_KEF_NoMH_AM_2004_sk sk;
    CH_KEF_NoMH_AM_2004_h h;
    CH_KEF_NoMH_AM_2004_r r, r_p;

    mpz_t m,m_p;
    mpz_inits(m, m_p, NULL);
    RandomGenerator::RandomInLength(m, 1024);
    RandomGenerator::RandomInLength(m_p, 1024);

    ch.Setup();

    ch.KeyGen(pk, sk, GetParam().k);

    ch.Hash(h, r, m, pk);

    ASSERT_TRUE(ch.Check(h, r, m, pk));

    ch.Adapt(r_p, m_p, h, r, m, pk, sk);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pk));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


