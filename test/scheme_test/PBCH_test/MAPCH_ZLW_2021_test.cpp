#include "ChameLib.h"
#include <gtest/gtest.h>

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class MAPCH_ZLW_2021_Test : public testing::TestWithParam<TestParams>{};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};
    // int lamudas[] = {256, 512, 1024};
    int lamudas[] = {32, 64, 128};

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
    MAPCH_ZLW_2021_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MAPCH_ZLW_2021_Test, Test){
    MAPCH_ZLW_2021 ch(GetParam().curve);

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
    std::vector<std::string> As = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = As.size();
    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    MAPCH_ZLW_2021_pp pp;

    CH_ET_BC_CDK_2017_pk pkCH;
    CH_ET_BC_CDK_2017_sk skCH;
    MA_ABE_gpk gpkABE;

    std::vector<MAPCH_ZLW_2021_mhk *> mhks;
    std::vector<MAPCH_ZLW_2021_mtk *> mtks;
    std::vector<MAPCH_ZLW_2021_mski *> mskis;
    MAPCH_ZLW_2021_h h;
    MAPCH_ZLW_2021_r r,r_p;

    std::string m = "message";
    std::string m_p = "message_p";

    ch.GlobalSetup(pkCH, skCH, pp, GetParam().lamuda);
    
    for(int i=0; i<GetParam().authNum; i++){
        MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
        MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();

        if (i < SIZE_OF_ATTRIBUTES) {
            ch.AuthSetUp(*mhk, *mtk, As[i], pkCH, skCH, pp);
        }else{
            ch.AuthSetUp(*mhk, *mtk, "ATTRIBUTE@AUTHORITY_" + std::to_string(i), pkCH, skCH, pp);
        }
        
        mhks.push_back(mhk);
        mtks.push_back(mtk);
    }

    for(int i=0; i<GetParam().authNum; i++){
        MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();
        if(i < SIZE_OF_ATTRIBUTES){
            ch.KeyGen(*mski, *mtks[i], *mhks[i], As[i], GID);
        }else{
            ch.KeyGen(*mski, *mtks[i], *mhks[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(i), GID);
        }
        mskis.push_back(mski);
    }

    ch.Hash(h, r, m, pp, mhks, MSP, POLICY);

    ASSERT_TRUE(ch.Check(h, r, m, mhks));

    // mskis.pop_back();
    ch.Adapt(r_p, m_p, h, r, m, mhks, mskis, MSP);

    ASSERT_TRUE(ch.Verify(h, r_p, m_p, mhks));
}

int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}