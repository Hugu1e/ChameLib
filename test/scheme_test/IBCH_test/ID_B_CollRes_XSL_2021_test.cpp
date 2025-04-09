#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    bool swap;
    int length;
};

class ID_B_CollRes_XSL_2021_Test : public testing::TestWithParam<TestParams>{};

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

    int lengths[] = {64, 128, 256};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for(int len: lengths){
                test_params.push_back({curve, swap, len});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	IBCH_Test,
    ID_B_CollRes_XSL_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(ID_B_CollRes_XSL_2021_Test, Test){
    ID_B_CollRes_XSL_2021 ch(GetParam().curve, GetParam().swap);

    ID_B_CollRes_XSL_2021_pp pp;
    ID_B_CollRes_XSL_2021_msk msk;
    ID_B_CollRes_XSL_2021_tk tk;
    ID_B_CollRes_XSL_2021_h h;
    ID_B_CollRes_XSL_2021_r r,r_p;

    std::string I = "IDENTITYIDENTITYIDENTITYIDENTITY";
    
    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();    

    ch.SetUp(pp, msk, GetParam().length);

    ch.KeyGen(tk, msk, I.c_str(), pp);
    
    ch.Hash(h, r, m, I.c_str(), pp);

    ASSERT_TRUE(ch.Check(h, m, r, I.c_str(), pp));

    ch.Adapt(r_p, m_p, h, m, r, tk);
    
    ASSERT_TRUE(ch.Verify(h, m_p, r_p, I.c_str(), pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



