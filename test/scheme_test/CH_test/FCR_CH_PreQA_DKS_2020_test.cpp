#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int group;
};

class FCR_CH_PreQA_DKS_2020_Test : public testing::TestWithParam<TestParams>{};

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
    FCR_CH_PreQA_DKS_2020_Test,
	testing::ValuesIn(test_values)
);

TEST_P(FCR_CH_PreQA_DKS_2020_Test, Test){
    FCR_CH_PreQA_DKS_2020 ch(GetParam().curve, GetParam().group);

    FCR_CH_PreQA_DKS_2020_pp pp;
    FCR_CH_PreQA_DKS_2020_pk pk;
    FCR_CH_PreQA_DKS_2020_sk sk;
    FCR_CH_PreQA_DKS_2020_h h;
    FCR_CH_PreQA_DKS_2020_r r, r_p;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    ch.SetUp(pp);

    ch.KeyGen(pk, sk, pp);

    ch.Hash(h, r, m, pk, pp);

    ASSERT_TRUE(ch.Check(h, r, m, pk, pp));

    ch.Adapt(r_p, m_p, m, h, r, sk, pk, pp);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pk, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}