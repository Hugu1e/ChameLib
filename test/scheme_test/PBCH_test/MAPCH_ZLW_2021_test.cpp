#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class MAPCH_ZLW_2021_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
            if(visiable)printf("%s %s authNum:%d lamuda:%d\n", testName.c_str(), curveName.c_str(), GetParam().authNum, GetParam().lamuda);
        }
};

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

    MAPCH_ZLW_2021_pp pp[repeat];

    CH_ET_BC_CDK_2017_pk pkCH[repeat];
    CH_ET_BC_CDK_2017_sk skCH[repeat];
    MA_ABE_gpk gpkABE[repeat];

    std::vector<std::vector<MAPCH_ZLW_2021_mhk *>> mhks(repeat);
    std::vector<std::vector<MAPCH_ZLW_2021_mtk *>> mtks(repeat);
    std::vector<std::vector<MAPCH_ZLW_2021_mski *>> mskis(repeat);
    MAPCH_ZLW_2021_h h[repeat];
    MAPCH_ZLW_2021_r r[repeat], r_p[repeat];

    std::string m = "message";
    std::string m_p = "message_p";
    
    this->start("GlobalSetup");
    for (int i = 0; i < repeat; i++) ch.GlobalSetup(pkCH[i], skCH[i], gpkABE[i], pp[i], GetParam().lamuda);
    this->end("GlobalSetup");
    
    for (int i = 0; i < repeat; i++) {
        for(int j=0; j<GetParam().authNum; j++){
            MAPCH_ZLW_2021_mhk *mhk = new MAPCH_ZLW_2021_mhk();
            MAPCH_ZLW_2021_mtk *mtk = new MAPCH_ZLW_2021_mtk();
    
            if (j < SIZE_OF_ATTRIBUTES) {
                this->start("AuthSetUp", false);
                ch.AuthSetUp(*mhk, *mtk, As[j], pkCH[i], skCH[i], gpkABE[i], pp[i]);
                this->end("AuthSetUp", false);
            }else{
                this->start("AuthSetUp", false);
                ch.AuthSetUp(*mhk, *mtk, "ATTRIBUTE@AUTHORITY_" + std::to_string(j), pkCH[i], skCH[i], gpkABE[i], pp[i]);
                this->end("AuthSetUp", false);
            }
            
            mhks[i].push_back(mhk);
            mtks[i].push_back(mtk);
        }
    }
    average("AuthSetUp", GetParam().authNum);

    for (int i = 0; i < repeat; i++) {
        for(int j=0; j<GetParam().authNum; j++){
            MAPCH_ZLW_2021_mski *mski = new MAPCH_ZLW_2021_mski();
            if(j < SIZE_OF_ATTRIBUTES){
                this->start("KeyGen", false);
                ch.KeyGen(*mski, *mtks[i][j], *mhks[i][j], As[j], GID);
                this->end("KeyGen", false);
            }else{
                this->start("KeyGen", false);
                ch.KeyGen(*mski, *mtks[i][j], *mhks[i][j], "ATTRIBUTE@AUTHORITY_" + std::to_string(j), GID);
                this->end("KeyGen", false);
            }
            mskis[i].push_back(mski);
        }
    }
    average("KeyGen", GetParam().authNum);

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m, pp[i], mhks[i], MSP, POLICY);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++)  check_result[i] = ch.Check(h[i], r[i], m, mhks[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    // mskis.pop_back();
    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p, h[i], r[i], m, mhks[i], mskis[i], MSP);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(h[i], r_p[i], m_p, mhks[i]);
    this->end("Verify");
    
    average();

    delete binary_tree_expression;
    delete MSP;
    for(int i=0; i<mhks.size(); i++){
        for(int j=0; j<mhks[i].size(); j++){
            delete mhks[i][j];
        }
    }
    for(int i=0; i<mtks.size(); i++){
        for(int j=0; j<mtks[i].size(); j++){
            delete mtks[i][j];
        }
    }
    for(int i=0; i<mskis.size(); i++){
        for(int j=0; j<mskis[i].size(); j++){
            delete mskis[i][j];
        }
    }
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);
    
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}