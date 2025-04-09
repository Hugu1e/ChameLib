#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCH_DSS_2019_Test : public testing::TestWithParam<TestParams>{};

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

    int ks[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int k : ks) {
                test_params.push_back({curve, swap, k});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    PCH_DSS_2019_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCH_DSS_2019_Test, Test){
    PCH_DSS_2019 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const int SIZE_OF_ATTR = attr_list.size();
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    PCH_DSS_2019_pp ppPCH;
    PCH_DSS_2019_sk skPCH;
    PCH_DSS_2019_pk pkPCH;
    PCH_DSS_2019_sks sksPCH;

    PCH_DSS_2019_h h;
    PCH_DSS_2019_r r,r_p;

    std::string m = "message to hash";
    std::string m_p = "message to adapt";

    ch.SetUp(ppPCH, pkPCH, skPCH, GetParam().k);

    ch.KeyGen(sksPCH, skPCH, pkPCH, attr_list);

    ch.Hash(h, r, m, MSP, POLICY, pkPCH, ppPCH);

    ASSERT_TRUE(ch.Check(h, r, m, pkPCH));
    
    ch.Adapt(r_p, m_p, h, r, m, sksPCH, pkPCH, MSP, POLICY);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pkPCH));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}