#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    bool swap;
    int leafNodeSize;
};

class RPCH_TMM_2022_Test : public testing::TestWithParam<TestParams>{};

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

    int leadNodeSizes[] = {8, 16, 32, 64};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            for (int leafNodeSize : leadNodeSizes) {
                test_params.push_back({curve, swap, leafNodeSize});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    RPCH_TMM_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(RPCH_TMM_2022_Test, Test){
    RPCH_TMM_2022 ch(GetParam().curve, GetParam().swap);

    std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    element_s *id_1, *id_2, *id_3;
    id_1 = ch.GetZrElement();
    id_2 = ch.GetZrElement();
    id_3 = ch.GetZrElement();

    // T1 < T3 < T_present < T2
    const time_t T_present = TimeUtils::TimeCast(2025, 2, 1, 0, 0, 0);  // present time
    const time_t re_time_1 = TimeUtils::TimeCast(2025, 1, 1, 0, 0, 0);
    const time_t re_time_2 = TimeUtils::TimeCast(2025, 2, 31, 0, 0, 0);
    const time_t re_time_3 = TimeUtils::TimeCast(2025, 1, 2, 0, 0, 0);
    
    RPCH_TMM_2022_sk skRPCH;
    RPCH_TMM_2022_pk pkRPCH;

    RPCH_TMM_2022_skid skidRPCH_1;
    RPCH_TMM_2022_skid skidRPCH_2;
    RPCH_TMM_2022_skid skidRPCH_3;

    RPCH_TMM_2022_dkidt dkidtRPCH_1;
    RPCH_TMM_2022_dkidt dkidtRPCH_2;
    RPCH_TMM_2022_dkidt dkidtRPCH_3;

    RPCH_TMM_2022_kut kut;

    RPCH_TMM_2022_h h;
    RPCH_TMM_2022_r r, r_p;

    RPCH_TMM_2022_RevokedPresonList rl;
    RPCH_TMM_2022_Binary_tree st;

    element_s *m = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();
    
    ch.SetUp(skRPCH, pkRPCH, rl, st, GetParam().leafNodeSize);
    
    ch.KeyGen(skidRPCH_1, pkRPCH, skRPCH, st, attr_list, id_1, re_time_1);
    ch.KeyGen(skidRPCH_2, pkRPCH, skRPCH, st, attr_list, id_2, re_time_2);
    ch.KeyGen(skidRPCH_3, pkRPCH, skRPCH, st, attr_list, id_3, re_time_3);

    ch.Rev(rl, id_1, re_time_1);
    ch.Rev(rl, id_2, re_time_2);
    ch.Rev(rl, id_3, re_time_3);

    ch.Hash(h, r, m, pkRPCH, MSP, T_present);
        
    ASSERT_TRUE(ch.Check(h, r, m, pkRPCH));
    
    ch.KUpt(kut, pkRPCH, st, rl, T_present);
    
    try{
        ch.DKGen(dkidtRPCH_1, pkRPCH, skidRPCH_1, kut);
    }catch(const std::runtime_error& e){}
    try{
        ch.DKGen(dkidtRPCH_3, pkRPCH, skidRPCH_3, kut);
    }catch(const std::runtime_error& e){}

    ch.DKGen(dkidtRPCH_2, pkRPCH, skidRPCH_2, kut);

    ch.Adapt(r_p, m_p, h, r, m, pkRPCH, dkidtRPCH_2, MSP);
    
    ASSERT_TRUE(ch.Verify(h, r_p, m_p, pkRPCH));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}