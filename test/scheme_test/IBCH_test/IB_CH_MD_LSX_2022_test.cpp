#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
};

class IB_CH_MD_LSX_2022_Test : public testing::TestWithParam<TestParams>{};

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
    IB_CH_MD_LSX_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_MD_LSX_2022_Test, Test){
    IB_CH_MD_LSX_2022 ch(GetParam().curve);

    IB_CH_MD_LSX_2022_pp pp;
    IB_CH_MD_LSX_2022_msk msk;
    IB_CH_MD_LSX_2022_td td;
    IB_CH_MD_LSX_2022_h h;
    IB_CH_MD_LSX_2022_r r,r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *ID = ch.GetZrElement();

    ch.SetUp(pp, msk);

    ch.KeyGen(td, ID, msk, pp);

    ch.Hash(h, r, ID, m, pp);

    ASSERT_TRUE(ch.Check(h, r, ID, m, pp));

    ch.Adapt(r_p, h, m, r, m_p, td);

    ASSERT_TRUE(ch.Verify(h, r_p, ID, m_p, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






