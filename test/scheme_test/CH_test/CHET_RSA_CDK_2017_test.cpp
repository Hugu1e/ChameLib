#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {128},
    {256},
    {512},
};

class CHET_RSA_CDK_2017_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CHET_RSA_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CHET_RSA_CDK_2017_Test, Test){
    CHET_RSA_CDK_2017 ch;
    CHET_RSA_CDK_2017_pk pk;
    CHET_RSA_CDK_2017_sk sk;
    CHET_RSA_CDK_2017_h h;
    CHET_RSA_CDK_2017_etd etd;

    mpz_t r, r_p;
    mpz_t m, m_p;
    mpz_inits(r, r_p, m, m_p, NULL);

    ch.SetUp();

    ch.KeyGen(pk, sk, GetParam().lamuda);

    RandomGenerator::RandomInLength(m, 100);

    ch.Hash(h, r, etd, m, pk);

    ASSERT_TRUE(ch.Check(h, r, m, pk));

    RandomGenerator::RandomInLength(m_p, 100);

    ch.Adapt(r_p, m_p, m, r, h, sk, etd, pk);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pk));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

