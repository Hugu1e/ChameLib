#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
};

class IB_CH_ZSS_S2_2003_Test : public testing::TestWithParam<TestParams>{};

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
	IBCH_Test,
    IB_CH_ZSS_S2_2003_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_ZSS_S2_2003_Test, Test){
    IB_CH_ZSS_S2_2003 ch(GetParam().curve);

    IB_CH_ZSS_S2_2003_pp pp;
    IB_CH_ZSS_S2_2003_msk msk;
    IB_CH_ZSS_S2_2003_sk sk;
    IB_CH_ZSS_S2_2003_h h;
    IB_CH_ZSS_S2_2003_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *ID = ch.GetZrElement();

    ch.SetUp(pp, msk);

    ch.Extract(sk, msk, ID, pp);

    ch.Hash(h, r, m, ID, pp);

    ASSERT_TRUE(ch.Check(h, m, r, ID, pp));

    ch.Adapt(r_p, m_p, h, m, r, ID, sk, pp);

    ASSERT_TRUE(ch.Verify(h, m_p, r_p, ID, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}