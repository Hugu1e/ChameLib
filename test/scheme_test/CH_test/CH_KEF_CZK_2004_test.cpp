#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int group;
};

class CH_KEF_CZK_2004_Test : public testing::TestWithParam<TestParams>{};

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
    CH_KEF_CZK_2004_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_KEF_CZK_2004_Test, Test){
    CH_KEF_CZK_2004 ch(GetParam().curve, GetParam().group);
    CH_KEF_CZK_2004_pp pp;
    CH_KEF_CZK_2004_pk pk;
    CH_KEF_CZK_2004_sk sk;
    CH_KEF_CZK_2004_r r,r_p;
    element_s *I = ch.GetG1Element();
    element_s *h = ch.GetG1Element();
    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    ch.SetUp(pp);

    ch.KeyGen(pk, sk, pp);

    ch.Hash(h, r, m, I, pk, pp);

    ASSERT_TRUE(ch.Check(I, m, r, h, pp));

    ch.Adapt(r_p, sk, h, m, r, m_p, I, pp);

    ASSERT_TRUE(ch.Verify(I, m_p, r_p, h, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}




