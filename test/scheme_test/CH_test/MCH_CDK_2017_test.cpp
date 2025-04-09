#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {256},
    {512},
    {1024},
    {2048}
};

class MCH_CDK_2017_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    MCH_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MCH_CDK_2017_Test, Test){
    MCH_CDK_2017 ch;
    MCH_CDK_2017_pk pk;
    MCH_CDK_2017_sk sk;
    MCH_CDK_2017_h h;
    MCH_CDK_2017_r r,r_p;

    mpz_t m, m_p;
    mpz_inits(m, m_p, NULL);

    ch.SetUp();

    ch.KeyGen(pk, sk, GetParam().lamuda);

    RandomGenerator::RandomInLength(m, 100);

    ch.Hash(h, r, m, pk);

    ASSERT_TRUE(ch.Check(h, r, m, pk));

    RandomGenerator::RandomInLength(m_p, 100);
    
    ch.Adapt(r_p, m_p, m, r, h, sk, pk);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pk));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

