#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int group;
};

class CH_FS_ECC_CCT_2024_Test : public testing::TestWithParam<TestParams>{};

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
    CH_FS_ECC_CCT_2024_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CH_FS_ECC_CCT_2024_Test, Test){
    CH_FS_ECC_CCT_2024 ch(GetParam().curve, GetParam().group);

    CH_FS_ECC_CCT_2024_pp pp;
    CH_FS_ECC_CCT_2024_pk pk;
    CH_FS_ECC_CCT_2024_sk sk;
    CH_FS_ECC_CCT_2024_r r, r_p;
    
    element_s* m;
    element_s* m_p;
    element_s* h;

    m = ch.GetZrElement();
    m_p = ch.GetZrElement();
    h = ch.GetG1Element();
    
    ch.SetUp(pp);

    ch.KeyGen(pk, sk, pp);

    ch.Hash(h, r, pk, m, pp);

    ASSERT_TRUE(ch.Check(pk, m, h, r, pp));

    ch.Adapt(r_p, pk, sk, m, m_p, h, r, pp);

    ASSERT_TRUE(ch.Verify(pk, m_p, h, r_p, pp));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}




