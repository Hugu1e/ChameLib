#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
};

class CH_KEF_MH_SDH_DL_AM_2004_Test : public testing::TestWithParam<TestParams>{};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        test_params.push_back({curve});
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_MH_SDH_DL_AM_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_MH_SDH_DL_AM_2004_Test, Test){
    CH_KEF_MH_SDH_DL_AM_2004 ch(GetParam().curve);
        
    CH_KEF_MH_SDH_DL_AM_2004_pp pp;
    CH_KEF_MH_SDH_DL_AM_2004_sk sk;
    CH_KEF_MH_SDH_DL_AM_2004_pk pk;
    CH_KEF_MH_SDH_DL_AM_2004_h h;
    CH_KEF_MH_SDH_DL_AM_2004_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *label = ch.GetZrElement();

    ch.SetUp(pp);

    ch.KeyGen(pk, sk, pp);

    ch.Hash(h, r, m, label, pk, pp);

    ASSERT_TRUE(ch.Check(h, r, m, label, pk, pp));

    ch.Adapt(r_p, m_p, h, r, m, label, sk, pp);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, label, pk, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






