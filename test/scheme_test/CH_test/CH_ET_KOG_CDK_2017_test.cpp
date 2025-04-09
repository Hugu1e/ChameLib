#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int group;
};

class CH_ET_KOG_CDK_2017_Test : public testing::TestWithParam<TestParams>{};

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
    CH_ET_KOG_CDK_2017_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_ET_KOG_CDK_2017_Test, Test){
    CH_ET_KOG_CDK_2017 ch(GetParam().curve, GetParam().group);

    CH_ET_KOG_CDK_2017_pp pp;
    CH_ET_KOG_CDK_2017_sk sk;
    CH_ET_KOG_CDK_2017_pk pk;
    CH_ET_KOG_CDK_2017_h h;
    CH_ET_KOG_CDK_2017_r r, r_p;
    CH_ET_KOG_CDK_2017_etd etd;

    element_s* m = ch.GetZrElement();
    element_s* m_p = ch.GetZrElement();

    ch.SetUp(pp);

    ch.KeyGen(sk, pk, pp, 1024);

    ch.Hash(h, r, etd, m, pk, pp);

    ASSERT_TRUE(ch.Check(h, m, r, pk, pp));

    ch.Adapt(r_p, m_p, h, m, r, pk, pp, sk, etd);

    ASSERT_TRUE(ch.Verify(h, m_p, r_p, pk, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



