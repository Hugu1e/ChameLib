#include "ABE/MA_ABE.h"
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
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){
        MA_ABE abe(GetParam().curve);

        const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
        const int SIZE_OF_POLICY = 4;
        // compute policy matrix
        Policy_resolution pr;
        Policy_generation pg;
        std::vector<std::string>* postfix_expression = pr.infixToPostfix(POLICY);
        if(visiable){
            printf("postfix_expression of Policy: ");
            for(int i = 0;i < postfix_expression->size();i++){
                printf("%s ", postfix_expression->at(i).c_str());
            }
            printf("\n");
        }
        Binary_tree_policy* binary_tree_expression = pr.postfixToBinaryTree(postfix_expression, abe.GetZrElement());
        pg.generatePolicyInMatrixForm(binary_tree_expression);
        Element_t_matrix* MSP = pg.getPolicyInMatrixFormFromTree(binary_tree_expression);
        if(visiable){
            printf("Policy Matrix:\n");
            MSP->printMatrix();
        }

        // attribute
        const std::string A = "ONE";
        const std::string B = "TWO";
        const std::string C = "THREE";
        const std::string D = "FOUR";
        const std::string ATTRIBUTES[] = {A, B, C, D};
        const int SIZE_OF_ATTRIBUTES = sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]);

        const std::string GID = "GID of A B C D with attribute ONE TOW THREE FOUR";

        std::vector<MA_ABE_pkTheta *> pkThetas;
        std::vector<MA_ABE_skTheta *> skThetas;
        std::vector<MA_ABE_skgidA *> skgidAs;

        MA_ABE_gpk gpk;

        MA_ABE_ciphertext c;


        element_s *msg = abe.GetGTElement();
        element_s *res = abe.GetGTElement();
        element_s *rt = abe.GetZrElement();

        this->start("GlobalSetup");
        abe.GlobalSetup(gpk);
        this->end("GlobalSetup");

        for(int i = 0; i < GetParam().authNum; i++) {
            MA_ABE_pkTheta *pkTheta = new MA_ABE_pkTheta();
            MA_ABE_skTheta *skTheta = new MA_ABE_skTheta();

            if(i < SIZE_OF_ATTRIBUTES){
                this->start("AuthSetup");
                abe.AuthSetup(*pkTheta, *skTheta, gpk, ATTRIBUTES[i]);
                this->end("AuthSetup");    
            }else{
                this->start("AuthSetup");
                abe.AuthSetup(*pkTheta, *skTheta, gpk, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
                this->end("AuthSetup");
            }

            pkThetas.push_back(pkTheta);
            skThetas.push_back(skTheta);
        }

        for(int i = 0; i < GetParam().authNum; i++) {
            MA_ABE_skgidA *skgidA = new MA_ABE_skgidA();

            if(i < SIZE_OF_ATTRIBUTES){
                this->start("KeyGen");
                abe.KeyGen(*skgidA, gpk, *skThetas[i], GID, ATTRIBUTES[i]);
                this->end("KeyGen");
            }else{
                this->start("KeyGen");
                abe.KeyGen(*skgidA, gpk, *skThetas[i], GID, "ATTRIBUTE@AUTHORITY_" + std::to_string(i));
                this->end("KeyGen");
            }

            skgidAs.push_back(skgidA);
        }

        if(visiable){
            Logger::PrintPbc("msg", msg);
            Logger::PrintPbc("rt", rt);        
        }

        this->start("Encrypt");
        abe.Encrypt(c, msg, rt, gpk, pkThetas, MSP, POLICY);
        this->end("Encrypt");
        
        // choose partial secret keys
        std::vector<MA_ABE_skgidA *> _skgidAs;  
        _skgidAs.push_back(skgidAs[0]);
        _skgidAs.push_back(skgidAs[1]);
        _skgidAs.push_back(skgidAs[2]);
        
        this->start("Decrypt");
        abe.Decrypt(res, _skgidAs, c, MSP);
        this->end("Decrypt");

        if(visiable){
            Logger::PrintPbc("msg", msg);
            Logger::PrintPbc("res", res);
        }
        ASSERT_TRUE(element_cmp(msg, res) == 0);
    }
    average();
}


int main(int argc, char **argv) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}