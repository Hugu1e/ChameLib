#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class DPCH_MXN_2022_Test : public testing::TestWithParam<TestParams>{};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};
    int lamudas[] = {256, 512, 1024};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            for (int lamuda : lamudas) {
                test_params.push_back({curve, authNum, lamuda});
            }
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	PBCH_Test,
    DPCH_MXN_2022_Test,
	testing::ValuesIn(test_values)
);

TEST_P(DPCH_MXN_2022_Test, Test){
    DPCH_MXN_2022 ch(GetParam().curve);

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    std::vector<std::string> postfix_expression = Policy_resolution::infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = Policy_resolution::postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    Element_t_matrix* MSP = Policy_generation::getPolicyInMatrixFormFromTree(binary_tree_expression);

    // attributes
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<DPCH_MXN_2022_pkTheta *> pkThetas;
    std::vector<DPCH_MXN_2022_skTheta *> skThetas;
    std::vector<DPCH_MXN_2022_skGidA *> skGidAs;

    DPCH_MXN_2022_pp ppDPCH;
    DPCH_MXN_2022_pk pkDPCH;
    DPCH_MXN_2022_sk skDPCH;
    DPCH_MXN_2022_skGid skGid;
    DPCH_MXN_2022_sigmaGid sigmaGid;
    DPCH_MXN_2022_h h;
    DPCH_MXN_2022_r r,r_p;
    std::string m = "message";
    std::string m_p = "message_p";

    ch.SetUp(ppDPCH, pkDPCH, skDPCH, GetParam().lamuda);

    ch.ModSetUp(skGid, sigmaGid, skDPCH, GID);

    for(int i = 0; i < GetParam().authNum; i++) {
        DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
        DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;

        if(i < SIZE_OF_ATTRIBUTES){
            ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, ATTRIBUTES[i]);
        }else{
            ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
        }

        pkThetas.push_back(pkTheta);
        skThetas.push_back(skTheta);
    }

    for(int i = 0; i < GetParam().authNum; i++) {
        DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;

        if (i < SIZE_OF_ATTRIBUTES) {
            ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], ATTRIBUTES[i]);
        } else {
            ch.ModKeyGen(*skGidA, ppDPCH, pkDPCH, GID, sigmaGid, *skThetas[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
        }
        
        skGidAs.push_back(skGidA);
    }

    ch.Hash(h, r, m, ppDPCH, pkDPCH, pkThetas, MSP, POLICY);

    ASSERT_TRUE(ch.Check(pkDPCH, m, h, r));

    // choose partial secret keys
    std::vector<DPCH_MXN_2022_skGidA *> _skGidAs;
    _skGidAs.push_back(skGidAs[0]);
    _skGidAs.push_back(skGidAs[1]);
    _skGidAs.push_back(skGidAs[2]);

    ch.Adapt(r_p, m_p, h, r, m, pkDPCH, skGid, _skGidAs, ppDPCH, pkThetas, MSP, POLICY);

    ASSERT_TRUE(ch.Verify(pkDPCH, m_p, h, r_p));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}