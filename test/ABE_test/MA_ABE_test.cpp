#include "ChameLib.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    int authNum;
};

class MA_ABE_Test : public BaseTest<TestParams>{
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s\n", testName.c_str(), curveName.c_str());
            if(visiable)printf("%s %s\n", testName.c_str(), curveName.c_str());
        }
};

std::vector<TestParams> generateTestParams() {
    int curves[] = {
        Curve::A,
        Curve::A1,
        Curve::E,
    };
    int authNums[] = {16, 32, 64};

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (int authNum : authNums) {
            test_params.push_back({curve, authNum});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	ABE_Test,
    MA_ABE_Test,
	testing::ValuesIn(test_values)
);

TEST_P(MA_ABE_Test, Test){
    MA_ABE abe(GetParam().curve);

    const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
    // compute MSP
    Policy_resolution pr;
    Policy_generation pg;
    std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
    Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
    pg.generatePolicyInMatrixForm(binary_tree_expression);
    Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);

    // attribute
    const std::string A = "ONE";
    const std::string B = "TWO";
    const std::string C = "THREE";
    const std::string D = "FOUR";
    const std::string ATTRIBUTES[] = {A, B, C, D};
    const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

    const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

    std::vector<std::vector<MA_ABE_pkTheta *>> pkThetas(repeat);
    std::vector<std::vector<MA_ABE_skTheta *>> skThetas(repeat);
    std::vector<std::vector<MA_ABE_skgidA *>> skgidAs(repeat);

    MA_ABE_gpk gpk[repeat];

    MA_ABE_ciphertext c[repeat];

    element_s *msg[repeat], *res[repeat], *rt[repeat];
    for (int i = 0; i < repeat; i++){
        msg[i] = abe.GetGTElement();
        res[i] = abe.GetGTElement();
        rt[i] = abe.GetZrElement();
    }

    this->start("GlobalSetup");
    for (int i = 0; i < repeat; i++) abe.GlobalSetup(gpk[i]);
    this->end("GlobalSetup");

    for (int i = 0; i < repeat; i++){
        for(int j = 0; j < GetParam().authNum; j++) {
            MA_ABE_pkTheta *pkTheta = new MA_ABE_pkTheta();
            MA_ABE_skTheta *skTheta = new MA_ABE_skTheta();

            if(j < SIZE_OF_ATTRIBUTES){
                this->start("AuthSetup", false);
                abe.AuthSetup(*pkTheta, *skTheta, gpk[i], ATTRIBUTES[j]);
                this->end("AuthSetup", false);    
            }else{
                this->start("AuthSetup", false);
                abe.AuthSetup(*pkTheta, *skTheta, gpk[i], "ATTRIBUTE@AUTHORITY_" + std::to_string(j));
                this->end("AuthSetup", false);
            }

            pkThetas[i].push_back(pkTheta);
            skThetas[i].push_back(skTheta);
        }
    }
    average("AuthSetup", GetParam().authNum);
    
    for (int i = 0; i < repeat; i++){
        for(int j = 0; j < GetParam().authNum; j++) {
            MA_ABE_skgidA *skgidA = new MA_ABE_skgidA();

            if(j < SIZE_OF_ATTRIBUTES){
                this->start("KeyGen", false);
                abe.KeyGen(*skgidA, gpk[i], *skThetas[i][j], GID, ATTRIBUTES[j]);
                this->end("KeyGen", false);
            }else{
                this->start("KeyGen", false);
                abe.KeyGen(*skgidA, gpk[i], *skThetas[i][j], GID, "ATTRIBUTE@AUTHORITY_" + std::to_string(j));
                this->end("KeyGen", false);
            }

            skgidAs[i].push_back(skgidA);
        }
    }
    average("KeyGen", GetParam().authNum);

    this->start("Encrypt");
    for (int i = 0; i < repeat; i++) abe.Encrypt(c[i], msg[i], rt[i], gpk[i], pkThetas[i], MSP, POLICY);
    this->end("Encrypt");
    
    // choose partial secret keys
    std::vector<std::vector<MA_ABE_skgidA *>> _skgidAs(repeat); 
    for (int i = 0; i < repeat; i++){
        _skgidAs[i].push_back(skgidAs[i][0]);
        _skgidAs[i].push_back(skgidAs[i][1]);
        _skgidAs[i].push_back(skgidAs[i][2]);
    }
    
    this->start("Decrypt");
    for (int i = 0; i < repeat; i++) abe.Decrypt(res[i], _skgidAs[i], c[i], MSP);
    this->end("Decrypt");

    for (int i = 0; i < repeat; i++) ASSERT_TRUE(element_cmp(msg[i], res[i]) == 0);
    
    average();

    int op_cnt_GlobalSetUp[] = {    
        1, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_GlobalSetUp, "GlobalSetup"));

    int op_cnt_AuthSetUp[] = {    
        0, 0, 0, 2, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 1, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_AuthSetUp, "AuthSetup"));

    int op_cnt_KeyGen[] = {    
        0, 0, 0, 1, 
        2, 0, 0, 0, 
        2, 0, 0, 0, 
        4, 0, 0, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_KeyGen, "KeyGen"));

    int l = MSP->row();
    int n = MSP->col();
    int op_cnt_Encrypt[] = {
        0, 0, 0, 2 * n - 1 + l, 
        l, 0, 0, 0, 
        l, 0, 1 + l, l * (2 * n + 2), 
        4 * l, 0, 1 + 2 * l, 0, 
        0
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Encrypt, "Encrypt"));

    int op_cnt_Decrypt[] = {
        0, 0, 0, 0, 
        l, 0, 0, 0, 
        0, 0, 4 * l, 0, 
        0, 0, l, 0, 
        3 * l
    };
    EXPECT_TRUE(check_time(GetParam().curve, op_cnt_Decrypt, "Decrypt"));
}


int main(int argc, char **argv){
    ParseCommandLineArgs(argc, argv);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}