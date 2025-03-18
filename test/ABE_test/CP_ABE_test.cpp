#include "ABE/CP_ABE.h"
#include "CommonTest.h"

struct TestParams{
	int curve;
    bool swap;
};

class CP_ABE_Test : public BaseTest<TestParams>{    
    protected:
        void SetUp() override {
            BaseTest::SetUp();

            std::string testName = ::testing::UnitTest::GetInstance()->current_test_info()->name();
            std::string curveName = Curve::curve_names[GetParam().curve];
            fprintf(out, "%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
            if(visiable)printf("%s %s swap: %d\n", testName.c_str(), curveName.c_str(), GetParam().swap);
        }
};

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

    std::vector<TestParams> test_params;

    for (int curve : curves) {
        for (bool swap : swaps) {
            test_params.push_back({curve, swap});
        }
    }

    return test_params;
}

const std::vector<TestParams> test_values = generateTestParams();

INSTANTIATE_TEST_CASE_P(
	ABE_Test,
    CP_ABE_Test,
	testing::ValuesIn(test_values)
);

TEST_P(CP_ABE_Test, Test){
    for(int i = 0; UpdateProcBar(i, repeat), i < repeat; i++){

        CP_ABE abe(GetParam().curve, GetParam().swap);
        CP_ABE_mpk mpk;
        CP_ABE_msk msk;
        CP_ABE_sks sks;
        CP_ABE_ciphertext ciphertext;

        std::vector<std::string> attr_list = {"ONE","TWO","THREE"};
        const int SIZE_OF_ATTR = attr_list.size();
        const std::string POLICY = "(ONE&THREE)&(TWO|FOUR)";
        const int SIZE_OF_POLICY = 4;

        // compute MSP
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

        element_s* msg = abe.GetGTElement();
        element_s* res = abe.GetGTElement();
        
        this->start("Setup");
        abe.Setup(msk, mpk);
        this->end("Setup");

        this->start("KeyGen");
        abe.KeyGen(sks, msk, mpk, attr_list);
        this->end("KeyGen");

        if(visiable){
            Logger::PrintPbcWithSize("msg", msg);
        }
        this->start("Encrypt");
        abe.Encrypt(ciphertext, mpk, msg, MSP);
        this->end("Encrypt");

        this->start("Decrypt");
        abe.Decrypt(res, ciphertext, MSP, mpk, sks);
        this->end("Decrypt");
        if(visiable){
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