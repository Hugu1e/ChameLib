#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    bool swap;
};

class IB_CH_KEF_CZS_2014_Test : public testing::TestWithParam<TestParams>{};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::D_159, Curve::D_201, Curve::D_224, Curve::D_105171_196_185, Curve::D_277699_175_167, Curve::D_278027_190_181,
        Curve::E,
        Curve::F, Curve::SM9,
        Curve::G_149
    };

    bool swaps[] = {false, true};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	IBCH_Test,
    IB_CH_KEF_CZS_2014_Test,
	testing::ValuesIn(test_values)
);

TEST_P(IB_CH_KEF_CZS_2014_Test, Test){
    IB_CH_KEF_CZS_2014 ch(GetParam().curve, GetParam().swap);

    IB_CH_KEF_CZS_2014_pp pp;
    IB_CH_KEF_CZS_2014_msk msk;
    IB_CH_KEF_CZS_2014_td td;
    IB_CH_KEF_CZS_2014_h h;
    IB_CH_KEF_CZS_2014_r r,r_p;

    std::string ID = "identity string ID";
    std::string L = "customozed identity L";

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    ch.SetUp(pp, msk);

    ch.Extract(td, ID, msk);

    ch.Hash(h, r, ID, L, m, pp);

    ASSERT_TRUE(ch.Check(h, r, L, m, td));

    ch.Adapt(r_p, m_p, h, r, L, m, td);

    ASSERT_TRUE(ch.Verify(h, r_p, L, m_p, td));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



