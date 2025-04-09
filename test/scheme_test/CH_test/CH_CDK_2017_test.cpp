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

class CH_CDK_2017_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
	CH_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_CDK_2017_Test, Test){
    CH_CDK_2017 ch;

    CH_CDK_2017_pk pk;
    CH_CDK_2017_sk sk;
    CH_CDK_2017_h h;
    CH_CDK_2017_r r, r_p;
    mpz_t m, tag, m_p, tag_p;
    mpz_inits(m, tag, m_p, tag_p, NULL);

    ch.SetUp();

    ch.KeyGen(pk, sk, GetParam().lamuda);

    RandomGenerator::RandomN(m, pk.get_rsa_pk()[AE_RSA::n]);
    RandomGenerator::RandomN(tag, pk.get_rsa_pk()[AE_RSA::n]);

    ch.Hash(h, r, m, tag, pk);

    ASSERT_TRUE(ch.Check(h, r, m, tag, pk));

    RandomGenerator::RandomN(m_p, pk.get_rsa_pk()[AE_RSA::n]);

    ch.Adapt(r_p, tag_p, h, r, m_p, sk, pk);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, tag_p, pk));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

