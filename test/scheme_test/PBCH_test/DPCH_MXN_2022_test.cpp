#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
    int lamuda;
};

class DPCH_MXN_2022_Test : public BaseTest<TestParams>{
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
    Policy_resolution pr;
    Policy_generation pg;
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, ch.GetZrElement());
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    // attributes
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<std::vector<DPCH_MXN_2022_pkTheta *>> pkThetas(repeat);
    std::vector<std::vector<DPCH_MXN_2022_skTheta *>> skThetas(repeat);
    std::vector<std::vector<DPCH_MXN_2022_skGidA *>> skGidAs(repeat);

    DPCH_MXN_2022_pp ppDPCH[repeat];
    DPCH_MXN_2022_pk pkDPCH[repeat];
    DPCH_MXN_2022_sk skDPCH[repeat];
    DPCH_MXN_2022_skGid skGid[repeat];
    DPCH_MXN_2022_sigmaGid sigmaGid[repeat];
    DPCH_MXN_2022_h h[repeat];
    DPCH_MXN_2022_r r[repeat], r_p[repeat];
    std::string m = "message";
    std::string m_p = "message_p";

    this->start("SetUp");
    for (int i = 0; i < repeat; i++) ch.SetUp(ppDPCH[i], pkDPCH[i], skDPCH[i], sigmaGid[i], GetParam().lamuda);
    this->end("SetUp");

    this->start("ModSetUp");
    for (int i = 0; i < repeat; i++) ch.ModSetUp(skGid[i], sigmaGid[i], skDPCH[i], GID);
    this->end("ModSetUp");
    
    for (int i = 0; i < repeat; i++){
        for(int j = 0; j < GetParam().authNum; j++) {
            DPCH_MXN_2022_pkTheta *pkTheta = new DPCH_MXN_2022_pkTheta; 
            DPCH_MXN_2022_skTheta *skTheta = new DPCH_MXN_2022_skTheta;
    
            if(j < SIZE_OF_ATTRIBUTES){
                this->start("AuthSetUp", false);
                ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH[i], ATTRIBUTES[j]);
                this->end("AuthSetUp", false);
            }else{
                this->start("AuthSetUp", false);
                ch.AuthSetUp(*pkTheta, *skTheta, ppDPCH[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(j));
                this->end("AuthSetUp", false);
            }
    
            pkThetas[i].push_back(pkTheta);
            skThetas[i].push_back(skTheta);
        }
    }
    average("AuthSetUp", GetParam().authNum);
    
    for (int i = 0; i < repeat; i++){
        for(int j = 0; j < GetParam().authNum; j++) {
            DPCH_MXN_2022_skGidA *skGidA = new DPCH_MXN_2022_skGidA;
    
            if (j < SIZE_OF_ATTRIBUTES) {
                this->start("ModKeyGen", false);
                ch.ModKeyGen(*skGidA, ppDPCH[i], pkDPCH[i], GID, sigmaGid[i], *skThetas[i][j], ATTRIBUTES[j]);
                this->end("ModKeyGen", false);
            } else {
                this->start("ModKeyGen", false);
                ch.ModKeyGen(*skGidA, ppDPCH[i], pkDPCH[i], GID, sigmaGid[i], *skThetas[i][j], "ATTRIBUTE@AUTHORITY_" + std::to_string(j));
                this->end("ModKeyGen", false);
            }
            
            skGidAs[i].push_back(skGidA);
        }
    }
    average("ModKeyGen", GetParam().authNum);

    this->start("Hash");
    for (int i = 0; i < repeat; i++) ch.Hash(h[i], r[i], m, ppDPCH[i], pkDPCH[i], pkThetas[i], MSP, POLICY);
    this->end("Hash");

    bool check_result[repeat];
    this->start("Check");
    for (int i = 0; i < repeat; i++) check_result[i] = ch.Check(pkDPCH[i], m, h[i], r[i]);
    this->end("Check");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(check_result[i]);

    // choose partial secret keys
    std::vector<std::vector<DPCH_MXN_2022_skGidA *>> _skGidAs(repeat);
    for (int i = 0; i < repeat; i++){
        _skGidAs[i].push_back(skGidAs[i][0]);
        _skGidAs[i].push_back(skGidAs[i][1]);
        _skGidAs[i].push_back(skGidAs[i][2]);
    }

    this->start("Adapt");
    for (int i = 0; i < repeat; i++) ch.Adapt(r_p[i], m_p, h[i], r[i], m, pkDPCH[i], skGid[i], _skGidAs[i], ppDPCH[i], pkThetas[i], MSP, POLICY);
    this->end("Adapt");

    bool verify_result[repeat];
    this->start("Verify");
    for (int i = 0; i < repeat; i++) verify_result[i] = ch.Verify(pkDPCH[i], m_p, h[i], r_p[i]);
    this->end("Verify");
    for (int i = 0; i < repeat; i++) ASSERT_TRUE(verify_result[i]);
    
    average();
}

int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}