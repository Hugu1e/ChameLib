#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int group;
};

class CH_KEF_DLP_LLA_2012_Test : public testing::TestWithParam<TestParams>{};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::D_159, Curve::D_201, Curve::D_224, Curve::D_105171_196_185, Curve::D_277699_175_167, Curve::D_278027_190_181,
        Curve::E,
        Curve::F, Curve::SM9,
        Curve::G_149
    };

    int groups[] = {Group::G1, Group::G2, Group::GT};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int group : groups) {
            test_params.push_back({curve, group});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	CH_Test,
    CH_KEF_DLP_LLA_2012_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_DLP_LLA_2012_Test, Test){
    CH_KEF_DLP_LLA_2012 ch(GetParam().curve, GetParam().group);

    CH_KEF_DLP_LLA_2012_pp pp;
    CH_KEF_DLP_LLA_2012_labelManager LM;
    CH_KEF_DLP_LLA_2012_sk sk;
    CH_KEF_DLP_LLA_2012_pk pk;
    CH_KEF_DLP_LLA_2012_label label;
        
    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    element_s *m_pp = ch.GetZrElement();
    element_s *r = ch.GetZrElement();
    element_s *r_p = ch.GetZrElement();
    element_s *r_pp = ch.GetZrElement();
    element_s *h = ch.GetG1Element();

    ch.SetUp(pp);

    ch.KeyGen(sk, pk, LM, pp);

    ch.Hash(h, r, label, pk, m, LM, pp);

    ASSERT_TRUE(ch.Check(m, r, pk, label, h, pp));

    ch.UForge(r_p, sk, pk, label, h, m, m_p, r, pp);

    ASSERT_TRUE(ch.Verify(m_p, r_p, pk, label, h, pp));

    ch.IForge(r_pp, label, m, m_p, r, r_p, m_pp);

    ASSERT_TRUE(ch.Verify(m_pp, r_pp, pk, label, h, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}






