#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    bool swap;
    int k;
};

class PCHBA_TLL_2020_Test : public testing::TestWithParam<TestParams>{};

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

    int ks[] = {8, 16, 24};

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
    PCHBA_TLL_2020_Test,
	testing::ValuesIn(test_values)
);

TEST_P(PCHBA_TLL_2020_Test, Test){
    PCHBA_TLL_2020 ch(GetParam().curve, GetParam().swap);

    const std::string POLICY = "A&(DDDD|(BB&CCC))";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    std::vector<std::string> S1 = {"A","DDDD"};
    const int SIZE_OF_S1 = S1.size();

    std::vector<std::string> S2 = {"BB","CCC"};
    const int SIZE_OF_S2 = S2.size();

    int K = GetParam().k;
    const int U1 = K/3;  // length of U1
    const int U2 = K/2;  // length of U2
    PCHBA_TLL_2020_ID ID12;
    ID12.get_IDABET().init(K);
    for(int i = 1;i<=K;i++){
        element_s *tmp_Zn = ch.GetZrElement();

        ID12.get_IDABET().set(i-1, tmp_Zn);
        element_clear(tmp_Zn);
    }

    PCHBA_TLL_2020_sk skPCHBA;
    PCHBA_TLL_2020_pk pkPCHBA;
    
    PCHBA_TLL_2020_sks sksPCHBA_1, sksPCHBA_2;
    PCHBA_TLL_2020_h h1, h2;
    PCHBA_TLL_2020_r r1, r2, r_p;

    element_s *m1 = ch.GetZrElement();
    element_s *m2 = ch.GetZrElement();
    element_s *m_p = ch.GetZrElement();

    ch.SetUp(pkPCHBA, skPCHBA, K);
    
    ch.KeyGen(sksPCHBA_1, pkPCHBA, skPCHBA, S1, ID12, U1);
    
    ch.KeyGen(sksPCHBA_2, pkPCHBA, skPCHBA, S2, ID12, U2);
    
    ch.Hash(h1, r1, m1, pkPCHBA, skPCHBA, MSP, ID12, U1);

    ASSERT_TRUE(ch.Check(h1, r1, m1, pkPCHBA));

    ch.Adapt(r_p, m_p, h1, r1, m1, MSP, ID12, U1, pkPCHBA, skPCHBA, sksPCHBA_1);

    ASSERT_TRUE(ch.Verify(h1, r_p, m_p, pkPCHBA));

    ch.Hash(h2, r2, m2, pkPCHBA, skPCHBA, MSP, ID12, U2);

    ASSERT_TRUE(ch.Check(h2, r2, m2, pkPCHBA));

    try{
        ch.Adapt(r_p, m_p, h2, r2, m2, MSP, ID12, U2, pkPCHBA, skPCHBA, sksPCHBA_2);
    }catch(const std::runtime_error& e){}

    ch.Adapt(r_p, m_p, h2, r2, m2, MSP, ID12, U1, pkPCHBA, skPCHBA, sksPCHBA_1);

    ASSERT_TRUE(ch.Verify(h2, r_p, m_p, pkPCHBA));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}