#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int lamuda;
};

const TestParams test_values[] = {
    {128},
    {256},
    {512},
    {1024},
    {2048}
};

class CH_ET_BC_CDK_2017_Test : public testing::TestWithParam<TestParams>{};

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_ET_BC_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_ET_BC_CDK_2017_Test, Test){
    CH_ET_BC_CDK_2017 ch;
    CH_ET_BC_CDK_2017_pp pp;
    CH_ET_BC_CDK_2017_pk pk;
    CH_ET_BC_CDK_2017_sk sk;
    CH_ET_BC_CDK_2017_etd etd;
    CH_ET_BC_CDK_2017_h h;
    CH_ET_BC_CDK_2017_r r,r_p;

    std::string m = "123456";
    std::string m_p = "789101";

    ch.SetUp(pp, GetParam().lamuda);

    ch.KeyGen(pk, sk, pp);

    ch.Hash(h, r, etd, pp, pk, m);

    ASSERT_TRUE(ch.Check(h, r, pk, m));

    ch.Adapt(r_p, sk, etd, pk, h, r, m, m_p);

    ASSERT_TRUE(ch.Verify(h, r_p, pk, m_p));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

